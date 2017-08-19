/*
 * Filter for exclusion of used devices.
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

#ifndef EXCLUDE_USED_FILTER_H
#define EXCLUDE_USED_FILTER_H

#include "idisk_filter.hpp"

class IBlockDevice;

class ExcludeUsedFilter : public IDiskFilter
{
private:
    ExcludeUsedFilter(const ExcludeUsedFilter&) = delete;
    ExcludeUsedFilter& operator=(const ExcludeUsedFilter&) = delete;
    bool operator==(const ExcludeUsedFilter &) = delete;

public:
    ExcludeUsedFilter() : IDiskFilter()
    {
    }

    bool operator()(const IBlockDevice& disk) const;
};
#endif // EXCLUDE_USED_FILTER_H
