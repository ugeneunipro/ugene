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

#include <QMessageBox>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationDialog.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/GScrollBar.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVAnnotationCreation.h>

#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "GSequenceGraphView.h"
#include "GraphLabelsSelectDialog.h"
#include "SaveGraphCutoffsDialogController.h"

namespace U2 {

GSequenceGraphView::GSequenceGraphView(QWidget* p, SequenceObjectContext* ctx,
                                       GSequenceLineView* _baseView, const QString& _vName)
    : GSequenceLineView(p, ctx),
      baseView(_baseView),
      vName(_vName),
      graphDrawer(NULL)
{
    GCOUNTER(cvar, tvar, "GSequenceGraphView");
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

    selectAllExtremumPoints = new QAction(tr("Select all extremum points..."), this);
    selectAllExtremumPoints->setObjectName("select_all_extremum_points");

    connect(selectAllExtremumPoints, SIGNAL(triggered()), SLOT(sl_onSelectExtremumPoints()));

    scrollBar->setDisabled(true);
    renderArea = new GSequenceGraphViewRA(this);

    renderArea->setMouseTracking(true);
    setMouseTracking(true);

    connect(static_cast<GSequenceGraphViewRA*>(renderArea), SIGNAL(si_graphRectChanged(const QRect&)),
        this, SLOT(sl_graphRectChanged(const QRect&)));

    visibleRange = baseView->getVisibleRange();
    setCoherentRangeView(baseView);
    setFrameView(baseView->getFrameView());

    //process double clicks as centering requests
    ADVSingleSequenceWidget* ssw = baseView == NULL ? NULL : qobject_cast<ADVSingleSequenceWidget*>(baseView->parentWidget());
    if (ssw != NULL) {
        connect(this, SIGNAL(si_centerPosition(qint64)), ssw, SLOT(sl_onLocalCenteringRequest(qint64)));
    }

    pack();
}

void GSequenceGraphView::setGraphDrawer(GSequenceGraphDrawer* gd) {
    graphDrawer = gd;
    update();
}
void GSequenceGraphView::mousePressEvent(QMouseEvent *me) {
    setFocus();

    if(me->modifiers() == Qt::ShiftModifier && me->button() == Qt::LeftButton) {
        float pos = toRenderAreaPoint(me->pos()).x() / renderArea->getCurrentScale() + getVisibleRange().startPos;
        addLabel(pos);
    }
    GSequenceLineView::mousePressEvent(me);
}
void GSequenceGraphView::mouseMoveEvent(QMouseEvent *me) {
    setFocus();
    QPoint areaPoint = toRenderAreaPoint(me->pos());
    QRect rect = static_cast<GSequenceGraphViewRA*>(renderArea)->getGraphRect();
    if (rect.contains(areaPoint)) {
        float pos = static_cast<double>(areaPoint.x()) / renderArea->getCurrentScale() + getVisibleRange().startPos;
        moveLabel(pos);

        update();
    }
    update();
    GSequenceLineView::mouseMoveEvent(me);
}
void GSequenceGraphView::leaveEvent(QEvent * /*le*/) {
    hideLabel();
}

void GSequenceGraphView::addLabel(float xPos) {
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
        if(NULL != graph->graphLabels.findLabelByPosition(xPos)) {
            continue;
        }
        GraphLabel* newLabel = new GraphLabel(xPos, renderArea);
        newLabel->show();

        graph->graphLabels.addLabel(newLabel);
        emit si_labelAdded(graph, newLabel, static_cast<GSequenceGraphViewRA*>(renderArea)->getGraphRect());
    }
}
void GSequenceGraphView::getLabelPositions(QList<QVariant> &labelPositions) {
    graphs.at(0)->graphLabels.getLabelPositions(labelPositions);
}
void GSequenceGraphView::createLabelsOnPositions(const QList<QVariant>& positions) {
    for (int i = 0; i < graphs.size(); ++i) {
        foreach(const QVariant &val, positions) {
            float pos = val.value<float>();
            addLabel(pos);
        }
    }
}
void GSequenceGraphView::moveLabel(float xPos) {
    GraphLabel* prevLabel = NULL;
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
        GraphLabel& label = graph->graphLabels.getMovingLabel();
        label.setPosition(xPos);
        label.show();
        label.raise();
        label.attachedLabel = prevLabel;
        setMouseTracking(false);
        emit si_labelMoved(graph, &label, static_cast<GSequenceGraphViewRA*>(renderArea)->getGraphRect());
        setMouseTracking(true);
        prevLabel = &label;
    }
}
void GSequenceGraphView::changeLabelsColor() {
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
        graph->graphLabels.getMovingLabel().hide();
        emit si_labelsColorChange(graph);
    }
}
void GSequenceGraphView::hideLabel() {
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs)
        graph->graphLabels.getMovingLabel().hide();
}

