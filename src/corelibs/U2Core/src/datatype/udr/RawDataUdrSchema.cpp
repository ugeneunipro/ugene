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
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrDbi.h>
#include <U2Core/UdrSchemaRegistry.h>

#include "RawDataUdrSchema.h"

namespace U2 {

const UdrSchemaId RawDataUdrSchema::ID("RawData");

namespace {
    // Fields numbers
    const int CONTENT = 1;
    const int SERIALIZER = 2;

    const int BUFFER_SIZE = 4*1024*1024;

    class DbiHelper {
        Q_DISABLE_COPY(DbiHelper)

        DbiConnection *con;
    public:
        DbiHelper(const U2DbiRef &dbiRef, U2OpStatus &os)
            : dbi(NULL)
        {
            con = new DbiConnection(dbiRef, os);
            CHECK_OP(os, );
            SAFE_POINT_EXT(NULL != con->dbi, os.setError("NULL DBI"), );
            dbi = con->dbi->getUdrDbi();
            SAFE_POINT_EXT(NULL != dbi, os.setError("NULL source UDR DBI"), );
        }

        ~DbiHelper() {
            delete con;
        }

        UdrDbi *dbi;
    };

    UdrRecordId getRecordId(UdrDbi *dbi, const U2DataId &objId, U2OpStatus &os) {
        const QList<UdrRecord> records = dbi->getObjectRecords(RawDataUdrSchema::ID, objId, os);
        CHECK_OP(os, UdrRecordId("", ""));
        CHECK_EXT(1 == records.size(), os.setError("Unexpected records count"), UdrRecordId("", ""));
        return records.first().getId();
    }

    UdrRecordId retrieveObject(UdrDbi *dbi, U2RawData &object, U2OpStatus &os) {
        UdrRecordId recId = getRecordId(dbi, object.id, os);
        CHECK_OP(os, recId);

        UdrRecord record = dbi->getRecord(recId, os);
        CHECK_OP(os, recId);

        U2Object obj;
        dbi->getRootDbi()->getObjectDbi()->getObject(obj, object.id, os);
        CHECK_OP(os, recId);

        object.visualName = obj.visualName;
        object.version = obj.version;
        CHECK_OP(os, recId);

        object.serializer = record.getString(SERIALIZER, os);
        CHECK_OP(os, recId);

        return recId;
    }

