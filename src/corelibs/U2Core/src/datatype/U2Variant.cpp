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

#include "U2Variant.h"

namespace U2 {

const QString U2VariantTrack::META_INFO_ATTIBUTE = "meta-info";
const QString U2VariantTrack::HEADER_ATTIBUTE = "header";

U2VariantTrack::U2VariantTrack()
    : trackType(TrackType_All)
{

}

U2VariantTrack::U2VariantTrack(const U2DataId &id, const QString &dbId, VariantTrackType trackType, qint64 version)
    : U2Object(id, dbId, version),
      trackType(trackType)
{

}

U2DataType U2VariantTrack::getType() const {
    return U2Type::VariantTrack;
}

const QString U2Variant::VCF4_QUAL = "QUAL";
const QString U2Variant::VCF4_FILTER = "FILTER";
const QString U2Variant::VCF4_INFO = "INFO";

U2Variant::U2Variant()
    : startPos(0),
      endPos(0)
{

}

}   // namespace U2
