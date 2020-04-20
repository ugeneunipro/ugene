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

#include <QFile>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2FeatureDbi.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2RawData.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/U2VariantDbi.h>
#include <U2Core/UserApplicationsSettings.h>

#include "DbiDataStorage.h"

namespace U2 {

namespace Workflow {

DbiDataStorage::DbiDataStorage()
: dbiHandle(NULL)
{

}

DbiDataStorage::~DbiDataStorage() {
    foreach (const U2DbiId &id, connections.keys()) {
        DbiConnection *connection = connections[id];
        delete connection;
    }
    foreach (const U2DbiId &dbiId, dbiList.keys()) {
        bool temporary = dbiList.value(dbiId);
        if (temporary) {
            if (QFile::exists(dbiId)) {
                QFile::remove(dbiId);
            }
        }
    }
    delete dbiHandle;
}

bool DbiDataStorage::init() {
    U2OpStatusImpl os;
    dbiHandle = new TmpDbiHandle(WORKFLOW_SESSION_TMP_DBI_ALIAS, os);
    CHECK_OP(os, false);

    QScopedPointer<DbiConnection> connection(new DbiConnection(dbiHandle->getDbiRef(), os));
    CHECK_OP(os, false);

    connections[dbiHandle->getDbiRef().dbiId] = connection.take();
    return true;
}

U2DbiRef DbiDataStorage::getDbiRef() {
    assert(NULL != dbiHandle);
    return dbiHandle->getDbiRef();
}

U2Object *DbiDataStorage::getObject(const SharedDbiDataHandler &handler, const U2DataType &type) {
    assert(NULL != dbiHandle);
    U2OpStatusImpl os;
    const U2DataId &objectId = handler->entRef.entityId;
    DbiConnection *connection = this->getConnection(handler->getDbiRef(), os);
    CHECK_OP(os, NULL);

    if (U2Type::Sequence == type) {
        U2SequenceDbi *dbi = connection->dbi->getSequenceDbi();
        U2Sequence seq = dbi->getSequenceObject(objectId, os);
        SAFE_POINT_OP(os, NULL);

        return new U2Sequence(seq);
    } else if (U2Type::Msa == type) {
        U2MsaDbi *dbi = connection->dbi->getMsaDbi();
        U2Msa msa = dbi->getMsaObject(objectId, os);
        SAFE_POINT_OP(os, NULL);

        return new U2Msa(msa);
    } else if (U2Type::VariantTrack == type) {
        U2VariantDbi *dbi = connection->dbi->getVariantDbi();
        U2VariantTrack track = dbi->getVariantTrack(objectId, os);
        SAFE_POINT_OP(os, NULL);

        return new U2VariantTrack(track);
    } else if (U2Type::Assembly == type) {
        U2AssemblyDbi *dbi = connection->dbi->getAssemblyDbi();
        U2Assembly assembly = dbi->getAssemblyObject(objectId, os);
        SAFE_POINT_OP(os, NULL);

        return new U2Assembly(assembly);
    } else if (U2Type::AnnotationTable == type) {
        U2FeatureDbi *dbi = connection->dbi->getFeatureDbi();
        U2AnnotationTable annTable = dbi->getAnnotationTableObject(objectId, os);
        SAFE_POINT_OP(os, NULL);

        return new U2AnnotationTable(annTable);
    } else if (U2Type::Text == type) {
        U2RawData rawData = RawDataUdrSchema::getObject(handler->entRef, os);
        SAFE_POINT_OP(os, NULL);

        return new U2RawData(rawData);
    } else {
        assert(0);
    }
    return NULL;
}

SharedDbiDataHandler DbiDataStorage::putSequence(const DNASequence &dnaSeq) {
    assert(NULL != dbiHandle);

    U2OpStatusImpl os;
    U2EntityRef ent = U2SequenceUtils::import(os, dbiHandle->getDbiRef(), dnaSeq);
    CHECK_OP(os, SharedDbiDataHandler());

    DbiConnection *connection = this->getConnection(dbiHandle->getDbiRef(), os);
    CHECK_OP(os, SharedDbiDataHandler());

    SharedDbiDataHandler handler(new DbiDataHandler(ent, connection->dbi->getObjectDbi(), true));

    return handler;
}

SharedDbiDataHandler DbiDataStorage::putSequence(const U2SequenceObject *sequenceObject) {
    SAFE_POINT(NULL != dbiHandle, "Invalid DBI handle", SharedDbiDataHandler());
    SAFE_POINT(NULL != sequenceObject, L10N::nullPointerError("Sequence object"), SharedDbiDataHandler());

    U2OpStatusImpl os;

    U2EntityRef entityRef = sequenceObject->getEntityRef();
    if (sequenceObject->getEntityRef().dbiRef != dbiHandle->getDbiRef()) {
        QScopedPointer<U2SequenceObject> clonedSequenceObject(qobject_cast<U2SequenceObject *>(sequenceObject->clone(dbiHandle->getDbiRef(), os)));
        SAFE_POINT_OP(os, SharedDbiDataHandler());
        entityRef = clonedSequenceObject->getEntityRef();
    }

    DbiConnection *connection = getConnection(dbiHandle->getDbiRef(), os);
    SAFE_POINT_OP(os, SharedDbiDataHandler());

    return SharedDbiDataHandler(new DbiDataHandler(entityRef, connection->dbi->getObjectDbi(), true));
}

SharedDbiDataHandler DbiDataStorage::putAlignment(const MultipleSequenceAlignment &al) {
    assert(NULL != dbiHandle);

    U2OpStatus2Log os;
    MultipleSequenceAlignment copiedAlignment = al->getCopy();
    QScopedPointer<MultipleSequenceAlignmentObject> obj(MultipleSequenceAlignmentImporter::createAlignment(dbiHandle->getDbiRef(), copiedAlignment, os));
    CHECK_OP(os, SharedDbiDataHandler());

    DbiConnection *connection = this->getConnection(dbiHandle->getDbiRef(), os);
    CHECK_OP(os, SharedDbiDataHandler());

    SharedDbiDataHandler handler(new DbiDataHandler(obj->getEntityRef(), connection->dbi->getObjectDbi(), true));

    return handler;
}

SharedDbiDataHandler DbiDataStorage::putAnnotationTable(const QList<SharedAnnotationData> &anns, const QString annTableName) {
    SAFE_POINT(NULL != dbiHandle, "Invalid DBI handle!", SharedDbiDataHandler());

    AnnotationTableObject obj(annTableName, dbiHandle->getDbiRef());
    U2OpStatusImpl os;
    obj.addAnnotations(anns);
    SAFE_POINT_OP(os, SharedDbiDataHandler());

    U2EntityRef ent = obj.getEntityRef();

    DbiConnection *connection = this->getConnection(dbiHandle->getDbiRef(), os);
    SAFE_POINT_OP(os, SharedDbiDataHandler());

    SharedDbiDataHandler handler(new DbiDataHandler(ent, connection->dbi->getObjectDbi(), true));

    return handler;
}

SharedDbiDataHandler DbiDataStorage::putAnnotationTable(AnnotationTableObject *annTable) {
    SAFE_POINT(NULL != dbiHandle, "Invalid DBI handle!", SharedDbiDataHandler());
    SAFE_POINT(NULL != annTable, L10N::nullPointerError("annotation table object"), SharedDbiDataHandler());

    U2OpStatusImpl os;

    U2EntityRef entityRef = annTable->getEntityRef();
    if (annTable->getEntityRef().dbiRef != dbiHandle->getDbiRef()) {
        QScopedPointer<AnnotationTableObject> clonedTable(qobject_cast<AnnotationTableObject *>(annTable->clone(dbiHandle->getDbiRef(), os)));
        SAFE_POINT_OP(os, SharedDbiDataHandler());
        entityRef = clonedTable->getEntityRef();
    }

    DbiConnection *connection = getConnection(dbiHandle->getDbiRef(), os);
    SAFE_POINT_OP(os, SharedDbiDataHandler());

    return SharedDbiDataHandler(new DbiDataHandler(entityRef, connection->dbi->getObjectDbi(), true));
}

QVariantList DbiDataStorage::putAnnotationTables(QList<AnnotationTableObject *> annTables) {
    QVariantList result;
    foreach (AnnotationTableObject *annTable, annTables) {
        result << QVariant::fromValue(putAnnotationTable(annTable));
    }
    return result;
}

bool DbiDataStorage::deleteObject(const U2DataId &, const U2DataType &) {
    assert(NULL != dbiHandle);
    return true;
}

SharedDbiDataHandler DbiDataStorage::getDataHandler(const U2EntityRef &entRef, bool useGC) {
    U2OpStatusImpl os;
    DbiConnection *connection = this->getConnection(entRef.dbiRef, os);
    CHECK_OP(os, SharedDbiDataHandler());

    DbiDataHandler *handler = new DbiDataHandler(entRef, connection->dbi->getObjectDbi(), useGC);

    return SharedDbiDataHandler(handler);
}

DbiConnection *DbiDataStorage::getConnection(const U2DbiRef &dbiRef, U2OpStatus &os) {
    // TODO: mutex
    if (connections.contains(dbiRef.dbiId)) {
        return connections[dbiRef.dbiId];
    } else {
        QScopedPointer<DbiConnection> connection(new DbiConnection(dbiRef, os));
        CHECK_OP(os, NULL);

        connections[dbiRef.dbiId] = connection.data();
        return connection.take();
    }
}

U2DbiRef DbiDataStorage::createTmpDbi(U2OpStatus &os) {
    // TODO: mutex
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();

    U2DbiRef dbiRef;
    dbiRef.dbiId = GUrlUtils::prepareTmpFileLocation(tmpDirPath, WORKFLOW_SESSION_TMP_DBI_ALIAS, "ugenedb", os);
    dbiRef.dbiFactoryId = DEFAULT_DBI_ID;
    SAFE_POINT_OP(os, U2DbiRef());

    SAFE_POINT(!dbiList.contains(dbiRef.dbiId),
               QString("Temp dbi already exists: %1").arg(dbiRef.dbiId), dbiRef);

    QScopedPointer<DbiConnection> con(new DbiConnection(dbiRef, true, os));
    SAFE_POINT_OP(os, U2DbiRef());

    dbiList[dbiRef.dbiId] = true;
    connections[dbiRef.dbiId] = con.take();

    return dbiRef;
}

void DbiDataStorage::openDbi(const U2DbiRef &dbiRef, U2OpStatus &os) {
    QScopedPointer<DbiConnection> con(new DbiConnection(dbiRef, false, os));
    CHECK_OP(os,);

    dbiList[dbiRef.dbiId] = false;
    connections[dbiRef.dbiId] = con.take();
}

/************************************************************************/
/* StorageUtils */
/************************************************************************/
U2SequenceObject *StorageUtils::getSequenceObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler) {
    CHECK(NULL != handler.constData(), NULL);
    //QScopedPointer<U2Sequence> seqDbi(dynamic_cast<U2Sequence*>(storage->getObject(handler, U2Type::Sequence)));
    QScopedPointer<U2Sequence> seqDbi(dynamic_cast<U2Sequence*>(storage->getObject(handler, 1)));
    CHECK(NULL != seqDbi.data(), NULL);

    U2EntityRef ent(handler->getDbiRef(), seqDbi->id);
    return new U2SequenceObject(seqDbi->visualName, ent);
}

VariantTrackObject *StorageUtils::getVariantTrackObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler) {
    CHECK(NULL != handler.constData(), NULL);
    //QScopedPointer<U2VariantTrack> track(dynamic_cast<U2VariantTrack*>(storage->getObject(handler, U2Type::VariantTrack)));
    QScopedPointer<U2VariantTrack> track(dynamic_cast<U2VariantTrack*>(storage->getObject(handler, 5)));
    CHECK(NULL != track.data(), NULL);

    U2EntityRef trackRef(handler->getDbiRef(), track->id);
    QString objName = track->sequenceName;

    return new VariantTrackObject(objName, trackRef);
}

