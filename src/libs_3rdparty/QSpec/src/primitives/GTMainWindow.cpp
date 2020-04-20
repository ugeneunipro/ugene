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

namespace HI {

#define GT_CLASS_NAME "GTMainWindow"

#define GT_METHOD_NAME "getMainWindows"
QList<QMainWindow*> GTMainWindow::getMainWindows(GUITestOpStatus &os) {
    QList<QMainWindow*> list;
    foreach(QWindow *window, qApp->topLevelWindows()){
        if(window->inherits("QMainWindow")){
            list.append(qobject_cast<QMainWindow*>(window));
        }
    }

    GT_CHECK_RESULT(!list.isEmpty(),"No one main window found", list);
    return list;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMainWindowsAsWidget"
QList<QWidget*> GTMainWindow::getMainWindowsAsWidget(GUITestOpStatus &os) {
    QList<QWidget*> list;
    foreach(QWidget *widget, qApp->topLevelWidgets()){
        if(widget->inherits("QMainWindow")){
            list.append(widget);
        }
    }
    if(list.isEmpty()){
        list = qApp->topLevelWidgets();
    }
    GT_CHECK_RESULT(!list.isEmpty(),"No one main window widget found", list);
    return list;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
