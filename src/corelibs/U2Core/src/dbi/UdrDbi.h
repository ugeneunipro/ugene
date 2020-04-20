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

#ifndef _U2_UDRDBI_H_
#define _U2_UDRDBI_H_

#include <U2Core/InputStream.h>
#include <U2Core/OutputStream.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/UdrRecord.h>

namespace U2 {

class U2CORE_EXPORT UdrDbi : public U2ChildDbi {
public:
    UdrDbi(U2Dbi *rootDbi);

    /**
     * Creates a new record and returns its identifier.
     */
    virtual UdrRecordId addRecord(const UdrSchemaId &schemaId, const QList<UdrValue> &data, U2OpStatus &os) = 0;

    /**
     * Updates the record's data.
     */
    virtual void updateRecord(const UdrRecordId &recordId, const QList<UdrValue> &data, U2OpStatus &os) = 0;

    /**
     * Returns the record with the given identifier.
     */
    virtual UdrRecord getRecord(const UdrRecordId &recordId, U2OpStatus &os) = 0;

    /**
     * Adds a new (empty) UDR object instance into database.
     * Sets the assigned id to the passed U2Object instance.
     * The folder must exist in the database.
     * Requires: U2DbiFeature_WriteUdr feature support.
     *           The schema with the @schemaId must have the object reference.
     */
    virtual void createObject(const UdrSchemaId &schemaId, U2Object &udrObject, const QString &folder, U2OpStatus &os) = 0;

    /**
     * Returns the list of records associated with the given @objectId.
     * The schema with the @schemaId must have the object reference.
     */
    virtual QList<UdrRecord> getObjectRecords(const UdrSchemaId &schemaId, const U2DataId &objectId, U2OpStatus &os) = 0;

    /**
     * Returns the list of records of the given schema.
     */
    virtual QList<UdrRecord> getRecords(const UdrSchemaId &schemaId, U2OpStatus &os) = 0;

    /**
     * Removes the record with the given identifier.
     */
    virtual void removeRecord(const UdrRecordId &recordId, U2OpStatus &os) = 0;

    /**
     * Returns the input stream for reading data from the specified field (@fieldNum)
     * of the record with @recordId.
     * The field must be BLOB.
     * Do not forget to release the memory.
     */
    virtual InputStream * createInputStream(const UdrRecordId &recordId, int fieldNum, U2OpStatus &os) = 0;

    /**
     * Returns the output stream for writing data to the specified field (@fieldNum)
     * of the record with @recordId.
     * The created OutputStream will be reserved to write exactly @size bytes.
     * The field must be BLOB.
     * Do not forget to release the memory.
     */
    virtual OutputStream * createOutputStream(const UdrRecordId &recordId, int fieldNum, qint64 size, U2OpStatus &os) = 0;

    virtual ModificationAction* getModificationAction(const U2DataId& id) = 0;
};

} // U2

#endif // _U2_UDRDBI_H_
