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

#include <mutex>
#include <sys/poll.h>

#include <QThread>

#include "iproc_watcher.hpp"

class ProcWatcher final: public IProcWatcher
{
    public:
        ProcWatcher();
        ~ProcWatcher();

        void watch(const char* path) override;

        void start_watching();  // blocking
        void stop_watching();   // return from start_watching()

    private:
        QThread m_thread;
        mutable std::mutex m_fds_mutex;
        std::map<int, QString> m_fds;
        int m_pipefd[2];

        std::vector<pollfd> make_pollfds() const;
        void read_file(int) const;
};

#endif // PROCWATCHER_HPP
