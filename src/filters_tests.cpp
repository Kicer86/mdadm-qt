
#include <gmock/gmock.h>

#include "empty_filter.hpp"
#include "exclude_used_filter.hpp"
#include "iblock_device_mock.hpp"

using ::testing::Return;

TEST(EmptyFilterTest, acceptsEverything)
{
    EmptyFilter filter;

    IBlockDeviceMock device;
    // no calls expected

    EXPECT_EQ(filter(device), true);
}


TEST(ExcludeUsedFilterTest, acceptsNotUsed)
{
    ExcludeUsedFilter filter;

    IBlockDeviceMock device;
    EXPECT_CALL(device, isUsed())
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_EQ(filter(device), false);    // mock will return "is used", do not accept it
    EXPECT_EQ(filter(device), true);     // mock will return "not used", accept it
}
