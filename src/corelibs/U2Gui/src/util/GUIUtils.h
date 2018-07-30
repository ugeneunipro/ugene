/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GUI_UTILS_H_
#define _U2_GUI_UTILS_H_

#include <QAction>
#include <QList>
#include <QMenu>
#include <QTreeWidgetItem>

#include <U2Core/global.h>

#include "U2FileDialog.h"

class QAbstractButton;

namespace U2 {

class U2GUI_EXPORT GUIUtils : public QObject {
    Q_OBJECT
public:
    static QAction* findAction(const QList<QAction*>& actions, const QString& name);

    static QAction* getCheckedAction(QList<QAction*> actions);
    static QAction* findActionByData(QList<QAction*> actions, const QString& data);
    
    static QAction* findActionAfter(const QList<QAction*>& actions, const QString& name);

    static QMenu* findSubMenu(QMenu* m, const QString& name);

    static void updateActionToolTip(QAction *action);
    static void updateButtonToolTip(QAbstractButton *button, const QKeySequence &shortcut);

    static void disableEmptySubmenus(QMenu* m);

    static QIcon  createSquareIcon(const QColor& c, int size);
    static QIcon  createRoundIcon(const QColor& c, int size);
    
    
    // Sets 'muted' look and feel. The item looks like disabled but still active and can be selected
    static void setMutedLnF(QTreeWidgetItem* item, bool disabled, bool recursive = false);
    static bool isMutedLnF(QTreeWidgetItem* item);
    

    static bool runWebBrowser(const QString& url);

    static void setWidgetWarning(QWidget *widget, bool value);

    static void showMessage(QWidget *widgetToPaintOn, QPainter& painter, const QString& message);

    static const QColor WARNING_COLOR;
    static const QColor OK_COLOR;
};

}   // namespace U2

#endif
