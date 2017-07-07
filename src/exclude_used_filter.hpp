#ifndef EXCLUDE_USED_FILTER_H
#define EXCLUDE_USED_FILTER_H

#include "idisk_filter.hpp"

class Disk;

class ExcludeUsedFilter : public IDiskFilter
{
private:
    ExcludeUsedFilter(const ExcludeUsedFilter&) = delete;
    ExcludeUsedFilter& operator=(const ExcludeUsedFilter&) = delete;
    bool operator==(const ExcludeUsedFilter &) = delete;

public:
    ExcludeUsedFilter() : IDiskFilter()
    {
    }

    bool operator()(const Disk& disk) const;
};
#endif // EXCLUDE_USED_FILTER_H
