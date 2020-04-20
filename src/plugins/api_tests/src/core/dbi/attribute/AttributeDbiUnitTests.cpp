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

#include "AttributeDbiUnitTests.h"

#include <U2Core/U2AttributeDbi.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

static const QString ATTRIBUTE_NAMES = "attribute_names";

static const QString OBJECT_ATTRIBUTES = "object_attributes";

static const QString OBJECT_ATTRIBUTES_NAME = "object_attributes_name";

static const QString OBJECT_ATTRIBUTES_CHILD_ID = "object_attributes_child_id";

const QString& AttributeTestData::ATT_DB_URL("attribute-dbi.ugenedb");

U2AttributeDbi* AttributeTestData::attributeDbi = NULL;
QList<U2DataId>* AttributeTestData::objects = NULL;
TestDbiProvider AttributeTestData::dbiProvider = TestDbiProvider();

static bool registerTests(){
    qRegisterMetaType<U2::AttributeDbiUnitTests_ByteArrayAttribute>("AttributeDbiUnitTests_ByteArrayAttribute");
    qRegisterMetaType<U2::AttributeDbiUnitTests_getAvailableAttributeNames>("AttributeDbiUnitTests_getAvailableAttributeNames");
    qRegisterMetaType<U2::AttributeDbiUnitTests_getObjectAttributes>("AttributeDbiUnitTests_getObjectAttributes");
    qRegisterMetaType<U2::AttributeDbiUnitTests_getObjectAttributesByName>("AttributeDbiUnitTests_getObjectAttributesByName");
    qRegisterMetaType<U2::AttributeDbiUnitTests_getObjectPairAttributes>("AttributeDbiUnitTests_getObjectPairAttributes");
    qRegisterMetaType<U2::AttributeDbiUnitTests_getObjectPairAttributesByName>("AttributeDbiUnitTests_getObjectPairAttributesByName");
    qRegisterMetaType<U2::AttributeDbiUnitTests_IntegerAttribute>("AttributeDbiUnitTests_IntegerAttribute");
    qRegisterMetaType<U2::AttributeDbiUnitTests_RealAttribute>("AttributeDbiUnitTests_RealAttribute");
    qRegisterMetaType<U2::AttributeDbiUnitTests_removeAttributes>("AttributeDbiUnitTests_removeAttributes");
    qRegisterMetaType<U2::AttributeDbiUnitTests_removeObjectAttributes>("AttributeDbiUnitTests_removeObjectAttributes");
    qRegisterMetaType<U2::AttributeDbiUnitTests_StringAttribute>("AttributeDbiUnitTests_StringAttribute");
    return true;
}

bool AttributeTestData::registerTest = registerTests();

void AttributeTestData::init() {
    bool ok = dbiProvider.init(ATT_DB_URL, false);
    SAFE_POINT(ok, "dbi provider failed to initialize",);
    U2Dbi* dbi = dbiProvider.getDbi();
    U2ObjectDbi* objDbi = dbi->getObjectDbi();
    U2OpStatusImpl opStatus;

    objects = new QList<U2DataId>(objDbi->getObjects("/", 0, U2DbiOptions::U2_DBI_NO_LIMIT, opStatus));
    SAFE_POINT_OP(opStatus, );

    attributeDbi = dbi->getAttributeDbi();
    SAFE_POINT(NULL != attributeDbi, "attribute database not loaded",);
}

U2AttributeDbi* AttributeTestData::getAttributeDbi(){
    if ( attributeDbi == NULL) {
        AttributeTestData::init();
    }
    return attributeDbi;
}

void AttributeTestData::shutdown() {
    if ( attributeDbi != NULL) {
        U2OpStatusImpl opStatus;
        dbiProvider.close();
        attributeDbi = NULL;
        SAFE_POINT_OP(opStatus, );
    }
}

static bool compareAttributesBase(const U2Attribute& attr1, const U2Attribute& attr2) {
    if (attr1.objectId != attr2.objectId) {
        return false;
    }
    if (attr1.childId != attr2.childId) {
        return false;
    }
    if (attr1.version != attr2.version) {
        return false;
    }
    if (attr1.name != attr2.name) {
        return false;
    }
    return true;
}

