/*
 * Interface for MDAdm process
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

#ifndef IMDADMPROCESS_HPP
#define IMDADMPROCESS_HPP

#include <functional>

#include <QByteArray>
#include <QStringList>

struct IMDAdmProcess
{
    typedef std::function<void(const QByteArray &, bool, int)> ExecutionResult;    // output, normal exit?, exit code
    typedef std::function<QString(const QByteArray &)> ReadChannelParser;

    virtual ~IMDAdmProcess() = default;

    /* execute mdadm with given parameters and returns result
     * through ExecutionResult. The ReadChannelParser is used
     * for interaction with user
     */
    virtual bool execute(const QStringList &, const ExecutionResult &,
                         const ReadChannelParser& parser = nullptr) = 0;
};

#endif // IMDADMPROCESS_HPP
