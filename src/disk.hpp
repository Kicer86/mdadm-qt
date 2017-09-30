/*
 * Disk representation.
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

#ifndef DISK_HPP
#define DISK_HPP

#include "block_device.hpp"

#include <QString>

class Disk : public BlockDevice
{
private:
    const QString m_name;
    const QString m_model;
    const QString m_serial;

    Disk(const Disk&) = delete;
    Disk& operator=(const Disk&) = delete;

public:
    Disk();
    Disk(const QString &, IFileSystem *);
    Disk(Disk&&) = default;

    bool operator==(const Disk&) const;

    const QString& serial() const;
    const QString& model() const;
    const QString& name() const;
    QString devPath() const;
    QString toString() const;
};

#endif // DISK_HPP
