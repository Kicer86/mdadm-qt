/*
 * Device type visitor
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

#include "device_type_visitor.hpp"

#include "iblock_device.hpp"

DeviceTypeVisitor::DeviceTypeVisitor():
    m_missing(),
    m_disk()
{
}


void DeviceTypeVisitor::setMissingHandler(const std::function<void(Missing *)>& handler)
{
    m_missing = handler;
}


void DeviceTypeVisitor::setDiskHandler(const std::function<void(Disk *)>& handler)
{
    m_disk = handler;
}


void DeviceTypeVisitor::goFor(IBlockDevice* dev)
{
    dev->accept(this);
}


void DeviceTypeVisitor::visit(Disk* d)
{
    if (m_disk)
        m_disk(d);
}


void DeviceTypeVisitor::visit(Missing* m)
{
    if (m_missing)
        m_missing(m);
}
