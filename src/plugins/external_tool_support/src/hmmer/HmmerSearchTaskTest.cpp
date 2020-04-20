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

#include <QDebug>
#include <QDomElement>
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include "HmmerSearchTaskTest.h"
#include "HmmerSearchTask.h"
#include "utils/OutputCollector.h"

namespace U2 {


/*******************************
* GTest_GeneralUHMM3Search
********************************/

const QString GTest_UHMM3Search::SEQ_DOC_CTX_NAME_TAG = "seqDoc";
const QString GTest_UHMM3Search::HMM_FILENAME_TAG = "hmm";
const QString GTest_UHMM3Search::OUTPUT_DIR_TAG = "outputDir";
const QString GTest_UHMM3Search::HMMSEARCH_TASK_CTX_NAME_TAG = "taskCtxName";
const QString GTest_UHMM3Search::ALGORITHM_TYPE_OPTION_TAG = "algo";
const QString GTest_UHMM3Search::SW_CHUNK_SIZE_OPTION_TAG = "chunk";

const QString GTest_UHMM3Search::SEQ_E_OPTION_TAG = "seqE";
const QString GTest_UHMM3Search::SEQ_T_OPTION_TAG = "seqT";
const QString GTest_UHMM3Search::Z_OPTION_TAG = "z";
const QString GTest_UHMM3Search::DOM_E_OPTION_TAG = "domE";
const QString GTest_UHMM3Search::DOM_T_OPTION_TAG = "domT";
const QString GTest_UHMM3Search::DOM_Z_OPTION_TAG = "domZ";
const QString GTest_UHMM3Search::USE_BIT_CUTOFFS_OPTION_TAG = "ubc";
const QString GTest_UHMM3Search::INC_SEQ_E_OPTION_TAG = "incE";
const QString GTest_UHMM3Search::INC_SEQ_T_OPTION_TAG = "incT";
const QString GTest_UHMM3Search::INC_DOM_E_OPTION_TAG = "incdomE";
const QString GTest_UHMM3Search::INC_DOM_T_OPTION_TAG = "incdomT";
const QString GTest_UHMM3Search::MAX_OPTION_TAG = "max";
const QString GTest_UHMM3Search::F1_OPTION_TAG = "f1";
const QString GTest_UHMM3Search::F2_OPTION_TAG = "f2";
const QString GTest_UHMM3Search::F3_OPTION_TAG = "f3";
const QString GTest_UHMM3Search::NOBIAS_OPTION_TAG = "nobias";
const QString GTest_UHMM3Search::NONULL2_OPTION_TAG = "nonull2";
const QString GTest_UHMM3Search::SEED_OPTION_TAG = "seed";
const QString GTest_UHMM3Search::REMOTE_MACHINE_VAR = "MACHINE";

static void setDoubleOption(double& num, const QDomElement& el, const QString& optionName, TaskStateInfo& si) {
    if (si.hasError()) {
        return;
    }
    QString numStr = el.attribute(optionName);
    if (numStr.isEmpty()) {
        return;
    }

    bool ok = false;
    double ret = numStr.toDouble(&ok);
    if (!ok) {
        si.setError(QString("cannot_parse_double_number_from %1. Option: %2").arg(numStr).arg(optionName));
        return;
    }
    num = ret;
}

static void setUseBitCutoffsOption(HmmerSearchSettings::BitCutoffs& ret, const QDomElement& el, const QString& opName, TaskStateInfo& si) {
    if (si.hasError()) {
        return;
    }

    QString str = el.attribute(opName).toLower();
    if ("ga" == str) {
        ret = HmmerSearchSettings::p7H_GA;
    } else if ("nc" == str) {
        ret = HmmerSearchSettings::p7H_NC;
    } else if ("tc" == str) {
        ret = HmmerSearchSettings::p7H_TC;
    } else if (!str.isEmpty()) {
        si.setError(QString("unrecognized_value_in %1 option").arg(opName));
    }
}

static void setBooleanOption(bool& ret, const QDomElement& el, const QString& opName, TaskStateInfo& si) {
    if (si.hasError()) {
        return;
    }
    QString str = el.attribute(opName).toLower();

    if (!str.isEmpty() && "n" != str && "no" != str) {
        ret = true;//TRUE;
    } else {
        ret = false;//FALSE;
    }
}

static void setIntegerOption(int& num, const QDomElement& el, const QString& optionName, TaskStateInfo& si) {
    if (si.hasError()) {
        return;
    }
    QString numStr = el.attribute(optionName);
    if (numStr.isEmpty()) {
        return;
    }

    bool ok = false;
    int ret = numStr.toInt(&ok);
    if (!ok) {
        si.setError(QString("cannot_parse_integer_number_from %1. Option: %2").arg(numStr).arg(optionName));
        return;
    }
    num = ret;
}

void GTest_UHMM3Search::setSearchTaskSettings(HmmerSearchSettings& settings, const QDomElement& el, TaskStateInfo& si) {
    setDoubleOption(settings.e, el, GTest_UHMM3Search::SEQ_E_OPTION_TAG, si);
    setDoubleOption(settings.t, el, GTest_UHMM3Search::SEQ_T_OPTION_TAG, si);
    setDoubleOption(settings.z, el, GTest_UHMM3Search::Z_OPTION_TAG, si);
    setDoubleOption(settings.f1, el, GTest_UHMM3Search::F1_OPTION_TAG, si);
    setDoubleOption(settings.f2, el, GTest_UHMM3Search::F2_OPTION_TAG, si);
    setDoubleOption(settings.f3, el, GTest_UHMM3Search::F3_OPTION_TAG, si);
    setDoubleOption(settings.domE, el, GTest_UHMM3Search::DOM_E_OPTION_TAG, si);
    setDoubleOption(settings.domT, el, GTest_UHMM3Search::DOM_T_OPTION_TAG, si);
    setDoubleOption(settings.domZ, el, GTest_UHMM3Search::DOM_Z_OPTION_TAG, si);

    setBooleanOption(settings.doMax, el, GTest_UHMM3Search::MAX_OPTION_TAG, si);
    setBooleanOption(settings.noBiasFilter, el, GTest_UHMM3Search::NOBIAS_OPTION_TAG, si);
    setBooleanOption(settings.noNull2, el, GTest_UHMM3Search::NONULL2_OPTION_TAG, si);

    setIntegerOption(settings.seed, el, GTest_UHMM3Search::SEED_OPTION_TAG, si);

    setUseBitCutoffsOption(settings.useBitCutoffs, el, GTest_UHMM3Search::USE_BIT_CUTOFFS_OPTION_TAG, si);
}

void GTest_UHMM3Search::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    hmmFilename = el.attribute(HMM_FILENAME_TAG);

