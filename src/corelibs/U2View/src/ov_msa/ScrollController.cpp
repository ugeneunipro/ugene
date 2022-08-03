/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "ScrollController.h"

#include "U2Core/U2SafePoints.h"
#include <U2Core/MultipleAlignmentObject.h>

#include "BaseWidthController.h"
#include "DrawHelper.h"
#include "RowHeightController.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MaEditorWgt.h"

namespace U2 {

ScrollController::ScrollController(MaEditor* maEditor, MaEditorWgt* maEditorUi)
    : QObject(maEditorUi),
      maEditor(maEditor),
      ui(maEditorUi),
      savedFirstVisibleMaRow(0),
      savedFirstVisibleMaRowOffset(0) {
    connect(this, SIGNAL(si_visibleAreaChanged()), maEditorUi, SIGNAL(si_completeRedraw()));
    connect(maEditor->getCollapseModel(), SIGNAL(si_aboutToBeToggled()), SLOT(sl_collapsibleModelIsAboutToBeChanged()));
    connect(maEditor->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_collapsibleModelChanged()));
}

void ScrollController::init(GScrollBar* hScrollBar, GScrollBar* vScrollBar) {
    this->hScrollBar = hScrollBar;
    hScrollBar->setValue(0);
    connect(hScrollBar, SIGNAL(valueChanged(int)), SIGNAL(si_visibleAreaChanged()));

    this->vScrollBar = vScrollBar;
    vScrollBar->setValue(0);
    connect(vScrollBar, SIGNAL(valueChanged(int)), SIGNAL(si_visibleAreaChanged()));

    sl_updateScrollBars();
}

QPoint ScrollController::getScreenPosition() const {
    return QPoint(hScrollBar->value(), vScrollBar->value());
}

QPoint ScrollController::getGlobalMousePosition(const QPoint& mousePos) const {
    return mousePos + getScreenPosition();
}

void ScrollController::updateVerticalScrollBar() {
    updateVerticalScrollBarPrivate();
    emit si_visibleAreaChanged();
}

void ScrollController::scrollToViewRow(int viewRowIndex, int widgetHeight) {
    const U2Region rowRegion = ui->getRowHeightController()->getGlobalYRegionByViewRowIndex(viewRowIndex);
    const U2Region visibleRegion = getVerticalRangeToDrawIn(widgetHeight);
    if (rowRegion.startPos < visibleRegion.startPos) {
        vScrollBar->setValue(static_cast<int>(rowRegion.startPos));
    } else if (rowRegion.endPos() >= visibleRegion.endPos()) {
        if (rowRegion.length > visibleRegion.length) {
            vScrollBar->setValue(static_cast<int>(rowRegion.startPos));
        } else if (rowRegion.startPos > visibleRegion.startPos) {
            vScrollBar->setValue(static_cast<int>(rowRegion.endPos() - widgetHeight));
        }
    }
}

void ScrollController::scrollToBase(int baseNumber, int widgetWidth) {
    const U2Region baseRange = U2Region(ui->getBaseWidthController()->getBaseGlobalOffset(baseNumber), maEditor->getColumnWidth());
    const U2Region visibleRange = getHorizontalRangeToDrawIn(widgetWidth);
    if (baseRange.startPos < visibleRange.startPos) {
        hScrollBar->setValue(static_cast<int>(baseRange.startPos));
    } else if (baseRange.endPos() >= visibleRange.endPos()) {
        hScrollBar->setValue(static_cast<int>(baseRange.endPos() - widgetWidth));
    }
}

void ScrollController::scrollToPoint(const QPoint& maPoint, const QSize& screenSize) {
    scrollToBase(maPoint.x(), screenSize.width());
    scrollToViewRow(maPoint.y(), screenSize.height());
}

int ScrollController::getHorizontalScrollBarValueWithBaseCentered(int baseIndex, int widgetWidth) const {
    U2Region baseGlobalRange = ui->getBaseWidthController()->getBaseGlobalRange(baseIndex);
    U2Region visibleRange = getHorizontalRangeToDrawIn(widgetWidth);
    return qBound(0, (int)(baseGlobalRange.startPos - visibleRange.length / 2), hScrollBar->maximum());
}

