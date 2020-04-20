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

#ifndef _U2_SQL_HELPERS_H_
#define _U2_SQL_HELPERS_H_

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Type.h>

#include <QMutex>
#include <QStringList>
#include <QVector>
#include <QThread>
#include <QHash>
#include <QSharedPointer>
#include <QReadWriteLock>

struct sqlite3;
struct sqlite3_stmt;

namespace U2 {

class SQLiteWriteQuery;
class SQLiteQuery;
class SQLiteTransaction;

class U2CORE_EXPORT DbRef {
public:
    DbRef(sqlite3* db = NULL) : handle(db), lock(QMutex::Recursive), useTransaction(true) {}

    sqlite3*                     handle;
    QMutex                       lock;
    QReadWriteLock               rwLock;
    bool                         useTransaction;
    bool                         useCache;
    QVector<SQLiteTransaction*>  transactionStack;
    QHash<QString, QSharedPointer<SQLiteQuery> > preparedQueries; //shared pointer because a query can be deleted elsewhere
};

class U2CORE_EXPORT SQLiteUtils {
public:

    /**
        Removes from the table all records with 'field' == id
        Checks 'expectedRowCount' the same way as 'update' method
    */
    static qint64 remove(const QString& table, const QString& field, const U2DataId& id, qint64 expectedRows, DbRef* db, U2OpStatus& os);

    /** Checks if the table exists in database */
    static bool isTableExists(const QString& tableName, DbRef* db, U2OpStatus& os);

    /** returns 1 if the database is read-only, 0 if it is read/write, or -1 if dbName is not the name of a database on connection */
    static int isDatabaseReadOnly(const DbRef* db, QString dbName);//dbName is usualy "main"

    /** Writes Memory counters */
    static bool getMemoryHint(int& currentMemory, int &maxMemory, int resetMax);
};

/** Common localization messages for U2Dbi*/
class U2CORE_EXPORT U2DbiL10n : public QObject {
    Q_OBJECT
public:
    static QString queryError(const QString& err);
    static QString tooManyResults();
};


/**
    SQLite query wrapper. Uses prepared statement internally
    An optimized and simplified interface for U2DBI needs.
*/
class U2CORE_EXPORT SQLiteQuery  {
public:
    /**
        Constructs prepared statement for SQLiteDB
        If failed the error message is written to 'os'
        It's desirable to release this object as soon as possible because it locks
        the database for concurrent modifications from other threads
    */
    SQLiteQuery(const QString& sql, DbRef* d, U2OpStatus& os);
    SQLiteQuery(const QString& sql, qint64 offset, qint64 count, DbRef* d, U2OpStatus& os);

    /** Releases all resources associated with the statement */
    virtual ~SQLiteQuery();

    //////////////////////////////////////////////////////////////////////////
    // Statement/query state manipulation methods

    /** Clears all bindings and resets statement */
    bool reset(bool clearBindings = true);

    /**
        Executes next step of the statement
        Returns true there are more results to fetch and no error occurs
    */
    virtual bool step() = 0;

    /**
        Ensures that there are no more results in result set
        Sets error message if more results are available
    */
    void ensureDone();


    //////////////////////////////////////////////////////////////////////////
    // param binding methods

    /** Binds NULL value  */
    void bindNull(int idx);

    /** Binds U2DataId  */
    void bindDataId(int idx, const U2DataId& val);

    /** Binds U2DataType */
    void bindType(int idx, U2DataType type);

    /** Binds 32bit integer value*/
    void bindInt32(int idx, qint32 val);

    /** Binds 64bit integer value*/
    void bindInt64(int idx, qint64 val);

    /** Binds 64bit real value*/
    void bindDouble(int idx, double val);

    /** Binds bool value */
    void bindBool(int idx, bool val);

    /** Binds text string */
    void bindString(int idx, const QString& val);

    /** Binds BLOB */
    void bindBlob(int idx, const QByteArray& blob, bool transient = true);
    void bindZeroBlob(int idx, int reservedSize);


    //////////////////////////////////////////////////////////////////////////
    // result retrieval methods

    U2DataId getDataId(int column, U2DataType type, const QByteArray& dbExtra = QByteArray()) const;

    U2DataId getDataIdExt(int column) const;

    U2DataType getDataType(int column) const;

    int getInt32(int column) const;

    qint64 getInt64(int column) const;

    double getDouble(int column) const;

    QString getString(int column) const;

    QByteArray getCString(int column) const;

    QByteArray getBlob(int column) const;

    bool getBool(int column) const {return getInt32(column) != 0;}

    //////////////////////////////////////////////////////////////////////////
    // Utility methods

    /** Executes update and returns last row id*/
    qint64 insert();

    /** Executes update and returns last row id converted to U2DataId using type info*/
    U2DataId insert(U2DataType type, const QByteArray& dbExtra = QByteArray());

    /** Executes query */
    void execute();

    /** Executes update and returns number of rows affected.
        Fails if result count != expectedRowCount
        'expectedRowCount' == -1 disables row-count check
    */
    qint64 update(qint64 expectedRows = -1);

    /** Selects a single int32 value */
    qint32 selectInt32();

    /** Selects a single int64 value */
    qint64 selectInt64();

    /** Selects a single int64 value, if no results found returns default value */
    qint64 selectInt64(qint64 defaultValue);

    /** Select list of ids and adds 'type' parameter to construct U2DataId */
    QList<U2DataId> selectDataIds(U2DataType type, const QByteArray& dbExtra = QByteArray());

    /** Select id(col=0), type(col=1) pairs  and constructs U2DataId */
    QList<U2DataId> selectDataIdsExt();

    /** Select list of strings */
    QStringList selectStrings();