template<class A>
static bool compareAttributes(const A& attr1, const A& attr2) {
    if (attr1.value != attr2.value) {
        return false;
    }
    return compareAttributesBase(attr1, attr2);
}

template<class T, typename Compare>
bool removeOne(QList<T>& list, const T& el, Compare compare) {
    QMutableListIterator<T> iter(list);
    while (iter.hasNext()) {
        const T& current = iter.next();
        if (compare(current, el)) {
            iter.remove();
            return true;
        }
    }
    return false;
}

template<class T>
void filterAttributesByName(QList<T>& attrs, const QString& name) {
    if (!name.isEmpty()) {
        QMutableListIterator<T> iter(attrs);
        while (iter.hasNext()) {
            const T& current = iter.next();
            if (current.name != name) {
                iter.remove();
            }
        }
    }
}

template<class T>
void filterAttributesByChildId(QList<T>& attrs, const U2DataId& childId) {
    QMutableListIterator<T> iter(attrs);
    while (iter.hasNext()) {
        const T& current = iter.next();
        if (current.childId != childId) {
            iter.remove();
        }
    }
}

void AttributeTestData::testAttributesMatch(QList<U2IntegerAttribute>& expectedInt,
                         QList<U2RealAttribute>& expectedReal,
                         QList<U2StringAttribute>& expectedString,
                         QList<U2ByteArrayAttribute>& expectedArray,
                         const QList<U2DataId>& attributes)
{
    foreach(const U2DataId& attrId, attributes) {
        U2DataType type = U2DbiUtils::toType(attrId);
        SAFE_POINT(U2Type::isAttributeType(type), "inncorrect attribute type", );

        U2OpStatusImpl os;

        if (type == U2Type::AttributeInteger) {
            U2IntegerAttribute attr = getAttributeDbi()->getIntegerAttribute(attrId, os);
            SAFE_POINT_OP(os, );
            SAFE_POINT(removeOne(expectedInt, attr, compareAttributes<U2IntegerAttribute>), "Integer attribute is not removed", );
        } else if (type == U2Type::AttributeReal) {
            U2RealAttribute attr = getAttributeDbi()->getRealAttribute(attrId, os);
            SAFE_POINT_OP(os, );
            SAFE_POINT(removeOne(expectedReal, attr, compareAttributes<U2RealAttribute>), "Real attribute is not removed", );
        } else if (type == U2Type::AttributeString) {
            U2StringAttribute attr = getAttributeDbi()->getStringAttribute(attrId, os);
            SAFE_POINT_OP(os, );
            SAFE_POINT(removeOne(expectedString, attr, compareAttributes<U2StringAttribute>), "String attribute is not removed", );
        } else { // AttributeByteArray
            U2ByteArrayAttribute attr = getAttributeDbi()->getByteArrayAttribute(attrId, os);
            SAFE_POINT_OP(os, );
            SAFE_POINT(removeOne(expectedArray, attr, compareAttributes<U2ByteArrayAttribute>), "ByteArray attribute is not removed", );
        }
    }

    SAFE_POINT(expectedInt.size() == 0, "expected integer list size should be 0", );
    SAFE_POINT(expectedReal.size() == 0, "expected real list size should be 0", );
    SAFE_POINT(expectedString.size() == 0, "expected string list size should be 0", );
    SAFE_POINT(expectedArray.size() == 0, "expected byte array list size should be 0", );
}

