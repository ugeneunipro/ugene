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

#include "RemoveDocumentTask.h"

#include "SaveDocumentTask.h"

#include <U2Core/ProjectModel.h>
#include <U2Core/AppContext.h>


namespace U2 {

RemoveMultipleDocumentsTask::RemoveMultipleDocumentsTask(Project* _p, const QList<Document*>& _docs, bool _saveModifiedDocs, bool _useGUI)
    : Task(tr("Remove document"), TaskFlags(TaskFlag_NoRun) | TaskFlag_CancelOnSubtaskCancel), p(_p), saveModifiedDocs(_saveModifiedDocs), useGUI(_useGUI) {
    assert(!_docs.empty());
    assert(p != NULL);

    foreach(Document* d, _docs) {
        docPtrs.append(d);
    }
    lock = new StateLock(getTaskName());
}

RemoveMultipleDocumentsTask::~RemoveMultipleDocumentsTask() {
    assert(lock == NULL);
}

void RemoveMultipleDocumentsTask::prepare() {
    p->lockState(lock);
    if (p->isTreeItemModified() && saveModifiedDocs) {
        QList<Document*> docs;
        foreach(Document* d, docPtrs) {
            if (d != NULL) {
                docs.append(d);
            }
        }
        QList<Document*> modifiedDocs = SaveMultipleDocuments::findModifiedDocuments(docs);
        if (!modifiedDocs.isEmpty()) {
            addSubTask(new SaveMultipleDocuments(modifiedDocs, useGUI));
        }
    }


}


Task::ReportResult RemoveMultipleDocumentsTask::report() {
    if (lock != NULL) {
        assert(!p.isNull());
        p->unlockState(lock);
        delete lock;
        lock = NULL;

        Task* t = getSubtaskWithErrors();
        if (t != NULL) {
            stateInfo.setError(t->getError());
            return Task::ReportResult_Finished;
        }
    }

    if (p.isNull()) {
        return Task::ReportResult_Finished;
    }

    if (p->isStateLocked()) {
        return Task::ReportResult_CallMeAgain;
    }

    if (isCanceled()) {
        return ReportResult_Finished;
    }

    foreach(Document* doc, docPtrs) {
        if (doc != NULL) {
            // check for "stay-alive" locked objects
            if (doc->hasLocks(StateLockableTreeFlags_ItemAndChildren, StateLockFlag_LiveLock)) {
                setError(tr("Cannot remove document %1, since it is locked by some task.").arg(doc->getName()));
                continue;
            } else {
                p->removeDocument(doc);
            }
        }
    }


    return Task::ReportResult_Finished;
}


}//namespace
