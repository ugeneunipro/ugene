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

#include <U2Core/U2SafePoints.h>

#include "MysqlAttributeDbi.h"
#include "util/MysqlHelpers.h"

namespace U2 {

MysqlAttributeDbi::MysqlAttributeDbi( MysqlDbi *dbi )
    : U2AttributeDbi( dbi ), MysqlChildDbiCommon( dbi )
{
}

void MysqlAttributeDbi::initSqlSchema( U2OpStatus &os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // object attribute main table
    // object -> object id this attribute is for
    // child -> optional object id in case if this attribute shows relation between 2 objects
    // otype, ctype -> object and child types
    // oextra, cextra -> object and child db extra
    // version -> object version is attribute is valid for
    // name -> name of the attribute
    U2SqlQuery( "CREATE TABLE Attribute (id BIGINT PRIMARY KEY AUTO_INCREMENT, type INTEGER NOT NULL, "
        "object BIGINT, child BIGINT, otype INTEGER NOT NULL, ctype INTEGER, oextra LONGBLOB NOT NULL, "
        "cextra LONGBLOB, version BIGINT NOT NULL, name LONGTEXT NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    //TODO: check if index is efficient for getting attribute for specific object
    U2SqlQuery("CREATE INDEX Attribute_object on Attribute(object)" , db, os).execute();

    U2SqlQuery("CREATE TABLE IntegerAttribute (attribute BIGINT, value BIGINT NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8" , db, os).execute();
    U2SqlQuery("CREATE INDEX IntegerAttribute_attribute on IntegerAttribute(attribute)", db, os).execute();

    U2SqlQuery("CREATE TABLE RealAttribute (attribute BIGINT, value DOUBLE NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    U2SqlQuery("CREATE INDEX RealAttribute_attribute on RealAttribute(attribute)" , db, os).execute();

    U2SqlQuery("CREATE TABLE StringAttribute (attribute BIGINT, value LONGTEXT NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8" , db, os).execute();
    U2SqlQuery("CREATE INDEX StringAttribute_attribute on StringAttribute(attribute)" , db, os).execute();

    U2SqlQuery("CREATE TABLE ByteArrayAttribute (attribute BIGINT, value LONGBLOB NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8" , db, os).execute();
    U2SqlQuery("CREATE INDEX ByteArrayAttribute_attribute on ByteArrayAttribute(attribute)" , db, os).execute();
}

/** Returns all attribute names available in the database */
QStringList MysqlAttributeDbi::getAvailableAttributeNames(U2OpStatus& os) {
    static const QString queryString = "SELECT DISTINCT name FROM Attribute";
    return U2SqlQuery(queryString, db, os).selectStrings();
}

/** Returns all attribute ids for the given object */
QList<U2DataId> MysqlAttributeDbi::getObjectAttributes(const U2DataId& objectId, const QString& name, U2OpStatus& os) {
    if (name.isEmpty()) {
        static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = :object ORDER BY id");
        U2SqlQuery q(queryString, db, os);
        q.bindDataId(":object", objectId);
        return q.selectDataIdsExt();
    }

    static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = :object AND name = :name ORDER BY id");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", objectId);
    q.bindString(":name", name);
    return q.selectDataIdsExt();
}

/** Returns all attribute ids for the given object */
QList<U2DataId> MysqlAttributeDbi::getObjectPairAttributes(const U2DataId& objectId, const U2DataId& childId, const QString& name, U2OpStatus& os) {
    if (name.isEmpty()) {
        static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = :object AND child = :child ORDER BY id");
        U2SqlQuery q(queryString, db, os);
        q.bindDataId(":object", objectId);
        q.bindDataId(":child", childId);
        return q.selectDataIdsExt();
    }

    static const QString queryString("SELECT id, type, '' FROM Attribute WHERE object = :object AND child = :child AND name = :name ORDER BY id");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", objectId);
    q.bindDataId(":child", childId);
    q.bindString(":name", name);
    return q.selectDataIdsExt();
}

/** Loads int64 attribute by id */
U2IntegerAttribute MysqlAttributeDbi::getIntegerAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("IntegerAttribute"));
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", attributeId);

