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

#include "BlastAlignToReferenceTask.h"

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BuiltInDistanceAlgorithms.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "blast/BlastNTask.h"

namespace U2 {
namespace Workflow {

/************************************************************************/
/* BlastAlignToReferenceMuxTask */
/************************************************************************/
BlastAlignToReferenceMuxTask::BlastAlignToReferenceMuxTask(const QString& _blastDbPath,
                                                           const QList<SharedDbiDataHandler>& _reads,
                                                           const SharedDbiDataHandler& _reference,
                                                           const QMap<SharedDbiDataHandler, QString>& _readRenameMap,
                                                           DbiDataStorage* _storage)
    : Task(tr("Align reads with BLAST & Smith-Waterman multiplexer task "), TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel),
      blastDbPath(_blastDbPath),
      reads(_reads),
      reference(_reference),
      readRenameMap(_readRenameMap),
      storage(_storage) {
    tpm = Progress_Manual;

    // Each sub-task will process N reads at time.
    // Invocation of BLAST external tool per every read is very time-consuming
    // and a single BLAST invocation can process multiple queries at a time.
    static int constexpr readCountPerBlast = 100;
    readsRangePerSubtask = U2Region::split({0, reads.size()}, readCountPerBlast);
}

void BlastAlignToReferenceMuxTask::prepare() {
    CHECK(!readsRangePerSubtask.isEmpty(), );
    QString tempPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    CHECK_EXT(!GUrlUtils::containSpaces(tempPath),
              setError(tr("The task uses a temporary folder to process the data. The folder path is required not to have spaces. "
                          "Please set up an appropriate path for the \"Temporary files\" parameter on the \"Directories\" tab of the UGENE Application Settings.")), );

    U2Region readsRange = readsRangePerSubtask.takeFirst();
    addSubTask(createNewSubtask(readsRange));
}

QList<Task*> BlastAlignToReferenceMuxTask::onSubTaskFinished(Task*) {
    CHECK_OP(stateInfo, {});
    CHECK(!readsRangePerSubtask.isEmpty(), {});
    U2Region readsRange = readsRangePerSubtask.takeFirst();
    return {createNewSubtask(readsRange)};
}

BlastAlignToReferenceTask* BlastAlignToReferenceMuxTask::createNewSubtask(const U2Region& readsRange) const {
    QList<SharedDbiDataHandler> readsPerSubtask = reads.mid(readsRange.startPos, readsRange.length);
    QString subtaskNameSuffix = tr(", reads range %1-%2").arg(readsRange.startPos).arg(readsRange.endPos());
    return new BlastAlignToReferenceTask(blastDbPath, readsPerSubtask, reference, readRenameMap, storage, subtaskNameSuffix);
}

Task::ReportResult BlastAlignToReferenceMuxTask::report() {
    QList<QPointer<Task>> subtasks = getSubtasks();
    for (const auto& task : qAsConst(subtasks)) {
        auto subtask = qobject_cast<BlastAlignToReferenceTask*>(task.data());
        SAFE_POINT(subtask != nullptr, "BlastAlignToReferenceMuxTask has wrong subtask", ReportResult_Finished);
        alignmentResults << subtask->getAlignmentResults();
    }
    return ReportResult_Finished;
}

const QList<AlignToReferenceResult>& BlastAlignToReferenceMuxTask::getAlignmentResults() const {
    return alignmentResults;
}

/************************************************************************/
/* BlastAlignToReferenceTask */
/************************************************************************/
BlastAlignToReferenceTask::BlastAlignToReferenceTask(const QString& _blastDbPath,
                                                     const QList<SharedDbiDataHandler>& _reads,
                                                     const SharedDbiDataHandler& _reference,
                                                     const QMap<SharedDbiDataHandler, QString>& _readRenameMap,
                                                     DbiDataStorage* _storage,
                                                     const QString& taskNameSuffix)
    : Task(tr("Align reads with BLAST & Smith-Waterman task") + taskNameSuffix, TaskFlags_NR_FOSE_COSC),
      dbPath(_blastDbPath),
      reads(_reads),
      reference(_reference),
      readRenameMap(_readRenameMap),
      storage(_storage) {
    QScopedPointer<U2SequenceObject> referenceSequence(StorageUtils::getSequenceObject(storage, reference));
    CHECK_EXT(referenceSequence->getSequenceLength() < INT_MAX, setError(tr("Maximum supported reference sequence length is 2Gb")), );
    setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS_AUTO);
}

void BlastAlignToReferenceTask::prepare() {
    CHECK(!reads.isEmpty(), );

    QScopedPointer<U2SequenceObject> referenceSequence(StorageUtils::getSequenceObject(storage, reference));
    CHECK_EXT(!referenceSequence.isNull(), setError(tr("Reference sequence is not found")), );

    BlastTaskSettings settings;
    settings.programName = "blastn";
    settings.databaseNameAndPath = dbPath;
    settings.wordSize = 11;
    settings.xDropoffGA = 20;
    settings.xDropoffUnGA = 10;
    settings.xDropoffFGA = 100;
    settings.numberOfProcessors = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();
    settings.numberOfHits = 1;  // 'culling_limit' - keep the longest hit.
    settings.gapOpenCost = 2;
    settings.gapExtendCost = 2;
    blastQuerySequenceIndexByReadIndex.resize(reads.size());
    for (int readIndex = 0; readIndex < reads.size(); readIndex++) {
        const SharedDbiDataHandler& read = reads[readIndex];
        blastQuerySequenceIndexByReadIndex[readIndex] = -1;
        QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, read));
        CHECK_EXT(!readObject.isNull(), setError(L10N::nullPointerError("U2SequenceObject")), );

