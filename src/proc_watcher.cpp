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


ProcWatcher::ProcWatcher():
    m_thread(),
    m_fds(),
    m_pipefd{-1, -1}
{
    pipe(m_pipefd);

    connect(&m_thread, &QThread::started,
            this, &ProcWatcher::start_watching,
            Qt::DirectConnection);

    m_thread.start();
}


ProcWatcher::~ProcWatcher()
{
    stop_watching();
    m_thread.quit();
    m_thread.wait();

    for(const auto& i: m_fds)
        close(i.first);

    if (m_pipefd[0] != -1)
        close(m_pipefd[0]);

    if (m_pipefd[1] != -1)
        close(m_pipefd[1]);
}


void ProcWatcher::watch(const char* path)
{
    const int fd = open(path, O_RDONLY);
    m_fds[fd] = path;

    const char data = 'R';
    write(m_pipefd[1], &data, sizeof(data));
}


void ProcWatcher::start_watching()
{
    while (true)
    {
        std::vector<pollfd> pfds;
        pfds.reserve(m_fds.size() + 1);           // all watched files + pipe

        const pollfd pipe_pfd = { m_pipefd[0], POLLIN, 0 };
        pfds.push_back(pipe_pfd);   // communication pipe

        for (const auto& i: m_fds)
        {
            const pollfd file_pfd = { i.first, POLLPRI, 0 };
            pfds.push_back(file_pfd); // watched files
        }

        const int status = poll(pfds.data(), pfds.size(), -1);
        assert(status);

        // check if pipe
        if (pfds[0].revents & POLLIN)
        {
            char d;
            read(pfds[0].fd, &d, sizeof(d));

            if (d == 'Q')
                break;               // finish the loop
            else if (d == 'R')
                continue;            // refresh files to be polled
            else
                assert(!"unknown command");
        };

        // check files
        for (int i = 1; i < pfds.size(); i++)
        {
            const pollfd& pfd = pfds[i];

            if (pfd.revents & POLLPRI)
            {
                const auto it = m_fds.find(pfd.fd);
                assert(it != m_fds.end());

                emit changed(it->second);

                // Here we read file to clear POLLPRI flag. TODO: Not nice.
                bool c = true;
                lseek(pfd.fd, 0, SEEK_SET);

                while(c)
                {
                    int buf;
                    const int g = read(pfd.fd, &buf, sizeof(buf));

                    c = g == sizeof(buf);
                }
            }
        }
    }
}


void ProcWatcher::stop_watching()
{
    const char data = 'Q';
    write(m_pipefd[1], &data, sizeof(data));
}
