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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include "UtilTestActions.h"

namespace U2 {

/************************************************************************/
/* GTest_CopyFile */
/************************************************************************/
const QString GTest_CopyFile::FROM_URL_ATTR = "from";
const QString GTest_CopyFile::TO_URL_ATTR ="to";
const QString GTest_CopyFile::IS_DIRECTORY ="is_dir";

void GTest_CopyFile::init(XMLTestFormat *, const QDomElement &el) {
    fromUrl = el.attribute(FROM_URL_ATTR);
    toUrl = el.attribute(TO_URL_ATTR);
    isDir = el.attribute(IS_DIRECTORY) == "true";
    XMLTestUtils::replacePrefix(env, fromUrl);
    XMLTestUtils::replacePrefix(env, toUrl);
}

Task::ReportResult GTest_CopyFile::report() {
    QDir().mkpath(QFileInfo(toUrl).absoluteDir().absolutePath());
    bool copied = isDir ? copyDirectry(fromUrl, toUrl) : QFile::copy(fromUrl, toUrl);
    if (!copied) {
        stateInfo.setError(tr("Can't copy %1 '%2' to '%3'.")
                           .arg(isDir ? "directory" : "file")
                           .arg(fromUrl)
                           .arg(toUrl));
    }

    return ReportResult_Finished;
}

bool GTest_CopyFile::copyDirectry(const QString& from, const QString& to) {
    QDir dirFrom(from);

    QStringList foldersList = dirFrom.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(const QString& folder, foldersList) {
        QString subfolder = to + QDir::separator() + folder;
        dirFrom.mkpath(subfolder);
        bool copied = copyDirectry(from + QDir::separator() + folder, subfolder);
        CHECK(copied, false);
    }

    QStringList filesList = dirFrom.entryList(QDir::Files);
    foreach(const QString& file, filesList) {
        bool copied = QFile::copy(from + QDir::separator() + file, to + QDir::separator() + file);
        CHECK(copied, false);
    }

    return true;
}

/************************************************************************/
/* GTest_AddSharedDbUrl */
/************************************************************************/
const QString GTest_AddSharedDbUrl::URL_ATTR = "url";
const QString GTest_AddSharedDbUrl::PORT_ATTR = "port";
const QString GTest_AddSharedDbUrl::DB_NAME_ATTR = "db_name";
const QString GTest_AddSharedDbUrl::USER_NAME_ATTR = "user";
const QString GTest_AddSharedDbUrl::PASSWORD_ATTR = "password";
const QString GTest_AddSharedDbUrl::CUSTOM_DB_NAME = "custom-db-name";

void GTest_AddSharedDbUrl::init(XMLTestFormat *, const QDomElement &el) {
    const QString url = el.attribute(URL_ATTR);
    CHECK_EXT(!url.isEmpty(), failMissingValue(URL_ATTR), );
    const QString portStr = el.attribute(PORT_ATTR);
    int port = -1;
    if (!portStr.isEmpty()) {
        bool conversionOk = false;
        port = portStr.toInt(&conversionOk);
        CHECK_EXT(conversionOk, stateInfo.setError(QString("Cannot convert %1 to an integer value of port number").arg(portStr)), );
    }
    const QString dbName = el.attribute(DB_NAME_ATTR);
    CHECK_EXT(!dbName.isEmpty(), failMissingValue(DB_NAME_ATTR), );

    dbUrl = U2DbiUtils::createDbiUrl(url, port, dbName);
    userName = el.attribute(USER_NAME_ATTR);
    passwordIsSet = el.hasAttribute(PASSWORD_ATTR);
    password = el.attribute(PASSWORD_ATTR);
    customDbName = el.attribute(CUSTOM_DB_NAME);
}

Task::ReportResult GTest_AddSharedDbUrl::report() {
    Settings *settings = AppContext::getSettings();
    CHECK_EXT(NULL != settings, stateInfo.setError("Invalid application settings"), ReportResult_Finished);
    const QString fullDbUrl = U2DbiUtils::createFullDbiUrl(userName, dbUrl);
    settings->setValue("/shared_database/recent_connections/" + customDbName, fullDbUrl);

    if (passwordIsSet) {
        PasswordStorage *passStorage = AppContext::getPasswordStorage();
        CHECK_EXT(NULL != passStorage, stateInfo.setError("Invalid shared DB passwords storage"), ReportResult_Finished);
        passStorage->addEntry(fullDbUrl, password, true);
    }

    return ReportResult_Finished;
}

/*******************************
* GUrlTests
*******************************/
QList<XMLTestFactory*> UtilTestActions::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_CopyFile::createFactory());
    res.append(GTest_AddSharedDbUrl::createFactory());
    return res;
}

}   // namespace U2
