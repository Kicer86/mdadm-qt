
#ifndef PRINTERS_FOR_GMOCK_HPP
#define PRINTERS_FOR_GMOCK_HPP

#include <deque>
#include <iostream>

class QModelIndex;
class QStringList;
class QString;

class RaidInfo;

void PrintTo(const QStringList& string_list, std::ostream* os);
void PrintTo(const std::deque<QString>& string_deque, std::ostream* os);
void PrintTo(const QString& str, std::ostream* os);
void PrintTo(const RaidInfo& info, std::ostream* os);
void PrintTo(const QModelIndex &, std::ostream* os);

#endif
