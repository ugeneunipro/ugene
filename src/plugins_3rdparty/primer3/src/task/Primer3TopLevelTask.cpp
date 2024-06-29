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

#include "Primer3TopLevelTask.h"

#include "CheckComplementTask.h"
#include "FindExonRegionsTask.h"
#include "Primer3Task.h"
#include "Primer3TaskSettings.h"
#include "PrimerPair.h"
#include "ProcessPrimer3ResultsToAnnotationsTask.h"
#include "primer3_core/libprimer3.h"

#include <U2Core/AppContext.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ImportSequenceFromRawDataTask.h>
#include <U2Gui/OpenViewTask.h>

namespace U2 {

static const QString PRIMER_ANNOTATION_NAME = "top_primers";
static const QString PRIMER_GOBJECT_NAME = "check_primers";

Primer3TopLevelTask::Primer3TopLevelTask(const QSharedPointer<Primer3TaskSettings>& _settings,
                                         U2SequenceObject* _seqObj,
                                         AnnotationTableObject* _aobj,
                                         const QString& _groupName,
                                         const QString& _annName,
                                         const QString& _annDescription)
    : Task(tr("Find primers with target sequence task"),
           TaskFlags(TaskFlag_NoRun) |
               TaskFlag_ReportingIsSupported |
               TaskFlag_ReportingIsEnabled |
               TaskFlag_FailOnSubtaskError |
               TaskFlag_FailOnSubtaskCancel |
               TaskFlag_CollectChildrenWarnings),
      settings(_settings), seqObj(_seqObj), annotationTableObject(_aobj),
      groupName(_groupName), annName(_annName), annDescription(_annDescription) {
    GCOUNTER(cvar, "Primer3Task");
}

Primer3TopLevelTask::Primer3TopLevelTask(const QSharedPointer<Primer3TaskSettings>& _settings,
                                         const QString& _resultFilePath,
                                         bool _openView)
    : Task(tr("Find primers without target sequence task"),
           TaskFlags(TaskFlag_NoRun) |
               TaskFlag_ReportingIsSupported |
               TaskFlag_ReportingIsEnabled |
               TaskFlag_FailOnSubtaskError |
               TaskFlag_FailOnSubtaskCancel |
               TaskFlag_CollectChildrenWarnings),
      settings(_settings), resultFilePath(_resultFilePath), openView(_openView),
      groupName(PRIMER_ANNOTATION_NAME), annName(PRIMER_ANNOTATION_NAME) {
    GCOUNTER(cvar, "Primer3Task_noTargetSequence");
}

void Primer3TopLevelTask::prepare() {
    if (settings->getSpanIntronExonBoundarySettings().enabled) {
        SAFE_POINT(!seqObj.isNull(), L10N::nullPointerError("U2SequenceObject"), );

        findExonsTask = new FindExonRegionsTask(seqObj, settings->getSpanIntronExonBoundarySettings().exonAnnotationName);
        addSubTask(findExonsTask);
    } else {
        primer3Task = new Primer3Task(settings);
        addSubTask(primer3Task);
    }
}

QList<Task*> Primer3TopLevelTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);
    CHECK(subTask->isFinished(), res);

    if (subTask == findExonsTask) {
        res << onFindExonTaskFinished();
    } else if (subTask == primer3Task) {
        res << onPrimer3TaskFinished();
    } else if (subTask == checkComplementTask) {
        res << onCheckComplementTaskFinished();
    } else if (subTask == processPrimer3ResultsToAnnotationsTask) {
        res << onProcessPrimer3ResultsToAnnotationsTaskFinished();
    } else if (subTask == importSequenceFromRawDataTask) {
        res << onImportSequenceFromRawDataTaskFinished();
    } else if (subTask == createAnnotationsTask) {
        res << onCreateAnnotationsTaskFinished();
    } else if (subTask == saveDocumentTask) {
        res << onSaveDocumentTaskFinished();
    }

    res.removeAll(nullptr);
    return res;
}

Task::ReportResult Primer3TopLevelTask::report() {
    if (!document.isNull()) {
        // If we created @document, but did not pass it to the project (some error appeared, for example),
        // we need to delete it on our own.
        document->deleteLater();
    }
    return Task::ReportResult::ReportResult_Finished;
}

