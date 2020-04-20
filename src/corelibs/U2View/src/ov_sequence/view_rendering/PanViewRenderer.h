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

#ifndef _U2_PAN_VIEW_PAINTER_H_
#define _U2_PAN_VIEW_PAINTER_H_

#include <U2Gui/GraphUtils.h>

#include "SequenceViewAnnotatedRenderer.h"

namespace U2 {

class ADVSequenceObjectContext;
class PanView;
class PanViewLinesSettings;
class PanViewRenderArea;
class PVRowData;

/************************************************************************/
/* DetViewAnnotationPainter */
/************************************************************************/
class PanViewRenderer : public SequenceViewAnnotatedRenderer {
    Q_OBJECT
public:
    PanViewRenderer(PanView* panView, SequenceObjectContext* ctx);
    virtual ~PanViewRenderer() {}

    qint64 coordToPos(const QPoint &p, const QSize &canvasSize, const U2Region &visibleRange) const;

    double getCurrentScale() const;

    U2Region getAnnotationYRange(Annotation *a, int r, const AnnotationSettings* as, const QSize& canvasSize, const U2Region& visibleRange) const;
    U2Region getMirroredYRange(const U2Strand &mStrand) const;

    qint64 getContentIndentY(const QSize& canvasSize, const U2Region& visibleRange) const;
    qint64 getMinimumHeight() const;

    QSize getBaseCanvasSize(const U2Region &visibleRange) const;
    void drawAll(QPainter& p, const U2Region& visibleRange);

    void drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);
    void drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);

    void drawAnnotations(QPainter &p, const QSize &canvasSize, const U2Region &visibleRange, const AnnotationDisplaySettings& displaySettings);

protected:
    int getLineY(int line) const;
    bool isSequenceCharsVisible() const;

    PanView*            panView;
    PanViewLinesSettings*   s;

private:
    virtual void drawSequence(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);
    void drawSequenceSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange);

    virtual void drawRuler(GraphUtils::RulerConfig c,  QPainter& p, const U2Region &visibleRange, int firstCharCenter, int firstLastWidth);
    void drawCustomRulers(GraphUtils::RulerConfig c,  QPainter& p, const U2Region &visibleRange,
                          int firstCharCenter, int lastCharCenter,
                          int width, int predefinedY = -1, bool ignoreVisibleRange = false);

    const QString getText(const PVRowData * rData) const;

    static const int RULER_NOTCH_SIZE;
//    static const int MAX_VISIBLE_ROWS;
//    static const int MAX_VISIBLE_ROWS_ON_START;
    static const int LINE_TEXT_OFFSET;
};

class PanViewRenderAreaFactory {
public:
    PanViewRenderAreaFactory();
    virtual ~PanViewRenderAreaFactory();

    virtual PanViewRenderArea *createRenderArea(PanView *panView) const;
};

} // namespace

#endif // _U2_PAN_VIEW_PAINTER_H_
