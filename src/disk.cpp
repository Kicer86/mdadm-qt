#include "disk.hpp"
#include "utils.hpp"

#include "errno.h"
#include "fcntl.h"
#include "unistd.h"

Disk::Disk(const QString& name) :
    m_name(name),
    m_model("N/A"),
    m_serial("N/A"),
    m_logical_block_size(getLogicalBlockSize()),
    m_size(getSize())
{
}

Disk::Disk(Disk&& other) :
    m_name(std::move(other.m_name)),
    m_model(std::move(other.m_model)),
    m_serial(std::move(other.m_serial)),
    m_logical_block_size(std::move(other.m_logical_block_size)),
    m_size(std::move(other.m_size))
{
}

size_t Disk::getSize()
{
    return utils::readValueFromFile<size_t>("/sys/block/" + m_name + "/size");
}

unsigned Disk::getLogicalBlockSize()
{
    return utils::readValueFromFile<unsigned>("/sys/block/" + m_name
                                              + "/queue/logical_block_size");
}

bool Disk::isUsed() const {
    const QString dev_path("/dev/" + m_name);
    bool ret = true;
    int fd = open(QFile::encodeName(dev_path), O_RDONLY | O_EXCL);
    if (fd > 0) {
        ret = false;
        close(fd);
    }
    return ret;
}

bool Disk::operator==(const Disk& other) const
{
    return this == &other ||
            this->m_name == other.m_name ||
            (this->m_serial == other.m_serial &&
             this->m_model == other.m_model);
}

QString Disk::toString() const
{
    return QString("%1, %2, %3, %4 [GiB]\n")
            .arg(m_name).arg(m_model).arg(m_serial)
            .arg(m_logical_block_size * m_size / 1024.0 / 1024.0 / 1024.0);
}
