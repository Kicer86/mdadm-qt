
#include <QDebug>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "ifilesystem_mock.hpp"
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

void PrintTo(const std::deque<QString>& string_deque, std::ostream* os)
{
    QString output;
    QDebug debug(&output);

    for (const auto& element : string_deque)
    {
        debug << element;
    }

    *os << output.toStdString();
}

void PrintTo(const QString& str, std::ostream* os)
{
    *os << str.toStdString();
}


TEST(MDAdmControllerTest, isConstructible)
{
    EXPECT_NO_THROW({
        MDAdmController controller(nullptr, nullptr);
    });
}


TEST(MDAdmControllerTest, usesRightParametersForRaid0Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md0", "--level", "stripe", "--raid-devices=2", "/dev/sda", "/dev/sdc" };
    EXPECT_CALL(mdadm_process, execute(expected_args, _))
        .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md0", MDAdmController::Type::Raid0, QStringList({"/dev/sda", "/dev/sdc"}) );
}


TEST(MDAdmControllerTest, usesRightParametersForRaid1Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1", "--level", "mirror", "--raid-devices=3", "/dev/sda", "/dev/sdc", "/dev/sde" };
    EXPECT_CALL(mdadm_process, execute(expected_args, _))
        .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md1", MDAdmController::Type::Raid1, QStringList({"/dev/sda", "/dev/sdc", "/dev/sde"}) );
}


TEST(MDAdmControllerTest, usesRightParametersForRaid4Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md2", "--level", "4", "--raid-devices=4", "/dev/sda", "/dev/sdb", "/dev/sdc", "/dev/sdd" };
    EXPECT_CALL(mdadm_process, execute(expected_args, _))
        .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md2", MDAdmController::Type::Raid4, QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc", "/dev/sdd"}) );
}


TEST(MDAdmControllerTest, usesRightParametersForRaid5Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md3", "--level", "5", "--raid-devices=3", "/dev/sda", "/dev/sdb", "/dev/sdc" };
    EXPECT_CALL(mdadm_process, execute(expected_args, _))
        .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md3", MDAdmController::Type::Raid5, QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc"}) );
}


TEST(MDAdmControllerTest, usesRightParametersForRaid6Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md4", "--level", "6", "--raid-devices=5", "/dev/sda", "/dev/sdb", "/dev/sdc", "/dev/sdd", "/dev/sde" };
    EXPECT_CALL(mdadm_process, execute(expected_args, _))
        .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md4", MDAdmController::Type::Raid6, QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc", "/dev/sdd", "/dev/sde"}) );
}


TEST(MDAdmControllerTest, usesRightParameterForRaidStop)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md127"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _))
            .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.stopRaid("/dev/md127");
}


TEST(MDAdmControllerTest,
     usesEmptyListForZeroSuperblockFunction)
{
    IMDAdmProcessMock mdadm_process;

    MDAdmController controller(&mdadm_process, nullptr);
    EXPECT_FALSE(controller.zeroSuperblock(QStringList()));
}

TEST(MDAdmControllerTest,
     usesRightParameterForClearingMetadataFromSingleDrive)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--zero-superblock",
        "/dev/sdb"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _))
            .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    EXPECT_TRUE(controller.zeroSuperblock(QStringList { "/dev/sdb" }));
}


TEST(MDAdmControllerTest,
     usesRightParameterForClearingMetadataFromMultipleDrives)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--zero-superblock",
        "/dev/sdb",
        "/dev/sdc",
        "/dev/sde"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _))
            .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, nullptr);
    EXPECT_TRUE(controller.zeroSuperblock(QStringList { "/dev/sdb",
                                            "/dev/sdc",
                                            "/dev/sde"}));
}

TEST(MDAdmControllerTest,
     usesRightParameterForRaidRemoval)
{
    IMDAdmProcessMock mdadm_process;
    IFileSystemMock filesystem;

    QString slavesPath("/sys/block/md127/slaves");

    QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md127",
        "--zero-superblock",
        "/dev/sdb",
        "/dev/sdc",
        "/dev/sdd"
    };

    EXPECT_CALL(filesystem, listDir(slavesPath, _))
            .WillOnce(Return(
                          std::deque<QString> { "sdb", "sdc", "sdd" }));

    EXPECT_CALL(mdadm_process, execute(expected_args, _))
            .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, &filesystem);
    EXPECT_TRUE(controller.removeRaid("/dev/md127"));

}

TEST(MDAdmControllerTest,
     usesRightParameterForEmptyRaidRemoval)
{
    IMDAdmProcessMock mdadm_process;
    IFileSystemMock filesystem;

    QString slavesPath("/sys/block/md4/slaves");

    QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md4"
    };

    EXPECT_CALL(filesystem, listDir(slavesPath, _))
            .WillOnce(Return(
                          std::deque<QString> { }));

    EXPECT_CALL(mdadm_process, execute(expected_args, _))
            .WillOnce(Return(true));

    MDAdmController controller(&mdadm_process, &filesystem);
    EXPECT_TRUE(controller.removeRaid("/dev/md4"));

}
