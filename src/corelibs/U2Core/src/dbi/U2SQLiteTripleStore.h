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

#ifndef _U2_QSLITE_TRIPLESTORE_
#define _U2_QSLITE_TRIPLESTORE_

#include <U2Core/global.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class DbRef;
class U2SQLiteTripleStore;

class U2CORE_EXPORT U2Triplet {
    friend class U2SQLiteTripleStore;
public:
    U2Triplet(const QString &key, const QString &role, const QString &value);
    U2Triplet(const U2Triplet& other);
    QString getKey() const;
    QString getRole() const;
    QString getValue() const;
private:
    qint64 id;

    QString key;
    QString role;
    QString value;
};

class U2CORE_EXPORT Owner {
    friend class U2SQLiteTripleStore;
public:
    Owner(const QString &name);
    Owner(const Owner &owner);
    QString getName() const;
private:
    qint64 id;
    QString name;
};

/** SQLite database for keeping triplets: key - role - value */
class U2CORE_EXPORT U2SQLiteTripleStore {
public:
    U2SQLiteTripleStore();
    virtual ~U2SQLiteTripleStore();

    void init(const QString &url, U2OpStatus &os);
    void shutdown(U2OpStatus &os);

    void addValue(const U2Triplet &value, U2OpStatus &os);
    bool contains(const U2Triplet &value, U2OpStatus &os) const;
    void removeValue(const U2Triplet &value, U2OpStatus &os);

    bool contains(const QString &key, const QString &role, U2OpStatus &os) const;
    QString getValue(const QString &key, const QString &role, U2OpStatus &os) const;

    QList<U2Triplet> getTriplets(U2OpStatus &os) const;

private:
    U2DbiState state;
    DbRef *db;

    bool isEmpty(U2OpStatus &os) const;
    void createTables(U2OpStatus &os);

    qint64 getTripletId(const U2Triplet &value, bool &found, U2OpStatus &os) const;
    /** Returns triplet id */
    qint64 insertTriplet(const U2Triplet &value, U2OpStatus &os);
    void removeTriplet(qint64 tripletId, U2OpStatus &os);
};

class TripleStoreL10N : public QObject {
    Q_OBJECT
};

} // U2

#endif // _U2_QSLITE_TRIPLESTORE_
