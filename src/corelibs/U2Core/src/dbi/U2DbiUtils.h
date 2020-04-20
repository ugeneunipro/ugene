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

#ifndef _U2_DBI_UTILS_H_
#define _U2_DBI_UTILS_H_

#include <U2Core/StrPackUtils.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2DbiRegistry.h>

namespace U2 {

class U2OpStatus;

/**
    Helper class to track DBI instance live range
*/
class U2CORE_EXPORT TmpDbiHandle {
public:
    TmpDbiHandle();

    TmpDbiHandle(const QString& alias, U2OpStatus& os,
        const U2DbiFactoryId &factoryId = DEFAULT_DBI_ID);

    TmpDbiHandle(const TmpDbiHandle& dbiHandle);

    TmpDbiHandle& operator=(const TmpDbiHandle& dbiHandle);

    ~TmpDbiHandle();

    bool isValid() const {return dbiRef.isValid();}

    inline QString getAlias() const { return alias; }

    inline U2DbiRef getDbiRef() const { return dbiRef; }

private:
    /** DBI alias */
    QString             alias;

    /** DBI reference */
    U2DbiRef            dbiRef;
};

class U2CORE_EXPORT TmpDbiObjects {
public:
    TmpDbiObjects(const U2DbiRef& _dbiRef, U2OpStatus& _os) : dbiRef(_dbiRef), os(_os){}
    ~TmpDbiObjects();

    U2DbiRef            dbiRef;
    QList<U2DataId>     objects;
    U2OpStatus&         os;
};

/**
    Iterator over buffered data set
*/
template<class T> class BufferedDbiIterator : public U2DbiIterator<T> {
public:
    BufferedDbiIterator(const QList<T>& _buffer, const T& _errValue = T()) : buffer(_buffer), pos(0), errValue(_errValue) {}

    /** returns true if there are more reads to iterate*/
    virtual bool hasNext() {
        return pos < buffer.size();
    }

    /** returns next read and shifts one element*/
    virtual T next() {
        if (!hasNext()) {
            return errValue;
        }
        const T& res = buffer.at(pos);
        pos++;
        return res;
    }

    /** returns next read without shifting*/
    virtual T peek() {
        if (!hasNext()) {
            return errValue;
        }
        return buffer.at(pos);

    }

private:
    QList<T>    buffer;
    int         pos;
    T           errValue;
};


class U2CORE_EXPORT U2DbiUtils : public QObject{
    Q_OBJECT
public:
    /**
        Logs that operation called is not supported by DBI
        If U2OpStatus has no error set, sets the error message
    */
    static void logNotSupported(U2DbiFeature f, U2Dbi* dbi, U2OpStatus& os);

    template<class T> static QList<T> toList(U2DbiIterator<T>* it);

    static U2DbiRef toRef(U2Dbi* dbi);

    /** Adds limit operator to the sql query */
    static void addLimit(QString& sql, qint64 offset, qint64 count);

    /** Converts internal database id to U2DataId*/
    static U2DataId toU2DataId(qint64 id, U2DataType type, const QByteArray& dbExtra = QByteArray());

    /** Converts U2DataId to internal database id*/
    static quint64 toDbiId(const U2DataId& id);

    /** Extracts type info from U2DataId */
    static U2DataType toType(const U2DataId& id);

    /** Extracts table info from U2DataId */
    static QByteArray toDbExtra(const U2DataId& id);

    /** Return textual representation of the id */
    static QString text(const U2DataId& id);

    static QString ref2Url(const U2DbiRef& dbiRef);

    /**
     * Dbi url (short dbi url) looks like "host:port/database_name"
     * Full dbi url looks like "user_name@host:port/database_name"
     * Short dbi url is used where user name is not defined. It should be converted
     * to the full url before using.
     */
    static QString createDbiUrl(const QString &host, int port, const QString &dbName);
    static QString createFullDbiUrl(const QString &userName, const QString &host, int port, const QString &dbName);
    static QString createFullDbiUrl(const QString &userName, const QString &dbiUrl);

    static bool parseDbiUrl(const QString& dbiUrl, QString& host, int& port, QString& dbName);
    static bool parseFullDbiUrl(const QString& dbiUrl, QString &userName, QString& host, int& port, QString& dbName);

    static QString full2shortDbiUrl(const QString& fullDbiUrl);
    static QString full2shortDbiUrl(const QString& fullDbiUrl, QString &userName);
    static bool isFullDbiUrl(const QString& dbiUrl);

    static QString makeFolderCanonical(const QString& folder);

    static bool isDbiReadOnly(const U2DbiRef &dbiRef);
    static Version getDbMinRequiredVersion(const U2DbiRef &dbiRef, U2OpStatus &os);
    static bool isDatabaseTooNew(const U2DbiRef &dbiRef, const Version &ugeneVersion, QString &minRequiredVersionString, U2OpStatus &os);
    static bool isDatabaseTooOld(const U2DbiRef &dbiRef, const Version &ugeneVersion, U2OpStatus &os);

    static const QString PUBLIC_DATABASE_NAME;
    static const QString PUBLIC_DATABASE_URL;
    static const QString PUBLIC_DATABASE_LOGIN;
    static const QString PUBLIC_DATABASE_PASSWORD;
};

/**
    This helper class provides a way to make the block of operations through the one transaction
*/
class U2CORE_EXPORT DbiOperationsBlock {
public:
    DbiOperationsBlock(const U2DbiRef &dbiRef, U2OpStatus &os);
    ~DbiOperationsBlock();

private:
    U2DbiRef dbiRef;
    DbiConnection *connection;
    U2OpStatus& os;
};

template<class T> QList<T> U2DbiUtils::toList(U2DbiIterator<T>* it) {
    QList<T> result;
    while (it->hasNext()) {
        result << it->next();
    }
    return result;
}

}   // namespace U2

#endif