void GSequenceGraphView::pack() {
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget(renderArea);

    // use extra layout with scrollbar to have synchronized width with pan-view
    // in future fake-scroll-bar can be replaced with graph specific toolbar
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    QScrollBar* fakeBar = new QScrollBar(Qt::Vertical, this);
    fakeBar->setDisabled(true);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(fakeBar);

    setContentLayout(hLayout);

    scrollBar->setHidden(true); //todo: support mode without scrollbar at all??

    setMinimumHeight(140);
}

void GSequenceGraphView::addGraphData(const QSharedPointer<GSequenceGraphData> &g) {
    assert(!graphs.contains(g));
    g->graphLabels.getMovingLabel().setParent(renderArea);
    graphs.append(g);
}


/**
 * Inserts the "Graphs" popup menu into the common popup menu.
 *
 * @param menu Common Sequence View popup menu.
 */
void GSequenceGraphView::buildPopupMenu(QMenu& menu)
{
    // Verify the cursor position
    QPoint cursorPosition = renderArea->mapFromGlobal(QCursor::pos());
    if (!renderArea->rect().contains(cursorPosition))
    {
        return;
    }

    // Verify that the menu is not empty (to get the top element - see below)
    SAFE_POINT(!menu.actions().isEmpty(),
        "Internal error: menu is not empty during insertion of the Graph menu!",);

    // Creating the Graphs menu
    QMenu* graphMenu = new QMenu(tr("Graph"));
    graphMenu->setIcon(QIcon(":core/images/graphs.png"));
    graphMenu->menuAction()->setObjectName("Graph");

    addActionsToGraphMenu(graphMenu);


    // Inserting the Graphs menu at the top
    QAction *menuBeginning = *(menu.actions().begin());
    menu.insertMenu(menuBeginning, graphMenu);
    menu.insertSeparator(menuBeginning);
}

/**
 * Adds actions to the graphs menu
 */
void GSequenceGraphView::addActionsToGraphMenu(QMenu* graphMenu)
{
    graphMenu->addAction(visualPropertiesAction);
    if(graphs.size() == 1){
        graphMenu->addAction(saveGraphCutoffsAction);
    }
    graphMenu->addAction(deleteAllLabelsAction);
    graphMenu->addAction(selectAllExtremumPoints);
}


void GSequenceGraphView::sl_onShowVisualProperties(bool) {
    graphDrawer->showSettingsDialog();
}

void GSequenceGraphView::sl_onDeleteAllLabels() {
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
        graph->graphLabels.deleteAllLabels();
    }
}

void GSequenceGraphView::sl_onSelectExtremumPoints() {
    const QRect &graphRect = static_cast<GSequenceGraphViewRA*>(renderArea)->getGraphRect();
    QObjectScopedPointer<GraphLabelsSelectDialog> dlg = new GraphLabelsSelectDialog(getSequenceLength(), this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        int windowSize = dlg->getWindowSize();
        bool usingIntervals = dlg->isUsedIntervals();
        const QVector<U2Region>& selection = getSequenceContext()->getSequenceSelection()->getSelectedRegions();
        foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
            if(true == usingIntervals) {
                foreach(const U2Region& selectedRegion, selection) {
                    graphDrawer->selectExtremumPoints(graph, graphRect, windowSize, selectedRegion);
                }
            }
            else
                graphDrawer->selectExtremumPoints(graph, graphRect, windowSize, getVisibleRange());
        }
    }
}


