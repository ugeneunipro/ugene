/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either versi
 * on 2
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

#include "MaGraphOverview.h"

#include <QMouseEvent>
#include <QPainter>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/Settings.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>

#include "MaGraphCalculationTask.h"
#include "ov_msa/ScrollController.h"
#include "ov_msa/MultilineScrollController.h"

#define MSA_GRAPH_OVERVIEW_COLOR_KEY "msa_graph_overview_color"
#define MSA_GRAPH_OVERVIEW_TYPE_KEY "msa_graph_overview_type"
#define MSA_GRAPH_OVERVIEW_ORIENTATION_KEY "msa_graph_overview_orientation_key"

namespace U2 {

MaGraphOverview::MaGraphOverview(MaEditor *editor, QWidget *ui)
    : MaOverview(editor, ui),
      redrawGraph(true),
      isBlocked(false),
      lastDrawnVersion(-1),
      method(Strict),
      graphCalculationTask(nullptr) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(FIXED_HEIGHT);

    Settings* settings = AppContext::getSettings();
    displaySettings.color = settings->getValue(MSA_GRAPH_OVERVIEW_COLOR_KEY, displaySettings.color).value<QColor>();
    displaySettings.type = (MaGraphOverviewDisplaySettings::GraphType)settings->getValue(MSA_GRAPH_OVERVIEW_TYPE_KEY, displaySettings.type).toInt();
    displaySettings.orientation = (MaGraphOverviewDisplaySettings::OrientationMode)settings->getValue(MSA_GRAPH_OVERVIEW_ORIENTATION_KEY, displaySettings.orientation).toInt();

    connect(&graphCalculationTaskRunner, &BackgroundTaskRunner_base::si_finished, this, [this]() {
        if (graphCalculationTaskRunner.isSuccessful()) {
            renderedState = inProgressState;
        }
        sl_redraw();
    });
    connect(editor->getMaObject(), &MultipleAlignmentObject::si_alignmentChanged, this, [this]() {
        state.maObjectVersion = editor->getMaObject()->getObjectVersion();
        recomputeGraphIfNeeded();
    });
    connect(ui, &MaEditorWgt::si_startMaChanging, this, [this]() {
        isMaChangeInProgress = true;
        graphCalculationTaskRunner.cancel();
    });
    connect(ui, &MaEditorWgt::si_stopMaChanging, this, [this]() {
        isMaChangeInProgress = false;
        recomputeGraphIfNeeded();
    });

    state.width = width();
    state.maObjectVersion = editor->getMaObject()->getObjectVersion();
}

void MaGraphOverview::sl_redraw() {
    redrawGraph = true;
    MaOverview::sl_redraw();
}

void MaGraphOverview::paintEvent(QPaintEvent* e) {
    QPainter p(this);
    if (!isValid()) {
        GUIUtils::showMessage(this, p, tr("Multiple sequence alignment is too big. Overview is unavailable."));
        QWidget::paintEvent(e);
        return;
    }
    if (state != inProgressState) {
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
    QWidget::paintEvent(e);
}

void MaGraphOverview::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    if (isVisible()) {
        redrawGraph = true;
        state.width = width();
        QTimer::singleShot(0, this, [this]() { recomputeGraphIfNeeded(); });
    }
}

void MaGraphOverview::hideEvent(QHideEvent* event) {
    graphCalculationTaskRunner.cancel();
    MaOverview::hideEvent(event);
}

void MaGraphOverview::showEvent(QShowEvent* event) {
    MaOverview::showEvent(event);
    QTimer::singleShot(0, this, [this]() { recomputeGraphIfNeeded(); });
}

