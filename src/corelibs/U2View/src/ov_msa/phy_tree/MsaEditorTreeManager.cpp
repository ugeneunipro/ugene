/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MsaEditorTreeManager.h"

#include <QApplication>
#include <QGraphicsView>
#include <QMessageBox>

#include <U2Algorithm/MSADistanceAlgorithm.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/MaEditorNameList.h>
#include <U2View/MsaEditor.h>
#include <U2View/MsaEditorSequenceArea.h>

#include "ov_msa/phy_tree/MsaEditorMultiTreeViewer.h"
#include "ov_msa/phy_tree/MsaEditorTreeViewer.h"
#include "ov_phyltree/TreeViewerTasks.h"
#include "phyltree/CreatePhyTreeDialogController.h"

namespace U2 {
MsaEditorTreeManager::MsaEditorTreeManager(MsaEditor* msaEditor)
    : QObject(msaEditor), editor(msaEditor) {
    SAFE_POINT(editor != nullptr, "Invalid parameter were passed into constructor MSAEditorTreeManager", );

    Project* project = AppContext::getProject();
    SAFE_POINT(project != nullptr, "Invalid project detected", );

    connect(project, SIGNAL(si_documentRemoved(Document*)), SLOT(sl_onDocumentRemovedFromProject(Document*)));
}

void MsaEditorTreeManager::sl_onDocumentRemovedFromProject(Document* doc) {
    CHECK(msaObject != nullptr, );
    if (doc == msaObject->getDocument()) {
        msaObject = nullptr;
        return;
    }
    QList<GObjectRelation> treeRelationList = msaObject->findRelatedObjectsByRole(ObjectRole_PhylogeneticTree);
    CHECK(!treeRelationList.isEmpty(), );

    for (const GObjectRelation& treeRelation : qAsConst(treeRelationList)) {
        if (treeRelation.ref.entityRef.isValid() && doc->getObjectById(treeRelation.ref.entityRef.entityId) != nullptr) {
            msaObject->removeObjectRelation(treeRelation);
        }
    }
}

void MsaEditorTreeManager::loadRelatedTrees() {
    msaObject = editor->getMaObject();
    QList<GObjectRelation> treeRelationList = msaObject->findRelatedObjectsByRole(ObjectRole_PhylogeneticTree);
    CHECK(!treeRelationList.isEmpty(), );

    for (const GObjectRelation& treeRelation : qAsConst(treeRelationList)) {
        const QString& treeFileName = treeRelation.getDocURL();
        Document* doc = AppContext::getProject()->findDocumentByURL(treeFileName);
        if (doc != nullptr) {
            loadTreeFromFile(treeFileName);
        }
    }
}

void MsaEditorTreeManager::buildTreeWithDialog() {
    msaObject = editor->getMaObject();
    QStringList phyTreeGeneratorNameList = AppContext::getPhyTreeGeneratorRegistry()->getNameList();
    addExistingTree = false;
    if (phyTreeGeneratorNameList.isEmpty()) {
        QMessageBox::information(editor->getUI(), tr("Calculate phy tree"), tr("No algorithms for building phylogenetic tree are available."));
        return;
    }

    QObjectScopedPointer<CreatePhyTreeDialogController> dlg = new CreatePhyTreeDialogController(editor->getUI(), msaObject, settings);
    const int rc = dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(rc == QDialog::Accepted, );

    buildTree(settings);
}

void MsaEditorTreeManager::buildTree(const CreatePhyTreeSettings& buildSettings) {
    createPhyTreeGeneratorTask(buildSettings);
}

void MsaEditorTreeManager::sl_refreshTree(MsaEditorTreeViewer* treeViewer) {
    CHECK(canRefreshTree(treeViewer), );

    createPhyTreeGeneratorTask(treeViewer->getCreatePhyTreeSettings(), true, treeViewer);
}

void MsaEditorTreeManager::createPhyTreeGeneratorTask(const CreatePhyTreeSettings& buildSettings, bool refreshExistingTree, MsaEditorTreeViewer* treeViewer) {
    const Msa msa = msaObject->getAlignment();
    settings = buildSettings;

    auto treeGeneratorTask = new PhyTreeGeneratorLauncherTask(msa, settings);
    if (refreshExistingTree) {
        activeRefreshTasks[treeViewer] = treeGeneratorTask;
        connect(new TaskSignalMapper(treeGeneratorTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_treeRebuildingFinished(Task*)));
        connect(treeViewer, &QObject::destroyed, treeGeneratorTask, &PhyTreeGeneratorLauncherTask::sl_onCalculationCanceled);
    } else {
        connect(new TaskSignalMapper(treeGeneratorTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_openTree(Task*)));
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(treeGeneratorTask);
}

void MsaEditorTreeManager::sl_treeRebuildingFinished(Task* _treeBuildTask) {
    auto treeBuildTask = qobject_cast<PhyTreeGeneratorLauncherTask*>(_treeBuildTask);
    if (treeBuildTask == nullptr || treeBuildTask->isCanceled()) {
        return;
    }

    MsaEditorTreeViewer* refreshingTree = activeRefreshTasks.key(treeBuildTask);
    CHECK(refreshingTree != nullptr, );
    activeRefreshTasks.remove(refreshingTree);

    PhyTreeObject* treeObj = refreshingTree->getPhyObject();
    treeObj->setTree(treeBuildTask->getResult());
}

bool MsaEditorTreeManager::canRefreshTree(MsaEditorTreeViewer* treeViewer) {
    bool canRefresh = (treeViewer->getParentAlignmentName() == msaObject->getAlignment()->getName());
    return canRefresh && !activeRefreshTasks.contains(treeViewer);
}

void MsaEditorTreeManager::sl_openTree(Task* treeBuildTask) {
    CHECK(treeBuildTask != nullptr && !treeBuildTask->isCanceled(), )

    auto treeGeneratorTask = qobject_cast<PhyTreeGeneratorLauncherTask*>(treeBuildTask);
    CHECK(treeGeneratorTask != nullptr, );

    const GUrl& msaURL = msaObject->getDocument()->getURL();
    SAFE_POINT(!msaURL.isEmpty(), QString("Tree URL in MSAEditorTreeManager::sl_openTree() is empty"), );

    Project* p = AppContext::getProject();
    treeDocument = nullptr;
    PhyTreeObject* newTreeObj = nullptr;
    QString treeFileName = settings.fileUrl.getURLString();
    if (treeFileName.isEmpty()) {
        treeFileName = GUrlUtils::rollFileName(msaURL.dirPath() + "/" + msaURL.baseFileName() + ".nwk", DocumentUtils::getNewDocFileNameExcludesHint());
    }

    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::NEWICK);
    IOAdapterFactory* iof = IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE);

