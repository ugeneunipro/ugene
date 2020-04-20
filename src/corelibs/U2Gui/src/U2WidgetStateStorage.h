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

#ifndef _U2_WIDGET_STATE_STORAGE_H_
#define _U2_WIDGET_STATE_STORAGE_H_

#include <QMultiMap>
#include <QVariantMap>

#include <U2Core/global.h>

namespace U2 {

class MWMDIWindow;
class U2SavableWidget;

class WidgetParamSnapshot {
public:
    explicit WidgetParamSnapshot(const QString &widgetId = QString());

    const QString & getWidgetId() const;
    void setParameter(const QString &name, const QVariant &value);
    QVariantMap getParameters() const;
    bool isValid() const;

private:
    QString widgetId;
    QVariantMap params;
};

class U2GUI_EXPORT U2WidgetStateStorage {
public:
    static void saveWidgetState(const U2SavableWidget &widget);
    static void restoreWidgetState(U2SavableWidget &widget);
    static void onWindowClose(MWMDIWindow *closedWindow);

private:
    static WidgetParamSnapshot findWidgetParams(const U2SavableWidget &widget);
    static bool windowExists(MWMDIWindow *window);

    static QMultiMap<MWMDIWindow *, WidgetParamSnapshot> window2widgetSnapshots;
};

} // namespace U2

#endif // _U2_WIDGET_STATE_STORAGE_H_
