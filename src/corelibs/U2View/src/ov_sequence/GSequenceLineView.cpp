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

#include "GSequenceLineView.h"

#include <QApplication>
#include <QTextEdit>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GScrollBar.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/OrderedToolbar.h>

#include "ADVSequenceObjectContext.h"

namespace U2 {

GSequenceLineView::GSequenceLineView(QWidget* p, SequenceObjectContext* _ctx)
    : WidgetWithLocalToolbar(p),
      ctx(_ctx),
      renderArea(nullptr),
      scrollBar(nullptr),
      lastPressPos(-1),
      lastUpdateFlags(GSLV_UF_ViewResized),
      featureFlags(GSLV_FF_SupportsCustomRange),
      frameView(nullptr),
      coherentRangeView(nullptr),
      ignoreMouseSelectionEvents(false),
      singleBaseSelection(false),
      isSelectionResizing(false) {
    GCOUNTER(cvar, "SequenceLineView");
    seqLen = ctx->getSequenceLength();
    setFocusPolicy(Qt::WheelFocus);

    coefScrollBarMapping = seqLen >= INT_MAX ? INT_MAX / (double)seqLen : 1;

    scrollBar = new GScrollBar(Qt::Horizontal, this);

    connect(ctx->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
            SLOT(sl_onDNASelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));

    connect(ctx->getSequenceGObject(), SIGNAL(si_sequenceChanged()), this, SLOT(sl_sequenceChanged()));
}

void GSequenceLineView::pack() {
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(renderArea);
    layout->addWidget(scrollBar);

    setContentLayout(layout);
    setMinimumHeight(layout->minimumSize().height());
}

void GSequenceLineView::resizeEvent(QResizeEvent* e) {
    updateScrollBar();
    addUpdateFlags(GSLV_UF_ViewResized);
    QWidget::resizeEvent(e);
}

void GSequenceLineView::updateScrollBar() {
    scrollBar->disconnect(this);

    scrollBar->setMinimum(0);
    // The rounding to 'int' below is safe because we use 'coefScrollBarMapping' which will map int64 values to int32 domain.
    scrollBar->setMaximum(qRound((double)(seqLen - visibleRange.length) * coefScrollBarMapping));
    scrollBar->setSliderPosition(qRound(coefScrollBarMapping * (double)visibleRange.startPos));
    scrollBar->setSingleStep(qRound((double)getSingleStep() / coefScrollBarMapping));
    scrollBar->setPageStep(qRound((double)getPageStep() * coefScrollBarMapping));

    connect(scrollBar, SIGNAL(valueChanged(int)), SLOT(sl_onScrollBarMoved(int)));
}

qint64 GSequenceLineView::getSingleStep() const {
    if (coherentRangeView != nullptr) {
        return coherentRangeView->getSingleStep();
    }
    return 1;
}

qint64 GSequenceLineView::getPageStep() const {
    if (coherentRangeView != nullptr) {
        return coherentRangeView->getPageStep();
    }
    return visibleRange.length;
}

void GSequenceLineView::sl_onScrollBarMoved(int pos) {
    if (coherentRangeView != nullptr) {
        coherentRangeView->sl_onScrollBarMoved(pos);
        return;
    }
    SAFE_POINT(coefScrollBarMapping != 0, "coefScrollBarMapping is null", );
    setStartPos(qRound64(pos / coefScrollBarMapping));

    if (lastPressPos != -1) {
        QAbstractSlider::SliderAction aAction = scrollBar->getRepeatAction();
        if (aAction == QAbstractSlider::SliderSingleStepAdd) {
            const qint64 selStart = qMin(lastPressPos, visibleRange.endPos());
            const qint64 selEnd = qMax(lastPressPos, visibleRange.endPos());
            const U2Region newSelection(selStart, selEnd - selStart);
            changeSelectionOnScrollbarMoving(newSelection);
        } else if (aAction == QAbstractSlider::SliderSingleStepSub) {
            const qint64 selStart = qMin(lastPressPos, visibleRange.startPos);
            const qint64 selEnd = qMax(lastPressPos, visibleRange.startPos);
            const U2Region newSelection(selStart, selEnd - selStart);
            changeSelectionOnScrollbarMoving(newSelection);
        }
    }
}

void GSequenceLineView::setSelection(const U2Region& r) {
    SAFE_POINT(r.startPos >= 0 && r.endPos() <= seqLen, QString("Selection is out of range! [%2, len: %3]").arg(r.startPos).arg(r.length), );
    ctx->getSequenceSelection()->setRegion(r);
}

void GSequenceLineView::addSelection(const U2Region& r) {
    SAFE_POINT(r.startPos >= 0 && r.endPos() <= seqLen, QString("Selection is out of range! [%2, len: %3]").arg(r.startPos).arg(r.length), );
    if (r.length != 0) {
        ctx->getSequenceSelection()->addRegion(r);
    }
}

void GSequenceLineView::mousePressEvent(QMouseEvent* me) {
    setFocus();
    isSelectionResizing = true;

    QPoint renderAreaPos = toRenderAreaPoint(me->pos());
    if (!renderArea->rect().contains(renderAreaPos)) {
        scrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
        lastPressPos = -1;
        QWidget::mousePressEvent(me);
        return;
    }

    Qt::CursorShape shape = cursor().shape();
    if (shape != Qt::ArrowCursor) {
        moveBorder(me->pos());
        QWidget::mousePressEvent(me);
        return;
    }

    lastPressPos = renderArea->coordToPos(renderAreaPos);
    SAFE_POINT(lastPressPos >= visibleRange.startPos && lastPressPos <= visibleRange.endPos(), "Last mouse press position is out of visible range!", );

    if (me->button() == Qt::RightButton) {
        QWidget::mousePressEvent(me);
        return;
    }

    if (!ignoreMouseSelectionEvents) {
        ctx->getSequenceSelection()->clear();
    }

    QWidget::mousePressEvent(me);
}

void GSequenceLineView::mouseReleaseEvent(QMouseEvent* me) {
    setFocus();

    if (!ignoreMouseSelectionEvents) {
        // click with 'alt' selects a single base
        Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
        bool singleBaseSelectionMode = km.testFlag(Qt::AltModifier) || singleBaseSelection;
        if (me->button() == Qt::LeftButton && singleBaseSelectionMode) {
            QPoint areaPoint = toRenderAreaPoint(me->pos());
            qint64 pos = renderArea->coordToPos(areaPoint);
            if (pos == lastPressPos) {
                U2Region rgn(pos, 1);
                if (rgn.startPos >= 0 && rgn.endPos() <= seqLen) {
                    setSelection(rgn);
                }
            }
        }
    }

    cancelSelectionResizing();
    lastPressPos = -1;
    resizableRegion = U2Region();
    overlappedRegions.clear();
    QWidget::mouseReleaseEvent(me);
}

void GSequenceLineView::mouseMoveEvent(QMouseEvent* me) {
    if (!me->buttons()) {
        updateCursorShapeOnMouseMove(me->pos());
    }

    if (isSelectionResizing) {
        if (me->buttons() & Qt::LeftButton) {
            Qt::CursorShape shape = cursor().shape();
            if (shape != Qt::ArrowCursor) {
                moveBorder(me->pos());
                QWidget::mouseMoveEvent(me);
                return;
            }
        }

        if (lastPressPos == -1) {
            QWidget::mouseMoveEvent(me);
            return;
        }

        if (me->buttons() & Qt::LeftButton) {
            moveBorder(me->pos());
        }
    }
    QWidget::mouseMoveEvent(me);
}

void GSequenceLineView::mouseDoubleClickEvent(QMouseEvent* me) {
    QPoint areaPoint = toRenderAreaPoint(me->pos());
    if (renderArea->rect().contains(areaPoint)) {
        qint64 pos = renderArea->coordToPos(areaPoint);
        emit si_centerPosition(pos);
    }
    QWidget::mouseDoubleClickEvent(me);
}

void GSequenceLineView::keyPressEvent(QKeyEvent* e) {
    int key = e->key();
    bool accepted = false;
    GSequenceLineView* view = coherentRangeView == nullptr ? this : coherentRangeView;
    switch (key) {
        case Qt::Key_Left:
        case Qt::Key_Up:
            view->setStartPos(qMax(qint64(0), visibleRange.startPos - 1));
            accepted = true;
            break;
        case Qt::Key_Right:
        case Qt::Key_Down:
            view->setStartPos(qMin(seqLen - 1, visibleRange.startPos + 1));
            accepted = true;
            break;
        case Qt::Key_Home:
            view->setStartPos(0);
            accepted = true;
            break;
        case Qt::Key_End:
            view->setStartPos(seqLen - 1);
            accepted = true;
            break;
        case Qt::Key_PageUp:
            view->scrollBar->triggerAction(QAbstractSlider::SliderPageStepSub);
            accepted = true;
            break;
        case Qt::Key_PageDown:
            view->scrollBar->triggerAction(QAbstractSlider::SliderPageStepAdd);
            accepted = true;
            break;
    }
    if (accepted) {
        e->accept();
    } else {
        QWidget::keyPressEvent(e);
    }
}

U2Region GSequenceLineView::getCapturingRenderAreaYRegionForPos(qint64) const {
    // Note: using height(), but not renderArea->height() to make the whole widget Y-range capturing (the original UGENE behaviour).
    return U2Region(0, height());
}

void GSequenceLineView::updateCursorShapeOnMouseMove(const QPoint& p) {
    QPoint renderAreaPoint = toRenderAreaPoint(p);
    int capturingDistance = SelectionModificationHelper::PIXEL_OFFSET_FOR_BORDER_POINTING;
    // On screen 'capturing' X-region around the cursor in pixels.
    // If the selection border is drawn inside the cursor shape is changed to 'Resize'.
    U2Region capturingXRegion(renderAreaPoint.x() - capturingDistance, 2 * capturingDistance);
    auto isCaptured = [this, capturingXRegion, renderAreaPoint](qint64 pos) {
        U2Region yRegion = getCapturingRenderAreaYRegionForPos(pos);
        if (!yRegion.contains(renderAreaPoint.y())) {
            return false;
        }
        int xCoord = renderArea->posToCoord(pos, true);
        return visibleRange.contains(pos) && capturingXRegion.contains(xCoord);
    };
    Qt::CursorShape shape = Qt::ArrowCursor;
    QVector<U2Region> regions = ctx->getSequenceSelection()->getSelectedRegions();
    for (const U2Region& region : qAsConst(regions)) {
        if (isCaptured(region.startPos) || isCaptured(region.endPos())) {
            shape = Qt::SizeHorCursor;
            break;
        }
    }
    setCursor(shape);
}

void GSequenceLineView::setCenterPos(qint64 centerPos) {
    SAFE_POINT(centerPos <= seqLen && centerPos >= 0, QString("Center pos is out of sequence range! value: %1").arg(centerPos), );

    qint64 newPos = qMax(qint64(0), centerPos - visibleRange.length / 2);
    setStartPos(newPos);
}

void GSequenceLineView::setStartPos(qint64 newPos) {
    if (newPos + visibleRange.length > seqLen) {
        newPos = seqLen - visibleRange.length;
    }
    if (newPos < 0) {
        newPos = 0;
    }
    if (visibleRange.startPos != newPos) {
        visibleRange.startPos = newPos;
        onVisibleRangeChanged();
    }
}

void GSequenceLineView::onVisibleRangeChanged(bool signal) {
    addUpdateFlags(GSLV_UF_VisibleRangeChanged);
    updateScrollBar();
    if (signal) {
        emit si_visibleRangeChanged();
    }
    update();
}

QPoint GSequenceLineView::toRenderAreaPoint(const QPoint& p) const {
    assert(contentWidget);
    return p - contentWidget->pos();
}

void GSequenceLineView::wheelEvent(QWheelEvent* we) {
    bool renderAreaWheel = QRect(renderArea->x(), renderArea->y(), renderArea->width(), renderArea->height()).contains(we->pos());
    if (!renderAreaWheel) {
        QWidget::wheelEvent(we);
        return;
    }
    setFocus();
    bool toMin = we->delta() > 0;
    if (we->modifiers() == 0) {
        scrollBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    } else if (we->modifiers() & Qt::SHIFT) {
        GSequenceLineView* moveView = coherentRangeView == nullptr ? this : coherentRangeView;
        if (toMin && visibleRange.startPos > 0) {
            moveView->setStartPos(visibleRange.startPos - 1);
        } else if (!toMin && visibleRange.endPos() < seqLen) {
            moveView->setStartPos(visibleRange.startPos + 1);
        }
    } else if (we->modifiers() & Qt::ALT) {
        QAction* zoomAction = toMin ? getZoomInAction() : getZoomOutAction();
        if (zoomAction != nullptr) {
            zoomAction->activate(QAction::Trigger);
        }
    }
}

void GSequenceLineView::sl_onDNASelectionChanged(LRegionsSelection*, const QVector<U2Region>& added, const QVector<U2Region>& removed) {
    QWidget* prevFocusedWidget = QApplication::focusWidget();
    if (QApplication::focusWidget() != this) {
        setFocus();
        if (prevFocusedWidget != nullptr) {
            prevFocusedWidget->setFocus();
        }
    }
    if (visibleRange.intersects(added) || visibleRange.intersects(removed)) {
        addUpdateFlags(GSLV_UF_SelectionChanged);
        update();
    }
}

void GSequenceLineView::focusInEvent(QFocusEvent* fe) {
    addUpdateFlags(GSLV_UF_FocusChanged);
    QWidget::focusInEvent(fe);
}

void GSequenceLineView::focusOutEvent(QFocusEvent* fe) {
    addUpdateFlags(GSLV_UF_FocusChanged);
    QWidget::focusOutEvent(fe);
}

bool GSequenceLineView::eventFilter(QObject* object, QEvent* event) {
    if (object == frameView) {
        // show-hide frame on frameView show/hide event
        if ((isVisible() && event->type() == QEvent::Show) || event->type() == QEvent::Hide) {
            if (visibleRange.contains(frameView->getVisibleRange())) {
                addUpdateFlags(GSLV_UF_FrameChanged);
                update();
            }
        }
    }
    return false;
}

void GSequenceLineView::setFrameView(GSequenceLineView* _frameView) {
    SAFE_POINT((frameView == nullptr) != (_frameView == nullptr), "Failed to set frame view!", );

    if (_frameView == nullptr) {
        frameView->disconnect(this);
        frameView->removeEventFilter(this);
        frameView = nullptr;
        return;
    }
    frameView = _frameView;
    frameView->installEventFilter(this);
    connect(frameView, SIGNAL(si_visibleRangeChanged()), SLOT(sl_onFrameRangeChanged()));
}

void GSequenceLineView::setCoherentRangeView(GSequenceLineView* _rangeView) {
    SAFE_POINT((coherentRangeView == nullptr) != (_rangeView == nullptr), "Failed to set coherent view!", );
    if (_rangeView == nullptr) {
        coherentRangeView->disconnect(this);
        coherentRangeView = nullptr;
        return;
    }
    coherentRangeView = _rangeView;
    setVisibleRange(coherentRangeView->getVisibleRange());
    connect(coherentRangeView, SIGNAL(si_visibleRangeChanged()), SLOT(sl_onCoherentRangeViewRangeChanged()));
}

void GSequenceLineView::sl_onFrameRangeChanged() {
    SAFE_POINT(frameView != nullptr, "frameView is NULL", );
    U2Region newRangeNC = frameView->getVisibleRange();
    qint64 len = ctx->getSequenceLength();
    if (newRangeNC.endPos() > len) {
        newRangeNC.startPos = 0;
        if (newRangeNC.length > len) {
            newRangeNC.length = len;
        }
        frameView->setVisibleRange(newRangeNC);
    }
    // TODO: optimize and do not redraw frame if visual coords of the frame are not changed!
#ifdef _DEBUG
    const U2Region& newRange = frameView->getVisibleRange();
    assert(newRange.startPos >= 0 && newRange.endPos() <= ctx->getSequenceLength() && newRange.length >= 0);
#endif
    addUpdateFlags(GSLV_UF_FrameChanged);
    update();
}

void GSequenceLineView::sl_onCoherentRangeViewRangeChanged() {
    const U2Region& newRange = coherentRangeView->getVisibleRange();
    if (newRange == visibleRange) {
        return;
    }
    setVisibleRange(newRange);
}

void GSequenceLineView::sl_onLocalCenteringRequest(qint64 pos) {
    setCenterPos(pos);
}

void GSequenceLineView::setVisibleRange(const U2Region& newRange, bool signal) {
    SAFE_POINT(newRange.startPos >= 0 && newRange.endPos() <= seqLen, "Failed to update visible range. Range is out of the sequence range!", );

    if (newRange == visibleRange) {
        return;
    }
    if (featureFlags.testFlag(GSLV_FF_SupportsCustomRange)) {
        visibleRange = newRange;
        onVisibleRangeChanged(signal);
    } else if (newRange.startPos != visibleRange.startPos) {
        setStartPos(newRange.startPos);
    }
}

U2SequenceObject* GSequenceLineView::getSequenceObject() const {
    return ctx->getSequenceObject();
}

void GSequenceLineView::completeUpdate() {
    addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
    update();
}

void GSequenceLineView::sl_sequenceChanged() {
    seqLen = ctx->getSequenceLength();
    updateScrollBar();
    completeUpdate();
}

void GSequenceLineView::moveBorder(const QPoint& p) {
    QPoint areaPoint = toRenderAreaPoint(p);
    autoScrolling(areaPoint);
    resizeSelection(areaPoint);
}

void GSequenceLineView::autoScrolling(const QPoint& areaPoint) {
    if (areaPoint.x() > width()) {
        scrollBar->setupRepeatAction(QAbstractSlider::SliderSingleStepAdd);
    } else if (areaPoint.x() <= 0) {
        scrollBar->setupRepeatAction(QAbstractSlider::SliderSingleStepSub);
    } else {
        scrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
    }
}

void GSequenceLineView::cancelSelectionResizing() {
    isSelectionResizing = false;
    scrollBar->setupRepeatAction(QAbstractSlider::SliderNoAction);
}

void GSequenceLineView::resizeSelection(const QPoint& areaPoint) {
    qint64 pos = renderArea->coordToPos(areaPoint);
    QVector<U2Region> regions = ctx->getSequenceSelection()->getSelectedRegions();
    std::sort(regions.begin(), regions.end());

    if (lastPressPos == -1) {
        if (!regions.isEmpty()) {
            qint64 diffToStart = qAbs(pos - regions[0].startPos);
            qint64 diffToEnd = qAbs(pos - regions[0].endPos());
            lastPressPos = diffToStart > diffToEnd ? regions[0].startPos : regions[0].endPos();
            qint64 sizeToResizableRegion = qMin(diffToStart, diffToEnd);
            resizableRegion = regions[0];
            for (int i = 1; i < regions.size(); i++) {
                diffToStart = qAbs(pos - regions[i].startPos);
                diffToEnd = qAbs(pos - regions[i].endPos());
                const qint64 currentSizeToResizableRegion = qMin(diffToStart, diffToEnd);
                if (currentSizeToResizableRegion < sizeToResizableRegion) {
                    const qint64 tempPressPos = diffToStart > diffToEnd ? regions[i].startPos : regions[i].endPos();
                    lastPressPos = tempPressPos;
                    sizeToResizableRegion = currentSizeToResizableRegion;
                    resizableRegion = regions[i];
                }
            }
        }
    }
    regions.removeOne(resizableRegion);

    qint64 selStart = qMin(lastPressPos, pos);
    qint64 selLen = qAbs(pos - lastPressPos);
    if (selStart < 0) {
        selLen += selStart;
        selStart = 0;
    } else if (selStart + selLen > seqLen) {
        selLen = seqLen - selStart;
    }
    CHECK(selLen != 0, );

    U2Region newSelection(selStart, selLen);

    if (!resizableRegion.isEmpty()) {
        foreach (const U2Region& reg, regions) {
            if (!reg.intersect(newSelection).isEmpty()) {
                newSelection = U2Region::join(QVector<U2Region>() << newSelection << reg).first();
                if (!overlappedRegions.contains(reg)) {
                    overlappedRegions << reg;
                }
                regions.removeOne(reg);
            }
        }
    }

    if (!overlappedRegions.isEmpty()) {
        QVector<U2Region> overlappedSelection = overlappedRegions.toVector();
        overlappedSelection << newSelection;
        overlappedSelection = U2Region::join(overlappedSelection);
        if (!overlappedSelection.isEmpty()) {
            foreach (const U2Region& sel, overlappedSelection) {
                if (sel.contains(newSelection)) {
                    newSelection = sel;
                } else {
                    regions << sel;
                }
            }
            foreach (const U2Region& reg, regions) {
                if (overlappedRegions.contains(reg)) {
                    overlappedRegions.removeOne(reg);
                }
            }
        }
    }

    changeSelection(regions, newSelection);
}

void GSequenceLineView::changeSelectionOnScrollbarMoving(const U2Region& newSelection) {
    QVector<U2Region> regions = ctx->getSequenceSelection()->getSelectedRegions();
    regions.removeOne(resizableRegion);
    changeSelection(regions, newSelection);
}

void GSequenceLineView::changeSelection(QVector<U2Region>& regions, const U2Region& newSelection) {
    resizableRegion = newSelection;
    regions << newSelection;
    std::sort(regions.begin(), regions.end());
    ctx->getSequenceSelection()->setSelectedRegions(regions);
}

//////////////////////////////////////////////////////////////////////////
/// GSequenceLineViewRenderArea

GSequenceLineViewRenderArea::GSequenceLineViewRenderArea(GSequenceLineView* v)
    : QWidget(v), cachedView(new QPixmap()) {
    view = v;

    sequenceFont.setFamily("Courier New");
    sequenceFont.setPointSize(12);

    smallSequenceFont.setFamily("Courier New");
    smallSequenceFont.setPointSize(8);

    rulerFont.setFamily("Arial");
    rulerFont.setPointSize(8);

    updateFontMetrics();
}

void GSequenceLineViewRenderArea::updateFontMetrics() {
    QFontMetrics fm(sequenceFont, view);
    yCharOffset = 4;
    lineHeight = fm.boundingRect('W').height() + 2 * yCharOffset;
    xCharOffset = 1;
    charWidth = fm.boundingRect('W').width() + 2 * xCharOffset;

    QFontMetrics fms(smallSequenceFont, view);
    smallCharWidth = fms.boundingRect('W').width();
}

void GSequenceLineViewRenderArea::drawFocus(QPainter& p) {
    p.setPen(QPen(Qt::black, 1, Qt::DotLine));
    p.drawRect(0, 0, width() - 1, height() - 1);
}

void GSequenceLineViewRenderArea::drawFrame(QPainter& p) {
    GSequenceLineView* frameView = view->getFrameView();
    if (frameView == nullptr || !frameView->isVisible()) {
        return;
    }
    const U2Region& frameRange = frameView->getVisibleRange();
    if (frameRange.length == 0) {
        return;
    }
    const U2Region& visibleRange = view->getVisibleRange();
    U2Region visibleFrameRange = visibleRange.intersect(frameRange);
    if (visibleFrameRange.isEmpty()) {
        return;
    }
    double scale = getCurrentScale();
    int xStart = posToCoord(visibleFrameRange.startPos - visibleRange.startPos);
    int xLen = qMax((int)(scale * (double)visibleFrameRange.length), 4);
    QPen pen(Qt::lightGray, 2, Qt::DashLine);
    p.setPen(pen);
    p.drawRect(xStart, 0, xLen, height());
}

void GSequenceLineViewRenderArea::paintEvent(QPaintEvent* e) {
    QSize cachedViewSize = cachedView->size() * devicePixelRatio();
    QSize currentSize = size() * devicePixelRatio();
    if (cachedViewSize != currentSize) {
        view->addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
        cachedView.reset(new QPixmap(currentSize));
        cachedView->setDevicePixelRatio(devicePixelRatio());
    }

    drawAll(this);

    view->clearUpdateFlags();

    QWidget::paintEvent(e);
}

double GSequenceLineViewRenderArea::getCurrentScale() const {
    return width() / (double)view->getVisibleRange().length;
}

qint64 GSequenceLineViewRenderArea::coordToPos(const QPoint& coord) const {
    int x = qBound(0, coord.x(), width());
    const U2Region& visibleRange = view->getVisibleRange();
    double scale = getCurrentScale();
    qint64 pos = qRound64((double)visibleRange.startPos + x / scale);
    return qBound(visibleRange.startPos, pos, visibleRange.endPos());
}

int GSequenceLineViewRenderArea::posToCoord(qint64 pos, bool useVirtualSpace) const {
    const U2Region& visibleRange = view->getVisibleRange();
    bool isInVisibleRange = visibleRange.contains(pos) || pos == visibleRange.endPos();
    if (!isInVisibleRange && !useVirtualSpace) {
        return -1;
    }
    int coord = qRound((double)(pos - visibleRange.startPos) * getCurrentScale());
    SAFE_POINT(useVirtualSpace || coord <= width(), "Position is out of range!", coord);
    return coord;
}

}  // namespace U2
