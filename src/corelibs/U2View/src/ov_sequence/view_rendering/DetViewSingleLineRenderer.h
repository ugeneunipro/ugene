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

#ifndef _U2_DET_VIEW_SINGLE_LINE_RENDERER_H_
#define _U2_DET_VIEW_SINGLE_LINE_RENDERER_H_

#include "DetViewRenderer.h"

namespace U2 {


/************************************************************************/
/* DetViewSingleLineRenderer */
/************************************************************************/
class DetViewSingleLineRenderer : public DetViewRenderer {
private:
    struct TranslationMetrics {
        TranslationMetrics();
        TranslationMetrics(DetView* detView,
                           const U2Region& visibleRange,
                           const QFont& commonSequenceFont);

        QVector<bool> visibleRows;

        qint64 minUsedPos;
        qint64 maxUsedPos;
        U2Region seqBlockRegion;

        QColor startC;
        QColor stopC;

        QFont fontB;
        QFont fontI;

        QFont sequenceFontSmall;
        QFont fontBS;
        QFont fontIS;
    };
public:
    DetViewSingleLineRenderer(DetView* detView, SequenceObjectContext* ctx);

    virtual qint64 coordToPos(const QPoint &p, const QSize &canvasSize, const U2Region &visibleRange) const;

    virtual U2Region getAnnotationYRange(Annotation *a, int r, const AnnotationSettings* as, const QSize& canvasSize, const U2Region& visibleRange) const;
    virtual U2Region getMirroredYRange(const U2Strand &mStrand) const;

    qint64 getMinimumHeight() const;
    qint64 getOneLineHeight() const;
    qint64 getLinesCount(const QSize& canvasSize) const;
    qint64 getContentIndentY(const QSize& canvasSize, const U2Region& visibleRange) const;

    int getDirectLine() const { return directLine; }

    int getRowsInLineCount() const;

    QSize getBaseCanvasSize(const U2Region &visibleRange) const;

    bool isOnTranslationsLine(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const;
    bool isOnAnnotationLine(const QPoint& p, Annotation*a, int region, const AnnotationSettings *as, const QSize &canvasSize, const U2Region& visibleRange) const;

    virtual void drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);
    virtual void drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);
    virtual void drawCursor(QPainter &p, const QSize &canvasSize, const U2Region& visibleRange);

    void update();

private:
    void drawDirect(QPainter& p, const U2Region& visibleRange);
    void drawComplement(QPainter& p, const U2Region& visibleRange);

    void drawTranslations(QPainter& p, const U2Region& visibleRange);
    void drawDirectTranslations(QPainter& p,
                                const U2Region& visibleRange,
                                const char* seqBlock,
                                const QList<SharedAnnotationData>& annotationsInRange);
    void drawComplementTranslations(QPainter& p,
                                     const U2Region& visibleRange,
                                     const char* seqBlock,
                                     const QList<SharedAnnotationData>& annotationsInRange);

    void drawRuler(QPainter& p, const QSize &canvasSize, const U2Region& visibleRange);
    void drawSequenceSelection(QPainter& p, const QSize &canvasSize, const U2Region& visibleRange);

private:
    int getLineY(int line) const;
    int getTextY(int line) const;

    int getVisibleDirectTransLine(int absoluteFrameNumber) const;
    int getVisibleComplTransLine(int absoluteFrameNumber) const;

    void updateLines();

    bool deriveTranslationCharColor(qint64 pos,
                                    const U2Strand &strand,
                                    const QList<SharedAnnotationData> &annotationsInRange,
                                    QColor &result);
    void setFontAndPenForTranslation(const char* seq,
                                     const QColor& charColor,
                                     bool inAnnotation,
                                     QPainter& p);

    void highlight(QPainter& p, const U2Region& regionToHighlight, int line, const QSize& canvasSize, const U2Region& visibleRange);

    int posToDirectTransLine(int p) const;
    int posToComplTransLine(int p) const;

private:
    TranslationMetrics  trMetrics;

    int numLines;
    int rulerLine;
    int directLine;
    int complementLine;
    int firstDirectTransLine;
    int firstComplTransLine;
};

} // namespace

#endif // _U2_DET_VIEW_SINGLE_LINE_RENDERER_H_
