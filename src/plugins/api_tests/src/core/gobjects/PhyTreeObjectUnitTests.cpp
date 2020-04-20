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
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UdrDbi.h>

#include "PhyTreeObjectUnitTests.h"

namespace U2 {

bool PhyTreeObjectTestData::inited = false;
const QString PhyTreeObjectTestData::UDR_DB_URL = "PhyTreeObjectUnitTests.ugenedb";
TestDbiProvider PhyTreeObjectTestData::dbiProvider = TestDbiProvider();
U2EntityRef PhyTreeObjectTestData::objRef = U2EntityRef();

U2DbiRef PhyTreeObjectTestData::getDbiRef() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getDbiRef();
}

U2EntityRef PhyTreeObjectTestData::getObjRef() {
    if (!inited) {
        init();
    }
    return objRef;
}

U2ObjectDbi * PhyTreeObjectTestData::getObjDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getObjectDbi();
}

UdrDbi * PhyTreeObjectTestData::getUdrDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getUdrDbi();
}

void PhyTreeObjectTestData::init() {
    bool ok = dbiProvider.init(UDR_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize",);

    initData();

    inited = true;
}

void PhyTreeObjectTestData::initData() {
    U2DbiRef dbiRef = dbiProvider.getDbi()->getDbiRef();
    U2RawData object(dbiRef);
    object.serializer = NewickPhyTreeSerializer::ID;

    U2OpStatusImpl os;
    RawDataUdrSchema::createObject(dbiRef, object, os);
    SAFE_POINT_OP(os, );

    objRef = U2EntityRef(dbiRef, object.id);

    RawDataUdrSchema::writeContent("(B:6.0,(A:5.0,C:3.0,E:4.0):5.0,D:11.0);\n", objRef, os);
    SAFE_POINT_OP(os, );
}

void PhyTreeObjectTestData::shutdown() {
    if (inited) {
        inited = false;
        U2OpStatusImpl os;
        dbiProvider.close();
        SAFE_POINT_OP(os, );
    }
}

namespace {
    PhyTree createTree() {
        PhyTree result(new PhyTreeData());
        PhyNode *root = new PhyNode();
        root->setName("ROOT");
        result->setRootNode(root);
        QStringList names; names << "A" << "B" << "C";
        foreach (const QString &name, names) {
            PhyNode *node = new PhyNode();
            node->setName(name);
            result->addBranch(root, node, 5.1);
        }
        return result;
    }
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, createInstance) {
    U2OpStatusImpl os;
    QScopedPointer<PhyTreeObject> object(PhyTreeObject::createInstance(createTree(), "object", PhyTreeObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    CHECK_TRUE(NULL != object->getTree().data(), "tree");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, createInstance_WrongDbi) {
    U2OpStatusImpl os;
    QScopedPointer<PhyTreeObject> object(PhyTreeObject::createInstance(createTree(), "object", U2DbiRef(), os));
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, getTree) {
    PhyTreeObject object("object", PhyTreeObjectTestData::getObjRef());
    CHECK_TRUE(NULL != object.getTree().data(), "tree");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, getTree_Null) {
    U2EntityRef objRef = PhyTreeObjectTestData::getObjRef();
    objRef.entityId = "some id";

    PhyTreeObject object("object", objRef);
    CHECK_TRUE(NULL == object.getTree().data(), "tree");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, setTree) {
    U2OpStatusImpl os;
    QScopedPointer<PhyTreeObject> object(PhyTreeObject::createInstance(createTree(), "object", PhyTreeObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    PhyTree tree = createTree();
    object->setTree(tree);
    CHECK_TRUE(tree == object->getTree(), "tree");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, clone) {
    PhyTreeObject object("object", PhyTreeObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(PhyTreeObjectTestData::getDbiRef(), os);
    QScopedPointer<PhyTreeObject> cloned(dynamic_cast<PhyTreeObject*>(clonedGObj));
    CHECK_NO_ERROR(os);

    PhyTree clonedTree = createTree();
    cloned->setTree(clonedTree);

    CHECK_TRUE(clonedTree != object.getTree(), "tree");
    CHECK_TRUE(clonedTree == cloned->getTree(), "cloned tree");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, clone_NullDbi) {
    PhyTreeObject object("object", PhyTreeObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(U2DbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, clone_NullObj) {
    U2EntityRef objRef = PhyTreeObjectTestData::getObjRef();
    objRef.entityId = "some id";
    PhyTreeObject object("object", objRef);

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(PhyTreeObjectTestData::getDbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(PhyTreeObjectUnitTests, remove) {
    U2OpStatusImpl os;
    QScopedPointer<PhyTreeObject> object(PhyTreeObject::createInstance(createTree(), "object", PhyTreeObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);
    U2DataId objId = object->getEntityRef().entityId;

    PhyTreeObjectTestData::getObjDbi()->removeObject(objId, os);
    CHECK_NO_ERROR(os);

    QList<UdrRecord> records = PhyTreeObjectTestData::getUdrDbi()->getObjectRecords(RawDataUdrSchema::ID, objId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(records.isEmpty(), "records");
}

} // U2
