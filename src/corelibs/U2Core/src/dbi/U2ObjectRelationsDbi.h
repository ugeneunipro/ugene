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

#ifndef _U2_OBJECT_RELATIONS_DBI_H_
#define _U2_OBJECT_RELATIONS_DBI_H_

#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2IdTypes.h>

namespace U2 {

class U2CORE_EXPORT U2ObjectRelation : public U2Entity {
public :
                            U2ObjectRelation( );

    bool                    operator ==( const U2ObjectRelation &other ) const;

    U2DataId                referencedObject;
    QString                 referencedName;
    GObjectType             referencedType;
    GObjectRelationRole     relationRole;
};

class U2CORE_EXPORT U2ObjectRelationsDbi : public U2ChildDbi {
public :
    virtual void                        createObjectRelation( U2ObjectRelation &relation, U2OpStatus &os ) = 0;
    virtual QList<U2ObjectRelation>     getObjectRelations( const U2DataId &object, U2OpStatus &os ) = 0;
    virtual QList<U2DataId>             getReferenceRelatedObjects( const U2DataId &reference, GObjectRelationRole relationRole, U2OpStatus &os ) = 0;
    virtual void                        removeObjectRelation( U2ObjectRelation &relation, U2OpStatus &os ) = 0;
    virtual void                        removeAllObjectRelations( const U2DataId &object, U2OpStatus &os ) = 0;
    virtual void                        removeReferencesForObject( const U2DataId &object, U2OpStatus &os ) = 0;

protected :
                                        U2ObjectRelationsDbi( U2Dbi *rootDbi );
};

} // namespace U2

#endif // _U2_OBJECT_RELATIONS_DBI_H_
