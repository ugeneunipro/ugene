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

#include "PanViewRenderer.h"

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/PanView.h>
#include <U2View/PanViewRows.h>

namespace U2 {

const int PanViewRenderer::RULER_NOTCH_SIZE = 2;
// const int PanViewRenderer::MAX_VISIBLE_ROWS  = 20;
// const int PanViewRenderer::MAX_VISIBLE_ROWS_ON_START = 10;
const int PanViewRenderer::LINE_TEXT_OFFSET = 8;

PanViewRenderer::PanViewRenderer(PanView* panView, SequenceObjectContext* ctx)
    : SequenceViewAnnotatedRenderer(ctx),
      panView(panView) {
    s = panView->getLinesSettings();
}

qint64 PanViewRenderer::coordToPos(const QPoint& /*p*/, const QSize& /*canvasSize*/, const U2Region& /*visibleRange*/) const {
    FAIL("The method must never be called", -1);
}

double PanViewRenderer::getCurrentScale() const {
    return double(panView->getRenderArea()->width()) / panView->getVisibleRange().length;
}

U2Region PanViewRenderer::getAnnotationYRange(Annotation* a, int r, const AnnotationSettings* as, int /* availableHeight*/) const {
    Q_UNUSED(r);
    CHECK(as->visible, U2Region(-1, 0));
    const int row = panView->getRowsManager()->getAnnotationRowIdx(a);
    const int line = s->getRowLine(row);
    return U2Region(getLineY(line) + 2, commonMetrics.lineHeight - 4);
}

U2Region PanViewRenderer::getCutSiteYRange(const U2Strand&, int) const {
    FAIL("Must not be called!", U2Region(-1, 0));
}

int PanViewRenderer::getContentIndentY(int canvasHeight) const {
    return (canvasHeight - s->numLines * commonMetrics.lineHeight) / 2;
}

int PanViewRenderer::getMinimumHeight() const {
    return commonMetrics.lineHeight * (s->getAdditionalLines() + 1);
}

QSize PanViewRenderer::getBaseCanvasSize(const U2Region& visibleRange) const {
    double halfChar = getCurrentScale() / 2;
    int firstCharCenter = qRound(halfChar);
    int lastCharCenter = qRound(visibleRange.length * getCurrentScale() + halfChar);

    int firstLastWidth = lastCharCenter - firstCharCenter;
    if (qRound(halfChar) == 0) {
        firstLastWidth--;  // make the end of the ruler visible
    }

    PVRowsManager* rm = panView->getRowsManager();
    int width = (int)(firstLastWidth + 2 * halfChar + 5);
    int height = commonMetrics.lineHeight * (rm->getRowCount() + s->showCustomRulers * s->customRulers.size() + s->showMainRuler + isSequenceCharsVisible() + 1) + 6;
    return QSize(width, height);
}

void PanViewRenderer::drawAll(QPainter& p, const U2Region& visibleRange) {
    int rowsOffsetOnScreen = s->rowLinesOffset;
    int numVisibleRows = s->numLines;
    s->rowLinesOffset = 0;
    s->numLines = panView->getRowsManager()->getRowCount() + s->getAdditionalLines();
    QSize canvasSize = getBaseCanvasSize(visibleRange);
    drawAll(p, canvasSize, visibleRange);
    s->rowLinesOffset = rowsOffsetOnScreen;
    s->numLines = numVisibleRows;
}

void PanViewRenderer::drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    p.fillRect(0, 0, canvasSize.width(), canvasSize.height(), Qt::white);
    p.setPen(Qt::black);

    GraphUtils::RulerConfig c;

    int hCenter = getContentIndentY(canvasSize.height());
    double halfChar = getCurrentScale() / 2;
    int firstCharCenter = posToXCoord(visibleRange.startPos, canvasSize, visibleRange) + halfChar;
    int lastCharCenter = posToXCoord(visibleRange.endPos() - 1, canvasSize, visibleRange) + halfChar;
    int firstLastWidth = lastCharCenter - firstCharCenter;
    if (qRound(halfChar) == 0) {
        firstLastWidth--;  // make the end of the ruler visible
    }
    c.notchSize = RULER_NOTCH_SIZE;
    int chunk = GraphUtils::calculateChunk(visibleRange.startPos + 1, visibleRange.endPos(), panView->width(), p);
    foreach (const RulerInfo& ri, s->customRulers) {
        chunk = qMax(chunk, GraphUtils::calculateChunk(visibleRange.startPos + 1 - ri.offset, visibleRange.endPos() - ri.offset, panView->width(), p));
    }
    c.predefinedChunk = chunk;

    p.translate(0, hCenter);

