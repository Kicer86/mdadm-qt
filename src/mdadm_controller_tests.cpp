
#include <QDebug>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "ifilesystem_mock.hpp"
#include "imdadm_process_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::Return;


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

typedef std::unique_ptr<IFileSystemMock::IFileMock> FilePtr;

void mockMdstatOutput(IFileSystemMock& filesystem, QTextStream* const outputStream)
{
    EXPECT_CALL(filesystem, openFile(QString("/proc/mdstat"), QIODevice::ReadOnly |
                                     QIODevice::Text))
            .WillOnce(::testing::Invoke(
                          [outputStream](const QString&,
                                          QIODevice::OpenMode)
                                         ->FilePtr
    {
        FilePtr ifile(new IFileSystemMock::IFileMock);
        EXPECT_CALL(*ifile, getStream()).WillOnce(Return(outputStream));
        return ifile;
    }));

}

void compareListOutput(MDAdmController& controller,
                       const std::vector<RaidInfo>& expected)
{
    EXPECT_TRUE(controller.listRaids(
                    [&expected](const std::vector<RaidInfo>& raids)
    {
        ASSERT_EQ(raids.size(), expected.size());
        auto expectedIt = expected.cbegin();
        for (const auto& element : raids)
        {
            EXPECT_EQ(element, *expectedIt);
            ++expectedIt;
        }
    }));
}

TEST(MDAdmControllerTest,
     listInactiveRaid0)
{
    IFileSystemMock filesystem;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4]\n"
                         "md1 : inactive sdf[1](S)\n"
                         "      130048 blocks super 1.2\n"
                         "\n"
                         "unused devices: <none>\n");
    QTextStream outputStream(&mdstatOutput);

    mockMdstatOutput(filesystem, &outputStream);

    MDAdmController controller(nullptr, &filesystem);

    const std::vector<RaidInfo> expectedOutput =
    {
        RaidInfo("md1", QStringList("sdf"), "")
    };

    compareListOutput(controller, expectedOutput);
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

    mockMdstatOutput(filesystem, &outputStream);

    MDAdmController controller(nullptr, &filesystem);

    const std::vector<RaidInfo> expectedOutput =
    {
        RaidInfo("md0", QStringList({"sdb", "sdc", "sdd"}), "raid5")
    };

    compareListOutput(controller, expectedOutput);
}

TEST(MDAdmControllerTest,
     listActiveRaid0Raid1Raid6)
{
    IFileSystemMock filesystem;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4] [raid0] "
                         "[raid1] [raid10]\n"
                         "md11 : active raid0 sdm[1] sdl[0]\n"
                         "       260096 blocks super 1.2 512k chunks\n"
                         "\n"
                         "md8 : active raid6 sdk[3] sdj[2] sdi[1] sdh[0]\n"
                         "      260096 blocks super 1.2 level 6, 512k chunk, "
                         "algorithm 2 [4/4] [UUUU]\n"
                         "\n"
                         "md3 : active raid1 sdg[1] sdf[0]\n"
                         "      130880 blocks super 1.2 [2/2] [UU]\n"
                         "\n"
                         "unused devices: <none>\n");
    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    MDAdmController controller(nullptr, &filesystem);

    const std::vector<RaidInfo> expectedOutput =
    {
        RaidInfo("md11", QStringList({"sdm", "sdl"}), "raid0"),
        RaidInfo("md8", QStringList({"sdk", "sdj", "sdi", "sdh"}), "raid6"),
        RaidInfo("md3", QStringList({"sdg", "sdf"}), "raid1")
    };

    compareListOutput(controller, expectedOutput);
}

TEST(MDAdmControllerTest, listNoRaids)
{
    IFileSystemMock filesystem;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4] [raid0] "
                         "[raid1] [raid10]\n"
                         "\n"
                         "unused devices: <none>\n");

    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    MDAdmController controller(nullptr, &filesystem);

    const std::vector<RaidInfo> expectedOutput;

    compareListOutput(controller, expectedOutput);
}

TEST(MDAdmControllerTest, listDegradedRaid)
{
    IFileSystemMock filesystem;

    QString mdstatOutput("Personalities : [raid6] [raid5] [raid4] [raid0] "
                         "[raid1] [raid10]\n"
                         "md8 : active (auto-read-only) raid6 sdk[3] sdj[2] "
                         "sdh[0]\n"
                         "      260096 blocks super 1.2 level 6, 512k chunk, "
                         "algorithm 2 [4/3] [U_UU]\n"
                         "\n"
                         "unused devices: <none>\n");

    QTextStream outputStream(&mdstatOutput, QIODevice::ReadOnly);

    mockMdstatOutput(filesystem, &outputStream);

    MDAdmController controller(nullptr, &filesystem);

    const std::vector<RaidInfo> expectedOutput =
    {
        RaidInfo("md8", QStringList({"sdk", "sdj", "sdh"}), "raid6"),
    };

    compareListOutput(controller, expectedOutput);
}
