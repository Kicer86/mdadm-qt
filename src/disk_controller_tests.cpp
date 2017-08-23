
#include <gmock/gmock.h>

#include "disk.hpp"
#include "disk_controller.hpp"
#include "idisk_filter_mock.hpp"
#include "ifilesystem_mock.hpp"
#include "printers_for_gmock.hpp"


using ::testing::_;
using ::testing::Return;


TEST(DiskControllerTest, returnsAllDevicesWhenEmptyFilterIsBeingUsed)
{
    IDiskFilterMock filter;
    FakeFileSystem fs;

    for(const char* device: {"sda", "sdb", "sdy", "sdz"})
    {
        fs.addFile( QString("/sys/block/%1/queue/logical_block_size").arg(device), "1024");
        fs.addFile( QString("/sys/block/%1/size").arg(device), "4096");
        fs.addFile( QString("/mnt/block/%1").arg(device), "1234567890");          // some random file to complicate situation
    }

    EXPECT_CALL(filter, func_op(_)).WillRepeatedly(Return(true));

    DiskController dc(fs.getFileSystem());

    std::vector<std::unique_ptr<IBlockDevice>> devices = dc.listDisks(filter);

    ASSERT_EQ(devices.size(), 4);
}
