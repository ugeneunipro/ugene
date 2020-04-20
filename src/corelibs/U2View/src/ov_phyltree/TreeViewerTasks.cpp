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

#include "TreeViewerTasks.h"
#include "TreeViewer.h"
#include "TreeViewerFactory.h"
#include "TreeViewerState.h"
#include "CreateRectangularBranchesTask.h"
#include "GraphicsRectangularBranchItem.h"

#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/TaskSignalMapper.h>

#include <U2Core/UnloadedObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/U2SafePoints.h>

#include "ov_msa/PhyTrees/MSAEditorTreeViewer.h"
#include "ov_msa/MSAEditor.h"
#include "ov_msa/PhyTrees/MSAEditorTreeManager.h"

#include <QSet>


namespace U2 {

/* TRANSLATOR U2::TreeViewer */
/* TRANSLATOR U2::ObjectViewTask */

//////////////////////////////////////////////////////////////////////////
/// open new view

OpenTreeViewerTask::OpenTreeViewerTask(PhyTreeObject* _obj, QObject* _parent)
: ObjectViewTask(TreeViewerFactory::ID), phyObject(_obj), parent(_parent), createMDIWindow(false) {
    assert(!phyObject.isNull());
}

OpenTreeViewerTask::OpenTreeViewerTask(UnloadedObject* _obj, QObject* _parent)
: ObjectViewTask(TreeViewerFactory::ID), unloadedReference(_obj), parent(_parent), createMDIWindow(false) {
    assert(_obj->getLoadedObjectType() == GObjectTypes::PHYLOGENETIC_TREE);
    documentsToLoad.append(_obj->getDocument());
}

OpenTreeViewerTask::OpenTreeViewerTask(Document* doc, QObject* _parent)
: ObjectViewTask(TreeViewerFactory::ID), phyObject(NULL), parent(_parent), createMDIWindow(false) {
    assert(!doc->isLoaded());
    documentsToLoad.append(doc);
}

OpenTreeViewerTask::~OpenTreeViewerTask(){
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
            if (obj != NULL && obj->getGObjectType() == GObjectTypes::PHYLOGENETIC_TREE) {
                phyObject = qobject_cast<PhyTreeObject*>(obj);
            }
        } else {
            QList<GObject*> objects = doc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE, UOF_LoadedAndUnloaded);
            phyObject = objects.isEmpty() ? NULL : qobject_cast<PhyTreeObject*>(objects.first());
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
    if (w != NULL) {
        PhyTreeObject* phyObj = tv->getPhyObject();
        QString newViewName = GObjectViewUtils::genUniqueViewName(phyObj->getDocument(), phyObj);
        tv->setName(newViewName);
        w->setWindowTitle(newViewName);
    }
}

//////////////////////////////////////////////////////////////////////////
// open view from state

OpenSavedTreeViewerTask::OpenSavedTreeViewerTask(const QString& viewName, const QVariantMap& stateData)
: ObjectViewTask(TreeViewerFactory::ID, viewName, stateData)
{
    TreeViewerState state(stateData);
    GObjectReference ref = state.getPhyObject();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == NULL) {
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
    if (doc == NULL) {
        stateIsIllegal = true;
        stateInfo.setError(L10N::errorDocumentNotFound(ref.docUrl));
        return;
    }
    GObject* obj = NULL;
    if (doc->isDatabaseConnection() && ref.entityRef.isValid()) {
        obj = doc->getObjectById(ref.entityRef.entityId);
    } else {
        obj = doc->findGObjectByName(ref.objName);
    }
    if (obj == NULL || obj->getGObjectType() != GObjectTypes::PHYLOGENETIC_TREE) {
        stateIsIllegal = true;
        stateInfo.setError(tr("Phylogeny tree object not found: %1").arg(ref.objName));
        return;
    }
    PhyTreeObject* phyObject = qobject_cast<PhyTreeObject*>(obj);
    SAFE_POINT(phyObject != NULL, "Invalid tree object detected", );

    Task* createTask = new CreateTreeViewerTask(viewName, phyObject, stateData);
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(createTask);

}

void OpenSavedTreeViewerTask::updateRanges(const QVariantMap& stateData, TreeViewer* ctx) {
    TreeViewerState state(stateData);

    QTransform m = state.getTransform();
    if (m != QTransform()) {
        ctx->setTransform(m);
    }

    qreal hZoom = state.getHorizontalZoom();
    ctx->setHorizontalZoom(hZoom);

    qreal vZoom = state.getVerticalZoom();
    ctx->setVerticalZoom(vZoom);

    ctx->setSettingsState(stateData);
}

