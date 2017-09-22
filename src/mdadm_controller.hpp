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

    bool operator==(const RaidComponentInfo&) const;
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

    private:
        IMDAdmProcess* m_mdadmProcess;
        IFileSystem* m_fileSystem;

    signals:
        void raidCreated();
        void raidRemoved();
};

#endif // MDADMCONTROLLER_HPP
