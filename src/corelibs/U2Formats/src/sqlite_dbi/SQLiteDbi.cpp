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

#include "SQLiteDbi.h"
#include "SQLiteObjectDbi.h"
#include "SQLiteObjectRelationsDbi.h"
#include "SQLiteSequenceDbi.h"
#include "SQLiteMsaDbi.h"
#include "SQLiteAssemblyDbi.h"
#include "SQLiteAttributeDbi.h"
#include "SQLiteVariantDbi.h"
#include "SQLiteFeatureDbi.h"
#include "SQLiteModDbi.h"
#include "SQLiteUdrDbi.h"
#include "util/SqliteUpgraderFrom_0_To_1_13.h"
#include "util/SqliteUpgraderFrom_1_13_To_1_25.h"

#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/Log.h>
#include <U2Core/Version.h>
#include <U2Core/GUrl.h>

#include <QFile>

#include <3rdparty/sqlite3/sqlite3.h>

namespace U2 {

const int SQLiteDbi::BIND_PARAMETERS_LIMIT = 999;

SQLiteDbi::SQLiteDbi()
    : U2AbstractDbi (SQLiteDbiFactory::ID)
{
    db = new DbRef();
    objectDbi = new SQLiteObjectDbi(this);
    objectRelationsDbi = new SQLiteObjectRelationsDbi(this);
    sequenceDbi = new SQLiteSequenceDbi(this);
    modDbi = new SQLiteModDbi(this);
    msaDbi = new SQLiteMsaDbi(this);
    assemblyDbi = new SQLiteAssemblyDbi(this);
    crossDbi = new SQLiteCrossDatabaseReferenceDbi(this);
    attributeDbi = new SQLiteAttributeDbi(this);
    variantDbi = new SQLiteVariantDbi(this);
    featureDbi = new SQLiteFeatureDbi(this);
    udrDbi = new SQLiteUdrDbi(this);

    upgraders << new SqliteUpgraderFrom_0_To_1_13(this);
    upgraders << new SqliteUpgraderFrom_1_13_To_1_25(this);
}

SQLiteDbi::~SQLiteDbi() {
    SAFE_POINT( NULL == db->handle, "Invalid DB handle detected!", );

    delete udrDbi;
    delete objectDbi;
    delete objectRelationsDbi;
    delete sequenceDbi;
    delete msaDbi;
    delete variantDbi;
    delete assemblyDbi;
    delete crossDbi;
    delete attributeDbi;
    delete featureDbi;
    delete modDbi;
    delete db;
}

U2ObjectDbi* SQLiteDbi::getObjectDbi()  {
    return objectDbi;
}

U2ObjectRelationsDbi* SQLiteDbi::getObjectRelationsDbi() {
    return objectRelationsDbi;
}

U2SequenceDbi* SQLiteDbi::getSequenceDbi()  {
    return sequenceDbi;
}

U2MsaDbi* SQLiteDbi::getMsaDbi() {
    return msaDbi;
}

U2AssemblyDbi* SQLiteDbi::getAssemblyDbi()  {
    return assemblyDbi;
}

U2CrossDatabaseReferenceDbi* SQLiteDbi::getCrossDatabaseReferenceDbi()  {
    return crossDbi;
}

U2AttributeDbi* SQLiteDbi::getAttributeDbi() {
    return attributeDbi;
}

U2VariantDbi* SQLiteDbi::getVariantDbi() {
    return variantDbi;
}

U2FeatureDbi * SQLiteDbi::getFeatureDbi( ) {
    return featureDbi;
}

U2ModDbi* SQLiteDbi::getModDbi() {
    return modDbi;
}

UdrDbi* SQLiteDbi::getUdrDbi() {
    return udrDbi;
}

SQLiteObjectDbi* SQLiteDbi::getSQLiteObjectDbi() const {
    return objectDbi;
}

SQLiteObjectRelationsDbi *SQLiteDbi::getSQLiteObjectRelationsDbi() const {
    return objectRelationsDbi;
}

SQLiteMsaDbi* SQLiteDbi::getSQLiteMsaDbi() const {
    return msaDbi;
}

SQLiteSequenceDbi* SQLiteDbi::getSQLiteSequenceDbi() const {
    return sequenceDbi;
}

SQLiteModDbi* SQLiteDbi::getSQLiteModDbi() const {
    return modDbi;
}

SQLiteUdrDbi* SQLiteDbi::getSQLiteUdrDbi() const {
    return udrDbi;
}

SQLiteFeatureDbi* SQLiteDbi::getSQLiteFeatureDbi() const {
    return featureDbi;
}

QString SQLiteDbi::getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT value FROM Meta WHERE name = ?1", db, os);
    q.bindString(1, name);
    bool found = q.step();
    if (os.hasError()) {
        return QString();
    }
    if (found) {
        return q.getString(0);
    }
    return defaultValue;
}

