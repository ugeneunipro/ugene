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
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include <U2Gui/ProjectUtils.h>

#include "MysqlUpgraderFrom_1_14_To_1_15.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/util/MysqlHelpers.h"

namespace U2 {

MysqlUpgraderFrom_1_14_To_1_15::MysqlUpgraderFrom_1_14_To_1_15(MysqlDbi *dbi) :
    MysqlUpgrader(Version::parseVersion("1.14.0"), Version::parseVersion("1.15.0"), dbi)
{
}

void MysqlUpgraderFrom_1_14_To_1_15::upgrade(U2OpStatus &os) const {
    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    upgradeObjectDbi(os, dbi->getDbRef());
    CHECK_OP(os, );

    dbi->setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, versionTo.text, os);
}

void MysqlUpgraderFrom_1_14_To_1_15::upgradeObjectDbi(U2OpStatus &os, MysqlDbRef *dbRef) const {
    const bool previousPathFieldExist = (1 == U2SqlQuery(QString("SELECT count(*) FROM information_schema.COLUMNS WHERE "
                                                         "TABLE_SCHEMA = '%1' AND TABLE_NAME = 'Folder' "
                                                         "AND COLUMN_NAME = 'previousPath'").
                                                         arg(dbRef->handle.databaseName()), dbRef, os).selectInt64());
    CHECK_OP(os, );

    if (!previousPathFieldExist) {
        U2SqlQuery("ALTER TABLE Folder ADD previousPath LONGTEXT", dbRef, os).execute();
        CHECK_OP(os, );
    }

    MysqlTransaction t(dbRef, os);
    Q_UNUSED(t);

    const QStringList folders = dbi->getObjectDbi()->getFolders(os);
    const QString recycleBinPrefix = U2ObjectDbi::ROOT_FOLDER + U2ObjectDbi::RECYCLE_BIN_FOLDER + U2ObjectDbi::PATH_SEP;
    StrStrMap oldAndNewPathes;

    foreach (const QString &folder, folders) {
        if (folder.startsWith(recycleBinPrefix)) {
            const QString parentFolder = getParentFolderFromList(oldAndNewPathes.keys(), folder);
            QString newPath;
            if (!parentFolder.isEmpty()) {
                newPath = oldAndNewPathes[parentFolder] + U2ObjectDbi::PATH_SEP + folder.mid(parentFolder.size() + 1);
            } else {
                newPath = recycleBinPrefix + Folder::getFolderName(folder);
            }

            rollNewFolderPath(newPath, oldAndNewPathes.values());
            oldAndNewPathes.insert(folder, newPath);
        }
    }

    foreach (const QString &folder, oldAndNewPathes.keys()) {
        const QByteArray oldHash = QCryptographicHash::hash(folder.toLatin1(), QCryptographicHash::Md5).toHex();
        const QByteArray newHash = QCryptographicHash::hash(oldAndNewPathes[folder].toLatin1(), QCryptographicHash::Md5).toHex();
        const QString originPath = U2ObjectDbi::ROOT_FOLDER + folder.mid(recycleBinPrefix.size());

        U2SqlQuery query("UPDATE Folder SET path = :newPath, hash = :newHash, previousPath = :originPath WHERE hash = :oldHash", dbRef, os);
        query.bindString(":newPath", oldAndNewPathes[folder]);
        query.bindBlob(":newHash", newHash);
        query.bindString(":originPath", originPath);
        query.bindBlob(":oldHash", oldHash);
        query.update();
        CHECK_OP(os, );
    }
}

QString MysqlUpgraderFrom_1_14_To_1_15::getParentFolderFromList(const QStringList &folders, const QString &folder) {
    QString parentFolder;
    foreach (const QString &possibleParentFolder, folders) {
        if (folder.startsWith(possibleParentFolder) && possibleParentFolder.size() > parentFolder.size()) {
            parentFolder = possibleParentFolder;
        }
    }

    return parentFolder;
}

void MysqlUpgraderFrom_1_14_To_1_15::rollNewFolderPath(QString &originalPath, const QStringList &allFolders) {
    QString resultPath = originalPath;
    int sameFolderNameCount = 0;
    while (allFolders.contains(resultPath)) {
        resultPath = QString("%1 (%2)").arg(originalPath).arg(++sameFolderNameCount);
    }
    originalPath = resultPath;
}

}   // namespace U2
