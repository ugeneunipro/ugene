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
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));

    auto toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(os, toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open Exclude List.
    GTWidget::click(os, toggleExcludeListButton);

    // Check Exclude List state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText(os, "Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton(os, "exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "ty3.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton(os, "exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveFromMsaButton = GTWidget::findToolButton(os, "exclude_list_move_from_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget(os, "exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit(os, "exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(os, toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Checks that rows can be moved between MSA & Exclude List.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0002");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::checkExcludeList(os, {});
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"a", "c"});

    // Check that a sibling sequence to the original selection is selected.
    GTUtilsMsaEditor::checkSelectionByNames(os, {"b"});
    GTUtilsMsaEditor::checkNameList(os, {"b", "d", "e", "f", "g", "h"});
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "c"});

    GTUtilsMsaEditor::moveRowFromExcludeList(os, "c");
    GTUtilsMsaEditor::checkNameList(os, {"b", "c", "d", "e", "f", "g", "h"});
    GTUtilsMsaEditor::checkExcludeList(os, {"a"});
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Checks that UNDO/REDO in MSA Editor affects Exclude List.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0003");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::checkExcludeList(os, {});

    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"a"});
    GTUtilsMsaEditor::checkExcludeList(os, {"a"});
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {});

    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"a"});
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"b"});
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "b"});
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a"});
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {});

    GTUtilsMsaEditor::redo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a"});
    GTUtilsMsaEditor::redo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "b"});

    GTUtilsMsaEditor::removeRows(os, 1, 1);
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "b"});
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a"});
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Checks that Exclude List is saved correctly.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0004");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Check that Exclude List is auto-saved on close.
    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"a", "h"});
    GTUtilsMsaEditor::closeExcludeList(os);
    GTFile::check(os, sandBoxDir + baseFileName + ".exclude-list.fasta");

    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "h"});

    // Check that Exclude List is auto-saved on MSA save.
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"c"});
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "h", "c"});
    GTUtilsDocument::saveDocument(os, baseFileName + ".aln");
    GTUtilsMdi::closeAllWindows(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::doubleClickItem(os, baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "h", "c"});

    // Check that Exclude List is auto-saved on project close.
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"d"});
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "h", "c", "d"});
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));  // Save the document.
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));  // Do not save the project.
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"a", "h", "c", "d"});
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Check that Exclude List shows sequence text.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0005");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"e", "f"});

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
    auto nameListArea = GTWidget::findListWidget(os, "exclude_list_name_list_widget", excludeListWidget);
    auto sequenceViewArea = GTWidget::findPlainTextEdit(os, "exclude_list_sequence_view", excludeListWidget);

    CHECK_SET_ERR(nameListArea->currentItem() == nullptr, "Name list must have no selection");
    CHECK_SET_ERR(sequenceViewArea->toPlainText().isEmpty(), "Sequence view must be empty");

    GTListWidget::click(os, nameListArea, "e");
    CHECK_SET_ERR(sequenceViewArea->toPlainText() == "TTAGTTTATTAATT", "Sequence e does not match");

    GTListWidget::click(os, nameListArea, "f");
    CHECK_SET_ERR(sequenceViewArea->toPlainText() == "TTAGTCTACTAATT", "Sequence f does not match");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Check that Exclude List correctly handles read-only o object state.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0006");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"g"});

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto nameListArea = GTWidget::findListWidget(os, "exclude_list_name_list_widget", msaEditorWindow);
    auto moveToMsaButton = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", msaEditorWindow);
    auto moveFromMsaButton = GTWidget::findToolButton(os, "exclude_list_move_from_msa_button", msaEditorWindow);

    GTListWidget::click(os, nameListArea, "g");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton is not enabled/1");
    CHECK_SET_ERR(moveFromMsaButton->isEnabled(), "moveFromMsaButton is not enabled/1");

    GTUtilsDocument::lockDocument(os, baseFileName + ".aln");
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsaButton is enabled");
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveFromMsaButton is enabled");

    GTUtilsDocument::unlockDocument(os, baseFileName + ".aln");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton is not enabled/2");
    CHECK_SET_ERR(moveFromMsaButton->isEnabled(), "moveFromMsaButton is not enabled/2");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Check that moving rows to a non-loaded Exclude List works correctly.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0007");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::selectRowsByName(os, {"b"});
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkExcludeList(os, {"b"});
    GTUtilsMsaEditor::closeExcludeList(os);

    // Exclude list is unloaded now. Move new columns and check that the columns are added after the list is loaded.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkExcludeList(os, {"b", "c"});
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Multi-selection in MSA Exclude List.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0008");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::openExcludeList(os);
    GTUtilsMsaEditor::moveRowsToExcludeList(os, {"a", "b", "c"});

    GTUtilsMsaEditor::checkExcludeList(os, {"a", "b", "c"});

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
    auto sequenceViewArea = GTWidget::findPlainTextEdit(os, "exclude_list_sequence_view", excludeListWidget);
    auto moveToMsaButton = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", msaEditorWindow);

    GTUtilsMsaEditor::selectRowsByNameInExcludeList(os, {"a"});
    CHECK_SET_ERR(sequenceViewArea->isEnabled(), "sequenceViewArea must be enabled/1");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/1");

    GTUtilsMsaEditor::selectRowsByNameInExcludeList(os, {"a", "c"});
    CHECK_SET_ERR(!sequenceViewArea->isEnabled(), "sequenceViewArea must be disabled");
    CHECK_SET_ERR(sequenceViewArea->toPlainText() == "2 sequences selected", "Unexpected sequence view area text");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/2");

    GTWidget::click(os, moveToMsaButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkExcludeList(os, {"b"});
    GTUtilsMsaEditor::checkExcludeListSelection(os, {"b"});
    CHECK_SET_ERR(sequenceViewArea->isEnabled(), "sequenceViewArea must be enabled/3");
    CHECK_SET_ERR(sequenceViewArea->toPlainText().startsWith("TAAGCTTACTAATC"), "Invalid sequence in sequence view area/1");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/3");

    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {"b", "a", "c"});
    GTUtilsMsaEditor::checkExcludeListSelection(os, {"b"});
    CHECK_SET_ERR(sequenceViewArea->isEnabled(), "sequenceViewArea must be enabled/4");
    CHECK_SET_ERR(sequenceViewArea->toPlainText().startsWith("TAAGCTTACTAATC"), "Invalid sequence in sequence view area/2");
    CHECK_SET_ERR(moveToMsaButton->isEnabled(), "moveToMsaButton must be enabled/4");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Check that collapsed groups are moved with the nested content.
    QString baseFileName = GTUtils::genUniqueString("exclude-list-test-0009");
    GTFile::copy(os, testDir + "_common_data/clustal/collapse_mode_1.aln", sandBoxDir + baseFileName + ".aln");
    GTFileDialog::openFile(os, sandBoxDir + baseFileName + ".aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::toggleCollapsingMode(os);
    GTUtilsMsaEditor::clickSequenceName(os, "c");

    GTUtilsMsaEditor::openExcludeList(os);
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto moveFromMsaButton = GTWidget::findToolButton(os, "exclude_list_move_from_msa_button", msaEditorWindow);

    GTWidget::click(os, moveFromMsaButton);
    GTUtilsMsaEditor::checkExcludeList(os, {"c", "e", "g"});

    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkExcludeList(os, {});
}

}  // namespace GUITest_common_scenarios_msa_exclude_list
}  // namespace U2.
