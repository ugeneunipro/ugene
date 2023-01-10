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

#include "GSequenceGraphView.h"

#include <QMessageBox>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GScrollBar.h>

#include <U2View/ADVAnnotationCreation.h>

#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "SaveGraphCutoffsDialogController.h"

namespace U2 {

GSequenceGraphView::GSequenceGraphView(QWidget* p, SequenceObjectContext* ctx, GSequenceLineView* _baseView, const QString& _vName)
    : GSequenceLineView(p, ctx),
      baseView(_baseView),
      graphViewName(_vName),
      graphDrawer(nullptr) {
    GCOUNTER(cvar, "GSequenceGraphView");
    assert(baseView);

    visualPropertiesAction = new QAction(tr("Graph settings..."), this);
    visualPropertiesAction->setObjectName("visual_properties_action");

    connect(visualPropertiesAction, SIGNAL(triggered(bool)), SLOT(sl_onShowVisualProperties(bool)));

    saveGraphCutoffsAction = new QAction(tr("Save cutoffs as annotations..."), this);
    saveGraphCutoffsAction->setObjectName("save_cutoffs_as_annotation");

    connect(saveGraphCutoffsAction, SIGNAL(triggered(bool)), SLOT(sl_onSaveGraphCutoffs(bool)));

    deleteAllLabelsAction = new QAction(tr("Delete all labels"), this);
    deleteAllLabelsAction->setObjectName("delete_all_labels");

    connect(deleteAllLabelsAction, SIGNAL(triggered()), SLOT(sl_onDeleteAllLabels()));

    showLocalMinMaxLabelsAction = new QAction(tr("Show labels for local min/max points..."), this);
    showLocalMinMaxLabelsAction->setObjectName("show_labels_for_min_max_points");
    connect(showLocalMinMaxLabelsAction, SIGNAL(triggered()), SLOT(sl_showLocalMinMaxLabels()));

    scrollBar->setDisabled(true);
    renderArea = new GSequenceGraphViewRA(this);

    renderArea->setMouseTracking(true);
    setMouseTracking(true);

    visibleRange = baseView->getVisibleRange();
    setCoherentRangeView(baseView);
    setFrameView(baseView->getFrameView());

    // process double clicks as centering requests
    ADVSingleSequenceWidget* ssw = baseView == nullptr ? nullptr : qobject_cast<ADVSingleSequenceWidget*>(baseView->parentWidget());
    if (ssw != nullptr) {
        connect(this, SIGNAL(si_centerPosition(qint64)), ssw, SLOT(sl_onLocalCenteringRequest(qint64)));
    }

    pack();
}

void GSequenceGraphView::setGraphDrawer(GSequenceGraphDrawer* gd) {
    SAFE_POINT(graphDrawer == nullptr, "GSequenceGraphDrawer was already set", );
    graphDrawer = gd;
    connect(graphDrawer, SIGNAL(si_graphDataUpdated()), renderArea, SLOT(update()));
    update();
}
void GSequenceGraphView::mousePressEvent(QMouseEvent* me) {
    setFocus();

    if (me->modifiers() == Qt::ShiftModifier && me->button() == Qt::LeftButton) {
        float sequencePos = toRenderAreaPoint(me->pos()).x() / renderArea->getCurrentScale() + getVisibleRange().startPos;
        int capturingDistancePx = 4;
        float posDeviation = capturingDistancePx * (float)getVisibleRange().length / getGraphRenderArea()->getGraphRect().width();
        for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
            GraphLabel* label = graph->labels.findLabelByPosition(sequencePos, posDeviation);
            if (label != nullptr) {
                graph->labels.removeLabel(label);
                continue;
            }
            graph->labels.addLabel(new GraphLabel(sequencePos, renderArea));
        }
    }
    GSequenceLineView::mousePressEvent(me);
}

void GSequenceGraphView::mouseMoveEvent(QMouseEvent* me) {
    setFocus();
    updateMovingLabels();
    GSequenceLineView::mouseMoveEvent(me);
}

void GSequenceGraphView::leaveEvent(QEvent*) {
    updateMovingLabels();
}

void GSequenceGraphView::getSavedLabelsState(QList<QVariant>& savedLabels) {
    // TODO: save/restore labels from all graphs.
    graphs.at(0)->labels.getLabelPositions(savedLabels);
}