void MaGraphOverview::drawVisibleRange(QPainter& p) {
    if (editor->isAlignmentEmpty()) {
        setVisibleRangeForEmptyAlignment();
    } else {
        recalculateScale();

        // range width is a sum of the widths of all visible children
        // X position is defined by the first visible child
        qint64 screenWidth = 0;
        int screenPositionX = -1;
        MaEditorWgt *wgt = editor->getMaEditorWgt(0);
        for (uint i = 0; wgt != nullptr; ) {
            if (wgt->isVisible()) {
                QRegion r = wgt->visibleRegion();
                if (r.rectCount() > 0) {
                    if (screenPositionX == -1) {
                        screenPositionX = wgt->getScrollController()->getScreenPosition().x();
                    }
                    screenWidth += wgt->getSequenceArea()->width();
                }
            }
            wgt = editor->getMaEditorWgt(++i);
        }

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

void MaGraphOverview::recomputeGraphIfNeeded() {
    CHECK(!isMaChangeInProgress && isVisible() && state != (graphCalculationTaskRunner.isIdle() ? renderedState : inProgressState), );
    graphCalculationTaskRunner.cancel();
    auto maObject = editor->getMaObject();
    MaGraphCalculationTask* task;
    switch (state.method) {
        case MaGraphCalculationMethod::Strict:
            task = new MaConsensusOverviewCalculationTask(maObject, width(), height());
            break;
        case MaGraphCalculationMethod::Gaps:
            task = new MaGapOverviewCalculationTask(maObject, width(), height());
            break;
        case MaGraphCalculationMethod::Clustal:
            task = new MaClustalOverviewCalculationTask(maObject, width(), height());
            break;
        case Highlighting:
            MsaHighlightingScheme *hScheme = editor->getMaEditorWgt()->getSequenceArea()->getCurrentHighlightingScheme();
            QString hSchemeId = hScheme->getFactory()->getId();

            MsaColorScheme *cScheme = editor->getMaEditorWgt()->getSequenceArea()->getCurrentColorScheme();
            QString cSchemeId = cScheme->getFactory()->getId();

            graphCalculationTask = new MaHighlightingOverviewCalculationTask(editor,
                                                                             cSchemeId,
                                                                             hSchemeId,
                                                                             width(),
                                                                             FIXED_HEIGHT);
            break;
    }
    SAFE_POINT(task != nullptr, "Unsupported overview method:" + QString::number((int)state.method), );
    connect(task, &MaGraphCalculationTask::si_calculationStarted, this, [this]() { emit si_renderingStateChanged(true); });
    connect(task, &MaGraphCalculationTask::si_calculationStoped, this, [this]() { emit si_renderingStateChanged(false); });

    inProgressState = state;
    graphCalculationTaskRunner.run(task);
    sl_redraw();
}

void MaGraphOverview::sl_highlightingChanged() {
    updateHighlightingSchemes();
    recomputeGraphIfNeeded();
}

void MaGraphOverview::updateHighlightingSchemes() {
    if (state.method == MaGraphCalculationMethod::Highlighting) {
        MaEditorSequenceArea* sequenceArea = ui->getSequenceArea();
        MsaHighlightingScheme* highlightingScheme = sequenceArea->getCurrentHighlightingScheme();
        MsaColorScheme* colorScheme = sequenceArea->getCurrentColorScheme();
        state.highlightingSchemeId = highlightingScheme->getFactory()->getId();
        state.colorSchemeId = colorScheme->getFactory()->getId();
        SAFE_POINT(!state.highlightingSchemeId.isEmpty() && !state.colorSchemeId.isEmpty(), "There must be valid highlighting and color schemes", );
    } else {
        state.highlightingSchemeId = "";
        state.colorSchemeId = "";
    }
}

void MaGraphOverview::sl_graphOrientationChanged(const MaGraphOverviewDisplaySettings::OrientationMode& orientation) {
    CHECK(displaySettings.orientation != orientation, );
    displaySettings.orientation = orientation;
    AppContext::getSettings()->setValue(MSA_GRAPH_OVERVIEW_ORIENTATION_KEY, orientation);
    update();
}

void MaGraphOverview::sl_graphTypeChanged(const MaGraphOverviewDisplaySettings::GraphType& type) {
    CHECK(displaySettings.type != type, );
    displaySettings.type = type;
    AppContext::getSettings()->setValue(MSA_GRAPH_OVERVIEW_TYPE_KEY, type);
    update();
}

void MaGraphOverview::sl_graphColorChanged(const QColor& color) {
    CHECK(displaySettings.color != color, )
    displaySettings.color = color;
    AppContext::getSettings()->setValue(MSA_GRAPH_OVERVIEW_COLOR_KEY, color);
    update();
}

void MaGraphOverview::sl_calculationMethodChanged(const MaGraphCalculationMethod& method) {
    state.method = method;
    updateHighlightingSchemes();
    recomputeGraphIfNeeded();
}

void MaGraphOverview::drawOverview(QPainter& p) {
    bool isTopToBottom = displaySettings.orientation == MaGraphOverviewDisplaySettings::FromTopToBottom;
    if (isTopToBottom) {
        // transform coordinate system
        p.translate(0, height());
        p.scale(1, -1);
    }

    p.fillRect(cachedConsensus.rect(), Qt::white);

    CHECK(editor->getAlignmentLen() > 0, )

    QPolygonF resultPolygon = graphCalculationTaskRunner.getResult();
    CHECK(!resultPolygon.isEmpty(), );

    p.setPen(displaySettings.color);
    p.setBrush(displaySettings.color);

    // area graph
    if (displaySettings.type == MaGraphOverviewDisplaySettings::Area) {
        p.drawPolygon(resultPolygon);
    }

    // line graph
    if (displaySettings.type == MaGraphOverviewDisplaySettings::Line) {
        p.drawPolyline(resultPolygon);
    }

    // histogram
    if (displaySettings.type == MaGraphOverviewDisplaySettings::Histogram) {
        int pointCount = graphCalculationTaskRunner.getResult().size();
        for (int i = 0; i < pointCount; i++) {
            auto p1 = resultPolygon.at(i).toPoint();
            auto p2 = (i < pointCount - 1 ? resultPolygon.at(i + 1) : QPointF(width(), p1.y())).toPoint();
            int w = p2.x() - p1.x();
            int h = height() - p1.y();
            p.drawRect(p1.x(), p1.y(), w, h);
        }
    }

    // Frame.
    p.setPen(Qt::gray);
    p.setBrush(Qt::transparent);
    int yp1 = isTopToBottom ? 1 : 0;
    int yp2 = -1 * (isTopToBottom ? 0 : 1);
    p.drawRect(rect().adjusted(0, yp1, -1, yp2));
}

void MaGraphOverview::moveVisibleRange(QPoint pos) {
    QRect newVisibleRange(cachedVisibleRange);
    QPoint newPos(qBound((cachedVisibleRange.width() - 1) / 2, pos.x(), width() - (cachedVisibleRange.width() - 1) / 2), height() / 2);

    newVisibleRange.moveCenter(newPos);

    const int newScrollBarValue = newVisibleRange.x() * stepX;
    MaEditorMultilineWgt *mui = qobject_cast<MaEditorMultilineWgt *>(ui);
    if (mui != nullptr) {
        mui->getScrollController()->setMultilineHScrollbarValue(newScrollBarValue);
    }

    update();
}

const MaGraphOverviewDisplaySettings& MaGraphOverview::getDisplaySettings() const {
    return displaySettings;
}

const MaGraphOverviewState& MaGraphOverview::getState() const {
    return state;
}

}  // namespace U2
