
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

#include "MSAEditorTreeViewer.h"

#include <QCursor>
#include <QDateTime>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStack>
#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include <U2View/GraphicsButtonItem.h>
#include <U2View/GraphicsRectangularBranchItem.h>
#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>

namespace U2 {

MSAEditorTreeViewer::MSAEditorTreeViewer(const QString& viewName, GObject* obj, GraphicsRectangularBranchItem* _root, qreal s)
    : TreeViewer(viewName, obj, _root, s) {
}

MSAEditorTreeViewer::~MSAEditorTreeViewer() {
    if (editor != nullptr && isSyncModeEnabled()) {
        MsaEditorWgt* msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI(0));
        if (msaEditorUi != nullptr) {
            msaEditorUi->getSequenceArea()->disableFreeRowOrderMode(this);
        }
    }
}

QWidget* MSAEditorTreeViewer::createWidget() {
    SAFE_POINT(ui == nullptr, QString("MSAEditorTreeViewer::createWidget error"), nullptr);
    SAFE_POINT(editor != nullptr, "MSAEditor must be set in createWidget!", nullptr);

    auto view = new QWidget();
    view->setObjectName("msa_editor_tree_view_container_widget");

    auto viewLayout = new QVBoxLayout();
    msaTreeViewerUi = new MSAEditorTreeViewerUI(this);
    ui = msaTreeViewerUi;

    auto toolBar = new QToolBar(tr("MSAEditor tree toolbar"));
    buildMSAEditorStaticToolbar(toolBar);

    syncModeAction = new QAction(ui);
    syncModeAction->setCheckable(true);
    syncModeAction->setObjectName("sync_msa_action");
    updateSyncModeActionState(false);
    connect(syncModeAction, SIGNAL(triggered()), SLOT(sl_syncModeActionTriggered()));

    refreshTreeAction = new QAction(QIcon(":core/images/refresh.png"), tr("Refresh tree"), ui);
    refreshTreeAction->setObjectName("Refresh tree");
    refreshTreeAction->setEnabled(false);
    connect(refreshTreeAction, SIGNAL(triggered()), SLOT(sl_refreshTree()));

    toolBar->addAction(refreshTreeAction);
    toolBar->addAction(syncModeAction);

    viewLayout->setSpacing(0);
    viewLayout->setMargin(0);
    viewLayout->addWidget(toolBar);
    viewLayout->addWidget(ui);
    view->setLayout(viewLayout);

    connect(editor, SIGNAL(si_referenceSeqChanged(qint64)), msaTreeViewerUi, SLOT(sl_onReferenceSeqChanged(qint64)));
    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), this, SLOT(sl_alignmentChanged()));

    MaCollapseModel* collapseModel = editor->getCollapseModel();
    connect(collapseModel, SIGNAL(si_toggled()), this, SLOT(sl_alignmentCollapseModelChanged()));

    MsaEditorWgt* msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI());
    MSAEditorSequenceArea* msaSequenceArea = msaEditorUi->getSequenceArea();
    connect(msaSequenceArea, SIGNAL(si_visibleRangeChanged(QStringList, int)), msaTreeViewerUi, SLOT(sl_onVisibleRangeChanged(const QStringList&, int)));
    connect(msaSequenceArea, SIGNAL(si_selectionChanged(const QStringList&)), msaTreeViewerUi, SLOT(sl_selectionChanged(const QStringList&)));

    MaEditorNameList* msaNameList = editor->getMaEditorWgt()->getEditorNameList();
    connect(msaNameList, SIGNAL(si_sequenceNameChanged(QString, QString)), msaTreeViewerUi, SLOT(sl_sequenceNameChanged(QString, QString)));

    return view;
}
const CreatePhyTreeSettings& MSAEditorTreeViewer::getCreatePhyTreeSettings() const {
    return buildSettings;
}

const QString& MSAEditorTreeViewer::getParentAlignmentName() const {
    return alignmentName;
}

OptionsPanel* MSAEditorTreeViewer::getOptionsPanel() {
    return nullptr;
}

void MSAEditorTreeViewer::setParentAignmentName(const QString& _alignmentName) {
    alignmentName = _alignmentName;
}

QAction* MSAEditorTreeViewer::getSortSeqsAction() const {
    return syncModeAction;
}

