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

#include "AlignSequencesToAlignmentTask.h"

#include <QDir>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>

#include <U2Gui/ProjectView.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/MSAEditor.h>

namespace U2 {

const int LoadSequencesTask::maxErrorListSize = 5;

/************************************************************************/
/* SequencesExtractor */
/************************************************************************/
SequenceObjectsExtractor::SequenceObjectsExtractor()
    : seqsAlphabet(nullptr),
      sequencesMaxLength(0) {
}

void SequenceObjectsExtractor::setAlphabet(const DNAAlphabet* newAlphabet) {
    seqsAlphabet = newAlphabet;
}

void SequenceObjectsExtractor::extractSequencesFromDocument(Document* doc) {
    extractSequencesFromObjects(doc->getObjects());
}

void SequenceObjectsExtractor::extractSequencesFromObjects(const QList<GObject*>& objects) {
    for (GObject* object : qAsConst(objects)) {
        Document* doc = object->getDocument();
        if (doc != nullptr) {
            if (!usedDocuments.contains(doc)) {
                usedDocuments << doc;
            }
        }

        if (object->getGObjectType() == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
            auto curObj = qobject_cast<MultipleSequenceAlignmentObject*>(object);
            SAFE_POINT(curObj != nullptr, "MultipleSequenceAlignmentObject is null", );

            checkAlphabet(curObj->getAlphabet(), curObj->getGObjectName());
            sequencesMaxLength = qMax(sequencesMaxLength, curObj->getLength());

            QList<MultipleSequenceAlignmentRow> msaRows = curObj->getMsa()->getMsaRows();
            for (const MultipleSequenceAlignmentRow& row : qAsConst(msaRows)) {
                U2EntityRef seqRef(curObj->getEntityRef().dbiRef, row->getRowDbInfo().sequenceId);
                sequenceRefs << seqRef;
                sequenceNames << row->getName();
            }
        } else if (object->getGObjectType() == GObjectTypes::SEQUENCE) {
            auto dnaObj = qobject_cast<U2SequenceObject*>(object);
            SAFE_POINT(dnaObj != nullptr, "U2SequenceObject is null", );
            sequencesMaxLength = qMax(sequencesMaxLength, dnaObj->getSequenceLength());
            sequenceRefs << dnaObj->getEntityRef();
            sequenceNames << dnaObj->getSequenceName();

            checkAlphabet(dnaObj->getAlphabet(), dnaObj->getGObjectName());
        }
    }
}

void SequenceObjectsExtractor::checkAlphabet(const DNAAlphabet* newAlphabet, const QString& objectName) {
    if (seqsAlphabet == nullptr) {
        seqsAlphabet = newAlphabet;
    } else {
        const DNAAlphabet* commonAlphabet = U2AlphabetUtils::deriveCommonAlphabet(newAlphabet, seqsAlphabet);
        if (commonAlphabet == nullptr) {
            errorList << objectName;
        } else {
            seqsAlphabet = commonAlphabet;
        }
    }
}

const QStringList& SequenceObjectsExtractor::getErrorList() const {
    return errorList;
}

const DNAAlphabet* SequenceObjectsExtractor::getAlphabet() const {
    return seqsAlphabet;
}

const QList<U2EntityRef>& SequenceObjectsExtractor::getSequenceRefs() const {
    return sequenceRefs;
}

const QStringList& SequenceObjectsExtractor::getSequenceNames() const {
    return sequenceNames;
}
qint64 SequenceObjectsExtractor::getMaxSequencesLength() const {
    return sequencesMaxLength;
}

const QList<Document*>& SequenceObjectsExtractor::getUsedDocuments() const {
    return usedDocuments;
}

/************************************************************************/
/* LoadSequencesTask */
/************************************************************************/
LoadSequencesTask::LoadSequencesTask(const DNAAlphabet* msaAlphabet, const QStringList& fileWithSequencesUrls)
    : Task(tr("Load sequences task"), TaskFlag_NoRun), msaAlphabet(msaAlphabet), urls(fileWithSequencesUrls), extractor() {
    assert(!fileWithSequencesUrls.isEmpty());
    extractor.setAlphabet(msaAlphabet);
}

void LoadSequencesTask::prepare() {
    foreach (const QString& fileWithSequencesUrl, urls) {
        QList<FormatDetectionResult> detectedFormats = DocumentUtils::detectFormat(fileWithSequencesUrl);
        if (!detectedFormats.isEmpty()) {
            LoadDocumentTask* loadTask = LoadDocumentTask::getDefaultLoadDocTask(fileWithSequencesUrl, {{DocumentFormat::STRONG_FORMAT_ACCORDANCE, true}});
            if (loadTask != nullptr) {
                addSubTask(loadTask);
            }
        } else {
            if (QFile(fileWithSequencesUrl).size() == 0) {
                setError(tr("The file is empty."));
            } else {
                setError(tr("Unknown format"));
            }
        }
    }
}

QList<Task*> LoadSequencesTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;

