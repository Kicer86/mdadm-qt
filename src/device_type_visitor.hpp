/*
 * Device type visitor
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

#ifndef DEVICETYPEVISITOR_HPP
#define DEVICETYPEVISITOR_HPP

#include <functional>

#include "idevice_visitor.hpp"

class IBlockDevice;

class DeviceTypeVisitor: public IDeviceVisitor
{
    public:
        DeviceTypeVisitor();
        virtual ~DeviceTypeVisitor() = default;

        void setMissingHandler(const std::function<void(Missing *)> &);
        void setDiskHandler(const std::function<void(Disk *)> &);

        void goFor(IBlockDevice *);

    private:
        std::function<void(Missing *)> m_missing;
        std::function<void(Disk *)> m_disk;

        void visit(Disk *) override;
        void visit(Missing *) override;
};

#endif // DEVICETYPEVISITOR_HPP
