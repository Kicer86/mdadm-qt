
#include <gmock/gmock.h>

#include "disk_controller.hpp"

TEST(DiskControllerTest, isConstructible)
{
    EXPECT_NO_THROW(
    {
        DiskController dc(nullptr);
    });
}