QString Primer3TopLevelTask::generateReport() const {
    const auto subtasks = getSubtasks();
    if (std::any_of(subtasks.cbegin(), subtasks.cend(), [](const QPointer<Task>& t) { return t->isCanceled(); }) &&
        !isCanceled()) {
        return tr("The task was canceled by the user");
    }

    QString res;

    if (hasError() || isCanceled()) {
        return res;
    }

    res += "<table cellspacing='7'>";
    res += "<tr><th>Statistics</th></tr>\n";

    QString highAnyCompl = tr("High any compl");
    QString high3Compl = tr("High 3' compl");
    QString highHairpins = tr("High hairpins");
    QStringList headers = { tr("Considered"),
                            tr("Too many Ns"),
                            tr("In target"),
                            tr("In exclude"),
                            tr("Not OK reg"),
                            tr("Bad GC%"),
                            tr("No GC clamp"),
                            tr("Tm too low"),
                            tr("Tm too high"),
                            highAnyCompl,
                            high3Compl,
                            highHairpins,
                            tr("Poly X"),
                            tr("High end stab"),
                            tr("OK") };
    QStringList thHeaders = { highAnyCompl,
                              high3Compl,
                              highHairpins };
    QString th = tr("th");

    bool hasThSettings = settings->getPrimerSettings()->thermodynamic_oligo_alignment != 0;
    if (!hasThSettings) {
        headers.removeOne(highHairpins);
    }
    res += "<tr><th></th> ";
    for (const auto& h : qAsConst(headers)) {
        QString thPostfix = hasThSettings && thHeaders.contains(h) ? " (" + th + ")" : "";
        res += QString("<th>%1%2</th> ").arg(h).arg(thPostfix);
    }

    auto getStatsValues = [hasThSettings](const oligo_stats& stats, oligo_type type) -> QList<int> {
        QList<int> res2;
        res2.append(stats.considered);
        res2.append(stats.ns);
        res2.append(stats.target);
        res2.append(stats.excluded);
        switch (type) {
        case OT_LEFT:
            res2.append(stats.not_in_any_left_ok_region);
            break;
        case OT_RIGHT:
            res2.append(stats.not_in_any_right_ok_region);
            break;
        case OT_INTL:
            res2.append(0);
            break;
        }
        res2.append(stats.gc);
        res2.append(stats.gc_clamp);
        res2.append(stats.temp_min);
        res2.append(stats.temp_max);
        res2.append(stats.compl_any);
        res2.append(stats.compl_end);
        if (hasThSettings) {
            res2.append(stats.hairpin_th);
        }
        res2.append(stats.poly_x);
        res2.append(stats.stability);
        res2.append(stats.ok);

        return res2;
    };

    if (settings->getPrimerSettings()->pick_left_primer) {
        oligo_stats leftStats = settings->getP3RetVal()->fwd.expl;
        res += QString("<tr><th>%1</th>").arg(tr("Left"));
        auto values = getStatsValues(leftStats, oligo_type::OT_LEFT);
        for (int v : qAsConst(values)) {
            res += tr("<th> %1 </th>").arg(QString::number(v));
        }
        res += "</tr>";
    }
    if (settings->getPrimerSettings()->pick_right_primer) {
        oligo_stats rightStats = settings->getP3RetVal()->rev.expl;
        res += QString("<tr><th>%1</th>").arg(tr("Right"));
        auto values = getStatsValues(rightStats, oligo_type::OT_RIGHT);
        for (int v : qAsConst(values)) {
            res += tr("<th> %1 </th>").arg(QString::number(v));
        }
        res += "</tr>";
    }
    if (settings->getPrimerSettings()->pick_internal_oligo) {
        oligo_stats internalStats = settings->getP3RetVal()->intl.expl;
        res += QString("<tr><th>%1</th>").arg(tr("Internal"));
        auto values = getStatsValues(internalStats, oligo_type::OT_INTL);
        for (int v : qAsConst(values)) {
            res += tr("<th> %1 </th>").arg(QString::number(v));
        }
        res += "</tr>";
    }

    res += "</table>";
    res += QString("<br>%1:<br>").arg(tr("Pair stats"));
    pair_stats pairStats = settings->getP3RetVal()->best_pairs.expl;
    res += tr("considered %1, unacceptable product size %2, high end compl %3, ok %4.")
                    .arg(pairStats.considered)
                    .arg(pairStats.product)
                    .arg(pairStats.compl_end)
                    .arg(pairStats.ok);

    if (checkComplementTask != nullptr) {
        res += checkComplementTask->generateReport();
    }

    return res;
}

