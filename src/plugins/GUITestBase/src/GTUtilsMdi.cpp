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

#include "GTUtilsMdi.h"
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>

#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>

#include "GTGlobals.h"
#include "primitives/GTMenu.h"
#include "utils/GTThread.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMdi"

#define GT_METHOD_NAME "click"
void GTUtilsMdi::click(HI::GUITestOpStatus& os, GTGlobals::WindowAction action) {
    if (action == GTGlobals::Close) {
        GTMenu::clickMainMenuItem(os, {"Window", "Close active view"});
        return;
    }
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != nullptr, "MainWindow == NULL");
    if (isOsMac()) {
        if (action == GTGlobals::Maximize) {
            MWMDIWindow* mdiWindow = mw->getMDIManager()->getActiveWindow();
            GT_CHECK(mdiWindow != nullptr, "MDIWindow == NULL");
            GTWidget::showMaximized(os, mdiWindow);
        } else {
            GT_FAIL("Unsupported action", );
        }
    } else {
        QMainWindow* mainWindow = mw->getQMainWindow();
        GT_CHECK(mainWindow != nullptr, "QMainWindow == NULL");
        GTMenuBar::clickCornerMenu(os, mainWindow->menuBar(), action);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findWindow"
QWidget* GTUtilsMdi::findWindow(HI::GUITestOpStatus& os, const QString& windowName, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(!windowName.isEmpty(), "windowname is empty", nullptr);

    MainWindow* mainWindow = AppContext::getMainWindow();
    GT_CHECK_RESULT(mainWindow != nullptr, "MainWindow == nullptr", nullptr);

    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);

        QList<MWMDIWindow*> mdiWindows = mainWindow->getMDIManager()->getWindows();
        foreach (MWMDIWindow* window, mdiWindows) {
            QString mdiTitle = window->windowTitle();
            switch (options.matchPolicy) {
                case Qt::MatchExactly:
                    if (mdiTitle == windowName) {
                        GTThread::waitForMainThread();
                        return window;
                    }
                    break;
                case Qt::MatchContains:
                    if (mdiTitle.contains(windowName, Qt::CaseInsensitive)) {
                        GTThread::waitForMainThread();
                        return window;
                    }
                    break;
                default:
                    GT_CHECK_RESULT(false, "Not implemented", nullptr);
            }
        }
        if (!options.failIfNotFound) {
            break;
        }
    }

    GT_CHECK_RESULT(!options.failIfNotFound, "Widget " + windowName + " not found", nullptr);
    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeActiveWindow"
