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

#ifndef _U2_UTIL_TEST_ACTIONS_H_
#define _U2_UTIL_TEST_ACTIONS_H_

#include <U2Test/XMLTestUtils.h>
#include <QDomElement>

namespace U2 {

class GTest_CopyFile : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CopyFile, "copy-file");

    ReportResult report();

private:
    bool copyDirectry(const QString& from, const QString& to);

    QString             fromUrl;
    QString             toUrl;
    bool                isDir;

    static const QString FROM_URL_ATTR;
    static const QString TO_URL_ATTR;
    static const QString IS_DIRECTORY;
};

class GTest_AddSharedDbUrl : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_AddSharedDbUrl, "add-shared-db");

    ReportResult report();

private:
    QString dbUrl;
    QString userName;
    QString password;
    QString customDbName;
    bool passwordIsSet;

    static const QString URL_ATTR;
    static const QString PORT_ATTR;
    static const QString DB_NAME_ATTR;
    static const QString USER_NAME_ATTR;
    static const QString PASSWORD_ATTR;
    static const QString CUSTOM_DB_NAME;
};

class UtilTestActions {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}   // namespace U2

#endif // _U2_UTIL_TEST_ACTIONS_H_
