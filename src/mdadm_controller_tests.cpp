
#include <QDebug>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "ifilesystem_mock.hpp"
#include "imdadm_process_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::Return;


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

    const QString slavesPath("/sys/block/md127/slaves");

    const QStringList expected_args = {
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

    const QString slavesPath("/sys/block/md4/slaves");

    const QStringList expected_args = {
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

typedef std::unique_ptr<IFileSystemMock::IFileMock> ifileptr;

TEST(MDAdmControllerTest,
     listInactiveRaid0)
{
    IFileSystemMock filesystem;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4]\n"
                         "md1 : inactive sdf[1](S)\n"
                         "      130048 blocks super 1.2\n"
                         "\n"
                         "unused devices: <none>\n");
    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    EXPECT_CALL(filesystem, openFile(QString("/proc/mdstat"), QIODevice::ReadOnly |
                                     QIODevice::Text))
            .WillOnce(::testing::Invoke(
                          [&outputStream](const QString&,
                                          QIODevice::OpenMode)
                                         ->ifileptr
    {
        ifileptr ifile(new IFileSystemMock::IFileMock);
        EXPECT_CALL(*ifile, getStream()).WillOnce(Return(&outputStream));
        return ifile;
    }));

    MDAdmController controller(nullptr, &filesystem);

    EXPECT_TRUE(controller.listRaids([](const std::vector<RaidInfo>& raids)
    {
        const QStringList devices { "sdf" };
        const RaidInfo expected_info("md1", devices, "");
        ASSERT_EQ(raids.size(), 1);
        EXPECT_EQ(raids[0], expected_info);
    }));

}

TEST(MDAdmControllerTest,
     listActiveRaid5)
{
    IFileSystemMock filesystem;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4]\n"
                         "md0 : active raid5 sdb[1] sdc[3] sdd[0]\n"
                         "      5860270080 blocks super 1.2 level 5, "
                         "512k chunk, algorithm 2 [3/3] [UUU]\n"
                         "\n"
                         "unused devices: <none>\n");
    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    EXPECT_CALL(filesystem, openFile(QString("/proc/mdstat"), QIODevice::ReadOnly |
                                     QIODevice::Text))
            .WillOnce(::testing::Invoke(
                          [&outputStream](const QString&,
                                          QIODevice::OpenMode)
                                          ->ifileptr
    {
        ifileptr ifile(new IFileSystemMock::IFileMock);
        EXPECT_CALL(*ifile, getStream()).WillOnce(Return(&outputStream));
        return ifile;
    }));

    MDAdmController controller(nullptr, &filesystem);

    EXPECT_TRUE(controller.listRaids([](const std::vector<RaidInfo>& raids)
    {
        const QStringList devices { "sdb", "sdc", "sdd" };
        const RaidInfo expected_info("md0", devices, "raid5");

        ASSERT_EQ(raids.size(), 1);
        EXPECT_EQ(raids[0], expected_info);
    }));
}