    drawRuler(c, p, visibleRange, firstCharCenter, firstLastWidth);
    drawCustomRulers(c, p, visibleRange, firstCharCenter, lastCharCenter, canvasSize.width());
    drawSequence(p, canvasSize, visibleRange);

    AnnotationDisplaySettings displaySettings;
    displaySettings.displayCutSites = false;
    drawAnnotations(p, canvasSize, visibleRange, displaySettings);

    p.translate(0, -hCenter);
}

void PanViewRenderer::drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    int hCenter = getContentIndentY(canvasSize.height());
    p.translate(0, hCenter);

    drawSequence(p, canvasSize, visibleRange);

    AnnotationDisplaySettings displaySettings;
    displaySettings.displayCutSites = false;
    drawAnnotationSelection(p, canvasSize, visibleRange, displaySettings);
    drawSequenceSelection(p, canvasSize, visibleRange);

    p.translate(0, -hCenter);
}

void PanViewRenderer::drawAnnotations(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange, const AnnotationDisplaySettings& displaySettings) {
    GTIMER(c2, t2, "PanViewRenderArea::drawAnnotations");
    const QPen dotty(Qt::lightGray, 1, Qt::DotLine);
    p.setPen(dotty);
    p.setFont(annMetrics.afSmall);
    const int cachedViewWidth = canvasSize.width();  // rename the variable

    // draw row names
    PVRowsManager* rm = panView->getRowsManager();
    const int maxVisibleRows = s->getNumVisibleRows();
    for (int i = 0; i < maxVisibleRows; i++) {
        const int row = i + s->rowLinesOffset;
        const int rowLine = s->getRowLine(row);
        const int lineY = getLineY(rowLine);
        p.drawLine(0, lineY, cachedViewWidth, lineY);

        const PVRowData* rData = rm->getRow(row);
        const QRect textRect(LINE_TEXT_OFFSET, lineY + 1, canvasSize.width(), commonMetrics.lineHeight - 2);
        p.drawText(textRect, getText(rData));

        if (rData != nullptr) {
            AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
            AnnotationSettings* as = asr->getAnnotationSettings(rData->key);
            if (as->visible) {
                QPen pen1(Qt::SolidLine);
                pen1.setWidth(1);
                foreach (Annotation* a, rData->annotations) {
                    drawAnnotation(p, canvasSize, visibleRange, a, displaySettings);
                }
                // restore pen
                p.setPen(dotty);
                p.setFont(annMetrics.afSmall);
            }
        }
    }

    const int firstRowLine = s->getRowLine(0);
    const int lineY = getLineY(firstRowLine) + commonMetrics.lineHeight;
    p.drawLine(0, lineY, cachedViewWidth, lineY);
}

int PanViewRenderer::getLineY(int line) const {
    return line * commonMetrics.lineHeight;
}

bool PanViewRenderer::isSequenceCharsVisible() const {
    return getCurrentScale() >= commonMetrics.smallCharWidth;
}

void PanViewRenderer::drawSequence(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    CHECK(isSequenceCharsVisible(), );

    p.setPen(Qt::black);
    double halfCharByScale = getCurrentScale() / 2;
    float halfCharByFont;
    if (getCurrentScale() >= commonMetrics.charWidth) {
        p.setFont(commonMetrics.sequenceFont);
        halfCharByFont = commonMetrics.charWidth / 2.0f;
    } else {
        p.setFont(commonMetrics.smallSequenceFont);
        halfCharByFont = commonMetrics.smallCharWidth / 2.0f;
    }
    U2OpStatusImpl os;
    QByteArray seq = ctx->getSequenceData(visibleRange, os);
    SAFE_POINT_OP(os, );
    int y = getLineY(s->getSelectionLine()) + commonMetrics.lineHeight - commonMetrics.yCharOffset;
    for (int i = 0; i < visibleRange.length; i++) {
        char c = seq[i];
        int x = posToXCoord(visibleRange.startPos + i, canvasSize, visibleRange) + halfCharByScale - halfCharByFont;
        p.drawText(x, y, QString(c));
    }
}

