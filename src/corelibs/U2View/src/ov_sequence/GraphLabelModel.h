/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QVariant>

#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

namespace U2 {

const int defaultRadius = 4;

class U2VIEW_EXPORT TextLabel : public QLabel {
    Q_OBJECT
public:
    TextLabel(QWidget *parent = nullptr);

private:
    void paintEvent(QPaintEvent *e);
};

class RoundHint : public QWidget {
public:
    RoundHint(QWidget *parent = nullptr, QColor _borderColor = Qt::white, QColor _fillingColor = Qt::black);

    void setBorderColor(QColor color) {
        borderColor = color;
    }
    QColor getBorderColor() {
        return borderColor;
    }

    void setFillingColor(QColor color) {
        fillingColor = color;
    }
    QColor getFillingColor() {
        return fillingColor;
    }

    void setMarkingColor(QColor color) {
        markedFillingColor = color;
    }
    QColor getMarkingingColor() {
        return markedFillingColor;
    }

    void mark();
    void unmark();

private:
    void paintEvent(QPaintEvent *e);
    QColor borderColor;
    QColor fillingColor;
    QColor markedFillingColor;
    bool isMarked;
};

class GraphLabel : public QObject {
    Q_OBJECT
public:
    GraphLabel();
    GraphLabel(float pos, QWidget *parent = nullptr, int _radius = defaultRadius);
    ~GraphLabel();

    bool isHidden() const;

    void setCoord(const QPoint &_coord);
    QPoint getCoord() const {
        return coord;
    }

    void setPosition(float pos) {
        position = pos;
    }
    float getPosition() const {
        return position;
    }

    void setValue(float val) {
        value = val;
    }
    float getValue() const {
        return value;
    }

    void setHintText(const QString &_hintText);
    QString getHintText() const;

    TextLabel &getTextLabel();

    void setHintRect(const QRect &_hintRect);
    QRect getHintRect();

    void setParent(QWidget *parent);

    int getDotRadius() const {
        return radius;
    }

    /** Calls setVisible on label subcomponents. */
    void setVisible(bool flag);

    void raise();

    void mark();
    void unmark();

    void setColor(QColor color, QColor markingColor);
    QColor getFillColor();

private:
    QPointer<TextLabel> text;
    QPointer<RoundHint> image;

    /** Position of the label in sequence coordinates. */
    float position;

    float value;
    QPoint coord;
    int radius;
};

class MultiLabel : public QObject {
    Q_OBJECT
public:
    MultiLabel();
    ~MultiLabel();

    void addLabel(GraphLabel *pLabel);
    void removeLabel(GraphLabel *pLabel);
    bool removeLabel(float xPos);

    void getLabelPositions(QList<QVariant> &labelPositions);

    void deleteAllLabels();

    GraphLabel *findLabelByPosition(float sequencePos, float deviation = 0) const;
    GraphLabel *at(int i) const;

    const QList<GraphLabel *> &getLabels() const {
        return labels;
    }

    GraphLabel *getMovingLabel() const;

private:
    Q_DISABLE_COPY(MultiLabel)
    QList<GraphLabel *> labels;
    QPointer<GraphLabel> movingLabel;
};

}  // namespace U2
#endif