void SQLiteDbi::setProperty(const QString& name, const QString& value, U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }
    SQLiteWriteQuery q1("DELETE FROM Meta WHERE name = ?1", db, os);
    q1.bindString(1, name);
    q1.execute();

    SQLiteWriteQuery q2("INSERT INTO Meta(name, value) VALUES (?1, ?2)", db, os);
    q2.bindString(1, name);
    q2.bindString(2, value);
    q2.execute();
}

void SQLiteDbi::startOperationsBlock(U2OpStatus &os) {
    db->useCache = true;
    operationsBlockTransactions.push(new SQLiteTransaction(this->db, os));
}

void SQLiteDbi::stopOperationBlock(U2OpStatus& os) {
    SAFE_POINT_EXT(!operationsBlockTransactions.isEmpty(), os.setError("There is no transaction to delete"), );
    delete operationsBlockTransactions.pop();

    if (operationsBlockTransactions.isEmpty()) {
        db->useCache = false;
    }
}

QMutex * SQLiteDbi::getDbMutex( ) const {
    return &db->lock;
}

bool SQLiteDbi::isReadOnly() const {
    return SQLiteUtils::isDatabaseReadOnly(db, "main") == 1;
}

bool SQLiteDbi::isTransactionActive() const {
    return !db->transactionStack.isEmpty();
}

static int isEmptyCallback(void *o, int argc, char ** /*argv*/, char ** /*column*/) {
    int* res = (int*)o;
    *res = argc;
    return 0;
}

bool SQLiteDbi::isInitialized(U2OpStatus &os) {
    QByteArray showTablesQuery = "SELECT * FROM sqlite_master WHERE type='table';";
    int nTables = 0;
    char* err;
    int rc = sqlite3_exec(db->handle, showTablesQuery.constData(), isEmptyCallback, &nTables, &err);
    if (rc != SQLITE_OK) {
        os.setError(U2DbiL10n::tr("Error checking SQLite database: %1!").arg(err));
        sqlite3_free(err);
        return false;
    }
    return nTables != 0;
}

#define CT(table, fields) \
{ \
    char* err = NULL; \
    QByteArray query = QByteArray("CREATE TABLE ") + table + " (" + fields + ");";\
    int rc = sqlite3_exec(db->handle, query, NULL, NULL, &err); \
    if (rc != SQLITE_OK) { \
        os.setError(U2DbiL10n::tr("Error creating table: %1, error: %2").arg(table).arg(err)); \
        sqlite3_free(err); \
        return; \
    } \
}

void SQLiteDbi::populateDefaultSchema(U2OpStatus& os) {
    // meta table, stores general db info
    SQLiteWriteQuery("CREATE TABLE Meta(name TEXT NOT NULL, value TEXT NOT NULL)", db, os).execute();

    objectDbi->initSqlSchema(os);
    objectRelationsDbi->initSqlSchema(os);
    sequenceDbi->initSqlSchema(os);
    msaDbi->initSqlSchema(os);
    assemblyDbi->initSqlSchema(os);
    crossDbi->initSqlSchema(os);
    attributeDbi->initSqlSchema(os);
    variantDbi->initSqlSchema(os);
    featureDbi->initSqlSchema(os);
    modDbi->initSqlSchema(os);
    udrDbi->initSqlSchema(os);

    setVersionProperties(Version::minVersionForSQLite(), os);
}

