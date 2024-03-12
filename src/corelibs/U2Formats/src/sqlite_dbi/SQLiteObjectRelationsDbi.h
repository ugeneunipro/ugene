/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/U2ObjectRelationsDbi.h>

#include "SQLiteDbi.h"

namespace U2 {

class SQLiteObjectRelationsDbi : public U2ObjectRelationsDbi, public SQLiteChildDBICommon {
public:
    SQLiteObjectRelationsDbi(SQLiteDbi* dbi);

    void initSqlSchema(U2OpStatus& os) override;

    void createObjectRelation(U2ObjectRelation& relation, U2OpStatus& os) override;
    QList<U2ObjectRelation> getObjectRelations(const U2DataId& object, U2OpStatus& os) override;
    QList<U2DataId> getReferenceRelatedObjects(const U2DataId& reference, GObjectRelationRole relationRole, U2OpStatus& os) override;
    void removeObjectRelation(U2ObjectRelation& relation, U2OpStatus& os) override;
    void removeAllObjectRelations(const U2DataId& object, U2OpStatus& os) override;
    void removeReferencesForObject(const U2DataId& object, U2OpStatus& os) override;
};

}  // namespace U2
