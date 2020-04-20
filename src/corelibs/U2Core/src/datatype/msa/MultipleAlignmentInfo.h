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

#ifndef _U2_MULTIPLE_ALIGNMENT_INFO_H_
#define _U2_MULTIPLE_ALIGNMENT_INFO_H_

#include <QString>
#include <QVariantMap>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT MultipleAlignmentInfo {
public:
    static const QString NAME;
    static const QString ACCESSION;
    static const QString DESCRIPTION;
    static const QString SS_CONSENSUS;          /* Secondary structure consensus */
    static const QString REFERENCE_LINE;        /* #=GC RF annotation in Stockholm markup */
    static const QString CUTOFFS;               /* #=GF GA, NC, TC annotation in Stockholm markup */

    enum Cutoffs { /* Should be same as in esl_msa.h( uhmmer plugin ) */
        CUTOFF_TC1,
        CUTOFF_TC2,
        CUTOFF_GA1,
        CUTOFF_GA2,
        CUTOFF_NC1,
        CUTOFF_NC2
    };

    static bool     isValid( const QVariantMap& map ); /* Checks obligatory fields in info */

    static QString  getName( const QVariantMap& map );
    static void     setName( QVariantMap& map, const QString& name );
    static bool     hasName( const QVariantMap& map );

    static QString  getAccession( const QVariantMap& map );
    static void     setAccession( QVariantMap& map, const QString& acc );
    static bool     hasAccession( const QVariantMap& map );

    static QString  getDescription( const QVariantMap& map );
    static void     setDescription( QVariantMap& map, const QString& desc );
    static bool     hasDescription( const QVariantMap& map );

    static QString  getSSConsensus( const QVariantMap& map );
    static void     setSSConsensus( QVariantMap& map, const QString& cs );
    static bool     hasSSConsensus( const QVariantMap& map );

    static QString  getReferenceLine( const QVariantMap& map );
    static void     setReferenceLine( QVariantMap& map ,const QString& rf );
    static bool     hasReferenceLine( const QVariantMap& map );

    static float    getCutoff( const QVariantMap& map, Cutoffs coff );
    static bool     hasCutoff( const QVariantMap& map, Cutoffs coff );
    static void     setCutoff( QVariantMap& map, Cutoffs coff, float val );
};

}   // namespace U2

#endif // _U2_MULTIPLE_ALIGNMENT_INFO_H_
