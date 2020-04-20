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

#ifndef _U2_ATTR_DBI_H_
#define _U2_ATTR_DBI_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Attribute.h>

namespace U2 {

/**
    A configuration for sorting by attribute operation
*/
class U2DbiSortConfig {
public:
    U2DbiSortConfig() : ascending(true) {}

    /** Sort column*/
    QString     sortColumnName;

    /** Type of the sort column: Integer, Real or String attribute type*/
    U2DataType  columnType;

    /** Tells  if sorting is ascending or descending */
    bool        ascending;

    /** Folder to localize sorting. If empty all folders are processed  */
    QString     folder;
};

/**
    An interface to obtain access to object attributes
*/
class U2AttributeDbi: public U2ChildDbi {
protected:
    U2AttributeDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi){}

public:
    /** Returns all attribute names available in the database */
    virtual QStringList getAvailableAttributeNames(U2OpStatus& os) = 0;

    /**
        Returns ids of all attributes with a name 'attributeName' for the object referenced by 'objectId'.
        If 'attributeName' is empty returns all object attributes.
    */
    virtual QList<U2DataId> getObjectAttributes(const U2DataId& objectId, const QString& attributeName, U2OpStatus& os) = 0;

    /**
        Returns ids of all attributes with a childId 'childId' and a name 'attributeName'
        for the object referenced by 'objectId'.
        If 'attributeName' is empty returns ids of all object attributes with a specified childId.
    */
    virtual QList<U2DataId> getObjectPairAttributes(const U2DataId& objectId, const U2DataId& childId, const QString& attributeName, U2OpStatus& os) = 0;

    /**
        Loads integer attribute by id.
        If there is no integer attribute with the specified id returns
    */
    virtual U2IntegerAttribute getIntegerAttribute(const U2DataId& attributeId, U2OpStatus& os) = 0;

    /** Loads real attribute by id */
    virtual U2RealAttribute getRealAttribute(const U2DataId& attributeId, U2OpStatus& os) = 0;

    /** Loads String attribute by id */
    virtual U2StringAttribute getStringAttribute(const U2DataId& attributeId, U2OpStatus& os) = 0;

    /** Loads byte attribute by id */
    virtual U2ByteArrayAttribute getByteArrayAttribute(const U2DataId& attributeId, U2OpStatus& os) = 0;

    /** Sorts all objects in database according to U2DbiSortConfig provided
        Requires U2DbiFeature_AttributeSorting support
    */
    virtual QList<U2DataId> sort(const U2DbiSortConfig& sc, qint64 offset, qint64 count, U2OpStatus& os) = 0;


    /**
        Removes attributes from database
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void removeAttributes(const QList<U2DataId>& attributeIds, U2OpStatus& os) = 0;

    /**
        Removes all attributes associated with the object
        Requires U2DbiFeature_WriteAttribute feature support
    */
    virtual void removeObjectAttributes(const U2DataId& objectId, U2OpStatus& os) = 0;

    /**
        Creates int64 attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support

        NOTE: When you create a new attribute, do not forget to add it to the clone method of the corresponding object!
    */
    virtual void createIntegerAttribute(U2IntegerAttribute& a, U2OpStatus& os) = 0;

    /**
        Creates real64 attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support

        NOTE: When you create a new attribute, do not forget to add it to the clone method of the corresponding object!
    */
    virtual void createRealAttribute(U2RealAttribute& a, U2OpStatus& os) = 0;

    /**
        Creates String attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support

        NOTE: When you create a new attribute, do not forget to add it to the clone method of the corresponding object!
    */
    virtual void createStringAttribute(U2StringAttribute& a, U2OpStatus& os) = 0;

    /**
        Creates Byte attribute in database. ObjectId must be already set in attribute and present in the same database
        Requires U2DbiFeature_WriteAttribute feature support

        NOTE: When you create a new attribute, do not forget to add it to the clone method of the corresponding object!
    */
    virtual void createByteArrayAttribute(U2ByteArrayAttribute& a, U2OpStatus& os) = 0;

};


} //namespace

#endif
