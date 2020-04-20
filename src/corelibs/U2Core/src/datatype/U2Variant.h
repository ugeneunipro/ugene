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

#ifndef _U2_VARIANT_H_
#define _U2_VARIANT_H_

#include <U2Core/StrPackUtils.h>
#include <U2Core/U2Type.h>

namespace U2 {

/**
    Representation for set of genomic variations.
*/

enum VariantTrackType {
    TrackType_All           = 1,
    TrackType_Perspective   = 2,
    TrackType_Discarded     = 3,
    TrackType_UnknownEffect = 4,

    // To check that int can be casted to the enum
    TrackType_FIRST         = TrackType_All,
    TrackType_LAST          = TrackType_UnknownEffect
};

class U2CORE_EXPORT U2VariantTrack : public U2Object {
public:
    U2VariantTrack();
    U2VariantTrack(const U2DataId &id, const QString &dbId, VariantTrackType trackType, qint64 version);

    /** Sequence id */
    U2DataId      sequence;

    /** Sequence name */
    QString     sequenceName;

    /** Track Type*/
    VariantTrackType trackType;

    /** File header */
    QString     fileHeader;

    U2DataType getType() const;

    static const QString META_INFO_ATTIBUTE;
    static const QString HEADER_ATTIBUTE;
};

/** Database representation of genomic variations such as snps, indels, etc.  */
class U2CORE_EXPORT U2Variant : public U2Entity {
public:
    U2Variant();

    qint64      startPos;
    qint64      endPos;
    QByteArray  refData;
    QByteArray  obsData;
    QString     publicId;
    StrStrMap   additionalInfo;

    static const QString VCF4_QUAL;
    static const QString VCF4_FILTER;
    static const QString VCF4_INFO;
};

}   // namespace U2

#endif // _U2_VARIANT_H_
