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

#ifndef _U2_SQLITEUDRDBI_H_
#define _U2_SQLITEUDRDBI_H_

#include "SQLiteDbi.h"

namespace U2 {

class SQLiteUdrDbi : public UdrDbi, public SQLiteChildDBICommon {
public:
    SQLiteUdrDbi(SQLiteDbi *dbi);

    void undo(const U2SingleModStep& modStep, U2OpStatus& os);
    void redo(const U2SingleModStep& modStep, U2OpStatus& os);

    UdrRecordId addRecord(const UdrSchemaId &schemaId, const QList<UdrValue> &data, U2OpStatus &os);
    void updateRecord(const UdrRecordId &recordId, const QList<UdrValue> &data, U2OpStatus &os);
    UdrRecord getRecord(const UdrRecordId &recordId, U2OpStatus &os);
    void createObject(const UdrSchemaId &schemaId, U2Object &udrObject, const QString &folder, U2OpStatus &os);
    QList<UdrRecord> getObjectRecords(const UdrSchemaId &schemaId, const U2DataId &objectId, U2OpStatus &os);
    QList<UdrRecord> getRecords(const UdrSchemaId &schemaId, U2OpStatus &os);
    void removeRecord(const UdrRecordId &recordId, U2OpStatus &os);
    InputStream * createInputStream(const UdrRecordId &recordId, int fieldNum, U2OpStatus &os);
    OutputStream * createOutputStream(const UdrRecordId &recordId, int fieldNum, qint64 size, U2OpStatus &os);
    void initSqlSchema(U2OpStatus &os);

    ModificationAction* getModificationAction(const U2DataId& id);

private:
    void initSchema(const UdrSchema *schema, U2OpStatus &os);
    void createTable(const UdrSchema *schema, U2OpStatus &os);
    void createIndex(const UdrSchemaId &schemaId, const QStringList &fields, U2OpStatus &os);
    QList<U2DataId> getObjectRecordIds(const UdrSchema *schema, const U2DataId &objectId, U2OpStatus &os);

    /* Utilities */
    static const UdrSchema * udrSchema(const UdrSchemaId &schemaId, U2OpStatus &os);
    static QString insertDef(const UdrSchema *schema, U2OpStatus &os);
    static QString updateDef(const UdrSchema *schema, U2OpStatus &os);
    static QString selectAllDef(const UdrSchema *schema, U2OpStatus &os);
    static QString selectDef(const UdrSchema *schema, U2OpStatus &os);
    static QString tableStartDef(const UdrSchemaId &schemaId);
    static QString fieldDef(const UdrSchema::FieldDesc &field);
    static QString foreignKeysDef(const UdrSchema *schema, U2OpStatus &os);
    static QStringList fieldNames(const UdrSchema *schema, U2OpStatus &os, const QList<int> &nums = QList<int>());
    static QList< QStringList > indexes(const UdrSchema *schema, U2OpStatus &os);
    static void bindData(const QList<UdrValue> &data, const UdrSchema *schema, SQLiteQuery &q, U2OpStatus &os);
    static void retreiveData(QList<UdrValue> &data, const UdrSchema *schema, SQLiteQuery &q, U2OpStatus &os);
    static UdrSchema::FieldDesc getBlobField(const UdrSchemaId &schemaId, int fieldNum, U2OpStatus &os);
};

} // U2

#endif // _U2_SQLITEUDRDBI_H_
