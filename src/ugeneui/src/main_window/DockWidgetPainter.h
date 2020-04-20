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

#ifndef _U2_DOCK_WIDGET_PAINTER_H_
#define _U2_DOCK_WIDGET_PAINTER_H_

#include <U2Gui/MainWindow.h>

namespace U2 {

class DockData;

class DockWidgetPainter {
public:
    static void updateLabel(DockData *d, bool active);

private:
    static QString findKeyPrefix(const QAction *action);
    static QColor getBackgroundColor();
    static QColor getInnerColor(bool active, const QColor &backgroundColor);
    static void drawBorder(bool active, const QSize &widgetSize, const QColor &backgroundColor, QPainter &updateLabel);
    static void setupOrientation(MWDockArea area, QPainter &updateLabel);
    static void drawIcon(const QIcon &icon, const QPoint &iconPoint, int iconSize, QPainter &updateLabel);
    static void drawText(const QString &keyPrefix, const QString &text, const QPoint &textPoint, QPainter &updateLabel);
};

class DockWidgetPaintData {
public:
    DockWidgetPaintData(const QIcon &icon, const QString &text, MWDockArea area);

    QSize calculateWidgetSize() const;
    QPoint calculateTextPoint(const QSize &widgetSize) const;
    QPoint calculateIconPoint(const QPoint &textPoint, const QSize &widgetSize) const;

    bool getHasIcon() const;
    int getIconSize() const;

private:
    MWDockArea area;
    bool hasIcon;
    int iconSize;
    int iconTextDist;
    QFontMetrics fm;
    int textWidth;
    int textHeight;

    static const int MAX_LABEL_BASE_WIDTH;
    static const int MAX_LABEL_EXTRA_WIDTH;
    static const int IDEAL_LABEL_HEIGHT;
    static const int MIN_LABEL_EXTRA_HEIGHT;
    static const int ICON_TEXT_DIST;
    static const int ICON_SIZE;
};

} // U2

#endif // _U2_DOCK_WIDGET_PAINTER_H_
