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

#ifdef __GNUC__
    #include <stdlib.h>
#endif

#include <U2Core/U2SafePoints.h>
#include <U2Lang/WorkflowRunSerializedSchemeTask.h>

#include "SchemeWrapper.h"
#include "TextConversionUtils.h"

#include "SchemeHandle.h"

extern U2ErrorType processTask( U2::Task *task );

extern "C" {

U2SCRIPT_EXPORT U2ErrorType createScheme( const wchar_t *_pathToScheme, SchemeHandle *scheme ) {
    const QString pathToScheme = QString::fromWCharArray( _pathToScheme );
    U2ErrorType result = U2_OK;
    try {
        *scheme = new U2::SchemeWrapper( pathToScheme, &result );
    } catch ( const std::bad_alloc & ) {
        delete reinterpret_cast<U2::SchemeWrapper *>( *scheme );
        result = U2_NOT_ENOUGH_MEMORY;
    }
    return result;
}

U2SCRIPT_EXPORT U2ErrorType addReaderToScheme( SchemeHandle _scheme, const wchar_t *_readerType,
    const wchar_t *_inputFilePath, int maxExpectedNameLength, wchar_t *_name )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _readerType && NULL != _inputFilePath && NULL != _name, U2_INVALID_STRING );
    const QString readerType = QString::fromWCharArray( _readerType );
    const QString inputFilePath = QString::fromWCharArray( _inputFilePath );

    QString readerName;
    U2ErrorType result = scheme->addReaderAndGetItsName( readerType, inputFilePath, readerName );
    CHECK( U2_OK == result, result );
    return U2::TextConversionUtils::qstringToCstring( readerName, maxExpectedNameLength, _name );
}

U2SCRIPT_EXPORT U2ErrorType addWriterToScheme( SchemeHandle _scheme, const wchar_t *_writerType,
    const wchar_t *_outputFilePath, int maxExpectedNameLength, wchar_t *_name )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _writerType && NULL != _outputFilePath && NULL != _name, U2_INVALID_STRING );
    const QString writerType = QString::fromWCharArray( _writerType );
    const QString outputFilePath = QString::fromWCharArray( _outputFilePath );

    QString writerName;
    U2ErrorType result = scheme->addWriterAndGetItsName( writerType, outputFilePath, writerName );
    CHECK( U2_OK == result, result );
    return U2::TextConversionUtils::qstringToCstring( writerName, maxExpectedNameLength, _name );
}

U2SCRIPT_EXPORT U2ErrorType setSchemeElementAttribute( SchemeHandle _scheme,
    const wchar_t *_elementName, const wchar_t *_attributeName, const wchar_t *_attributeValue )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _elementName && NULL != _attributeName && NULL != _attributeValue,
        U2_INVALID_STRING );
    const QString elementName = QString::fromWCharArray( _elementName );
    const QString attributeName = QString::fromWCharArray( _attributeName );
    const QString attributeValue = QString::fromWCharArray( _attributeValue);

    return scheme->setElementAttribute( elementName, attributeName, attributeValue );
}

U2SCRIPT_EXPORT U2ErrorType getSchemeElementAttribute( SchemeHandle _scheme,
    const wchar_t *_elementName, const wchar_t *_attributeName, int maxExpectedNameLength,
    wchar_t *_attributeValue )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _elementName && NULL != _attributeName && NULL != _attributeValue,
        U2_INVALID_STRING );
    const QString elementName = QString::fromWCharArray( _elementName );
    const QString attributeName = QString::fromWCharArray( _attributeName );

    QString value;
    U2ErrorType result = scheme->getElementAttribute( elementName, attributeName, value );
    CHECK( U2_OK == result, result );
    return U2::TextConversionUtils::qstringToCstring( value, maxExpectedNameLength,
        _attributeValue );
}

U2SCRIPT_EXPORT U2ErrorType addElementToScheme( SchemeHandle _scheme, const wchar_t *_elementType,
    int maxExpectedNameLength, wchar_t *_name )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _elementType, U2_INVALID_STRING );
    const QString elementType = QString::fromWCharArray( _elementType );

    QString elementName;
    U2ErrorType result = scheme->addNewElementAndGetItsName( elementType, elementName );
    CHECK( U2_OK == result, result );
    return U2::TextConversionUtils::qstringToCstring( elementName, maxExpectedNameLength, _name );
}

U2SCRIPT_EXPORT U2ErrorType addFlowToScheme( SchemeHandle _scheme, const wchar_t *_srcElementName,
    const wchar_t *_srcPortName, const wchar_t *_dstElementName, const wchar_t *_dstPortName )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _srcElementName && NULL != _srcPortName && NULL != _dstElementName
        && NULL != _dstPortName, U2_INVALID_STRING );
    const QString srcElementName = QString::fromWCharArray( _srcElementName );
    const QString srcPortName = QString::fromWCharArray( _srcPortName );
    const QString dstElementName = QString::fromWCharArray( _dstElementName );
    const QString dstPortName = QString::fromWCharArray( _dstPortName );

    return scheme->addFlow( srcElementName, srcPortName, dstElementName, dstPortName );
}

