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

#include "HoverQLabel.h"

#include <QMouseEvent>

namespace U2 {

HoverQLabel::HoverQLabel(const QString& html, const QString& normalStyle, const QString& hoveredStyle, const QString& objectName)
    : QLabel(html), isHovered(false) {
    setCursor(Qt::PointingHandCursor);
    setObjectName(objectName);
    updateStyles(normalStyle, hoveredStyle);
}

void HoverQLabel::updateStyles(const QString& newNormalStyle, const QString& newHoveredStyle) {
    QString objName = objectName();
    normalStyle = objName.isEmpty() ? newNormalStyle : "#" + objName + " {" + newNormalStyle + "}";
    hoveredStyle = objName.isEmpty() ? newHoveredStyle : "#" + objName + " {" + newHoveredStyle + "}";
    setStyleSheet(isHovered ? hoveredStyle : normalStyle);
}

void HoverQLabel::enterEvent(QEvent* event) {
    isHovered = true;
    setStyleSheet(hoveredStyle);
    QLabel::enterEvent(event);
}

void HoverQLabel::leaveEvent(QEvent* event) {
    isHovered = false;
    setStyleSheet(normalStyle);
    QLabel::leaveEvent(event);
}

void HoverQLabel::mousePressEvent(QMouseEvent* event) {
    QLabel::mousePressEvent(event);
    emit clicked();
    event->accept();
}

}  // namespace U2
