/*
 * Interface for MDAdm process
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

#ifndef IMDADMPROCESS_HPP
#define IMDADMPROCESS_HPP

#include <functional>
#include <string>
#include <vector>

struct ArrayInfo
{
    std::string array_device;
    std::vector<std::string> block_devices;
    std::string array_type;

    ArrayInfo(const std::string& _array_device,
              const std::vector<std::string>& _block_devices,
              const std::string& _type):
        array_device(_array_device),
        block_devices(_block_devices),
        array_type(_type)
    {}
};

struct IMDAdmProcess
{
    virtual ~IMDAdmProcess() = default;

    typedef std::function<void(const std::vector<ArrayInfo> &)> ListResult;

    virtual bool listArrays(const ListResult &) = 0;  // list arrays asynchronicaly, call function when done
};

#endif // IMDADMPROCESS_HPP
