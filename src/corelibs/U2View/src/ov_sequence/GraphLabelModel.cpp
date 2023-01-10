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
#include "GraphLabelModel.h"

#include <QBitmap>
#include <QPainterPath>

namespace U2 {

GraphLabelTextBox::GraphLabelTextBox(QWidget* parent)
    : QLabel(parent) {
}

void GraphLabelTextBox::paintEvent(QPaintEvent* e) {
    QPainter paint;
    paint.begin(this);
    paint.setBrush(QBrush(QColor(255, 255, 255, 200)));
    paint.setPen(Qt::NoPen);
    paint.drawRect(0, 0, width(), height());
    paint.end();
    QLabel::paintEvent(e);
}

GraphLabelDot::GraphLabelDot(QWidget* parent, const QColor& _borderColor, const QColor& _fillColor)
    : QWidget(parent), borderColor(_borderColor), fillColor(_fillColor), markedFillColor(_borderColor) {
    this->setGeometry(QRect(0, 0, 0, 0));
}

void GraphLabelDot::paintEvent(QPaintEvent*) {
    QPainter paint;
    paint.begin(this);
    paint.setPen(QPen(borderColor));
    QRect geometryRect = this->geometry();
    if (!isMarked) {
        paint.setBrush(QBrush(fillColor));
        paint.drawEllipse(QRect(2, 2, geometryRect.width() - 4, geometryRect.height() - 4));
    } else {
        paint.setBrush(QBrush(markedFillColor));
        paint.drawEllipse(QRect(2, 2, geometryRect.width() - 4, geometryRect.height() - 4));
    }
    paint.end();
}

void GraphLabelDot::mark() {
    isMarked = true;
}

void GraphLabelDot::unmark() {
    isMarked = false;
}

GraphLabel::GraphLabel(float pos, QWidget* parent, int _radius)
    : textBox(new GraphLabelTextBox(parent)), dotImage(new GraphLabelDot(parent)), position(pos), value(0.0), coord(0, 0), radius(_radius) {
    textBox->setLineWidth(3);
    textBox->setAlignment(Qt::AlignCenter);
    textBox->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
}

GraphLabel::~GraphLabel() {
    if (!textBox.isNull()) {
        delete textBox;
    }
    if (!dotImage.isNull()) {
        delete dotImage;
    }
}

void GraphLabel::setCoord(const QPoint& newCoord) {
    coord = newCoord;
    dotImage->setGeometry(QRect(coord.x() - radius - 1, coord.y() - radius - 1, radius * 2 + 2, radius * 2 + 2));
}

void GraphLabel::setTextRect(const QRect& textBoxRect) {
    textBox->setGeometry(textBoxRect);
}

GraphLabelTextBox* GraphLabel::getTextBox() const {
    return textBox;
}

void GraphLabel::setVisible(bool flag) {
    dotImage->setVisible(flag);
    textBox->setVisible(flag);
}

bool GraphLabel::isHidden() const {
    return textBox->isHidden();
}

void GraphLabel::raise() {
    textBox->raise();
}

void GraphLabel::mark() {
    dotImage->mark();
}

void GraphLabel::unmark() {
    dotImage->unmark();
}

void GraphLabel::setColor(const QColor& color, const QColor& markingColor) {
    textBox->setStyleSheet(tr("QLabel {color : %1; }").arg(color.name()));
    dotImage->setFillColor(color);
    QColor invertedColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
    dotImage->setBorderColor(invertedColor);
    dotImage->setMarkedFillColor(markingColor);
}

const QColor& GraphLabel::getFillColor() const {
    return dotImage->getFillColor();
}

const QRect& GraphLabel::getTextBoxRect() const {
    return textBox->geometry();
}

void GraphLabel::setText(const QString& labelText) {
    textBox->setText(labelText);
}

GraphLabelSet::GraphLabelSet(QWidget* parent)
    : movingLabel(new GraphLabel(-1, parent)) {
    movingLabel->setTextRect(QRect(0, 0, 0, 0));
    movingLabel->setColor(Qt::black, Qt::red);
}

GraphLabelSet::~GraphLabelSet() {
    deleteAllLabels();
    if (!movingLabel.isNull()) {
        delete movingLabel;
    }
}

void GraphLabelSet::deleteAllLabels() {
    QList<GraphLabel*> copyOfLabels = labels;
    for (GraphLabel* label : qAsConst(copyOfLabels)) {
        removeLabel(label);
    }
}

void GraphLabelSet::getLabelPositions(QList<QVariant>& labelPositions) {
    for (GraphLabel* label : qAsConst(labels)) {
        labelPositions.append(label->getPosition());
    }
}

void GraphLabelSet::addLabel(GraphLabel* pLabel) {
    labels.append(pLabel);
}

void GraphLabelSet::removeLabel(GraphLabel* pLabel) {
    labels.removeAll(pLabel);
    delete pLabel;
}

GraphLabel* GraphLabelSet::findLabelByPosition(float sequencePos, float distance) const {
    for (GraphLabel* label : qAsConst(labels)) {
        float labelPos = label->getPosition();
        if ((labelPos >= sequencePos - distance && labelPos <= sequencePos + distance) || qFuzzyCompare(labelPos, sequencePos)) {
            return label;
        }
    }
    return nullptr;
}

GraphLabel* GraphLabelSet::getMovingLabel() const {
    return movingLabel;
}

}  // namespace U2
