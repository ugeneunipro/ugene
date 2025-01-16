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
#pragma once
#include <U2Core/U2Region.h>

#include <U2Test/XMLTestUtils.h>

#include "MfoldSettings.h"

namespace U2 {
// Runs MfoldTask.
// Prerequisites: this test must contain an input sequence context within its context.
// Attributes:
//   - mandatory:
//     - "inp" -- key for searching input sequence in the context.
//     - "region" -- location of input sequence that needs to be fed to task.
//         This is a string in format A..B, where A is starting position and B is ending position, 1 <= A, B <= seqLen.
//         A can be greater than B if sequence is circular.
//   - optional:
//     - shouldFail -- boolean attribute. By default, this attribute is not set, so it will check that task completes
//         successfully. If set, task failure will be checked.
//     - "T", "P", "MAX", "W", "MAXBP", "LAB_FR" -- internal parameters of mfold script, type int.
//         Restrictions on parameters can be found in GUI dialog.
//     - "NA_CONC", "MG_CONC", "ROT_ANG"-- internal parameters of mfold script, type double.
//         Restrictions on parameters can be found in GUI dialog.
//     - "message" -- check that log contains specified line.
// Output folder like mfold/2006.01.02_15-04-05 is saved in temporary directory of test suite.
class GTest_Mfold final : public XmlTest {
    Q_OBJECT

    MfoldSettings settings;
    QPair<int, int> region;  // start and end (not start and len like in U2Region)
    GTestLogHelper logHelper;
    QString inpCtx;
    QStringList logExpectedMessages;
    bool shouldFail = false;

protected:
    QList<Task*> onSubTaskFinished(Task*);

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Mfold, "mfold", TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel)

    void prepare();
    Task::ReportResult report();
};

namespace MfoldTests {
QList<XMLTestFactory*> createTestFactories();
};
}  // namespace U2
