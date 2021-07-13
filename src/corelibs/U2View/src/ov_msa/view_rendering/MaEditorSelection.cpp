/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MaEditorSelection.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

/************************************************************************/
/* MaEditorSelection */
/************************************************************************/

MaEditorSelection::MaEditorSelection(const QList<QRect> &rects) {
    for (const QRect &rect : qAsConst(rects)) {
        addRect(rect);
    }
}

bool MaEditorSelection::addRect(const QRect &rect) {
    SAFE_POINT(rect.x() >= 0 && rect.width() >= 0 && rect.y() >= 0 && rect.height() >= 0,
               QString("Invalid MSA selection rect: x:%1 y:%2 w:%3 h:%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()),
               false);
    for (const QRect &oldRect : qAsConst(rectList)) {
        if (oldRect.contains(rect)) {
            return false;
        }
        SAFE_POINT(!oldRect.intersects(rect), "Unsupported mode: selection rects must not intersect!", false);
    }
    rectList << rect;
    return true;
}

bool MaEditorSelection::isEmpty() const {
    return rectList.isEmpty();
}

QRect MaEditorSelection::toRect() const {
    if (rectList.isEmpty()) {
        return {0, 0, 0, 0};
    }
    QRect boundingRect = rectList[0];
    for (int i = 1; i < rectList.length(); i++) {
        const QRect &rect = rectList[i];
        QPoint topLeft(qMin(rect.x(), boundingRect.x()), qMin(rect.y(), boundingRect.y()));
        QPoint bottomRight(qMax(rect.right(), boundingRect.right()), qMax(rect.bottom(), boundingRect.bottom()));
        boundingRect = QRect(topLeft, bottomRight);
    }
    return boundingRect;
}

const QList<QRect> &MaEditorSelection::getRectList() const {
    return rectList;
}

bool MaEditorSelection::operator==(const MaEditorSelection &other) const {
    return other.getRectList() == rectList;
}

}    // namespace U2
