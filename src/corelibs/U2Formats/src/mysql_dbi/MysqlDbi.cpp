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

#include <QSqlError>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include "MysqlAssemblyDbi.h"
#include "MysqlAttributeDbi.h"
#include "MysqlCrossDatabaseReferenceDbi.h"
#include "MysqlDbi.h"
#include "MysqlFeatureDbi.h"
#include "MysqlModDbi.h"
#include "MysqlMsaDbi.h"
#include "MysqlObjectDbi.h"
#include "MysqlObjectRelationsDbi.h"
#include "MysqlSequenceDbi.h"
#include "MysqlUdrDbi.h"
#include "MysqlVariantDbi.h"
#include "util/MysqlDbiUtils.h"
#include "util/MysqlHelpers.h"
#include "util/upgraders/MysqlUpgraderFrom_1_14_To_1_15.h"
#include "util/upgraders/MysqlUpgraderFrom_1_15_To_1_16.h"
#include "util/upgraders/MysqlUpgraderFrom_1_16_To_1_17.h"
#include "util/upgraders/MysqlUpgraderFrom_1_16_To_1_24.h"
#include "util/upgraders/MysqlUpgraderFrom_1_24_To_1_25.h"

namespace U2 {

const int MysqlDbi::BIND_PARAMETERS_LIMIT = USHRT_MAX;

MysqlDbi::MysqlDbi()
    : U2AbstractDbi(MysqlDbiFactory::ID), tablesAreCreated(false)
{
    db = new MysqlDbRef;

    assemblyDbi =           new MysqlAssemblyDbi(this);
    attributeDbi =          new MysqlAttributeDbi(this);
    crossDbi =              new MysqlCrossDatabaseReferenceDbi(this);
    featureDbi =            new MysqlFeatureDbi(this);
    modDbi =                new MysqlModDbi(this);
    msaDbi =                new MysqlMsaDbi(this);
    objectDbi =             new MysqlObjectDbi(this);
    objectRelationsDbi =    new MysqlObjectRelationsDbi(this);
    sequenceDbi =           new MysqlSequenceDbi(this);
    udrDbi =                new MysqlUdrDbi(this);
    variantDbi =            new MysqlVariantDbi(this);

    upgraders << new MysqlUpgraderFrom_1_14_To_1_15(this);
    upgraders << new MysqlUpgraderFrom_1_15_To_1_16(this);
    upgraders << new MysqlUpgraderFrom_1_16_To_1_17(this);
    upgraders << new MysqlUpgraderFrom_1_16_To_1_24(this);
    upgraders << new MysqlUpgraderFrom_1_24_To_1_25(this);
}

MysqlDbi::~MysqlDbi() {
    delete variantDbi;
    delete udrDbi;
    delete sequenceDbi;
    delete objectDbi;
    delete msaDbi;
    delete modDbi;
    delete featureDbi;
    delete crossDbi;
    delete attributeDbi;
    delete assemblyDbi;

    delete db;

    QSqlDatabase::removeDatabase(QString::number((qint64)QThread::currentThread()));
}

U2AssemblyDbi* MysqlDbi::getAssemblyDbi() {
    return assemblyDbi;
}

U2AttributeDbi * MysqlDbi::getAttributeDbi() {
    return attributeDbi;
}

U2CrossDatabaseReferenceDbi* MysqlDbi::getCrossDatabaseReferenceDbi() {
    return crossDbi;
}

U2FeatureDbi* MysqlDbi::getFeatureDbi() {
    return featureDbi;
}

U2ModDbi* MysqlDbi::getModDbi() {
    return modDbi;
}

U2MsaDbi* MysqlDbi::getMsaDbi() {
    return msaDbi;
}

U2ObjectDbi* MysqlDbi::getObjectDbi() {
    return objectDbi;
}

U2ObjectRelationsDbi * MysqlDbi::getObjectRelationsDbi() {
    return objectRelationsDbi;
}

U2SequenceDbi* MysqlDbi::getSequenceDbi() {
    return sequenceDbi;
}

UdrDbi* MysqlDbi::getUdrDbi() {
    return udrDbi;
}

U2VariantDbi* MysqlDbi::getVariantDbi() {
    return variantDbi;
}

MysqlAssemblyDbi* MysqlDbi::getMysqlAssemblyDbi() {
    return assemblyDbi;
}

MysqlAttributeDbi* MysqlDbi::getMysqlAttributeDbi() {
    return attributeDbi;
}

MysqlCrossDatabaseReferenceDbi* MysqlDbi::getMysqlCrossDatabaseReferenceDbi() {
    return crossDbi;
}

MysqlFeatureDbi* MysqlDbi::getMysqlFeatureDbi() {
    return featureDbi;
}

MysqlModDbi* MysqlDbi::getMysqlModDbi() {
    return modDbi;
}

MysqlMsaDbi* MysqlDbi::getMysqlMsaDbi() {
    return msaDbi;
}

MysqlObjectDbi* MysqlDbi::getMysqlObjectDbi() {
    return objectDbi;
}

MysqlSequenceDbi* MysqlDbi::getMysqlSequenceDbi() {
    return sequenceDbi;
}

MysqlUdrDbi* MysqlDbi::getMysqlUdrDbi() {
    return udrDbi;
}

MysqlVariantDbi * MysqlDbi::getMysqlVariantDbi() {
    return variantDbi;
}

MysqlDbRef* MysqlDbi::getDbRef() {
    return db;
}

bool MysqlDbi::isInitialized(U2OpStatus &os) {
    if (!tablesAreCreated) {
        U2SqlQuery q("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = :name and TABLE_TYPE='BASE TABLE'", db, os);
        q.bindString(":name", db->handle.databaseName());

        const int countOfTables = q.selectInt64();
        CHECK_OP(os, false);

        tablesAreCreated = 0 != countOfTables;
    }
    return tablesAreCreated;
}

QString MysqlDbi::getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os) {
    const bool appVersionRequested = U2DbiOptions::APP_MIN_COMPATIBLE_VERSION == name;

    if (appVersionRequested && !minCompatibleAppVersion.isEmpty()) {
        return minCompatibleAppVersion;
    }

    U2SqlQuery q("SELECT value FROM Meta WHERE name = :name", db, os);
    q.bindString(":name", name);
    QStringList res = q.selectStrings();
    CHECK_OP(os, defaultValue);

    if (!res.isEmpty()) {
        if (U2DbiOptions::APP_MIN_COMPATIBLE_VERSION == name) {
            CHECK_EXT(minCompatibleAppVersion.isEmpty(), os.setError("Unexpected value of minimum application version"), QString());
            minCompatibleAppVersion = res.first();
        }
        return res.first();
    }

    return defaultValue;
}

