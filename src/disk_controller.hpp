#ifndef DISKCONTROLLER_HPP
#define DISKCONTROLLER_HPP

#include <functional>
#include <memory>
#include <vector>


#include "iblock_device.hpp"

class IDiskFilter;
struct IFileSystem;

class DiskController
{
    IFileSystem* m_fileSystem;

    DiskController(const DiskController&) = delete;
    DiskController& operator=(const DiskController&) = delete;
    bool operator==(const DiskController&) = delete;

public:
    DiskController(IFileSystem *);

    std::vector<std::unique_ptr<IBlockDevice>> listDisks(const IDiskFilter&) const;
    std::unique_ptr<IBlockDevice> getMissingDevice() const;
};

#endif // DISKCONTROLLER_HPP