#define ARROW_DY 5
#define ARROW_DX 5
void PanViewRenderer::drawSequenceSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) {
    const QVector<U2Region>& selection = ctx->getSequenceSelection()->getSelectedRegions();
    bool showSequenceMode = isSequenceCharsVisible();
    CHECK(!selection.isEmpty(), );

    QPen pen1(Qt::darkGray, 1, Qt::SolidLine);
    QPen pen2(QColor("#007DE3"), 2, Qt::SolidLine);
    p.setFont(commonMetrics.rulerFont);
    QFontMetrics rfm(commonMetrics.rulerFont);

    int lineY = getLineY(s->getSelectionLine());
    int ly = lineY + commonMetrics.lineHeight / 2;  // ruler line

    bool drawRect = showSequenceMode;
    bool drawGraphics = true;
    if (showSequenceMode) {
        ly = lineY - commonMetrics.lineHeight + RULER_NOTCH_SIZE;
        drawGraphics = ly > 0;
    }
    int halfNum = rfm.boundingRect('1').height() / 2;
    int rty = ly + halfNum;

    const DNAAlphabet* alphabet = ctx->getSequenceObject()->getAlphabet();
    QString unitType;
    if (alphabet->isAmino()) {
        unitType = "aa";
    } else {
        unitType = "bp";
    }

    QString rangePattern = " " + panView->tr("[%1 %2]") + " ";
    foreach (const U2Region& r, selection) {
        if (!visibleRange.intersects(r)) {
            continue;
        }
        int x1 = qMax(0, posToXCoord(r.startPos, canvasSize, visibleRange));
        int x2 = qMin(canvasSize.width(), posToXCoord(r.endPos(), canvasSize, visibleRange));
        x2 = x2 < 0 ? canvasSize.width() : x2;

        p.setPen(pen1);
        if (visibleRange.contains(r.startPos) && s->numLines > 1) {
            p.drawLine(x1, -getContentIndentY(canvasSize.height()), x1, ly);
        }
        if (visibleRange.contains(r.endPos() - 1) && s->numLines > 1) {
            p.drawLine(x2, -getContentIndentY(canvasSize.height()), x2, ly);
        }

        if (drawRect) {
            p.setPen(Qt::black);
            p.drawRect(x1, lineY + 1, x2 - x1, commonMetrics.lineHeight - 2);
        }

        if (drawGraphics) {
            // draw line
            p.setPen(pen2);
            p.drawLine(x1, ly, x2, ly);
            int dArrow = 2 * ARROW_DX;

            QString t1 = QString::number(r.startPos + 1);
            QString t2 = QString::number(r.endPos());
            int tOffs = ARROW_DX - 1;
            QRect t1Rect = rfm.boundingRect(t1);
            t1Rect.translate(x1 - t1Rect.width() - tOffs, rty);
            QRect t2Rect = rfm.boundingRect(t2).translated(x2 + tOffs, rty);

            // define range text coords
            QString rangeText = rangePattern.arg(r.length).arg(unitType);
            QRect rtRect = rfm.boundingRect(rangeText);
            int rulerWidth = x2 - x1;
            bool rangeTextInTheMiddle = rulerWidth - dArrow > rtRect.width();
            if (rangeTextInTheMiddle) {
                int rtx = x1 + (rulerWidth - rtRect.width()) / 2 + 1;
                assert(rtx - x1 >= ARROW_DX);
                rtRect.translate(rtx, rty);
                p.fillRect(rtRect, Qt::white);
                p.drawText(rtRect, Qt::AlignCenter, rangeText);
            } else {  // if range text is not in the middle glue it to one of the boundary texts
                QString newT2 = t2 + rangeText;
                QRect newT2Rect = rfm.boundingRect(newT2).translated(x2 + tOffs, rty);
                if (newT2Rect.right() < canvasSize.width()) {
                    t2Rect = newT2Rect;
                    t2 = newT2;
                } else {
                    QString newT1 = rangeText + t1;
                    QRect newT1Rect = rfm.boundingRect(newT1);
                    newT1Rect.translate(x1 - newT1Rect.width() - tOffs, rty);
                    if (newT1Rect.left() >= 0) {
                        t1 = newT1;
                        t1Rect = newT1Rect;
                    }
                }
            }

            // check if regions overlap
            int interWidth = t2Rect.left() - t1Rect.right();
            if (interWidth < dArrow) {
                int deltaW = interWidth > 0 ? dArrow : qAbs(interWidth) + dArrow;
                if (t1Rect.x() - deltaW > 0) {
                    t1Rect.translate(-deltaW, 0);
                } else if (t2Rect.right() + deltaW < canvasSize.width()) {
                    t2Rect.translate(deltaW, 0);
                }
            }

            // draw regions
            p.fillRect(t1Rect, Qt::white);
            p.fillRect(t2Rect, Qt::white);
            p.drawText(t1Rect, Qt::AlignCenter, t1);
            p.drawText(t2Rect, Qt::AlignCenter, t2);

            // draw arrows (after the text -> can overlap with text rect boundaries)
            if (visibleRange.contains(r.startPos)) {
                p.drawLine(x1, ly, x1 + ARROW_DX, ly + ARROW_DY);
                p.drawLine(x1, ly, x1 + ARROW_DX, ly - ARROW_DY);
            }
            if (visibleRange.contains(r.endPos() - 1)) {
                p.drawLine(x2, ly, x2 - ARROW_DX, ly + ARROW_DY);
                p.drawLine(x2, ly, x2 - ARROW_DX, ly - ARROW_DY);
            }
        }
    }
}