void MysqlDbi::setProperty(const QString& name, const QString& value, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q1("DELETE FROM Meta WHERE name = :name", db, os);
    q1.bindString(":name", name);
    q1.execute();
    CHECK_OP(os, );

    U2SqlQuery q2("INSERT INTO Meta(name, value) VALUES (:name, :value)", db, os);
    q2.bindString(":name", name);
    q2.bindString(":value", value);
    q2.execute();

    if (U2DbiOptions::APP_MIN_COMPATIBLE_VERSION == name) {
        minCompatibleAppVersion = value;
    }
}

void MysqlDbi::startOperationsBlock(U2OpStatus& os) {
    operationsBlockTransactions.push(new MysqlTransaction(db, os));
}

void MysqlDbi::stopOperationBlock(U2OpStatus& os) {
    SAFE_POINT_EXT(!operationsBlockTransactions.isEmpty(), os.setError("There is no transaction to delete"), );
    delete operationsBlockTransactions.pop();
}

bool MysqlDbi::isReadOnly() const {
    return features.contains(U2DbiFeature_GlobalReadOnly);
}

bool MysqlDbi::isTransactionActive() const {
    return !db->transactionStack.isEmpty();
}

void MysqlDbi::createHandle(const QHash<QString, QString> &props) {
    const QString url = props.value(U2DbiOptions::U2_DBI_OPTION_URL);
    const QString connectionName = url + "_" + QString::number((qint64)QThread::currentThread());
    QSqlDatabase database = QSqlDatabase::database(connectionName);
    if (!database.isValid()) {
        database = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    }
    db->handle = database;
}