AssemblyObject *StorageUtils::getAssemblyObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler) {
    CHECK(NULL != handler.constData(), NULL);
    //QScopedPointer<U2Assembly> assembly(dynamic_cast<U2Assembly*>(storage->getObject(handler, U2Type::Assembly)));
    QScopedPointer<U2Assembly> assembly(dynamic_cast<U2Assembly*>(storage->getObject(handler, 4)));
    CHECK(NULL != assembly.data(), NULL);

    U2EntityRef assemblyRef(handler->getDbiRef(), assembly->id);
    QString objName = assembly->visualName;

    return new AssemblyObject(objName, assemblyRef);
}

MultipleSequenceAlignmentObject *StorageUtils::getMsaObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler) {
    CHECK(NULL != handler.constData(), NULL);
    //QScopedPointer<U2Ma> msa(dynamic_cast<U2Ma*>(storage->getObject(handler, U2Type::Msa)));
    QScopedPointer<U2Msa> msa(dynamic_cast<U2Msa*>(storage->getObject(handler, 2)));
    CHECK(NULL != msa.data(), NULL);

    U2EntityRef msaRef(handler->getDbiRef(), msa->id);
    QString objName = msa->visualName;

    return new MultipleSequenceAlignmentObject(objName, msaRef);
}

