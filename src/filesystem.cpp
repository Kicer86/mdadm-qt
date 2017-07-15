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


#include "filesystem.hpp"

#include <QString>

namespace
{
    struct File: IFileSystem::IFile
    {
        QTextStream* getStream() override
        {
            return nullptr;
        }
    };
}


FileSystem::FileSystem()
{

}


FileSystem::~FileSystem()
{

}


std::unique_ptr<IFileSystem::IFile> FileSystem::openFile(const QString&)
{
    std::unique_ptr<File> file;

    return file;
}


std::deque<QString> FileSystem::listDir(const QString&, const char* filter)
{
    std::deque<QString> list;

    return list;
}
