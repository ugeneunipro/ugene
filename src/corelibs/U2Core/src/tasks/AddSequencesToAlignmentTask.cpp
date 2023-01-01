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

#include "AddSequencesToAlignmentTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

/** Returns a common alphabet for MSA object & all sequences in the list and the alignment. Defaults to RAW if no common alphabet can be derived. */
static const DNAAlphabet* deriveCommonAlphabet(const QList<DNASequence>& sequenceList,
                                               const DNAAlphabet* msaAlphabet,
                                               bool recheckNewSequenceAlphabetOnMismatch) {
    SAFE_POINT(msaAlphabet != nullptr, "msaAlphabet can't be null!", {});
    const DNAAlphabet* alphabet = msaAlphabet;
    for (const DNASequence& sequence : qAsConst(sequenceList)) {
        SAFE_POINT(sequence.alphabet != nullptr, "sequence alphabet can't be null!", {});
        const DNAAlphabet* commonAlphabet = U2AlphabetUtils::deriveCommonAlphabet(sequence.alphabet, alphabet);
        SAFE_POINT(commonAlphabet != nullptr, "deriveCommonAlphabet returns null for non-null input!", {});
        if (!alphabet->isRaw() && alphabet != commonAlphabet && recheckNewSequenceAlphabetOnMismatch) {
            QList<const DNAAlphabet*> allValidAlphabets = U2AlphabetUtils::findAllAlphabets(sequence.constSequence());
            if (allValidAlphabets.contains(alphabet)) {
                commonAlphabet = alphabet;
            }
        }
        alphabet = commonAlphabet;
    }
    return alphabet;
}

/** Adds MSA rows to the given MSA object. Adds rows to the same DBI with the MSA object. */
static void createMsaRowsFromResultSequenceList(U2OpStatus& os,
                                                const MultipleSequenceAlignmentObject* msaObject,
                                                const QList<DNASequence>& inputSequenceList,
                                                QList<U2MsaRow>& resultRows) {
    QSet<QString> usedRowNames;
    for (const MultipleAlignmentRow& row : qAsConst(msaObject->getRows())) {
        usedRowNames.insert(row->getName());
    }
    U2DbiRef dbiRef = msaObject->getEntityRef().dbiRef;
    for (const DNASequence& sequenceObject : qAsConst(inputSequenceList)) {
        CHECK_OP(os, );
        QString rowName = MSAUtils::rollMsaRowName(sequenceObject.getName(), usedRowNames);
        U2MsaRow row = MSAUtils::copyRowFromSequence(sequenceObject, dbiRef, os);
        CHECK_OP(os, );
        if (rowName != sequenceObject.getName()) {
            U2EntityRef rowSequenceRef(dbiRef, row.sequenceId);
            U2SequenceUtils::updateSequenceName(rowSequenceRef, rowName, os);
            CHECK_OP(os, );
        }
        if (row.gend > 0) {
            resultRows << row;
            usedRowNames.insert(rowName);
        }
    }
}

/** Inserts rows into the alignment. The rows must be stored in the same DBI with MSA object. */
static void addRowsToAlignment(U2OpStatus& os,
                               MaModificationInfo& mi,
                               MultipleSequenceAlignmentObject* msaObject,
                               U2MsaDbi* msaDbi,
                               QList<U2MsaRow>& rows,
                               int insertMaRowIndex) {
    CHECK(!rows.isEmpty(), );
    U2DataId msaEntityId = msaObject->getEntityRef().entityId;
    msaDbi->addRows(msaEntityId, rows, insertMaRowIndex, os);
    CHECK_OP(os, );
    mi.rowListChanged = true;
    mi.alignmentLengthChanged = true;
}

MaModificationInfo AddSequenceObjectsToAlignmentUtils::addObjectsToAlignment(
    U2OpStatus& os,
    MultipleSequenceAlignmentObject* msaObject,
    const QList<DNASequence>& sequenceList,
    int insertRowIndex,
    bool recheckNewSequenceAlphabetOnMismatch) {
    MaModificationInfo modInfo;
    // TODO: MaModificationInfo must be clear by default!
    modInfo.alignmentLengthChanged = false;
    modInfo.rowContentChanged = false;
    modInfo.rowListChanged = false;

    CHECK_EXT(msaObject != nullptr, os.setError(L10N::nullPointerError("MSA object")), modInfo);
    CHECK_EXT(!msaObject->isStateLocked(), os.setError(tr("Object is locked for modifications.")), modInfo);

    {  // Start of MA-object state lock.
        StateLocker stateLocker(msaObject, new StateLock("add_sequences_to_alignment"));
        const DNAAlphabet* derivedAlphabet = deriveCommonAlphabet(sequenceList, msaObject->getAlphabet(), recheckNewSequenceAlphabetOnMismatch);
        CHECK_EXT(derivedAlphabet != nullptr, os.setError(tr("Failed to derive common alphabet")), modInfo);

        U2UseCommonUserModStep modStep(msaObject->getEntityRef(), os);
        QList<U2MsaRow> rows;
        createMsaRowsFromResultSequenceList(os, msaObject, sequenceList, rows);
        CHECK_OP(os, modInfo);

        const DNAAlphabet* alphabetBefore = msaObject->getAlphabet();
        U2MsaDbi* msaDbi = modStep.getDbi()->getMsaDbi();
        addRowsToAlignment(os, modInfo, msaObject, msaDbi, rows, insertRowIndex);
        CHECK_OP(os, modInfo);

        if (msaObject->getAlphabet() != derivedAlphabet) {
            const U2EntityRef& entityRef = msaObject->getEntityRef();
            msaDbi->updateMsaAlphabet(entityRef.entityId, derivedAlphabet->getId(), os);
            CHECK_OP(os, modInfo);
            modInfo.alphabetChanged = alphabetBefore != msaObject->getAlphabet();
        }

    }  // End of MA-object state lock.

    msaObject->updateCachedMultipleAlignment(modInfo);
    return modInfo;
}

