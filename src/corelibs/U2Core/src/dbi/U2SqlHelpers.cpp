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

#include "U2SqlHelpers.h"

#include <U2Core/Log.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include <3rdparty/sqlite3/sqlite3.h>

namespace U2 {

static U2DataId     emptyId;
static QByteArray   emptyBlob;
static QString      emptyString;

qint64 SQLiteUtils::remove(const QString& table, const QString& field, const U2DataId& id, qint64 expectedRows, DbRef* db, U2OpStatus& os) {
    QMutexLocker m(&db->lock); // lock db in order to retrieve valid row id for insert

    SQLiteWriteQuery q(QString("DELETE FROM %1 WHERE %2 = ?1").arg(table).arg(field), db, os);
    q.bindDataId(1, id);
    return q.update(expectedRows);
}

bool SQLiteUtils::isTableExists(const QString& tableName, DbRef* db, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT name FROM sqlite_master WHERE type='table' AND name=?1", db, os);
    q.bindString(1, tableName);
    return q.step();
}

int SQLiteUtils::isDatabaseReadOnly(const DbRef *db, QString dbName){
    int res = sqlite3_db_readonly(db->handle, dbName.toUtf8());
    return res;
}

bool SQLiteUtils::getMemoryHint(int& currentMemory, int &maxMemory, int resetMax) {

    return SQLITE_OK == sqlite3_status(SQLITE_STATUS_MEMORY_USED, &currentMemory, &maxMemory, resetMax);
}

//////////////////////////////////////////////////////////////////////////
// L10N
QString U2DbiL10n::queryError(const QString& err) {
    return tr("Error querying database: %1").arg(err);
}

QString U2DbiL10n::tooManyResults() {
    return tr("Found more results than expected!");
}

//////////////////////////////////////////////////////////////////////////
// SQLiteReadOnlyQuery

//#define U2_TRACE_SQLITE_QUERIES

#ifdef U2_TRACE_SQLITE_QUERIES
static int nActiveQueries = 0;

static void traceQueryPrepare(const QString& q) {
    nActiveQueries++;
    ioLog.trace(QString("SQLite new query! Active queries: %1, Q: %2").arg(nActiveQueries).arg(q));
}

static void traceQueryDestroy(const QString& q) {
    nActiveQueries--;
    ioLog.trace(QString("SQLite destroying query! Active queries: %1, Q: %2").arg(nActiveQueries).arg(q));
}
#endif

SQLiteQuery::SQLiteQuery(const QString& _sql, DbRef* d, U2OpStatus& _os)
: db(d), os(&_os), st(NULL), sql(_sql)
{
    prepare();

#ifdef U2_TRACE_SQLITE_QUERIES
    traceQueryPrepare(sql);
#endif
}

SQLiteQuery::SQLiteQuery(const QString& _sql, qint64 offset, qint64 count, DbRef* d, U2OpStatus& _os)
: db(d), os(&_os), st(NULL), sql(_sql)
{
    U2DbiUtils::addLimit(sql, offset, count);
    prepare();

#ifdef U2_TRACE_SQLITE_QUERIES
    traceQueryPrepare(sql);
#endif
}

void SQLiteQuery::setError(const QString& err) {
    ioLog.trace("SQL: error: " + err + " in query: " + sql);
    if (!os->hasError()) {
        os->setError(err);
    }
}

void SQLiteQuery::prepare() {
    if (os->hasError()) {
        return;
    }
    QByteArray utf8 = sql.toUtf8();
    int rc = sqlite3_prepare_v2(db->handle, utf8.constData() ,utf8.size(), &st, NULL);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::queryError(sqlite3_errmsg(db->handle)));
        return;
    }
    assert(st!=NULL);
}

SQLiteQuery::~SQLiteQuery() {
    if (st != NULL) {
        int rc = sqlite3_finalize(st);
        if (rc != SQLITE_OK) {
            setError(QString("SQLite: Error finalizing statement: ") + U2DbiL10n::queryError(sqlite3_errmsg(db->handle)));
            return;
        }
    }
#ifdef U2_TRACE_SQLITE_QUERIES
    traceQueryDestroy(sql);
#endif
}

