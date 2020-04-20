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

#ifndef _U2_ALIGNMENT_LOGO_H_
#define _U2_ALIGNMENT_LOGO_H_

#include <U2Core/global.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleAlignmentInfo.h>

#include <QMainWindow>
#include <QGraphicsItem>

namespace U2 {

class MSAEditor;

enum SequenceType {Auto, NA, AA};
/************************************************************************/
/* Settings                                                             */
/************************************************************************/
class U2VIEW_EXPORT AlignmentLogoSettings {
public:

    AlignmentLogoSettings(const MultipleSequenceAlignment& _ma) : ma(_ma->getCopy()) {
        for (int i = 0; i < 256; i++) {
            colorScheme[i] = Qt::black;
        }

        if (ma->getAlphabet()->isNucleic()) {
            sequenceType = NA;
            colorScheme['G'] = QColor(255, 128, 0);
            colorScheme['T'] = Qt::red;
            colorScheme['C'] = Qt::blue;
            colorScheme['A'] = Qt::green;
            colorScheme['U'] = Qt::red;
        } else if (ma->getAlphabet()->isAmino()) {
            sequenceType = AA;
        } else {
            sequenceType = Auto;
        }

        if (!ma->getAlphabet()->isNucleic()) {
            colorScheme['G'] = Qt::green;
            colorScheme['S'] = Qt::green;
            colorScheme['T'] = Qt::green;
            colorScheme['Y'] = Qt::green;
            colorScheme['C'] = Qt::green;
            colorScheme['N'] = QColor(192, 0, 192);
            colorScheme['Q'] = QColor(192, 0, 192);
            colorScheme['K'] = Qt::blue;
            colorScheme['R'] = Qt::blue;
            colorScheme['H'] = Qt::blue;
            colorScheme['D'] = Qt::red;
            colorScheme['E'] = Qt::red;
            colorScheme['P'] = Qt::black;
            colorScheme['A'] = Qt::black;
            colorScheme['W'] = Qt::black;
            colorScheme['F'] = Qt::black;
            colorScheme['L'] = Qt::black;
            colorScheme['I'] = Qt::black;
            colorScheme['M'] = Qt::black;
            colorScheme['V'] = Qt::black;
        }
        startPos = 0;
        len = ma->getLength();

        /*colorScheme.insert('S', Qt::green);
        colorScheme.insert('G', Qt::green);
        colorScheme.insert('H', Qt::green);
        colorScheme.insert('T', Qt::green);
        colorScheme.insert('A', Qt::green);
        colorScheme.insert('P', Qt::green);
        colorScheme.insert('Y', Qt::blue);
        colorScheme.insert('V', Qt::blue);
        colorScheme.insert('M', Qt::blue);
        colorScheme.insert('C', Qt::blue);
        colorScheme.insert('L', Qt::blue);
        colorScheme.insert('F', Qt::blue);
        colorScheme.insert('I', Qt::blue);
        colorScheme.insert('W', Qt::blue);*/
    }

    MultipleSequenceAlignment              ma;
    SequenceType            sequenceType;
    int                     startPos;
    int                     len;
    QColor                  colorScheme[256];

private:
    AlignmentLogoSettings();
};

/************************************************************************/
/* LogoRenderArea                                                       */
/************************************************************************/
class U2VIEW_EXPORT AlignmentLogoRenderArea : public QWidget {
public:
    AlignmentLogoRenderArea(const AlignmentLogoSettings& s, QWidget* p);
    void replaceSettings(const AlignmentLogoSettings& s);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void resizeEvent(QResizeEvent* e);

    void evaluateHeights();
    void sortCharsByHeight();
    qreal getH(int pos);

private:
    AlignmentLogoSettings           settings;
    qreal                           s;//4||20
    qreal                           error;
    QVector<char>*                  acceptableChars;
    QVector<char>                   bases;
    QVector<char>                   aminoacids;

    QVector<QVector<char> >         columns;
    //frequency and height of a particular char at position
    QVector<qreal>                  frequencies[256];
    QVector<qreal>                  heights[256];

    int                             bitWidth;
    int                             bitHeight;
};

/************************************************************************/
/* LogoItem                                                             */
/************************************************************************/
class AlignmentLogoItem : public QGraphicsItem {
public:
    AlignmentLogoItem(char _ch, QPointF _baseline, int _charWidth, int _charHeight, QFont _font, QColor _color=Qt::black);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
private:
    char            ch;
    QPointF         baseline;
    int             charWidth;
    int             charHeight;
    QFont           font;
    QColor          color;
    QPainterPath    path;
};

}//namespace

#endif