void GSequenceGraphView::setLabelsFromSavedState(const QList<QVariant>& savedLabels) {
    CHECK(!graphs.isEmpty(), );
    // Labels are stored for the first graph only today.
    const QSharedPointer<GSequenceGraphData>& graph = graphs[0];
    graph->labels.deleteAllLabels();
    for (const QVariant& savedLabel : qAsConst(savedLabels)) {
        float pos = savedLabel.toFloat();
        graph->labels.addLabel(new GraphLabel(pos, renderArea));
    }
    update();
}

void GSequenceGraphView::updateMovingLabels() {
    QPoint areaPoint = renderArea->mapFromGlobal(QCursor::pos());
    QRect rect = getGraphRenderArea()->getGraphRect();
    bool isMouseInsideRenderArea = rect.contains(areaPoint);
    float sequencePos = isMouseInsideRenderArea ? areaPoint.x() / renderArea->getCurrentScale() + getVisibleRange().startPos : -1;
    for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
        GraphLabel* label = graph->labels.getMovingLabel();
        label->setPosition(sequencePos);
    }
    graphDrawer->updateMovingLabels(graphs, getGraphRenderArea()->getGraphRect());
    for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
        GraphLabel* label = graph->labels.getMovingLabel();
        if (!label->isHidden()) {
            label->raise();
        }
    }
}

void GSequenceGraphView::pack() {
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget(renderArea);

    // use extra layout with scrollbar to have synchronized width with pan-view
    // in future fake-scroll-bar can be replaced with graph specific toolbar
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    QScrollBar* fakeBar = new QScrollBar(Qt::Vertical, this);
    fakeBar->setDisabled(true);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(fakeBar);

    setContentLayout(hLayout);

    scrollBar->setHidden(true);  // todo: support mode without scrollbar at all??

    setMinimumHeight(140);
}

void GSequenceGraphView::addGraph(const QSharedPointer<GSequenceGraphData>& graph) {
    // TODO: design flow: moving label is already created but has no valid parent.
    graph->labels.getMovingLabel()->setParent(renderArea);
    graphs.append(graph);
}

/**
 * Inserts the "Graphs" popup menu into the common popup menu.
 *
 * @param menu Common Sequence View popup menu.
 */
void GSequenceGraphView::buildPopupMenu(QMenu& menu) {
    // Verify the cursor position
    QPoint cursorPosition = renderArea->mapFromGlobal(QCursor::pos());
    if (!renderArea->rect().contains(cursorPosition)) {
        return;
    }

    // Verify that the menu is not empty (to get the top element - see below)
    SAFE_POINT(!menu.actions().isEmpty(),
               "Internal error: menu is not empty during insertion of the Graph menu!", );

    // Creating the Graphs menu
    QMenu* graphMenu = new QMenu(tr("Graph"));
    graphMenu->setIcon(QIcon(":core/images/graphs.png"));
    graphMenu->menuAction()->setObjectName("Graph");

    addActionsToGraphMenu(graphMenu);

    // Prepend the Graph menu.
    QAction* menuBeginning = *(menu.actions().begin());
    menu.insertMenu(menuBeginning, graphMenu);
    menu.insertSeparator(menuBeginning);
}

/**
 * Adds actions to the graphs menu
 */
void GSequenceGraphView::addActionsToGraphMenu(QMenu* graphMenu) {
    graphMenu->addAction(visualPropertiesAction);
    if (graphs.size() == 1) {
        graphMenu->addAction(saveGraphCutoffsAction);
    }
    graphMenu->addAction(deleteAllLabelsAction);
    graphMenu->addAction(showLocalMinMaxLabelsAction);
}

void GSequenceGraphView::sl_onShowVisualProperties(bool) {
    graphDrawer->showSettingsDialog();
}

void GSequenceGraphView::sl_onDeleteAllLabels() {
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
        graph->labels.deleteAllLabels();
    }
}

void GSequenceGraphView::sl_showLocalMinMaxLabels() {
    QVector<U2Region> regionsToAnnotate = getSequenceContext()->getSequenceSelection()->getSelectedRegions();
    if (regionsToAnnotate.isEmpty()) {
        regionsToAnnotate.append(getVisibleRange());
    }
    const QRect& graphRect = getGraphRenderArea()->getGraphRect();
    for (const QSharedPointer<GSequenceGraphData>& graph : graphs) {
        for (const U2Region& region : qAsConst(regionsToAnnotate)) {
            graphDrawer->addLabelsForLocalMinMaxPoints(graph, region, graphRect);
        }
    }
}