bool SQLiteQuery::reset(bool clearBindings) {
    if (hasError()) {
        return false;
    }
    assert(st != NULL);
    if (clearBindings) {
        int rc = sqlite3_clear_bindings(st);
        if (rc != SQLITE_OK) {
            setError(QString("SQLite: Error clearing statement bindings: ") + U2DbiL10n::queryError(sqlite3_errmsg(db->handle)));
            return false;
        }
    }
    int rc = sqlite3_reset(st);
    if (rc != SQLITE_OK) {
        setError(QString("SQLite: Error resetting statement: ") + U2DbiL10n::queryError(sqlite3_errmsg(db->handle)));
        return false;
    }
    return true;
}

bool SQLiteQuery::stepImpl() {
    if (hasError()) {
        return false;
    }
    assert(st != NULL);
    int rc = sqlite3_step(st);
    if (rc == SQLITE_DONE || rc == SQLITE_READONLY) {
        return false;
    } else if (rc == SQLITE_ROW) {
        return true;
    }
    setError(U2DbiL10n::tr("Unexpected query result code: %1 (%2)").arg(rc).arg(sqlite3_errmsg(db->handle)));
    return false;
}

void SQLiteQuery::ensureDone() {
    bool done = !step();
    if (!done && !hasError()) {
        setError(U2DbiL10n::tooManyResults());
        assert(0);
    }
}

void SQLiteQuery::bindNull(int idx) {
    CHECK(!hasError(), );
    assert(st != NULL);

    int rc = sqlite3_bind_null(st, idx);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding NULL value! Query: '%1', idx: %2").arg(sql).arg(idx));
        return;
    }
}

int SQLiteQuery::getInt32(int column) const {
    if (hasError()) {
        return -1;
    }
    assert(st!=NULL);
    return sqlite3_column_int(st, column);
}

qint64 SQLiteQuery::getInt64(int column) const {
    if (hasError()) {
        return -1;
    }
    assert(st!=NULL);
    return sqlite3_column_int64(st, column);
}

double SQLiteQuery::getDouble(int column) const {
    if (hasError()) {
        return -1;
    }
    assert(st!=NULL);
    return sqlite3_column_double(st, column);
}

U2DataId SQLiteQuery::getDataId(int column, U2DataType type, const QByteArray& dbExtra) const {
    if (hasError()) {
        return 0;
    }
    assert(st!=NULL);
    U2DataId res = U2DbiUtils::toU2DataId(getInt64(column), type, dbExtra);
    return res;
}

U2DataId SQLiteQuery::getDataIdExt(int column) const {
    if (hasError()) {
        return emptyId;
    }
    assert(st!=NULL);
    U2DataType type = getInt32(column + 1);
    if (type == U2Type::Unknown) {
        return emptyId;
    }
    QByteArray dbExtra = getBlob(column + 2);
    U2DataId res = U2DbiUtils::toU2DataId(getInt64(column), type, dbExtra);
    return res;
}


U2DataType SQLiteQuery::getDataType(int column) const {
    if (hasError()) {
        return U2Type::Unknown;
    }
    assert(st!=NULL);
    return (U2DataType)sqlite3_column_int(st, column);
}

QString SQLiteQuery::getString(int column) const {
    if (hasError()) {
        return emptyString;
    }
    assert(st!=NULL);
    return QString::fromUtf8((const char*)sqlite3_column_text(st, column));
}

QByteArray SQLiteQuery::getCString(int column) const {
    if (hasError()) {
        return emptyBlob;
    }
    assert(st!=NULL);
    return QByteArray((const char*)sqlite3_column_text(st, column));
}

QByteArray SQLiteQuery::getBlob(int column) const {
    if (hasError()) {
        return emptyBlob;
    }
    assert(st!=NULL);
    int nBytes = sqlite3_column_bytes(st, column);
    if (nBytes == 0) {
        return emptyBlob;
    }
    QByteArray res(static_cast<const char *>(sqlite3_column_blob(st, column)), nBytes);
    return res;
}

// param binding methods
void SQLiteQuery::bindDataId(int idx, const U2DataId& val) {
    if (!val.isEmpty()) {
        bindInt64(idx, U2DbiUtils::toDbiId(val));
    } else {
        bindNull(idx);
    }
}

void SQLiteQuery::bindType(int idx, U2DataType type) {
    bindInt64(idx, type);
}

void SQLiteQuery::bindString(int idx, const QString& val) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    QByteArray utf8 = val.toUtf8();
    int rc = sqlite3_bind_text(st, idx, utf8, utf8.length(), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding text value! Query: '%1', idx: %2, value: '%3'").arg(sql).arg(idx).arg(val));
        return;
    }
}