    const QList<Document*> documents = AppContext::getProject()->getDocuments();
    bool isNewDocument = true;
    for (Document* doc : qAsConst(documents)) {
        if (treeFileName == doc->getURLString()) {
            treeDocument = doc;
            isNewDocument = false;
            break;
        }
    }

    if (treeDocument == nullptr) {
        U2OpStatus2Log os;
        treeDocument = df->createNewLoadedDocument(iof, treeFileName, os);
        CHECK_OP(os, );
    }

    if (isNewDocument) {
        U2OpStatus2Log os;
        newTreeObj = PhyTreeObject::createInstance(treeGeneratorTask->getResult(), "Tree", treeDocument->getDbiRef(), os);
        CHECK_OP(os, );
        treeDocument->addObject(newTreeObj);
    } else if (!treeDocument->isLoaded()) {
        phyTree = treeGeneratorTask->getResult();
        auto loadUnloadedDocumentTask = new LoadUnloadedDocumentTask(treeDocument);
        connect(new TaskSignalMapper(loadUnloadedDocumentTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_onPhyTreeDocLoaded(Task*)));
        AppContext::getTaskScheduler()->registerTopLevelTask(loadUnloadedDocumentTask);
        return;
    } else {
        const QList<GObject*>& objects = treeDocument->getObjects();
        for (GObject* obj : qAsConst(objects)) {
            if (auto treeObj = qobject_cast<PhyTreeObject*>(obj)) {
                treeObj->setTree(treeGeneratorTask->getResult());
                newTreeObj = treeObj;
            }
        }
    }

