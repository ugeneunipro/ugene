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

#ifndef _U2_MYSQL_DBI_UTILS_H_
#define _U2_MYSQL_DBI_UTILS_H_

#include <U2Core/global.h>
#include <U2Core/U2Type.h>

#include <QString>

namespace U2 {

class MysqlDbi;
class MysqlModificationAction;
class U2OpStatus;

class U2FORMATS_EXPORT MysqlDbiUtils {
public:
    /** Creates an URL that contains authentification information */
    static QString createAuthDbiUrl(const QString& userName, const QString& password, const QString& host,
        int port, const QString& dbName);

    static QString createAuthDbiUrl(const QString& userName, const QString& password, const QString& dbUrl);

    static bool parseAuthDbiUrl(const QString& url, QString& userName, QString& password, QString& host,
        int& port, QString& dbName);

    static U2DbiId createDbiUrl(const QString &host, int port, const QString &dbName);

    static bool parseDbiUrl(const U2DbiId& dbiId, QString& host, int& port, QString& dbName);

    static bool isDbInitialized(const U2DbiRef &dbiRef, U2OpStatus& os);

    /**
     * Updates the object name and increments the version.
     * The changes are tracked if it is needed.
     * Applies all changes to @object too.
     */
    static void renameObject(MysqlDbi* dbi, U2Object& object, const QString& newName,
        U2OpStatus& os);
    static void renameObject(MysqlModificationAction& updateAction, MysqlDbi* dbi,
        U2Object& object, const QString& newName, U2OpStatus& os);

    static void upgrade(const U2DbiRef &dbiRef, U2OpStatus &os);
};

}   // namespace U2

#endif // _U2_MYSQL_DBI_UTILS_H_
