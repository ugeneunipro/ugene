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

#include <U2Core/AssemblyImporter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AssemblyObject.h"

namespace U2 {

AssemblyObject::AssemblyObject(const QString& objectName, const U2EntityRef& ref, const QVariantMap& hints)
    : GObject(GObjectTypes::ASSEMBLY, objectName, hints)
{
    this->entityRef = ref;
}

GObject* AssemblyObject::clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    U2EntityRef dstEntityRef = AssemblyObject::dbi2dbiClone(this, dstDbiRef, os, gHints.getMap());
    CHECK_OP(os, NULL);
    AssemblyObject *dstObj = new AssemblyObject(this->getGObjectName(), dstEntityRef, gHints.getMap());

    return dstObj;
}

class CloneInfo : public U2AssemblyReadsImportInfo {
public:
    CloneInfo(qint64 readsCount, U2OpStatus &os)
    : U2AssemblyReadsImportInfo(NULL), os(os), readsCount(readsCount), addedCount(0), currentChunkSize(0)
    {
        chunkSize = readsCount / 100;
    }

    void onReadImported() {
        addedCount++;
        currentChunkSize++;
        if (currentChunkSize >= chunkSize) {
            os.setProgress(100 * addedCount / readsCount);
            currentChunkSize = 0;
        }
    }

private:
    U2OpStatus &os;
    qint64 readsCount;
    qint64 addedCount;
    qint64 currentChunkSize;
    qint64 chunkSize;
};

void copyReadsUnrelatedAttributes(const U2DataId &srcObjId, const U2DataId &dstObjId, U2AttributeDbi *srcAttributeDbi, U2AttributeDbi *dstAttributeDbi, U2OpStatus &os) {
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

    const QStringList attributesNotToCopy = U2BaseAttributeName::getReadsRelatedAttributes();

    QList<U2DataId> attrIds = srcAttributeDbi->getObjectAttributes(srcObjId, "", os);
    CHECK_OP(os, );

    foreach(const U2DataId &attrId, attrIds) {
        U2DataType attrType = srcDbi->getEntityTypeById(attrId);
        if (U2Type::AttributeInteger == attrType) {
            U2IntegerAttribute attr = srcAttributeDbi->getIntegerAttribute(attrId, os);
            CHECK_OP(os, );
            if (attributesNotToCopy.contains(attr.name)) {
                continue;
            }
            attr.objectId = dstObjId;
            dstAttributeDbi->createIntegerAttribute(attr, os);
        } else if (U2Type::AttributeReal == attrType) {
            U2RealAttribute attr = srcAttributeDbi->getRealAttribute(attrId, os);
            CHECK_OP(os, );
            if (attributesNotToCopy.contains(attr.name)) {
                continue;
            }
            attr.objectId = dstObjId;
            dstAttributeDbi->createRealAttribute(attr, os);
        } else if (U2Type::AttributeString == attrType) {
            U2StringAttribute attr = srcAttributeDbi->getStringAttribute(attrId, os);
            CHECK_OP(os, );
            if (attributesNotToCopy.contains(attr.name)) {
                continue;
            }
            attr.objectId = dstObjId;
            dstAttributeDbi->createStringAttribute(attr, os);
        } else if (U2Type::AttributeByteArray == attrType) {
            U2ByteArrayAttribute attr = srcAttributeDbi->getByteArrayAttribute(attrId, os);
            CHECK_OP(os, );
            if (attributesNotToCopy.contains(attr.name)) {
                continue;
            }
            attr.objectId = dstObjId;
            dstAttributeDbi->createByteArrayAttribute(attr, os);
        }
        CHECK_OP(os, );
    }
}

U2EntityRef AssemblyObject::dbi2dbiExtractRegion(const AssemblyObject *const srcObj, const U2DbiRef &dstDbiRef, U2OpStatus &os,
    const U2Region &desiredRegion, const QVariantMap &hints)
{
    U2DbiRef srcDbiRef = srcObj->getEntityRef().dbiRef;
    U2DataId srcObjId = srcObj->getEntityRef().entityId;
    const QString dstFolder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    DbiConnection dstCon(dstDbiRef, true, os);
    CHECK_OP(os, U2EntityRef());
    DbiConnection srcCon(srcDbiRef, os);
    CHECK_OP(os, U2EntityRef());

    U2ObjectDbi *dstObjectDbi = dstCon.dbi->getObjectDbi();
    U2ObjectDbi *srcObjectDbi = srcCon.dbi->getObjectDbi();
    SAFE_POINT_EXT(NULL != dstObjectDbi, os.setError("NULL destination object dbi"), U2EntityRef());
    SAFE_POINT_EXT(NULL != srcObjectDbi, os.setError("NULL source object dbi"), U2EntityRef());

    U2AssemblyDbi *dstAssemblyDbi = dstCon.dbi->getAssemblyDbi();
    U2AssemblyDbi *srcAssemblyDbi = srcCon.dbi->getAssemblyDbi();
    SAFE_POINT_EXT(NULL != dstAssemblyDbi, os.setError("NULL destination assembly dbi"), U2EntityRef());
    SAFE_POINT_EXT(NULL != srcAssemblyDbi, os.setError("NULL source assembly dbi"), U2EntityRef());

    // prepare reads
    CHECK_OP(os, U2EntityRef());
    qint64 readsCount = srcAssemblyDbi->countReads(srcObjId, desiredRegion, os);
    CHECK_OP(os, U2EntityRef());
    U2DbiIterator<U2AssemblyRead> *iter = srcAssemblyDbi->getReads(srcObjId, desiredRegion, os, true);
    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iterPtr(iter);
    CHECK_OP(os, U2EntityRef());
    Q_UNUSED(iterPtr);

    // copy object
    U2Assembly assembly;
    assembly.visualName = srcObj->getGObjectName();
    CloneInfo info(readsCount, os);

    AssemblyImporter importer(os);
    importer.createAssembly(dstDbiRef, dstFolder, iter, info, assembly);
    CHECK_OP(os, U2EntityRef());

    // copy attributes
    U2AttributeDbi *srcAttributeDbi = srcCon.dbi->getAttributeDbi();
    U2AttributeDbi *dstAttributeDbi = dstCon.dbi->getAttributeDbi();
    if (desiredRegion == U2_REGION_MAX) {
        U2AttributeUtils::copyObjectAttributes(srcObjId, assembly.id, srcAttributeDbi, dstAttributeDbi, os);
    } else {
        copyReadsUnrelatedAttributes(srcObjId, assembly.id, srcAttributeDbi, dstAttributeDbi, os);
    }
    CHECK_OP(os, U2EntityRef());

    U2EntityRef dstEntityRef(dstDbiRef, assembly.id);

    return dstEntityRef;
}

U2EntityRef AssemblyObject::dbi2dbiClone(const AssemblyObject *const srcObj, const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints) {
    return dbi2dbiExtractRegion(srcObj, dstDbiRef, os, U2_REGION_MAX, hints);
}



}//namespace
