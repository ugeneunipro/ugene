/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QStack>

#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/Version.h>

struct sqlite3;

namespace U2 {

class DbRef;
class SQLiteAssemblyDbi;
class SQLiteAttributeDbi;
class SQLiteCrossDatabaseReferenceDbi;
class SQLiteFeatureDbi;
class SQLiteModDbi;
class SQLiteMsaDbi;
class SQLiteObjectDbi;
class SQLiteObjectRelationsDbi;
class SQLiteSequenceDbi;
class SQLiteUdrDbi;
class SQLiteVariantDbi;

/** Name of the init property used to indicate assembly reads storage method for all new assemblies */
#define SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY "sqlite-assembly-reads-elen-method"

/** Stores all reads in a single table. Not optimal if read effective length varies */
#define SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE "single-table"

/** Store all reads in N tables sorted by effective read length */
#define SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1 "multi-table-v1"

/** No compression is applied. Best for manual DB browsing  (default)*/
#define SQLITE_DBI_ASSEMBLY_READ_COMPRESSION_METHOD_NO_COMPRESSION "no-compression"

class U2FORMATS_EXPORT SQLiteDbi : public U2AbstractDbi {
public:
    SQLiteDbi();
    ~SQLiteDbi();

    /**
     * Minimal version of UGENE that is compatible with the current SQLite DBI implementation.
     * if (MIN_COMPATIBLE_UGENE_VERSION < active UGENE version ) UGENE will be able to open ".ugenedb" file.
     */
    static const Version MIN_COMPATIBLE_UGENE_VERSION;

    /**
    Boots the database up to functional state.
    Can be called again after successful shutdown, to re-load the database.
    special flags
    "assembly-reads-compression=1" - enables reads compression for assembly. Affects new DB only
    */
    void init(const QHash<QString, QString>& properties, const QVariantMap& persistentData, U2OpStatus& os) override;

    /** Stops the database and frees up used resources. */
    QVariantMap shutdown(U2OpStatus& os) override;

    /**
    Ensures that dbi state is synchronized with storage
    Return 'true' of operation is successful
    */
    bool flush(U2OpStatus& os) override;

    /** Unique database id. Used for cross-database references. */
    QString getDbiId() const override;

    bool isInitialized(U2OpStatus& os) override;

    void populateDefaultSchema(U2OpStatus& os) override;

    /** Returns database meta-info. Any set of properties to be shown to user */
    QHash<QString, QString> getDbiMetaInfo(U2OpStatus& os) override;

    /** Returns type of the entity referenced by the given ID */
    U2DataType getEntityTypeById(const U2DataId& id) const override;

    U2ObjectDbi* getObjectDbi() override;

    U2ObjectRelationsDbi* getObjectRelationsDbi() override;

    U2SequenceDbi* getSequenceDbi() override;

    U2MsaDbi* getMsaDbi() override;

    U2AssemblyDbi* getAssemblyDbi() override;

    U2CrossDatabaseReferenceDbi* getCrossDatabaseReferenceDbi() override;

    U2AttributeDbi* getAttributeDbi() override;

    U2VariantDbi* getVariantDbi() override;

    U2FeatureDbi* getFeatureDbi() override;

    U2ModDbi* getModDbi() override;

    UdrDbi* getUdrDbi() override;

    DbRef* getDbRef() const {
        return db;
    }

    SQLiteObjectDbi* getSQLiteObjectDbi() const;

    SQLiteObjectRelationsDbi* getSQLiteObjectRelationsDbi() const;

    SQLiteMsaDbi* getSQLiteMsaDbi() const;

    SQLiteSequenceDbi* getSQLiteSequenceDbi() const;

    SQLiteModDbi* getSQLiteModDbi() const;

    SQLiteUdrDbi* getSQLiteUdrDbi() const;

    SQLiteFeatureDbi* getSQLiteFeatureDbi() const;

    /** Returns properties used to initialized the database */
    QHash<QString, QString> getInitProperties() const override {
        return initProperties;
    }

    QString getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os) override;

    void setProperty(const QString& name, const QString& value, U2OpStatus& os) override;

    void startOperationsBlock(U2OpStatus& os) override;

    void stopOperationBlock(U2OpStatus& os) override;

    QMutex* getDbMutex() const override;

    bool isReadOnly() const override;

    bool isTransactionActive() const override;

    static const int BIND_PARAMETERS_LIMIT;

private:
    QString getLastErrorMessage(int rc);

    void setState(U2DbiState state);

    void internalInit(const QHash<QString, QString>& props, U2OpStatus& os);

    void testDatabaseLocked(U2OpStatus& os);

    QString url;
    DbRef* db;

    SQLiteObjectDbi* objectDbi;
    SQLiteObjectRelationsDbi* objectRelationsDbi;
    SQLiteSequenceDbi* sequenceDbi;
    SQLiteMsaDbi* msaDbi;
    SQLiteAssemblyDbi* assemblyDbi;
    SQLiteCrossDatabaseReferenceDbi* crossDbi;
    SQLiteAttributeDbi* attributeDbi;
    SQLiteVariantDbi* variantDbi;
    SQLiteFeatureDbi* featureDbi;
    SQLiteModDbi* modDbi;
    SQLiteUdrDbi* udrDbi;

    QStack<SQLiteTransaction*> operationsBlockTransactions;

    friend class SQLiteObjectDbi;
    friend class SQLiteCrossDatabaseReferenceDbi;
    friend class SQLiteSequenceDbi;
    friend class SQLiteAssemblyDbi;
    friend class SQLiteMsaDbi;
    friend class SQLiteAttributeDbi;
};

class U2FORMATS_EXPORT SQLiteDbiFactory : public U2DbiFactory {
public:
    SQLiteDbiFactory();

    /** Creates new DBI instance */
    U2Dbi* createDbi() override;

    /** Returns DBI type ID */
    U2DbiFactoryId getId() const override;

    /** Checks that data pointed by properties is a valid DBI resource */
    FormatCheckResult isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& os) const override;

    GUrl id2Url(const U2DbiId& id) const override {
        return GUrl(id, GUrl_File);
    }

    bool isDbiExists(const U2DbiId& id) const override;

public:
    static const U2DbiFactoryId ID;
};

/** helper class, used as a base for all SQLite<child>Dbis */
class SQLiteChildDBICommon {
public:
    SQLiteChildDBICommon(SQLiteDbi* dbi)
        : dbi(dbi), db(dbi->getDbRef()) {
    }
    virtual ~SQLiteChildDBICommon() {
    }

    virtual void initSqlSchema(U2OpStatus& os) = 0;
    virtual void shutdown(U2OpStatus&) {
    }

protected:
    SQLiteDbi* dbi;
    DbRef* db;
};

}  // namespace U2
