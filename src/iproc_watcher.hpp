
#ifndef IPROC_WATCHER_HPP
#define IPROC_WATCHER_HPP

#include <QObject>

struct IProcWatcher: public QObject
{
        Q_OBJECT

    public:
        virtual ~IProcWatcher() = default;

        virtual void watch(const char *) = 0;

    signals:
        void changed(const QString &);
};

#endif
