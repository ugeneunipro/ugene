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

#ifndef _HI_GUI_GTTOOLBAR_H_
#define _HI_GUI_GTTOOLBAR_H_

#include "GTGlobals.h"
#include <QToolBar>

namespace HI {

class HI_EXPORT GTToolbar {
public:

    static QToolBar* getToolbar(GUITestOpStatus &os, const QString &toolbarSysName);

    static QWidget* getWidgetForAction(GUITestOpStatus &os, const QToolBar *toolbar, QAction *action);
    static QWidget* getWidgetForActionName(GUITestOpStatus &os, const QToolBar *toolbar, const QString &actionName);
    static QWidget* getWidgetForActionTooltip(GUITestOpStatus &os, const QToolBar *toolbar, const QString &tooltip);

    static void clickButtonByTooltipOnToolbar(GUITestOpStatus &os, const QString &toolbarSysName, const QString &tooltip);

private:

    static QAction* getToolbarAction(GUITestOpStatus &os, const QString &actionName, const QToolBar *toolbar);


};

} // namespace

#endif