void SQLiteQuery::bindInt32(int idx, qint32 val) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    int rc = sqlite3_bind_int(st, idx, val);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding int32 value! Query: '%1', idx: %2, value: %3").arg(sql).arg(idx).arg(val));
        return;
    }
}

void SQLiteQuery::bindDouble(int idx, double val) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    int rc = sqlite3_bind_double(st, idx, val);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding int64 value! Query: '%1', idx: %2, value: %3").arg(sql).arg(idx).arg(val));
        return;
    }
}

void SQLiteQuery::bindInt64(int idx, qint64 val) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    int rc = sqlite3_bind_int64(st, idx, val);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding int64 value! Query: '%1', idx: %2, value: %3").arg(sql).arg(idx).arg(val));
        return;
    }
}

void SQLiteQuery::bindBool(int idx, bool val) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    int b = (val == 0 ? 0 : 1);
    int rc = sqlite3_bind_int(st, idx, b);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding boolean value! Query: '%1', idx: %2, value: %3").arg(sql).arg(idx).arg(b));
        return;
    }
}

void SQLiteQuery::bindBlob(int idx, const QByteArray& blob, bool transient) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    int rc = SQLITE_OK;
    if (blob.isEmpty()) {
        rc = sqlite3_bind_zeroblob(st, idx, -1);
    } else {
        rc = sqlite3_bind_blob(st, idx, blob.constData(), blob.size(), transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
    }
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding blob value! Query: '%1', idx: %2, size: %3").arg(sql).arg(idx).arg(blob.size()));
        return;
    }
}

void SQLiteQuery::bindZeroBlob(int idx, int reservedSize) {
    if (hasError()) {
        return;
    }
    assert(st!=NULL);
    int rc = sqlite3_bind_zeroblob(st, idx, reservedSize);
    if (rc != SQLITE_OK) {
        setError(U2DbiL10n::tr("Error binding blob value! Query: '%1', idx: %2").arg(sql).arg(idx));
        return;
    }
}

void SQLiteQuery::execute() {
    update(-1);
}

qint64 SQLiteQuery::update(qint64 expectedRows) {
    if (step()) {
        qint64 res = getInt64(0);
        if (expectedRows != -1 && expectedRows != res) {
            setError(U2DbiL10n::tr("Unexpected row count! Query: '%1', rows: %2").arg(sql).arg(res));
        }
        return res;
    }
    return -1;
}

qint64 SQLiteQuery::insert() {
    execute();
    if (hasError()) {
        return -1;
    }
    return getLastRowId();
}

U2DataId SQLiteQuery::insert(U2DataType type, const QByteArray& dbExtra) {
    qint64 lastRowId = insert();
    if (hasError()) {
        return emptyId;
    }
    return U2DbiUtils::toU2DataId(lastRowId, type, dbExtra);
}

qint32 SQLiteQuery::selectInt32() {
    if (step()) {
        return getInt32(0);
    }
    return -1;
}

qint64 SQLiteQuery::selectInt64() {
    if (step()) {
        return getInt64(0);
    }
    return -1;
}

qint64 SQLiteQuery::selectInt64(qint64 defaultValue) {
    if (step()) {
        return getInt64(0);
    }
    return defaultValue;
}

QList<U2DataId> SQLiteQuery::selectDataIds(U2DataType type, const QByteArray& dbExtra) {
    QList<U2DataId> res;
    while(step()) {
        U2DataId id = getDataId(0, type, dbExtra);
        res.append(id);
    }
    return res;
}

QList<U2DataId> SQLiteQuery::selectDataIdsExt() {
    QList<U2DataId> res;
    while(step()) {
        U2DataId id = getDataIdExt(0);
        res.append(id);
    }
    return res;
}

QStringList SQLiteQuery::selectStrings() {
    QStringList res;
    while (step()) {
        QString text = getString(0);
        res.append(text);
    }
    return res;
}


qint64 SQLiteQuery::getLastRowId() {
    qint64 sqliteId = sqlite3_last_insert_rowid(db->handle);
    return sqliteId;
}

//////////////////////////////////////////////////////////////////////////
///SQLiteReadQuery
SQLiteReadQuery::SQLiteReadQuery(const QString& _sql, DbRef* d, U2OpStatus& _os)
: SQLiteQuery(_sql, d, _os)
{
}

SQLiteReadQuery::SQLiteReadQuery(const QString& _sql, qint64 offset, qint64 count, DbRef* d, U2OpStatus& _os)
: SQLiteQuery(_sql, offset, count, d, _os)
{
}

