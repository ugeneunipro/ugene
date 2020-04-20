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

#include <QAbstractButton>
#include <QLabel>
#include <QMouseEvent>

#include <U2Core/U2SafePoints.h>

#include "LabelClickTransmitter.h"

namespace U2 {

LabelClickTransmitter::LabelClickTransmitter(QLabel *label, QAbstractButton *button)
: QObject(label), label(label), button(button)
{
    label->installEventFilter(this);
}

bool LabelClickTransmitter::eventFilter(QObject *object, QEvent *event) {
    CHECK(NULL != label, false);
    CHECK(NULL != button, false);
    CHECK(label == object, false);

    CHECK(QEvent::MouseButtonRelease == event->type(), false);
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    CHECK(NULL != event, false);
    CHECK(Qt::LeftButton == mouseEvent->button(), false);

    button->toggle();
    return false;
}

} // U2
