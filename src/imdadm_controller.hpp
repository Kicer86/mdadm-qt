
#ifndef IMDADM_CONTROLLER_HPP
#define IMDADM_CONTROLLER_HPP

#include <vector>


struct IMDAdmController
{
    typedef QString RaidId;

    virtual ~IMDAdmController() = default;

    virtual std::vector<RaidId> listRaids() const = 0;
};


#endif
