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

#include "ComposeResultSubtask.h"

#include <QThread>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppResources.h>
#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleChromatogramAlignment.h>
#include <U2Core/MultipleChromatogramAlignmentImporter.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>

#include "BlastReadsSubtask.h"

namespace U2 {
namespace Workflow {

static qint64 calcMemUsageBytes(DbiDataStorage *storage, const SharedDbiDataHandler &seqId, U2OpStatus &os) {
    QScopedPointer<U2SequenceObject> object(StorageUtils::getSequenceObject(storage, seqId));
    CHECK_EXT(!object.isNull(), os.setError(L10N::nullPointerError("Sequence object")), 0);

    return object->getSequenceLength();
}

static int toMb(qint64 bytes) {
    return (int)qRound((double(bytes) / (1024 * 1024)));
}

/************************************************************************/
/* ComposeResultSubTask */
/************************************************************************/
ComposeResultSubtask::ComposeResultSubtask(const SharedDbiDataHandler &reference,
                                           const QList<SharedDbiDataHandler> &reads,
                                           const QList<BlastAndSwReadTask *> subTasks,
                                           DbiDataStorage *storage)
    : Task(tr("Compose alignment"), TaskFlags_FOSE_COSC),
      reference(reference),
      reads(reads),
      subTasks(subTasks),
      storage(storage) {
    tpm = Task::Progress_Manual;
}

void ComposeResultSubtask::prepare() {
    qint64 memUsage = calcMemUsageBytes(storage, reference, stateInfo);
    CHECK_OP(stateInfo, );

    for (const SharedDbiDataHandler &read : qAsConst(reads)) {
        memUsage += calcMemUsageBytes(storage, read, stateInfo);
        CHECK_OP(stateInfo, );
    }
    if (toMb(memUsage) > 0) {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, toMb(memUsage), false));
    }
}

void ComposeResultSubtask::run() {
    referenceSequenceObject = StorageUtils::getSequenceObject(storage, reference);
    CHECK_EXT(referenceSequenceObject != nullptr, setError(L10N::nullPointerError("reference sequence object")), );
    referenceSequenceObject->setParent(this);

    DbiConnection con(storage->getDbiRef(), stateInfo);
    CHECK_OP(stateInfo, );

    con.dbi->getObjectDbi()->setObjectRank(referenceSequenceObject->getEntityRef().entityId, U2DbiObjectRank_Child, stateInfo);
    CHECK_OP(stateInfo, );

    createAlignmentAndAnnotations();
    CHECK_OP(stateInfo, );

    insertShiftedGapsIntoReference();
    CHECK_OP(stateInfo, );

    enlargeReferenceByGaps();
    CHECK_OP(stateInfo, );

    mcaObject->changeLength(stateInfo, qMax(mcaObject->getLength(), referenceSequenceObject->getSequenceLength()));
    CHECK_OP(stateInfo, );

    referenceSequenceObject->moveToThread(thread());
}

const SharedDbiDataHandler &ComposeResultSubtask::getAnnotations() const {
    return annotationsDataHandler;
}

U2SequenceObject *ComposeResultSubtask::takeReferenceSequenceObject() {
    QScopedPointer<DbiConnection> con(new DbiConnection(storage->getDbiRef(), stateInfo));
    CHECK_OP(stateInfo, nullptr);
    CHECK(con->dbi != nullptr, nullptr);

    U2DataId seqId = referenceSequenceObject->getSequenceRef().entityId;
    con->dbi->getObjectDbi()->setTrackModType(seqId, TrackOnUpdate, stateInfo);
    CHECK_OP(stateInfo, nullptr);

    U2SequenceObject *resultSequenceObject = referenceSequenceObject;
    referenceSequenceObject->setParent(nullptr);
    resultSequenceObject->moveToThread(QThread::currentThread());
    referenceSequenceObject = nullptr;
    return resultSequenceObject;
}

MultipleChromatogramAlignmentObject *ComposeResultSubtask::takeMcaObject() {
    MultipleChromatogramAlignmentObject *resultMcaObject = mcaObject;
    mcaObject->setParent(nullptr);
    mcaObject->moveToThread(QThread::currentThread());
    mcaObject = nullptr;
    return resultMcaObject;
}

