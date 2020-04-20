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

#ifndef _HI_GUI_GTMOUSEDRIVER_H_
#define _HI_GUI_GTMOUSEDRIVER_H_

#include "GTGlobals.h"
#include <QtCore/QPoint>

namespace HI {
/*!
 * \brief The base class for mouse's actions imitation
 */
class HI_EXPORT GTMouseDriver {
public:
    // fails if given coordinates are not in the screen's rect
    // Linux: fails if there is an opening X display error
    static bool moveTo(const QPoint& p);

    // fails if press or release fails
    // Linux: fails if there is an opening X display error
    static bool click(Qt::MouseButton = Qt::LeftButton); //all click methods work with the current position of the cursor
    static bool doubleClick();

    // fails if the given button is not Qt::LeftButton, Qt::RightButton or Qt::MiddleButton
    // Linux: fails if there is an opening X display error
    static bool press(Qt::MouseButton = Qt::LeftButton);
    static bool release(Qt::MouseButton = Qt::LeftButton); //release method should check if this key has been already pressed. Do nothing otherwise

    static bool dragAndDrop(const QPoint& start, const QPoint& end);
    static bool selectArea(const QPoint& start, const QPoint& end);

    // Linux: fails if there is an opening X display error
    static bool scroll(int value); //positive values for scrolling up, negative for scrolling down

    static QPoint getMousePosition();
private:
    static QPoint mousePos;
    static Qt::MouseButtons bp;
};

} //namespace

#endif
