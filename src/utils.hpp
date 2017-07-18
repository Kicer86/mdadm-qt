#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>
#include <QTextStream>

#include "ifilesystem.hpp"

namespace utils {

template <typename T>
T readValueFromFile(QTextStream& file_stream)
{
    T value {};

    file_stream >> value;

    return value;
}

template <typename T>
T readValueFromFile(IFileSystem* fs, const QString& path)
{
    auto file = fs->openFile(path);

    QTextStream* file_stream = file->getStream();

    const T value = file_stream == nullptr? T{} : readValueFromFile<T>(*file_stream);

    return value;
}

} // namespace utils

#endif // UTILS_HPP
