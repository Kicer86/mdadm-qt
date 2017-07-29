#include "empty_filter.hpp"

#include "iblock_device.hpp"

bool EmptyFilter::operator()(const IBlockDevice &) const
{
    return true;
}
