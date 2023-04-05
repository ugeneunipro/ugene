
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

#include "MSAEditorTreeViewer.h"

#include <QCursor>
#include <QDateTime>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStack>
#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/TvNodeItem.h>
#include <U2View/TvRectangularBranchItem.h>
#include <U2View/TvTextItem.h>

namespace U2 {

MSAEditorTreeViewer::MSAEditorTreeViewer(const QString& viewName, PhyTreeObject* phyTreeObject)
    : TreeViewer(viewName, phyTreeObject, false) {
}

MSAEditorTreeViewer::~MSAEditorTreeViewer() {
    if (editor != nullptr && isSyncModeEnabled()) {
        auto msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI(0));
        if (msaEditorUi != nullptr) {
            msaEditorUi->getSequenceArea()->disableFreeRowOrderMode(this);
        }
    }
}

QWidget* MSAEditorTreeViewer::createViewWidget(QWidget* parent) {
    SAFE_POINT(ui == nullptr, QString("MSAEditorTreeViewer::createWidget error"), nullptr);
    SAFE_POINT(editor != nullptr, "MSAEditor must be set in createWidget!", nullptr);

    auto view = new QWidget(parent);
    view->setObjectName("msa_editor_tree_view_container_widget");

    msaTreeViewerUi = new MSAEditorTreeViewerUI(this, view);
    ui = msaTreeViewerUi;

    auto toolBar = new QToolBar(tr("MSAEditor tree toolbar"));
    buildStaticToolbar(toolBar);

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

    auto viewLayout = new QVBoxLayout();
    viewLayout->setSpacing(0);
    viewLayout->setMargin(0);
    viewLayout->addWidget(toolBar);
    viewLayout->addWidget(ui);
    view->setLayout(viewLayout);

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), this, SLOT(sl_alignmentChanged()));

    MaCollapseModel* collapseModel = editor->getCollapseModel();
    connect(collapseModel, SIGNAL(si_toggled()), this, SLOT(sl_alignmentCollapseModelChanged()));

    auto msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI(0));
    SAFE_POINT(msaEditorUi != nullptr, "MSAEditorTreeViewer::createWidget: msaEditorUi is null!", nullptr);
    MSAEditorSequenceArea* msaSequenceArea = msaEditorUi->getSequenceArea();
    connect(msaSequenceArea, SIGNAL(si_selectionChanged(const QStringList&)), msaTreeViewerUi, SLOT(sl_selectionChanged(const QStringList&)));

    MaEditorNameList* msaNameList = editor->getMaEditorWgt()->getEditorNameList();
    connect(msaNameList, &MaEditorNameList::si_sequenceNameChanged, msaTreeViewerUi, &MSAEditorTreeViewerUI::sl_sequenceNameChanged);

    return view;
}
const CreatePhyTreeSettings& MSAEditorTreeViewer::getCreatePhyTreeSettings() const {
    return buildSettings;
}

const QString& MSAEditorTreeViewer::getParentAlignmentName() const {
    return alignmentName;
}

void MSAEditorTreeViewer::setParentAlignmentName(const QString& _alignmentName) {
    alignmentName = _alignmentName;
}

void MSAEditorTreeViewer::updateSyncModeActionState(bool isSyncModeOn) {
    bool isEnabled = editor != nullptr && checkTreeAndMsaCanBeSynchronized();
    syncModeAction->setEnabled(isEnabled);

    bool isChecked = isEnabled && isSyncModeOn;  // Override 'isSyncModeOn' with a safer option.
    syncModeAction->setChecked(isChecked);
    syncModeAction->setText(isChecked ? tr("Disable Tree and Alignment synchronization") : tr("Enable Tree and Alignment synchronization"));
    syncModeAction->setIcon(QIcon(isChecked ? ":core/images/sync-msa-on.png" : ":core/images/sync-msa-off.png"));
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
    updateSyncModeActionState(true);

    // Trigger si_visibleRangeChanged that will make tree widget update geometry to the correct scale. TODO: create a better API for this.
    editor->getMaEditorWgt()->getSequenceArea()->onVisibleRangeChanged();

    return true;
}

