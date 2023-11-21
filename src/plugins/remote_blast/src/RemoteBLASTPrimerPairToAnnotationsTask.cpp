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

#include "RemoteBLASTPrimerPairToAnnotationsTask.h"

#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <QPointer>

namespace U2 {

// Values of these qualifiers are similar for both primers
static const QStringList COMMON_QUALIFIERS = {"id", "def", "accession", "hit_len"};

RemoteBLASTPrimerPairToAnnotationsTask::RemoteBLASTPrimerPairToAnnotationsTask(const QString& _pairName,
                                                                               const QPointer<U2SequenceObject>& _seqObj,
                                                                               const QPointer<AnnotationTableObject>& _ato,
                                                                               const SharedAnnotationData& _leftPrimer,
                                                                               const SharedAnnotationData& _rightPrimer,
                                                                               const RemoteBLASTTaskSettings& _cfg,
                                                                               const QString& _groupPath)
    : Task(tr("BLAST primer pair \"%1\"").arg(_pairName), TaskFlags_NR_FOSE_COSC),
    pairName(_pairName), seqObj(_seqObj), ato(_ato), leftPrimer(_leftPrimer), rightPrimer(_rightPrimer), cfg(_cfg), groupPath(_groupPath) {
    // We should not run more than one BLAST request per time,
    // because multiple requests looks very suspicious and BLAST could block them all
    setMaxParallelSubtasks(1);
}

void RemoteBLASTPrimerPairToAnnotationsTask::prepare() {
    // Validate primers
    auto leftDirection = leftPrimer->getStrand().getDirection();
    auto rightDirection = rightPrimer->getStrand().getDirection();
    CHECK_EXT(leftDirection != rightDirection, setError("Left and right primers should be located on different strands"), );

    // Run BLAST for both left and right primers.
    CHECK_EXT(leftPrimer.data() != nullptr, setError(tr("The left primer is lost, probably, annotation object has been removed from the project or the current annotation has been removed from the file")), );

    leftPrimerBlastTask = getBlastTaskForAnnotationRegion(leftPrimer);
    CHECK_OP(stateInfo, );

    addSubTask(leftPrimerBlastTask);
    CHECK_EXT(rightPrimer.data() != nullptr, setError(tr("The right primer is lost, probably, annotation object has been removed from the project or the current annotation has been removed from the file")), );

    rightPrimerBlastTask = getBlastTaskForAnnotationRegion(rightPrimer);
    CHECK_OP(stateInfo, );

    addSubTask(rightPrimerBlastTask);
}

QList<Task*> RemoteBLASTPrimerPairToAnnotationsTask::onSubTaskFinished(Task* subTask) {
    CHECK_OP(stateInfo, {});
    CHECK(leftPrimerBlastTask->isFinished() && rightPrimerBlastTask->isFinished(), {});
    CHECK(subTask != createAnnotationsTask, {});

    auto leftPrimerBlastResults = leftPrimerBlastTask->getResultedAnnotations();
    auto rightPrimerBlastResults = rightPrimerBlastTask->getResultedAnnotations();
    QList<SharedAnnotationData> resultAnnotations;
    for (const auto& leftRes : qAsConst(leftPrimerBlastResults)) {
        auto leftAccession = leftRes->findFirstQualifierValue("accession");
        for (const auto& rightRes : qAsConst(rightPrimerBlastResults)) {
            // Find left and fight primers results from the same sequence.
            auto rightAccession = rightRes->findFirstQualifierValue("accession");
            CHECK_CONTINUE(leftAccession == rightAccession);

            SharedAnnotationData annotationData(new AnnotationData);
            annotationData->name = leftRes->name;
            annotationData->type = leftRes->type;
            annotationData->setStrand(leftRes->getStrand());
            CHECK_EXT(leftPrimer.data() != nullptr, setError(tr("The left primer is lost, probably, annotation object has been closed")), {});

            auto allRegions = leftPrimer->getRegions();
            CHECK_EXT(rightPrimer.data() != nullptr, setError(tr("The right primer is lost, probably, annotation object has been closed")), {});

            allRegions.append(rightPrimer->getRegions());
            annotationData->location->regions = U2Region::join(allRegions);
            annotationData->location->op = U2LocationOperator::U2LocationOperator_Join;

            qint64 leftFrom = 0;
            qint64 leftTo = 0;
            qint64 rightFrom = 0;
            qint64 rightTo = 0;
            for (const auto& qual : qAsConst(leftRes->qualifiers)) {
                if (COMMON_QUALIFIERS.contains(qual.name)) {
                    annotationData->qualifiers << qual;
                } else if (qual.name == "hit-from") {
                    leftFrom = qual.value.toInt();
                    rightFrom = rightRes->findFirstQualifierValue(qual.name).toInt();
                } else if (qual.name == "hit-to") {
                    leftTo = qual.value.toInt();
                    rightTo = rightRes->findFirstQualifierValue(qual.name).toInt();
                } else {
                    U2Qualifier commonQual(qual.name,
                                           qual.value + " | " + rightRes->findFirstQualifierValue(qual.name));
                    annotationData->qualifiers << commonQual;
                }
            }
            // If primer is located on reverse-complementary strand,
            // than "hit-from" is higher than "hit-to" - we are considering the strand from right to left.
            // Also, primers should have they 3' ends points to each other.
            // Than means, we have two possible ways of primer locations:
            //
            //
            //                   Left
            // 1    (hit-from) ----------> (hit-to)                                      1000
            // 1                                    (hit-to) <-------------- (hit-from)  1000
            //                                                   Right
            //
            // OR
            //
            //                  Right
            // 1    (hit-from) ----------> (hit-to)                                      1000
            // 1                                    (hit-to) <-------------- (hit-from)  1000
            //                                                   Left
            //
            // Result region would be more convenient to have in usual genbank formats here.
            if (leftFrom < leftTo && rightFrom > rightTo && leftTo < rightFrom) {
                annotationData->qualifiers << U2Qualifier("left-reg-gb", QString("%1..%2").arg(leftFrom).arg(leftTo));
                annotationData->qualifiers << U2Qualifier("right-reg-gb", QString("%1..%2").arg(rightTo).arg(rightFrom));
            } else if (leftFrom > leftTo && rightFrom < rightTo && leftFrom > rightFrom) {
                annotationData->qualifiers << U2Qualifier("right-reg-gb", QString("%1..%2").arg(leftTo).arg(leftFrom));
                annotationData->qualifiers << U2Qualifier("left-reg-gb", QString("%1..%2").arg(rightFrom).arg(rightTo));
            } else {
                continue;
            }

            resultAnnotations << annotationData;
        }
    }

    CHECK_EXT(!resultAnnotations.isEmpty(), stateInfo.addWarning(tr("No BLAST pairs have been found for \"%1\"").arg(pairName)), {});
    CHECK_EXT(!ato.isNull(), setError(L10N::nullPointerError("AnnotationTableObject")), {});

    // Remove results, which are already presented to avoid duplicity
    removeAlreadyFoundBlastResults(resultAnnotations);
    CHECK_OP(stateInfo, {});
    CHECK_EXT(!resultAnnotations.isEmpty(), stateInfo.addWarning(tr("All found BLAST results already presented for \"%1\"").arg(pairName)), {});

    QMap<QString, QList<SharedAnnotationData>> resultAnnotationsMap;
    resultAnnotationsMap.insert(groupPath, resultAnnotations);
    createAnnotationsTask = new CreateAnnotationsTask(ato, resultAnnotationsMap);
    return {createAnnotationsTask};
}

RemoteBLASTTask* RemoteBLASTPrimerPairToAnnotationsTask::getBlastTaskForAnnotationRegion(const SharedAnnotationData& ann) {
    const auto& regions = ann->getRegions();
    int regionsSize = regions.size();
    RemoteBLASTTaskSettings newCfg(cfg);
    SAFE_POINT_EXT(!seqObj.isNull(), setError(L10N::nullPointerError("U2SequenceObject")), nullptr);

    // One region is a regular primer, two regions - if the primer is located on junction point
    if (regionsSize == 1) {
        newCfg.query = seqObj->getSequenceData(regions.first(), stateInfo);
        CHECK_OP(stateInfo, nullptr);
    } else if (regionsSize == 2) {
        newCfg.query = seqObj->getSequenceData(regions.last(), stateInfo);
        CHECK_OP(stateInfo, nullptr);

        newCfg.query += seqObj->getSequenceData(regions.first(), stateInfo);
        CHECK_OP(stateInfo, nullptr);
    } else {
        setError(tr("Primer \"%1\" has unexpected number of regions (%2) - it should not be more than two. Please, check this primer manually.")
                     .arg(ann->name)
                     .arg(regionsSize));
        return nullptr;
    }
    if (ann == rightPrimer) {
        newCfg.query = DNASequenceUtils::reverseComplement(newCfg.query);
    }

    return new RemoteBLASTTask(newCfg);
}

void RemoteBLASTPrimerPairToAnnotationsTask::removeAlreadyFoundBlastResults(QList<SharedAnnotationData>& resultAnnotations) {
    auto rootGroup = ato->getRootGroup();
    SAFE_POINT_EXT(rootGroup != nullptr, setError("Root group is not found, but should be"), );

    auto primerPairGroup = rootGroup->getSubgroup(groupPath, false);
    SAFE_POINT_EXT(primerPairGroup != nullptr, setError("Primer pair group is not found, but should be"), );

    auto primerGroupAnnotations = primerPairGroup->getAnnotations();
    QList<SharedAnnotationData> alreadyExistedAnnotations;
    for (auto primerGroupAnn : qAsConst(primerGroupAnnotations)) {
        for (auto resultAnn : qAsConst(resultAnnotations)) {
            CHECK_CONTINUE(*primerGroupAnn->getData() == *resultAnn);

            alreadyExistedAnnotations << resultAnn;
        }
    }
    for (auto alreadyExistsData : qAsConst(alreadyExistedAnnotations)) {
        auto foundRes = std::find_if(resultAnnotations.begin(), resultAnnotations.end(), [&alreadyExistsData](const SharedAnnotationData& resData) {
            return *alreadyExistsData == *resData;
        });
        CHECK_CONTINUE(foundRes != resultAnnotations.end());

        resultAnnotations.removeOne(*foundRes);
    }
}


}  // namespace U2
