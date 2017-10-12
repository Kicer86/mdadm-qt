/*
 * Structure with information about raid.
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
 */

#include "raid_info.hpp"


bool RaidComponentInfo::operator==(const RaidComponentInfo& other) const
{
    return this->name == other.name &&
           this->type == other.type &&
           this->descriptor_index == other.descriptor_index;
}

bool RaidComponentInfo::operator<(const RaidComponentInfo& other) const
{
    bool less = false;

    if (this->name < other.name)
        less = true;
    else if (this->name == other.name)
    {
        if (this->type < other.type)
            less = true;
        else if (this->type == other.type)
            less = this->descriptor_index < other.descriptor_index;
    }

    return less;
}

bool RaidInfo::operator==(const RaidInfo &other) const
{
    return this->raid_device == other.raid_device &&
           this->raid_type == other.raid_type &&
           this->block_devices == other.block_devices;
}


bool RaidInfo::operator!=(const RaidInfo& other) const
{
    return this->raid_device != other.raid_device ||
           this->raid_type != other.raid_type ||
           this->block_devices != other.block_devices;
}


bool RaidInfo::operator<(const RaidInfo& other) const
{
    bool less = false;

    if (this->raid_device < other.raid_device)
        less = true;
    else if (this->raid_device == other.raid_device)
    {
        if (this->raid_type < other.raid_type)
            less = true;
        else if (this->raid_type == other.raid_type)
            less = this->block_devices < other.block_devices;
    }

    return less;
}
