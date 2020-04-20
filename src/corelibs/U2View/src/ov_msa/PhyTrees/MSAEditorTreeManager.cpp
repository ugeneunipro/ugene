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

#include <U2Algorithm/MSADistanceAlgorithm.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportDocumentDialogController.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/U2FileDialog.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "MSAEditorTreeManager.h"
#include "ov_msa/PhyTrees/MSAEditorMultiTreeViewer.h"
#include "ov_msa/TreeOptions/TreeOptionsWidgetFactory.h"
#include "ov_phyltree/GraphicsRectangularBranchItem.h"
#include "ov_phyltree/TreeViewer.h"
#include "ov_phyltree/TreeViewerTasks.h"
#include "phyltree/CreatePhyTreeDialogController.h"
#include "MsaEditorTreeTabArea.h"

namespace U2 {
MSAEditorTreeManager::MSAEditorTreeManager(MSAEditor* _editor)
    : QObject(_editor), editor(_editor), msaObject(NULL), addExistingTree(false)
{
     SAFE_POINT(NULL != editor, "Invalid parameter were passed into constructor MSAEditorTreeManager", );

     Project *project = AppContext::getProject();
     SAFE_POINT(NULL != project, "Invalid project detected", );

     connect(project, SIGNAL(si_documentRemoved(Document *)), SLOT(sl_onDocumentRemovedFromProject(Document*)));
}

void MSAEditorTreeManager::sl_onDocumentRemovedFromProject(Document *doc) {
    CHECK(NULL != msaObject, );
    if (doc == msaObject->getDocument()) {
        msaObject = NULL;
        return;
    }
    QList<GObjectRelation> relatedTrees = msaObject->findRelatedObjectsByRole(ObjectRole_PhylogeneticTree);
    CHECK(!relatedTrees.isEmpty(), );

    foreach (const GObjectRelation &r, relatedTrees) {
        if (r.ref.entityRef.isValid() && NULL != doc->getObjectById(r.ref.entityRef.entityId)) {
            msaObject->removeObjectRelation(r);
        }
    }
}

void MSAEditorTreeManager::loadRelatedTrees() {
    msaObject = editor->getMaObject();
    QList<GObjectRelation> relatedTrees = msaObject->findRelatedObjectsByRole(ObjectRole_PhylogeneticTree);
    CHECK(!relatedTrees.isEmpty(),);

    foreach(const GObjectRelation &rel, relatedTrees) {
        const QString& treeFileName = rel.getDocURL();
        Document* doc = AppContext::getProject()->findDocumentByURL(treeFileName);
        if(NULL != doc) {
            loadTreeFromFile(treeFileName);
        }
    }
}

void MSAEditorTreeManager::buildTreeWithDialog() {
    msaObject = editor->getMaObject();
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    QStringList list = registry->getNameList();
    addExistingTree = false;
    if (list.size() == 0) {
        QMessageBox::information(editor->getUI(), tr("Calculate phy tree"),
            tr("No algorithms for building phylogenetic tree are available.") );
        return;
    }

    QObjectScopedPointer<CreatePhyTreeDialogController> dlg = new CreatePhyTreeDialogController(editor->getUI(), msaObject, settings);
    const int rc = dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(rc == QDialog::Accepted, );

    settings.rowsOrder = msaObject->getMultipleAlignment()->getRowNames();
    buildTree(settings);
}

void MSAEditorTreeManager::buildTree(const CreatePhyTreeSettings& buildSettings) {
    createPhyTreeGeneratorTask(buildSettings);
}

void MSAEditorTreeManager::sl_refreshTree(MSAEditorTreeViewer* treeViewer) {
    CHECK(canRefreshTree(treeViewer),);

    createPhyTreeGeneratorTask(treeViewer->getCreatePhyTreeSettings(), true, treeViewer);
}

void MSAEditorTreeManager::createPhyTreeGeneratorTask(const CreatePhyTreeSettings& buildSettings, bool refreshExistingTree, MSAEditorTreeViewer* treeViewer) {
    const MultipleSequenceAlignment msa = msaObject->getMultipleAlignment();
    settings = buildSettings;

    PhyTreeGeneratorLauncherTask* treeGeneratorTask = new PhyTreeGeneratorLauncherTask(msa, settings);
    if(refreshExistingTree) {
        activeRefreshTasks[treeViewer] = treeGeneratorTask;
        connect(new TaskSignalMapper(treeGeneratorTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_treeRebuildingFinished(Task*)));
        connect(treeViewer, SIGNAL(destroyed()), treeGeneratorTask, SLOT(sl_onCalculationCanceled()));
    }
    else {
        connect(new TaskSignalMapper(treeGeneratorTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_openTree(Task*)));
    }
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(treeGeneratorTask);
}

void MSAEditorTreeManager::sl_treeRebuildingFinished(Task* _treeBuildTask) {
    PhyTreeGeneratorLauncherTask* treeBuildTask = qobject_cast<PhyTreeGeneratorLauncherTask*>(_treeBuildTask);
    if(NULL == treeBuildTask || treeBuildTask->isCanceled()) {
        return;
    }

    MSAEditorTreeViewer* refreshingTree = activeRefreshTasks.key(treeBuildTask);
    CHECK(NULL != refreshingTree,);
    activeRefreshTasks.remove(refreshingTree);

    PhyTreeObject *treeObj = refreshingTree->getPhyObject();
    treeObj->setTree(treeBuildTask->getResult());
}

bool MSAEditorTreeManager::canRefreshTree(MSAEditorTreeViewer* treeViewer) {
    bool canRefresh = (treeViewer->getParentAlignmentName() == msaObject->getMultipleAlignment()->getName());
    return canRefresh && !activeRefreshTasks.contains(treeViewer);
}

void MSAEditorTreeManager::sl_openTree(Task* treeBuildTask) {
    CHECK(!treeBuildTask->isCanceled(), )

    PhyTreeGeneratorLauncherTask* treeGeneratorTask = qobject_cast<PhyTreeGeneratorLauncherTask*>(treeBuildTask);
    if(NULL == treeBuildTask) {
        return;
    }

    const GUrl& msaURL = msaObject->getDocument()->getURL();
    SAFE_POINT(!msaURL.isEmpty(), QString("Tree URL in MSAEditorTreeManager::sl_openTree() is empty"),);

    Project* p = AppContext::getProject();
    d = NULL;
    PhyTreeObject *newTreeObj = NULL;
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    QString treeFileName = settings.fileUrl.getURLString();
    if (treeFileName.isEmpty()) {
        treeFileName = GUrlUtils::rollFileName(msaURL.dirPath() + "/" + msaURL.baseFileName() + ".nwk", DocumentUtils::getNewDocFileNameExcludesHint());
    }

    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::NEWICK);
    IOAdapterFactory *iof = IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE);

    const QList<Document *> documents = AppContext::getProject()->getDocuments();
    bool isNewDocument = true;
    foreach(Document *doc, documents) {
        if(treeFileName == doc->getURLString()) {
            d = doc;
            isNewDocument = false;
            break;
        }
    }

    if(NULL == d) {
        U2OpStatus2Log os;
        d = df->createNewLoadedDocument(iof, treeFileName, os);
        CHECK_OP(os, );
    }

    if(isNewDocument) {
        U2OpStatus2Log os;
        newTreeObj = PhyTreeObject::createInstance(treeGeneratorTask->getResult(), "Tree", d->getDbiRef(), os);
        CHECK_OP(os, );
        d->addObject(newTreeObj);
    }
    else {
        if(!d->isLoaded()) {
            phyTree = treeGeneratorTask->getResult();
            LoadUnloadedDocumentTask* t = new LoadUnloadedDocumentTask(d);
            connect(new TaskSignalMapper(t), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_onPhyTreeDocLoaded(Task*)));
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
            return;
        }
        else {
            const QList<GObject*>& objects = d->getObjects();
            foreach(GObject* obj, objects) {
                PhyTreeObject* treeObj = qobject_cast<PhyTreeObject*>(obj);
                if(treeObj) {
                    treeObj->setTree(treeGeneratorTask->getResult());
                    newTreeObj = treeObj;
                }
            }
        }
    }

    if(!p->getDocuments().contains(d)) {
        p->addDocument(d);
    }

    if(isNewDocument) {
        msaObject->addObjectRelation(GObjectRelation(GObjectReference(newTreeObj), ObjectRole_PhylogeneticTree));
    }

    Task* saveTask = new SaveDocumentTask(d);
    scheduler->registerTopLevelTask(saveTask);

    openTreeViewer(newTreeObj);
}

