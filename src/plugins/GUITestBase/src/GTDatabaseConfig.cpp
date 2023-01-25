/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "GTDatabaseConfig.h"
#include <harness/UGUITestBase.h>

#include <QSettings>

#include <U2Core/AppContext.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

namespace {

const QString SETTINGS_RECENT = "/shared_database/recent_connections/";

const QString SETTINGS_ROOT = "/gui_tests_database/";
const QString HOST = "host";
const QString PORT = "port";
const QString DATABASE_PREFIX = "database-prefix";
const QString DATABASE_PREFIX_2 = "database-prefix-2";
const QString DATABASE_UNINITED_PREFIX = "database-uninited-prefix";
const QString DATABASE_VERSION = "database-version";
const QString LOGIN = "login";
const QString READ_ONLY_LOGIN = "read_only_login";
const QString PASSWORD = "password";

QVariant getSetting(const QString& key) {
    QString customDatabaseIniPath = qgetenv("UGENE_TESTS_DATABASE_INI_PATH");
    QString databaseIniPath = customDatabaseIniPath.isEmpty() ? UGUITest::testDir + "_common_data/database.ini" : customDatabaseIniPath;
    QSettings settings(databaseIniPath, QSettings::IniFormat);
    return settings.value(SETTINGS_ROOT + key);
}

QString getStringSetting(const QString& key) {
    return getSetting(key).toString();
}

QString getOsSuffix() {
    if (isOsWindows()) {
        return "_win";
    } else if (isOsMac()) {
        return "_mac";
    } else {
        return "_linux";
    }
}

}  // namespace

QString GTDatabaseConfig::host() {
    return getStringSetting(HOST);
}

int GTDatabaseConfig::port() {
    return getSetting(PORT).toInt();
}

QString GTDatabaseConfig::database() {
    return getStringSetting(DATABASE_PREFIX) + getOsSuffix();
}

QString GTDatabaseConfig::secondDatabase() {
    return getStringSetting(DATABASE_PREFIX_2) + getOsSuffix();
}

QString GTDatabaseConfig::uninitializedDatabase() {
    return getStringSetting(DATABASE_UNINITED_PREFIX) + getOsSuffix();
}

QString GTDatabaseConfig::versionDatabase() {
    return getStringSetting(DATABASE_VERSION);
}

QString GTDatabaseConfig::login() {
    return getStringSetting(LOGIN);
}

QString GTDatabaseConfig::readOnlyLogin() {
    return getStringSetting(READ_ONLY_LOGIN);
}

QString GTDatabaseConfig::password() {
    return getStringSetting(PASSWORD);
}

void GTDatabaseConfig::initTestConnectionInfo(const QString& name, const QString& db, bool withCredentials, bool readOnly) {
    QString url = U2DbiUtils::createFullDbiUrl((readOnly ? readOnlyLogin() : login()), host(), port(), db);
    AppContext::getSettings()->setValue(SETTINGS_RECENT + name, url);
    if (withCredentials) {
        AppContext::getPasswordStorage()->addEntry(url, password(), true /*remember*/);
    }
}

}  // namespace U2