void MysqlDbi::open(const QHash<QString, QString> &props, U2OpStatus &os) {
    QString userName;
    const QString password = props.value(U2DbiOptions::U2_DBI_OPTION_PASSWORD);
    QString host;
    int port = -1;
    QString dbName;

    bool parseResult = U2DbiUtils::parseFullDbiUrl(props.value(U2DbiOptions::U2_DBI_OPTION_URL), userName, host, port, dbName);
    if (!parseResult) {
        os.setError(U2DbiL10n::tr("Database url is incorrect"));
        setState(U2DbiState_Void);
        return;
    }

    if (userName.isEmpty()) {
        os.setError(U2DbiL10n::tr("User login is not specified"));
        setState(U2DbiState_Void);
        return;
    }

    if (host.isEmpty()) {
        os.setError(U2DbiL10n::tr("Host is not specified"));
        setState(U2DbiState_Void);
        return;
    }

    if (dbName.isEmpty()) {
        os.setError(U2DbiL10n::tr("Database name is not specified"));
        setState(U2DbiState_Void);
        return;
    }

    db->handle.setUserName(userName);
    db->handle.setPassword(password);
    db->handle.setHostName(host);
    if (port != -1) {
        db->handle.setPort(port);
    }
    db->handle.setDatabaseName(dbName);

    db->handle.setConnectOptions("CLIENT_COMPRESS=1;MYSQL_OPT_RECONNECT=1;");

    if (!db->handle.open() ) {
        os.setError(U2DbiL10n::tr("Error opening MySQL database: %1").arg(db->handle.lastError().text()));
        setState(U2DbiState_Void);
        return;
    }
}

void MysqlDbi::setState(U2DbiState s) {
    state = s;
}

#define CHECK_DB_INIT(os) \
    if (os.hasError()) { \
        db->handle.close(); \
        setState(U2DbiState_Void); \
        return; \
    }

void MysqlDbi::populateDefaultSchema(U2OpStatus& os) {
    MysqlTransaction transaction(db, os);
    Q_UNUSED(transaction);

    // meta table, stores general db info
    U2SqlQuery("CREATE TABLE Meta(name TEXT NOT NULL, value TEXT NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_DB_INIT(os);

    objectDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    sequenceDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    assemblyDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    attributeDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    crossDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    featureDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    modDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    msaDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    objectRelationsDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    udrDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    variantDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);

    setVersionProperties(Version::minVersionForMySQL(), os);
    CHECK_DB_INIT(os);
}

void MysqlDbi::internalInit(const QHash<QString, QString>& props, U2OpStatus& os) {
    if (isInitialized(os)) {
        checkVersion(os);
        CHECK_OP(os, );
        setupProperties(props, os);
        CHECK_OP(os, );
    }

    setupFeatures();
    checkUserPermissions(os);
    CHECK_OP(os, );
    setupTransactions(os);
}

void MysqlDbi::setupProperties(const QHash<QString, QString> &props, U2OpStatus &os) {
    foreach (const QString& key, props.keys()) {
        if (key.startsWith("mysql-")) {
            setProperty(key, props.value(key), os);
            CHECK_OP(os, );
        }
    }
}

void MysqlDbi::checkVersion(U2OpStatus& os) {
    const QString appVersionText = getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "", os);
    CHECK_OP(os, );

    if (appVersionText.isEmpty()) {
        // Not an error since other databases might be opened with this interface
        coreLog.info(U2DbiL10n::tr("Not a %1 MySQL database: %2, %3")
            .arg(U2_PRODUCT_NAME).arg(db->handle.hostName()).arg(db->handle.databaseName()));
    } else {
        Version dbAppVersion = Version::parseVersion(appVersionText);
        Version currentVersion = Version::appVersion();
        if (dbAppVersion > currentVersion) {
            coreLog.info(U2DbiL10n::tr("Warning! The database was created with a newer %1 version: "
                "%2. Not all database features may be supported! Current %1 version: %3.")
                .arg(U2_PRODUCT_NAME).arg(dbAppVersion.text).arg(currentVersion.text));
        }
    }
}