void MSAEditorTreeManager::sl_onPhyTreeDocLoaded(Task* task) {
    LoadUnloadedDocumentTask* loadTask = qobject_cast<LoadUnloadedDocumentTask*>(task);
    d = loadTask->getDocument();
    PhyTreeObject* treeObj = NULL;
    foreach(GObject* obj, d->getObjects()) {
        treeObj = qobject_cast<PhyTreeObject*>(obj);
        if(treeObj) {
            treeObj->setTree(phyTree);
            break;
        }
    }
    openTreeViewer(treeObj);
}

void MSAEditorTreeManager::openTreeViewer(PhyTreeObject* treeObj) {
    Task* openTask = NULL;
    if(true == settings.displayWithAlignmentEditor) {
        openTask = new MSAEditorOpenTreeViewerTask(treeObj, this);
    }
    else {
        openTask = new OpenTreeViewerTask(treeObj, this);
    }
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(openTask);
}

void MSAEditorTreeManager::sl_openTreeTaskFinished(Task* t) {
    CreateMSAEditorTreeViewerTask* task = qobject_cast<CreateMSAEditorTreeViewerTask*> (t);
    if(NULL != task) {
        if(settings.displayWithAlignmentEditor) {
            MSAEditorTreeViewer* treeView = dynamic_cast<MSAEditorTreeViewer*>(task->getTreeViewer());
            SAFE_POINT(NULL != treeView, tr("Can not convert TreeViewer* to MSAEditorTreeViewer* in function MSAEditorTreeManager::sl_openTreeTaskFinished(Task* t)"),);
            GObjectViewWindow* w = new GObjectViewWindow(task->getTreeViewer(), editor->getName(), !task->getStateData().isEmpty());

            connect(w, SIGNAL(si_windowClosed(GObjectViewWindow*)), this, SLOT(sl_onWindowClosed(GObjectViewWindow*)));

            MsaEditorWgt* msaUI = editor->getUI();
            msaUI->addTreeView(w);

            if(!addExistingTree) {
                treeView->setCreatePhyTreeSettings(settings);
                treeView->setParentAignmentName(msaObject->getMultipleAlignment()->getName());
            }

            treeView->setMSAEditor(editor);
            if (settings.syncAlignmentWithTree) {
                treeView->sync();
            }

            connect(treeView, SIGNAL(si_refreshTree(MSAEditorTreeViewer*)), SLOT(sl_refreshTree(MSAEditorTreeViewer*)));
        }
        else {
            GObjectViewWindow* w = new GObjectViewWindow(task->getTreeViewer(), editor->getName(), !task->getStateData().isEmpty());
            MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
            mdiManager->addMDIWindow(w);
        }
    }
}


