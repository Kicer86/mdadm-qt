
#include "printers_for_gmock.hpp"

#include <QDebug>
#include <QModelIndex>
#include <QString>

#include "mdadm_controller.hpp"

void PrintTo(const QStringList& string_list, std::ostream* os)
{
    QString output;
    QDebug debug(&output);
    debug << string_list;

    *os << output.toStdString();
}

void PrintTo(const std::deque<QString>& string_deque, std::ostream* os)
{
    QString output;
    QDebug debug(&output);

    for (const auto& element : string_deque)
    {
        debug << element;
    }

    *os << output.toStdString();
}

void PrintTo(const QString& str, std::ostream* os)
{
    *os << str.toStdString();
}

void PrintTo(const RaidInfo& info, std::ostream* os)
{
    QString output;
    QDebug debug(&output);
    debug << info.raid_device;
    debug << info.raid_type;
    for (const auto& device : info.block_devices)
    {
        debug << device.name
              << "[" << device.descriptor_index << "]"
              << "(" << static_cast<int>(device.type) << ")";
    }


    *os << output.toStdString();
}

void PrintTo(const QModelIndex& idx, std::ostream* os)
{
    QString output;
    QDebug debug(&output);
    debug << idx;

    *os << output.toStdString();
}
