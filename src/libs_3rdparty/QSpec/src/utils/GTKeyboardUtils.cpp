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

#include "utils/GTKeyboardUtils.h"

namespace HI {

void GTKeyboardUtils::selectAll(GUITestOpStatus &os)
{
    GTKeyboardDriver::keyClick( 'a', Qt::ControlModifier);
}

void GTKeyboardUtils::copy(GUITestOpStatus &os)
{
    GTKeyboardDriver::keyClick( 'c', Qt::ControlModifier);
}

void GTKeyboardUtils::paste(GUITestOpStatus &os)
{
    GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);
}

void GTKeyboardUtils::cut(GUITestOpStatus &os)
{

    GTKeyboardDriver::keyClick( 'x', Qt::ControlModifier);
}

} // namespace