    searchTask = NULL;

    seqDocCtxName = el.attribute(SEQ_DOC_CTX_NAME_TAG);
    setSearchTaskSettings(settings, el, stateInfo);

    hmmFilename = el.attribute(HMM_FILENAME_TAG);

    outputDir = el.attribute(OUTPUT_DIR_TAG);
    settings.annotationTable = NULL;
    settings.noali = false;
}

void GTest_UHMM3Search::setAndCheckArgs() {
    assert(!stateInfo.hasError());
    if (hmmFilename.isEmpty()) {
        stateInfo.setError("hmm_filename_is_empty");
        return;
    }
    hmmFilename = env->getVar("COMMON_DATA_DIR") + "/" + hmmFilename;

    if (seqDocCtxName.isEmpty()) {
        stateInfo.setError("sequence_document_ctx_name_is_empty");
        return;
    }

    if (outputDir.isEmpty()) {
        stateInfo.setError("output_dir_is_empty");
        return;
    }

    outputDir = env->getVar("TEMP_DATA_DIR") + "/" + outputDir;

    Document* seqDoc = getContext<Document>(this, seqDocCtxName);
    if (NULL == seqDoc) {
        stateInfo.setError(QString("context %1 not found").arg(seqDocCtxName));
        return;
    }
    settings.sequenceUrl = seqDoc->getURLString();
}

void GTest_UHMM3Search::prepare() {
    CHECK_OP(stateInfo, );
    setAndCheckArgs();
    CHECK_OP(stateInfo, );

    settings.hmmProfileUrl = hmmFilename;
    settings.workingDir = outputDir;

    searchTask = new HmmerSearchTask(settings);
    //TODO: make collector as 'fancy' pointer
    searchTask->addListeners(QList<ExternalToolListener*>() << new OutputCollector());
    addSubTask(searchTask);
}

QList< Task* > GTest_UHMM3Search::onSubTaskFinished(Task * sub) {
    assert(NULL != sub);
    QList< Task* > res;
    if (searchTask != sub) {
        return res;
    }
    OutputCollector *collector = dynamic_cast<OutputCollector*>(searchTask->getListener(0));
    if (collector != NULL) {
        QString hmmSearchLog = collector->getLog();
        //TODO: check non empty log and file existence after writing
        QFile file(settings.workingDir + "/output.txt");
        file.open(QIODevice::WriteOnly);
        file.write(hmmSearchLog.toLatin1());
        file.close();
        delete collector;
    }
    return res;
}

