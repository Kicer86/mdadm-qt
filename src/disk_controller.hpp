#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <vector>

#include "disk.hpp"

class DiskFilter {
public:
    enum class Filter {
        NO_FILTER,
        EXCLUDE_USED
    };

private:
    DiskFilter(const DiskFilter&) = delete;
    DiskFilter& operator=(const DiskFilter&) = delete;
    bool operator==(const DiskFilter &) = delete;

    Filter m_filter;

public:
    DiskFilter() : m_filter(Filter::NO_FILTER) {}

    bool operator()(const Disk& disk) const {
        if (m_filter == Filter::EXCLUDE_USED) {
            return !disk.isUsed();
        }
        return true;
    }
};

class DiskController
{
    DiskController(const DiskController&) = delete;
    DiskController& operator=(const DiskController&) = delete;
    bool operator==(const DiskController&) = delete;

public:
    DiskController();

    std::vector<Disk> listDisks(const DiskFilter& df = DiskFilter()) const;

};

#endif // DISKCONTROLLER_HPP
