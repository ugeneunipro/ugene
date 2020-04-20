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

#include <QCoreApplication>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Type.h>
#include <U2Core/U2OpStatusUtils.h>

#include "TextConversionUtils.h"
#include "CommonDbi.h"

static U2::DocumentFormatId toDocumentFormatId( FileFormat format ) {
    U2::DocumentFormatId result;
    switch ( format ) {
    case CLUSTAL_ALN :
        result = U2::BaseDocumentFormats::CLUSTAL_ALN;
        break;
    case FASTA :
        result = U2::BaseDocumentFormats::FASTA;
        break;
    case FASTQ :
        result = U2::BaseDocumentFormats::FASTQ;
        break;
    case GENBANK :
        result = U2::BaseDocumentFormats::PLAIN_GENBANK;
        break;
    case PLAIN_TEXT :
        result = U2::BaseDocumentFormats::PLAIN_TEXT;
        break;
    case RAW :
        result = U2::BaseDocumentFormats::RAW_DNA_SEQUENCE;
        break;
    }
    return result;
}

static ObjectType toObjectType( U2::GObjectType type ) {
    ObjectType result = UNSUPPORTED;
    if ( U2::GObjectTypes::SEQUENCE == type ) {
        result = SEQUENCE;
    } else if ( U2::GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT == type ) {
        result = MSA;
    }
    return result;
}

extern "C" {

U2SCRIPT_EXPORT void importFileToUgeneDb( const wchar_t *_url, UgeneDbHandle *resultObjects, int maxObjectCount )
{
    using namespace U2;

    const QString url = QString::fromWCharArray( _url );
    QFileInfo fileInfo( url );
    if ( !fileInfo.exists( ) ) {
        coreLog.error( QObject::tr( "File \"%1\" does not exist" ).arg( url ) );
        return;
    }
    const GUrl adoptedUrl( url );
    QList<DocumentFormat *> formatList = DocumentUtils::toFormats(
        DocumentUtils::detectFormat( adoptedUrl ) );
    CHECK_EXT( !formatList.isEmpty( ), coreLog.error( QObject::tr( "Could not detect file format" ) ),
        );
    DocumentFormat *format = formatList.first( );

    ioLog.info( QObject::tr( "Importing objects from %1 [%2]" ).arg( url,
        format->getFormatName( ) ) );
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry( )->getIOAdapterFactoryById(
        IOAdapterUtils::url2io( adoptedUrl ) );
    QVariantMap hints;
    U2OpStatusImpl stateInfo;
    QScopedPointer<Document> doc( format->loadDocument( iof, adoptedUrl, hints, stateInfo ) );
    CHECK_OP( stateInfo, );
    doc->setDocumentOwnsDbiResources(false);

    int objectCount = 0;
    foreach ( GObject* object, doc->getObjects( ) ) {
        resultObjects[objectCount] = object;
        if ( ++objectCount >= maxObjectCount ) {
            break;
        }
    }
}

U2SCRIPT_EXPORT void releaseObject( UgeneDbHandle resultObject ) {
    U2::GObject *unwrappedObject = reinterpret_cast<U2::GObject *>( resultObject );
    delete unwrappedObject; // TODO: make sure about db resources deallocation
}

U2SCRIPT_EXPORT UgeneDbHandle cloneObject( UgeneDbHandle object ) {
    U2::GObject *unwrappedObject = reinterpret_cast<U2::GObject *>( object );
    U2::GObject *result = NULL;
    if ( NULL != unwrappedObject ) {
        U2::U2OpStatusImpl statusInfo;
        result = unwrappedObject->clone( unwrappedObject->getEntityRef( ).dbiRef, statusInfo );
        CHECK_OP( statusInfo, NULL );
    }
    return result;
}

U2SCRIPT_EXPORT void saveObjectsToFile( UgeneDbHandle *objects, int objectCount, const wchar_t *_url,
    FileFormat format )
{
    using namespace U2;

    CHECK( NULL != objects && NULL != _url, );

    DocumentFormat *docFormat = AppContext::getDocumentFormatRegistry()->getFormatById(
        toDocumentFormatId( format ) );
    CHECK_EXT( NULL != docFormat, coreLog.error( QObject::tr( "The unsupported format"
        " was provided" ) ), );
    const QString url = QString::fromWCharArray( _url );
    QFileInfo fileInfo( url );
    GUrl adoptedUrl = fileInfo.isAbsolute( ) ? GUrl( url ) : GUrl(
        QCoreApplication::applicationDirPath( ) + "/" + QString( url ) );
    U2OpStatusImpl stateInfo;
    adoptedUrl =  GUrlUtils::prepareFileLocation( adoptedUrl.getURLString( ), stateInfo );
    CHECK_OP_EXT( stateInfo, coreLog.error( QString( "Could not prepare folder"
        " according to supplied path \"%1\"" ).arg( url ) ), );
    Document *doc = docFormat->createNewLoadedDocument( IOAdapterUtils::get(
        BaseIOAdapters::LOCAL_FILE ), adoptedUrl, stateInfo );
    for ( int i = 0; i < objectCount; ++i ) {
        GObject *object = reinterpret_cast<GObject *>( objects[i] );
        if ( NULL != object ) {
            doc->addObject( object );
        }
    }
    Task *saveDoc = new SaveDocumentTask( doc, SaveDoc_Overwrite );
    AppContext::getTaskScheduler( )->registerTopLevelTask( saveDoc );
}

U2SCRIPT_EXPORT ObjectType getObjectType( UgeneDbHandle object ) {
    U2::GObject *unwrappedObject = reinterpret_cast<U2::GObject *>( object );
    U2::GObjectType result = U2::GObjectTypes::UNKNOWN;
    if ( NULL != unwrappedObject ) {
        result = unwrappedObject->getGObjectType( );
    }
    return toObjectType( result );
}

U2SCRIPT_EXPORT void getObjectName( UgeneDbHandle object, int expectedMaxNameLength, wchar_t *name ) {
    U2::GObject *unwrappedObject = reinterpret_cast<U2::GObject *>( object );
    QString result;
    if ( NULL != unwrappedObject ) {
        result = unwrappedObject->getGObjectName( );
    }
    U2ErrorType error = U2::TextConversionUtils::qstringToCstring( result, expectedMaxNameLength,
        name );
    Q_UNUSED( error );
}

U2SCRIPT_EXPORT void setObjectName( UgeneDbHandle object, const wchar_t *newName ) {
    U2::GObject *unwrappedObject = reinterpret_cast<U2::GObject *>( object );
    if ( NULL != unwrappedObject && NULL != newName ) {
        unwrappedObject->setGObjectName( QString::fromWCharArray( newName ) );
    }
}

};
