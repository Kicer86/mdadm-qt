/*
 * filesystem facade
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP


#include "ifilesystem.hpp"


class FileSystem: public IFileSystem
{
    public:
        FileSystem();
        FileSystem(const FileSystem &) = delete;
        ~FileSystem();
        FileSystem& operator=(const FileSystem &) = delete;
        bool operator==(const FileSystem &) const = delete;

        std::unique_ptr<IFile> openFile(const QString &, const QIODevice::OpenMode &) const override;
        bool isDeviceUsed(const QString &) override;
        std::deque<QString> listDir(const QString & , const char* filter, QDir::Filters) override;
};

#endif // FILESYSTEM_HPP
