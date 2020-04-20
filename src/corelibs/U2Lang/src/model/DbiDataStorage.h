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

#ifndef _WORKFLOW_DBI_DATA_STORAGE_H_
#define _WORKFLOW_DBI_DATA_STORAGE_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/VariantTrackObject.h>

#include <U2Lang/DbiDataHandler.h>

namespace U2 {

class AnnotationTableObject;

namespace Workflow {

class U2LANG_EXPORT DbiDataStorage {
public:
    DbiDataStorage();
    virtual ~DbiDataStorage();

    bool init();
    U2DbiRef getDbiRef();

    /* NOTE: deallocate memory! */
    virtual U2Object *getObject(const SharedDbiDataHandler &handler, const U2DataType &type);
    virtual SharedDbiDataHandler putSequence(const DNASequence &sequence);
    virtual SharedDbiDataHandler putSequence(const U2SequenceObject *sequenceObject);
    virtual SharedDbiDataHandler putAlignment(const MultipleSequenceAlignment &al);
    virtual SharedDbiDataHandler putAnnotationTable(const QList<SharedAnnotationData> &anns, const QString annTableName = "Annotations");
    virtual SharedDbiDataHandler putAnnotationTable(AnnotationTableObject *annTable);

    // returns QVariant with QVariantList inside, which have SharedDbiDataHandler in each QVariant
    virtual QVariantList putAnnotationTables(QList<AnnotationTableObject *> annTables);

    virtual bool deleteObject(const U2DataId &objectId, const U2DataType &type);

    /* If @useGC is true then the data will be removed from the database by GC */
    virtual SharedDbiDataHandler getDataHandler(const U2EntityRef &entRef, bool useGC = true);

    U2DbiRef createTmpDbi(U2OpStatus &os);
    void openDbi(const U2DbiRef &dbiRef, U2OpStatus &os);

private:
    DbiDataStorage(const DbiDataStorage &) {}
    TmpDbiHandle *dbiHandle;
    QMap<U2DbiId, DbiConnection*> connections;
    /* DbiRef <-> temporary */
    QMap<U2DbiId, bool> dbiList;

    DbiConnection *getConnection(const U2DbiRef &dbiRef, U2OpStatus &os);
};

class U2LANG_EXPORT StorageUtils {
public:
    /**
     * Memory should be deallocated by caller
     * Do not forget to move the object to thread you need
     **/
    static U2SequenceObject *getSequenceObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler);
    static VariantTrackObject *getVariantTrackObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler);
    static AssemblyObject *getAssemblyObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler);
    static MultipleSequenceAlignmentObject *getMsaObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler);

    static AnnotationTableObject *getAnnotationTableObject(DbiDataStorage *storage, const SharedDbiDataHandler &handler);
    static QList<AnnotationTableObject *> getAnnotationTableObjects(DbiDataStorage *storage, const QList<SharedDbiDataHandler> &handlers);
    static QList<AnnotationTableObject *> getAnnotationTableObjects(DbiDataStorage *storage, const QVariant &packedHandlers);
    static QList<SharedAnnotationData> getAnnotationTable(DbiDataStorage *storage, const QVariant &annObjList);
    static QList<SharedDbiDataHandler> getAnnotationTableHandlers(const QVariant &annObjList);

    static QString getText(DbiDataStorage *storage, const QVariant &data);
};

} // Workflow

} // U2

#endif // _WORKFLOW_DBI_DATA_STORAGE_H_
