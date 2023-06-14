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
QPoint GTMouseDriver::mousePos = QPoint(-1, -1);
Qt::MouseButtons GTMouseDriver::bp = Qt::NoButton;

static bool isPointInsideScreen(const QPoint& point) {
    CGDirectDisplayID displayID = CGMainDisplayID();
    int horres = CGDisplayPixelsWide(displayID);
    int vertres = CGDisplayPixelsHigh(displayID);
    QRect screen(1, 1, horres - 2, vertres - 2);  // Exclude border pixels like (0,0).
    return screen.contains(point);
}

static bool isPointInsideScreen(int x, int y) {
    return isPointInsideScreen(QPoint(x, y));
}

#    define GT_METHOD_NAME "selectAreaMac"
static bool selectAreaMac(int x, int y) {
    qDebug("selectAreaMac %d %d", x, y);
    DRIVER_CHECK(isPointInsideScreen(x, y), QString("selectAreaMac: Invalid coordinates %1 %2").arg(x).arg(y));

    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDragged, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != NULL, "Can't create event");

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

    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != NULL, "Can't create event");

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
    CGEventRef event = CGEventCreateMouseEvent(NULL, eventType, pt, btn);
    DRIVER_CHECK(event != NULL, "Can't create event");
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
    CGEventRef event = CGEventCreateMouseEvent(NULL, eventType, pt, btn);
    DRIVER_CHECK(event != NULL, "Can't create event");
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 1);

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(0);  // don't touch, it's Mac's magic
    CFRelease(event);

    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "doubleClick"
bool GTMouseDriver::doubleClick() {
    QPoint mousePos = QCursor::pos();
    CGEventType eventTypeMouseDown = kCGEventLeftMouseDown;
    CGEventRef eventPress = CGEventCreateMouseEvent(NULL, eventTypeMouseDown, CGPointMake(mousePos.x(), mousePos.y()), kCGMouseButtonLeft);
    DRIVER_CHECK(eventPress != NULL, "Can't create event");

    CGEventType eventTypeMouseUp = kCGEventLeftMouseUp;
    CGEventRef eventRelease = CGEventCreateMouseEvent(NULL, eventTypeMouseUp, CGPointMake(mousePos.x(), mousePos.y()), kCGMouseButtonLeft);
    DRIVER_CHECK(eventRelease != NULL, "Can't create event");

    CGEventPost(kCGSessionEventTap, eventPress);
    GTGlobals::sleep(0);  // don't touch, it's Mac's magic
    CGEventPost(kCGSessionEventTap, eventRelease);
    GTGlobals::sleep(0);

    CGEventSetDoubleValueField(eventPress, kCGMouseEventClickState, 2);
    CGEventSetDoubleValueField(eventRelease, kCGMouseEventClickState, 2);

    CGEventPost(kCGSessionEventTap, eventPress);
    GTGlobals::sleep(0);  // don't touch, it's Mac's magic
    CGEventPost(kCGSessionEventTap, eventRelease);
    GTGlobals::sleep(0);

    GTGlobals::sleep(100);

    CFRelease(eventPress);
    CFRelease(eventRelease);

    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "scroll"
bool GTMouseDriver::scroll(int value) {
    CGEventRef event = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 1, value > 0 ? 10 : -10);
    DRIVER_CHECK(event != NULL, "Can't create event");
    //  Scrolling movement is generally represented by small signed integer values, typically in a range from -10 to +10.
    //  Large values may have unexpected results, depending on the application that processes the event.
    value = value > 0 ? value : -value;
    for (int i = 0; i < value; i += 10) {
        CGEventPost(kCGSessionEventTap, event);
        GTGlobals::sleep(0);  // don't touch, it's Mac's magic
    }

    CFRelease(event);
    GTGlobals::sleep(100);
    return true;
}
#    undef GT_METHOD_NAME
#    undef GT_CLASS_NAME

#endif  // Q_OS_DARWIN
}  // namespace HI
