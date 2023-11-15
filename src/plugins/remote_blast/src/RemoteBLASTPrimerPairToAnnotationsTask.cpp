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
#include <U2Core/DNASequenceUtils.h>

#include <U2View/ADVSequenceObjectContext.h>

#include <QPointer>

namespace U2 {

static const QStringList COMMON_QUALIFIERS = {"id", "def", "accession", "hit_len"};

RemoteBLASTPrimerPairToAnnotationsTask::RemoteBLASTPrimerPairToAnnotationsTask(const QString& _pairName,
                                                                               const QPointer<ADVSequenceObjectContext>& _ctx,
                                                                               const SharedAnnotationData& _leftPrimer,
                                                                               const SharedAnnotationData& _rightPrimer,
                                                                               const RemoteBLASTTaskSettings& _cfg,
                                                                               const QString& _groupPath)
    : Task(tr("BLAST primer pair \"%1\"").arg(_pairName), TaskFlags_NR_FOSE_COSC),
      pairName(_pairName), ctx(_ctx), leftPrimer(_leftPrimer), rightPrimer(_rightPrimer), cfg(_cfg), groupPath(_groupPath) {
    // We should not run more than one BLAST request per time,
    // because multiple requests looks very suspicious and BLAST could block them all
    setMaxParallelSubtasks(1);
}

void RemoteBLASTPrimerPairToAnnotationsTask::prepare() {
    CHECK_EXT(leftPrimer.data() != nullptr, setError(tr("The left primer is lost, probably, annotation object has been closed")), );

    leftPrimerBlastTask = getBlastTaskForAnnotationRegion(leftPrimer);
    CHECK_OP(stateInfo, );

    addSubTask(leftPrimerBlastTask);
    CHECK_EXT(rightPrimer.data() != nullptr, setError(tr("The right primer is lost, probably, annotation object has been closed")), );

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
                } else {
                    if (qual.name == "hit-from") {
                        leftFrom = qual.value.toInt();
                        // The right primer is on reverse-complementary strand, this is why, I suppose,
                        // "hit-from" is higher than "hit-to" - we are considering the strand from right to left.
                        // But it would be more convenient to have region in usual genbank formats here.
                        rightTo = rightRes->findFirstQualifierValue(qual.name).toInt();
                    } else if (qual.name == "hit-to") {
                        leftTo = qual.value.toInt();
                        rightFrom = rightRes->findFirstQualifierValue(qual.name).toInt();
                    } else {
                        U2Qualifier commonQual(qual.name,
                                               qual.value + " | " + rightRes->findFirstQualifierValue(qual.name));
                        annotationData->qualifiers << commonQual;
                    }
                }
            }
            // The left primer should be on the left of the right primer
            CHECK_CONTINUE(leftTo < rightFrom);

            annotationData->qualifiers << U2Qualifier("left-reg", QString("%1..%2").arg(leftFrom).arg(leftTo));
            annotationData->qualifiers << U2Qualifier("right-reg", QString("%1..%2").arg(rightFrom).arg(rightTo));

            resultAnnotations << annotationData;
        }
    }

    CHECK_EXT(!resultAnnotations.isEmpty(), stateInfo.addWarning(tr("No BLAST pairs have been found for  \"%1\"").arg(pairName)), {});
    CHECK_EXT(!ctx.isNull(), setError(tr("Sequence is not found, it has, probably, been closed")), {});

    auto annotationTableObjects = ctx->getAnnotationObjects();
    AnnotationTableObject* annotationTableObject = nullptr;
    for (auto ato : qAsConst(annotationTableObjects)) {
        auto currentAtoAnnotations = ato->getAnnotations();
        bool leftFound = false;
        bool rightFound = false;
        for (auto ann : qAsConst(currentAtoAnnotations)) {
            if (*ann->getData() == *leftPrimer) {
                leftFound = true;
            } else if (*ann->getData() == *rightPrimer) {
                rightFound = true;
            }
            CHECK_BREAK(!leftFound || !rightFound);
        }
        CHECK_CONTINUE(leftFound && rightFound);

        annotationTableObject = ato;
        break;
    }
    CHECK_EXT(annotationTableObject != nullptr,
              setError(tr("Annotation Table with left and right primer selected has not been found, probably, it has been removed")), {});

    QMap<QString, QList<SharedAnnotationData>> resultAnnotationsMap;
    resultAnnotationsMap.insert(groupPath, resultAnnotations);
    createAnnotationsTask = new CreateAnnotationsTask(annotationTableObject, resultAnnotationsMap);
    return {createAnnotationsTask};
}

RemoteBLASTTask* RemoteBLASTPrimerPairToAnnotationsTask::getBlastTaskForAnnotationRegion(const SharedAnnotationData& ann) {
    const auto& regions = ann->getRegions();
    int regionsSize = regions.size();
    RemoteBLASTTaskSettings newCfg(cfg);
    // One region is a regular primer, two regions - if the primer is located on junction point
    if (regionsSize == 1) {
        newCfg.query = ctx->getSequenceData(regions.first(), stateInfo);
        CHECK_OP(stateInfo, nullptr);
    } else if (regionsSize == 2) {
        newCfg.query = ctx->getSequenceData(regions.last(), stateInfo);
        CHECK_OP(stateInfo, nullptr);

        newCfg.query += ctx->getSequenceData(regions.first(), stateInfo);
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


}  // namespace U2