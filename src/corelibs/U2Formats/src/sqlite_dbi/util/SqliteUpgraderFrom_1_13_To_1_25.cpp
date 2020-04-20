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

#include <U2Core/L10n.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include "SqliteUpgraderFrom_1_13_To_1_25.h"
#include "../SQLiteDbi.h"
#include "../SQLiteAssemblyDbi.h"
#include "../SQLiteObjectRelationsDbi.h"

namespace U2 {

SqliteUpgraderFrom_1_13_To_1_25::SqliteUpgraderFrom_1_13_To_1_25(SQLiteDbi *dbi) :
    SqliteUpgrader(Version::parseVersion("1.13.0"), Version::parseVersion("1.25.0"), dbi)
{
}

void SqliteUpgraderFrom_1_13_To_1_25::upgrade(U2OpStatus &os) const {
    SQLiteTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    upgradeCoverageAttribute(os);
    CHECK_OP(os, );

    dbi->setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, versionTo.text, os);
}

void SqliteUpgraderFrom_1_13_To_1_25::upgradeCoverageAttribute(U2OpStatus &os) const {
    //get assembly ids
    QList<U2DataId> assemblyIds = dbi->getObjectDbi()->getObjects(U2Type::Assembly, 0, U2DbiOptions::U2_DBI_NO_LIMIT, os);
    CHECK_OP(os, );
    CHECK(!assemblyIds.isEmpty(),);
    U2AttributeDbi * attributeDbi = dbi->getAttributeDbi();
    CHECK_EXT(attributeDbi != NULL, os.setError("Attribute dbi is NULL"),);

    foreach (const U2DataId &id, assemblyIds) {
        //find and remove coverage attribute from ByteArrayAttribute table
        U2ByteArrayAttribute attr = U2AttributeUtils::findByteArrayAttribute(attributeDbi, id, U2BaseAttributeName::coverage_statistics, os);

        if (!attr.value.isEmpty()){//if empty, then nothing to remove
            U2AttributeUtils::removeAttribute(attributeDbi, attr.id, os);
        }

        //calculate new coverage
        U2AssemblyDbi* assemblyDbi = dbi->getAssemblyDbi();
        CHECK_EXT(attributeDbi != NULL, os.setError("Assembly dbi is NULL"),);
        U2Assembly assembly = assemblyDbi->getAssemblyObject(id, os);
        CHECK_OP(os, );

        U2IntegerAttribute lengthAttr = U2AttributeUtils::findIntegerAttribute(attributeDbi, id, U2BaseAttributeName::reference_length, os);
        CHECK_OP(os, );
        if (lengthAttr.value == 0){//Nothing to calculate
            continue;
        }
        static const qint64 MAX_COVERAGE_CACHE_SIZE = 1000*1000;
        int coverageSize = (int)qMin(MAX_COVERAGE_CACHE_SIZE, lengthAttr.value);
        U2AssemblyCoverageStat coverageStat;
        coverageStat.resize(coverageSize);

        assemblyDbi->calculateCoverage(id, U2Region(0, lengthAttr.value), coverageStat, os);
        CHECK_OP(os, );

        //write new coverage attribute to ByteArrayAttribute table
        U2ByteArrayAttribute attribute;
        attribute.objectId = id;
        attribute.name = U2BaseAttributeName::coverage_statistics;
        attribute.value = U2AssemblyUtils::serializeCoverageStat(coverageStat);
        attribute.version = assembly.version;
        attributeDbi->createByteArrayAttribute(attribute, os);
        CHECK_OP(os, );
    }
}

}   // namespace U2
