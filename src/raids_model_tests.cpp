


#include <gmock/gmock.h>

#include "raids_model.hpp"


TEST(RaidsModelTests, isEmptyWhenConstructed)
{
    RaidsModel model;

    QAbstractItemModel* qt_model = model.model();

    EXPECT_EQ(false, qt_model->canFetchMore(QModelIndex()));
    EXPECT_EQ(0, qt_model->rowCount());
    EXPECT_EQ(3, qt_model->columnCount());
}
