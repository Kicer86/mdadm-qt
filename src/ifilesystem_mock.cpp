
#include "ifilesystem_mock.hpp"

#include <QTextStream>

#include "printers_for_gmock.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

FakeFileSystem::FakeFileSystem(): m_fs()
{
    EXPECT_CALL(m_fs, listDir(_, _))
        .WillRepeatedly(Invoke(this, &FakeFileSystem::getDirContent));
}


FakeFileSystem::~FakeFileSystem()
{

}


void FakeFileSystem::addFile(const QString& path, const QString& content)
{
    m_files.insert(path);

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


std::deque<QString> FakeFileSystem::getDirContent(const QString& dir, const char* filter)
{
    std::set<QString> result;

    const QStringList dir_hierarchy = dir.split("/");

    for(const QString& file_path: m_files)
    {
        const QStringList file_hierarchy = file_path.split("/");

        const int file_size = file_hierarchy.size();
        const int required_level = dir_hierarchy.size();

        bool ok = true;
        if (file_size > required_level)
            for(std::size_t i = 0; i < required_level; i++)
                if (dir_hierarchy[i] != file_hierarchy[i])
                    ok = false;

        if (ok)
        {
            const QString fileName = file_hierarchy[required_level];

            // TODO: respect filter
            result.insert(fileName);
        }
    }

    return std::deque<QString>(result.cbegin(), result.cend());
}
