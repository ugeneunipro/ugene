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
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTAction.h>
#include <primitives/GTWidget.h>

#include <QGraphicsView>

#include <U2View/MSAEditor.h>

#include "GTGlobals.h"
#include "GTTestsMSAEditorTree.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_editor_tree {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Check that original name list is correct.
    MSAEditor* msaEditor = GTUtilsMsaEditor::getEditor();
    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    QStringList nameList = msaObject->getMultipleAlignment()->getRowNames();
    QStringList originalNameList = {"a", "b", "c", "d", "e", "f", "g", "h"};
    CHECK_SET_ERR(nameList == originalNameList, "1. Wrong original name list: " + nameList.join(","));

    // Build a tree.
    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "tree_test_0001.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that the tree is opened and MSA order was changed to match the tree order. The original MSA-object order must not change.
    nameList = msaObject->getMultipleAlignment()->getRowNames();
    CHECK_SET_ERR(nameList == originalNameList, "2. Wrong original name list: " + nameList.join(","));
    nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    QStringList expectedExpandedTreeNameList = {"h", "b", "f", "d", "c", "e", "g", "a"};
    CHECK_SET_ERR(nameList == expectedExpandedTreeNameList, "Initial full tree name list not matched: " + nameList.join(","));

    // Collapse subtree. Check that MSA name list has a collapsed group.
    TvNodeItem* parentOfSequenceC = GTUtilsPhyTree::getNodeByBranchText("0", "0");
    GTUtilsPhyTree::doubleClickNode(parentOfSequenceC);
    nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    expectedExpandedTreeNameList = QStringList({"h", "b", "f", "d", "g", "a"});
    CHECK_SET_ERR(nameList == expectedExpandedTreeNameList, "Collapsed tree name list not matched: " + nameList.join(","));

    GTUtilsPhyTree::doubleClickNode(parentOfSequenceC);
    nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    expectedExpandedTreeNameList = QStringList({"h", "b", "f", "d", "c", "e", "g", "a"});
    CHECK_SET_ERR(nameList == expectedExpandedTreeNameList, "Restored full tree name list not matched: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Check that after a tree is built and sync mode is enabled the MaEditorRowOrder::Free mode is enabled:
    //  - 'toggle_sequence_row_order_action' is unchecked.
    //  - 'refresh_sequence_row_order_action' is disabled.
    //  - sequences are ordered by the tree.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    QStringList originalSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "msa_editor_tree_test_0002", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::TreeOptions);

    QStringList byTreeSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    QAbstractButton* toggleSequenceOrderButton = GTAction::button("toggle_sequence_row_order_action");
    QAbstractButton* refreshSequenceOrderButton = GTAction::button("refresh_sequence_row_order_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/1");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/1");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/1");
    CHECK_SET_ERR(originalSequenceNames1 != byTreeSequenceNames1, "MSA must be re-ordered by tree");

    // Switch to the 'Sequence' mode.
    GTWidget::click(toggleSequenceOrderButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/1");
    CHECK_SET_ERR(toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be checked/1");
    CHECK_SET_ERR(refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be enabled/1");

    // Switch to the 'Original' mode.
    GTWidget::click(toggleSequenceOrderButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList originalSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/2");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/2");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/2");
    CHECK_SET_ERR(originalSequenceNames1 == originalSequenceNames2, "original sequence name list is not restored");

    // Switch back to the order-by-tree (MaEditorRowOrder::Free) mode.
    GTWidget::click(syncModeButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList byTreeSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/2");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/3");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/3");
    CHECK_SET_ERR(byTreeSequenceNames1 == byTreeSequenceNames2, "by-tree sequence name list is not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    // Check that when sync mode is turned OFF from inside the Tree widget the MSA sequence order is restored to the Original.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    QStringList originalSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "msa_editor_tree_test_0003_1");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList byTreeSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(originalSequenceNames1 != byTreeSequenceNames1, "MSA must be re-ordered by tree");

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    GTWidget::click(syncModeButton);

    QStringList originalSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(originalSequenceNames2 == originalSequenceNames2, "Original sequence order must be restored");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    // Check that a Tree tab that is in Sync mode is closed the MSA sequence order is restored to the Original.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    QStringList originalSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "msa_editor_tree_test_0003_2");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList byTreeSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(originalSequenceNames1 != byTreeSequenceNames1, "MSA must be re-ordered by tree");

    GTUtilsMsaEditor::closeActiveTreeTab();

    QStringList originalSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(originalSequenceNames2 == originalSequenceNames2, "Original sequence order must be restored");
}

}  // namespace GUITest_common_scenarios_msa_editor_tree
}  // namespace U2
