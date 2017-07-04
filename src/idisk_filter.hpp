#ifndef IDISK_FILTER_HPP
#define IDISK_FILTER_HPP

class Disk;

struct IDiskFilter
{
    virtual ~IDiskFilter() = default;
    virtual bool operator()(const Disk& disk) const = 0;
};

#endif // IDISK_FILTER_HPP
