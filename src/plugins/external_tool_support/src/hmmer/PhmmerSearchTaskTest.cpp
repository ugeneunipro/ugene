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
#include <QFile>

#include <U2Core/L10n.h>

#include "HmmerSearchTaskTest.h"
#include "PhmmerSearchTaskTest.h"
#include "utils/OutputCollector.h"

namespace U2 {

/****************************************
* GTest_UHMM3Phmmer
****************************************/

const QString GTest_UHMM3Phmmer::QUERY_FILENAME_TAG = "query";
const QString GTest_UHMM3Phmmer::DB_FILENAME_TAG = "db";

const QString GTest_UHMM3Phmmer::GAP_OPEN_PROBAB_OPTION_TAG = "popen";
const QString GTest_UHMM3Phmmer::GAP_EXTEND_PROBAB_OPTION_TAG = "pextend";
const QString GTest_UHMM3Phmmer::SUBST_MATR_NAME_OPTION_TAG = "substMatr";

const QString GTest_UHMM3Phmmer::OUTPUT_DIR_TAG = "outputDir";

const double BAD_DOUBLE_OPTION = -1.0;

static void setDoubleOption(double & to, const QString & str, TaskStateInfo & ti) {
    if (str.isEmpty()) {
        return;
    }
    bool ok = false;
    to = str.toDouble(&ok);
    if (!ok) {
        to = BAD_DOUBLE_OPTION;
        ti.setError(QString("cannot_parse_double_from: %1").arg(str));
    }
}

void GTest_UHMM3Phmmer::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    phmmerTask = NULL;
    queryFilename = el.attribute(QUERY_FILENAME_TAG);
    dbFilename = el.attribute(DB_FILENAME_TAG);

    setSearchTaskSettings(searchSettings, el, stateInfo);
    searchSettings.annotationTable = NULL;

    setDoubleOption(searchSettings.popen, el.attribute(GAP_OPEN_PROBAB_OPTION_TAG), stateInfo);

    setDoubleOption(searchSettings.pextend, el.attribute(GAP_EXTEND_PROBAB_OPTION_TAG), stateInfo);

    outputDir = el.attribute(OUTPUT_DIR_TAG);

    if (queryFilename.isEmpty()) {
        stateInfo.setError(L10N::badArgument("query sequence filename"));
        return;
    }
    queryFilename = env->getVar("COMMON_DATA_DIR") + "/" + queryFilename;

    searchSettings.querySequenceUrl = queryFilename;
}

void GTest_UHMM3Phmmer::setAndCheckArgs() {
    assert(!stateInfo.hasError());

    if (dbFilename.isEmpty()) {
        stateInfo.setError(L10N::badArgument("db sequence filename"));
        return;
    }
    dbFilename = env->getVar("COMMON_DATA_DIR") + "/" + dbFilename;

    if (outputDir.isEmpty()) {
        stateInfo.setError("output_dir_is_empty");
        return;
    }

    outputDir = env->getVar("TEMP_DATA_DIR") + "/" + outputDir;
}

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

// An unused function. Commneted to suppress the warning
//static void setUseBitCutoffsOption(int& ret, const QDomElement& el, const QString& opName, TaskStateInfo& si) {
//    if (si.hasError()) {
//        return;
//    }
//    QString str = el.attribute(opName).toLower();
//    if ("ga" == str) {
//        ret = HmmerSearchSettings::p7H_GA;
//    } else if ("nc" == str) {
//        ret = HmmerSearchSettings::p7H_NC;
//    } else if ("tc" == str) {
//        ret = HmmerSearchSettings::p7H_TC;
//    } else if (!str.isEmpty()) {
//        si.setError(QString("unrecognized_value_in %1 option").arg(opName));
//    }
//}

static void setBooleanOption(bool&ret, const QDomElement& el, const QString& opName, TaskStateInfo& si) {
    if (si.hasError()) {
        return;
    }
    QString str = el.attribute(opName).toLower();
    if (!str.isEmpty() && "n" != str && "no" != str) {
        ret = true;
    } else {
        ret = false;
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

void GTest_UHMM3Phmmer::setSearchTaskSettings(PhmmerSearchSettings& settings, const QDomElement& el, TaskStateInfo& si) {
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
}

void GTest_UHMM3Phmmer::prepare() {
    assert(!hasError() && NULL == phmmerTask);
    setAndCheckArgs();
    if (hasError()) {
        return;
    }
    searchSettings.workingDir = outputDir;
    searchSettings.targetSequenceUrl = dbFilename;
    searchSettings.querySequenceUrl = queryFilename;
    phmmerTask = new PhmmerSearchTask(searchSettings);
    phmmerTask->addListeners(QList<ExternalToolListener*>() << new OutputCollector());
    addSubTask(phmmerTask);
}

QList<Task*> GTest_UHMM3Phmmer::onSubTaskFinished(Task* subTask) {
    QList< Task* > res;
    if (subTask == phmmerTask) {
        OutputCollector *collector = dynamic_cast<OutputCollector*>(phmmerTask->getListener(0));
        if (collector != NULL) {
            QString hmmSearchLog = collector->getLog();
            //TODO: check non empty log and file existence after writing
            QFile file(searchSettings.workingDir + "/output.txt");
            file.open(QIODevice::WriteOnly);
            file.write(hmmSearchLog.toLatin1());
            file.close();
            delete collector;
        }
    }
    return res;
}

Task::ReportResult GTest_UHMM3Phmmer::report() {
    return ReportResult_Finished;
}

/****************************************
* GTest_UHMM3PhmmerCompare
****************************************/
const QString GTest_UHMM3PhmmerCompare::ACTUAL_OUT_FILE_TAG = "actualOut";
const QString GTest_UHMM3PhmmerCompare::TRUE_OUT_FILE_TAG = "trueOut";

void GTest_UHMM3PhmmerCompare::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    trueOutFilename = el.attribute(TRUE_OUT_FILE_TAG);
    actualOutFilename = el.attribute(ACTUAL_OUT_FILE_TAG);
}

void GTest_UHMM3PhmmerCompare::setAndCheckArgs() {
    if (trueOutFilename.isEmpty()) {
        stateInfo.setError(L10N::badArgument("true out filename"));
        return;
    }
    trueOutFilename = env->getVar("COMMON_DATA_DIR") + "/" + trueOutFilename;

    if (actualOutFilename.isEmpty()) {
        stateInfo.setError("actual_out_filename_is_empty");
        return;
    }
    actualOutFilename = env->getVar("TEMP_DATA_DIR") + "/" + actualOutFilename;
}

Task::ReportResult GTest_UHMM3PhmmerCompare::report() {
    assert(!hasError());
    setAndCheckArgs();
    if (hasError()) {
        return ReportResult_Finished;
    }

    UHMM3SearchResult trueRes;
    UHMM3SearchResult actualRes;
    try {
        trueRes = GTest_UHMM3SearchCompare::getSearchResultFromOutput(trueOutFilename);
        actualRes = GTest_UHMM3SearchCompare::getSearchResultFromOutput(actualOutFilename);
    } catch (const QString& ex) {
        stateInfo.setError(ex);
    } catch (...) {
        stateInfo.setError("undefined_error_occurred");
    }

    if (hasError()) {
        return ReportResult_Finished;
    }

    GTest_UHMM3SearchCompare::generalCompareResults(actualRes, trueRes, stateInfo);

    return ReportResult_Finished;
}

}
