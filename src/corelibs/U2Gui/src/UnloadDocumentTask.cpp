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

#include <QApplication>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/RemoveDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Core/QObjectScopedPointer.h>

#include "UnloadDocumentTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// unload document

const QString UnloadDocumentTask::ACTIVE_VIEW_ERROR = QCoreApplication::translate("UnloadDocumentTask", "There is an active view with the document content.");

UnloadDocumentTask::UnloadDocumentTask(Document* _doc, bool save)
    : Task(tr("Unload document task: %1").arg(_doc->getURLString()), TaskFlag_NoRun),
      doc(_doc),
      saveTask(NULL),
      lock(NULL)
{
    GCOUNTER(cvar, tvar, "UnloadDocumentTask");
    lock = new StateLock(Document::UNLOAD_LOCK_NAME, StateLockFlag_LiveLock);
    lock->setParent(this);
    doc->lockState(lock);

    if (save) {
        saveTask = new SaveDocumentTask(doc);
        addSubTask(saveTask);
    }
    setVerboseLogMode(true);
}

Task::ReportResult UnloadDocumentTask::report() {
    if (doc.isNull() || !doc->isLoaded()) {
        if (!doc.isNull()) {
            doc->unlockState(lock);
            doc->setModified(false);
        }
        return Task::ReportResult_Finished;
    }
    propagateSubtaskError();
    QString errPrefix = tr("Document '%1' can't be unloaded: ").arg(doc->getName());
    if (hasError()) {
        assert(saveTask!=NULL);
        coreLog.error(errPrefix +  tr("save failed!"));
        doc->unlockState(lock);
        return Task::ReportResult_Finished;
    }
    QString error = checkSafeUnload(doc);
    if (!error.isEmpty()) {
        stateInfo.setError(errPrefix + error);
        coreLog.error(stateInfo.getError());
        doc->unlockState(lock);
        return Task::ReportResult_Finished;
    }
    bool ok = doc->unload(doc->isDocumentOwnsDbiResources());
    CHECK_EXT(ok, stateInfo.setError(errPrefix + tr("unexpected error")),
        Task::ReportResult_Finished);
    doc->unlockState(lock);
    doc->setModified(false);
    return Task::ReportResult_Finished;
}

QList<Task *> UnloadDocumentTask::runUnloadTaskHelper(const QList<Document*>& docs, UnloadDocumentTask_SaveMode sm) {
    QMap<Document*, QString> failedToUnload;

    // document can be unloaded if there are no active view with this doc + it's not state locked by user
    TriState saveAll = sm == UnloadDocumentTask_SaveMode_Ask ? TriState_Unknown :
        (sm == UnloadDocumentTask_SaveMode_NotSave ? TriState_No : TriState_Yes);

    QList<Task *> result;

    foreach(Document* doc, docs) {
        QString err = checkSafeUnload(doc);
        if (err == ACTIVE_VIEW_ERROR){
            QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes) | QMessageBox::No;
            QMessageBox::StandardButton res = QMessageBox::question(NULL,
                tr("Question?"), tr("Close views for document: %1").arg(doc->getURLString()),
                buttons, QMessageBox::Yes);
            if(res == QMessageBox::Yes ){
                foreach(GObjectViewWindow *v,  GObjectViewUtils::findViewsWithAnyOfObjects(doc->getObjects())){
                    v->closeView();
                }
            }
        }

        err = checkSafeUnload(doc);
        if (!err.isEmpty()) {
            failedToUnload[doc] = err;
            continue;
        }
        bool saveCurrentDoc = doc->isModified() && saveAll == TriState_Yes;
        if (doc->isModified() && saveAll == TriState_Unknown) {

            QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes) | QMessageBox::No;
            if (docs.size() > 1) {
                buttons = buttons | QMessageBox::YesToAll | QMessageBox::NoToAll;
            }

            QMessageBox::StandardButton res = saveAll ? QMessageBox::YesToAll : QMessageBox::question(NULL,
                tr("Question?"), tr("Save document: %1").arg(doc->getURLString()),
                buttons, QMessageBox::Yes);

            if (res == QMessageBox::NoToAll) {
                saveAll = TriState_No;
            } else  if (res == QMessageBox::YesToAll) {
                saveAll = TriState_Yes;
                saveCurrentDoc = true;
            } else if (res == QMessageBox::No) {
                saveCurrentDoc = false;
            } else {
                assert(res == QMessageBox::Yes);
                saveCurrentDoc = true;
            }
        }
        result.append(new UnloadDocumentTask(doc, saveCurrentDoc));
    }

    if (!failedToUnload.isEmpty()) {
        QString text = tr("Failed to unload document")+"<br>";
        foreach(Document* doc, failedToUnload.keys()) {
            QString err = failedToUnload[doc];
            text+=doc->getName()+" : " + err + "<br>";
            coreLog.error(tr("Failed to unload document: %1, error: %2").arg(doc->getName()).arg(err));
        }
        QObjectScopedPointer<QMessageBox> warning = new QMessageBox(QMessageBox::Warning, tr("Warning"), text, QMessageBox::Ok, QApplication::activeWindow());
        warning->setObjectName("UnloadWarning");
        warning->exec();
        CHECK(!warning.isNull(), result);
    }
    return result;
}

