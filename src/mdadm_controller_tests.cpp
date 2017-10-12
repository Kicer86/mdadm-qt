
#include <QTextStream>

#include <gmock/gmock.h>

#include "mdadm_controller.hpp"
#include "ifilesystem_mock.hpp"
#include "imdadm_process_mock.hpp"
#include "iraid_info_provider_mock.hpp"
#include "printers_for_gmock.hpp"

using testing::_;
using testing::DoAll;
using testing::InvokeArgument;
using testing::Return;
using testing::SetArgReferee;


TEST(MDAdmControllerTest, usesRightParametersForRaid0Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md0",
                                        "--level", "stripe", "--raid-devices=2",
                                        "/dev/sda", "/dev/sdc" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md0",
                          MDAdmController::Type::Raid0,
                          QStringList({"/dev/sda", "/dev/sdc"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid0Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1",
                                        "--level", "stripe", "--raid-devices=2",
                                        "/dev/sda", "/dev/sdc",
                                        "--spare-devices=1", "/dev/sde" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("error"), false, 2),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md1",
                          MDAdmController::Type::Raid0,
                          QStringList({"/dev/sda", "/dev/sdc"}),
                          QStringList("/dev/sde"));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid1Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1",
                                        "--level", "mirror", "--raid-devices=3",
                                        "/dev/sda", "/dev/sdc", "/dev/sde" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md1",
                          MDAdmController::Type::Raid1,
                          QStringList({"/dev/sda", "/dev/sdc", "/dev/sde"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid1Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md1",
                                        "--level", "mirror", "--raid-devices=3",
                                        "/dev/sda", "/dev/sdc", "/dev/sde",
                                        "--spare-devices=2", "/dev/sdf",
                                        "/dev/sdg"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md1",
                          MDAdmController::Type::Raid1,
                          QStringList({"/dev/sda", "/dev/sdc", "/dev/sde"}),
                          QStringList({"/dev/sdf", "/dev/sdg"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid4Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md2",
                                        "--level", "4", "--raid-devices=4",
                                        "/dev/sda", "/dev/sdb", "/dev/sdc",
                                        "/dev/sdd" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md2",
                          MDAdmController::Type::Raid4,
                          QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc",
                                       "/dev/sdd"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid4Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md7",
                                        "--level", "4", "--raid-devices=4",
                                        "/dev/sdb", "/dev/sdc", "/dev/sdd",
                                        "/dev/sde", "--spare-devices=1",
                                        "/dev/sdg"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md7",
                          MDAdmController::Type::Raid4,
                          QStringList({"/dev/sdb", "/dev/sdc", "/dev/sdd",
                                      "/dev/sde"}),
                          QStringList({"/dev/sdg"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid5Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md3",
                                        "--level", "5", "--raid-devices=3",
                                        "/dev/sda", "/dev/sdb", "/dev/sdc" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md3",
                          MDAdmController::Type::Raid5,
                          QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid5Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md8",
                                        "--level", "5", "--raid-devices=4",
                                        "/dev/sdb", "/dev/sdc", "/dev/sdd",
                                        "/dev/sde", "--spare-devices=2",
                                        "/dev/sdg", "/dev/sdh"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md8",
                          MDAdmController::Type::Raid5,
                          QStringList({"/dev/sdb", "/dev/sdc", "/dev/sdd",
                                      "/dev/sde"}),
                          QStringList({"/dev/sdg", "/dev/sdh"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid6Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md4",
                                        "--level", "6", "--raid-devices=5",
                                        "/dev/sda", "/dev/sdb", "/dev/sdc",
                                        "/dev/sdd", "/dev/sde" };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md4",
                          MDAdmController::Type::Raid6,
                          QStringList({"/dev/sda", "/dev/sdb", "/dev/sdc",
                                       "/dev/sdd", "/dev/sde"}),
                          QStringList());
}


TEST(MDAdmControllerTest, usesSparesForRaid6Creation)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = { "--create", "--verbose", "/dev/md2",
                                        "--level", "6", "--raid-devices=5",
                                        "/dev/sdb", "/dev/sdc", "/dev/sdd",
                                        "/dev/sde", "/dev/sdf",
                                        "--spare-devices=3",
                                        "/dev/sdg", "/dev/sdh", "/dev/sdad"
                                      };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
        .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                        Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md2",
                          MDAdmController::Type::Raid6,
                          QStringList({"/dev/sdb", "/dev/sdc", "/dev/sdd",
                                      "/dev/sde", "/dev/sdf"}),
                          QStringList({"/dev/sdg", "/dev/sdh", "/dev/sdad"}));
}


TEST(MDAdmControllerTest, usesRightParametersForRaid1CreationWithMissing)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args =
    {
        "--create", "--verbose", "/dev/md7", "--level", "mirror",
        "--raid-devices=2", "/dev/sdb", "missing"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md7",
                          MDAdmController::Type::Raid1,
                          QStringList({"/dev/sdb", "missing"}),
                          QStringList());
}

TEST(MDAdmControllerTest, usesRightParametersForRaid5CreationWithMissing)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args =
    {
        "--create", "--verbose", "/dev/md8", "--level", "5",
        "--raid-devices=3", "/dev/sdm", "missing", "/dev/sdn"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md8",
                          MDAdmController::Type::Raid5,
                          QStringList({"/dev/sdm", "missing", "/dev/sdn"}),
                          QStringList());
}

TEST(MDAdmControllerTest, usesRightParametersForRaid6CreationWithMissing)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args =
    {
        "--create", "--verbose", "/dev/md127", "--level", "6",
        "--raid-devices=4", "/dev/sdl", "/dev/sdm", "missing",
        "missing"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.createRaid("/dev/md127",
                          MDAdmController::Type::Raid6,
                          QStringList({"/dev/sdl", "/dev/sdm", "missing",
                                       "missing"}),
                          QStringList());
}

TEST(MDAdmControllerTest, usesRightParameterForRaidStop)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md127"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

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

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

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


    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    EXPECT_TRUE(controller.zeroSuperblock(QStringList { "/dev/sdb",
                                            "/dev/sdc",
                                            "/dev/sde"}));
}


TEST(MDAdmControllerTest,
     usesRightParameterForRaidRemoval)
{
    IRaidInfoProviderMock raid_info_provider;
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md127",
        "--zero-superblock",
        "/dev/sdb",
        "/dev/sdc",
        "/dev/sdd"
    };

    const QStringList slaves = {"/dev/sdb", "/dev/sdc", "/dev/sdd"};
    const QString device_name = "md127";

    EXPECT_CALL(raid_info_provider, listComponents(device_name, _))
        .WillOnce(DoAll(SetArgReferee<1>(slaves), Return(true)));

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, &raid_info_provider);
    EXPECT_TRUE(controller.removeRaid("/dev/md127"));
}


