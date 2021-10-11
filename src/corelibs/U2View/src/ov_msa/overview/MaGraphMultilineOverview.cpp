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

#include "MaGraphMultilineOverview.h"

#include <QMouseEvent>
#include <QPainter>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/Settings.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorConsensusCache.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>

#include "MaGraphCalculationTask.h"
#include "ov_msa/helpers/ScrollController.h"

namespace U2 {

MaGraphMultilineOverview::MaGraphMultilineOverview(MaEditorMultilineWgt *ui)
    : MaMultilineOverview(ui),
      redrawGraph(true),
      isBlocked(false),
      lastDrawnVersion(-1),
      method(Strict),
      graphCalculationTask(nullptr) {
    setFixedHeight(FIXED_HEIGHT);

    displaySettings = new MaGraphMultilineOverviewDisplaySettings();

    Settings *s = AppContext::getSettings();
    CHECK(s != nullptr, );
    if (s->contains(MSA_GRAPH_OVERVIEW_COLOR_KEY)) {
        displaySettings->color = s->getValue(MSA_GRAPH_OVERVIEW_COLOR_KEY).value<QColor>();
    }

    if (s->contains(MSA_GRAPH_OVERVIEW_TYPE_KEY)) {
        displaySettings->type = (MaGraphMultilineOverviewDisplaySettings::GraphType)s->getValue(MSA_GRAPH_OVERVIEW_TYPE_KEY).toInt();
    }

    if (s->contains(MSA_GRAPH_OVERVIEW_ORIENTAION_KEY)) {
        displaySettings->orientation = (MaGraphMultilineOverviewDisplaySettings::OrientationMode)s->getValue(MSA_GRAPH_OVERVIEW_ORIENTAION_KEY).toInt();
    }

    connect(&graphCalculationTaskRunner, SIGNAL(si_finished()), SLOT(sl_redraw()));

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_drawGraph()));

    connect(ui, SIGNAL(si_startMaChanging()), SLOT(sl_blockRendering()));
    connect(ui, SIGNAL(si_stopMaChanging(bool)), SLOT(sl_unblockRendering(bool)));

    sl_drawGraph();
}

void MaGraphMultilineOverview::cancelRendering() {
    if (isRendering) {
        graphCalculationTaskRunner.cancel();
        lastDrawnVersion = -1;
    }
}

void MaGraphMultilineOverview::sl_redraw() {
    redrawGraph = true;
    MaMultilineOverview::sl_redraw();
}

void MaGraphMultilineOverview::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    if (!isValid()) {
        GUIUtils::showMessage(this, p, tr("Multiple sequence alignment is too big. Overview is unavailable."));
        QWidget::paintEvent(e);
        return;
    }
    if (isBlocked) {
        GUIUtils::showMessage(this, p, tr("Waiting..."));
        QWidget::paintEvent(e);
        return;
    }

    if (!graphCalculationTaskRunner.isIdle()) {
        GUIUtils::showMessage(this, p, tr("Overview is rendering..."));
        QWidget::paintEvent(e);
        return;
    } else {
        if (redrawGraph) {
            cachedConsensus = QPixmap(size());
            QPainter pConsensus(&cachedConsensus);
            drawOverview(pConsensus);
        }
    }

    cachedView = cachedConsensus;

    QPainter pVisibleRange(&cachedView);
    drawVisibleRange(pVisibleRange);

    p.drawPixmap(0, 0, cachedView);
    lastDrawnVersion = editor->getMaObject()->getModificationVersion();

    QWidget::paintEvent(e);
}

void MaGraphMultilineOverview::resizeEvent(QResizeEvent *e) {
    if (!isBlocked) {
        redrawGraph = true;
        sl_drawGraph();
    }
    QWidget::resizeEvent(e);
}

