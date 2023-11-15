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

#include "OpenViewTask.h"

#include <QApplication>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

/* TRANSLATOR U2::LoadUnloadedDocumentTask */

const int OpenViewTask::MAX_DOC_NUMBER_TO_OPEN_VIEWS = 5;

//////////////////////////////////////////////////////////////////////////
// LoadUnloadedDocumentAndOpenViewTask

LoadUnloadedDocumentAndOpenViewTask::LoadUnloadedDocumentAndOpenViewTask(Document* d)
    : Task("", TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText | TaskFlag_CollectChildrenWarnings) {
    loadUnloadedTask = new LoadUnloadedDocumentTask(d);
    setUseDescriptionFromSubtask(true);

    setVerboseLogMode(true);
    setTaskName(tr("Load document: '%1'").arg(d->getName()));

    addSubTask(loadUnloadedTask);
}

static Task* createOpenViewTask(const MultiGSelection& ms) {
    QList<GObjectViewFactory*> fs = AppContext::getObjectViewFactoryRegistry()->getAllFactories();
    QList<GObjectViewFactory*> ls;

    foreach (GObjectViewFactory* f, fs) {
        // check if new view can be created
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
    return ls.size() == 1 ? ls[0]->createViewTask(ms, true) : nullptr;
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

    res.append(new OpenViewTask(doc));
    return res;
}

//////////////////////////////////////////////////////////////////////////
// OpenViewTask

OpenViewTask::OpenViewTask(Document* d)
    : Task("Open view", TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText), doc(d) {
    assert(doc != nullptr);
    assert(doc->isLoaded());
}

void OpenViewTask::prepare() {
    QList<Task*> res;

    // if any of existing views has added an object from the document -> do not open new view
    const QList<GObject*>& docObjects = doc->getObjects();
    if (!GObjectViewUtils::findViewsWithAnyOfObjects(docObjects).isEmpty()) {
        return;
    }

    // try open new view
    GObjectSelection os;
    os.addToSelection(docObjects);
    MultiGSelection ms;
    ms.addSelection(&os);

    QList<GObjectViewState*> sl = GObjectViewUtils::selectStates(ms, AppContext::getProject()->getGObjectViewStates());
    if (sl.size() == 1) {
        GObjectViewState* state = sl.first();
        SAFE_POINT_EXT(state, setError("State is NULL"), );
        GObjectViewFactory* f = AppContext::getObjectViewFactoryRegistry()->getFactoryById(state->getViewFactoryId());
        SAFE_POINT_EXT(f, setError("GObject factory is NULL"), );
        res.append(f->createViewTask(state->getViewName(), state->getStateData()));
    } else {
        Task* openViewTask = createOpenViewTask(ms);
        if (openViewTask != nullptr) {
            openViewTask->setSubtaskProgressWeight(0);
            res.append(openViewTask);
        }
    }

    if (res.isEmpty()) {
        // no view can be opened -> check special case: loaded object contains annotations associated with sequence
        // -> load sequence and open view for it;
        foreach (GObject* obj, doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE)) {
            QList<GObjectRelation> rels = obj->findRelatedObjectsByRole(ObjectRole_Sequence);
            if (rels.isEmpty()) {
                continue;
            }
            const GObjectRelation& rel = rels.first();
            Document* seqDoc = AppContext::getProject()->findDocumentByURL(rel.ref.docUrl);
            if (seqDoc != nullptr) {
                if (seqDoc->isLoaded()) {  // try open sequence view
                    GObject* seqObj = seqDoc->findGObjectByName(rel.ref.objName);
                    if (seqObj != nullptr && seqObj->getGObjectType() == GObjectTypes::SEQUENCE) {
                        GObjectSelection os2;
                        os2.addToSelection(seqObj);
                        MultiGSelection ms2;
                        ms2.addSelection(&os2);
                        Task* openViewTask = createOpenViewTask(ms2);
                        if (openViewTask != nullptr) {
                            openViewTask->setSubtaskProgressWeight(0);
                            res.append(openViewTask);
                        }
                    }
                } else {  // try load doc and open sequence view
                    AppContext::getTaskScheduler()->registerTopLevelTask(new LoadUnloadedDocumentAndOpenViewTask(seqDoc));
                }
            }
            if (!res.isEmpty()) {  // one view is ok
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
                if (openViewTask != nullptr) {
                    openViewTask->setSubtaskProgressWeight(0);
                    res.append(openViewTask);
                }
            }
        }
    }

    foreach (Task* task, res) {
        addSubTask(task);
    }
}

//////////////////////////////////////////////////////////////////////////

LoadRemoteDocumentAndAddToProjectTask::LoadRemoteDocumentAndAddToProjectTask(const GUrl& url)
    : Task(tr("Load remote document and add to project"), TaskFlags_NR_FOSCOE |
                                                          TaskFlag_MinimizeSubtaskErrorText |
                                                          TaskFlag_ReportingIsSupported |
                                                          TaskFlag_ReportingIsEnabled) {
    docUrl = url;
}