void MSAEditorTreeViewer::updateSyncModeActionState(bool isSyncModeOn) {
    bool isEnabled = editor != nullptr && checkTreeAndMsaCanBeSynchronized();
    syncModeAction->setEnabled(isEnabled);

    bool isChecked = isEnabled && isSyncModeOn;  // Override 'isSyncModeOn' with a safer option.
    syncModeAction->setChecked(isChecked);
    syncModeAction->setText(isChecked ? tr("Disable Tree and Alignment synchronization") : tr("Enable Tree and Alignment synchronization"));
    syncModeAction->setIcon(QIcon(isChecked ? ":core/images/sync-msa-on.png" : ":core/images/sync-msa-off.png"));

    msaTreeViewerUi->updateRect();
}

void MSAEditorTreeViewer::setMSAEditor(MSAEditor* newEditor) {
    SAFE_POINT(newEditor != nullptr, "MSAEditor can't be null!", );
    SAFE_POINT(editor == nullptr, "MSAEditor can't be set twice!", );
    editor = newEditor;
}

MSAEditor* MSAEditorTreeViewer::getMsaEditor() const {
    return editor;
}

void MSAEditorTreeViewer::setCreatePhyTreeSettings(const CreatePhyTreeSettings& newBuildSettings) {
    buildSettings = newBuildSettings;
    refreshTreeAction->setEnabled(true);
}

void MSAEditorTreeViewer::sl_refreshTree() {
    emit si_refreshTree(this);
}

bool MSAEditorTreeViewer::enableSyncMode() {
    if (!checkTreeAndMsaCanBeSynchronized()) {
        updateSyncModeActionState(false);
        return false;
    }
    orderAlignmentByTree();
    msaTreeViewerUi->highlightBranches();
    updateSyncModeActionState(true);

    // Trigger si_visibleRangeChanged that will make tree widget update geometry to the correct scale. TODO: create a better API for this.
    editor->getMaEditorWgt()->getSequenceArea()->onVisibleRangeChanged();

    return true;
}

void MSAEditorTreeViewer::disableSyncMode() {
    MsaEditorWgt* msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI());
    // Reset the MSA state back to the original from 'Free'.
    msaEditorUi->getSequenceArea()->disableFreeRowOrderMode(this);

    MaEditorNameList* msaNameList = editor->getMaEditorWgt()->getEditorNameList();
    msaNameList->update();

    updateSyncModeActionState(false);
}

bool MSAEditorTreeViewer::isSyncModeEnabled() const {
    return syncModeAction->isChecked();
}

void MSAEditorTreeViewer::sl_alignmentChanged() {
    disableSyncModeIfTreeAndMsaContentIsNotInSync();
}

void MSAEditorTreeViewer::sl_alignmentCollapseModelChanged() {
    disableSyncModeIfTreeAndMsaContentIsNotInSync();
}

void MSAEditorTreeViewer::disableSyncModeIfTreeAndMsaContentIsNotInSync() {
    if (!checkTreeAndMsaNameListsAreSynchronized()) {
        // Disable sync mode if MSA modification breaks sync mode.
        disableSyncMode();
    }
}

bool MSAEditorTreeViewer::checkTreeAndMsaNameListsAreSynchronized() const {
    QList<QStringList> groupStateGuidedByTree = msaTreeViewerUi->getGroupingStateForMsa();
    QStringList treeNameList;  // The list of sequences names to compare with MSA state.
    for (const QStringList& namesInGroup : qAsConst(groupStateGuidedByTree)) {
        SAFE_POINT(!namesInGroup.isEmpty(), "Group must have at least 1 sequence!", false);
        treeNameList << namesInGroup[0];
    }
    const MaCollapseModel* collapseModel = editor->getCollapseModel();
    int msaViewRowCount = collapseModel->getViewRowCount();
    if (msaViewRowCount != treeNameList.size()) {
        return false;
    }
    MultipleSequenceAlignmentObject* maObject = editor->getMaObject();
    for (int viewRowIndex = 0; viewRowIndex < msaViewRowCount; viewRowIndex++) {
        int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
        const MultipleAlignmentRow& row = maObject->getRow(maRowIndex);
        QString msaRowSequenceName = row->getName();
        if (msaRowSequenceName != treeNameList[viewRowIndex]) {
            return false;
        }
    }
    return true;
}

