/*
 * The "missing" device representation.
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

#include "missing.hpp"
#include <QObject>

Missing::Missing() : IBlockDevice()
{

}

size_t Missing::size() const
{
    return 0;
}

size_t Missing::sizeInSectorUnits() const
{
    return 0;
}

unsigned Missing::logicalBlockSize() const
{
    return 0;
}

bool Missing::isUsed() const
{
    return false;
}

bool Missing::isPhysical() const
{
    return false;
}

QString Missing::devPath() const
{
    return "missing";
}

QString Missing::toString() const
{
    return QObject::tr("Missing device representation");
}
