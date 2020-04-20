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


#ifndef _U2_SELECTION_MODIFICATION_HELPER_
#define _U2_SELECTION_MODIFICATION_HELPER_

#include <QCursor>


namespace U2 {

class U2GUI_EXPORT SelectionModificationHelper {
public:
    enum MovableSide {
        NoMovableBorder,
        LeftBorder,
        RightBorder,
        TopBorder,
        BottomBorder,
        LeftTopCorner,
        LeftBottomCorner,
        RightTopCorner,
        RightBottomCorner
    };

    static MovableSide getMovableSide(const Qt::CursorShape shape, const QPoint& globalMousePos, const QRect& selection, const QSizeF& baseSize);
    static MovableSide getMovableSide(const double arcsinCurrent, const int startBase, const int endBase, const int sequenceLength);
    static Qt::CursorShape getCursorShape(const QPoint& globalMousePos, const QRect& selection, const double baseWidth, const double baseHeight);
    static Qt::CursorShape getCursorShape(const MovableSide border, const Qt::CursorShape currentShape);
    static Qt::CursorShape getCursorShape(const double arcsinCurrent, const int startBase, const int endBase, const int sequenceLength);
    static Qt::CursorShape getCursorShape(const double arcsinCurrent);
    static QRect getNewSelection(MovableSide& movableSide, const QPoint& globalMousePos, const QSizeF& baseSize, const QRect& currentSelection);
    static QList<U2Region> getNewSelection(MovableSide& border, const double mouseAngle, const double rotation, const int sequenceLength, const int startBase, const int endBase, bool& isTwoRegions);

private:
    static MovableSide getMovableSide(const int globalMousePos, const int selectionPos, const int selectionSize, const double baseSize);
    static U2Region getNewSelectionForBorderMoving(MovableSide& border, const int globalMousePos, const double baseSize, const U2Region& currentSelection);
    static QRect getNewSelectionForCornerMoving(MovableSide& corner, const QPoint& globalMousePos, const QSizeF& baseSize, const QRect& currentSelection);
    static void calculateBordersPositions(const int selectionPos, const int selectionSize, const double baseSize, double& leftOrTopBorderPosition, double& rightOrBottomBorderPosition);
    static MovableSide getOppositeBorder(const MovableSide border);
    static MovableSide getNewMovableCorner(const MovableSide horizontalBorder, const MovableSide verticalBorder);

    static const int PIXEL_OFFSET_FOR_BORDER_POINTING;
    static const double PIXEL_OFFSET_FOR_CIRCULAR_VIEW;
    /**
    *Must be equal to the similar named value in CircularView class
    */
    static const int GRADUATION;
};

} // namespace

#endif // _U2_SELECTION_MODIFICATION_HELPER_