        QByteArray readSequence = readObject->getWholeSequenceData(stateInfo);
        CHECK_OP(stateInfo, );
        bool isEmptyReadSequence = std::all_of(readSequence.begin(), readSequence.end(), [](char c) { return c == U2Msa::GAP_CHAR || c == 'N'; });
        CHECK_CONTINUE(!isEmptyReadSequence);  // An empty read will be added as 'failed' later since it has no results.
        settings.querySequences << readSequence;
        blastQuerySequenceIndexByReadIndex[readIndex] = settings.querySequences.size() - 1;

        const DNAAlphabet* readAlphabet = readObject->getAlphabet();
        if (settings.alphabet == nullptr) {
            settings.alphabet = readAlphabet;
        } else {
            const DNAAlphabet* commonAlphabet = U2AlphabetUtils::deriveCommonAlphabet(settings.alphabet, readAlphabet);
            if (commonAlphabet != settings.alphabet) {
                CHECK_EXT(commonAlphabet != nullptr, setError(tr("Failed to derive common alphabet for all reads")), );
                settings.alphabet = commonAlphabet;
            }
        }
    }
    CHECK_EXT(!settings.querySequences.isEmpty(), coreLog.error(tr("All input reads contain gaps or Ns only, abort")), );
    CHECK_EXT(settings.alphabet->isNucleic(), setError(tr("Can't run alignment on non-nucleic reads")), );
    settings.isNucleotideSeq = true;
    settings.needCreateAnnotations = false;
    settings.groupName = "blast";
    CHECK_OP(stateInfo, );
    settings.outputType = 5;
    settings.strandSource = BlastTaskSettings::HitFrame;
    addSubTask(new BlastNTask(settings));
}

static const char* READ_ID_KEY = "read-sequence-data-id-property-key";