bool ScrollController::isBaseCentered(int baseIndex, int widgetWidth) const {
    int newScreenXOffset = getHorizontalScrollBarValueWithBaseCentered(baseIndex, widgetWidth);
    return newScreenXOffset == hScrollBar->value();
}

void ScrollController::centerBase(int baseIndex, int widgetWidth) {
    int newScreenXOffset = getHorizontalScrollBarValueWithBaseCentered(baseIndex, widgetWidth);
    hScrollBar->setValue(newScreenXOffset);
}

void ScrollController::centerViewRow(int viewRowIndex, int widgetHeight) {
    const U2Region rowGlobalRange = ui->getRowHeightController()->getGlobalYRegionByViewRowIndex(viewRowIndex);
    const U2Region visibleRange = getVerticalRangeToDrawIn(widgetHeight);
    const int newScreenYOffset = rowGlobalRange.startPos - visibleRange.length / 2;
    vScrollBar->setValue(newScreenYOffset);
}

void ScrollController::centerPoint(const QPoint& maPoint, const QSize& widgetSize) {
    centerBase(maPoint.x(), widgetSize.width());
    centerViewRow(maPoint.y(), widgetSize.height());
}

void ScrollController::setHScrollbarValue(int value) {
    hScrollBar->setValue(value);
}

void ScrollController::setVScrollbarValue(int value) {
    vScrollBar->setValue(value);
}

void ScrollController::setFirstVisibleBase(int firstVisibleBase) {
    hScrollBar->setValue(ui->getBaseWidthController()->getBaseGlobalOffset(firstVisibleBase));
}

void ScrollController::setFirstVisibleViewRow(int viewRowIndex) {
    int y = ui->getRowHeightController()->getGlobalYRegionByViewRowIndex(viewRowIndex).startPos;
    vScrollBar->setValue(y);
}

void ScrollController::setFirstVisibleMaRow(int maRowIndex) {
    int y = ui->getRowHeightController()->getGlobalYPositionByMaRowIndex(maRowIndex);
    vScrollBar->setValue(y);
}

void ScrollController::scrollSmoothly(const Directions& directions) {
    QAbstractSlider::SliderAction horizontalAction = QAbstractSlider::SliderNoAction;
    QAbstractSlider::SliderAction verticalAction = QAbstractSlider::SliderNoAction;

    if (directions.testFlag(Up)) {
        verticalAction = QAbstractSlider::SliderSingleStepSub;
    }
    if (directions.testFlag(Down)) {
        verticalAction = QAbstractSlider::SliderSingleStepAdd;
    }

    if (directions.testFlag(Left)) {
        horizontalAction = QAbstractSlider::SliderSingleStepSub;
    }
    if (directions.testFlag(Right)) {
        horizontalAction = QAbstractSlider::SliderSingleStepAdd;
    }

    if (verticalAction != vScrollBar->getRepeatAction()) {
        vScrollBar->setupRepeatAction(verticalAction, 500, 50);
    }

    if (horizontalAction != hScrollBar->getRepeatAction()) {
        hScrollBar->setupRepeatAction(horizontalAction, 500, 50);
    }
}

void ScrollController::stopSmoothScrolling() {
    hScrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
    vScrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
}

void ScrollController::scrollStep(ScrollController::Direction direction) {
    switch (direction) {
        case Up:
            vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
            break;
        case Down:
            vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
            break;
        case Left:
            hScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
            break;
        case Right:
            hScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
            break;
        default:
            FAIL("An unknown direction", );
            break;
    }
}

void ScrollController::scrollPage(ScrollController::Direction direction) {
    switch (direction) {
        case Up:
            vScrollBar->triggerAction(QAbstractSlider::SliderPageStepSub);
            break;
        case Down:
            vScrollBar->triggerAction(QAbstractSlider::SliderPageStepAdd);
            break;
        case Left:
            hScrollBar->triggerAction(QAbstractSlider::SliderPageStepSub);
            break;
        case Right:
            hScrollBar->triggerAction(QAbstractSlider::SliderPageStepAdd);
            break;
        default:
            FAIL("An unknown direction", );
            break;
    }
}

void ScrollController::scrollToEnd(ScrollController::Direction direction) {
    switch (direction) {
        case Up:
            vScrollBar->triggerAction(QAbstractSlider::SliderToMinimum);
            break;
        case Down:
            vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);
            break;
        case Left:
            hScrollBar->triggerAction(QAbstractSlider::SliderToMinimum);
            break;
        case Right:
            hScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);
            break;
        default:
            FAIL("An unknown direction", );
            break;
    }
}

