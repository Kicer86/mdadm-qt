/*
 * Generic block device representation.
 * Copyright (C) 2017  Arkadiusz Bubała <arek2407066@gmail.com>
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

#include "block_device.hpp"
#include "ifilesystem.hpp"
#include "utils.hpp"

namespace
{
    size_t getSize(IFileSystem* fs, const QString& name)
    {
        return utils::readValueFromFile<size_t>(fs,
                                                "/sys/block/" + name + "/size");
    }

    unsigned getLogicalBlockSize(IFileSystem* fs, const QString& name)
    {
        return utils::readValueFromFile<unsigned>(fs,
                                                  "/sys/block/" + name
                                                    + "/queue/logical_block_size");
    }
}


BlockDevice::BlockDevice():
    IBlockDevice(),
    m_name(""),
    m_logical_block_size(0),
    m_size(0),
    m_fileSystem(nullptr)
{
}


BlockDevice::BlockDevice(const QString& name, IFileSystem* filesystem) :
    IBlockDevice(),
    m_name(name),
    m_logical_block_size(getLogicalBlockSize(filesystem, name)),
    m_size(getSize(filesystem, name)),
    m_fileSystem(filesystem)
{

}

inline size_t BlockDevice::size() const
{
    return m_size * m_logical_block_size;
}

inline size_t BlockDevice::sizeInSectorUnits() const
{
    return m_size;
}

inline unsigned BlockDevice::logicalBlockSize() const
{
    return m_logical_block_size;
}

bool BlockDevice::isUsed() const
{
    const QString dev_path("/dev/" + m_name);
    const bool used = m_fileSystem->isDeviceUsed(dev_path);

    return used;
}
