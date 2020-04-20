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

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MSAConsensusAlgorithm.h>

#include <U2Core/MultipleAlignmentObject.h>

#include <U2Gui/GraphUtils.h>

#include "MaConsensusAreaRenderer.h"
#include "MaEditorWgt.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MSAEditorConsensusArea.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/ScrollController.h"
#include "ov_msa/view_rendering/MaEditorSequenceArea.h"

namespace U2 {

ConsensusRenderSettings::ConsensusRenderSettings()
    : columnWidth(0),
      drawSelection(true),
      colorScheme(NULL),
      resizeMode(MaEditor::ResizeMode_FontAndContent),
      highlightMismatches(false),
      rulerWidth(0),
      firstNotchedBasePosition(0),
      lastNotchedBasePosition(0)
{

}

bool ConsensusRenderData::isValid() const {
    return data.size() == static_cast<int>(region.length) &&
            mismatches.size() == static_cast<int>(region.length);
}

ConsensusCharRenderData::ConsensusCharRenderData()
    : column(0),
      consensusChar(U2Msa::GAP_CHAR),
      isMismatch(false),
      isSelected(false)
{

}

QRect ConsensusCharRenderData::getCharRect() const {
    return QRect(xRange.startPos, yRange.startPos, xRange.length + 1, yRange.length);
}

const QColor MaConsensusAreaRenderer::DEFAULT_MISMATCH_COLOR = Qt::red;

MaConsensusAreaRenderer::MaConsensusAreaRenderer(MaEditorConsensusArea *area)
    : QObject(area),
      editor(area->getEditorWgt()->getEditor()),
      ui(area->getEditorWgt()),
      area(area)
{

}

namespace {

QFont getRulerFont(const QFont &font) {
    QFont rulerFont = font;
    rulerFont.setFamily("Arial");
    rulerFont.setPointSize(qMax(8, qRound(font.pointSize() * 0.7)));
    return rulerFont;
}

}

void MaConsensusAreaRenderer::drawContent(QPainter &painter) {
    CHECK(!editor->isAlignmentEmpty(), );

    const MaEditorConsensusAreaSettings consensusSettings = area->getDrawSettings();
    const ConsensusRenderData consensusRenderData = getScreenDataToRender();
    const ConsensusRenderSettings renderSettings = getScreenRenderSettings(consensusSettings);

    drawContent(painter, consensusRenderData, consensusSettings, renderSettings);
}

void MaConsensusAreaRenderer::drawContent(QPainter &painter,
                                          const ConsensusRenderData &consensusRenderData,
                                          const MaEditorConsensusAreaSettings &consensusSettings,
                                          const ConsensusRenderSettings &renderSettings) {
    SAFE_POINT(consensusRenderData.isValid(), "Incorrect consensus data to draw", );
    SAFE_POINT(NULL != renderSettings.colorScheme, "Color scheme is NULL", );

    if (consensusSettings.isVisible(MSAEditorConsElement_CONSENSUS_TEXT)) {
        drawConsensus(painter, consensusRenderData, renderSettings);
    }

    if (consensusSettings.isVisible(MSAEditorConsElement_RULER)) {
        drawRuler(painter, renderSettings);
    }

    if (consensusSettings.isVisible(MSAEditorConsElement_HISTOGRAM)) {
        drawHistogram(painter, consensusRenderData, renderSettings);
    }
}

ConsensusRenderData MaConsensusAreaRenderer::getConsensusRenderData(const QList<int> &seqIdx, const U2Region &region) const {
    ConsensusRenderData consensusRenderData;
    consensusRenderData.region = region;
    consensusRenderData.selectedRegion = ui->getSequenceArea()->getSelection().getXRegion();
    consensusRenderData.mismatches.resize(static_cast<int>(region.length));

    MSAConsensusAlgorithm *algorithm = area->getConsensusAlgorithm();
    const MultipleAlignment ma = editor->getMaObject()->getMultipleAlignment();
    for (int i = 0, n = static_cast<int>(region.length); i < n; i++) {
        const int column = region.startPos + i;
        int score = 0;
        const char consensusChar = algorithm->getConsensusCharAndScore(ma, column, score);
        consensusRenderData.data += consensusChar;
        consensusRenderData.percentage << qRound(score * 100. / seqIdx.size());
        consensusRenderData.mismatches[i] = (consensusChar != editor->getReferenceCharAt(column));
    }

    return consensusRenderData;
}

ConsensusRenderSettings MaConsensusAreaRenderer::getRenderSettigns(const U2Region &region, const MaEditorConsensusAreaSettings &consensusSettings) const {
    ConsensusRenderSettings renderSettings;
    renderSettings.xRangeToDrawIn = U2Region(0, ui->getBaseWidthController()->getBasesWidth(region));
    foreach (const MaEditorConsElement element, consensusSettings.order) {
        renderSettings.yRangeToDrawIn.insert(element, getYRange(consensusSettings.visibleElements, element));
    }
    renderSettings.columnWidth = ui->getBaseWidthController()->getBaseWidth();
    renderSettings.font = editor->getFont();
    renderSettings.rulerFont = getRulerFont(editor->getFont());
    renderSettings.drawSelection = false;
    renderSettings.colorScheme = ui->getSequenceArea()->getCurrentColorScheme();
    renderSettings.resizeMode = editor->getResizeMode();
    renderSettings.highlightMismatches = consensusSettings.highlightMismatches;

    renderSettings.rulerWidth = ui->getBaseWidthController()->getBasesWidth(region);
    renderSettings.firstNotchedBasePosition = region.startPos;
    renderSettings.lastNotchedBasePosition = region.endPos() - 1;
    const int xCanvasOffset = ui->getBaseWidthController()->getBaseGlobalOffset(region.startPos);
    renderSettings.firstNotchedBaseXRange = ui->getBaseWidthController()->getBaseScreenRange(region.startPos, xCanvasOffset);
    renderSettings.lastNotchedBaseXRange = ui->getBaseWidthController()->getBaseScreenRange(region.endPos() - 1, xCanvasOffset);

    return renderSettings;
}

int MaConsensusAreaRenderer::getHeight() const {
    return getHeight(area->getDrawSettings().visibleElements);
}

int MaConsensusAreaRenderer::getHeight(const MaEditorConsElements &visibleElements) const {
    int height = 0;
    foreach (const MaEditorConsElement element, area->getDrawSettings().order) {
        if (visibleElements.testFlag(element)) {
            height += getYRangeLength(element);
        }
    }
    return height + 1;
}

U2Region MaConsensusAreaRenderer::getYRange(const MaEditorConsElements &visibleElements, MaEditorConsElement element) const {
    const MaEditorConsensusAreaSettings consensusSettings = area->getDrawSettings();
    U2Region yRange;
    for (QList<MaEditorConsElement>::const_iterator it = consensusSettings.order.constBegin(); it != consensusSettings.order.constEnd(); it++) {
        if (*it == element) {
            yRange.length = getYRangeLength(element) * visibleElements.testFlag(*it);
            break;
        } else {
            yRange.startPos += getYRangeLength(*it) * visibleElements.testFlag(*it);
        }
    }
    return yRange;
}

U2Region MaConsensusAreaRenderer::getYRange(MaEditorConsElement element) const {
    return getYRange(area->getDrawSettings().visibleElements, element);
}

void MaConsensusAreaRenderer::drawConsensus(QPainter &painter, const ConsensusRenderData &consensusRenderData, const ConsensusRenderSettings &settings) {
    painter.setPen(Qt::black);

    QFont font = settings.font;
    font.setWeight(QFont::DemiBold);
    painter.setFont(font);

    ConsensusCharRenderData charData;
    charData.xRange = U2Region(settings.xRangeToDrawIn.startPos, settings.columnWidth);
    charData.yRange = settings.yRangeToDrawIn[MSAEditorConsElement_CONSENSUS_TEXT];

    for (int i = 0, n = static_cast<int>(consensusRenderData.region.length); i < n; i++) {
        charData.column = static_cast<int>(consensusRenderData.region.startPos + i);
        charData.consensusChar = consensusRenderData.data[i];
        if (MSAConsensusAlgorithm::INVALID_CONS_CHAR == charData.consensusChar) {
            charData.xRange.startPos += settings.columnWidth;
            continue;
        }
        charData.isMismatch = consensusRenderData.mismatches[i];
        charData.isSelected = settings.drawSelection && consensusRenderData.selectedRegion.contains(charData.column);

        drawConsensusChar(painter, charData, settings);
        charData.xRange.startPos += settings.columnWidth;
    }
}

void MaConsensusAreaRenderer::drawConsensusChar(QPainter &painter, const ConsensusCharRenderData& charData, const ConsensusRenderSettings &settings) {
    const QRect charRect = charData.getCharRect();

    QColor color;
    if (charData.isSelected) {
        color = Qt::lightGray;
        color = color.lighter(115);
    }

    if (settings.highlightMismatches && charData.isMismatch) {
        color = settings.colorScheme->getBackgroundColor(0, 0, charData.consensusChar);
        if (!color.isValid()) {
            color = DEFAULT_MISMATCH_COLOR;
        }
    }
    if (color.isValid()) {
        painter.fillRect(charRect, color);
    }

    if (settings.resizeMode == MaEditor::ResizeMode_FontAndContent) {
        painter.drawText(charRect, Qt::AlignVCenter | Qt::AlignHCenter, QString(charData.consensusChar));
    }
}

void MaConsensusAreaRenderer::drawRuler(QPainter &painter, const ConsensusRenderSettings &settings) {
    painter.setPen(Qt::darkGray);

    U2Region rulerYRange = settings.yRangeToDrawIn[MSAEditorConsElement_RULER];
    U2Region consensusTextYRange = settings.yRangeToDrawIn[MSAEditorConsElement_CONSENSUS_TEXT];

    // TODO: move this range calculations to getYRange method
    const int dy = rulerYRange.startPos - consensusTextYRange.endPos();
    rulerYRange.length += dy;
    rulerYRange.startPos -= dy;

    const int firstLastDistance = settings.lastNotchedBaseXRange.startPos - settings.firstNotchedBaseXRange.startPos;
    QPoint startPoint(settings.firstNotchedBaseXRange.center(), rulerYRange.startPos);

    const QFontMetrics fontMetrics(settings.rulerFont, painter.device());

    GraphUtils::RulerConfig config;
    config.singleSideNotches = true;
    config.notchSize = MaEditorConsensusAreaSettings::RULER_NOTCH_SIZE;
    config.textOffset = (rulerYRange.length - fontMetrics.ascent()) / 2;
    config.extraAxisLenBefore = startPoint.x();
    config.extraAxisLenAfter = settings.rulerWidth - (startPoint.x() + firstLastDistance);
    config.textBorderStart = -settings.firstNotchedBaseXRange.length / 2;
    config.textBorderEnd = -settings.firstNotchedBaseXRange.length / 2;

    GraphUtils::drawRuler(painter, startPoint, firstLastDistance, settings.firstNotchedBasePosition + 1, settings.lastNotchedBasePosition + 1, settings.rulerFont, config);

    startPoint.setY(rulerYRange.endPos());
    config.drawNumbers = false;
    config.textPosition = GraphUtils::LEFT;
    GraphUtils::drawRuler(painter, startPoint, firstLastDistance, settings.firstNotchedBasePosition + 1, settings.lastNotchedBasePosition + 1, settings.rulerFont, config);
}

void MaConsensusAreaRenderer::drawHistogram(QPainter &painter, const ConsensusRenderData &consensusRenderData, const ConsensusRenderSettings &settings) {
    QColor color("#255060");
    painter.setPen(color);

    // TODO: move calculations to getYRange method
    U2Region yRange = settings.yRangeToDrawIn[MSAEditorConsElement_HISTOGRAM];
    yRange.startPos++;
    yRange.length -= 2; //keep borders

    QBrush brush(color, Qt::Dense4Pattern);
    painter.setBrush(brush);

    QVector<QRect> rects;
    U2Region xRange = U2Region(settings.xRangeToDrawIn.startPos, settings.columnWidth);
    for (int i = 0, n = static_cast<int>(consensusRenderData.region.length); i < n; i++) {
        const int height = qRound((double)consensusRenderData.percentage[i] * yRange.length / 100.0);
        const QRect histogramRecT(xRange.startPos + 1, yRange.endPos() - height, xRange.length - 2, height);
        rects << histogramRecT;
        xRange.startPos += settings.columnWidth;
    }

    painter.drawRects(rects);
}

ConsensusRenderData MaConsensusAreaRenderer::getScreenDataToRender() const {
    const QSharedPointer<MSAEditorConsensusCache> consensusCache = area->getConsensusCache();

    ConsensusRenderData consensusRenderData;
    consensusRenderData.region = ui->getDrawHelper()->getVisibleBases(area->width());
    const MaEditorSelection selection = ui->getSequenceArea()->getSelection();
    consensusRenderData.selectedRegion = U2Region(selection.x(), selection.width());
    consensusRenderData.data = consensusCache->getConsensusLine(consensusRenderData.region, true);
    consensusRenderData.percentage << consensusCache->getConsensusPercents(consensusRenderData.region);

    consensusRenderData.mismatches.resize(consensusRenderData.region.length);
    for (int i = 0, n = static_cast<int>(consensusRenderData.region.length); i < n; i++) {
        const int column = static_cast<int>(consensusRenderData.region.startPos + i);
        consensusRenderData.mismatches[i] = area->highlightConsensusChar(column);
    }

    return consensusRenderData;
}

ConsensusRenderSettings MaConsensusAreaRenderer::getScreenRenderSettings(const MaEditorConsensusAreaSettings &consensusSettings) const {
    const U2Region region = ui->getDrawHelper()->getVisibleBases(area->width());

    ConsensusRenderSettings renderSettings;
    renderSettings.xRangeToDrawIn = ui->getBaseWidthController()->getBasesScreenRange(region);
    foreach (const MaEditorConsElement element, consensusSettings.order) {
        renderSettings.yRangeToDrawIn.insert(element, getYRange(element));
    }
    renderSettings.columnWidth = ui->getBaseWidthController()->getBaseWidth();
    renderSettings.font = editor->getFont();
    renderSettings.rulerFont = getRulerFont(editor->getFont());
    renderSettings.drawSelection = true;
    renderSettings.colorScheme = ui->getSequenceArea()->getCurrentColorScheme();
    renderSettings.resizeMode = editor->getResizeMode();
    renderSettings.highlightMismatches = consensusSettings.highlightMismatches;

    renderSettings.rulerWidth = ui->getBaseWidthController()->getBasesWidth(region);
    renderSettings.firstNotchedBasePosition = ui->getScrollController()->getFirstVisibleBase();
    renderSettings.lastNotchedBasePosition = ui->getScrollController()->getLastVisibleBase(area->width());
    renderSettings.firstNotchedBaseXRange = ui->getBaseWidthController()->getBaseScreenRange(renderSettings.firstNotchedBasePosition);
    renderSettings.lastNotchedBaseXRange = ui->getBaseWidthController()->getBaseScreenRange(renderSettings.lastNotchedBasePosition);

    return renderSettings;
}

int MaConsensusAreaRenderer::getYRangeLength(MaEditorConsElement element) const {
    switch (element) {
    case MSAEditorConsElement_HISTOGRAM:
        return 50;
    case MSAEditorConsElement_CONSENSUS_TEXT:
        return ui->getRowHeightController()->getSingleRowHeight();
    case MSAEditorConsElement_RULER: {
        QFontMetrics fm(area->getDrawSettings().getRulerFont());
        return fm.height() + 2 * MaEditorConsensusAreaSettings::RULER_NOTCH_SIZE + 4;
    }
    default:
        FAIL(false, 0);
    }
}

}   // namespace U2
