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
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UdrDbi.h>

#include <U2Test/TestRunnerSettings.h>

#include "BioStruct3DObjectUnitTests.h"

namespace U2 {

bool BioStruct3DObjectTestData::inited = false;
const QString BioStruct3DObjectTestData::UDR_DB_URL = "BioStruct3DObjectUnitTests.ugenedb";
TestDbiProvider BioStruct3DObjectTestData::dbiProvider = TestDbiProvider();
U2EntityRef BioStruct3DObjectTestData::objRef = U2EntityRef();
BioStruct3D BioStruct3DObjectTestData::bioStruct = BioStruct3D();

U2DbiRef BioStruct3DObjectTestData::getDbiRef() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getDbiRef();
}

U2EntityRef BioStruct3DObjectTestData::getObjRef() {
    if (!inited) {
        init();
    }
    return objRef;
}

U2ObjectDbi * BioStruct3DObjectTestData::getObjDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getObjectDbi();
}

UdrDbi * BioStruct3DObjectTestData::getUdrDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getUdrDbi();
}

void BioStruct3DObjectTestData::init() {
    bool ok = dbiProvider.init(UDR_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize",);

    initData();

    inited = true;
}

void BioStruct3DObjectTestData::initData() {
    U2OpStatusImpl os;
    bioStruct = readBioStruct("3INS.PDB", os, false);
    SAFE_POINT_OP(os, );

    U2DbiRef dbiRef = dbiProvider.getDbi()->getDbiRef();
    U2RawData object(dbiRef);
    object.serializer = BioStruct3DSerializer::ID;

    RawDataUdrSchema::createObject(dbiRef, object, os);
    SAFE_POINT_OP(os, );

    objRef = U2EntityRef(dbiRef, object.id);

    QByteArray data = BioStruct3DSerializer::serialize(bioStruct);
    RawDataUdrSchema::writeContent(data, objRef, os);
    SAFE_POINT_OP(os, );
}

void BioStruct3DObjectTestData::shutdown() {
    if (inited) {
        inited = false;
        U2OpStatusImpl os;
        dbiProvider.close();
        SAFE_POINT_OP(os, );
    }
}

const BioStruct3D & BioStruct3DObjectTestData::getBioStruct() {
    if (!inited) {
        init();
    }
    return bioStruct;
}

BioStruct3D BioStruct3DObjectTestData::readBioStruct(const QString &fileName, U2OpStatus &os, bool useSessionDbi) {
    TestRunnerSettings *trs = AppContext::getAppSettings()->getTestRunnerSettings();
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    DocumentFormat *f = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_PDB);
    SAFE_POINT_EXT(NULL != f, os.setError("NULL format"), BioStruct3D());

    QVariantMap hints;
    if (!useSessionDbi) {
        hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(dbiProvider.getDbi()->getDbiRef());
    }
    QScopedPointer<Document> doc(f->loadDocument(iof, trs->getVar("COMMON_DATA_DIR") + "/" + fileName, hints, os));
    CHECK_OP(os, BioStruct3D());

    QList<GObject*> objs = doc->findGObjectByType(GObjectTypes::BIOSTRUCTURE_3D);
    CHECK_EXT(1 == objs.size(), os.setError("object list size"), BioStruct3D());

    BioStruct3DObject *obj = dynamic_cast<BioStruct3DObject*>(objs.first());
    CHECK_EXT(NULL != obj, os.setError("NULL object"), BioStruct3D());

    return obj->getBioStruct3D();
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, createInstance) {
    U2OpStatusImpl os;
    QScopedPointer<BioStruct3DObject> object(BioStruct3DObject::createInstance(BioStruct3DObjectTestData::getBioStruct(), "object", BioStruct3DObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    CHECK_TRUE(BioStruct3DObjectTestData::getBioStruct().pdbId == object->getBioStruct3D().pdbId, "pdbId");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, createInstance_WrongDbi) {
    U2OpStatusImpl os;
    QScopedPointer<BioStruct3DObject> object(BioStruct3DObject::createInstance(BioStruct3DObjectTestData::getBioStruct(), "object", U2DbiRef(), os));
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, getBioStruct3D) {
    BioStruct3DObject object("object", BioStruct3DObjectTestData::getObjRef());
    CHECK_TRUE(BioStruct3DObjectTestData::getBioStruct().pdbId == object.getBioStruct3D().pdbId, "pdbId");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, getBioStruct3D_Null) {
    U2EntityRef objRef = BioStruct3DObjectTestData::getObjRef();
    objRef.entityId = "some id";

    BioStruct3DObject object("object", objRef);
    CHECK_TRUE(object.getBioStruct3D().pdbId.isEmpty(), "pdbId");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, clone) {
    BioStruct3DObject object("object", BioStruct3DObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(BioStruct3DObjectTestData::getDbiRef(), os);
    QScopedPointer<BioStruct3DObject> cloned(dynamic_cast<BioStruct3DObject*>(clonedGObj));
    CHECK_NO_ERROR(os);

    CHECK_TRUE(cloned->getBioStruct3D().pdbId == object.getBioStruct3D().pdbId, "pdbId");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, clone_NullDbi) {
    BioStruct3DObject object("object", BioStruct3DObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(U2DbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, clone_NullObj) {
    U2EntityRef objRef = BioStruct3DObjectTestData::getObjRef();
    objRef.entityId = "some id";
    BioStruct3DObject object("object", objRef);

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(BioStruct3DObjectTestData::getDbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(BioStruct3DObjectUnitTests, remove) {
    U2OpStatusImpl os;
    QScopedPointer<BioStruct3DObject> object(BioStruct3DObject::createInstance(BioStruct3DObjectTestData::getBioStruct(), "object", BioStruct3DObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);
    U2DataId objId = object->getEntityRef().entityId;

    BioStruct3DObjectTestData::getObjDbi()->removeObject(objId, os);
    CHECK_NO_ERROR(os);

    QList<UdrRecord> records = BioStruct3DObjectTestData::getUdrDbi()->getObjectRecords(RawDataUdrSchema::ID, objId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(records.isEmpty(), "records");
}

} // U2
