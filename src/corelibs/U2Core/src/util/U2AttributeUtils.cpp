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

#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {


void U2AttributeUtils::init(U2Attribute &attr, const U2Object &obj, const QString &name) {
    attr.objectId = obj.id;
    attr.version = obj.version;
    attr.name = name;
}

void U2AttributeUtils::removeAttribute(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus& os) {
    QList<U2DataId> ids;
    ids << attrId;
    adbi->removeAttributes(ids, os);
}

// Finders: generic functionality

template <class AttrType>
static AttrType getAttribute(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus& os);


template <class AttrType>
static AttrType findGenericAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& attrName, U2DataType type, U2OpStatus& os) {
    QList<U2DataId> attributeIds = adbi->getObjectAttributes(objectId, attrName, os);
    if (attributeIds.isEmpty() || os.hasError()) {
        return AttrType();
    }
    U2Dbi* dbi = adbi->getRootDbi();
    foreach(const U2DataId& id, attributeIds) {
        if (dbi->getEntityTypeById(id) == type) {
            return getAttribute<AttrType>(adbi, id, os);
        }
    }
    return AttrType();
}

// Implementations

U2IntegerAttribute U2AttributeUtils::findIntegerAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& attrName, U2OpStatus& os) {
    return findGenericAttribute<U2IntegerAttribute>(adbi, objectId, attrName, U2Type::AttributeInteger, os);
}


U2RealAttribute U2AttributeUtils::findRealAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& attrName, U2OpStatus& os) {
    return findGenericAttribute<U2RealAttribute>(adbi, objectId, attrName, U2Type::AttributeReal, os);
}


U2ByteArrayAttribute U2AttributeUtils::findByteArrayAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& attrName, U2OpStatus& os) {
    return findGenericAttribute<U2ByteArrayAttribute>(adbi, objectId, attrName, U2Type::AttributeByteArray, os);
}


U2StringAttribute U2AttributeUtils::findStringAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& attrName, U2OpStatus& os) {
    return findGenericAttribute<U2StringAttribute>(adbi, objectId, attrName, U2Type::AttributeString, os);
}

// Getting attribute template specializations

template <>
U2IntegerAttribute getAttribute<U2IntegerAttribute>(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus &os) {
    return adbi->getIntegerAttribute(attrId, os);
}

template <>
U2RealAttribute getAttribute<U2RealAttribute>(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus &os) {
    return adbi->getRealAttribute(attrId, os);
}

template <>
U2ByteArrayAttribute getAttribute<U2ByteArrayAttribute>(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus &os) {
    return adbi->getByteArrayAttribute(attrId, os);
}

template <>
U2StringAttribute getAttribute<U2StringAttribute>(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus &os) {
    return adbi->getStringAttribute(attrId, os);
}

void U2AttributeUtils::copyObjectAttributes(const U2DataId &srcObjId, const U2DataId &dstObjId,
                                 U2AttributeDbi *srcAttributeDbi, U2AttributeDbi *dstAttributeDbi,
                                 U2OpStatus &os) {
    CHECK_EXT(NULL != srcAttributeDbi, os.setError("NULL source attribute dbi"), );
    CHECK_EXT(NULL != dstAttributeDbi, os.setError("NULL destination attribute dbi"), );

    U2Dbi *dstDbi = dstAttributeDbi->getRootDbi();
    U2Dbi *srcDbi = srcAttributeDbi->getRootDbi();
    CHECK_EXT(NULL != srcDbi, os.setError("NULL source root dbi"), );
    CHECK_EXT(NULL != dstDbi, os.setError("NULL destination root dbi"), );

    if (!dstDbi->getFeatures().contains(U2DbiFeature_WriteAttributes)) {
        os.setError("Destination dbi does not support writing");
        return;
    }

    QList<U2DataId> attrIds = srcAttributeDbi->getObjectAttributes(srcObjId, "", os);
    CHECK_OP(os, );

    foreach (const U2DataId &attrId, attrIds) {
        U2DataType attrType = srcDbi->getEntityTypeById(attrId);
        if (U2Type::AttributeInteger == attrType) {
            U2IntegerAttribute attr = srcAttributeDbi->getIntegerAttribute(attrId, os);
            CHECK_OP(os, );
            attr.objectId = dstObjId;
            dstAttributeDbi->createIntegerAttribute(attr, os);
        } else if (U2Type::AttributeReal == attrType) {
            U2RealAttribute attr = srcAttributeDbi->getRealAttribute(attrId, os);
            CHECK_OP(os, );
            attr.objectId = dstObjId;
            dstAttributeDbi->createRealAttribute(attr, os);
        } else if (U2Type::AttributeString == attrType) {
            U2StringAttribute attr = srcAttributeDbi->getStringAttribute(attrId, os);
            CHECK_OP(os, );
            attr.objectId = dstObjId;
            dstAttributeDbi->createStringAttribute(attr, os);
        } else if (U2Type::AttributeByteArray == attrType) {
            U2ByteArrayAttribute attr = srcAttributeDbi->getByteArrayAttribute(attrId, os);
            CHECK_OP(os, );
            attr.objectId = dstObjId;
            dstAttributeDbi->createByteArrayAttribute(attr, os);
        }
        CHECK_OP(os, );
    }
}

void U2AttributeUtils::copyObjectAttributes(const U2EntityRef &srcObjRef, const U2EntityRef &dstObjRef, U2OpStatus &os) {
    DbiConnection srcCon(srcObjRef.dbiRef, os);
    CHECK_OP(os, );
    DbiConnection dstCon(dstObjRef.dbiRef, os);
    CHECK_OP(os, );
    copyObjectAttributes(srcObjRef.entityId, dstObjRef.entityId, srcCon.dbi->getAttributeDbi(), dstCon.dbi->getAttributeDbi(), os);
}

}   // namespace U2