void ScrollController::scrollToMovedSelection(int deltaX, int deltaY) {
    const Direction direction = (deltaX != 0 ? (deltaX < 0 ? ScrollController::Left : ScrollController::Right) : (deltaY != 0 ? (deltaY < 0 ? ScrollController::Up : ScrollController::Down) : ScrollController::None));
    scrollToMovedSelection(direction);
}

void ScrollController::scrollToMovedSelection(ScrollController::Direction direction) {
    U2Region fullyVisibleRegion;
    U2Region selectionRegion;
    int selectionEdgePosition;
    QSize widgetSize = ui->getSequenceArea()->size();
    QRect selectionRect = ui->getEditor()->getSelection().toRect();
    switch (direction) {
        case Up:
            fullyVisibleRegion = ui->getDrawHelper()->getVisibleViewRowsRegion(widgetSize.height(), false, false);
            selectionRegion = U2Region(selectionRect.y(), selectionRect.height());
            selectionEdgePosition = static_cast<int>(selectionRegion.startPos);
            break;
        case Down:
            fullyVisibleRegion = ui->getDrawHelper()->getVisibleViewRowsRegion(widgetSize.height(), false, false);
            selectionRegion = U2Region(selectionRect.y(), selectionRect.height());
            selectionEdgePosition = static_cast<int>(selectionRegion.endPos() - 1);
            break;
        case Left:
            fullyVisibleRegion = ui->getDrawHelper()->getVisibleBases(widgetSize.width(), false, false);
            selectionRegion = U2Region(selectionRect.x(), selectionRect.width());
            selectionEdgePosition = static_cast<int>(selectionRegion.startPos);
            break;
        case Right:
            fullyVisibleRegion = ui->getDrawHelper()->getVisibleBases(widgetSize.width(), false, false);
            selectionRegion = U2Region(selectionRect.x(), selectionRect.width());
            selectionEdgePosition = static_cast<int>(selectionRegion.endPos() - 1);
            break;
        case None:
            return;
        default:
            FAIL("An unknown direction", );
            break;
    }

    const bool selectionEdgeIsFullyVisible = fullyVisibleRegion.contains(selectionEdgePosition);
    if (!selectionEdgeIsFullyVisible) {
        switch (direction) {
            case Up:
            case Down:
                scrollToViewRow(static_cast<int>(selectionEdgePosition), widgetSize.height());
                break;
            case Left:
            case Right:
                scrollToBase(static_cast<int>(selectionEdgePosition), widgetSize.width());
                break;
            case None:
                return;
            default:
                FAIL("An unknown direction", );
                break;
        }
    }
}

int ScrollController::getFirstVisibleBase(bool countClipped) const {
    int alignmentLength = maEditor->getAlignmentLen();
    CHECK(alignmentLength > 0, 0);
    int additionalXOffset = getAdditionalXOffset();
    bool removeClippedBase = !countClipped && additionalXOffset != 0;
    int hScrollBarValue = hScrollBar->value();
    int column = ui->getBaseWidthController()->globalXPositionToColumn(hScrollBarValue);
    int firstVisibleBase = column + (removeClippedBase && additionalXOffset != 0 ? 1 : 0);

    // The following comment is a fix for UGENE-7605
    // SAFE_POINT(firstVisibleBase < alignmentLength, "Invalid first visible base: " + QString::number(firstVisibleBase), 0);
    return qMin(firstVisibleBase, alignmentLength - 1);
}

int ScrollController::getLastVisibleBase(int widgetWidth, bool countClipped) const {
    const bool removeClippedBase = !countClipped && ((hScrollBar->value() + widgetWidth) % maEditor->getColumnWidth() != 0);
    const int lastVisibleBase = ui->getBaseWidthController()->globalXPositionToColumn(hScrollBar->value() + widgetWidth - 1) - (removeClippedBase ? 1 : 0);
    return qMin(lastVisibleBase, maEditor->getAlignmentLen() - 1);
}

int ScrollController::getFirstVisibleMaRowIndex(bool countClipped) const {
    const bool removeClippedRow = !(countClipped || getAdditionalYOffset() == 0);
    return ui->getRowHeightController()->getMaRowIndexByGlobalYPosition(vScrollBar->value()) + (removeClippedRow ? 1 : 0);
}

