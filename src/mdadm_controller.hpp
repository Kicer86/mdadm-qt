/*
 * Class responsible for communication with mdadm process
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

#ifndef MDADMCONTROLLER_HPP
#define MDADMCONTROLLER_HPP

#include <functional>
#include <vector>

#include <QStringList>


struct IMDAdmProcess;

struct ArrayInfo
{
    QString array_device;
    QStringList block_devices;
    QString array_type;

    ArrayInfo(const QString& _array_device,
              const QStringList& _block_devices,
              const QString& _type):
        array_device(_array_device),
        block_devices(_block_devices),
        array_type(_type)
    {}
};

class MDAdmController
{
    public:
        typedef std::function<void(const std::vector<ArrayInfo> &)> ListResult;

        MDAdmController(IMDAdmProcess *);
        MDAdmController(const MDAdmController &) = delete;
        ~MDAdmController();

        MDAdmController& operator=(const MDAdmController &) = delete;
        bool operator==(const MDAdmController &) const = delete;

        // operations
        bool listArrays(const ListResult &);             // list arrays asynchronicaly, call ListResult when done

    private:
        IMDAdmProcess* m_mdadmProcess;
};

#endif // MDADMCONTROLLER_HPP
