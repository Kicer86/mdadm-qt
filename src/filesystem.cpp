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

#include <QDirIterator>
#include <QFile>
#include <QString>
#include <QTextStream>

namespace
{
    struct File: IFileSystem::IFile
    {
        File(const QString& path): m_stream(nullptr), m_file(path) {}

        QTextStream* getStream() override
        {
            if (m_file.isOpen() == false)
            {
                const bool status = m_file.open(QFile::ReadOnly);      // TODO: flag for RW?

                if (status)
                    m_stream = std::make_unique<QTextStream>(&m_file);
            }

            return m_stream.get();
        }

        std::unique_ptr<QTextStream> m_stream;
        QFile m_file;
    };
}


FileSystem::FileSystem()
{

}


FileSystem::~FileSystem()
{

}


std::unique_ptr<IFileSystem::IFile> FileSystem::openFile(const QString& path)
{
    std::unique_ptr<File> file = std::make_unique<File>(path);

    return file;
}


std::deque<QString> FileSystem::listDir(const QString& dir, const char* filter)
{
    std::deque<QString> list;

    QDirIterator di(dir, QStringList(filter), QDir::Dirs | QDir::NoDotAndDotDot);
    while (di.hasNext())
    {
        di.next();

        list.emplace_back(di.fileName());
    }

    return list;
}
