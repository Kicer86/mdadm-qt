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


RaidsModel::RaidsModel(IRaidInfoProvider* raidInfoProvider):
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
    m_raidInfoProvider(raidInfoProvider)
{
    m_model.setHorizontalHeaderLabels( { tr("device"), tr("type"), tr("status") } );

    connect(raidInfoProvider, &IRaidInfoProvider::raidAdded,
            this, &RaidsModel::appendRaid);

    connect(raidInfoProvider, &IRaidInfoProvider::raidRemoved,
            this, &RaidsModel::removeRaid);

    connect(raidInfoProvider, &IRaidInfoProvider::raidChanged,
            this, &RaidsModel::updateRaid);

    const auto raids = m_raidInfoProvider->listRaids();
    for(const RaidInfo& raid: raids)
        append(raid);
}


RaidsModel::~RaidsModel()
{

}


RaidsModel::ItemType RaidsModel::getTypeFor(const QModelIndex& index) const
{
    const ItemType result = index.parent().isValid()?
        ItemType::Component:
        Raid;

    return result;
}


RaidInfo RaidsModel::infoForRaid(const QModelIndex& index) const
{
    assert(index.isValid());
    assert(getTypeFor(index) == Raid);

    const QModelIndex first_in_row = index.sibling(index.row(), 0);

    QStandardItem* item = m_model.itemFromIndex(first_in_row);
    assert(item != nullptr);

    const auto it = m_infos.find(item);
    assert(it != m_infos.end());

    const RaidId& id = it->second;
    const RaidInfo info = m_raidInfoProvider->getInfoFor(id);

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


RaidsModel::RaidsMap::iterator RaidsModel::itFor(const RaidId& id)
{
     RaidsMap::iterator it = std::find_if(m_infos.begin(), m_infos.end(),
        [&id](const auto& item) { return item.second == id; }
    );

    assert(it != m_infos.end());

    return it;
}


RaidsModel::RaidsMap::const_iterator RaidsModel::itFor(const RaidId& id) const
{
     RaidsMap::const_iterator it = std::find_if(m_infos.begin(), m_infos.end(),
        [&id](const auto& item) { return item.second == id; }
    );

    assert(it != m_infos.end());

    return it;
}


QStandardItem* RaidsModel::itemFor(const RaidId& name) const
{
    RaidsMap::const_iterator it = itFor(name);

    assert(it != m_infos.end());

    return it->first;
}


void RaidsModel::appendRaid(const RaidId& raid_id)
{
    const RaidInfo raidInfo = m_raidInfoProvider->getInfoFor(raid_id);
    append(raidInfo);
}


void RaidsModel::removeRaid(const RaidId& raid_id)
{
    // remove related components
    removeComponentsOf(raid_id);

    QStandardItem* raidItem = itemFor(raid_id);

    const QModelIndex raidIndex = m_model.indexFromItem(raidItem);
    m_model.removeRow(raidIndex.row(), raidIndex.parent());
    m_infos.erase(raidItem);
}


void RaidsModel::updateRaid(const RaidId& raid_id)
{
    // Re-add all components.
    // This is not the nicest solution, but it is easy, and is good enough.

    removeComponentsOf(raid_id);

    auto raidIt = itFor(raid_id);

    const RaidInfo raid = m_raidInfoProvider->getInfoFor(raid_id);

    QStandardItem* raidItem = raidIt->first;
    for (const auto& blkdev : raid.devices())
        appendComponent(raidItem, blkdev);

    // update raid information
    const QModelIndex idxForRaid = m_model.indexFromItem(raidItem);
    const QModelIndex idxForType = idxForRaid.sibling(idxForRaid.row(), 1);   // TODO: use constant for column type
    QStandardItem* typeItem = m_model.itemFromIndex(idxForType);

    // TODO: prepare common code for update and appendRaid()
    typeItem->setText(raid.type());
}


void RaidsModel::append(const RaidInfo& raidInfo)
{
    QStandardItem* raid_device_item = new QStandardItem(raidInfo.device());
    QStandardItem* raid_type_item = new QStandardItem(raidInfo.type());
    QStandardItem* raid_status = new QStandardItem(tr("TO DO"));

    const QList<QStandardItem *> row =
    {
        raid_device_item,
        raid_type_item,
        raid_status,
    };

    for (const auto& blkdev : raidInfo.devices())
        appendComponent(raid_device_item, blkdev);

    m_infos.emplace(raid_device_item, raidInfo.id());
    m_model.appendRow(row);
}


void RaidsModel::removeComponentsOf(const RaidId& raid_id)
{
    QStandardItem* raidItem = itemFor(raid_id);
    const QModelIndex raidIdx = m_model.indexFromItem(raidItem);

    std::vector<QStandardItem *> toRemove;

    for (QModelIndex componentIdx = raidIdx.child(0, 0);
         componentIdx.isValid();
         componentIdx = componentIdx.sibling(componentIdx.row() + 1, 0))
    {
        QStandardItem* componentItem = m_model.itemFromIndex(componentIdx);

        toRemove.push_back(componentItem);
    }

    for(std::size_t i = 0; i < toRemove.size(); i++)
    {
        QStandardItem* componentItem = toRemove[i];
        m_model.removeRow(componentItem->row(), raidIdx);
        m_componentInfos.erase(componentItem);
    }
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
