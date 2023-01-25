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

void MultilineScrollController::init(GScrollBar* _vScrollBar,
                                     QScrollArea* childrenArea) {
    this->childrenScrollArea = childrenArea;

    vScrollBar = _vScrollBar;
    vScrollBar->setValue(0);
    vScrollBar->setSingleStep(maEditor->getRowHeight());
    vScrollBar->setPageStep(childrenScrollArea->height());
    vScrollBar->installEventFilter(this);

    setEnable(enabled);
    sl_updateScrollBars();
}
void MultilineScrollController::initSignals(bool enable) {
    if (enable) {
        if (!connAreaChanged)
            connAreaChanged = connect(this,
                                      SIGNAL(si_visibleAreaChanged()),
                                      this,
                                      SLOT(sl_updateScrollBars()));

        if (!connVValueChanged)
            connVValueChanged = connect(vScrollBar,
                                        SIGNAL(valueChanged(int)),
                                        this,
                                        SLOT(sl_vScrollValueChanged()));
        if (!connVActionTriggered)
            connVActionTriggered = connect(vScrollBar,
                                           SIGNAL(actionTriggered(int)),
                                           this,
                                           SLOT(sl_handleVScrollAction(int)));
    } else {
        if (connAreaChanged)
            disconnect(connAreaChanged);
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
            // return vertEventFilter((QWheelEvent *) event);
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

    if (maEditor->getMultilineMode()) {
        MsaMultilineScrollArea* scroller = qobject_cast<MsaMultilineScrollArea*>(
            childrenScrollArea);
        CHECK(scroller != nullptr, );
        if (directions.testFlag(SliderMinimum)) {
            setFirstVisibleBase(0);
            vScrollBar->setValue(0);
            scroller->verticalScrollBar()->setValue(0);
        } else if (directions.testFlag(SliderMaximum)) {
            scroller->verticalScrollBar()->setValue(scroller->verticalScrollBar()->maximum());
            int seqAreaBaseLength = ui->getSequenceAreaBaseLen(0);
            int evenBase = maEditor->getAlignmentLen() / seqAreaBaseLength * seqAreaBaseLength - seqAreaBaseLength * (ui->getChildrenCount() - 1);
            setFirstVisibleBase(evenBase);
            vScrollBar->setValue(vScrollBar->maximum());
        } else {
            scroller->scrollVert(directions, byStep);
        }
    }

    ui->setUpdatesEnabled(true);
}

int MultilineScrollController::getViewHeight() {
    return childrenScrollArea->height();
}

void MultilineScrollController::sl_vScrollValueChanged() {
    if (maEditor->getMultilineMode()) {
        // TODO:ichebyki
        // int v = vScrollBar->value();
        // setMultilineHScrollbarValue(v);
    } else {
        int v = vScrollBar->value();
        ui->getUI(0)->getScrollController()->setHScrollbarValue(v);
    }
}

void MultilineScrollController::scrollToViewRow(QPoint maPoint) {
    int baseNumber = maPoint.x();
    int viewRowIndex = maPoint.y();
    QList<int> widgetIndex;
    for (int i = 0; i < ui->getChildrenCount(); i++) {
        if (!ui->getUI(i)->visibleRegion().isEmpty()) {
            widgetIndex.append(i);
        }
    }

    QPoint pTop, pBottom;
    int height = ui->getChildrenScrollArea()->height();
    U2Region rowRegion;
    int indexFound = -1;
    for (int i : qAsConst(widgetIndex)) {
        indexFound = -1;

        if (baseNumber >= ui->getFirstVisibleBase(i) && baseNumber <= ui->getLastVisibleBase(i)) {
            indexFound = i;
        }
        rowRegion = ui->getUI(i)
                        ->getRowHeightController()
                        ->getGlobalYRegionByViewRowIndex(viewRowIndex);
        pTop = ui->getUI(i)->getSequenceArea()->mapTo(ui->getChildrenScrollArea(),
                                                      QPoint(0, rowRegion.startPos));
        pBottom = ui->getUI(i)->getSequenceArea()->mapTo(ui->getChildrenScrollArea(),
                                                         QPoint(0, rowRegion.endPos()));
        if (indexFound != -1) {
            if (pTop.y() >= 0 && pBottom.y() <= height) {
                return;
            }
            break;
        }
    }

    auto scroller = ui->getChildrenScrollArea()->verticalScrollBar();
    if (indexFound != -1) {
        if (pTop.y() < 0) {
            vScrollBar->setValue(vScrollBar->value() + pTop.y());
            scroller->setValue(scroller->value() + pTop.y());
        } else if (pBottom.y() > height) {
            vScrollBar->setValue(vScrollBar->value() + pBottom.y() - height);
            scroller->setValue(scroller->value() + pBottom.y() - height);
        }
    } else {
        const int length = ui->getLastVisibleBase(0) + 1 - ui->getFirstVisibleBase(0);
        int firstBase = baseNumber / length * length;
        setFirstVisibleBase(firstBase);
        scroller->setValue(0);
        rowRegion = ui->getUI(0)
                        ->getRowHeightController()
                        ->getGlobalYRegionByViewRowIndex(viewRowIndex);
        pTop = ui->getUI(0)->getSequenceArea()->mapTo(ui->getChildrenScrollArea(),
                                                      QPoint(0, rowRegion.startPos));
        vScrollBar->setValue(vScrollBar->value() + pTop.y());
        scroller->setValue(scroller->value() + pTop.y());
    }
}

void MultilineScrollController::scrollToBase(QPoint maPoint) {
    int baseNumber = maPoint.x();
    QList<int> widgetIndex;
    for (int i = 0; i < ui->getChildrenCount(); i++) {
        if (!ui->getUI(i)->visibleRegion().isEmpty()) {
            widgetIndex.append(i);
        }
    }

    const int length = ui->getLastVisibleBase(0) + 1 - ui->getFirstVisibleBase(0);
    int indexFound = -1;
    for (int i : qAsConst(widgetIndex)) {
        if (baseNumber >= ui->getFirstVisibleBase(i) && baseNumber <= ui->getLastVisibleBase(i)) {
            indexFound = i;
        }
    }

    if (indexFound == -1) {
        if ((baseNumber - length) < 0) {
            vertScroll(SliderMinimum, false);
        } else if ((baseNumber + length) >= maEditor->getAlignmentLen()) {
            vertScroll(SliderMaximum, false);
        } else {
            int evenFirstVisibleBase = baseNumber / length * length;
            int scrollChildrenAreaValue = 0;
            const int lineHeight = ui->getUI(0)->height();
            while ((evenFirstVisibleBase + length * ((int)ui->getChildrenCount() - 1)) >= maEditor->getAlignmentLen()) {
                evenFirstVisibleBase -= length;
                scrollChildrenAreaValue += lineHeight;
            }
            setFirstVisibleBase(evenFirstVisibleBase);
            setMultilineVScrollbarBase(evenFirstVisibleBase);
            childrenScrollArea->verticalScrollBar()->setValue(scrollChildrenAreaValue);
        }
    }
}

void MultilineScrollController::scrollToPoint(const QPoint& maPoint) {
    scrollToBase(maPoint);
    scrollToViewRow(maPoint);
}

void MultilineScrollController::centerBase(int baseNumber) {
    int length = ui->getLastVisibleBase(0) + 1 - ui->getFirstVisibleBase(0);
    int fistBase = (baseNumber / length) * length;
    setFirstVisibleBase(fistBase);
}

void MultilineScrollController::centerViewRow(QPoint maPoint) {
    assert(false && "Need to implement");
    int viewRowIndex = maPoint.y();
    const U2Region rowGlobalRange = ui->getUI(0)
                                        ->getRowHeightController()
                                        ->getGlobalYRegionByViewRowIndex(viewRowIndex);
    U2Region visibleRange = getVerticalRangeToDrawIn(childrenScrollArea->width());
    int newScreenYOffset = int(rowGlobalRange.startPos - visibleRange.length / 2);
    vScrollBar->setValue(newScreenYOffset);
}

void MultilineScrollController::centerPoint(const QPoint& maPoint) {
    centerBase(maPoint.x());
    centerViewRow(maPoint);
}

void MultilineScrollController::setMultilineVScrollbarBase(int base) {
    int columnWidth = maEditor->getColumnWidth();
    int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    int lineHeight = ui->getUI(0)->height();
    int vScrollValue = int(((double)base * columnWidth / sequenceAreaWidth) * lineHeight);
    setMultilineVScrollbarValue(vScrollValue);
}

void MultilineScrollController::setMultilineVScrollbarValue(int value) {
    int maximum = vScrollBar->maximum();
    value = qMin(value, maximum);
    if (value >= maximum) {
        sl_handleVScrollAction(QAbstractSlider::SliderToMaximum);
        return;
    } else if (value <= 0) {
        sl_handleVScrollAction(QAbstractSlider::SliderToMinimum);
        return;
    }

    // TODO:ichebyki
    int alignmentLength = maEditor->getAlignmentLen();
    int columnWidth = maEditor->getColumnWidth();
    int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    int scrollAreaHeight = childrenScrollArea->height();
    int lineHeight = ui->getUI(0)->height();
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
    if (maEditor->getMultilineMode()) {
        QSignalBlocker signalBlocker(this);
        ui->setUpdatesEnabled(false);

        int length = ui->getLastVisibleBase(0) + 1 - ui->getFirstVisibleBase(0);
        for (int i = 0; i < ui->getChildrenCount(); i++) {
            ui->getUI(i)->getScrollController()->setFirstVisibleBase(firstVisibleBase);
            firstVisibleBase += length;
        }

        ui->setUpdatesEnabled(true);
        ui->getOverviewArea()->update();
    } else {
        if (ui->getUI(0) != nullptr) {
            ui->getUI(0)->getScrollController()->setFirstVisibleBase(firstVisibleBase);
        }
    }
}

void MultilineScrollController::setCenterVisibleBase(int firstVisibleBase) {
    int visibleLength = ui->getSequenceAreaBaseLen(0);
    if (!maEditor->getMultilineMode()) {
        if (ui->getUI(0) != nullptr) {
            ui->getUI(0)->getScrollController()->setFirstVisibleBase(firstVisibleBase - visibleLength / 2);
        }
    } else {
        // int x = ui->getUI(0)->getBaseWidthController()->getBaseGlobalOffset(firstVisibleBase - visibleLength / 2);
        ui->getChildrenScrollArea()->verticalScrollBar()->setValue(0);
        ui->getScrollController()->setFirstVisibleBase(firstVisibleBase - visibleLength / 2);
        // hScrollBar->setValue(x);
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
    QAbstractSlider::SliderAction verticalAction = QAbstractSlider::SliderNoAction;

    if (directions.testFlag(Up)) {
        verticalAction = QAbstractSlider::SliderSingleStepSub;
    }
    if (directions.testFlag(Down)) {
        verticalAction = QAbstractSlider::SliderSingleStepAdd;
    }
    if (verticalAction != vScrollBar->getRepeatAction()) {
        vScrollBar->setupRepeatAction(verticalAction, 500, 50);
    }
}

void MultilineScrollController::stopSmoothScrolling() {
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
        default:
            FAIL("Unsupported direction: " + QString::number(direction), );
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
        case Right:
        default:
            FAIL("An unknown direction", );
            break;
    }
}

void MultilineScrollController::scrollToEnd(MultilineScrollController::Direction direction) {
    switch (direction) {
        case Up:
        case SliderMinimum:
            vScrollBar->triggerAction(QAbstractSlider::SliderToMinimum);
            break;
        case Down:
        case SliderMaximum:
            vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);
            break;
        case Left:
        case Right:
        default:
            FAIL("An unknown direction", );
            break;
    }
}

