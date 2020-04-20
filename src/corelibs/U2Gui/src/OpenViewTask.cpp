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

#include "OpenViewTask.h"

#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Log.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/GHints.h>
#include <U2Core/AppResources.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/LoadRemoteDocumentTask.h>

#include <U2Gui/ObjectViewModel.h>

#include <QFileInfo>
#include <QApplication>

namespace U2 {

/* TRANSLATOR U2::LoadUnloadedDocumentTask */

const int OpenViewTask::MAX_DOC_NUMBER_TO_OPEN_VIEWS = 5;

//////////////////////////////////////////////////////////////////////////
// LoadUnloadedDocumentAndOpenViewTask

LoadUnloadedDocumentAndOpenViewTask::LoadUnloadedDocumentAndOpenViewTask(Document* d) :
Task("", TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText | TaskFlag_CollectChildrenWarnings)
{
    loadUnloadedTask = new LoadUnloadedDocumentTask(d);
    setUseDescriptionFromSubtask(true);

    setVerboseLogMode(true);
    setTaskName(tr("Load document: '%1'").arg(d->getName()));

    addSubTask(loadUnloadedTask);
}

static Task* createOpenViewTask(const MultiGSelection& ms) {
    QList<GObjectViewFactory*> fs = AppContext::getObjectViewFactoryRegistry()->getAllFactories();
    QList<GObjectViewFactory*> ls;

    foreach(GObjectViewFactory* f, fs) {
        //check if new view can be created
        if (f->canCreateView(ms)) {
            ls.append(f);
        }
    }

    if (ls.size() > 1) {
        GObjectViewFactory* f = AppContext::getObjectViewFactoryRegistry()->getFactoryById(GObjectViewFactory::SIMPLE_TEXT_FACTORY);
        if (ls.contains(f)) {
            // ignore auxiliary text data
            ls.removeAll(f);
        }
    }

    if (ls.size() == 1) {
        GObjectViewFactory* f = ls.first();
        Task* t = f->createViewTask(ms, true);
        return t;
    }
    return NULL;
}

Document* LoadUnloadedDocumentAndOpenViewTask::getDocument() {
    return loadUnloadedTask->getDocument();
}

QList<Task*> LoadUnloadedDocumentAndOpenViewTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask != loadUnloadedTask || hasError() || isCanceled()) {
        return res;
    }

    // look if saved state can be loaded
    Document* doc = loadUnloadedTask->getDocument();
    assert(doc->isLoaded());

    res.append( new OpenViewTask(doc));
    return res;
}

//////////////////////////////////////////////////////////////////////////
// OpenViewTask


OpenViewTask::OpenViewTask( Document* d )
: Task("Open view", TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText ), doc(d)
{
    assert(doc != NULL);
    assert(doc->isLoaded());

}

