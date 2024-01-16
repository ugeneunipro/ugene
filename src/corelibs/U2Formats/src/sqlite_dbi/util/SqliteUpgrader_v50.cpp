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

#include "../SQLiteAssemblyDbi.h"
#include "../SQLiteObjectRelationsDbi.h"

namespace U2 {

SqliteUpgrader_v50::SqliteUpgrader_v50(SQLiteDbi* dbi)
    : SqliteUpgrader(Version::parseVersion("1.25.0"), Version::parseVersion("1.50.0"), dbi) {
}

void SqliteUpgrader_v50::upgrade(U2OpStatus& os) const {
    SQLiteTransaction t(dbi->getDbRef(), os);

    upgradeMsaGaps(os);
    CHECK_OP(os, );

    dbi->setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, versionTo.toString(), os);
}

void SqliteUpgrader_v50::upgradeMsaGaps(U2OpStatus&) const {
    // TODO
}

}  // namespace U2
