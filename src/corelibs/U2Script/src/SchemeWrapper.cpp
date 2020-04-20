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

#include <QFile>
#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowUtils.h>

#include "SchemeWrapper.h"
#include "WorkflowElementFacade.h"

static const QString TMP_FILE_NAME_BASE =   "scheme_for_script";
static QString NAME_ATTR_PATTERN =          "\\s" + U2::WorkflowSerialize::Constants::NAME_ATTR + "\\s*"
                                                + U2::WorkflowSerialize::Constants::EQUALS_SIGN;
static QString TYPE_ATTR_PATTERN =          "\\s" + U2::WorkflowSerialize::Constants::TYPE_ATTR + "\\s*"
                                                + U2::WorkflowSerialize::Constants::EQUALS_SIGN;
static QString BLOCK_START_PATTERN =        "[\\s\"]\\" + U2::WorkflowSerialize::Constants::BLOCK_START
                                                + "[\\s\"]";
static QString BLOCK_END_PATTERN =          "[\\s\\"+ U2::WorkflowSerialize::Constants::SEMICOLON + "]\\"
                                                + U2::WorkflowSerialize::Constants::BLOCK_END + "\\s";
static QString DEFAULT_DATASET_NAME =       "Dataset";

static int SUBSTRING_NOT_FOUND =            -1;

namespace U2 {
using namespace WorkflowSerialize;

SchemeWrapper::SchemeWrapper( const QString &initPathToScheme, U2ErrorType *result )
    : pathToScheme( ), schemeContent( ), elementNamesAndIds( ), commentLinesPositions( )
{
    if ( QFile::exists( initPathToScheme ) ) {
        QFile schemeFile( initPathToScheme );
        CHECK_EXT( schemeFile.open( QIODevice::ReadOnly | QIODevice::Text ),
            *result = U2_FAILED_TO_READ_FILE, );
        QTextStream contentReader( &schemeFile );
        schemeContent = contentReader.readAll( );
        skipComments( );
        *result = fillElementNamesFromSchemeContent( );
        CHECK_EXT( U2_OK == *result, schemeContent = QString( ), );
    } else if ( initPathToScheme.isEmpty( ) ) {
        initSchemeContentWithEmptyScheme( );
    } else {
        *result = U2_INVALID_PATH;
        return;
    }
    const QString tmpDirPath = U2::AppContext::getAppSettings( )->getUserAppsSettings( )
        ->getCurrentProcessTemporaryDirPath( );
    U2OpStatusImpl stateInfo;
    pathToScheme = GUrlUtils::prepareTmpFileLocation( tmpDirPath, TMP_FILE_NAME_BASE,
        WorkflowUtils::WD_FILE_EXTENSIONS.first( ), stateInfo );
    CHECK_OP_EXT( stateInfo, schemeContent = QString( ); *result = U2_FAILED_TO_CREATE_DIR, );

    QFile schemeFile( pathToScheme );
    CHECK_EXT( schemeFile.open( QIODevice::WriteOnly | QIODevice::Text ),
        schemeContent = QString( ); *result = U2_FAILED_TO_CREATE_FILE, );
    *result = U2_OK;
}

SchemeWrapper::~SchemeWrapper( ) {
    if ( QFile::exists( pathToScheme ) ) {
        QFile::remove( pathToScheme );
    }
}

U2ErrorType SchemeWrapper::addReaderAndGetItsName( const QString &readerType,
    const QString &inputFilePath, QString &name )
{
    const QString inputFileAttributeName = Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ).getId( )
        + Constants::DOT + Constants::FILE_URL;
    return addElementWithAttributeAndGetItsName( readerType, inputFileAttributeName, inputFilePath,
        name );
}

U2ErrorType SchemeWrapper::addWriterAndGetItsName( const QString &writerType,
    const QString &outputFilePath, QString &name )
{
    return addElementWithAttributeAndGetItsName( writerType,
        Workflow::BaseAttributes::URL_OUT_ATTRIBUTE( ).getId( ), outputFilePath, name );
}

U2ErrorType SchemeWrapper::setElementAttribute( const QString &elementName,
    const QString &attributeName, const QString &attributeValue )
{
    CHECK( elementNamesAndIds.contains( elementName ), U2_ELEMENT_NOT_FOUND );
    QString elementType;
    U2ErrorType result = getElementType( elementName, elementType );
    CHECK( U2_OK == result, result );

    int elementDescStartPosition = SUBSTRING_NOT_FOUND;
    int elementDescEndPosition = SUBSTRING_NOT_FOUND;
    result = getEnclosingElementBoundaries( elementName, &elementDescStartPosition,
        &elementDescEndPosition );
    CHECK( U2_OK == result, result );

    QStringList attributesHierarchy = attributeName.split( Constants::DOT );
    bool attributeExists = false;
    result = WorkflowElementFacade::doesElementHaveParameter( elementType,
        attributesHierarchy[0], &attributeExists );
    CHECK( U2_OK == result, result );
    CHECK( attributeExists, U2_INVALID_NAME );
    QString deepestAttributeName;
    bool replaceIfExists = true;
    QStringList valuesTuple( attributeValue );
    if ( Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ) == attributesHierarchy[0] ) {
        if ( Constants::DATASET_NAME == attributesHierarchy.last( ) ) {
            result = getBoundariesOfUrlInAttribute( attributeValue, false,
                &elementDescStartPosition, &elementDescEndPosition );
            CHECK( U2_OK != result, U2_INVALID_STRING ); // dataset with the given name already exists
        }
        result = getUrlInAttributePositionByName( attributesHierarchy, true,
            &elementDescStartPosition, &elementDescEndPosition, deepestAttributeName,
            replaceIfExists );
        CHECK( U2_OK == result, result );
        if ( Constants::FILE_URL == deepestAttributeName ) {
            valuesTuple = attributeValue.split( Constants::SEMICOLON,
                QString::SkipEmptyParts );
        }
    }
    else {
        deepestAttributeName = attributeName;
    }
    foreach ( QString value, valuesTuple ) {
        result = setElementAttributeInRange( deepestAttributeName, value, elementDescStartPosition,
            elementDescEndPosition, replaceIfExists );
        CHECK( U2_OK == result, result );
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::getElementAttribute( const QString &elementName,
    const QString &attributeName, QString &attributeValue )
{
    CHECK( elementNamesAndIds.contains( elementName ), U2_ELEMENT_NOT_FOUND );
    QString elementType;
    U2ErrorType result = getElementType( elementName, elementType );
    CHECK( U2_OK == result, result );

    int elementDescStartPosition = SUBSTRING_NOT_FOUND;
    int elementDescEndPosition = SUBSTRING_NOT_FOUND;
    result = getEnclosingElementBoundaries( elementName, &elementDescStartPosition,
        &elementDescEndPosition );
    CHECK( U2_OK == result, result );

    QStringList attributesHierarchy = attributeName.split( Constants::DOT );
    bool attributeExists = false;
    result = WorkflowElementFacade::doesElementHaveParameter( elementType,
        attributesHierarchy[0], &attributeExists );
    CHECK( U2_OK == result, result );
    CHECK( attributeExists, U2_INVALID_NAME );
    QString deepestAttributeName;
    if ( Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ) == attributesHierarchy[0] ) {
        bool replaceIfExists = false; // unused
        result = getUrlInAttributePositionByName( attributesHierarchy, false,
            &elementDescStartPosition, &elementDescEndPosition, deepestAttributeName,
            replaceIfExists );
        CHECK( U2_OK == result, result );
    }
    else {
        deepestAttributeName = attributeName;
    }
    result = getElementAttributeFromRange( deepestAttributeName, elementDescStartPosition,
        elementDescEndPosition, attributeValue );
    CHECK( U2_OK == result, result );
    return U2_OK;
}

