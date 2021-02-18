/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include <primitives/GTComboBox.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QGraphicsView>

#include <U2View/MSAEditor.h>

#include "GTGlobals.h"
#include "GTTestsMSAEditorTree.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_editor_tree {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Check that original name list is correct.
    MSAEditor *msaEditor = GTUtilsMsaEditor::getEditor(os);
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
    QStringList nameList = msaObject->getMultipleAlignment()->getRowNames();
    QStringList originalNameList = {"a", "b", "c", "d", "e", "f", "g", "h"};
    CHECK_SET_ERR(nameList == originalNameList, "1. Wrong original name list: " + nameList.join(","));

    // Build a tree.
    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "tree_test_0001.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that the tree is opened and MSA order was changed to match the tree order. The original MSA-object order must not change.
    nameList = msaObject->getMultipleAlignment()->getRowNames();
    CHECK_SET_ERR(nameList == originalNameList, "2. Wrong original name list: " + nameList.join(","));
    nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    QStringList expectedExpandedTreeNameList = {"h", "b", "f", "d", "c", "e", "g", "a"};
    CHECK_SET_ERR(nameList == expectedExpandedTreeNameList, "Initial full tree name list not matched: " + nameList.join(","));

    // Collapse subtree. Check that MSA name list has a collapsed group.
    QList<GraphicsButtonItem *> nodeList = GTUtilsPhyTree::getOrderedRectangularNodes(os);
    GraphicsButtonItem *parentOfSequenceC = nodeList[1];
    GTUtilsPhyTree::doubleClickNode(os, parentOfSequenceC);
    nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    expectedExpandedTreeNameList = QStringList({"h", "b", "f", "d", "g", "a"});
    CHECK_SET_ERR(nameList == expectedExpandedTreeNameList, "Collapsed tree name list not matched: " + nameList.join(","));

    GTUtilsPhyTree::doubleClickNode(os, parentOfSequenceC);
    nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    expectedExpandedTreeNameList = QStringList({"h", "b", "f", "d", "c", "e", "g", "a"});
    CHECK_SET_ERR(nameList == expectedExpandedTreeNameList, "Restored full tree name list not matched: " + nameList.join(","));
}

}    // namespace GUITest_common_scenarios_msa_editor_tree
}    // namespace U2
