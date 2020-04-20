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

#include <QCryptographicHash>

#include <U2Core/Folder.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include <U2Gui/ProjectUtils.h>

#include "MysqlUpgraderFrom_1_15_To_1_16.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/util/MysqlHelpers.h"

namespace U2 {

MysqlUpgraderFrom_1_15_To_1_16::MysqlUpgraderFrom_1_15_To_1_16(MysqlDbi *dbi) :
    MysqlUpgrader(Version::parseVersion("1.15.0"), Version::parseVersion("1.16.0"), dbi)
{
}

void MysqlUpgraderFrom_1_15_To_1_16::upgrade(U2OpStatus &os) const {
    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    upgradeFeatureDbi(os, dbi->getDbRef());
    CHECK_OP(os, );

    dbi->setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, versionTo.text, os);
}

void MysqlUpgraderFrom_1_15_To_1_16::upgradeFeatureDbi(U2OpStatus &os, MysqlDbRef *dbRef) const {
    const bool featureClassFieldExist = (1 == U2SqlQuery(QString("SELECT count(*) FROM information_schema.COLUMNS WHERE "
                                                         "TABLE_SCHEMA = '%1' AND TABLE_NAME = 'Feature' "
                                                         "AND COLUMN_NAME = 'class'").
                                                         arg(dbRef->handle.databaseName()), dbRef, os).selectInt64());
    CHECK_OP(os, );
    CHECK(!featureClassFieldExist, );

    U2SqlQuery("ALTER TABLE Feature CHANGE type class INTEGER NOT NULL", dbRef, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("ALTER TABLE Feature ADD type INTEGER NOT NULL DEFAULT 0", dbRef, os).execute();
    if (os.isCoR()) {
        U2OpStatusImpl innerOs;
        U2SqlQuery("ALTER TABLE Feature CHANGE class type INTEGER NOT NULL", dbRef, innerOs).execute();
        return;
    }

    U2FeatureDbi *featureDbi = dbi->getFeatureDbi();
    SAFE_POINT_EXT(NULL != featureDbi, os.setError(L10N::nullPointerError("feature dbi")), );

    U2DbiIterator<U2Feature> *dbIterator = featureDbi->getFeatures(FeatureQuery(), os);
    CHECK_OP(os, );

    while (dbIterator->hasNext()) {
        U2Feature feature = dbIterator->next();

        U2FeatureType guessedFeatureType = U2FeatureTypes::MiscFeature;
        GBFeatureKey gbFeatureKey = GBFeatureUtils::getKey(feature.name);
        if (GBFeatureKey_UNKNOWN != gbFeatureKey) {
            guessedFeatureType = GBFeatureUtils::getKeyInfo(GBFeatureUtils::getKey(feature.name)).type;
        }

        U2FeatureUtils::updateFeatureType(feature.id, guessedFeatureType, dbi->getDbiRef(), os);
        CHECK_OP(os, );
    }

    U2SqlQuery("ALTER TABLE Feature ALTER type DROP DEFAULT", dbRef, os).execute();
    CHECK_OP(os, );
}

}   // namespace U2