void AttributeTestData::getObjectAttrsTest(const APITestData& testData, bool filterByName, bool filterByChild) {
    const ObjectAttributesTestData& attrsData = testData.getValue<ObjectAttributesTestData>(OBJECT_ATTRIBUTES);

    const U2DataId& objectId = attrsData.objId;

    QList<U2IntegerAttribute> attrsInt = attrsData.intAttrs;
    QList<U2RealAttribute> attrsReal = attrsData.realAttrs;
    QList<U2StringAttribute> attrsString = attrsData.stringAttrs;
    QList<U2ByteArrayAttribute> attrsByteArray = attrsData.byteArrAttrs;

    const U2DataId& childId = testData.getValue<U2DataId>(OBJECT_ATTRIBUTES_CHILD_ID);
    QString name;

    if (filterByName) {
        name = testData.getValue<QString>(OBJECT_ATTRIBUTES_NAME);
        filterAttributesByName(attrsInt, name);
        filterAttributesByName(attrsReal, name);
        filterAttributesByName(attrsString, name);
        filterAttributesByName(attrsByteArray, name);
    }

    QList<U2DataId> attributes;
    U2OpStatusImpl os;
    if (filterByChild) {
        filterAttributesByChildId(attrsInt, childId);
        filterAttributesByChildId(attrsReal, childId);
        filterAttributesByChildId(attrsString, childId);
        filterAttributesByChildId(attrsByteArray, childId);
        attributes = getAttributeDbi()->getObjectPairAttributes(objectId, childId, name, os);
    } else {
        attributes = getAttributeDbi()->getObjectAttributes(objectId, name, os);
    }
    SAFE_POINT_OP(os, );

    testAttributesMatch(attrsInt, attrsReal, attrsString, attrsByteArray, attributes);
}

void AttributeDbiUnitTests_getAvailableAttributeNames::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();

    QStringList names;
    names << "int1" << "int2" << "real1" << "str1" << "str2" << "arr1";

    U2OpStatusImpl os;
    QStringList actual = attributeDbi->getAvailableAttributeNames(os);
    CHECK_NO_ERROR(os);

    CHECK_TRUE(actual.size() == names.size(), "incorrect expected available attribute names size");
    foreach(const QString& name, names) {
        CHECK_TRUE(actual.removeOne(name), "available attribute name not removed");
    }
    CHECK_TRUE(actual.size() == 0, "available attribute names size should be 0");
}

void AttributeDbiUnitTests_getObjectAttributes::Test() {
    const U2DataId& childId = U2DbiUtils::toU2DataId(2, U2Type::Assembly);
    APITestData testData;

    // getObjectAttributes
    ObjectAttributesTestData oat;
    {
        oat.objId = U2DbiUtils::toU2DataId(1, U2Type::Sequence);

        {
            U2IntegerAttribute attr;
            attr.objectId = oat.objId;
            attr.name = "int1";
            attr.value = 5;
            attr.childId = childId;
            oat.intAttrs.append(attr);
        }

        {
            U2IntegerAttribute attr;
            attr.objectId = oat.objId;
            attr.name = "int2";
            attr.value = 3;
            attr.childId = childId;
            oat.intAttrs.append(attr);
        }

        {
            U2RealAttribute attr;
            attr.objectId = oat.objId;
            attr.name = "real1";
            attr.value = 2.7;
            attr.childId = childId;
            oat.realAttrs.append(attr);
        }

        {
            U2StringAttribute attr;
            attr.objectId = oat.objId;
            attr.name = "str1";
            attr.value = "some string";
            oat.stringAttrs.append(attr);
        }

        {
            U2StringAttribute attr;
            attr.objectId = oat.objId;
            attr.name = "str2";
            attr.value = "other string";
            oat.stringAttrs.append(attr);
        }

        {
            U2ByteArrayAttribute attr;
            attr.objectId = oat.objId;
            attr.name = "arr1";
            attr.value = "some array";
            oat.byteArrAttrs.append(attr);
        }
    }

    testData.addValue(OBJECT_ATTRIBUTES, oat);
    AttributeTestData::getObjectAttrsTest(testData, false, false);
}

void AttributeDbiUnitTests_getObjectAttributesByName::Test() {
     APITestData testData;
    testData.addValue<QString>(OBJECT_ATTRIBUTES_NAME, "int2");
    AttributeTestData::getObjectAttrsTest(testData, true, false);
}

void AttributeDbiUnitTests_getObjectPairAttributes::Test() {
     APITestData testData;
    const U2DataId& childId = U2DbiUtils::toU2DataId(2, U2Type::Assembly);
    testData.addValue<U2DataId>(OBJECT_ATTRIBUTES_CHILD_ID, childId);
    AttributeTestData::getObjectAttrsTest(testData, false, true);
}

