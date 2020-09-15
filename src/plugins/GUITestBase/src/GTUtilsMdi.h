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

#ifndef _U2_GT_UTILS_MDI_H_
#define _U2_GT_UTILS_MDI_H_

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
    static void click(HI::GUITestOpStatus &os, GTGlobals::WindowAction action);
    static QPoint getMdiItemPosition(HI::GUITestOpStatus &os, const QString &windowName);
    static void selectRandomRegion(HI::GUITestOpStatus &os, const QString &windowName);
    static bool isAnyPartOfWindowVisible(HI::GUITestOpStatus &os, const QString &windowName);

    // fails if MainWindow is NULL or because of FindOptions settings
    static QWidget *activeWindow(HI::GUITestOpStatus &os, const GTGlobals::FindOptions & = GTGlobals::FindOptions());
    static QWidget *getActiveObjectViewWindow(HI::GUITestOpStatus &os, const QString &viewId);

    /** Checks that there are not view windows opened (active or non-active) with the given view id. */
    static void checkNoObjectViewWindowIsOpened(HI::GUITestOpStatus &os, const QString &viewId);

    /** Checks if window with a given windowTitlePart is active or fails otherwise. Waits for the window to be active up to default timeout. */
    static QWidget *checkWindowIsActive(HI::GUITestOpStatus &os, const QString &windowTitlePart);

    /** Returns list of all object view windows of the given type. */
    static QList<QWidget *> getAllObjectViewWindows(const QString &viewId);

    static QString activeWindowTitle(HI::GUITestOpStatus &os);

    /** Activates window with the given title substring. Fails if no such window found. */
    static void activateWindow(HI::GUITestOpStatus &os, const QString &windowTitlePart);

    /**
     * Finds a window with a given window title.
     * Fails if windowName is empty or because of FindOptions settings.
     */
    static QWidget *findWindow(HI::GUITestOpStatus &os, const QString &windowTitle, const GTGlobals::FindOptions & = GTGlobals::FindOptions());

    static void closeActiveWindow(HI::GUITestOpStatus &os);
    static void closeWindow(HI::GUITestOpStatus &os, const QString &windowName, const GTGlobals::FindOptions & = GTGlobals::FindOptions());
    static void closeAllWindows(HI::GUITestOpStatus &os);

    static bool isTabbedLayout(HI::GUITestOpStatus &os);

    static QTabBar *getTabBar(HI::GUITestOpStatus &os);
    static int getCurrentTab(HI::GUITestOpStatus &os);
    static void clickTab(HI::GUITestOpStatus &os, int tabIndex);
};

}    // namespace U2

#endif