U2ErrorType SchemeWrapper::addNewElementAndGetItsName( const QString &elementType,
    QString &elementName )
{
    QString newElementId;
    elementName = QString( );
    U2ErrorType result = setUniqueElementNameAndId( elementType, elementName, newElementId );
    CHECK( U2_OK == result, result );
    QString newElementBlockContent;
    newElementBlockContent += HRSchemaSerializer::makeEqualsPair( Constants::TYPE_ATTR,
        elementType );
    newElementBlockContent += HRSchemaSerializer::makeEqualsPair( Constants::NAME_ATTR,
        elementName );
    QString newElementBlock = HRSchemaSerializer::makeBlock( newElementId,
        Constants::NO_NAME, newElementBlockContent );

    int newElementBlockInsertPosition = SUBSTRING_NOT_FOUND;
    result = getSchemeDescriptionStart( &newElementBlockInsertPosition );
    CHECK( U2_OK == result, result );

    newElementBlockInsertPosition = schemeContent.indexOf( QRegExp( "[\\w\\n]" ),
        newElementBlockInsertPosition );
    CHECK( SUBSTRING_NOT_FOUND != newElementBlockInsertPosition, U2_INVALID_SCHEME );
    if ( Constants::NEW_LINE[0] == schemeContent[newElementBlockInsertPosition] ) {
        ++newElementBlockInsertPosition;
    } else {
        newElementBlockInsertPosition -= 2;
    }
    result = insertStringToScheme( newElementBlockInsertPosition, newElementBlock );
    CHECK( U2_OK == result, U2_INVALID_SCHEME );
    elementNamesAndIds[elementName] = newElementId;
    return U2_OK;
}

U2ErrorType SchemeWrapper::addFlow( const QString &srcElementName, const QString &srcPortName,
    const QString &dstElementName, const QString &dstPortName )
{
    // validate input data
    U2ErrorType result = validatePortAndSlot( srcElementName, srcPortName, QString( ) );
    CHECK( U2_OK == result, result );
    result = validatePortAndSlot( dstElementName, dstPortName, QString( ) );
    CHECK( U2_OK == result, result );
    // validation end
    int flowDescriptionInsertPosition = schemeContent.indexOf(
        getBlockStartPattern( Constants::ACTOR_BINDINGS ) );
    if ( SUBSTRING_NOT_FOUND == flowDescriptionInsertPosition ) {
        result = addActorBindingsBlock( &flowDescriptionInsertPosition );
        CHECK( U2_OK == result, result );
    }
    flowDescriptionInsertPosition = schemeContent.indexOf( QRegExp( BLOCK_END_PATTERN ),
        flowDescriptionInsertPosition );
    CHECK( SUBSTRING_NOT_FOUND != flowDescriptionInsertPosition, U2_INVALID_SCHEME );
    flowDescriptionInsertPosition = qMax(
        schemeContent.lastIndexOf( QRegExp( BLOCK_START_PATTERN ), flowDescriptionInsertPosition ),
        schemeContent.lastIndexOf( Constants::NEW_LINE, flowDescriptionInsertPosition ) );
    CHECK( SUBSTRING_NOT_FOUND != flowDescriptionInsertPosition, U2_INVALID_SCHEME );
    ++flowDescriptionInsertPosition;

    const QString flowDescription = HRSchemaSerializer::makeArrowPair(
        elementNamesAndIds[srcElementName] + Constants::DOT + srcPortName,
        elementNamesAndIds[dstElementName] + Constants::DOT + dstPortName, 2 )
        + Constants::NEW_LINE;
    result = insertStringToScheme( flowDescriptionInsertPosition, flowDescription );
    CHECK( U2_OK == result, U2_INVALID_SCHEME );
    return U2_OK;
}

U2ErrorType SchemeWrapper::addActorsBinding( const QString &srcElementName,
    const QString &srcSlotName, const QString &dstElementName, const QString &dstPortAndSlotNames )
{
    const int dstPortAndSlotDelimeterPos = dstPortAndSlotNames.indexOf( Constants::DOT );
    CHECK( SUBSTRING_NOT_FOUND != dstPortAndSlotDelimeterPos, U2_INVALID_STRING );
    CHECK( SUBSTRING_NOT_FOUND == dstPortAndSlotNames.indexOf( Constants::DOT,
        dstPortAndSlotDelimeterPos + 1 ), U2_INVALID_STRING );
    const QString dstPortName = dstPortAndSlotNames.left( dstPortAndSlotDelimeterPos );
    const QString dstSlotName = dstPortAndSlotNames.mid( dstPortAndSlotDelimeterPos + 1 );
    // validate input data
    U2ErrorType result = validatePortAndSlot( srcElementName, QString( ), srcSlotName );
    CHECK( U2_OK == result, result );
    result = validatePortAndSlot( dstElementName, dstPortName, dstSlotName );
    CHECK( U2_OK == result, result );
    // validation end

    int bindingDescriptionPosition = schemeContent.indexOf( getBlockStartPattern(
        Constants::META_START ) );
    if ( SUBSTRING_NOT_FOUND == bindingDescriptionPosition ) {
        bindingDescriptionPosition = schemeContent.lastIndexOf( QRegExp( BLOCK_END_PATTERN ) );
        CHECK( SUBSTRING_NOT_FOUND != bindingDescriptionPosition, U2_INVALID_SCHEME );
        bindingDescriptionPosition = schemeContent.lastIndexOf( Constants::NEW_LINE,
            bindingDescriptionPosition );
    } else {
        QRegExp flowsDescriptionEndPattern( BLOCK_END_PATTERN );
        bindingDescriptionPosition = qMax( flowsDescriptionEndPattern.lastIndexIn( schemeContent,
            bindingDescriptionPosition ), schemeContent.lastIndexOf( Constants::NEW_LINE,
            bindingDescriptionPosition ) );
        CHECK( SUBSTRING_NOT_FOUND != bindingDescriptionPosition, U2_INVALID_SCHEME );
    }
    const QString bindingDescription = HRSchemaSerializer::makeArrowPair(
        elementNamesAndIds[srcElementName] + Constants::DOT + srcSlotName,
        elementNamesAndIds[dstElementName] + Constants::DOT + dstPortName
        + Constants::DOT + dstSlotName ) + Constants::NEW_LINE;
    result = insertStringToScheme( bindingDescriptionPosition, bindingDescription );
    CHECK( U2_OK == result, result );
    return U2_OK;
}

