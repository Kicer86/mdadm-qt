
#include "disk.hpp"
#include "utils.hpp"
#include "idevice_visitor.hpp"

Disk::Disk():
    BlockDevice(),
    m_name(""),
    m_model(""),
    m_serial("")
{
}


Disk::Disk(const QString& name, IFileSystem* filesystem) :
    BlockDevice(name, filesystem),
    m_name(name),
    m_model("N/A"),
    m_serial("N/A")
{
}


bool Disk::operator==(const Disk& other) const
{
    return this->m_name == other.m_name &&
           this->m_serial == other.m_serial &&
           this->m_model == other.m_model;
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


void Disk::accept(IDeviceVisitor* visitor)
{
    visitor->visit(this);
}


