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

#include "BlastReadsSubTask.h"

#include "blast_plus/BlastNPlusSupportTask.h"

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BuiltInDistanceAlgorithms.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/UserApplicationsSettings.h>


namespace U2 {
namespace Workflow {

/************************************************************************/
/* BlastReadsSubTask */
/************************************************************************/
BlastReadsSubTask::BlastReadsSubTask(const QString &dbPath,
                                     const QList<SharedDbiDataHandler> &reads,
                                     const SharedDbiDataHandler &reference,
                                     const int minIdentityPercent,
                                     const QMap<SharedDbiDataHandler, QString> &readsNames,
                                     DbiDataStorage *storage)
    : Task(tr("Map reads with BLAST & SW task"), TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel),
      dbPath(dbPath),
      reads(reads),
      readsNames(readsNames),
      reference(reference),
      minIdentityPercent(minIdentityPercent),
      storage(storage)
{
    setMaxParallelSubtasks(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
}

void BlastReadsSubTask::prepare() {
    QString tempPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    CHECK_EXT(!GUrlUtils::containSpaces(tempPath), setError(tr("The task uses a temporary folder to process the data. The folder path is required not to have spaces. "
        "Please set up an appropriate path for the \"Temporary files\" parameter on the \"Directories\" tab of the UGENE Application Settings.")), );

    foreach (const SharedDbiDataHandler &read, reads) {
        BlastAndSwReadTask* subTask = new BlastAndSwReadTask(dbPath, read, reference, minIdentityPercent, readsNames[read], storage);
        addSubTask(subTask);

        blastSubTasks << subTask;
    }
}

const QList<BlastAndSwReadTask*>& BlastReadsSubTask::getBlastSubtasks() const {
    return blastSubTasks;
}

/************************************************************************/
/* BlastAndSwReadTask */
/************************************************************************/
BlastAndSwReadTask::BlastAndSwReadTask(const QString &dbPath,
                                       const SharedDbiDataHandler &read,
                                       const SharedDbiDataHandler &reference,
                                       const int minIdentityPercent,
                                       const QString &readName,
                                       DbiDataStorage *storage)
    : Task(tr("Map one read with BLAST & SW task"), TaskFlags_NR_FOSE_COSC),
      dbPath(dbPath),
      read(read),
      reference(reference),
      minIdentityPercent(minIdentityPercent),
      readIdentity(0),
      offset(0),
      readShift(0),
      storage(storage),
      blastTask(NULL),
      readName(readName),
      complement(false),
      skipped(false)
{
    blastResultDir = ExternalToolSupportUtils::createTmpDir("blast_reads", stateInfo);

    QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, reference));
    referenceLength = refObject->getSequenceLength();
}

void BlastAndSwReadTask::prepare() {
    blastTask = getBlastTask();
    CHECK_OP(stateInfo, );
    SAFE_POINT_EXT(NULL != blastTask, setError("BLAST subtask is NULL"), );
    addSubTask(blastTask);
}

QList<Task*> BlastAndSwReadTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    if (subTask->hasError() && subTask == blastTask) {
        QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, reference));
        CHECK_EXT(!refObject.isNull(), setError(L10N::nullPointerError("Reference sequence")), result);
        setError(tr("A problem occurred while mapping \"%1\" to \"%2\".").arg(readName).arg(refObject->getGObjectName()));
    }
    CHECK(subTask != NULL, result);
    CHECK(!subTask->hasError() && !subTask->isCanceled(), result);

    if (subTask == blastTask) {
        U2Region referenceRegion = getReferenceRegion(blastTask->getResultedAnnotations());
        if (referenceRegion.isEmpty()) {
            skipped = true;
            readIdentity = 0;
            taskLog.info(tr("%1 was skipped. No BLAST results.").arg(getReadName()));
            return result;
        }
        createAlignment(referenceRegion);

        AbstractAlignmentTaskFactory *factory = getAbstractAlignmentTaskFactory("Smith-Waterman", "SW_classic", stateInfo);
        CHECK_OP(stateInfo, result);

        QScopedPointer<PairwiseAlignmentTaskSettings> settings(createSettings(storage, msa, stateInfo));
        CHECK_OP(stateInfo, result);
        settings->setCustomValue("SW_gapOpen", -10);
        settings->setCustomValue("SW_gapExtd", -1);
        settings->setCustomValue("SW_scoringMatrix", "dna");

        result << factory->getTaskInstance(settings.take());
    } else if (qobject_cast<AbstractAlignmentTask*>(subTask) != NULL) {
        QScopedPointer<MultipleSequenceAlignmentObject> msaObject(StorageUtils::getMsaObject(storage, msa));
        CHECK_EXT(!msaObject.isNull(), setError(L10N::nullPointerError("MSA object for %1").arg(getReadName())), result);
        int rowCount = msaObject->getNumRows();
        CHECK_EXT(2 == rowCount, setError(L10N::internalError("Wrong rows count: " + QString::number(rowCount))), result);

        referenceGaps = msaObject->getMsaRow(0)->getGapModel();
        readGaps = msaObject->getMsaRow(1)->getGapModel();

        if (offset > 0) {
            shiftGaps(referenceGaps);
            MsaRowUtils::addOffsetToGapModel(readGaps, offset);
        }

        msaObject->crop(msaObject->getRow(1)->getCoreRegion());
        MSADistanceAlgorithmFactory* factory = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactory(BuiltInDistanceAlgorithms::SIMILARITY_ALGO);
        CHECK_EXT(NULL != factory, setError("MSADistanceAlgorithmFactory is NULL"), result);
        factory->resetFlag(DistanceAlgorithmFlag_ExcludeGaps);

        MSADistanceAlgorithm* algo = factory->createAlgorithm(msaObject->getMsa());
        CHECK_EXT(NULL != algo, setError("MSADistanceAlgorithm is NULL"), result);
        result << algo;
    } else if (qobject_cast<MSADistanceAlgorithm*>(subTask) != NULL){
        MSADistanceAlgorithm* algo = qobject_cast<MSADistanceAlgorithm*>(subTask);
        const MSADistanceMatrix& mtx = algo->getMatrix();

        readIdentity = mtx.getSimilarity(0, 1, true);
        if (readIdentity < minIdentityPercent) {
            skipped = true;
            taskLog.info(tr("%1 was skipped. Low similarity: %2. Minimum similarity was set to %3")
                         .arg(getReadName()).arg(readIdentity).arg(minIdentityPercent));
        }
    }
    return result;
}

