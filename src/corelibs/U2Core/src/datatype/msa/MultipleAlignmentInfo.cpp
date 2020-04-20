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

#include "MultipleAlignmentInfo.h"

namespace U2 {

const QString MultipleAlignmentInfo::NAME                  = "NAME";
const QString MultipleAlignmentInfo::ACCESSION             = "ACCESSION";
const QString MultipleAlignmentInfo::DESCRIPTION           = "DESCRIPTION";
const QString MultipleAlignmentInfo::SS_CONSENSUS          = "SS_CONSENSUS";
const QString MultipleAlignmentInfo::REFERENCE_LINE        = "REFERENCE_LINE";
const QString MultipleAlignmentInfo::CUTOFFS               = "CUTOFFS";

bool MultipleAlignmentInfo::isValid( const QVariantMap& map ) {
    return hasName( map );
}

static QVariant getValue( const QString& tag, const QVariantMap& map ) {
    return map.value( tag );
}

static void setValue( QVariantMap& map, const QString& tag, const QVariant& val ) {
    assert( !tag.isEmpty() );
    if( !val.isNull() ) {
        map.insert( tag, val );
    }
}

QString MultipleAlignmentInfo::getName( const QVariantMap& map ) {
    return getValue( NAME, map ).toString();
}

void MultipleAlignmentInfo::setName( QVariantMap& map, const QString& name ) {
    setValue( map, NAME, name );
}

bool MultipleAlignmentInfo::hasName( const QVariantMap& map ) {
    return !getName( map ).isEmpty();
}

QString MultipleAlignmentInfo::getAccession( const QVariantMap& map ) {
    return getValue( ACCESSION, map ).toString();
}

void MultipleAlignmentInfo::setAccession( QVariantMap& map, const QString& acc ) {
    setValue( map, ACCESSION, acc );
}

bool MultipleAlignmentInfo::hasAccession( const QVariantMap& map ) {
    return !getAccession( map ).isEmpty();
}

QString MultipleAlignmentInfo::getDescription( const QVariantMap& map ) {
    return getValue( DESCRIPTION, map ).toString();
}

void MultipleAlignmentInfo::setDescription( QVariantMap& map, const QString& desc ) {
    setValue( map, DESCRIPTION, desc );
}

bool MultipleAlignmentInfo::hasDescription( const QVariantMap& map ) {
    return !getDescription( map ).isEmpty();
}

QString MultipleAlignmentInfo::getSSConsensus( const QVariantMap& map ) {
    return getValue( SS_CONSENSUS, map ).toString();
}

void MultipleAlignmentInfo::setSSConsensus( QVariantMap& map, const QString& cs ) {
    setValue( map, SS_CONSENSUS, cs );
}

bool MultipleAlignmentInfo::hasSSConsensus( const QVariantMap& map ) {
    return !getSSConsensus( map ).isEmpty();
}

QString MultipleAlignmentInfo::getReferenceLine( const QVariantMap& map ) {
    return getValue( REFERENCE_LINE, map ).toString();
}

void MultipleAlignmentInfo::setReferenceLine( QVariantMap& map ,const QString& rf ) {
    setValue( map, REFERENCE_LINE, rf );
}

bool MultipleAlignmentInfo::hasReferenceLine( const QVariantMap& map ) {
    return !getReferenceLine( map ).isEmpty();
}

void MultipleAlignmentInfo::setCutoff( QVariantMap& map, Cutoffs coff, float val ) {
    setValue( map, CUTOFFS + QString::number( static_cast< int >( coff ) ), val );
}

float MultipleAlignmentInfo::getCutoff( const QVariantMap& map, Cutoffs coff ) {
    return static_cast< float >( getValue( CUTOFFS + QString::number( static_cast< int >( coff ) ), map ).toDouble() );
}

bool MultipleAlignmentInfo::hasCutoff( const QVariantMap& map, Cutoffs coff ) {
    bool ok = false;
    getValue( CUTOFFS + QString::number( static_cast< int >( coff ) ), map ).toDouble( &ok );
    return ok;
}

}   // namespace U2
