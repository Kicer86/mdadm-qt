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

RaidsModel::RaidsModel(): m_model()
{

}


RaidsModel::~RaidsModel()
{

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
