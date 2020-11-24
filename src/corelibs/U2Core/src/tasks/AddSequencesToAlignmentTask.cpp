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

const int AddSequenceObjectsToAlignmentTask::maxErrorListSize = 5;

AddSequenceObjectsToAlignmentTask::AddSequenceObjectsToAlignmentTask(MultipleSequenceAlignmentObject *obj,
                                                                     const QList<DNASequence> &sequenceList,
                                                                     int insertRowIndex,
                                                                     bool recheckNewSequenceAlphabetOnMismatch)
    : Task("Add sequences to alignment task", TaskFlags(TaskFlags_FOSE_COSC) | TaskFlag_RunInMainThread),
      sequenceList(sequenceList),
      insertRowIndex(insertRowIndex),
      maObj(obj),
      msaAlphabet(maObj->getAlphabet()),
      recheckNewSequenceAlphabetOnMismatch(recheckNewSequenceAlphabetOnMismatch) {
    // Reset modification info.
    mi.alignmentLengthChanged = false;
    mi.rowContentChanged = false;
    mi.rowListChanged = false;
}

void AddSequenceObjectsToAlignmentTask::run() {
    if (maObj.isNull()) {
        stateInfo.setError(tr("Object is empty."));
        return;
    }

    if (maObj->isStateLocked()) {
        stateInfo.setError(tr("Object is locked for modifications."));
        return;
    }

    {    // Start of MA-object state lock.
        StateLocker stateLocker(maObj, new StateLock("add_sequences_to_alignment"));
        QList<DNASequence> resultSequenceList = prepareResultSequenceList();

        if (resultSequenceList.isEmpty()) {
            return;
        }

        {    // Start of U2UseCommonUserModStep scope.
            U2UseCommonUserModStep modStep(maObj->getEntityRef(), stateInfo);
            U2MsaDbi *msaDbi = modStep.getDbi()->getMsaDbi();

            QList<U2MsaRow> rows;
            qint64 maxLength = createMsaRowsFromResultSequenceList(resultSequenceList, rows);
            if (isCanceled() || hasError() || rows.isEmpty() || maxLength == 0) {
                return;
            }
            CHECK_OP(stateInfo, );
            addRowsToAlignment(msaDbi, rows, maxLength);
            CHECK_OP(stateInfo, );
            updateAlphabet(msaDbi);

        }    // End of U2UseCommonUserModStep scope.
    }    // End of MA-object state lock.
    CHECK_OP(stateInfo, );

    maObj->updateCachedMultipleAlignment(mi);
    if (!errorList.isEmpty()) {
        setupError();
    }
}

QList<DNASequence> AddSequenceObjectsToAlignmentTask::prepareResultSequenceList() {
    QList<DNASequence> resultSequenceList;
    for (const DNASequence &sequence : sequenceList) {
        const DNAAlphabet *newAlphabet = U2AlphabetUtils::deriveCommonAlphabet(sequence.alphabet, msaAlphabet);
        if (newAlphabet == nullptr) {
            errorList << sequence.getName();
            continue;
        }
        if (!msaAlphabet->isRaw() && msaAlphabet != newAlphabet && recheckNewSequenceAlphabetOnMismatch) {
            QList<const DNAAlphabet *> allValidAlphabets = U2AlphabetUtils::findAllAlphabets(sequence.constSequence());
            if (allValidAlphabets.contains(msaAlphabet)) {
                newAlphabet = msaAlphabet;
            }
        }
        msaAlphabet = newAlphabet;
        resultSequenceList.append(sequence);
    }
    return sequenceList;
}

qint64 AddSequenceObjectsToAlignmentTask::createMsaRowsFromResultSequenceList(const QList<DNASequence> &inputSequenceList, QList<U2MsaRow> &resultRows) {
    U2EntityRef entityRef = maObj->getEntityRef();
    QSet<QString> usedRowNames;
    for (const MultipleAlignmentRow &row : maObj->getRows()) {
        usedRowNames.insert(row->getName());
    }
    qint64 maxLength = 0;
    for (const DNASequence &sequenceObject : inputSequenceList) {
        CHECK(!isCanceled() && !hasError(), 0);
        QString rowName = MSAUtils::rollMsaRowName(sequenceObject.getName(), usedRowNames);
        U2MsaRow row = MSAUtils::copyRowFromSequence(sequenceObject, entityRef.dbiRef, stateInfo);
        CHECK_OP(stateInfo, 0);
        if (rowName != sequenceObject.getName()) {
            U2EntityRef rowSequenceRef(entityRef.dbiRef, row.sequenceId);
            U2SequenceUtils::updateSequenceName(rowSequenceRef, rowName, stateInfo);
            CHECK_OP(stateInfo, 0);
        }
        if (row.gend > 0) {
            resultRows << row;
            maxLength = qMax(maxLength, (qint64)sequenceObject.length());
            usedRowNames.insert(rowName);
        }
    }
    return maxLength;
}