    U2IntegerAttribute res;
    readAttribute(q, res);
    CHECK_OP(os, res);
    res.value = q.getInt64(0);
    q.ensureDone();

    return res;
}

/** Loads real64 attribute by id */
U2RealAttribute MysqlAttributeDbi::getRealAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("RealAttribute"));
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", attributeId);

    U2RealAttribute res;
    readAttribute(q, res);
    CHECK_OP(os, res);
    res.value = q.getDouble(0);
    q.ensureDone();

    return res;
}

/** Loads String attribute by id */
U2StringAttribute MysqlAttributeDbi::getStringAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("StringAttribute"));
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", attributeId);

    U2StringAttribute res;
    readAttribute(q, res);
    CHECK_OP(os, res);
    res.value = q.getString(0);
    q.ensureDone();

    return res;
}

/** Loads byte attribute by id */
U2ByteArrayAttribute MysqlAttributeDbi::getByteArrayAttribute(const U2DataId& attributeId, U2OpStatus& os) {
    static const QString queryString(buildSelectAttributeQuery("ByteArrayAttribute"));
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", attributeId);

    U2ByteArrayAttribute res;
    readAttribute(q, res);
    CHECK_OP(os, res);
    res.value = q.getBlob(0);
    q.ensureDone();

    return res;
}

/** Sorts all objects in database according to U2DbiSortConfig provided  */
QList<U2DataId> MysqlAttributeDbi::sort(const U2DbiSortConfig& , qint64 , qint64 , U2OpStatus& os) {
    os.setError("not implemented");
    return QList<U2DataId>();
}

/**
Removes attribute from database
Requires U2DbiFeature_WriteAttribute feature support
*/
void MysqlAttributeDbi::removeAttributes(const QList<U2DataId>& attributeIds, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString mainQueryStr("DELETE FROM Attribute WHERE id = :attribute");
    static const QString secQueryStr("DELETE FROM %1 WHERE attribute = :attribute");

    static const QString intergerAttrString = "IntegerAttribute";
    static const QString realAttrString = "RealAttribute";
    static const QString stringAttrString = "StringAttribute";
    static const QString bytearrayAttrString = "ByteArrayAttribute";

    QString tableName;
    foreach (const U2DataId& id, attributeIds) {
        U2DataType type = U2DbiUtils::toType(id);
        switch (type) {
            case U2Type::AttributeInteger:
                tableName = intergerAttrString;
                break;
            case U2Type::AttributeReal:
                tableName = realAttrString;
                break;
            case U2Type::AttributeString:
                tableName = stringAttrString;
                break;
            case U2Type::AttributeByteArray:
                tableName = bytearrayAttrString;
                break;
            default:
                os.setError(U2DbiL10n::tr("Unsupported attribute type: %1").arg(type));
                break;
        }

        U2SqlQuery removeAttrDetails(secQueryStr.arg(tableName), db, os);
        removeAttrDetails.bindDataId(":attribute", id);
        removeAttrDetails.execute();
        CHECK_OP(os, );

        U2SqlQuery removeAttr(mainQueryStr, db, os);
        removeAttr.bindDataId(":attribute", id);
        removeAttr.execute();
        CHECK_OP(os, );
    }
}

void MysqlAttributeDbi::removeObjectAttributes(const U2DataId& objectId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QList<U2DataId> attributes = getObjectAttributes(objectId, "", os);
    CHECK_OP(os, );
    if (!attributes.isEmpty()) {
        removeAttributes(attributes, os);
    }
}

/**
 * Creates int64 attribute in database. ObjectId must be already set in attribute and present in the same database
 * Requires U2DbiFeature_WriteAttribute feature support
 */
