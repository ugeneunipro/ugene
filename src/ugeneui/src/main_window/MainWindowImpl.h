/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MAINWINDOW_IMPL_
#define _U2_MAINWINDOW_IMPL_

#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>

#include <U2Gui/MainWindow.h>
#include <U2Gui/Notification.h>

class QMdiArea;
class QToolBar;

#if defined(Q_OS_DARWIN) && !defined(_DEBUG)
#    define _INSTALL_TO_PATH_ACTION
// TODO: Temporary disable menu item which creates link to app on desktop
//       Need to fix for Catalina/Big Sur
#    undef _INSTALL_TO_PATH_ACTION
#endif

namespace U2 {

class MWDockManagerImpl;
class MWMenuManagerImpl;
class MWToolBarManagerImpl;
class TmpDirChecker;

// workaround for QMdiArea issues
class FixedMdiArea : public QMdiArea {
    Q_OBJECT
public:
    FixedMdiArea(QWidget* parent = 0);
    void setViewMode(QMdiArea::ViewMode mode);
    QMdiSubWindow* addSubWindow(QWidget* widget);

public slots:
    void tileSubWindows();

private slots:
    // Workaround for QTBUG-17428
    void sysContextMenuAction(QAction*);
};

class MainWindowImpl : public MainWindow {
    Q_OBJECT
public:
    MainWindowImpl() = default;
    ~MainWindowImpl();

    virtual QMenu* getTopLevelMenu(const QString& sysName) const;
    virtual QToolBar* getToolbar(const QString& sysName) const;

    virtual MWMDIManager* getMDIManager() const {
        return mdiManager;
    }
    virtual MWDockManager* getDockManager() const {
        return dockManager;
    }
    virtual QMainWindow* getQMainWindow() const {
        return mw;
    }
    virtual NotificationStack* getNotificationStack() const {
        return nStack;
    }

    virtual void setWindowTitle(const QString& title);
    void registerAction(QAction* action);

    void prepare();
    void close();

    void runClosingTask();
    void setShutDownInProcess(bool flag);
    void registerStartupChecks(const QList<Task*>& tasks);
    void addNotification(const QString& message, NotificationType type);
signals:
    void si_show();
    void si_showWelcomePage();
    void si_paste();
public slots:
    void sl_tempDirPathCheckFailed(QString path);
    void sl_show();

private slots:
    void sl_exitAction();
    void sl_aboutAction();
    void sl_checkUpdatesAction();
    void sl_createDesktopShortcutAction();
    void sl_visitWeb();
    void sl_viewOnlineDocumentation();
    void sl_showWhatsNew();
    void sl_crashUgene();
#ifdef _INSTALL_TO_PATH_ACTION
    void sl_installToPathAction();
#endif
protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    void createActions();
    void prepareGUI();

    QMainWindow* mw = nullptr;
    FixedMdiArea* mdi = nullptr;

    MWMenuManagerImpl* menuManager = nullptr;
    MWToolBarManagerImpl* toolbarManager = nullptr;
    MWMDIManager* mdiManager = nullptr;
    MWDockManager* dockManager = nullptr;

    NotificationStack* nStack = nullptr;

    QAction* exitAction = nullptr;
    QAction* aboutAction = nullptr;
    QAction* checkUpdateAction = nullptr;
    QAction* createDesktopShortcutAction = nullptr;
    QAction* visitWebAction = nullptr;
    QAction* viewOnlineDocumentation = nullptr;
    QAction* welcomePageAction = nullptr;
    QAction* crashUgeneAction = nullptr;
    QAction* showWhatsNewAction = nullptr;
#ifdef _INSTALL_TO_PATH_ACTION
    QAction* installToPathAction = nullptr;
#endif
    bool shutDownInProcess = false;

    QList<Task*> startupTasklist;
};

class MainWindowDragNDrop {
public:
    static void dragEnterEvent(QDragEnterEvent* event);
    static void dropEvent(QDropEvent* event);
    static void dragMoveEvent(QDragMoveEvent* event);
};

}  // namespace U2

#endif
