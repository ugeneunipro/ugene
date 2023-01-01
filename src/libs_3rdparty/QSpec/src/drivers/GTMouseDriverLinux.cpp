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

#include <QByteArray>

#include "GTMouseDriver.h"
#ifdef __linux__
#    include <X11/extensions/XTest.h>
#endif

namespace HI {

#ifdef __linux__

QPoint GTMouseDriver::mousePos = QPoint(-1, -1);

#    define DELAY_ON_EVERY_N_PX 16
/**
 * Returns delay = 1 millisecond for every DELAY_ON_EVERY_N_PX pixel or 0 otherwise.
 * Part of the GTMouseDriver::moveTo internal logic.
 */
static int getMouseMoveDelayMillis(int pos) {
    return pos % DELAY_ON_EVERY_N_PX == 0 ? 1 : 0;
}

bool GTMouseDriver::moveTo(const QPoint& p) {
    QByteArray display_name = qgetenv("DISPLAY");
    DRIVER_CHECK(!display_name.isEmpty(), "Environment variable \"DISPLAY\" not found");

    Display* display = XOpenDisplay(display_name.constData());
    DRIVER_CHECK(display != nullptr, "display is NULL");

    int horres = XDisplayWidth(display, 0);
    int vertres = XDisplayHeight(display, 0);

    QRect screen(0, 0, horres - 1, vertres - 1);
    DRIVER_CHECK(screen.contains(p), "Invalid coordinates");

    Window root, child;
    int root_x, root_y, pos_x, pos_y;
    unsigned mask;
    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &root, &child, &root_x, &root_y, &pos_x, &pos_y, &mask);

    int x = pos_x;
    int y = pos_y;

    if (x == p.x()) {
        while (y != p.y()) {
            y += (y < p.y()) ? 1 : -1;
            XTestFakeMotionEvent(display, -1, p.x(), y, getMouseMoveDelayMillis(y));
            XFlush(display);
        }
    } else if (y == p.y()) {
        while (x != p.x()) {
            x += (x < p.x()) ? 1 : -1;
            XTestFakeMotionEvent(display, -1, x, p.y(), getMouseMoveDelayMillis(x));
            XFlush(display);
        }
    } else {
        // moved by the shortest way
        // equation of the line by two points y = (-(x0 * y1 - x1 * y0) - x*(y0 - y1)) / (x1 - x0)
        int diff_x = p.x() - x;
        int diff_y = y - p.y();
        int diff_xy = -(x * p.y() - p.x() * y);
        while (x != p.x()) {
            x += (x < p.x()) ? 1 : -1;
            y = (diff_xy - x * diff_y) / diff_x;
            XTestFakeMotionEvent(display, -1, x, y, getMouseMoveDelayMillis(x));
            XFlush(display);
        }
    }

    XCloseDisplay(display);
    GTGlobals::sleep(100);
    return true;
}

bool GTMouseDriver::press(Qt::MouseButton button) {
    QByteArray display_name = qgetenv("DISPLAY");
    DRIVER_CHECK(!display_name.isEmpty(), "Environment variable \"DISPLAY\" not found");

    Display* display = XOpenDisplay(display_name.constData());
    DRIVER_CHECK(display != nullptr, "display is NULL");

    // 1 = Left, 2 = Middle, 3 = Right
    unsigned int btn = button == Qt::LeftButton ? 1 : button == Qt::RightButton ? 3
                                                  : button == Qt::MidButton     ? 2
                                                                                : 0;
    DRIVER_CHECK(btn != 0, "button is 0");

    XTestFakeButtonEvent(display, btn, True, 0);
    XFlush(display);

    XCloseDisplay(display);

    return true;
}

bool GTMouseDriver::release(Qt::MouseButton button) {
    // TODO: check if this key has been already pressed
    QByteArray display_name = qgetenv("DISPLAY");
    DRIVER_CHECK(!display_name.isEmpty(), "Environment variable \"DISPLAY\" not found");

    Display* display = XOpenDisplay(display_name.constData());
    DRIVER_CHECK(display != nullptr, "display is NULL");

    unsigned int btn = button == Qt::LeftButton ? 1 : button == Qt::RightButton ? 3
                                                  : button == Qt::MidButton     ? 2
                                                                                : 0;
    DRIVER_CHECK(btn != 0, "button is 0");

    XTestFakeButtonEvent(display, btn, False, 0);
    XFlush(display);

    XCloseDisplay(display);

    return true;
}

bool GTMouseDriver::scroll(int value) {
    QByteArray display_name = qgetenv("DISPLAY");
    DRIVER_CHECK(!display_name.isEmpty(), "Environment variable \"DISPLAY\" not found");

    Display* display = XOpenDisplay(display_name.constData());
    DRIVER_CHECK(display != nullptr, "display is NULL");

    unsigned button = value > 0 ? Button4 : Button5;  // Button4 - scroll up, Button5 - scroll down
    value = value > 0 ? value : -value;

    for (int i = 0; i < value; i++) {
        XTestFakeButtonEvent(display, button, True, 0);
        XTestFakeButtonEvent(display, button, False, 0);
    }

    XFlush(display);
    XCloseDisplay(display);

    GTThread::waitForMainThread();
    return true;
}

#endif
}  // namespace HI
