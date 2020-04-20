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

#ifndef _U2_SEQUENCE_VIEW_RENDERER_H_
#define _U2_SEQUENCE_VIEW_RENDERER_H_

#include <QFont>

#include <U2Core/global.h>

namespace U2 {

class SequenceObjectContext;
class U2Region;

struct CommonSequenceViewMetrics {
    CommonSequenceViewMetrics();

    QFont   sequenceFont;
    QFont   smallSequenceFont;
    QFont   rulerFont;

    int     charWidth;
    int     smallCharWidth;

    int     lineHeight;
    int     yCharOffset;
    int     xCharOffset;
};

/************************************************************************/
/* SequenceViewRenderer */
/************************************************************************/
class U2VIEW_EXPORT SequenceViewRenderer : public QObject {
public:
    SequenceViewRenderer(SequenceObjectContext* ctx);

    virtual qint64 coordToPos(const QPoint& p, const QSize& canvasSize, const U2Region& visibleRange) const = 0;

    int posToXCoord(const qint64 p, const QSize& canvasSize, const U2Region& visibleRange) const;
    virtual float posToXCoordF(const qint64 p, const QSize& canvasSize, const U2Region& visibleRange) const;

    virtual qint64 getRowLineHeight() const;

    virtual double getCurrentScale() const = 0;

    virtual qint64 getContentIndentY(const QSize& canvasSize, const U2Region& visibleRange) const = 0;
    virtual qint64 getMinimumHeight() const = 0;

    virtual void drawAll(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) = 0;
    virtual void drawSelection(QPainter& p, const QSize& canvasSize, const U2Region& visibleRange) = 0;

    virtual QSize getBaseCanvasSize(const U2Region& visibleRange) const = 0;

protected:
    SequenceObjectContext*      ctx;
    CommonSequenceViewMetrics   commonMetrics;

};

} // namespace

#endif // _U2_SEQUENCE_VIEW_RENDERER_H_
