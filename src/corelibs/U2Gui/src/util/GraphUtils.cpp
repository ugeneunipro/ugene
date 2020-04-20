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

#include <math.h>

#include <QPainter>
#include <QVector>

#include <U2Core/FormatUtils.h>
#include <U2Core/U2SafePoints.h>

#include "GraphUtils.h"

namespace U2 {

GraphUtils::ArrowConfig::ArrowConfig()
    : lineWidth(0),
      lineLength(0),
      arrowHeadWidth(0),
      arrowHeadLength(0),
      direction(LeftToRight)
{

}

static void drawNum(QPainter& p, int x1, int x2, const QString& num, int lBorder, int rBorder, int y1, int y2) {
    if (x1 < lBorder|| x2 > rBorder) {
        return;
    }
    QRect rect(x1, y1, x2-x1+1, y2-y1+1);
    p.drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, num);
}

#define MIN_RULER_LEN 10
void GraphUtils::drawRuler(QPainter& p, const QPoint& pos, qint64 len, qint64 start, qint64 end, const QFont& font, const RulerConfig& c) {

    if (start == end || len < MIN_RULER_LEN) {
        return;
    }
    p.save();

    assert(c.drawArrow ? c.drawAxis : true);

    p.setFont(font);
    QFontMetrics fm = p.fontMetrics();
    int cw = fm.width('0');
    int ch = fm.height();
    QString st = FormatUtils::splitThousands(start);
    QString en = FormatUtils::splitThousands(end);

    int stW = fm.width(st);
    int enW = fm.width(en);

    qint64 span = qMax(start, end) - qMin(start, end);
    int chunk = c.predefinedChunk == 0 ? calculateChunk(start, end, len, p) : c.predefinedChunk;

    while (chunk > span) {
        chunk /= 2;
    }
    float scale =  len / (float)span;
    if (c.direction == BottomToTop) {
        if(c.drawAxis) {
            p.drawLine(pos.x(), pos.y() - c.extraAxisLenBefore, pos.x(), pos.y() + len + c.extraAxisLenAfter);
        }
        if (c.drawBorderNotches) {
            p.drawLine(pos.x()-c.notchSize, pos.y(), pos.x() + c.notchSize, pos.y());
            p.drawLine(pos.x()-c.notchSize, pos.y() + len, pos.x() + c.notchSize, pos.y() + len);
        }
        if (c.drawArrow) {
            p.drawLine(pos.x(), pos.y(), pos.x() - c.arrowWidth, pos.y() + c.arrowLen);
            p.drawLine(pos.x(), pos.y(), pos.x() + c.arrowWidth, pos.y() + c.arrowLen);
        }
        if (c.drawNumbers) {
            if (c.textPosition == LEFT) {
                p.drawText(pos.x() - c.textOffset - st.length() * cw, pos.y() + len, st);
                p.drawText(pos.x() - c.textOffset - en.length() * cw, pos.y() + ch, en);
            } else {
                assert(c.textPosition == RIGHT);
                p.drawText(pos.x() + c.textOffset, pos.y() + len, st);
                p.drawText(pos.x() + c.textOffset, pos.y() + ch, en);
            }
        }
    } else if (c.direction == TopToBottom) {
        int notchDX1 =  c.notchSize;
        int notchDX2 =  c.notchSize;
        QFontMetrics fm(font);
        int fontHeight = fm.height();
        qint64 fontCenteringOffset = fontHeight / 2 - 2; // -2 is for baseline offset
        if(c.drawAxis) {
            p.drawLine(pos.x(), pos.y() - c.extraAxisLenBefore , pos.x(), pos.y() + len + c.extraAxisLenAfter);
        }
        if (c.drawBorderNotches) {
            p.drawLine(pos.x()- c.notchSize, pos.y(), pos.x() + c.notchSize, pos.y());
            p.drawLine(pos.x()- c.notchSize, pos.y() + len, pos.x() + c.notchSize, pos.y() + len);
        }
        if (c.drawArrow) {
            p.drawLine(pos.x(), pos.y() + len, pos.x() - c.arrowWidth, pos.y() + len - c.arrowLen);
            p.drawLine(pos.x(), pos.y() + len, pos.x() + c.arrowWidth, pos.y() + len - c.arrowLen);
        }
        if (start / chunk != end / chunk) {
            qint64 currnotch;
            if(c.correction != 0){
                currnotch = c.correction;
            }else{
                currnotch = chunk * (start / chunk + 1);
            }
            for (; currnotch < end; currnotch += chunk) {
                int y = qRound((end - currnotch) * scale);
                if (c.drawNotches) {
                    p.drawLine(pos.x() - notchDX1, pos.y() + len - y, pos.x() + notchDX2, pos.y() + len - y);
                }
                if (c.drawNumbers) {
                    QString snum = FormatUtils::formatNumber(currnotch);
                    qint64 textY = pos.y() + len - (y - fontCenteringOffset);
                    if (y > fontHeight && textY > pos.y() + fontHeight) {
                        if (c.textPosition == LEFT) {
                            qint64 textX = pos.x() - c.textOffset - snum.length() * cw;
                            p.drawText(textX, textY, snum);
                        } else {
                            assert(c.textPosition == RIGHT);
                            qint64 textX = pos.x() + c.textOffset;
                            p.drawText(textX, textY, snum);
                        }
                    }
                }
            }
        }
        if (c.drawNumbers) {
            QString startStr = QString::number(start);
            if (c.textPosition == LEFT) {
                p.drawText(pos.x() - c.textOffset - startStr.length() * cw, pos.y() + fontCenteringOffset, startStr);
            } else {
                assert(c.textPosition == RIGHT);
                p.drawText(pos.x() + c.textOffset - startStr.length() * cw, pos.y() + fontCenteringOffset, startStr);
            }
            QString endStr = QString::number(end);
            if (c.textPosition == LEFT) {
                p.drawText(pos.x() - c.textOffset - endStr.length() * cw, pos.y() + len, endStr);
            } else {
                assert(c.textPosition == RIGHT);
                p.drawText(pos.x() + c.textOffset - endStr.length() * cw, pos.y() + len, endStr);
            }
        }
    } else {
        int notchDY1 =  c.notchSize;
        int notchDY2 =  c.notchSize;
        if (c.singleSideNotches) {
            if (c.textPosition == LEFT) {
                notchDY2 = 0;
            } else {
                notchDY1 = 0;
            }
        }
        if(c.drawAxis) {
            p.drawLine(pos.x() - c.extraAxisLenBefore, pos.y(), pos.x() + len + c.extraAxisLenAfter, pos.y());
        }
        if (c.drawBorderNotches) {
            p.drawLine(pos.x(), pos.y() - notchDY1, pos.x(), pos.y() + notchDY2);
            p.drawLine(pos.x() + len, pos.y() - notchDY1, pos.x() + len, pos.y() + notchDY2);
        }

        //text properties
        int yt1 = c.textPosition == LEFT ? pos.y() - c.textOffset - ch : pos.y() + c.textOffset;
        int yt2 = c.textPosition == LEFT ? pos.y() - c.textOffset : pos.y() + c.textOffset + ch;
        int stX1 = pos.x() + c.textBorderStart;
        int stX2 = pos.x() + stW;
        int enX1 = pos.x() + len - enW;
        int enX2 = pos.x() + len - c.textBorderEnd;
        if (c.drawNumbers) {
            drawNum(p, stX1, stX2, st, stX1, stX2, yt1, yt2);
            drawNum(p, enX1, enX2, en, enX1, enX2, yt1, yt2);
        }

        if (c.direction == LeftToRight) {
            int leftborder = stX2;
            int rightborder= enX1;
            if (c.drawArrow) {
                p.drawLine(pos.x() + len, pos.y(), pos.x() + len - c.arrowLen, pos.y() - c.arrowWidth);
                p.drawLine(pos.x() + len, pos.y(), pos.x() + len - c.arrowLen, pos.y() + c.arrowWidth);
            }
            if (start / chunk != end / chunk) {
                qint64 currnotch;
                if(c.correction != 0){
                    currnotch = c.correction;
                }else{
                    currnotch = chunk * (start / chunk + 1);
                }
                for (; currnotch < end; currnotch += chunk) {
                    int x = qRound(scale * (currnotch - start));
                    if (c.drawNotches) {
                        p.drawLine(pos.x() + x, pos.y() - notchDY1, pos.x() + x, pos.y() + notchDY2);
                    }
                    if (c.drawNumbers) {
                        QString snum = FormatUtils::formatNumber(currnotch);
                        int notchleft = pos.x() + x - snum.length() * cw / 2;
                        int notchright = notchleft + snum.length() * cw;
                        drawNum(p, notchleft, notchright, snum, leftborder, rightborder, yt1, yt2);
                    }
                }
            }
        } else if (c.direction == RightToLeft) {
            int leftborder = enX2;
            int rightborder= stX1;
            if (c.drawArrow) {
                p.drawLine(pos.x(), pos.y(), pos.x() + c.arrowLen, pos.y() - c.arrowWidth);
                p.drawLine(pos.x(), pos.y(), pos.x() + c.arrowLen, pos.y() + c.arrowWidth);
            }
            if (start / chunk != end / chunk) {
                qint64 currnotch;
                if(c.correction != 0){
                    currnotch = c.correction;
                }else{
                    currnotch = chunk * (start / chunk + 1);
                }
                for (; currnotch < end; currnotch += chunk) {
                    int x = qRound((currnotch - start) * scale);
                    if (c.drawNotches) {
                        p.drawLine(pos.x() + len - x, pos.y() - notchDY1, pos.x() + len - x, pos.y() + notchDY2);
                    }
                    if (c.drawNumbers) {
                        QString snum = FormatUtils::formatNumber(currnotch);
                        int notchleft = pos.x() + len - x - snum.length() * cw / 2;
                        int notchright = notchleft + snum.length() * cw;
                        drawNum(p, notchleft, notchright, snum, leftborder, rightborder, yt1, yt2);
                    }
                }
            }
        }
    }
    p.restore();
}

