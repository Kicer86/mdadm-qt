#include "disk.hpp"
#include "utils.hpp"



Disk::Disk(const QString& name) :
    BlockDevice(name),
    m_name(name),
    m_model("N/A"),
    m_serial("N/A")
{
}

Disk::Disk(Disk&& other) :
    BlockDevice(std::move(other)),
    m_name(std::move(other.m_name)),
    m_model(std::move(other.m_model)),
    m_serial(std::move(other.m_serial))
{
}

bool Disk::operator==(const Disk& other) const
{
    return this == &other ||
            this->m_name == other.m_name ||
            (this->m_serial == other.m_serial &&
             this->m_model == other.m_model);
}


const QString& Disk::serial() const
{
    return m_serial;
}


const QString& Disk::model() const
{
    return m_model;
}


const QString& Disk::name() const {
    return m_name;
}


QString Disk::devPath() const {
    return QString("/dev/") + m_name;
}


QString Disk::toString() const
{
    return QString("%1, %2, %3, %4 [GiB]\n")
            .arg(m_name).arg(m_model).arg(m_serial)
            .arg(m_logical_block_size * m_size / 1024.0 / 1024.0 / 1024.0);
}
