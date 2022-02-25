/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "MysqlUpgraderFrom_1_25_unknown_To_1_25.h"

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/util/MysqlHelpers.h"

namespace U2 {

MysqlUpgraderFrom_1_25_unknown_To_1_25::MysqlUpgraderFrom_1_25_unknown_To_1_25(MysqlDbi* dbi)
    : MysqlUpgrader(Version::parseVersion("unknown"), Version::parseVersion("1.25.0"), dbi) {
}

void MysqlUpgraderFrom_1_25_unknown_To_1_25::upgrade(U2OpStatus& os) const {
    MysqlTransaction t(dbi->getDbRef(), os);
    // Update version in the database to the correct one.
    // Only UGENE v41 has 'unknown' in the DB which must be replaced with "1.25.0".
    QString currentVersion = dbi->getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "", os);
    if (currentVersion == "unknown") {
        dbi->setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, versionTo.toString(), os);
    }
}

}  // namespace U2
