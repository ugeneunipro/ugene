/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

class U2VIEW_EXPORT GraphLabelTextBox : public QLabel {
    Q_OBJECT
public:
    GraphLabelTextBox(QWidget* parent);

private:
    void paintEvent(QPaintEvent* e);
};

class GraphLabelDot : public QWidget {
public:
    GraphLabelDot(QWidget* parent, const QColor& borderColor = Qt::white, const QColor& fillColor = Qt::black);

    void setBorderColor(const QColor& color) {
        borderColor = color;
    }

    const QColor& getBorderColor() const {
        return borderColor;
    }

    void setFillColor(const QColor& color) {
        fillColor = color;
    }

    const QColor& getFillColor() const {
        return fillColor;
    }

    void setMarkedFillColor(const QColor& color) {
        markedFillColor = color;
    }

    const QColor& getMarkedFillColor() const {
        return markedFillColor;
    }

    void mark();

    void unmark();

private:
    void paintEvent(QPaintEvent* e) override;

    QColor borderColor;
    QColor fillColor;
    QColor markedFillColor;
    bool isMarked = false;
};

class GraphLabel : public QObject {
    Q_OBJECT
public:
    GraphLabel(float pos, QWidget* parent, int dotRadius = 4);
    ~GraphLabel();

    bool isHidden() const;

    void setCoord(const QPoint& _coord);

    const QPoint& getCoord() const {
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

    void setText(const QString& labelText);

    void setTextRect(const QRect& textRect);

    GraphLabelTextBox* getTextBox() const;

    const QRect& getTextBoxRect() const;

    int getDotRadius() const {
        return radius;
    }

    /** Calls setVisible on label subcomponents. */
    void setVisible(bool flag);

    void raise();

    void mark();

    void unmark();

    void setColor(const QColor& color, const QColor& markingColor);

    const QColor& getFillColor() const;

private:
    QPointer<GraphLabelTextBox> textBox;
    QPointer<GraphLabelDot> dotImage;

    /** Position of the label in sequence coordinates. */
    float position;

    float value;
    QPoint coord;
    int radius;
};

/** Set of all labels per graph. Every graph has at least 1 moving cursor label + optional set of other labels. */
class GraphLabelSet : public QObject {
    Q_OBJECT
public:
    GraphLabelSet(QWidget* parent);
    ~GraphLabelSet();

    void addLabel(GraphLabel* pLabel);

    void removeLabel(GraphLabel* pLabel);

    void getLabelPositions(QList<QVariant>& labelPositions);

    void deleteAllLabels();

    GraphLabel* findLabelByPosition(float sequencePos, float distance = 0) const;

    const QList<GraphLabel*>& getLabels() const {
        return labels;
    }

    GraphLabel* getMovingLabel() const;

private:
    Q_DISABLE_COPY(GraphLabelSet)
    QList<GraphLabel*> labels;
    QPointer<GraphLabel> movingLabel;
};

}  // namespace U2
#endif
