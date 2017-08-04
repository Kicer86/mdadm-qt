/*
 * Wrapper over mdadm process
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

#ifndef MDADMPROCESS_HPP
#define MDADMPROCESS_HPP

#include "imdadm_process.hpp"

class MDAdmProcess: public IMDAdmProcess
{
    public:
        MDAdmProcess();
        MDAdmProcess(const MDAdmProcess &) = delete;
        virtual ~MDAdmProcess();

        MDAdmProcess& operator=(const MDAdmProcess &) = delete;
        bool operator==(const MDAdmProcess &) const = delete;

        // overrides:
        bool execute(const QStringList &, const ExecutionResult &,
                     const ReadChannelParser& parser = nullptr) override;
};

#endif // MDADMPROCESS_HPP