void MysqlAttributeDbi::createIntegerAttribute(U2IntegerAttribute& a, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 id = createAttribute(a, U2Type::AttributeInteger, os);
    CHECK_OP(os, );
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeInteger);

    static const QString queryString("INSERT INTO IntegerAttribute(attribute, value) VALUES(:attribute, :value)");
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":attribute", id);
    q.bindInt64(":value", a.value);
    q.execute();
}

/**
Creates real64 attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void MysqlAttributeDbi::createRealAttribute(U2RealAttribute& a, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 id = createAttribute(a, U2Type::AttributeReal, os);
    CHECK_OP(os, );
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeReal);

    static const QString queryString("INSERT INTO RealAttribute(attribute, value) VALUES(:attribute, :value)");
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":attribute", id);
    q.bindDouble(":value", a.value);
    q.execute();
}

/**
Creates String attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void MysqlAttributeDbi::createStringAttribute(U2StringAttribute& a, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 id = createAttribute(a, U2Type::AttributeString, os);
    CHECK_OP(os, );
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeString);

    static const QString queryString("INSERT INTO StringAttribute(attribute, value) VALUES(:attribute, :value)");
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":attribute", id);
    q.bindString(":value", a.value.isNull() ? "" : a.value);
    q.execute();
}

/**
Creates Byte attribute in database. ObjectId must be already set in attribute and present in the same database
Requires U2DbiFeature_WriteAttribute feature support
*/
void MysqlAttributeDbi::createByteArrayAttribute(U2ByteArrayAttribute& a, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 id = createAttribute(a, U2Type::AttributeByteArray, os);
    CHECK_OP(os, );
    a.id = U2DbiUtils::toU2DataId(id, U2Type::AttributeByteArray);

    static const QString queryString("INSERT INTO ByteArrayAttribute(attribute, value) VALUES(:attribute, :value)");
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":attribute", id);
    q.bindBlob(":value", a.value.isNull() ? "" : a.value); // not null field
    q.execute();
}

qint64 MysqlAttributeDbi::createAttribute(U2Attribute& attr, U2DataType type, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString("INSERT INTO Attribute(type, object, child, otype, ctype, oextra, cextra, version, name) "
        " VALUES(:type, :object, :child, :otype, :ctype, :oextra, :cextra, :version, :name)");
    U2SqlQuery q(queryString, db, os);

    q.bindType(":type", type);
    q.bindDataId(":object", attr.objectId);
    q.bindDataId(":child", attr.childId);
    q.bindType(":otype", U2DbiUtils::toType(attr.objectId));
    q.bindType(":ctype", U2DbiUtils::toType(attr.childId));
    q.bindBlob(":oextra", U2DbiUtils::toDbExtra(attr.objectId));
    q.bindBlob(":cextra", U2DbiUtils::toDbExtra(attr.childId));
    q.bindInt64(":version", attr.version);
    q.bindString(":name", attr.name);

    return q.insert();
}

QString MysqlAttributeDbi::buildSelectAttributeQuery(const QString& attributeTable) {
    return "SELECT t.value, a.id, a.type, '', a.object, a.otype, a.oextra, a.child, a.ctype, a.cextra, a.version, a.name "
            " FROM Attribute AS a, " + attributeTable + " AS t WHERE a.id = :id AND t.attribute = a.id";
}

void MysqlAttributeDbi::readAttribute(U2SqlQuery &q, U2Attribute& attr) {
    if (!q.step()) {
        if (!q.hasError()) {
            q.setError(U2DbiL10n::tr("Required attribute is not found"));
        }
        return;
    }

    attr.id = q.getDataIdExt(1);
    attr.objectId = q.getDataIdExt(4);
    attr.childId = q.getDataIdExt(7);
    attr.version = q.getInt64(10);
    attr.name = q.getString(11);
}

}   // namespace U2
