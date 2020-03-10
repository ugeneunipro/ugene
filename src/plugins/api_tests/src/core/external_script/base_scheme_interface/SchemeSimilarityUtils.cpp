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

#include <U2Lang/HRSchemaSerializer.h>

#include "SchemeSimilarityUtils.h"

static const QString TEMP_SCHEMES_DIR_PATH = QDir::tempPath( );
static const int SUBSTRING_NOT_FOUND = -1;

namespace U2 {
using namespace WorkflowSerialize;

void SchemeSimilarityUtils::checkSchemesSimilarity( SchemeHandle assembledScheme,
    const QString &pathToProperScheme, U2OpStatus &stateInfo )
{
    QString assembledSchemeContent = getSchemeContentByHandle( assembledScheme, stateInfo );
    CHECK_OP( stateInfo, );
    skipSchemeSpecificNames( assembledSchemeContent );
    QString properSchemeContent = getSchemeContentByFilePath( pathToProperScheme, stateInfo );
    CHECK_OP( stateInfo, );
    skipSchemeSpecificNames( properSchemeContent );

    QStringList properSchemeStatements = getNonSpaceStatementsFromScheme( properSchemeContent );
    QStringList assembledSchemeStatements = getNonSpaceStatementsFromScheme(
        assembledSchemeContent );

    foreach ( QString statement, assembledSchemeStatements ) {
        CHECK_EXT( properSchemeContent.contains( statement ),
            stateInfo.setError( QString( "The proper scheme doesn't contain \"%1\""
            " statement from assembled scheme" ).arg( statement ) ), );
        const int properStatementPos = properSchemeStatements.indexOf( statement );
        properSchemeStatements.removeAt( properStatementPos );
    }
    CHECK_EXT( properSchemeStatements.isEmpty( ), stateInfo.setError( "Too few definitions"
        " were listed in test scheme" ), );
}

int SchemeSimilarityUtils::getSchemeDescriptionStartPos( const QString &schemeContent ) {
    return schemeContent.indexOf( Constants::BLOCK_START ) + 1;
}

int SchemeSimilarityUtils::getSchemeDescriptionEndPos( const QString &schemeContent ) {
    int result = schemeContent.indexOf( Constants::META_START );
    if ( -1 == result ) {
        result = schemeContent.lastIndexOf( Constants::BLOCK_END );
    }
    return result;
}

QString SchemeSimilarityUtils::getSchemeContentByHandle( SchemeHandle scheme,
    U2OpStatus &stateInfo )
{
    QString pathToScheme( TEMP_SCHEMES_DIR_PATH + "/test_scheme.uwl" );
    wchar_t *wPathToScheme = ( wchar_t * )malloc( ( pathToScheme.length( ) + 1 )
        * sizeof( wchar_t ) );
    pathToScheme.toWCharArray( wPathToScheme );
    wPathToScheme[pathToScheme.length( )] = '\0';
    U2ErrorType error = saveSchemeToFile( scheme, wPathToScheme );
    CHECK_EXT( U2_OK == error, stateInfo.setError(
        QString::fromWCharArray( getErrorString( error ) ) ), QString( ) );

    QFile schemeFile( pathToScheme );
    const QString schemeContent = readFileContent( schemeFile, stateInfo );
    schemeFile.remove( );
    CHECK_OP( stateInfo, QString( ) );

    CHECK_EXT( schemeContent.startsWith( Constants::HEADER_LINE ),
        stateInfo.setError( "The file with scheme doesn't start with header line" );
    schemeFile.remove( ), QString( ) );
    return schemeContent;
}

QString SchemeSimilarityUtils::getSchemeContentByFilePath( const QString &pathToScheme,
    U2OpStatus &stateInfo )
{
    QFile schemeFile( pathToScheme );
    const QString schemeContent = readFileContent( schemeFile, stateInfo );
    return schemeContent;
}

QString SchemeSimilarityUtils::readFileContent( QFile &file, U2OpStatus &stateInfo ) {
    CHECK_EXT( ( file.isOpen( ) || file.open( QIODevice::ReadOnly | QIODevice::Text ) ),
        stateInfo.setError( QString( "Could not open the file: \"%1\"" )
        .arg( file.fileName( ) ) ), QString( ) );

    QTextStream contentReader( &file );
    return contentReader.readAll( );
}

void SchemeSimilarityUtils::skipSchemeSpecificNames( QString &schemeContent ) {
    skipElementNames( schemeContent );
    skipElementIds( schemeContent );
    skipActorBindingsBlockBoundaries( schemeContent );
    skipValidatorBlocks( schemeContent );
}

void SchemeSimilarityUtils::skipElementNames( QString &schemeContent ) {
    int nextNameAttributePosition = schemeContent.indexOf( QRegExp( Constants::NAME_ATTR + "\\s*" + Constants::COLON ) );
    while ( SUBSTRING_NOT_FOUND != nextNameAttributePosition ) {
        const int nameStartPos = schemeContent.indexOf( Constants::COLON,
            nextNameAttributePosition ) + 1;
        const int nameEndPos = schemeContent.indexOf( Constants::SEMICOLON,
            nameStartPos );
        schemeContent.replace( nameStartPos, nameEndPos - nameStartPos, "\"\"" );
        nextNameAttributePosition = schemeContent.indexOf( Constants::NAME_ATTR + Constants::COLON ,
            nameStartPos );
    }
}

void SchemeSimilarityUtils::skipElementIds( QString &schemeContent ) {
    const QRegExp elementIdStartPattern( Constants::NEW_LINE + Constants::TAB
        + "\\w" );
    int elementIdEndPos = 0;
    int elementIdStartPos = 0;
    Q_FOREVER {
        elementIdStartPos = schemeContent.indexOf( elementIdStartPattern, elementIdEndPos )
            + Constants::NEW_LINE.length( ) + Constants::TAB.length( );
        if ( SUBSTRING_NOT_FOUND == elementIdStartPos || elementIdStartPos < elementIdEndPos ) {
            break;
        }
        elementIdEndPos = schemeContent.indexOf( Constants::BLOCK_START,
            elementIdStartPos ) - 1;
        const QString elementId = schemeContent
            .mid( elementIdStartPos, elementIdEndPos - elementIdStartPos ).trimmed( );
        schemeContent.replace( elementIdStartPos, elementId.length( ), QString( ) );
        const int elementDescEnd = schemeContent.indexOf( Constants::BLOCK_END,
            elementIdStartPos );

        int elementIdPos = schemeContent.indexOf( elementId, elementDescEnd );
        while ( SUBSTRING_NOT_FOUND != elementIdPos ) {
            QChar nextSymbol = schemeContent[elementIdPos + elementId.length( )];
            if ( !nextSymbol.isLetterOrNumber( ) && Constants::DASH[0] != nextSymbol
                && Constants::COLON[0] != schemeContent[elementIdPos - 1] )
            {
                schemeContent.replace( elementIdPos, elementId.length( ), QString( ) );
            } else {
                elementIdPos += elementId.length( );
            }
            elementIdPos = schemeContent.indexOf( elementId, elementIdPos );
        }
    }
}

void SchemeSimilarityUtils::skipValidatorBlocks( QString &schemeContent ) {
    const QRegExp validatorBlockStartPattern( "\\s+\\" + Constants::VALIDATOR + "\\s+" );
    int validatorBlockStartPos = 0;
    int validatorBlockEndPos = 0;
    Q_FOREVER {
        validatorBlockStartPos = schemeContent.indexOf( validatorBlockStartPattern,
            validatorBlockEndPos );
        if ( SUBSTRING_NOT_FOUND == validatorBlockStartPos ) {
            break;
        }

        int currentBracePos = schemeContent.indexOf( Constants::BLOCK_START,
            validatorBlockStartPos );
        int braceCount = 1;
        while ( 0 != braceCount ) {
            const int blockStartPos = schemeContent.indexOf( Constants::BLOCK_START,
                currentBracePos + 1 );
            const int blockEndPos = schemeContent.indexOf( Constants::BLOCK_END,
                currentBracePos + 1 );
            if ( blockStartPos < blockEndPos ) {
                ++braceCount;
                currentBracePos = blockStartPos;
            } else {
                --braceCount;
                currentBracePos = blockEndPos;
            }
        }
        validatorBlockEndPos = currentBracePos + 1;
        schemeContent.replace( validatorBlockStartPos,
            validatorBlockEndPos - validatorBlockStartPos, QString( ) );
        validatorBlockEndPos = validatorBlockStartPos;
    }
}

QStringList SchemeSimilarityUtils::getNonSpaceStatementsFromScheme( const QString &schemeContent )
{
    const int schemeDescStartPos = getSchemeDescriptionStartPos( schemeContent );
    const int schemeDescEndPos = getSchemeDescriptionEndPos( schemeContent );

    QStringList statements = schemeContent.mid( schemeDescStartPos,
        schemeDescEndPos - schemeDescStartPos ).split( Constants::NEW_LINE,
        QString::SkipEmptyParts );
    for ( int i = 0; i < statements.length( ); ++i ) {
        statements[i] = statements[i].trimmed( );
        if ( statements[i].isEmpty( ) ) {
            statements.removeAt( i );
        }
    }
    return statements;
}

void SchemeSimilarityUtils::skipActorBindingsBlockBoundaries( QString &schemeContent ) {
    const QString actorBindingsLine( Constants::NEW_LINE + Constants::TAB
        + Constants::ACTOR_BINDINGS );
    const int actorBindingsLineStart = schemeContent.indexOf( actorBindingsLine );
    if ( SUBSTRING_NOT_FOUND != actorBindingsLineStart ) {
        const int actorBindingsLineEnd = schemeContent.indexOf( Constants::BLOCK_START,
            actorBindingsLineStart ) + 1;
        schemeContent.remove( actorBindingsLineStart,
            actorBindingsLineEnd - actorBindingsLineStart );
        const QString actorBindingsEndBlock( Constants::NEW_LINE + Constants::TAB
            + Constants::BLOCK_END );
        const int actorBindingsBlockEndPos = schemeContent.indexOf( actorBindingsEndBlock,
            actorBindingsLineStart );
        schemeContent.remove( actorBindingsBlockEndPos, actorBindingsEndBlock.length( ) );
    }
}

} // namespace U2
