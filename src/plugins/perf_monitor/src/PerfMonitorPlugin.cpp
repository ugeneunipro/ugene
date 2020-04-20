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

#include "PerfMonitorPlugin.h"
#include "PerfMonitorView.h"

#include <U2Core/AppContext.h>
#include <U2Gui/MainWindow.h>

#include <QMenu>

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin * U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        PerfMonitorPlugin * plug = new PerfMonitorPlugin();
        return plug;
    }
    return NULL;
}

PerfMonitorPlugin::PerfMonitorPlugin() : Plugin( tr("Performance Monitor"), tr("Shows performance counters") ) {
    windowId = 0;
    openWindowAction = new QAction(tr("Show counters"), this);
    openWindowAction->setObjectName("Show counters");
    openWindowAction->setIcon(QIcon(":perf_monitor/images/mon.png"));
    connect(openWindowAction, SIGNAL(triggered()), SLOT(sl_openWindow()));

    QMenu* toolsMenu = AppContext::getMainWindow()->getTopLevelMenu(MWMENU_TOOLS);
    toolsMenu->addAction(openWindowAction);
}

void PerfMonitorPlugin::sl_openWindow() {
    MWMDIManager* mdi = AppContext::getMainWindow()->getMDIManager();
    MWMDIWindow* mdiWindow = mdi->getWindowById(windowId);
    if (mdiWindow==NULL) {
        mdiWindow = new PerfMonitorView();
        mdiWindow->setWindowIcon(QIcon(":perf_monitor/images/mon.png"));
        windowId = mdiWindow->getId();
        mdi->addMDIWindow(mdiWindow);
    }
    mdi->activateWindow(mdiWindow);

}

} //namespace
