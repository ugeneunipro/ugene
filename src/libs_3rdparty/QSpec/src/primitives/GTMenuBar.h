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

#pragma once
#include <QMenuBar>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTMenuBar {
public:
    // clicks cornerMenu of QMenuBar as if it is a standard {Minimize, Maximize, Close} buttons widget
    // fails if menuBar is NULL or action as int value is not one of GTGlobals::WindowAction values
    static void clickCornerMenu(QMenuBar* mBar, GTGlobals::WindowAction action);
};

}  // namespace HI
