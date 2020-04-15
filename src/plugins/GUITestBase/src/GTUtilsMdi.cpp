/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>
#include <U2Gui/MainWindow.h>

#include "GTUtilsMdi.h"
#include "GTGlobals.h"
#include "primitives/GTMenu.h"
#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTThread.h"
#include <base_dialogs/MessageBoxFiller.h>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMdi"

#define GT_METHOD_NAME "click"
void GTUtilsMdi::click(HI::GUITestOpStatus &os, GTGlobals::WindowAction action) {

    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow == NULL");

    QMainWindow* mainWindow = mw->getQMainWindow();
    GT_CHECK(mainWindow != NULL, "QMainWindow == NULL");

    // TODO: batch tests run fails because of not maximized window by default from settings
//    if ((action == GTGlobals::Maximize) || (action == GTGlobals::Minimize)) {
//        return;
//    }

#ifndef Q_OS_MAC
    switch (action) {
    case GTGlobals::Close: {
#ifdef Q_OS_UNIX
        GTMenu::clickMainMenuItem(os, QStringList() << "Window" << "Close active view");
#else
        GTKeyboardDriver::keyPress(Qt::Key_Control);
        GTKeyboardDriver::keyClick(Qt::Key_F4);
        GTKeyboardDriver::keyRelease(Qt::Key_Control);
#endif
        break;
    }
    default:
        GTMenuBar::clickCornerMenu(os, mainWindow->menuBar(), action);
        break;
    }
#else
    MWMDIWindow *mdiWindow = mw->getMDIManager()->getActiveWindow();
    GT_CHECK(mdiWindow != NULL, "MDIWindow == NULL");

    // TODO: make click on button
    switch (action) {
    case GTGlobals::Maximize:
        GTWidget::showMaximized(os, mdiWindow);
        break;
    case GTGlobals::Close: {
        int left = mdiWindow->rect().left();
        int top = mdiWindow->rect().top();
        QPoint p(left + 15,top - 10);
        GTMouseDriver::moveTo(mdiWindow->mapToGlobal(p));
        GTMouseDriver::click();
        break;
    }
    default:
        assert(false);
        break;
    }
#endif
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findWindow"
QWidget * GTUtilsMdi::findWindow(HI::GUITestOpStatus &os, const QString &windowName, const GTGlobals::FindOptions &options) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(windowName.isEmpty() == false, "windowname is empty", NULL);

    MainWindow *mw = AppContext::getMainWindow();
    GT_CHECK_RESULT(mw != NULL, "MainWindow == NULL", NULL);

    QList<MWMDIWindow *> mdiWindows = mw->getMDIManager()->getWindows();
    foreach (MWMDIWindow *w, mdiWindows) {
        QString mdiTitle = w->windowTitle();
        switch (options.matchPolicy) {
        case Qt::MatchExactly:
            if (mdiTitle == windowName) {
                return w;
            }
            break;
        case Qt::MatchContains:
            if (mdiTitle.contains(windowName)) {
                return w;
            }
            break;
        default:
            GT_CHECK_RESULT(false, "Not implemented", NULL);
        }
    }

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(false, "Widget " + windowName + " not found", NULL);
    }

    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeActiveWindow"
