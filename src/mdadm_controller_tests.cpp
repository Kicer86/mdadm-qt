
#include <gmock/gmock.h>

#include "mdadm_controller.hpp"

TEST(MDAdmControllerTest, isConstructible)
{
    EXPECT_NO_THROW({
        MDAdmController controller(nullptr);
    });
}
