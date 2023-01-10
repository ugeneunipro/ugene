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

#ifndef _HI_GT_MAIN_WINDOW_H_
#define _HI_GT_MAIN_WINDOW_H_

#include <QMainWindow>
#include <QWidget>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTMainWindow {
public:
    static QList<QMainWindow*> getMainWindows(GUITestOpStatus& os);

    static QList<QWidget*> getMainWindowsAsWidget(GUITestOpStatus& os);

    /**
     * Returns the first matching QMainWindow named "name" as the widget. Names are compared case-sensitive and
     * locale-insensitive.
     */
    static QWidget* getMainWindowWidgetByName(GUITestOpStatus& os, const QString& name);

    /** Checks that there is a main window with the given title. */
    static void checkTitle(GUITestOpStatus& os, const QString& title);

    /** Minimizes main window: calls QMainWindow->showMinimized safely. */
    static void minimizeMainWindow(GUITestOpStatus& os, QMainWindow* mainWindow);

    /** Maximizes main window: calls QMainWindow->showMaximized safely. */
    static void maximizeMainWindow(GUITestOpStatus& os, QMainWindow* mainWindow);
};

}  // namespace HI

#endif
