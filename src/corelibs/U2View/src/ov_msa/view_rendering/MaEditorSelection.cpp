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

#include "MaEditorSelection.h"

namespace U2 {

/************************************************************************/
/* MaEditorSelection */
/************************************************************************/
MaEditorSelection::MaEditorSelection() {

}

MaEditorSelection::MaEditorSelection(int left, int top, int width, int height)
    : selArea(left, top, width, height) {

}

MaEditorSelection::MaEditorSelection(const QPoint& topLeft, const QPoint& bottomRight)
    : selArea(topLeft, bottomRight) {

}

MaEditorSelection::MaEditorSelection(const QPoint& topLeft, int width, int height)
    : selArea(topLeft, QSize(width,height)) {

}

bool MaEditorSelection::isEmpty() const {
    return selArea.width() <= 0 || selArea.height() <= 0;
}

QPoint MaEditorSelection::topLeft() const {
    return selArea.topLeft();
}

QPoint MaEditorSelection::bottomRight() const {
    return selArea.bottomRight();
}

QRect MaEditorSelection::toRect() const {
    return isEmpty() ? QRect(0, 0, 0, 0) : selArea;
}

int MaEditorSelection::x() const {
    return selArea.x();
}

int MaEditorSelection::y() const {
    return selArea.y();
}

int MaEditorSelection::width() const {
    return selArea.width();
}

int MaEditorSelection::height() const {
    return selArea.height();
}

int MaEditorSelection::bottom() const {
    return selArea.bottom();
}

U2Region MaEditorSelection::getXRegion() const {
    return U2Region(selArea.x(), selArea.width());
}

U2Region MaEditorSelection::getYRegion() const {
    return U2Region(selArea.y(), selArea.height());
}

bool MaEditorSelection::operator==(const MaEditorSelection& other) const {
    return selArea == other.selArea;
}

MaEditorSelection MaEditorSelection::intersected(const MaEditorSelection& selection) const {
    QRect r = selArea.intersected(selection.selArea);
    return MaEditorSelection(r);
}

MaEditorSelection::MaEditorSelection(QRect& rect)
    : selArea(rect) {

}

} // namespace