TEST(MDAdmControllerTest,
     usesRightParameterForEmptyRaidRemoval)
{
    IMDAdmProcessMock mdadm_process;
    IRaidInfoProviderMock raidInfoProvider;

    const QStringList expected_args = {
        "--stop",
        "--verbose",
        "/dev/md4"
    };

    const QString device_name = "md4";

    EXPECT_CALL(raidInfoProvider, listComponents(device_name, _))
        .WillOnce(Return(false));

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0), Return(true)));

    MDAdmController controller(&mdadm_process, &raidInfoProvider);
    EXPECT_TRUE(controller.removeRaid("/dev/md4"));
}


TEST(MDAdmControllerTest,
     simulateProcessCrash)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "--zero-superblock",
        "/dev/sdb"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("crash"), false, 0), Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    EXPECT_TRUE(controller.zeroSuperblock(QStringList { "/dev/sdb" }));
}


TEST(MDAdmControllerTest, usesRightParameterForMarkAsFaulty)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "/dev/md10",
        "--fail",
        "/dev/sdo"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.markAsFaulty("/dev/md10", "/dev/sdo");
}


TEST(MDAdmControllerTest, usesRightParameterForReAdd)
{
    IMDAdmProcessMock mdadm_process;

    const QStringList expected_args = {
        "/dev/md2",
        "--re-add",
        "/dev/sdc"
    };

    EXPECT_CALL(mdadm_process, execute(expected_args, _, _))
            .WillOnce(DoAll(InvokeArgument<1>(QByteArray("done"), true, 0),
                            Return(true)));

    MDAdmController controller(&mdadm_process, nullptr);
    controller.reAdd("/dev/md2", "/dev/sdc");
}
