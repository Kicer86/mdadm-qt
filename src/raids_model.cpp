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


RaidsModel::RaidsModel(): 
    m_model(), 
    m_infos(), 
    m_componentInfos(),
    m_diskType({{ RaidComponentInfo::Type::Normal, "active" },
                { RaidComponentInfo::Type::Faulty, "faulty" },
                { RaidComponentInfo::Type::Journal, "journal" },
                { RaidComponentInfo::Type::Replacement, "replacement" },
                { RaidComponentInfo::Type::Spare, "spare" },
                { RaidComponentInfo::Type::WriteMostly, "write mostly" },
               })
{
    m_model.setHorizontalHeaderLabels( { tr("device"), tr("type"), tr("status") } );
}


RaidsModel::~RaidsModel()
{

}


void RaidsModel::load(const std::vector<RaidInfo>& raids)
{
    m_infos.clear();
    m_componentInfos.clear();
    const int rows = m_model.rowCount();
    m_model.removeRows(0, rows);     // .clear() would clear headers also

    for(const RaidInfo& raid: raids)
    {
        QStandardItem* raid_device_item = new QStandardItem(raid.raid_device);
        QStandardItem* raid_type_item = new QStandardItem(raid.raid_type);
        QStandardItem* raid_status = new QStandardItem(tr("TO DO"));

        const QList<QStandardItem *> row =
        {
            raid_device_item,
            raid_type_item,
            raid_status,
        };

        for (const auto& blkdev : raid.block_devices)
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
            row.first()->appendRow(leaf);
        }

        m_infos.emplace(raid_device_item, raid);        
        m_model.appendRow(row);
    }
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