void SQLiteDbi::internalInit(const QHash<QString, QString>& props, U2OpStatus& os) {
    if (isInitialized(os)) {
        const QString appVersionText = getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "", os);
        CHECK_OP(os, );

        if (appVersionText.isEmpty()) {
            // Not an error since other databases might be opened with this interface
            coreLog.info(U2DbiL10n::tr("Not a %1 SQLite database: %2").arg(U2_PRODUCT_NAME)
                .arg(url));
        } else {
            Version dbAppVersion = Version::parseVersion(appVersionText);
            Version currentVersion = Version::appVersion();
            if (dbAppVersion > currentVersion) {
                coreLog.info(U2DbiL10n::tr("Warning! The database was created with a newer %1 version: "
                    "%2. Not all database features may be supported! Current %1 version: %3.")
                    .arg(U2_PRODUCT_NAME).arg(dbAppVersion.text).arg(currentVersion.text));
            }
        }

        foreach (const QString& key, props.keys()) {
            if (key.startsWith("sqlite-")) {
                setProperty(key, props.value(key), os);
            }
        }
    }

    // set up features list
    features.insert(U2DbiFeature_ReadSequence);
    features.insert(U2DbiFeature_ReadMsa);
    features.insert(U2DbiFeature_ReadAssembly);
    features.insert(U2DbiFeature_WriteSequence);
    features.insert(U2DbiFeature_WriteMsa);
    features.insert(U2DbiFeature_WriteAssembly);
    features.insert(U2DbiFeature_AssemblyReadsPacking);
    features.insert(U2DbiFeature_AssemblyCoverageStat);
    features.insert(U2DbiFeature_RemoveObjects);
    features.insert(U2DbiFeature_ChangeFolders);
    features.insert(U2DbiFeature_ReadCrossDatabaseReferences);
    features.insert(U2DbiFeature_WriteCrossDatabaseReferences);
    features.insert(U2DbiFeature_ReadAttributes);
    features.insert(U2DbiFeature_WriteAttributes);
    features.insert(U2DbiFeature_ReadProperties);
    features.insert(U2DbiFeature_WriteProperties);
    features.insert(U2DbiFeature_ReadVariant);
    features.insert(U2DbiFeature_WriteVariant);
    features.insert(U2DbiFeature_ReadFeatures);
    features.insert(U2DbiFeature_WriteFeatures);
    features.insert(U2DbiFeature_ReadModifications);
    features.insert(U2DbiFeature_WriteModifications);
    features.insert(U2DbiFeature_ReadUdr);
    features.insert(U2DbiFeature_WriteUdr);
}

void SQLiteDbi::setState(U2DbiState s) {
    state = s;
}

QString SQLiteDbi::getLastErrorMessage(int rc) {
    QString err = db->handle == NULL ? QString(" error-code: %1").arg(rc) : QString(sqlite3_errmsg(db->handle));
    return err;
}

void SQLiteDbi::init(const QHash<QString, QString>& props, const QVariantMap&, U2OpStatus& os) {
    if (db->handle != NULL) {
        os.setError(U2DbiL10n::tr("Database is already opened!"));
        return;
    }
    if (state != U2DbiState_Void) {
        os.setError(U2DbiL10n::tr("Illegal database state: %1").arg(state));
        return;
    }
    setState(U2DbiState_Starting);
    url = props.value(U2DbiOptions::U2_DBI_OPTION_URL);
    if (url.isEmpty()) {
        os.setError(U2DbiL10n::tr("URL is not specified"));
        setState(U2DbiState_Void);
        return;
    }
    do {
        int flags = SQLITE_OPEN_READWRITE;
        bool create = props.value(U2DbiOptions::U2_DBI_OPTION_CREATE, "0").toInt() > 0;
        if (create) {
            flags |= SQLITE_OPEN_CREATE;
        }
        QByteArray file = url.toUtf8();
        int rc = sqlite3_open_v2(file.constData(), &db->handle, flags, NULL);
        if (rc != SQLITE_OK) {
            QString err = getLastErrorMessage(rc);
            os.setError(U2DbiL10n::tr("Error opening SQLite database: %1!").arg(err));
            break;
        }

        SQLiteWriteQuery("PRAGMA synchronous = OFF", db, os).execute();
        QString lockingMode = props.value(U2DbiOptions::U2_DBI_LOCKING_MODE, "exclusive");
        if (lockingMode == "normal") {
            SQLiteWriteQuery("PRAGMA main.locking_mode = NORMAL", db, os).execute();
        } else {
            SQLiteWriteQuery("PRAGMA main.locking_mode = EXCLUSIVE", db, os).execute();
        }
        SQLiteWriteQuery("PRAGMA temp_store = MEMORY", db, os).execute();
        SQLiteWriteQuery("PRAGMA journal_mode = MEMORY", db, os).execute();
        SQLiteWriteQuery("PRAGMA cache_size = 50000", db, os).execute();
        SQLiteWriteQuery("PRAGMA recursive_triggers = ON", db, os).execute();
        SQLiteWriteQuery("PRAGMA foreign_keys = ON", db, os).execute();
        //SQLiteQuery("PRAGMA page_size = 4096", db, os).execute();
        //TODO: int sqlite3_enable_shared_cache(int);
        //TODO: read_uncommitted
        //TODO: incremental_vacuum
        //TODO: temp_store_directory

        // check if the opened database is valid sqlite dbi
        initProperties = props;
        if (!isInitialized(os) && create) {
            populateDefaultSchema(os);
        } else {
            upgrade(os);
        }
        if (os.hasError()) {
            break;
        }

        dbiId = url;
        internalInit(props, os);
        // OK, initialization complete
        if (!os.hasError()) {
            ioLog.trace(QString("SQLite: initialized: %1\n").arg(url));
        }
    } while (0);

    if (os.hasError()) {
        sqlite3_close(db->handle);
        db->handle = NULL;
        setState(U2DbiState_Void);
        return;
    }
    setState(U2DbiState_Ready);
}