void MSAEditorTreeManager::openTreeFromFile() {
    LastUsedDirHelper h;
    QString filter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::NEWICK, false, QStringList());
    QString file;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        file = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select files to open..."), h.dir,  filter, 0, QFileDialog::DontUseNativeDialog);
    }else
#endif
    file = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select files to open..."), h.dir,  filter);
    CHECK(!file.isEmpty(),);
    if (QFileInfo(file).exists()) {
        h.url = file;
        loadTreeFromFile(file);
    }
}

void MSAEditorTreeManager::loadTreeFromFile(const QString& treeFileName) {
    TaskScheduler* scheduler = AppContext::getTaskScheduler();

    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::NEWICK);
    IOAdapterFactory *iof = IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE);
    U2OpStatus2Log os;

    const QList<Document *> documents = AppContext::getProject()->getDocuments();
    addExistingTree = true;
    bool isNewDocument = true;
    Document *doc = NULL;
    foreach(doc, documents) {
        if(treeFileName == doc->getURLString()) {
            isNewDocument = false;
            break;
        }
    }

    if(isNewDocument || !doc->isLoaded()) {
        if(!isNewDocument && !doc->isLoaded()) {
            if(AppContext::getProject()->getDocuments().contains(doc)) {
                AppContext::getProject()->removeDocument(doc);
            }
        }
        doc = df->loadDocument(iof, treeFileName, QVariantMap(), os);
        CHECK(NULL != doc,);
        AppContext::getProject()->addDocument(doc);
    }

    const QList<GObject*>& objects = doc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE);
    foreach(GObject* obj, objects) {
        PhyTreeObject* treeObject = qobject_cast<PhyTreeObject*>(obj);
        msaObject->addObjectRelation(GObjectRelation(GObjectReference(treeObject), ObjectRole_PhylogeneticTree));
        if(NULL == treeObject) {
            continue;
        }
        const MSAEditorMultiTreeViewer* multiTreeViewer = getMultiTreeViewer();
        if(NULL == multiTreeViewer || !multiTreeViewer->getTreeNames().contains(doc->getName())) {
            Task* task = new MSAEditorOpenTreeViewerTask(treeObject, this);
            scheduler->registerTopLevelTask(task);
        }
    }
}

void MSAEditorTreeManager::sl_onWindowClosed(GObjectViewWindow* viewWindow) {
    MSAEditorMultiTreeViewer* multiTreeViewer = getMultiTreeViewer();
    CHECK(NULL != multiTreeViewer,);
    multiTreeViewer->sl_onTabCloseRequested(viewWindow);
}

MSAEditorMultiTreeViewer* MSAEditorTreeManager::getMultiTreeViewer() const {
    SAFE_POINT(NULL != editor, tr("Incorrect reference to the MSAEditor"), NULL);
    MsaEditorWgt* msaEditorUi = editor->getUI();
    SAFE_POINT(NULL != msaEditorUi, tr("Incorrect reference to the MSAEditor"), NULL);
    return msaEditorUi->getMultiTreeViewer();
}

}//namespace
