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

#include <U2Core/U2ObjectTypeUtils.h>
#include <U2Core/U2SafePoints.h>

#include "util/MysqlHelpers.h"
#include "MysqlObjectRelationsDbi.h"

namespace U2 {

MysqlObjectRelationsDbi::MysqlObjectRelationsDbi( MysqlDbi *dbi )
    : U2ObjectRelationsDbi( dbi ), MysqlChildDbiCommon( dbi )
{

}

void MysqlObjectRelationsDbi::initSqlSchema( U2OpStatus &os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery( "CREATE TABLE ObjectRelation (object BIGINT NOT NULL, "
        "reference BIGINT NOT NULL, role INTEGER NOT NULL, "
        "PRIMARY KEY(object, reference), "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE,"
        "FOREIGN KEY(reference) REFERENCES Object(id) ON DELETE CASCADE) "
        "ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os ).execute( );
    CHECK_OP( os, );

    U2SqlQuery( "CREATE INDEX ObjectRelationRole ON ObjectRelation(role)", db, os ).execute( );
}

void MysqlObjectRelationsDbi::createObjectRelation( U2ObjectRelation &relation, U2OpStatus &os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString( "INSERT INTO ObjectRelation (object, reference, role) VALUES(:object, :reference, :role)" );
    U2SqlQuery q( queryString, db, os );
    CHECK_OP(os, );
    q.bindDataId( ":object", relation.id );
    q.bindDataId( ":reference", relation.referencedObject );
    q.bindInt32( ":role", relation.relationRole );
    q.insert( );
}

QList<U2ObjectRelation> MysqlObjectRelationsDbi::getObjectRelations( const U2DataId &object,
    U2OpStatus &os )
{
    QList<U2ObjectRelation> result;

    static const QString queryString = "SELECT o.type, o.name, o_r.object, o_r.reference, o_r.role FROM ObjectRelation AS o_r "
        "INNER JOIN Object AS o ON o.id = o_r.reference WHERE o_r.object = :object";
    U2SqlQuery q( queryString, db, os );
    CHECK_OP(os, result);
    q.bindDataId( ":object", object );
    while ( q.step( ) ) {
        U2ObjectRelation relation;
        const U2DataType objectType = U2DbiUtils::toType( object );
        const U2DataType referenceType = q.getInt32( 0 );
        relation.referencedType = U2ObjectTypeUtils::toGObjectType( referenceType );
        relation.referencedName = q.getString( 1 );
        relation.id = q.getDataId( 2, objectType );
        relation.referencedObject = q.getDataId( 3, referenceType );
        relation.relationRole = static_cast<GObjectRelationRole>( q.getInt32( 4 ) );
        result << relation;
        CHECK_OP(os, result);
    }

    return result;
}

QList<U2DataId> MysqlObjectRelationsDbi::getReferenceRelatedObjects( const U2DataId &reference,
    GObjectRelationRole relationRole, U2OpStatus &os )
{
    QList<U2DataId> result;

    static const QString queryString = "SELECT o.id, o.type FROM Object AS o INNER JOIN ObjectRelation AS o_r "
        "ON o.id = o_r.object WHERE o_r.reference = :reference AND o_r.role = :role";
    U2SqlQuery q( queryString, db, os );
    CHECK_OP(os, result);
    q.bindDataId( ":reference", reference );
    q.bindInt32( ":role", relationRole );
    while ( q.step( ) ) {
        const U2DataType objType = q.getInt32( 1 );
        result.append( q.getDataId( 0, objType ) );
        CHECK_OP(os, result);
    }

    return result;
}

void MysqlObjectRelationsDbi::removeObjectRelation( U2ObjectRelation &relation, U2OpStatus &os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString( "DELETE FROM ObjectRelation "
        "WHERE object = :object AND reference = :reference" );
    U2SqlQuery q( queryString, db, os );
    CHECK_OP(os, );
    q.bindDataId( ":object", relation.id );
    q.bindDataId( ":reference", relation.referencedObject );
    q.execute( );
}

void MysqlObjectRelationsDbi::removeAllObjectRelations( const U2DataId &object, U2OpStatus &os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString( "DELETE FROM ObjectRelation WHERE object = :object OR reference = :reference" );
    U2SqlQuery q( queryString, db, os );
    CHECK_OP(os, );
    q.bindDataId( ":object", object );
    q.bindDataId( ":reference", object );
    q.execute( );
}

void MysqlObjectRelationsDbi::removeReferencesForObject( const U2DataId &object, U2OpStatus &os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString( "DELETE FROM ObjectRelation WHERE object = :object" );
    U2SqlQuery q( queryString, db, os );
    CHECK_OP(os, );
    q.bindDataId( ":object", object );
    q.execute( );
}

} // namespace U2
