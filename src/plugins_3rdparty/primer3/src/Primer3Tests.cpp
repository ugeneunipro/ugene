/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "Primer3Tests.h"

namespace U2 {

#define DBFILEPATH_ATTR "dbfile"
#define PROTEIN_ATTR "is_protein"
#define EXPECTED_LOAD_ATTR "is_load_expected"
#define CREATION_DATE_ATTR "creation_date"
#define CONTEXT_NAME_ATTR "index"
#define CHECK_DATE_ATTR "check_date"
/*
PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY
PRIMER_MISPRIMING_LIBRARY
PRIMER_SEQUENCE_QUALITY
*/

static const QString extensionsToCheck[14] = {".nhr", ".nnd", ".nni", ".nsd", ".nsi", ".nsq", ".nin", ".phr", ".pnd", ".pni", ".psd", ".psi", ".psq", ".pin"};

void GTest_Primer3::init(XMLTestFormat*, const QDomElement& el) {
    settings.setIncludedRegion(U2Region(0, -1));

    QString buf;
    int n_quality = 0;
    QDomNodeList inputParameters = el.elementsByTagName("plugin_primer_3_in");
    QSet<QString> changedIntervalValues;
    for (int inputParametersIndex = 0; inputParametersIndex < inputParameters.size(); inputParametersIndex++) {
        QDomNode n = inputParameters.item(inputParametersIndex);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement elInput = n.toElement();

        for (const QString& key : settings.getIntPropertyList()) {
            buf = elInput.attribute(key);
            if (!buf.isEmpty()) {
                settings.setIntProperty(key, buf.toInt());
            }
        }
        for (const QString& key : settings.getDoublePropertyList()) {
            buf = elInput.attribute(key);
            if (!buf.isEmpty()) {
                settings.setDoubleProperty(key, buf.toDouble());
            }
        }
        // 1
        buf = elInput.attribute("SEQUENCE_ID");
        if (!buf.isEmpty()) {
            settings.setSequenceName(buf.toLatin1());
        }
        // 2
        buf = elInput.attribute("SEQUENCE_TEMPLATE");
        if (!buf.isEmpty()) {
            settings.setSequence(buf.toLatin1());
        }

        buf = elInput.attribute("CIRCULAR");
        if (!buf.isEmpty()) {
            settings.setCircularity(buf == "true" ? true : false);
        }

        // 3
        buf = elInput.attribute("SEQUENCE_TARGET");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                if (str.split(',').size() == 2) {
                    regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
                } else {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_TARGET value: %1").arg(buf));
                    break;
                }
            }
            settings.setTarget(regionList);
        }
        // 4
        buf = elInput.attribute("SEQUENCE_OVERLAP_JUNCTION_LIST");
        if (!buf.isEmpty()) {
            QList<int> intList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                bool ok = false;
                int v = str.toInt(&ok);
                if (!ok) {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_OVERLAP_JUNCTION_LIST value: %1").arg(buf));
                    break;
                }
                intList.append(v);
            }
            settings.setOverlapJunctionList(intList);
        }
        // 5
        buf = elInput.attribute("SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST");
        if (!buf.isEmpty()) {
            QList<int> intList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                bool ok = false;
                int v = str.toInt(&ok);
                if (!ok) {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST value: %1").arg(buf));
                    break;
                }
                intList.append(v);
            }
            settings.setInternalOverlapJunctionList(intList);
        }
        // 6
        buf = elInput.attribute("SEQUENCE_EXCLUDED_REGION");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                if (str.split(',').size() == 2) {
                    regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
                } else {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_EXCLUDED_REGION value: %1").arg(buf));
                    break;
                }
            }
            settings.setExcludedRegion(regionList);
        }
        // 7
        buf = elInput.attribute("SEQUENCE_PRIMER_PAIR_OK_REGION_LIST");
        if (!buf.isEmpty()) {
            QList<QList<int>> intListList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                auto strList = str.split(',');
                if (strList.size() == 4) {
                    QList<int> list;
                    for (int i = 0; i < 4; i++) {
                        bool ok = false;
                        int v = str.toInt(&ok);
                        if (!ok) {
                            stateInfo.setError(GTest::tr("Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1").arg(buf));
                            break;
                        }

                        list << v;
                    }
                    intListList.append(list);
                } else {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1").arg(buf));
                    break;
                }
            }
            settings.setOkRegion(intListList);
        }
        // 8
        buf = elInput.attribute("SEQUENCE_INCLUDED_REGION");
        if (!buf.isEmpty()) {
            U2Region region;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                if (str.split(',').size() == 2) {
                    region = U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt());
                } else {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_INCLUDED_REGION value: %1").arg(buf));
                    break;
                }
            }
            settings.setIncludedRegion(region);
        }
        // 9
        buf = elInput.attribute("SEQUENCE_INTERNAL_EXCLUDED_REGION");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                if (str.split(',').size() == 2) {
                    regionList << U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt());
                } else {
                    stateInfo.setError(GTest::tr("Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1").arg(buf));
                    break;
                }
            }
            settings.setInternalOligoExcludedRegion(regionList);
        }
        // 9
        buf = elInput.attribute("PRIMER_PRODUCT_SIZE_RANGE");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                auto strList = str.split('-');
                if (strList.size() == 2) {
                    regionList << U2Region(strList[0].toInt(), strList[1].toInt() - strList[0].toInt() + 1);
                } else {
                    stateInfo.setError(GTest::tr("Illegal PRIMER_PRODUCT_SIZE_RANGE value: %1").arg(buf));
                    break;
                }
            }
            settings.setProductSizeRange(regionList);
        }
        // 10
        buf = elInput.attribute("SEQUENCE_START_CODON_SEQUENCE");
        if (!buf.isEmpty()) {
            settings.setStartCodonSequence(buf.toLatin1());
        }
        // 11
        buf = elInput.attribute("SEQUENCE_PRIMER");
        if (!buf.isEmpty()) {
            settings.setLeftInput(buf.toLatin1());
        }
        // 12
        buf = elInput.attribute("SEQUENCE_PRIMER_REVCOMP");
        if (!buf.isEmpty()) {
            settings.setRightInput(buf.toLatin1());
        }
        // 13
        buf = elInput.attribute("SEQUENCE_INTERNAL_OLIGO");
        if (!buf.isEmpty()) {
            settings.setInternalInput(buf.toLatin1());
        }
        // 14
        buf = elInput.attribute("SEQUENCE_OVERHANG_LEFT");
        if (!buf.isEmpty()) {
            settings.setLeftOverhang(buf.toLatin1());
        }
        // 15
        buf = elInput.attribute("SEQUENCE_OVERHANG_RIGHT");
        if (!buf.isEmpty()) {
            settings.setRightOverhang(buf.toLatin1());
        }
        // 16
        buf = elInput.attribute("PRIMER_MUST_MATCH_FIVE_PRIME");
        if (!buf.isEmpty()) {
            settings.setPrimerMustMatchFivePrime(buf.toLatin1());
        }
        // 17
        buf = elInput.attribute("PRIMER_MUST_MATCH_THREE_PRIME");
        if (!buf.isEmpty()) {
            settings.setPrimerMustMatchThreePrime(buf.toLatin1());
        }
        // 18
        buf = elInput.attribute("PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME");
        if (!buf.isEmpty()) {
            settings.setInternalPrimerMustMatchFivePrime(buf.toLatin1());
        }
        // 19
        buf = elInput.attribute("PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME");
        if (!buf.isEmpty()) {
            settings.setInternalPrimerMustMatchThreePrime(buf.toLatin1());
        }
        // 20
        buf = elInput.attribute("PRIMER_TASK");
        if (!buf.isEmpty()) {
            settings.setTaskByName(buf);
        }
        //21
        buf = elInput.attribute("SEQUENCE_QUALITY");
        if (!buf.isEmpty()) {
            QVector<int> qualityVecor;
            QStringList qualityList = buf.split(' ', QString::SkipEmptyParts);
            n_quality = qualityList.size();
            for (int qualityIndex = 0; qualityIndex < n_quality; qualityIndex++) {
                qualityVecor.append(qualityList.at(qualityIndex).toInt());
            }
            settings.setSequenceQuality(qualityVecor);
        }
        // 22
        buf = elInput.attribute("PRIMER_MISPRIMING_LIBRARY");
        if (!buf.isEmpty()) {
            settings.setRepeatLibraryPath((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
        }
        // 23
        buf = elInput.attribute("PRIMER_INTERNAL_MISHYB_LIBRARY");
        if (!buf.isEmpty()) {
            settings.setMishybLibraryPath((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Check parameters
    QDomNodeList outputParameters = el.elementsByTagName("plugin_primer_3_out");
    for (int outputParametersIndex = 0; outputParametersIndex < outputParameters.size(); outputParametersIndex++) {
        QDomNode n = outputParameters.item(outputParametersIndex);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement elOutput = n.toElement();

        buf = elOutput.attribute("PRIMER_ERROR");
        if (!buf.isEmpty()) {
            expectedErrorMessage = buf;
        }

        int leftCount = 0;
        int rightCount = 0;
        int internalCount = 0;
        int pairsCount = 0;

        buf = elOutput.attribute("PRIMER_LEFT_NUM_RETURNED");
        if (!buf.isEmpty()) {
            leftCount = buf.toInt();
        }
        buf = elOutput.attribute("PRIMER_RIGHT_NUM_RETURNED");
        if (!buf.isEmpty()) {
            rightCount = buf.toInt();
        }
        buf = elOutput.attribute("PRIMER_INTERNAL_NUM_RETURNED");
        if (!buf.isEmpty()) {
            internalCount = buf.toInt();
        }
        buf = elOutput.attribute("PRIMER_PAIR_NUM_RETURNED");
        if (!buf.isEmpty()) {
            pairsCount = buf.toInt();
        }

        if (pairsCount > 0 && (pairsCount != leftCount || pairsCount != rightCount || (internalCount != 0 && pairsCount != rightCount))) {
            stateInfo.setError(GTest::tr("Incorrect results num. Pairs: %1, left: %2, right: %3, inernal: %4")
                .arg(pairsCount).arg(leftCount).arg(rightCount).arg(internalCount));
            return;
        }

        QList<Primer> leftPrimers;
        for (int i = 0; i < leftCount; i++) {
            Primer primer;
            if (readPrimer(elOutput, "PRIMER_LEFT_" + QString::number(i), &primer, false)) {
                leftPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_LEFT_" + QString::number(i)));
                return;
            }
        }
        QList<Primer> rightPrimers;
        for (int i = 0; i < rightCount; i++) {
            Primer primer;
            if (readPrimer(elOutput, "PRIMER_RIGHT_" + QString::number(i), &primer, false)) {
                rightPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_RIGHT_" + QString::number(i)));
                return;
            }
        }
        QList<Primer> internalPrimers;
        for (int i = 0; i < internalCount; i++) {
            Primer primer;
            if (readPrimer(elOutput, "PRIMER_INTERNAL_" + QString::number(i), &primer, true)) {
                internalPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_INTERNAL_" + QString::number(i)));
                return;
            }
        }

        for (int pairIndex = 0; pairIndex < pairsCount; pairIndex++) {
            PrimerPair result;
            result.setLeftPrimer(&leftPrimers[pairIndex]);
            result.setRightPrimer(&rightPrimers[pairIndex]);
            if (internalCount > 0) {
                result.setInternalOligo(&internalPrimers[pairIndex]);
            }
            auto suffix = "_" + QString::number(pairIndex);
            {
                QString buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY");
                if (!buf.isEmpty()) {
                    result.setComplAny(buf.toDouble());
                } else {
                    buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY_TH");
                    if (!buf.isEmpty()) {
                        result.setComplAny(buf.toDouble());
                    }
                }
            }
            {
                QString buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END");
                if (!buf.isEmpty()) {
                    result.setComplEnd(buf.toDouble());
                } else {
                    buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END_TH");
                    if (!buf.isEmpty()) {
                        result.setComplEnd(buf.toDouble());
                    }
                }
            }
            {
                QString buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_PRODUCT_SIZE");
                if (!buf.isEmpty()) {
                    result.setProductSize(buf.toInt());
                }
            }
            {
                QString buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_PRODUCT_TM");
                if (!buf.isEmpty()) {
                    result.setProductTm(buf.toDouble());
                }
            }
            {
                QString buf = elOutput.attribute("PRIMER_PAIR" + suffix + "_PENALTY");
                if (!buf.isEmpty()) {
                    result.setProductQuality(buf.toDouble());
                }
            }

            expectedBestPairs << result;
        }
        /*for (int pairIndex = 0; pairIndex < pairsCount; pairIndex++) {
            expectedBestPairs.append(readPrimerPair(elOutput, (pairIndex > 0) ? ("_" + QString::number(pairIndex)) : QString()));
        }*/
    }

    if (settings.getSequence().isEmpty())
        stateInfo.setError(GTest::tr("Missing SEQUENCE tag"));  //??? may be remove from this place
    else {
        int sequenceLength = settings.getSequence().size();
        if (settings.getSequenceRange().isEmpty()) {
            settings.setSequenceRange(U2Region(0, sequenceLength));
        }
        if (n_quality != 0 && n_quality != sequenceLength)
            stateInfo.setError(GTest::tr("Error in sequence quality data"));  //??? may be remove from this place
        if ((settings.getPrimerSettings()->p_args.min_quality != 0 || settings.getPrimerSettings()->o_args.min_quality != 0) && n_quality == 0)
            stateInfo.setError(GTest::tr("Sequence quality data missing"));  //??? may be remove from this place
        if (settings.getPrimerSettings()->p_args.min_quality != 0 && settings.getPrimerSettings()->p_args.min_end_quality < settings.getPrimerSettings()->p_args.min_quality)
            settings.getPrimerSettings()->p_args.min_end_quality = settings.getPrimerSettings()->p_args.min_quality;
    }
}

void GTest_Primer3::prepare() {
    task = new Primer3SWTask(settings);
    addSubTask(task);
}

Task::ReportResult GTest_Primer3::report() {
    QList<PrimerPair> currentBestPairs = task->getBestPairs();

    if (!expectedErrorMessage.isEmpty()) {
        if (!task->hasError()) {
            stateInfo.setError(GTest::tr("No error, but expected: %1").arg(expectedErrorMessage));
        } else {
            auto currentErrorMessage = task->getError().replace("&lt;", "<");
            if (!currentErrorMessage.contains(expectedErrorMessage)) {
                stateInfo.setError(GTest::tr("An unexpected error. Expected: %1, but Actual: %2").arg(expectedErrorMessage).arg(currentErrorMessage));
            }
        }

        return ReportResult_Finished;
    }

    if (task->hasError() && (expectedBestPairs.size() > 0 || expectedSinglePrimers.size() > 0)) {
        stateInfo.setError(task->getError());
        return ReportResult_Finished;
    }

    if (currentBestPairs.size() != expectedBestPairs.size()) {
        stateInfo.setError(GTest::tr("PRIMER_PAIR_NUM_RETURNED is incorrect. Expected:%2, but Actual:%3").arg(expectedBestPairs.size()).arg(currentBestPairs.size()));
        return ReportResult_Finished;
    }

    for (int i = 0; i < expectedBestPairs.size(); i++) {
        if (!checkPrimerPair(currentBestPairs[i], expectedBestPairs[i], "_" + QString::number(i))) {
            return ReportResult_Finished;
        }
    }

    if (expectedBestPairs.size() != 0) {
        return ReportResult_Finished;
    }

    QList<Primer> currentSinglePrimers = task->getSinglePrimers();
    if (currentSinglePrimers.size() != expectedSinglePrimers.size()) {
        stateInfo.setError(GTest::tr("Incorrect single primers num. Expected:%2, but Actual:%3").arg(expectedBestPairs.size()).arg(currentBestPairs.size()));
        return ReportResult_Finished;
    }

    if (expectedSinglePrimers.size() > 0 && expectedBestPairs.size() == 0) {
        int i = 0;
    }

    /*    for (int i=0;i<currentBestPairs.num_pairs;i++)
        {
            //currentBestPairs->pairs[i]->
            if(!(currentBestPairs.pairs[i].left->position_penalty==expectedBestPairs.pairs[i].left->position_penalty)){
                stateInfo.setError(GTest::tr("PRIMER_LEFT_PENALTY_%1 is incorrect. Expected:%2, but Actual:%3").arg(i).arg(expectedBestPairs.pairs[i].left->position_penalty).arg(currentBestPairs.pairs[i].left->position_penalty));
                return ReportResult_Finished;
            }
        }
    */

    /*    need check error messages
    -        PRIMER_PAIR_PENALTY="3.4770"

            PRIMER_LEFT_PENALTY="3.380952"
            PRIMER_LEFT_SEQUENCE="TGACNACTGACGATGCAGA"
            PRIMER_LEFT="15,19"
            PRIMER_LEFT_TM="57.619"
            PRIMER_LEFT_GC_PERCENT="50.000"
            PRIMER_LEFT_SELF_ANY="4.00"
            PRIMER_LEFT_SELF_END="0.00"
            PRIMER_LEFT_END_STABILITY="8.2000"

            PRIMER_RIGHT_PENALTY="0.096021"
            PRIMER_RIGHT_SEQUENCE="ATCGATTTGGGTCGGGAT"
            PRIMER_RIGHT="94,18"
            PRIMER_RIGHT_TM="60.096"
            PRIMER_RIGHT_GC_PERCENT="50.000"
            PRIMER_RIGHT_SELF_ANY="6.00"
            PRIMER_RIGHT_SELF_END="2.00"
            PRIMER_RIGHT_END_STABILITY="9.3000"

            PRIMER_INTERNAL_OLIGO_PENALTY="3.098711"
            PRIMER_INTERNAL_OLIGO_SEQUENCE="GGTATTAGTGGGCCATTCG"
            PRIMER_INTERNAL_OLIGO="58,19"
            PRIMER_INTERNAL_OLIGO_TM="57.901"
            PRIMER_INTERNAL_OLIGO_GC_PERCENT="52.632"
            PRIMER_INTERNAL_OLIGO_SELF_ANY="5.00"
            PRIMER_INTERNAL_OLIGO_SELF_END="2.00"


            PRIMER_PAIR_COMPL_ANY="4.00"
            PRIMER_PAIR_COMPL_END="3.00"
            PRIMER_PRODUCT_SIZE="80"
    */

    return ReportResult_Finished;
}

GTest_Primer3::~GTest_Primer3() {
}

bool GTest_Primer3::readPrimer(QDomElement element, QString prefix, Primer* outPrimer, bool internalOligo) {
    {
        QString buf = element.attribute(prefix);
        if (!buf.isEmpty()) {
            int start = buf.split(',')[0].toInt();
            int length = buf.split(',')[1].toInt();
            outPrimer->setStart(start);
            outPrimer->setLength(length);
            if (prefix.contains("RIGHT")) {
                outPrimer->setStart(start - length + 1);
            }
        } else {
            return false;
        }
    }
    {
        QString buf = element.attribute(prefix + "_TM");
        if (!buf.isEmpty()) {
            outPrimer->setMeltingTemperature(buf.toDouble());
        }
    }
    {
        QString buf = element.attribute(prefix + "_GC_PERCENT");
        if (!buf.isEmpty()) {
            outPrimer->setGcContent(buf.toDouble());
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_ANY");
        if (!buf.isEmpty()) {
            outPrimer->setSelfAny(buf.toDouble());
        } else {
            buf = element.attribute(prefix + "_SELF_ANY_TH");
            if (!buf.isEmpty()) {
                outPrimer->setSelfAny(buf.toDouble());
            }
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_END");
        if (!buf.isEmpty()) {
            outPrimer->setSelfEnd(buf.toDouble());
        } else {
            buf = element.attribute(prefix + "_SELF_END_TH");
            if (!buf.isEmpty()) {
                outPrimer->setSelfEnd(buf.toDouble());
            }
        }
    }
    {
        QString buf = element.attribute(prefix + "_HAIRPIN_TH");
        if (!buf.isEmpty()) {
            outPrimer->setHairpin(buf.toDouble());
        }
    }
    {
        QString buf = element.attribute(prefix + "_PENALTY");
        if (!buf.isEmpty()) {
            outPrimer->setQuality(buf.toDouble());
        }
    }
    if (!internalOligo) {
        QString buf = element.attribute(prefix + "_END_STABILITY");
        if (!buf.isEmpty()) {
            outPrimer->setEndStability(buf.toDouble());
        }
    }
    return true;
}

PrimerPair GTest_Primer3::readPrimerPair(QDomElement element, QString suffix) {
    PrimerPair result;
    /*{
        Primer primer;
        if (readPrimer(element, "PRIMER_LEFT" + suffix, &primer, false)) {
            result.setLeftPrimer(&primer);
        }
    }
    {
        Primer primer;
        if (readPrimer(element, "PRIMER_RIGHT" + suffix, &primer, false)) {
            result.setRightPrimer(&primer);
        }
    }
    {
        Primer primer;
        if (readPrimer(element, "PRIMER_INTERNAL" + suffix, &primer, true)) {
            result.setInternalOligo(&primer);
        }
    }
    {
        QString buf = element.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY");
        if (!buf.isEmpty()) {
            result.setComplAny((buf.toDouble() * 100));
        } else {
            buf = element.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY_TH");
            if (!buf.isEmpty()) {
                result.setComplAny((buf.toDouble() * 100));
            }
        }
    }
    {
        QString buf = element.attribute("PRIMER_PAIR" + suffix + "_COMPL_END");
        if (!buf.isEmpty()) {
            result.setComplEnd(buf.toDouble());
        } else {
            buf = element.attribute("PRIMER_PAIR" + suffix + "_COMPL_END_TH");
            if (!buf.isEmpty()) {
                result.setComplAny(buf.toDouble());
            }
        }
    }
    {
        QString buf = element.attribute("PRIMER_PRODUCT_SIZE" + suffix);
        if (!buf.isEmpty()) {
            result.setProductSize(buf.toInt());
        }
    }*/
    return result;
}

bool GTest_Primer3::checkPrimerPair(const PrimerPair& primerPair, const PrimerPair& expectedPrimerPair, QString suffix) {
    if (!checkPrimer(primerPair.getLeftPrimer(), expectedPrimerPair.getLeftPrimer(), "PRIMER_LEFT" + suffix, false)) {
        return false;
    }
    if (!checkPrimer(primerPair.getRightPrimer(), expectedPrimerPair.getRightPrimer(), "PRIMER_RIGHT" + suffix, false)) {
        return false;
    }
    if (!checkPrimer(primerPair.getInternalOligo(), expectedPrimerPair.getInternalOligo(), "PRIMER_INTERNAL_OLIGO" + suffix, true)) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getComplAny(), expectedPrimerPair.getComplAny(), "PRIMER_PAIR" + suffix + "_COMPL_ANY")) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getComplEnd(), expectedPrimerPair.getComplEnd(), "PRIMER_PAIR" + suffix + "_COMPL_END")) {
        return false;
    }
    if (!checkIntProperty(primerPair.getProductSize(), expectedPrimerPair.getProductSize(), "PRIMER_PRODUCT_SIZE" + suffix)) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getProductQuality(), expectedPrimerPair.getProductQuality(), "PRIMER_PAIR" + suffix + "_PENALTY")) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getProductTm(), expectedPrimerPair.getProductTm(), "PRIMER_PAIR" + suffix + "_PRODUCT_TM")) {
        return false;
    }
    return true;
}

