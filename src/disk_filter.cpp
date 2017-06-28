#include "disk_filter.hpp"

#include "disk.hpp"

bool DiskFilter::operator()(const Disk& disk) const {
    if (m_filter == Filter::EXCLUDE_USED) {
        return !disk.isUsed();
    }
    return true;
}