Task::ReportResult GTest_UHMM3Search::report() {
    return ReportResult_Finished;
}

const QString GTest_UHMM3SearchCompare::ACTUAL_OUT_FILE_TAG = "actualOut";
const QString GTest_UHMM3SearchCompare::TRUE_OUT_FILE_TAG = "trueOut";

const int   BUF_SZ = 2048;
const char  TERM_SYM = '\0';

static bool getSignificance(const QByteArray& str) {
    if ("!" == str) {
        return true;
    } else if ("?" == str) {
        return false;
    }
    throw QString(GTest_UHMM3SearchCompare::tr("Can't parse significance:%1").arg(QString(str)));
}

static double getDouble(const QByteArray& numStr) {
    bool ok = false;
    double ret = numStr.toDouble(&ok);
    if (ok) {
        return ret;
    }
    throw QString(GTest_UHMM3SearchCompare::tr("Internal error (cannot parse float number from string '%1')").arg(QString(numStr)));
}

static float getFloat(const QByteArray& numStr) {
    return (float)getDouble(numStr);
}

static QByteArray getNextToken(QStringList& tokens) {
    if (tokens.isEmpty()) {
        throw QString("unexpected_end_of_line:token_is_missing");
    }
    return tokens.takeFirst().toLatin1();
}

static UHMM3SearchSeqDomainResult getDomainRes(QStringList& tokens) {
    UHMM3SearchSeqDomainResult res;

    getNextToken(tokens);
    res.isSignificant = getSignificance(getNextToken(tokens));
    res.score = getFloat(getNextToken(tokens));
    res.bias = getFloat(getNextToken(tokens));
    res.cval = getDouble(getNextToken(tokens));
    res.ival = getDouble(getNextToken(tokens));

    int hmmFrom = (int)getFloat(getNextToken(tokens));
    int hmmTo = (int)getFloat(getNextToken(tokens));
    res.queryRegion = U2Region(hmmFrom, hmmTo - hmmFrom);
    getNextToken(tokens);

    int aliFrom = (int)getFloat(getNextToken(tokens));
    int aliTo = (int)getFloat(getNextToken(tokens));
    res.seqRegion = U2Region(aliFrom - 1, aliTo - aliFrom + 1);
    getNextToken(tokens);

    int envFrom = (int)getFloat(getNextToken(tokens));
    int envTo = (int)getFloat(getNextToken(tokens));
    res.envRegion = U2Region(envFrom, envTo - envFrom);
    getNextToken(tokens);

    res.acc = getDouble(getNextToken(tokens));
    return res;
}

static void readLine(IOAdapter* io, QByteArray& to, QStringList* tokens = NULL) {
    assert(NULL != io);
    to.clear();
    QByteArray buf(BUF_SZ, TERM_SYM);
    bool there = false;
    int bytes = 0;
    while (!there) {
        int ret = io->readUntil(buf.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &there);
        if (0 > ret) {
            throw QString("read_error_occurred");
        }
        if (0 == ret) {
            break;
        }
        to.append(QByteArray(buf.data(), ret));
        bytes += ret;
    }
    to = to.trimmed();
    if (0 == bytes) {
        throw QString("unexpected_end_of_file_found");
    }

    if (NULL != tokens) {
        *tokens = QString(to).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    }
}

const double COMPARE_PERCENT_BORDER = 0.1; // 10 percent

template<class T>
static bool compareNumbers(T f1, T f2) {
    bool ret = false;
    if (0 == f1) {
        ret = 0 == f2 ? true : f2 < COMPARE_PERCENT_BORDER;
    } else if (0 == f2) {
        ret = f1 < COMPARE_PERCENT_BORDER;
    } else {
        ret = (qAbs(f1 - f2)) < COMPARE_PERCENT_BORDER;
    }
    if (!ret) {
        qDebug() << "!!! compare numbers mismatch: " << f1 << " and " << f2 << " !!!\n";
    }
    return ret;
}

void GTest_UHMM3SearchCompare::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    trueOutFilename = el.attribute(TRUE_OUT_FILE_TAG);
    actualOutFilename = el.attribute(ACTUAL_OUT_FILE_TAG);
}

