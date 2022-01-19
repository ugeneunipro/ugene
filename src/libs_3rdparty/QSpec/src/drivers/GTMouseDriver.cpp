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

#include <utils/GTThread.h>

#include <QApplication>

#include "GTMouseDriver.h"

namespace HI {

bool GTMouseDriver::click(Qt::MouseButton button) {
    GTGlobals::sleep(QApplication::doubleClickInterval());  // Protect from double-clicks.
    DRIVER_CHECK(press(button), "Button could not be pressed");
    DRIVER_CHECK(release(button), "Button could not be released");
    GTThread::waitForMainThread();
    return true;
}

#ifndef Q_OS_DARWIN
bool GTMouseDriver::click(const QPoint &p, Qt::MouseButton button) {
    GTGlobals::sleep(QApplication::doubleClickInterval());  // Protect from double-clicks.
    DRIVER_CHECK(moveTo(p), "Mouse move was failed");
    DRIVER_CHECK(press(button), "Button could not be pressed");
    DRIVER_CHECK(release(button), "Button could not be released");
    GTThread::waitForMainThread();
    return true;
}
#endif

namespace {

#ifdef Q_OS_WIN
bool isFarEnoughToStartDnd(const QPoint &start, const QPoint &end) {
    return (end - start).manhattanLength() > 2 * QApplication::startDragDistance();
}
#endif

}  // namespace

bool GTMouseDriver::dragAndDrop(const QPoint &start, const QPoint &end) {
    GTGlobals::sleep(QApplication::doubleClickInterval());  // Protect from double-clicks.
    DRIVER_CHECK(moveTo(start), QString("Mouse was not moved to the start point (%1, %2)").arg(start.x()).arg(start.y()));
    DRIVER_CHECK(press(), "Mouse button was not be pressed");

// After Linux version was improved the Windows has a lot of regressions. Keeping Windows version with no changes below during investigation.
#ifdef Q_OS_WIN
    QPoint farPoint = (isFarEnoughToStartDnd(start, (end + start) / 2) ? (end + start) / 2 : QPoint(0, 0));
    DRIVER_CHECK(moveTo(farPoint), QString("Mouse could not be moved to point (%1, %2)").arg(farPoint.x()).arg(farPoint.y()));
    DRIVER_CHECK(moveTo(end), QString("Mouse could not be moved to point (%1, %2)").arg(end.x()).arg(end.y()));
#else
    GTThread::waitForMainThread();
    DRIVER_CHECK(moveTo(end), QString("Mouse was not moved to the end point (%1, %2)").arg(end.x()).arg(end.y()));
    GTThread::waitForMainThread();
    GTGlobals::sleep(500);  // Do extra wait before the release. Otherwise the method is not stable on Linux.
#endif
    DRIVER_CHECK(release(), "Mouse button was not released");
    GTThread::waitForMainThread();
    return true;
}

bool GTMouseDriver::selectArea(const QPoint &start, const QPoint &end) {
    DRIVER_CHECK(dragAndDrop(start, end), "Drag and drop failed");
    return true;
}

#ifndef Q_OS_DARWIN
bool GTMouseDriver::doubleClick() {
    GTGlobals::sleep(QApplication::doubleClickInterval());  // Avoid merging into a "double-click" with previous mouse events on the first press/release.

    DRIVER_CHECK(press(Qt::LeftButton), "Left button could not be pressed on first click");
    DRIVER_CHECK(release(Qt::LeftButton), "Left button could not be released on first click");
    GTGlobals::sleep(QApplication::doubleClickInterval() / 2);  // Use an interval below "doubleClickInterval" becase we want it to be a "double-click".

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
