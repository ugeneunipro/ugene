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

#ifndef _U2_UDRRECORD_H_
#define _U2_UDRRECORD_H_

#include <U2Core/U2Type.h>
#include <U2Core/UdrSchema.h>

namespace U2 {

/**
 * Unique record identifier.
 */
class U2CORE_EXPORT UdrRecordId {
public:
    UdrRecordId(const UdrSchemaId &schemaId, const U2DataId &recordId);

    /**
     * Schema identifier that is unique within UGENE.
     */
    const UdrSchemaId &getSchemaId() const;
    /**
     * Record identifier that is unique within the schema.
     */
    const U2DataId &getRecordId() const;

private:
    UdrSchemaId schemaId;
    U2DataId recordId;
};

/**
 * Value container.
 * Provides storing the data of a type specified in a constructor.
 * Only one of the getters can be used (with the specified type).
 * Empty constructor creates the container of the NULL value, getters can not be used.
 */
class U2CORE_EXPORT UdrValue {
public:
    UdrValue();
    UdrValue(qint64 intValue);
    UdrValue(double doubleValue);
    UdrValue(const QString &stringValue);
    UdrValue(const U2DataId &dataId);

    /**
     * Returns an integer value if it is the integer container.
     */
    qint64 getInt(U2OpStatus &os) const;
    /**
     * Returns a double value if it is the double container.
     */
    double getDouble(U2OpStatus &os) const;
    /**
     * Returns a string value if it is the string container.
     */
    QString getString(U2OpStatus &os) const;
    /**
     * Returns a object id if it is the U2DataId container.
     */
    U2DataId getDataId(U2OpStatus &os) const;

private:
    bool isNull;
    UdrSchema::DataType dataType;
    qint64 intValue;
    double doubleValue;
    QString stringValue;
    U2DataId dataId;

private:
    void init(UdrSchema::DataType dataType, bool isNull = false);
    bool checkType(UdrSchema::DataType askedDataType, U2OpStatus &os) const;
};

/**
 * Unified Data Record container. It contains the unique identifier and
 * the record's data.
 */
class U2CORE_EXPORT UdrRecord {
public:
    UdrRecord(const UdrRecordId &id, const QList<UdrValue> &data, U2OpStatus &os);

    /**
     * Returns the unique record identifier.
     */
    const UdrRecordId & getId() const;

    /**
     * Returns the integer value of the field with the number @fieldNum.
     */
    qint64 getInt(int fieldNum, U2OpStatus &os) const;

    /**
     * Returns the double value of the field with the number @fieldNum.
     */
    double getDouble(int fieldNum, U2OpStatus &os) const;

    /**
     * Returns the string value of the field with the number @fieldNum.
     */
    QString getString(int fieldNum, U2OpStatus &os) const;

    /**
     * Returns the object id value of the field with the number @fieldNum.
     */
    U2DataId getDataId(int fieldNum, U2OpStatus &os) const;

private:
    UdrRecordId id;
    const UdrSchema *schema;
    QList<UdrValue> data;

private:
    bool checkNum(int fieldNum, U2OpStatus &os) const;
};

} // U2

#endif // _U2_UDRRECORD_H_
