/*
 * Utility for watching pollable files from /proc/
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef PROCWATCHER_HPP
#define PROCWATCHER_HPP

#include <QObject>

class ProcWatcher final: public QObject
{
        Q_OBJECT

    public:
        ProcWatcher(const char* path, QObject * = nullptr);
        ~ProcWatcher();

        void watch();           // blocking
        void stop_watching();   // return from watch()

    signals:
        void changed();
        void watching();

    private:
        int m_fd;
        int m_pipefd[2];
        bool m_listen;
};

#endif // PROCWATCHER_HPP