Task::ReportResult BlastAndSwReadTask::report() {
    if (hasError() || isCanceled()) {
        skipped = true;
        readIdentity = 0;
    }
    return ReportResult_Finished;
}

bool BlastAndSwReadTask::isComplement() const {
    return complement;
}

const SharedDbiDataHandler& BlastAndSwReadTask::getRead() const {
    return read;
}

const U2MsaRowGapModel& BlastAndSwReadTask::getReferenceGaps() const {
    return referenceGaps;
}

const U2MsaRowGapModel& BlastAndSwReadTask::getReadGaps() const {
    return readGaps;
}

bool BlastAndSwReadTask::isReadAligned() const {
    return !skipped;
}

QString BlastAndSwReadTask::getReadName() const {
    return readName;
}

MultipleSequenceAlignment BlastAndSwReadTask::getMAlignment() {
    QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(storage, msa));
    CHECK(msaObj != NULL, MultipleSequenceAlignment());

    return msaObj->getMultipleAlignment();
}

qint64 BlastAndSwReadTask::getOffset() const {
    return offset;
}

int BlastAndSwReadTask::getReadIdentity() const {
    return readIdentity;
}

BlastNPlusSupportTask *BlastAndSwReadTask::getBlastTask() {
    BlastTaskSettings settings;

    settings.programName = "blastn";
    settings.databaseNameAndPath = dbPath;
    settings.megablast = true;
    settings.wordSize = 28;
    settings.xDropoffGA = 20;
    settings.xDropoffUnGA = 10;
    settings.xDropoffFGA = 100;
    settings.numberOfProcessors = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();
    settings.numberOfHits = 100;
    settings.gapOpenCost = 2;
    settings.gapExtendCost = 2;

    QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, read));
    CHECK_EXT(!readObject.isNull(), setError(L10N::nullPointerError("U2SequenceObject")), NULL);

    if (readName.isEmpty()) {
        readName = readObject->getSequenceName();
    }

    settings.querySequence = readObject->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, NULL);

    checkRead(settings.querySequence);
    CHECK_OP(stateInfo, NULL);

    settings.alphabet = readObject->getAlphabet();
    settings.isNucleotideSeq = settings.alphabet->isNucleic();

    settings.needCreateAnnotations = false;
    settings.groupName = "blast";

    settings.outputResFile = GUrlUtils::prepareTmpFileLocation(blastResultDir, "read_sequence", "gb", stateInfo);
    settings.outputType = 5;
    settings.strandSource = BlastTaskSettings::HitFrame;

    return new BlastNPlusSupportTask(settings);
}

void BlastAndSwReadTask::checkRead(const QByteArray &sequenceData) {
    const int gapsCount = sequenceData.count(U2Msa::GAP_CHAR);
    const int nCount = sequenceData.count("N");
    if (gapsCount + nCount == sequenceData.length()) {
        setError(tr("Read doesn't contain meaningful data"));
    }
}

