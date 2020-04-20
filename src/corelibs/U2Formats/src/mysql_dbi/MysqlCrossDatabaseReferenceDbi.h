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

#ifndef _U2_MYSQL_CROSS_DATABASE_REFERENCE_DBI_H_
#define _U2_MYSQL_CROSS_DATABASE_REFERENCE_DBI_H_

#include "MysqlDbi.h"

namespace U2 {

class U2FORMATS_EXPORT MysqlCrossDatabaseReferenceDbi : public U2CrossDatabaseReferenceDbi, public MysqlChildDbiCommon {
public:
    MysqlCrossDatabaseReferenceDbi(MysqlDbi* dbi);

    /**
        Adds new remote object to database.
        Sets local object id assigned to the new value
        Requires: U2DbiFeature_WriteCrossDatabaseReferences
    */
    virtual void createCrossReference(U2CrossDatabaseReference& reference, const QString &fodler, U2OpStatus& os);

    /**
        Removes a DB representation of CrossDatabaseReference
    */
    virtual void removeCrossReferenceData(const U2DataId& referenceId, U2OpStatus& os);

    /**
        Loads remote object information from DB
        Requires: U2DbiFeature_ReadCrossDatabaseReferences
    */
    virtual U2CrossDatabaseReference getCrossReference(const U2DataId& objectId, U2OpStatus& os);

    /**
        Updates all fields of cross database reference object
        Requires: U2DbiFeature_WriteCrossDatabaseReferences
    */
    virtual void updateCrossReference(const U2CrossDatabaseReference& reference, U2OpStatus& os);

    virtual void initSqlSchema(U2OpStatus& os);
};

}   // namespace U2

#endif // _U2_MYSQL_CROSS_DATABASE_REFERENCE_DBI_H_