U2SCRIPT_EXPORT U2ErrorType addSchemeActorsBinding( SchemeHandle _scheme,
    const wchar_t *_srcElementName, const wchar_t *_srcSlotName, const wchar_t *_dstElementName,
    const wchar_t *_dstPortAndSlotNames )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _srcElementName && NULL != _srcSlotName && NULL != _dstElementName
        && NULL != _dstPortAndSlotNames, U2_INVALID_STRING );
    const QString srcElementName = QString::fromWCharArray( _srcElementName );
    const QString srcSlotName = QString::fromWCharArray( _srcSlotName );
    const QString dstElementName = QString::fromWCharArray( _dstElementName );
    const QString dstPortAndSlotNames = QString::fromWCharArray( _dstPortAndSlotNames );

    return scheme->addActorsBinding( srcElementName, srcSlotName, dstElementName,
        dstPortAndSlotNames );
}

U2SCRIPT_EXPORT U2ErrorType saveSchemeToFile( SchemeHandle _scheme, const wchar_t *_path ) {
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    CHECK( NULL != scheme, U2_INVALID_SCHEME );
    CHECK( NULL != _path, U2_INVALID_STRING );
    QString pathString = QString::fromWCharArray( _path );
    return scheme->saveToFile( pathString );
}

U2SCRIPT_EXPORT void releaseScheme( SchemeHandle _scheme ) {
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    delete scheme;
}

U2SCRIPT_EXPORT U2ErrorType createSas( const wchar_t *_algorithmType, const wchar_t *_inputPath,
    const wchar_t *_outputPath, SchemeHandle *scheme )
{
    CHECK( NULL != _algorithmType, U2_INVALID_STRING );
    const QString algorithmType = QString::fromWCharArray( _algorithmType );
    const QString inputPath = QString::fromWCharArray( _inputPath );
    const QString outputPath = QString::fromWCharArray( _outputPath );
    return U2::SchemeWrapper::createSas( algorithmType, inputPath, outputPath,
        reinterpret_cast<U2::SchemeWrapper **>( scheme ) );
}

U2SCRIPT_EXPORT U2ErrorType launchScheme( SchemeHandle _scheme, int *outputFilesCount,
    wchar_t ***outputFileNames )
{
    U2::SchemeWrapper *scheme = reinterpret_cast<U2::SchemeWrapper *>( _scheme );
    if ( NULL == scheme ) {
        return U2_INVALID_SCHEME;
    }
    QString pathToScheme;
    U2ErrorType result = scheme->saveToFile( pathToScheme );
    CHECK( U2_OK == result, result );
    U2::BaseSerializedSchemeRunner *launchSchemeTask = NULL;
    try {
        QStringList outputFiles;
        launchSchemeTask = new U2::WorkflowRunSerializedSchemeTask( pathToScheme, NULL,
            outputFiles );
        result = processTask( launchSchemeTask );
        CHECK( U2_OK == result, result );
        const int countOfFiles = outputFiles.size( );
        *outputFileNames = ( wchar_t ** )malloc( countOfFiles * sizeof( wchar_t ** ) );
        if ( NULL == *outputFileNames ) {
            throw std::bad_alloc();
        }
        for ( int i = 0; i < countOfFiles; ++i ) {
            const int pathLength = outputFiles[i].size( ) + 1;
            ( *outputFileNames )[i] = ( wchar_t * )malloc( pathLength * sizeof( wchar_t ) );
            // handle failed memory allocation
            if ( NULL == ( *outputFileNames )[i] ) {
                for ( int j = 0; j < i; ++j ) {
                    free( ( *outputFileNames )[i] );
                }
                free( *outputFileNames );
                throw std::bad_alloc();
            }
            outputFiles[i].toWCharArray( ( *outputFileNames )[i] );
            ( *outputFileNames )[i][outputFiles[i].size( )] = '\0';
        }
        *outputFilesCount = countOfFiles;
    } catch ( const std::bad_alloc & ) {
        delete launchSchemeTask;
        return U2_NOT_ENOUGH_MEMORY;
    }
    return U2_OK;
}

U2SCRIPT_EXPORT U2ErrorType launchSas( const wchar_t *_algorithmType, const wchar_t *_inputPath,
    const wchar_t *_outputPath, int *outputFilesCount, wchar_t ***_outputFileNames )
{
    CHECK( NULL != _algorithmType && NULL != _inputPath && NULL != _outputPath, U2_INVALID_STRING );
    U2::SchemeWrapper *scheme = NULL;
    const QString algorithmType = QString::fromWCharArray( _algorithmType );
    const QString inputPath = QString::fromWCharArray( _inputPath );
    const QString outputPath = QString::fromWCharArray( _outputPath );

    U2ErrorType result = U2::SchemeWrapper::createSas( algorithmType, inputPath, outputPath,
        &scheme );
    CHECK( U2_OK == result, result );
    result = launchScheme( scheme, outputFilesCount, _outputFileNames );

    delete scheme;
    return U2_OK;
}

}
