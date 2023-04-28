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

#include "BAMUtilsUnitTests.h"

#include <QDir>
#include <QUuid>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/TestRunnerSettings.h>

#include <U2Formats/BAMUtils.h>

namespace U2 {

IMPLEMENT_TEST(BAMUtilsUnitTests, bamMergeCore) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString inputFiles = trs->getVar("COMMON_DATA_DIR") + "/regression/7862/orig.bam";
    auto uuid = QUuid::createUuid().toString();
    auto tempDir = QDir::temp().absoluteFilePath(uuid.mid(1, uuid.size() - 2));
    bool ok = QDir(tempDir).mkpath(".");
    CHECK_TRUE(ok, "Can't create tmp dir");

    QStringList bamUrls;
    for (int i = 0; i < 1050; i++) {
        auto fileName = tempDir + "/" + QString::number(i + 1) + ".bam";
        ok = QFile::copy(inputFiles, fileName);
        CHECK_TRUE(ok, QString("Can't copy %1 to %2").arg(inputFiles).arg(fileName));

        bamUrls << fileName;
    }
    QString resExpectedFile = tempDir + "/" + "res.bam";

    U2OpStatusImpl os;
    auto resFile = BAMUtils::mergeBam(bamUrls, resExpectedFile, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(resExpectedFile, resFile.getURLString(), "Files not equal");
    // 1050 input files + 1 result
    CHECK_TRUE(QDir(tempDir).entryList({ "*.bam" }).size() == 1051, "Incorrect files number");
}


}
