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

#include <cassert>

#include <QRegExp>
#include <QFileInfo>

#include "imdadm_process.hpp"

namespace
{
    QString levelName(MDAdmController::Type type)
    {
        QString result;

        switch(type)
        {
            case MDAdmController::Type::Raid0:
                result = "stripe";
                break;

            case MDAdmController::Type::Raid1:
                result = "mirror";
                break;

            case MDAdmController::Type::Raid4:
                result = "4";
                break;

            case MDAdmController::Type::Raid5:
                result = "5";
                break;

            case MDAdmController::Type::Raid6:
                result = "6";
                break;
        }

        return result;
    }

    void nullResultCallback(const QByteArray &, bool,int) {  }
}


MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess,
                                 IRaidInfoProvider* raidInfoProvider):
    m_mdadmProcess(mdadmProcess),
    m_raidInfoProvider(raidInfoProvider)
{

}


MDAdmController::~MDAdmController()
{

}


bool MDAdmController::createRaid(const QString& raid_device,
                                 MDAdmController::Type type,
                                 const QStringList& block_devices,
                                 const QStringList& spare_devices,
                                 const OutputParser& callback)
{
    QStringList mdadm_args;

    mdadm_args << "--create" << "--verbose" << raid_device;
    mdadm_args << "--level" << levelName(type);
    mdadm_args << QString("--raid-devices=%1")
                  .arg(block_devices.size())
               << block_devices;
    if (!spare_devices.empty())
    {
        mdadm_args << QString("--spare-devices=%1")
                      .arg(spare_devices.size())
                   << spare_devices;
    }

    m_mdadmProcess->execute(mdadm_args,
                            [this](const QByteArray& output,
                                               bool success,
                                               int exitCode)
                            {
                                if (success)
                                    emit raidCreated();
                            },
                            [callback](const QByteArray& output)->QString
                            {
                                return callback?
                                       callback(output):
                                       QString();
                            });

    return true;
}


bool MDAdmController::removeRaid(const QString& raid_device)
{
    QStringList mdadm_args;
    QStringList components;

    mdadm_args << "--stop" << "--verbose" << raid_device;

    if (m_raidInfoProvider->listComponents(QFileInfo(raid_device).baseName(), components))
    {
        mdadm_args << "--zero-superblock" << components;
    }

    m_mdadmProcess->execute(mdadm_args, [this](const QByteArray& output,
                                               bool success,
                                               int exitCode)
    {
        if (success)
            emit raidRemoved();
    });

    return true;
}

bool MDAdmController::stopRaid(const QString& raid_device)
{
    QStringList mdadm_args;

    mdadm_args << "--stop" << "--verbose" << raid_device;

    m_mdadmProcess->execute(mdadm_args, nullResultCallback);

    return true;
}

bool MDAdmController::zeroSuperblock(const QStringList& raid_components)
{
    QStringList mdadm_args;

    if (raid_components.isEmpty())
        return false;

    mdadm_args << "--zero-superblock" << raid_components;

    m_mdadmProcess->execute(mdadm_args, nullResultCallback);

    return true;
}

bool MDAdmController::markAsFaulty(const QString& raid_device,
                                   const QString& component)
{
    QStringList mdadm_args;

    mdadm_args << raid_device << "--fail" << component;

    m_mdadmProcess->execute(mdadm_args, [this](const QByteArray &,
                                               bool success,
                                               int)
    {
        if (success)
            emit componentStateUpdated();
    });

    emit componentStateUpdated();

    return true;
}

bool MDAdmController::reAdd(const QString& raid_device,
                            const QString& component)
{
    QStringList mdadm_args;

    mdadm_args << raid_device << "--re-add" << component;

    m_mdadmProcess->execute(mdadm_args, [this](const QByteArray &,
                                               bool success,
                                               int)
    {
        if (success)
            emit componentStateUpdated();
    });

    return true;
}