bool GTest_Primer3::checkPrimer(const Primer* primer, const Primer* expectedPrimer, QString prefix, bool internalOligo) {
    if (nullptr == primer) {
        if (nullptr == expectedPrimer) {
            return true;
        } else {
            stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2,%3, but Actual:NULL")
                                   .arg(prefix)
                                   .arg(expectedPrimer->getStart())
                                   .arg(expectedPrimer->getLength()));
            return false;
        }
    }
    if (nullptr == expectedPrimer) {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:NULL, but Actual:%2,%3")
                               .arg(prefix)
                               .arg(primer->getStart())
                               .arg(primer->getLength()));
        return false;
    }
    {
        if ((primer->getStart() + settings.getFirstBaseIndex() != expectedPrimer->getStart()) ||
            (primer->getLength() != expectedPrimer->getLength())) {
            stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2,%3, but Actual:%4,%5")
                                   .arg(prefix)
                                   .arg(expectedPrimer->getStart() + settings.getFirstBaseIndex())
                                   .arg(expectedPrimer->getLength())
                                   .arg(primer->getStart())
                                   .arg(primer->getLength()));
            return false;
        }
    }
    if (!checkDoubleProperty(primer->getMeltingTemperature(), expectedPrimer->getMeltingTemperature(), prefix + "_TM")) {
        return false;
    }
    if (!checkDoubleProperty(primer->getGcContent(), expectedPrimer->getGcContent(), prefix + "_GC_PERCENT")) {
        return false;
    }
    if (!checkDoubleProperty(primer->getSelfAny(), expectedPrimer->getSelfAny(), prefix + "_SELF_ANY")) {
        return false;
    }
    if (!checkDoubleProperty(primer->getSelfEnd(), expectedPrimer->getSelfEnd(), prefix + "_SELF_END")) {
        return false;
    }
    if (!internalOligo) {
        if (!checkDoubleProperty(primer->getEndStability(), expectedPrimer->getEndStability(), prefix + "_END_STABILITY")) {
            return false;
        }
    }
    return true;
}

bool GTest_Primer3::checkIntProperty(int value, int expectedValue, QString name) {
    if (value != expectedValue) {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2, but Actual:%3").arg(name).arg(expectedValue).arg(value));
        return false;
    }
    return true;
}

bool GTest_Primer3::checkDoubleProperty(double value, double expectedValue, QString name) {
    if (qAbs(value - expectedValue) > qAbs(value / 1000)) {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2, but Actual:%3").arg(name).arg(expectedValue).arg(value));
        return false;
    }
    return true;
}

}  // namespace U2
