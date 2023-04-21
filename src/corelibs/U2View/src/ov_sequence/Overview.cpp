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

#include "Overview.h"

#include <QPainterPath>
#include <QToolTip>
#include <QVBoxLayout>

#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GScrollBar.h>
#include <U2Gui/GraphUtils.h>

#include "ADVSingleSequenceWidget.h"
#include "DetView.h"
#include "PanView.h"

namespace U2 {

const QString Overview::ANNOTATION_GRAPH_STATE = "sequenceViewSettings/annotationGraphState";

Overview::Overview(ADVSingleSequenceWidget* p, ADVSequenceObjectContext* ctx)
    : GSequenceLineView(p, ctx),
      seqWidget(p) {
    overviewRenderArea = new OverviewRenderArea(this);
    renderArea = overviewRenderArea;
    visibleRange = U2Region(0, ctx->getSequenceLength());
    renderArea->setMouseTracking(true);
    renderArea->setObjectName("OverviewRenderArea");

    panView = p->getPanView();
    detView = p->getDetView();

    QAction* densityGraphAction = new QAction(QIcon(":core/images/sum.png"), "", this);
    densityGraphAction->setObjectName("density_graph_action");
    densityGraphAction->setCheckable(true);
    densityGraphAction->setToolTip(tr("Toggle annotation density graph"));
    addActionToLocalToolbar(densityGraphAction);

    connect(densityGraphAction, SIGNAL(triggered()), SLOT(sl_graphActionTriggered()));
    connect(panView, SIGNAL(si_visibleRangeChanged()), SLOT(sl_visibleRangeChanged()));
    connect(detView, SIGNAL(si_visibleRangeChanged()), SLOT(sl_visibleRangeChanged()));
    connect(ctx, SIGNAL(si_annotationObjectAdded(AnnotationTableObject*)), SLOT(sl_annotationObjectAdded(AnnotationTableObject*)));
    connect(ctx, SIGNAL(si_annotationObjectRemoved(AnnotationTableObject*)), SLOT(sl_annotationObjectRemoved(AnnotationTableObject*)));
    foreach (AnnotationTableObject* at, ctx->getAnnotationObjects(true)) {
        connectAnnotationTableObject(at);
    }
    connect(AppContext::getAnnotationsSettingsRegistry(), SIGNAL(si_annotationSettingsChanged(const QStringList&)), SLOT(sl_onAnnotationSettingsChanged(const QStringList&)));

    sl_visibleRangeChanged();
    bool graphState = AppContext::getSettings()->getValue(ANNOTATION_GRAPH_STATE, QVariant(false)).toBool();
    setGraphActionVisible(graphState);
    densityGraphAction->setChecked(graphState);
    pack();
}

void Overview::sl_annotationObjectAdded(AnnotationTableObject* obj) {
    connectAnnotationTableObject(obj);
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void Overview::sl_annotationObjectRemoved(AnnotationTableObject* obj) {
    disconnect(obj, nullptr, this, nullptr);
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void Overview::sl_annotationsAdded(const QList<Annotation*>& a) {
    Q_UNUSED(a);

    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void Overview::sl_annotationsRemoved(const QList<Annotation*>& a) {
    Q_UNUSED(a);

    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void Overview::sl_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*) {
    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void Overview::sl_annotationsModified(const QList<AnnotationModification>& annotationModifications) {
    foreach (const AnnotationModification& annotationModification, annotationModifications) {
        if (annotationModification.type == AnnotationModification_LocationChanged) {
            addUpdateFlags(GSLV_UF_AnnotationsChanged);
            update();
            break;
        }
    }
}

void Overview::sl_onAnnotationSettingsChanged(const QStringList& changedSettings) {
    Q_UNUSED(changedSettings);

    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
}

void Overview::sl_sequenceChanged() {
    seqLen = ctx->getSequenceLength();
    visibleRange = U2Region(0, seqLen);
    completeUpdate();
}

void Overview::pack() {
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(renderArea);
    setContentLayout(layout);
    scrollBar->setHidden(true);
    setFixedHeight(renderArea->height());
}

void Overview::sl_graphActionTriggered() {
    setGraphActionVisible(!overviewRenderArea->isGraphVisible());
}

void Overview::sl_visibleRangeChanged() {
    renderArea->update();
}

void Overview::mousePressEvent(QMouseEvent* me) {
    if (me->buttons() & Qt::LeftButton) {
        QPoint renderAreaPos = toRenderAreaPoint(me->pos());

        // make selection if shift was pressed
        if (me->modifiers().testFlag(Qt::ShiftModifier)) {
            lastPressPos = overviewRenderArea->coordXToPos(renderAreaPos.x());
            QWidget::mousePressEvent(me);
            return;
        }

        QRectF panSlider(overviewRenderArea->getPanSlider());
        QRectF detSlider(overviewRenderArea->getDetSlider());
        panSliderClicked = panSlider.contains(renderAreaPos);
        panSliderMovedRight = (panSlider.topRight().x() - renderAreaPos.x() < 10) && panSliderClicked;
        panSliderMovedLeft = (renderAreaPos.x() - panSlider.topLeft().x() < 10) && panSliderClicked;
        offset = overviewRenderArea->coordXToPos(me->pos().x()) - overviewRenderArea->coordXToPos(panSlider.left());
        // don't process detSlider when details view is collapsed
        if (seqWidget->isDetViewCollapsed()) {
            detSliderClicked = false;
        } else {
            detSliderClicked = detSlider.contains(renderAreaPos);
        }
        if (panSliderClicked && detSliderClicked) {
            panSliderClicked = false;
            detSliderClicked = true;
        }

        if (panSliderClicked) {
            mousePosToSlider = QPoint(renderAreaPos - panSlider.topLeft().toPoint());
        } else if (detSliderClicked) {
            mousePosToSlider = QPoint(renderAreaPos - detSlider.center().toPoint());
        } else {
            qint64 seqLen = ctx->getSequenceLength();
            qint64 panVisLen = panView->getVisibleRange().length;
            qint64 detVisLen = detView->getVisibleRange().length;

            qint64 panPos = overviewRenderArea->coordXToPos(renderAreaPos.x() - panSlider.width() / 2);
            panPos = qBound(qint64(0), panPos, seqLen - panVisLen);
            panView->setVisibleRange(U2Region(panPos, panVisLen));

            qint64 detPos = overviewRenderArea->coordXToPos(renderAreaPos.x());
            detPos = qBound(qint64(0), detPos, seqLen - detVisLen);
            detView->setVisibleRange(U2Region(detPos, detVisLen));
        }
    }
    QWidget::mousePressEvent(me);
}

void Overview::mouseReleaseEvent(QMouseEvent* me) {
    lastPressPos = -1;
    panSliderMovedRight = false;
    panSliderMovedLeft = false;
    QWidget::mouseReleaseEvent(me);
}

void Overview::mouseMoveEvent(QMouseEvent* me) {
    QPoint renderAreaPos = toRenderAreaPoint(me->pos());

    QRectF panSlider(overviewRenderArea->getPanSlider());
    if (((panSlider.topRight().x() - renderAreaPos.x() < 10 && panSlider.topRight().x() - renderAreaPos.x() > -5) || (renderAreaPos.x() - panSlider.topLeft().x() > -5 && renderAreaPos.x() - panSlider.topLeft().x() < 10)) && panView->isVisible()) {
        setCursor(QCursor(Qt::SizeHorCursor));
    } else {
        setCursor(QCursor(Qt::ArrowCursor));
    }

    if (me->buttons() & Qt::LeftButton) {
        qint64 pos = overviewRenderArea->coordXToPos(renderAreaPos.x() - mousePosToSlider.x());
        if (lastPressPos != -1) {  // make selection
            U2Region selection;
            qint64 mousePos = overviewRenderArea->coordXToPos(renderAreaPos.x());
            qint64 selLen = mousePos - lastPressPos;
            if (selLen > 0) {
                selection.startPos = lastPressPos;
                selection.length = selLen;
            } else {
                selection.startPos = mousePos;
                selection.length = -selLen;
            }
            setSelection(selection);
            QWidget::mouseMoveEvent(me);
            return;
        }

        if (panView->isVisible()) {
            qint64 seqLen = ctx->getSequenceLength();
            if (panSliderMovedRight) {
                qreal length = me->pos().x() - panSlider.right();
                qint64 panVisLen = overviewRenderArea->coordXToPos(qAbs(length));
                if (length < 0) {
                    panVisLen *= -1;
                }

                panVisLen = panView->getVisibleRange().length + panVisLen;
                pos = overviewRenderArea->coordXToPos(panSlider.left());
                if (panVisLen > 0 && seqLen >= (panVisLen + pos)) {
                    panView->setVisibleRange(U2Region(pos, panVisLen));
                }
            } else if (panSliderMovedLeft) {
                int length = panSlider.left() - me->pos().x();
                qint64 panVisLen = overviewRenderArea->coordXToPos(qAbs(length));
                if (length < 0) {
                    panVisLen *= -1;
                }

                panVisLen = panView->getVisibleRange().length + panVisLen + offset;
                pos = panView->getVisibleRange().endPos() - panVisLen;
                if (panVisLen > 0 && pos > 0 && (panVisLen + pos) <= seqLen) {
                    panView->setVisibleRange(U2Region(pos, panVisLen));
                }
            } else if (panSliderClicked) {
                qint64 panVisLen = panView->getVisibleRange().length;
                pos = qBound(qint64(0), qint64(pos), seqLen - panVisLen);
                panView->setVisibleRange(U2Region(pos, panVisLen));
            }
        }
        if (detSliderClicked) {
            detView->setStartPos(pos);
        }
    }
    QWidget::mouseMoveEvent(me);
}

void Overview::mouseDoubleClickEvent(QMouseEvent* me) {
    if (me->buttons() & Qt::LeftButton) {
        qint64 seqLen = ctx->getSequenceLength();
        QRectF panSlider(overviewRenderArea->getPanSlider());
        qint64 panVisLen = panView->getVisibleRange().length;
        QPoint renderAreaPos = toRenderAreaPoint(me->pos());
        qint64 panPos = overviewRenderArea->coordXToPos(renderAreaPos.x() - panSlider.width() / 2);
        panPos = qBound(qint64(0), qint64(panPos), seqLen - panVisLen);
        panView->setVisibleRange(U2Region(panPos, panVisLen));

        // don't process detSlider when details view is collapsed
        if (!seqWidget->isDetViewCollapsed()) {
            qint64 detPos = overviewRenderArea->coordXToPos(renderAreaPos.x());
            detView->setStartPos(detPos);
        }

        panSliderClicked = false;
        detSliderClicked = false;
        panSliderMovedRight = false;
        panSliderMovedLeft = false;
    }
    QWidget::mouseDoubleClickEvent(me);
}

void Overview::wheelEvent(QWheelEvent* we) {
    bool renderAreaWheel = QRect(renderArea->x(), renderArea->y(), renderArea->width(), renderArea->height()).contains(we->pos());
    if (!renderAreaWheel) {
        QWidget::wheelEvent(we);
        return;
    }
    setFocus();
    bool toMin = we->delta() > 0;
    QAction* zoomAction = toMin ? panView->getZoomInAction() : panView->getZoomOutAction();
    if (zoomAction != nullptr) {
        zoomAction->activate(QAction::Trigger);
    }
}

bool Overview::event(QEvent* e) {
    if (e->type() == QEvent::ToolTip) {
        auto helpEvent = static_cast<QHelpEvent*>(e);
        int x = overviewRenderArea->mapFrom(this, helpEvent->pos()).x();
        QString tip = createToolTip(x);
        if (!tip.isEmpty()) {
            QToolTip::showText(helpEvent->globalPos(), tip);
        }
    }
    return GSequenceLineView::event(e);
}

QString Overview::createToolTip(int renderAreaXOffset) {
    int halfChar = overviewRenderArea->getCurrentScale() / 2;
    qint64 pos = overviewRenderArea->coordXToPos(renderAreaXOffset + halfChar);
    qint64 pos2 = overviewRenderArea->coordXToPos(renderAreaXOffset + halfChar + 1);
    qint64 delta = 0;
    if (pos2 - 1 > pos) {
        delta = pos2 - pos - 1;
    }
    QString tip = tr("Position ") + QString::number(pos);
    if (delta != 0) {
        tip += ".." + QString::number(pos + delta);
    }
    if (overviewRenderArea->isGraphVisible()) {
        int density = overviewRenderArea->getAnnotationDensity(pos);
        for (int i = pos; i <= pos + delta; ++i) {
            int nextPosDensity = overviewRenderArea->getAnnotationDensity(i);
            density = qMax(density, nextPosDensity);
        }
        tip += "\n" + tr("Annotation density ") + QString::number(density);
    }
    return tip;
}

PanView* Overview::getPan() const {
    return panView;
}

DetView* Overview::getDet() const {
    return detView;
}

void Overview::connectAnnotationTableObject(AnnotationTableObject* object) {
    CHECK(object != nullptr, );
    connect(object, SIGNAL(si_onAnnotationsAdded(const QList<Annotation*>&)), SLOT(sl_annotationsAdded(const QList<Annotation*>&)));
    connect(object, SIGNAL(si_onAnnotationsRemoved(const QList<Annotation*>&)), SLOT(sl_annotationsRemoved(const QList<Annotation*>&)));
    connect(object, SIGNAL(si_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*)), SLOT(sl_onAnnotationsInGroupRemoved(const QList<Annotation*>&, AnnotationGroup*)));
    connect(object, SIGNAL(si_onAnnotationsModified(const QList<AnnotationModification>&)), SLOT(sl_annotationsModified(const QList<AnnotationModification>&)));
}

void Overview::setGraphActionVisible(const bool setVisible) {
    CHECK(overviewRenderArea->isGraphVisible() != setVisible, );

    AppContext::getSettings()->setValue(ANNOTATION_GRAPH_STATE, QVariant(setVisible));
    overviewRenderArea->setGraphVisibility(setVisible);
    addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
    update();
}

//////////////////////////////////////////////////////////////////////////
/// GlobalViewRenderArea
#define ANNOTATION_GRAPH_HEIGHT 9
#define RULER_NOTCH_SIZE 2
#define SLIDER_MIN_WIDTH 3
#define ARROW_WIDTH 12
#define ARROW_HEIGHT 10
#define ARROW_TOP_PAD ANNOTATION_GRAPH_HEIGHT + 2 * RULER_NOTCH_SIZE
#define PEN_WIDTH 1

#define RENDER_AREA_HEIGHT lineHeight + ANNOTATION_GRAPH_HEIGHT
OverviewRenderArea::OverviewRenderArea(Overview* p)
    : GSequenceLineViewRenderArea(p) {
    setFixedHeight(RENDER_AREA_HEIGHT);
    QLinearGradient gradient(0, 0, 0, 1);  // vertical
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0.00, QColor(255, 255, 255, 120));
    gradient.setColorAt(0.50, QColor(0, 0, 0, 0));
    gradient.setColorAt(0.70, QColor(0, 0, 0, 0));
    gradient.setColorAt(1.00, QColor(0, 0, 0, 70));
    gradientMaskBrush = QBrush(gradient);
    graphVisible = false;
}

const QRectF OverviewRenderArea::getPanSlider() const {
    return panSlider;
}

const QRectF OverviewRenderArea::getDetSlider() const {
    return detSlider;
}

// pos [1, seqLen]; annotationsOnPos [0, seqLen)
int OverviewRenderArea::getAnnotationDensity(int pos) const {
    if (pos < 1 || pos > annotationsOnPos.size()) {
        return 0;
    }
    return annotationsOnPos.at(pos - 1);
}

void OverviewRenderArea::setGraphVisibility(const bool isVisible) {
    graphVisible = isVisible;
}

bool OverviewRenderArea::isGraphVisible() const {
    return graphVisible;
}

void OverviewRenderArea::setAnnotationsOnPos() {
    annotationsOnPos.clear();
    const SequenceObjectContext* ctx = view->getSequenceContext();
    const qint64 len = ctx->getSequenceLength();
    annotationsOnPos.resize(len);

    const U2Region sequenceRange(0, ctx->getSequenceObject()->getSequenceLength());
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    QSet<AnnotationTableObject*> aObjs = ctx->getAnnotationObjects(true);

    for (AnnotationTableObject* at : qAsConst(aObjs)) {
        foreach (Annotation* a, at->getAnnotations()) {
            const SharedAnnotationData& ad = a->getData();
            const AnnotationSettings* as = asr->getAnnotationSettings(ad);
            if (as->visible) {
                QVector<U2Region> regions = ad->getRegions();
                for (const U2Region& r : qAsConst(regions)) {
                    const U2Region innerRegion = r.intersect(sequenceRange);
                    for (qint64 i = innerRegion.startPos; i < innerRegion.endPos(); i++) {
                        annotationsOnPos[i]++;
                    }
                }
            }
        }
    }
}

void OverviewRenderArea::drawAll(QPaintDevice* pd) {
    QPen pen(Qt::SolidLine);
    pen.setWidth(PEN_WIDTH);
    GSLV_UpdateFlags uf = view->getUpdateFlags();
    bool completeRedraw = uf.testFlag(GSLV_UF_NeedCompleteRedraw) || uf.testFlag(GSLV_UF_AnnotationsChanged) || uf.testFlag(GSLV_UF_ViewResized);
    if (completeRedraw) {
        QPainter pCached(getCachedPixmap());
        pCached.fillRect(0, 0, pd->width(), pd->height(), Qt::white);
        if (graphVisible) {
            setAnnotationsOnPos();
            drawGraph(pCached);
        }
        pCached.end();
    }

    QPainter p(pd);
    p.drawPixmap(0, 0, *cachedView);
    auto gv = static_cast<Overview*>(view);
    int panX = posToCoord(gv->getPan()->getVisibleRange().startPos);
    int panW = qMax(posToCoord(gv->getPan()->getVisibleRange().length), SLIDER_MIN_WIDTH);
    int detX = posToCoord(gv->getDet()->getVisibleRange().startPos) - int(ARROW_WIDTH / 2);

    int panSliderHeight = pd->height() - PEN_WIDTH;
    int panSliderTop = 0;
    if (graphVisible) {
        panSliderHeight -= ANNOTATION_GRAPH_HEIGHT;
        panSliderTop += ANNOTATION_GRAPH_HEIGHT;
    }
    panSlider.setRect(panX, panSliderTop, panW - PEN_WIDTH, panSliderHeight);
    detSlider.setRect(detX, ARROW_TOP_PAD, ARROW_WIDTH, ARROW_HEIGHT);

    pen.setColor(Qt::darkGray);
    p.setPen(pen);

    // don't show arrow when det view collapsed
    auto overview = qobject_cast<Overview*>(view);
    SAFE_POINT(overview != nullptr, tr("Overview is NULL"), );
    ADVSingleSequenceWidget* ssw = overview->seqWidget;
    SAFE_POINT(ssw != nullptr, tr("ADVSingleSequenceWidget is NULL"), );
    if (!ssw->isPanViewCollapsed()) {
        drawSlider(p, panSlider, QColor(230, 230, 230));
    }

    if (!ssw->isDetViewCollapsed()) {
        drawArrow(p, detSlider, QColor(255, 187, 0));
    }

    drawRuler(p);
    drawSelection(p);
}

void OverviewRenderArea::drawSlider(QPainter& p, QRectF rect, QColor col) {
    QPainterPath path;
    path.addRect(rect);
    path.setFillRule(Qt::WindingFill);
    p.fillPath(path, col);
    p.fillPath(path, gradientMaskBrush);
    p.drawPath(path);
}

void OverviewRenderArea::drawArrow(QPainter& p, QRectF rect, QColor col) {
    QPainterPath arrPath;
    arrPath.moveTo(rect.bottomLeft());
    arrPath.lineTo(rect.center().x(), rect.center().y() - rect.width() / 2);
    arrPath.lineTo(rect.bottomRight());
    arrPath.closeSubpath();
    arrPath.setFillRule(Qt::WindingFill);
    p.fillPath(arrPath, col);
    p.fillPath(arrPath, gradientMaskBrush);
    p.drawPath(arrPath);
}

void OverviewRenderArea::drawRuler(QPainter& p) {
    p.save();
    QPen pen(Qt::black);
    pen.setWidth(PEN_WIDTH);
    p.setPen(pen);
    auto gv = static_cast<Overview*>(view);
    qint64 seqLen = gv->ctx->getSequenceLength();
    U2Region visibleRange = gv->getVisibleRange();

    float halfChar = getCurrentScale() / 2;
    int firstCharCenter = qRound(posToCoord(visibleRange.startPos) + halfChar);
    int lastCharCenter = qRound(posToCoord(visibleRange.endPos() - 1) + halfChar);
    int firstLastWidth = lastCharCenter - firstCharCenter;
    if (qRound(halfChar) == 0) {
        firstLastWidth--;  // make the end of the ruler visible
    }
    GraphUtils::RulerConfig c;
    if (graphVisible) {
        c.singleSideNotches = true;
    }
    c.notchSize = RULER_NOTCH_SIZE;
    GraphUtils::drawRuler(p, QPoint(firstCharCenter, ANNOTATION_GRAPH_HEIGHT), firstLastWidth, 1, seqLen, rulerFont, c);
    p.restore();
}

#define SELECTION_LINE_WIDTH 3
void OverviewRenderArea::drawSelection(QPainter& p) {
    QPen pen(QColor("#007DE3"));
    pen.setWidth(SELECTION_LINE_WIDTH);
    p.setPen(pen);
    auto gv = qobject_cast<Overview*>(view);
    DNASequenceSelection* sel = gv->ctx->getSequenceSelection();
    foreach (const U2Region& r, sel->getSelectedRegions()) {
        int x1 = posToCoord(r.startPos);
        int x2 = posToCoord(r.endPos());
        p.drawLine(x1, ANNOTATION_GRAPH_HEIGHT, x2, ANNOTATION_GRAPH_HEIGHT);
    }
}

void OverviewRenderArea::drawGraph(QPainter& p) {
    p.save();
    QPen graphPen;
    graphPen.setWidth(1);
    p.fillRect(0, 0, width() - PEN_WIDTH, ANNOTATION_GRAPH_HEIGHT - PEN_WIDTH, Qt::white);

    int halfChar = getCurrentScale() / 2;
    for (int x = 0; x < width(); x++) {
        int count;
        qint64 pos1 = coordXToPos(x + halfChar);
        if (pos1 < 1 || pos1 > annotationsOnPos.size() + 1) {
            continue;
        }
        if (coordXToPos(x + 1) > pos1) {
            count = annotationsOnPos.at(pos1 - 1);
        } else {
            qint64 pos2 = coordXToPos(x) - 1;
            count = annotationsOnPos.at(pos1 - 1);
            for (int pos = pos1; pos < pos2; pos++) {
                int nextCount = annotationsOnPos.at(pos - 1);
                count = qMax(count, nextCount);
            }
        }
        QColor col = getUnitColor(count);
        graphPen.setColor(col);
        p.setPen(graphPen);
        p.drawLine(x, 0, x, ANNOTATION_GRAPH_HEIGHT);
    }
    p.restore();
}

QColor OverviewRenderArea::getUnitColor(int count) {
    switch (count) {
        case 0:
            return QColor(0xFF, 0xFF, 0xFF);
        case 1:
            return QColor(0xCC, 0xCC, 0xCC);
        case 2:
        case 3:
            return QColor(0x66, 0x66, 0x66);
        default:
            return QColor(0x00, 0x00, 0x00);
    }
}
}  // namespace U2
