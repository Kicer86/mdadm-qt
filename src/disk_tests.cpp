
#include <QDebug>

#include <gmock/gmock.h>

#include "disk.hpp"
#include "ifilesystem_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::Return;


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


TEST(DiskTest, returnsProperSerial)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_EQ(disk.serial(), "N/A");
}


TEST(DiskTest, returnsProperModel)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_EQ(disk.model(), "N/A");
}


TEST(DiskTest, returnsProperName)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_EQ(disk.name(), "sda");
}


TEST(DiskTest, returnsProperDevPath)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_EQ(disk.devPath(), "/dev/sda");
}


TEST(DiskTest, returnsProperIdentification)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1073741824");    // 1 GiB
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_EQ(disk.toString(), "sda, N/A, N/A, 4096 [GiB]\n");
}


TEST(DiskTest, returnsTrueForEqualObjects)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk1("sda", fs.getFileSystem());
    Disk disk2("sda", fs.getFileSystem());

    EXPECT_TRUE(disk1 == disk2);
}


TEST(DiskTest, returnsFalseForDifferentObjects)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");
    fs.addFile("/sys/block/sdb/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sdb/size", "4096");

    Disk disk1("sda", fs.getFileSystem());
    Disk disk2("sdb", fs.getFileSystem());

    EXPECT_FALSE(disk1 == disk2);
}


TEST(DiskTest, returnsTrueWhenComparingObjectWithItself)
{
    FakeFileSystem fs;
    fs.addFile("/sys/block/sda/queue/logical_block_size", "1024");
    fs.addFile("/sys/block/sda/size", "4096");

    Disk disk("sda", fs.getFileSystem());

    EXPECT_TRUE(disk == disk);
}