QPointer<AnnotationTableObject> Primer3TopLevelTask::getAnnotationTableObject() const {
    return annotationTableObject;
}

Task* Primer3TopLevelTask::onFindExonTaskFinished() {
    QList<U2Region> regions = findExonsTask->getRegions();
    if (regions.isEmpty()) {
        SAFE_POINT_EXT(!seqObj.isNull(), setError(L10N::nullPointerError("U2SequenceObject")), nullptr);

        setError(tr("Failed to find any exon annotations associated with the sequence %1."
                    "Make sure the provided sequence is cDNA and has exonic structure annotated")
                    .arg(seqObj->getSequenceName()));
        return nullptr;
    }

    const U2Range<int>& exonRange = settings->getSpanIntronExonBoundarySettings().exonRange;

    if (exonRange.minValue != 0 && exonRange.maxValue != 0) {
        int firstExonIdx = exonRange.minValue;
        int lastExonIdx = exonRange.maxValue;
        if (firstExonIdx > regions.size()) {
            setError(tr("The first exon from the selected range [%1,%2] is larger the number of exons (%3)."
                        " Please set correct exon range.")
                        .arg(firstExonIdx)
                        .arg(lastExonIdx)
                        .arg(regions.size()));
            return nullptr;
        }

        if (lastExonIdx > regions.size()) {
            setError(tr("The the selected exon range [%1,%2] is larger the number of exons (%3)."
                        " Please set correct exon range.")
                        .arg(firstExonIdx)
                        .arg(lastExonIdx)
                        .arg(regions.size()));
            return nullptr;
        }

        regions = regions.mid(firstExonIdx - 1, lastExonIdx - firstExonIdx + 1);
        int totalLen = 0;
        for (const U2Region& r : regions) {
            totalLen += r.length;
        }
        settings->setIncludedRegion(regions.first().startPos + settings->getFirstBaseIndex(), totalLen);
    }
    settings->setExonRegions(regions);
    // reset target and excluded regions regions
    QList<U2Region> emptyList;
    settings->setExcludedRegion(emptyList);
    settings->setTarget(emptyList);

    primer3Task = new Primer3Task(settings);
    return primer3Task;
}

Task* Primer3TopLevelTask::onPrimer3TaskFinished() {
    if (settings->getCheckComplementSettings().enabled) {
        SAFE_POINT(!seqObj.isNull(), L10N::nullPointerError("U2SequenceObject"), nullptr);

        checkComplementTask = new CheckComplementTask(settings->getCheckComplementSettings(), primer3Task->getBestPairs(), seqObj);
        return checkComplementTask;
    }

    processPrimer3ResultsToAnnotationsTask = createProcessPrimer3ResultsToAnnotationsTaskPrimer();
    return processPrimer3ResultsToAnnotationsTask;
}

Task* Primer3TopLevelTask::onCheckComplementTaskFinished() {
    processPrimer3ResultsToAnnotationsTask = createProcessPrimer3ResultsToAnnotationsTaskPrimer();
    return processPrimer3ResultsToAnnotationsTask;
}

Task* Primer3TopLevelTask::onProcessPrimer3ResultsToAnnotationsTaskFinished() {
    const auto& resultAnnotations = processPrimer3ResultsToAnnotationsTask->getResultAnnotations();
    if (resultAnnotations.isEmpty()) {
        if (primer3Task->getBestPairs().isEmpty()) {
            stateInfo.addWarning(tr("No primers has been found due to the parameters you've set up"));
        } else {
            stateInfo.addWarning(tr("All found primers has been filtered due to the \"Check complement\" parameters"));
        }
        return nullptr;
    }

    if (seqObj.isNull()) {
        auto dbiReg = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
        CHECK_OP(stateInfo, nullptr);

        importSequenceFromRawDataTask = new ImportSequenceFromRawDataTask(dbiReg, U2ObjectDbi::ROOT_FOLDER, DNASequence(PRIMER_GOBJECT_NAME, settings->getSequence()));
        return importSequenceFromRawDataTask;
    } else {
        CHECK_EXT(!annotationTableObject.isNull(), setError(tr("Object with annotations was removed")), nullptr);

        createAnnotationsTask = new CreateAnnotationsTask(annotationTableObject, resultAnnotations);
        return createAnnotationsTask;
    }
}

