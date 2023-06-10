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

#pragma once

#include <primitives/GTWidget.h>

#include <QPoint>
#include <QTabBar>

#include "GTGlobals.h"
#include "primitives/GTMenuBar.h"

namespace U2 {
class MWMDIWindow;
using namespace HI;
class GTUtilsMdi {
public:
    static void click(GTGlobals::WindowAction action);
    static QPoint getMdiItemPosition(const QString& windowName);
    static void selectRandomRegion(const QString& windowName);
    static bool isAnyPartOfWindowVisible(const QString& windowName);

    // fails if MainWindow is NULL or because of FindOptions settings
    static QWidget* activeWindow(const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static QWidget* getActiveObjectViewWindow(const QString& viewId);

    /** Checks that there are not view windows opened (active or non-active) with the given view id. */
    static void checkNoObjectViewWindowIsOpened(const QString& viewId);

    /** Checks if window with a given windowTitlePart is active or fails otherwise. Waits for the window to be active up to default timeout. */
    static QWidget* checkWindowIsActive(const QString& windowTitlePart);

    /** Returns list of all object view windows of the given type. */
    static QList<QWidget*> getAllObjectViewWindows(const QString& viewId);

    static QString activeWindowTitle();

    /** Activates window with the given title substring. Fails if no such window found. */
    static void activateWindow(const QString& windowTitlePart);

    /**
     * Finds a window with a given window title.
     * Fails if windowName is empty or because of FindOptions settings.
     */
    static QWidget* findWindow(const QString& windowTitle, const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    static void closeActiveWindow();
    static void closeWindow(const QString& windowName, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static void closeAllWindows();

    static QTabBar* getTabBar();
    static int getCurrentTab();
    static void clickTab(int tabIndex);
};

}  // namespace U2
