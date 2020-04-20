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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include "SchemeWrapper.h"
#include "UgeneContextWrapper.h"

#include "U2Script.h"

static U2::UgeneContextWrapper *GLOBAL_CONTEXT = NULL;

U2ErrorType processTask( U2::Task *task ) {
    if ( NULL == GLOBAL_CONTEXT ) { // when script interface was used from inside UGENE
                                    // now it's being used in unit tests
        CHECK( U2::UgeneContextWrapper::isAppContextInitialized( ), U2_INVALID_CALL );
        U2::TaskScheduler *scheduler = U2::AppContext::getTaskScheduler( );
        CHECK( NULL != scheduler, U2_INVALID_CALL );
        scheduler->registerTopLevelTask( task );
    } else {
        GLOBAL_CONTEXT->processTask( task );
    }
    return U2_OK;
}

extern "C" {

U2SCRIPT_EXPORT U2ErrorType initContext( const wchar_t *_workingDirectoryPath ) {
    QString workingDirectoryPath = QString::fromWCharArray( _workingDirectoryPath );
    QFileInfo info( workingDirectoryPath );
    if ( NULL == workingDirectoryPath || !info.isDir( ) || !info.exists( ) ) {
        return U2_INVALID_PATH;
    }
    if ( NULL == GLOBAL_CONTEXT ) {
        try {
            GLOBAL_CONTEXT = new U2::UgeneContextWrapper( workingDirectoryPath );
        } catch ( const std::bad_alloc & ) {
            delete GLOBAL_CONTEXT;
            return U2_NOT_ENOUGH_MEMORY;
        }
    } else {
        return U2_INVALID_CALL;
    }
    return U2_OK;
}

U2SCRIPT_EXPORT U2ErrorType releaseContext( ) {
    if ( NULL != GLOBAL_CONTEXT ) {
        delete GLOBAL_CONTEXT;
        GLOBAL_CONTEXT = NULL;
    } else {
        return U2_INVALID_CALL;
    }
    return U2_OK;
}

};