AnnotationTableObject *StorageUtils::getAnnotationTableObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler) {
    CHECK(NULL != handler, NULL);
    QScopedPointer<U2AnnotationTable> annotationTable(dynamic_cast<U2AnnotationTable *>(storage->getObject(handler, 10 /*U2Type::AnnotationTable*/)));
    CHECK(NULL != annotationTable, NULL);

    U2EntityRef annotationTableRef(handler->getDbiRef(), annotationTable->id);
    QString objName = annotationTable->visualName;

    return new AnnotationTableObject(objName, annotationTableRef);
}

QList<AnnotationTableObject *> StorageUtils::getAnnotationTableObjects(DbiDataStorage *storage, const QList<SharedDbiDataHandler> &handlers) {
    QList<AnnotationTableObject *> result;
    foreach (const SharedDbiDataHandler &handler, handlers) {
        AnnotationTableObject *annTable = getAnnotationTableObject(storage, handler);
        if (Q_UNLIKELY(NULL == annTable)) {
            coreLog.error(L10N::internalError("an unexpected object, skip the object"));
            Q_ASSERT(false);
            continue;
        }
        result << annTable;
    }
    return result;
}

QList<AnnotationTableObject *> StorageUtils::getAnnotationTableObjects(DbiDataStorage *storage, const QVariant &packedHandlers) {
    const QList<SharedDbiDataHandler> handlers = getAnnotationTableHandlers(packedHandlers);
    return getAnnotationTableObjects(storage, handlers);
}

