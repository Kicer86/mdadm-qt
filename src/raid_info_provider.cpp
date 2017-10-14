/*
 * Raid information hub
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
 */

#include "raid_info_provider.hpp"

#include <cassert>

#include <QTextStream>

#include "ifilesystem.hpp"


RaidInfoProvider::RaidInfoProvider(IFileSystem* fileSystem):
    m_fileSystem(fileSystem),
    m_raidType(),
    m_raidDevice(),
    m_raidComponents()
{
}


RaidInfoProvider::~RaidInfoProvider()
{
}


std::vector<RaidInfo> RaidInfoProvider::listRaids() const
{
    // TODO: we are filling cache here.
    //       Whole idea of caching and cache flush needs to be
    //       prepared properly.
    m_raidDevice.clear();
    m_raidType.clear();
    m_raidComponents.clear();
    std::vector<RaidInfo> raid_infos;

    listRaids([&raid_infos](const std::vector<RaidInfo>& infos)
    {
        raid_infos = infos;
    });

    return raid_infos;
}


RaidInfo RaidInfoProvider::getInfoFor(const RaidId& id) const
{
    return RaidInfo(this, id);
}


const QString& RaidInfoProvider::raidDevice(const RaidId& id) const
{
    return m_raidDevice.at(id);
}


const QList<RaidComponentInfo>& RaidInfoProvider::blockDevices(const RaidId& id) const
{
    return m_raidComponents.at(id);
}


const QString& RaidInfoProvider::raidType(const RaidId& id) const
{
    return m_raidType.at(id);
}


bool RaidInfoProvider::listComponents(const QString& raid_device,
                                     QStringList& block_devices) const
{
    QString slaves_path = "/sys/block/" + raid_device + "/slaves";

    const std::deque<QString> files =
            m_fileSystem->listDir(slaves_path, "*",
                                  QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& file: files)
        block_devices << ("/dev/" + file);

    return !block_devices.empty();
}


bool RaidInfoProvider::listRaids(const ListResult& result) const
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

                const RaidId id(dev);

                m_raidType[id] = type;
                m_raidComponents[id] = devices_list;
                m_raidDevice[id] = dev;

                results.push_back(getInfoFor(id));
            }
        }

        result(results);
    }

    return open_status;
}