void GSequenceGraphView::sl_onSaveGraphCutoffs(bool) {
    QObjectScopedPointer<SaveGraphCutoffsDialogController> d = new SaveGraphCutoffsDialogController(graphs.first(), graphDrawer->getCutOffState(), this, ctx);
    d->exec();
}

GSequenceGraphViewRA* GSequenceGraphView::getGraphRenderArea() const {
    return static_cast<GSequenceGraphViewRA*>(renderArea);
}

const QString& GSequenceGraphView::getGraphViewName() const {
    return graphViewName;
}

const QList<QSharedPointer<GSequenceGraphData>>& GSequenceGraphView::getGraphs() const {
    return graphs;
}

GSequenceGraphDrawer* GSequenceGraphView::getGraphDrawer() const {
    return graphDrawer;
}

//////////////////////////////////////////////////////////////////////////
// GSequenceGraphViewRA
GSequenceGraphViewRA::GSequenceGraphViewRA(GSequenceGraphView* view)
    : GSequenceLineViewRenderArea(view), headerFont("Courier", 10) {
    setObjectName("GSequenceGraphViewRenderArea");
}

void GSequenceGraphViewRA::drawAll(QPaintDevice* pd) {
    // todo: use cached view here!!

    QPainter p(pd);
    p.fillRect(0, 0, pd->width(), pd->height(), Qt::white);
    p.setPen(Qt::black);

    graphRect = QRect(1, headerHeight + 1, pd->width() - 2, pd->height() - headerHeight - 2);

    if (view->hasFocus()) {
        drawFocus(p);
    }

    drawHeader(p);

    const QList<QSharedPointer<GSequenceGraphData>>& graphs = getGraphView()->getGraphs();
    GSequenceGraphDrawer* drawer = getGraphView()->getGraphDrawer();
    drawer->draw(p, graphs, graphRect);

    drawFrame(p);
    drawSelection(p);
}

const QRect& GSequenceGraphViewRA::getGraphRect() const {
    return graphRect;
}

GSequenceGraphView* GSequenceGraphViewRA::getGraphView() const {
    return static_cast<GSequenceGraphView*>(view);
}

void GSequenceGraphViewRA::drawHeader(QPainter& p) {
    p.setFont(headerFont);

    const U2Region& visibleRange = view->getVisibleRange();

    GSequenceGraphDrawer* drawer = getGraphView()->getGraphDrawer();
    QString text = GSequenceGraphView::tr("%1 [%2, %3], Window: %4, Step %5")
                       .arg(getGraphView()->getGraphViewName())
                       .arg(QString::number(visibleRange.startPos + 1))
                       .arg(QString::number(visibleRange.endPos()))
                       .arg(QString::number(drawer->getWindow()))
                       .arg(QString::number(drawer->getStep()));
    QRect rect(1, 1, cachedView->width() - 2, headerHeight - 2);
    p.drawText(rect, Qt::AlignLeft, text);
}

double GSequenceGraphViewRA::getCurrentScale() const {
    return double(graphRect.width()) / view->getVisibleRange().length;
}

void GSequenceGraphViewRA::drawSelection(QPainter& p) {
    const QVector<U2Region>& selection = view->getSequenceContext()->getSequenceSelection()->getSelectedRegions();
    if (selection.isEmpty()) {
        return;
    }
    const U2Region& visibleRange = view->getVisibleRange();
    QPen pen1(Qt::darkGray, 1, Qt::SolidLine);
    foreach (const U2Region& r, selection) {
        if (!visibleRange.intersects(r)) {
            continue;
        }
        int x1 = graphRect.left() + qMax(0, posToCoord(r.startPos, true));
        int x2 = graphRect.left() + qMin(cachedView->width(), posToCoord(r.endPos(), true));

        p.setPen(pen1);
        if (visibleRange.contains(r.startPos)) {
            p.drawLine(x1, graphRect.top(), x1, graphRect.bottom());
        }
        if (visibleRange.contains(r.endPos())) {
            p.drawLine(x2, graphRect.top(), x2, graphRect.bottom());
        }
    }
}

}  // namespace U2
