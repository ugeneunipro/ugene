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

#ifndef _U2_RAWDATAUDRSCHEMA_H_
#define _U2_RAWDATAUDRSCHEMA_H_

#include <U2Core/U2RawData.h>
#include <U2Core/UdrSchema.h>
#include <U2Core/global.h>

namespace U2 {

/**
 * The utility class for working with raw data objects:
 * creating objects, writing and reading objects' data.
 */
class U2CORE_EXPORT RawDataUdrSchema {
public:
    /**
     * The schema identifier.
     */
    static const UdrSchemaId ID;

    /**
     * Registers the schema in UGENE.
     */
    static void init(U2OpStatus &os);

    /**
     * Creates provided @object in the database with @dbiRef.
     * Assigns the id of @object.
     */
    static void createObject(const U2DbiRef &dbiRef, U2RawData &object, U2OpStatus &os);
    static void createObject(const U2DbiRef &dbiRef, const QString &folder, U2RawData &object, U2OpStatus &os);

    /**
     * Returns the retrieved object from a database by provided @objRef.
     */
    static U2RawData getObject(const U2EntityRef &objRef, U2OpStatus &os);

    /**
     * Retrieves an object from the database by provided @srcObjRef
     * and creates its copy on the database with @dstDbiRef.
     * Returns the cloned object.
     */
    static void cloneObject(const U2EntityRef &srcObjRef, const U2DbiRef &dstDbiRef, const QString &dstFolder,
        U2RawData &dstObject, U2OpStatus &os);

    /**
     * Updates the data of the object by provided @objRef.
     */
    static void writeContent(const QByteArray &data, const U2EntityRef &objRef, U2OpStatus &os);
    static void writeContent(const U2DataId& masterId, const QByteArray &data, const U2EntityRef &objRef, U2OpStatus &os);

    /**
     * Returns the data of the object by provided @objRef.
     */
    static QByteArray readAllContent(const U2EntityRef &objRef, U2OpStatus &os);
};

} // U2

#endif // _U2_RAWDATAUDRSCHEMA_H_
