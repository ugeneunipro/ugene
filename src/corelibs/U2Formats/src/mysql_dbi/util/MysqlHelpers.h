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

#ifndef _U2_MYSQL_HELPERS_H_
#define _U2_MYSQL_HELPERS_H_

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2Type.h>

#include <QSqlQuery>

namespace U2 {

class MysqlTransaction;

class U2FORMATS_EXPORT MysqlDbRef {
public:
                                    MysqlDbRef();

    QSqlDatabase                    handle;
    QMutex                          mutex;
    QVector<MysqlTransaction *>     transactionStack;
};

class U2FORMATS_EXPORT MysqlUtils {
public:
    /**
        Removes from the table all records with 'field' == id
        Checks 'expectedRowCount' the same way as 'update' method
    */
    static qint64 remove(const QString& table, const QString& field, const U2DataId& id, qint64 expectedRows, MysqlDbRef* db, U2OpStatus& os);

    /** Checks if the table exists in database */
    static bool isTableExists(const QString& tableName, MysqlDbRef* db, U2OpStatus& os);
};

/**
    QSqlQuery wrapper. Uses prepared statement internally
    An optimized and simplified interface for U2DBI needs.
*/
class U2FORMATS_EXPORT U2SqlQuery {
public:
    /**
        Constructs prepared statement for SQLiteDB
        If failed the error message is written to 'os'
    */
    U2SqlQuery(const QString& sql, MysqlDbRef* db, U2OpStatus& os);
    U2SqlQuery(const QString& sql, qint64 offset, qint64 count, MysqlDbRef* db, U2OpStatus& os);
    ~U2SqlQuery();

    //////////////////////////////////////////////////////////////////////////
    // Statement/query state manipulation methods

    /**
        Executes next step of the statement
        Returns true there are more results to fetch and no error occurs
    */
    bool step();

    /**
        Ensures that there are no more results in result set
        Sets error message if more results are available
    */
    void ensureDone();

    /**
        Instructs database driver that no more data will be fetched with the query until it re-executed.
        Bound values retain their values.
    */
    void finish();

    //////////////////////////////////////////////////////////////////////////
    // param binding methods

    /** Binds U2DataId  */
    void bindDataId(const QString &placeholder, const U2DataId& val);
    void addBindDataId(const U2DataId &val);

    void bindNull(const QString &placeholder);
    void addBindNull();

    /** Binds U2DataType */
    void bindType(const QString &placeholder, U2DataType type);
    void addBindType(U2DataType type);

    /** Binds 32bit integer value*/
    void bindInt32(const QString &placeholder, qint32 val);
    void addBindInt32(qint32 val);

    /** Binds 64bit integer value*/
    void bindInt64(const QString &placeholder, qint64 val);
    void addBindInt64(qint64 val);

    /** Binds 64bit real value*/
    void bindDouble(const QString &placeholder, double val);
    void addBindDouble(double val);

    /** Binds bool value */
    void bindBool(const QString &placeholder, bool val);
    void addBindBool(bool val);

    /** Binds text string */
    void bindString(const QString &placeholder, const QString& val);
    void addBindString(const QString &val);

    /** Binds BLOB */
    void bindBlob(const QString &placeholder, const QByteArray& blob);
    void addBindBlob(const QByteArray &blob);

    void bindZeroBlob(const QString &placeholder, int reservedSize);
    void addBindZeroBlob(int reservedSize);

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

    bool getBool(int column) const;

    //////////////////////////////////////////////////////////////////////////
    // Utility methods

    /**
     *Executes update and returns last row id
    */
    qint64 insert();

    /**
     * Executes update and returns last row id converted to U2DataId using type info
    */
    U2DataId insert(U2DataType type, const QByteArray& dbExtra = QByteArray());

    /** Executes query without additional checks*/
    void execute();

    /**
     * Executes update and returns number of rows affected.
     * Returns count of rows affected by the query.
    */
    qint64 update();

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
    QString getQueryText() const;

    void setError(const QString& err) const;

    bool hasError() const;

    void setOpStatus(U2OpStatus& os);

    U2OpStatus& getOpStatus();

    const MysqlDbRef* getDb() const;

private:
    QString getBoundValues() const;

    MysqlDbRef*     db;
    U2OpStatus*     os;
    QSqlQuery       query;
};

class MysqlTransaction {
public:
    MysqlTransaction(MysqlDbRef *db, U2OpStatus &os);
    ~MysqlTransaction();

private:
    MysqlDbRef *db;
    U2OpStatus &os;
    bool started;
};

/** Data loader adapter for SqlQueryIterator */
template <class T> class MysqlRSLoader {
public:
    virtual ~MysqlRSLoader(){}
    virtual T load(U2SqlQuery* q) = 0;
};

/** Filter for SqlRSIterator. Checks if value must be filtered out from the result */
template <class T> class MysqlRSFilter {
public:
    virtual ~MysqlRSFilter(){}
    virtual bool filter(const T&) = 0;
};

/** SQL query result set iterator */
template<class T> class MysqlRSIterator : public U2DbiIterator<T> {
public:
    MysqlRSIterator(QSharedPointer<U2SqlQuery> q, MysqlRSLoader<T>* l, MysqlRSFilter<T>* f, const T& d, U2OpStatus& o)
        : query(q), loader(l), filter(f), defaultValue(d), os(o), endOfStream(false)
    {
        fetchNext();
    }

    virtual ~MysqlRSIterator() {
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

    QSharedPointer<U2SqlQuery>    query;
    MysqlRSLoader<T>* loader;
    MysqlRSFilter<T>* filter;
    T               defaultValue;
    U2OpStatus&     os;
    bool            endOfStream;
    T               nextResult;
    T               currentResult;
    bool            deleteQuery;
};

class MysqlDataIdRSLoader : public MysqlRSLoader<U2DataId> {
public:
    MysqlDataIdRSLoader(U2DataType _type, const QByteArray& _dbExra = QByteArray()) : type(_type), dbExtra(_dbExra){}
    U2DataId load(U2SqlQuery* q) { return q->getDataId(0, type, dbExtra);}

protected:
    U2DataType type;
    QByteArray dbExtra;
};

class MysqlDataIdRSLoaderEx : public MysqlRSLoader<U2DataId> {
public:
    MysqlDataIdRSLoaderEx(const QByteArray& _dbExra = QByteArray()) : dbExtra(_dbExra){}
    U2DataId load(U2SqlQuery* q) { return q->getDataId(0, q->getDataType(1), dbExtra);}

protected:
    U2DataType type;
    QByteArray dbExtra;
};

}   // namespace U2

#endif // _U2_MYSQL_HELPERS_H_