    //////////////////////////////////////////////////////////////////////////
    // Query info methods
    const QString& getQueryText() const {return sql;}

    void setError(const QString& err);

    bool hasError() const {return (os!=NULL) ? os->hasError() : true;}

    void setOpStatus(U2OpStatus& _os) {os = &_os;}

    U2OpStatus& getOpStatus() {return  *os;}

    DbRef*          getDb() const {return db;}

protected:
    bool stepImpl();

    DbRef*          db;

private:
    /** Returns last insert row*/
    qint64 getLastRowId();

    void prepare();

    U2OpStatus*     os;
    sqlite3_stmt*   st;
    QString         sql;
};

class U2CORE_EXPORT SQLiteReadQuery : public SQLiteQuery {
public:
    /**
        Constructs prepared statement for SQLiteDB
        If failed the error message is written to 'os'
        It's desirable to release this object as soon as possible because it locks
        the database for concurrent modifications from other threads
    */
    SQLiteReadQuery(const QString& sql, DbRef* d, U2OpStatus& os);
    SQLiteReadQuery(const QString& sql, qint64 offset, qint64 count, DbRef* d, U2OpStatus& os);

    bool step();
};

class U2CORE_EXPORT SQLiteWriteQuery : public SQLiteQuery {
public:
    /**
        Constructs prepared statement for SQLiteDB
        If failed the error message is written to 'os'
        It's desirable to release this object as soon as possible because it locks
        the database for concurrent modifications from other threads
    */
    SQLiteWriteQuery(const QString& sql, DbRef* d, U2OpStatus& os);
    SQLiteWriteQuery(const QString& sql, qint64 offset, qint64 count, DbRef* d, U2OpStatus& os);

    bool step();
};

/** Helper class to mark transaction regions */
class U2CORE_EXPORT SQLiteTransaction {
public:
    SQLiteTransaction(DbRef* db, U2OpStatus& os);
    virtual ~SQLiteTransaction();

    QSharedPointer<SQLiteQuery> getPreparedQuery(const QString &sql, DbRef *d, U2OpStatus &os);
    QSharedPointer<SQLiteQuery> getPreparedQuery(const QString &sql, qint64 offset, qint64 count, DbRef *d, U2OpStatus &os);

    bool isCacheQueries() {return cacheQueries;}

private:
    DbRef* db;
    U2OpStatus& os;
    bool cacheQueries;
    bool started;

    void clearPreparedQueries();

#ifdef _DEBUG
public:
    QThread* thread;
#endif
};

/** Data loader adapter for SqlQueryIterator */
template <class T> class SQLiteResultSetLoader {
public:
    virtual ~SQLiteResultSetLoader(){}
    virtual T load(SQLiteQuery* q) = 0;
};

/** Filter for SqlRSIterator. Checks if value must be filtered out from the result */
template <class T> class SQLiteResultSetFilter {
public:
    virtual ~SQLiteResultSetFilter(){}
    virtual bool filter(const T&) = 0;
};

/** SQL query result set iterator */
template<class T> class SQLiteResultSetIterator : public U2DbiIterator<T> {
public:
    SQLiteResultSetIterator(QSharedPointer<SQLiteQuery> q, SQLiteResultSetLoader<T>* l, SQLiteResultSetFilter<T>* f, const T& d, U2OpStatus& o)
        : query(q), loader(l), filter(f), defaultValue(d), os(o), endOfStream(false)
    {
        fetchNext();
    }

    virtual ~SQLiteResultSetIterator() {
        delete filter;
        delete loader;
        query.clear();
    }

    virtual bool hasNext() {
        return !endOfStream;
    }

    virtual T next() {
        if (endOfStream) {
            assert(0);
            return defaultValue;
        }
        currentResult = nextResult;
        fetchNext();
        return currentResult;
    }

    virtual T peek() {
        if (endOfStream) {
            assert(0);
            return defaultValue;
        }
        return nextResult;
    }

private:
    void fetchNext() {
        do {
            if (!query->step()) {
                endOfStream = true;
                return;
            }
            nextResult = loader->load(query.data());
        } while (filter != NULL && !filter->filter(nextResult));
    }

    QSharedPointer<SQLiteQuery>    query;
    SQLiteResultSetLoader<T>* loader;
    SQLiteResultSetFilter<T>* filter;
    T               defaultValue;
    U2OpStatus&     os;
    bool            endOfStream;
    T               nextResult;
    T               currentResult;
    bool            deleteQuery;
};

class SQLiteDataIdResultSetLoader : public SQLiteResultSetLoader<U2DataId> {
public:
    SQLiteDataIdResultSetLoader(U2DataType _type, const QByteArray& _dbExra = QByteArray()) : type(_type), dbExtra(_dbExra){}
    U2DataId load(SQLiteQuery* q) { return q->getDataId(0, type, dbExtra);}

protected:
    U2DataType type;
    QByteArray dbExtra;
};

class SQLiteDataIdResultSetLoaderEx : public SQLiteResultSetLoader<U2DataId> {
public:
    SQLiteDataIdResultSetLoaderEx(const QByteArray& _dbExra = QByteArray()) : dbExtra(_dbExra){}
    U2DataId load(SQLiteQuery* q) { return q->getDataId(0, q->getDataType(1), dbExtra);}

protected:
    U2DataType type;
    QByteArray dbExtra;
};

#define DBI_TYPE_CHECK(dataId,  expectedType, os, res)\
    if (!dataId.isEmpty()) {\
        U2DataType realType = U2DbiUtils::toType(dataId);\
        if (realType != expectedType) {\
            os.setError(QString("Illegal data type: %1, expected %2").arg(realType).arg(expectedType));\
            return res;\
        }\
    }\

} // namespace U2

#endif // _U2_SQL_HELPERS_H_
