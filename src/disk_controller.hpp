#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <memory>
#include <vector>


#include "iblock_device.hpp"

class IDiskFilter;

class DiskController
{
    DiskController(const DiskController&) = delete;
    DiskController& operator=(const DiskController&) = delete;
    bool operator==(const DiskController&) = delete;

public:
    DiskController();

    std::vector<std::unique_ptr<IBlockDevice>> listDisks(const IDiskFilter&) const;

};

#endif // DISKCONTROLLER_HPP