void PanViewRenderer::drawRuler(GraphUtils::RulerConfig c, QPainter& p, const U2Region& visibleRange, int firstCharCenter, int firstLastWidth) {
    CHECK(s->showMainRuler, );

    int y = getLineY(s->getRulerLine()) + c.notchSize;
    GraphUtils::drawRuler(p, QPoint(firstCharCenter, y), firstLastWidth, visibleRange.startPos + 1, visibleRange.endPos(), commonMetrics.rulerFont, c);
}

void PanViewRenderer::drawCustomRulers(GraphUtils::RulerConfig c, QPainter& p, const U2Region& visibleRange, int firstCharCenter, int lastCharCenter, int w, int predefinedY, bool ignoreVisibleRange) {
    CHECK(s->showCustomRulers && !s->customRulers.isEmpty(), );

    double pixelsPerChar = getCurrentScale();
    double halfChar = pixelsPerChar / 2;
    QFont crf = commonMetrics.rulerFont;
    crf.setBold(true);
    QFontMetrics fm(crf);

    int maxRulerTextWidth = 0;
    foreach (const RulerInfo& ri, s->customRulers) {
        int _w = fm.width(ri.name);
        maxRulerTextWidth = qMax(maxRulerTextWidth, _w);
    }
    QTextOption rulerTo;
    rulerTo.setWrapMode(QTextOption::NoWrap);
    for (int i = 0, n = s->customRulers.count(); i < n; i++) {
        const RulerInfo& ri = s->customRulers[i];
        p.setPen(ri.color);
        p.setFont(crf);
        int y = (predefinedY == -1) ? getLineY(s->getCustomRulerLine(i)) + c.notchSize : predefinedY - i * commonMetrics.lineHeight;
        QRect rulerTextRect(ignoreVisibleRange ? firstCharCenter + LINE_TEXT_OFFSET : LINE_TEXT_OFFSET, y, maxRulerTextWidth, commonMetrics.lineHeight);
        p.drawText(rulerTextRect, ri.name, rulerTo);
        int rulerStartOffset = maxRulerTextWidth + 2 * LINE_TEXT_OFFSET + (ignoreVisibleRange ? firstCharCenter : 0);
        if (rulerStartOffset >= w) {
            continue;
        }
        qint64 startPos = visibleRange.startPos + 1 - ri.offset;
        qint64 endPos = visibleRange.endPos() - ri.offset;
        int x = firstCharCenter;

        if (firstCharCenter <= rulerStartOffset) {
            int deltaPixels = rulerStartOffset - firstCharCenter;
            int nChars = qMax(1, qRound(deltaPixels / pixelsPerChar));
            double deltaPixels2 = nChars * pixelsPerChar;
            startPos += nChars;
            x += deltaPixels2;
        }

        int rulerWidth = lastCharCenter - x;
        if (qRound(halfChar) == 0) {
            rulerWidth--;  // make the end of the ruler visible
        }

        int offsetToFirstNotch = (int)(c.predefinedChunk - visibleRange.startPos % c.predefinedChunk);
        qint64 mainRuler = visibleRange.startPos + offsetToFirstNotch;
        qint64 newStartPos = visibleRange.startPos - ri.offset + offsetToFirstNotch;
        qint64 lim = startPos + ri.offset;
        for (; mainRuler < lim; mainRuler += c.predefinedChunk, newStartPos += c.predefinedChunk)
            ;
        c.correction = (int)newStartPos;

        GraphUtils::drawRuler(p, QPoint(x, y), rulerWidth, startPos, endPos, commonMetrics.rulerFont, c);
    }
}

const QString PanViewRenderer::getText(const PVRowData* rData) const {
    const QString text = (rData == nullptr) ? U2::PanView::tr("empty") : rData->key + " (" + QString::number(rData->annotations.size()) + ")";
    return text;
}

PanViewRenderAreaFactory::PanViewRenderAreaFactory() {
}

PanViewRenderAreaFactory::~PanViewRenderAreaFactory() {
}

PanViewRenderArea* PanViewRenderAreaFactory::createRenderArea(PanView* panView) const {
    auto renderer = new PanViewRenderer(panView, panView->getSequenceContext());
    auto renderArea = new PanViewRenderArea(panView, renderer);
    renderer->setParent(renderArea);  // Delete renderer when render area is deleted.
    return renderArea;
}

}  // namespace U2