void AddSequenceObjectsToAlignmentTask::addRowsToAlignment(U2MsaDbi *msaDbi, QList<U2MsaRow> &rows, qint64 maxLength) {
    CHECK(!rows.isEmpty(), );
    const U2EntityRef &entityRef = maObj->getEntityRef();
    msaDbi->addRows(entityRef.entityId, rows, insertRowIndex, stateInfo);
    CHECK_OP(stateInfo, );

    mi.rowListChanged = true;
    mi.alignmentLengthChanged = true;

    if (maxLength > maObj->getLength()) {
        msaDbi->updateMsaLength(entityRef.entityId, maxLength, stateInfo);
    }
}

void AddSequenceObjectsToAlignmentTask::updateAlphabet(U2MsaDbi *msaDbi) {
    if (maObj->getAlphabet() != msaAlphabet) {
        SAFE_POINT(msaAlphabet != nullptr, "NULL result alphabet", );
        const U2EntityRef &entityRef = maObj->getEntityRef();
        msaDbi->updateMsaAlphabet(entityRef.entityId, msaAlphabet->getId(), stateInfo);
        CHECK_OP(stateInfo, );
        mi.alphabetChanged = true;
    }
}

void AddSequenceObjectsToAlignmentTask::setupError() {
    CHECK(!errorList.isEmpty(), );

    QStringList smallList = errorList.mid(0, maxErrorListSize);
    QString error = tr("Some sequences have wrong alphabet: ");
    error += smallList.join(", ");
    if (smallList.size() < errorList.size()) {
        error += tr(" and others");
    }
    setError(error);
}

AddSequencesFromFilesToAlignmentTask::AddSequencesFromFilesToAlignmentTask(MultipleSequenceAlignmentObject *obj, const QStringList &urls, int insertRowIndex)
    : AddSequenceObjectsToAlignmentTask(obj, QList<DNASequence>(), insertRowIndex, false), urlList(urls), loadTask(nullptr) {
    connect(maObj, SIGNAL(si_invalidateAlignmentObject()), SLOT(sl_onCancel()));
}

void AddSequencesFromFilesToAlignmentTask::prepare() {
    AddSequenceObjectsToAlignmentTask::prepare();
    for (const QString &fileWithSequencesUrl : urlList) {
        QList<FormatDetectionResult> detectedFormats = DocumentUtils::detectFormat(fileWithSequencesUrl);
        if (!detectedFormats.isEmpty()) {
            IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
            DocumentFormat *format = detectedFormats.first().format;
            loadTask = new LoadDocumentTask(format->getFormatId(), fileWithSequencesUrl, factory);
            addSubTask(loadTask);
        } else {
            setError("Unknown format");
        }
    }
}

QList<Task *> AddSequencesFromFilesToAlignmentTask::onSubTaskFinished(Task *subTask) {
    const QList<Task *> emptySubTasks;    // Helper constant. This method never returns any subtasks.

    propagateSubtaskError();
    if (isCanceled() || hasError()) {
        return emptySubTasks;
    }

    LoadDocumentTask *loadDocumentSubTask = qobject_cast<LoadDocumentTask *>(subTask);
    SAFE_POINT(loadDocumentSubTask != nullptr, "loadTask is NULL", emptySubTasks);
    Document *doc = loadDocumentSubTask->getDocument();
    for (const GObject *objects : doc->findGObjectByType(GObjectTypes::SEQUENCE)) {
        const U2SequenceObject *sequenceObject = qobject_cast<const U2SequenceObject *>(objects);
        SAFE_POINT(sequenceObject != nullptr, "Cast to U2SequenceObject failed", emptySubTasks);
        DNASequence sequence = sequenceObject->getWholeSequence(stateInfo);
        CHECK(!stateInfo.isCoR(), emptySubTasks);
        sequence.alphabet = sequenceObject->getAlphabet();
        sequenceList.append(sequence);
    }
    return emptySubTasks;
}

////////////////////////////////////////////////////////////////////////////////
// AddSequencesFromDocumentsToAlignmentTask
AddSequencesFromDocumentsToAlignmentTask::AddSequencesFromDocumentsToAlignmentTask(MultipleSequenceAlignmentObject *obj,
                                                                                   const QList<Document *> &docs,
                                                                                   int insertRowIndex,
                                                                                   bool recheckNewSequenceAlphabets)
    : AddSequenceObjectsToAlignmentTask(obj, QList<DNASequence>(), insertRowIndex, recheckNewSequenceAlphabets), docs(docs) {
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

}    // namespace U2
