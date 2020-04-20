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

#ifndef _U2_PHMMER_SEARCH_TASK_TEST_H_
#define _U2_PHMMER_SEARCH_TASK_TEST_H_

#include "PhmmerSearchTask.h"

#include <U2Test/GTest.h>
#include <U2Test/XMLTestFormat.h>
#include <U2Test/XMLTestUtils.h>

namespace U2 {

/*****************************************
* Test for hmmer3 phmmer.
* settings set by same tags from hmm3-search and hmm3-build tests + gaps probab. options and subst. matr
* we test here 1<->1 queries
*****************************************/
class GTest_UHMM3Phmmer : public XmlTest {
    Q_OBJECT
public:
    static const QString QUERY_FILENAME_TAG;
    static const QString DB_FILENAME_TAG;

    static const QString GAP_OPEN_PROBAB_OPTION_TAG;
    static const QString GAP_EXTEND_PROBAB_OPTION_TAG;
    static const QString SUBST_MATR_NAME_OPTION_TAG; /* name of registered substitution matrix. if empty - BLOSUM62 is used */

    static const QString OUTPUT_DIR_TAG;

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_UHMM3Phmmer, "hmm3-phmmer");

    void prepare();
    ReportResult report();
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    void setAndCheckArgs();

    static void setSearchTaskSettings(PhmmerSearchSettings& set, const QDomElement& el, TaskStateInfo& si);

private:
    PhmmerSearchSettings    searchSettings;
    QString                 queryFilename;
    QString                 dbFilename;
    PhmmerSearchTask *      phmmerTask;
    QString                 outputDir;
}; // GTest_UHMM3Phmmer

/*****************************************
* Test compares original hmmer3 phmmer results with UHMM3SearchResults
*
* Note, that you should make original hmmer3 to show results in academic version (e.g. 1.01e-23)
*****************************************/

class GTest_UHMM3PhmmerCompare : public XmlTest {
    Q_OBJECT
public:
    static const QString ACTUAL_OUT_FILE_TAG;
    static const QString TRUE_OUT_FILE_TAG;

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_UHMM3PhmmerCompare, "hmm3-phmmer-compare");
    ReportResult report();

private:
    void setAndCheckArgs();

private:
    QString             actualOutFilename;
    QString             trueOutFilename;
}; // GTest_UHMM3PhmmerCompare

}

#endif
