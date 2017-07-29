#ifndef EMPTY_FILTER_H
#define EMPTY_FILTER_H

#include "idisk_filter.hpp"

class Disk;

class EmptyFilter : public IDiskFilter
{
private:
    EmptyFilter(const EmptyFilter&) = delete;
    EmptyFilter& operator=(const EmptyFilter&) = delete;
    bool operator==(const EmptyFilter &) = delete;

public:
    EmptyFilter() : IDiskFilter()
    {
    }

    bool operator()(const IBlockDevice &) const;
};
#endif // EMPTY_FILTER_H