void OpenViewTask::prepare()
{

    QList<Task*> res;

    //if any of existing views has added an object from the document -> do not open new view
    const QList<GObject*>& docObjects = doc->getObjects();
    if (!GObjectViewUtils::findViewsWithAnyOfObjects(docObjects).isEmpty()) {
        return;
    }

    //try open new view
    GObjectSelection os;
    os.addToSelection(docObjects);
    MultiGSelection ms;
    ms.addSelection(&os);

    QList<GObjectViewState*> sl = GObjectViewUtils::selectStates(ms, AppContext::getProject()->getGObjectViewStates());
    if (sl.size() == 1) {
        GObjectViewState* state = sl.first();
        SAFE_POINT_EXT(state, setError(tr("State is NULL")), );
        GObjectViewFactory* f = AppContext::getObjectViewFactoryRegistry()->getFactoryById(state->getViewFactoryId());
        SAFE_POINT_EXT(f, setError(tr("GObject factory is NULL")), );
        res.append(f->createViewTask(state->getViewName(), state->getStateData()));
    } else {
        Task* openViewTask = createOpenViewTask(ms);
        if (openViewTask!=NULL) {
            openViewTask->setSubtaskProgressWeight(0);
            res.append(openViewTask);
        }
    }

    if (res.isEmpty()) {
        // no view can be opened -> check special case: loaded object contains annotations associated with sequence
        // -> load sequence and open view for it;
        foreach(GObject* obj, doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE)) {
            QList<GObjectRelation> rels = obj->findRelatedObjectsByRole(ObjectRole_Sequence);
            if (rels.isEmpty()) {
                continue;
            }
            const GObjectRelation& rel = rels.first();
            Document* seqDoc = AppContext::getProject()->findDocumentByURL(rel.ref.docUrl);
            if (seqDoc!=NULL) {
                if (seqDoc->isLoaded()) { //try open sequence view
                    GObject* seqObj = seqDoc->findGObjectByName(rel.ref.objName);
                    if (seqObj!=NULL && seqObj->getGObjectType() == GObjectTypes::SEQUENCE) {
                        GObjectSelection os2;
                        os2.addToSelection(seqObj);
                        MultiGSelection ms2;
                        ms2.addSelection(&os2);
                        Task* openViewTask = createOpenViewTask(ms2);
                        if (openViewTask != NULL) {
                            openViewTask->setSubtaskProgressWeight(0);
                            res.append(openViewTask);
                        }
                    }
                } else { //try load doc and open sequence view
                    AppContext::getTaskScheduler()->registerTopLevelTask(new LoadUnloadedDocumentAndOpenViewTask(seqDoc));
                }
            }
            if (!res.isEmpty()) { //one view is ok
                break;
            }
        }

        if (res.isEmpty()) {
            // no view can be opened -> check another special cases: loaded object contains
            // 1. assemblies with their references
            // 2. multiple chromatogram alignment with a reference
            // -> load assemblies/mca and their references and open view for the first object;
            QList<GObject*> objectsToOpen;

            objectsToOpen << doc->findGObjectByType(GObjectTypes::ASSEMBLY);
            if (objectsToOpen.isEmpty()) {
                objectsToOpen << doc->findGObjectByType(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT);
            }

            if (!objectsToOpen.isEmpty()) {
                GObjectSelection os2;
                os2.addToSelection(objectsToOpen.first());
                MultiGSelection ms2;
                ms2.addSelection(&os2);

                Task* openViewTask = createOpenViewTask(ms2);
                if (openViewTask != NULL) {
                    openViewTask->setSubtaskProgressWeight(0);
                    res.append(openViewTask);
                }
            }
        }
    }

    foreach(Task* task, res) {
        addSubTask(task);
    }
}

//////////////////////////////////////////////////////////////////////////

LoadRemoteDocumentAndAddToProjectTask::LoadRemoteDocumentAndAddToProjectTask(const QString &accId, const QString &dbName)
    : Task(tr("Load remote document and add to project"), TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText), 
      mode(LoadRemoteDocumentMode_OpenView), loadRemoteDocTask(NULL)
{
    accNumber = accId;
    databaseName = dbName;
}

LoadRemoteDocumentAndAddToProjectTask::LoadRemoteDocumentAndAddToProjectTask(const GUrl &url)
    : Task(tr("Load remote document and add to project"), TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText), 
      mode(LoadRemoteDocumentMode_OpenView), loadRemoteDocTask(NULL)
{
    docUrl = url;
}

LoadRemoteDocumentAndAddToProjectTask::LoadRemoteDocumentAndAddToProjectTask(const QString& accId, const QString& dbName,
    const QString &fp, const QString &format, const QVariantMap &hints, LoadRemoteDocumentMode mode)
    : Task(tr("Load remote document and add to project"), TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText),
    accNumber(accId), databaseName(dbName), fileFormat(format), fullpath(fp), hints(hints), mode(mode), loadRemoteDocTask(NULL)
{
    
    if (mode == LoadRemoteDocumentMode_LoadOnly) {
        setReportingSupported(true);
        setReportingEnabled(true);
        setTaskName(tr("Load remote document"));
    }
}

void LoadRemoteDocumentAndAddToProjectTask::prepare()
{
    if (docUrl.isEmpty()) {
        loadRemoteDocTask = new LoadRemoteDocumentTask(accNumber, databaseName, fullpath, fileFormat, hints);
    } else {
        loadRemoteDocTask = new LoadRemoteDocumentTask(docUrl);
    }
    addSubTask(loadRemoteDocTask);
}

namespace {
    Task * createLoadedDocTask(Document *loadedDoc, bool openView) {
        if (loadedDoc->isLoaded() && openView) {
            return new OpenViewTask(loadedDoc);
        }
        if (!loadedDoc->isLoaded() && openView) {
            return new LoadUnloadedDocumentAndOpenViewTask(loadedDoc);
        }
        if (!loadedDoc->isLoaded() && !openView) {
            return new LoadUnloadedDocumentTask(loadedDoc);
        }
        return NULL;
    }
}

