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
#include <QProcess>


MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess): m_mdadmProcess(mdadmProcess)
{

}


MDAdmController::~MDAdmController()
{

}


bool MDAdmController::listArrays(const ListResult& result)
{
    QProcess* mdstat = new QProcess;

    QObject::connect(mdstat, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
        [result, mdstat](int exitCode, QProcess::ExitStatus exitStatus)
    {
        //                         arr device  status   type   devices
        const QRegExp mdadm_info("^(md[^ ]+) : ([^ ]+) ([^ ]+) (.*)");
        while(mdstat->canReadLine())
        {
            const QByteArray outputLine = mdstat->readLine();


        }

        mdstat->deleteLater();
    });

    mdstat->start("cat", {"/proc/mdstat"}, QProcess::ReadOnly);

    return true;
}