void ComposeResultSubtask::createAlignmentAndAnnotations() {
    MultipleChromatogramAlignment result("Mapped reads");
    result->setAlphabet(referenceSequenceObject->getAlphabet());

    QVector<U2MsaGap> referenceGaps = getReferenceGaps();
    CHECK_OP(stateInfo, );

    // initialize annotations table on reference
    QString annotationsObjectName = referenceSequenceObject->getSequenceName() + " features";
    QScopedPointer<AnnotationTableObject> annotationsObject(new AnnotationTableObject(annotationsObjectName, storage->getDbiRef()));
    QList<SharedAnnotationData> annotations;

    int rowsCounter = 0;
    for (int i = 0; i < reads.size(); i++) {
        BlastAndSwReadTask *subTask = getBlastSwTask(i);
        CHECK_OP(stateInfo, );
        if (!subTask->isReadAligned()) {
            continue;
        }

        // add the read row
        DNASequence readSeq = getReadSequence(i);
        CHECK_OP(stateInfo, );

        DNAChromatogram readChromatogram = getReadChromatogram(i);
        CHECK_OP(stateInfo, );

        result->addRow(subTask->getReadName(), readChromatogram, readSeq, QVector<U2MsaGap>(), stateInfo);
        CHECK_OP(stateInfo, );

        if (subTask->isComplement()) {
            result->getMcaRow(result->getRowCount() - 1)->reverseComplement();
        }

        const QVector<U2MsaGap> &gaps = subTask->getReadGaps();
        for (const U2MsaGap &gap : qAsConst(gaps)) {
            result->insertGaps(rowsCounter, gap.startPos, gap.length, stateInfo);
            CHECK_OP(stateInfo, );
        }

        // add reference gaps to the read
        insertShiftedGapsIntoRead(result, i, rowsCounter, referenceGaps);
        CHECK_OP(stateInfo, );

        // add read annotation to the reference
        const MultipleChromatogramAlignmentRow readRow = result->getMcaRow(rowsCounter);
        U2Region region = getReadRegion(readRow, referenceGaps);
        SharedAnnotationData annotation(new AnnotationData());
        annotation->location = getLocation(region, subTask->isComplement());
        annotation->name = GBFeatureUtils::getKeyInfo(GBFeatureKey_misc_feature).text;
        annotation->qualifiers << U2Qualifier("label", subTask->getReadName());
        annotations.append(annotation);

        ++rowsCounter;
    }
    if (rowsCounter == 0) {
        stateInfo.setError(tr("No read satisfy minimum similarity criteria."));
        return;
    }
    result->trim(false);  // just recalculates alignment len

    mcaObject = MultipleChromatogramAlignmentImporter::createAlignment(stateInfo, storage->getDbiRef(), U2ObjectDbi::ROOT_FOLDER, result);
    mcaObject->setParent(this);
    CHECK_OP(stateInfo, );

    DbiConnection con(storage->getDbiRef(), stateInfo);
    CHECK_OP(stateInfo, );

    con.dbi->getObjectDbi()->setParent(mcaObject->getEntityRef().entityId, referenceSequenceObject->getEntityRef().entityId, stateInfo);
    CHECK_OP(stateInfo, );

    U2ByteArrayAttribute attribute;
    U2Object obj;
    obj.dbiId = storage->getDbiRef().dbiId;
    obj.id = mcaObject->getEntityRef().entityId;
    obj.version = mcaObject->getModificationVersion();
    U2AttributeUtils::init(attribute, obj, MultipleChromatogramAlignmentObject::MCAOBJECT_REFERENCE);
    attribute.value = referenceSequenceObject->getEntityRef().entityId;
    con.dbi->getAttributeDbi()->createByteArrayAttribute(attribute, stateInfo);
    CHECK_OP(stateInfo, );

    annotationsObject->addAnnotations(annotations);
    annotationsDataHandler = storage->getDataHandler(annotationsObject->getEntityRef());
}

void ComposeResultSubtask::enlargeReferenceByGaps() {
    qint64 sequenceLength = referenceSequenceObject->getSequenceLength();
    qint64 alignmentLength = mcaObject->getLength();
    int gapsNeedToInsertToReference = alignmentLength - sequenceLength;
    if (gapsNeedToInsertToReference > 0) {
        U2DataId id = mcaObject->getEntityRef().entityId;
        U2Region region(sequenceLength, 0);
        QByteArray insert(gapsNeedToInsertToReference, U2Msa::GAP_CHAR);
        DNASequence seq(insert);
        referenceSequenceObject->replaceRegion(id, region, seq, stateInfo);
        CHECK_OP(stateInfo, );
    }
}

U2Region ComposeResultSubtask::getReadRegion(const MultipleChromatogramAlignmentRow &readRow, const QVector<U2MsaGap> &referenceGapModel) const {
    U2Region region(0, readRow->getRowLengthWithoutTrailing());

    // calculate read start
    if (!readRow->getGaps().isEmpty()) {
        U2MsaGap firstGap = readRow->getGaps().first();
        if (0 == firstGap.startPos) {
            region.startPos += firstGap.length;
            region.length -= firstGap.length;
        }
    }

    qint64 leftGap = 0;
    qint64 innerGap = 0;
    foreach (const U2MsaGap &gap, referenceGapModel) {
        qint64 endPos = gap.startPos + gap.length;
        if (gap.startPos < region.startPos) {
            leftGap += gap.length;
        } else if (endPos <= region.endPos()) {
            innerGap += gap.length;
        } else {
            break;
        }
    }

    region.startPos -= leftGap;
    region.length -= innerGap;
    return region;
}

