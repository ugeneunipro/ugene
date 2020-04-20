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

#include "DetViewMultiLineRenderer.h"

#include "DetViewSingleLineRenderer.h"

#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>


namespace U2 {

/************************************************************************/
/* DetViewMultiLineRenderer */
/************************************************************************/
const int DetViewMultiLineRenderer::INDENT_BETWEEN_LINES = 30;
DetViewMultiLineRenderer::DetViewMultiLineRenderer(DetView *detView, SequenceObjectContext *ctx)
    : DetViewRenderer(detView, ctx),
      extraIndent(0) {
    singleLinePainter = new DetViewSingleLineRenderer(detView, ctx);
}

DetViewMultiLineRenderer::~DetViewMultiLineRenderer() {
    delete singleLinePainter;
}

qint64 DetViewMultiLineRenderer::coordToPos(const QPoint &p, const QSize &canvasSize, const U2Region &visibleRange) const {
    qint64 symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    U2Region firstLineVisibleRange(visibleRange.startPos, symbolsPerLine);
    qint64 posOnFirstLine = singleLinePainter->coordToPos(p, canvasSize, firstLineVisibleRange);
    int line = p.y() / getOneLineHeight();
    return qMin( ctx->getSequenceLength(), posOnFirstLine + line * symbolsPerLine);
}

float DetViewMultiLineRenderer::posToXCoordF(const qint64 p, const QSize &canvasSize, const U2Region &visibleRange) const {
    CHECK(visibleRange.contains(p), -1);

    qint64 symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    return commonMetrics.charWidth * (p % symbolsPerLine);
}

U2Region DetViewMultiLineRenderer::getAnnotationYRange(Annotation *a, int r, const AnnotationSettings *as, const QSize& canvasSize, const U2Region& visibleRange) const {
    if (qgetenv(ENV_GUI_TEST).toInt() == 1) {
        U2Region res = singleLinePainter->getAnnotationYRange(a, r, as, QSize(canvasSize.width(), getOneLineHeight()), visibleRange);
        res.startPos += INDENT_BETWEEN_LINES / 2;
        return res;
    }
    FAIL("The method must never be called", U2Region());
}

U2Region DetViewMultiLineRenderer::getMirroredYRange(const U2Strand &) const {
    FAIL("The method must never be called", U2Region());
}

bool DetViewMultiLineRenderer::isOnTranslationsLine(const QPoint &p, const QSize& canvasSize, const U2Region& visibleRange) const {
    qint64 symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    U2Region range(visibleRange.startPos, qMin(symbolsPerLine, visibleRange.length));
    do {
        if (singleLinePainter->isOnTranslationsLine(p, canvasSize, range)) {
            return true;
        }
        range.startPos += symbolsPerLine;
    } while (visibleRange.endPos() > range.endPos());

    return false;
}

bool DetViewMultiLineRenderer::isOnAnnotationLine(const QPoint &p, Annotation *a, int region, const AnnotationSettings *as, const QSize &canvasSize, const U2Region& visibleRange) const {
    qint64 symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    QSize oneLineMinSize(canvasSize.width(), getMinimumHeight());
    U2Region yRange = singleLinePainter->getAnnotationYRange(a, region, as, oneLineMinSize,
                                                            U2Region(visibleRange.startPos, qMin(visibleRange.length, symbolsPerLine)));
    yRange.startPos += (INDENT_BETWEEN_LINES + extraIndent) / 2;
    do {
        if (yRange.contains(p.y())) {
            return true;
        }
        yRange.startPos += getOneLineHeight();
    } while (canvasSize.height() > yRange.endPos());

    return false;
}

qint64 DetViewMultiLineRenderer::getMinimumHeight() const {
    return singleLinePainter->getMinimumHeight();
}

qint64 DetViewMultiLineRenderer::getOneLineHeight() const {
    return singleLinePainter->getOneLineHeight() + INDENT_BETWEEN_LINES + extraIndent;
}

qint64 DetViewMultiLineRenderer::getLinesCount(const QSize& canvasSize) const {
    return canvasSize.height() / getOneLineHeight();
}

qint64 DetViewMultiLineRenderer::getContentIndentY(const QSize& , const U2Region& ) const {
    return 0;
}

int DetViewMultiLineRenderer::getDirectLine() const {
    return singleLinePainter->getDirectLine();
}

int DetViewMultiLineRenderer::getRowsInLineCount() const {
    return singleLinePainter->getRowsInLineCount() + 2;
}

QSize DetViewMultiLineRenderer::getBaseCanvasSize(const U2Region &visibleRange) const {
    int defaultW = detView->getRenderArea()->width();
    int lineCount = visibleRange.length / getSymbolsPerLine(defaultW);
    if (visibleRange.length % getSymbolsPerLine(defaultW)) {
        lineCount++;
    }
    defaultW = qMin(defaultW, (int)(getCurrentScale() * visibleRange.length));
    return QSize(defaultW, (getOneLineHeight() - extraIndent) * lineCount);
}

void DetViewMultiLineRenderer::drawAll(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange) {

    int symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    U2Region oneLineRegion(visibleRange.startPos, symbolsPerLine);
    p.fillRect(QRect(QPoint(0, 0), canvasSize), Qt::white);

    // wide the indent between lines if neccessary
    extraIndent = 0;
    int sequenceLinesCount = visibleRange.length / symbolsPerLine + 1;
    if (ctx->getSequenceLength() == visibleRange.length) {
        int fullContentH = getOneLineHeight() * sequenceLinesCount;
        if (canvasSize.height() > fullContentH && detView->getShift() == 0) {
            extraIndent = (canvasSize.height() - fullContentH) / sequenceLinesCount;
        }
    }

    int indentCounter = 0;
    do {
        // cut the extra space at the end of the sequence
        oneLineRegion.length = qMin(visibleRange.endPos() - oneLineRegion.startPos, oneLineRegion.length);

        singleLinePainter->drawAll(p,
                                   QSize(canvasSize.width(), getOneLineHeight()),
                                   oneLineRegion);

        p.translate(0, getOneLineHeight());
        indentCounter += getOneLineHeight();

        oneLineRegion.startPos += symbolsPerLine;

    } while (oneLineRegion.startPos < visibleRange.endPos());

    // move painter back to [0, 0] position
    p.translate(0, - indentCounter);
}

void DetViewMultiLineRenderer::drawSelection(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange) {
    int symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    U2Region oneLineRegion(visibleRange.startPos, symbolsPerLine);

    int indentCounter = 0;
    do {
        // cut the extra space at the end of the sequence
        oneLineRegion.length = qMin(visibleRange.endPos() - oneLineRegion.startPos, oneLineRegion.length);

        singleLinePainter->drawSelection(p,
                                   QSize(canvasSize.width(), getOneLineHeight()),
                                   oneLineRegion);

        p.translate(0, getOneLineHeight());
        indentCounter += getOneLineHeight();

        oneLineRegion.startPos += symbolsPerLine;

    } while (oneLineRegion.startPos < visibleRange.endPos());

    // move painter back to [0, 0] position
    p.translate(0, - indentCounter);
}

void DetViewMultiLineRenderer::drawCursor(QPainter &p, const QSize &canvasSize, const U2Region& visibleRange) {
    CHECK(detView->isEditMode(), );

    int symbolsPerLine = getSymbolsPerLine(canvasSize.width());
    U2Region oneLineRegion(visibleRange.startPos, symbolsPerLine);
    int indentCounter = 0;
    do {
        // cut the extra space at the end of the sequence
        oneLineRegion.length = qMin(visibleRange.endPos() - oneLineRegion.startPos, oneLineRegion.length);
        singleLinePainter->drawCursor(p,
                                   QSize(canvasSize.width(), getOneLineHeight()),
                                   oneLineRegion);

        p.translate(0, getOneLineHeight());
        indentCounter += getOneLineHeight();

        oneLineRegion.startPos += symbolsPerLine;

    } while (oneLineRegion.startPos < visibleRange.endPos());

    // move painter back to [0, 0] position
    p.translate(0, - indentCounter);
}

void DetViewMultiLineRenderer::update() {
    singleLinePainter->update();
}

} // namespace
