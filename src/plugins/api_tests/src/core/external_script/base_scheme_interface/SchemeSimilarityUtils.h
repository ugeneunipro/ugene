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

#ifndef _U2_SCHEME_SIMILARITY_UTILS_H_
#define _U2_SCHEME_SIMILARITY_UTILS_H_

#include <QFile>
#include <U2Core/U2OpStatus.h>
#include <U2Script/U2Script.h>

#define CHECK_U2_ERROR( errorCode ) \
    CHECK_TRUE( U2_OK == errorCode, QString::fromWCharArray( getErrorString( errorCode ) ) )

namespace U2 {

class SchemeSimilarityUtils {
public:
    static void             checkSchemesSimilarity( SchemeHandle assembledScheme,
                                const QString &pathToProperScheme, U2OpStatus &stateInfo );

private:
    static int              getSchemeDescriptionStartPos( const QString &schemeContent );
    static int              getSchemeDescriptionEndPos( const QString &schemeContent );
    static QString          getSchemeContentByHandle( SchemeHandle scheme,
                                U2OpStatus &stateInfo );
    static QString          getSchemeContentByFilePath( const QString &pathToScheme,
                                U2OpStatus &stateInfo );
    static QString          readFileContent( QFile &file, U2OpStatus &stateInfo );
    static void             skipSchemeSpecificNames( QString &schemeContent );
    static QStringList      getNonSpaceStatementsFromScheme( const QString &schemeContent );
    static void             skipElementNames( QString &schemeContent );
    static void             skipElementIds( QString &schemeContent );
    static void             skipActorBindingsBlockBoundaries( QString &schemeContent );
    static void             skipValidatorBlocks( QString &schemeContent );
};

} // namespace U2

#endif // _U2_C_INTERFACE_SAS_TESTS_H_
