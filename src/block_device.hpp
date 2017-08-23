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

#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <QString>

#include "iblock_device.hpp"

struct IFileSystem;

class BlockDevice : public IBlockDevice
{
public:
    BlockDevice();
    BlockDevice(const QString &, IFileSystem *);
    BlockDevice(BlockDevice&&) = default;

    size_t size() const;
    size_t sizeInSectorUnits() const;
    unsigned logicalBlockSize() const;
    bool isUsed() const;

protected:
    const QString m_name;
    const unsigned m_logical_block_size;
    const size_t m_size;
    IFileSystem* m_fileSystem;

private:
    BlockDevice(const BlockDevice&) = delete;
    BlockDevice& operator=(const BlockDevice&) = delete;

};

#endif // BLOCK_DEVICE_H
