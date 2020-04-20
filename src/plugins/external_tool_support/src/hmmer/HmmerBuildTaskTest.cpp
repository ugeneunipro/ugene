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

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TextUtils.h>

#include "HmmerBuildTaskTest.h"

namespace U2 {

/**************************
* GTest_UHMMER3Build
**************************/

const QString GTest_UHMMER3Build::INPUT_FILE_TAG = "inputFile";
const QString GTest_UHMMER3Build::OUTPUT_FILE_TAG = "outputFile";
const QString GTest_UHMMER3Build::OUTPUT_DIR_TAG = "outputDir";
const QString GTest_UHMMER3Build::DEL_OUTPUT_TAG = "delOutput";

const QString GTest_UHMMER3Build::SEED_OPTION_TAG = "seed";

const QString GTest_UHMMER3Build::MODEL_CONSTRUCTION_OPTION_TAG = "mc";
const QString GTest_UHMMER3Build::RELATIVE_WEIGHTING_OPTION_TAG = "rw";
const QString GTest_UHMMER3Build::EFFECTIVE_WEIGHTING_OPTION_TAG = "ew";
const QString GTest_UHMMER3Build::E_VALUE_CALIBRATION_OPTION_TAG = "eval";

float infinity() {
    return std::numeric_limits< float >::infinity();
}

bool isfin(float x) {
    return !qIsNaN(x) && qIsInf(x);
}

#ifdef _WINDOWS

int isnan(float x) {
    return x != x;
}

int isinf(float x) {
    float inf = infinity();
    return inf == x || inf == -x;
}

#if (!defined(_MSC_VER) || _MSC_VER < 1800)
float roundf(float x) {
    if (isnan(x) || isinf(x)) {
        -return x;
    }
    return (float)(x >= 0.0 ? (int)(x + 0.5) : (int)(x - (int)(x - 1) + 0.5) + (int)(x - 1));
}
#endif

#endif // _WINDOWS

static void setSeedOption(HmmerBuildSettings & settings, TaskStateInfo & stateInfo, const QString& str) {
    if (str.isEmpty()) {
        return;
    }

    bool ok = false;
    int num = str.toInt(&ok);
    if (!ok || !(0 <= num)) {
        stateInfo.setError("cannot_parse_option_seed");
        return;
    }
    settings.seed = num;
}

static void setModelConstructionOption(HmmerBuildSettings & settings, TaskStateInfo & stateInfo, const QString& s) {
    QString str = s.toLower();
    if (str.startsWith("fast")) {
        settings.modelConstructionStrategy = HmmerBuildSettings::p7_ARCH_FAST;
        QString numStr = str.mid(4).trimmed();

        if (!numStr.isEmpty()) {
            bool ok = false;
            float num = numStr.toFloat(&ok);
            if (!ok || !(0 <= num && 1 >= num)) {
                stateInfo.setError("cannot_parse_mc_option: symfrac");
                return;
            }
            settings.symfrac = num;
        }
        return;
    }
    if (str.startsWith("hand")) {
        settings.modelConstructionStrategy = HmmerBuildSettings::p7_ARCH_HAND;
        return;
    }
    if (!str.isEmpty()) {
        stateInfo.setError("unrecognized_mc_option");
    }
}

static void setRelativeWeightingOption(HmmerBuildSettings & settings, TaskStateInfo & stateInfo, const QString& s) {
    QString str = s.toLower();
    if (str.startsWith("wgsc")) {
        settings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_GSC;
        return;
    }
    if (str.startsWith("wblosum")) {
        settings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_BLOSUM;
        QString numStr = str.mid(7).trimmed();
        if (!numStr.isEmpty()) {
            bool ok = false;
            float num = numStr.toFloat(&ok);
            if (!ok || !(0 <= num && 1 >= num)) {
                stateInfo.setError("cannot_parse_rw_option:wid");
                return;
            }
            settings.wid = num;
        }
        return;
    }
    if (str.startsWith("wpb")) {
        settings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_PB;
        return;
    }
    if (str.startsWith("wnone")) {
        settings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_NONE;
        return;
    }
    if (str.startsWith("wgiven")) {
        settings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_GIVEN;
        return;
    }
    if (!str.isEmpty()) {
        stateInfo.setError("unrecognized_rw_option");
    }
}

static void setEffectiveWeightingOption(HmmerBuildSettings & settings, TaskStateInfo & stateInfo, const QString& s) {
    QString str = s.toLower();
    if (str.startsWith("eent")) {
        settings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_ENTROPY;
        QString numStr = str.mid(4).trimmed();

        if (!numStr.isEmpty()) {
            QStringList nums = numStr.split(" ", QString::SkipEmptyParts);

            if (nums.isEmpty()) {
                stateInfo.setError("cannot_parse_ew_option: eent");
                return;
            }
            bool ok = false;
            double num = nums[0].toDouble(&ok);
            if (!ok) {
                stateInfo.setError("cannot_parse_ew_option: ere");
                return;
            }
            if (num > 0) {
                settings.ere = num;
                return;
            }
        }
        return;
    }
    if (str.startsWith("eclust")) {
        settings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_CLUST;
        QString numStr = str.mid(6).trimmed();

        if (!numStr.isEmpty()) {
            bool ok = false;
            double num = numStr.toDouble(&ok);
            if (!ok || !(0 <= num && 1 >= num)) {
                stateInfo.setError("cannot_parse_ew_option: eid");
                return;
            }
            settings.eid = num;
        }
        return;
    }
    if (str.startsWith("enone")) {
        settings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_NONE;
        return;
    }
    if (str.startsWith("eset")) {
        settings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_SET;
        QString numStr = str.mid(4).trimmed();
        bool ok = false;
        double num = numStr.toDouble(&ok);

        if (!ok || 0 > num) {
            stateInfo.setError("cannot_parse_ew_option: eset");
            return;
        }
        settings.eset = num;
        return;
    }
    if (!str.isEmpty()) {
        stateInfo.setError("unrecognized_ew_option");
    }
}

static void setEvalueCalibrationOption(HmmerBuildSettings & settings, TaskStateInfo & stateInfo, const QString& s) {
    QString str = s.toLower();
    if (str.isEmpty()) {
        return;
    }
    QStringList l = str.split(" ", QString::SkipEmptyParts);
    if (5 != l.size()) {
        stateInfo.setError("error_parsing_eval_option");
        return;
    }
    settings.evl = l[0].toInt();
    settings.evn = l[1].toInt();
    settings.efl = l[2].toInt();
    settings.efn = l[3].toInt();
    settings.eft = l[4].toDouble();
}

void GTest_UHMMER3Build::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    inFile = el.attribute(INPUT_FILE_TAG);
    outFile = el.attribute(OUTPUT_FILE_TAG);
    outputDir = el.attribute(OUTPUT_DIR_TAG);

