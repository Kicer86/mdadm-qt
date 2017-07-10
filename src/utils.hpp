#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QFile>
#include <QTextStream>

namespace utils {

template <typename T>
T readValueFromFile(const QString& path)
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

} // namespace utils

#endif // UTILS_HPP
