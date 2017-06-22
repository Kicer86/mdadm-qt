
#include <QDebug>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "imdadm_process_mock.hpp"

using testing::_;
using testing::Return;

void PrintTo(const QStringList& string_list, std::ostream* os)
{
    QString output;
    QDebug debug(&output);
    debug << string_list;

    *os << output.toStdString();
}


TEST(MDAdmControllerTest, isConstructible)
{
    EXPECT_NO_THROW({
        MDAdmController controller(nullptr);
    });
}


TEST(MDAdmControllerTest, usesRightParametersForRaid0Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md0", "--level", "stripe", "--raid-devices=2", "/dev/sda", "/dev/sdc" };
    EXPECT_CALL(mdadm_process, execute(expected_args, _))
        .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process);
    controller.createRaid("/dev/md0", MDAdmController::Type::Raid0, QStringList({"/dev/sda", "/dev/sdc"}) );
}
