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

#include <U2Core/AppContext.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2MsaDbi.h>

#include "AddSequencesToAlignmentTask.h"

namespace U2 {

const int AddSequenceObjectsToAlignmentTask::maxErrorListSize = 5;

AddSequenceObjectsToAlignmentTask::AddSequenceObjectsToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QList<DNASequence>& seqList, bool recheckNewSequenceAlphabetOnMismatch)
    : Task("Add sequences to alignment task", TaskFlags(TaskFlags_FOSE_COSC)),
    seqList(seqList),
    maObj(obj),
    stateLock(NULL),
    msaAlphabet(maObj->getAlphabet()),
    dbi(NULL),
    modStep(NULL),
    recheckNewSequenceAlphabetOnMismatch(recheckNewSequenceAlphabetOnMismatch) {
    entityRef = maObj->getEntityRef();

    // reset modification inf.
    mi.alignmentLengthChanged = false;
    mi.rowContentChanged = false;
    mi.rowListChanged = false;
}

void AddSequenceObjectsToAlignmentTask::prepare() {
    if (maObj.isNull()) {
        stateInfo.setError(tr("Object is empty."));
        return;
    }

    if (maObj->isStateLocked()) {
        stateInfo.setError(tr("Object is locked for modifications."));
        return;
    }

    stateLock = new StateLock("Adding_files_to_alignment");
    maObj->lockState(stateLock);
    processObjectsAndSetResultingAlphabet();

    // Create user mod step
    modStep = new U2UseCommonUserModStep(entityRef, stateInfo);
    CHECK_OP(stateInfo, );
    dbi = modStep->getDbi()->getMsaDbi();
}

void AddSequenceObjectsToAlignmentTask::run() {
    if (seqList.isEmpty()) {
        return;
    }
    QList<U2MsaRow> rows;
    qint64 maxLength = createRows(rows);
    if (isCanceled() || hasError() || rows.isEmpty()) {
        return;
    }
    CHECK_OP(stateInfo, );
    addRows(rows, maxLength);
    CHECK_OP(stateInfo, );
    updateAlphabet();
}

void AddSequenceObjectsToAlignmentTask::processObjectsAndSetResultingAlphabet() {
    QList<DNASequence>    newSeqList;
    foreach(const DNASequence& dnaObj, seqList) {
        const DNAAlphabet* newAlphabet = U2AlphabetUtils::deriveCommonAlphabet(dnaObj.alphabet, msaAlphabet);
        if (newAlphabet != NULL) {
            if (!msaAlphabet->isRaw() && msaAlphabet != newAlphabet && recheckNewSequenceAlphabetOnMismatch) {
                QList<const DNAAlphabet*> allValidAlphabets = U2AlphabetUtils::findAllAlphabets(dnaObj.constSequence());
                if (allValidAlphabets.contains(msaAlphabet)) {
                    newAlphabet = msaAlphabet;
                }
            }
            msaAlphabet = newAlphabet;
            newSeqList.append(dnaObj);
        } else {
            errorList << dnaObj.getName();
        }
    }
    seqList = newSeqList;
}

Task::ReportResult AddSequenceObjectsToAlignmentTask::report() {
    delete modStep;
    releaseLock();

    if (isCanceled() || hasError() || seqList.size() == 0) {
        return ReportResult_Finished;
    }

    // Update object
    maObj->updateCachedMultipleAlignment(mi);

    if (!errorList.isEmpty()) {
        setupError();
    }
    return ReportResult_Finished;
}

qint64 AddSequenceObjectsToAlignmentTask::createRows(QList<U2MsaRow> &rows) {
    qint64 maxLength = 0;
    U2EntityRef entityRef = maObj->getEntityRef();
    foreach(const DNASequence& seqObj, seqList) {
        if (isCanceled() || hasError()) {
            return 0;
        }
        U2MsaRow row = MSAUtils::copyRowFromSequence(seqObj, entityRef.dbiRef, stateInfo);
        if (0 < row.gend) {
            rows << row;
            maxLength = qMax(maxLength, (qint64)seqObj.length());
        }
        CHECK_OP(stateInfo, 0);
    }
    return maxLength;
}

