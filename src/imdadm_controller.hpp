
#ifndef IMDADM_CONTROLLER_HPP
#define IMDADM_CONTROLLER_HPP

#include <vector>

#include "raidinfo.hpp"

struct IMDAdmController
{
    typedef QString RaidId;

    virtual ~IMDAdmController() = default;

    virtual std::vector<RaidId> listRaids() const = 0;
    virtual RaidInfo getInfoFor(const RaidId &) const = 0;
};


#endif