void GSequenceGraphView::sl_onSaveGraphCutoffs( bool ){
    QObjectScopedPointer<SaveGraphCutoffsDialogController> d = new SaveGraphCutoffsDialogController(graphDrawer, graphs.first(), this, ctx);
    d->exec();
}

void GSequenceGraphView::sl_graphRectChanged(const QRect& rect) {
    foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
        emit si_frameRangeChanged(graph, rect);
    }
}

void GSequenceGraphView::onVisibleRangeChanged(bool signal) {
    if(signal) {
        foreach (const QSharedPointer<GSequenceGraphData> graph, graphs) {
            emit si_frameRangeChanged(graph, static_cast<GSequenceGraphViewRA*>(renderArea)->getGraphRect());
            GraphLabel& cursorLabel = graph->graphLabels.getMovingLabel();
            if (!cursorLabel.isHidden()) { // do not move cursor label if it was hidden for some reason (widget is not focused, etc...)
                float pos = static_cast<double>(cursorLabel.getCoord().x()) / renderArea->getCurrentScale() + getVisibleRange().startPos;
                cursorLabel.setPosition(pos);
                emit si_labelMoved(graph, &cursorLabel, static_cast<GSequenceGraphViewRA*>(renderArea)->getGraphRect());
            }
        }
    }
    GSequenceLineView::onVisibleRangeChanged(signal);
}



//////////////////////////////////////////////////////////////////////////
// RA
GSequenceGraphViewRA::GSequenceGraphViewRA(GSequenceGraphView* g)
    : GSequenceLineViewRenderArea(g),
      gd(NULL)
{
    setObjectName( "GSequenceGraphViewRenderArea" );
    headerFont=  new QFont("Courier", 10);
    headerHeight = 20;
}

GSequenceGraphViewRA::~GSequenceGraphViewRA() {
    delete headerFont;
}

void GSequenceGraphViewRA::drawAll(QPaintDevice* pd) {
    //todo: use cached view here!!

    QPainter p(pd);
    p.fillRect(0, 0, pd->width(), pd->height(), Qt::white);
    p.setPen(Qt::black);

    if (graphRect != QRect(1, headerHeight + 1, pd->width() - 2, pd->height() - headerHeight - 2)) {
        graphRect = QRect(1, headerHeight + 1, pd->width() - 2, pd->height() - headerHeight - 2);
        emit si_graphRectChanged(graphRect);
    }

    if (view->hasFocus()) {
        drawFocus(p);
    }

    gd = getGraphView()->getGSequenceGraphDrawer();
    assert(gd!=NULL);
    connect(gd, SIGNAL(si_graphDataUpdated()), SLOT(sl_graphDataUpdated()), Qt::UniqueConnection);

    drawHeader(p);

    const QList<QSharedPointer<GSequenceGraphData> >& graphs = getGraphView()->getGraphs();
    gd->draw(p, graphs, graphRect);

    drawFrame(p);
    drawSelection(p);
}

void GSequenceGraphViewRA::drawHeader(QPainter& p) {
    p.setFont(*headerFont);

    const GSequenceGraphWindowData& wd = gd->getWindowData();
    const U2Region& visibleRange = view->getVisibleRange();
    QString text = GSequenceGraphView::tr("%1 [%2, %3], Window: %4, Step %5").arg(getGraphView()->getGraphViewName())
        .arg(QString::number(visibleRange.startPos+1)).arg(QString::number(visibleRange.endPos())).arg(QString::number(wd.window)).arg(QString::number(wd.step));
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
    foreach(const U2Region& r, selection) {
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

void GSequenceGraphViewRA::sl_graphDataUpdated() {
    update();
}

GSequenceGraphView::~GSequenceGraphView() {
    foreach (const QSharedPointer<GSequenceGraphData>& gsdata, graphs) {
        gsdata->graphLabels.deleteAllLabels();
    }
}

} // namespace
