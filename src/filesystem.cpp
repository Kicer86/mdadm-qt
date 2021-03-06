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

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDirIterator>
#include <QFile>
#include <QString>
#include <QTextStream>

namespace
{
    struct File: IFileSystem::IFile
    {
        File(const QString& path, const QIODevice::OpenMode& mode): m_stream(nullptr), m_file(path), m_mode(mode) {}

        QTextStream* getStream() override
        {
            if (m_file.isOpen() == false)
            {
                const bool status = m_file.open(m_mode);

                if (status)
                    m_stream = std::make_unique<QTextStream>(&m_file);
            }

            return m_stream.get();
        }

        std::unique_ptr<QTextStream> m_stream;
        QFile m_file;
        const QIODevice::OpenMode m_mode;
    };
}


FileSystem::FileSystem()
{

}


FileSystem::~FileSystem()
{

}


std::unique_ptr<IFileSystem::IFile> FileSystem::openFile(const QString& path, const QIODevice::OpenMode& mode) const
{
    std::unique_ptr<IFileSystem::IFile> file = std::make_unique<File>(path, mode);

    return file;
}


bool FileSystem::isDeviceUsed(const QString& dev_path)
{
    bool result = true;

    if (dev_path.left(5) == "/dev/")
    {
        const int fd = open(QFile::encodeName(dev_path), O_RDONLY | O_EXCL);
        if (fd > 0)
        {
            result = false;
            close(fd);
        }
    }

    return result;
}


std::deque<QString> FileSystem::listDir(const QString& dir, const char* filter, QDir::Filters type_filter)
{
    std::deque<QString> list;

    QDirIterator di(dir, QStringList(filter), type_filter);
    while (di.hasNext())
    {
        di.next();

        list.emplace_back(di.fileName());
    }

    return list;
}
