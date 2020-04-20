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

#include "SQLiteAttributeDbi.h"

#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/Timer.h>

namespace U2 {

SQLiteAttributeDbi::SQLiteAttributeDbi(SQLiteDbi* dbi) : U2AttributeDbi(dbi), SQLiteChildDBICommon(dbi) {
}

void SQLiteAttributeDbi::initSqlSchema(U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }

    // object attribute main table
    // object -> object id this attribute is for
    // child -> optional object id in case if this attribute shows relation between 2 objects
    // otype, ctype -> object and child types
    // oextra, cextra -> object and child db extra
    // version -> object version is attribute is valid for
    // name -> name of the attribute
    SQLiteWriteQuery("CREATE TABLE Attribute (id INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, "
        "object INTEGER, child INTEGER, otype INTEGER NOT NULL, ctype INTEGER, oextra BLOB NOT NULL, cextra BLOB, "
        "version INTEGER NOT NULL, name TEXT NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE)" , db, os).execute();

    //TODO: check if index is efficient for getting attribute for specific object
    SQLiteWriteQuery("CREATE INDEX Attribute_name on Attribute(name)" , db, os).execute();
    SQLiteWriteQuery("CREATE INDEX Attribute_object on Attribute(object)" , db, os).execute();

    SQLiteWriteQuery("CREATE TABLE IntegerAttribute (attribute INTEGER, value INTEGER NOT NULL, "
        "FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE)" , db, os).execute();
    SQLiteWriteQuery("CREATE INDEX IntegerAttribute_attribute on IntegerAttribute(attribute)" , db, os).execute();

    SQLiteWriteQuery("CREATE TABLE RealAttribute (attribute INTEGER, value REAL NOT NULL, "
        "FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE)" , db, os).execute();
    SQLiteWriteQuery("CREATE INDEX RealAttribute_attribute on RealAttribute(attribute)" , db, os).execute();

    SQLiteWriteQuery("CREATE TABLE StringAttribute (attribute INTEGER, value TEXT NOT NULL, "
        "FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE)" , db, os).execute();
    SQLiteWriteQuery("CREATE INDEX StringAttribute_attribute on StringAttribute(attribute)" , db, os).execute();

    SQLiteWriteQuery("CREATE TABLE ByteArrayAttribute (attribute INTEGER, value BLOB NOT NULL, "
        "FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE)" , db, os).execute();
    SQLiteWriteQuery("CREATE INDEX ByteArrayAttribute_attribute on ByteArrayAttribute(attribute)" , db, os).execute();
}

/** Returns all attribute names available in the database */
QStringList SQLiteAttributeDbi::getAvailableAttributeNames(U2OpStatus& os) {
    return SQLiteReadQuery("SELECT DISTINCT name FROM Attribute", db, os).selectStrings();
}

/** Returns all attribute ids for the given object */
QList<U2DataId> SQLiteAttributeDbi::getObjectAttributes(const U2DataId& objectId, const QString& name, U2OpStatus& os) {
    if (name.isEmpty()) {
        static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = ?1 ORDER BY id");
        SQLiteReadQuery q(queryString, db, os);
        q.bindDataId(1, objectId);
        return q.selectDataIdsExt();
    }
    static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = ?1 AND name = ?2 ORDER BY id");
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, objectId);
    q.bindString(2, name);
    return q.selectDataIdsExt();
}

/** Returns all attribute ids for the given object */
QList<U2DataId> SQLiteAttributeDbi::getObjectPairAttributes(const U2DataId& objectId, const U2DataId& childId, const QString& name, U2OpStatus& os) {
    if (name.isEmpty()) {
        static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = ?1 AND child = ?2 ORDER BY id");
        SQLiteReadQuery q(queryString, db, os);
        q.bindDataId(1, objectId);
        q.bindDataId(2, childId);
        return q.selectDataIdsExt();
    }
    static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = ?1 AND child = ?2 AND name = ?3 ORDER BY id");
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, objectId);
    q.bindDataId(2, childId);
    q.bindString(3, name);
    return q.selectDataIdsExt();
}

/** Loads int64 attribute by id */
U2IntegerAttribute SQLiteAttributeDbi::getIntegerAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("IntegerAttribute"));
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, attributeId);
    U2IntegerAttribute res;
    readAttribute(q, res);
    res.value = q.getInt64(0);
    q.ensureDone();
    return res;
}

/** Loads real64 attribute by id */
U2RealAttribute SQLiteAttributeDbi::getRealAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("RealAttribute"));
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, attributeId);
    U2RealAttribute res;
    readAttribute(q, res);
    res.value = q.getDouble(0);
    q.ensureDone();
    return res;
}

/** Loads String attribute by id */
U2StringAttribute SQLiteAttributeDbi::getStringAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("StringAttribute"));
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, attributeId);
    U2StringAttribute res;
    readAttribute(q, res);
    res.value = q.getString(0);
    q.ensureDone();
    return res;
}

/** Loads byte attribute by id */
U2ByteArrayAttribute SQLiteAttributeDbi::getByteArrayAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("ByteArrayAttribute"));
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, attributeId);
    U2ByteArrayAttribute res;
    readAttribute(q, res);
    res.value = q.getBlob(0);
    q.ensureDone();
    return res;
}


QString SQLiteAttributeDbi::buildSelectAttributeQuery(const QString& attributeTable) {
    return "SELECT t.value, a.id, a.type, '', a.object, a.otype, a.oextra, a.child, a.ctype, a.cextra, a.version, a.name "
            " FROM Attribute AS a, " + attributeTable + " AS t WHERE a.id = ?1 AND t.attribute = a.id";
}

