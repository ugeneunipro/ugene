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

#include "BaseWidthController.h"
#include "DrawHelper.h"
#include "RowHeightController.h"
#include "ScrollController.h"
#include "ov_msa/MaEditorNameList.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MaEditorWgt.h"
#include "ov_msa/MsaEditor.h"
#include "ov_msa/MsaEditorOverviewArea.h"
#include "ov_msa/MsaMultilineScrollArea.h"

namespace U2 {

MultilineScrollController::MultilineScrollController(MsaEditor* _maEditor, MsaEditorMultilineWgt* _maEditorUi)
    : QObject(_maEditorUi),
      maEditor(_maEditor),
      ui(_maEditorUi) {
}

void MultilineScrollController::init(GScrollBar* _vScrollBar,
                                     QScrollArea* childrenArea) {
    this->childrenScrollArea = childrenArea;

    vScrollBar = _vScrollBar;
    vScrollBar->setValue(0);
    vScrollBar->setSingleStep(maEditor->getRowHeight());
    vScrollBar->setPageStep(childrenScrollArea->height());
    vScrollBar->installEventFilter(this);

    setEnabled(enabled);
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

void MultilineScrollController::setEnabled(bool enable) {
    if (enable != enabled) {
        enabled = enable;
    }
    initSignals(enabled);
}

bool MultilineScrollController::isEnabled() const {
    return enabled;
};

// TODO: new vertical scrollbar mode
bool MultilineScrollController::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::Wheel && maEditor->isMultilineMode()) {
        if (object == vScrollBar) {
            // return vertEventFilter((QWheelEvent *) event);
            return true;
        }
    }

    return false;
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

    if (maEditor->isMultilineMode()) {
        auto scroller = qobject_cast<MsaMultilineScrollArea*>(
            childrenScrollArea);
        CHECK(scroller != nullptr, );
        if (directions.testFlag(SliderMinimum)) {
            setFirstVisibleBase(0);
            vScrollBar->setValue(0);
            scroller->verticalScrollBar()->setValue(0);
        } else if (directions.testFlag(SliderMaximum)) {
            scroller->verticalScrollBar()->setValue(scroller->verticalScrollBar()->maximum());
            int seqAreaBaseLength = ui->getSequenceAreaBaseLen(0);
            int evenBase = maEditor->getAlignmentLen() / seqAreaBaseLength * seqAreaBaseLength - seqAreaBaseLength * (ui->getLineWidgetCount() - 1);
            setFirstVisibleBase(evenBase);
            vScrollBar->setValue(vScrollBar->maximum());
        } else {
            scroller->scrollVert(directions, byStep);
        }
    }

    ui->setUpdatesEnabled(true);
}

bool MultilineScrollController::checkBoundary() const {
    int firstBase;
    int prevFistBase = -1;
    bool needUpdate = false;
    int childrenCount = ui->getLineWidgetCount();
    int alignmentLen = maEditor->getAlignmentLen();
    for (int i = 0; i < childrenCount; i++) {
        MaEditorWgt* wgt = ui->getLineWidget(i);
        SAFE_POINT_NN(wgt, false);
        firstBase = wgt->getScrollController()->getFirstVisibleBase(false);
        if (firstBase == prevFistBase) {
            needUpdate = true;
            break;
        }
        if (firstBase + 1 >= alignmentLen) {
            needUpdate = true;
            break;
        }
        if (!wgt->isVisible()) {
            needUpdate = true;
            break;
        }
        prevFistBase = firstBase;
    }
    if (needUpdate) {
        int length = ui->getSequenceAreaBaseLen(0);
        if ((firstBase + 1) >= alignmentLen || firstBase == prevFistBase) {
            firstBase = qMax(0, firstBase == 0 ? 0 : alignmentLen / length * length - length * (childrenCount - (alignmentLen % length > 0 ? 1 : 0)));
            prevFistBase = -1;
            for (int i = 0; i < childrenCount; i++) {
                MaEditorWgt* wgt = ui->getLineWidget(i);
                SAFE_POINT(wgt != nullptr, "Unexpected nullptr multiline editor child widget", false);
                if (firstBase == prevFistBase ||
                    ((firstBase + 1) >= alignmentLen && alignmentLen > 1) ||
                    ((firstBase + 1) >= alignmentLen && alignmentLen == 1 && prevFistBase != -1)) {
                    wgt->setHidden(true);
                } else {
                    wgt->setHidden(false);
                    QSignalBlocker signalBlocker(wgt->getScrollController());
                    wgt->getScrollController()->setFirstVisibleBase(firstBase);
                }
                prevFistBase = firstBase;
                firstBase += length;
            }
        } else {
            for (int i = 0; i < childrenCount; i++) {
                MaEditorWgt* wgt = ui->getLineWidget(i);
                SAFE_POINT(wgt != nullptr, "Unexpected nullptr multiline editor child widget", false);
                firstBase = wgt->getScrollController()->getFirstVisibleBase(false);
                if (firstBase == prevFistBase || firstBase + 1 >= alignmentLen) {
                    wgt->setHidden(true);
                } else {
                    wgt->setHidden(false);
                }
                prevFistBase = firstBase;
            }
        }
        ui->updateGeometry();
        return true;
    }
    return false;
}

