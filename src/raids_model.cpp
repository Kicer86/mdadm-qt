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


RaidsModel::RaidsModel(): m_model(), m_infos(),
    m_diskType({{ RaidComponent::Type::Normal, "active" },
                { RaidComponent::Type::Faulty, "faulty" },
                { RaidComponent::Type::Journal, "journal" },
                { RaidComponent::Type::Replacement, "replacement" },
                { RaidComponent::Type::Spare, "spare" },
                { RaidComponent::Type::WriteMostly, "write mostly" },
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

            row.first()->appendRow(leaf);
        }

        m_infos.emplace(raid_device_item, raid);
        
        m_model.appendRow(row);
    }
}


RaidData RaidsModel::infoForIndex(const QModelIndex& index) const
{
    assert(index.isValid());

    auto parent = index.parent();

    if (parent.isValid()) /* this is RAID component */
    {        
        const QModelIndex parent = index.parent();
        const RaidInfo& info = infoFor(parent);        
        
        RaidData data = {
            info,
            info.block_devices[index.row()].name
        };
        return data;
    }
    else
    {
        const QModelIndex first_in_row = index.sibling(index.row(), 0);
        const RaidInfo& info = infoFor(first_in_row);        
        
        RaidData data = { info, QString() };
        return data;
    }
}


QAbstractItemModel* RaidsModel::model()
{
    return &m_model;
}


const RaidInfo& RaidsModel::infoFor(const QModelIndex& index) const
{
    QStandardItem* item = m_model.itemFromIndex(index);    
    assert(item != nullptr);
    
    const auto it = m_infos.find(item);
    assert(it != m_infos.end());
        
    return it->second;
}
