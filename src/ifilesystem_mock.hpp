#ifndef IFILESYSTEM_MOCK_HPP
#define IFILESYSTEM_MOCK_HPP

#include <gmock/gmock.h>

#include "ifilesystem.hpp"

struct IFileSystemMock : public IFileSystem
{
    struct IFileMock : public IFile
    {
        MOCK_METHOD0(getStream, QTextStream*());
    };

    MOCK_METHOD2(openFile, std::unique_ptr<IFile>(const QString&, const QIODevice::OpenMode&));
    MOCK_METHOD2(listDir, std::deque<QString>(const QString&, const char*));
};


class FakeFileSystem
{
    public:
        FakeFileSystem();
        ~FakeFileSystem();

        void addFile(const QString& path, const QString& content);
        IFileSystem* getFileSystem();

    private:
        IFileSystemMock m_fs;
        std::deque<QString> m_contents;
        std::deque<QTextStream> m_streams;
};


#endif // IFILESYSTEM_MOCK_HPP
