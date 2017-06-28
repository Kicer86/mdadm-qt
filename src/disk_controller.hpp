#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <vector>

#include "disk.hpp"
#include "disk_filter.hpp"

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