LoadRemoteDocumentAndAddToProjectTask::LoadRemoteDocumentAndAddToProjectTask(const QString& _accId, const QString& _dbName,
                                                                             const QString& _fp, const QString& _format,
                                                                             const QVariantMap& _hints, bool _openView)
    : Task(tr("Load remote document and add to project"), TaskFlags_NR_FOSCOE |
                                                          TaskFlag_MinimizeSubtaskErrorText |
                                                          TaskFlag_ReportingIsSupported |
                                                          TaskFlag_ReportingIsEnabled),
      accNumber(_accId),
      databaseName(_dbName),
      fileFormat(_format),
      fullpath(_fp),
      hints(_hints),
      openView(_openView) {
}

void LoadRemoteDocumentAndAddToProjectTask::prepare() {
    if (docUrl.isEmpty()) {
        loadRemoteDocTask = new LoadRemoteDocumentTask(accNumber, databaseName, fullpath, fileFormat, hints);
    } else {
        loadRemoteDocTask = new LoadRemoteDocumentTask(docUrl);
    }
    loadRemoteDocTask->setReportingSupported(true);
    loadRemoteDocTask->setReportingEnabled(true);
    addSubTask(loadRemoteDocTask);
}

namespace {
Task* createLoadedDocTask(Document* loadedDoc, bool openView) {
    if (loadedDoc->isLoaded() && openView) {
        return new OpenViewTask(loadedDoc);
    }
    if (!loadedDoc->isLoaded() && openView) {
        return new LoadUnloadedDocumentAndOpenViewTask(loadedDoc);
    }
    if (!loadedDoc->isLoaded() && !openView) {
        return new LoadUnloadedDocumentTask(loadedDoc);
    }
    return nullptr;
}
}  // namespace

QList<Task*> LoadRemoteDocumentAndAddToProjectTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;
    if (subTask->hasError()) {
        return subTasks;
    }

    if (subTask->isCanceled()) {
        return subTasks;
    }

    if (subTask == loadRemoteDocTask) {
        // hack for handling errors with http requests with bad resource id
        Document* d = loadRemoteDocTask->getDocument();
        if (d->getDocumentFormatId() == BaseDocumentFormats::PLAIN_TEXT) {
            setError(tr("Cannot find %1 in %2 database").arg(accNumber).arg(databaseName));
            // try to delete file with response that was created
            QFile::remove(d->getURLString());
            // and remove it from downloaded cache
            RecentlyDownloadedCache* cache = AppContext::getRecentlyDownloadedCache();
            if (cache != nullptr) {
                cache->remove(d->getURLString());
            }
            return subTasks;
        }

        QString fullPath = loadRemoteDocTask->getLocalUrl();
        Project* proj = AppContext::getProject();
        if (proj == nullptr) {
            QVariantMap hints;
            hints[ProjectLoaderHint_LoadWithoutView] = !openView;
            Task* openWithProjectTask = AppContext::getProjectLoader()->openWithProjectTask(fullPath, hints);
            if (openWithProjectTask != nullptr) {
                subTasks.append(openWithProjectTask);
            }
        } else {
            Document* doc = loadRemoteDocTask->getDocument();
            SAFE_POINT(doc != nullptr, "loadRemoteDocTask->takeDocument() returns NULL!", subTasks);
            QString url = doc->getURLString();
            Document* loadedDoc = proj->findDocumentByURL(url);
            if (loadedDoc != nullptr) {
                Task* task = createLoadedDocTask(loadedDoc, openView);
                if (task != nullptr) {
                    subTasks.append(task);
                }
            } else {
                // Add document to project
                doc = loadRemoteDocTask->takeDocument();
                SAFE_POINT(doc != nullptr, "loadRemoteDocTask->takeDocument() returns NULL!", subTasks);
                subTasks.append(new AddDocumentTask(doc));
                if (openView) {
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
    SAFE_POINT_NN(loadRemoteDocTask, {});

    return loadRemoteDocTask->generateReport();
}

AddDocumentAndOpenViewTask::AddDocumentAndOpenViewTask(Document* doc, const AddDocumentTaskConfig& conf)
    : Task(tr("Opening view for document: 'NONAME'"), TaskFlags_NR_FOSE_COSC | TaskFlag_CollectChildrenWarnings) {
    if (doc != nullptr) {
        GUrl url = doc->getURL();
        setTaskName(tr("Opening view for document: %1").arg(url.fileName()));
    } else {
        setError(tr("Provided document is NULL"));
        return;
    }
    setMaxParallelSubtasks(1);
    addSubTask(new AddDocumentTask(doc, conf));
}

AddDocumentAndOpenViewTask::AddDocumentAndOpenViewTask(DocumentProviderTask* dp, const AddDocumentTaskConfig& conf)
    : Task(tr("Opening view for document: 'NONAME'"), TaskFlags_NR_FOSE_COSC | TaskFlag_CollectChildrenWarnings) {
    if (dp != nullptr) {
        setTaskName(tr("Opening view for document: %1").arg(dp->getDocumentDescription()));
    } else {
        setError(tr("Document provider is NULL"));
        return;
    }
    setMaxParallelSubtasks(1);
    addSubTask(new AddDocumentTask(dp, conf));
}

QList<Task*> AddDocumentAndOpenViewTask::onSubTaskFinished(Task* t) {
    QList<Task*> res;
    auto addTask = qobject_cast<AddDocumentTask*>(t);
    if (addTask != nullptr && !addTask->getStateInfo().isCoR()) {
        Document* doc = addTask->getDocument();
        assert(doc != nullptr);
        res << new LoadUnloadedDocumentAndOpenViewTask(doc);
    }
    return res;
}

}  // namespace U2
