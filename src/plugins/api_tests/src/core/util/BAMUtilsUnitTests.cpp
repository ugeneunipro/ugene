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

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/TestRunnerSettings.h>

#include <U2Formats/BAMUtils.h>

namespace U2 {

IMPLEMENT_TEST(BAMUtilsUnitTests, bamMergeCore) {
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString inputFilesDir = trs->getVar("COMMON_DATA_DIR") + "/regression/7862/";

    QStringList bamUrls;
    for (int i = 0; i < 550; i++) {
        bamUrls << inputFilesDir + QString::number(i + 1) + ".bam";
    }
    QString tmpFile = QDir::temp().absoluteFilePath("res.bam");

    U2OpStatusImpl os;
    auto resFile = BAMUtils::mergeBam(bamUrls, tmpFile, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(tmpFile, resFile.getURLString(), "Files not equal");
}


}