AddSequenceObjectsToAlignmentTask::AddSequenceObjectsToAlignmentTask(MultipleSequenceAlignmentObject* obj,
                                                                     const QList<DNASequence>& sequenceList,
                                                                     int insertMaRowIndex,
                                                                     bool recheckNewSequenceAlphabetOnMismatch)
    : Task(tr("Add sequences to alignment task"), TaskFlags(TaskFlags_FOSE_COSC) | TaskFlag_RunInMainThread),
      sequenceList(sequenceList),
      insertMaRowIndex(insertMaRowIndex),
      maObj(obj),
      recheckNewSequenceAlphabetOnMismatch(recheckNewSequenceAlphabetOnMismatch) {
    // Reset modification info.
    mi.alignmentLengthChanged = false;
    mi.rowContentChanged = false;
    mi.rowListChanged = false;
}

void AddSequenceObjectsToAlignmentTask::run() {
    mi = AddSequenceObjectsToAlignmentUtils::addObjectsToAlignment(stateInfo, maObj, sequenceList, insertMaRowIndex, recheckNewSequenceAlphabetOnMismatch);
}

const MaModificationInfo& AddSequenceObjectsToAlignmentTask::getMaModificationInfo() const {
    return mi;
}

AddSequencesFromFilesToAlignmentTask::AddSequencesFromFilesToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QStringList& urls, int insertRowIndex)
    : AddSequenceObjectsToAlignmentTask(obj, QList<DNASequence>(), insertRowIndex, false), urlList(urls), loadTask(nullptr) {
    connect(maObj, SIGNAL(si_invalidateAlignmentObject()), SLOT(sl_onCancel()));
}

void AddSequencesFromFilesToAlignmentTask::prepare() {
    AddSequenceObjectsToAlignmentTask::prepare();
    for (const QString& fileWithSequencesUrl : qAsConst(urlList)) {
        QList<FormatDetectionResult> detectedFormats = DocumentUtils::detectFormat(fileWithSequencesUrl);
        if (detectedFormats.isEmpty()) {
            setError(tr("Unsupported document format: %1").arg(fileWithSequencesUrl));
            continue;
        }
        IOAdapterFactory* factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        DocumentFormat* format = detectedFormats.first().format;
        loadTask = new LoadDocumentTask(format->getFormatId(), fileWithSequencesUrl, factory);
        addSubTask(loadTask);
    }
}

QList<Task*> AddSequencesFromFilesToAlignmentTask::onSubTaskFinished(Task* subTask) {
    propagateSubtaskError();
    CHECK_OP(stateInfo, {});

    auto loadDocumentSubTask = qobject_cast<LoadDocumentTask*>(subTask);
    SAFE_POINT(loadDocumentSubTask != nullptr, "Not a LoadDocumentTask", {});

    Document* doc = loadDocumentSubTask->getDocument();
    QList<GObject*> sequenceObjects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    for (const GObject* object : qAsConst(sequenceObjects)) {
        auto sequenceObject = qobject_cast<const U2SequenceObject*>(object);
        SAFE_POINT(sequenceObject != nullptr, "Not a sequence object:" + object->getGObjectName(), {});
        DNASequence sequence = sequenceObject->getWholeSequence(stateInfo);
        CHECK(!stateInfo.isCoR(), {});
        sequenceList.append(sequence);
    }

    QList<GObject*> msaObjects = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    for (const GObject* object : qAsConst(msaObjects)) {
        auto msaObject = qobject_cast<const MultipleSequenceAlignmentObject*>(object);
        SAFE_POINT(msaObject != nullptr, "Not an alignment object:" + object->getGObjectName(), {});
        for (int i = 0; i < msaObject->getRowCount(); i++) {
            // Keep all gaps, so alignment sequences are added in the 'aligned' form.
            MultipleAlignmentRow row = msaObject->getRow(i);
            DNASequence sequence(row->getName(), row->getSequenceWithGaps(true, true), msaObject->getAlphabet());
            sequenceList.append(sequence);
        }
    }

    return {};
}

////////////////////////////////////////////////////////////////////////////////
// AddSequencesFromDocumentsToAlignmentTask
AddSequencesFromDocumentsToAlignmentTask::AddSequencesFromDocumentsToAlignmentTask(MultipleSequenceAlignmentObject* obj,
                                                                                   const QList<Document*>& docs,
                                                                                   int insertMaRowIndex,
                                                                                   bool recheckNewSequenceAlphabets)
    : AddSequenceObjectsToAlignmentTask(obj, {}, insertMaRowIndex, recheckNewSequenceAlphabets), docs(docs) {
}

void AddSequencesFromDocumentsToAlignmentTask::prepare() {
    AddSequenceObjectsToAlignmentTask::prepare();
    sequenceList = PasteUtils::getSequences(docs, stateInfo);
    if (sequenceList.isEmpty()) {
        stateInfo.setError("No valid sequences found to add to the alignment.");
        return;
    }
}

////////////////////////////////////////////////////////////////////////////////
// AddSequencesFromFilesToAlignmentTask
void AddSequencesFromFilesToAlignmentTask::sl_onCancel() {
    if (loadTask != nullptr && !loadTask->isFinished() && !loadTask->isCanceled()) {
        loadTask->cancel();
    }
}

}  // namespace U2