void AttributeDbiUnitTests_getObjectPairAttributesByName::Test() {

     APITestData testData;
    testData.addValue<QString>(OBJECT_ATTRIBUTES_NAME, "int2");
    const U2DataId& childId = U2DbiUtils::toU2DataId(2, U2Type::Assembly);
    testData.addValue<U2DataId>(OBJECT_ATTRIBUTES_CHILD_ID, childId);
    AttributeTestData::getObjectAttrsTest(testData, true, true);
}

void AttributeDbiUnitTests_removeAttributes::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();


    const U2DataId& objectId = AttributeTestData::getObjects()->first();
    U2OpStatusImpl os;
    const QList<U2DataId>& attrs = attributeDbi->getObjectAttributes(objectId, "", os);
    CHECK_NO_ERROR(os);

    {
        U2OpStatusImpl os;
        attributeDbi->removeAttributes(attrs, os);
        CHECK_NO_ERROR(os);
    }
    {
        U2OpStatusImpl os;
        const QList<U2DataId>& attrs = attributeDbi->getObjectAttributes(objectId, "", os);
        CHECK_NO_ERROR(os);
        CHECK_TRUE(attrs.isEmpty(), "attribute list should be empty");
    }
}

void AttributeDbiUnitTests_removeObjectAttributes::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();

    const U2DataId& objectId = AttributeTestData::getObjects()->first();
    {
        U2OpStatusImpl os;
        attributeDbi->removeObjectAttributes(objectId, os);
        CHECK_NO_ERROR(os);
    }
    U2OpStatusImpl os;
    const QList<U2DataId>& attrs = attributeDbi->getObjectAttributes(objectId, "", os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(attrs.isEmpty(), "object attribute list should be empty");
}

void AttributeDbiUnitTests_IntegerAttribute::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();

    U2IntegerAttribute attr;
    attr.objectId = AttributeTestData::getObjects()->first();
    attr.value = 46;

    {
        U2OpStatusImpl os;
        attributeDbi->createIntegerAttribute(attr, os);
        CHECK_NO_ERROR(os);
    }

    U2OpStatusImpl os;
    U2IntegerAttribute actual = attributeDbi->getIntegerAttribute(attr.id, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(compareAttributes(actual, attr), "incorrect integer attribute");
}

void AttributeDbiUnitTests_RealAttribute::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();

    U2RealAttribute attr;
    attr.objectId = AttributeTestData::getObjects()->first();
    attr.value = 46.46;

    {
        U2OpStatusImpl os;
        attributeDbi->createRealAttribute(attr, os);
        CHECK_NO_ERROR(os);
    }

    U2OpStatusImpl os;
    U2RealAttribute actual = attributeDbi->getRealAttribute(attr.id, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(compareAttributes(actual, attr), "incorrect real attribute");
}


void AttributeDbiUnitTests_StringAttribute::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();

    U2StringAttribute attr;
    attr.objectId = AttributeTestData::getObjects()->first();
    attr.value = "some string";

    {
        U2OpStatusImpl os;
        attributeDbi->createStringAttribute(attr, os);
        CHECK_NO_ERROR(os);
    }

    U2OpStatusImpl os;
    U2StringAttribute actual = attributeDbi->getStringAttribute(attr.id, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(compareAttributes(actual, attr), "incorrect string attribute");
}

void AttributeDbiUnitTests_ByteArrayAttribute::Test() {
    U2AttributeDbi* attributeDbi = AttributeTestData::getAttributeDbi();

    U2ByteArrayAttribute attr;
    attr.objectId = AttributeTestData::getObjects()->first();
    attr.value = "some array";

    {
        U2OpStatusImpl os;
        attributeDbi->createByteArrayAttribute(attr, os);
        CHECK_NO_ERROR(os);
    }

    U2OpStatusImpl os;
    U2ByteArrayAttribute actual = attributeDbi->getByteArrayAttribute(attr.id, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(compareAttributes(actual, attr), "incorrect byte array attribute");
}

} //namspace
