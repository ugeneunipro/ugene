/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <utils/GTThread.h>

#include "GTMouseDriver.h"

namespace HI {

bool GTMouseDriver::click(Qt::MouseButton button)
{
    DRIVER_CHECK(press(button), "Button could not be pressed");
    DRIVER_CHECK(release(button), "Button could not be released");
    GTGlobals::sleep(500);
    return true;
}

namespace {

bool isFarEnoughToStartDnd(const QPoint &start, const QPoint &end) {
    return (end - start).manhattanLength() > 2 * QApplication::startDragDistance();
}

}

bool GTMouseDriver::dragAndDrop(const QPoint& start, const QPoint& end) {
    DRIVER_CHECK(moveTo(start), QString("Mouse could not be moved to point (%1, %2)").arg(start.x()).arg(start.y()));

    DRIVER_CHECK(press(), "Left button could not be pressed");

    const QPoint farPoint = (isFarEnoughToStartDnd(start, (end + start) / 2) ?
                                 (end + start) / 2 :
                                 QPoint(0, 0));
    DRIVER_CHECK(moveTo(farPoint), QString("Mouse could not be moved to point (%1, %2)").arg(farPoint.x()).arg(farPoint.y()));

    DRIVER_CHECK(moveTo(end), QString("Mouse could not be moved to point (%1, %2)").arg(end.x()).arg(end.y()));;
    DRIVER_CHECK(release(), "Button could not be released");

    GTThread::waitForMainThread();
    return true;
}

bool GTMouseDriver::selectArea(const QPoint &start, const QPoint &end){
    DRIVER_CHECK(dragAndDrop(start, end), "Drag and drop failed");
    return true;
}

#ifndef Q_OS_MAC
bool GTMouseDriver::doubleClick()
{
    DRIVER_CHECK(press(Qt::LeftButton), "Left button could not be pressed on first click");
    DRIVER_CHECK(release(Qt::LeftButton), "Left button could not be released on first click");;
    GTGlobals::sleep(100);

    DRIVER_CHECK(press(Qt::LeftButton), "Left button could not be pressed on second click");
    DRIVER_CHECK(release(Qt::LeftButton), "Left button could not be released on second click");
    GTGlobals::sleep(250);
    return true;
}
#endif

QPoint GTMouseDriver::getMousePosition(){
    return QCursor::pos();
}

} //namespace