U2Region BlastAndSwReadTask::getReferenceRegion(const QList<SharedAnnotationData> &blastAnnotations) {
    CHECK(!blastAnnotations.isEmpty(), U2Region());
    U2Region refRegion;
    U2Region blastReadRegion;
    int maxIdentity = 0;
    foreach (const SharedAnnotationData& ann, blastAnnotations) {
        QString percentQualifier = ann->findFirstQualifierValue("identities");
        int annIdentity = percentQualifier.left(percentQualifier.indexOf('/')).toInt();
        if (annIdentity  > maxIdentity ) {
            // identity
            maxIdentity = annIdentity;

            // annotation region on read
            blastReadRegion = ann->getRegions().first();

            // region on reference
            qint64 hitFrom = ann->findFirstQualifierValue("hit-from").toInt();
            qint64 hitTo = ann->findFirstQualifierValue("hit-to").toInt();
            qint64 leftMost = qMin(hitFrom, hitTo);
            qint64 rightMost = qMax(hitFrom, hitTo);
            refRegion = U2Region(leftMost - 1, rightMost - leftMost);

            // frame
            QString frame = ann->findFirstQualifierValue("source_frame");
            complement = (frame == "complement");
        }
    }
    QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, read));
    CHECK_EXT(!readObject.isNull(), setError(L10N::nullPointerError("Read sequence")), U2Region());
    qint64 undefinedLen = readObject->getSequenceLength() - maxIdentity;
    readShift = undefinedLen - blastReadRegion.startPos;

    // extend ref region to the read
    refRegion.startPos = qMax((qint64)0, (qint64)(refRegion.startPos - undefinedLen));
    refRegion.length = qMin(referenceLength - refRegion.startPos, (qint64)(blastReadRegion.length + 2 * undefinedLen));

    return refRegion;
}

void BlastAndSwReadTask::createAlignment(const U2Region& refRegion) {
    QScopedPointer<U2SequenceObject> refObject(StorageUtils::getSequenceObject(storage, reference));
    CHECK_EXT(!refObject.isNull(), setError(L10N::nullPointerError("Reference sequence")), );
    QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, read));
    CHECK_EXT(!readObject.isNull(), setError(L10N::nullPointerError("Read sequence")), );

    QByteArray referenceData = refObject->getSequenceData(refRegion, stateInfo);
    CHECK_OP(stateInfo, );

    MultipleSequenceAlignment alignment("msa", refObject->getAlphabet());
    alignment->addRow(refObject->getSequenceName(), referenceData);
    CHECK_OP(stateInfo, );
    QByteArray readData = readObject->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    if (readShift != 0) {
        alignment->addRow(readObject->getSequenceName(),
                          complement ? DNASequenceUtils::reverseComplement(readData) : readData,
                          U2MsaRowGapModel() << U2MsaGap(0, readShift),
                          stateInfo);
    } else {
        alignment->addRow(readObject->getSequenceName(), complement ? DNASequenceUtils::reverseComplement(readData) : readData);
    }

    CHECK_OP(stateInfo, );

    QScopedPointer<MultipleSequenceAlignmentObject> msaObj(MultipleSequenceAlignmentImporter::createAlignment(storage->getDbiRef(), alignment, stateInfo));
    CHECK_OP(stateInfo, );
    msa = storage->getDataHandler(msaObj->getEntityRef());
    offset = refRegion.startPos;
}

void BlastAndSwReadTask::shiftGaps(U2MsaRowGapModel &gaps) const {
    for (int i = 0; i < gaps.size(); i++) {
        gaps[i].offset += offset;
    }
}

AbstractAlignmentTaskFactory* BlastAndSwReadTask::getAbstractAlignmentTaskFactory(const QString &algoId, const QString &implId, U2OpStatus &os) {
    AlignmentAlgorithm *algo = AppContext::getAlignmentAlgorithmsRegistry()->getAlgorithm(algoId);
    CHECK_EXT(NULL != algo, os.setError(BlastAndSwReadTask::tr("The %1 algorithm is not found. Add the %1 plugin.").arg(algoId)), NULL);

    AlgorithmRealization *algoImpl = algo->getAlgorithmRealization(implId);
    CHECK_EXT(NULL != algoImpl, os.setError(BlastAndSwReadTask::tr("The %1 algorithm is not found. Check that the %1 plugin is up to date.").arg(algoId)), NULL);

    return algoImpl->getTaskFactory();
}

PairwiseAlignmentTaskSettings* BlastAndSwReadTask::createSettings(DbiDataStorage *storage, const SharedDbiDataHandler &msa, U2OpStatus &os) {
    QScopedPointer<MultipleSequenceAlignmentObject> msaObject(StorageUtils::getMsaObject(storage, msa));
    CHECK_EXT(!msaObject.isNull(), os.setError(L10N::nullPointerError("MSA object")), NULL);

    U2DataId referenceId = msaObject->getMsaRow(0)->getRowDbInfo().sequenceId;
    U2DataId readId = msaObject->getMsaRow(1)->getRowDbInfo().sequenceId;

    PairwiseAlignmentTaskSettings *settings = new PairwiseAlignmentTaskSettings();
    settings->alphabet = msaObject->getAlphabet()->getId();
    settings->inNewWindow = false;
    settings->msaRef = msaObject->getEntityRef();
    settings->firstSequenceRef = U2EntityRef(msaObject->getEntityRef().dbiRef, referenceId);
    settings->secondSequenceRef = U2EntityRef(msaObject->getEntityRef().dbiRef, readId);
    return settings;
}

} // namespace Workflow
} // namespace U2
