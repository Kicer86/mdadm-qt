
#ifndef IFILESYSTEM_HPP
#define IFILESYSTEM_HPP

#include <deque>
#include <memory>

#include <QDir>
#include <QIODevice>

class QTextStream;
class QString;

struct IFileSystem
{
    struct IFile
    {
        virtual ~IFile() = default;

        virtual QTextStream* getStream() = 0;
    };

    virtual ~IFileSystem() = default;

    virtual std::unique_ptr<IFile> openFile(const QString &, const QIODevice::OpenMode & = QIODevice::ReadOnly) = 0;
    virtual std::deque<QString> listDir(const QString &, const char* filter, QDir::Filters) = 0;
};

#endif
