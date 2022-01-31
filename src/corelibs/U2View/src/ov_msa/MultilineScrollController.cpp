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

#include "MultilineScrollController.h"
#include "ScrollController.h"

#include "U2Core/U2SafePoints.h"
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/SignalBlocker.h>

#include "BaseWidthController.h"
#include "DrawHelper.h"
#include "RowHeightController.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorNameList.h"
#include "ov_msa/MaEditorMultilineWgt.h"
#include "ov_msa/MSAEditorOverviewArea.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MaEditorWgt.h"

namespace U2 {

MultilineScrollController::MultilineScrollController(MaEditor *maEditor, MaEditorMultilineWgt *maEditorUi)
    : QObject(maEditorUi),
      maEditor(maEditor),
      ui(maEditorUi),
      savedFirstVisibleMaRow(0),
      savedFirstVisibleMaRowOffset(0) {
    // TODO: need change horz scrollbar
    connect(this, SIGNAL(si_hScrollValueChanged()), SLOT(sl_hScrollValueChanged()));
    connect(this, SIGNAL(si_vScrollValueChanged()), SLOT(sl_vScrollValueChanged()));
}

void MultilineScrollController::init(GScrollBar *hScrollBar, GScrollBar *vScrollBar) {
    this->hScrollBar = hScrollBar;
    // TODO: need change horz scrollbar
    connect(hScrollBar, SIGNAL(valueChanged(int)), SIGNAL(si_hScrollValueChanged()));
    hScrollBar->setValue(0);

    this->vScrollBar = vScrollBar;
    connect(vScrollBar, SIGNAL(valueChanged(int)), SIGNAL(si_vScrollValueChanged()));
    vScrollBar->setValue(0);

    sl_updateScrollBars();
}

void MultilineScrollController::sl_hScrollValueChanged()
{
    QSignalBlocker signalBlocker(this);

    int h = hScrollBar->value();

    const int columnWidth = maEditor->getColumnWidth();

    if (ui->getMultilineMode()) {
        setMultilineVScrollbarValue(h);
    }
    for (uint i = 0; i < ui->getChildrenCount(); i++) {
        ui->getUI(i)->getScrollController()->setHScrollbarValue(h);
        h += ui->getSequenceAreaBaseWidth(i) * columnWidth;
    }
    ui->getOverviewArea()->update();
}

void MultilineScrollController::sl_vScrollValueChanged()
{
    if (ui->getMultilineMode()) {
        int v = vScrollBar->value();
        setMultilineHScrollbarValue(v);
    } else {
        int v = vScrollBar->value();
        ui->getUI(0)->getScrollController()->setHScrollbarValue(v);
    }
}

void MultilineScrollController::scrollToViewRow(int viewRowIndex, int widgetHeight) {
    const U2Region rowRegion = ui->getUI(0)->getRowHeightController()->getGlobalYRegionByViewRowIndex(viewRowIndex);
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

void MultilineScrollController::scrollToBase(int baseNumber, int widgetWidth) {
    const U2Region baseRange = U2Region(ui->getUI(0)->getBaseWidthController()->getBaseGlobalOffset(baseNumber), maEditor->getColumnWidth());
    const U2Region visibleRange = getHorizontalRangeToDrawIn(widgetWidth);
    if (baseRange.startPos < visibleRange.startPos) {
        hScrollBar->setValue(static_cast<int>(baseRange.startPos));
    } else if (baseRange.endPos() >= visibleRange.endPos()) {
        hScrollBar->setValue(static_cast<int>(baseRange.endPos() - widgetWidth));
    }
}

void MultilineScrollController::scrollToPoint(const QPoint &maPoint, const QSize &screenSize) {
    scrollToBase(maPoint.x(), screenSize.width());
    scrollToViewRow(maPoint.y(), screenSize.height());
}

void MultilineScrollController::centerBase(int baseNumber, int widgetWidth) {
    const U2Region baseGlobalRange = ui->getUI(0)->getBaseWidthController()->getBaseGlobalRange(baseNumber);
    const U2Region visibleRange = getHorizontalRangeToDrawIn(widgetWidth);
    const int newScreenXOffset = baseGlobalRange.startPos - visibleRange.length / 2;
    hScrollBar->setValue(newScreenXOffset);
}

void MultilineScrollController::centerViewRow(int viewRowIndex, int widgetHeight) {
    const U2Region rowGlobalRange = ui->getUI(0)->getRowHeightController()->getGlobalYRegionByViewRowIndex(viewRowIndex);
    const U2Region visibleRange = getVerticalRangeToDrawIn(widgetHeight);
    const int newScreenYOffset = rowGlobalRange.startPos - visibleRange.length / 2;
    vScrollBar->setValue(newScreenYOffset);
}

void MultilineScrollController::centerPoint(const QPoint &maPoint, const QSize &widgetSize) {
    centerBase(maPoint.x(), widgetSize.width());
    centerViewRow(maPoint.y(), widgetSize.height());
}

void MultilineScrollController::setMultilineHScrollbarValue(int value) {
    hScrollBar->setValue(value);
}

void MultilineScrollController::setMultilineVScrollbarValue(int value) {
    vScrollBar->setValue(value);
}

void MultilineScrollController::setFirstVisibleBase(int firstVisibleBase) {
    if (!ui->getMultilineMode()) {
        ui->getUI(0)->getScrollController()->setFirstVisibleBase(firstVisibleBase);
    } else {
        int x = ui->getUI(0)->getBaseWidthController()->getBaseGlobalOffset(firstVisibleBase);
        hScrollBar->setValue(x);
    }
}

void MultilineScrollController::setFirstVisibleViewRow(int viewRowIndex) {
    if (!ui->getMultilineMode()) {
        ui->getUI(0)->getScrollController()->setFirstVisibleViewRow(viewRowIndex);
    }
}

void MultilineScrollController::setFirstVisibleMaRow(int maRowIndex) {
    if (!ui->getMultilineMode()) {
        ui->getUI(0)->getScrollController()->setFirstVisibleMaRow(maRowIndex);
    }
}

void MultilineScrollController::scrollSmoothly(const Directions &directions) {
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

void MultilineScrollController::stopSmoothScrolling() {
    hScrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
    vScrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
}

void MultilineScrollController::scrollStep(MultilineScrollController::Direction direction) {
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

void MultilineScrollController::scrollPage(MultilineScrollController::Direction direction) {
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

void MultilineScrollController::scrollToEnd(MultilineScrollController::Direction direction) {
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

int MultilineScrollController::getFirstVisibleBase(bool countClipped) const {
    if (maEditor->getAlignmentLen() == 0) {
        return 0;
    }
    const bool removeClippedBase = !countClipped && (getAdditionalXOffset() != 0);
    const int firstVisibleBase = ui->getMultilineMode()
                                     ? ui->getUI(0)->getBaseWidthController()->globalXPositionToColumn(hScrollBar->value()) + (removeClippedBase ? 1 : 0)
                                     : ui->getUI(0)->getScrollController()->getFirstVisibleBase(countClipped);
    assert(firstVisibleBase < maEditor->getAlignmentLen());
    return qMin(firstVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getLastVisibleBase(int widgetWidth, bool countClipped) const {
    const bool removeClippedBase = !countClipped && ((hScrollBar->value() + widgetWidth) % maEditor->getColumnWidth() != 0);
    const int lastVisibleBase = ui->getMultilineMode()
                                    ? ui->getUI(0)->getBaseWidthController()->globalXPositionToColumn(hScrollBar->value() + widgetWidth - 1) - (removeClippedBase ? 1 : 0)
                                    : ui->getUI(0)->getScrollController()->getLastVisibleBase(widgetWidth, countClipped);
    return qMin(lastVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getFirstVisibleMaRowIndex(bool countClipped) const {
    const bool removeClippedRow = !(countClipped || getAdditionalYOffset() == 0);
    return ui->getMultilineMode()
               ? ui->getUI(0)->getRowHeightController()->getMaRowIndexByGlobalYPosition(vScrollBar->value()) + (removeClippedRow ? 1 : 0)
               : ui->getUI(0)->getScrollController()->getFirstVisibleMaRowIndex(countClipped);
}

int MultilineScrollController::getFirstVisibleViewRowIndex(bool countClipped) const {
    int maRowIndex = getFirstVisibleMaRowIndex(countClipped);
    return maEditor->getCollapseModel()->getViewRowIndexByMaRowIndex(maRowIndex);
}

int MultilineScrollController::getLastVisibleViewRowIndex(int widgetHeight, bool countClipped) const {
    int lastVisibleViewRow = ui->getUI(0)->getRowHeightController()->getViewRowIndexByGlobalYPosition(ui->getMultilineMode() ? vScrollBar->value() + widgetHeight : ui->getUI(0)->getScrollController()->getLastVisibleViewRowIndex(widgetHeight, countClipped));
    if (lastVisibleViewRow < 0) {
        lastVisibleViewRow = maEditor->getCollapseModel()->getViewRowCount() - 1;
    }
    U2Region lastRowScreenRegion = ui->getUI(0)->getRowHeightController()->getScreenYRegionByViewRowIndex(lastVisibleViewRow);
    bool removeClippedRow = !countClipped && lastRowScreenRegion.endPos() > widgetHeight;
    return lastVisibleViewRow - (removeClippedRow ? 1 : 0);
}

GScrollBar *MultilineScrollController::getHorizontalScrollBar() const {
    return hScrollBar;
}

GScrollBar *MultilineScrollController::getVerticalScrollBar() const {
    return vScrollBar;
}

void MultilineScrollController::sl_zoomScrollBars() {
    zoomHorizontalScrollBarPrivate();
    zoomVerticalScrollBarPrivate();
    emit si_visibleAreaChanged();
}

void MultilineScrollController::sl_updateScrollBars() {
    updateHorizontalScrollBarPrivate();
    updateVerticalScrollBarPrivate();
    emit si_visibleAreaChanged();
}

int MultilineScrollController::getAdditionalXOffset() const {
    return hScrollBar->value() % maEditor->getColumnWidth();
}

int MultilineScrollController::getAdditionalYOffset() const {
    int maRow = ui->getUI(0)->getRowHeightController()->getMaRowIndexByGlobalYPosition(vScrollBar->value());
    int viewRow = ui->getUI(0)->getRowHeightController()->getGlobalYPositionByMaRowIndex(maRow);
    return vScrollBar->value() - viewRow;
}

U2Region MultilineScrollController::getHorizontalRangeToDrawIn(int widgetWidth) const {
    return U2Region(hScrollBar->value(), widgetWidth);
}

U2Region MultilineScrollController::getVerticalRangeToDrawIn(int widgetHeight) const {
    return U2Region(vScrollBar->value(), widgetHeight);
}

void MultilineScrollController::zoomHorizontalScrollBarPrivate() {
}

void MultilineScrollController::zoomVerticalScrollBarPrivate() {
}

void MultilineScrollController::updateHorizontalScrollBarPrivate() {
    SAFE_POINT(nullptr != hScrollBar, "Horizontal scrollbar is not initialized", );
    SignalBlocker signalBlocker(hScrollBar);
    Q_UNUSED(signalBlocker);

    CHECK_EXT(!maEditor->isAlignmentEmpty(), hScrollBar->setVisible(false), );

    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth() * columnWidth;

    QSplitter *splitter = qobject_cast<QSplitter *>(hScrollBar->parent());
    if (splitter != nullptr) {
        if (ui->getMultilineMode()) {
            const int vScrollBarWidth = vScrollBar->width();
            const int nameListWidth = ui->getUI(ui->getChildrenCount() - 1)->getEditorNameList()->width() + 2;
            splitter->setVisible(true);
            splitter->setFixedHeight(vScrollBarWidth);
            splitter->setSizes({nameListWidth, sequenceAreaWidth, vScrollBarWidth});
        } else {
            splitter->setVisible(false);
        }
    }
    hScrollBar->setMinimum(0);
    int allWidth = 0;
    for (uint i = 0; i < ui->getChildrenCount(); i++) {
        allWidth += ui->getSequenceAreaBaseWidth(i) * columnWidth;
    }
    hScrollBar->setMaximum(qMax(0, alignmentLength * columnWidth - allWidth));
    hScrollBar->setSingleStep(columnWidth);
    hScrollBar->setPageStep(sequenceAreaWidth);

    // don't show horz scrollbar
    hScrollBar->setVisible(ui->getMultilineMode());
}

void MultilineScrollController::updateVerticalScrollBarPrivate() {
    SAFE_POINT(nullptr != vScrollBar, "Multiline Vertical scrollbar is not initialized", );
    SignalBlocker signalBlocker(vScrollBar);
    Q_UNUSED(signalBlocker);

    CHECK_EXT(!maEditor->isAlignmentEmpty(), vScrollBar->setVisible(false), );

    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth() * columnWidth;

    vScrollBar->setMinimum(0);
    int allWidth = 0;
    for (uint i = 0; i < ui->getChildrenCount(); i++) {
        allWidth += ui->getSequenceAreaBaseWidth(i) * columnWidth;
    }
    vScrollBar->setMaximum(qMax(0, alignmentLength * columnWidth - allWidth));
    vScrollBar->setSingleStep(columnWidth);
    vScrollBar->setPageStep(sequenceAreaWidth);

    // don't show vert scrollbar in non-multiline mode
    vScrollBar->setVisible(ui->getMultilineMode());
}

}  // namespace U2
