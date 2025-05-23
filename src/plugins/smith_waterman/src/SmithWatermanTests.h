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

#include <QByteArray>
#include <QDomElement>

#include <U2Algorithm/SmithWatermanResult.h>
#include <U2Algorithm/SmithWatermanSettings.h>

#include <U2Core/GObject.h>
#include <U2Core/U2Region.h>

#include <U2Test/XMLTestUtils.h>

#include "SWAlgorithmTask.h"

namespace U2 {

struct SWresult {
    U2Region sInterval;
    int score;
};

class GTest_SmithWatermnan : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SmithWatermnan, "plugin_sw-algorithm");

    void prepare();
    Task::ReportResult report();

    static void sortByScore(QList<SmithWatermanResult>& resultsForSort);

private:
    SmithWatermanSettings s;
    bool parseExpected_res();
    bool toInt(QString& str, int& num);

    QByteArray patternSeq;
    QByteArray searchSeq;

    QString patternSeqDocName;
    QString searchSeqDocName;
    QString expected_res;
    QString pathToSubst;
    QString impl;
    int gapOpen;
    int gapExtension;
    float percentOfScore;

    QString resultDocContextName;
    QList<SWresult> expectedRes;

    Task* swAlgorithmTask;

    QString machinePath;
};

// Runs PairwiseAlignmentTask with Smith-Waterman SSE2 algorithm. Verifies that the task was completed successfully.
// Input attributes: "msa" -- name of the alignment in the context. The algorithm will run on the 1st and 2nd sequences
// of this alignment.
class GTest_SwAlignmentSse : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SwAlignmentSse, "sw-sse-alignment");

    void prepare();
    Task::ReportResult report();

private:
    QString msaObjNameInCtx;
    PairwiseAlignmentSmithWatermanTaskSettings* settings;
    PairwiseAlignmentSmithWatermanTask* task;
};

class GTest_SmithWatermnanPerf : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SmithWatermnanPerf, "test-sw-performance");

    void prepare();
    Task::ReportResult report();

private:
    SmithWatermanSettings s;

    QByteArray patternSeq;
    QByteArray searchSeq;

    QString patternSeqDocName;
    QString searchSeqDocName;
    QString pathToSubst;
    QString impl;
    int gapOpen;
    int gapExtension;
    float percentOfScore;

    QString resultDocContextName;
    QList<SWresult> expectedRes;

    Task* swAlgorithmTask;
};

}  // namespace U2
