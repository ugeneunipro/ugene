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

#include <U2Core/Version.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2OpStatus.h>

#include "MysqlDbiUtils.h"
#include "MysqlHelpers.h"
#include "MysqlModificationAction.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/MysqlObjectDbi.h"

namespace U2 {

QString MysqlDbiUtils::createAuthDbiUrl(const QString &userName, const QString &password, const QString &host, int port, const QString &dbName) {
    return userName + ":" + password + "@" + host + ":" + QString::number(port) + "/" + dbName;
}

QString MysqlDbiUtils::createAuthDbiUrl(const QString& userName, const QString& password, const QString& dbUrl) {
    return userName + ":" + password + "@" + dbUrl;
}

bool MysqlDbiUtils::parseAuthDbiUrl(const QString &url, QString &userName, QString &password, QString &host, int &port, QString &dbName) {
//    user:password@host:port/db
    int sepIndex = url.indexOf("@");
    if (sepIndex < 0) {
        return false;
    }
    QString loginInfo = url.left(sepIndex);
    QString dbInfo = url.right(url.length() - sepIndex - 1);

    sepIndex = loginInfo.indexOf(":");
    if (sepIndex < 0) {
        return false;
    }
    userName = loginInfo.left(sepIndex);
    password = loginInfo.right(loginInfo.length() - sepIndex - 1);

    sepIndex = dbInfo.indexOf(":");
    if (sepIndex < 0) {
        return false;
    }
    host = dbInfo.left(sepIndex);

    sepIndex = dbInfo.indexOf("/", sepIndex);
    if (sepIndex < 0) {
        return false;
    }
    QString portString = dbInfo.mid(host.length() + 1, sepIndex - host.length() - 1);
    if (portString.isEmpty()) {
        port = -1;
    } else {
        bool ok = false;
        port = portString.toInt(&ok);
        if (!ok) {
            return false;
        }
    }
    dbName = dbInfo.right(dbInfo.length() - sepIndex - 1);

    return true;
}

U2::U2DbiId MysqlDbiUtils::createDbiUrl(const QString &host, int port, const QString &dbName )
{
    return host + ":" + QString::number(port) + "/" + dbName;
}

bool MysqlDbiUtils::parseDbiUrl(const U2DbiId& dbiId, QString& host, int& port, QString& dbName) {
    int sepIndex = dbiId.indexOf(":");
    if (sepIndex < 0) {
        return false;
    }
    host = dbiId.left(sepIndex);

    sepIndex = dbiId.indexOf("/", sepIndex);
    if (sepIndex < 0) {
        return false;
    }
    QString portString = dbiId.mid(host.length() + 1, sepIndex - host.length() - 1);
    if (portString.isEmpty()) {
        port = -1;
    } else {
        bool ok = false;
        port = portString.toInt(&ok);
        if (!ok) {
            return false;
        }
    }
    dbName = dbiId.right(dbiId.length() - sepIndex - 1);

    return true;
}

bool MysqlDbiUtils::isDbInitialized(const U2DbiRef &dbiRef, U2OpStatus& os) {
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, false);
    return con.dbi->isInitialized(os);
}

void MysqlDbiUtils::renameObject(MysqlDbi* dbi, U2Object &object, const QString &newName, U2OpStatus &os) {
    CHECK_OP(os, );
    SAFE_POINT(NULL != dbi, "NULL dbi", );
    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, object.id);
    updateAction.prepare(os);
    CHECK_OP(os, );

    renameObject(updateAction, dbi, object, newName, os);
    CHECK_OP(os, );

    // Increment version; track the modification, if required
    updateAction.complete(os);
}

void MysqlDbiUtils::renameObject(MysqlModificationAction& updateAction, MysqlDbi* dbi, U2Object &object, const QString &newName, U2OpStatus &os) {
    CHECK_OP(os, );
    SAFE_POINT(NULL != dbi, "NULL dbi", );
    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        modDetails = U2DbiPackUtils::packObjectNameDetails(object.visualName, newName);
    }

    object.visualName = newName;
    dbi->getMysqlObjectDbi()->updateObject(object, os);
    CHECK_OP(os, );

    updateAction.addModification(object.id, U2ModType::objUpdatedName, modDetails, os);
}

void MysqlDbiUtils::upgrade(const U2DbiRef &dbiRef, U2OpStatus &os) {
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, );

    MysqlDbi *mysqlDbi = dynamic_cast<MysqlDbi *>(con.dbi);
    SAFE_POINT_EXT(NULL != mysqlDbi, os.setError("The proposed dbi reference doesn't corresponds a mysql database"), );

    mysqlDbi->upgrade(os);
}

}   // namespace U2
