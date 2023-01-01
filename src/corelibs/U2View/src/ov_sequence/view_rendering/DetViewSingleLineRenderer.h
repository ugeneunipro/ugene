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
        TranslationMetrics(const SequenceObjectContext* ctx,
                           const U2Region& visibleRange,
                           const QFont& commonSequenceFont);

        /** Visible translation frames. */
        QVector<bool> visibleFrames;

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

    qint64 coordToPos(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const override;

    /** Returns all y regions used to draw the given location of the annotation. */
    QList<U2Region> getAnnotationYRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, const QSize& canvasSize, const U2Region& visibleRange) const override;

    U2Region getAnnotationYRange(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, int availableHeight) const override;

    U2Region getCutSiteYRange(const U2Strand& mStrand, int availableHeight) const override;

    int getMinimumHeight() const override;
    qint64 getOneLineHeight() const override;
    qint64 getLinesCount(const QSize& canvasSize) const override;

    int getDirectLine() const override {
        return directLine;
    }

    int getRowsInLineCount() const override;

    QSize getBaseCanvasSize(const U2Region& visibleRange) const override;

    bool isOnTranslationsLine(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const override;
    bool isOnAnnotationLine(const QPoint& p, Annotation* a, int region, const AnnotationSettings* as, const QSize& canvasSize, const U2Region& visibleRange) const override;

    void drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) override;
    void drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) override;
    void drawCursor(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) override;

    void update() override;

private:
    void drawDirect(QPainter& p, int availableHeight, const U2Region& visibleRange);
    void drawComplement(QPainter& p, int availableHeight, const U2Region& visibleRange);

    void drawTranslations(QPainter& p, int availableHeight, const U2Region& visibleRange);

    /**
     * Draws direct translation rows.
     * 'visibleSequence' is a pointer to the visible region sequence with -1 & +1 extra bases required for the complete translation.
     * */
    void drawDirectTranslations(QPainter& painter,
                                const U2Region& visibleRange,
                                const char* visibleSequence,
                                const QList<SharedAnnotationData>& annotationsInRange,
                                const TranslationMetrics& translationMetrics,
                                int availableHeight);

    void drawComplementTranslations(QPainter& painter,
                                    const U2Region& visibleRange,
                                    const char* seqBlock,
                                    const QList<SharedAnnotationData>& annotationsInRange,
                                    TranslationMetrics& translationMetrics,
                                    int availableHeight);

    void drawRuler(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);
    void drawSequenceSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);

private:
    int getLineY(int line, int availableHeight) const;
    int getTextY(int line, int availableHeight) const;
    int getContentIndentY(int canvasHeight) const;

    int getVisibleDirectTransLine(int absoluteFrameNumber) const;
    int getVisibleComplTransLine(int absoluteFrameNumber) const;

    void updateLines();

    bool deriveTranslationCharColor(qint64 pos,
                                    const U2Strand& strand,
                                    const QList<SharedAnnotationData>& annotationsInRange,
                                    QColor& result);
    void setFontAndPenForTranslation(const char* seq,
                                     const QColor& charColor,
                                     bool inAnnotation,
                                     QPainter& p,
                                     const TranslationMetrics& translationMetrics);

    void highlight(QPainter& p, const U2Region& regionToHighlight, int line, const QSize& canvasSize, const U2Region& visibleRange);

    int posToDirectTransLine(int p) const;
    int posToComplTransLine(int p) const;

private:
    int numLines;
    int rulerLine;

    /** Index of the direct line. */
    int directLine;

    /** Index of the complement line. -1 if the complement line is not visible. */
    int complementLine;

    int firstDirectTransLine;
    int firstComplTransLine;
};

}  // namespace U2

#endif  // _U2_DET_VIEW_SINGLE_LINE_RENDERER_H_
