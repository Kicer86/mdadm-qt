
#include "ifilesystem_mock.hpp"

#include <QTextStream>

using ::testing::_;
using ::testing::Return;

FakeFileSystem::FakeFileSystem(): m_fs()
{

}


FakeFileSystem::~FakeFileSystem()
{

}


void FakeFileSystem::addFile(const QString& path, const QString& content)
{
    m_contents.push_back(content);
    QString& current_content = m_contents.back();

    m_streams.emplace_back(&current_content, QIODevice::ReadOnly | QIODevice::Text);
    QTextStream& current_stream = m_streams.back();

    EXPECT_CALL(m_fs, openFile(path, _))
        .WillRepeatedly(::testing::Invoke(
                    [&current_stream](const QString&, QIODevice::OpenMode)
    {
        std::unique_ptr<IFileSystemMock::IFileMock> file
            = std::make_unique<IFileSystemMock::IFileMock>();

        EXPECT_CALL(*file, getStream())
            .WillRepeatedly(Return(&current_stream));

        return file;
    }));
}


IFileSystem* FakeFileSystem::getFileSystem()
{
    return &m_fs;
}
