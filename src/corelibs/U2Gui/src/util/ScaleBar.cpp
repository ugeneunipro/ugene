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

#include <QAction>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>

#include "ScaleBar.h"

namespace U2 {

ScaleBar::ScaleBar(Qt::Orientation ori, QWidget* parent)
    : QWidget(parent)
{
    scaleBar = new QSlider(ori);
    scaleBar->setTracking(true);
    scaleBar->setRange(100, 2000);
    scaleBar->setTickPosition(QSlider::TicksLeft);
    scaleBar->setTickInterval(100);
    connect(scaleBar, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)));
    connect(scaleBar, SIGNAL(valueChanged(int)), SLOT(sl_updateState()));

    minusAction = new QAction(QIcon(":core/images/minus.png"), tr("Decrease peaks height"), this);
    connect(minusAction, SIGNAL(triggered()), SLOT(sl_minusButtonClicked()));

    minusButton = new QToolButton();
    minusButton->setText(QString(tr("Decrease peaks height")));
    minusButton->setIcon(QIcon(":core/images/minus.png"));
    minusButton->setFixedSize(20, 20);
    minusButton->setAutoRepeat(true);
    minusButton->setAutoRepeatInterval(20);
    connect(minusButton, SIGNAL(clicked()), minusAction, SLOT(trigger()));

    plusAction = new QAction(QIcon(":core/images/plus.png"), tr("Increase peaks height"), this);
    connect(plusAction, SIGNAL(triggered()), SLOT(sl_plusButtonClicked()));

    plusButton = new QToolButton(this);
    plusButton->setText(QString(tr("Increase peaks height")));
    plusButton->setIcon(QIcon(":core/images/plus.png"));
    plusButton->setAutoRepeat(true);
    plusButton->setAutoRepeatInterval(20);
    plusButton->setFixedSize(20, 20);
    connect(plusButton, SIGNAL(clicked()), plusAction, SLOT(trigger()));

    //layout
    QBoxLayout *zoomLayout = new QBoxLayout(ori == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::RightToLeft);
    zoomLayout->addWidget(plusButton);
    zoomLayout->addWidget(scaleBar);
    zoomLayout->addWidget(minusButton);
    zoomLayout->setMargin(0);
    zoomLayout->setSpacing(0);
    setLayout(zoomLayout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    sl_updateState();
}

int ScaleBar::value() const {
    return scaleBar->value();
}

void ScaleBar::setValue(int value) {
    scaleBar->setValue(value);
}

void ScaleBar::setRange(int minumum, int maximum) {
    scaleBar->setRange(minumum, maximum);
    sl_updateState();
}

void ScaleBar::setTickInterval(int interval) {
    scaleBar->setTickInterval(interval);
}

QAction *ScaleBar::getPlusAction() const {
    return plusAction;
}

QAction *ScaleBar::getMinusAction() const {
    return minusAction;
}

QAbstractButton *ScaleBar::getPlusButton() const {
    return plusButton;
}

QAbstractButton *ScaleBar::getMinusButton() const {
    return minusButton;
}

void ScaleBar::sl_minusButtonClicked() {
    scaleBar->setValue(scaleBar->value()-scaleBar->pageStep());
}

void ScaleBar::sl_plusButtonClicked() {
    scaleBar->setValue(scaleBar->value()+scaleBar->pageStep());
}

void ScaleBar::sl_updateState() {
    minusAction->setEnabled(scaleBar->value() != scaleBar->minimum());
    minusButton->setEnabled(minusAction->isEnabled());
    plusAction->setEnabled(scaleBar->value() != scaleBar->maximum());
    plusButton->setEnabled(plusAction->isEnabled());
}

}   // namespace U2
