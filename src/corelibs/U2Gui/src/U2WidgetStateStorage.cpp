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

#include <QWidget>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/U2SavableWidget.h>

#include "U2WidgetStateStorage.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//// WidgetParamSnapshot
//////////////////////////////////////////////////////////////////////////

WidgetParamSnapshot::WidgetParamSnapshot(const QString &widgetId)
    : widgetId(widgetId)
{

}

const QString & WidgetParamSnapshot::getWidgetId() const {
    return widgetId;
}

void WidgetParamSnapshot::setParameter(const QString &name, const QVariant &value) {
    params[name] = value;
}

QVariantMap WidgetParamSnapshot::getParameters() const {
    return params;
}

bool WidgetParamSnapshot::isValid() const {
    return !widgetId.isEmpty();
}

//////////////////////////////////////////////////////////////////////////
//// U2WidgetStateStorage
//////////////////////////////////////////////////////////////////////////

QMultiMap<MWMDIWindow *, WidgetParamSnapshot> U2WidgetStateStorage::window2widgetSnapshots;

void U2WidgetStateStorage::saveWidgetState(const U2SavableWidget &widget) {
    MWMDIWindow *contextWindow = widget.getContextWindow();
    CHECK(windowExists(contextWindow), );

    const QString widgetId = widget.getWidgetId();
    WidgetParamSnapshot snapshot(widgetId);
    foreach (const QString &childId, widget.getChildIds()) {
        snapshot.setParameter(childId, widget.getChildValue(childId));
    }
    window2widgetSnapshots.insert(contextWindow, snapshot);
}

void U2WidgetStateStorage::restoreWidgetState(U2SavableWidget &widget) {
    const WidgetParamSnapshot snapshot = findWidgetParams(widget);
    CHECK(snapshot.isValid(), );

    const QVariantMap params = snapshot.getParameters();
    foreach (const QString &paramName, params.keys()) {
        if (widget.childValueIsAcceptable(paramName, params[paramName])) {
            widget.setChildValue(paramName, params[paramName]);
        }
    }
}

void U2WidgetStateStorage::onWindowClose(MWMDIWindow *closedWindow) {
    window2widgetSnapshots.remove(closedWindow);
}

bool U2WidgetStateStorage::windowExists(MWMDIWindow *window) {
    CHECK(NULL != window, true);

    MWMDIManager *mdiManager = AppContext::getMainWindow()->getMDIManager();
    SAFE_POINT(NULL != mdiManager, "Invalid MDI manager", false);
    return mdiManager->getWindows().contains(window);
}

WidgetParamSnapshot U2WidgetStateStorage::findWidgetParams(const U2SavableWidget &widget) {
    MWMDIWindow *contextWindow = widget.getContextWindow();
    CHECK(window2widgetSnapshots.contains(contextWindow), WidgetParamSnapshot());
    foreach (const WidgetParamSnapshot &snapshot, window2widgetSnapshots.values(contextWindow)) {
        if (snapshot.getWidgetId() == widget.getWidgetId()) {
            return snapshot;
        }
    }
    return WidgetParamSnapshot();
}

} // namespace U2
