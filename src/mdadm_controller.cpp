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
#include "utils.hpp"

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

    inline QString getScanPath(const QString& raid_device)
    {
        return "/sys/block/" + raid_device + "/md/";
    }

    void nullResultCallback(const QByteArray &, bool,int) {  }
}


MDAdmController::MDAdmController(IMDAdmProcess* mdadmProcess,
                                 IRaidInfoProvider* raidInfoProvider,
                                 IFileSystem* filesystem
                                ):
    m_mdadmProcess(mdadmProcess),
    m_raidInfoProvider(raidInfoProvider),
    m_fileSystem(filesystem)
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

/*
 * Scan functions
 */

QString MDAdmController::scanTypeToString(const ScanInfo::ScanType type) const
{
    QMap<ScanInfo::ScanType, QString> scanTypes =
    {
        { ScanInfo::ScanType::Idle, "idle" },
        { ScanInfo::ScanType::Check, "check" },
        { ScanInfo::ScanType::Recovery, "recover" },
        { ScanInfo::ScanType::Repair, "repair" },
        { ScanInfo::ScanType::Resync, "resync" },
        { ScanInfo::ScanType::Reshape, "reshape" },
        { ScanInfo::ScanType::Frozen, "frozen" },
    };

    return scanTypes.value(type);
}

ScanInfo::ScanType
MDAdmController::scanStringToType(const QString& type) const
{
    QMap<QString, ScanInfo::ScanType> scanTypes =
    {
        { "idle", ScanInfo::ScanType::Idle },
        { "check", ScanInfo::ScanType::Check },
        { "recovery", ScanInfo::ScanType::Recovery },
        { "repair", ScanInfo::ScanType::Repair },
        { "resync", ScanInfo::ScanType::Resync },
        { "reshape", ScanInfo::ScanType::Reshape },
        { "frozen", ScanInfo::ScanType::Frozen },
    };

    return scanTypes.value(type, ScanInfo::ScanType::Idle);
}

bool MDAdmController::runScan(const QString& raid_device,
                              const ScanInfo::ScanType scan_type)
{
    const QString scan_action_path =
            "/sys/block/" + raid_device + "/md/sync_action";

    return utils::writeValueToFile(m_fileSystem, scan_action_path,
                                   scanTypeToString(scan_type));
}

QString MDAdmController::reshapeDirectionToString(
        const ScanInfo::ReshapeDirection direction) const
{
    QMap<ScanInfo::ReshapeDirection, QString> reshapeDirections =
    {
        { ScanInfo::ReshapeDirection::Backward, "backwards" },
        { ScanInfo::ReshapeDirection::Forward, "forwards" },
    };

    return reshapeDirections.value(direction);
}

ScanInfo::ReshapeDirection MDAdmController::stringToReshapeDirection(
        const QString& direction) const
{
    QMap<QString, ScanInfo::ReshapeDirection> reshapeDirections =
    {
        { "backwards", ScanInfo::ReshapeDirection::Backward },
        { "forwards", ScanInfo::ReshapeDirection::Forward },
    };

    return reshapeDirections.value(direction,
                                   ScanInfo::ReshapeDirection::Forward);
}

ScanInfo MDAdmController::getScanData(const QString& raid_device)
{
    const QString sync_action_path = getScanPath(raid_device) + "sync_action";
    const QString last_sync_action_path =
            getScanPath(raid_device) + "last_sync_action";
    const QString mismatch_cnt_path = getScanPath(raid_device) + "mismatch_cnt";
    const QString reshape_direction_path =
            getScanPath(raid_device) + "reshape_direction";
    const QString reshape_position_path =
            getScanPath(raid_device) + "reshape_position";
    const QString resync_start_path =
            getScanPath(raid_device) + "resync_start";
    const QString sync_completed_path =
            getScanPath(raid_device) + "sync_completed";
    const QString sync_max_path =
            getScanPath(raid_device) + "sync_max";
    const QString sync_min_path =
            getScanPath(raid_device) + "sync_min";
    const QString sync_speed_path =
            getScanPath(raid_device) + "sync_speed";
    const QString sync_speed_max_path =
            getScanPath(raid_device) + "sync_speed_max";
    const QString sync_speed_min_path =
            getScanPath(raid_device) + "sync_speed_min";

    ScanInfo scan_info;
    scan_info.sync_action = scanStringToType(
                utils::readValueFromFile<QString>(m_fileSystem,
                                                  sync_action_path));
    scan_info.last_scan = scanStringToType(
                utils::readValueFromFile<QString>(m_fileSystem,
                                                  last_sync_action_path));
    scan_info.mismatch_cnt =
            utils::readValueFromFile<uint64_t>(m_fileSystem,
                                                mismatch_cnt_path);
    scan_info.reshape_direction = stringToReshapeDirection(
                utils::readValueFromFile<QString>(m_fileSystem,
                                                  reshape_direction_path));
    scan_info.reshape_position =
            utils::readValueFromFile<uint64_t>(m_fileSystem,
                                                reshape_position_path);
    scan_info.resync_start =
            utils::readValueFromFile<uint64_t>(m_fileSystem,
                                               resync_start_path);

    auto sync_completed_file = m_fileSystem->openFile(sync_completed_path);
    QTextStream* sync_completed_stream = sync_completed_file->getStream();
    const auto progress = sync_completed_stream?
            utils::readValueFromFile<QString>(*sync_completed_stream):
            "none";

    if (progress == "none" || progress == "delayed")
        scan_info.progress = std::make_tuple(0, 0);
    else
    {
        utils::readValueFromFile<QString>(*sync_completed_stream); //spacer
        const auto end =
                utils::readValueFromFile<QString>(*sync_completed_stream);
        scan_info.progress = std::make_tuple(progress.toUInt(),
                                             end.toUInt());
    }

    scan_info.scan_limits = std::make_tuple(
                utils::readValueFromFile<uint64_t>(m_fileSystem,
                                                   sync_min_path),
                utils::readValueFromFile<uint64_t>(m_fileSystem,
                                                   sync_max_path));
    scan_info.sync_speed =
            utils::readValueFromFile<unsigned>(m_fileSystem, sync_speed_path);
    scan_info.speed_limits = std::make_tuple(
                utils::readValueFromFile<uint64_t>(m_fileSystem,
                                                   sync_speed_min_path),
                utils::readValueFromFile<uint64_t>(m_fileSystem,
                                                   sync_speed_max_path));
    return scan_info;
}

bool MDAdmController::stopScan(const QString& raid_device)
{
    const QString sync_min_path =
            getScanPath(raid_device) + "sync_min";

    return (runScan(raid_device, ScanInfo::ScanType::Frozen) &&
            utils::writeValueToFile(m_fileSystem, sync_min_path, "0") &&
            // TODO: Retest if it doesn't restart scan action
            runScan(raid_device, ScanInfo::ScanType::Idle));
}

bool MDAdmController::pauseScan(const QString& raid_device)
{
    return runScan(raid_device, ScanInfo::ScanType::Frozen);
}

bool MDAdmController::resumeScan(const QString& raid_device)
{
    bool ret = false;
    ScanInfo scan_info = getScanData(raid_device);
    if (scan_info.sync_action == ScanInfo::ScanType::Frozen) {
        ret = runScan(raid_device, scan_info.last_scan);
    }

    return ret;
}
