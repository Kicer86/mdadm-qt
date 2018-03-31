
#ifndef IMDADM_CONTROLLER_HPP
#define IMDADM_CONTROLLER_HPP

#include <vector>

#include <QObject>

#include "raid_info.hpp"
#include "objects_ids.hpp"

class IRaidInfoProvider: public QObject
{
        Q_OBJECT

    public:
        virtual ~IRaidInfoProvider() = default;

        virtual std::vector<RaidInfo> listRaids() const = 0;
        virtual RaidInfo getInfoFor(const RaidId &) const = 0;

        // TODO: refactor, base on RaidId
        virtual bool listComponents(const QString& raid_device,
                                    QStringList& block_devices) const = 0;

    signals:
        void raidAdded(const RaidId &) const;
        void raidRemoved(const RaidId &) const;
        void raidChanged(const RaidId &) const;

        // signal emited at any modification in /proc/mdstat (together with signals above)
        void raidsModified() const;
};


#endif
