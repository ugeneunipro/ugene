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

#ifndef _U2_DET_VIEW_MULTI_LINE_RENDERER_H_
#define _U2_DET_VIEW_MULTI_LINE_RENDERER_H_

#include "DetViewRenderer.h"

namespace U2 {

class DetViewSingleLineRenderer;

/************************************************************************/
/* DetViewMultiLineRenderer */
/************************************************************************/
class DetViewMultiLineRenderer : public DetViewRenderer {
public:
    DetViewMultiLineRenderer(DetView* detView, SequenceObjectContext* ctx);
    ~DetViewMultiLineRenderer();

    qint64 coordToPos(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const override;
    int posToXCoord(qint64 pos, const QSize& canvasSize, const U2Region& visibleRange) const override;

    /** Returns all y regions used to draw the given location of the annotation. */
    QList<U2Region> getAnnotationYRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, const QSize& canvasSize, const U2Region& visibleRange) const override;

    U2Region getCutSiteYRange(const U2Strand& mStrand, int availableHeight) const override;

    int getMinimumHeight() const override;
    qint64 getOneLineHeight() const override;
    qint64 getLinesCount(const QSize& canvasSize) const override;

    int getDirectLine() const override;

    int getRowsInLineCount() const override;

    QSize getBaseCanvasSize(const U2Region& visibleRange) const override;

    bool isOnTranslationsLine(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const override;
    bool isOnAnnotationLine(const QPoint& p, Annotation* a, int region, const AnnotationSettings* as, const QSize& canvasSize, const U2Region& visibleRange) const override;

    void drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) override;
    void drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) override;
    void drawCursor(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) override;

    void update() override;

protected:
    U2Region getAnnotationYRange(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings, int availableHeight) const override;

private:
    DetViewSingleLineRenderer* singleLineRenderer;

    int extraIndent;

    static const int INDENT_BETWEEN_LINES;
};

}  // namespace U2

#endif  // _U2_DET_VIEW_MULTI_LINE_RENDERER_H_
