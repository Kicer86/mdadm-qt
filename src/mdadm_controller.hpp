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
#include <map>
#include <vector>

#include <QObject>
#include <QStringList>

#include "iraid_info_provider.hpp"


struct IMDAdmProcess;
struct IRaidInfoProvider;
struct IFileSystem;


struct ScanInfo
{
    /* scan details */
    /* scan types:
    *   resync
    *      redundancy is being recalculated after unclean shutdown or creation
    *   recover
    *      a hot spare is being built to replace a failed/missing device
    *   idle
    *     nothing is happening
    *   check
    *     A full check of redundancy was requested and is happening. This reads
    *     all blocks and checks them. A repair may also happen for some raid
    *     levels.
    *   repair
    *     A full check and repair is happening. This is similar to resync,
    *     but was requested by the user, and the write-intent bitmap is NOT used
    *     to optimise the process.
    */

    enum class ScanType
    {
        Idle,
        Check,
        Repair,
        Recovery,
        Resync,
        Reshape,
        Frozen,
    };

    enum class ReshapeDirection
    {
        Forward,
        Backward
    };

    ScanType last_scan;
    ScanType sync_action;

    uint64_t mismatch_cnt;
    ReshapeDirection reshape_direction;
    uint64_t reshape_position;
    uint64_t resync_start; /* start position */

    std::tuple<uint64_t, uint64_t> progress; /* sync_completed */
    std::tuple<uint64_t, uint64_t> scan_limits; /* sync_max, sync_min */
    unsigned sync_speed;
    std::tuple<unsigned, unsigned> speed_limits;

};

class MDAdmController: public QObject
{
        Q_OBJECT

    public:
        // OutputParser - callback function for process read channel
        typedef std::function<QString(const QString &)> OutputParser;

        enum class Type
        {
            Raid0,
            Raid1,
            Raid4,
            Raid5,
            Raid6,
        };

        MDAdmController(IMDAdmProcess *, IRaidInfoProvider *, IFileSystem *);
        MDAdmController(const MDAdmController &) = delete;
        ~MDAdmController();

        MDAdmController& operator=(const MDAdmController &) = delete;
        bool operator==(const MDAdmController &) const = delete;

        bool createRaid(const QString& raid_device, Type,
                        const QStringList& block_devices,
                        const QStringList& spare_devices,
                        const OutputParser& output_parser = nullptr);
        bool removeRaid(const QString& raid_device);
        bool stopRaid(const QString& raid_device);
        bool zeroSuperblock(const QStringList& raid_components);
        bool markAsFaulty(const QString& raid_device, const QString& component);
        bool reAdd(const QString& raid_device, const QString& component);
        bool runScan(const QString& raid_device,
                     const ScanInfo::ScanType scan_type);
        bool stopScan(const QString& raid_device);
        bool pauseScan(const QString& raid_device);
        bool resumeScan(const QString& raid_device);
        ScanInfo getScanData(const QString&);

    private:
        IMDAdmProcess* m_mdadmProcess;
        IRaidInfoProvider* m_raidInfoProvider;
        IFileSystem* m_fileSystem;

        QString scanTypeToString(const ScanInfo::ScanType) const;
        ScanInfo::ScanType scanStringToType(const QString &) const;
        QString reshapeDirectionToString(
                const ScanInfo::ReshapeDirection) const;
        ScanInfo::ReshapeDirection stringToReshapeDirection(
                const QString &) const;

    signals:
        void raidCreated();
        void raidRemoved();
        void componentStateUpdated();
};

#endif // MDADMCONTROLLER_HPP
