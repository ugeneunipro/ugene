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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MysqlUpgraderFrom_1_16_To_1_17.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/util/MysqlHelpers.h"

namespace U2 {

MysqlUpgraderFrom_1_16_To_1_17::MysqlUpgraderFrom_1_16_To_1_17(MysqlDbi *dbi) :
    MysqlUpgrader(Version::parseVersion("1.16.0"), Version::parseVersion("1.17.0"), dbi)
{

}

void MysqlUpgraderFrom_1_16_To_1_17::upgrade(U2OpStatus &os) const {
    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    upgradeFeatureDbi(os, dbi->getDbRef());
    CHECK_OP(os, );
}

void MysqlUpgraderFrom_1_16_To_1_17::upgradeFeatureDbi(U2OpStatus &os, MysqlDbRef *dbRef) const {
    U2OpStatus2Log nonCriticalOs;
    U2SqlQuery("DROP INDEX FeatureRootIndex ON Feature", dbRef, nonCriticalOs).execute();

    U2SqlQuery("CREATE INDEX FeatureRootIndex ON Feature(root, class)", dbRef, os).execute();
    CHECK_OP(os, );
}

}   // namespace U2