void MaGraphMultilineOverview::drawVisibleRange(QPainter &p) {
    if (editor->isAlignmentEmpty()) {
        setVisibleRangeForEmptyAlignment();
    } else {
        recalculateScale();

        const int screenPositionX = editor->getUI()->getScrollController()->getScreenPosition().x();
        const qint64 screenWidth = editor->getUI()->getSequenceArea()->width();

        cachedVisibleRange.setY(0);
        cachedVisibleRange.setHeight(FIXED_HEIGHT);
        cachedVisibleRange.setX(qRound(screenPositionX / stepX));
        cachedVisibleRange.setWidth(qRound(screenWidth / stepX));

        if (cachedVisibleRange.width() == 0) {
            cachedVisibleRange.setWidth(1);
        }

        if (cachedVisibleRange.width() < VISIBLE_RANGE_CRITICAL_SIZE || cachedVisibleRange.height() < VISIBLE_RANGE_CRITICAL_SIZE) {
            p.setPen(Qt::red);
        }
    }

    p.fillRect(cachedVisibleRange, VISIBLE_RANGE_COLOR);
    p.drawRect(cachedVisibleRange.adjusted(0, 0, -1, -1));
}

void MaGraphMultilineOverview::sl_drawGraph() {
    if (!isVisible() || isBlocked) {
        return;
    }
    graphCalculationTaskRunner.cancel();

    switch (method) {
        case Strict:
            graphCalculationTask = new MaConsensusOverviewCalculationTask(editor->getMaObject(),
                                                                          width(),
                                                                          FIXED_HEIGHT);
            break;
        case Gaps:
            graphCalculationTask = new MaGapOverviewCalculationTask(editor->getMaObject(),
                                                                    width(),
                                                                    FIXED_HEIGHT);
            break;
        case Clustal:
            graphCalculationTask = new MaClustalOverviewCalculationTask(editor->getMaObject(),
                                                                        width(),
                                                                        FIXED_HEIGHT);
            break;
        case Highlighting:
            // TODO:ichebyki
//            MsaHighlightingScheme *hScheme = sequenceArea->getCurrentHighlightingScheme();
//            QString hSchemeId = hScheme->getFactory()->getId();

//            MsaColorScheme *cScheme = sequenceArea->getCurrentColorScheme();
//            QString cSchemeId = cScheme->getFactory()->getId();

//            graphCalculationTask = new MaHighlightingOverviewCalculationTask(editor,
//                                                                             cSchemeId,
//                                                                             hSchemeId,
//                                                                             width(),
//                                                                             FIXED_HEIGHT);
            break;
    }

    connect(graphCalculationTask, SIGNAL(si_calculationStarted()), SLOT(sl_startRendering()));
    connect(graphCalculationTask, SIGNAL(si_calculationStoped()), SLOT(sl_stopRendering()));
    graphCalculationTaskRunner.run(graphCalculationTask);

    sl_redraw();
}

void MaGraphMultilineOverview::sl_highlightingChanged() {
    if (method == Highlighting) {
        sl_drawGraph();
    }
}

void MaGraphMultilineOverview::sl_graphOrientationChanged(MaGraphMultilineOverviewDisplaySettings::OrientationMode orientation) {
    if (orientation != displaySettings->orientation) {
        displaySettings->orientation = orientation;

        Settings *s = AppContext::getSettings();
        s->setValue(MSA_GRAPH_OVERVIEW_ORIENTAION_KEY, orientation);

        update();
    }
}

void MaGraphMultilineOverview::sl_graphTypeChanged(MaGraphMultilineOverviewDisplaySettings::GraphType type) {
    if (type != displaySettings->type) {
        displaySettings->type = type;

        Settings *s = AppContext::getSettings();
        s->setValue(MSA_GRAPH_OVERVIEW_TYPE_KEY, type);

        update();
    }
}

void MaGraphMultilineOverview::sl_graphColorChanged(QColor color) {
    if (color != displaySettings->color) {
        displaySettings->color = color;

        Settings *s = AppContext::getSettings();
        s->setValue(MSA_GRAPH_OVERVIEW_COLOR_KEY, color);

        update();
    }
}

