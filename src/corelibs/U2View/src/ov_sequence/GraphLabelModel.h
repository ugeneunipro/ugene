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

#ifndef _U2_LABEL_MODEL_H_
#define _U2_LABEL_MODEL_H_

#include <U2Core/global.h>
#include <U2Core/U2SafePoints.h>

#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QLabel>
#include <QVariant>

namespace U2 {

const int defaultRadius = 4;

class U2VIEW_EXPORT TextLabel: public QLabel
{
    Q_OBJECT
public:
    TextLabel(QWidget *parent = NULL);
    ~TextLabel();
private:
    void mouseMoveEvent(QMouseEvent *me);

    void paintEvent(QPaintEvent *e);
};

class RoundHint: public QWidget
{
public:
    RoundHint(QWidget *parent = NULL, QColor _borderColor = Qt::white, QColor _fillingColor = Qt::black);
    ~RoundHint();

    void setBorderColor(QColor color){ borderColor = color;}
    QColor getBorderColor(){ return borderColor;}

    void setFillingColor(QColor color){fillingColor = color;}
    QColor getFillingColor(){return fillingColor;}

    void setMarkingColor(QColor color) {markedFillingColor = color;}
    QColor getMarkingingColor(){return markedFillingColor;}


    void mark();
    void unmark();

private:
    void paintEvent(QPaintEvent *e);
    QColor borderColor;
    QColor fillingColor;
    QColor markedFillingColor;
    bool isMarked;
};

class GraphLabel: public QObject{
    Q_OBJECT
public:

    GraphLabel();
    GraphLabel(float pos, QWidget *parent = NULL, int _radius = defaultRadius);
    ~GraphLabel();

    bool select(float _position);
    bool isSelected() const;
    bool isHidden() const;

    void setCoord(const QPoint &_coord);
    QPoint getCoord() const {return coord;}

    void setPosition(float pos) {position = pos;}
    float getPosition() const {return position;}

    void setValue(float val) {value = val;}
    float getValue() const {return value;}

    void setHintText(const QString &_hintText);
    QString getHintText() const;

    TextLabel& getTextLabel();

    void setHintRect(const QRect &_hintRect);
    QRect getHintRect();

    void setParent(QWidget *parent);

    int getSize() const {return radius;}

    void show();
    void hide();
    void raise();

    void mark();
    void unmark();

    void setColor(QColor color, QColor markingColor);
    QColor getFillingColor();

    GraphLabel *attachedLabel;
signals:
    void si_onHintDeleted(GraphLabel *label);
private:
    bool eventFilter(QObject *target, QEvent* e);
    QPointer<TextLabel> text;
    QPointer<RoundHint> image;
    float position;
    float value;
    QPoint coord;
    int radius;
};



typedef QList<GraphLabel *> Labels;

class MultiLabel : public QObject
{
    Q_OBJECT
public:
    MultiLabel();
    ~MultiLabel();

    void addLabel(GraphLabel *pLabel);
    void removeLabel(GraphLabel *pLabel);
    bool removeLabel(float xPos);

    void getLabelPositions(QList<QVariant> &labelPositions);

    void deleteAllLabels();

    GraphLabel* findLabelByPosition(float xPos) const;
    GraphLabel* at(int i) const;

    Labels& getLabels() {return labels;}

    GraphLabel& getMovingLabel();

private:
    Q_DISABLE_COPY(MultiLabel)
    Labels labels;
    QPointer<GraphLabel> movingLabel;
private slots:
    void sl_deleteLabel(GraphLabel *label);
};

}//namespace
#endif
