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

#include <QWheelEvent>

#include "U2Core/U2SafePoints.h"
#include <U2Core/MultipleAlignmentObject.h>

#include "BaseWidthController.h"
#include "DrawHelper.h"
#include "RowHeightController.h"
#include "ScrollController.h"
#include "ov_msa/MSAEditorOverviewArea.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorMultilineWgt.h"
#include "ov_msa/MaEditorNameList.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MaEditorWgt.h"
#include "ov_msa/MsaMultilineScrollArea.h"

namespace U2 {

MultilineScrollController::MultilineScrollController(MaEditor* maEditor, MaEditorMultilineWgt* maEditorUi)
    : QObject(maEditorUi),
      maEditor(maEditor),
      ui(maEditorUi),
      savedFirstVisibleMaRow(0),
      savedFirstVisibleMaRowOffset(0) {
}

void MultilineScrollController::init(GScrollBar* hScrollBar,
                                     GScrollBar* vScrollBar,
                                     QScrollArea* childrenArea) {
    this->childrenScrollArea = childrenArea;

    this->hScrollBar = hScrollBar;
    hScrollBar->setValue(0);
    hScrollBar->setSingleStep(maEditor->getColumnWidth());
    hScrollBar->setPageStep(childrenScrollArea->width());

    this->vScrollBar = vScrollBar;
    vScrollBar->setValue(0);
    vScrollBar->setSingleStep(maEditor->getRowHeight());
    vScrollBar->setPageStep(childrenScrollArea->height());
    vScrollBar->installEventFilter(this);

    setEnable(enabled);
    sl_updateScrollBars();
}
void MultilineScrollController::initSignals(bool enable) {
    if (enable) {
        // TODO:ichebyki
        //connect(this, SIGNAL(si_hScrollValueChanged()), SLOT(sl_hScrollValueChanged()));
        //connect(this, SIGNAL(si_vScrollValueChanged()), SLOT(sl_vScrollValueChanged()));
        if (!connAreaChanged)
            connAreaChanged = connect(this, SIGNAL(si_visibleAreaChanged()), this, SLOT(sl_updateScrollBars()));
        // TODO:ichebyki
        //connect(hScrollBar, SIGNAL(valueChanged(int)), SIGNAL(si_hScrollValueChanged()));
        if (!connHValueChanged)
            connHValueChanged = connect(hScrollBar, SIGNAL(valueChanged(int)), this, SLOT(sl_hScrollValueChanged()));
        if (!connHActionTriggered)
            connHActionTriggered = connect(hScrollBar, SIGNAL(actionTriggered(int)), this, SLOT(sl_handleHScrollAction(int)));
        // TODO:ichebyki
        //connect(vScrollBar, SIGNAL(valueChanged(int)), SIGNAL(si_vScrollValueChanged()));
        if (!connVValueChanged)
            connVValueChanged = connect(vScrollBar, SIGNAL(valueChanged(int)), this, SLOT(sl_vScrollValueChanged()));
        if (!connVActionTriggered)
            connVActionTriggered = connect(vScrollBar, SIGNAL(actionTriggered(int)), this, SLOT(sl_handleVScrollAction(int)));
    } else {
        if (connAreaChanged)
            disconnect(connAreaChanged);
        if (connHValueChanged)
            disconnect(connHValueChanged);
        if (connHActionTriggered)
            disconnect(connHActionTriggered);
        if (connVValueChanged)
            disconnect(connVValueChanged);
        if (connVActionTriggered)
            disconnect(connVActionTriggered);
    }
}

void MultilineScrollController::setEnable(bool enable) {
    if (enable != enabled) {
        enabled = enable;
    }
    initSignals(enabled);
}

// TODO: new vertical scrollbar mode
bool MultilineScrollController::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::Wheel && maEditor->getMultilineMode()) {
        if (object == vScrollBar) {
            //return vertEventFilter((QWheelEvent *) event);
            return true;
        }
    }

    return false;
}

