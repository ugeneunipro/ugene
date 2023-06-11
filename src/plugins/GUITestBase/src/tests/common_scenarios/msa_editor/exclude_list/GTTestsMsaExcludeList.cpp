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

#include <api/GTUtils.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMenu.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>

#include "GTTestsMsaExcludeList.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_exclude_list {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Check initial state of MSA editor & Exclude list with no selection.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    auto toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open Exclude List.
    GTWidget::click(toggleExcludeListButton);

    // Check Exclude List state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText("Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton("exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "ty3.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton("exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton("exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveFromMsaButton = GTWidget::findToolButton("exclude_list_move_from_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget("exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit("exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Checks that rows can be moved between MSA & Exclude List.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0002");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::checkExcludeList({});
    GTUtilsMsaEditor::moveRowsToExcludeList({"a", "c"});

    // Check that a sibling sequence to the original selection is selected.
    GTUtilsMsaEditor::checkSelectionByNames({"b"});
    GTUtilsMsaEditor::checkNameList({"b", "d", "e", "f", "g", "h"});
    GTUtilsMsaEditor::checkExcludeList({"a", "c"});

    GTUtilsMsaEditor::moveRowFromExcludeList("c");
    GTUtilsMsaEditor::checkNameList({"b", "c", "d", "e", "f", "g", "h"});
    GTUtilsMsaEditor::checkExcludeList({"a"});
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Checks that UNDO/REDO in MSA Editor affects Exclude List.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0003");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::checkExcludeList({});

    GTUtilsMsaEditor::moveRowsToExcludeList({"a"});
    GTUtilsMsaEditor::checkExcludeList({"a"});
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({});

    GTUtilsMsaEditor::moveRowsToExcludeList({"a"});
    GTUtilsMsaEditor::moveRowsToExcludeList({"b"});
    GTUtilsMsaEditor::checkExcludeList({"a", "b"});
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({"a"});
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({});

    GTUtilsMsaEditor::redo();
    GTUtilsMsaEditor::checkExcludeList({"a"});
    GTUtilsMsaEditor::redo();
    GTUtilsMsaEditor::checkExcludeList({"a", "b"});

    GTUtilsMsaEditor::removeRows(1, 1);
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({"a", "b"});
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({"a"});
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Checks that Exclude List is saved correctly.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0004");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Check that Exclude List is auto-saved on close.
    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::moveRowsToExcludeList({"a", "h"});
    GTUtilsMsaEditor::closeExcludeList();
    GTFile::check(sandBoxDir + baseFileName + ".exclude-list.fasta");

    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::checkExcludeList({"a", "h"});

    // Check that Exclude List is auto-saved on MSA save.
    GTUtilsMsaEditor::moveRowsToExcludeList({"c"});
    GTUtilsMsaEditor::checkExcludeList({"a", "h", "c"});
    GTUtilsDocument::saveDocument(baseFileName + ".aln");
    GTUtilsMdi::closeAllWindows();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::doubleClickItem(baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::checkExcludeList({"a", "h", "c"});

    // Check that Exclude List is auto-saved on project close.
    GTUtilsMsaEditor::moveRowsToExcludeList({"d"});
    GTUtilsMsaEditor::checkExcludeList({"a", "h", "c", "d"});
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));  // Save the document.
    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));  // Do not save the project.
    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::checkExcludeList({"a", "h", "c", "d"});
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Check that Exclude List shows sequence text.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0005");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::moveRowsToExcludeList({"e", "f"});

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
    auto nameListArea = GTWidget::findListWidget("exclude_list_name_list_widget", excludeListWidget);
    auto sequenceViewArea = GTWidget::findPlainTextEdit("exclude_list_sequence_view", excludeListWidget);

    CHECK_SET_ERR(nameListArea->currentItem() == nullptr, "Name list must have no selection");
    CHECK_SET_ERR(sequenceViewArea->toPlainText().isEmpty(), "Sequence view must be empty");

    GTListWidget::click(nameListArea, "e");
    CHECK_SET_ERR(sequenceViewArea->toPlainText() == "TTAGTTTATTAATT", "Sequence e does not match");

    GTListWidget::click(nameListArea, "f");
    CHECK_SET_ERR(sequenceViewArea->toPlainText() == "TTAGTCTACTAATT", "Sequence f does not match");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Check that Exclude List correctly handles read-only o object state.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0006");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::moveRowsToExcludeList({"g"});

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto nameListArea = GTWidget::findListWidget("exclude_list_name_list_widget", msaEditorWindow);
    auto moveToMsaButton = GTWidget::findToolButton("exclude_list_move_to_msa_button", msaEditorWindow);
    auto moveFromMsaButton = GTWidget::findToolButton("exclude_list_move_from_msa_button", msaEditorWindow);

    GTListWidget::click(nameListArea, "g");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton is not enabled/1");
    CHECK_SET_ERR(moveFromMsaButton->isEnabled(), "moveFromMsaButton is not enabled/1");

    GTUtilsDocument::lockDocument(baseFileName + ".aln");
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsaButton is enabled");
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveFromMsaButton is enabled");

    GTUtilsDocument::unlockDocument(baseFileName + ".aln");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton is not enabled/2");
    CHECK_SET_ERR(moveFromMsaButton->isEnabled(), "moveFromMsaButton is not enabled/2");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Check that moving rows to a non-loaded Exclude List works correctly.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0007");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::selectRowsByName({"b"});
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::checkExcludeList({"b"});
    GTUtilsMsaEditor::closeExcludeList();

    // Exclude list is unloaded now. Move new columns and check that the columns are added after the list is loaded.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::checkExcludeList({"b", "c"});
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Multi-selection in MSA Exclude List.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0008");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::openExcludeList();
    GTUtilsMsaEditor::moveRowsToExcludeList({"a", "b", "c"});

    GTUtilsMsaEditor::checkExcludeList({"a", "b", "c"});

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
    auto sequenceViewArea = GTWidget::findPlainTextEdit("exclude_list_sequence_view", excludeListWidget);
    auto moveToMsaButton = GTWidget::findToolButton("exclude_list_move_to_msa_button", msaEditorWindow);

    GTUtilsMsaEditor::selectRowsByNameInExcludeList({"a"});
    CHECK_SET_ERR(sequenceViewArea->isEnabled(), "sequenceViewArea must be enabled/1");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/1");

    GTUtilsMsaEditor::selectRowsByNameInExcludeList({"a", "c"});
    CHECK_SET_ERR(!sequenceViewArea->isEnabled(), "sequenceViewArea must be disabled");
    CHECK_SET_ERR(sequenceViewArea->toPlainText() == "2 sequences selected", "Unexpected sequence view area text");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/2");

    GTWidget::click(moveToMsaButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::checkExcludeList({"b"});
    GTUtilsMsaEditor::checkExcludeListSelection({"b"});
    CHECK_SET_ERR(sequenceViewArea->isEnabled(), "sequenceViewArea must be enabled/3");
    CHECK_SET_ERR(sequenceViewArea->toPlainText().startsWith("TAAGCTTACTAATC"), "Invalid sequence in sequence view area/1");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/3");

    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({"b", "a", "c"});
    GTUtilsMsaEditor::checkExcludeListSelection({"b"});
    CHECK_SET_ERR(sequenceViewArea->isEnabled(), "sequenceViewArea must be enabled/4");
    CHECK_SET_ERR(sequenceViewArea->toPlainText().startsWith("TAAGCTTACTAATC"), "Invalid sequence in sequence view area/2");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/4");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Check that collapsed groups are moved with the nested content.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0009");
    GTFile::copy(testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::toggleCollapsingMode();
    GTUtilsMsaEditor::clickSequenceName("c");

    GTUtilsMsaEditor::openExcludeList();
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto moveFromMsaButton = GTWidget::findToolButton("exclude_list_move_from_msa_button", msaEditorWindow);

    GTWidget::click(moveFromMsaButton);
    GTUtilsMsaEditor::checkExcludeList({"c", "e", "g"});

    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkExcludeList({});
}

}  // namespace GUITest_common_scenarios_msa_exclude_list
}  // namespace U2.
