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

#ifndef _U2_DET_VIEW_RENDERER_H_
#define _U2_DET_VIEW_RENDERER_H_

#include "SequenceViewAnnotatedRenderer.h"


namespace U2 {

class ADVSequenceObjectContext;
class DetView;

/************************************************************************/
/* DetViewRenderer */
/************************************************************************/
class U2VIEW_EXPORT DetViewRenderer : public SequenceViewAnnotatedRenderer {
public:
    DetViewRenderer(DetView* detView, SequenceObjectContext* ctx);
    virtual ~DetViewRenderer() {}

    double getCurrentScale() const;
    qint64 getSymbolsPerLine(const qint64 width) const;
    virtual int getDirectLine() const = 0;

    virtual void drawCursor(QPainter &p, const QSize &canvasSize, const U2Region& visibleRange) = 0;

    virtual int getRowsInLineCount() const = 0;

    virtual bool isOnTranslationsLine(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const = 0;
    virtual bool isOnAnnotationLine(const QPoint& p, Annotation*a, int region, const AnnotationSettings *as, const QSize &canvasSize, const U2Region& visibleRange) const = 0;

    virtual qint64 getOneLineHeight() const = 0;
    virtual qint64 getLinesCount(const QSize& canvasSize) const = 0;

    virtual void update() = 0;

protected:
    DetView*    detView;
};

/************************************************************************/
/* DetViewRendererFactory */
/************************************************************************/
class DetViewRendererFactory {
public:
    static DetViewRenderer* createRenderer(DetView* detView, SequenceObjectContext* ctx, bool multiLine = false);
};

} // namespace

#endif // _U2_DET_VIEW_RENDERER_H_
