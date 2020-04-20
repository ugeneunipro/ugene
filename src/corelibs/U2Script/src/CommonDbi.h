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

#ifndef _COMMON_DBI_H_
#define _COMMON_DBI_H_

#include "globals.h"

// object types that can be imported to UGENE DB
enum ObjectType {
    UNSUPPORTED,
    SEQUENCE,
    MSA
};

// available formats for storing objects from UGENE DB
enum FileFormat {
    CLUSTAL_ALN,
    PLAIN_TEXT,
    FASTA,
    FASTQ,
    GENBANK,
    RAW
};

typedef void * UgeneDbHandle;

#ifdef __cplusplus
extern "C" {
#endif

U2SCRIPT_EXPORT void                importFileToUgeneDb( const wchar_t *url,
                                        UgeneDbHandle *resultObjects, int maxObjectCount );
U2SCRIPT_EXPORT void                releaseObject( UgeneDbHandle resultObject );
U2SCRIPT_EXPORT UgeneDbHandle       cloneObject( UgeneDbHandle object );
U2SCRIPT_EXPORT void                saveObjectsToFile( UgeneDbHandle *objects, int objectCount,
                                        const wchar_t *url, FileFormat format );
U2SCRIPT_EXPORT ObjectType          getObjectType( UgeneDbHandle object );
U2SCRIPT_EXPORT void                getObjectName( UgeneDbHandle object, int maxExpectedNameLength,
                                        wchar_t *name );
U2SCRIPT_EXPORT void                setObjectName( UgeneDbHandle object, const wchar_t *newName );

#ifdef __cplusplus
}
#endif

#endif // _COMMON_DBI_H_