void GTUtilsMdi::closeActiveWindow(GUITestOpStatus &os) {
    closeWindow(os, activeWindowTitle(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeWindow"
void GTUtilsMdi::closeWindow(HI::GUITestOpStatus &os, const QString &windowName, const GTGlobals::FindOptions& options) {
    GT_CHECK(windowName.isEmpty() == false, "windowname is empty");

    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow == NULL");

    MWMDIWindow* window = qobject_cast<MWMDIWindow*>(findWindow(os, windowName, options));
    GT_CHECK(window != NULL, "Cannot find MDI window");
    GTWidget::close(os, window->parentWidget());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeAllWindows"
void GTUtilsMdi::closeAllWindows(HI::GUITestOpStatus &os) {
#ifndef Q_OS_MAC
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            const QList<QMdiSubWindow *> mdiWindows = AppContext::getMainWindow()->getQMainWindow()->findChildren<QMdiSubWindow *>();
            foreach (QMdiSubWindow *mdiWindow, mdiWindows) {
                MessageBoxDialogFiller *filler = new MessageBoxDialogFiller(os, QMessageBox::Discard);
                GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, filler);
                mdiWindow->close();
                GTGlobals::sleep(100);
                GTUtilsDialog::removeRunnable(filler);
            }
        }
    };

    GTThread::runInMainThread(os, new Scenario);
#else
    // GUI on Mac hangs because of bug in QCocoaEventDispatcher
    // It looks like this issue: https://bugreports.qt.io/browse/QTBUG-45389
    // This part can be removed after Qt bug will be fixed
    // And now: some magic!

    QWidget *prevWindow = NULL;
    QWidget *mdiWindow = NULL;
    GTGlobals::FindOptions options(false);

    bool tabbedView = isTabbedLayout(os);

    while (NULL != (mdiWindow = GTUtilsMdi::activeWindow(os, options))) {
        GT_CHECK(prevWindow != mdiWindow, "Can't close MDI window");
        prevWindow = mdiWindow;

        MessageBoxDialogFiller *filler = new MessageBoxDialogFiller(os, QMessageBox::Discard);
        GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, filler);

        if (!tabbedView) {
            const QPoint closeButtonPos = GTWidget::getWidgetGlobalTopLeftPoint(os, mdiWindow) + QPoint(10, 5);
            GTMouseDriver::moveTo(closeButtonPos);
            GTMouseDriver::click();
        } else {
            GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Close active view");
        }
        GTGlobals::sleep(100);
        GTThread::waitForMainThread();
        GTUtilsDialog::removeRunnable(filler);
    }
#endif
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "waitWindowOpened"
void GTUtilsMdi::waitWindowOpened(HI::GUITestOpStatus &os, const QString &windowNamePart, qint64 timeout) {
    MainWindow *mainWindow = AppContext::getMainWindow();
    GT_CHECK(mainWindow != NULL, "MainWindow == NULL");
    MWMDIManager *mdiManager = mainWindow->getMDIManager();
    GT_CHECK(mdiManager != NULL, "MainWindow == NULL");

    bool found = false;
    int passedTime = 0;
    while (!found && passedTime < timeout / 1000) {
        foreach (MWMDIWindow *window, mdiManager->getWindows()) {
            found |= window->windowTitle().contains(windowNamePart, Qt::CaseInsensitive);
        }
        GTGlobals::sleep(1000);
        passedTime++;
    }

    if (!found) {
        os.setError(QString("Cannot find MDI window with part of name '%1', timeout").arg(windowNamePart));
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isTabbedLayout"
bool GTUtilsMdi::isTabbedLayout(HI::GUITestOpStatus &os) {
    MainWindow* mainWindow = AppContext::getMainWindow();
    GT_CHECK_RESULT(mainWindow != NULL, "MainWindow == NULL", NULL);
    QMdiArea *mdiArea = GTWidget::findExactWidget<QMdiArea *>(os, "MDI_Area", mainWindow->getQMainWindow());
    GT_CHECK_RESULT(mdiArea != NULL, "mdiArea == NULL", NULL);
    return mdiArea->viewMode() == QMdiArea::TabbedView;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activeWindow"
QWidget* GTUtilsMdi::activeWindow(HI::GUITestOpStatus &os, const GTGlobals::FindOptions& options) {

    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK_RESULT(mw != NULL, "MainWindow == NULL", NULL);

    QWidget* w = mw->getMDIManager()->getActiveWindow();
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(w != NULL, "Active window is not found", NULL);
    }
    return w;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activeWindowTitle"
QString GTUtilsMdi::activeWindowTitle(HI::GUITestOpStatus &os){
    QWidget* w = activeWindow(os);
    MWMDIWindow* mdi = qobject_cast<MWMDIWindow*>(w);
    GT_CHECK_RESULT(mdi, "unexpected object type", QString());
    return mdi->windowTitle();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activateWindow"
void GTUtilsMdi::activateWindow(HI::GUITestOpStatus &os, const QString &windowName){
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow == NULL");

    CHECK(!(activeWindowTitle(os) == windowName), );

    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    MWMDIWindow* window = qobject_cast<MWMDIWindow*>(findWindow(os, windowName, options));
    GT_CHECK(window != NULL, "window " + windowName + " not found");

    GTMenu::clickMainMenuItem(os, QStringList() << "Window" << windowName, GTGlobals::UseMouse, Qt::MatchContains);
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMdiItemPosition"
QPoint GTUtilsMdi::getMdiItemPosition(HI::GUITestOpStatus &os, const QString& windowName){
    QWidget* w = findWindow(os, windowName);
    GT_CHECK_RESULT(w != NULL, "MDI window not found", QPoint());
    const QRect r = w->rect();
    return w->mapToGlobal(r.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRandomRegion"
void GTUtilsMdi::selectRandomRegion(HI::GUITestOpStatus &os, const QString& windowName){
    QWidget* w = findWindow(os, windowName);
    GT_CHECK(w != NULL, "MDI window not found");
    const QRect r = w->rect();
    QPoint p = QPoint((r.topLeft().x() + r.bottomLeft().x())/2 + 5, r.center().y()/2);
    GTMouseDriver::moveTo(w->mapToGlobal(p));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(w->mapToGlobal(r.center()));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

namespace {

bool isWidgetPartVisible(QWidget *widget) {
    CHECK(NULL != widget, false);

    if (!widget->visibleRegion().isEmpty()) {
        return true;
    }

    foreach (QObject *child, widget->children()) {
        if (child->isWidgetType() && isWidgetPartVisible(qobject_cast<QWidget *>(child))) {
            return true;
        }
    }

    return false;
}

}

#define GT_METHOD_NAME "isAnyPartOfWindowVisible"
bool GTUtilsMdi::isAnyPartOfWindowVisible(HI::GUITestOpStatus &os, const QString &windowName) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *window = findWindow(os, windowName, options);
    CHECK(NULL != window, false);
    return isWidgetPartVisible(window);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabBar"
QTabBar* GTUtilsMdi::getTabBar(HI::GUITestOpStatus &os) {
    MainWindow* mainWindow = AppContext::getMainWindow();
    GT_CHECK_RESULT(mainWindow != nullptr, "MainWindow == nullptr", NULL);

    QMdiArea *mdiArea = GTWidget::findExactWidget<QMdiArea *>(os, "MDI_Area", mainWindow->getQMainWindow());
    GT_CHECK_RESULT(mdiArea != nullptr, "mdiArea == nullptr", NULL);

    QTabBar *tabBar = mdiArea->findChild<QTabBar *>("", Qt::FindDirectChildrenOnly);
    GT_CHECK_RESULT(tabBar != nullptr, "MDI tabbar not found", NULL);

    return tabBar;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabBar"
int GTUtilsMdi::getCurrentTab(HI::GUITestOpStatus &os) {
    QTabBar* tabBar = getTabBar(os);
    GT_CHECK_RESULT(tabBar != NULL, "tabBar == NULL", -1);

    return tabBar->currentIndex();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTUtilsMdi::clickTab(HI::GUITestOpStatus &os, int tabIndex) {
    QTabBar* tabBar = getTabBar(os);
    GT_CHECK_RESULT(tabBar != NULL, "tabBar == NULL", );

    coreLog.info(QString("Try to click tab %1(%2)").arg(tabIndex).arg(tabBar->tabText(tabIndex)));
    QPoint tabCenter = tabBar->mapToGlobal(tabBar->tabRect(tabIndex).center());
    GTMouseDriver::moveTo(tabCenter);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