QList<Task*> BlastAlignToReferenceTask::onSubTaskFinished(Task* subTask) {
    if (subTask->hasError()) {
        QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, reference));
        CHECK_EXT(!refObject.isNull(), setError(tr("Reference sequence is not accessible anymore")), {});
        QString referenceName = refObject->getGObjectName();
        QString error = subTask->getError();
        if (qobject_cast<BlastNTask*>(subTask) != nullptr) {
            setError(tr("Error while running BLAST to align reads to reference: '%1', %2").arg(referenceName).arg(error));
        } else if (qobject_cast<AbstractAlignmentTask*>(subTask) != nullptr) {
            setError(tr("Error while aligning a read to reference with Smith-Waterman: '%1").arg(error));
        }  // Else inherit an error from the failed sub-task.
        return {};
    }
    CHECK_OP(stateInfo, {});

    QList<Task*> result;
    if (auto blastTask = qobject_cast<BlastNTask*>(subTask)) {
        auto pairwiseAlignmentFactory = getAbstractAlignmentTaskFactory("Smith-Waterman", "SW_classic", stateInfo);
        CHECK_OP(stateInfo, result);

        const DNAAlphabet* msaAlphabet = blastTask->getSettings().alphabet;
        QScopedPointer<U2SequenceObject> referenceObject(StorageUtils::getSequenceObject(storage, reference));
        CHECK_EXT(!referenceObject.isNull(), setError(L10N::nullPointerError("Reference sequence")), {});
        DNASequence referenceSequence = referenceObject->getWholeSequence(stateInfo);
        CHECK_OP(stateInfo, {});
        for (int readIndex = 0; readIndex < reads.length(); readIndex++) {
            const SharedDbiDataHandler& read = reads[readIndex];
            QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, read));
            {
                AlignToReferenceResult alignmentResult;
                alignmentResult.readHandle = read;
                alignmentResult.readName = readRenameMap.value(read);
                if (alignmentResult.readName.isEmpty()) {
                    alignmentResult.readName = readObject.isNull() ? "?" : readObject->getSequenceName();
                }
                alignmentResults << alignmentResult;
            }
            CHECK_CONTINUE(!readObject.isNull());

            int blastResultIndex = blastQuerySequenceIndexByReadIndex[readIndex];
            QList<SharedAnnotationData> blastResults = blastResultIndex == -1
                                                           ? QList<SharedAnnotationData>()
                                                           : blastTask->getResultPerQuerySequence(blastResultIndex);
            CHECK_CONTINUE(!blastResults.isEmpty());

            AlignToReferenceResult& alignmentResult = alignmentResults.last();
            QByteArray readKey = read->getEntityRef().entityId;
            alignmentResultByRead[readKey] = &alignmentResult;

            SharedAnnotationData blastResult = *std::max_element(blastResults.begin(), blastResults.end(), [](const auto& r1, const auto& r2) {
                return r1->findFirstQualifierValue("score").toInt() < r2->findFirstQualifierValue("score").toInt();
            });

            U2OpStatusImpl readOs;
            DNASequence readSequence = readObject->getWholeSequence(readOs);
            CHECK_CONTINUE(!readOs.hasError());

            convertBlastResultToAlignmentResult(blastResult, alignmentResult);
            assignReferencePairwiseAlignmentRegion(alignmentResult, readSequence.length(), referenceSequence.length());

            QScopedPointer<MultipleSequenceAlignmentObject> pairwiseMsaObject(
                createPairwiseAlignment(readOs, storage->getDbiRef(), referenceSequence, readSequence, msaAlphabet, alignmentResult));
            CHECK_CONTINUE(!readOs.hasError());
            SAFE_POINT(!pairwiseMsaObject.isNull() && pairwiseMsaObject->getRowCount() == 2 && pairwiseMsaObject->getEntityRef().dbiRef == storage->getDbiRef(),
                       "Failed to create MSA for pairwise align",
                       {});

            auto pairwiseAlignmentSettings = new PairwiseAlignmentTaskSettings();
            pairwiseAlignmentSettings->alphabet = msaAlphabet->getId();
            pairwiseAlignmentSettings->inNewWindow = false;
            pairwiseAlignmentSettings->msaRef = pairwiseMsaObject->getEntityRef();
            pairwiseAlignmentSettings->firstSequenceRef = U2EntityRef(storage->getDbiRef(), pairwiseMsaObject->getMsaRow(0)->getRowDbInfo().sequenceId);
            pairwiseAlignmentSettings->secondSequenceRef = U2EntityRef(storage->getDbiRef(), pairwiseMsaObject->getMsaRow(1)->getRowDbInfo().sequenceId);
            pairwiseAlignmentSettings->setCustomValue("SW_gapOpen", -10);
            pairwiseAlignmentSettings->setCustomValue("SW_gapExtd", -1);
            pairwiseAlignmentSettings->setCustomValue("SW_scoringMatrix", "dna");
            pairwiseMsaByRead.insert(readKey, pairwiseMsaObject->getEntityRef());
            auto pairwiseAlignmentTask = pairwiseAlignmentFactory->getTaskInstance(pairwiseAlignmentSettings);
            pairwiseAlignmentTask->setProperty(READ_ID_KEY, readKey);
            result << pairwiseAlignmentTask;
        }
    } else if (auto pairwiseAlignTask = qobject_cast<AbstractAlignmentTask*>(subTask)) {
        U2DataId readKey = pairwiseAlignTask->property(READ_ID_KEY).toByteArray();
        U2EntityRef msaRef = pairwiseMsaByRead.value(readKey);

        // Read 'pairwiseMsaObject' from the DB. It was created when 'blast' task was finished and now has a complete pairwise alignment.
        QScopedPointer<MultipleSequenceAlignmentObject> pairwiseMsaObject(new MultipleSequenceAlignmentObject("pairwise-msa", msaRef));

        SAFE_POINT(pairwiseMsaObject->getRowCount() == 2, "Invalid pairwise MSA", {});
        // TODO: pairwiseMsaObject is never deallocated in DB: workflow DB is growing until the task ends! See DbiDataStorage::deleteObject!

        SAFE_POINT_EXT(alignmentResultByRead.contains(readKey), setError("Internal error! Read not found"), {});
        AlignToReferenceResult* alignmentResult = alignmentResultByRead[readKey];
        SAFE_POINT_EXT(alignmentResult != nullptr, setError("Internal error! Read result is not found"), {});

        QVector<U2MsaGap> referenceGaps = pairwiseMsaObject->getMsaRow(0)->getGaps();
        QVector<U2MsaGap> readGaps = pairwiseMsaObject->getMsaRow(1)->getGaps();

        // Shift reference gaps back to the global coordinates: add a constant offset to all reference gaps.
        int referenceRegionOffset = (int)alignmentResult->pairwiseAlignmentReferenceRegion.startPos;  // Cast is SAFE. We run 2GB check during the start.
        for (U2MsaGap& referenceGap : referenceGaps) {
            referenceGap.startPos += referenceRegionOffset;
        }

        // For reads extend the first gap (if any) by 'referenceRegionOffset' and shift other gaps the same as we did for the reference above.
        MsaRowUtils::addOffsetToGapModel(readGaps, referenceRegionOffset);

        alignmentResult->referenceGaps = referenceGaps;
        alignmentResult->readGaps = readGaps;

        // Run similarity algorithm for the pairwise alignment to compute 'identity percent'.
        pairwiseMsaObject->crop(pairwiseMsaObject->getRow(1)->getCoreRegion());
        MSADistanceAlgorithmFactory* factory = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactory(BuiltInDistanceAlgorithms::SIMILARITY_ALGO);
        CHECK_EXT(factory != nullptr, setError("MSADistanceAlgorithmFactory is NULL"), result);
        factory->resetFlag(DistanceAlgorithmFlag_ExcludeGaps);

        MSADistanceAlgorithm* similarityTask = factory->createAlgorithm(pairwiseMsaObject->getMsa());
        CHECK_EXT(similarityTask != nullptr, setError(L10N::nullPointerError("MSADistanceAlgorithm")), result);
        similarityTask->setProperty(READ_ID_KEY, readKey);
        result << similarityTask;
    } else if (auto similarityTask = qobject_cast<MSADistanceAlgorithm*>(subTask)) {
        U2DataId readKey = similarityTask->property(READ_ID_KEY).toByteArray();
        SAFE_POINT_EXT(alignmentResultByRead.contains(readKey), setError("Internal error! Read not found"), {});
        AlignToReferenceResult* alignmentResult = alignmentResultByRead[readKey];
        SAFE_POINT_EXT(alignmentResult != nullptr, setError("Internal error! Read result is not found"), {});

        const MSADistanceMatrix& matrix = similarityTask->getMatrix();
        alignmentResult->identityPercent = matrix.getSimilarity(0, 1, true);
    } else {
        FAIL("Unexpected alignment pipeline state: task is unknown: " + subTask->getTaskName(), {})
    }
    return result;
}

