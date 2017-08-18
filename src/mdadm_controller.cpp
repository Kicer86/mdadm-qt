/*
 * Class responsible for communication with mdadm process
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mdadm_controller.hpp"

#include <QDebug>
#include <QRegExp>
#include <QFileInfo>
#include <QTextStream>

#include "ifilesystem.hpp"
#include "imdadm_process.hpp"

namespace
{
    QString levelName(MDAdmController::Type type)
    {
        QString result;

        switch(type)
        {
            case MDAdmController::Type::Raid0:
                result = "stripe";
                break;

            case MDAdmController::Type::Raid1:
                result = "mirror";
                break;

            case MDAdmController::Type::Raid4:
                result = "4";
                break;

            case MDAdmController::Type::Raid5:
                result = "5";
                break;

            case MDAdmController::Type::Raid6:
                result = "6";
                break;
        }

        return result;
    }
}

bool RaidInfo::operator==(const RaidInfo &other) const
{
    return this->block_devices == other.block_devices &&
            this->raid_device == other.raid_device &&
            this->raid_type == other.raid_type;
}

MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess, IFileSystem* fileSystem):
    m_mdadmProcess(mdadmProcess),
    m_fileSystem(fileSystem)
{

}


MDAdmController::~MDAdmController()
{

}


bool MDAdmController::listRaids(const ListResult& result)
{
    auto file = m_fileSystem->openFile("/proc/mdstat", QIODevice::ReadOnly |
                                                       QIODevice::Text);

    QTextStream* mdstat_stream = file->getStream();

    const bool open_status = mdstat_stream != nullptr;

    if (open_status)
    {
        //                        raid device  status  read-only        type             devices
        const QRegExp mdadm_info("^(md[^ ]+) : ([^ ]+) (?:\\([^ ]+\\) )?(?:(raid[^ ]+) )?(.*)");
        std::vector<RaidInfo> results;

        // simple loop with atEnd() won't work
        // see: http://doc.qt.io/qt-5/qiodevice.html#atEnd
        // and  http://doc.qt.io/qt-5/qfile.html#details
        // for details

        for(QString outputLine = mdstat_stream->readLine();
            outputLine.isNull() == false;
            outputLine = mdstat_stream->readLine())
        {
            if (mdadm_info.exactMatch(outputLine))
            {
                const QString dev = mdadm_info.cap(1);
                const QString status = mdadm_info.cap(2);
                const QString type = mdadm_info.cap(3);
                const QString devices = mdadm_info.cap(4);

                const QStringList devices_list_raw = devices.split(" ");

                //drop role numbers (convert sda1[0] to sda1)
                QStringList devices_list;
                for(const QString& device_with_role: devices_list_raw)
                {
                    const int l = device_with_role.lastIndexOf('[');
                    const QString device = device_with_role.left(l);

                    devices_list.append(device);
                }

                results.emplace_back(dev, devices_list, type);
            }
        }

        result(results);
    }

    return open_status;
}

bool MDAdmController::listComponents(const QString& raid_device,
                                     QStringList& block_devices) {
    QString slaves_path = "/sys/block/" + raid_device + "/slaves";

    const std::deque<QString> files = m_fileSystem->listDir(slaves_path, "*");

    for (const QString& file: files)
        block_devices << ("/dev/" + file);

    return !block_devices.empty();
}


bool MDAdmController::createRaid(const QString& raid_device,
                                 MDAdmController::Type type,
                                 const QStringList& block_devices,
                                 const OutputParser& callback)
{
    QStringList mdadm_args;

    mdadm_args << "--create" << "--verbose" << raid_device;
    mdadm_args << "--level" << levelName(type);
    mdadm_args << QString("--raid-devices=%1").arg(block_devices.size()) << block_devices;

    qDebug() << "executing mdadm with args: " << mdadm_args;

    m_mdadmProcess->execute(mdadm_args, [this](const QByteArray& output,
                                               bool success,
                                               int exitCode)
    {
        if (success)
        {
            qDebug() << "mdadm exited normally with code: " << exitCode << " and output:";
            qDebug() << output;

            emit raidCreated();
        }
        else
            qDebug() << "mdadm crashed";
    },
    [callback](const QByteArray& output)->QString {
        if (callback != nullptr)
            return callback(QString(output));
        return "";
    });

    return true;
}

bool MDAdmController::removeRaid(const QString& raid_device)
{
    QStringList mdadm_args;
    QStringList components;

    mdadm_args << "--stop" << "--verbose" << raid_device;

    if (listComponents(QFileInfo(raid_device).baseName(), components))
    {
        mdadm_args << "--zero-superblock" << components;
    }

    m_mdadmProcess->execute(mdadm_args, [](const QByteArray& output,
                                           bool success, int exitCode)
    {
        if (success)
        {
            qDebug() << "mdadm exited normally with code: "
                     << exitCode << " and output:";
            qDebug() << output;
        }
        else
            qDebug() << "mdadm crashed";
    });

    return true;
}

bool MDAdmController::stopRaid(const QString& raid_device)
{
    QStringList mdadm_args;

    mdadm_args << "--stop" << "--verbose" << raid_device;

    m_mdadmProcess->execute(mdadm_args, [](const QByteArray& output,
                                           bool success, int exitCode)
    {
        if (success)
        {
            qDebug() << "mdadm exited normally with code: "
                     << exitCode << " and output:";
            qDebug() << output;
        }
        else
            qDebug() << "mdadm crashed";
    });

    return true;
}

bool MDAdmController::zeroSuperblock(const QStringList& raid_components)
{
    QStringList mdadm_args;

    if (raid_components.isEmpty())
        return false;

    mdadm_args << "--zero-superblock" << raid_components;

    m_mdadmProcess->execute(mdadm_args, [](const QByteArray& output,
                                           bool success, int exitCode)
    {
        if (success)
        {
            qDebug() << "mdadm exited normally with code: "
                     << exitCode << " and output:";
            qDebug() << output;
        }
        else
            qDebug() << "mdadm crashed";
    });

    return true;
}
