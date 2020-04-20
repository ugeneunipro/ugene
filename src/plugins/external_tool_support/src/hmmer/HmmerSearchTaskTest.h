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

#ifndef _U2_HMMER_SEARCH_TASK_TESTS_H_
#define _U2_HMMER_SEARCH_TASK_TESTS_H_

#include <U2Test/GTest.h>
#include <U2Test/XMLTestFormat.h>
#include <U2Test/XMLTestUtils.h>

#include "HmmerSearchSettings.h"

namespace U2 {

class HmmerSearchTask;

class GTest_UHMM3Search : public XmlTest {
    Q_OBJECT
public:
    static const QString SEQ_DOC_CTX_NAME_TAG; /* loaded sequence document */
    static const QString HMM_FILENAME_TAG;
    static const QString OUTPUT_DIR_TAG;
    static const QString HMMSEARCH_TASK_CTX_NAME_TAG; /* finished UHMM3SearchTask */
    static const QString ALGORITHM_TYPE_OPTION_TAG;
    static const QString SW_CHUNK_SIZE_OPTION_TAG;
    /* reporting thresholds options */
    static const QString SEQ_E_OPTION_TAG; /* -E */
    static const QString SEQ_T_OPTION_TAG; /* -T */
    static const QString Z_OPTION_TAG; /* -Z */
    static const QString DOM_E_OPTION_TAG; /* --domE */
    static const QString DOM_T_OPTION_TAG; /* --domT */
    static const QString DOM_Z_OPTION_TAG; /* --domZ */
    static const QString USE_BIT_CUTOFFS_OPTION_TAG; /* --cut_ga, --cut_nc, --cut_tc or none */
    /* significance thresholds options */
    static const QString INC_SEQ_E_OPTION_TAG; /* --incE */
    static const QString INC_SEQ_T_OPTION_TAG; /* --incT */
    static const QString INC_DOM_E_OPTION_TAG; /* --incdomE */
    static const QString INC_DOM_T_OPTION_TAG; /* --incdomT */
    /* acceleration heuristics options */
    static const QString MAX_OPTION_TAG; /* --max */
    static const QString F1_OPTION_TAG; /* --F1 */
    static const QString F2_OPTION_TAG; /* --F2 */
    static const QString F3_OPTION_TAG; /* --F3 */
    static const QString NOBIAS_OPTION_TAG; /* --nobias */
    static const QString NONULL2_OPTION_TAG; /* --nonull2 */
    static const QString SEED_OPTION_TAG; /* --seed */

    static const QString REMOTE_MACHINE_VAR;

    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_UHMM3Search, "hmm3-search");

    void prepare();
    ReportResult report();

    virtual QList< Task* > onSubTaskFinished(Task * sub);

private:
    void setAndCheckArgs();
    static void setSearchTaskSettings(HmmerSearchSettings& set, const QDomElement& el, TaskStateInfo& si);

    HmmerSearchSettings     settings;
    QString                 hmmFilename;
    QString                 seqDocCtxName;

    QString                 outputDir;

    HmmerSearchTask *searchTask;

}; // GTest_GeneralUHMM3Search

class UHMM3SearchSeqDomainResult {
public:
    UHMM3SearchSeqDomainResult() : score(0), bias(0), ival(0), cval(0), acc(0), isSignificant(false) {}

    float   score;
    float   bias;
    double  ival; /* independent e-value */
    double  cval; /* conditional e-value */

    U2Region queryRegion; /* hmm region for hmmsearch and seq region for phmmer */
    U2Region seqRegion;
    U2Region envRegion; /* envelope of domains location */

    double  acc; /* expected accuracy per residue of the alignment */

    bool    isSignificant; /* domain meets inclusion tresholds */
}; // UHMM3SearchSeqDomainResult

class UHMM3SearchCompleteSeqResult {
public:
    double  eval;
    float   score;
    float   bias;
    float   expectedDomainsNum;
    int     reportedDomainsNum;
    bool    isReported;

    UHMM3SearchCompleteSeqResult() : eval(0), score(0), bias(0), expectedDomainsNum(0), reportedDomainsNum(0), isReported(false) {}
}; // UHMM3SearchCompleteSeqResult

class UHMM3SearchResult {
public:
    UHMM3SearchCompleteSeqResult           fullSeqResult;
    QList< UHMM3SearchSeqDomainResult >    domainResList;
}; // UHMM3SearchResult

class GTest_UHMM3SearchCompare : public XmlTest {
    Q_OBJECT
public:
    static const QString ACTUAL_OUT_FILE_TAG;
    static const QString TRUE_OUT_FILE_TAG; /* file with original hmmer3 output */

    static UHMM3SearchResult getSearchResultFromOutput(const QString & filename);
    static void generalCompareResults(const UHMM3SearchResult& myRes, const UHMM3SearchResult& trueRes, TaskStateInfo& ti);

    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_UHMM3SearchCompare, "hmm3-search-compare");
    ReportResult report();

private:
    void setAndCheckArgs();

private:
    QString                     actualOutFilename;
    QString                     trueOutFilename;
}; // GTest_GeneralUHMM3SearchCompare

}

#endif