Task::ReportResult BlastAlignToReferenceTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);
    // Re-sort results to use the original reads order.
    QMap<SharedDbiDataHandler, int> originalIndexByHandle;
    for (int i = 0; i < reads.length(); i++) {
        originalIndexByHandle[reads[i]] = i;
    }
    std::sort(alignmentResults.begin(), alignmentResults.end(), [&originalIndexByHandle](const auto& r1, const auto& r2) {
        return originalIndexByHandle[r1.readHandle] < originalIndexByHandle[r2.readHandle];
    });
    return ReportResult_Finished;
}

void BlastAlignToReferenceTask::convertBlastResultToAlignmentResult(const SharedAnnotationData& blastResult, AlignToReferenceResult& alignResult) {
    alignResult.blastReadRegion = blastResult->getRegions().first();
    alignResult.isOnComplementaryStrand = blastResult->findFirstQualifierValue("source_frame") == "complement";

    int hitFrom = blastResult->findFirstQualifierValue("hit-from").toInt();
    int hitTo = blastResult->findFirstQualifierValue("hit-to").toInt();
    int leftMost = qMin(hitFrom, hitTo);
    int rightMost = qMax(hitFrom, hitTo);

    alignResult.blastReferenceRegion = U2Region(leftMost - 1, rightMost - leftMost);

    QString identitiesValue = blastResult->findFirstQualifierValue("identities");
    alignResult.blastIdentity = identitiesValue.leftRef(identitiesValue.indexOf('/')).toInt();
}

