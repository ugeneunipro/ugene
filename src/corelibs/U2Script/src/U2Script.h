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

#ifndef _U2_SCRIPT_H_
#define _U2_SCRIPT_H_

#include "FormatDetection.h"
#include "SchemeHandle.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function initializes UGENE environment, namely creates all plugins and services.
 *
 * This function has to be called before any other functions in U2Script.
 * All subsequent calls do not affect the environment until releaseContext( ) is called.
 *
 * Possible returning error codes:
 * U2_OK -                 success
 * U2_INVALID_CALL -       on subsequent invocation without calling releaseContext( )
 * U2_INVALID_PATH -       the `workingDirectoryPath` does not exist or contains path to a file
 * U2_NOT_ENOUGH_MEMORY -  context initialization ran out of memory
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 * If the `workingDirectoryPath` is valid but the folder contains no UGENE binaries
 * then UGENE plugins is not found. But the Workflow Designer computational elements
 * are defined in plugins, so the computational schemes do not pass the validation
 * since they use undefined elements.
 *
 */
U2SCRIPT_EXPORT U2ErrorType         initContext(    const wchar_t *        workingDirectoryPath );

/*
 * This function deallocates all resources acquired by UGENE environment.
 *
 * This function has be called after all custom interaction with U2Script interface is finished.
 *
 * Possible returning error codes:
 * U2_OK -                 success
 * U2_INVALID_CALL -       the invocation does not follow corresponding initContext( ) call
 *
 * If returning value is not U2_OK the environment is not affected by the invocation.
 *
 */
U2SCRIPT_EXPORT U2ErrorType         releaseContext( );

#ifdef __cplusplus
}
#endif

#endif // _U2_SCRIPT_H_
