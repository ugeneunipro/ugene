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

#include "AlignmentLogo.h"

#include <U2View/MSAEditor.h>

#include <QPainter>
#include <QHBoxLayout>
#include "U2Core/MultipleSequenceAlignment.h"
#include "U2Core/DNAAlphabet.h"
#include <math.h>

namespace U2 {

/************************************************************************/
/* LogoRenderArea                                                       */
/************************************************************************/
AlignmentLogoRenderArea::AlignmentLogoRenderArea(const AlignmentLogoSettings& _s, QWidget* p)
: QWidget(p), settings(_s) {
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(this);
    p->setLayout(layout);



    bases<<'A'<<'G'<<'C'<<'T'<<'U';

/*aminoacids<<'A'<<'C'<<'D'<<'E'<<'F'<<'G'<<'H'
        <<'I'<<'K'<<'L'<<'M'<<'N'<<'P'<<'Q'<<'R'
        <<'S'<<'T'<<'V'<<'W'<<'Y';*/

    acceptableChars = new QVector<char>();
    switch (settings.sequenceType)
    {
        case NA:
            acceptableChars = &bases;
            s = 4.0;
            break;
        default:
            QByteArray chars = settings.ma->getAlphabet()->getAlphabetChars();
            foreach(char ch, chars) {
                if(ch!=U2Msa::GAP_CHAR)
                    acceptableChars->append(ch);
            }
            s = 20.0;
            acceptableChars = &aminoacids;
            break;
    }

    evaluateHeights();
    sortCharsByHeight();
}

void AlignmentLogoRenderArea::replaceSettings(const AlignmentLogoSettings& _s) {
    settings = _s;

    acceptableChars = new QVector<char>();
    switch (settings.sequenceType)
    {
        case NA:
            acceptableChars = &bases;
            s = 4.0;
            break;
        default:
            QByteArray chars = settings.ma->getAlphabet()->getAlphabetChars();
            foreach(char ch, chars) {
                if(ch!=U2Msa::GAP_CHAR)
                    acceptableChars->append(ch);
            }
            s = 20.0;
            acceptableChars = &aminoacids;
            break;
    }
    columns.clear();
    for (int i = 0; i < 256; i++) {
        frequencies[i].clear();
        heights[i].clear();
    }
    evaluateHeights();
    sortCharsByHeight();
}

#define SPACER 1
#define MIN_WIDTH 8
void AlignmentLogoRenderArea::paintEvent(QPaintEvent* e) {
    QPainter p(this);
    p.fillRect(0,0,width(),height(),Qt::white);
    QFont charFont("Helvetica");
    charFont.setPixelSize(bitHeight);
    charFont.setBold(true);

    for(int pos=0; pos < settings.len; pos++) {
        assert(pos < columns.size());
        const QVector<char>& charsAt = columns.at(pos);
        int yLevel = height();
        foreach(char ch, charsAt) {
            QPointF baseline(pos * (bitWidth + SPACER), yLevel);
            int charHeight = heights[(int)uchar(ch)][pos] * bitHeight;
            QColor charColor = settings.colorScheme[(int)uchar(ch)];
            AlignmentLogoItem* logoItem = new AlignmentLogoItem(ch, baseline, bitWidth, charHeight, charFont, charColor);
            logoItem->paint(&p, NULL, this);
            yLevel -= charHeight + SPACER;
        }
    }

    QWidget::paintEvent(e);
}

void AlignmentLogoRenderArea::resizeEvent(QResizeEvent* e) {
    bitWidth = qMax(width() / settings.ma->getLength() - SPACER, MIN_WIDTH);
    bitHeight = (height() - s) * log(2.0) / log(s);

    QWidget::resizeEvent(e);
}

void AlignmentLogoRenderArea::evaluateHeights() {
    const MultipleSequenceAlignment& ma = settings.ma;
    int numRows = ma->getNumRows();
    error = (s - 1)/(2*log(2.0)*numRows);

    foreach (char ch, *acceptableChars) {
        QVector<qreal> freqs(settings.len);
        QVector<qreal> hts(settings.len);
        frequencies[(int)uchar(ch)] = freqs;
        heights[(int)uchar(ch)] = hts;
    }
    columns.resize(settings.len);

    for (int pos = settings.startPos; pos < settings.len + settings.startPos; pos++) {
        for (int idx = 0; idx < numRows; idx++) {
            const MultipleSequenceAlignmentRow row = ma->getMsaRow(idx);
            assert(pos < ma->getLength());
            char ch = row->charAt(pos);
            if(acceptableChars->contains(ch)) {
                int arrIdx = pos - settings.startPos;
                assert(arrIdx >= 0);
                assert(arrIdx < frequencies[(int)uchar(ch)].size());
                frequencies[(int)uchar(ch)][arrIdx]+=1.0;
                if (!columns[arrIdx].contains(ch)) {
                    columns[arrIdx].append(ch);
                }
            }
        }
    }

    int rows = settings.ma->getNumRows();
    for(int pos=0; pos < settings.len; pos++) {
        qreal h = getH(pos);
        foreach(char c, columns[pos]) {
            qreal freq = frequencies[(int)uchar(c)][pos] / rows;
            heights[(int)uchar(c)][pos] = freq * ( log(s) / log(2.0) - ( h + error ) );
        }
    }
}

qreal AlignmentLogoRenderArea::getH(int pos) {
    qreal h = 0.0;
    int rows = settings.ma->getNumRows();
    foreach(char ch, columns.at(pos)) {
        qreal freq = frequencies[(int)uchar(ch)][pos] / rows;
        h += -freq * log(freq) / log(2.0);
    }
    assert (h >= 0.0);
    return h;
}

void AlignmentLogoRenderArea::sortCharsByHeight() {
    for(int pos = 0; pos < columns.size(); pos++) {
        QVector<char>& chars = columns[pos];
        char temp;
        int count = chars.size();
        for(int j = 0; j < chars.size() - 1; j++) {
            for(int i = 0; i < count - 1; i++) {
                temp = chars[i];
                qreal tempFreq = frequencies[(int)uchar(temp)][pos];
                qreal nextFreq = frequencies[(int)uchar(chars[i+1])][pos];
                if (tempFreq>nextFreq) {
                    chars[i] = chars[i+1];
                    chars[i+1] = temp;
                }
                else {
                    temp = chars[i+1];
                }
            }
            --count;
        }
    }
}

/************************************************************************/
/* Logo item                                                            */
/************************************************************************/
AlignmentLogoItem::AlignmentLogoItem(char _ch, QPointF _baseline, int _charWidth, int _charHeight, QFont _font, QColor _color)
: ch(_ch), baseline(_baseline), charWidth(_charWidth), charHeight(_charHeight), font(_font), color(_color) {}

QRectF AlignmentLogoItem::boundingRect() const {
    return path.boundingRect();
}

void AlignmentLogoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    QString chStr(ch);
    path.addText(baseline, font, chStr);

    //adjust item's height
    QRectF bound = path.boundingRect();

    //epsilon value needed to avoid division by extremely small values and raising exception in qt gui
    qreal eps = 1e-3;

    qreal sx = qMax(charWidth / bound.width(), eps);
    qreal sy = qMax(charHeight / bound.height(), eps);

    painter->scale(sx, sy);

    //map baseline position to scaled coordinates
    qreal offsetx = baseline.x() * (1/sx - 1);
    qreal offsety = baseline.y() * (1/sy - 1);
    painter->translate(offsetx, offsety);

    painter->fillPath(path, color);
    painter->restore();
}

}//namespace
