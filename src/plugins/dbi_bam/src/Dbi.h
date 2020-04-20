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

#ifndef _U2_BAM_DBI_H_
#define _U2_BAM_DBI_H_

#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2SqlHelpers.h>

#include "Reader.h"

namespace U2 {
namespace BAM {

class ObjectDbi;
class AssemblyDbi;

class Dbi : public U2AbstractDbi {
public:
    Dbi();

    virtual void init(const QHash<QString, QString> &properties, const QVariantMap &persistentData, U2OpStatus &os);

    virtual QVariantMap shutdown(U2OpStatus &os);

    virtual QHash<QString, QString> getDbiMetaInfo(U2OpStatus &);

    virtual U2DataType getEntityTypeById(const U2DataId& id) const;

    virtual U2ObjectDbi *getObjectDbi();

    virtual U2AssemblyDbi *getAssemblyDbi();

    virtual bool isReadOnly() const;

private:
    void buildIndex(U2OpStatus &os);

private:
    static const int COLUMN_DISTANCE = 100;

    GUrl url;
    GUrl sqliteUrl;
    DbRef dbRef;
    int assembliesCount;
    QScopedPointer<IOAdapter> ioAdapter;
    QScopedPointer<BamReader> reader;
    QScopedPointer<ObjectDbi> objectDbi;
    QScopedPointer<AssemblyDbi> assemblyDbi;
};

class DbiFactory : public U2DbiFactory {
public:
    DbiFactory();

    virtual U2Dbi *createDbi();

    virtual U2DbiFactoryId getId()const;

    virtual FormatCheckResult isValidDbi(const QHash<QString, QString> &properties, const QByteArray &rawData, U2OpStatus &os) const;

    virtual GUrl id2Url(const U2DbiId& id) const {return GUrl(id, GUrl_File);}

    virtual bool isDbiExists(const U2DbiId& id) const;

public:
    static const QString ID;
};

class ObjectDbi : public U2SimpleObjectDbi {
public:
    ObjectDbi(Dbi &dbi, DbRef &dbRef, int assembliesCount);

    virtual qint64 countObjects(U2OpStatus &os);

    virtual qint64 countObjects(U2DataType type, U2OpStatus &os);

    virtual QHash<U2DataId, QString> getObjectNames(qint64 offset, qint64 count, U2OpStatus &os);

    virtual QList<U2DataId> getObjects(qint64 offset, qint64 count, U2OpStatus &os);

    virtual QList<U2DataId> getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus &os);

    virtual QList<U2DataId> getParents(const U2DataId& entityId, U2OpStatus &os);

    virtual QStringList getFolders(U2OpStatus &os);

    virtual qint64 countObjects(const QString &folder, U2OpStatus &os);

    virtual QList<U2DataId> getObjects(const QString &folder, qint64 offset, qint64 count, U2OpStatus &os);

    virtual QStringList getObjectFolders(const U2DataId& objectId, U2OpStatus &os);

    virtual qint64 getObjectVersion(const U2DataId& objectId, U2OpStatus &os);

    virtual qint64 getFolderLocalVersion(const QString &folder, U2OpStatus &os);

    virtual qint64 getFolderGlobalVersion(const QString &folder, U2OpStatus &os);

    virtual U2DbiIterator<U2DataId>* getObjectsByVisualName(const QString& visualName, U2DataType type, U2OpStatus& os);

    virtual void renameObject(const U2DataId &id, const QString &newName, U2OpStatus &os);

    virtual void setObjectRank(const U2DataId &objectId, U2DbiObjectRank newRank, U2OpStatus &os);

    virtual U2DbiObjectRank getObjectRank(const U2DataId &objectId, U2OpStatus& os);

    virtual void setParent(const U2DataId& parentId, const U2DataId& childId, U2OpStatus& os);

private:
    Dbi &dbi;
    DbRef &dbRef;
    int assembliesCount;
};

class AssemblyDbi : public U2SimpleAssemblyDbi {
public:
    AssemblyDbi(Dbi &dbi, BamReader &reader, DbRef &dbRef, QList<qint64> maxReadLengths);

    virtual U2Assembly getAssemblyObject(const U2DataId& id, U2OpStatus &os);

    virtual qint64 countReads(const U2DataId& assemblyId, const U2Region &r, U2OpStatus &os);

    virtual U2DbiIterator<U2AssemblyRead>* getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint = false);

    virtual U2DbiIterator<U2AssemblyRead>* getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os);

    virtual U2DbiIterator<U2AssemblyRead>* getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os);

    virtual qint64 getMaxPackedRow(const U2DataId& assemblyId, const U2Region &r, U2OpStatus &os);

    virtual qint64 getMaxEndPos(const U2DataId& assemblyId, U2OpStatus &os);

    static U2AssemblyRead alignmentToRead(const Alignment &alignment);

    U2AssemblyRead getReadById(const U2DataId& rowId, U2OpStatus &os);


private:
    qint64 getMaxReadLength(const U2DataId& assemblyId, const U2Region &r);
    U2AssemblyRead getReadById(const U2DataId& rowId, qint64 packedRow, U2OpStatus &os);
    QList<U2AssemblyRead> getReadsByIds(QList<U2DataId> rowIds, QList<qint64> packedRows, U2OpStatus &os);

    Dbi &dbi;
    BamReader &reader;
    DbRef &dbRef;
    QList<qint64> maxReadLengths;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_DBI_H_
