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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>

#include "FormatDetection.h"

static U2Format fromFormatId( const U2::DocumentFormatId &id ) {
    if ( U2::BaseDocumentFormats::ABIF == id ) {
        return U2_ABI;
    } else if ( U2::BaseDocumentFormats::ACE == id ) {
        return U2_ACE;
    } else if ( U2::BaseDocumentFormats::CLUSTAL_ALN == id ) {
        return U2_CLUSTAL;
    } else if ( U2::BaseDocumentFormats::PLAIN_EMBL == id ) {
        return U2_EMBL;
    } else if ( U2::BaseDocumentFormats::FASTA == id ) {
        return U2_FASTA;
    } else if ( U2::BaseDocumentFormats::FASTQ == id ) {
        return U2_FASTQ;
    } else if ( U2::BaseDocumentFormats::PLAIN_GENBANK == id ) {
        return U2_GENBANK;
    } else if ( U2::BaseDocumentFormats::MEGA == id ) {
        return U2_MEGA;
    } else if ( U2::BaseDocumentFormats::MSF == id ) {
        return U2_MSF;
    } else if ( U2::BaseDocumentFormats::NEXUS == id ) {
        return U2_NEXUS;
    } else if ( U2::BaseDocumentFormats::PLAIN_TEXT == id ) {
        return U2_PLAIN_TEXT;
    } else if ( U2::BaseDocumentFormats::STOCKHOLM == id ) {
        return U2_STOCKHOLM;
    } else if ( U2::BaseDocumentFormats::PLAIN_SWISS_PROT == id ) {
        return U2_SWISS;
    } else {
        return U2_UNSUPPORTED;
    }
}

extern "C" {

U2SCRIPT_EXPORT U2ErrorType detectFileFormat( const wchar_t *_pathToFile, U2Format *format ) {
    const QString pathToFile = QString::fromWCharArray( _pathToFile );
    QFileInfo info( pathToFile );
    if ( NULL == pathToFile || !info.isFile( ) || !info.exists( ) ) {
        return U2_INVALID_PATH;
    }
    *format = U2_UNSUPPORTED;
    QList<U2::FormatDetectionResult> formatList = U2::DocumentUtils::detectFormat(
        U2::GUrl( pathToFile ) );
    if ( formatList.isEmpty( ) ) {
        return U2_OK;
    }
    *format = fromFormatId( formatList.first( ).format->getFormatId( ) );
    return U2_OK;
}

};