U2ErrorType SchemeWrapper::saveToFile( QString &path ) {
    if ( path.isEmpty( ) ) {
        path = pathToScheme;
        CHECK( validateSchemeContent( ), U2_INVALID_SCHEME );
    }
    const QString schemeFileExtension = WorkflowUtils::WD_FILE_EXTENSIONS.first( );
    QFile resultFile( path );
    if ( !path.endsWith( schemeFileExtension ) ) {
        resultFile.setFileName( path + "." + schemeFileExtension );
    }
    CHECK_EXT( resultFile.open( QFile::WriteOnly | QFile::Truncate ), path = QString( ),
        U2_FAILED_TO_CREATE_FILE );
    restoreComments( );
    QTextStream contentWriter( &resultFile );
    contentWriter << schemeContent;
    return U2_OK;
}

#define CHECK_DEL_OBJECT( statement, object ) \
    CHECK_EXT( U2_OK == statement, delete object; object = NULL, statement )

U2ErrorType SchemeWrapper::createSas( const QString &elementType, const QString &inputFilePath,
    const QString &outputFilePath, SchemeWrapper **sas )
{
    bool suits = false;
    U2ErrorType result = WorkflowElementFacade::doesElementSuitForSas( elementType, &suits );
    CHECK( U2_OK == result, result );
    CHECK( suits, U2_ELEMENT_NOT_SUIT_SAS );

    SchemeWrapper *newSas = NULL;
    try {
        newSas = new SchemeWrapper( QString( ), &result );
        CHECK( U2_OK == result, result );
        QString elementName;
        // add required element to scheme
        result = newSas->addNewElementAndGetItsName( elementType, elementName );
        CHECK_DEL_OBJECT( result, newSas );

        // get names of slots from input port of the element
        QStringList inputSlotNames;
        result = WorkflowElementFacade::getElementSlotIds( elementType, true, QString( ),
            inputSlotNames );
        CHECK_DEL_OBJECT( result, newSas );
        const QString elementInputSlot( inputSlotNames.first( ) );
        // get names of slots from output port of the element
        QStringList outputSlotNames;
        result = WorkflowElementFacade::getElementSlotIds( elementType, false, QString( ),
            outputSlotNames );
        CHECK_DEL_OBJECT( result, newSas );
        const QString elementOutputSlot( outputSlotNames.first( ) );

        // add appropriate reader to the scheme
        QString readerType;
        result = WorkflowElementFacade::getReadElementTypeForSlot( elementInputSlot, readerType );
        CHECK_DEL_OBJECT( result, newSas );
        QString readerName;
        result = newSas->addReaderAndGetItsName( readerType, inputFilePath, readerName );
        CHECK_DEL_OBJECT( result, newSas );

        // add appropriate writer to the scheme
        QString writerType;
        result = WorkflowElementFacade::getWriteElementTypeForSlot( elementOutputSlot,
            writerType );
        CHECK_DEL_OBJECT( result, newSas );
        QString writerName;
        result = newSas->addWriterAndGetItsName( writerType, outputFilePath, writerName );
        CHECK_DEL_OBJECT( result, newSas );

        // get the reader's output port name
        QString outputReaderPortId;
        result = WorkflowElementFacade::getOutputPortIdForSlot( readerType, elementInputSlot,
            outputReaderPortId );
        CHECK_DEL_OBJECT( result, newSas );
        // get the element's input port name
        QString inputElementPortId;
        result = WorkflowElementFacade::getInputPortIdForSlot( elementType, elementInputSlot,
            inputElementPortId );
        CHECK_DEL_OBJECT( result, newSas );
        // add flow between found ports
        result = newSas->addFlow( readerName, outputReaderPortId, elementName, inputElementPortId );
        CHECK_DEL_OBJECT( result, newSas );

        // get the element's output port name
        QString outputElementPortId;
        result = WorkflowElementFacade::getOutputPortIdForSlot( elementType, elementOutputSlot,
            outputElementPortId );
        CHECK_DEL_OBJECT( result, newSas );
        // get the writer's output port name
        QString inputWriterPortId;
        result = WorkflowElementFacade::getInputPortIdForSlot( writerType, elementOutputSlot,
            inputWriterPortId );
        CHECK_DEL_OBJECT( result, newSas );
        // add flow between found ports
        result = newSas->addFlow( elementName, outputElementPortId, writerName, inputWriterPortId );
        CHECK_DEL_OBJECT( result, newSas );

        // bind the reader's and the element's slots
        result = newSas->addActorsBinding( readerName, elementInputSlot, elementName,
            inputElementPortId  + Constants::DOT + elementInputSlot );
        CHECK_DEL_OBJECT( result, newSas );
        // bind the element's and the writer's slots
        result = newSas->addActorsBinding( elementName, elementOutputSlot, writerName,
            inputWriterPortId + Constants::DOT + elementOutputSlot );
        CHECK_DEL_OBJECT( result, newSas );

        // bind unbinded reader's and writer's slots
        // first get reader's output slots
        QStringList readerSlots;
        result = WorkflowElementFacade::getElementSlotIds( readerType, true, outputReaderPortId,
            readerSlots );
        CHECK_DEL_OBJECT( result, newSas );
        // then get writer's input slots
        QStringList writerSlots;
        result = WorkflowElementFacade::getElementSlotIds( writerType, true, inputWriterPortId,
            writerSlots );
        CHECK_DEL_OBJECT( result, newSas );
        // finally bind slots with the same name
        foreach ( QString readerSlot, readerSlots ) {
            if ( writerSlots.contains( readerSlot )
                && Workflow::BaseSlots::URL_SLOT( ) != readerSlot )
            {
                result = newSas->addActorsBinding( readerName, readerSlot, writerName,
                    inputWriterPortId + Constants::DOT + readerSlot );
                CHECK_DEL_OBJECT( result, newSas );
            }
        }
    } catch ( const std::bad_alloc & ) {
        delete newSas;
        newSas = NULL;
        return U2_NOT_ENOUGH_MEMORY;
    }
    *sas = newSas;
    return U2_OK;
}

#undef CHECK_DEL_OBJECT

