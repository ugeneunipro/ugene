/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>

#include "GTUtilsEscClicker.h"

namespace U2 {

GTUtilsEscClicker::GTUtilsEscClicker( HI::GUITestOpStatus &_os, const QString &menuObjectName, bool _mouse )
    : Filler( _os, GUIDialogWaiter::WaitSettings( menuObjectName, GUIDialogWaiter::Popup ) ),
    mouse( _mouse )
{

}

void GTUtilsEscClicker::commonScenario( )
{
    GTGlobals::sleep( 1000 );
    if ( mouse ) {
        QPoint p = GTMouseDriver::getMousePosition( );
        p.setX( p.x( ) - 50 );
        GTMouseDriver::moveTo( p );
        GTMouseDriver::click();
    } else {
        GTKeyboardDriver::keyClick(Qt::Key_Escape);
    }
}

} // namespace U2
