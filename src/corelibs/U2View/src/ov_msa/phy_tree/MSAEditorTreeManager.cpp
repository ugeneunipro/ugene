/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "MSAEditorTreeManager.h"

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
#include <U2Core/GObjectRelationRoles.h>
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

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>

#include "MsaEditorTreeTabArea.h"
#include "ov_msa/phy_tree/MSAEditorMultiTreeViewer.h"
#include "ov_msa/phy_tree_tab/TreeOptionsWidgetFactory.h"
#include "ov_phyltree/GraphicsRectangularBranchItem.h"
#include "ov_phyltree/TreeViewer.h"
#include "ov_phyltree/TreeViewerTasks.h"
#include "phyltree/CreatePhyTreeDialogController.h"

namespace U2 {
MSAEditorTreeManager::MSAEditorTreeManager(MSAEditor* msaEditor)
    : QObject(msaEditor), editor(msaEditor), msaObject(nullptr), addExistingTree(false) {
    SAFE_POINT(editor != nullptr, "Invalid parameter were passed into constructor MSAEditorTreeManager", );

    Project* project = AppContext::getProject();
    SAFE_POINT(project != nullptr, "Invalid project detected", );

    connect(project, SIGNAL(si_documentRemoved(Document*)), SLOT(sl_onDocumentRemovedFromProject(Document*)));
}

void MSAEditorTreeManager::sl_onDocumentRemovedFromProject(Document* doc) {
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

void MSAEditorTreeManager::loadRelatedTrees() {
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

void MSAEditorTreeManager::buildTreeWithDialog() {
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

    settings.rowsOrder = msaObject->getMultipleAlignment()->getRowNames();
    buildTree(settings);
}

void MSAEditorTreeManager::buildTree(const CreatePhyTreeSettings& buildSettings) {
    createPhyTreeGeneratorTask(buildSettings);
}

void MSAEditorTreeManager::sl_refreshTree(MSAEditorTreeViewer* treeViewer) {
    CHECK(canRefreshTree(treeViewer), );

    createPhyTreeGeneratorTask(treeViewer->getCreatePhyTreeSettings(), true, treeViewer);
}

void MSAEditorTreeManager::createPhyTreeGeneratorTask(const CreatePhyTreeSettings& buildSettings, bool refreshExistingTree, MSAEditorTreeViewer* treeViewer) {
    const MultipleSequenceAlignment msa = msaObject->getMultipleAlignment();
    settings = buildSettings;

    auto treeGeneratorTask = new PhyTreeGeneratorLauncherTask(msa, settings);
    if (refreshExistingTree) {
        activeRefreshTasks[treeViewer] = treeGeneratorTask;
        connect(new TaskSignalMapper(treeGeneratorTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_treeRebuildingFinished(Task*)));
        connect(treeViewer, SIGNAL(destroyed()), treeGeneratorTask, SLOT(sl_onCalculationCanceled()));
    } else {
        connect(new TaskSignalMapper(treeGeneratorTask), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_openTree(Task*)));
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(treeGeneratorTask);
}

void MSAEditorTreeManager::sl_treeRebuildingFinished(Task* _treeBuildTask) {
    auto treeBuildTask = qobject_cast<PhyTreeGeneratorLauncherTask*>(_treeBuildTask);
    if (treeBuildTask == nullptr || treeBuildTask->isCanceled()) {
        return;
    }

    MSAEditorTreeViewer* refreshingTree = activeRefreshTasks.key(treeBuildTask);
    CHECK(refreshingTree != nullptr, );
    activeRefreshTasks.remove(refreshingTree);

    PhyTreeObject* treeObj = refreshingTree->getPhyObject();
    treeObj->setTree(treeBuildTask->getResult());
}

bool MSAEditorTreeManager::canRefreshTree(MSAEditorTreeViewer* treeViewer) {
    bool canRefresh = (treeViewer->getParentAlignmentName() == msaObject->getMultipleAlignment()->getName());
    return canRefresh && !activeRefreshTasks.contains(treeViewer);
}

void MSAEditorTreeManager::sl_openTree(Task* treeBuildTask) {
    CHECK(treeBuildTask != nullptr && !treeBuildTask->isCanceled(), )

    PhyTreeGeneratorLauncherTask* treeGeneratorTask = qobject_cast<PhyTreeGeneratorLauncherTask*>(treeBuildTask);
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
        LoadUnloadedDocumentTask* t = new LoadUnloadedDocumentTask(treeDocument);
        connect(new TaskSignalMapper(t), SIGNAL(si_taskSucceeded(Task*)), SLOT(sl_onPhyTreeDocLoaded(Task*)));
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
        return;
    } else {
        const QList<GObject*>& objects = treeDocument->getObjects();
        for (GObject* obj : qAsConst(objects)) {
            PhyTreeObject* treeObj = qobject_cast<PhyTreeObject*>(obj);
            if (treeObj) {
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

void MSAEditorTreeManager::sl_onPhyTreeDocLoaded(Task* task) {
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

void MSAEditorTreeManager::openTreeViewer(PhyTreeObject* treeObj) {
    Task* openTask = settings.displayWithAlignmentEditor
                         ? new MSAEditorOpenTreeViewerTask(treeObj, this)
                         : new OpenTreeViewerTask(treeObj, this);
    AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
}

void MSAEditorTreeManager::sl_openTreeTaskFinished(Task* task) {
    auto createTreeViewerTask = qobject_cast<CreateMSAEditorTreeViewerTask*>(task);
    CHECK(createTreeViewerTask != nullptr, );

    if (!settings.displayWithAlignmentEditor) {
        GObjectViewWindow* w = new GObjectViewWindow(createTreeViewerTask->getTreeViewer(), editor->getName(), !createTreeViewerTask->getStateData().isEmpty());
        MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
        mdiManager->addMDIWindow(w);
        return;
    }

    auto treeViewer = qobject_cast<MSAEditorTreeViewer*>(createTreeViewerTask->getTreeViewer());
    SAFE_POINT(treeViewer != nullptr, tr("Can not convert TreeViewer* to MSAEditorTreeViewer* in function MSAEditorTreeManager::sl_openTreeTaskFinished(Task* t)"), );

    // TODO: pass MSA editor to the constructor of MSAEditorTreeViewer and avoid extra state when MSAEditorTreeViewer has no msaEditor assigned.
    treeViewer->setMSAEditor(editor);

    auto viewWindow = new GObjectViewWindow(treeViewer, editor->getName(), !createTreeViewerTask->getStateData().isEmpty());
    connect(viewWindow, SIGNAL(si_windowClosed(GObjectViewWindow*)), this, SLOT(sl_onWindowClosed(GObjectViewWindow*)));

    MsaEditorWgt *msaUI = qobject_cast<MsaEditorWgt *>(editor->getUI()->getUI());
    msaUI->addTreeView(viewWindow);

    // Once tree is added to the splitter make the tree-view viewport state consistent:
    // scroll to the top-right corner to make sequence names visible.
    QTimer::singleShot(0, treeViewer, [treeViewer]() {
        QGraphicsView* ui = treeViewer->getTreeViewerUI();
        ui->centerOn(ui->scene()->width(), 0);
    });

    if (!addExistingTree) {
        treeViewer->setCreatePhyTreeSettings(settings);
        treeViewer->setParentAignmentName(msaObject->getMultipleAlignment()->getName());
    }

    if (settings.syncAlignmentWithTree) {
        treeViewer->enableSyncMode();
    }

    connect(treeViewer, SIGNAL(si_refreshTree(MSAEditorTreeViewer*)), SLOT(sl_refreshTree(MSAEditorTreeViewer*)));
}

void MSAEditorTreeManager::openTreeFromFile() {
    LastUsedDirHelper h;
    QString filter = FileFilters::createFileFilterByObjectTypes({BaseDocumentFormats::NEWICK});
    QString file;
#ifdef Q_OS_DARWIN
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        file = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select files to open..."), h.dir, filter, 0, QFileDialog::DontUseNativeDialog);
    } else
#endif
        file = U2FileDialog::getOpenFileName(QApplication::activeWindow(), tr("Select files to open..."), h.dir, filter);
    CHECK(!file.isEmpty(), );
    if (QFileInfo(file).exists()) {
        h.url = file;
        loadTreeFromFile(file);
    }
}

void MSAEditorTreeManager::loadTreeFromFile(const QString& treeFileName) {
    addExistingTree = true;
    bool isNewDocument = true;
    Document* doc = nullptr;
    const QList<Document*> documents = AppContext::getProject()->getDocuments();
    foreach (doc, documents) {
        if (doc->getURLString() == treeFileName) {
            isNewDocument = false;
            break;
        }
    }

    if (isNewDocument || !doc->isLoaded()) {
        if (!isNewDocument && !doc->isLoaded()) {
            if (AppContext::getProject()->getDocuments().contains(doc)) {
                AppContext::getProject()->removeDocument(doc);
            }
        }
        U2OpStatus2Log os;
        IOAdapterFactory* ioFactory = IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE);
        DocumentFormat* documentFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::NEWICK);
        doc = documentFormat->loadDocument(ioFactory, treeFileName, QVariantMap(), os);
        CHECK(doc != nullptr, );
        AppContext::getProject()->addDocument(doc);
    }

    const QList<GObject*> treeObjectList = doc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE);
    for (GObject* obj : qAsConst(treeObjectList)) {
        auto treeObject = qobject_cast<PhyTreeObject*>(obj);
        msaObject->addObjectRelation(GObjectRelation(GObjectReference(treeObject), ObjectRole_PhylogeneticTree));
        if (treeObject == nullptr) {
            continue;
        }
        const MSAEditorMultiTreeViewer* multiTreeViewer = getMultiTreeViewer();
        if (multiTreeViewer == nullptr || !multiTreeViewer->getTreeNames().contains(doc->getName())) {
            AppContext::getTaskScheduler()->registerTopLevelTask(new MSAEditorOpenTreeViewerTask(treeObject, this));
        }
    }
}

void MSAEditorTreeManager::sl_onWindowClosed(GObjectViewWindow* viewWindow) {
    MSAEditorMultiTreeViewer* multiTreeViewer = getMultiTreeViewer();
    CHECK(multiTreeViewer != nullptr, );
    multiTreeViewer->sl_onTabCloseRequested(viewWindow);
}

MSAEditorMultiTreeViewer* MSAEditorTreeManager::getMultiTreeViewer() const {
    SAFE_POINT(editor != nullptr, tr("Incorrect reference to the MSAEditor"), nullptr);
    MsaEditorWgt *msaEditorUi = qobject_cast<MsaEditorWgt *>(editor->getUI()->getUI());
    SAFE_POINT(msaEditorUi != nullptr, tr("Incorrect reference to the MSAEditor"), nullptr);
    return msaEditorUi->getMultiTreeViewer();
}

}  // namespace U2
