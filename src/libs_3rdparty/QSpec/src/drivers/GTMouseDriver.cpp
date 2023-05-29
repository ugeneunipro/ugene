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

#include <utils/GTThread.h>

#include <QApplication>
#include <QScreen>

#include "GTMouseDriver.h"

namespace HI {

bool GTMouseDriver::click(Qt::MouseButton button) {
    DRIVER_CHECK(press(button), "Button could not be pressed");
    DRIVER_CHECK(release(button), "Button could not be released");
    GTThread::waitForMainThread();
    return true;
}

bool GTMouseDriver::click(const QPoint& p, Qt::MouseButton button) {
    DRIVER_CHECK(moveTo(p), "Mouse move was failed");
    return click(button);
}

bool GTMouseDriver::dragAndDrop(const QPoint& start, const QPoint& end) {
    GTGlobals::sleep(QApplication::doubleClickInterval() + 1);  // Protect from double-clicks.
    DRIVER_CHECK(moveTo(start), QString("Mouse was not moved to the start point (%1, %2)").arg(start.x()).arg(start.y()));
    DRIVER_CHECK(press(), "Mouse button was not be pressed");
    GTThread::waitForMainThread();

    QPoint middlePoint = (end + start) / 2;
    int startDragDistance = QApplication::startDragDistance();
    bool isMiddlePointValid = (middlePoint - start).manhattanLength() > 2 * startDragDistance;
    if (!isMiddlePointValid) {
        middlePoint = start + QPoint(3 * startDragDistance, 3 * startDragDistance);
        QRect screenRect = QGuiApplication::primaryScreen()->geometry();
        if (middlePoint.x() > screenRect.right()) {
            middlePoint.setX(start.x() - 3 * startDragDistance);
        }
        if (middlePoint.y() > screenRect.bottom()) {
            middlePoint.setY(start.y() - 3 * startDragDistance);
        }
    }
    DRIVER_CHECK(moveTo(middlePoint), QString("Mouse could not be moved to point (%1, %2)").arg(middlePoint.x()).arg(middlePoint.y()));

    DRIVER_CHECK(moveTo(end), QString("Mouse was not moved to the end point (%1, %2)").arg(end.x()).arg(end.y()));
    GTThread::waitForMainThread();

#ifndef Q_OS_WIN
    GTGlobals::sleep(500);  // Do extra wait before the release. Otherwise, the method is not stable on Linux.
#endif

    DRIVER_CHECK(release(), "Mouse button was not released");
    GTThread::waitForMainThread();
    return true;
}

bool GTMouseDriver::selectArea(const QPoint& start, const QPoint& end) {
    DRIVER_CHECK(dragAndDrop(start, end), "Drag and drop failed");
    return true;
}

#ifndef Q_OS_DARWIN
bool GTMouseDriver::doubleClick() {
    DRIVER_CHECK(press(Qt::LeftButton), "Left button could not be pressed on first click");
    DRIVER_CHECK(release(Qt::LeftButton), "Left button could not be released on first click");
    // Use an interval below "doubleClickInterval" between clicks because we want it to be a "double-click".
    GTGlobals::sleep(QApplication::doubleClickInterval() / 2);

    DRIVER_CHECK(press(Qt::LeftButton), "Left button could not be pressed on second click");
    DRIVER_CHECK(release(Qt::LeftButton), "Left button could not be released on second click");
    GTThread::waitForMainThread();
    return true;
}
#endif

QPoint GTMouseDriver::getMousePosition() {
    return QCursor::pos();
}

}  // namespace HI
