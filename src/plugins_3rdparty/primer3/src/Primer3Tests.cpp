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

#include "Primer3Tests.h"

#include "task/Primer3TaskSettings.h"
#include "task/Primer3TopLevelTask.h"
#include "task/PrimerPair.h"
#include "task/PrimerSingle.h"

#include <U2Core/AppContext.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>

#include <QFile>
#include <QtGlobal>

namespace U2 {

GTest_Primer3::~GTest_Primer3() {
    if (annotationTableId.isEmpty()) {
        delete annotationTableObject;
    }
}

void GTest_Primer3::init(XMLTestFormat*, const QDomElement& el) {
    seqObjCtx = el.attribute("sequence");
    annotationTableId = el.attribute("annotation");
    circular = el.attribute("circular") == "true";

    settings = QSharedPointer<Primer3TaskSettings>(new Primer3TaskSettings);
    settings->getPrimerSettings()->first_base_index = 0;  // Default mode for XML tests.
    auto buf = el.attribute("range");
    if (!buf.isEmpty()) {
        auto rangeSplit = buf.split(",");
        int start = rangeSplit.first().toInt();
        int length = rangeSplit.last().toInt();
        settings->setSequenceRange(U2Region(start, length));
    }

    CheckComplementSettings ccs;
    ccs.enabled = el.attribute("enable-check-complement-settings").toInt() == 1;
    if (ccs.enabled) {
        auto getIntValue = [&el](const QString& attributeName, int defaultAttributeValue) -> int {
            bool ok = false;
            int value = el.attribute(attributeName).toInt(&ok);
            int result = defaultAttributeValue;
            if (ok) {
                result = value;
            }
            return result;
        };
        ccs.enableMaxComplementPairs = el.attribute("enable-max-complement-pairs").toInt() == 1;
        ccs.maxComplementPairs = getIntValue("max-complement-pairs", ccs.maxComplementPairs);
        ccs.enableMaxGcContent = el.attribute("enable-max-gc-content").toInt() == 1;
        ccs.maxGcContent = getIntValue("max-gc-content", ccs.maxGcContent);
        if (el.attribute("save-csv-report").toInt() == 1) {
            auto tmpDataDir = env->getVar("TEMP_DATA_DIR") + "/";
            csvReportTmpFile = QSharedPointer<QTemporaryFile>(new QTemporaryFile(env->getVar("TEMP_DATA_DIR") + "/XXXXXX.csv"));
            CHECK_EXT(csvReportTmpFile->open(), L10N::errorWritingFile(csvReportTmpFile->fileName()), );

            ccs.csvReportPath = csvReportTmpFile->fileName();
        }
        settings->setCheckComplementSettings(ccs);
    }

    auto reportPath = el.attribute("report-path");
    if (!reportPath.isEmpty()) {
        reportPath = env->getVar("COMMON_DATA_DIR") + "/" + reportPath;
    }

    QDomNodeList inputParameters = el.elementsByTagName("plugin_primer_3_in");
    for (int inputParametersIndex = 0; inputParametersIndex < inputParameters.size(); inputParametersIndex++) {
        QDomNode n = inputParameters.item(inputParametersIndex);
        SAFE_POINT_EXT(n.isElement(), localErrorMessage = "QDomNode isn't element", );

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

        // 3
        buf = elInput.attribute("SEQUENCE_TARGET");
        if (!buf.isEmpty()) {
            QList<U2Region> regionList;
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            auto qwe = buf.split(';', Qt::SkipEmptyParts);
            for (const QString& str : buf.split(';', Qt::SkipEmptyParts)) {
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
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            for (const QString& str : buf.split(' ', Qt::SkipEmptyParts)) {
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
            QStringList qualityList = buf.split(' ', Qt::SkipEmptyParts);
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
        SAFE_POINT_EXT(n.isElement(), localErrorMessage = "QDomNode isn't element", );

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

        QList<QSharedPointer<PrimerSingle>> leftPrimers;
        for (int i = 0; i < leftCount; i++) {
            QSharedPointer<PrimerSingle> primer(new PrimerSingle);;
            if (readPrimer(elOutput, "PRIMER_LEFT_" + QString::number(i), primer, false)) {
                leftPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_LEFT_" + QString::number(i)));
                return;
            }
        }
        QList<QSharedPointer<PrimerSingle>> rightPrimers;
        for (int i = 0; i < rightCount; i++) {
            QSharedPointer<PrimerSingle> primer(new PrimerSingle(OT_RIGHT));
            if (readPrimer(elOutput, "PRIMER_RIGHT_" + QString::number(i), primer, false)) {
                rightPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_RIGHT_" + QString::number(i)));
                return;
            }
        }
        QList<QSharedPointer<PrimerSingle>> internalPrimers;
        for (int i = 0; i < internalCount; i++) {
            QSharedPointer<PrimerSingle> primer(new PrimerSingle(OT_INTL));
            if (readPrimer(elOutput, "PRIMER_INTERNAL_" + QString::number(i), primer, true)) {
                internalPrimers << primer;
                expectedSinglePrimers << primer;
            } else {
                stateInfo.setError(GTest::tr("Incorrect parameter: %1").arg("PRIMER_INTERNAL_" + QString::number(i)));
                return;
            }
        }

        for (int pairIndex = 0; pairIndex < pairsCount; pairIndex++) {
            QSharedPointer<PrimerPair> result(new PrimerPair);
            result->leftPrimer = leftPrimers[pairIndex];
            result->rightPrimer = rightPrimers[pairIndex];
            if (internalCount > 0) {
                result->internalOligo = internalPrimers[pairIndex];
            }
            auto suffix = "_" + QString::number(pairIndex);
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY");
                if (!value.isEmpty()) {
                    result->complAny = value.toDouble();
                } else {
                    value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY_TH");
                    if (!value.isEmpty()) {
                        result->complAny = value.toDouble();
                    }
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END");
                if (!value.isEmpty()) {
                    result->complEnd = value.toDouble();
                } else {
                    value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END_TH");
                    if (!value.isEmpty()) {
                        result->complEnd = value.toDouble();
                    }
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_PRODUCT_SIZE");
                if (!value.isEmpty()) {
                    result->productSize = value.toInt();
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_PRODUCT_TM");
                if (!value.isEmpty()) {
                    result->tm = value.toDouble();
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_PENALTY");
                if (!value.isEmpty()) {
                    result->quality = value.toDouble();
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_LIBRARY_MISPRIMING");
                if (!value.isEmpty()) {
                    auto mispriming = value.split(", ");
                    result->repeatSim = mispriming.first().toDouble();
                    result->repeatSimName = mispriming.last();
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY_STUCT");
                if (!value.isEmpty()) {
                    result->complAnyStruct = value;
                }
            }
            {
                QString value = elOutput.attribute("PRIMER_PAIR" + suffix + "_COMPL_END_STUCT");
                if (!value.isEmpty()) {
                    result->complEndStruct = value;
                }
            }
            expectedBestPairs << result;
        }
    }

    if (settings->getTask() == check_primers) {
        settings->setIntProperty("PRIMER_PICK_LEFT_PRIMER", !settings->getLeftInput().isEmpty());
        settings->setIntProperty("PRIMER_PICK_RIGHT_PRIMER", !settings->getRightInput().isEmpty());
        settings->setIntProperty("PRIMER_PICK_INTERNAL_OLIGO", !settings->getInternalInput().isEmpty());
    }
    if (settings->getPrimerSettings()->p_args.min_quality != 0 && settings->getPrimerSettings()->p_args.min_end_quality < settings->getPrimerSettings()->p_args.min_quality) {
        settings->getPrimerSettings()->p_args.min_end_quality = settings->getPrimerSettings()->p_args.min_quality;
    }
}

void GTest_Primer3::prepare() {
    if (!seqObjCtx.isEmpty()) {
        seqObj = getContext<U2SequenceObject>(this, seqObjCtx);
        CHECK_EXT(seqObj != nullptr, setError(QString("Sequence context not found %1").arg(seqObjCtx)), );

        QByteArray seqData = seqObj->getWholeSequenceData(stateInfo);
        CHECK_OP(stateInfo, );

        settings->setSequenceName(seqObj->getSequenceName().toLocal8Bit());
        settings->setSequence(seqData, circular);
        seqObj->setCircular(circular);
    }
    int sequenceLength = settings->getSequence().size();
    if (settings->getSequenceRange().isEmpty()) {
        settings->setSequenceRange(U2Region(0, sequenceLength));
    }

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

    if (seqObj != nullptr) {
        auto dbiReg = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
        CHECK_OP(stateInfo, );

        if (!annotationTableId.isEmpty()) {
            annotationTableObject = getContext<AnnotationTableObject>(this, annotationTableId);
        } else {
            annotationTableObject = new AnnotationTableObject(seqObj->getSequenceName(), dbiReg);
            annotationTableObject->addObjectRelation(seqObj, ObjectRole_Sequence);
        }

        task = new Primer3TopLevelTask(settings, seqObj, annotationTableObject, "top_primers", "top_primers", "");
    } else {
        auto newFilePath = env->getVar("TEMP_DATA_DIR") + "/check_primers.gb";
        task = new Primer3TopLevelTask(settings, newFilePath, false);
    }
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

    if (!reportPath.isEmpty()) {
        QString currentReport;
        if (!csvReportTmpFile.isNull()) {
            QFile csvReportFile(csvReportTmpFile->fileName());
            CHECK_EXT(csvReportFile.open(QIODevice::ReadOnly), setError(L10N::errorReadingFile(csvReportTmpFile->fileName())), Task::ReportResult::ReportResult_Finished);

            currentReport = csvReportFile.readAll();
            csvReportFile.close();
        } else {
            currentReport = task->generateReport();
        }
        QFile reportFile(reportPath);
        CHECK_EXT(reportFile.open(QIODevice::ReadOnly), setError(L10N::errorReadingFile(reportPath)), Task::ReportResult::ReportResult_Finished);

        QString expectedReport = reportFile.readAll();
        CHECK_EXT(currentReport == expectedReport, setError("Expected and result reports are not equal"), Task::ReportResult::ReportResult_Finished);
    }

    if (task->getAnnotationTableObject() == nullptr) {
        CHECK_EXT(expectedBestPairs.isEmpty() && expectedSinglePrimers.isEmpty(), setError("No primers, but expected"), ReportResult_Finished);
        return ReportResult_Finished;
    }

    AnnotationGroup* rootGroup = task->getAnnotationTableObject()->getRootGroup();
    const auto& topPrimersGroups = rootGroup->getSubgroups();
    if (topPrimersGroups.isEmpty()) {
        CHECK_EXT(expectedBestPairs.isEmpty() && expectedSinglePrimers.isEmpty(), setError("No primers, but expected"), ReportResult_Finished);
        return ReportResult_Finished;
    }

    auto topPrimersGroup = std::find_if(topPrimersGroups.begin(), topPrimersGroups.end(), [](AnnotationGroup* group) {
        return group->getName() == "top_primers";
    });
    CHECK_EXT(topPrimersGroup != topPrimersGroups.end(), setError("top_primers group is not found"), ReportResult_Finished);

    if (!expectedBestPairs.isEmpty()) {
        const auto& primerGroups = (*topPrimersGroup)->getSubgroups();
        CHECK_EXT(primerGroups.size() == expectedBestPairs.size(), setError("Unexpected primer pairs size"), ReportResult_Finished);

        for (int i = 0; i < primerGroups.size(); i++) {
            auto annPrimerGroup = primerGroups[i];
            const auto& annotations = annPrimerGroup->getAnnotations();
            auto expectedPair = expectedBestPairs[i];
            if (!expectedPair->leftPrimer.isNull()) {
                CHECK(comparePrimerSingleFromPairAndAnnotation(expectedPair, annotations, oligo_type::OT_LEFT), ReportResult_Finished);
            }

            if (!expectedPair->rightPrimer.isNull()) {
                CHECK(comparePrimerSingleFromPairAndAnnotation(expectedPair, annotations, oligo_type::OT_RIGHT), ReportResult_Finished);
            }

            if (!expectedPair->internalOligo.isNull()) {
                CHECK(comparePrimerSingleFromPairAndAnnotation(expectedPair, annotations, oligo_type::OT_INTL), ReportResult_Finished);
            }
        }
    } else if (!expectedSinglePrimers.isEmpty()) {
        const auto& annotations = (*topPrimersGroup)->getAnnotations();
        CHECK_EXT(annotations.size() == expectedSinglePrimers.size(), setError("Unexpected single primers size"), ReportResult_Finished);

        for (int i = 0; i < annotations.size(); i++) {
            auto annotation = annotations[i];
            const auto& primer = expectedSinglePrimers[i];

            CHECK(comparePrimerSingleAndAnnotation(primer, annotation, "Single #" + QString::number(i + 1)), ReportResult_Finished);
        }
    } else {
        setError("Not expected primers, but annotation has been created");
    }

    return ReportResult_Finished;
}

bool GTest_Primer3::readPrimer(QDomElement element, QString prefix, QSharedPointer<PrimerSingle> outPrimer, bool internalOligo) {
    {
        QString buf = element.attribute(prefix);
        if (!buf.isEmpty()) {
            int start = buf.split(',')[0].toInt();
            int length = buf.split(',')[1].toInt();
            outPrimer->start = start;
            outPrimer->length = length;
            if (prefix.contains("RIGHT")) {
                outPrimer->start = start - length + 1;
            }
            outPrimer->start = outPrimer->start - settings->getPrimerSettings()->first_base_index;
        } else {
            return false;
        }
    }
    {
        QString buf = element.attribute(prefix + "_TM");
        if (!buf.isEmpty()) {
            outPrimer->meltingTemperature = buf.toDouble();
        }
    }
    {
        QString buf = element.attribute(prefix + "_GC_PERCENT");
        if (!buf.isEmpty()) {
            outPrimer->gcContent = buf.toDouble();
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_ANY");
        if (!buf.isEmpty()) {
            outPrimer->selfAny = buf.toDouble();
        } else {
            buf = element.attribute(prefix + "_SELF_ANY_TH");
            if (!buf.isEmpty()) {
                outPrimer->selfAny = buf.toDouble();
            }
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_END");
        if (!buf.isEmpty()) {
            outPrimer->selfEnd = buf.toDouble();
        } else {
            buf = element.attribute(prefix + "_SELF_END_TH");
            if (!buf.isEmpty()) {
                outPrimer->selfEnd = buf.toDouble();
            }
        }
    }
    {
        QString buf = element.attribute(prefix + "_TEMPLATE_MISPRIMING");
        if (!buf.isEmpty()) {
            outPrimer->templateMispriming = buf.toDouble();
        } else {
            buf = element.attribute(prefix + "_TEMPLATE_MISPRIMING_TH");
            if (!buf.isEmpty()) {
                outPrimer->templateMispriming = buf.toDouble();
            }
        }
    }
    {
        QString buf = element.attribute(prefix + "_HAIRPIN_TH");
        if (!buf.isEmpty()) {
            outPrimer->hairpin = buf.toDouble();
        }
    }
    {
        QString buf = element.attribute(prefix + "_PENALTY");
        if (!buf.isEmpty()) {
            outPrimer->quality = buf.toDouble();
        }
    }
    {
        QString buf = element.attribute(prefix + "_BOUND");
        if (!buf.isEmpty()) {
            outPrimer->bound = buf.toDouble();
        }
    }
    {
        QString buf = element.attribute(prefix + "_LIBRARY_" + (internalOligo ? "MISHYB" : "MISPRIMING"));
        if (!buf.isEmpty()) {
            auto mispriming = buf.split(", ");
            outPrimer->repeatSim = mispriming.first().toDouble();
            mispriming.removeFirst();
            outPrimer->repeatSimName = mispriming.join(", ");
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_ANY_STUCT");
        if (!buf.isEmpty()) {
            outPrimer->selfAnyStruct = buf;
        }
    }
    {
        QString buf = element.attribute(prefix + "_SELF_END_STUCT");
        if (!buf.isEmpty()) {
            outPrimer->selfEndStruct = buf;
        }
    }


    if (!internalOligo) {
        {
            QString buf = element.attribute(prefix + "_END_STABILITY");
            if (!buf.isEmpty()) {
                outPrimer->endStability = buf.toDouble();
            }
        }
    }
    return true;
}

bool GTest_Primer3::comparePrimerSingleFromPairAndAnnotation(const QSharedPointer<PrimerPair>& pair, const QList<Annotation*>& annotations, oligo_type type) {
    QSharedPointer<PrimerSingle> primer;
    Annotation* annotation = nullptr;
    QString primerText;
    switch (type) {
    case oligo_type::OT_LEFT:
        primer = pair->leftPrimer;
        for (auto ann : qAsConst(annotations)) {
            CHECK_CONTINUE(ann->getName() == "top_primers" && ann->getStrand() == U2Strand::Direct);

            annotation = ann;
            break;
        }
        annotation = annotations.first();
        primerText = "Left";
        break;
    case oligo_type::OT_RIGHT:
        primer = pair->rightPrimer;
        for (auto ann : qAsConst(annotations)) {
            CHECK_CONTINUE(ann->getName() == "top_primers" && ann->getStrand() == U2Strand::Complementary);

            annotation = ann;
            break;
        }
        primerText = "Right";
        break;
    case oligo_type::OT_INTL:
        primer = pair->internalOligo;
        for (auto ann : qAsConst(annotations)) {
            CHECK_CONTINUE(ann->getName() == "internalOligo");

            annotation = ann;
            break;
        }
        primerText = "Internal";
        break;
    }

    return comparePrimerSingleAndAnnotation(primer, annotation, primerText, pair->productSize);
}

bool GTest_Primer3::comparePrimerSingleAndAnnotation(const QSharedPointer<PrimerSingle>& primer, Annotation* annotation, const QString& primerText, int productSize) {
    CHECK_EXT(!primer.isNull(), setError(QString("%1 primer is missed").arg(primerText)), false);
    CHECK_EXT(annotation != nullptr, setError(QString("%1 annotation is missed").arg(primerText)), false);

    int start = primer->start;
    int length = primer->length;
    auto regs = annotation->getRegions();
    if (regs.size() == 2) {
        CHECK_EXT(settings->isSequenceCircular(), setError("Sequence is not circular, but should be"), false);

        auto firstRegion = regs.first();
        int regStart = firstRegion.startPos;
        int regLength = firstRegion.length + regs.last().length;
        CHECK_EXT(regStart == start && regLength == length, setError(QString("%1 primer on junction point has incorrect region").arg(primerText)), false);
    } else if (regs.size() == 1) {
        auto region = regs.first();
        CHECK_EXT(region.startPos == start && region.length == length, setError(QString("%1 primer has incorrect region").arg(primerText)), false);
    } else {
        setError("Unexpected region size");
        return false;
    }
    const auto qualifiers = annotation->getQualifiers();
    for (const auto qual : qAsConst(qualifiers)) {
        if (qual.name == "3'") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->endStability, "3'"), false);
        } else if (qual.name == "any") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->selfAny, "any"), false);
        } else if (qual.name == "end") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->selfEnd, "end"), false);
        } else if (qual.name == "gc%") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->gcContent, "gc%"), false);
        } else if (qual.name == "penalty") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->quality, "penalty"), false);
        } else if (qual.name == "product_size") {
            CHECK(checkIntProperty(qual.value.toInt(), productSize, "product_size"), false);
        } else if (qual.name == "tm") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->meltingTemperature, "tm"), false);
        } else if (qual.name == "bound%") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->bound, "bound%"), false);
        } else if (qual.name == "template_mispriming") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->templateMispriming, "template_mispriming"), false);
        } else if (qual.name == "hairpin") {
            CHECK(checkDoubleProperty(qual.value.toDouble(), primer->hairpin, "hairpin"), false);
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