Task* Primer3TopLevelTask::onImportSequenceFromRawDataTaskFinished() {
    SAFE_POINT(!resultFilePath.isEmpty(), "Result file path is empty", nullptr);

    auto ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(resultFilePath));
    SAFE_POINT(ioAdapterFactory != nullptr, L10N::nullPointerError("IOAdapterFactory"), nullptr);

    auto documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    SAFE_POINT(documentFormat != nullptr, L10N::nullPointerError("DocumentFormat"), nullptr);

    document = documentFormat->createNewLoadedDocument(ioAdapterFactory, resultFilePath, stateInfo);
    CHECK_OP(stateInfo, nullptr);

    seqObj = new U2SequenceObject(importSequenceFromRawDataTask->getSequenceName(), importSequenceFromRawDataTask->getEntityRef());
    document->addObject(seqObj);

    annotationTableObject = new AnnotationTableObject(PRIMER_GOBJECT_NAME, document->getDbiRef());
    annotationTableObject->addObjectRelation(seqObj, ObjectRole_Sequence);
    document->addObject(annotationTableObject);

    const auto& resultAnnotations = processPrimer3ResultsToAnnotationsTask->getResultAnnotations();
    createAnnotationsTask = new CreateAnnotationsTask(annotationTableObject, resultAnnotations);
    return createAnnotationsTask;
}

Task* Primer3TopLevelTask::onCreateAnnotationsTaskFinished() {
    CHECK(document != nullptr, nullptr);

    saveDocumentTask = new SaveDocumentTask(document);
    return saveDocumentTask;
}

Task* Primer3TopLevelTask::onSaveDocumentTaskFinished() {
    CHECK(openView, nullptr);

    // From now, @document will be handled by the Project, we can just forget about it.
    auto task = new AddDocumentAndOpenViewTask(document);
    document = nullptr;
    return task;
}

ProcessPrimer3ResultsToAnnotationsTask* Primer3TopLevelTask::createProcessPrimer3ResultsToAnnotationsTaskPrimer() const {
    SAFE_POINT(primer3Task != nullptr, L10N::nullPointerError("Primer3Task"), {});

    auto bestPairs = primer3Task->getBestPairs();
    QList<QSharedPointer<PrimerPair>> filteredPairs;
    if (checkComplementTask != nullptr) {
        filteredPairs = checkComplementTask->getFilteredPrimers();
    }
    auto singlePrimers = primer3Task->getSinglePrimers();
    qint64 sequenceLength = 0;
    if (!seqObj.isNull()) {
        sequenceLength = seqObj->getSequenceLength();
    } else {
        sequenceLength = settings->getSequence().length();
    }

    int maxPairNumber = 0;
    if (!annotationTableObject.isNull()) {
        auto rootGroup = annotationTableObject->getRootGroup();
        SAFE_POINT_NN(rootGroup, {});

        auto primer3ResultsGroup = rootGroup->getSubgroup(groupName, false);
        if (primer3ResultsGroup != nullptr) {
            auto pairGroups = primer3ResultsGroup->getSubgroups();
            for (auto pairGroup : qAsConst(pairGroups)) {
                auto name = pairGroup->getName();
                static const QString PAIR_NAME_BEGINNING = "pair ";
                CHECK_CONTINUE(name.startsWith(PAIR_NAME_BEGINNING));

                auto orderNumberString = name.mid(PAIR_NAME_BEGINNING.size());
                bool ok = false;
                int orderNumber = orderNumberString.toInt(&ok);
                CHECK_CONTINUE(ok);

                maxPairNumber = qMax(maxPairNumber, orderNumber);
            }
        }
    }

    return new ProcessPrimer3ResultsToAnnotationsTask(settings, bestPairs, filteredPairs, singlePrimers, groupName, annName, annDescription, sequenceLength, maxPairNumber);
}


}
