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

#ifndef _U2_GRAPH_UTILS_H_
#define _U2_GRAPH_UTILS_H_

#include <QColor>

#include <U2Core/global.h>

class QPainter;

namespace U2 {

#define PI 3.1415926535897932384626433832795

class U2GUI_EXPORT GraphUtils {
public:
    enum Direction {
        LeftToRight,
        RightToLeft,
        BottomToTop,
        TopToBottom
    };

    enum TextPos {
        LEFT,
        RIGHT
    };

    struct RulerConfig {
        RulerConfig() {
            drawArrow = false;
            arrowLen = 5;
            arrowWidth = 2;
            notchSize = 2;
            textOffset = 3;
            drawNumbers = true;
            drawNotches = true;
            drawBorderNotches = true;
            singleSideNotches = false;
            drawAxis = true;
            direction = LeftToRight;
            textPosition = RIGHT;
            extraAxisLenBefore =0;
            extraAxisLenAfter =0;
            textBorderStart = 2;
            textBorderEnd = 2;
            predefinedChunk = 0;
            correction = 0;
        }
        bool    drawArrow;
        int     arrowLen;
        int     arrowWidth;
        int     notchSize;
        int     textOffset;
        int     extraAxisLenBefore;
        int     extraAxisLenAfter;
        int     textBorderStart; //offset in pixels for the first text label at the start pos
        int     textBorderEnd;   //offset in pixels for the last text label at the end pos
        int     predefinedChunk;
        int     correction;
        bool    drawNumbers;
        bool    drawNotches;
        bool    drawBorderNotches;
        bool    singleSideNotches;
        bool    drawAxis;
        Direction direction;
        TextPos   textPosition;
    };

    struct U2GUI_EXPORT ArrowConfig {
        ArrowConfig();

        qreal lineWidth;
        qreal lineLength;
        qreal arrowHeadWidth;
        qreal arrowHeadLength;
        QColor color;
        Direction direction;
    };

    static void drawRuler(QPainter& p, const QPoint& pos, qint64 len, qint64 start, qint64 end, const QFont& f, const RulerConfig& config);
    static int calculateChunk(qint64 start, qint64 end, qint64 len, const QPainter &p);
    static int findChunk(qint64 len, qint64 span, int N);

    //static void drawDensityPlot(QPainter& p, QRect& drawRect, QRect& calcRect, quint32 n, quint32* x, quint32 *y, quint32* len);

    static QColor proposeLightColorByKey(const QString& key);

    /*
       The method is designed to be used by different algorithms that needs to
       select some "nice looking" number in range to be presented to user.

       Selects "nice looking" number below maxVal.
       Example (input, result) : (231, 200), (49, 40), (12421452, 12000000)
       Works only for positive numbers, returns maxVal for negative or zero
    */
    static qint64 pickRoundedNumberBelow(qint64 maxVal);

    static void drawArrow(QPainter &painter, const QRectF &rect, const ArrowConfig &config);
};

}   // namespace U2

#endif // _U2_GRAPH_UTILS_H_
