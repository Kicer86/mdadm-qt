#include <QFile>
#include <QDir>
#include <QTextStream>

#include "disk.hpp"

Disk::Disk(const QString& name) :
    m_name(name),
    m_model("N/A"),
    m_serial("N/A"),
    m_logical_block_size(getLogicalBlockSize(name)),
    m_size(getSize(name))
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

template <typename T>
T Disk::readFromSysfs(const QString &path)
{
    QFile file(path);
    T value {};

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream file_stream(&file);
        file_stream >> value;
        file.close();
    }

    return value;
}

size_t Disk::getSize(const QString& name)
{
    return readFromSysfs<size_t>("/sys/block/" + name + "/size");
}

unsigned Disk::getLogicalBlockSize(const QString& name)
{
    return readFromSysfs<unsigned>("/sys/block/" + name
                                   + "/queue/logical_block_size");
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
