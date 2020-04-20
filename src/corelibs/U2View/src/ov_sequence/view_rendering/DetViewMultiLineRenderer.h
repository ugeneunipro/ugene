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

#ifndef _U2_DET_VIEW_MULTI_LINE_RENDERER_H_
#define _U2_DET_VIEW_MULTI_LINE_RENDERER_H_

#include "DetViewRenderer.h"

namespace U2 {

class DetViewSingleLineRenderer;

/************************************************************************/
/* DetViewMultiLineRenderer */
/************************************************************************/
class DetViewMultiLineRenderer: public DetViewRenderer {
public:
    DetViewMultiLineRenderer(DetView* detView, SequenceObjectContext* ctx);
    ~DetViewMultiLineRenderer();

    qint64 coordToPos(const QPoint &p, const QSize &canvasSize, const U2Region &visibleRange) const;
    float posToXCoordF(const qint64 p, const QSize& canvasSize, const U2Region& visibleRange) const;

    U2Region getAnnotationYRange(Annotation *a, int r, const AnnotationSettings* as, const QSize& canvasSize, const U2Region& visibleRange) const;
    U2Region getMirroredYRange(const U2Strand &mStrand) const;

    qint64 getMinimumHeight() const;
    qint64 getOneLineHeight() const;
    qint64 getLinesCount(const QSize& canvasSize) const;
    qint64 getContentIndentY(const QSize& canvasSize, const U2Region& visibleRange) const;

    int getDirectLine() const;

    int getRowsInLineCount() const;

    QSize getBaseCanvasSize(const U2Region &visibleRange) const;

    bool isOnTranslationsLine(const QPoint &p, const QSize& canvasSize, const U2Region& visibleRange) const;
    bool isOnAnnotationLine(const QPoint& p, Annotation*a, int region, const AnnotationSettings *as, const QSize &canvasSize, const U2Region& visibleRange) const;

    void drawAll(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange);
    void drawSelection(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange);
    void drawCursor(QPainter &p, const QSize &canvasSize, const U2Region& visibleRange);

    void update();

private:
    DetViewSingleLineRenderer*    singleLinePainter;

    int extraIndent;

    static const int INDENT_BETWEEN_LINES;
};

} // namespace

#endif // _U2_DET_VIEW_MULTI_LINE_RENDERER_H_