U2ErrorType SchemeWrapper::getEnclosingElementBoundaries( const QString &elementName, int *start,
    int *end ) const
{
    int elementNamePosition = SUBSTRING_NOT_FOUND;
    U2ErrorType result = getElementNameAttributePosition( elementName, &elementNamePosition );
    CHECK( U2_OK == result, result );

    *start = SUBSTRING_NOT_FOUND;
    *end = SUBSTRING_NOT_FOUND;
    CHECK( 0 < elementNamePosition && ( schemeContent.length( ) - 1 ) > elementNamePosition,
        U2_NUM_ARG_OUT_OF_RANGE );
    const QRegExp elementDescStartPattern( BLOCK_START_PATTERN );
    *start = schemeContent.lastIndexOf( elementDescStartPattern, elementNamePosition );
    CHECK( SUBSTRING_NOT_FOUND != *start, U2_INVALID_SCHEME );
    const QRegExp elementDescEndPattern( BLOCK_END_PATTERN );
    *end = schemeContent.indexOf( elementDescEndPattern, elementNamePosition );
    // check if the founded boundaries belong to an attribute description
    int bracesBalance = 0;
    result = checkBracesBalanceInRange( *start + 1, *end, &bracesBalance );
    CHECK( U2_OK == result, U2_INVALID_SCHEME );
    QRegExp typeAttributePattern( TYPE_ATTR_PATTERN );
    int typeAttributePosition = schemeContent.indexOf( typeAttributePattern, *start );
    while ( 0 != bracesBalance || typeAttributePosition > *end
        || SUBSTRING_NOT_FOUND == typeAttributePosition )
    {
        // consequently considering all possible cases
        // 1. braces disbalance
        if ( 0 != bracesBalance ) {
            while ( 0 < bracesBalance ) {
                *end = schemeContent.indexOf( elementDescEndPattern, *end + 1 );
                CHECK( SUBSTRING_NOT_FOUND != *end, U2_INVALID_SCHEME );
                --bracesBalance;
            }
            while ( 0 > bracesBalance ) {
                *start = schemeContent.lastIndexOf( elementDescStartPattern, *start - 1 );
                CHECK( SUBSTRING_NOT_FOUND != *start, U2_INVALID_SCHEME );
                ++bracesBalance;
            }
            result = checkBracesBalanceInRange( *start + 1, *end, &bracesBalance );
            CHECK( U2_OK == result, U2_INVALID_SCHEME );
            typeAttributePosition = schemeContent.indexOf( typeAttributePattern, *start );
            if ( 0 != bracesBalance ) {
                continue;
            }
        }
        // 2. element type attribute is out of range
        if ( typeAttributePosition > *end ) {
            *end = schemeContent.indexOf( elementDescEndPattern, *end + 1 );
        }
        // 3. element type attribute is not found after `*end`
        // -> it has to be located before `*start`
        else if ( SUBSTRING_NOT_FOUND == typeAttributePosition ) {
            *start = schemeContent.lastIndexOf( elementDescStartPattern, *start - 1 );
        }
        result = checkBracesBalanceInRange( *start + 1, *end, &bracesBalance );
        CHECK( U2_OK == result, U2_INVALID_SCHEME );
        typeAttributePosition = schemeContent.indexOf( typeAttributePattern, *start );
    }
    // make founded boundaries contain an integer number of UWL strings
    *end = schemeContent.lastIndexOf( QRegExp( "[(" + Constants::BLOCK_END + ")\\"
        + Constants::SEMICOLON + "]" ), *end - 1 );
    CHECK( SUBSTRING_NOT_FOUND != *end, U2_INVALID_SCHEME );
    *end += 2;
    *start = schemeContent.indexOf( QRegExp( "[\\n\\w]" ), *start + 1 );
    return U2_OK;
}

