#include "exclude_used_filter.hpp"
#include "disk.hpp"

bool ExcludeUsedFilter::operator()(const Disk& disk) const
{
    return !disk.isUsed();
}
