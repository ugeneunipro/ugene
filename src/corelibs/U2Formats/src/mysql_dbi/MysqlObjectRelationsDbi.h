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

#ifndef _U2_MYSQL_OBJECT_RELATIONS_DBI_H_
#define _U2_MYSQL_OBJECT_RELATIONS_DBI_H_

#include <U2Core/U2ObjectRelationsDbi.h>

#include "MysqlDbi.h"

namespace U2 {

class MysqlObjectRelationsDbi : public U2ObjectRelationsDbi, public MysqlChildDbiCommon {
public :
                                MysqlObjectRelationsDbi( MysqlDbi *dbi );

    void                        initSqlSchema( U2OpStatus &os );

    void                        createObjectRelation( U2ObjectRelation &relation, U2OpStatus &os );
    QList<U2ObjectRelation>     getObjectRelations( const U2DataId &object, U2OpStatus &os );
    QList<U2DataId>             getReferenceRelatedObjects( const U2DataId &reference, GObjectRelationRole relationRole, U2OpStatus &os );
    void                        removeObjectRelation( U2ObjectRelation &relation, U2OpStatus &os );
    void                        removeAllObjectRelations( const U2DataId &object, U2OpStatus &os );
    void                        removeReferencesForObject( const U2DataId &object, U2OpStatus &os );
};

} // namespace U2

#endif // _U2_MYSQL_OBJECT_RELATIONS_DBI_H_
