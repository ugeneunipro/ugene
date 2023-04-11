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

#include "TreeViewerTasks.h"

#include <QSet>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UnloadedObject.h>

#include "TreeViewer.h"
#include "TreeViewerFactory.h"
#include "TreeViewerState.h"
#include "ov_msa/MSAEditor.h"
#include "ov_msa/phy_tree/MSAEditorTreeManager.h"
#include "ov_msa/phy_tree/MSAEditorTreeViewer.h"

namespace U2 {

/* TRANSLATOR U2::TreeViewer */
/* TRANSLATOR U2::ObjectViewTask */

//////////////////////////////////////////////////////////////////////////
/// open new view

OpenTreeViewerTask::OpenTreeViewerTask(PhyTreeObject* _obj, QObject* _parent)
    : ObjectViewTask(TreeViewerFactory::ID), phyObject(_obj), parent(_parent) {
    SAFE_POINT(!phyObject.isNull(), "PhyTreeObject is null!", );
}

OpenTreeViewerTask::OpenTreeViewerTask(UnloadedObject* _obj, QObject* _parent)
    : ObjectViewTask(TreeViewerFactory::ID), unloadedReference(_obj), parent(_parent) {
    SAFE_POINT(_obj->getLoadedObjectType() == GObjectTypes::PHYLOGENETIC_TREE, "Object is not a tree", );
    documentsToLoad.append(_obj->getDocument());
}

OpenTreeViewerTask::OpenTreeViewerTask(Document* doc, QObject* _parent)
    : ObjectViewTask(TreeViewerFactory::ID), phyObject(nullptr), parent(_parent) {
    SAFE_POINT(!doc->isLoaded(), "Document is not loaded", );
    documentsToLoad.append(doc);
}

void OpenTreeViewerTask::open() {
    if (stateInfo.hasError() || (phyObject.isNull() && documentsToLoad.isEmpty())) {
        return;
    }
    if (phyObject.isNull()) {
        Document* doc = documentsToLoad.first();
        QList<GObject*> objects;
        if (unloadedReference.isValid()) {
            GObject* obj = doc->findGObjectByName(unloadedReference.objName);
            if (obj != nullptr && obj->getGObjectType() == GObjectTypes::PHYLOGENETIC_TREE) {
                phyObject = qobject_cast<PhyTreeObject*>(obj);
            }
        } else {
            QList<GObject*> phyTreeObjects = doc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE, UOF_LoadedAndUnloaded);
            phyObject = phyTreeObjects.isEmpty() ? nullptr : qobject_cast<PhyTreeObject*>(phyTreeObjects.first());
        }
        if (phyObject.isNull()) {
            stateInfo.setError(tr("Phylogenetic tree object not found"));
            return;
        }
    }
    viewName = GObjectViewUtils::genUniqueViewName(phyObject->getDocument(), phyObject);
    uiLog.details(tr("Opening tree viewer for object %1").arg(phyObject->getGObjectName()));

    createTreeViewer();
}
void OpenTreeViewerTask::createTreeViewer() {
    Task* createTask = new CreateTreeViewerTask(viewName, phyObject, stateData);

    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(createTask);
}

void OpenTreeViewerTask::updateTitle(TreeViewer* tv) {
    const QString& oldViewName = tv->getName();
    GObjectViewWindow* w = GObjectViewUtils::findViewByName(oldViewName);
    if (w != nullptr) {
        PhyTreeObject* phyObj = tv->getPhyObject();
        QString newViewName = GObjectViewUtils::genUniqueViewName(phyObj->getDocument(), phyObj);
        tv->setName(newViewName);
        w->setWindowTitle(newViewName);
    }
}

//////////////////////////////////////////////////////////////////////////
// open view from state

OpenSavedTreeViewerTask::OpenSavedTreeViewerTask(const QString& viewName, const QVariantMap& stateData)
    : ObjectViewTask(TreeViewerFactory::ID, viewName, stateData) {
    TreeViewerState state(stateData);
    GObjectReference ref = state.getPhyObject();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {
        doc = createDocumentAndAddToProject(ref.docUrl, AppContext::getProject(), stateInfo);
        CHECK_OP_EXT(stateInfo, stateIsIllegal = true, );
    }
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    }
}

void OpenSavedTreeViewerTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    TreeViewerState state(stateData);
    GObjectReference ref = state.getPhyObject();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {
        stateIsIllegal = true;
        stateInfo.setError(L10N::errorDocumentNotFound(ref.docUrl));
        return;
    }
    GObject* obj = doc->findGObjectByName(ref.objName);
    if (obj == nullptr || obj->getGObjectType() != GObjectTypes::PHYLOGENETIC_TREE) {
        stateIsIllegal = true;
        stateInfo.setError(tr("Phylogeny tree object not found: %1").arg(ref.objName));
        return;
    }
    auto phyObject = qobject_cast<PhyTreeObject*>(obj);
    SAFE_POINT(phyObject != nullptr, "Invalid tree object detected", );

    auto createTask = new CreateTreeViewerTask(viewName, phyObject, stateData);
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(createTask);
}

void OpenSavedTreeViewerTask::applySavedState(TreeViewer* treeViewer, const QVariantMap& stateData) {
    TreeViewerState state(stateData);
    treeViewer->setZoomLevel(state.getZoomLevel());
    treeViewer->setSettingsState(stateData);
}

//////////////////////////////////////////////////////////////////////////
// update
UpdateTreeViewerTask::UpdateTreeViewerTask(GObjectViewController* v, const QString& stateName, const QVariantMap& stateData)
    : ObjectViewTask(v, stateName, stateData) {
}

void UpdateTreeViewerTask::update() {
    CHECK(!view.isNull() && view->getFactoryId() == TreeViewerFactory::ID, );  // View may be already closed.

    auto phyView = qobject_cast<TreeViewer*>(view.data());
    SAFE_POINT(phyView != nullptr, "Not a treeViewer!", );

    OpenSavedTreeViewerTask::applySavedState(phyView, stateData);
}

//////////////////////////////////////////////////////////////////////////
/// create view

CreateMSAEditorTreeViewerTask::CreateMSAEditorTreeViewerTask(const QString& name, const QPointer<PhyTreeObject>& obj, const QVariantMap& sData)
    : Task("Open tree viewer", TaskFlag_NoRun),
      viewName(name),
      phyObj(obj),
      stateData(sData) {
    SAFE_POINT(phyObj != nullptr, "Invalid tree object detected", );
    connect(obj.data(), SIGNAL(destroyed(QObject*)), SLOT(cancel()));
}

void CreateMSAEditorTreeViewerTask::prepare() {
    CHECK(!stateInfo.isCoR(), );
}

Task::ReportResult CreateMSAEditorTreeViewerTask::report() {
    CHECK(!stateInfo.isCoR(), Task::ReportResult_Finished);
    view = new MSAEditorTreeViewer(viewName, phyObj);

    if (!stateData.isEmpty()) {
        OpenSavedTreeViewerTask::applySavedState(view, stateData);
    }
    return Task::ReportResult_Finished;
}

TreeViewer* CreateMSAEditorTreeViewerTask::getTreeViewer() const {
    return view;
}

const QVariantMap& CreateMSAEditorTreeViewerTask::getStateData() const {
    return stateData;
}

CreateTreeViewerTask::CreateTreeViewerTask(const QString& name, const QPointer<PhyTreeObject>& obj, const QVariantMap& sData)
    : Task(tr("Open tree viewer"), TaskFlag_NoRun),
      viewName(name),
      phyObj(obj),
      stateData(sData) {
    SAFE_POINT_EXT(phyObj != nullptr, setError(tr("Invalid tree object detected")), );
    connect(obj.data(), SIGNAL(destroyed(QObject*)), SLOT(cancel()));
}

void CreateTreeViewerTask::prepare() {
    CHECK_OP(stateInfo, );
}

Task::ReportResult CreateTreeViewerTask::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

    auto treeViewer = new TreeViewer(viewName, phyObj);

    auto w = new GObjectViewWindow(treeViewer, viewName, !stateData.isEmpty());
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(w);
    if (!stateData.isEmpty()) {
        OpenSavedTreeViewerTask::applySavedState(treeViewer, stateData);
    }
    return Task::ReportResult_Finished;
}

MSAEditorOpenTreeViewerTask::MSAEditorOpenTreeViewerTask(PhyTreeObject* obj, MSAEditorTreeManager* _parent)
    : OpenTreeViewerTask(obj), treeManager(_parent) {
}

void MSAEditorOpenTreeViewerTask::createTreeViewer() {
    auto createTask = new CreateMSAEditorTreeViewerTask(phyObject->getDocument()->getName(), phyObject, stateData);
    connect(new TaskSignalMapper(createTask), SIGNAL(si_taskFinished(Task*)), treeManager, SLOT(sl_openTreeTaskFinished(Task*)));
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(createTask);
}

}  // namespace U2
