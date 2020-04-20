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

#include <U2Core/DNAInfo.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/SharedDbUrlUtils.h>

#include "DbFolderScanner.h"

namespace U2 {

DbFolderScanner::DbFolderScanner(const QString &url, const QString &accFilter, const QString &objNameFilter, bool recursive)
    : accFilter(accFilter)
{
    const U2DbiRef dbiRef = SharedDbUrlUtils::getDbRefFromEntityUrl(url);

    U2OpStatusImpl os;
    dbConnection = DbiConnection(dbiRef, os);
    CHECK_OP(os, );

    typeFilter = SharedDbUrlUtils::getDbFolderDataTypeByUrl(url);
    const QString folderPath = SharedDbUrlUtils::getDbFolderPathByUrl(url);
    QSet<QString> allRequestedPaths;
    allRequestedPaths.insert(folderPath);
    if (recursive) {
        getSubfolders(folderPath, allRequestedPaths, os);
        CHECK_OP(os, );
    }

    initTargetObjectList(allRequestedPaths, objNameFilter, os);
}

void DbFolderScanner::getSubfolders(const QString &folderPath, QSet<QString> &subfolders, U2OpStatus &os) {
    SAFE_POINT(NULL != dbConnection.dbi, "Invalid DBI", );
    U2ObjectDbi *oDbi = dbConnection.dbi->getObjectDbi();
    SAFE_POINT(NULL != oDbi, "Invalid object DBI", );

    const QStringList allFolders = oDbi->getFolders(os);
    CHECK_OP(os, );
    foreach (const QString &folder, allFolders) {
        if (folder.startsWith(folderPath)) {
            subfolders.insert(folder);
        }
    }
}

void DbFolderScanner::initTargetObjectList(const QSet<QString> &paths, const QString &objNameFilter, U2OpStatus &os) {
    SAFE_POINT(NULL != dbConnection.dbi, "Invalid DBI", );
    U2ObjectDbi *oDbi = dbConnection.dbi->getObjectDbi();
    SAFE_POINT(NULL != oDbi, "Invalid object DBI", );
    const U2DbiRef dbiRef = dbConnection.dbi->getDbiRef();

    const bool nameFilterApplied = !objNameFilter.isEmpty();
    foreach (const QString &path, paths) {
        const QList<U2DataId> objIds = oDbi->getObjects(path, U2DbiOptions::U2_DBI_NO_LIMIT, U2DbiOptions::U2_DBI_NO_LIMIT, os);
        CHECK_OP(os, );
        QHash<U2DataId, QString> objNames;
        if (nameFilterApplied) {
            objNames = oDbi->getObjectNames(U2DbiOptions::U2_DBI_NO_LIMIT, U2DbiOptions::U2_DBI_NO_LIMIT, os);
            CHECK_OP(os, );
        }
        foreach (const U2DataId &objId, objIds) {
            const bool passedNameFilter = nameFilterApplied && (!objNames.contains(objId) || objNames[objId] != objNameFilter);
            const bool passedTypeFilter = typeFilter != U2DbiUtils::toType(objId);
            if (passedNameFilter || passedTypeFilter) {
                continue;
            }
            const QString objUrl = SharedDbUrlUtils::createDbObjectUrl(dbiRef, objId, objNames.value(objId, "Object"));
            if (passFilter(objUrl)) {
                unusedObjects << objUrl;
            }
        }
    }
}

QString DbFolderScanner::getNextFile() {
    SAFE_POINT(hasNext(), "URL list is empty", QString());
    return unusedObjects.takeFirst();
}

bool DbFolderScanner::passFilter(const QString &objUrl) {
    if (!accFilter.isEmpty()) {
        return hasAccession(objUrl);
    }
    return true;
}

bool DbFolderScanner::hasAccession(const QString &objUrl) {
    SAFE_POINT(NULL != dbConnection.dbi, "Invalid DBI", false);
    U2AttributeDbi *aDbi = dbConnection.dbi->getAttributeDbi();
    SAFE_POINT(NULL != aDbi, "Invalid attribute DBI", false);

    const U2DataId objId = SharedDbUrlUtils::getObjectIdByUrl(objUrl);
    SAFE_POINT(!objId.isEmpty(), "Invalid DB object ID", false);

    U2OpStatusImpl os;
    const QList<U2DataId> attrs = aDbi->getObjectAttributes(objId, DNAInfo::ACCESSION, os);
    CHECK_OP(os, false);
    CHECK(!attrs.isEmpty(), false);

    const U2StringAttribute accAttr = aDbi->getStringAttribute(attrs.first(), os);
    return accAttr.value == accFilter;
}

bool DbFolderScanner::hasNext() {
    return !unusedObjects.isEmpty();
}

} // U2