int GraphUtils::findChunk(qint64 len, qint64 span, int N) {
    int dN = 0;
    int chunk = 1;
    while (span > 2 * chunk) {
        //dN = chunk > (1000 * 1000)? cw * 2 : (chunk > 1000 ?  cw * 2 : 0);
        qint64 reqLen = qint64((double(span) / chunk) * (N - dN));
        assert(reqLen > 0);
        if (reqLen < len) {
            break;
        }
        if (reqLen / 2 < len) {
            int cchunk = chunk*2;
            //dN = cchunk > (1000 * 1000)? cw * 2 :cchunk > 1000 ? cw * 2 : 0;
            qint64 reqLen2 = qint64((double(span) / cchunk) * (N - dN));
            if (reqLen2 < len) {
                chunk = cchunk;
                break;
            }
        }

        if (reqLen / 5 < len) {
            int cchunk = chunk*5;
            //dN = cchunk > (1000 * 1000)? cw * 2 :cchunk > 1000 ? cw * 2 : 0;
            qint64 reqLen5 = qint64((double(span) / cchunk) * (N - dN));
            if (reqLen5 < len) {
                chunk = cchunk;
                break;
            }
        }
        chunk *= 10;
    }
    assert(span / chunk < 1000);
    while (chunk > span) {
        chunk /= 2;
    }
    return chunk;
}


