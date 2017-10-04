/*
 * Utilites.
 * Copyright (C) 2017  Arkadiusz Bubała <arek2407066@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

    const T value = file_stream == nullptr ?
                T{} : readValueFromFile<T>(*file_stream);

    return value;
}

template <typename T>
void writeValueToFile(QTextStream& file_stream, const T& value)
{
    file_stream << value << endl; // FIXME consider passing value with \n
}

template <typename T>
bool writeValueToFile(IFileSystem* fs, const QString& path, const T& value)
{
    auto file = fs->openFile(path, QIODevice::WriteOnly);

    QTextStream* file_stream = file->getStream();

    if (file_stream != nullptr)
        writeValueToFile(*file_stream, value);

    return file_stream != nullptr;
}

} // namespace utils

#endif // UTILS_HPP
