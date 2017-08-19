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

#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <memory>
#include <vector>


#include "iblock_device.hpp"

class IDiskFilter;
struct IFileSystem;

class DiskController
{
    IFileSystem* m_fileSystem;

    DiskController(const DiskController&) = delete;
    DiskController& operator=(const DiskController&) = delete;
    bool operator==(const DiskController&) = delete;

public:
    DiskController(IFileSystem *);

    std::vector<std::unique_ptr<IBlockDevice>> listDisks(const IDiskFilter&) const;
};

#endif // DISKCONTROLLER_HPP
