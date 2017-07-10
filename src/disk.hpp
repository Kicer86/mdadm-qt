#ifndef DISK_HPP
#define DISK_HPP

#include "block_device.hpp"

#include <QString>

class Disk : public BlockDevice
{
private:
    const QString m_name;
    const QString m_model;
    const QString m_serial;

    Disk(const Disk&) = delete;
    Disk& operator=(const Disk&) = delete;
public:
    Disk(const QString&);
    Disk(Disk&&);

    bool operator==(const Disk&) const;

    const QString& serial() const
    {
        return m_serial;
    }

    const QString& model() const
    {
        return m_model;
    }

    const QString& name() const {
        return m_name;
    }

    QString devPath() const {
        return QString("/dev/") + m_name;
    }

    QString toString() const;
};

#endif // DISK_HPP