void BlastAlignToReferenceTask::assignReferencePairwiseAlignmentRegion(AlignToReferenceResult& alignResult, int readLength, int referenceLength) {
    // TODO: below is the original algo and it is not correct: even 1 unmapped base can be N symbols away.
    int unmappedReadLength = readLength - alignResult.blastIdentity;
    int pairwiseReferenceRegionStart = qMax<int>(0, alignResult.blastReferenceRegion.startPos - unmappedReadLength);
    int pairwiseReferenceRegionLength = qMin<int>(referenceLength - pairwiseReferenceRegionStart,
                                                  alignResult.blastReadRegion.length + 2 * unmappedReadLength);
    alignResult.pairwiseAlignmentReferenceRegion = {pairwiseReferenceRegionStart, pairwiseReferenceRegionLength};
    alignResult.pairwiseAlignmentLeadingReadGap = unmappedReadLength - alignResult.blastReadRegion.startPos;
}

MultipleSequenceAlignmentObject* BlastAlignToReferenceTask::createPairwiseAlignment(
    U2OpStatus& os,
    const U2DbiRef& dbiRef,
    const DNASequence& referenceSequence,
    const DNASequence& readSequence,
    const DNAAlphabet* alphabet,
    const AlignToReferenceResult& alignmentResult) {
    MultipleSequenceAlignment pairwiseMsa("pairwise-msa", alphabet);
    QByteArray croppedReferenceSequence = referenceSequence.seq.mid(alignmentResult.pairwiseAlignmentReferenceRegion.startPos,
                                                                    alignmentResult.pairwiseAlignmentReferenceRegion.length);
    pairwiseMsa->addRow("reference", croppedReferenceSequence);
    QByteArray translatedReadSequence = readSequence.seq;
    if (alignmentResult.isOnComplementaryStrand) {
        translatedReadSequence = DNASequenceUtils::reverseComplement(translatedReadSequence);
    }
    QVector<U2MsaGap> readGaps;
    if (alignmentResult.pairwiseAlignmentLeadingReadGap > 0) {
        readGaps.append({0, alignmentResult.pairwiseAlignmentLeadingReadGap});
    }
    pairwiseMsa->addRow("read", translatedReadSequence, readGaps, os);
    CHECK_OP(os, nullptr);
    return MultipleSequenceAlignmentImporter::createAlignment(dbiRef, pairwiseMsa, os);
}

const QList<AlignToReferenceResult>& BlastAlignToReferenceTask::getAlignmentResults() const {
    return alignmentResults;
}

AbstractAlignmentTaskFactory* BlastAlignToReferenceTask::getAbstractAlignmentTaskFactory(const QString& algoId,
                                                                                         const QString& implId,
                                                                                         U2OpStatus& os) {
    AlignmentAlgorithm* algorithm = AppContext::getAlignmentAlgorithmsRegistry()->getAlgorithm(algoId);
    CHECK_EXT(algorithm != nullptr, os.setError(tr("The %1 algorithm is not found. Add the %1 plugin.").arg(algoId)), nullptr);

    AlgorithmRealization* algorithmRealization = algorithm->getAlgorithmRealization(implId);
    CHECK_EXT(algorithmRealization != nullptr, os.setError(tr("The %1 algorithm is not found. Check that the %1 plugin is up to date.").arg(algoId)), nullptr);

    return algorithmRealization->getTaskFactory();
}

}  // namespace Workflow
}  // namespace U2
