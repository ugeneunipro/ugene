/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>

#include <U2Gui/MainWindow.h>
#include <U2Gui/GUIUtils.h>

class QMdiArea;
class QToolBar;

#if defined(Q_OS_DARWIN) && !defined(_DEBUG)
#    define _INSTALL_TO_PATH_ACTION
#endif

namespace U2 {

class MWDockManagerImpl;
class MWMenuManagerImpl;
class MWToolBarManagerImpl;
class TmpDirChecker;

class MainWindowImpl : public MainWindow {
    Q_OBJECT
public:
    MainWindowImpl();
    ~MainWindowImpl();

    virtual QMenu* getTopLevelMenu(const QString& sysName) const;
    virtual QToolBar* getToolbar(const QString& sysName) const;

    virtual MWMDIManager* getMDIManager() const {
        return nullptr;
    }
    virtual MWDockManager* getDockManager() const {
        return nullptr;
    }
    virtual QMainWindow* getQMainWindow() const {
        return mw;
    }
    virtual NotificationStack* getNotificationStack() const {
        return nullptr;
    }

    virtual void setWindowTitle(const QString& title);
    void registerAction(QAction* action) override;

    // Dark theme is enabled if true
    bool isDarkTheme() const override;

    // Set style and color theme type. Possible styles:
    // Windows, Fusion, windowsvista (Windows only), macintosh (macOS only)/
    // Possible color theme types:
    // Luight, Dark, Auto (follow the system style)
    void setNewStyle(const QString& style, int colorThemeIndex) override;

    void runClosingTask();
    void setShutDownInProcess(bool flag);

    void addNotification(const QString& message, NotificationType type) override;

signals:
    void si_show();
    void si_showWelcomePage();
    void si_paste();

private:

    // Fake main window
    // corelibs need it, but it's difficult to have here a real QMainWindow
    QMainWindow* mw = nullptr;

    bool shutDownInProcess = false;
    QList<Task*> startupTasklist;
};

}  // namespace U2

