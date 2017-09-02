/*
 * Debug layer for MDAdmProcess - wrapper dumping details about mdadm execution
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


#include "mdadm_debug_layer.hpp"

#include <QDebug>


MDAdmDebugLayer::MDAdmDebugLayer(IMDAdmProcess* to_wrap): m_wrapped(to_wrap)
{

}


MDAdmDebugLayer::~MDAdmDebugLayer()
{

}


bool MDAdmDebugLayer::execute(const QStringList& args, const IMDAdmProcess::ExecutionResult& result_callback, const IMDAdmProcess::ReadChannelParser& parser)
{
    auto result_dumper = [result_callback](const QByteArray& output, bool success, int exitCode)
    {
        if (success)
        {
            qDebug() << "mdadm exited normally with code: "
                     << exitCode << " and output:";
            qDebug() << output;
        }
        else
            qDebug() << "mdadm crashed";
        
        // pass result to original caller
        result_callback(output, success, exitCode);
    };    
    
    qDebug() << "executing mdadm with args:" << args;
    
    const bool result = m_wrapped->execute(args, result_dumper, parser);
    
    return result;
}