U2ErrorType SchemeWrapper::checkBracesBalanceInRange( int startPos, int endPos, int *disbalance )
    const
{
    CHECK( 0 < startPos && 0 < endPos && ( schemeContent.length( ) - 1 ) > startPos
        && ( schemeContent.length( ) - 1 ) > endPos, U2_NUM_ARG_OUT_OF_RANGE );
    *disbalance = 0;
    const QRegExp bracesPattern( "[\\" + Constants::BLOCK_START + "\\"
        + Constants::BLOCK_END + "][\\s\"\\" + Constants::SEMICOLON + "]" );
    int searchPosition = schemeContent.indexOf( bracesPattern, startPos + 1 );
    while ( searchPosition < endPos && searchPosition != SUBSTRING_NOT_FOUND ) {
        // suppose that `BLOCK_START` and `BLOCK_END` start with different symbols
        if ( Constants::BLOCK_START[0] == schemeContent[searchPosition] ) {
            ++( *disbalance );
        } else {
            --( *disbalance );
        }
        if ( 0 > *disbalance ) {
            break;
        }
        searchPosition = schemeContent.indexOf( bracesPattern, searchPosition + 1 );
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::getElementNameAttributePosition( const QString &elementName,
    int *position ) const
{
    *position = SUBSTRING_NOT_FOUND;
    const int nameStartPosition = schemeContent.indexOf(
        HRSchemaSerializer::valueString( elementName ) );
    CHECK( SUBSTRING_NOT_FOUND != nameStartPosition, U2_ELEMENT_NOT_FOUND );
    const int nameAttributeStartPos = schemeContent.lastIndexOf( QRegExp( NAME_ATTR_PATTERN ),
        nameStartPosition );
    CHECK( SUBSTRING_NOT_FOUND != nameAttributeStartPos, U2_INVALID_SCHEME );
    *position = nameAttributeStartPos;
    return U2_OK;
}

U2ErrorType SchemeWrapper::fillElementNamesFromSchemeContent( ) {
    U2ErrorType result = U2_OK;
    QRegExp nameAttrStartPattern( NAME_ATTR_PATTERN );
    QRegExp blockStartPattern( BLOCK_START_PATTERN );
    QRegExp schemeNamePattern( "\"[^\"]*\"" );
    QRegExp blockEndPattern( BLOCK_END_PATTERN );
    QRegExp letterOrNumberPattern( "\\w" );
    int searchStartPosition = SUBSTRING_NOT_FOUND;
    result = getSchemeDescriptionStart( &searchStartPosition );
    CHECK( U2_OK == result, result );
    CHECK( SUBSTRING_NOT_FOUND != searchStartPosition, U2_INVALID_SCHEME );
    searchStartPosition = schemeContent.indexOf( blockStartPattern, searchStartPosition );
    while ( SUBSTRING_NOT_FOUND != searchStartPosition ) {
        int nextBlockStart = schemeContent.indexOf( blockStartPattern, searchStartPosition + 2 );
        int blockEnd = blockEndPattern.indexIn( schemeContent, searchStartPosition + 2 );
        CHECK_EXT( SUBSTRING_NOT_FOUND != blockEnd, elementNamesAndIds.clear( ),
            U2_INVALID_SCHEME );
        int elementNameStart = nameAttrStartPattern.indexIn( schemeContent, searchStartPosition );
        if ( SUBSTRING_NOT_FOUND == elementNameStart ) {
            return ( elementNamesAndIds.isEmpty( ) ) ? U2_INVALID_SCHEME : U2_OK;
        }
        elementNameStart += nameAttrStartPattern.matchedLength( );
        int elementNameEnd = SUBSTRING_NOT_FOUND;
        while ( SUBSTRING_NOT_FOUND != nextBlockStart && nextBlockStart < blockEnd
            && elementNameStart > nextBlockStart )
        {
            int bracesDisbalance = 1;
            int lastFoundBlockBoundaryPos = nextBlockStart;
            while ( 0 != bracesDisbalance ) {
                blockEnd = schemeContent.indexOf( blockEndPattern,
                    lastFoundBlockBoundaryPos + 1 );
                nextBlockStart = schemeContent.indexOf( blockStartPattern,
                    lastFoundBlockBoundaryPos + 1 );
                if ( nextBlockStart < blockEnd ) {
                    lastFoundBlockBoundaryPos = nextBlockStart;
                    ++bracesDisbalance;
                } else {
                    lastFoundBlockBoundaryPos = blockEnd;
                    --bracesDisbalance;
                }
                CHECK_EXT( 0 <= bracesDisbalance, elementNamesAndIds.clear( ), U2_INVALID_SCHEME );
            }
            elementNameStart = nameAttrStartPattern.indexIn( schemeContent, blockEnd );
            CHECK_EXT( SUBSTRING_NOT_FOUND != elementNameStart, elementNamesAndIds.clear( ),
                U2_INVALID_SCHEME );
            elementNameStart += nameAttrStartPattern.matchedLength( );
            nextBlockStart = schemeContent.indexOf( blockStartPattern,
                nextBlockStart + 1 );
            blockEnd = blockEndPattern.indexIn( schemeContent, blockEnd + 1 );
        }
        elementNameStart = schemeContent.indexOf( letterOrNumberPattern, elementNameStart );
        elementNameEnd = schemeContent.indexOf( Constants::SEMICOLON, elementNameStart );
        elementNameEnd = schemeContent.lastIndexOf( letterOrNumberPattern, elementNameEnd );

        CHECK_EXT( SUBSTRING_NOT_FOUND != elementNameStart, elementNamesAndIds.clear( ),
            U2_INVALID_SCHEME );
        CHECK_EXT( SUBSTRING_NOT_FOUND != elementNameEnd, elementNamesAndIds.clear( ),
            U2_INVALID_SCHEME );
        const QString elementName = schemeContent.mid( elementNameStart,
            elementNameEnd - elementNameStart + 1 );
        CHECK_EXT( !elementNamesAndIds.contains( elementName ), elementNamesAndIds.clear( ),
            U2_INVALID_SCHEME );
        QString elementType;
        result = getElementType( elementName, elementType );
        CHECK_EXT( U2_OK == result, elementNamesAndIds.clear( ), result );
        bool typeExists = false;
        result = WorkflowElementFacade::doesElementTypeExist( elementType, &typeExists );
        CHECK_EXT( U2_OK == result && typeExists, elementNamesAndIds.clear( ),
            U2_UNKNOWN_ELEMENT );
        elementNamesAndIds[elementName] = elementType;
        result = getEnclosingElementBoundaries( elementName, &nextBlockStart, &searchStartPosition );
        CHECK_EXT( U2_OK == result, elementNamesAndIds.clear( ), result );
        searchStartPosition = schemeContent.indexOf( blockStartPattern, searchStartPosition );
    }
    return U2_OK;
}

void SchemeWrapper::initSchemeContentWithEmptyScheme( ) {
    HRSchemaSerializer::addPart( schemeContent, Constants::HEADER_LINE );

    HRSchemaSerializer::addPart( schemeContent, HRSchemaSerializer::makeBlock(
        Constants::BODY_START, Constants::NO_NAME, QString( ), 0, true ) );
}

U2ErrorType SchemeWrapper::addActorBindingsBlock( int *position ) {
    *position = -1;
    const int schemeDescriptionEndPos = schemeContent.lastIndexOf(
        QRegExp( BLOCK_END_PATTERN ) );
    CHECK( SUBSTRING_NOT_FOUND != schemeDescriptionEndPos, U2_INVALID_SCHEME );
    const QRegExp metaStartPattern = getBlockStartPattern( Constants::META_START );
    const int metaStartPos = metaStartPattern.lastIndexIn( schemeContent,
        schemeDescriptionEndPos );
    int lastBlockEndPos = schemeContent.lastIndexOf( QRegExp( BLOCK_END_PATTERN ),
        ( SUBSTRING_NOT_FOUND == metaStartPos ) ? schemeDescriptionEndPos : metaStartPos );
    CHECK( SUBSTRING_NOT_FOUND != lastBlockEndPos, U2_INVALID_SCHEME );
    lastBlockEndPos = schemeContent.lastIndexOf( Constants::BLOCK_END, lastBlockEndPos );
    CHECK( SUBSTRING_NOT_FOUND != lastBlockEndPos, U2_INVALID_SCHEME );
    const QString dataflowDescription = Constants::NEW_LINE + Constants::NEW_LINE
        + HRSchemaSerializer::makeBlock( Constants::ACTOR_BINDINGS,
        Constants::NO_NAME, QString( ) ) + Constants::NEW_LINE;
    U2ErrorType result = insertStringToScheme( lastBlockEndPos + 1, dataflowDescription );
    CHECK( U2_OK == result, U2_INVALID_SCHEME );
    *position = schemeContent.indexOf( Constants::ACTOR_BINDINGS, lastBlockEndPos );
    return U2_OK;
}

U2ErrorType SchemeWrapper::setUniqueElementNameAndId( const QString &elementType,
    QString &elementName, QString &elementId ) const
{
    elementName = QString( );
    elementId = QString( );
    U2ErrorType result = WorkflowElementFacade::getElementNameByType( elementType, elementName );
    CHECK( U2_OK == result, result );
    elementId = elementType;
    if ( elementNamesAndIds.contains( elementName ) ) {
        elementName = WorkflowUtils::createUniqueString( elementName, " ",
            elementNamesAndIds.keys( ) );
        elementId = WorkflowUtils::createUniqueString( elementId, Constants::DASH,
            elementNamesAndIds.values( ) );
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::getElementType( const QString &elementName, QString &type ) const {
    type = QString( );
    int elementDescStartPosition = SUBSTRING_NOT_FOUND;
    int elementDescEndPosition = SUBSTRING_NOT_FOUND;
    U2ErrorType result = getEnclosingElementBoundaries( elementName, &elementDescStartPosition,
        &elementDescEndPosition );
    CHECK( U2_OK == result, result );

    const QRegExp letterOrQuotePattern( "[\\w\"]" );
    int elementTypeAttrStartPos = schemeContent.indexOf( QRegExp( TYPE_ATTR_PATTERN ),
        elementDescStartPosition );
    CHECK( SUBSTRING_NOT_FOUND != elementTypeAttrStartPos
        && elementTypeAttrStartPos < elementDescEndPosition, U2_INVALID_SCHEME );
    int outerBlockStartPos = schemeContent.lastIndexOf( QRegExp( BLOCK_START_PATTERN ),
        elementTypeAttrStartPos );
    if ( outerBlockStartPos > elementDescStartPosition ) {
        int bracesDisbalance = -1;
        while ( 0 != bracesDisbalance ) {
            result = checkBracesBalanceInRange( elementDescStartPosition, elementTypeAttrStartPos,
                &bracesDisbalance );
            CHECK( U2_OK == result, U2_INVALID_SCHEME );
            if ( 0 != bracesDisbalance ) {
                elementTypeAttrStartPos = schemeContent.indexOf( QRegExp( TYPE_ATTR_PATTERN ),
                    elementTypeAttrStartPos + 1 );
            }
        }
    }
    elementTypeAttrStartPos +=  Constants::TYPE_ATTR.length( ) + 2;
    elementTypeAttrStartPos = schemeContent.indexOf( letterOrQuotePattern,
        elementTypeAttrStartPos );
    CHECK( SUBSTRING_NOT_FOUND != elementTypeAttrStartPos
        && elementTypeAttrStartPos < elementDescEndPosition, U2_INVALID_SCHEME );

    int elementTypeAttrEndPos = schemeContent.indexOf( Constants::SEMICOLON,
        elementTypeAttrStartPos );
    CHECK( SUBSTRING_NOT_FOUND != elementTypeAttrEndPos, U2_INVALID_SCHEME );
    elementTypeAttrEndPos = schemeContent.lastIndexOf( letterOrQuotePattern,
        elementTypeAttrEndPos );
    CHECK( SUBSTRING_NOT_FOUND != elementTypeAttrEndPos, U2_INVALID_SCHEME );

    if ( Constants::QUOTE[0] == schemeContent[elementTypeAttrStartPos] ) {
        ++elementTypeAttrStartPos;
    }
    if ( Constants::QUOTE[0] != schemeContent[elementTypeAttrEndPos] ) {
        ++elementTypeAttrEndPos;
    }
    type = schemeContent.mid( elementTypeAttrStartPos,
        elementTypeAttrEndPos - elementTypeAttrStartPos );

    return U2_OK;
}

bool SchemeWrapper::validateSchemeContent( ) const {
    Workflow::Schema *scheme = new Workflow::Schema( );
    const QString conversionResult = HRSchemaSerializer::string2Schema( schemeContent, scheme );
    CHECK( Constants::NO_ERROR == conversionResult && NULL != scheme, false );
    QStringList validationErrors;
    bool result = WorkflowUtils::validate( *scheme, validationErrors );
    delete scheme;
    return result;
}

U2ErrorType SchemeWrapper::addElementWithAttributeAndGetItsName( const QString &elementType,
    const QString &attributeName, const QString &attributeValue, QString &name )
{
    const QString schemeContentBackup = schemeContent;
    U2ErrorType result = addNewElementAndGetItsName( elementType, name );
    CHECK_EXT( U2_OK == result, schemeContent = schemeContentBackup, result );
    if ( !attributeValue.isEmpty( ) ) {
        result = setElementAttribute( name, attributeName, attributeValue );
        CHECK_EXT( U2_OK == result, schemeContent = schemeContentBackup, result );
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::setElementAttributeInRange( const QString &attributeName,
    const QString &attributeValue, int start, int end, bool replaceIfExists )
{
    int valueStart = start, valueEnd = end;
    U2ErrorType result = getAttributeValuePositionFromRange( attributeName, &valueStart,
        &valueEnd );
    CHECK( U2_OK == result, result );
    if ( replaceIfExists && SUBSTRING_NOT_FOUND != valueStart && SUBSTRING_NOT_FOUND != valueEnd )
    {
        result = replaceStringInScheme( valueStart, valueEnd - valueStart,
            HRSchemaSerializer::valueString( attributeValue ) );
    } else {
        // suppose that element descriptions starts with new line in UWL format,
        // i.e. all indents are retained
        const int tabsCount = ( schemeContent.indexOf( QRegExp( "\\w" ), start ) - start )
            / Constants::TAB.length( );
        const QString attributeString = HRSchemaSerializer::makeEqualsPair( attributeName,
            attributeValue, tabsCount );
        result = insertStringToScheme( end, attributeString );
    }
    return result;
}

U2ErrorType SchemeWrapper::getElementAttributeFromRange( const QString &attributeName, int start,
    int end, QString &attributeValue )
{
    attributeValue = QString( );
    int valueStart = start, valueEnd = end;
    QStringList valuesTuple;
    do {
        U2ErrorType result = getAttributeValuePositionFromRange( attributeName, &valueStart,
            &valueEnd );
        CHECK( U2_OK == result, result );
        if ( SUBSTRING_NOT_FOUND != valueStart && SUBSTRING_NOT_FOUND != valueEnd ) {
            valuesTuple.append( schemeContent.mid( valueStart, valueEnd - valueStart )
                .remove( Constants::QUOTE ) );
        }
        valueStart = valueEnd;
        valueEnd = end;
    } while ( SUBSTRING_NOT_FOUND != valueStart && SUBSTRING_NOT_FOUND != valueEnd );
    attributeValue = valuesTuple.join( Constants::SEMICOLON );
    return U2_OK;
}

U2ErrorType SchemeWrapper::getBlockBoundaries( const QString &blockName, int *start, int *end )
    const
{
    CHECK( 0 < *start && 0 < *end && *start < *end && *start < schemeContent.length( )
        && *end < schemeContent.length( ), U2_NUM_ARG_OUT_OF_RANGE );
    QRegExp blockStartPattern = getBlockStartPattern( blockName );
    *start = blockStartPattern.indexIn( schemeContent, *start );
    if ( SUBSTRING_NOT_FOUND == *start || *start > *end ) {
        *start = SUBSTRING_NOT_FOUND;
        *end = SUBSTRING_NOT_FOUND;
        return U2_OK;
    }
    *start += blockStartPattern.matchedLength( ) - 1;
    QRegExp blockEndPattern( BLOCK_END_PATTERN );
    int blockEnd = schemeContent.indexOf( blockEndPattern, *start );
    CHECK( SUBSTRING_NOT_FOUND != blockEnd, U2_INVALID_SCHEME );
    CHECK( *end > blockEnd, U2_NUM_ARG_OUT_OF_RANGE );

    int bracesBalance = 0;
    U2ErrorType result = checkBracesBalanceInRange( *start, blockEnd, &bracesBalance );
    CHECK( U2_OK == result, U2_INVALID_CALL );
    while ( 0 != bracesBalance ) {
        CHECK( 0 > bracesBalance, U2_INVALID_SCHEME );
        while ( 0 < bracesBalance ) {
            blockEnd = schemeContent.indexOf( blockEndPattern, blockEnd + 1 );
            CHECK( SUBSTRING_NOT_FOUND != *end, U2_INVALID_SCHEME );
            CHECK( *end < blockEnd, U2_NUM_ARG_OUT_OF_RANGE );
            --bracesBalance;
        }
        result = checkBracesBalanceInRange( *start, blockEnd, &bracesBalance );
        CHECK( U2_OK == result, U2_INVALID_CALL );
    }
    *end = schemeContent.lastIndexOf( QRegExp( "[(" + Constants::BLOCK_END + ")\\"
        + Constants::SEMICOLON + "]" ), blockEnd - 1 );
    CHECK( SUBSTRING_NOT_FOUND != *end, U2_INVALID_SCHEME );
    *end += 2;
    return U2_OK;
}

U2ErrorType SchemeWrapper::validatePortAndSlot( const QString &elementName,
    const QString &portName, const QString &slotName ) const
{
    CHECK( elementNamesAndIds.contains( elementName ), U2_ELEMENT_NOT_FOUND );
    CHECK( !portName.isEmpty( ) || !slotName.isEmpty( ), U2_INVALID_STRING );

    QString elementType;
    U2ErrorType result = getElementType( elementName, elementType );
    CHECK( U2_OK == result, result );

    bool hasQuality = false;
    if ( !portName.isEmpty( ) ) {
        result = WorkflowElementFacade::doesElementHavePort( elementType, portName,
            &hasQuality );
        CHECK( U2_OK == result, result );
        CHECK( hasQuality, U2_INVALID_NAME );
    }
    if ( !slotName.isEmpty( ) ) {
        if ( !portName.isEmpty( ) ) {
            result = WorkflowElementFacade::doesElementHaveInputSlot( elementType, portName,
                slotName, &hasQuality );
            CHECK( U2_OK == result, result );
            CHECK( hasQuality, U2_INVALID_NAME );
        } else {
            result = WorkflowElementFacade::doesElementHaveOutputSlot( elementType, slotName,
                &hasQuality );
            CHECK( U2_OK == result, result );
            CHECK( hasQuality, U2_INVALID_NAME );
        }
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::getBoundariesOfUrlInAttribute( const QString &datasetName,
    bool createIfNotExists, int *start, int *end )
{
    int blockStart = *start, blockEnd = *end;
    const QString urlBlockName = Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ).getId( );
    U2ErrorType result = getBlockBoundaries( urlBlockName, &blockStart, &blockEnd );
    CHECK( U2_OK == result, U2_INVALID_CALL );
    if ( SUBSTRING_NOT_FOUND == blockStart ) {
        const QRegExp simpleAttributePattern( "[\\s\\" + Constants::SEMICOLON + "]"
            + urlBlockName + "\\s*\\" + Constants::EQUALS_SIGN );
        blockStart = schemeContent.indexOf( simpleAttributePattern, *start );
        if ( SUBSTRING_NOT_FOUND != blockStart && blockStart < blockEnd ) {
            return U2_OK;
        }
    }
    if ( SUBSTRING_NOT_FOUND == blockStart && createIfNotExists ) {
        result = insertUrlInAttributeInRange( start, end );
        return result;
    } else if ( !datasetName.isEmpty( ) ) {
        if ( SUBSTRING_NOT_FOUND == blockStart && SUBSTRING_NOT_FOUND == blockEnd ) {
            blockStart = *start, blockEnd = *end;
        }
        QString foundDatasetName;
        do {
            int datasetNameStart = blockStart, datasetNameEnd = blockEnd;
            result = getAttributeValuePositionFromRange( Constants::DATASET_NAME,
                &datasetNameStart, &datasetNameEnd );
            CHECK( U2_OK == result, result );
            CHECK( SUBSTRING_NOT_FOUND != datasetNameStart
                && SUBSTRING_NOT_FOUND != datasetNameEnd, U2_INVALID_STRING );
            foundDatasetName = schemeContent.mid( datasetNameStart,
                datasetNameEnd - datasetNameStart );
            foundDatasetName.remove( Constants::QUOTE );
            if ( foundDatasetName != datasetName ) {
                blockStart = blockEnd;
                blockEnd = *end;
                CHECK( blockStart < blockEnd, U2_INVALID_STRING );
                result = getBlockBoundaries( urlBlockName, &blockStart, &blockEnd );
                CHECK( U2_OK == result, U2_INVALID_CALL );
                CHECK( SUBSTRING_NOT_FOUND != blockStart && SUBSTRING_NOT_FOUND != blockEnd,
                    U2_INVALID_STRING );
            }
        } while ( datasetName != foundDatasetName );
    }
    *start = blockStart;
    *end = blockEnd;

    return U2_OK;
}

void SchemeWrapper::skipComments( ) {
    const int schemeHeaderStart = schemeContent.indexOf( Constants::HEADER_LINE );
    int commentStartPosition = schemeContent.lastIndexOf( Constants::SERVICE_SYM );
    while ( SUBSTRING_NOT_FOUND != commentStartPosition
        && schemeHeaderStart != commentStartPosition )
    {
        const int commentLineEnd = schemeContent.indexOf( Constants::NEW_LINE,
            commentStartPosition ) + 1;
        const int commentLength = commentLineEnd - commentStartPosition;
        const QString commentLine = schemeContent.mid( commentStartPosition, commentLength );
        commentLinesPositions[ commentStartPosition ] = commentLine;
        schemeContent.remove( commentStartPosition,  commentLength );
        commentStartPosition = schemeContent.lastIndexOf( Constants::SERVICE_SYM,
            commentStartPosition );
    }
}

void SchemeWrapper::restoreComments( ) {
    QList<int> commentPositions = commentLinesPositions.keys( );
    for ( int i = 0; i < commentPositions.size( ); ++i ) {
        const int commentPosition = commentPositions[i];
        const QString currentComment = commentLinesPositions[commentPosition];
        schemeContent.insert( commentPosition, currentComment );
        commentLinesPositions.remove( commentPosition );
    }
}

U2ErrorType SchemeWrapper::insertStringToScheme( int i, const QString &string ) {
    CHECK( 0 <= i && schemeContent.length( ) > i, U2_NUM_ARG_OUT_OF_RANGE );
    const int shift = string.length( );
    U2ErrorType result = updateCommentPositionsOnShift( i, shift );
    CHECK( U2_OK == result, result );
    schemeContent.insert( i, string );
    return U2_OK;
}

U2ErrorType SchemeWrapper::replaceStringInScheme( int i, int len, const QString &string ) {
    CHECK( 0 <= i && 0 < len && schemeContent.length( ) > i, U2_NUM_ARG_OUT_OF_RANGE );
    const int shift = string.length( ) - len;
    U2ErrorType result = updateCommentPositionsOnShift( i, shift );
    CHECK( U2_OK == result, result );
    schemeContent.replace( i, len, string );
    return U2_OK;
}

U2ErrorType SchemeWrapper::getSchemeDescriptionStart( int *pos ) const {
    QRegExp blockStartPattern( BLOCK_START_PATTERN );
    QRegExp schemeNamePattern( "\"[^\"]*\"" );
    *pos = schemeContent.indexOf( Constants::BODY_START );
    CHECK( SUBSTRING_NOT_FOUND != *pos, U2_INVALID_SCHEME );
    int bodyBlockStartPos = blockStartPattern.indexIn( schemeContent, *pos );
    CHECK( SUBSTRING_NOT_FOUND != bodyBlockStartPos, U2_INVALID_SCHEME );
    bodyBlockStartPos += 2;
    int schemeNamePos = schemeNamePattern.indexIn( schemeContent, *pos );
    if ( SUBSTRING_NOT_FOUND != schemeNamePos && bodyBlockStartPos > schemeNamePos ) {
        schemeNamePos += schemeNamePattern.matchedLength( );
        if ( bodyBlockStartPos < schemeNamePos ) {
            bodyBlockStartPos = blockStartPattern.indexIn( schemeContent, schemeNamePos ) + 2;
        }
    }
    *pos = bodyBlockStartPos;
    return U2_OK;
}

U2ErrorType SchemeWrapper::updateCommentPositionsOnShift( int shiftStartPosition, int shiftLength )
{
    CHECK( 0 <= shiftStartPosition && 0 <= shiftStartPosition + shiftLength
        && schemeContent.length( ) > shiftStartPosition, U2_NUM_ARG_OUT_OF_RANGE );
    const QList<int> commentPositions = commentLinesPositions.keys( );
    int totalPreviousCommentsLength = 0;
    for ( int commentCount = 0; commentCount < commentPositions.size( ); ++commentCount ) {
        const int commentPosition = commentPositions[commentCount];
        const QString commentLine = commentLinesPositions[commentPosition];
        if ( shiftStartPosition <= commentPosition - totalPreviousCommentsLength ) {
            commentLinesPositions.remove( commentPosition );
            commentLinesPositions[shiftLength + commentPosition] = commentLine;
        }
        totalPreviousCommentsLength += commentLine.length( );
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::getAttributeValuePositionFromRange( const QString &attributeName,
    int *start, int *end ) const
{
    CHECK( 0 < *start && 0 < *end && *start < *end && *start < schemeContent.length( )
        && *end < schemeContent.length( ), U2_NUM_ARG_OUT_OF_RANGE );
    const QRegExp letterOrNumberPattern( "\\w" );
    const QRegExp attributeNamePattern = QRegExp( "[\\s\\" + Constants::SEMICOLON + "]"
        + attributeName + "\\s*\\" + Constants::EQUALS_SIGN );
    const int attributePosition = attributeNamePattern.indexIn( schemeContent, *start );
    if ( SUBSTRING_NOT_FOUND != attributePosition && attributePosition < *end ) {
        int valueStartPosition = attributePosition + attributeNamePattern.matchedLength( );
        const int firstValueSymbolPos = schemeContent.indexOf( letterOrNumberPattern,
            valueStartPosition );
        const int firstOpenQuotePos = schemeContent.indexOf( Constants::QUOTE,
            valueStartPosition );
        const bool quotesExist = ( SUBSTRING_NOT_FOUND != firstOpenQuotePos
            && firstOpenQuotePos < firstValueSymbolPos );
        valueStartPosition = quotesExist ? firstOpenQuotePos : firstValueSymbolPos;
        int valueEndPosition = quotesExist ? schemeContent.indexOf( Constants::QUOTE,
            firstOpenQuotePos + 1 ) + 1 : schemeContent.indexOf( Constants::SEMICOLON,
            valueStartPosition );
        CHECK( SUBSTRING_NOT_FOUND != valueEndPosition, U2_INVALID_SCHEME );
        *start = valueStartPosition;
        *end = valueEndPosition;
    } else {
        *start = SUBSTRING_NOT_FOUND;
        *end = SUBSTRING_NOT_FOUND;
    }
    return U2_OK;
}

U2ErrorType SchemeWrapper::getUrlInAttributePositionByName( const QStringList &attributesHierarchy,
    bool createIfNotExists, int *start, int *end, QString &deepestAttributeName,
    bool &replaceIfExists )
{
    replaceIfExists = false;
    CHECK( 0 < *start && 0 < *end && *start < *end && *start < schemeContent.length( )
        && *end < schemeContent.length( ), U2_NUM_ARG_OUT_OF_RANGE );
    U2ErrorType result = U2_OK;
    CHECK( Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ) == attributesHierarchy[0],
        U2_INVALID_CALL );
    deepestAttributeName = ( 1 < attributesHierarchy.size( ) ) ? attributesHierarchy.last( )
        : Constants::FILE_URL ;
    CHECK( Constants::FILE_URL == deepestAttributeName
        || Constants::DATASET_NAME == deepestAttributeName
        || Constants::DIRECTORY_URL == deepestAttributeName, U2_INVALID_NAME );
    QString datasetName;
    if ( 2 < attributesHierarchy.size( ) ) {
        QStringList datasetNameParts = attributesHierarchy;
        datasetNameParts.removeAt( 0 );
        datasetNameParts.removeAt( datasetNameParts.length( ) - 1 );
        datasetName = datasetNameParts.join( Constants::DOT );
    }
    if ( Constants::DATASET_NAME == deepestAttributeName && createIfNotExists ) {
        result = insertUrlInAttributeInRange( start, end );
        replaceIfExists = true;
    } else {
        int urlAttrStart = *start, urlAttrEnd = *end;
        result = getBoundariesOfUrlInAttribute( datasetName, createIfNotExists, &urlAttrStart,
            &urlAttrEnd );
        CHECK( U2_OK == result, result );
        // that means that old style of in-url attribute is used in scheme
        if ( urlAttrStart == *start && urlAttrEnd == *end ) {
            deepestAttributeName = Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ).getId( );
        }
        *start = urlAttrStart;
        *end = urlAttrEnd;
    }
    return result;
}

U2ErrorType SchemeWrapper::insertUrlInAttributeInRange( int *start, int *end ) {
    CHECK( 0 < *start && 0 < *end && *start < *end && *start < schemeContent.length( )
        && *end < schemeContent.length( ), U2_NUM_ARG_OUT_OF_RANGE );
    const QString urlBlockName = Workflow::BaseAttributes::URL_IN_ATTRIBUTE( ).getId( );
    QString blockContent( HRSchemaSerializer::makeEqualsPair( Constants::DATASET_NAME,
        DEFAULT_DATASET_NAME, 3 ) );
    QString newBlock = HRSchemaSerializer::makeBlock( urlBlockName,
        Constants::NO_NAME, blockContent, 2 );
    U2ErrorType result = insertStringToScheme( *end, newBlock );
    CHECK( U2_OK == result, result );
    *start = *end;
    *start = schemeContent.indexOf( Constants::NEW_LINE,
        *start );
    *end += newBlock.length( );
    *end = schemeContent.lastIndexOf( QRegExp( BLOCK_END_PATTERN ), *end );
    *end = schemeContent.lastIndexOf( Constants::SEMICOLON, *end ) + 2;
    return U2_OK;
}

QRegExp SchemeWrapper::getBlockStartPattern( const QString &blockName ) {
    return QRegExp( "\\s" + QRegExp::escape( blockName ) + "\\s*"
        + QRegExp::escape( Constants::BLOCK_START ) );
}

} // namespace U2
