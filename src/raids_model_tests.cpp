


#include <gmock/gmock.h>

#include "raids_model.hpp"


class RaidsModelTests: public testing::Test
{
    public:

    protected:
        const RaidComponentInfo dev1 = RaidComponentInfo
        (
            "dev1",
            RaidComponentInfo::Type::Normal,
            128
        );

        const RaidComponentInfo dev2 = RaidComponentInfo
        (
            "dev2",
            RaidComponentInfo::Type::Spare,
            129
        );

        const RaidComponentInfo dev3 = RaidComponentInfo
        (
            "dev3",
            RaidComponentInfo::Type::Normal,
            130
        );

        const RaidComponentInfo dev4 = RaidComponentInfo
        (
            "dev4",
            RaidComponentInfo::Type::Spare,
            131
        );

        const RaidComponentInfo dev5 = RaidComponentInfo
        (
            "dev5",
            RaidComponentInfo::Type::Normal,
            132
        );

        const RaidComponentInfo dev6 = RaidComponentInfo
        (
            "dev6",
            RaidComponentInfo::Type::Spare,
            133
        );

        const RaidComponentInfo dev7 = RaidComponentInfo
        (
            "dev7",
            RaidComponentInfo::Type::Normal,
            134
        );

        const RaidComponentInfo dev8 = RaidComponentInfo
        (
            "dev8",
            RaidComponentInfo::Type::Spare,
            135
        );

        const RaidInfo raid1 = RaidInfo
        (
            "md1",
            { dev1, dev2 },
             "type 0"
        );

        const RaidInfo raid2 = RaidInfo
        (
            "md2",
            { dev3, dev4, dev5, dev6 },
             "type 1"
        );

        const RaidInfo raid3 = RaidInfo
        (
            "md3",
            { dev7, dev8 },
             "type 2"
        );
};


TEST_F(RaidsModelTests, isEmptyWhenConstructed)
{
    RaidsModel model;

    QAbstractItemModel* qt_model = model.model();

    EXPECT_EQ(false, qt_model->canFetchMore(QModelIndex()));
    EXPECT_EQ(0, qt_model->rowCount());
    EXPECT_EQ(3, qt_model->columnCount());
}


TEST_F(RaidsModelTests, addingRaidsToEmptyModel)
{
    RaidsModel model;

    const std::vector<RaidInfo> raids = {raid1, raid2};

    model.load(raids);

    QAbstractItemModel* qt_model = model.model();

    if (qt_model->canFetchMore(QModelIndex()))
       qt_model->fetchMore(QModelIndex());

    EXPECT_EQ(3, qt_model->columnCount());
    ASSERT_EQ(2, qt_model->rowCount());

    const QModelIndex first_raid = qt_model->index(0, 0);
    const QModelIndex second_raid = qt_model->index(1, 0);

    if (qt_model->canFetchMore(first_raid))
        qt_model->fetchMore(first_raid);

    if (qt_model->canFetchMore(second_raid))
        qt_model->fetchMore(second_raid);

    EXPECT_EQ(2, qt_model->rowCount(first_raid));
    EXPECT_EQ(4, qt_model->rowCount(second_raid));
}