    QString delOutStr = el.attribute(DEL_OUTPUT_TAG);
    delOutFile = !delOutStr.isEmpty() && delOutStr.toLower() != "no" && delOutStr.toLower() != "n";
    setBuildSettings(bldSettings, el, stateInfo);
}

void GTest_UHMMER3Build::setBuildSettings(HmmerBuildSettings & settings, const QDomElement& el, TaskStateInfo & ti) {
    setModelConstructionOption(settings, ti, el.attribute(MODEL_CONSTRUCTION_OPTION_TAG));
    setRelativeWeightingOption(settings, ti, el.attribute(RELATIVE_WEIGHTING_OPTION_TAG));
    setEffectiveWeightingOption(settings, ti, el.attribute(EFFECTIVE_WEIGHTING_OPTION_TAG));
    setEvalueCalibrationOption(settings, ti, el.attribute(E_VALUE_CALIBRATION_OPTION_TAG));
    setSeedOption(settings, ti, el.attribute(SEED_OPTION_TAG));
}

void GTest_UHMMER3Build::setAndCheckArgs() {
    if (hasError()) {
        return;
    }

    if (inFile.isEmpty()) {
        stateInfo.setError(tr("No input file given"));
        return;
    }
    inFile = env->getVar("COMMON_DATA_DIR") + "/" + inFile;

    if (outFile.isEmpty()) {
        stateInfo.setError(tr("No output file given"));
        return;
    }

    outFile = env->getVar("TEMP_DATA_DIR") + "/" + outFile;

    bldSettings.profileUrl = outFile;
    bldSettings.workingDir = env->getVar("TEMP_DATA_DIR");
}

void GTest_UHMMER3Build::prepare() {
    setAndCheckArgs();
    if (hasError()) {
        return;
    }

    buildTask = new HmmerBuildTask(bldSettings, inFile);
    addSubTask(buildTask);
}

Task::ReportResult GTest_UHMMER3Build::report() {
    if (buildTask->hasError()) {
        if (delOutFile) {
            QFile::remove(outFile);
        }
        stateInfo.setError(buildTask->getError());
    }
    return ReportResult_Finished;
}

void GTest_UHMMER3Build::cleanup() {
    if (!hasError() && delOutFile) {
        QFile::remove(outFile);
    }

    XmlTest::cleanup();
}

/**************************
* GTest_CompareHmmFiles
**************************/

const QString GTest_CompareHmmFiles::FILE1_NAME_TAG = "file1";
const QString GTest_CompareHmmFiles::FILE2_NAME_TAG = "file2";
const QString GTest_CompareHmmFiles::FILE1_TMP_TAG = "tmp1";
const QString GTest_CompareHmmFiles::FILE2_TMP_TAG = "tmp2";

const int   BUF_SZ = 2048;
const char  TERM_SYM = '\0';

const QByteArray DATE_STR = "DATE";
const QByteArray NAME_STR = "NAME";
const QByteArray HEADER_STR = "HMMER3/";