    UdrRecordId createObjectCore(UdrDbi *dbi, const QString& folder, U2RawData &object, U2OpStatus &os) {
        dbi->createObject(RawDataUdrSchema::ID, object, folder, os);
        CHECK_OP(os, UdrRecordId("", ""));

        QList<UdrValue> data;
        data << UdrValue(object.id);
        data << UdrValue();
        data << UdrValue(object.serializer);

        return dbi->addRecord(RawDataUdrSchema::ID, data, os);
    }
}

void RawDataUdrSchema::init(U2OpStatus &os) {
    UdrSchema::FieldDesc content("content", UdrSchema::BLOB);
    UdrSchema::FieldDesc serializer("serializer", UdrSchema::STRING);

    QScopedPointer<UdrSchema> fileSchema(new UdrSchema(ID, true));
    fileSchema->addField(content, os);
    CHECK_OP(os, );
    fileSchema->addField(serializer, os);
    CHECK_OP(os, );

    AppContext::getUdrSchemaRegistry()->registerSchema(fileSchema.data(), os);
    if (!os.hasError()) {
        fileSchema.take();
    }
}

U2RawData RawDataUdrSchema::getObject(const U2EntityRef &objRef, U2OpStatus &os) {
    DbiHelper con(objRef.dbiRef, os);
    CHECK_OP(os, U2RawData());

    U2RawData result(objRef.dbiRef);
    result.id = objRef.entityId;
    retrieveObject(con.dbi, result, os);
    return result;
}

void RawDataUdrSchema::createObject(const U2DbiRef &dbiRef, U2RawData &object, U2OpStatus &os) {
    createObject(dbiRef, U2ObjectDbi::ROOT_FOLDER, object, os);
}

void RawDataUdrSchema::createObject(const U2DbiRef &dbiRef, const QString& folder, U2RawData &object, U2OpStatus &os) {
    DbiHelper con(dbiRef, os);
    CHECK_OP(os, );

    createObjectCore(con.dbi, folder, object, os);
}

void RawDataUdrSchema::writeContent(const QByteArray &data, const U2EntityRef &objRef, U2OpStatus &os) {
    DbiHelper con(objRef.dbiRef, os);
    CHECK_OP(os, );

    const UdrRecordId id = getRecordId(con.dbi, objRef.entityId, os);
    CHECK_OP(os, );

    QScopedPointer<OutputStream> oStream(con.dbi->createOutputStream(id, CONTENT, data.size(), os));
    CHECK_OP(os, );
    oStream->write(data.data(), data.size(), os);
}

void RawDataUdrSchema::writeContent(const U2DataId &masterId, const QByteArray &data, const U2EntityRef &objRef, U2OpStatus &os) {
    DbiHelper con(objRef.dbiRef, os);
    CHECK_OP(os, );
    QScopedPointer<ModificationAction> updateAction(con.dbi->getModificationAction(masterId));
    U2TrackModType trackMod = updateAction->prepare(os);
    CHECK_OP(os, );

    QByteArray modDetails;
    if (trackMod == TrackOnUpdate) {
        QByteArray olderData = readAllContent(objRef, os);
        modDetails = U2DbiPackUtils::packUdr(olderData, data);
    }

    writeContent(data, objRef, os);

    updateAction->addModification(objRef.entityId, U2ModType::udrUpdated, modDetails, os);
    updateAction->complete(os);
}

QByteArray RawDataUdrSchema::readAllContent(const U2EntityRef &objRef, U2OpStatus &os) {
    DbiHelper con(objRef.dbiRef, os);
    CHECK_OP(os, "");

    const UdrRecordId id = getRecordId(con.dbi, objRef.entityId, os);
    CHECK_OP(os, "");

    QScopedPointer<InputStream> iStream(con.dbi->createInputStream(id, CONTENT, os));
    CHECK_OP(os, "");

    QByteArray result(iStream->available(), 0);
    iStream->read(result.data(), iStream->available(), os);
    CHECK_OP(os, "");

    return result;
}

void RawDataUdrSchema::cloneObject(const U2EntityRef &srcObjRef, const U2DbiRef &dstDbiRef,
                                   const QString &dstFolder, U2RawData &dstObject, U2OpStatus &os)
{
    DbiOperationsBlock srcOpBlock(srcObjRef.dbiRef, os);
    Q_UNUSED(srcOpBlock);
    CHECK_OP(os, );
    DbiOperationsBlock dstOpBlock(dstDbiRef, os);
    Q_UNUSED(dstOpBlock);
    CHECK_OP(os, );

    // Prepare dbi connection
    DbiHelper src(srcObjRef.dbiRef, os);
    CHECK_OP(os, );
    DbiHelper dst(dstDbiRef, os);
    CHECK_OP(os, );

    // Copy object
    dstObject.dbiId = dstDbiRef.dbiId;
    dstObject.id = srcObjRef.entityId;
    const UdrRecordId srcId = retrieveObject(src.dbi, dstObject, os);
    CHECK_OP(os, );
    dstObject.version = 0;
    const UdrRecordId dstId = createObjectCore(dst.dbi, dstFolder, dstObject, os);
    CHECK_OP(os, );

    // Copy content
    QScopedPointer<InputStream> iStream(src.dbi->createInputStream(srcId, CONTENT, os));
    CHECK_OP(os, );
    QScopedPointer<OutputStream> oStream(dst.dbi->createOutputStream(dstId, CONTENT, iStream->available(), os));
    CHECK_OP(os, );
    QByteArray buffer(BUFFER_SIZE, 0);
    char *bytes = buffer.data();
    while (iStream->available() > 0) {
        int read = iStream->read(bytes, BUFFER_SIZE, os);
        CHECK_OP(os, );
        oStream->write(bytes, read, os);
        CHECK_OP(os, );
    }
}

} // U2
