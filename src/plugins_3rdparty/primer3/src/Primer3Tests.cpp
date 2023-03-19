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

#include "Primer3Tests.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

void GTest_Primer3::init(XMLTestFormat*, const QDomElement& el) {
    settings = new Primer3TaskSettings();
    settings->getPrimerSettings()->first_base_index = 0;  // Default mode for XML tests.
    settings->setIncludedRegion(U2Region(0, -1));

    QString buf;
    QDomNodeList inputParameters = el.elementsByTagName("plugin_primer_3_in");
    for (int inputParametersIndex = 0; inputParametersIndex < inputParameters.size(); inputParametersIndex++) {
        QDomNode n = inputParameters.item(inputParametersIndex);
        SAFE_POINT_EXT(n.isElement(), localErrorMessage = GTest::tr("QDomNode isn't element"), );

        if (!n.isElement()) {
            continue;
        }
        QDomElement elInput = n.toElement();

        for (const QString& key : settings->getIntPropertyList()) {
            buf = elInput.attribute(key);
            if (!buf.isEmpty()) {
                settings->setIntProperty(key, buf.toInt());
            }
        }
        for (const QString& key : settings->getDoublePropertyList()) {
            buf = elInput.attribute(key);
            if (!buf.isEmpty()) {
                settings->setDoubleProperty(key, buf.toDouble());
            }
        }
        // 1
        buf = elInput.attribute("SEQUENCE_ID");
        if (!buf.isEmpty()) {
            settings->setSequenceName(buf.toLatin1());
        }
        // 2
        buf = elInput.attribute("SEQUENCE_TEMPLATE");
        if (!buf.isEmpty()) {
            settings->setSequence(buf.toLatin1());
        }

        buf = elInput.attribute("CIRCULAR");
        if (!buf.isEmpty()) {
            settings->setCircularity(buf == "true");
        }

        // 3
        buf = elInput.attribute("SEQUENCE_TARGET");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                auto sting = str.split(',');
                if (sting.size() == 2) {
                    QList<int> v;
                    for (int i = 0; i < 2; i++) {
                        bool ok = false;
                        v << sting[i].toInt(&ok);
                        if (!ok) {
                            localErrorMessage = GTest::tr("Illegal SEQUENCE_TARGET value: %1").arg(str);
                            break;
                        }
                    }
                    regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
                } else {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_TARGET value: %1").arg(str);
                    break;
                }
            }
            settings->setTarget(regionList);
        }
        // 4
        buf = elInput.attribute("SEQUENCE_OVERLAP_JUNCTION_LIST");
        if (!buf.isEmpty()) {
            QList<int> intList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                bool ok = false;
                int v = str.toInt(&ok);
                if (!ok) {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_OVERLAP_JUNCTION_LIST value: %1").arg(str);
                    break;
                }
                intList.append(v);
            }
            settings->setOverlapJunctionList(intList);
        }
        // 5
        buf = elInput.attribute("SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST");
        if (!buf.isEmpty()) {
            QList<int> intList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                bool ok = false;
                int v = str.toInt(&ok);
                if (!ok) {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST value: %1").arg(str);
                    break;
                }
                intList.append(v);
            }
            settings->setInternalOverlapJunctionList(intList);
        }
        // 6
        buf = elInput.attribute("SEQUENCE_EXCLUDED_REGION");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                if (str.split(',').size() == 2) {
                    regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
                } else {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_EXCLUDED_REGION value: %1").arg(str);
                    break;
                }
            }
            settings->setExcludedRegion(regionList);
        }
        // 7
        buf = elInput.attribute("SEQUENCE_PRIMER_PAIR_OK_REGION_LIST");
        if (!buf.isEmpty()) {
            QList<QList<int>> intListList;
            auto qwe = buf.split(';', QString::SkipEmptyParts);
            for (const QString& str : buf.split(';', QString::SkipEmptyParts)) {
                auto strList = str.split(',');
                if (strList.size() == 4) {
                    QList<int> list;
                    for (int i = 0; i < 4; i++) {
                        bool ok = false;
                        int v = strList[i].toInt(&ok);
                        if (!ok) {
                            v = -1;
                        }

                        list << v;
                    }
                    intListList.append(list);
                } else {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1").arg(str);
                    break;
                }
            }
            settings->setOkRegion(intListList);
        }
        // 8
        buf = elInput.attribute("SEQUENCE_INCLUDED_REGION");
        if (!buf.isEmpty()) {
            U2Region region;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                if (str.split(',').size() == 2) {
                    region = U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt());
                } else {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_INCLUDED_REGION value: %1").arg(str);
                    break;
                }
            }
            settings->setIncludedRegion(region);
        }
        // 9
        buf = elInput.attribute("SEQUENCE_INTERNAL_EXCLUDED_REGION");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', QString::SkipEmptyParts)) {
                auto string = str.split(',');
                if (string.size() == 2) {
                    QList<int> v;
                    for (int i = 0; i < 2; i++) {
                        bool ok = false;
                        v << string[i].toInt(&ok);
                        if (!ok) {
                            localErrorMessage = GTest::tr("Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1").arg(str);
                            break;
                        }
                    }

                    regionList << U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt());
                } else {
                    localErrorMessage = GTest::tr("Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1").arg(str);
                    break;
                }
            }
            settings->setInternalOligoExcludedRegion(regionList);
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
                    localErrorMessage = GTest::tr("Illegal PRIMER_PRODUCT_SIZE_RANGE value: %1").arg(str);
                    break;
                }
            }
            settings->setProductSizeRange(regionList);
        }
        // 10
        buf = elInput.attribute("SEQUENCE_START_CODON_SEQUENCE");
        if (!buf.isEmpty()) {
            settings->setStartCodonSequence(buf.toLatin1());
        }
        // 11
        buf = elInput.attribute("SEQUENCE_PRIMER");
        if (!buf.isEmpty()) {
            settings->setLeftInput(buf.toLatin1());
        }
        // 12
        buf = elInput.attribute("SEQUENCE_PRIMER_REVCOMP");
        if (!buf.isEmpty()) {
            settings->setRightInput(buf.toLatin1());
        }
        // 13
        buf = elInput.attribute("SEQUENCE_INTERNAL_OLIGO");
        if (!buf.isEmpty()) {
            settings->setInternalInput(buf.toLatin1());
        }
        // 14
        buf = elInput.attribute("SEQUENCE_OVERHANG_LEFT");
        if (!buf.isEmpty()) {
            settings->setLeftOverhang(buf.toLatin1());
        }
        // 15
        buf = elInput.attribute("SEQUENCE_OVERHANG_RIGHT");
        if (!buf.isEmpty()) {
            settings->setRightOverhang(buf.toLatin1());
        }
        // 16
        buf = elInput.attribute("PRIMER_MUST_MATCH_FIVE_PRIME");
        if (!buf.isEmpty()) {
            settings->setPrimerMustMatchFivePrime(buf.toLatin1());
        }
        // 17
        buf = elInput.attribute("PRIMER_MUST_MATCH_THREE_PRIME");
        if (!buf.isEmpty()) {
            settings->setPrimerMustMatchThreePrime(buf.toLatin1());
        }
        // 18
        buf = elInput.attribute("PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME");
        if (!buf.isEmpty()) {
            settings->setInternalPrimerMustMatchFivePrime(buf.toLatin1());
        }
        // 19
        buf = elInput.attribute("PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME");
        if (!buf.isEmpty()) {
            settings->setInternalPrimerMustMatchThreePrime(buf.toLatin1());
        }
        // 20
        buf = elInput.attribute("PRIMER_TASK");
        if (!buf.isEmpty()) {
            settings->setTaskByName(buf);
        }
        // 21
        buf = elInput.attribute("SEQUENCE_QUALITY");
        if (!buf.isEmpty()) {
            QVector<int> qualityVecor;
            QStringList qualityList = buf.split(' ', QString::SkipEmptyParts);
            qualityNumber = qualityList.size();
            for (int qualityIndex = 0; qualityIndex < qualityNumber; qualityIndex++) {
                bool ok = false;
                int v = qualityList.at(qualityIndex).toInt(&ok);
                if (!ok) {
                    qualityNumber = -1;
                    break;
                }
                qualityVecor.append(v);
            }
            settings->setSequenceQuality(qualityVecor);
        }
        // 22
        buf = elInput.attribute("PRIMER_MISPRIMING_LIBRARY");
        if (!buf.isEmpty()) {
            settings->setRepeatLibraryPath((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
        }
        // 23
        buf = elInput.attribute("PRIMER_INTERNAL_MISHYB_LIBRARY");
        if (!buf.isEmpty()) {
            settings->setMishybLibraryPath((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
        }
        // 24
        buf = elInput.attribute("PRIMER_THERMODYNAMIC_PARAMETERS_PATH");
        if (!buf.isEmpty()) {
            settings->setThermodynamicParametersPath((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
        }
        // 25
        buf = elInput.attribute("PRIMER_MIN_THREE_PRIME_DISTANCE");
        if (!buf.isEmpty()) {
            settings->setIntProperty("PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE", buf.toInt());
            settings->setIntProperty("PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE", buf.toInt());
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Check parameters
    QDomNodeList outputParameters = el.elementsByTagName("plugin_primer_3_out");
    for (int outputParametersIndex = 0; outputParametersIndex < outputParameters.size(); outputParametersIndex++) {
        QDomNode n = outputParameters.item(outputParametersIndex);
        SAFE_POINT_EXT(n.isElement(), localErrorMessage = GTest::tr("QDomNode isn't element"), );

        if (!n.isElement()) {
            continue;
        }
        QDomElement elOutput = n.toElement();

        buf = elOutput.attribute("PRIMER_ERROR");
        if (!buf.isEmpty()) {
            expectedErrorMessage = buf;
        }

        buf = elOutput.attribute("PRIMER_WARNING");
        if (!buf.isEmpty()) {
            expectedWarningMessage = buf;
        }

        buf = elOutput.attribute("PRIMER_STOP_CODON_POSITION");
        if (!buf.isEmpty()) {
            stopCodonPos = buf.toInt();
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
                                   .arg(pairsCount)
                                   .arg(leftCount)
                                   .arg(rightCount)
                                   .arg(internalCount));
            return;
        }

        QList<PrimerSingle> leftPrimers;
        for (int i = 0; i < leftCount; i++) {
            PrimerSingle primer;
            if (readPrimer(elOutput, "PRIMER_LEFT_" + QString::number(i), &primer, false)) {
                leftPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_LEFT_" + QString::number(i)));
                return;
            }
        }
        QList<PrimerSingle> rightPrimers;
        for (int i = 0; i < rightCount; i++) {
            PrimerSingle primer(OT_RIGHT);
            if (readPrimer(elOutput, "PRIMER_RIGHT_" + QString::number(i), &primer, false)) {
                rightPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_RIGHT_" + QString::number(i)));
                return;
            }
        }
        QList<PrimerSingle> internalPrimers;
        for (int i = 0; i < internalCount; i++) {
            PrimerSingle primer(OT_INTL);
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
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY");
                if (!value.isEmpty()) {
                    result.setComplAny(value.toDouble());
                } else {
                    value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY_TH");
                    if (!value.isEmpty()) {
                        result.setComplAny(value.toDouble());
                    }
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END");
                if (!value.isEmpty()) {
                    result.setComplEnd(value.toDouble());
                } else {
                    value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END_TH");
                    if (!value.isEmpty()) {
                        result.setComplEnd(value.toDouble());
                    }
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_PRODUCT_SIZE");
                if (!value.isEmpty()) {
                    result.setProductSize(value.toInt());
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_PRODUCT_TM");
                if (!value.isEmpty()) {
                    result.setProductTm(value.toDouble());
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_PENALTY");
                if (!value.isEmpty()) {
                    result.setProductQuality(value.toDouble());
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_LIBRARY_MISPRIMING");
                if (!value.isEmpty()) {
                    auto mispriming = value.split(", ");
                    result.setRepeatSim(mispriming.first().toDouble());
                    result.setRepeatSimName(mispriming.last());
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY_STUCT");
                if (!value.isEmpty()) {
                    result.setComplAnyStruct(value);
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END_STUCT");
                if (!value.isEmpty()) {
                    result.setComplEndStruct(value);
                }
            }
            expectedBestPairs << result;
        }
    }

    int sequenceLength = settings->getSequence().size();
    if (settings->getSequenceRange().isEmpty()) {
        settings->setSequenceRange(U2Region(0, sequenceLength));
    }
    if (settings->getTask() == check_primers) {
        settings->setIntProperty("PRIMER_PICK_LEFT_PRIMER", 0);
        settings->setIntProperty("PRIMER_PICK_RIGHT_PRIMER", 0);
        settings->setIntProperty("PRIMER_PICK_INTERNAL_OLIGO", 0);
    }
    if (!settings->getLeftInput().isEmpty()) {
        settings->setIntProperty("PRIMER_PICK_LEFT_PRIMER", 1);
    }
    if (!settings->getRightInput().isEmpty()) {
        settings->setIntProperty("PRIMER_PICK_RIGHT_PRIMER", 1);
    }
    if (!settings->getInternalInput().isEmpty()) {
        settings->setIntProperty("PRIMER_PICK_INTERNAL_OLIGO", 1);
    }
    if (settings->getPrimerSettings()->p_args.min_quality != 0 && settings->getPrimerSettings()->p_args.min_end_quality < settings->getPrimerSettings()->p_args.min_quality) {
        settings->getPrimerSettings()->p_args.min_end_quality = settings->getPrimerSettings()->p_args.min_quality;
    }
}

void GTest_Primer3::prepare() {
    if (!localErrorMessage.isEmpty()) {
        return;
    }
    if (qualityNumber != 0 && qualityNumber != settings->getSequence().size()) {
        localErrorMessage = GTest::tr("Error in sequence quality data");
        return;
    }
    if ((settings->getPrimerSettings()->p_args.min_quality != 0 || settings->getPrimerSettings()->o_args.min_quality != 0) && qualityNumber == 0) {
        localErrorMessage = GTest::tr("Sequence quality data missing");
        return;
    }
    task = new Primer3SWTask(settings);
    addSubTask(task);
}

Task::ReportResult GTest_Primer3::report() {
    QString currentErrorMessage = (task != nullptr ? task->getError() : localErrorMessage).replace("&lt;", "<");
    if (!expectedErrorMessage.isEmpty()) {
        if (currentErrorMessage.isEmpty()) {
            stateInfo.setError(GTest::tr("No error, but expected: %1").arg(expectedErrorMessage));
        } else {
            if (!currentErrorMessage.contains(expectedErrorMessage)) {
                stateInfo.setError(GTest::tr("An unexpected error. Expected: %1, but Actual: %2").arg(expectedErrorMessage).arg(currentErrorMessage));
            }
        }

        return ReportResult_Finished;
    }

    if (!expectedWarningMessage.isEmpty()) {
        if (!task->hasWarning()) {
            stateInfo.setError(GTest::tr("No warning, but expected: %1").arg(expectedWarningMessage));
        } else {
            auto currentWarningMessage = task->getWarnings().first().replace("&lt;", "<").replace("&gt;", ">");
            if (!currentWarningMessage.contains(expectedWarningMessage)) {
                stateInfo.setError(GTest::tr("An unexpected warning. Expected: %1, but Actual: %2").arg(expectedWarningMessage).arg(currentWarningMessage));
            }
        }
    }

    if (task->hasError() && (expectedBestPairs.size() > 0 || expectedSinglePrimers.size() > 0)) {
        stateInfo.setError(task->getError());
        return ReportResult_Finished;
    }

    QList<PrimerPair> currentBestPairs = task->getBestPairs();
    if (currentBestPairs.size() != expectedBestPairs.size()) {
        stateInfo.setError(GTest::tr("PRIMER_PAIR_NUM_RETURNED is incorrect. Expected:%1, but Actual:%2").arg(expectedBestPairs.size()).arg(currentBestPairs.size()));
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

    QList<PrimerSingle> currentSinglePrimers = task->getSinglePrimers();
    if (currentSinglePrimers.size() != expectedSinglePrimers.size()) {
        stateInfo.setError(GTest::tr("Incorrect single primers num. Expected:%1, but Actual:%2").arg(expectedSinglePrimers.size()).arg(currentSinglePrimers.size()));
        return ReportResult_Finished;
    }

    if (expectedSinglePrimers.size() > 0 && expectedBestPairs.size() == 0) {
        int leftCount = 0;
        int rightCount = 0;
        for (int i = 0; i < expectedSinglePrimers.size(); i++) {
            const auto& expectedrimer = expectedSinglePrimers.value(i);
            const auto& currentPrimer = currentSinglePrimers.value(i);
            if (expectedrimer.getType() != currentPrimer.getType()) {
                stateInfo.setError(GTest::tr("Incorrect single primer type, num: %1. Expected:%2, but Actual:%3").arg(i).arg(expectedrimer.getType()).arg(currentPrimer.getType()));
                return ReportResult_Finished;
            }

            QString suffix = "PRIMER_";
            bool internalOligo = false;
            switch (expectedrimer.getType()) {
                case OT_LEFT:
                    suffix += "LEFT_" + QString::number(i);
                    leftCount++;
                    break;
                case OT_RIGHT:
                    suffix += "RIGHT_" + QString::number(i - leftCount);
                    rightCount++;
                    break;
                case OT_INTL:
                    suffix += "INTERNAL_" + QString::number(i - leftCount - rightCount);
                    internalOligo = true;
                    break;
            }
            if (!checkPrimer(&currentPrimer, &expectedrimer, suffix, internalOligo)) {
                return ReportResult_Finished;
            }
        }
    }

    return ReportResult_Finished;
}

GTest_Primer3::~GTest_Primer3() {
    delete settings;
}

bool GTest_Primer3::readPrimer(QDomElement element, QString prefix, PrimerSingle* outPrimer, bool internalOligo) {
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
    {
        QString buf = element.attribute(prefix + "_LIBRARY_" + (internalOligo ? "MISHYB" : "MISPRIMING"));
        if (!buf.isEmpty()) {
            auto mispriming = buf.split(", ");
            outPrimer->setRepeatSim(mispriming.first().toDouble());
            mispriming.removeFirst();
            outPrimer->setRepeatSimName(mispriming.join(", "));
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_ANY_STUCT");
        if (!buf.isEmpty()) {
            outPrimer->setSelfAnyStruct(buf);
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_END_STUCT");
        if (!buf.isEmpty()) {
            outPrimer->setSelfEndStruct(buf);
        }
    }

    if (!internalOligo) {
        {
            QString buf = element.attribute(prefix + "_END_STABILITY");
            if (!buf.isEmpty()) {
                outPrimer->setEndStability(buf.toDouble());
            }
        }
    }
    return true;
}

bool GTest_Primer3::checkPrimerPair(const PrimerPair& primerPair, const PrimerPair& expectedPrimerPair, QString suffix) {
    if (!checkPrimer(primerPair.getLeftPrimer(), expectedPrimerPair.getLeftPrimer(), "PRIMER_LEFT" + suffix, false)) {
        return false;
    }
    if (!checkPrimer(primerPair.getRightPrimer(), expectedPrimerPair.getRightPrimer(), "PRIMER_RIGHT" + suffix, false)) {
        return false;
    }
    if (!checkPrimer(primerPair.getInternalOligo(), expectedPrimerPair.getInternalOligo(), "PRIMER_INTERNAL" + suffix, true)) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getComplAny(), expectedPrimerPair.getComplAny(), "PRIMER_PAIR" + suffix + "_COMPL_ANY")) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getComplEnd(), expectedPrimerPair.getComplEnd(), "PRIMER_PAIR" + suffix + "_COMPL_END")) {
        return false;
    }
    if (!checkIntProperty(primerPair.getProductSize() + settings->getOverhangLeft().size() + settings->getOverhangRight().size(), expectedPrimerPair.getProductSize(), "PRIMER_PAIR" + suffix + "_PRODUCT_SIZE")) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getProductQuality(), expectedPrimerPair.getProductQuality(), "PRIMER_PAIR" + suffix + "_PENALTY")) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getProductTm(), expectedPrimerPair.getProductTm(), "PRIMER_PAIR" + suffix + "_PRODUCT_TM")) {
        return false;
    }
    if (!checkDoubleProperty(primerPair.getRepeatSim(), expectedPrimerPair.getRepeatSim(), "PRIMER_PAIR" + suffix + "_LIBRARY_MISPRIMING")) {
        return false;
    }
    if (primerPair.getRepeatSimName() != expectedPrimerPair.getRepeatSimName()) {
        stateInfo.setError(GTest::tr("PRIMER_PAIR%1_LIBRARY_MISPRIMING_NAME name is incorrect. Expected:%2, but Actual:%3").arg(suffix).arg(expectedPrimerPair.getRepeatSimName()).arg(primerPair.getRepeatSimName()));
        return false;
    }
    if (primerPair.getComplAnyStruct() != expectedPrimerPair.getComplAnyStruct()) {
        stateInfo.setError(GTest::tr("PRIMER_PAIR%1_COMPL_ANY_STUCT name is incorrect. Expected:%2, but Actual:%3").arg(suffix).arg(expectedPrimerPair.getComplAnyStruct()).arg(primerPair.getComplAnyStruct()));
        return false;
    }
    if (primerPair.getComplEndStruct() != expectedPrimerPair.getComplEndStruct()) {
        stateInfo.setError(GTest::tr("PRIMER_PAIR%1_COMPL_END_STUCT name is incorrect. Expected:%2, but Actual:%3").arg(suffix).arg(expectedPrimerPair.getComplEndStruct()).arg(primerPair.getComplEndStruct()));
        return false;
    }

    return true;
}

bool GTest_Primer3::checkPrimer(const PrimerSingle* primer, const PrimerSingle* expectedPrimer, QString prefix, bool internalOligo) {
    if (primer == nullptr) {
        if (expectedPrimer == nullptr) {
            return true;
        } else {
            stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2,%3, but Actual:NULL")
                                   .arg(prefix)
                                   .arg(expectedPrimer->getStart())
                                   .arg(expectedPrimer->getLength()));
            return false;
        }
    }
    if (expectedPrimer == nullptr) {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:NULL, but Actual:%2,%3")
                               .arg(prefix)
                               .arg(primer->getStart())
                               .arg(primer->getLength()));
        return false;
    }
    {
        if ((primer->getStart() + settings->getFirstBaseIndex() != expectedPrimer->getStart()) ||
            (primer->getLength() != expectedPrimer->getLength())) {
            stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2,%3, but Actual:%4,%5")
                                   .arg(prefix)
                                   .arg(expectedPrimer->getStart() + settings->getFirstBaseIndex())
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
    if (!checkDoubleProperty(primer->getRepeatSim(), expectedPrimer->getRepeatSim(), prefix + "_LIBRARY_MISPRIMING")) {
        return false;
    }
    if (primer->getRepeatSimName() != expectedPrimer->getRepeatSimName()) {
        stateInfo.setError(GTest::tr("%1_LIBRARY_MISPRIMING_NAME name is incorrect. Expected:%2, but Actual:%3").arg(prefix).arg(expectedPrimer->getRepeatSimName()).arg(primer->getRepeatSimName()));
        return false;
    }
    if (primer->getSelfAnyStruct() != expectedPrimer->getSelfAnyStruct()) {
        stateInfo.setError(GTest::tr("%1_SELF_ANY_STUCT name is incorrect. Expected:%2, but Actual:%3").arg(prefix).arg(expectedPrimer->getSelfAnyStruct()).arg(primer->getSelfAnyStruct()));
        return false;
    }
    if (primer->getSelfEndStruct() != expectedPrimer->getSelfEndStruct()) {
        stateInfo.setError(GTest::tr("%1_SELF_END_STUCT name is incorrect. Expected:%2, but Actual:%3").arg(prefix).arg(expectedPrimer->getSelfEndStruct()).arg(primer->getSelfEndStruct()));
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
    if (qAbs(value - expectedValue) > qMax(qAbs(value / 1000), 0.005)) {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2, but Actual:%3").arg(name).arg(expectedValue).arg(value));
        return false;
    }
    return true;
}

}  // namespace U2