U2Location ComposeResultSubtask::getLocation(const U2Region &region, bool isComplement) {
    U2Location result;
    result->strand = isComplement ? U2Strand(U2Strand::Complementary) : U2Strand(U2Strand::Direct);

    if (region.startPos < 0) {
        // TODO: just trim the region because it is incorrect to make the annotation circular: the left (negative) part of the read is not aligned.
        // Fix it when the task can work with circular references.
        result->regions << U2Region(0, region.length + region.startPos);
    } else {
        result->regions << region;
    }

    return result;
}

BlastAndSwReadTask *ComposeResultSubtask::getBlastSwTask(int readNum) {
    CHECK_EXT(readNum < subTasks.size(), setError(L10N::internalError("Wrong reads number")), nullptr);
    return subTasks[readNum];
}

DNASequence ComposeResultSubtask::getReadSequence(int readNum) {
    BlastAndSwReadTask *subTask = getBlastSwTask(readNum);
    CHECK_OP(stateInfo, {});

    QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, subTask->getRead()));
    CHECK_EXT(!readObject.isNull(), setError(L10N::nullPointerError("Read sequence")), DNASequence());
    DNASequence seq = readObject->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, {});
    return seq;
}

DNAChromatogram ComposeResultSubtask::getReadChromatogram(int readNum) {
    BlastAndSwReadTask *subTask = getBlastSwTask(readNum);
    CHECK_OP(stateInfo, DNAChromatogram());

    QScopedPointer<U2SequenceObject> readObject(StorageUtils::getSequenceObject(storage, subTask->getRead()));
    CHECK_EXT(!readObject.isNull(), setError(L10N::nullPointerError("Read sequence")), DNAChromatogram());

    const U2EntityRef chromatogramRef = ChromatogramUtils::getChromatogramIdByRelatedSequenceId(stateInfo, readObject->getEntityRef());
    CHECK_OP(stateInfo, DNAChromatogram());
    CHECK_EXT(chromatogramRef.isValid(), setError(tr("The related chromatogram not found")), DNAChromatogram());

    DNAChromatogram chromatogram = ChromatogramUtils::exportChromatogram(stateInfo, chromatogramRef);
    CHECK_OP(stateInfo, DNAChromatogram());

    return chromatogram;
}

namespace {
bool compare(const U2MsaGap &gap1, const U2MsaGap &gap2) {
    return gap1.startPos < gap2.startPos;
}
}  // namespace

QVector<U2MsaGap> ComposeResultSubtask::getReferenceGaps() {
    QVector<U2MsaGap> result;

    for (int i = 0; i < reads.size(); i++) {
        result << getShiftedGaps(i);
        CHECK_OP(stateInfo, result);
    }
    std::sort(result.begin(), result.end(), compare);
    return result;
}

QVector<U2MsaGap> ComposeResultSubtask::getShiftedGaps(int rowNum) {
    QVector<U2MsaGap> result;

    BlastAndSwReadTask *subTask = getBlastSwTask(rowNum);
    CHECK_OP(stateInfo, result);

    qint64 wholeGap = 0;
    foreach (const U2MsaGap &gap, subTask->getReferenceGaps()) {
        result << U2MsaGap(gap.startPos - wholeGap, gap.length);
        wholeGap += gap.length;
    }
    return result;
}

void ComposeResultSubtask::insertShiftedGapsIntoReference() {
    CHECK_EXT(referenceSequenceObject != nullptr, setError(L10N::nullPointerError("Reference sequence")), );

    QVector<U2MsaGap> referenceGaps = getReferenceGaps();
    CHECK_OP(stateInfo, );

    DNASequence dnaSeq = referenceSequenceObject->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, );
    for (int i = referenceGaps.size() - 1; i >= 0; i--) {
        const U2MsaGap &gap = referenceGaps[i];
        dnaSeq.seq.insert(gap.startPos, QByteArray(gap.length, U2Msa::GAP_CHAR));
    }
    referenceSequenceObject->setWholeSequence(dnaSeq);

    // Remove columns or gaps.
    mcaObject->deleteColumnsWithGaps(stateInfo);
}

void ComposeResultSubtask::insertShiftedGapsIntoRead(MultipleChromatogramAlignment &alignment, int readNum, int rowNum, const QVector<U2MsaGap> &gaps) {
    QVector<U2MsaGap> ownGaps = getShiftedGaps(readNum);
    CHECK_OP(stateInfo, );

    qint64 globalOffset = 0;
    for (const U2MsaGap &gap : qAsConst(gaps)) {
        if (ownGaps.contains(gap)) {  // Task takes gaps into account but don't insert them.
            globalOffset += gap.length;
            ownGaps.removeOne(gap);
            continue;
        }
        alignment->insertGaps(rowNum, globalOffset + gap.startPos, gap.length, stateInfo);
        CHECK_OP(stateInfo, );
        globalOffset += gap.length;
    }
}

}  // namespace Workflow
}  // namespace U2