    propagateSubtaskError();
    if (subTask->isCanceled() || isCanceled() || hasError()) {
        return subTasks;
    }

    auto loadTask = qobject_cast<LoadDocumentTask*>(subTask);
    SAFE_POINT(loadTask != nullptr, "LoadDocumentTask is null", subTasks);

    CHECK(loadTask->getDocument() != nullptr, subTasks);
    extractor.extractSequencesFromDocument(loadTask->getDocument());
    return subTasks;
}

void LoadSequencesTask::setupError() {
    CHECK(!extractor.getErrorList().isEmpty(), );

    QStringList smallList = extractor.getErrorList().mid(0, maxErrorListSize);
    QString error = tr("Some sequences have wrong alphabet: ");
    error += smallList.join(", ");
    if (smallList.size() < extractor.getErrorList().size()) {
        error += tr(" and others");
    }
    setError(error);
}

Task::ReportResult LoadSequencesTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    if (!extractor.getErrorList().isEmpty()) {
        setupError();
    }
    if (extractor.getSequenceRefs().isEmpty()) {
        QString filesSeparator(", ");
        setError(tr("There are no sequences to align in the document(s): %1").arg(urls.join(filesSeparator)));
        return ReportResult_Finished;
    }
    if (U2AlphabetUtils::deriveCommonAlphabet(extractor.getAlphabet(), msaAlphabet) == nullptr) {
        setError(tr("Sequences have incompatible alphabets"));
    }
    return ReportResult_Finished;
}

const SequenceObjectsExtractor& LoadSequencesTask::getExtractor() const {
    return extractor;
}

/************************************************************************/
/* AlignSequencesToAlignmentTask */
/************************************************************************/
AlignSequencesToAlignmentTask::AlignSequencesToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QString& algorithmId, const SequenceObjectsExtractor& extractor)
    : Task(tr("Align sequences to alignment task"), TaskFlags_NR_FOSE_COSC), maObjPointer(obj), stateLock(nullptr), docStateLock(nullptr),
      sequencesMaxLength(extractor.getMaxSequencesLength()), sequenceObjectsExtractor(extractor) {
    settings.addAsFragments = sequencesMaxLength < 100 && maObjPointer->getLength() / sequencesMaxLength > 3;
    settings.msaRef = maObjPointer->getEntityRef();
    settings.inNewWindow = false;
    settings.algorithmId = algorithmId;
    settings.addedSequencesRefs = extractor.getSequenceRefs();
    settings.addedSequencesNames = extractor.getSequenceNames();
    settings.maxSequenceLength = extractor.getMaxSequencesLength();
    settings.alphabet = extractor.getAlphabet()->getId();
    usedDocuments = extractor.getUsedDocuments();
    if (obj != nullptr) {
        initialMsaAlphabet = obj->getAlphabet();
    }
}

