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

#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BioStruct3DObject.h"

namespace U2 {

/////// U2BioStruct3D Implementation ///////////////////////////////////////////////////////////////////

U2BioStruct3D::U2BioStruct3D() : U2RawData() {

}

U2BioStruct3D::U2BioStruct3D(const U2DbiRef &dbiRef) : U2RawData(dbiRef) {

}

U2DataType U2BioStruct3D::getType() const {
    return U2Type::BioStruct3D;
}

/////// BioStruct3DObject Implementation ///////////////////////////////////////////////////////////////////

BioStruct3DObject * BioStruct3DObject::createInstance(const BioStruct3D &bioStruct3D,
    const QString &objectName, const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hintsMap) {
    U2BioStruct3D object(dbiRef);
    object.visualName = objectName;
    object.serializer = BioStruct3DSerializer::ID;

    const QString folder = hintsMap.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    RawDataUdrSchema::createObject(dbiRef, folder, object, os);
    CHECK_OP(os, NULL);

    const U2EntityRef entRef(dbiRef, object.id);
    const QByteArray data = BioStruct3DSerializer::serialize(bioStruct3D);
    RawDataUdrSchema::writeContent(data, entRef, os);
    CHECK_OP(os, NULL);

    return new BioStruct3DObject(bioStruct3D, objectName, entRef, hintsMap);
}

BioStruct3DObject::BioStruct3DObject(const QString &objectName, const U2EntityRef &structRef,
    const QVariantMap &hintsMap)
    : GObject(GObjectTypes::BIOSTRUCTURE_3D, objectName, hintsMap) {
    entityRef = structRef;
}

BioStruct3DObject::BioStruct3DObject(const BioStruct3D &bioStruct3D, const QString &objectName,
    const U2EntityRef &structRef, const QVariantMap &hintsMap)
    : GObject(GObjectTypes::BIOSTRUCTURE_3D, objectName, hintsMap), bioStruct3D(bioStruct3D) {
    entityRef = structRef;
}

const BioStruct3D & BioStruct3DObject::getBioStruct3D() const {
    ensureDataLoaded();
    return bioStruct3D;
}

void BioStruct3DObject::loadDataCore(U2OpStatus &os) {
    const QString serializer = RawDataUdrSchema::getObject(entityRef, os).serializer;
    CHECK_OP(os, );
    SAFE_POINT(BioStruct3DSerializer::ID == serializer, "Unknown serializer id", );

    const QByteArray data = RawDataUdrSchema::readAllContent(entityRef, os);
    CHECK_OP(os, );

    bioStruct3D = BioStruct3DSerializer::deserialize(data, os);
}

GObject * BioStruct3DObject::clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);
    const QString dstFolder = gHints.get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    U2BioStruct3D dstObject;
    RawDataUdrSchema::cloneObject(entityRef, dstDbiRef, dstFolder, dstObject, os);
    CHECK_OP(os, NULL);

    U2EntityRef dstEntRef(dstDbiRef, dstObject.id);
    BioStruct3DObject *dst = new BioStruct3DObject(getGObjectName(), dstEntRef, gHints.getMap());
    dst->setIndexInfo(getIndexInfo());
    return dst;
}

} // U2