QList<SharedAnnotationData> StorageUtils::getAnnotationTable(DbiDataStorage *storage, const QVariant &annObjList) {
    QList<SharedAnnotationData> result;
    const QList<SharedDbiDataHandler> handlers = getAnnotationTableHandlers(annObjList);

    foreach (const SharedDbiDataHandler &annTableId, handlers) {
        //U2Object *dbObject = storage->getObject(annTableId, U2Type::AnnotationTable);
        U2Object *dbObject = storage->getObject(annTableId, 10);
        QScopedPointer<U2AnnotationTable> table(dynamic_cast<U2AnnotationTable *>(dbObject));
        SAFE_POINT(NULL != table, "Invalid annotation table object referenced!", result);

        U2EntityRef tableRef(annTableId->getDbiRef(), table->id);
        QString objName = table->visualName;

        AnnotationTableObject annTableObj(objName, tableRef);
        foreach (Annotation *a, annTableObj.getAnnotations()) {
            result << a->getData();
        }
    }
    return result;
}

QList<SharedDbiDataHandler> StorageUtils::getAnnotationTableHandlers(const QVariant &annObjList) {
    QList<SharedDbiDataHandler> result;

    QVariantList objectList;
    if (annObjList.canConvert<QVariantList>()) {
        const QVariantList packedHandlers = annObjList.toList();
        foreach (const QVariant &packedHandler, packedHandlers) {
            if (packedHandler.canConvert<SharedDbiDataHandler>()) {
                objectList << packedHandler;
            }
        }
    } else if (annObjList.canConvert<SharedDbiDataHandler>()) {
        objectList << annObjList;
    } else {
        return result;
    }
    CHECK(!objectList.isEmpty(), result);

    foreach (const QVariant &varObj, objectList) {
        const SharedDbiDataHandler annTableId = varObj.value<SharedDbiDataHandler>();
        SAFE_POINT(NULL != annTableId.constData(), "Invalid annotation table object reference!", result);
        result << annTableId;
    }

    return result;
}

QString StorageUtils::getText(DbiDataStorage *storage, const QVariant &data) {
    if (data.canConvert<SharedDbiDataHandler>()) {
        SharedDbiDataHandler handler = data.value<SharedDbiDataHandler>();
        //QScopedPointer<U2RawData> rawData(dynamic_cast<U2RawData*>(storage->getObject(handler, RawData)));
        QScopedPointer<U2RawData> rawData(dynamic_cast<U2RawData*>(storage->getObject(handler, 102)));
        CHECK(NULL != rawData.data(), "");

        U2EntityRef objRef(storage->getDbiRef(), rawData->id);
        TextObject textObj(rawData->visualName, objRef);
        return textObj.getText();
    } else if (data.canConvert<QString>()) {
        return data.toString();
    }
    SAFE_POINT(true, "Unexpected data type", "");
    return "";
}

} // Workflow
} // U2