bool SQLiteReadQuery::step(){
    QReadLocker locker(&db->rwLock);
    return stepImpl();
}

//////////////////////////////////////////////////////////////////////////
///SQLiteWriteQuery
SQLiteWriteQuery::SQLiteWriteQuery(const QString& _sql, DbRef* d, U2OpStatus& _os)
: SQLiteQuery(_sql, d, _os)
{
}

SQLiteWriteQuery::SQLiteWriteQuery(const QString& _sql, qint64 offset, qint64 count, DbRef* d, U2OpStatus& _os)
: SQLiteQuery(_sql, offset, count, d, _os)
{
}

bool SQLiteWriteQuery::step(){
    QMutexLocker mutexLocker(&db->lock);
    QWriteLocker writeLocker(&db->rwLock);
    return stepImpl();
}

//////////////////////////////////////////////////////////////////////////
// SQLite transaction helper

static void checkStack(const QVector<SQLiteTransaction*>& stack) {
#ifdef _DEBUG
    QThread* expectedThread = QThread::currentThread();
    for (int i = 0; i < stack.size(); i++) {
        SQLiteTransaction* t = stack[i];
        assert(t->thread == expectedThread);
    }
#else
    Q_UNUSED(stack);
#endif
}

SQLiteTransaction::SQLiteTransaction(DbRef* ref, U2OpStatus& _os)
    : db(ref), os(_os), cacheQueries(true), started(false)
{
#ifdef _DEBUG
    thread = QThread::currentThread();
#endif
    QMutexLocker m(&db->lock);
    CHECK(db->useTransaction, );

    if (db->transactionStack.isEmpty()) {
        db->lock.lock();
        int rc = sqlite3_exec(db->handle, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        if (rc != SQLITE_OK) {
            db->lock.unlock();
            os.setError(U2DbiL10n::queryError(sqlite3_errmsg(db->handle)));
            return;
        }
    }
    checkStack(db->transactionStack);
    db->transactionStack << this;
    started = true;
}

void SQLiteTransaction::clearPreparedQueries() {
    foreach (const QString &sql, db->preparedQueries.keys()) {
        db->preparedQueries[sql].clear();
    }
    db->preparedQueries.clear();
}

SQLiteTransaction::~SQLiteTransaction() {
    QMutexLocker m(&db->lock);
    CHECK(db->useTransaction, );
    CHECK(started, );
    SAFE_POINT(!db->transactionStack.isEmpty(), "Empty transaction stack", );
    SAFE_POINT(db->transactionStack.last() == this, "Wrong transaction in stack", );

    checkStack(db->transactionStack);
    db->transactionStack.pop_back();

    if (db->transactionStack.isEmpty()) {
        int rc;
        if (os.hasError()) {
            rc = sqlite3_exec(db->handle, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
        } else {
            rc = sqlite3_exec(db->handle, "COMMIT TRANSACTION;", NULL, NULL, NULL);
        }
        clearPreparedQueries();
        db->lock.unlock();
        if (rc != SQLITE_OK) {
            os.setError(U2DbiL10n::queryError(sqlite3_errmsg(db->handle)));
        }
    }
}

QSharedPointer<SQLiteQuery> SQLiteTransaction::getPreparedQuery(const QString &sql, DbRef *d, U2OpStatus &os) {
    if (db->preparedQueries.contains(sql)) {
        QSharedPointer<SQLiteQuery> result = db->preparedQueries[sql];
        result->setOpStatus(os);
        result->reset(false);
        return result;
    }
    QSharedPointer<SQLiteQuery> result (new SQLiteWriteQuery(sql, d, os));
    CHECK_OP(os, QSharedPointer<SQLiteQuery>());
    if(cacheQueries){
        db->preparedQueries[sql] = result;
    }
    return result;
}

QSharedPointer<SQLiteQuery> SQLiteTransaction::getPreparedQuery(const QString &sql, qint64 offset, qint64 count, DbRef *d, U2OpStatus &os) {
    if (db->preparedQueries.contains(sql)) {
        QSharedPointer<SQLiteQuery> result = db->preparedQueries[sql];
        result->setOpStatus(os);
        result->reset(false);
        return result;
    }
    QSharedPointer<SQLiteWriteQuery> result (new SQLiteWriteQuery(sql, offset, count, d, os));
    CHECK_OP(os, QSharedPointer<SQLiteQuery>());
    if(cacheQueries){
        db->preparedQueries[sql] = result;
    }
    return result;
}

} //namespace
