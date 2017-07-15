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
    Disk(Disk&&) = default;

    bool operator==(const Disk&) const;

    const QString& serial() const;
    const QString& model() const;
    const QString& name() const;
    QString devPath() const;
    QString toString() const;
};

#endif // DISK_HPP
