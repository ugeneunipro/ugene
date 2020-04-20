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

#ifndef _U2_VARIANT_DBI_H_
#define _U2_VARIANT_DBI_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Variant.h>

namespace U2 {

/**
    An interface to access to Variant tracks
*/
class U2VariantDbi : public U2ChildDbi {
protected:
    U2VariantDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi){}

public:

    /** Returns available VariantTracks */
    virtual U2DbiIterator<U2VariantTrack>* getVariantTracks(VariantTrackType trackType, U2OpStatus& os) = 0;

    /** Returns available VariantTracks for given sequence id*/
    virtual U2DbiIterator<U2VariantTrack>* getVariantTracks(const U2DataId& seqId, U2OpStatus& os) = 0;

    /** Returns available VariantTracks for given sequence id and type
    if type == TrackType_All returns all tracks of given sequence */
    virtual U2DbiIterator<U2VariantTrack>* getVariantTracks(const U2DataId& seqId, VariantTrackType trackType, U2OpStatus& os) = 0;


    /** Returns VariantTrack instance by the given track id */
    virtual U2VariantTrack getVariantTrack(const U2DataId& id, U2OpStatus& os) = 0;

    /** Returns VariantTrack instance by the given variant id */
    virtual U2VariantTrack getVariantTrackofVariant(const U2DataId& variantId, U2OpStatus& os) = 0;

    /**
        Adds all Variants provided by iterator to the track
        Requires U2DbiFeature_WriteVariants feature support
    */
    virtual void addVariantsToTrack(const U2VariantTrack& track, U2DbiIterator<U2Variant>* it, U2OpStatus& os) = 0;
     /**
        Creates new VariantTrack instance
        Requires U2DbiFeature_WriteVariants feature support
    */
    virtual void createVariantTrack(U2VariantTrack& track, VariantTrackType trackType, const QString& folder, U2OpStatus& os) = 0;

    /**
        Creates new index for variations.
    */
    virtual void createVariationsIndex(U2OpStatus& os) = 0;

    /**
        Updates VariantTrack instance
        Requires U2DbiFeature_WriteVariants feature support
    */
    virtual void updateVariantTrack(U2VariantTrack& track, U2OpStatus& os) = 0;

    /** Returns all Variants from the given sequence region
    U2_REGION_MAX to get all variants*/
    virtual U2DbiIterator<U2Variant>* getVariants(const U2DataId& track, const U2Region& region, U2OpStatus& os) = 0;

    /** Returns a selection of variants of the track starting from the offset */
    virtual U2DbiIterator<U2Variant>* getVariantsRange(const U2DataId& track, int offset, int limit, U2OpStatus& os ) = 0;

    /** Return number of variants in track */
    virtual int getVariantCount(const U2DataId& track, U2OpStatus& os) = 0;

    /** Delete the variant track from the database */
    virtual void removeTrack(const U2DataId& track, U2OpStatus& os) = 0;

    /**Update variant public ID*/
    virtual void updateVariantPublicId(const U2DataId& track, const U2DataId& variant, const QString& newId, U2OpStatus& os) = 0;

    /**Update variant track ID*/
    virtual void updateTrackIDofVariant(const U2DataId& variant, const U2DataId& newTrackId, U2OpStatus& os) = 0;

};


} //namespace

#endif
