
#ifndef IFILESYSTEM_HPP
#define IFILESYSTEM_HPP

#include <deque>
#include <memory>

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

    virtual std::unique_ptr<IFile> openFile(const QString &) = 0;
    virtual std::deque<QString> listDir(const QString &, const char* filter = "*.*") = 0;
};

#endif
