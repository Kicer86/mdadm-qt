/*
 * Model representing list of raid devices
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


#include "raids_model.hpp"

#include <cassert>


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

    ///////

    bool raidNameLess(const RaidInfo& lhs, const RaidInfo& rhs)
    {
        return lhs.raid_device < rhs.raid_device;
    }

}


RaidsModel::RaidsModel(IRaidInfoProvider* mdadmCtrl):
    m_model(),
    m_infos(),
    m_componentInfos(),
    m_diskType({{ RaidComponentInfo::Type::Normal, "active" },
                { RaidComponentInfo::Type::Faulty, "faulty" },
                { RaidComponentInfo::Type::Journal, "journal" },
                { RaidComponentInfo::Type::Replacement, "replacement" },
                { RaidComponentInfo::Type::Spare, "spare" },
                { RaidComponentInfo::Type::WriteMostly, "write mostly" },
               }),
    m_mdadmCtrl(mdadmCtrl)
{
    m_model.setHorizontalHeaderLabels( { tr("device"), tr("type"), tr("status") } );
}


RaidsModel::~RaidsModel()
{

}


void RaidsModel::load()
{
    std::vector<RaidInfo> raids = m_mdadmCtrl->listRaids();

    const std::set<RaidInfo> newRaids(raids.cbegin(), raids.cend());
    const std::set<RaidInfo> oldRaids(value_map_iterator<RaidsMap>(m_infos.cbegin()),
                                      value_map_iterator<RaidsMap>(m_infos.cend()));

    eraseRemoved(oldRaids, newRaids);
    appendAdded(oldRaids, newRaids);
    refreshChanged(oldRaids, newRaids);
}


RaidsModel::ItemType RaidsModel::getTypeFor(const QModelIndex& index) const
{
    const ItemType result = index.parent().isValid()?
        ItemType::Component:
        Raid;

    return result;
}


const RaidInfo& RaidsModel::infoForRaid(const QModelIndex& index) const
{
    assert(index.isValid());
    assert(getTypeFor(index) == Raid);

    const QModelIndex first_in_row = index.sibling(index.row(), 0);

    QStandardItem* item = m_model.itemFromIndex(first_in_row);
    assert(item != nullptr);

    const auto it = m_infos.find(item);
    assert(it != m_infos.end());

    const RaidInfo& info = it->second;

    return info;
}


const RaidComponentInfo& RaidsModel::infoForComponent(const QModelIndex& index) const
{
    assert(index.isValid());
    assert(getTypeFor(index) == Component);

    const QModelIndex first_in_row = index.sibling(index.row(), 0);

    QStandardItem* item = m_model.itemFromIndex(first_in_row);
    assert(item != nullptr);

    const auto it = m_componentInfos.find(item);
    assert(it != m_componentInfos.end());

    const RaidComponentInfo& info = it->second;

    return info;
}


QAbstractItemModel* RaidsModel::model()
{
    return &m_model;
}


RaidsModel::RaidsMap::iterator RaidsModel::itFor(const QString& name)
{
     RaidsMap::iterator it = std::find_if(m_infos.begin(), m_infos.end(),
        [&name](const auto& item) { return item.second.raid_device == name; }
    );

    assert(it != m_infos.end());

    return it;
}


RaidsModel::RaidsMap::const_iterator RaidsModel::itFor(const QString& name) const
{
     RaidsMap::const_iterator it = std::find_if(m_infos.begin(), m_infos.end(),
        [&name](const auto& item) { return item.second.raid_device == name; }
    );

    assert(it != m_infos.end());

    return it;
}


const RaidInfo& RaidsModel::infoFor(const QString& name) const
{
    RaidsMap::const_iterator it = itFor(name);

    assert(it != m_infos.end());

    return it->second;
}


QStandardItem* RaidsModel::itemFor(const QString& name) const
{
    RaidsMap::const_iterator it = itFor(name);

    assert(it != m_infos.end());

    return it->first;
}


void RaidsModel::appendRaid(const RaidInfo& raidInfo)
{
    QStandardItem* raid_device_item = new QStandardItem(raidInfo.raid_device);
    QStandardItem* raid_type_item = new QStandardItem(raidInfo.raid_type);
    QStandardItem* raid_status = new QStandardItem(tr("TO DO"));

    const QList<QStandardItem *> row =
    {
        raid_device_item,
        raid_type_item,
        raid_status,
    };

    for (const auto& blkdev : raidInfo.block_devices)
        appendComponent(raid_device_item, blkdev);

    m_infos.emplace(raid_device_item, raidInfo);
    m_model.appendRow(row);
}


void RaidsModel::removeRaid(const QString& raid_name)
{
    // remove related components
    removeComponentsOf(raid_name);

    QStandardItem* raidItem = itemFor(raid_name);

    const QModelIndex raidIndex = m_model.indexFromItem(raidItem);
    m_model.removeRow(raidIndex.row(), raidIndex.parent());
    m_infos.erase(raidItem);
}


void RaidsModel::updateRaid(const RaidInfo& raid)
{
    // Re-add all components.
    // This is not the nicest solution, but it is easy, and is good enough.

    removeComponentsOf(raid.raid_device);

    auto raidIt = itFor(raid.raid_device);

    QStandardItem* raidItem = raidIt->first;
    for (const auto& blkdev : raid.block_devices)
        appendComponent(raidItem, blkdev);

    // update raid information
    const QModelIndex idxForRaid = m_model.indexFromItem(raidItem);
    const QModelIndex idxForType = idxForRaid.sibling(idxForRaid.row(), 1);   // TODO: use constant for column type
    QStandardItem* typeItem = m_model.itemFromIndex(idxForType);

    // update stored RaidInfo
    raidIt->second = raid;

    // TODO: prepare common code for update and appendRaid()
    typeItem->setText(raid.raid_type);
}


void RaidsModel::removeComponentsOf(const QString& raid_name)
{
    const RaidInfo& raidInfo = infoFor(raid_name);

    for(const RaidComponentInfo& component: raidInfo.block_devices)
        removeComponent(component);
}


void RaidsModel::appendComponent(QStandardItem* raidItem, const RaidComponentInfo& blkdev)
{
    QStandardItem* component_item = new QStandardItem(blkdev.name);
    QStandardItem* component_status =
            new QStandardItem(m_diskType[blkdev.type]);

    const QList<QStandardItem *> leaf =
    {
        component_item,
        new QStandardItem(),
        component_status
    };

    m_componentInfos.emplace(component_item, blkdev);
    raidItem->appendRow(leaf);
}


void RaidsModel::removeComponent(const RaidComponentInfo& component)
{
    ComponentsMap::iterator it = std::find_if(m_componentInfos.begin(), m_componentInfos.end(),
        [&component](const auto& item) { return item.second.name == component.name; }
    );

    assert(it != m_componentInfos.end());

    const QModelIndex componentIndex = m_model.indexFromItem(it->first);
    m_model.removeRow(componentIndex.row(), componentIndex.parent());
    m_componentInfos.erase(it);
}


void RaidsModel::eraseRemoved(const std::set<RaidInfo>& oldRaids, const std::set<RaidInfo>& newRaids)
{
    std::vector<RaidInfo> removed;
    std::set_difference(oldRaids.cbegin(), oldRaids.cend(),
                        newRaids.cbegin(), newRaids.cend(),
                        std::back_inserter(removed),
                        raidNameLess);

    for (const RaidInfo& raid: removed)
        removeRaid(raid.raid_device);
}


void RaidsModel::appendAdded(const std::set<RaidInfo>& oldRaids, const std::set<RaidInfo>& newRaids)
{
    std::vector<RaidInfo> added;
    std::set_difference(newRaids.cbegin(), newRaids.cend(),
                        oldRaids.cbegin(), oldRaids.cend(),
                        std::back_inserter(added),
                        raidNameLess);

    for (const RaidInfo& raid: added)
        appendRaid(raid);
}


void RaidsModel::refreshChanged(const std::set<RaidInfo>& oldRaids, const std::set<RaidInfo>& newRaids)
{
    std::vector<RaidInfo> modified;

    // find all raids which changed (same name different content)
    for (const RaidInfo& oldRaid: oldRaids)
    {
        const auto it =
            std::find_if(newRaids.cbegin(), newRaids.cend(),
                         [&oldRaid](const RaidInfo& newRaid)
                         {
                             // Find raids which differ in anything but name
                             return newRaid.raid_device == oldRaid.raid_device &&
                                    newRaid != oldRaid;
                         });

        if (it != newRaids.end())
            modified.push_back(*it);
    }

    for (const RaidInfo& raid: modified)
        updateRaid(raid);
}
