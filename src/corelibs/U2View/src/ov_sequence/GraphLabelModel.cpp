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
#include "GraphLabelModel.h"

#include <QBitmap>
#include <QPainterPath>

namespace U2 {

TextLabel::TextLabel(QWidget *parent)
    : QLabel(parent) {
}

void TextLabel::paintEvent(QPaintEvent *e) {
    QPainter paint;
    paint.begin(this);
    paint.setBrush(QBrush(QColor(255, 255, 255, 200)));
    paint.setPen(Qt::NoPen);
    paint.drawRect(0, 0, width(), height());
    paint.end();
    QLabel::paintEvent(e);
}

RoundHint::RoundHint(QWidget *parent, QColor _borderColor, QColor _fillingColor)
    : QWidget(parent), borderColor(_borderColor), fillingColor(_fillingColor), markedFillingColor(_borderColor), isMarked(false) {
    this->setGeometry(QRect(0, 0, 0, 0));
}

void RoundHint::paintEvent(QPaintEvent *) {
    QPainter paint;
    paint.begin(this);
    paint.setPen(QPen(borderColor));
    if (!isMarked) {
        paint.setBrush(QBrush(fillingColor));
        paint.drawEllipse(QRect(2, 2, this->geometry().width() - 4, this->geometry().height() - 4));
    } else {
        paint.setBrush(QBrush(markedFillingColor));
        paint.drawEllipse(QRect(2, 2, this->geometry().width() - 4, this->geometry().height() - 4));
    }
    paint.end();
}

void RoundHint::mark() {
    isMarked = true;
}
void RoundHint::unmark() {
    isMarked = false;
}

GraphLabel::GraphLabel()
    : text(new TextLabel(nullptr)), image(new RoundHint()), position(-1), value(0.0), coord(-1, -1), radius(defaultRadius) {
    text->setLineWidth(3);
    text->setAlignment(Qt::AlignCenter);
    text->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
}

GraphLabel::GraphLabel(float pos, QWidget *parent, int _radius)
    : text(new TextLabel(parent)), image(new RoundHint(parent)), position(pos), value(0.0), coord(0, 0), radius(_radius) {
    text->setLineWidth(3);
    text->setAlignment(Qt::AlignCenter);
    text->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
}

GraphLabel::~GraphLabel() {
    if (!text.isNull()) {
        delete text;
    }
    if (!image.isNull()) {
        delete image;
    }
}

void GraphLabel::setCoord(const QPoint &_coord) {
    coord = _coord;
    image->setGeometry(QRect(coord.x() - radius - 1, coord.y() - radius - 1, radius * 2 + 2, radius * 2 + 2));
}

void GraphLabel::setHintRect(const QRect &_hintRect) {
    text->setGeometry(_hintRect);
}

void GraphLabel::setVisible(bool flag) {
    image->setVisible(flag);
    text->setVisible(flag);
}

bool GraphLabel::isHidden() const {
    return text->isHidden();
}

void GraphLabel::raise() {
    text->raise();
}

void GraphLabel::mark() {
    image->mark();
}

void GraphLabel::unmark() {
    image->unmark();
}

void GraphLabel::setColor(QColor color, QColor markingColor) {
    text->setStyleSheet(tr("QLabel {color : %1; }").arg(color.name()));
    image->setFillingColor(color);
    QColor invertingColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
    image->setBorderColor(invertingColor);
    image->setMarkingColor(markingColor);
}

void GraphLabel::setParent(QWidget *parent) {
    text->setParent(parent);
    image->setParent(parent);
}

QColor GraphLabel::getFillColor() {
    return image->getFillingColor();
}

QRect GraphLabel::getHintRect() {
    return text->geometry();
}

QString GraphLabel::getHintText() const {
    return text->text();
}

void GraphLabel::setHintText(const QString &_hintText) {
    text->setText(_hintText);
}

TextLabel &GraphLabel::getTextLabel() {
    return *text;
}

MultiLabel::MultiLabel()
    : movingLabel(new GraphLabel()) {
    movingLabel->setHintRect(QRect(0, 0, 0, 0));
    movingLabel->setColor(Qt::black, Qt::red);
}
MultiLabel::~MultiLabel() {
    deleteAllLabels();
    if (!movingLabel.isNull()) {
        delete movingLabel;
    }
}
void MultiLabel::deleteAllLabels() {
    foreach (GraphLabel *currentLabel, labels) {
        removeLabel(currentLabel);
    }
}
void MultiLabel::getLabelPositions(QList<QVariant> &labelPositions) {
    foreach (GraphLabel *currentLabel, labels)
        labelPositions.append(currentLabel->getPosition());
}

void MultiLabel::addLabel(GraphLabel *pLabel) {
    labels.append(pLabel);
}

void MultiLabel::removeLabel(GraphLabel *pLabel) {
    labels.removeAll(pLabel);
    delete pLabel;
}

bool MultiLabel::removeLabel(float xPos) {
    GraphLabel *label = findLabelByPosition(xPos);
    CHECK(nullptr != label, false)
    removeLabel(label);
    return true;
}

GraphLabel *MultiLabel::at(int i) const {
    return labels.at(i);
}

GraphLabel *MultiLabel::findLabelByPosition(float sequencePos, float deviation) const {
    for (GraphLabel *label : qAsConst(labels)) {
        float labelPos = label->getPosition();
        if ((labelPos >= sequencePos - deviation && labelPos <= sequencePos + deviation) || qFuzzyCompare(labelPos, sequencePos)) {
            return label;
        }
    }
    return nullptr;
}

GraphLabel *MultiLabel::getMovingLabel() const {
    return movingLabel;
}

}  // namespace U2