int ScrollController::getFirstVisibleViewRowIndex(bool countClipped) const {
    int maRowIndex = getFirstVisibleMaRowIndex(countClipped);
    return maEditor->getCollapseModel()->getViewRowIndexByMaRowIndex(maRowIndex);
}

int ScrollController::getLastVisibleViewRowIndex(int widgetHeight, bool countClipped) const {
    int lastVisibleViewRow = ui->getRowHeightController()->getViewRowIndexByGlobalYPosition(vScrollBar->value() + widgetHeight);
    if (lastVisibleViewRow < 0) {
        lastVisibleViewRow = maEditor->getCollapseModel()->getViewRowCount() - 1;
    }
    U2Region lastRowScreenRegion = ui->getRowHeightController()->getScreenYRegionByViewRowIndex(lastVisibleViewRow);
    bool removeClippedRow = !countClipped && lastRowScreenRegion.endPos() > widgetHeight;
    return lastVisibleViewRow - (removeClippedRow ? 1 : 0);
}

GScrollBar* ScrollController::getHorizontalScrollBar() const {
    return hScrollBar;
}

GScrollBar* ScrollController::getVerticalScrollBar() const {
    return vScrollBar;
}

void ScrollController::updateScrollBarsOnFontOrZoomChange() {
    CHECK(!maEditor->isAlignmentEmpty(), );
    QSignalBlocker signalBlocker(hScrollBar);

    // Keep the top-left point in place while zooming,
    // so when zooming in an just opened alignment the start position is always visible.
    double sequenceAreaWidth = ui->getSequenceArea()->width();
    double leftX = hScrollBar->value();
    double alignmentLength = maEditor->getAlignmentLen();
    double maxX = hScrollBar->maximum() + sequenceAreaWidth;
    double leftXPointPos = alignmentLength * leftX / (double)maxX;

    int baseWidth = (hScrollBar->maximum() + sequenceAreaWidth) / alignmentLength;
    sequenceAreaWidth = ui->getSequenceArea()->width() - ui->getSequenceArea()->width() % (int)baseWidth;
    maxX = hScrollBar->maximum() + sequenceAreaWidth;
    leftXPointPos = alignmentLength * leftX / (double)maxX;
    updateHorizontalScrollBarPrivate();
    setFirstVisibleBase(qMax(0, (int)leftXPointPos));

    double sequenceAreaHeight = ui->getSequenceArea()->height();
    double topY = vScrollBar->value();
    double numSequences = maEditor->getNumSequences();
    double maxYPoint = vScrollBar->maximum() + sequenceAreaHeight;
    double topColumnIndex = numSequences * topY / (double)maxYPoint;
    updateVerticalScrollBarPrivate();
    setFirstVisibleViewRow(qMax(0, (int)topColumnIndex));

    emit si_visibleAreaChanged();
}

void ScrollController::sl_updateScrollBars() {
    updateHorizontalScrollBarPrivate();
    updateVerticalScrollBarPrivate();
    emit si_visibleAreaChanged();
}

void ScrollController::sl_collapsibleModelIsAboutToBeChanged() {
    savedFirstVisibleMaRow = getFirstVisibleMaRowIndex(true);
    savedFirstVisibleMaRowOffset = getScreenPosition().y() -
                                   ui->getRowHeightController()->getGlobalYPositionByMaRowIndex(savedFirstVisibleMaRow);
}

void ScrollController::sl_collapsibleModelChanged() {
    int firstVisibleMaRowOffset = ui->getRowHeightController()->getGlobalYPositionByMaRowIndex(savedFirstVisibleMaRow);
    setVScrollbarValue(firstVisibleMaRowOffset + savedFirstVisibleMaRowOffset);
    updateVerticalScrollBar();
}

int ScrollController::getAdditionalXOffset() const {
    return hScrollBar->value() % maEditor->getColumnWidth();
}

int ScrollController::getAdditionalYOffset() const {
    int maRow = ui->getRowHeightController()->getMaRowIndexByGlobalYPosition(vScrollBar->value());
    int viewRow = ui->getRowHeightController()->getGlobalYPositionByMaRowIndex(maRow);
    return vScrollBar->value() - viewRow;
}

