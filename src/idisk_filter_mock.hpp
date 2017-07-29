#ifndef IDISK_FILTER_MOCK_HPP
#define IDISK_FILTER_MOCK_HPP

#include "idisk_filter.hpp"

struct IDiskFilterMock: IDiskFilter
{
    MOCK_CONST_METHOD1(func_op, bool(const IBlockDevice& disk));

    virtual bool operator()(const IBlockDevice& disk) const { return func_op(disk); }
};

#endif // IDISK_FILTER_MOCK_HPP
