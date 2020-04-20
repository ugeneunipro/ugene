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

#include <QDomElement>
#include <QFileInfo>
#include <QTextStream>

#include <U2Core/U2SafePoints.h>

#include "ClarkTests.h"

namespace U2 {

const QString GTest_CompareClarkDatabaseMetafiles::DATABASE1 = "database1";
const QString GTest_CompareClarkDatabaseMetafiles::DATABASE2 = "database2";
const QString GTest_CompareClarkDatabaseMetafiles::DATABASE1_PREFIXES = "database1-prefixes";
const QString GTest_CompareClarkDatabaseMetafiles::DATABASE2_PREFIXES = "database2-prefixes";

const QString GTest_CompareClarkDatabaseMetafiles::DATABASE_PREFIX_PLACEHOLDER = "!@#$%^&*()";
const QStringList GTest_CompareClarkDatabaseMetafiles::DATABASE_METAFILES = { ".custom",
                                                                             ".custom.fileToAccssnTaxID",
                                                                             ".custom.fileToTaxIDs",
                                                                             ".custom_rejected",
                                                                             "files_excluded.txt",
                                                                             "targets.txt" };
const QString GTest_CompareClarkDatabaseMetafiles::PREFIXES_SEPARATOR = ";";

void GTest_CompareClarkDatabaseMetafiles::init(XMLTestFormat *, const QDomElement &element) {
    // database 1
    checkNecessaryAttributeExistence(element, DATABASE1);
    CHECK_OP(stateInfo, );

    database1 = element.attribute(DATABASE1);
    CHECK_EXT(!database1.isEmpty(), setError("Database 1 URL is empty"), );

    XMLTestUtils::replacePrefix(env, database1);

    // database 2
    checkNecessaryAttributeExistence(element, DATABASE2);
    CHECK_OP(stateInfo, );

    database2 = element.attribute(DATABASE2);
    CHECK_EXT(!database2.isEmpty(), setError("Database 2 URL is empty"), );

    XMLTestUtils::replacePrefix(env, database2);

    // database 1 prefix
    checkNecessaryAttributeExistence(element, DATABASE1_PREFIXES);
    CHECK_OP(stateInfo, );

    foreach (QString prefix, element.attribute(DATABASE1_PREFIXES).split(PREFIXES_SEPARATOR)) {
        XMLTestUtils::replacePrefix(env, prefix);
        database1Prefixes << prefix;
    }

    // database 2 prefix
    checkNecessaryAttributeExistence(element, DATABASE2_PREFIXES);
    CHECK_OP(stateInfo, );

    foreach (QString prefix, element.attribute(DATABASE2_PREFIXES).split(PREFIXES_SEPARATOR)) {
        XMLTestUtils::replacePrefix(env, prefix);
        database2Prefixes << prefix;
    }
}

Task::ReportResult GTest_CompareClarkDatabaseMetafiles::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    CHECK_EXT(QFileInfo::exists(database1), setError(QString("Database 1 doesn't exist: '%1'").arg(database1)), ReportResult_Finished);
    CHECK_EXT(QFileInfo(database1).isDir(), setError(QString("Database 1 is not a directory: '%1'").arg(database1)), ReportResult_Finished);

    CHECK_EXT(QFileInfo::exists(database2), setError(QString("Database 2 doesn't exist: '%1'").arg(database2)), ReportResult_Finished);
    CHECK_EXT(QFileInfo(database2).isDir(), setError(QString("Database 2 is not a directory: '%1'").arg(database2)), ReportResult_Finished);

    foreach (const QString &metafileName, DATABASE_METAFILES) {
        const QString metafile1Url = database1 + "/" + metafileName;
        QFile metafile1(metafile1Url);
        bool opened = metafile1.open(QIODevice::ReadOnly);
        CHECK_EXT(opened, setError(QString("Can't open metafile '%1' for reading").arg(metafile1.fileName())), ReportResult_Finished);
        QTextStream metafileStream1(&metafile1);

        const QString metafile2Url = database2 + "/" + metafileName;
        QFile metafile2(metafile2Url);
        opened = metafile2.open(QIODevice::ReadOnly);
        CHECK_EXT(opened, setError(QString("Can't open metafile '%1' for reading").arg(metafile2.fileName())), ReportResult_Finished);
        QTextStream metafileStream2(&metafile2);

        int counter = 0;
        while (!metafileStream1.atEnd() && !metafileStream2.atEnd()) {
            QString metafile1Line = metafileStream1.readLine();
            foreach (const QString &prefix, database1Prefixes) {
                metafile1Line.replace(prefix, DATABASE_PREFIX_PLACEHOLDER);
            }

            QString metafile2Line = metafileStream2.readLine();
            foreach (const QString &prefix, database2Prefixes) {
                metafile2Line.replace(prefix, DATABASE_PREFIX_PLACEHOLDER);
            }

            CHECK_EXT(metafile1Line == metafile2Line,
                      setError(QString("Metafiles '%1' and '%2' differs at line %3: '%4' and '%5'")
                               .arg(metafile1Url).arg(metafile2Url).arg(++counter).arg(metafile1Line).arg(metafile2Line)), ReportResult_Finished);
        }

        CHECK_EXT(metafileStream1.atEnd() && metafileStream2.atEnd(),
                  setError(QString("Metafiles '%1' and '%2' have different number of lines")
                           .arg(metafile1Url).arg(metafile2Url)), ReportResult_Finished);
    }

    return ReportResult_Finished;
}

QList<XMLTestFactory *> ClarkTests::createTestFactories() {
    return { GTest_CompareClarkDatabaseMetafiles::createFactory() };
}

}   // naamespace U2
