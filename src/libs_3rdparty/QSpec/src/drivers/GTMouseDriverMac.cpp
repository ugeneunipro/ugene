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

#include <QCursor>

#include "GTMouseDriver.h"

#ifdef Q_OS_DARWIN
#    include <ApplicationServices/ApplicationServices.h>
#endif

namespace HI {

#ifdef Q_OS_DARWIN
#    define GT_CLASS_NAME "GTMouseDriverMac"

Qt::MouseButtons GTMouseDriver::bp = Qt::NoButton;

static bool isPointInsideScreen(const QPoint& point) {
    CGDirectDisplayID displayID = CGMainDisplayID();
    int hRes = (int)CGDisplayPixelsWide(displayID);
    int vRes = (int)CGDisplayPixelsHigh(displayID);
    QRect screen(1, 1, hRes - 2, vRes - 2);  // Exclude border pixels like (0,0).
    return screen.contains(point);
}

static bool isPointInsideScreen(int x, int y) {
    return isPointInsideScreen(QPoint(x, y));
}

#    define GT_METHOD_NAME "selectAreaMac"
static bool selectAreaMac(int x, int y) {
    qDebug("selectAreaMac %d %d", x, y);
    DRIVER_CHECK(isPointInsideScreen(x, y), QString("selectAreaMac: Invalid coordinates %1 %2").arg(x).arg(y));

    CGEventRef event = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDragged, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != nullptr, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(100);

    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "moveToP"
bool GTMouseDriver::moveTo(const QPoint& p) {
    int x = p.x();
    int y = p.y();
    if (bp.testFlag(Qt::LeftButton)) {
        return selectAreaMac(x, y);
    }
    qDebug("GTMouseDriver::moveTo %d %d", x, y);

    DRIVER_CHECK(isPointInsideScreen(x, y), QString("moveTo: Invalid coordinates %1 %2").arg(x).arg(y));

    CGEventRef event = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != nullptr, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(100);

    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "press"
bool GTMouseDriver::press(Qt::MouseButton button) {
    bp |= button;
    QPoint mousePos = QCursor::pos();
    CGEventType eventType;
    CGMouseButton btn;
    if (button == Qt::LeftButton) {
        eventType = kCGEventLeftMouseDown;
        btn = kCGMouseButtonLeft;
    } else if (button == Qt::RightButton) {
        eventType = kCGEventRightMouseDown;
        btn = kCGMouseButtonRight;
    } else if (button == Qt::MiddleButton) {
        eventType = kCGEventOtherMouseDown;
        btn = kCGMouseButtonCenter;
    } else {
        DRIVER_CHECK(false, "Unknown mouse button");
    }
    CGPoint pt = CGPointMake(mousePos.x(), mousePos.y());
    CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, pt, btn);
    DRIVER_CHECK(event != nullptr, "Can't create event");
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(0);  // don't touch, it's Mac's magic
    CFRelease(event);

    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "release"
bool GTMouseDriver::release(Qt::MouseButton button) {
    bp &= (Qt::MouseButtonMask ^ button);
    QPoint mousePos = QCursor::pos();
    CGEventType eventType;
    CGMouseButton btn;
    if (button == Qt::LeftButton) {
        eventType = kCGEventLeftMouseUp;
        btn = kCGMouseButtonLeft;
    } else if (button == Qt::RightButton) {
        eventType = kCGEventRightMouseUp;
        btn = kCGMouseButtonRight;
    } else if (button == Qt::MiddleButton) {
        eventType = kCGEventOtherMouseUp;
        btn = kCGMouseButtonCenter;
    } else {
        DRIVER_CHECK(false, "Unknown mouse button");
    }
    CGPoint pt = CGPointMake(mousePos.x(), mousePos.y());
    CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, pt, btn);
    DRIVER_CHECK(event != nullptr, "Can't create event");
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(0);  // don't touch, it's Mac's magic
    CFRelease(event);

    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "scroll"
bool GTMouseDriver::scroll(int value) {
    int wheelDeviceCount = 1;
    int wheelDevice1Change = value > 0 ? 1 : -1;
    for (int i = 0; i < qAbs(value); i++) {
        CGEventRef event = CGEventCreateScrollWheelEvent(nullptr, kCGScrollEventUnitLine, wheelDeviceCount, wheelDevice1Change);
        DRIVER_CHECK(event != nullptr, "Can't create event");
        CGEventPost(kCGSessionEventTap, event);
        GTGlobals::sleep(10);
        CFRelease(event);
    }
    GTGlobals::sleep(100);
    return true;
}
#    undef GT_METHOD_NAME
#    undef GT_CLASS_NAME

void GTMouseDriver::releasePressedButtons() {
}

#endif  // Q_OS_DARWIN
}  // namespace HI
