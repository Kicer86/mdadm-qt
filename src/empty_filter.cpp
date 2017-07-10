#include "empty_filter.hpp"

#include "disk.hpp"

bool EmptyFilter::operator()(const Disk&) const
{
    return true;
}
