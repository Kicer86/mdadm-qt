/*
 * Empty disk filter class.
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

#ifndef EMPTY_FILTER_H
#define EMPTY_FILTER_H

#include "idisk_filter.hpp"

class Disk;

class EmptyFilter : public IDiskFilter
{
private:
    EmptyFilter(const EmptyFilter&) = delete;
    EmptyFilter& operator=(const EmptyFilter&) = delete;
    bool operator==(const EmptyFilter &) = delete;

public:
    EmptyFilter() : IDiskFilter()
    {
    }

    bool operator()(const IBlockDevice &) const;
};
#endif // EMPTY_FILTER_H
