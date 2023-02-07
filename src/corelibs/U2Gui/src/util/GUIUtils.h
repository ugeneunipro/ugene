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

#include <QAction>
#include <QList>
#include <QMenu>
#include <QAbstractSlider>
#include <QLabel>
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

    /** Inserts 'actionToInsert' right after 'insertionPointMarkerAction' in the menu. */
    static void insertActionAfter(QMenu* menu, QAction* insertionPointMarkerAction, QAction* actionToInsert);

    static void updateActionToolTip(QAction* action);
    static void updateButtonToolTip(QAbstractButton* button, const QKeySequence& shortcut);

    static void disableEmptySubmenus(QMenu* m);

    static QIcon createSquareIcon(const QColor& c, int size);
    static QIcon createRoundIcon(const QColor& c, int size);

    // Sets 'muted' look and feel. The item looks like disabled but still active and can be selected
    static void setMutedLnF(QTreeWidgetItem* item, bool disabled, bool recursive = false);
    static bool isMutedLnF(QTreeWidgetItem* item);

    static bool runWebBrowser(const QString& url);

    static void setWidgetWarningStyle(QWidget* widget, bool value);

    static void showMessage(QWidget* widgetToPaintOn, QPainter& painter, const QString& message);

    static const QColor WARNING_COLOR;
    static const QColor OK_COLOR;

    /**
     * Maximum size of the pixmap that can be safely created for UGENE.
     * Pixmaps with dimensions larger than this size may cause memory errors or not supported by OS.
     * Example: https://github.com/radekp/qt/blob/b881d8fb99972f1bd04ab4c84843cc8d43ddbeed/src/gui/image/qpixmap_x11.cpp#L1823
     */
    static constexpr int MAX_SAFE_PIXMAP_WIDTH = 10 * 1000;
    static constexpr int MAX_SAFE_PIXMAP_HEIGHT = 10 * 1000;
};

/** Resets QSlider value on double clicks. Uses the slider as a parent and is auto-deleted with a slider. */
class U2GUI_EXPORT ResetSliderOnDoubleClickBehavior : public QObject {
public:
    ResetSliderOnDoubleClickBehavior(QAbstractSlider* slider, QLabel* relatedLabel = nullptr);
    bool eventFilter(QObject*, QEvent* event) override;

    int defaultValue = 0;
};

}  // namespace U2
