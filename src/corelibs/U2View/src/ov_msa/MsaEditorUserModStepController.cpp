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

#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "MsaEditorUserModStepController.h"

namespace U2 {

MsaEditorUserModStepController::MsaEditorUserModStepController( const U2EntityRef &_msaEntityRef )
    : msaEntityRef( _msaEntityRef ), msaChangeTracker( NULL )
{

}

MsaEditorUserModStepController::~MsaEditorUserModStepController( )
{
    delete msaChangeTracker;
}

void MsaEditorUserModStepController::startTracking( U2OpStatus &os )
{
    if ( NULL != msaChangeTracker ) {
        os.setError( "Another action changing alignment is being performed now" );
        return;
    }
    msaChangeTracker = new U2UseCommonUserModStep( msaEntityRef, os );
}

void MsaEditorUserModStepController::finishTracking( )
{
    delete msaChangeTracker;
    msaChangeTracker = NULL;
}

} // namespace U2
