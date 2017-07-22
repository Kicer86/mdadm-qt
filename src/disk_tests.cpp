
#include <QDebug>

#include <gmock/gmock.h>

#include "disk.hpp"
#include "ifilesystem_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::Return;

class FakeFileSystem
{
    public:
        FakeFileSystem(): m_fs()
        {

        }

        void addFile(const QString& path, const QString& content)
        {
            m_contents.push_back(content);
            QString& current_content = m_contents.back();

            m_streams.emplace_back(&current_content, QIODevice::ReadOnly | QIODevice::Text);
            QTextStream& current_stream = m_streams.back();

            EXPECT_CALL(m_fs, openFile(path, _))
                .WillOnce(::testing::Invoke(
                            [&current_stream](const QString&, QIODevice::OpenMode)
            {
                std::unique_ptr<IFileSystemMock::IFileMock> file
                    = std::make_unique<IFileSystemMock::IFileMock>();

                EXPECT_CALL(*file, getStream()).WillOnce(Return(&current_stream));
                return file;
            }));
        }

        IFileSystem* getFileSystem()
        {
            return &m_fs;
        }

    private:
        IFileSystemMock m_fs;
        std::deque<QString> m_contents;
        std::deque<QTextStream> m_streams;
};


TEST(DiskTest, simpleConstruction)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    EXPECT_NO_THROW(
    {
        Disk disk("sda", fs.getFileSystem());
    });
}


TEST(DiskTest, returnsProperSizes)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_EQ(disk.size(), 1024 * 4096);
    EXPECT_EQ(disk.sizeInSectorUnits(), 4096);
    EXPECT_EQ(disk.logicalBlockSize(), 1024);
}