void MaGraphMultilineOverview::sl_calculationMethodChanged(MaGraphMultilineCalculationMethod _method) {
    if (method != _method) {
        method = _method;
        sl_drawGraph();
    }
}

void MaGraphMultilineOverview::sl_startRendering() {
    isRendering = true;
    emit si_renderingStateChanged(isRendering);
}

void MaGraphMultilineOverview::sl_stopRendering() {
    isRendering = false;
    emit si_renderingStateChanged(isRendering);
}

void MaGraphMultilineOverview::sl_blockRendering() {
    disconnect(editor->getMaObject(), 0, this, 0);
    isBlocked = true;
}

void MaGraphMultilineOverview::sl_unblockRendering(bool update) {
    isBlocked = false;

    if (update && lastDrawnVersion != editor->getMaObject()->getModificationVersion()) {
        sl_drawGraph();
    } else {
        this->update();
    }

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_drawGraph()));
}

void MaGraphMultilineOverview::drawOverview(QPainter &p) {
    if (displaySettings->orientation == MaGraphMultilineOverviewDisplaySettings::FromTopToBottom) {
        // transform coordinate system
        p.translate(0, height());
        p.scale(1, -1);
    }

    p.fillRect(cachedConsensus.rect(), Qt::white);

    if (editor->getAlignmentLen() == 0) {
        return;
    }

    p.setPen(displaySettings->color);
    p.setBrush(displaySettings->color);

    if (graphCalculationTaskRunner.getResult().isEmpty() && !editor->isAlignmentEmpty() && !isBlocked) {
        sl_drawGraph();
        return;
    }

    QPolygonF resultPolygon = graphCalculationTaskRunner.getResult();
    if (!editor->isAlignmentEmpty() && resultPolygon.last().x() != width()) {
        sl_drawGraph();
        return;
    }

    // area graph
    if (displaySettings->type == MaGraphMultilineOverviewDisplaySettings::Area) {
        p.drawPolygon(resultPolygon);
    }

    // line graph
    if (displaySettings->type == MaGraphMultilineOverviewDisplaySettings::Line) {
        p.drawPolyline(resultPolygon);
    }

    // hystogram
    if (displaySettings->type == MaGraphMultilineOverviewDisplaySettings::Hystogram) {
        int size = graphCalculationTaskRunner.getResult().size();
        for (int i = 0; i < size; i++) {
            const QPointF point = resultPolygon.at(i);
            QPointF nextPoint;
            if (i != size - 1) {
                nextPoint = resultPolygon.at(i + 1);
            } else {
                nextPoint = QPointF(width(), point.y());
            }

            p.drawRect(point.x(), point.y(), static_cast<int>(nextPoint.x() - point.x()) - 2 * (width() > 2 * size), height() - point.y());
        }
    }

    // gray frame
    p.setPen(Qt::gray);
    p.setBrush(Qt::transparent);
    p.drawRect(rect().adjusted(0, (displaySettings->orientation == MaGraphMultilineOverviewDisplaySettings::FromTopToBottom), -1, -1 * (displaySettings->orientation == MaGraphMultilineOverviewDisplaySettings::FromBottomToTop)));
}

void MaGraphMultilineOverview::moveVisibleRange(QPoint _pos) {
    QRect newVisibleRange(cachedVisibleRange);
    const QPoint newPos(qBound((cachedVisibleRange.width() - 1) / 2, _pos.x(), width() - (cachedVisibleRange.width() - 1) / 2), height() / 2);

    newVisibleRange.moveCenter(newPos);

    // TODO:ichebyki
//    const int newScrollBarValue = newVisibleRange.x() * stepX;
//    ui->getScrollController()->setHScrollbarValue(newScrollBarValue);

    update();
}

}  // namespace U2
