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

#include "proc_watcher.hpp"

#include <cassert>
#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>


ProcWatcher::ProcWatcher(const char* path, QObject* p):
    QObject(p),
    m_fd(-1),
    m_pipefd{-1, -1},
    m_listen(true)
{
    m_fd = open(path, O_RDONLY);
    pipe(m_pipefd);
}


ProcWatcher::~ProcWatcher()
{
    if (m_fd != -1)
        close(m_fd);

    if (m_pipefd[0] != -1)
        close(m_pipefd[0]);

    if (m_pipefd[1] != -1)
        close(m_pipefd[1]);
}


void ProcWatcher::watch()
{
    emit watching();

    struct pollfd pfds[2] = { {0, 0, 0}, {0, 0, 0} };

    pfds[0].fd = m_fd;
    pfds[0].events = POLLPRI;
    pfds[1].fd = m_pipefd[0];
    pfds[1].events = POLLIN;

    while (m_listen)
    {
        const int status = poll(pfds, 2, -1);
        assert(status);

        if (pfds[0].revents & POLLPRI)
        {
            emit changed();

            // Here we read file to clear POLLPRI flag. TODO: Not nice.
            bool c = true;
            lseek(m_fd, 0, SEEK_SET);

            while(c)
            {
                int buf;
                const int g = read(m_fd, &buf, sizeof(buf));

                c = g == sizeof(buf);
            }
        }

        if (pfds[1].revents & POLLIN)
            break;
    }
}


void ProcWatcher::stop_watching()
{
    m_listen = false;

    const int data = 0;
    write(m_pipefd[1], &data, sizeof(data));
}