void MysqlDbi::setupFeatures() {
    features.insert(U2DbiFeature_ReadAssembly);
    features.insert(U2DbiFeature_ReadAttributes);
    features.insert(U2DbiFeature_ReadCrossDatabaseReferences);
    features.insert(U2DbiFeature_ReadFeatures);
    features.insert(U2DbiFeature_ReadModifications);
    features.insert(U2DbiFeature_ReadMsa);
    features.insert(U2DbiFeature_ReadProperties);
    features.insert(U2DbiFeature_ReadSequence);
    features.insert(U2DbiFeature_ReadUdr);
    features.insert(U2DbiFeature_ReadVariant);
    features.insert(U2DbiFeature_ReadRelations);

    features.insert(U2DbiFeature_WriteAssembly);
    features.insert(U2DbiFeature_WriteAttributes);
    features.insert(U2DbiFeature_WriteCrossDatabaseReferences);
    features.insert(U2DbiFeature_WriteFeatures);
    features.insert(U2DbiFeature_WriteModifications);
    features.insert(U2DbiFeature_WriteMsa);
    features.insert(U2DbiFeature_WriteProperties);
    features.insert(U2DbiFeature_WriteSequence);
    features.insert(U2DbiFeature_WriteUdr);
    features.insert(U2DbiFeature_WriteVariant);
    features.insert(U2DbiFeature_WriteRelations);

    features.insert(U2DbiFeature_AssemblyCoverageStat);
    features.insert(U2DbiFeature_AssemblyReadsPacking);
    features.insert(U2DbiFeature_RemoveObjects);
}

void MysqlDbi::checkUserPermissions(U2OpStatus& os) {
    const QString databaseName = db->handle.databaseName();
    const QString userName = db->handle.userName();
    CHECK_EXT(!databaseName.isEmpty() && !userName.isEmpty(), os.setError("Unable to check user permissions, database is not connected"), );

    const QString selectPrivilegeStr = "SELECT";
    const QString updatePrivilegeStr = "UPDATE";
    const QString deletePrivilegeStr = "DELETE";
    const QString insertPrivilegeStr = "INSERT";

    bool selectEnabled = false;
    bool updateEnabled = false;
    bool deleteEnabled = false;
    bool insertEnabled = false;

    const QString schemaQueryString = "SELECT DISTINCT PRIVILEGE_TYPE FROM information_schema.schema_privileges "
        "WHERE GRANTEE LIKE :userName AND TABLE_SCHEMA = :tableSchema";
    U2SqlQuery sq(schemaQueryString, db, os);
    sq.bindString(":userName", QString("'%1'%").arg(userName));
    sq.bindString(":tableSchema", databaseName);

    while (sq.step() && !(selectEnabled && updateEnabled && deleteEnabled && insertEnabled)) {
        const QString grantString = sq.getString(0);
        CHECK_OP(os, );

        selectEnabled |= grantString == selectPrivilegeStr;
        updateEnabled |= grantString == updatePrivilegeStr;
        deleteEnabled |= grantString == deletePrivilegeStr;
        insertEnabled |= grantString == insertPrivilegeStr;
    }

    const QString userQueryString = "SELECT DISTINCT PRIVILEGE_TYPE FROM information_schema.user_privileges "
        "WHERE GRANTEE LIKE :userName";
    U2SqlQuery uq(userQueryString, db, os);
    uq.bindString(":userName", QString("'%1'%").arg(userName));

    while (!(selectEnabled && updateEnabled && deleteEnabled && insertEnabled) && uq.step()) {
        const QString grantString = uq.getString(0);
        CHECK_OP(os, );

        selectEnabled |= grantString == selectPrivilegeStr;
        updateEnabled |= grantString == updatePrivilegeStr;
        deleteEnabled |= grantString == deletePrivilegeStr;
        insertEnabled |= grantString == insertPrivilegeStr;
    }

    if (!updateEnabled || !deleteEnabled || !insertEnabled) {
        if (selectEnabled) {
            features.insert(U2DbiFeature_GlobalReadOnly);
        } else {
            os.setError(QObject::tr("Invalid database user permissions set, so UGENE unable to use this database. "
                "Connect to your system administrator to fix the issue."));
        }
    }
}