QString UnloadDocumentTask::checkSafeUnload(Document* doc) {
    bool hasViews = !GObjectViewUtils::findViewsWithAnyOfObjects(doc->getObjects()).isEmpty();
    if (hasViews) {
        return ACTIVE_VIEW_ERROR;
    }

    QList<StateLock*> locks = doc->findLocks(StateLockableTreeFlags_ItemAndChildren, StateLockFlag_LiveLock);
    bool liveLocked = (locks.size() != 1) && (locks.size() != 0);
    if (locks.size() == 1) {
        liveLocked = (locks.first()->getUserDesc() != Document::UNLOAD_LOCK_NAME);
    }
    if (liveLocked) {
        return tr("The document is locked by some algorithm.");
    }

    return QString();
}

ReloadDocumentTask::ReloadDocumentTask( Document *d )
    : Task( "Reloading given document", TaskFlags_NR_FOSE_COSC ), doc( d ), url( d->getURL( ) ),
    removeDocTask( NULL ), openDocTask( NULL )
{
    GCOUNTER(cvar, tvar, "ReloadDocumentTask");
}

void ReloadDocumentTask::prepare( ) {
    saveObjectRelationsFromDoc( );

    removeDocTask = new RemoveMultipleDocumentsTask( AppContext::getProject( ),
        QList<Document *>( ) << doc, false, false );
    addSubTask( removeDocTask );
}

QList<Task *> ReloadDocumentTask::onSubTaskFinished( Task* subTask ) {
    QList<Task *> res;

    if ( subTask == removeDocTask ) {
        openDocTask = AppContext::getProjectLoader( )->openWithProjectTask( url );
        if ( openDocTask != NULL ) {
            res.append( openDocTask );
        }
    } else if ( subTask == openDocTask ) {
        if (openDocTask->hasError()) {
            return res;
        }
        Project *currentProj = AppContext::getProject( );
        SAFE_POINT( NULL != currentProj, "Invalid project state!", res );
        doc = currentProj->findDocumentByURL( url );
        SAFE_POINT( NULL != doc, "Reloaded document not found!", res );
        SAFE_POINT(doc->isLoaded(), "The reloaded document unexpectedly has unloaded state", res );
        restoreObjectRelationsForDoc( );

        // annotation files without seq.reference can be opened after adding relations
        if (GObjectViewUtils::findViewsWithAnyOfObjects(doc->getObjects()).isEmpty()) {
            res.append(new OpenViewTask(doc));
        }
    }

    return res;
}

void ReloadDocumentTask::saveObjectRelationsFromDoc( ) {
    foreach ( GObject *curObj, doc->getObjects( ) ) {
        const QList<GObjectRelation> curObjRelations = curObj->getObjectRelations( );
        if ( !curObjRelations.isEmpty( ) ) {
            const QString curObjName = curObj->getGObjectName( );
            if ( savedObjectRelations.contains( curObjName ) ) {
                coreLog.error( "Objects with same names detected during saving of object relations!" );
            }
            foreach ( const GObjectRelation &relation, curObjRelations ) {
                if ( doc->getURLString( ) != relation.getDocURL( ) ) { // don't save relations within a single object
                    savedObjectRelations.insert( curObjName, relation );
                }
            }
        }
    }
}

void ReloadDocumentTask::restoreObjectRelationsForDoc( ) {
    foreach ( GObject *curObj, doc->getObjects( ) ) {
        const QString curObjName = curObj->getGObjectName( );
        if ( !savedObjectRelations.contains( curObjName ) ) {
            continue;
        }
        restoreObjectRelationsForObject( curObj, savedObjectRelations.values( curObjName ) );
    }
}

void ReloadDocumentTask::restoreObjectRelationsForObject(GObject *obj, const QList<GObjectRelation> &relations) {
    Project *currentProj = AppContext::getProject();
    SAFE_POINT(NULL != currentProj, "Invalid project state!", );

    obj->setObjectRelations(QList<GObjectRelation>());

    foreach (const GObjectRelation &relation, relations) {
        Document *relatedDoc = currentProj->findDocumentByURL(relation.ref.docUrl);
        if (NULL == relatedDoc) {
            continue;
        }
        GObject *relatedObj = relatedDoc->findGObjectByName(relation.ref.objName);
        if (NULL != relatedObj && relatedObj->getGObjectType() == relation.ref.objType) {
            obj->addObjectRelation(relation);
        }
    }
}

} // namespace U2
