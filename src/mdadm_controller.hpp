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

struct RaidInfo
{
    QString raid_device;
    QStringList block_devices;
    QString raid_type;

    RaidInfo (const QString& _raid_device,
              const QStringList& _block_devices,
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
                        const QStringList& block_devices);
        bool removeRaid(const QString& raid_device);
        bool stopRaid(const QString& raid_device);
        bool zeroSuperblock(const QStringList& raid_components);

    private:
        IMDAdmProcess* m_mdadmProcess;
        IFileSystem* m_fileSystem;

    signals:
        void raidCreated();
};

#endif // MDADMCONTROLLER_HPP
