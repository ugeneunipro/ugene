/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "primitives/GTMainWindow.h"

#include <QApplication>
#include <QWindow>

#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTMainWindow"

#define GT_METHOD_NAME "getMainWindows"
QList<QMainWindow*> GTMainWindow::getMainWindows(GUITestOpStatus& os) {
    QList<QMainWindow*> list;
    foreach (QWindow* window, qApp->topLevelWindows()) {
        if (window->inherits("QMainWindow")) {
            list.append(qobject_cast<QMainWindow*>(window));
        }
    }

    GT_CHECK_RESULT(!list.isEmpty(), "No one main window found", list);
    return list;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMainWindowsAsWidget"
QList<QWidget*> GTMainWindow::getMainWindowsAsWidget(GUITestOpStatus& os) {
    QList<QWidget*> mainWindows;
    QList<QWidget*> topLevelWidgets = qApp->topLevelWidgets();
    for (QWidget* widget : qAsConst(topLevelWidgets)) {
        if (widget->inherits("QMainWindow")) {
            mainWindows.append(widget);
        }
    }
    // TODO: remove this hack.
    if (mainWindows.isEmpty()) {
        mainWindows = qApp->topLevelWidgets();
    }
    GT_CHECK_RESULT(!mainWindows.isEmpty(), "No main window widget found", mainWindows);
    return mainWindows;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMainWindowWidgetByName"
QWidget* GTMainWindow::getMainWindowWidgetByName(GUITestOpStatus& os, const QString& name) {
    for (QWidget* w : GTMainWindow::getMainWindowsAsWidget(os)) {
        if (w->objectName() == name) {
            return w;
        }
    }
    GT_FAIL(QString("There is no window named '%1'").arg(name), nullptr)
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTitle"
void GTMainWindow::checkTitle(GUITestOpStatus& os, const QString& title) {
    QList<QWidget*> mainWindowWidgets = getMainWindowsAsWidget(os);
    for (const QWidget* widget : qAsConst(mainWindowWidgets)) {
        if (widget->windowTitle() == title) {
            return;
        }
    }
    GT_FAIL(QString("Can't find window with the title: '%1'").arg(title), );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "maximizeMainWindow"
void GTMainWindow::maximizeMainWindow(GUITestOpStatus& os, QMainWindow* mainWindow) {
    class MaximizeMainWindowScenario : public CustomScenario {
    public:
        MaximizeMainWindowScenario(QMainWindow* mw)
            : mainWindow(mw) {
        }
        void run(HI::GUITestOpStatus&) override {
            mainWindow->setWindowState(mainWindow->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
        }
        QMainWindow* mainWindow;
    };
    // Minimize main window.
    GTThread::runInMainThread(os, new MaximizeMainWindowScenario(mainWindow));
    // Wait for OS to complete the op.
    GTGlobals::sleep(1500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "minimizeMainWindow"
void GTMainWindow::minimizeMainWindow(GUITestOpStatus& os, QMainWindow* mainWindow) {
    class MinimizeMainWindowScenario : public CustomScenario {
    public:
        MinimizeMainWindowScenario(QMainWindow* mw)
            : mainWindow(mw) {
        }
        void run(HI::GUITestOpStatus&) override {
            mainWindow->setWindowState(mainWindow->windowState() | Qt::WindowMinimized);
        }
        QMainWindow* mainWindow;
    };
    // Minimize main window.
    GTThread::runInMainThread(os, new MinimizeMainWindowScenario(mainWindow));
    // Wait for OS to complete the op.
    GTGlobals::sleep(1500);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