void GTest_CompareHmmFiles::init(XMLTestFormat *tf, const QDomElement &el) {
    Q_UNUSED(tf);

    filename1 = el.attribute(FILE1_NAME_TAG);
    filename2 = el.attribute(FILE2_NAME_TAG);

    QString file1TmpStr = el.attribute(FILE1_TMP_TAG);
    file1Tmp = !file1TmpStr.isEmpty() && file1TmpStr.toLower() != "no" && file1TmpStr.toLower() != "n";

    QString file2TmpStr = el.attribute(FILE2_TMP_TAG);
    file2Tmp = !file2TmpStr.isEmpty() && file2TmpStr.toLower() != "no" && file2TmpStr.toLower() != "n";
}

void GTest_CompareHmmFiles::setAndCheckArgs() {
    if (filename1.isEmpty()) {
        stateInfo.setError(tr("File #1 not set"));
        return;
    }
    filename1 = env->getVar(file1Tmp ? "TEMP_DATA_DIR" : "COMMON_DATA_DIR") + "/" + filename1;

    if (filename2.isEmpty()) {
        stateInfo.setError(tr("File #2 not set"));
        return;
    }
    filename2 = env->getVar(file2Tmp ? "TEMP_DATA_DIR" : "COMMON_DATA_DIR") + "/" + filename2;
}

static const float BUILD_COMPARE_FLOAT_EPS = (float)0.00002;

static bool compareStr(const QString& s1, const QString& s2) {
    assert(s1.size() == s2.size());

    QStringList words1 = s1.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QStringList words2 = s2.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    if (words1.size() != words2.size()) {
        return false;
    }

    int sz = words1.size();
    for (int i = 0; i < sz; ++i) {
        bool ok1 = false;
        bool ok2 = false;
        float num1 = 0;
        float num2 = 0;

        num1 = words1.at(i).toFloat(&ok1);
        num2 = words2.at(i).toFloat(&ok2);

        if (ok1 != ok2) {
            return false;
        } else {
            if (ok1) {
                if ((isfin(num1) && !isfin(num2)) || (isfin(num2) && !isfin(num1))) {
                    return false;
                }
                if (qAbs(num1 - num2) > BUILD_COMPARE_FLOAT_EPS) {
                    return false;
                }
                continue;
            } else {
                if (words1.at(i) != words2.at(i)) {
                    return false;
                }
                continue;
            }
        }
    }
    return true;
}

Task::ReportResult GTest_CompareHmmFiles::report() {
    assert(!hasError());
    setAndCheckArgs();
    if (hasError()) {
        return ReportResult_Finished;
    }

    IOAdapterFactory* iof1 = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filename1));
    QScopedPointer<IOAdapter> io1(iof1->createIOAdapter());
    if (io1.isNull()) {
        stateInfo.setError(tr("Error creating ioadapter for first file"));
        return ReportResult_Finished;
    }
    if (!io1->open(filename1, IOAdapterMode_Read)) {
        stateInfo.setError(tr("Error opening 1 file"));
        return ReportResult_Finished;
    }

    IOAdapterFactory* iof2 = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filename2));
    QScopedPointer<IOAdapter> io2(iof2->createIOAdapter());
    if (io2.isNull()) {
        stateInfo.setError(tr("Error creating ioadapter for second file"));
        return ReportResult_Finished;
    }
    if (!io2->open(filename2, IOAdapterMode_Read)) {
        stateInfo.setError(tr("Error opening second file"));
        return ReportResult_Finished;
    }

    QByteArray buf1(BUF_SZ, TERM_SYM);
    QByteArray buf2(BUF_SZ, TERM_SYM);
    int bytes1 = 0;
    int bytes2 = 0;

    do {
        bytes1 = io1->readUntil(buf1.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Include);
        bytes2 = io2->readUntil(buf2.data(), BUF_SZ, TextUtils::LINE_BREAKS, IOAdapter::Term_Include);
        if (buf1.startsWith(DATE_STR) && buf2.startsWith(DATE_STR)) {
            continue;
        }
        if (buf1.startsWith(HEADER_STR) && buf2.startsWith(HEADER_STR)) {
            continue;
        }
        if (buf1.startsWith(NAME_STR) && buf2.startsWith(NAME_STR)) {
            QString name1 = QByteArray(buf1.data(), bytes1).mid(NAME_STR.size()).trimmed();
            QString name2 = QByteArray(buf2.data(), bytes2).mid(NAME_STR.size()).trimmed();

            if (name1.startsWith(name2) || name2.startsWith(name1)) {
                continue;
            }
            stateInfo.setError(tr("Names of aligments not matched"));
            return ReportResult_Finished;
        }
        if (bytes1 != bytes2) {
            stateInfo.setError(tr("Comparing files length not matched"));
            return ReportResult_Finished;
        }

        QString s1 = QString::fromLatin1(buf1.data(), bytes1);
        QString s2 = QString::fromLatin1(buf2.data(), bytes2);
        if (!compareStr(s1, s2)) {
            stateInfo.setError(tr("Files parts not equal:'%1' and '%2'").arg(s1).arg(s2));
            return ReportResult_Finished;
        }
    } while (0 < bytes1 && 0 < bytes2);

    return ReportResult_Finished;
}

}
