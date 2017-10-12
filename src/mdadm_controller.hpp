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

        MDAdmController(IMDAdmProcess *, IRaidInfoProvider *);
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

    private:
        IMDAdmProcess* m_mdadmProcess;
        IRaidInfoProvider* m_raidInfoProvider;

    signals:
        void raidCreated();
        void raidRemoved();
        void componentStateUpdated();
};

#endif // MDADMCONTROLLER_HPP
