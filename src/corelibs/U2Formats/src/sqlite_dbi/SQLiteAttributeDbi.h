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

#ifndef _U2_SQLITE_ATTRIBUTE_DBI_H_
#define _U2_SQLITE_ATTRIBUTE_DBI_H_

#include "SQLiteDbi.h"

namespace U2 {

class SQLiteAttributeDbi : public U2AttributeDbi, public SQLiteChildDBICommon {

public:
    SQLiteAttributeDbi(SQLiteDbi* dbi);

    /** Returns all attribute names available in the database */
    virtual QStringList getAvailableAttributeNames(U2OpStatus& os);

    /** Returns all attribute ids for the given object */
    virtual QList<U2DataId> getObjectAttributes(const U2DataId& objectId, const QString& name, U2OpStatus& os);

    /** Returns all attribute ids for the given object */
    virtual QList<U2DataId> getObjectPairAttributes(const U2DataId& objectId, const U2DataId& childId, const QString& name, U2OpStatus& os);

    /** Loads int64 attribute by id */
    virtual U2IntegerAttribute getIntegerAttribute(const U2DataId& attributeId, U2OpStatus& os);

    /** Loads real64 attribute by id */
    virtual U2RealAttribute getRealAttribute(const U2DataId& attributeId, U2OpStatus& os);

    /** Loads String attribute by id */
    virtual U2StringAttribute getStringAttribute(const U2DataId& attributeId, U2OpStatus& os);

    /** Loads byte attribute by id */
    virtual U2ByteArrayAttribute getByteArrayAttribute(const U2DataId& attributeId, U2OpStatus& os);


    /** Sorts all objects in database according to U2DbiSortConfig provided  */
    virtual QList<U2DataId> sort(const U2DbiSortConfig& sc, qint64 offset, qint64 count, U2OpStatus& os);


    /**
        Removes attributes from database
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void removeAttributes(const QList<U2DataId>& attributeIds, U2OpStatus& os);

    /**
        Removes all attributes associated with the object
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void removeObjectAttributes(const U2DataId& objectId, U2OpStatus& os);

    /**
        Creates int64 attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void createIntegerAttribute(U2IntegerAttribute& a, U2OpStatus& os);

    /**
        Creates real64 attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void createRealAttribute(U2RealAttribute& a, U2OpStatus& os);

    /**
        Creates String attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void createStringAttribute(U2StringAttribute& a, U2OpStatus& os);

    /**
        Creates Byte attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void createByteArrayAttribute(U2ByteArrayAttribute& a, U2OpStatus& os);


    virtual void initSqlSchema(U2OpStatus& os);

private:
    qint64 createAttribute(U2Attribute& attr, U2DataType type, SQLiteTransaction& t, U2OpStatus& os);

    QString buildSelectAttributeQuery(const QString& attributeTable);

    void readAttribute(SQLiteReadQuery &q, U2Attribute& attr);
};


} //namespace

#endif