void AddSequenceObjectsToAlignmentTask::addRows(QList<U2MsaRow> &rows, qint64 maxLength) {
    if (rows.isEmpty()) {
        return;
    }

    dbi->addRows(entityRef.entityId, rows, stateInfo);
    CHECK_OP(stateInfo, );

    mi.rowListChanged = true;
    mi.alignmentLengthChanged = true;

    if (maxLength > maObj->getLength()) {
        dbi->updateMsaLength(entityRef.entityId, maxLength, stateInfo);
        CHECK_OP(stateInfo, );
    }
}

void AddSequenceObjectsToAlignmentTask::updateAlphabet() {
    if (maObj->getAlphabet() != msaAlphabet) {
        SAFE_POINT(NULL != msaAlphabet, "NULL result alphabet", );
        dbi->updateMsaAlphabet(entityRef.entityId, msaAlphabet->getId(), stateInfo);
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

void AddSequenceObjectsToAlignmentTask::releaseLock() {
    if (stateLock != NULL) {
        if (maObj != NULL) {
            maObj->unlockState(stateLock);
        }
        delete stateLock;
        stateLock = NULL;
    }
}

AddSequencesFromFilesToAlignmentTask::AddSequencesFromFilesToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QStringList& urls)
    : AddSequenceObjectsToAlignmentTask(obj, QList<DNASequence>(), false), urlList(urls), loadTask(NULL) {
    connect(maObj, SIGNAL(si_invalidateAlignmentObject()), SLOT(sl_onCancel()));
}

void AddSequencesFromFilesToAlignmentTask::prepare() {
    AddSequenceObjectsToAlignmentTask::prepare();
    foreach(const QString& fileWithSequencesUrl, urlList) {
        QList<FormatDetectionResult> detectedFormats = DocumentUtils::detectFormat(fileWithSequencesUrl);
        if (!detectedFormats.isEmpty()) {
            IOAdapterFactory* factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
            DocumentFormat* format = detectedFormats.first().format;
            loadTask = new LoadDocumentTask(format->getFormatId(), fileWithSequencesUrl, factory);
            addSubTask(loadTask);
        } else {
            setError("Unknown format");
        }
    }
}

QList<Task*> AddSequencesFromFilesToAlignmentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;

    propagateSubtaskError();
    if (isCanceled() || hasError()) {
        return subTasks;
    }

    LoadDocumentTask* loadDocumentSubTask = qobject_cast<LoadDocumentTask*>(subTask);
    SAFE_POINT(loadDocumentSubTask != NULL, "loadTask is NULL", subTasks);
    Document* doc = loadDocumentSubTask->getDocument();
    foreach(GObject *seqObj, doc->findGObjectByType(GObjectTypes::SEQUENCE)) {
        U2SequenceObject *casted = qobject_cast<U2SequenceObject*>(seqObj);
        SAFE_POINT(casted != NULL, "Cast to U2SequenceObject failed", subTasks);
        DNASequence seq = casted->getWholeSequence(stateInfo);
        CHECK(!stateInfo.isCoR(), subTasks);
        seq.alphabet = casted->getAlphabet();
        seqList.append(seq);
    }
    processObjectsAndSetResultingAlphabet();
    return subTasks;
}


////////////////////////////////////////////////////////////////////////////////
//AddSequencesFromDocumentsToAlignmentTask
AddSequencesFromDocumentsToAlignmentTask::AddSequencesFromDocumentsToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QList<Document*>& docs, bool recheckNewSequenceAlphabets)
    : AddSequenceObjectsToAlignmentTask(obj, QList<DNASequence>(), recheckNewSequenceAlphabets), docs(docs) {}

void AddSequencesFromDocumentsToAlignmentTask::prepare() {
    AddSequenceObjectsToAlignmentTask::prepare();
    seqList = PasteUtils::getSequences(docs, stateInfo);
    if (seqList.isEmpty()) {
        stateInfo.setError("No valid sequences found to add to the alignment.");
        return;
    }
    processObjectsAndSetResultingAlphabet();
}

void AddSequencesFromFilesToAlignmentTask::sl_onCancel() {
    if (loadTask != NULL && !loadTask->isFinished() && !loadTask->isCanceled()) {
        loadTask->cancel();
    }
    releaseLock();
}

}

