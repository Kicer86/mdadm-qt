
#ifndef IMDADM_PROCESS_MOCK_HPP
#define IMDADM_PROCESS_MOCK_HPP

#include <gmock/gmock.h>

#include "imdadm_process.hpp"

struct IMDAdmProcessMock: IMDAdmProcess
{
    MOCK_METHOD3(execute, bool(const QStringList &,
                               const ExecutionResult &,
                               const ReadChannelParser &));
};

#endif