void GTUtilsMdi::closeActiveWindow(GUITestOpStatus& os) {
    closeWindow(os, activeWindowTitle(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeWindow"
void GTUtilsMdi::closeWindow(HI::GUITestOpStatus& os, const QString& windowName, const GTGlobals::FindOptions& options) {
    GT_CHECK(windowName.isEmpty() == false, "windowname is empty");

    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != nullptr, "MainWindow == NULL");

    auto window = qobject_cast<MWMDIWindow*>(findWindow(os, windowName, options));
    GT_CHECK(window != nullptr, "Cannot find MDI window");
    GTWidget::close(os, window->parentWidget());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeAllWindows"
void GTUtilsMdi::closeAllWindows(HI::GUITestOpStatus& os) {
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QList<QMdiSubWindow*> mdiWindows = AppContext::getMainWindow()->getQMainWindow()->findChildren<QMdiSubWindow*>();
            for (QMdiSubWindow* mdiWindow : qAsConst(mdiWindows)) {
                auto filler = new MessageBoxDialogFiller(os, QMessageBox::Discard);
                GTUtilsDialog::waitForDialog(os, filler);
                mdiWindow->close();
                GTGlobals::sleep(100);
                GTUtilsDialog::removeRunnable(filler);
            }
        }
    };

    GTThread::runInMainThread(os, new Scenario());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activeWindow"
QWidget* GTUtilsMdi::activeWindow(HI::GUITestOpStatus& os, const GTGlobals::FindOptions& options) {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK_RESULT(mw != nullptr, "MainWindow == NULL", nullptr);

    QWidget* w = mw->getMDIManager()->getActiveWindow();
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(w != nullptr, "Active window is not found", nullptr);
    }
    return w;
}
#undef GT_METHOD_NAME

static QString getActiveMdiWindowTitle() {
    MainWindow* mainWindow = AppContext::getMainWindow();
    QWidget* mdiWindow = mainWindow == nullptr ? nullptr : mainWindow->getMDIManager()->getActiveWindow();
    return mdiWindow == nullptr ? "<no active window>" : mdiWindow->windowTitle();
}

#define GT_METHOD_NAME "getActiveObjectViewWindow"
QWidget* GTUtilsMdi::getActiveObjectViewWindow(GUITestOpStatus& os, const QString& viewId) {
    GObjectViewWindow* viewWindow = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && viewWindow == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        MainWindow* mainWindow = AppContext::getMainWindow();
        QWidget* mdiWindow = mainWindow == nullptr ? nullptr : mainWindow->getMDIManager()->getActiveWindow();
        if (mdiWindow == nullptr) {
            continue;
        }
        auto activeViewWindow = qobject_cast<GObjectViewWindow*>(mdiWindow);
        if (activeViewWindow != nullptr && activeViewWindow->getViewFactoryId() == viewId) {
            viewWindow = activeViewWindow;
        }
    }
    GT_CHECK_RESULT(viewWindow != nullptr, "View window is not found: " + viewId + ", active window: " + getActiveMdiWindowTitle(), nullptr);
    return viewWindow;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoObjectViewWindowIsOpened"
void GTUtilsMdi::checkNoObjectViewWindowIsOpened(GUITestOpStatus& os, const QString& viewId) {
    QList<QWidget*> allWindows = getAllObjectViewWindows(viewId);
    for (int time = 0; time < GT_OP_WAIT_MILLIS && !allWindows.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        allWindows = getAllObjectViewWindows(viewId);
    }
    GT_CHECK(allWindows.isEmpty(), "Found object view windows: " + viewId + ", when expected no window to be present");
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAllObjectViewWindows"
QList<QWidget*> GTUtilsMdi::getAllObjectViewWindows(const QString& viewId) {
    MainWindow* mainWindow = AppContext::getMainWindow();
    QList<QWidget*> result;
    if (mainWindow != nullptr) {
        foreach (QWidget* window, mainWindow->getMDIManager()->getWindows()) {
            auto objectViewWindow = qobject_cast<GObjectViewWindow*>(window);
            if (objectViewWindow != nullptr && objectViewWindow->getViewFactoryId() == viewId) {
                result << objectViewWindow;
            }
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activeWindowTitle"
QString GTUtilsMdi::activeWindowTitle(HI::GUITestOpStatus& os) {
    QWidget* w = activeWindow(os);
    auto mdi = qobject_cast<MWMDIWindow*>(w);
    GT_CHECK_RESULT(mdi, "unexpected object type", QString());
    return mdi->windowTitle();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activateAppWindow"
void GTUtilsMdi::activateWindow(HI::GUITestOpStatus& os, const QString& windowTitlePart) {
    MainWindow* mainWindow = AppContext::getMainWindow();

    GT_CHECK(mainWindow != nullptr, "MainWindow == nullptr");
    CHECK(!activeWindowTitle(os).contains(windowTitlePart, Qt::CaseInsensitive), );

    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchContains;
    QWidget* window = findWindow(os, windowTitlePart, options);

    GTMenu::clickMainMenuItem(os, {"Window", window->windowTitle()}, GTGlobals::UseMouse, Qt::MatchContains);
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkWindowIsActive"
QWidget* GTUtilsMdi::checkWindowIsActive(HI::GUITestOpStatus& os, const QString& windowTitlePart) {
    GT_CHECK_RESULT(!windowTitlePart.isEmpty(), "windowTitlePart is empty", nullptr);

    MainWindow* mainWindow = AppContext::getMainWindow();
    GT_CHECK_RESULT(mainWindow != nullptr, "MainWindow == nullptr", nullptr);

    QWidget* window = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && window == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QWidget* activeWindow = mainWindow->getMDIManager()->getActiveWindow();
        if (activeWindow->windowTitle().contains(windowTitlePart, Qt::CaseInsensitive)) {
            window = activeWindow;
        }
    }
    GT_CHECK_RESULT(window != nullptr, "Window with title part '" + windowTitlePart + "' is not found", nullptr);
    GTThread::waitForMainThread();
    return window;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMdiItemPosition"
QPoint GTUtilsMdi::getMdiItemPosition(HI::GUITestOpStatus& os, const QString& windowName) {
    QWidget* w = findWindow(os, windowName);
    GT_CHECK_RESULT(w != nullptr, "MDI window not found", QPoint());
    const QRect r = w->rect();
    return w->mapToGlobal(r.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRandomRegion"
void GTUtilsMdi::selectRandomRegion(HI::GUITestOpStatus& os, const QString& windowName) {
    QWidget* w = findWindow(os, windowName);
    GT_CHECK(w != nullptr, "MDI window not found");
    const QRect r = w->rect();
    QPoint p = QPoint((r.topLeft().x() + r.bottomLeft().x()) / 2 + 5, r.center().y() / 2);
    GTMouseDriver::moveTo(w->mapToGlobal(p));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(w->mapToGlobal(r.center()));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

namespace {

bool isWidgetPartVisible(QWidget* widget) {
    CHECK(widget != nullptr, false);

    if (!widget->visibleRegion().isEmpty()) {
        return true;
    }

    foreach (QObject* child, widget->children()) {
        if (child->isWidgetType() && isWidgetPartVisible(qobject_cast<QWidget*>(child))) {
            return true;
        }
    }

    return false;
}

}  // namespace

#define GT_METHOD_NAME "isAnyPartOfWindowVisible"
bool GTUtilsMdi::isAnyPartOfWindowVisible(HI::GUITestOpStatus& os, const QString& windowName) {
    QWidget* window = findWindow(os, windowName, {false});
    return isWidgetPartVisible(window);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabBar"
QTabBar* GTUtilsMdi::getTabBar(HI::GUITestOpStatus& os) {
    MainWindow* mainWindow = AppContext::getMainWindow();
    GT_CHECK_RESULT(mainWindow != nullptr, "MainWindow == nullptr", nullptr);

    auto mdiArea = GTWidget::findMdiArea(os, "MDI_Area", mainWindow->getQMainWindow());

    QTabBar* tabBar = mdiArea->findChild<QTabBar*>("", Qt::FindDirectChildrenOnly);
    GT_CHECK_RESULT(tabBar != nullptr, "MDI tabbar not found", nullptr);

    return tabBar;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabBar"
int GTUtilsMdi::getCurrentTab(HI::GUITestOpStatus& os) {
    QTabBar* tabBar = getTabBar(os);
    GT_CHECK_RESULT(tabBar != nullptr, "tabBar == NULL", -1);

    return tabBar->currentIndex();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTUtilsMdi::clickTab(HI::GUITestOpStatus& os, int tabIndex) {
    QTabBar* tabBar = getTabBar(os);
    QPoint tabCenter = tabBar->mapToGlobal(tabBar->tabRect(tabIndex).center());
    GTMouseDriver::moveTo(tabCenter);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
