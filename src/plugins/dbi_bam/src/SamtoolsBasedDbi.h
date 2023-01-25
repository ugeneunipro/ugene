/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_SAMTOOLS_BASED_DBI_H_
#define _U2_SAMTOOLS_BASED_DBI_H_

extern "C" {
#include <bam.h>
}

#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2SqlHelpers.h>

#include "Reader.h"

namespace U2 {
namespace BAM {

class SamtoolsBasedDbi;

class SamtoolsBasedObjectDbi : public U2SimpleObjectDbi {
public:
    SamtoolsBasedObjectDbi(SamtoolsBasedDbi& dbi, const QList<U2DataId>& assemblyObjectIds);

    virtual qint64 countObjects(U2OpStatus& os);
    virtual qint64 countObjects(U2DataType type, U2OpStatus& os);
    virtual QHash<U2DataId, QString> getObjectNames(qint64 offset, qint64 count, U2OpStatus& os);
    virtual void getObject(U2Object& object, const U2DataId& id, U2OpStatus& os);
    virtual QList<U2DataId> getObjects(qint64 offset, qint64 count, U2OpStatus& os);
    virtual QList<U2DataId> getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus& os);
    virtual QList<U2DataId> getParents(const U2DataId& entityId, U2OpStatus& os);
    virtual QStringList getFolders(U2OpStatus& os);
    virtual qint64 countObjects(const QString& folder, U2OpStatus& os);
    virtual QList<U2DataId> getObjects(const QString& folder, qint64 offset, qint64 count, U2OpStatus& os);
    virtual QStringList getObjectFolders(const U2DataId& objectId, U2OpStatus& os);
    virtual qint64 getObjectVersion(const U2DataId& objectId, U2OpStatus& os);
    virtual qint64 getFolderLocalVersion(const QString& folder, U2OpStatus& os);
    virtual qint64 getFolderGlobalVersion(const QString& folder, U2OpStatus& os);
    virtual U2DbiIterator<U2DataId>* getObjectsByVisualName(const QString& visualName, U2DataType type, U2OpStatus& os);
    virtual void renameObject(const U2DataId& id, const QString& newName, U2OpStatus& os);
    virtual void setObjectRank(const U2DataId& objectId, U2DbiObjectRank newRank, U2OpStatus& os);
    virtual U2DbiObjectRank getObjectRank(const U2DataId& objectId, U2OpStatus& os);
    virtual void setParent(const U2DataId& parentId, const U2DataId& childId, U2OpStatus& os);

private:
    SamtoolsBasedDbi& dbi;
    QList<U2DataId> assemblyObjectIds;
};  // SamtoolsBasedObjectDbi

int bamFetchFunction(const bam1_t* b, void* data);

class SamtoolsBasedReadsIterator : public U2DbiIterator<U2AssemblyRead> {
    friend int bamFetchFunction(const bam1_t* b, void* data);

public:
    SamtoolsBasedReadsIterator(int assemblyId, const U2Region& r, SamtoolsBasedDbi& dbi, const QByteArray& nameFilter = "");

    bool hasNext() override;
    U2AssemblyRead next() override;
    U2AssemblyRead peek() override;

private:
    int assemblyId = 0;
    U2Region r;
    SamtoolsBasedDbi& dbi;
    QByteArray nameFilter;

    qint64 nextPosToRead = 0;
    std::shared_ptr<BGZF> bamFile;
    QList<U2AssemblyRead> reads;
    QList<U2AssemblyRead>::Iterator current;

    QList<U2DataId> borderReadIds;
    QList<U2DataId> newBorderReadIds;

    static const int BUFFERED_INTERVAL_SIZE;

private:
    void fetchNextChunk();
    void applyNameFilter();
};  // SamtoolsBasedReadsIterator

class SamtoolsBasedAssemblyDbi : public U2SimpleAssemblyDbi {
public:
    SamtoolsBasedAssemblyDbi(SamtoolsBasedDbi& dbi);