void MysqlDbi::setupTransactions(U2OpStatus &os) {
    U2SqlQuery("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("SET autocommit = 0", db, os).execute();
}

void MysqlDbi::init(const QHash<QString, QString>& props, const QVariantMap&, U2OpStatus& os) {
    createHandle(props);
    QMutexLocker(&db->mutex);

    CHECK_EXT(state == U2DbiState_Void, os.setError(U2DbiL10n::tr("Illegal database state: %1").arg(state)), );

    setState(U2DbiState_Starting);

    if (!db->handle.isOpen()) {
        open(props, os);
        CHECK_OP(os, );
    }

    initProperties = props;

    dbiId = U2DbiUtils::createFullDbiUrl(db->handle.userName(), db->handle.hostName(), db->handle.port(), db->handle.databaseName());
    internalInit(props, os);
    if (os.hasError()) {
        db->handle.close();
        setState(U2DbiState_Void);
        return;
    }
    setState(U2DbiState_Ready);
}

QVariantMap MysqlDbi::shutdown(U2OpStatus& os) {
    CHECK(db->handle.isOpen(), QVariantMap());

    CHECK_EXT(state == U2DbiState_Ready,
                db->handle.close();
              , QVariantMap());


    if (!flush(os)) {
        CHECK_OP(os, QVariantMap());
        os.setError(U2DbiL10n::tr("Can't synchronize database state"));
        return QVariantMap();
    }

    QMutexLocker(&db->mutex);

    assemblyDbi->shutdown(os);
    attributeDbi->shutdown(os);
    crossDbi->shutdown(os);
    featureDbi->shutdown(os);
    modDbi->shutdown(os);
    msaDbi->shutdown(os);
    objectDbi->shutdown(os);
    objectRelationsDbi->shutdown(os);
    sequenceDbi->shutdown(os);
    variantDbi->shutdown(os);

    setState(U2DbiState_Stopping);
    db->handle.close();

    initProperties.clear();

    setState(U2DbiState_Void);
    return QVariantMap();
}

bool MysqlDbi::flush(U2OpStatus& ) {
    return true;
}

QString MysqlDbi::getDbiId() const {
    return dbiId;
}

QHash<QString, QString> MysqlDbi::getDbiMetaInfo(U2OpStatus& ) {
    QHash<QString, QString> res;
    res[U2DbiOptions::U2_DBI_OPTION_URL] = U2DbiUtils::ref2Url(getDbiRef());
    return res;
}

U2DataType MysqlDbi::getEntityTypeById(const U2DataId& id) const {
    return U2DbiUtils::toType(id);
}

// MysqlDbiFactory

const U2DbiFactoryId MysqlDbiFactory::ID = MYSQL_DBI_ID;

MysqlDbiFactory::MysqlDbiFactory() : U2DbiFactory() {
}

U2Dbi *MysqlDbiFactory::createDbi() {
    return new MysqlDbi();
}

U2DbiFactoryId MysqlDbiFactory::getId() const {
    return ID;
}

FormatCheckResult MysqlDbiFactory::isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& ) const {
    Q_UNUSED(properties);
    Q_UNUSED(rawData);
    // TODO: check the result
    return FormatDetection_Matched;
}

bool MysqlDbiFactory::isDbiExists(const U2DbiId& id) const {
    // TODO: check the connection
    Q_UNUSED(id);
//    QString host;
//    int port;
//    QString dbName;
//    MysqlDbiUtils::parseDbiId(id, host, port, dbName);

//    U2OpStatusImpl os;
//    U2SqlQuery q("SELECT DATA LIKE :dbName", db, os);
//    q.bindString(":dbName", dbName);
//    return !q.selectStrings().isEmpty();
    return false;
}

MysqlChildDbiCommon::MysqlChildDbiCommon(MysqlDbi* dbi) :
    dbi(dbi),
    db(NULL == dbi ? NULL : dbi->getDbRef())
{
}

} // namespace U2
