/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QGraphicsItem>
#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/Msa.h>
#include <U2Core/MsaInfo.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

class MsaEditor;

enum SequenceType { Auto,
                    NA,
                    AA };
/************************************************************************/
/* Settings                                                             */
/************************************************************************/
class U2VIEW_EXPORT AlignmentLogoSettings {
public:
    AlignmentLogoSettings(const Msa& _ma);

    Msa ma;
    SequenceType sequenceType;
    int startPos;
    int len;
    QColor colorScheme[256];

public:
    void updateColors();
};

/************************************************************************/
/* LogoRenderArea                                                       */
/************************************************************************/
class U2VIEW_EXPORT AlignmentLogoRenderArea : public QWidget {
    Q_OBJECT
public:
    AlignmentLogoRenderArea(const AlignmentLogoSettings& s, QWidget* p);
    void replaceSettings(const AlignmentLogoSettings& s);

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

    void evaluateHeights();
    void sortCharsByHeight();
    qreal getH(int pos);

private slots:
    void sl_colorThemeSwitched();

private:
    AlignmentLogoSettings settings;
    qreal s;  // 4||20
    qreal error;
    QVector<char>* acceptableChars;
    QVector<char> bases;
    QVector<char> aminoacids;

    QVector<QVector<char>> columns;
    // frequency and height of a particular char at position
    QVector<qreal> frequencies[256];
    QVector<qreal> heights[256];

    int bitWidth;
    int bitHeight;
};

/************************************************************************/
/* LogoItem                                                             */
/************************************************************************/
class AlignmentLogoItem : public QGraphicsItem {
public:
    AlignmentLogoItem(char _ch, QPointF _baseline, int _charWidth, int _charHeight, QFont _font, QColor _color = Qt::black);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /* = 0 */) override;

private:
    char ch;
    QPointF baseline;
    int charWidth;
    int charHeight;
    QFont font;
    QColor color;
    QPainterPath path;
};

}  // namespace U2