bool MSAEditorTreeViewer::checkTreeAndMsaCanBeSynchronized() const {
    CHECK(msaTreeViewerUi->isRectangularLayoutMode(), false);  // Only 'rect' mode can be synchronized.

    QStringList treeNameList;  // The list of sequences names in the tree.
    QList<QStringList> groupStateGuidedByTree = msaTreeViewerUi->getGroupingStateForMsa();
    for (const QStringList& namesInGroup : qAsConst(groupStateGuidedByTree)) {
        treeNameList.append(namesInGroup);
    }
    QSet<QString> treeNameSet = treeNameList.toSet();
    bool treeHasUniqueNames = treeNameSet.size() == treeNameList.size();
    CHECK(treeHasUniqueNames, false);  // Tree is ambiguous: there is no straight way to map tree branches to MSA sequences.

    QStringList msaNameList = editor->getMaObject()->getMsa()->getRowNames();  // The list of sequences names in the MSA.
    QSet<QString> msaNameSet = msaNameList.toSet();
    bool msaHasUniqueNames = msaNameSet.size() == msaNameList.size();
    CHECK(msaHasUniqueNames, false);  // MSA is ambiguos: : there is no straight way to map tree branches to MSA sequences.

    // Check that 2 name lists are identical.
    return treeNameSet == msaNameSet;
}

void MSAEditorTreeViewer::sl_syncModeActionTriggered() {
    if (syncModeAction->isChecked()) {
        enableSyncMode();
    } else {
        disableSyncMode();
    }
}

void MSAEditorTreeViewer::orderAlignmentByTree() {
    QList<QStringList> groupList = msaTreeViewerUi->getGroupingStateForMsa();
    MsaEditorWgt* msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI());
    msaEditorUi->getSequenceArea()->enableFreeRowOrderMode(this, groupList);
}

//---------------------------------------------
// MSAEditorTreeViewerUI
//---------------------------------------------
MSAEditorTreeViewerUI::MSAEditorTreeViewerUI(MSAEditorTreeViewer* treeViewer)
    : TreeViewerUI(treeViewer), isRectangularLayout(true),
      msaEditorTreeViewer(treeViewer) {
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void MSAEditorTreeViewerUI::sl_selectionChanged(const QStringList& selectedSequenceNameList) {
    CHECK(msaEditorTreeViewer->isSyncModeEnabled(), );
    bool cleanSelection = true;
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        auto branchItem = dynamic_cast<GraphicsBranchItem*>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        if (selectedSequenceNameList.contains(nameItem->text(), Qt::CaseInsensitive)) {
            if (cleanSelection) {
                cleanSelection = false;
                getRoot()->setSelectedRecurs(false, true);
            }
            branchItem->setSelectedRecurs(true, false);
        } else {
            branchItem->setSelectedRecurs(false, false);
        }
    }
}

void MSAEditorTreeViewerUI::sl_sequenceNameChanged(QString prevName, QString newName) {
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        GraphicsBranchItem* branchItem = dynamic_cast<GraphicsBranchItem*>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        if (prevName == nameItem->text()) {
            nameItem->setText(newName);
        }
    }
    scene()->update();
}

void MSAEditorTreeViewerUI::onLayoutChanged(const TreeLayout& layout) {
    isRectangularLayout = layout == RECTANGULAR_LAYOUT;
    msaEditorTreeViewer->getSortSeqsAction()->setEnabled(false);
    if (isRectangularLayout) {
        if (msaEditorTreeViewer->isSyncModeEnabled()) {
            msaEditorTreeViewer->getSortSeqsAction()->setEnabled(true);
            MSAEditor* msa = msaEditorTreeViewer->getMsaEditor();
            CHECK(msa != nullptr, );
            msa->getMaEditorWgt()->getSequenceArea()->onVisibleRangeChanged();
        }
    }
}

void MSAEditorTreeViewerUI::sl_onReferenceSeqChanged(qint64) {
    CHECK(msaEditorTreeViewer->isSyncModeEnabled(), );

    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        auto branchItem = dynamic_cast<GraphicsBranchItem*>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        // TODO: heh?
        QPen brush(Qt::white);
    }
    scene()->update();
}

void MSAEditorTreeViewerUI::highlightBranches() {
    OptionsMap rootSettings = rectRoot->getSettings();
    rootSettings[BRANCH_COLOR] = static_cast<int>(Qt::black);
    if (rectRoot) {
        rectRoot->updateSettings(rootSettings);
        rectRoot->updateChildSettings(rootSettings);
    }
}

QList<GraphicsBranchItem*> MSAEditorTreeViewerUI::getBranchItemsWithNames() const {
    QList<QGraphicsItem*> sceneItemList = scene()->items();
    QList<GraphicsBranchItem*> result;
    for (QGraphicsItem* item : qAsConst(sceneItemList)) {
        auto branchItem = dynamic_cast<GraphicsBranchItem*>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if (nameItem != nullptr) {
            result.append(branchItem);
        }
    }
    return result;
}

