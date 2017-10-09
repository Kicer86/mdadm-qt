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

#include "imdadm_controller.hpp"


struct IMDAdmProcess;
struct IFileSystem;


class MDAdmController: public QObject, public IMDAdmController
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

        // overrides
        std::vector<RaidId> listRaids() const override;
        RaidInfo getInfoFor(const IMDAdmController::RaidId & ) const override;

        // operations
        bool listRaids(const ListResult &) const;       // list raids asynchronicaly, call ListResult when done
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
        mutable std::map<RaidId, RaidInfo> m_infoCache;
        IMDAdmProcess* m_mdadmProcess;
        IFileSystem* m_fileSystem;

    signals:
        void raidCreated();
        void raidRemoved();
        void componentStateUpdated();
};

#endif // MDADMCONTROLLER_HPP