void SQLiteAttributeDbi::readAttribute(SQLiteReadQuery &q, U2Attribute& attr) {
    if (q.hasError()) {
        return;
    }
    if (!q.step()) {
        coreLog.details(QString("Query %1 can't execute: required attribute not found in sqlite attribute database").arg(q.getQueryText()));
        return;
    }
    attr.id = q.getDataIdExt(1);
    attr.objectId = q.getDataIdExt(4);
    attr.childId = q.getDataIdExt(7);
    attr.version = q.getInt64(10);
    attr.name = q.getString(11);
}


/** Sorts all objects in database according to U2DbiSortConfig provided  */
QList<U2DataId> SQLiteAttributeDbi::sort(const U2DbiSortConfig& , qint64 , qint64 , U2OpStatus& os) {
    QList<U2DataId> res;
    os.setError("not implemented");
    return res;
}


static void removeAttribute(SQLiteQuery *q, const U2DataId& id) {
    q->reset();
    q->bindDataId(1, id);
    q->execute();
}
/**
Removes attribute from database
Requires U2DbiFeature_WriteAttribute feature support
*/
void SQLiteAttributeDbi::removeAttributes(const QList<U2DataId>& attributeIds, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString qString("DELETE FROM Attribute WHERE id = ?1");
    static const QString qiString("DELETE FROM IntegerAttribute WHERE attribute = ?1");
    static const QString qrString("DELETE FROM RealAttribute WHERE attribute = ?1");
    static const QString qsString("DELETE FROM StringAttribute WHERE attribute = ?1");
    static const QString qbString("DELETE FROM ByteArrayAttribute WHERE attribute = ?1");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(qString, db, os);
    QSharedPointer<SQLiteQuery> qi = t.getPreparedQuery(qiString, db, os);
    QSharedPointer<SQLiteQuery> qr = t.getPreparedQuery(qrString, db, os);
    QSharedPointer<SQLiteQuery> qs = t.getPreparedQuery(qsString, db, os);
    QSharedPointer<SQLiteQuery> qb = t.getPreparedQuery(qbString, db, os);
    foreach(const U2DataId& id, attributeIds) {
        U2DataType type = U2DbiUtils::toType(id);
        switch (type) {
            case U2Type::AttributeInteger:
                removeAttribute(qi.data(), id);
                break;
            case U2Type::AttributeReal:
                removeAttribute(qr.data(), id);
                break;
            case U2Type::AttributeString:
                removeAttribute(qs.data(), id);
                break;
            case U2Type::AttributeByteArray:
                removeAttribute(qb.data(), id);
                break;
            default:
                os.setError(U2DbiL10n::tr("Unsupported attribute type: %1").arg(type));
                break;
        }
        CHECK_OP(os, );

        q->bindDataId(1, id);
        q->execute();
        q->reset();
        CHECK_OP(os, );
    }
}

void SQLiteAttributeDbi::removeObjectAttributes(const U2DataId& objectId, U2OpStatus& os)  {
    SQLiteTransaction t(db, os);
    QList<U2DataId> attributes = getObjectAttributes(objectId, "", os);
    if (!attributes.isEmpty()) {
        removeAttributes(attributes, os);
    }
}

qint64 SQLiteAttributeDbi::createAttribute(U2Attribute& attr, U2DataType type, SQLiteTransaction& t, U2OpStatus& os) {
    static const QString queryString("INSERT INTO Attribute(type, object, child, otype, ctype, oextra, cextra, version, name) "
        " VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, -1);

    q->bindType(1, type);
    q->bindDataId(2, attr.objectId);
    q->bindDataId(3, attr.childId);
    q->bindType(4, U2DbiUtils::toType(attr.objectId));
    q->bindType(5, U2DbiUtils::toType(attr.childId));
    q->bindBlob(6, U2DbiUtils::toDbExtra(attr.objectId));
    q->bindBlob(7, U2DbiUtils::toDbExtra(attr.childId));
    q->bindInt64(8, attr.version);
    q->bindString(9, attr.name);

    return q->insert();
}


/**
Creates int64 attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void SQLiteAttributeDbi::createIntegerAttribute(U2IntegerAttribute& a, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    qint64 id = createAttribute(a, U2Type::AttributeInteger, t, os);
    if (os.hasError()) {
        return;
    }
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeInteger);

    static const QString queryString("INSERT INTO IntegerAttribute(attribute, value) VALUES(?1, ?2)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindInt64(1, id);
    q->bindInt64(2, a.value);
    q->execute();
}

/**
Creates real64 attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void SQLiteAttributeDbi::createRealAttribute(U2RealAttribute& a, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    qint64 id = createAttribute(a, U2Type::AttributeReal, t, os);
    if (os.hasError()) {
        return;
    }
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeReal);

    static const QString queryString("INSERT INTO RealAttribute(attribute, value) VALUES(?1, ?2)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindInt64(1, id);
    q->bindDouble(2, a.value);
    q->execute();
}

/**
Creates String attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void SQLiteAttributeDbi::createStringAttribute(U2StringAttribute& a, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    qint64 id = createAttribute(a, U2Type::AttributeString, t, os);
    if (os.hasError()) {
        return;
    }
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeString);

    static const QString queryString("INSERT INTO StringAttribute(attribute, value) VALUES(?1, ?2)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindInt64(1, id);
    q->bindString(2, a.value);
    q->execute();
}

/**
Creates Byte attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void SQLiteAttributeDbi::createByteArrayAttribute(U2ByteArrayAttribute& a, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    qint64 id = createAttribute(a, U2Type::AttributeByteArray, t, os);
    if (os.hasError()) {
        return;
    }
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeByteArray);

    static const QString queryString("INSERT INTO ByteArrayAttribute(attribute, value) VALUES(?1, ?2)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindInt64(1, id);
    q->bindBlob(2, a.value, false);
    q->execute();
}

} //namespace
