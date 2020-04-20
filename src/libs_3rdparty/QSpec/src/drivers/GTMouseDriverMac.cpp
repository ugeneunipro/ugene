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

#include "GTMouseDriver.h"

#include <QCursor>

#ifdef  Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace HI {

#ifdef  Q_OS_MAC
#define GT_CLASS_NAME "GTMouseDriverMac"
QPoint GTMouseDriver::mousePos = QPoint(-1, -1);
Qt::MouseButtons GTMouseDriver::bp = Qt::NoButton;

namespace {

bool isPointInsideScreen(const QPoint &point) {
    const CGDirectDisplayID displayID = CGMainDisplayID();
    const size_t horres = CGDisplayPixelsWide(displayID);
    const size_t vertres = CGDisplayPixelsHigh(displayID);

    const QRect screen(0, 0, horres, vertres);
    return screen.contains(point);
}

bool isPointInsideScreen(int x, int y) {
    return isPointInsideScreen(QPoint(x, y));
}

#define GT_METHOD_NAME "selectAreaMac"
bool selectAreaMac(const int x, const int y) {
    DRIVER_CHECK(isPointInsideScreen(x, y), "Invalid coordinates");

    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDragged, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != NULL, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(100);

    return true;
}
#undef GT_METHOD_NAME

}

#define GT_METHOD_NAME "moveToP"
bool GTMouseDriver::moveTo(const QPoint& p)
{
    int x = p.x();
    int y = p.y();
    if (bp.testFlag(Qt::LeftButton)) {
        return selectAreaMac(x, y);
    }

    DRIVER_CHECK(isPointInsideScreen(x, y), "Invalid coordinates");

    CGEventRef event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, CGPointMake(x, y), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != NULL, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(100);

    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "press"
bool GTMouseDriver::press(Qt::MouseButton button)
{
    bp |= button;
    QPoint mousePos = QCursor::pos();
    CGEventType eventType = button == Qt::LeftButton ? kCGEventLeftMouseDown :
                                button == Qt::RightButton ? kCGEventRightMouseDown:
                                button == Qt::MidButton ? kCGEventOtherMouseDown : kCGEventNull;
    CGEventRef event = CGEventCreateMouseEvent(NULL, eventType, CGPointMake(mousePos.x(), mousePos.y()), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != NULL, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(0); // don't touch, it's Mac's magic
    CFRelease(event);

    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "release"
bool GTMouseDriver::release(Qt::MouseButton button)
{
    bp &= (Qt::MouseButtonMask^button);
    QPoint mousePos = QCursor::pos();
    CGEventType eventType = button == Qt::LeftButton ? kCGEventLeftMouseUp :
                                button == Qt::RightButton ? kCGEventRightMouseUp:
                                button == Qt::MidButton ? kCGEventOtherMouseUp : kCGEventNull;
    CGEventRef event = CGEventCreateMouseEvent(NULL, eventType, CGPointMake(mousePos.x(), mousePos.y()), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(event != NULL, "Can't create event");

    CGEventPost(kCGSessionEventTap, event);
    GTGlobals::sleep(0); // don't touch, it's Mac's magic
    CFRelease(event);

    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClick"
bool GTMouseDriver::doubleClick() {

    QPoint mousePos = QCursor::pos();
    CGEventType eventTypeMouseDown = kCGEventLeftMouseDown ;
    CGEventRef eventPress = CGEventCreateMouseEvent(NULL, eventTypeMouseDown, CGPointMake(mousePos.x(), mousePos.y()), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(eventPress != NULL, "Can't create event");

    CGEventType eventTypeMouseUp = kCGEventLeftMouseUp ;
    CGEventRef eventRelease = CGEventCreateMouseEvent(NULL, eventTypeMouseUp, CGPointMake(mousePos.x(), mousePos.y()), kCGMouseButtonLeft /*ignored*/);
    DRIVER_CHECK(eventRelease != NULL, "Can't create event");

    CGEventPost(kCGSessionEventTap, eventPress);
    GTGlobals::sleep(0); // don't touch, it's Mac's magic
    CGEventPost(kCGSessionEventTap, eventRelease);
    GTGlobals::sleep(0);

    CGEventSetDoubleValueField(eventPress, kCGMouseEventClickState, 2);
    CGEventSetDoubleValueField(eventRelease, kCGMouseEventClickState, 2);

    CGEventPost(kCGSessionEventTap, eventPress);
    GTGlobals::sleep(0); // don't touch, it's Mac's magic
    CGEventPost(kCGSessionEventTap, eventRelease);
    GTGlobals::sleep(0);

    GTGlobals::sleep(100);

    CFRelease(eventPress);
    CFRelease(eventRelease);

    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scroll"
bool GTMouseDriver::scroll(int value)
{
    CGEventRef event = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitPixel, 1, value > 0 ? 10 : -10);
    DRIVER_CHECK(event != NULL, "Can't create event");
    //  Scrolling movement is generally represented by small signed integer values, typically in a range from -10 to +10.
    //  Large values may have unexpected results, depending on the application that processes the event.
    value = value > 0 ? value : -value;
    for (int i = 0; i < value; i += 10) {
        CGEventPost(kCGSessionEventTap, event);
        GTGlobals::sleep(0); // don't touch, it's Mac's magic
    }

    CFRelease(event);

    return true;
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#endif // Q_OS_MAC
} //namespace