QVariantMap SQLiteDbi::shutdown(U2OpStatus& os) {
    if (db == NULL) {
        os.setError(U2DbiL10n::tr("Database is already closed!"));
        return QVariantMap();
    }
    if (state != U2DbiState_Ready) {
        os.setError(U2DbiL10n::tr("Illegal database state %1!").arg(state));
        return QVariantMap();
    }

    if (!flush(os)) {
        CHECK_OP(os, QVariantMap());
        os.setError(U2DbiL10n::tr("Can't synchronize database state"));
        return QVariantMap();
    }

    udrDbi->shutdown(os);
    objectDbi->shutdown(os);
    sequenceDbi->shutdown(os);
    msaDbi->shutdown(os);
    assemblyDbi->shutdown(os);
    crossDbi->shutdown(os);
    attributeDbi->shutdown(os);
    variantDbi->shutdown(os);
    featureDbi->shutdown(os);
    modDbi->shutdown(os);

    setState(U2DbiState_Stopping);
    int rc = sqlite3_close(db->handle);

    if (rc != SQLITE_OK) {
        ioLog.error(U2DbiL10n::tr("Failed to close database: %1, err: %2").arg(url).arg(getLastErrorMessage(rc)));
    }

    ioLog.trace(QString("SQLite: shutting down: %1\n").arg(url));

    db->handle = NULL;
    url.clear();
    initProperties.clear();

    setState(U2DbiState_Void);
    return QVariantMap();
}

bool SQLiteDbi::flush(U2OpStatus& ) {
    return true;
}

QString SQLiteDbi::getDbiId() const {
    return url;
}

QHash<QString, QString> SQLiteDbi::getDbiMetaInfo(U2OpStatus& ) {
    QHash<QString, QString> res;
    res["url"] = url;
    return res;

}

U2DataType SQLiteDbi::getEntityTypeById(const U2DataId& id) const {
    return U2DbiUtils::toType(id);
}

// SQLiteDbiFactory

SQLiteDbiFactory::SQLiteDbiFactory() : U2DbiFactory()
{
}

U2Dbi *SQLiteDbiFactory::createDbi() {
    return new SQLiteDbi();
}

U2DbiFactoryId SQLiteDbiFactory::getId()const {
    return ID;
}

FormatCheckResult SQLiteDbiFactory::isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& ) const {
    QString surl  = properties.value(U2DbiOptions::U2_DBI_OPTION_URL);
    GUrl url(surl);
    if (!url.isLocalFile()) {
        return FormatDetection_NotMatched;
    }
    if (rawData.startsWith("SQLite format 3")) {
        return FormatDetection_Matched;
    }
    return FormatDetection_NotMatched;
}

bool SQLiteDbiFactory::isDbiExists(const U2DbiId& id) const {
    return QFile::exists(id);
}

const U2DbiFactoryId SQLiteDbiFactory::ID = SQLITE_DBI_ID;

}//namespace