QList<Task*> LoadRemoteDocumentAndAddToProjectTask::onSubTaskFinished( Task* subTask) {
    QList<Task*> subTasks;
    if (subTask->hasError()) {
        return subTasks;
    }

    if (subTask->isCanceled()) {
        return subTasks;
    }

    if (subTask == loadRemoteDocTask ) {
        if (mode == LoadRemoteDocumentMode_LoadOnly) {
            return subTasks;
        }
        // hack for handling errors with http requests with bad resource id
        Document * d = loadRemoteDocTask->getDocument();
        if(d->getDocumentFormatId() == BaseDocumentFormats::PLAIN_TEXT) {
            setError(tr("Cannot find %1 in %2 database").arg(accNumber).arg(databaseName));
            // try to delete file with response that was created
            QFile::remove(d->getURLString());
            // and remove it from downloaded cache
            RecentlyDownloadedCache * cache = AppContext::getRecentlyDownloadedCache();
            if( cache != NULL ) {
                cache->remove(d->getURLString());
            }
            return subTasks;
        }

        QString fullPath = loadRemoteDocTask->getLocalUrl();
        Project* proj = AppContext::getProject();
        if (proj == NULL) {
            QVariantMap hints;
            hints[ProjectLoaderHint_LoadWithoutView] = mode != LoadRemoteDocumentMode_OpenView;
            Task* openWithProjectTask = AppContext::getProjectLoader()->openWithProjectTask(fullPath, hints);
            if (openWithProjectTask != NULL) {
                subTasks.append(openWithProjectTask);
            }
        } else {
            Document* doc = loadRemoteDocTask->getDocument();
            SAFE_POINT(doc != NULL, "loadRemoteDocTask->takeDocument() returns NULL!", subTasks);
            QString url = doc->getURLString();
            Document* loadedDoc = proj->findDocumentByURL(url);
            if (loadedDoc != NULL){
                Task *task = createLoadedDocTask(loadedDoc, mode == LoadRemoteDocumentMode_OpenView);
                if (NULL != task) {
                    subTasks.append(task);
                }
            } else {
                // Add document to project
                doc = loadRemoteDocTask->takeDocument();
                SAFE_POINT(doc != NULL, "loadRemoteDocTask->takeDocument() returns NULL!", subTasks);
                subTasks.append(new AddDocumentTask(doc));
                if (mode == LoadRemoteDocumentMode_OpenView) {
                    subTasks.append(new LoadUnloadedDocumentAndOpenViewTask(doc));
                } else {
                    subTasks.append(new LoadUnloadedDocumentTask(doc));
                }
           }
        }
    }

    return subTasks;
}

QString LoadRemoteDocumentAndAddToProjectTask::generateReport() const {
    // Note: reporting is enabled only for db + accession mode.
    if (hasError()) {
        return tr("Failed to download %1 from %2. Error: %3").arg(accNumber).arg(databaseName).arg(getError());
    }
    if (isCanceled()) {
        return QString();
    }
    QString url = loadRemoteDocTask->getLocalUrl();
    return tr("Document was successfully downloaded: [%1, %2] -> <a href='%3'>%4</a>")
            .arg(databaseName).arg(accNumber).arg(url).arg(url);
}

AddDocumentAndOpenViewTask::AddDocumentAndOpenViewTask( Document* doc, const AddDocumentTaskConfig& conf)
:Task(tr("Opening view for document: 'NONAME'"), TaskFlags_NR_FOSE_COSC | TaskFlag_CollectChildrenWarnings)
{
    if(doc != NULL){
        GUrl url = doc->getURL();
        setTaskName(tr("Opening view for document: %1").arg(url.fileName()));
    }else{
        setError(tr("Provided document is NULL"));
        return;
    }
    setMaxParallelSubtasks(1);
    addSubTask(new AddDocumentTask(doc, conf));
}

AddDocumentAndOpenViewTask::AddDocumentAndOpenViewTask( DocumentProviderTask* dp, const AddDocumentTaskConfig& conf )
:Task(tr("Opening view for document: 'NONAME'"), TaskFlags_NR_FOSE_COSC | TaskFlag_CollectChildrenWarnings)
{
    if(dp != NULL){
        setTaskName(tr("Opening view for document: %1").arg(dp->getDocumentDescription()));
    }else{
        setError(tr("Document provider is NULL"));
        return;
    }
    setMaxParallelSubtasks(1);
    addSubTask(new AddDocumentTask(dp, conf));
}


QList<Task*> AddDocumentAndOpenViewTask::onSubTaskFinished(Task* t) {
    QList<Task*> res;
    AddDocumentTask* addTask = qobject_cast<AddDocumentTask*>(t);
    if (addTask != NULL && !addTask->getStateInfo().isCoR()) {
        Document* doc = addTask->getDocument();
        assert(doc != NULL);
        res << new LoadUnloadedDocumentAndOpenViewTask(doc);
    }
    return res;
}

}//namespace
