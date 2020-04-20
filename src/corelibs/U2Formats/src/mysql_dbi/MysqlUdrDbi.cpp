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

#include <U2Core/AppContext.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchemaRegistry.h>

#include "MysqlBlobInputStream.h"
#include "MysqlBlobOutputStream.h"
#include "MysqlObjectDbi.h"
#include "MysqlUdrDbi.h"

#include "util/MysqlHelpers.h"
#include "util/MysqlModificationAction.h"

static const QString PLACEHOLDER_MARK = ":";

namespace U2 {

namespace {
    const QString TABLE_PREFIX = "UdrSchema_";

    QString tableName(const UdrSchemaId &schemaId) {
        return TABLE_PREFIX + schemaId;
    }
}

MysqlUdrDbi::MysqlUdrDbi(MysqlDbi *dbi)
    : UdrDbi(dbi), MysqlChildDbiCommon(dbi)
{

}

void MysqlUdrDbi::undo(const U2SingleModStep& modStep, U2OpStatus& os) {
    SAFE_POINT_EXT(modStep.modType == U2ModType::udrUpdated, os.setError("Unknown modStep"), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QByteArray oldData;
    QByteArray newData;
    bool ok =  U2DbiPackUtils::unpackUdr(modStep.details, oldData, newData);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating UDR")), );

    RawDataUdrSchema::writeContent(oldData, U2EntityRef(getRootDbi()->getDbiRef(), modStep.objectId), os);
}

void MysqlUdrDbi::redo(const U2SingleModStep& modStep, U2OpStatus& os) {
    SAFE_POINT_EXT(modStep.modType == U2ModType::udrUpdated, os.setError("Unknown modStep"), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QByteArray oldData;
    QByteArray newData;
    bool ok = U2DbiPackUtils::unpackUdr(modStep.details, oldData, newData);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating UDR")), );

    RawDataUdrSchema::writeContent(newData, U2EntityRef(getRootDbi()->getDbiRef(), modStep.objectId), os);
}

UdrRecordId MysqlUdrDbi::addRecord(const UdrSchemaId &schemaId, const QList<UdrValue> &data, U2OpStatus &os) {
    UdrRecordId result("", "");
    const UdrSchema *schema = udrSchema(schemaId, os);
    CHECK_OP(os, result);
    CHECK_EXT(data.size() == schema->size(), os.setError("Size mismatch"), result);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q(insertDef(schema, os), db, os);
    CHECK_OP(os, result);

    bindData(data, schema, q, os);
    CHECK_OP(os, result);

    U2DataId recordId = q.insert(U2Type::UdrRecord);
    return UdrRecordId(schemaId, recordId);
}

void MysqlUdrDbi::updateRecord(const UdrRecordId &recordId, const QList<UdrValue> &data, U2OpStatus &os) {
    const UdrSchema *schema = udrSchema(recordId.getSchemaId(), os);
    CHECK_OP(os, );
    CHECK_EXT(data.size() == schema->size(), os.setError("Size mismatch"), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q(updateDef(schema, os), db, os);
    CHECK_OP(os, );

    bindData(data, schema, q, os);
    CHECK_OP(os, );

    q.bindDataId(PLACEHOLDER_MARK + UdrSchema::RECORD_ID_FIELD_NAME, recordId.getRecordId());

    q.update();
}

UdrRecord MysqlUdrDbi::getRecord(const UdrRecordId &recordId, U2OpStatus &os) {
    UdrRecord result(recordId, QList<UdrValue>(), os);
    const UdrSchema *schema = udrSchema(recordId.getSchemaId(), os);
    CHECK_OP(os, result);

    U2SqlQuery q(selectDef(schema, os), db, os);
    CHECK_OP(os, result);

    q.bindDataId(PLACEHOLDER_MARK + QString(UdrSchema::RECORD_ID_FIELD_NAME), recordId.getRecordId());

    bool ok = q.step();
    CHECK_EXT(ok, os.setError("Unknown record id"), result);

    QList<UdrValue> data;
    retreiveData(data, schema, q, os);
    CHECK_OP(os, result);
    q.ensureDone();
    CHECK_OP(os, result);

    return UdrRecord(recordId, data, os);
}

void MysqlUdrDbi::createObject(const UdrSchemaId &schemaId, U2Object &udrObject, const QString &folder, U2OpStatus &os) {
    const UdrSchema *schema = udrSchema(schemaId, os);
    CHECK_OP(os, );
    SAFE_POINT_EXT(schema->hasObjectReference(), os.setError("No object reference"), );

    dbi->getMysqlObjectDbi()->createObject(udrObject, folder, U2DbiObjectRank_TopLevel, os);
}

QList<U2DataId> MysqlUdrDbi::getObjectRecordIds(const UdrSchema *schema, const U2DataId &objectId, U2OpStatus &os) {
    QList<U2DataId> result;
    SAFE_POINT_EXT(schema->hasObjectReference(), os.setError("No object reference"), result);

    U2SqlQuery q("SELECT " + UdrSchema::RECORD_ID_FIELD_NAME + " FROM " + tableName(schema->getId())
        + " WHERE " + UdrSchema::OBJECT_FIELD_NAME + " = :obj", db, os);
    q.bindDataId(":obj", objectId);

    while (q.step()) {
        result << q.getDataId(0, U2Type::UdrRecord);
    }
    return result;
}

QList<UdrRecord> MysqlUdrDbi::getObjectRecords(const UdrSchemaId &schemaId, const U2DataId &objectId, U2OpStatus &os) {
    QList<UdrRecord> result;
    const UdrSchema *schema = udrSchema(schemaId, os);
    CHECK_OP(os, result);

    const QList<U2DataId> ids = getObjectRecordIds(schema, objectId, os);
    CHECK_OP(os, result);

    foreach (const U2DataId &id, ids) {
        result << getRecord(UdrRecordId(schemaId, id), os);
        CHECK_OP(os, result);
    }

    return result;
}

QList<UdrRecord> MysqlUdrDbi::getRecords(const UdrSchemaId &schemaId, U2OpStatus &os) {
    QList<UdrRecord> result;
    const UdrSchema *schema = udrSchema(schemaId, os);
    CHECK_OP(os, result);

    U2SqlQuery q(selectDef(schema, os), db, os);
    CHECK_OP(os, result);

    while (q.step()) {
        QList<UdrValue> data;
        retreiveData(data, schema, q, os);
        CHECK_OP(os, result);
        U2DataId dataId = q.getDataId(0, U2Type::UdrRecord);
        result << UdrRecord(UdrRecordId(schemaId, dataId), data, os);
    }

    return result;
}

void MysqlUdrDbi::removeRecord(const UdrRecordId &recordId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q("DELETE FROM " + tableName(recordId.getSchemaId())
        + " WHERE " + UdrSchema::RECORD_ID_FIELD_NAME + " = :id", db, os);
    q.bindDataId(":id", recordId.getRecordId());
    q.execute();
}

InputStream * MysqlUdrDbi::createInputStream(const UdrRecordId &recordId, int fieldNum, U2OpStatus &os) {
    const UdrSchema *schema = udrSchema(recordId.getSchemaId(), os);
    CHECK_OP(os, NULL);

    UdrSchema::FieldDesc field = UdrSchema::getBlobField(schema, fieldNum, os);
    CHECK_OP(os, NULL);

    return new MysqlBlobInputStream(db, tableName(recordId.getSchemaId()).toLatin1(), field.getName(), recordId.getRecordId(), os);
}

OutputStream * MysqlUdrDbi::createOutputStream(const UdrRecordId &recordId, int fieldNum, qint64 size, U2OpStatus &os) {
    CHECK_EXT(size >= 0, os.setError("Negative stream size"), NULL);
    CHECK_EXT(size <= INT_MAX, os.setError("Too big stream size"), NULL);

    const UdrSchema *schema = udrSchema(recordId.getSchemaId(), os);
    CHECK_OP(os, NULL);

    UdrSchema::FieldDesc field = UdrSchema::getBlobField(schema, fieldNum, os);
    CHECK_OP(os, NULL);

    return new MysqlBlobOutputStream(db, tableName(recordId.getSchemaId()).toLatin1(), field.getName(), recordId.getRecordId(), (int)size, os);
}

ModificationAction* MysqlUdrDbi::getModificationAction(const U2DataId &id) {
    return new MysqlModificationAction(dbi, id);
}

/************************************************************************/
/* SQL initialization */
/************************************************************************/
void MysqlUdrDbi::initSqlSchema(U2OpStatus &os) {
    UdrSchemaRegistry *udrRegistry = AppContext::getUdrSchemaRegistry();
    SAFE_POINT_EXT(NULL != udrRegistry, os.setError("NULL UDR registry"), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    foreach (const UdrSchemaId &id, udrRegistry->getRegisteredSchemas()) {
        const UdrSchema *schema = udrSchema(id, os);
        CHECK_OP(os, );
        initSchema(schema, os);
        CHECK_OP(os, );
    }
}

void MysqlUdrDbi::initSchema(const UdrSchema *schema, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    CHECK_EXT(NULL != schema, os.setError("NULL schema"), );
    createTable(schema, os);
    CHECK_OP(os, );

    foreach (const QStringList index, indexes(schema, os)) {
        createIndex(schema->getId(), index, os);
        CHECK_OP(os, );
    }
}

void MysqlUdrDbi::createTable(const UdrSchema *schema, U2OpStatus &os) {
    CHECK_EXT(schema->size() > 0, os.setError("Empty schema"), );

    QString query = tableStartDef(schema->getId());
    CHECK_OP(os, );
    for (int i=0; i<schema->size(); i++) {
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, );
        query += ", " + fieldDef(field);
        CHECK_OP(os, );
    }
    query += foreignKeysDef(schema, os);
    CHECK_OP(os, );
    query += ") ENGINE=InnoDB DEFAULT CHARSET=utf8";

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery(query, db, os).execute();
}

void MysqlUdrDbi::createIndex(const UdrSchemaId &schemaId, const QStringList &fields, U2OpStatus &os) {
    QString query = "CREATE INDEX "
        + tableName(schemaId) + "_" + fields.join("_") + " "
        + "on " + tableName(schemaId)
        + "("
        + fields.join(", ")
        + ")";

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery(query, db, os).execute();
}

/************************************************************************/
/* Utilities */
/************************************************************************/
const UdrSchema * MysqlUdrDbi::udrSchema(const UdrSchemaId &schemaId, U2OpStatus &os) {
    UdrSchemaRegistry *udrRegistry = AppContext::getUdrSchemaRegistry();
    SAFE_POINT_EXT(NULL != udrRegistry, os.setError("NULL UDR registry"), NULL);

    const UdrSchema *schema = udrRegistry->getSchemaById(schemaId);
    SAFE_POINT_EXT(NULL != schema, os.setError("NULL UDR schema"), NULL);
    return schema;
}

QString MysqlUdrDbi::insertDef(const UdrSchema *schema, U2OpStatus &os) {
    const QStringList fieldNames = UdrSchema::fieldNames(schema, os);
    CHECK_OP(os, "");

    QStringList placeholders;
    foreach ( const QString &name, fieldNames ) {
        placeholders << ":" + name;
    }

    return "INSERT INTO " + tableName(schema->getId())
        + "(" + fieldNames.join(", ") + ") "
        + "VALUES(" + placeholders.join(", ") + ")";
}

QString MysqlUdrDbi::updateDef(const UdrSchema *schema, U2OpStatus &os) {
    QStringList assignments;
    for (int i=0; i<schema->size(); i++) {
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, "");
        assignments << QString("%1 = :%1").arg(field.getName().constData());
    }

    return "UPDATE " + tableName(schema->getId())
        + " SET " + assignments.join(", ")
        + " WHERE " + QString("%1 = :%1").arg(UdrSchema::RECORD_ID_FIELD_NAME.constData());
}

QString MysqlUdrDbi::selectAllDef(const UdrSchema *schema, U2OpStatus &os) {
    QList<int> directFields = UdrSchema::notBinary(schema, os);
    CHECK_OP(os, "");

    const bool isObjectReferenced = schema->hasObjectReference();

    return "SELECT " + UdrSchema::RECORD_ID_FIELD_NAME + ", "
        + UdrSchema::fieldNames(schema, os, directFields).join(", ")
        + (isObjectReferenced ? ", o.type" : "")
        + " FROM " + tableName(schema->getId())
        + (isObjectReferenced ? " AS udr INNER JOIN Object AS o ON o.id = udr." + UdrSchema::OBJECT_FIELD_NAME : "");
}

QString MysqlUdrDbi::selectDef(const UdrSchema *schema, U2OpStatus &os) {
    return selectAllDef(schema, os)
        + " WHERE " + UdrSchema::RECORD_ID_FIELD_NAME + " = :" + UdrSchema::RECORD_ID_FIELD_NAME;
}

QString MysqlUdrDbi::tableStartDef(const UdrSchemaId &schemaId) {
    return "CREATE TABLE " + tableName(schemaId) + " (" +
        UdrSchema::RECORD_ID_FIELD_NAME + " BIGINT PRIMARY KEY AUTO_INCREMENT";
}

QString MysqlUdrDbi::fieldDef(const UdrSchema::FieldDesc &field) {
    QString def = field.getName() + " ";
    switch (field.getDataType()) {
        case UdrSchema::INTEGER:
            def += "BIGINT";
            break;
        case UdrSchema::DOUBLE:
            def += "DOUBLE";
            break;
        case UdrSchema::STRING:
            def += "LONGTEXT";
            break;
        case UdrSchema::BLOB:
            def += "LONGBLOB";
            break;
        case UdrSchema::ID:
            def += "BIGINT NOT NULL";
            break;
        default:
            FAIL("Unknown UDR data type detected!", QString());
    }
    return def;
}

QString MysqlUdrDbi::foreignKeysDef(const UdrSchema *schema, U2OpStatus &os) {
    QString result;

    for (int i=0; i<schema->size(); i++) {
        const UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, "");

        if (UdrSchema::ID == field.getDataType()) {
            result += ", FOREIGN KEY(" + field.getName() + ") REFERENCES Object(id) ON DELETE CASCADE";
        }
    }

    return result;
}

QList< QStringList > MysqlUdrDbi::indexes(const UdrSchema *schema, U2OpStatus &os) {
    QList< QStringList > result;

    // single column indexes
    for (int i=0; i<schema->size(); i++) {
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, result);
        if (UdrSchema::INDEXED == field.getIndexType()) {
            QStringList index;
            index << field.getName();
            result << index;
        }
    }

    // multi column indexes
    foreach (const QList<int> &multiIndex, schema->getMultiIndexes()) {
        result << UdrSchema::fieldNames(schema, os, multiIndex);
        CHECK_OP(os, result);
    }

    return result;
}

void MysqlUdrDbi::bindData(const QList<UdrValue> &data, const UdrSchema *schema, U2SqlQuery &q, U2OpStatus &os) {
    for (int i=0; i<data.size(); i++) {
        const UdrValue &value = data[i];
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, );

        switch (field.getDataType()) {
            case UdrSchema::INTEGER:
                q.bindInt64(PLACEHOLDER_MARK + field.getName(), value.getInt(os));
                break;
            case UdrSchema::DOUBLE:
                q.bindDouble(PLACEHOLDER_MARK + field.getName(), value.getDouble(os));
                break;
            case UdrSchema::STRING:
                q.bindString(PLACEHOLDER_MARK + field.getName(), value.getString(os));
                break;
            case UdrSchema::BLOB:
                q.bindBlob(PLACEHOLDER_MARK + field.getName(), "");
                break;
            case UdrSchema::ID:
                q.bindDataId(PLACEHOLDER_MARK + field.getName(), value.getDataId(os));
                break;
            default:
                FAIL("Unknown UDR data type detected!", );
        }
        CHECK_OP(os, );
    }
}

void MysqlUdrDbi::retreiveData(QList<UdrValue> &data, const UdrSchema *schema, U2SqlQuery &q, U2OpStatus &os) {
    QList<int> fields = UdrSchema::notBinary(schema, os);
    CHECK_OP(os, );

    for (int i=0; i<schema->size(); i++) {
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, );
        int colNum = -1;
        if (UdrSchema::BLOB != field.getDataType()) {
            colNum = fields.lastIndexOf(i) + 1;
        }
        switch (field.getDataType()) {
            case UdrSchema::INTEGER:
                data << UdrValue(q.getInt64(colNum));
                break;
            case UdrSchema::DOUBLE:
                data << UdrValue(q.getDouble(colNum));
                break;
            case UdrSchema::STRING:
                data << UdrValue(q.getString(colNum));
                break;
            case UdrSchema::BLOB:
                data << UdrValue();
                break;
            case UdrSchema::ID:
                const U2DataType objectType = q.getInt32(schema->size());
                data << UdrValue(q.getDataId(colNum, objectType));
                break;
        }
        CHECK_OP(os, );
    }
}

} // U2