void MSAEditorTreeViewer::disableSyncMode() {
    auto msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI(0));
    SAFE_POINT(msaEditorUi != nullptr, "MSAEditorTreeViewer::disableSyncMode msaEditorUi is null!", );
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
    CHECK(msaHasUniqueNames, false);  // MSA is ambiguous: there is no straight way to map tree branches to MSA sequences.

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
    auto msaEditorUi = qobject_cast<MsaEditorWgt*>(editor->getUI()->getUI(0));
    SAFE_POINT(msaEditorUi != nullptr, "MSAEditorTreeViewer::orderAlignmentByTree: msaEditorUi is null", );
    msaEditorUi->getSequenceArea()->enableFreeRowOrderMode(this, groupList);
}

//---------------------------------------------
// MSAEditorTreeViewerUI
//---------------------------------------------
MSAEditorTreeViewerUI::MSAEditorTreeViewerUI(MSAEditorTreeViewer* treeViewer, QWidget* parent)
    : TreeViewerUI(treeViewer, parent),
      msaEditorTreeViewer(treeViewer) {
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void MSAEditorTreeViewerUI::sl_selectionChanged(const QStringList& selectedSequenceNameList) {
    CHECK(msaEditorTreeViewer->isSyncModeEnabled(), );
    getRoot()->setSelectedRecursively(false);
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        auto branchItem = dynamic_cast<TvBranchItem*>(item);
        if (branchItem == nullptr) {
            continue;
        }
        TvTextItem* nameItem = branchItem->getNameTextItem();
        if (nameItem == nullptr) {
            continue;
        }
        branchItem->setSelectedRecursively(selectedSequenceNameList.contains(nameItem->text(), Qt::CaseInsensitive));
    }
}

void MSAEditorTreeViewerUI::sl_sequenceNameChanged(const QString& prevName, const QString& newName) {
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        auto branchItem = dynamic_cast<TvBranchItem*>(item);
        if (branchItem == nullptr) {
            continue;
        }
        TvTextItem* nameItem = branchItem->getNameTextItem();
        if (nameItem == nullptr) {
            continue;
        }
        if (prevName == nameItem->text()) {
            nameItem->setText(newName);
        }
    }
    scene()->update();
}

void MSAEditorTreeViewerUI::sl_onBranchCollapsed(TvBranchItem* branch) {
    TreeViewerUI::sl_onBranchCollapsed(branch);
    if (msaEditorTreeViewer->isSyncModeEnabled()) {
        msaEditorTreeViewer->orderAlignmentByTree();
    }
}

QList<QStringList> MSAEditorTreeViewerUI::getGroupingStateForMsa() const {
    QList<QStringList> groupList;

    // treeBranchStack is used here for Depth-First-Search algorithm implementation with no recursion.
    QStack<TvBranchItem*> treeBranchStack;
    treeBranchStack.push(getRoot());

    while (!treeBranchStack.isEmpty()) {
        TvBranchItem* branchItem = treeBranchStack.pop();
        if (branchItem->isCollapsed()) {
            groupList.append(MSAEditorTreeViewerUtils::getSeqsNamesInBranch(branchItem));
            continue;
        }

        QGraphicsSimpleTextItem* branchNameItem = branchItem->getNameTextItem();
        if (branchNameItem != nullptr && !branchNameItem->text().isEmpty()) {
            // Add this leaf of as a separate non-grouped sequence to the list.
            groupList.append({branchNameItem->text()});
            continue;
        }

        QList<QGraphicsItem*> childItemList = branchItem->childItems();
        // Reverse items, so processing order will be the same with the tree.
        std::reverse(childItemList.begin(), childItemList.end());

        for (QGraphicsItem* childItem : qAsConst(childItemList)) {
            auto childBranchItem = dynamic_cast<TvBranchItem*>(childItem);
            if (childBranchItem != nullptr) {
                treeBranchStack.push(childBranchItem);
            }
        }
    }
    return groupList;
}

QStringList MSAEditorTreeViewerUtils::getSeqsNamesInBranch(const TvBranchItem* branch) {
    QStringList seqNames;
    QStack<const TvBranchItem*> treeBranches;
    treeBranches.push(branch);

    do {
        const TvBranchItem* parentBranch = treeBranches.pop();

        QList<QGraphicsItem*> childItemList = parentBranch->childItems();
        for (QGraphicsItem* graphItem : qAsConst(childItemList)) {
            auto childrenBranch = dynamic_cast<TvBranchItem*>(graphItem);
            if (childrenBranch == nullptr) {
                continue;
            }
            QGraphicsSimpleTextItem* nameItem = childrenBranch->getNameTextItem();
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