//////////////////////////////////////////////////////////////////////////
// update
UpdateTreeViewerTask::UpdateTreeViewerTask(GObjectView* v, const QString& stateName, const QVariantMap& stateData)
: ObjectViewTask(v, stateName, stateData)
{
}

void UpdateTreeViewerTask::update() {
    if (view.isNull() || view->getFactoryId() != TreeViewerFactory::ID) {
        return; //view was closed;
    }

    TreeViewer* phyView = qobject_cast<TreeViewer*>(view.data());
    assert(phyView != NULL);

    OpenSavedTreeViewerTask::updateRanges(stateData, phyView);
}



//////////////////////////////////////////////////////////////////////////
/// create view

CreateMSAEditorTreeViewerTask::CreateMSAEditorTreeViewerTask(const QString& name, const QPointer<PhyTreeObject>& obj, const QVariantMap& sData) :
    Task("Open tree viewer", TaskFlag_NoRun),
    viewName(name),
    phyObj(obj),
    subTask(NULL),
    stateData(sData),
    view(NULL),
    tempTree(NULL == phyObj ? PhyTree() : phyObj->getTree())
{
    SAFE_POINT(phyObj != NULL, "Invalid tree object detected",);
    connect(obj.data(), SIGNAL(destroyed(QObject *)), SLOT(cancel()));
}

void CreateMSAEditorTreeViewerTask::prepare() {
    CHECK(!stateInfo.isCoR(), );
    subTask = new CreateRectangularBranchesTask(tempTree->getRootNode());
    addSubTask(subTask);
}

Task::ReportResult CreateMSAEditorTreeViewerTask::report() {
    CHECK(!stateInfo.isCoR(), Task::ReportResult_Finished);
    GraphicsRectangularBranchItem* root = dynamic_cast<GraphicsRectangularBranchItem*>(subTask->getResult());
    view = new MSAEditorTreeViewer(viewName, phyObj, root, subTask->getScale());

    if (!stateData.isEmpty()) {
        OpenSavedTreeViewerTask::updateRanges(stateData, view);
    }
    return Task::ReportResult_Finished;
}
TreeViewer* CreateMSAEditorTreeViewerTask::getTreeViewer() {
    return view;
}
const QVariantMap& CreateMSAEditorTreeViewerTask::getStateData() {
    return stateData;
}

CreateTreeViewerTask::CreateTreeViewerTask(const QString& name, const QPointer<PhyTreeObject>& obj, const QVariantMap& sData) :
    Task(tr("Open tree viewer"), TaskFlag_NoRun),
    viewName(name),
    phyObj(obj),
    subTask(NULL),
    stateData(sData),
    tempTree(NULL == phyObj ? PhyTree() : phyObj->getTree())
{
    SAFE_POINT_EXT(phyObj != NULL, setError(tr("Invalid tree object detected")), );
    connect(obj.data(), SIGNAL(destroyed(QObject *)), SLOT(cancel()));
}

void CreateTreeViewerTask::prepare() {
    CHECK_OP(stateInfo, );
    subTask = new CreateRectangularBranchesTask(tempTree->getRootNode());
    addSubTask(subTask);
}

Task::ReportResult CreateTreeViewerTask::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

    GraphicsRectangularBranchItem* root = dynamic_cast<GraphicsRectangularBranchItem*>(subTask->getResult());
    TreeViewer* v = new TreeViewer(viewName, phyObj, root, subTask->getScale());

    GObjectViewWindow* w = new GObjectViewWindow(v, viewName, !stateData.isEmpty());
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(w);
    if (!stateData.isEmpty()) {
        OpenSavedTreeViewerTask::updateRanges(stateData, v);
    }
    return Task::ReportResult_Finished;
}


MSAEditorOpenTreeViewerTask::MSAEditorOpenTreeViewerTask( PhyTreeObject* obj, MSAEditorTreeManager* _parent)
: OpenTreeViewerTask(obj), treeManager(_parent) {}

MSAEditorOpenTreeViewerTask::MSAEditorOpenTreeViewerTask( UnloadedObject* obj, MSAEditorTreeManager* _parent)
: OpenTreeViewerTask(obj), treeManager(_parent) {}

MSAEditorOpenTreeViewerTask::MSAEditorOpenTreeViewerTask( Document* doc, MSAEditorTreeManager* _parent)
: OpenTreeViewerTask(doc), treeManager(_parent) {}

void MSAEditorOpenTreeViewerTask::createTreeViewer() {
    Task* createTask = new CreateMSAEditorTreeViewerTask(phyObject->getDocument()->getName(), phyObject, stateData);
    connect(new TaskSignalMapper(createTask), SIGNAL(si_taskFinished(Task*)), treeManager, SLOT(sl_openTreeTaskFinished(Task*)));
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(createTask);
}

} // namespace