    if (!p->getDocuments().contains(treeDocument)) {
        p->addDocument(treeDocument);
    }

    if (isNewDocument) {
        msaObject->addObjectRelation(GObjectRelation(GObjectReference(newTreeObj), ObjectRole_PhylogeneticTree));
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(new SaveDocumentTask(treeDocument));

    openTreeViewer(newTreeObj);
}

void MsaEditorTreeManager::sl_onPhyTreeDocLoaded(Task* task) {
    auto loadTask = qobject_cast<LoadUnloadedDocumentTask*>(task);
    treeDocument = loadTask->getDocument();
    PhyTreeObject* treeObj = nullptr;
    for (GObject* obj : qAsConst(treeDocument->getObjects())) {
        treeObj = qobject_cast<PhyTreeObject*>(obj);
        if (treeObj != nullptr) {
            treeObj->setTree(phyTree);
            break;
        }
    }
    openTreeViewer(treeObj);
}

void MsaEditorTreeManager::openTreeViewer(PhyTreeObject* treeObj) {
    Task* openTask = settings.displayWithAlignmentEditor
                         ? new MSAEditorOpenTreeViewerTask(treeObj, this)
                         : new OpenTreeViewerTask(treeObj, this);
    AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
}

MsaEditor* MsaEditorTreeManager::getMsaEditor() const {
    return editor;
}

void MsaEditorTreeManager::sl_openTreeTaskFinished(Task* task) {
    auto createTreeViewerTask = qobject_cast<CreateMSAEditorTreeViewerTask*>(task);
    CHECK(createTreeViewerTask != nullptr, );

    if (!settings.displayWithAlignmentEditor) {
        auto objectViewWindow = new GObjectViewWindow(createTreeViewerTask->getTreeViewer(), editor->getName(), !createTreeViewerTask->getStateData().isEmpty());
        MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
        mdiManager->addMDIWindow(objectViewWindow);
        return;
    }

    auto treeViewer = qobject_cast<MsaEditorTreeViewer*>(createTreeViewerTask->getTreeViewer());
    SAFE_POINT(treeViewer != nullptr, "Can not convert TreeViewer* to MSAEditorTreeViewer* in function MSAEditorTreeManager::sl_openTreeTaskFinished(Task* t)", );

    auto viewWindow = new GObjectViewWindow(treeViewer, editor->getName(), !createTreeViewerTask->getStateData().isEmpty());
    connect(viewWindow, SIGNAL(si_windowClosed(GObjectViewWindow*)), this, SLOT(sl_onWindowClosed(GObjectViewWindow*)));

    auto msaUI = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI(0));
    msaUI->addTreeView(viewWindow);

    // Once tree is added to the splitter make the tree-view viewport state consistent:
    // scroll to the top-right corner to make sequence names visible.
    QTimer::singleShot(0, treeViewer, [treeViewer]() {
        QGraphicsView* ui = treeViewer->getTreeViewerUI();
        ui->centerOn(ui->scene()->width(), 0);
    });

    if (!addExistingTree) {
        treeViewer->setCreatePhyTreeSettings(settings);
        treeViewer->setParentAlignmentName(msaObject->getAlignment()->getName());
    }

    if (settings.syncAlignmentWithTree) {
        treeViewer->enableSyncMode();
    }

    connect(treeViewer, &MsaEditorTreeViewer::si_refreshTree, this, &MsaEditorTreeManager::sl_refreshTree);
}