int MultilineScrollController::getFirstVisibleBase(bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    const int firstVisibleBase = ui->getUI(0)->getScrollController()->getFirstVisibleBase(countClipped);
    assert(firstVisibleBase < maEditor->getAlignmentLen());
    return qMin(firstVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getLastVisibleBase(int widgetWidth, bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    const int lastVisibleBase = ui->getUI(ui->getChildrenCount() - 1)
                                    ->getScrollController()
                                    ->getLastVisibleBase(widgetWidth, countClipped);
    return qMin(lastVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getFirstVisibleMaRowIndex(bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    return ui->getUI(0)->getScrollController()->getFirstVisibleMaRowIndex(countClipped);
}

int MultilineScrollController::getFirstVisibleViewRowIndex(bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    int maRowIndex = getFirstVisibleMaRowIndex(countClipped);
    return maEditor->getCollapseModel()->getViewRowIndexByMaRowIndex(maRowIndex);
}

int MultilineScrollController::getLastVisibleViewRowIndex(int widgetHeight, bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    int lastVisibleViewRow = ui->getUI(ui->getChildrenCount() - 1)
                                 ->getScrollController()
                                 ->getLastVisibleViewRowIndex(widgetHeight, countClipped);
    if (lastVisibleViewRow < 0) {
        lastVisibleViewRow = maEditor->getCollapseModel()->getViewRowCount() - 1;
    }
    U2Region lastRowScreenRegion = ui->getUI(ui->getChildrenCount() - 1)
                                       ->getRowHeightController()
                                       ->getScreenYRegionByViewRowIndex(lastVisibleViewRow);
    bool removeClippedRow = !countClipped && lastRowScreenRegion.endPos() > widgetHeight;
    return lastVisibleViewRow - (removeClippedRow ? 1 : 0);
}

GScrollBar* MultilineScrollController::getVerticalScrollBar() const {
    return vScrollBar;
}

void MultilineScrollController::sl_zoomScrollBars() {
    zoomVerticalScrollBarPrivate();
    // emit si_visibleAreaChanged();
}
void MultilineScrollController::sl_updateScrollBars() {
    updateVerticalScrollBarPrivate();
    updateChildrenScrollBarsPeivate();
}

void MultilineScrollController::updateChildrenScrollBarsPeivate() {
    int val;
    for (int i = 0; i < ui->getChildrenCount(); i++) {
        GScrollBar* hbar = ui->getUI(i)->getScrollController()->getHorizontalScrollBar();
        if (i == 0) {
            val = hbar->value();
        }
        ui->getUI(i)->getScrollController()->setHScrollbarValue(val);
        val += ui->getSequenceAreaBaseWidth(i);
    }
}

int MultilineScrollController::getAdditionalYOffset() const {
    int maRow = ui->getUI(0)->getRowHeightController()->getMaRowIndexByGlobalYPosition(vScrollBar->value());
    int viewRow = ui->getUI(0)->getRowHeightController()->getGlobalYPositionByMaRowIndex(maRow);
    return vScrollBar->value() - viewRow;
}

U2Region MultilineScrollController::getVerticalRangeToDrawIn(int widgetHeight) const {
    return U2Region(vScrollBar->value(), widgetHeight);
}

void MultilineScrollController::zoomVerticalScrollBarPrivate() {
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
    const int sequenceAreaWidth = qMax(1, ui->getSequenceAreaBaseWidth(0));
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

    // Set values
    int gScrollValue = getFirstVisibleBase() * columnWidth / sequenceAreaWidth * lineHeight + childrenScrollArea->verticalScrollBar()->value();
    vScrollBar->setValue(gScrollValue);
}

}  // namespace U2
