#include "exclude_used_filter.hpp"
#include "iblock_device.hpp"

bool ExcludeUsedFilter::operator()(const IBlockDevice& disk) const
{
    return !disk.isUsed();
}
