/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
#include "ov_msa/MultilineScrollController.h"
#include "ov_msa/ScrollController.h"

#define MSA_GRAPH_OVERVIEW_COLOR_KEY "msa_graph_overview_color"
#define MSA_GRAPH_OVERVIEW_TYPE_KEY "msa_graph_overview_type"
#define MSA_GRAPH_OVERVIEW_ORIENTATION_KEY "msa_graph_overview_orientation_key"

namespace U2 {

MaGraphOverview::MaGraphOverview(MaEditor* _editor, QWidget* _ui)
    : MaOverview(_editor, _ui) {
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

    auto mui = qobject_cast<MaEditorMultilineWgt*>(_ui);
    CHECK(mui != nullptr, );

    connect(editor->getMaObject(), &MsaObject::si_alignmentChanged, this, [this]() {
        state.maObjectVersion = editor->getMaObject()->getObjectVersion();
        recomputeGraphIfNeeded();
    });
    connect(mui, &MaEditorMultilineWgt::si_startMaChanging, this, [this]() {
        isMaChangeInProgress = true;
        graphCalculationTaskRunner.cancel();
    });
    connect(mui, &MaEditorMultilineWgt::si_stopMaChanging, this, [this]() {
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
        QTimer::singleShot(0, this, [this]() { recomputeGraphIfNeeded(); });
    }
}

void MaGraphOverview::hideEvent(QHideEvent* event) {
    graphCalculationTaskRunner.cancel();
    MaOverview::hideEvent(event);
}

void MaGraphOverview::showEvent(QShowEvent* event) {
    MaOverview::showEvent(event);
    if (isVisible()) {
        QTimer::singleShot(0, this, [this]() { recomputeGraphIfNeeded(); });
    }
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
        auto mui = qobject_cast<MaEditorMultilineWgt*>(ui);
        if (mui->getMultilineMode()) {
            screenPositionX = mui->getUI(0)->getScrollController()->getScreenPosition().x();
            screenWidth = mui->getUI(0)->getSequenceArea()->width() * mui->getChildrenCount();
        } else {
            screenPositionX = mui->getUI(0)->getScrollController()->getScreenPosition().x();
            screenWidth = mui->getUI(0)->getSequenceArea()->width();
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
    state.width = width();

    const MaGraphOverviewState& stateToDedup = graphCalculationTaskRunner.isIdle() ? renderedState : inProgressState;
    bool isViewVisible = isOsMac() ? !visibleRegion().isEmpty() : isVisible(); // On MacOS even a background window has isVisible() true! May be a "too old QT" issue.
    CHECK(!isMaChangeInProgress && isViewVisible && state != stateToDedup && state.width > 0, );
//    uiLog.info(QString("recomputeGraphIfNeeded: %1,%2 -> %3,%4").arg(int(stateToDedup.method)).arg(stateToDedup.width).arg(int(state.method)).arg(state.width));

    graphCalculationTaskRunner.cancel();
    auto maObject = editor->getMaObject();
    MaGraphCalculationTask* task = nullptr;
    switch (state.method) {
        case MaGraphCalculationMethod::Strict:
            task = new MaConsensusOverviewCalculationTask(maObject, state.width, height());
            break;
        case MaGraphCalculationMethod::Gaps:
            task = new MaGapOverviewCalculationTask(maObject, state.width, height());
            break;
        case MaGraphCalculationMethod::Clustal:
            task = new MaClustalOverviewCalculationTask(maObject, state.width, height());
            break;
        case MaGraphCalculationMethod::Highlighting:
            task = new MaHighlightingOverviewCalculationTask(editor, state.colorSchemeId, state.highlightingSchemeId, state.width, height());
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
        auto mui = qobject_cast<MaEditorMultilineWgt*>(ui);
        CHECK(mui != nullptr, );
        MaEditorSequenceArea* sequenceArea = mui->getUI(0)->getSequenceArea();
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
    QPoint newPos(qBound((cachedVisibleRange.width() - 1) / 2,
                         pos.x(),
                         width() - (cachedVisibleRange.width() - 1) / 2),
                  height() / 2);

    newVisibleRange.moveCenter(newPos);

    auto mui = qobject_cast<MaEditorMultilineWgt*>(ui);
    if (mui != nullptr) {
        if (mui->getMultilineMode()) {
            // value = <overview-rect>.X / <overview>.width * <alignment-len>
            // but scroll bar has other min/max, so map it
            if (newVisibleRange.right() >= width()) {
                mui->getScrollController()->scrollToEnd(MultilineScrollController::Down);
            } else {
                int sequenceAreaBaseLen = mui->getSequenceAreaBaseLen(0);
                int rest = editor->getAlignmentLen() % sequenceAreaBaseLen;
                int evenLength = (editor->getAlignmentLen() / sequenceAreaBaseLen + (rest > 0 ? 1 : 0)) * sequenceAreaBaseLen;
                int newVScrollBarBase = newVisibleRange.x() * (double)evenLength / (double)width();
                mui->getScrollController()->setMultilineVScrollbarBase(newVScrollBarBase);
            }
        } else {
            int newScrollBarValue = newVisibleRange.x() * stepX;
            mui->getUI(0)->getScrollController()->setHScrollbarValue(newScrollBarValue);
        }
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
