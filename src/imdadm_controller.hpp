
#ifndef IMDADM_CONTROLLER_HPP
#define IMDADM_CONTROLLER_HPP

#include <QStringList>


struct IMDAdmController
{
    virtual ~IMDAdmController() = default;

        virtual QStringList listRaids() const = 0;
};


#endif