    virtual U2Assembly getAssemblyObject(const U2DataId& id, U2OpStatus& os);
    virtual qint64 countReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os);
    virtual U2DbiIterator<U2AssemblyRead>* getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint = false);
    virtual qint64 getMaxEndPos(const U2DataId& assemblyId, U2OpStatus& os);
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os);

    static int toSamtoolsId(const U2DataId& assemblyId, U2OpStatus& os);

    /**
     * Unsupported methods
     */
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os);
    virtual qint64 getMaxPackedRow(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os);

private:
    SamtoolsBasedDbi& dbi;

    U2Region getCorrectRegion(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os);
};  // SamtoolsBasedAssemblyDbi

class SamtoolsBasedAttributeDbi : public U2SimpleAttributeDbi {
public:
    SamtoolsBasedAttributeDbi(SamtoolsBasedDbi& dbi);

    virtual QStringList getAvailableAttributeNames(U2OpStatus& os);
    virtual QList<U2DataId> getObjectAttributes(const U2DataId& objectId, const QString& attributeName, U2OpStatus& os);
    virtual QList<U2DataId> getObjectPairAttributes(const U2DataId& bjectId, const U2DataId& childId, const QString& attributeName, U2OpStatus& os);
    virtual U2IntegerAttribute getIntegerAttribute(const U2DataId& attributeId, U2OpStatus& os);
    virtual U2RealAttribute getRealAttribute(const U2DataId& attributeId, U2OpStatus& os);
    virtual U2StringAttribute getStringAttribute(const U2DataId& attributeId, U2OpStatus& os);
    virtual U2ByteArrayAttribute getByteArrayAttribute(const U2DataId& attributeId, U2OpStatus& os);

    virtual QList<U2DataId> sort(const U2DbiSortConfig& sc, qint64 offset, qint64 count, U2OpStatus& os);

private:
    SamtoolsBasedDbi& dbi;
};  // SamtoolsBasedAttributeDbi

/**
 * This DBI could be initialized to work only with sorted indexed BAM files.
 */
class SamtoolsBasedDbi : public U2AbstractDbi {
public:
    SamtoolsBasedDbi();
    ~SamtoolsBasedDbi() override;

    void init(const QHash<QString, QString>& properties, const QVariantMap& persistentData, U2OpStatus& os) override;
    QVariantMap shutdown(U2OpStatus& os) override;
    QHash<QString, QString> getDbiMetaInfo(U2OpStatus&) override {
        return QHash<QString, QString>();
    }
    U2DataType getEntityTypeById(const U2DataId& id) const override;
    U2ObjectDbi* getObjectDbi() override;
    U2AssemblyDbi* getAssemblyDbi() override;
    U2AttributeDbi* getAttributeDbi() override;
    bool isReadOnly() const override;

    const bam_header_t* getHeader() const;
    const bam_index_t* getIndex() const;

    /**
     * Creates a new 'bamFile' structure for the BAM file backed by the current DBI instance.
     * The result bamFile is used as an input for various bam_* methods that may modify internal bamFile state and are not thread safe.
     * Caller is responsible for closing the file.
     */
    bamFile openNewBamFileHandler() const;

private:
    GUrl url;
    int assembliesCount;
    bam_header_t* header;
    bam_index_t* index;
    QScopedPointer<SamtoolsBasedObjectDbi> objectDbi;
    QScopedPointer<SamtoolsBasedAssemblyDbi> assemblyDbi;
    QScopedPointer<SamtoolsBasedAttributeDbi> attributeDbi;

    void createObjectDbi();
    void cleanup();
    /**
     *  Returns true if all right
     */
    bool initBamStructures(const GUrl& fileName);
};  // SamtoolsBasedDbi

class SamtoolsBasedDbiFactory : public U2DbiFactory {
public:
    SamtoolsBasedDbiFactory();

    virtual U2Dbi* createDbi();
    virtual U2DbiFactoryId getId() const;
    virtual FormatCheckResult isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& os) const;
    virtual GUrl id2Url(const U2DbiId& id) const {
        return GUrl(id, GUrl_File);
    }
    virtual bool isDbiExists(const U2DbiId& id) const;

public:
    static const QString ID;
};  // SamtoolsBasedDbiFactory

}  // namespace BAM
}  // namespace U2

#endif  // _U2_SAMTOOLS_BASED_DBI_H_
