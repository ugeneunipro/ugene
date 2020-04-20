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

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2VariantDbi.h>

#include "GObjectTypes.h"
#include "VariantTrackObject.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//VariantTrackObject

VariantTrackObject::VariantTrackObject( const QString& objectName, const U2EntityRef& trackRef, const QVariantMap& hintsMap )
: GObject(GObjectTypes::VARIANT_TRACK, objectName, hintsMap) {

    entityRef = trackRef;
}

VariantTrackObject::~VariantTrackObject(){

}

U2DbiIterator<U2Variant>* VariantTrackObject::getVariants( const U2Region& reg, U2OpStatus& os ) const {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, NULL);

    U2VariantDbi* vdbi = con.dbi->getVariantDbi();
    SAFE_POINT(vdbi != NULL, "Variant DBI is NULL", NULL);

    return vdbi->getVariants(entityRef.entityId, reg, os);
}

int VariantTrackObject::getVariantCount(U2OpStatus &os) const {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, 0);

    U2VariantDbi *variantDbi = con.dbi->getVariantDbi();
    SAFE_POINT(variantDbi != NULL, "Variant DBI is NULL", 0);

    return variantDbi->getVariantCount(entityRef.entityId, os);
}

U2VariantTrack VariantTrackObject::getVariantTrack(U2OpStatus &os) const {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, U2VariantTrack());

    U2VariantDbi* vdbi = con.dbi->getVariantDbi();
    SAFE_POINT(vdbi != NULL, "Variant DBI is NULL", U2VariantTrack());

    return vdbi->getVariantTrack(entityRef.entityId, os);
}

GObject* VariantTrackObject::clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints) const{
    DbiOperationsBlock opBlock(dstDbiRef, os);
    Q_UNUSED(opBlock);
    CHECK_OP(os, NULL);

    DbiConnection srcCon(entityRef.dbiRef, true, os);
    CHECK_OP(os, NULL);
    DbiConnection dstCon(dstDbiRef, true, os);
    CHECK_OP(os, NULL);

    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);
    const QString dstFolder = gHints.get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    U2VariantDbi *dstVDbi = dstCon.dbi->getVariantDbi();
    SAFE_POINT(dstVDbi != NULL, "NULL destination variant DBI", NULL);

    U2VariantTrack track = this->getVariantTrack(os);
    CHECK_OP(os, NULL);
    U2VariantTrack clonedTrack = track;
    dstVDbi->createVariantTrack(clonedTrack, TrackType_All, dstFolder, os);
    CHECK_OP(os, NULL);

    QScopedPointer< U2DbiIterator<U2Variant> > varsIter(this->getVariants(U2_REGION_MAX, os));
    CHECK_OP(os, NULL);
    dstVDbi->addVariantsToTrack(clonedTrack, varsIter.data(), os);
    CHECK_OP(os, NULL);

    U2AttributeUtils::copyObjectAttributes(entityRef.entityId, clonedTrack.id, srcCon.dbi->getAttributeDbi(), dstCon.dbi->getAttributeDbi(), os);

    U2EntityRef clonedTrackRef(dstDbiRef, clonedTrack.id);
    VariantTrackObject *clonedObj = new VariantTrackObject(getGObjectName(), clonedTrackRef, gHints.getMap());
    return clonedObj;
}

void VariantTrackObject::addVariants( const QList<U2Variant>& variants, U2OpStatus& os ){
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );

    U2VariantDbi* vdbi = con.dbi->getVariantDbi();
    SAFE_POINT(vdbi != NULL, "Variant DBI is NULL", );

    U2VariantTrack track = vdbi->getVariantTrack(entityRef.entityId, os);
    CHECK_OP(os, );

    BufferedDbiIterator<U2Variant> bufIter(variants);
    vdbi->addVariantsToTrack(track, &bufIter, os);
}

}//namespace
