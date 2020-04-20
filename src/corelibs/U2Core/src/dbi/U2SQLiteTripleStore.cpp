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

#include <3rdparty/sqlite3/sqlite3.h>

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include "U2SQLiteTripleStore.h"

namespace U2 {

/************************************************************************/
/* Triplet */
/************************************************************************/
U2Triplet::U2Triplet(const QString &_key, const QString &_role, const QString &_value)
: id(-1), key(_key), role(_role), value(_value)
{

}

U2Triplet::U2Triplet(const U2Triplet& other)
: id(other.id), key(other.key), role(other.role), value(other.value)
{

}

QString U2Triplet::getKey() const {
    return key;
}

QString U2Triplet::getRole() const {
    return role;
}

QString U2Triplet::getValue() const {
    return value;
}

/************************************************************************/
/* Owner */
/************************************************************************/
Owner::Owner(const QString &_name)
: id(-1), name(_name)
{

}

Owner::Owner(const Owner &owner) {
    id = owner.id;
    name = owner.name;
}

QString Owner::getName() const {
    return name;
}

/************************************************************************/
/* U2SQLiteTripleStore */
/************************************************************************/
U2SQLiteTripleStore::U2SQLiteTripleStore() {
    state = U2DbiState_Void;
    db = new DbRef();
}

U2SQLiteTripleStore::~U2SQLiteTripleStore() {
    delete db;
}

void U2SQLiteTripleStore::init(const QString &url, U2OpStatus &os) {
    if (db->handle != NULL) {
        os.setError(TripleStoreL10N::tr("Database is already opened!"));
        return;
    }
    if (state != U2DbiState_Void) {
        os.setError(TripleStoreL10N::tr("Illegal database state: %1").arg(state));
        return;
    }
    state = U2DbiState_Starting;
    if (url.isEmpty()) {
        os.setError(TripleStoreL10N::tr("URL is not specified"));
        state = U2DbiState_Void;
        return;
    }
    do {
        int flags = SQLITE_OPEN_READWRITE;
        flags |= SQLITE_OPEN_CREATE;
        QByteArray file = url.toUtf8();
        int rc = sqlite3_open_v2(file.constData(), &db->handle, flags, NULL);
        if (rc != SQLITE_OK) {
            QString err = db->handle == NULL ? QString(" error-code: %1").arg(rc) : QString(sqlite3_errmsg(db->handle));
            os.setError(TripleStoreL10N::tr("Error opening SQLite database: %1!").arg(err));
            break;
        }

        SQLiteWriteQuery("PRAGMA synchronous = OFF", db, os).execute();
        SQLiteWriteQuery("PRAGMA main.locking_mode = NORMAL", db, os).execute();
        SQLiteWriteQuery("PRAGMA temp_store = MEMORY", db, os).execute();
        SQLiteWriteQuery("PRAGMA journal_mode = MEMORY", db, os).execute();
        SQLiteWriteQuery("PRAGMA cache_size = 10000", db, os).execute();

        // check if the opened database is valid sqlite dbi
        if (isEmpty(os)) {
            createTables(os);
            if (os.hasError()) {
                break;
            }
        }

        // OK, initialization complete
        if (!os.hasError()) {
            ioLog.trace(QString("SQLite: initialized: %1\n").arg(url));
        }
    } while (0);

    if (os.hasError()) {
        sqlite3_close(db->handle);
        db->handle = NULL;
        state = U2DbiState_Void;
        return;
    }
    state = U2DbiState_Ready;
}

static int isEmptyCallback(void *o, int argc, char ** /*argv*/, char ** /*column*/) {
    int* res = (int*)o;
    *res = argc;
    return 0;
}

bool U2SQLiteTripleStore::isEmpty(U2OpStatus &os) const {
    QMutexLocker lock(&db->lock);
    QByteArray showTablesQuery = "SELECT * FROM sqlite_master WHERE type='table';";
    int nTables = 0;
    char* err;
    int rc = sqlite3_exec(db->handle, showTablesQuery.constData(), isEmptyCallback, &nTables, &err);
    if (rc != SQLITE_OK) {
        os.setError(TripleStoreL10N::tr("Error checking SQLite database: %1!").arg(err));
        sqlite3_free(err);
        return false;
    }
    return nTables == 0;
}

void U2SQLiteTripleStore::createTables(U2OpStatus &os) {
    QMutexLocker lock(&db->lock);
    SQLiteWriteQuery("CREATE TABLE Triplets (id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "key TEXT NOT NULL, role TEXT NOT NULL, value TEXT NOT NULL)", db, os).execute();
}

void U2SQLiteTripleStore::shutdown(U2OpStatus &os) {
    if (db == NULL) {
        os.setError(TripleStoreL10N::tr("Database is already closed!"));
        return;
    }
    if (state != U2DbiState_Ready) {
        os.setError(TripleStoreL10N::tr("Illegal database state %1!").arg(state));
        return;
    }

    state = U2DbiState_Stopping;
    {
        int rc = sqlite3_close(db->handle);

        if (rc != SQLITE_OK) {
            QString err = db->handle == NULL ? QString(" error-code: %1").arg(rc) : QString(sqlite3_errmsg(db->handle));
            ioLog.error(TripleStoreL10N::tr("Failed to close triple store database: %1").arg(err));
        }
        db->handle = NULL;
    }
    state = U2DbiState_Void;
    return;
}

void U2SQLiteTripleStore::addValue(const U2Triplet &value, U2OpStatus &os) {
    QMutexLocker lock(&db->lock);
    bool found = false;

    // find triplet
    this->getTripletId(value, found, os);
    CHECK_OP(os, );

    if (!found) { // insert triplet
        this->insertTriplet(value, os);
        CHECK_OP(os, );
    }
}

bool U2SQLiteTripleStore::contains(const QString &key, const QString &role, U2OpStatus &os) const {
    QMutexLocker lock(&db->lock);
    QString value = this->getValue(key, role, os);
    return !value.isEmpty();
}

bool U2SQLiteTripleStore::contains(const U2Triplet &value, U2OpStatus &os) const {
    QMutexLocker lock(&db->lock);
    static const QString queryString("SELECT t.id FROM Triplets t WHERE t.key = ?1 AND t.role = ?2 AND t.value = ?3");
    SQLiteReadQuery q(queryString, db, os);
    q.bindString(1, value.getKey());
    q.bindString(2, value.getRole());
    q.bindString(3, value.getValue());

    if (q.step()) {
        QString result = q.getString(0);
        q.ensureDone();
        return true;
    }
    return false;
}

QString U2SQLiteTripleStore::getValue(const QString &key, const QString &role, U2OpStatus &os) const {
    QMutexLocker lock(&db->lock);
    static const QString queryString("SELECT t.value FROM Triplets t WHERE t.key = ?1 AND t.role = ?2 ORDER BY t.id");
    SQLiteReadQuery q(queryString, db, os);
    q.bindString(1, key);
    q.bindString(2, role);

    QStringList results;
    while (q.step()) {
        results << q.getString(0);
    }
    return results.isEmpty() ? "" : results.last();
}

qint64 U2SQLiteTripleStore::getTripletId(const U2Triplet &triplet, bool &found, U2OpStatus &os) const {
    QMutexLocker lock(&db->lock);
    static const QString queryString("SELECT t.id FROM Triplets t WHERE t.key = ?1 AND t.role = ?2 AND t.value = ?3");
    SQLiteReadQuery q(queryString, db, os);
    q.bindString(1, triplet.getKey());
    q.bindString(2, triplet.getRole());
    q.bindString(3, triplet.getValue());

    found = false;
    if (q.step()) {
        qint64 dataId = q.getInt64(0);
        q.ensureDone();
        found = true;
        return dataId;
    }
    return 0;
}

qint64 U2SQLiteTripleStore::insertTriplet(const U2Triplet &triplet, U2OpStatus &os) {
    QMutexLocker lock(&db->lock);
    static const QString queryString("INSERT INTO Triplets(key, role, value) VALUES(?1, ?2, ?3)");
    SQLiteWriteQuery q(queryString, db, os);
    q.bindString(1, triplet.getKey());
    q.bindString(2, triplet.getRole());
    q.bindString(3, triplet.getValue());

    return q.insert();
}

void U2SQLiteTripleStore::removeTriplet(qint64 tripletId, U2OpStatus &os) {
    QMutexLocker lock(&db->lock);
    static const QString queryString("DELETE FROM Triplets WHERE id = ?1");
    SQLiteWriteQuery q(queryString, db, os);
    q.bindInt64(1, tripletId);

    q.execute();
}

QList<U2Triplet> U2SQLiteTripleStore::getTriplets(U2OpStatus &os) const {
    QMutexLocker lock(&db->lock);
    static const QString queryString("SELECT t.id, t.key, t.role, t.value FROM Triplets t");
    SQLiteReadQuery q(queryString, db, os);

    QList<U2Triplet> result;
    while (q.step()) {
        U2Triplet t(q.getString(1), q.getString(2), q.getString(3));
        t.id = q.getInt64(0);
        result << t;
    }
    return result;
}

void U2SQLiteTripleStore::removeValue(const U2Triplet &value, U2OpStatus &os) {
    QMutexLocker lock(&db->lock);
    static const QString queryString("DELETE FROM Triplets WHERE id = ?1");
    SQLiteWriteQuery q(queryString, db, os);
    q.bindInt64(1, value.id);

    q.execute();
}

} // U2
