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

#ifndef _SCHEME_WRAPPER_H_
#define _SCHEME_WRAPPER_H_

#include <QMap>
#include <QString>

#include "globals.h"

namespace U2 {

class SchemeWrapper {
public:
                            SchemeWrapper( const QString &initPathToScheme, U2ErrorType *result );
                            ~SchemeWrapper( );

    U2ErrorType             addReaderAndGetItsName( const QString &readerType,
                                const QString &inputFilePath, QString &name );
    U2ErrorType             addWriterAndGetItsName( const QString &writerType,
                                const QString &outputFilePath, QString &name );
    U2ErrorType             addNewElementAndGetItsName( const QString &elementType,
                                QString &elementName );
    U2ErrorType             setElementAttribute( const QString &elementName,
                                const QString &attributeName, const QString &attributeValue );
    U2ErrorType             getElementAttribute( const QString &elementName,
                                const QString &attributeName, QString &attributeValue );
    U2ErrorType             addFlow( const QString &srcElementName, const QString &srcPortName,
                                const QString &dstElementName, const QString &dstPortName );
    // `dstPortAndSlotNames` should contain port name and slot name separated by dot
    U2ErrorType             addActorsBinding( const QString &srcElementName,
                                const QString &srcSlotName, const QString &dstElementName,
                                const QString &dstPortAndSlotNames );
    // if `path` is empty string after invocation it will contain the path to temporary file with scheme
    // if saving did not succeed `path` will be empty after invocation
    U2ErrorType             saveToFile( QString &path );

    // SAS stands for "Single Algorithm Scheme"
    static U2ErrorType      createSas( const QString &elementType, const QString &inputFilePath,
                                const QString &outputFilePath, SchemeWrapper **sas );

private:
    U2ErrorType             getEnclosingElementBoundaries( const QString &elementName, int *start,
                                int *end ) const;
    U2ErrorType             getElementNameAttributePosition( const QString &elementName,
                                int *position ) const;
    U2ErrorType             fillElementNamesFromSchemeContent( );
    void                    initSchemeContentWithEmptyScheme( );
    U2ErrorType             setUniqueElementNameAndId( const QString &elementType,
                                QString &elementName, QString &elementId ) const;
    U2ErrorType             getElementType( const QString &elementName, QString &type ) const;
    bool                    validateSchemeContent( ) const;
    // `*disbalance` signals how many unbalanced open braces detected in the range if `*disbalance > 0`
    // the same for close braces if `*disbalance < 0`
    // if `0 == *disbalance` then all braces are balanced in the range
    U2ErrorType             checkBracesBalanceInRange( int startPos, int endPos, int *disbalance )
                                const;
    U2ErrorType             addElementWithAttributeAndGetItsName( const QString &elementType,
                                const QString &attributeName, const QString &attributeValue,
                                QString &name );
    U2ErrorType             setElementAttributeInRange( const QString &attributeName,
                                const QString &attributeValue, int start, int end,
                                bool replaceIfExists = true );
    U2ErrorType             getElementAttributeFromRange( const QString &attributeName, int start,
                                int end, QString &attributeValue );
    // `*start` and `*end` have to contain the boundaries of search. They are also containing output values
    U2ErrorType             getBlockBoundaries( const QString &blockName, int *start, int *end )
                                const;
    // If `portName` is empty `slotName` is treated as output slot,
    // else `slotName` is considered as input slot of the `portName` port.
    // If `slotName` is empty only `portName` is validated
    U2ErrorType             validatePortAndSlot( const QString &elementName,
                                const QString &portName, const QString &slotName ) const;
    // `*start` and `*end` should contain read-element boundaries before invocation.
    // these values contain result boundaries after invocation.
    // if "url-in" attribute doesn't exist an appropriate block is created inside the given boundaries
    // if `datasetName` is non-empty string then the attribute with such name will be searched
    U2ErrorType             getBoundariesOfUrlInAttribute( const QString &datasetName,
                                bool createIfNotExists, int *start, int *end );
    void                    skipComments( );
    // besides clears comments cache
    void                    restoreComments( );
    // the method is supposed to be used for all scheme content modifications,
    // the reason is to keep comment positions actual
    U2ErrorType             insertStringToScheme( int i, const QString &string );
    U2ErrorType             replaceStringInScheme( int i, int len, const QString &string );
    U2ErrorType             updateCommentPositionsOnShift( int shiftStartPosition,
                                int shiftLength );
    U2ErrorType             getSchemeDescriptionStart( int *pos ) const;
    // `*start` and `*end` have to contain the range of search. They are also represent output values
    U2ErrorType             getAttributeValuePositionFromRange( const QString &attributeName,
                                int *start, int *end ) const;
    U2ErrorType             getUrlInAttributePositionByName(
                                const QStringList &attributesHierarchy, bool createIfNotExists,
                                int *start, int *end, QString &deepestAttributeName,
                                bool &replaceIfExists );
    U2ErrorType             insertUrlInAttributeInRange( int *start, int *end );
    U2ErrorType             addActorBindingsBlock( int *position );
    static QRegExp          getBlockStartPattern( const QString &blockName );

    QString                 pathToScheme;
    QString                 schemeContent;
    QMap<QString, QString>  elementNamesAndIds;
    QMap<int, QString>      commentLinesPositions;
};

} // namespace U2

#endif // _SCHEME_WRAPPER_H_
