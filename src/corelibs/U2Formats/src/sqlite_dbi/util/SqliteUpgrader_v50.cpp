/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "SqliteUpgrader_v50.h"

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include "../SQLiteMsaDbi.h"

namespace U2 {

SqliteUpgrader_v50::SqliteUpgrader_v50(SQLiteDbi* dbi)
    : SqliteUpgrader(Version::parseVersion("1.50.0"), dbi) {
}

void SqliteUpgrader_v50::upgrade(U2OpStatus& os) const {
    SQLiteTransaction t(dbi->getDbRef(), os);

    upgradeMsaGaps(os);
    CHECK_OP(os, );

    SqliteUpgrader::upgrade(os);
}

void SqliteUpgrader_v50::upgradeMsaGaps(U2OpStatus& os) const {
    DbRef* db = dbi->getDbRef();
    SQLiteWriteQuery alterQuery("ALTER TABLE MsaRow ADD COLUMN gaps TEXT NOT NULL DEFAULT ''", db, os);
    alterQuery.execute();
    SAFE_POINT_OP(os, );

    SQLiteReadQuery readMsaOrMcaQuery("SELECT id, type FROM object WHERE type = ?1 OR type = ?2", db, os);
    readMsaOrMcaQuery.bindType(1, U2Type::Msa);
    readMsaOrMcaQuery.bindType(2, U2Type::Mca);
    QList<U2DataId> maObjectIds;
    while (readMsaOrMcaQuery.step()) {
        SAFE_POINT_OP(os, );
        U2DataType type = readMsaOrMcaQuery.getType(1);
        U2DataId objectId = readMsaOrMcaQuery.getDataId(0, type);
        maObjectIds << objectId;
    }
    for (const U2DataId& maObjectId : qAsConst(maObjectIds)) {
        SQLiteReadQuery readRowsQuery("SELECT rowId FROM MsaRow WHERE msa = ?1", db, os);
        readRowsQuery.bindDataId(1, maObjectId);
        QList<qint64> rowIds;
        while (readRowsQuery.step()) {
            SAFE_POINT_OP(os, );
            rowIds << readRowsQuery.getInt64(0);
        }
        for (qint64 rowId : qAsConst(rowIds)) {
            SQLiteReadQuery readGapsQuery("SELECT gapStart, gapEnd FROM MsaRowGap WHERE msa = ?1 AND rowId = ?2 ORDER BY gapStart", db, os);
            readGapsQuery.bindDataId(1, maObjectId);
            readGapsQuery.bindInt64(2, rowId);
            QVector<U2MsaGap> gaps;
            while (readGapsQuery.step()) {
                SAFE_POINT_OP(os, );
                qint64 gapStart = readGapsQuery.getInt64(0);
                qint64 gapEnd = readGapsQuery.getInt64(1);
                qint64 gapLength = gapEnd - gapStart;
                gaps << U2MsaGap(gapStart, gapLength);
            }
            if (!gaps.isEmpty()) {
                SQLiteWriteQuery updateGapsQuery("UPDATE MsaRow SET gaps = ?1 WHERE msa = ?2 AND rowId = ?3", db, os);
                updateGapsQuery.bindBlob(1, SQLiteMsaDbi::packGaps(gaps));
                updateGapsQuery.bindDataId(2, maObjectId);
                updateGapsQuery.bindInt64(3, rowId);
                updateGapsQuery.execute();
            }
        }
    }
    SQLiteWriteQuery dropGapsTableQuery("DROP TABLE MsaRowGap", db, os);
    dropGapsTableQuery.execute();
}

}  // namespace U2
