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
#include <set>

#include <QTextStream>

#include "ifilesystem.hpp"


namespace
{
    // NOTE: taken from:
    // https://github.com/Kicer86/photobroom/blob/master/src/core/iterator_wrapper.hpp
    // https://github.com/Kicer86/photobroom/blob/master/src/core/map_iterator.hpp
    // TODO: share it in a common library

    template<typename R, typename B, typename T>
    struct iterator_wrapper: B
    {
        iterator_wrapper(): B(), m_operation()
        {

        }

        iterator_wrapper(const B& base): B(base), m_operation()
        {

        }

        iterator_wrapper(const iterator_wrapper &) = default;

        ~iterator_wrapper()
        {
        }

        const R& operator*() const
        {
            return m_operation(*this);
        }

        private:
            T m_operation;
    };

    template<typename T>
    struct MapKeyAccessor
    {
        const typename T::value_type::first_type& operator()(const typename T::const_iterator& v) const
        {
            return v->first;
        }
    };

    template<typename T>
    struct MapValueAccessor
    {
        const typename T::value_type::second_type& operator()(const typename T::const_iterator& v) const
        {
            return v->second;
        }
    };


    template<typename T>
    using key_map_iterator = iterator_wrapper<typename T::value_type::first_type, typename T::const_iterator, MapKeyAccessor<T>>;

    template<typename T>
    using value_map_iterator = iterator_wrapper<typename T::value_type::second_type, typename T::const_iterator, MapValueAccessor<T>>;
}



RaidInfoProvider::RaidInfoProvider(IFileSystem* fileSystem):
    m_raids(),
    m_fileSystem(fileSystem)
{
}


RaidInfoProvider::~RaidInfoProvider()
{
}


std::vector<RaidInfo> RaidInfoProvider::listRaids() const
{
    reCache();

    std::vector<RaidInfo> raid_infos;

    for(const auto& raid: m_raids)
        raid_infos.emplace_back(this, raid.first);

    return raid_infos;
}


RaidInfo RaidInfoProvider::getInfoFor(const RaidId& id) const
{
    return RaidInfo(this, id);
}


const QString& RaidInfoProvider::raidDevice(const RaidId& id) const
{
    return m_raids.at(id).device;
}


const QList<RaidComponentInfo>& RaidInfoProvider::blockDevices(const RaidId& id) const
{
    return m_raids.at(id).components;
}


const QString& RaidInfoProvider::raidType(const RaidId& id) const
{
    return m_raids.at(id).type;
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


void RaidInfoProvider::reCache() const
{
    const std::map<RaidId, RaidData> oldRaidSet = m_raids;
    m_raids = readRaids();
    const std::map<RaidId, RaidData>& newRaidSet = m_raids;

    // generate diff
    const std::vector<RaidId> raidsAdded = findAdded(oldRaidSet, newRaidSet);
    const std::vector<RaidId> raidsRemoved = findRemoved(oldRaidSet, newRaidSet);
    const std::vector<RaidId> raidsChanged = findChanged(oldRaidSet, newRaidSet);

    for (const RaidId& id: raidsAdded)
        emit raidAdded(id);

    for (const RaidId& id: raidsRemoved)
        emit raidRemoved(id);

    for (const RaidId& id: raidsChanged)
        emit raidChanged(id);
}

RaidInfoProvider::RaidsMap RaidInfoProvider::readRaids() const
{
    RaidsMap result;

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
                        const RaidComponentInfo::Type comp_type =
                                (tmp.isEmpty() ?
                                     RaidComponentInfo::Type::Normal :
                                     static_cast<RaidComponentInfo::Type>(
                                         tmp.toStdString().c_str()[0]));
                        devices_list.append(
                                    RaidComponentInfo(device, comp_type, descr_nr)
                                    );
                    }
                }

                const RaidId id(dev);

                RaidData data;
                data.device = dev;
                data.components = devices_list;
                data.type = type;

                result[id] = data;
            }
        }
    }

    return result;
}



std::vector<RaidId> RaidInfoProvider::findRemoved(const RaidsMap& oldRaids,
                                                  const RaidsMap& newRaids) const
{
    const std::set<RaidId> oldRaidsSet(key_map_iterator<RaidsMap>(oldRaids.cbegin()),
                                       key_map_iterator<RaidsMap>(oldRaids.cend()));

    const std::set<RaidId> newRaidsSet(key_map_iterator<RaidsMap>(newRaids.cbegin()),
                                       key_map_iterator<RaidsMap>(newRaids.cend()));

    std::vector<RaidId> removed;
    std::set_difference(oldRaidsSet.cbegin(), oldRaidsSet.cend(),
                        newRaidsSet.cbegin(), newRaidsSet.cend(),
                        std::back_inserter(removed));

    return removed;
}


std::vector<RaidId> RaidInfoProvider::findAdded(const RaidsMap& oldRaids,
                                                const RaidsMap& newRaids) const
{
    const std::set<RaidId> oldRaidsSet(key_map_iterator<RaidsMap>(oldRaids.cbegin()),
                                       key_map_iterator<RaidsMap>(oldRaids.cend()));

    const std::set<RaidId> newRaidsSet(key_map_iterator<RaidsMap>(newRaids.cbegin()),
                                       key_map_iterator<RaidsMap>(newRaids.cend()));

    std::vector<RaidId> added;
    std::set_difference(newRaidsSet.cbegin(), newRaidsSet.cend(),
                        oldRaidsSet.cbegin(), oldRaidsSet.cend(),
                        std::back_inserter(added));

    return added;
}


std::vector<RaidId> RaidInfoProvider::findChanged(const RaidsMap& oldRaids,
                                                  const RaidsMap& newRaids) const
{
    const std::set<RaidId> oldRaidsSet(key_map_iterator<RaidsMap>(oldRaids.cbegin()),
                                       key_map_iterator<RaidsMap>(oldRaids.cend()));

    const std::set<RaidId> newRaidsSet(key_map_iterator<RaidsMap>(newRaids.cbegin()),
                                       key_map_iterator<RaidsMap>(newRaids.cend()));

    std::vector<RaidId> common;
    std::set_union(newRaidsSet.cbegin(), newRaidsSet.cend(),
                   oldRaidsSet.cbegin(), oldRaidsSet.cend(),
                   std::back_inserter(common));

    std::vector<RaidId> changed;
    for (const RaidId& id: common)
    {
        const RaidsMap::const_iterator oldIt = oldRaids.find(id);
        const RaidsMap::const_iterator newIt = newRaids.find(id);

        if (oldIt->second != newIt->second)
            changed.push_back(id);
    }

    return changed;
}
