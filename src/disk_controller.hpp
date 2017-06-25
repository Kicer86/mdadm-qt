#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <vector>

#include "disk.hpp"

class DiskController
{
    DiskController(const DiskController&) = delete;
    DiskController& operator=(const DiskController&) = delete;
    bool operator==(const DiskController&) = delete;
public:
    // ListResult - callback function for listDisks
    typedef std::function<void(const std::vector<Disk> &)> ListResult;

    DiskController();

    bool listDisks(const ListResult &);

};

#endif // DISKCONTROLLER_HPP
