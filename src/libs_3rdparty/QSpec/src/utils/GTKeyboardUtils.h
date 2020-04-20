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

#ifndef GTKEYBOARD_UTILS_H
#define GTKEYBOARD_UTILS_H

#include "drivers/GTKeyboardDriver.h"
#include "GTGlobals.h"

namespace HI {
/*!
 * \brief The class contains the most commonly used keyboard hotkeys
 */
class HI_EXPORT GTKeyboardUtils {
public:
    static void selectAll(GUITestOpStatus&); // ctrl (or cmd on MacOS) + A
    static void copy(GUITestOpStatus&);      // ctrl (or cmd on MacOS) + C
    static void paste(GUITestOpStatus&);     // ctrl (or cmd on MacOS) + V
    static void cut(GUITestOpStatus&);       // ctrl (or cmd on MacOS) + X
};

} // namespace

#endif
