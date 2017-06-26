#ifndef DISK_HPP
#define DISK_HPP

#include <QString>

class Disk
{
private:
    QString m_name;
    const QString m_model;
    const QString m_serial;
    const unsigned m_logical_block_size;
    const size_t m_size;

    size_t getSize();
    unsigned getLogicalBlockSize();

    Disk(const Disk&) = delete;
    Disk& operator=(const Disk&) = delete;
public:
    Disk(const QString& name);
    Disk(Disk&&);

    bool operator==(const Disk&) const;

    size_t size() const
    {
        return m_logical_block_size * m_size;
    }

    const QString& serial() const
    {
        return m_serial;
    }

    const QString& model() const
    {
        return m_model;
    }

    QString toString() const;
};

#endif // DISK_HPP