/*
void GraphUtils::drawDensityPlot(QPainter& p, QRect& drawRect,
    QRect& calcRect, quint32 n, quint32* x, quint32* y, quint32* len){
    if (n == 0) {
        return;
    }
    quint32 PIX_SIZE = 2;
    quint32 squaresX = drawRect.width() / PIX_SIZE;
    quint32 squaresY = drawRect.height() / PIX_SIZE;
    if (squaresX == 0 || squaresY == 0) {
        return;
    }
    quint32 dMapSize = squaresX* squaresY;
    quint32* densityMap = new quint32[dMapSize];
    memset(densityMap, 0, dMapSize * 4);
    double xScale = calcRect.width() / (double) squaresX;
    double yScale = calcRect.height() / (double) squaresY;
    quint32 offsetX = calcRect.x();
    quint32 offsetY = calcRect.y();
    quint32 i;
    for (i = 0; i < n; i++) {
        quint32 x0 = x[i] - offsetX;
        quint32 y0 = y[i] - offsetY;
        quint32 l = len[i];

        quint32 sx = quint32(x0 / xScale);
        quint32 sy = quint32(y0 / yScale);
        quint32 index = sx* squaresY + sy;
        densityMap[index]++;
        if (l > xScale || l > yScale) {
            quint32 lx = l;
            quint32 ly = l;
            while (lx || ly) {
                if (lx) {
                    sx++;
                    lx = lx > xScale ? quint32(lx - xScale) : 0;
                }
                if (ly) {
                    sy++;
                    ly = ly > yScale ? quint32(ly - yScale) : 0;
                }
            }
            index = sx * squaresY + sy;
            densityMap[index]++;
        }
    }

    float maxDensity = 0;
    for (i = 0; i < dMapSize; i++) {
        if (densityMap[i] < maxDensity) {
            continue;
        }
        maxDensity = densityMap[i];
        if (maxDensity > 1000) {
            maxDensity = 1000;
            break;
        }
    }
    double k = 1.7;
    quint32 EXP = 0;
    while (maxDensity >= 1) {
        EXP++;
        maxDensity = maxDensity / k;
    }
    double m = 1;
    for (i = 0; i < EXP; i++) {
        m = m * k;
    }
    quint32 MAX = (quint32) m;
    quint32* colors = new quint32[MAX + 1];
    for (i = 0; i < MAX; colors[i++] = 0xFFFFFF)
        ;
    quint32 color;
    double exp = 1;
    float dc = 255. / (EXP - 1);
    for (i = 1; i <= EXP; i++) {
        double endExp = exp* k;
        quint32 c = 255 - (quint32) (i* dc);
        color = (c << 16) + (c << 8) + c;
        for (quint32 j = (quint32) exp; j <= (quint32) endExp; j++) {
            colors[j] = color;
        }
        exp = endExp;
    }
    colors[MAX] = 0;

    for (quint32 xi = 0; xi < squaresX; xi++) {
        quint32 xo = xi* squaresY;
        for (quint32 yj = 0; yj < squaresY; yj++) {
            quint32 d = densityMap[xo + yj];
            if (d == 0) {
                continue;
            }
            d > MAX && (d = MAX);
            color = colors[d];
            p.fillRect(QRect(xi * PIX_SIZE, yj * PIX_SIZE, PIX_SIZE, PIX_SIZE),
                QBrush(color));
        }
    }
    delete colors;
    delete[] densityMap;
}
*/