void MSAEditorTreeViewerUI::updateScene(bool) {
    // A tree viewer embedded into MSA editor never uses 'fitSceneToView' option today:
    // 1. The option is not compatible with sync mode.
    // 2. If sync mode if OFF:
    //   2.1. 'fit-to-view' will fit the tree into a limited screen space and will cause tree text labels overlap (tree labels do not scale).
    //   2.2. There are no tree-related zoom actions in Sync-OFF mode, so a user can't fix the bad looking tree layout from 2.1.
    // Until the issues above are not resolved we enforce 'fit-to-screen' to be false.
    // With 'fit-to-screen' equal to false a tree is rendered using the default zoom level and enables scroll bars to handle overflow.
    TreeViewerUI::updateScene(false);

    MSAEditor* msaEditor = msaEditorTreeViewer->getMsaEditor();
    CHECK(msaEditor != nullptr, );
    msaEditor->getMaEditorWgt()->getSequenceArea()->onVisibleRangeChanged();
    updateRect();
}

void MSAEditorTreeViewerUI::sl_rectLayoutRecomputed() {
    QMatrix curMatrix = matrix();
    TreeViewerUI::sl_rectLayoutRecomputed();
    setMatrix(curMatrix);
}

void MSAEditorTreeViewerUI::sl_onBranchCollapsed(GraphicsRectangularBranchItem* branch) {
    TreeViewerUI::sl_onBranchCollapsed(branch);
    if (msaEditorTreeViewer->isSyncModeEnabled()) {
        msaEditorTreeViewer->orderAlignmentByTree();
    }
}

QList<QStringList> MSAEditorTreeViewerUI::getGroupingStateForMsa() const {
    QList<QStringList> groupList;

    // treeBranchStack is used here for Depth-First-Search algorithm implementation with no recursion.
    QStack<const GraphicsBranchItem*> treeBranchStack;
    treeBranchStack.push(getRoot());

    while (!treeBranchStack.isEmpty()) {
        const GraphicsBranchItem* branchItem = treeBranchStack.pop();
        if (branchItem->isCollapsed()) {
            groupList.append(MSAEditorTreeViewerUtils::getSeqsNamesInBranch(branchItem));
            continue;
        }

        QGraphicsSimpleTextItem* branchNameItem = branchItem->getNameText();
        if (branchNameItem != nullptr && !branchNameItem->text().isEmpty()) {
            // Add this leaf of as a separate non-grouped sequence to the list.
            groupList.append({branchNameItem->text()});
            continue;
        }

        QList<QGraphicsItem*> childItemList = branchItem->childItems();

        // Sort items by Y, so virtual order will be the same with the tree.
        // The item with the higher Y must go first: this way it will be processed last when pushed to the stack below.
        std::sort(childItemList.begin(), childItemList.end(), [](const QGraphicsItem* item1, const QGraphicsItem* item2) {
            return item1->y() > item2->y();
        });

        for (QGraphicsItem* childItem : qAsConst(childItemList)) {
            auto childBranchItem = dynamic_cast<GraphicsBranchItem*>(childItem);
            if (childBranchItem == nullptr) {
                continue;
            }
            treeBranchStack.push(childBranchItem);
        }
    }
    return groupList;
}

QStringList MSAEditorTreeViewerUtils::getSeqsNamesInBranch(const GraphicsBranchItem* branch) {
    QStringList seqNames;
    QStack<const GraphicsBranchItem*> treeBranches;
    treeBranches.push(branch);

    do {
        const GraphicsBranchItem* parentBranch = treeBranches.pop();

        QList<QGraphicsItem*> childItemList = parentBranch->childItems();
        for (QGraphicsItem* graphItem : qAsConst(childItemList)) {
            auto childrenBranch = dynamic_cast<GraphicsBranchItem*>(graphItem);
            if (childrenBranch == nullptr) {
                continue;
            }
            QGraphicsSimpleTextItem* nameItem = childrenBranch->getNameText();
            if (nameItem == nullptr) {
                treeBranches.push(childrenBranch);
                continue;
            }

            QString seqName = nameItem->text();
            if (!seqName.isEmpty()) {
                seqNames.append(seqName);
                continue;
            }
            treeBranches.push(childrenBranch);
        }
    } while (!treeBranches.isEmpty());

    return seqNames;
}

}  // namespace U2