void GTest_UHMM3SearchCompare::setAndCheckArgs() {
    assert(!hasError());

    if (trueOutFilename.isEmpty()) {
        stateInfo.setError("true_out_filename_is_empty");
        return;
    }
    trueOutFilename = env->getVar("COMMON_DATA_DIR") + "/" + trueOutFilename;

    if (actualOutFilename.isEmpty()) {
        stateInfo.setError("actual_out_filename_is_empty");
        return;
    }
    actualOutFilename = env->getVar("TEMP_DATA_DIR") + "/" + actualOutFilename;
}

UHMM3SearchResult GTest_UHMM3SearchCompare::getSearchResultFromOutput(const QString & filename) {
    assert(!filename.isEmpty());

    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filename));
    QScopedPointer< IOAdapter > io(iof->createIOAdapter());
    if (io.isNull()) {
        throw QString("cannot_create_io_adapter_for_'%1'_file").arg(filename);
    }
    if (!io->open(filename, IOAdapterMode_Read)) {
        throw QString("cannot_open_'%1'_file").arg(filename);
    }

    UHMM3SearchResult res;
    QByteArray buf;
    QStringList tokens;
    bool wasHeader = false;
    bool wasFullSeqResult = false;
    readLine(io.data(), buf); /* the first line. starts with # search or # phmmer */
    do {
        readLine(io.data(), buf);
        if (buf.isEmpty()) { /* but no error - empty line here */
            continue;
        }
        if (buf.startsWith("# HMMER 3")) {
            wasHeader = true;
            continue;
        }
        if (buf.startsWith("Scores for complete sequences")) {
            if (!wasHeader) {
                throw QString("hmmer_output_header_is_missing");
            }
            UHMM3SearchCompleteSeqResult& fullSeqRes = res.fullSeqResult;
            readLine(io.data(), buf);
            readLine(io.data(), buf);
            readLine(io.data(), buf);
            readLine(io.data(), buf, &tokens);
            if (buf.contains("inclusion threshold")) {
                readLine(io.data(), buf, &tokens);
            }
            if (buf.startsWith("[No hits detected")) {
                fullSeqRes.isReported = false;
                break;
            } else {
                fullSeqRes.eval = U2::getDouble(getNextToken(tokens));
                fullSeqRes.score = getFloat(getNextToken(tokens));
                fullSeqRes.bias = getFloat(getNextToken(tokens));
                /* skip best domain res. we will check it later */
                getNextToken(tokens);getNextToken(tokens);getNextToken(tokens);
                fullSeqRes.expectedDomainsNum = getFloat(getNextToken(tokens));
                fullSeqRes.reportedDomainsNum = (int)getFloat(getNextToken(tokens));
                fullSeqRes.isReported = true;
                wasFullSeqResult = true;
            }
            continue;
        }
        if (buf.startsWith("Domain annotation for each sequence")) {
            if (!wasFullSeqResult) {
                throw QString("full_seq_result_is_missing");
            }
            readLine(io.data(), buf);
            readLine(io.data(), buf);
            readLine(io.data(), buf);
            QList< UHMM3SearchSeqDomainResult >& domainResList = res.domainResList;
            assert(domainResList.isEmpty());

            int nDomains = res.fullSeqResult.reportedDomainsNum;
            int i = 0;
            for (i = 0; i < nDomains; ++i) {
                readLine(io.data(), buf, &tokens);
                domainResList << getDomainRes(tokens);
            }
            break;
        }
    } while (1);
    return res;
}

