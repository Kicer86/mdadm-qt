/*
 * Class responsible for communication with block layer.
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

#include "disk_controller.hpp"

#include <QStringList>

#include "idisk_filter.hpp"
#include "iblock_device.hpp"
#include "ifilesystem.hpp"

#include "disk.hpp"
#include "missing.hpp"

DiskController::DiskController(IFileSystem* filesystem): m_fileSystem(filesystem)
{

}

std::vector<std::unique_ptr<IBlockDevice>>
DiskController::listDisks(const IDiskFilter& filter) const
{
    std::vector<std::unique_ptr<IBlockDevice>> disks;

    const std::deque<QString> files =
            m_fileSystem->listDir("/sys/block", "sd*");

    for(const QString& file_name: files)
    {
        std::unique_ptr<Disk> disk(new Disk(file_name, m_fileSystem));
        if (filter(*disk))
            disks.emplace_back(std::move(disk));
    }

    return disks;
}
