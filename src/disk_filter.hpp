#ifndef DISK_FILTER_H
#define DISK_FILTER_H

class Disk;

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
    DiskFilter(const Filter& filter = Filter::NO_FILTER) :
        m_filter(filter) {}

    bool operator()(const Disk& disk) const;
};
#endif // DISK_FILTER_H