void AlignSequencesToAlignmentTask::prepare() {
    if (maObjPointer.isNull()) {
        stateInfo.setError(tr("Object is empty."));
        return;
    }

    if (maObjPointer->isStateLocked()) {
        stateInfo.setError(tr("Object is locked for modifications."));
        return;
    }
    Document* document = maObjPointer->getDocument();
    if (document != nullptr) {
        docStateLock = new StateLock("Lock MSA for align sequences to alignment", StateLockFlag_LiveLock);
        document->lockState(docStateLock);
        foreach (Document* curDoc, usedDocuments) {
            curDoc->lockState(docStateLock);
        }
    }

    stateLock = new StateLock("Align sequences to alignment", StateLockFlag_LiveLock);
    maObjPointer->lockState(stateLock);

    AlignmentAlgorithmsRegistry* alignmentRegistry = AppContext::getAlignmentAlgorithmsRegistry();
    SAFE_POINT(alignmentRegistry != nullptr, "AlignmentAlgorithmsRegistry is NULL.", );
    AlignmentAlgorithm* addAlgorithm = alignmentRegistry->getAlgorithm(settings.algorithmId);
    SAFE_POINT_EXT(addAlgorithm != nullptr, setError(QString("Can not find \"%1\" algorithm").arg(settings.algorithmId)), );
    addSubTask(addAlgorithm->getFactory()->getTaskInstance(&settings));
}

Task::ReportResult AlignSequencesToAlignmentTask::report() {
    if (stateLock != nullptr) {
        maObjPointer->unlockState(stateLock);
        delete stateLock;
    }

    if (docStateLock != nullptr) {
        Document* document = maObjPointer->getDocument();
        document->unlockState(docStateLock);

        for (Document* curDoc : qAsConst(usedDocuments)) {
            curDoc->unlockState(docStateLock);
        }

        delete docStateLock;
    }
    MaModificationInfo mi;
    mi.alphabetChanged = sequenceObjectsExtractor.getAlphabet()->getId() != initialMsaAlphabet->getId();
    mi.rowListChanged = true;
    if (!hasError() && !isCanceled()) {
        maObjPointer->updateCachedMultipleAlignment(mi);
    }

    return ReportResult_Finished;
}

/************************************************************************/
/* LoadSequencesAndAlignToAlignmentTask */
/************************************************************************/
LoadSequencesAndAlignToAlignmentTask::LoadSequencesAndAlignToAlignmentTask(MultipleSequenceAlignmentObject* obj, const QString& _algorithmId, const QStringList& urls)
    : Task(tr("Load sequences and add to alignment task"), TaskFlag_NoRun | TaskFlag_CollectChildrenWarnings),
      urls(urls), algorithmId(_algorithmId), maObjPointer(obj), loadSequencesTask(nullptr) {
}

void LoadSequencesAndAlignToAlignmentTask::prepare() {
    SAFE_POINT_EXT(!maObjPointer.isNull(), setError(tr("Alignment object is null")), );

    loadSequencesTask = new LoadSequencesTask(maObjPointer->getAlphabet(), urls);
    loadSequencesTask->setSubtaskProgressWeight(5);
    addSubTask(loadSequencesTask);
}

QList<Task*> LoadSequencesAndAlignToAlignmentTask::onSubTaskFinished(Task* subTask) {
    propagateSubtaskError();
    if (subTask != loadSequencesTask || loadSequencesTask->hasError() || loadSequencesTask->isCanceled()) {
        return QList<Task*>();
    }
    if (maObjPointer.isNull()) {
        setError(tr("Alignment object was removed"));
        return QList<Task*>();
    }
    QList<Task*> subTasks;
    auto alignSequencesToAlignmentTask = new AlignSequencesToAlignmentTask(maObjPointer, algorithmId, loadSequencesTask->getExtractor());
    alignSequencesToAlignmentTask->setSubtaskProgressWeight(95);
    subTasks << alignSequencesToAlignmentTask;
    return subTasks;
}

bool LoadSequencesAndAlignToAlignmentTask::propagateSubtaskError() {
    if (hasError()) {
        return true;
    }
    Task* badChild = getSubtaskWithErrors();
    if (badChild != nullptr) {
        stateInfo.setError(tr("Data from the \"%1\" file can't be alignment to the \"%2\" alignment - %3")
                               .arg(QFileInfo(urls.first()).fileName())
                               .arg(maObjPointer.isNull() ? "null" : maObjPointer->getGObjectName())
                               .arg(badChild->getError().toLower()));
    }
    return stateInfo.hasError();
}

}  // namespace U2
