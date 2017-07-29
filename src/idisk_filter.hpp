#ifndef IDISK_FILTER_HPP
#define IDISK_FILTER_HPP

class IBlockDevice;

struct IDiskFilter
{
    virtual ~IDiskFilter() = default;
    virtual bool operator()(const IBlockDevice& disk) const = 0;
};

#endif // IDISK_FILTER_HPP
