
#include "printers_for_gmock.hpp"

#include <QDebug>
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
    debug << info.block_devices;

    *os << output.toStdString();
}
