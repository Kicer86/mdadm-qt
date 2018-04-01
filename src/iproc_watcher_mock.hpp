
#ifndef IPROC_WATCHER_MOCK_HPP
#define IPROC_WATCHER_MOCK_HPP

#include <gmock/gmock.h>

#include "iproc_watcher.hpp"


struct IProcWatcherMock: public IProcWatcher
{
    MOCK_METHOD1(watch, void(const char *));
};

#endif