bool MultilineScrollController::vertEventFilter(QWheelEvent* event) {
    if (maEditor->getMultilineMode()) {
        int inverted = event->inverted() ? -1 : 1;
        int direction = event->angleDelta().isNull()
                            ? 0
                            : event->angleDelta().y() == 0
                                  ? 0
                                  : inverted * (event->angleDelta().y() > 0 ? 1 : -1);

        if (direction == 0) {
            return false;
        } else if (direction < 0) {
            sl_handleVScrollAction(QAbstractSlider::SliderSingleStepAdd);
        } else if (direction > 0) {
            sl_handleVScrollAction(QAbstractSlider::SliderSingleStepSub);
        }
        return true;  // filter the original event out
    }
    return false;  // pass other events
}

void MultilineScrollController::sl_handleHScrollAction(int action) {
    Q_UNUSED(action);
    ui->setUpdatesEnabled(false);
    ui->updateChildrenCount();
    GScrollBar* hBar = ui->getScrollController()->getHorizontalScrollBar();
    GScrollBar* vBar = ui->getScrollController()->getVerticalScrollBar();

    vBar->setSliderPosition((int)((double)hBar->sliderPosition() * (double)vBar->maximum() / (double)hBar->maximum()));

    ui->setUpdatesEnabled(true);
}

void MultilineScrollController::sl_handleVScrollAction(int action) {
    if (action == QAbstractSlider::SliderSingleStepSub) {
        vertScroll(Up, true);
    } else if (action == QAbstractSlider::SliderPageStepSub) {
        vertScroll(Up, false);
    } else if (action == QAbstractSlider::SliderSingleStepAdd) {
        vertScroll(Down, true);
    } else if (action == QAbstractSlider::SliderPageStepAdd) {
        vertScroll(Down, false);
    } else if (action == QAbstractSlider::SliderToMaximum) {
        vertScroll(SliderMaximum, false);
    } else if (action == QAbstractSlider::SliderToMinimum) {
        vertScroll(SliderMinimum, false);
    } else if (action == QAbstractSlider::SliderMove) {
        vertScroll(SliderMoved, false);
    }
}

void MultilineScrollController::vertScroll(const Directions& directions, bool byStep) {
    ui->setUpdatesEnabled(false);
    ui->updateChildrenCount();

    if (maEditor->getMultilineMode()) {
        MsaMultilineScrollArea* scroller = qobject_cast<MsaMultilineScrollArea*>(
            childrenScrollArea);
        CHECK(scroller != nullptr, );
        scroller->scrollVert(directions, byStep);
    }

    ui->setUpdatesEnabled(true);
}

int MultilineScrollController::getViewHeight() {
    return childrenScrollArea->height();
}

void MultilineScrollController::sl_hScrollValueChanged() {
    QSignalBlocker signalBlocker(this);
    ui->setUpdatesEnabled(false);

    int h = hScrollBar->value();
    for (uint i = 0; i < ui->getChildrenCount(); i++) {
        ui->getUI(i)->getScrollController()->setHScrollbarValue(h);
        h += ui->getSequenceAreaBaseWidth(i);
    }

    ui->setUpdatesEnabled(true);
    hScrollBar->update();
    ui->getOverviewArea()->update();
}

void MultilineScrollController::sl_vScrollValueChanged() {
    if (maEditor->getMultilineMode()) {
        // TODO:ichebyki
        //int v = vScrollBar->value();
        //setMultilineHScrollbarValue(v);
    } else {
        int v = vScrollBar->value();
        ui->getUI(0)->getScrollController()->setHScrollbarValue(v);
    }
}

