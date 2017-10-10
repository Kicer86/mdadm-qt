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

#include <QObject>
#include <QStringList>


struct IMDAdmProcess;
struct IFileSystem;

struct RaidComponentInfo
{
    enum class Type {
        Normal,
        WriteMostly = 'W',
        Journal = 'J',
        Faulty = 'F',
        Spare = 'S',
        Replacement ='R',
    };

    QString name;
    Type type;
    int descriptor_index;

    RaidComponentInfo(const QString& _name, Type _type, int _descr_nr) :
        name(_name),
        type(_type),
        descriptor_index(_descr_nr)
    {
    }

    RaidComponentInfo(const RaidComponentInfo &) = default;
    RaidComponentInfo(RaidComponentInfo &&) = default;
    RaidComponentInfo& operator=(const RaidComponentInfo &) = default;
    RaidComponentInfo& operator=(RaidComponentInfo &&) = default;

    bool operator==(const RaidComponentInfo &) const;
    bool operator<(const RaidComponentInfo &) const;
};

struct RaidInfo
{
    /*
     * device types in mdstat:
     * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/drivers/md/md.c#n7711
     */

    QString raid_device;
    QList<RaidComponentInfo> block_devices;
    QString raid_type;

    RaidInfo (const QString& _raid_device,
              const QList<RaidComponentInfo>& _block_devices,
              const QString& _type):
        raid_device(_raid_device),
        block_devices(_block_devices),
        raid_type(_type)
    {}

    RaidInfo(const RaidInfo &) = default;
    RaidInfo(RaidInfo &&) = default;

    RaidInfo& operator=(const RaidInfo &) = default;
    RaidInfo& operator=(RaidInfo &&) = default;

    bool operator==(const RaidInfo&) const;
    bool operator!=(const RaidInfo&) const;
    bool operator<(const RaidInfo&) const;
};

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
        // ListResult - callback function for listRaids
        typedef std::function<void(const std::vector<RaidInfo> &)> ListResult;
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

        MDAdmController(IMDAdmProcess *, IFileSystem *);
        MDAdmController(const MDAdmController &) = delete;
        ~MDAdmController();

        MDAdmController& operator=(const MDAdmController &) = delete;
        bool operator==(const MDAdmController &) const = delete;

        // operations
        bool listRaids(const ListResult &);             // list raids asynchronicaly, call ListResult when done
        bool listComponents(const QString& raid_device,
                            QStringList& block_devices);
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
