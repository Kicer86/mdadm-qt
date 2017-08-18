/*
 * Wrapper over mdadm process
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

#include "mdadm_process.hpp"

#include <QProcess>


MDAdmProcess::MDAdmProcess()
{

}


MDAdmProcess::~MDAdmProcess()
{

}

bool MDAdmProcess::execute(const QStringList& args,
                           const ExecutionResult& result,
                           const ReadChannelParser& parser)
{
    QProcess* mdadm = new QProcess;
    mdadm->setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(mdadm, &QProcess::readyReadStandardOutput,
                     [parser, mdadm]()
    {
        if (parser != nullptr) {
            QByteArray channel = mdadm->readAll();
            QString response = parser(channel) + '\n';
            if (!response.isEmpty())
                mdadm->write(response.toStdString().c_str());
        }
    });

    QObject::connect(mdadm, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
        [result, mdadm](int exitCode, QProcess::ExitStatus status)
    {
        const QByteArray output = mdadm->readAll();
        
        result(output, status == QProcess::NormalExit, exitCode);

        mdadm->deleteLater();
    });

    // TODO: find path to mdadm
    mdadm->start("mdadm", args, QProcess::ReadWrite);

    return true;
}
