#ifndef IFILESYSTEM_MOCK_HPP
#define IFILESYSTEM_MOCK_HPP

#include <gmock/gmock.h>

#include "ifilesystem.hpp"

struct IFileSystemMock : public IFileSystem
{
    MOCK_METHOD2(openFile, std::unique_ptr<IFile>(const QString&, const QIODevice::OpenMode&));
    MOCK_METHOD2(listDir, std::deque<QString>(const QString&, const char*));
};

#endif // IFILESYSTEM_MOCK_HPP