void MultilineScrollController::sl_vScrollValueChanged() {
    if (ui->isWrapMode()) {
        checkBoundary();
    } else {
        int v = vScrollBar->value();
        ui->getLineWidget(0)->getScrollController()->setHScrollbarValue(v);
    }
}

void MultilineScrollController::scrollToViewRow(QPoint maPoint) {
    int baseNumber = maPoint.x();
    int viewRowIndex = maPoint.y();
    QList<int> widgetIndex;
    for (int i = 0; i < ui->getLineWidgetCount(); i++) {
        if (!ui->getLineWidget(i)->visibleRegion().isEmpty()) {
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
        rowRegion = ui->getLineWidget(i)
                        ->getRowHeightController()
                        ->getGlobalYRegionByViewRowIndex(viewRowIndex);
        pTop = ui->getLineWidget(i)->getSequenceArea()->mapTo(ui->getChildrenScrollArea(),
                                                              QPoint(0, rowRegion.startPos));
        pBottom = ui->getLineWidget(i)->getSequenceArea()->mapTo(ui->getChildrenScrollArea(),
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
        rowRegion = ui->getLineWidget(0)
                        ->getRowHeightController()
                        ->getGlobalYRegionByViewRowIndex(viewRowIndex);
        pTop = ui->getLineWidget(0)->getSequenceArea()->mapTo(ui->getChildrenScrollArea(),
                                                              QPoint(0, rowRegion.startPos));
        vScrollBar->setValue(vScrollBar->value() + pTop.y());
        scroller->setValue(scroller->value() + pTop.y());
    }
}

void MultilineScrollController::scrollToBase(QPoint maPoint) {
    int baseNumber = maPoint.x();
    QList<int> widgetIndex;
    for (int i = 0; i < ui->getLineWidgetCount(); i++) {
        if (!ui->getLineWidget(i)->visibleRegion().isEmpty()) {
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
            const int lineHeight = ui->getLineWidget(0)->height();
            while ((evenFirstVisibleBase + length * ((int)ui->getLineWidgetCount() - 1)) >= maEditor->getAlignmentLen()) {
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

void MultilineScrollController::setMultilineVScrollbarBase(int base) {
    int columnWidth = maEditor->getColumnWidth();
    int sequenceAreaWidth = ui->getSequenceAreaBaseWidth(0);
    int lineHeight = ui->getLineWidget(0)->height();
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
    int lineHeight = ui->getLineWidget(0)->height();
    double a = double(value + scrollAreaHeight) / lineHeight;
    double b = a - (restWidth > 0 ? 1 : 0);
    double c = b * sequenceAreaWidth;
    double d = c / columnWidth;
    double g = ((double)value / lineHeight) * lineHeight;
    double h = value - g;
    setFirstVisibleBase(d);
    childrenScrollArea->verticalScrollBar()->setValue(h);
    vScrollBar->setValue(value);
}

void MultilineScrollController::setFirstVisibleBase(int firstVisibleBase) {
    if (maEditor->isMultilineMode()) {
        QSignalBlocker signalBlocker(this);
        Q_UNUSED(signalBlocker);

        ui->setUpdatesEnabled(false);

        int length = ui->getLastVisibleBase(0) + 1 - ui->getFirstVisibleBase(0);
        for (int i = 0; i < ui->getLineWidgetCount(); i++) {
            ui->getLineWidget(i)->getScrollController()->setFirstVisibleBase(firstVisibleBase);
            firstVisibleBase += length;
        }

        ui->setUpdatesEnabled(true);
        ui->getOverviewArea()->update();
    } else {
        if (ui->getLineWidget(0) != nullptr) {
            ui->getLineWidget(0)->getScrollController()->setFirstVisibleBase(firstVisibleBase);
        }
    }
}

void MultilineScrollController::setFirstVisibleViewRow(int viewRowIndex) {
    if (!maEditor->isMultilineMode()) {
        ui->getLineWidget(0)->getScrollController()->setFirstVisibleViewRow(viewRowIndex);
    }
}

void MultilineScrollController::setFirstVisibleMaRow(int maRowIndex) {
    if (!maEditor->isMultilineMode()) {
        ui->getLineWidget(0)->getScrollController()->setFirstVisibleMaRow(maRowIndex);
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
    }
}

int MultilineScrollController::getFirstVisibleBase(bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    const int firstVisibleBase = ui->getLineWidget(0)->getScrollController()->getFirstVisibleBase(countClipped);
    assert(firstVisibleBase < maEditor->getAlignmentLen());
    return qMin(firstVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getLastVisibleBase(int widgetWidth, bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    const int lastVisibleBase = ui->getLineWidget(ui->getLineWidgetCount() - 1)
                                    ->getScrollController()
                                    ->getLastVisibleBase(widgetWidth, countClipped);
    return qMin(lastVisibleBase, maEditor->getAlignmentLen() - 1);
}

int MultilineScrollController::getFirstVisibleMaRowIndex(bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    return ui->getLineWidget(0)->getScrollController()->getFirstVisibleMaRowIndex(countClipped);
}

int MultilineScrollController::getFirstVisibleViewRowIndex(bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    int maRowIndex = getFirstVisibleMaRowIndex(countClipped);
    return maEditor->getCollapseModel()->getViewRowIndexByMaRowIndex(maRowIndex);
}

int MultilineScrollController::getLastVisibleViewRowIndex(int widgetHeight, bool countClipped) const {
    CHECK(maEditor->getAlignmentLen() > 0, 0);
    int lastVisibleViewRow = ui->getLineWidget(ui->getLineWidgetCount() - 1)
                                 ->getScrollController()
                                 ->getLastVisibleViewRowIndex(widgetHeight, countClipped);
    if (lastVisibleViewRow < 0) {
        lastVisibleViewRow = maEditor->getCollapseModel()->getViewRowCount() - 1;
    }
    U2Region lastRowScreenRegion = ui->getLineWidget(ui->getLineWidgetCount() - 1)
                                       ->getRowHeightController()
                                       ->getScreenYRegionByViewRowIndex(lastVisibleViewRow);
    bool removeClippedRow = !countClipped && lastRowScreenRegion.endPos() > widgetHeight;
    return lastVisibleViewRow - (removeClippedRow ? 1 : 0);
}

GScrollBar* MultilineScrollController::getVerticalScrollBar() const {
    return vScrollBar;
}

void MultilineScrollController::sl_zoomScrollBars() {
    // emit si_visibleAreaChanged();
}

void MultilineScrollController::sl_updateScrollBars() {
    checkBoundary();
    updateVerticalScrollBarPrivate();
    updateChildrenScrollBarsPrivate();
}

void MultilineScrollController::updateChildrenScrollBarsPrivate() {
    int val;
    for (int i = 0; i < ui->getLineWidgetCount(); i++) {
        auto wgt = ui->getLineWidget(i);
        SAFE_POINT_NN(wgt, );

        if (i == 0) {
            GScrollBar* hBar = wgt->getScrollController()->getHorizontalScrollBar();
            val = hBar->value();
        }

        QSignalBlocker signalBlocker(wgt->getScrollController());
        wgt->getScrollController()->setHScrollbarValue(val);
        val += ui->getSequenceAreaBaseWidth(i);
    }
}

void MultilineScrollController::updateVerticalScrollBarPrivate() {
    CHECK(ui->getLineWidgetCount() > 0, );

    SAFE_POINT(vScrollBar != nullptr, "Multiline Vertical scrollbar is not initialized", );
    QSignalBlocker signalBlocker(vScrollBar);

    CHECK_EXT(!maEditor->isAlignmentEmpty(), vScrollBar->setVisible(false), );
    CHECK_EXT(ui->getLineWidgetCount() > 0, vScrollBar->setVisible(false), );

    const int alignmentLength = maEditor->getAlignmentLen();
    const int columnWidth = maEditor->getColumnWidth();
    const int sequenceAreaWidth = qMax(1, ui->getSequenceAreaBaseWidth(0));
    const int restWidth = (alignmentLength * columnWidth) % sequenceAreaWidth;
    const int scrollAreaHeight = childrenScrollArea->height();

    const int lineHeight = ui->getLineWidget(0)->height();
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
    vScrollBar->setVisible(maEditor->isMultilineMode());

    // Special
    childrenScrollArea->verticalScrollBar()->setMinimum(0);
    childrenScrollArea->verticalScrollBar()->setMaximum(ui->getLineWidgetCount() * lineHeight -
                                                        scrollAreaHeight);
    childrenScrollArea->verticalScrollBar()->setSingleStep(rowHeight);
    childrenScrollArea->verticalScrollBar()->setPageStep(scrollAreaHeight);

    // Set values
    int gScrollValue = getFirstVisibleBase() * columnWidth / sequenceAreaWidth * lineHeight + childrenScrollArea->verticalScrollBar()->value();
    vScrollBar->setValue(gScrollValue);
}

}  // namespace U2
