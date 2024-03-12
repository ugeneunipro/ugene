/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/U2VariantDbi.h>

#include "SQLiteDbi.h"

namespace U2 {

class SQLiteVariantDbi : public U2VariantDbi, public SQLiteChildDBICommon {
public:
    SQLiteVariantDbi(SQLiteDbi* dbi);

    U2DbiIterator<U2VariantTrack>* getVariantTracks(VariantTrackType trackType, U2OpStatus& os) override;

    U2DbiIterator<U2VariantTrack>* getVariantTracks(const U2DataId& seqId, U2OpStatus& os) override;

    U2DbiIterator<U2VariantTrack>* getVariantTracks(const U2DataId& seqId, VariantTrackType trackType, U2OpStatus& os) override;

    /** Returns VariantTrack instance by the given track id */
    U2VariantTrack getVariantTrack(const U2DataId& trackId, U2OpStatus& os) override;

    /** Returns VariantTrack instance by the given variant id */
    U2VariantTrack getVariantTrackofVariant(const U2DataId& variantId, U2OpStatus& os) override;

    /**
        Create Variant and add it to the track
        Requires U2DbiFeature_WriteVariants feature support
    */
    void addVariantsToTrack(const U2VariantTrack& track, U2DbiIterator<U2Variant>* it, U2OpStatus& os) override;

    /**
        Creates new index for variations.
    */
    void createVariationsIndex(U2OpStatus& os) override;

    /**
        Creates new VariantTrack instance
        Requires U2DbiFeature_WriteVariants feature support
    */
    void createVariantTrack(U2VariantTrack& track, VariantTrackType trackType, const QString& folder, U2OpStatus& os) override;

    /**
        Updates VariantTrack instance
        Requires U2DbiFeature_WriteVariants feature support
    */
    void updateVariantTrack(U2VariantTrack& track, U2OpStatus& os) override;

    /** Returns all Variants from the given region
    U2_REGION_MAX to get all variants*/
    U2DbiIterator<U2Variant>* getVariants(const U2DataId& track, const U2Region& region, U2OpStatus& os) override;

    // TODO ADD ID
    U2DbiIterator<U2Variant>* getVariantsRange(const U2DataId& track, int offset, int limit, U2OpStatus& os) override;

    /** Return number of variants in track */
    int getVariantCount(const U2DataId& track, U2OpStatus& os) override;

    void initSqlSchema(U2OpStatus& os) override;

    /** Delete the variant track from the database */
    void removeTrack(const U2DataId& track, U2OpStatus& os) override;

    /**Update variant public ID*/
    void updateVariantPublicId(const U2DataId& track, const U2DataId& variant, const QString& newId, U2OpStatus& os) override;

    /**Update variant track ID*/
    void updateTrackIDofVariant(const U2DataId& variant, const U2DataId& newTrackId, U2OpStatus& os) override;
};

}  // namespace U2
