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

#include "filesystem.hpp"
#include "mdadm_controller.hpp"
#include "mdadm_debug_layer.hpp"
#include "mdadm_process.hpp"
#include "raids_model.hpp"

class QTabWidget;
class QTableView;
class QTreeView;
class QSettings;

class MainWindow: public QMainWindow
{
    public:
        MainWindow();
        MainWindow(const MainWindow &) = delete;
        ~MainWindow();

        MainWindow& operator=(const MainWindow &) = delete;
        bool operator==(const MainWindow &) const = delete;

    private:
        FileSystem m_fileSystem;
        MDAdmProcess m_mdadmProcess;
        MDAdmDebugLayer m_mdadmDebug;
        MDAdmController m_mdadmController;
        RaidsModel m_raidsModel;
        QStandardItemModel m_disksModel;
        QTabWidget* m_viewTabs;
        QTreeView* m_raidsView;
        QTableView* m_disksView;

        void contextMenu(const QPoint&);
        bool removeRaid(const QString&);
        void refreshArraysList();
        void refreshDisksList();
        void createRaid();

        bool confirmDialog(const QString& title, const QString& message,
                           const QString& confirmWord);

        QString getSettingsLocation();
        void loadSettings();
        void saveSettings();
};

#endif // MAINWINDOW_HPP