U2Region ScrollController::getHorizontalRangeToDrawIn(int widgetWidth) const {
    return U2Region(hScrollBar->value(), widgetWidth);
}

U2Region ScrollController::getVerticalRangeToDrawIn(int widgetHeight) const {
    return U2Region(vScrollBar->value(), widgetHeight);
}

void ScrollController::updateHorizontalScrollBarPrivate() {
    SAFE_POINT(nullptr != hScrollBar, "Horizontal scrollbar is not initialized", );
    QSignalBlocker signalBlocker(hScrollBar);

    maEditor->multilineViewAction->setEnabled(!maEditor->isAlignmentEmpty() || maEditor->getMultilineMode());
    CHECK_EXT(!maEditor->isAlignmentEmpty(), hScrollBar->setVisible(false), );

    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceArea()->width() - ui->getSequenceArea()->width() % columnWidth;

    maEditor->multilineViewAction->setEnabled(hScrollBar->maximum() > sequenceAreaWidth || maEditor->getMultilineMode());

    hScrollBar->setMinimum(0);
    int hScrollBarMax = qMax(0, alignmentLength * columnWidth - sequenceAreaWidth);
    hScrollBar->setMaximum(hScrollBarMax);
    hScrollBar->setSingleStep(columnWidth);
    hScrollBar->setPageStep(sequenceAreaWidth);

    int numVisibleBases = getLastVisibleBase(sequenceAreaWidth) - getFirstVisibleBase();
    SAFE_POINT(numVisibleBases <= alignmentLength, "Horizontal scrollbar appears unexpectedly: numVisibleBases is too small", );

    // hide scrollbar
    hScrollBar->setVisible(hScrollBarVisible && numVisibleBases < alignmentLength);
}

void ScrollController::updateVerticalScrollBarPrivate() {
    SAFE_POINT(vScrollBar != nullptr, "Vertical scrollbar is not initialized", );
    QSignalBlocker signalBlocker(vScrollBar);

    CHECK_EXT(!maEditor->isAlignmentEmpty(), vScrollBar->setVisible(false), );

    const int viewRowCount = ui->getSequenceArea()->getViewRowCount();
    const int sequenceAreaHeight = ui->getSequenceArea()->height();
    const int totalAlignmentHeight = ui->getRowHeightController()->getTotalAlignmentHeight();

    vScrollBar->setMinimum(0);
    vScrollBar->setMaximum(qMax(0, totalAlignmentHeight - sequenceAreaHeight));
    vScrollBar->setSingleStep(ui->getRowHeightController()->getSingleRowHeight());
    vScrollBar->setPageStep(sequenceAreaHeight);

    int firstVisibleViewRowIndex = getFirstVisibleViewRowIndex();
    int lastVisibleViewRowIndex = getLastVisibleViewRowIndex(sequenceAreaHeight);
    int numVisibleSequences = lastVisibleViewRowIndex - firstVisibleViewRowIndex + 1;
    SAFE_POINT(numVisibleSequences <= viewRowCount, "Vertical scrollbar appears unexpectedly: numVisibleSequences is too small", );

    // hide scrollbar
    vScrollBar->setVisible(vScrollBarVisible && numVisibleSequences < viewRowCount);
}

QPoint ScrollController::getViewPosByScreenPoint(const QPoint& point, bool reportOverflow) const {
    int column = ui->getBaseWidthController()->screenXPositionToColumn(point.x());
    int row = ui->getRowHeightController()->getViewRowIndexByScreenYPosition(point.y());
    QPoint result(column, row);
    if (ui->getSequenceArea()->isInRange(result)) {
        return result;
    }
    if (reportOverflow) {
        row = row == -1 && point.y() > 0 ? ui->getSequenceArea()->getViewRowCount() : row;
        column = qMin(column, maEditor->getAlignmentLen());
        return QPoint(column, row);
    }
    return QPoint(-1, -1);
}


void ScrollController::setHScrollBarVisible(bool visible) {
    hScrollBarVisible = visible;
}

bool ScrollController::getHScrollBarVisible() {
    return hScrollBarVisible;
}

void ScrollController::setVScrollBarVisible(bool visible) {
    vScrollBarVisible = visible;
}

bool ScrollController::getVScrollBarVisible() {
    return vScrollBarVisible;
}

}  // namespace U2