static QVector<QColor>  prepareColors() {
    QVector<QColor> colors(6*6*6);
    //00 = 0, 1 = 33, 2 = 66, 3 = 99, 4 = CC, 5 = FF
    int nLightColors = 0;
    for(int i=0; i< colors.count(); i++) {
        int color = i;
        int r = 0x33 * (color%6);
        int g = 0x33 * ((color/6)%6);
        int b = 0x33 * ((color/36)%6);
        if (r==0xFF || g==0xFF || b==0xFF) {
            colors[nLightColors] = QColor(r, g, b);
            nLightColors++;
        }

    }
    return colors;
}

QColor GraphUtils::proposeLightColorByKey(const QString& key) {
    //TODO: make thread safe!
    static QVector<QColor> colors = prepareColors();

    int hash =0;
    uint len = key.length();
    for (int j=len-1; j>=0; j--) {
        hash+=key.at(j).toLatin1();
    }
    return colors.at((hash*hash)%colors.size());
}

int GraphUtils::calculateChunk( qint64 start, qint64 end, qint64 len, const QPainter &p ){
    QFontMetrics fm = p.fontMetrics();
    int cw = fm.width('0');
    //the width of the bigger number
    int N = cw * qMax(QString::number(start).length(), QString::number(end).length()) * 4 / 3;
    qint64 span = qMax(start, end) - qMin(start, end);
    int chunk = 1;
    while (span > 2 * chunk) {
        int dN = chunk > (1000 * 1000)? cw * 2 : (chunk > 1000 ?  cw * 2 : 0);
        qint64 reqLen = qint64((double(span) / chunk) * (N - dN));
        assert(reqLen > 0);
        if (reqLen < len) {
            break;
        }
        if (reqLen / 2 < len) {
            int cchunk = chunk*2;
            dN = cchunk > (1000 * 1000)? cw * 2 :cchunk > 1000 ? cw * 2 : 0;
            qint64 reqLen2 = qint64((double(span) / cchunk) * (N - dN));
            if (reqLen2 < len) {
                chunk = cchunk;
                break;
            }
        }

        if (reqLen / 5 < len) {
            int cchunk = chunk*5;
            dN = cchunk > (1000 * 1000)? cw * 2 :cchunk > 1000 ? cw * 2 : 0;
            qint64 reqLen5 = qint64((double(span) / cchunk) * (N - dN));
            if (reqLen5 < len) {
                chunk = cchunk;
                break;
            }
        }
        chunk *= 10;
    }
    return chunk;
}

