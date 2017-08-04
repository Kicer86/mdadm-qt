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
    m_infos = raids;
    const int rows = m_model.rowCount();
    m_model.removeRows(0, rows);     // .clear() would clear headers also

    for(const RaidInfo& raid: raids)
    {
        QStandardItem* raid_device_item = new QStandardItem(raid.raid_device);
        QStandardItem* raid_type_item = new QStandardItem(raid.raid_type);
        QStandardItem* raid_blk_devices_item = new QStandardItem(raid.block_devices.join(", "));

        const QList<QStandardItem *> row = { raid_device_item, raid_type_item, raid_blk_devices_item };
        m_model.appendRow(row);
    }
}


const RaidInfo& RaidsModel::infoForRow(int row) const
{
    assert(row < m_infos.size());

    return m_infos[row];
}


int RaidsModel::columnCount(const QModelIndex& parent) const
{
    return m_model.columnCount(parent);
}


QVariant RaidsModel::data(const QModelIndex& index, int role) const
{
    return m_model.data(index, role);
}


QModelIndex RaidsModel::index(int row, int column, const QModelIndex& parent) const
{
    return m_model.index(row, column, parent);
}


QModelIndex RaidsModel::parent(const QModelIndex& child) const
{
    return m_model.parent(child);
}


int RaidsModel::rowCount(const QModelIndex& parent) const
{
    return m_model.rowCount(parent);
}


QVariant RaidsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model.headerData(section, orientation, role);
}