void MultilineScrollController::scrollToViewRow(int viewRowIndex, int widgetHeight) {
    const U2Region rowRegion = ui->getUI(0)->getRowHeightController()->getGlobalYRegionByViewRowIndex(viewRowIndex);
    const U2Region visibleRegion = getVerticalRangeToDrawIn(widgetHeight);
    // TODO:ichebyki auto xxx = ui->getUI(0)->mapToParent(ui->getUI(0)->rect().topLeft());
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

void MultilineScrollController::scrollToPoint(const QPoint& maPoint, const QSize& screenSize) {
    scrollToBase(maPoint.x(), screenSize.width());
    scrollToViewRow(maPoint.y(), screenSize.height());
    // TODO:ichebyki centerViewRow(maPoint.y(), screenSize.height());
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

void MultilineScrollController::centerPoint(const QPoint& maPoint, const QSize& widgetSize) {
    centerBase(maPoint.x(), widgetSize.width());
    centerViewRow(maPoint.y(), widgetSize.height());
}

void MultilineScrollController::setMultilineHScrollbarValue(int value) {
    hScrollBar->setValue(value);
}

void MultilineScrollController::setMultilineVScrollbarBase(int base) {
    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    const int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    const int scrollAreaHeight = childrenScrollArea->height();
    const int lineHeight = ui->getUI(0)->height();
    const int vScrollValue = ((double)base * columnWidth / sequenceAreaWidth) * lineHeight;
    setMultilineVScrollbarValue(vScrollValue);
}

void MultilineScrollController::setMultilineHScrollbarBase(int base) {
    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    const int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    const int scrollAreaHeight = childrenScrollArea->height();
    const int lineHeight = ui->getUI(0)->height();
    const int vScrollValue = ((double)base * columnWidth / sequenceAreaWidth) * lineHeight;

    int max = (alignmentLength * columnWidth / sequenceAreaWidth + (restWidth > 0 ? 1 : 0)) * sequenceAreaWidth - hScrollTail;
    setMultilineVScrollbarValue(((double)base * columnWidth / sequenceAreaWidth /*+
                                 (restWidth > 0 ? 1 : 0)*/
                                 ) *
                                sequenceAreaWidth);
}

void MultilineScrollController::setMultilineVScrollbarValue(int value) {
    int maximum = vScrollBar->maximum();
    value = qMin(value, maximum);
    if (value >= maximum) {
        sl_handleVScrollAction(QAbstractSlider::SliderToMaximum);
        return;
    }

    // TODO:ichebyki
    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    const int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    const int scrollAreaHeight = childrenScrollArea->height();
    const int lineHeight = ui->getUI(0)->height();
    const int rowHeight = maEditor->getRowHeight();
    const int vScrollMaxValue = (alignmentLength * columnWidth / sequenceAreaWidth + (restWidth > 0 ? 1 : 0)) * lineHeight - scrollAreaHeight;
    double a = (value + scrollAreaHeight) / lineHeight;
    double b = a - (restWidth > 0 ? 1 : 0);
    double c = b * sequenceAreaWidth;
    double d = c / columnWidth;
    double g = (value / lineHeight) * lineHeight;
    double h = value - g;
    setFirstVisibleBase(d);
    childrenScrollArea->verticalScrollBar()->setValue(h);
    vScrollBar->setValue(value);
}

void MultilineScrollController::setFirstVisibleBase(int firstVisibleBase) {
    if (!maEditor->getMultilineMode()) {
        if (ui->getUI(0) != nullptr) {
            ui->getUI(0)->getScrollController()->setFirstVisibleBase(firstVisibleBase);
        }
    } else {
        int x = ui->getUI(0)->getBaseWidthController()->getBaseGlobalOffset(firstVisibleBase);
        hScrollBar->setValue(x);
    }
}

void MultilineScrollController::setCenterVisibleBase(int firstVisibleBase) {
    int visibleLength = ui->getSequenceAreaBaseLen(0);
    if (!maEditor->getMultilineMode()) {
        if (ui->getUI(0) != nullptr) {
            ui->getUI(0)->getScrollController()->setFirstVisibleBase(firstVisibleBase - visibleLength / 2);
        }
    } else {
        //int x = ui->getUI(0)->getBaseWidthController()->getBaseGlobalOffset(firstVisibleBase - visibleLength / 2);
        ui->getChildrenScrollArea()->verticalScrollBar()->setValue(0);
        ui->getScrollController()->setFirstVisibleBase(firstVisibleBase - visibleLength / 2);
        //hScrollBar->setValue(x);
    }
}

void MultilineScrollController::setFirstVisibleViewRow(int viewRowIndex) {
    if (!maEditor->getMultilineMode()) {
        ui->getUI(0)->getScrollController()->setFirstVisibleViewRow(viewRowIndex);
    }
}

void MultilineScrollController::setFirstVisibleMaRow(int maRowIndex) {
    if (!maEditor->getMultilineMode()) {
        ui->getUI(0)->getScrollController()->setFirstVisibleMaRow(maRowIndex);
    }
}

void MultilineScrollController::scrollSmoothly(const Directions& directions) {
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
    const int firstVisibleBase = maEditor->getMultilineMode()
                                     ? ui->getUI(0)->getBaseWidthController()->globalXPositionToColumn(hScrollBar->value()) + (removeClippedBase ? 1 : 0)
                                     : ui->getUI(0)->getScrollController()->getFirstVisibleBase(countClipped);
    assert(firstVisibleBase < maEditor->getAlignmentLen());
    return qMin(firstVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getLastVisibleBase(int widgetWidth, bool countClipped) const {
    const bool removeClippedBase = !countClipped && ((hScrollBar->value() + widgetWidth) % maEditor->getColumnWidth() != 0);
    const int lastVisibleBase = maEditor->getMultilineMode()
                                    ? ui->getUI(0)->getBaseWidthController()->globalXPositionToColumn(hScrollBar->value() + widgetWidth - 1) - (removeClippedBase ? 1 : 0)
                                    : ui->getUI(0)->getScrollController()->getLastVisibleBase(widgetWidth, countClipped);
    return qMin(lastVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getFirstVisibleMaRowIndex(bool countClipped) const {
    const bool removeClippedRow = !(countClipped || getAdditionalYOffset() == 0);
    return maEditor->getMultilineMode()
               ? ui->getUI(0)->getRowHeightController()->getMaRowIndexByGlobalYPosition(vScrollBar->value()) + (removeClippedRow ? 1 : 0)
               : ui->getUI(0)->getScrollController()->getFirstVisibleMaRowIndex(countClipped);
}

int MultilineScrollController::getFirstVisibleViewRowIndex(bool countClipped) const {
    int maRowIndex = getFirstVisibleMaRowIndex(countClipped);
    return maEditor->getCollapseModel()->getViewRowIndexByMaRowIndex(maRowIndex);
}

int MultilineScrollController::getLastVisibleViewRowIndex(int widgetHeight, bool countClipped) const {
    int lastVisibleViewRow = maEditor->getMultilineMode()
                                 ? ui->getUI(0)->getRowHeightController()->getViewRowIndexByGlobalYPosition(vScrollBar->value() + widgetHeight)
                                 : ui->getUI(0)->getScrollController()->getLastVisibleViewRowIndex(widgetHeight, countClipped);
    if (lastVisibleViewRow < 0) {
        lastVisibleViewRow = maEditor->getCollapseModel()->getViewRowCount() - 1;
    }
    U2Region lastRowScreenRegion = ui->getUI(0)->getRowHeightController()->getScreenYRegionByViewRowIndex(lastVisibleViewRow);
    bool removeClippedRow = !countClipped && lastRowScreenRegion.endPos() > widgetHeight;
    return lastVisibleViewRow - (removeClippedRow ? 1 : 0);
}

GScrollBar* MultilineScrollController::getHorizontalScrollBar() const {
    return hScrollBar;
}

GScrollBar* MultilineScrollController::getVerticalScrollBar() const {
    return vScrollBar;
}

void MultilineScrollController::sl_zoomScrollBars() {
    zoomHorizontalScrollBarPrivate();
    zoomVerticalScrollBarPrivate();
    //emit si_visibleAreaChanged();
}
void MultilineScrollController::sl_updateScrollBars() {
    // TODO:ichebyki set hscroll enable
    updateVerticalScrollBarPrivate();
    updateHorizontalScrollBarPrivate();
    updateChildrenScrollBarsPeivate();
    //emit si_visibleAreaChanged();
}

void MultilineScrollController::updateChildrenScrollBarsPeivate() {
    int h = hScrollBar->value();
    for (uint i = 0; i < ui->getChildrenCount(); i++) {
        GScrollBar* hbar = ui->getUI(i)->getScrollController()->getHorizontalScrollBar();
        // TODO:ichebyki
        bool x = hbar->isVisible();
        bool y = hbar->isEnabled();
        int x1 = hbar->minimum();
        int y1 = hbar->maximum();
        int x2 = hbar->singleStep();
        int y2 = hbar->pageStep();
        ui->getUI(i)->getScrollController()->setHScrollbarValue(h);
        h += ui->getSequenceAreaBaseWidth(i);
    }
    // TODO:ichebyki
    //emit si_visibleAreaChanged();
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
    CHECK(ui->getChildrenCount() > 0, );

    SAFE_POINT(nullptr != hScrollBar, "Horizontal scrollbar is not initialized", );
    QSignalBlocker signalBlocker(hScrollBar);
    Q_UNUSED(signalBlocker);

    CHECK_EXT(!maEditor->isAlignmentEmpty(), hScrollBar->setVisible(false), );

    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    const int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    const int allChildrenBaseWidth = ui->getSequenceAreaAllBaseWidth();

    hScrollTail = sequenceAreaWidth * ui->getChildrenCount();
    const int hScrollMaxValue = maEditor->getMultilineMode()
                                    ? (alignmentLength * columnWidth / sequenceAreaWidth +
                                       (restWidth > 0 ? 1 : 0)) *
                                              sequenceAreaWidth -
                                          hScrollTail
                                    : alignmentLength * columnWidth - allChildrenBaseWidth;
    hScrollBar->setMinimum(0);
    hScrollBar->setMaximum(qMax(0, hScrollMaxValue));
    hScrollBar->setSingleStep(columnWidth);
    hScrollBar->setPageStep(sequenceAreaWidth);

    // don't show horz scrollbar in non-multiline mode
    hScrollBar->setVisible(maEditor->getMultilineMode());

    // Special
    QSplitter* splitter = qobject_cast<QSplitter*>(hScrollBar->parent());
    if (splitter != nullptr) {
        if (maEditor->getMultilineMode() && ui->getChildrenCount() > 0) {
            const int vScrollBarWidth = vScrollBar->width();
            const int nameListWidth = ui->getUI(ui->getChildrenCount() - 1)->getEditorNameList()->width() + 2;
            splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            splitter->setVisible(true);
            splitter->setFixedHeight(vScrollBarWidth);
            splitter->setSizes({nameListWidth, sequenceAreaWidth, vScrollBarWidth});
        } else {
            splitter->setVisible(false);
        }
    }
}

void MultilineScrollController::updateVerticalScrollBarPrivate() {
    CHECK(ui->getChildrenCount() > 0, );

    SAFE_POINT(nullptr != vScrollBar, "Multiline Vertical scrollbar is not initialized", );
    QSignalBlocker signalBlocker(vScrollBar);
    Q_UNUSED(signalBlocker);

    CHECK_EXT(!maEditor->isAlignmentEmpty(), vScrollBar->setVisible(false), );
    CHECK_EXT(ui->getChildrenCount() > 0, vScrollBar->setVisible(false), );

    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    const int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    const int scrollAreaHeight = childrenScrollArea->height();

    const int lineHeight = ui->getUI(0)->height();
    const int rowHeight = maEditor->getRowHeight();
    vScrollTail = scrollAreaHeight;
    const int vScrollMaxValue = (alignmentLength * columnWidth / sequenceAreaWidth +
                                 (restWidth > 0 ? 1 : 0)) *
                                    lineHeight -
                                vScrollTail;

    vScrollBar->setMinimum(0);
    vScrollBar->setMaximum(qMax(0, vScrollMaxValue));
    vScrollBar->setSingleStep(rowHeight);
    vScrollBar->setPageStep(scrollAreaHeight);

    // don't show vert scrollbar in non-multiline mode
    vScrollBar->setVisible(maEditor->getMultilineMode());

    // Special
    childrenScrollArea->verticalScrollBar()->setMinimum(0);
    childrenScrollArea->verticalScrollBar()->setMaximum(ui->getChildrenCount() * lineHeight -
                                                        scrollAreaHeight);
    childrenScrollArea->verticalScrollBar()->setSingleStep(rowHeight);
    childrenScrollArea->verticalScrollBar()->setPageStep(scrollAreaHeight);
}

}  // namespace U2
