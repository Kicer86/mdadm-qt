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


RaidsModel::RaidsModel(): m_model(), m_infos()
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
        QStandardItem* raid_blk_devices_item = new QStandardItem(raid.block_devices.join(", "));

        m_infos.emplace(raid_device_item, raid);
        
        const QList<QStandardItem *> row = { raid_device_item, raid_type_item, raid_blk_devices_item };
        m_model.appendRow(row);
    }
}


const RaidInfo& RaidsModel::infoForRow(int row) const
{
    assert(row < m_infos.size());

    const QModelIndex idx = m_model.index(row, 0, QModelIndex());
    QStandardItem* item = m_model.itemFromIndex(idx);    
    assert(item != nullptr);
    
    const auto it = m_infos.find(item);
    assert(it != m_infos.end());
    
    return it->second;
}


QAbstractItemModel* RaidsModel::model()
{
    return &m_model;
}
