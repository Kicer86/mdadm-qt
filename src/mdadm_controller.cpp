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

#include "mdadm_controller.hpp"

#include <QRegExp>
#include <QFile>

MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess): m_mdadmProcess(mdadmProcess)
{

}


MDAdmController::~MDAdmController()
{

}


bool MDAdmController::listRaids (const ListResult& result)
{
    QFile mdstat("/proc/mdstat");

    if (!mdstat.exists() || !mdstat.open(QIODevice::ReadOnly |
                                         QIODevice::Text))
        return false;

    const QRegExp mdadm_info("^(md[^ ]+) : ([^ ]+) ([^ ]+) (.*)\n");
    std::vector<RaidInfo> results;

    while(!mdstat.atEnd())
    {
        const QByteArray outputLine = mdstat.readLine();

        if (mdadm_info.exactMatch(outputLine))
        {
            const QString dev = mdadm_info.cap(1);
            const QString status = mdadm_info.cap(2);
            const QString type = mdadm_info.cap(3);
            const QString devices = mdadm_info.cap(4);

            const QStringList devices_list = devices.split(" ");

            results.emplace_back(dev, devices_list, type);
        }
    }
    mdstat.close();

    result(results);

    return true;
}
