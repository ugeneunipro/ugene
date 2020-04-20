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

#include <QSet>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchemaRegistry.h>

#include "UdrRecord.h"

namespace U2 {

/************************************************************************/
/* UdrRecordId */
/************************************************************************/
UdrRecordId::UdrRecordId(const UdrSchemaId &schemaId, const U2DataId &recordId)
: schemaId(schemaId), recordId(recordId)
{

}

const UdrSchemaId &UdrRecordId::getSchemaId() const {
    return schemaId;
}

const U2DataId &UdrRecordId::getRecordId() const {
    return recordId;
}

/************************************************************************/
/* UdrValue */
/************************************************************************/
void UdrValue::init(UdrSchema::DataType dataType, bool isNull) {
    this->dataType = dataType;
    intValue = 0;
    doubleValue = 0.0;
    this->isNull = isNull;
}

UdrValue::UdrValue() {
    init(UdrSchema::INTEGER, true);
}

UdrValue::UdrValue(qint64 intValue) {
    init(UdrSchema::INTEGER);
    this->intValue = intValue;
}

UdrValue::UdrValue(double doubleValue) {
    init(UdrSchema::DOUBLE);
    this->doubleValue = doubleValue;
}

UdrValue::UdrValue(const QString &stringValue) {
    init(UdrSchema::STRING);
    this->stringValue = stringValue;
}

UdrValue::UdrValue(const U2DataId &dataId) {
    init(UdrSchema::ID);
    this->dataId = dataId;
}

bool UdrValue::checkType(UdrSchema::DataType askedDataType, U2OpStatus &os) const {
    if (isNull) {
        os.setError("NULL value");
        return false;
    }
    if (dataType != askedDataType) {
        os.setError("Type mismatch");
        return false;
    }
    return true;
}

qint64 UdrValue::getInt(U2OpStatus &os) const {
    CHECK(checkType(UdrSchema::INTEGER, os), 0);
    return intValue;
}

double UdrValue::getDouble(U2OpStatus &os) const {
    CHECK(checkType(UdrSchema::DOUBLE, os), 0.0);
    return doubleValue;
}

QString UdrValue::getString(U2OpStatus &os) const {
    CHECK(checkType(UdrSchema::STRING, os), "");
    return stringValue;
}

U2DataId UdrValue::getDataId(U2OpStatus &os) const {
    CHECK(checkType(UdrSchema::ID, os), "");
    return dataId;
}

/************************************************************************/
/* UdrRecord */
/************************************************************************/
UdrRecord::UdrRecord(const UdrRecordId &id, const QList<UdrValue> &data, U2OpStatus &os)
: id(id), data(data)
{
    UdrSchemaRegistry *udrRegistry = AppContext::getUdrSchemaRegistry();
    SAFE_POINT_EXT(NULL != udrRegistry, os.setError("NULL UDR registry"), );
    schema = udrRegistry->getSchemaById(id.getSchemaId());
    SAFE_POINT_EXT(NULL != schema, os.setError("Unknown schema id: " + QString(id.getSchemaId())), );
}

const UdrRecordId & UdrRecord::getId() const {
    return id;
}

qint64 UdrRecord::getInt(int fieldNum, U2OpStatus &os) const {
    CHECK(checkNum(fieldNum, os), 0);
    return data[fieldNum].getInt(os);
}

double UdrRecord::getDouble(int fieldNum, U2OpStatus &os) const {
    CHECK(checkNum(fieldNum, os), 0.0);
    return data[fieldNum].getDouble(os);
}

QString UdrRecord::getString(int fieldNum, U2OpStatus &os) const {
    CHECK(checkNum(fieldNum, os), "");
    return data[fieldNum].getString(os);
}

U2DataId UdrRecord::getDataId(int fieldNum, U2OpStatus &os) const {
    CHECK(checkNum(fieldNum, os), "");
    return data[fieldNum].getDataId(os);
}

bool UdrRecord::checkNum(int fieldNum, U2OpStatus &os) const {
    SAFE_POINT_EXT(NULL != schema, os.setError("NULL schema"), false);
    SAFE_POINT_EXT(data.size() == schema->size(), os.setError("Size mismatch"), false);
    SAFE_POINT_EXT(0 <= fieldNum && fieldNum < schema->size(), os.setError("Out of range"), false);
    return true;
}

} // U2
