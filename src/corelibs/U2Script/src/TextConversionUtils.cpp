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

#include <U2Core/U2SafePoints.h>

#include "TextConversionUtils.h"

static QString LAST_UNCONVERTED_STRING = QString( );

namespace U2 {

U2ErrorType TextConversionUtils::qstringToCstring( const QString &source, int maxExpectedLength,
    wchar_t *destination )
{
    CHECK( NULL != destination, U2_INVALID_STRING );
    const int nameLength = source.length( ) + 1;
    CHECK_EXT( nameLength <= maxExpectedLength, LAST_UNCONVERTED_STRING = source,
        U2_TOO_SMALL_BUFFER );
    source.toWCharArray(destination);
    destination[source.length( )] = '\0';
    return U2_OK;
}

U2ErrorType TextConversionUtils::repeatLastConversion( int maxExpectedLength, wchar_t *destination,
    int *requiredSize )
{
    CHECK( NULL != requiredSize, U2_NUM_ARG_OUT_OF_RANGE );
    *requiredSize = LAST_UNCONVERTED_STRING.length( ) + 1;
    U2ErrorType result = qstringToCstring( LAST_UNCONVERTED_STRING, maxExpectedLength, destination );
    if ( U2_OK == result ) {
        LAST_UNCONVERTED_STRING = QString( );
    }
    return result;
}

} // namespace U2
