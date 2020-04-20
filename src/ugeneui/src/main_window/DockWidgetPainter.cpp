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

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QPainter>

#include <U2Core/U2SafePoints.h>

#include "DockManagerImpl.h"
#include "DockWidgetPainter.h"

namespace U2 {

const int DockWidgetPaintData::MAX_LABEL_BASE_WIDTH = 90;
const int DockWidgetPaintData::MAX_LABEL_EXTRA_WIDTH = 20;
const int DockWidgetPaintData::IDEAL_LABEL_HEIGHT = 25;
const int DockWidgetPaintData::MIN_LABEL_EXTRA_HEIGHT = 6;
const int DockWidgetPaintData::ICON_TEXT_DIST = 6;
const int DockWidgetPaintData::ICON_SIZE = 16;

void DockWidgetPainter::updateLabel(DockData *d, bool active) {
    const QIcon icon = d->wrapWidget->windowIcon();
    const QString text = d->wrapWidget->windowTitle();
    const QString keyPrefix = findKeyPrefix(d->action);
    const DockWidgetPaintData paintData(icon, keyPrefix + text, d->area);

    // Create pixmap
    const QSize widgetSize = paintData.calculateWidgetSize();
    const int devicePixelRatio = d->wrapWidget->devicePixelRatio();
    QPixmap pixmap(widgetSize * devicePixelRatio);
    pixmap.setDevicePixelRatio(devicePixelRatio);
    pixmap.fill(getBackgroundColor());

    // Paint
    QPainter painter;
    painter.begin(&pixmap);
    drawBorder(active, widgetSize, getBackgroundColor(), painter);
    setupOrientation(d->area, painter);
    QPoint textPoint = paintData.calculateTextPoint(widgetSize);
    drawText(keyPrefix, text, textPoint, painter);
    if (paintData.getHasIcon()) {
       QPoint iconPoint = paintData.calculateIconPoint(textPoint, widgetSize);
       drawIcon(icon, iconPoint, paintData.getIconSize(), painter);
    }
    painter.end();

    // Save results
    d->label->resize(widgetSize);
    d->label->setPixmap(pixmap);
}

QString DockWidgetPainter::findKeyPrefix(const QAction *action) {
    const QKeySequence ks = action == NULL ? QKeySequence(): action->shortcut();
    if (ks.count() == 1) {
        for (int k = (int)Qt::Key_0; k <= (int)Qt::Key_9; k++) {
            if (ks[0] == (k | (int)Qt::ALT)) {
                return QString::number(k - (int)Qt::Key_0) + ": ";
            }
        }
    }
    return "";
}

QColor DockWidgetPainter::getBackgroundColor() {
#ifdef Q_OS_WIN
    return QColor(Qt::transparent);
#else
    return QApplication::palette().brush(QPalette::Window).color();
#endif
}

QColor DockWidgetPainter::getInnerColor(bool active, const QColor &backgroundColor) {
#ifdef Q_OS_WIN
    Q_UNUSED(active);
    Q_UNUSED(backgroundColor);
    return QColor(0, 0, 0, active ? 30 : 5);
#else
    QColor innerColor = backgroundColor;
    if (active) {
        innerColor = backgroundColor.darker(115);
    }
    return innerColor;
#endif
}

void DockWidgetPainter::drawBorder(bool active, const QSize &widgetSize, const QColor &backgroundColor, QPainter &painter) {
    const QRectF roundedRect(2, 2, widgetSize.width() - 4, widgetSize.height() - 4);
    const QColor innerColor = getInnerColor(active, backgroundColor);
    painter.setPen(Qt::black);
    painter.fillRect(roundedRect, innerColor);
    painter.drawLine((int) roundedRect.left() + 1, (int)roundedRect.top(), (int)roundedRect.right() - 1, (int)roundedRect.top());
    painter.drawLine((int)roundedRect.left() + 1, (int)roundedRect.bottom(), (int)roundedRect.right() - 1, (int)roundedRect.bottom());
    painter.drawLine((int)roundedRect.left(), (int)roundedRect.top() + 1, (int)roundedRect.left(), (int)roundedRect.bottom() - 1);
    painter.drawLine((int)roundedRect.right(), (int)roundedRect.top() + 1, (int)roundedRect.right(), (int)roundedRect.bottom() - 1);
}

void DockWidgetPainter::setupOrientation(MWDockArea area, QPainter &painter) {
    if (area == MWDockArea_Left) {
        painter.rotate(-90);
    } else if (area == MWDockArea_Right) {
        painter.rotate(90);
    }
}

void DockWidgetPainter::drawIcon(const QIcon &icon, const QPoint &iconPoint, int iconSize, QPainter &painter) {
    const QPixmap p = icon.pixmap(iconSize, iconSize);
    painter.drawPixmap(iconPoint, p);
}

void DockWidgetPainter::drawText(const QString &keyPrefix, const QString &text, const QPoint &textPoint, QPainter &painter) {
    int prefixDx = 0;
    QString plainText = text;
    if (!keyPrefix.isEmpty()) {
        QFont font; //app default
        font.setUnderline(true);

        painter.setFont(font);
        prefixDx = QFontMetrics(font).width(keyPrefix[0]);
        painter.drawText(textPoint.x(), textPoint.y(), keyPrefix.left(1));
        plainText = keyPrefix.mid(1) + text;

        font.setUnderline(false);
        painter.setFont(font);
    }
    painter.drawText(textPoint.x() + prefixDx, textPoint.y(), plainText);
}

DockWidgetPaintData::DockWidgetPaintData(const QIcon &icon, const QString &text, MWDockArea area)
: area(area),
  fm(QFontMetrics(QFont())) //app default
{
    hasIcon = !icon.isNull();
    iconSize = hasIcon ? ICON_SIZE : 0;
    iconTextDist = hasIcon ? ICON_TEXT_DIST : 0;

    textWidth = fm.width(text);
    textHeight = fm.height();
}

QSize DockWidgetPaintData::calculateWidgetSize() const {
    const bool horizontal = (area == MWDockArea_Bottom);
    const int width = qMax(textWidth + iconSize + iconTextDist, MAX_LABEL_BASE_WIDTH) + MAX_LABEL_EXTRA_WIDTH;
    const int height = qMax(IDEAL_LABEL_HEIGHT, textHeight + MIN_LABEL_EXTRA_HEIGHT);
    return QSize(horizontal ? width : height, horizontal ? height: width);
}

QPoint DockWidgetPaintData::calculateTextPoint(const QSize &widgetSize) const {
    const int widgetWidth = (area == MWDockArea_Bottom) ? widgetSize.width() : widgetSize.height();
    const int fontYOffset = fm.ascent() / 2;
    const int fontXOffset = (widgetWidth - textWidth - iconSize - iconTextDist) / 2 + iconSize + iconTextDist;
    if (area == MWDockArea_Left) {
        return QPoint(fontXOffset - widgetWidth, widgetSize.width()/2 + fontYOffset);
    } else if (area == MWDockArea_Right) {
        return QPoint(fontXOffset, -widgetSize.width()/2 + fontYOffset);
    } else {
        return QPoint(fontXOffset, widgetSize.height()/2 + fontYOffset);
    }
}

QPoint DockWidgetPaintData::calculateIconPoint(const QPoint &textPoint, const QSize &widgetSize) const {
    CHECK(hasIcon, QPoint());

    const int x = textPoint.x() - iconTextDist - iconSize;
    if (area == MWDockArea_Left) {
        return QPoint(x, 1 + (widgetSize.width() - iconSize) / 2);
    } else if (area == MWDockArea_Right) {
        return QPoint(x, -(1 + (widgetSize.width() - iconSize) / 2) - iconSize);
    } else {
        return QPoint(x, 1 + (widgetSize.height() - iconSize) / 2);
    }
}

bool DockWidgetPaintData::getHasIcon() const {
    return hasIcon;
}

int DockWidgetPaintData::getIconSize() const {
    return iconSize;
}

} // U2
