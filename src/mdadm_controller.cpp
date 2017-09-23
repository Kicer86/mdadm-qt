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

#include <QRegExp>
#include <QFileInfo>
#include <QTextStream>

#include "ifilesystem.hpp"
#include "imdadm_process.hpp"
#include "utils.hpp"

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
    
    void nullResultCallback(const QByteArray &, bool,int) {  }    
}

bool RaidComponentInfo::operator==(const RaidComponentInfo& other) const
{
    return this->name == other.name &&
           this->type == other.type &&
           this->descriptor_index == other.descriptor_index;
}

bool RaidInfo::operator==(const RaidInfo &other) const
{
    return this->block_devices == other.block_devices &&
           this->raid_device == other.raid_device &&
           this->raid_type == other.raid_type;
}

MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess,
                                 IFileSystem* fileSystem):
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

        const QRegExp device_info("([a-zA-Z0-9]+)\\[([0-9]+)\\](?:\\(([A-Z])\\))?");

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
                QList<RaidComponentInfo> devices_list;
                for(const QString& device_with_role: devices_list_raw)
                {
                    if (device_info.exactMatch(device_with_role)) {
                        QString device = device_info.cap(1);
                        int descr_nr = device_info.cap(2).toInt();
                        QString tmp = device_info.cap(3);
                        RaidComponentInfo::Type type =
                                (tmp.isEmpty() ?
                                     RaidComponentInfo::Type::Normal :
                                     static_cast<RaidComponentInfo::Type>(
                                         tmp.toStdString().c_str()[0]));
                        devices_list.append(
                                    RaidComponentInfo(device, type, descr_nr)
                                    );
                    }
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

    const std::deque<QString> files =
            m_fileSystem->listDir(slaves_path, "*",
                                  QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& file: files)
        block_devices << ("/dev/" + file);

    return !block_devices.empty();
}


bool MDAdmController::createRaid(const QString& raid_device,
                                 MDAdmController::Type type,
                                 const QStringList& block_devices,
                                 const QStringList& spare_devices,
                                 const OutputParser& callback)
{
    QStringList mdadm_args;

    mdadm_args << "--create" << "--verbose" << raid_device;
    mdadm_args << "--level" << levelName(type);
    mdadm_args << QString("--raid-devices=%1")
                  .arg(block_devices.size())
               << block_devices;
    if (!spare_devices.empty())
    {
        mdadm_args << QString("--spare-devices=%1")
                      .arg(spare_devices.size())
                   << spare_devices;
    }

    m_mdadmProcess->execute(mdadm_args,
                            [this](const QByteArray& output,
                                               bool success,
                                               int exitCode)
    {
        if (success)
            emit raidCreated();
    },
                            [callback](const QByteArray& output)->QString
    {
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

    m_mdadmProcess->execute(mdadm_args, [this](const QByteArray& output,
                                               bool success,
                                               int exitCode)
    {
        if (success)
            emit raidRemoved();
    });
    
    return true;
}

bool MDAdmController::stopRaid(const QString& raid_device)
{
    QStringList mdadm_args;

    mdadm_args << "--stop" << "--verbose" << raid_device;

    m_mdadmProcess->execute(mdadm_args, nullResultCallback);

    return true;
}

bool MDAdmController::zeroSuperblock(const QStringList& raid_components)
{
    QStringList mdadm_args;

    if (raid_components.isEmpty())
        return false;

    mdadm_args << "--zero-superblock" << raid_components;

    m_mdadmProcess->execute(mdadm_args, nullResultCallback);

    return true;
}

bool MDAdmController::markAsFaulty(const QString& raid_device,
                                   const QString& component)
{
    QStringList mdadm_args;

    mdadm_args << raid_device << "--fail" << component;

    m_mdadmProcess->execute(mdadm_args, nullResultCallback);

    return true;
}

bool MDAdmController::reAdd(const QString& raid_device,
                            const QString& component)
{
    QStringList mdadm_args;

    mdadm_args << raid_device << "--re-add" << component;

    m_mdadmProcess->execute(mdadm_args, nullResultCallback);

    return true;
}

/*
 * Scan functions
 */

QString MDAdmController::scanTypeToString(const ScanType type) const
{
    QMap<ScanType, QString> scanTypes =
    {
        { ScanType::Idle, "idle" },
        { ScanType::Check, "check" },
        { ScanType::Recover, "recover" },
        { ScanType::Repair, "repair" },
        { ScanType::Resync, "resync" }
    };

    return scanTypes.value(type);
}

MDAdmController::ScanType
MDAdmController::scanStringToType(const QString& type) const
{
    QMap<QString, ScanType> scanTypes =
    {
        { "idle", ScanType::Idle },
        { "check", ScanType::Check },
        { "recover", ScanType::Recover },
        { "repair", ScanType::Repair },
        { "resync", ScanType::Resync }
    };

    return scanTypes.value(type, ScanType::Idle);
}

bool MDAdmController::runScan(const QString& raid_device,
                              const ScanType scan_type)
{
    const QString scan_action_path =
            "/sys/block/" + raid_device + "/md/sync_action";

    return utils::writeValueToFile(m_fileSystem, scan_action_path,
                                   scanTypeToString(scan_type));
}

MDAdmController::ScanType
MDAdmController::getScanType(const QString& raid_device)
{
    const QString scan_action_path =
            "/sys/block/" + raid_device + "/md/sync_action";

    return scanStringToType(
            utils::readValueFromFile<QString>(m_fileSystem, scan_action_path));

}
