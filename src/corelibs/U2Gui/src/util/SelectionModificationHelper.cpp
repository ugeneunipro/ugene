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

#include <QRect>
#include <QLineF>

#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GraphUtils.h>

#include "SelectionModificationHelper.h"

namespace U2 {

const int SelectionModificationHelper::PIXEL_OFFSET_FOR_BORDER_POINTING = 3;
const double SelectionModificationHelper::PIXEL_OFFSET_FOR_CIRCULAR_VIEW = 0.075;
const int SelectionModificationHelper::GRADUATION = 16;

SelectionModificationHelper::MovableSide SelectionModificationHelper::getMovableSide(const Qt::CursorShape shape, const QPoint& globalMousePos, const QRect& selection, const QSizeF& baseSize) {
    double leftBorderPosition = 0;
    double rightBorderPosition = 0;
    calculateBordersPositions(selection.x(), selection.width(), baseSize.width(), leftBorderPosition, rightBorderPosition);

    double topBorderPosition = 0;
    double bottomBorderPosition = 0;
    calculateBordersPositions(selection.y(), selection.height(), baseSize.height(), topBorderPosition, bottomBorderPosition);

    MovableSide border = NoMovableBorder;
    switch (shape) {
    case Qt::SizeFDiagCursor: {/*    \      */
        const QLineF fromMouseToLeftTop(QPoint(leftBorderPosition, topBorderPosition), globalMousePos);
        const qreal distanceFromMouseToLeftTop = fromMouseToLeftTop.length();
        const QLineF fromMouseToRightBottom(QPoint(rightBorderPosition, bottomBorderPosition), globalMousePos);
        const qreal distanceFromMouseToRightBottom = fromMouseToRightBottom.length();
        if (distanceFromMouseToLeftTop <= distanceFromMouseToRightBottom) {
            border = LeftTopCorner;
        } else {
            border = RightBottomCorner;
        }
        break;
    }
    case Qt::SizeBDiagCursor: {/*    /      */
        const QLineF fromMouseToRightTop(QPoint(rightBorderPosition, topBorderPosition), globalMousePos);
        const qreal distanceFromMouseToRightTop = fromMouseToRightTop.length();
        const QLineF fromMouseToLeftBottom(QPoint(leftBorderPosition, bottomBorderPosition), globalMousePos);
        const qreal distanceFromMouseToLeftBottom = fromMouseToLeftBottom.length();
        if (distanceFromMouseToRightTop <= distanceFromMouseToLeftBottom) {
            border = RightTopCorner;
        } else {
            border = LeftBottomCorner;
        }
        break;
    }
    case Qt::SizeHorCursor: {
        const double distanceFromMouseToLeft = qAbs(globalMousePos.x() - leftBorderPosition);
        const double distanceFromMouseToRight = qAbs(globalMousePos.x() - rightBorderPosition);
        if (distanceFromMouseToLeft <= distanceFromMouseToRight) {
            border = LeftBorder;
        } else {
            border = RightBorder;
        }
        break;
    }
    case Qt::SizeVerCursor: {
        const double distanceFromMouseToTop = qAbs(globalMousePos.y() - topBorderPosition);
        const double distanceFromMouseToBottom = qAbs(globalMousePos.y() - bottomBorderPosition);
        if (distanceFromMouseToTop <= distanceFromMouseToBottom) {
            border = TopBorder;
        } else {
            border = BottomBorder;
        }
        break;
    }
    default:
        assert(false);
        break;
    }

    assert(border != NoMovableBorder);
    return border;
}


SelectionModificationHelper::MovableSide SelectionModificationHelper::getMovableSide(const double arcsinCurrent, const int startBase, const int endBase, const int sequenceLength) {
    double asinStart = 0;
    double asinEnd = 0;
    int selectionSize = endBase - startBase;
    CHECK(sequenceLength > 0, NoMovableBorder);

    double baseSize = 2 * PI / sequenceLength;
    calculateBordersPositions(startBase, selectionSize, baseSize, asinStart, asinEnd);

    while (asinStart >= 2 * PI) {
        asinStart -= 2 * PI;
    }
    while (asinEnd > 2 * PI) {
        asinEnd -= 2 * PI;
    }

    if (asinStart - PIXEL_OFFSET_FOR_CIRCULAR_VIEW <= arcsinCurrent && arcsinCurrent <= asinStart + PIXEL_OFFSET_FOR_CIRCULAR_VIEW) {
        return LeftBorder;
    } else if (asinEnd - PIXEL_OFFSET_FOR_CIRCULAR_VIEW <= arcsinCurrent && arcsinCurrent <= asinEnd + PIXEL_OFFSET_FOR_CIRCULAR_VIEW) {
        return RightBorder;
    }
    return NoMovableBorder;
}

Qt::CursorShape SelectionModificationHelper::getCursorShape(const QPoint& globalMousePos, const QRect& selection, const double baseWidth, const double baseHeight) {
    double leftBorder = 0;
    double rightBorder = 0;
    calculateBordersPositions(selection.x(), selection.width(), baseWidth, leftBorder, rightBorder);
    const MovableSide verticalBorder = getMovableSide(globalMousePos.x(), selection.x(), selection.width(), baseWidth);

    double topBorder = 0;
    double bottomBorder = 0;
    calculateBordersPositions(selection.y(), selection.height(), baseHeight, topBorder, bottomBorder);
    const MovableSide horizontalBorder = getMovableSide(globalMousePos.y(), selection.y(), selection.height(), baseHeight);

    if ((verticalBorder == LeftBorder && horizontalBorder == LeftBorder) || (verticalBorder == RightBorder && horizontalBorder == RightBorder)) {
        return Qt::SizeFDiagCursor;
    } else if ((verticalBorder == RightBorder && horizontalBorder == LeftBorder) || (verticalBorder == LeftBorder && horizontalBorder == RightBorder)) {
        return Qt::SizeBDiagCursor;
    } else if (verticalBorder != 0 && topBorder <= globalMousePos.y() && globalMousePos.y() <= bottomBorder) {
        return Qt::SizeHorCursor;
    } else if (horizontalBorder != 0 && globalMousePos.x() >= leftBorder && globalMousePos.x() <= rightBorder) {
        return Qt::SizeVerCursor;
    }
    return Qt::ArrowCursor;
}

Qt::CursorShape SelectionModificationHelper::getCursorShape(const SelectionModificationHelper::MovableSide border, const Qt::CursorShape currentShape) {
    Qt::CursorShape newShape = currentShape;
    switch (border) {
    case RightTopCorner:
    case LeftBottomCorner:
        newShape = Qt::SizeBDiagCursor;
        break;
    case RightBottomCorner:
    case LeftTopCorner:
        newShape = Qt::SizeFDiagCursor;
        break;
    case LeftBorder:
    case RightBorder:
        newShape = Qt::SizeHorCursor;
        break;
    case TopBorder:
    case BottomBorder:
        newShape = Qt::SizeVerCursor;
        break;
    default:
        newShape = Qt::ArrowCursor;
        break;
    }

    return newShape;
}

Qt::CursorShape SelectionModificationHelper::getCursorShape(const double arcsinCurrent, const int startBase, const int endBase, const int sequenceLength) {
    if (getMovableSide(arcsinCurrent, startBase, endBase, sequenceLength) != NoMovableBorder) {
        return getCursorShape(arcsinCurrent);
    }

    return Qt::ArrowCursor;
}

Qt::CursorShape SelectionModificationHelper::getCursorShape(const double arcsinCurrent) {
    Qt::CursorShape resultShape = Qt::ArrowCursor;
    if ((PI / 8 <= arcsinCurrent && arcsinCurrent <= 3 * PI / 8) || ((9 * PI / 8 <= arcsinCurrent && arcsinCurrent <= 11 * PI / 8))) {
        resultShape = Qt::SizeBDiagCursor;
    } else if ((3 * PI / 8 <= arcsinCurrent && arcsinCurrent <= 5 * PI / 8) || ((11 * PI / 8 <= arcsinCurrent && arcsinCurrent <= 13 * PI / 8))) {
        resultShape = Qt::SizeHorCursor;
    } else if ((5 * PI / 8 <= arcsinCurrent && arcsinCurrent <= 7 * PI / 8) || ((13 * PI / 8 <= arcsinCurrent && arcsinCurrent <= 15 * PI / 8))) {
        resultShape = Qt::SizeFDiagCursor;
    } else {
        resultShape = Qt::SizeVerCursor;
    }
    return resultShape;
}

U2Region SelectionModificationHelper::getNewSelectionForBorderMoving(MovableSide& border, const int globalMousePos, const double baseSize, const U2Region& currentSelection) {
    CHECK(border != NoMovableBorder, U2Region());
    CHECK(globalMousePos >= 0, U2Region());
    CHECK(baseSize > 0, U2Region());

    const double tmp = globalMousePos / baseSize;
    const int numOfNewSelBase = qRound(tmp);
    U2Region resultSelection;
    switch (border) {
    case LeftBorder:
    case TopBorder: {
        int diff = currentSelection.startPos - numOfNewSelBase;
        int newLength = currentSelection.length + diff;
        if (newLength < 0) {
            resultSelection = U2Region(numOfNewSelBase + newLength, qAbs(newLength));
            border = getOppositeBorder(border);
        } else if (newLength == 0) {
            resultSelection = U2Region(currentSelection.startPos, currentSelection.length);
        } else {
            resultSelection = U2Region(numOfNewSelBase, newLength);
        }
        break;
    }
    case RightBorder:
    case BottomBorder: {
        int newLength = numOfNewSelBase - currentSelection.startPos;
        if (newLength < 0) {
            resultSelection = U2Region(numOfNewSelBase, qAbs(newLength));
            border = getOppositeBorder(border);
        } else {
            newLength = qMax(1, newLength);
            resultSelection = U2Region(currentSelection.startPos, newLength);
        }
        break;
    }
    default:
        return currentSelection;
    }

    return resultSelection;
}

QRect SelectionModificationHelper::getNewSelection(MovableSide& movableSide, const QPoint& globalMousePos, const QSizeF& baseSize, const QRect& currentSelection) {
    CHECK(movableSide != NoMovableBorder, QRect());
    CHECK(globalMousePos.x() >= 0 && globalMousePos.y() >= 0, QRect());

    QRect resultSelection;

    switch (movableSide) {
    case LeftTopCorner:
    case LeftBottomCorner:
    case RightTopCorner:
    case RightBottomCorner:
        resultSelection = getNewSelectionForCornerMoving(movableSide, globalMousePos, baseSize, currentSelection);
        break;
    case LeftBorder:
    case RightBorder: {
        U2Region horizontalSelection = getNewSelectionForBorderMoving(movableSide, globalMousePos.x(), baseSize.width(), U2Region(currentSelection.x(), currentSelection.width()));
        resultSelection = QRect(horizontalSelection.startPos, currentSelection.y(), horizontalSelection.length, currentSelection.height());
        break;
    }
    case TopBorder:
    case BottomBorder: {
        U2Region verticalSelection = getNewSelectionForBorderMoving(movableSide, globalMousePos.y(), baseSize.height(), U2Region(currentSelection.y(), currentSelection.height()));
        resultSelection = QRect(currentSelection.x(), verticalSelection.startPos, currentSelection.width(), verticalSelection.length);
        break;
    }
    default:
        assert(false);
        break;
    }

    return resultSelection;
}

QRect SelectionModificationHelper::getNewSelectionForCornerMoving(MovableSide& corner, const QPoint& globalMousePos, const QSizeF& baseSize, const QRect& currentSelection) {
    CHECK(corner != NoMovableBorder, QRect());
    CHECK(globalMousePos.x() >= 0 && globalMousePos.y() >= 0, QRect());

    MovableSide horizontalBorder = NoMovableBorder;
    MovableSide verticalBorder = NoMovableBorder;
    switch (corner) {
    case LeftTopCorner: {
        horizontalBorder = LeftBorder;
        verticalBorder = TopBorder;
        break;
    }
    case LeftBottomCorner: {
        horizontalBorder = LeftBorder;
        verticalBorder = BottomBorder;
        break;
    }
    case RightTopCorner: {
        horizontalBorder = RightBorder;
        verticalBorder = TopBorder;
        break;
    }
    case RightBottomCorner:
        horizontalBorder = RightBorder;
        verticalBorder = BottomBorder;
        break;
    default:
        return currentSelection;
    }

    CHECK(horizontalBorder != NoMovableBorder, QRect());
    CHECK(verticalBorder != NoMovableBorder, QRect());

    U2Region horizontalSelection = getNewSelectionForBorderMoving(horizontalBorder, globalMousePos.x(), baseSize.width(), U2Region(currentSelection.x(), currentSelection.width()));
    U2Region verticalSelection = getNewSelectionForBorderMoving(verticalBorder, globalMousePos.y(), baseSize.height(), U2Region(currentSelection.y(), currentSelection.height()));
    corner = getNewMovableCorner(horizontalBorder, verticalBorder);

    QRect resultSelection(horizontalSelection.startPos, verticalSelection.startPos, horizontalSelection.length, verticalSelection.length);
    return resultSelection;
}

QList<U2Region> SelectionModificationHelper::getNewSelection(MovableSide& board, const double mouseAngle, const double rotationDegree, const int sequenceLength, const int startBase, const int endBase, bool& isTwoRegions) {
    double currentAngle = 180 * GRADUATION * mouseAngle / PI;
    currentAngle -= rotationDegree * GRADUATION;
    if (currentAngle < 0) {
        currentAngle += 360 * GRADUATION;
    }
    CHECK(sequenceLength > 0, QList<U2Region>());
    int newSelEdge = int(currentAngle / (360.0 * GRADUATION) * sequenceLength + 0.5f);
    newSelEdge = newSelEdge == 0 ? sequenceLength : newSelEdge;

    int newStartBase = startBase;
    if (startBase == 0 /*&& isTwoPartsLastSelecton && mouseAngle > PI / 2*/) {
        board = LeftBorder;
    }
    int newEndBase = endBase;
    switch(board) {
    case LeftBorder:
        if (newEndBase < newSelEdge) {
            board = RightBorder;
            newStartBase = endBase;
            newEndBase = newSelEdge;
            isTwoRegions = !isTwoRegions;
        } else {
            newStartBase = newSelEdge;
        }
        break;
    case RightBorder:
        if (newStartBase > newSelEdge) {
            board = LeftBorder;
            newEndBase = startBase;
            newStartBase = newSelEdge;
            isTwoRegions = !isTwoRegions;
        } else {
            newEndBase = newSelEdge;
        }
        break;
    default:
        return QList<U2Region>();
    }

    if (newStartBase == newEndBase) {
        newEndBase++;
    } else if (newEndBase < newStartBase) {
        qSwap(newEndBase, newStartBase);
    }

    QList<U2Region> result;
    if (isTwoRegions) {
        result << U2Region(0, newStartBase);
        result << U2Region(newEndBase, sequenceLength - newEndBase);
    } else {
        result << U2Region(newStartBase, newEndBase - newStartBase);
    }
    return result;
}

SelectionModificationHelper::MovableSide SelectionModificationHelper::getMovableSide(const int globalMousePos, const int selectionPos, const int selectionSize, const double baseSize) {
    double leftOrTopBorderPosition = 0;
    double rightOrBottomBorderPosition = 0;
    calculateBordersPositions(selectionPos, selectionSize, baseSize, leftOrTopBorderPosition, rightOrBottomBorderPosition);

    if (leftOrTopBorderPosition - PIXEL_OFFSET_FOR_BORDER_POINTING <= globalMousePos && globalMousePos <= leftOrTopBorderPosition + PIXEL_OFFSET_FOR_BORDER_POINTING) {
        return LeftBorder;
    } else if (rightOrBottomBorderPosition - PIXEL_OFFSET_FOR_BORDER_POINTING <= globalMousePos && globalMousePos <= rightOrBottomBorderPosition + PIXEL_OFFSET_FOR_BORDER_POINTING) {
        return RightBorder;
    }
    return NoMovableBorder;
}

void SelectionModificationHelper::calculateBordersPositions(const int selectionPos, const int selectionSize, const double baseSize, double& leftOrTopBorderPosition, double& rightOrBottomBorderPosition) {
    leftOrTopBorderPosition = selectionPos * baseSize;
    rightOrBottomBorderPosition = (selectionPos + selectionSize) * baseSize;
}

SelectionModificationHelper::MovableSide SelectionModificationHelper::getOppositeBorder(const MovableSide border) {
    MovableSide oppositeBorder = NoMovableBorder;
    switch (border) {
    case LeftBorder:
        oppositeBorder = RightBorder;
        break;
    case RightBorder:
        oppositeBorder = LeftBorder;
        break;
    case TopBorder:
        oppositeBorder = BottomBorder;
        break;
    case BottomBorder:
        oppositeBorder = TopBorder;
        break;
    default:
        FAIL("An unexpected case", NoMovableBorder);
    }

    assert(oppositeBorder != NoMovableBorder);
    return oppositeBorder;
}

SelectionModificationHelper::MovableSide SelectionModificationHelper::getNewMovableCorner(const MovableSide horizontalBorder, const MovableSide verticalBorder) {
    MovableSide newMovableCorner = NoMovableBorder;
    if (horizontalBorder == RightBorder && verticalBorder == TopBorder) {
        newMovableCorner = RightTopCorner;
    } else if (horizontalBorder == RightBorder && verticalBorder == BottomBorder) {
        newMovableCorner = RightBottomCorner;
    } else if (horizontalBorder == LeftBorder && verticalBorder == TopBorder) {
        newMovableCorner = LeftTopCorner;
    } else if (horizontalBorder == LeftBorder && verticalBorder == BottomBorder) {
        newMovableCorner = LeftBottomCorner;
    } else {
        assert(false);
    }

    return newMovableCorner;
}


} // namespace