void MsaEditorTreeManager::openTreeFromFile() {
    LastUsedDirHelper h;
    QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::PHYLOGENETIC_TREE});
    QString file = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select files to open..."), h.dir, filter);
    CHECK(!file.isEmpty(), );
    if (QFileInfo::exists(file)) {
        h.url = file;
        loadTreeFromFile(file);
    }
}

void MsaEditorTreeManager::loadTreeFromFile(const QString& treeFileName) {
    addExistingTree = true;
    Document* doc = AppContext::getProject()->findDocumentByURL(treeFileName);
    if (doc != nullptr && doc->isLoaded()) {
        addTreesFromDocument(doc);
        return;
    }
    U2OpStatusImpl os;
    auto loadDocumentTask = LoadDocumentTask::getDefaultLoadDocTask(os, treeFileName);
    CHECK_EXT(loadDocumentTask != nullptr, uiLog.error(tr("Failed to load document: %1").arg(os.getError())), );
    loadDocumentTask->moveDocumentToMainThread = true;
    AppContext::getTaskScheduler()->registerTopLevelTask(loadDocumentTask);
    connect(loadDocumentTask, &Task::si_stateChanged, [this, loadDocumentTask] {
        CHECK(loadDocumentTask->isFinished() && !loadDocumentTask->isCanceled(), );
        CHECK_EXT(!loadDocumentTask->hasError(), uiLog.error(tr("Tree loading task is finished with error: %1").arg(loadDocumentTask->getError())), );
        Project* project = AppContext::getProject();
        Document* loadedDocument = loadDocumentTask->getDocument();
        Document* documentWithTree;
        Document* existingDocument = project->findDocumentByURL(loadedDocument->getURL());
        if (existingDocument != nullptr && existingDocument->isLoaded()) {
            documentWithTree = existingDocument;  // Re-use document that was loaded in parallel.
        } else {
            CHECK_EXT(!loadedDocument->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE).isEmpty(), uiLog.error(tr("Document contains no tree objects!")), );
            if (existingDocument != nullptr) {  // Remove unloaded document.
                project->removeDocument(existingDocument);
            }
            documentWithTree = loadDocumentTask->takeDocument();
            project->addDocument(documentWithTree);
        }
        addTreesFromDocument(documentWithTree);
    });
}

void MsaEditorTreeManager::addTreesFromDocument(Document* document) {
    SAFE_POINT(document != nullptr, "addTreesFromDocument: Document is null", );
    QList<GObject*> treeObjectList = document->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE);
    CHECK_EXT(!treeObjectList.isEmpty(), uiLog.error(tr("Document contains no tree objects!")), );
    MsaEditorMultiTreeViewer* multiTreeViewer = getMultiTreeViewer();
    for (GObject* obj : qAsConst(treeObjectList)) {
        auto treeObject = qobject_cast<PhyTreeObject*>(obj);
        if (treeObject == nullptr) {
            continue;
        }
        msaObject->addObjectRelation(GObjectRelation(GObjectReference(treeObject), ObjectRole_PhylogeneticTree));
        if (multiTreeViewer == nullptr || !multiTreeViewer->getTreeNames().contains(document->getName())) {
            AppContext::getTaskScheduler()->registerTopLevelTask(new MSAEditorOpenTreeViewerTask(treeObject, this));
        }
    }
}

void MsaEditorTreeManager::sl_onWindowClosed(GObjectViewWindow* viewWindow) {
    MsaEditorMultiTreeViewer* multiTreeViewer = getMultiTreeViewer();
    CHECK(multiTreeViewer != nullptr, );
    multiTreeViewer->sl_onTabCloseRequested(viewWindow);
}

MsaEditorMultiTreeViewer* MsaEditorTreeManager::getMultiTreeViewer() const {
    SAFE_POINT(editor != nullptr, "Incorrect reference to the MSAEditor", nullptr);
    auto mui = qobject_cast<MsaEditorMultilineWgt*>(editor->getUI());
    SAFE_POINT(mui != nullptr, "Incorrect reference to the MSAEditor", nullptr);
    return mui->getPhylTreeWidget();
}

}  // namespace U2
