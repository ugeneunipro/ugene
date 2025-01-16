/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QtGlobal>

#include "GTMouseDriver.h"

#ifdef Q_OS_DARWIN
#    include <ApplicationServices/ApplicationServices.h>

#    define GT_CLASS_NAME "GTMouseDriverMac"
namespace HI {

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

static bool selectAreaMac(int x, int y) {
    QPoint currentPos = QCursor::pos();
    qDebug("selectAreaMac %d %d -> %d %d", currentPos.x(), currentPos.y(), x, y);
    DRIVER_CHECK(isPointInsideScreen(x, y), QString("selectAreaMac: Invalid coordinates %1 %2").arg(x).arg(y));

    CGEventRef event = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDragged, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != nullptr, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(100);

    return true;
}

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
    qDebug("GTMouseDriver::press %s button at %d,%d", button == Qt::LeftButton ? "left" : (button == Qt::RightButton ? "right" : "other"), mousePos.x(), mousePos.y());
    CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, pt, btn);
    DRIVER_CHECK(event != nullptr, "Can't create event");
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(10);
    CFRelease(event);

    return true;
}

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
    qDebug("GTMouseDriver::release %s button at %d,%d", button == Qt::LeftButton ? "left" : (button == Qt::RightButton ? "right" : "other"), mousePos.x(), mousePos.y());
    CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, pt, btn);
    DRIVER_CHECK(event != nullptr, "Can't create event");
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(10);
    CFRelease(event);

    return true;
}

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

void GTMouseDriver::releasePressedButtons() {
    //    The code below was tested and had shown no benefits in July, 2023.
    //    release(Qt::LeftButton);
    //    release(Qt::RightButton);
    //    release(Qt::MiddleButton);
}

#    undef GT_CLASS_NAME

}  // namespace HI

#endif  // Q_OS_DARWIN
