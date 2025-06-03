/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "PrimersDimerFinderUnitTest.h"

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/PrimerDimersFinder.h>

#include <U2Test/TestRunnerSettings.h>

namespace U2 {

IMPLEMENT_TEST(PrimersDimerFinderUnitTest, regression_8162) {
    HeteroDimersFinder dimersFinder("CAACTATTCCCGATTGCGA", "GTTATACAGCTTCGCCTGAA");
    auto res = dimersFinder.getResult();
    QString resDimerNoNewLine = res.dimersOverlap.remove('\n').remove('\r');
    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString expectedResFilePath = trs->getVar("COMMON_DATA_DIR") + "/unit_tests/datatype/primers/regression_8162.txt";
    QFile expectedResFile(expectedResFilePath);
    expectedResFile.open(QIODevice::ReadOnly);
    QString expectedDimer = expectedResFile.readAll();
    QString expectedDimerNoNewLine = expectedDimer.remove('\n').remove('\r');
    CHECK_EQUAL(resDimerNoNewLine, resDimerNoNewLine, "dimer");
}

}  // namespace U2
