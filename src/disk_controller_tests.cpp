
#include <gmock/gmock.h>

#include "disk.hpp"
#include "disk_controller.hpp"
#include "idisk_filter_mock.hpp"
#include "ifilesystem_mock.hpp"


using ::testing::_;
using ::testing::Return;

TEST(DiskControllerTest, isConstructible)
{
    EXPECT_NO_THROW(
    {
        DiskController dc(nullptr);
    });
}


TEST(DiskControllerTest, returnsAllDevicesWhenEmptyFilterIsBeingUsed)
{
    IDiskFilterMock filter;
    IFileSystemMock fs;

    const std::deque<QString> drives = { "sda", "sdb", "sdy", "sdz" };

    EXPECT_CALL(filter, func_op(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(fs, listDir(QString("/sys/block"), _))
        .WillOnce(Return(drives));

    DiskController dc(&fs);

    std::vector<std::unique_ptr<IBlockDevice>> devices = dc.listDisks(filter);

    ASSERT_EQ(devices.size(), 4);
}
