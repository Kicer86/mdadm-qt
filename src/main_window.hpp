/*
 * Main window class
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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStandardItemModel>

#include "mdadm_controller.hpp"
#include "mdadm_process.hpp"

class QTableView;

class MainWindow: public QMainWindow
{
    public:
        MainWindow();
        MainWindow(const MainWindow &) = delete;
        ~MainWindow();

        MainWindow& operator=(const MainWindow &) = delete;
        bool operator==(const MainWindow &) const = delete;

    private:
        MDAdmProcess m_mdadmProcess;
        MDAdmController m_mdadmController;
        QTableView* m_raidsView;
        QStandardItemModel m_raidsModel;

        void refreshArraysList();
};

#endif // MAINWINDOW_HPP