void GTest_UHMM3SearchCompare::generalCompareResults(const UHMM3SearchResult& myRes, const UHMM3SearchResult& trueRes, TaskStateInfo& ti) {
    const UHMM3SearchCompleteSeqResult& myFull = myRes.fullSeqResult;
    const UHMM3SearchCompleteSeqResult& trueFull = trueRes.fullSeqResult;

    if (myFull.isReported != trueFull.isReported) {
        ti.setError(QString("reported_flag_not_matched: %1 and %2").arg(myFull.isReported).arg(trueFull.isReported));
        return;
    }

    if (myFull.isReported) {
        if (!compareNumbers<float>(myFull.bias, trueFull.bias)) {
            ti.setError(QString("full_seq_bias_not_matched: %1 and %2").arg(myFull.bias).arg(trueFull.bias));  return;
        }
        if (!compareNumbers<double>(myFull.eval, trueFull.eval)) {
            ti.setError(QString("full_seq_eval_not_matched: %1 and %2").arg(myFull.eval).arg(trueFull.eval));  return;
        }
        if (!compareNumbers<float>(myFull.score, trueFull.score)) {
            ti.setError(QString("full_seq_score_not_matched: %1 and %2").arg(myFull.score).arg(trueFull.score)); return;
        }
        if (!compareNumbers<float>(myFull.expectedDomainsNum, trueFull.expectedDomainsNum)) {
            ti.setError(QString("full_seq_exp_not_matched: %1 and %2").arg(myFull.expectedDomainsNum).arg(trueFull.expectedDomainsNum));
            return;
        }
        if (myFull.reportedDomainsNum != trueFull.reportedDomainsNum) {
            ti.setError(QString("full_seq_n_not_matched: %1 and %2").arg(myFull.reportedDomainsNum).arg(trueFull.reportedDomainsNum));
            return;
        }
    }

    const QList< UHMM3SearchSeqDomainResult >& myDoms = myRes.domainResList;
    const QList< UHMM3SearchSeqDomainResult >& trueDoms = trueRes.domainResList;
    if (myDoms.size() != trueDoms.size()) {
        ti.setError(QString("domain_res_number_not_matched: %1 and %2").arg(myDoms.size()).arg(trueDoms.size()));
        return;
    }
    for (int i = 0; i < myDoms.size(); ++i) {
        UHMM3SearchSeqDomainResult myCurDom = myDoms.at(i);
        UHMM3SearchSeqDomainResult trueCurDom = trueDoms.at(i);
        if (!compareNumbers<double>(myCurDom.acc, trueCurDom.acc)) {
            ti.setError(QString("dom_acc_not_matched: %1 and %2").arg(myCurDom.acc).arg(trueCurDom.acc));   return;
        }
        if (!compareNumbers<float>(myCurDom.bias, trueCurDom.bias)) {
            ti.setError(QString("dom_bias_not_matched: %1 and %2").arg(myCurDom.bias).arg(trueCurDom.bias));  return;
        }
        if (!compareNumbers<double>(myCurDom.cval, trueCurDom.cval)) {
            ti.setError(QString("dom_cval_not_matched: %1 and %2").arg(myCurDom.cval).arg(trueCurDom.cval));  return;
        }
        if (!compareNumbers<double>(myCurDom.ival, trueCurDom.ival)) {
            ti.setError(QString("dom_ival_not_matched: %1 and %2").arg(myCurDom.ival).arg(trueCurDom.ival));  return;
        }
        if (!compareNumbers<float>(myCurDom.score, trueCurDom.score)) {
            ti.setError(QString("dom_score_not_matched: %1 and %2").arg(myCurDom.score).arg(trueCurDom.score)); return;
        }
        if (myCurDom.envRegion != trueCurDom.envRegion) {
            ti.setError(QString("dom_env_region_not_matched: %1---%2 and %3---%4").
                arg(myCurDom.envRegion.startPos).arg(myCurDom.envRegion.length).arg(trueCurDom.envRegion.startPos).
                arg(trueCurDom.envRegion.length)); return;
        }
        if (myCurDom.queryRegion != trueCurDom.queryRegion) {
            ti.setError(QString("dom_hmm_region_not_matched: %1---%2 and %3---%4").
                arg(myCurDom.queryRegion.startPos).arg(myCurDom.queryRegion.length).arg(trueCurDom.queryRegion.startPos).
                arg(trueCurDom.queryRegion.length)); return;
        }
        if (myCurDom.seqRegion != trueCurDom.seqRegion) {
            ti.setError(QString("dom_seq_region_not_matched: %1---%2 and %3---%4").
                arg(myCurDom.seqRegion.startPos).arg(myCurDom.seqRegion.length).arg(trueCurDom.seqRegion.startPos).
                arg(trueCurDom.seqRegion.length)); return;
        }
        if (myCurDom.isSignificant != trueCurDom.isSignificant) {
            ti.setError(QString("dom_sign_not_matched: %1 and %2").arg(myCurDom.isSignificant).arg(trueCurDom.isSignificant));
            return;
        }
    }
}

Task::ReportResult GTest_UHMM3SearchCompare::report() {
    assert(!hasError());
    setAndCheckArgs();
    if (hasError()) {
        return ReportResult_Finished;
    }

    UHMM3SearchResult trueRes;
    UHMM3SearchResult actualRes;
    try {
        trueRes = getSearchResultFromOutput(trueOutFilename);
        actualRes = getSearchResultFromOutput(actualOutFilename);
    } catch (const QString& ex) {
        stateInfo.setError(ex);
    } catch (...) {
        stateInfo.setError("undefined_error_occurred");
    }

    if (hasError()) {
        return ReportResult_Finished;
    }

    generalCompareResults(actualRes, trueRes, stateInfo);

    return ReportResult_Finished;
}

}