qint64 GraphUtils::pickRoundedNumberBelow(qint64 maxVal) {
    if (maxVal <= 0) {
        return maxVal;
    }
    //current implementation: starts with seed 1, multiplies sequentially by 2, 5 and steps to 10
    qint64 i = 1, res = 1;
    do {
        res = i;
        if (i * 10 <= maxVal) {
            i *= 10;
            continue;
        }
        if (i * 5 <= maxVal) {
            i *= 5;
            continue;
        }
        i *= 2;
    } while (i <= maxVal);
    return res;
}

void GraphUtils::drawArrow(QPainter &painter, const QRectF &rect, const ArrowConfig &config) {
    SAFE_POINT(LeftToRight == config.direction || RightToLeft == config.direction, "Vertical arrows drawing is not implemented", );
    painter.save();

    QLineF line;
    QPolygonF arrowHead(3);
    switch (config.direction) {
    case LeftToRight:
        arrowHead[0] = QPointF(rect.left() + config.lineLength, rect.top() + config.arrowHeadWidth / 2);
        arrowHead[1] = QPointF(rect.left() + config.lineLength - config.arrowHeadLength, rect.top());
        arrowHead[2] = QPointF(rect.left() + config.lineLength - config.arrowHeadLength, rect.top() + config.arrowHeadWidth);
        line.setP1(QPointF(rect.left(), rect.top() + config.arrowHeadWidth / 2));
        line.setP2(QPointF(rect.left() + config.lineLength - config.arrowHeadLength, rect.top() + config.arrowHeadWidth / 2));
        break;
    case RightToLeft:
        arrowHead[0] = QPointF(rect.left(), rect.top() + config.arrowHeadWidth / 2);
        arrowHead[1] = QPointF(rect.left() + config.arrowHeadLength, rect.top());
        arrowHead[2] = QPointF(rect.left() + config.arrowHeadLength, rect.top() + config.arrowHeadWidth);
        line.setP1(QPointF(rect.left() + config.arrowHeadLength, rect.top() + config.arrowHeadWidth / 2));
        line.setP2(QPointF(rect.left() + config.lineLength, rect.top() + config.arrowHeadWidth / 2));
        break;
    default:
        FAIL("Not implemented", );
    }

    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(config.color);
    pen.setWidth(config.lineWidth);
    painter.setPen(pen);
    painter.drawLine(line);

    painter.setPen(config.color);
    painter.setBrush(QBrush(config.color));
    painter.drawPolygon(arrowHead);
    painter.restore();
}

}   // namespace U2
