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

#include <GTGlobals.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsDialog.h>

#include <U2View/MSAEditor.h>

#include "GTTestsUndoRedo.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/util/RenameSequenceFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/clustalw/ClustalWDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_undo_redo {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {  // DIFFERENCE: lock document is checked
    // Check Undo/Redo functional
    // 1. Open document COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Insert several spaces.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    for (int i = 0; i < 6; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }

    QAbstractButton* undo = GTAction::button("msa_action_undo");
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    // 3. Undo this
    for (int i = 0; i < 3; i++) {
        GTWidget::click(undo);
    }
    // 4. lock document
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_DOCUMENT__LOCK}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: Undo and redo buttons are disabled
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button is enebled after locking document");
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is enebled after locking document");

    // 5. Unlock document
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_DOCUMENT__UNLOCK}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: undo and redo buttons are enebled and work properly
    CHECK_SET_ERR(undo->isEnabled(), "Undo button is disabled after unlocking document");
    CHECK_SET_ERR(redo->isEnabled(), "Redo button is disabled after unlocking document");

    // check undo
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTWidget::click(undo);
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(9, 0));
    GTKeyboardUtils::copy();
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "--TAAGACTT", "Undo works wrong. Found text is: " + clipboardText);

    // check redo
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTWidget::click(redo);
    GTWidget::click(redo);
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(9, 0));
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "----TAAGAC", "Redo works wrong. Found text is: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {  // DIFFERENCE: delete sequence is checked
    // The test checks Undo/Redo functional.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Delete 4-th sequence.
    GTUtilsMsaEditor::selectRows(3, 3);
    GTUtilsMsaEditor::checkSelectionByNames({"Roeseliana_roeseli"});
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: sequence deleted.
    GTUtilsMsaEditor::selectRows(3, 3);
    GTUtilsMsaEditor::checkSelectionByNames({"Montana_montana"});

    GTWidget::click(GTAction::button("msa_action_undo"));

    // Expected state: deletion undone.
    GTUtilsMsaEditor::selectRows(3, 3);
    GTUtilsMsaEditor::checkSelectionByNames({"Roeseliana_roeseli"});

    // Redo delition
    GTWidget::click(GTAction::button("msa_action_redo"));

    // Expected state: delition is redone.
    GTUtilsMsaEditor::selectRows(3, 3);
    GTUtilsMsaEditor::checkSelectionByNames({"Montana_montana"});
}

GUI_TEST_CLASS_DEFINITION(test_0003) {  // DIFFERENCE: add sequence is checked
    // Check Undo/Redo functional.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTUtilsDialog::add(new GTFileDialogUtils(dataDir + "/samples/Raw/", "raw.seq"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Expected state: raw sequence appeared in alignment.
    GTUtilsMsaEditor::selectRowsByName({"raw"});

    // Undo the deletion.
    GTWidget::click(GTAction::button("msa_action_undo"));

    // Expected state: raw doesn't present in namelist.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(!nameList.contains("raw"), "adding raw is not undone");

    // Redo the delition.
    GTWidget::click(GTAction::button("msa_action_redo"));

    // Expected state: delition is redone.
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList().contains("raw"), "Adding raw is not redone");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {  // DIFFERENCE: add sequence is checked
    // Check Undo/Redo functional
    // 1. Open document COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. insert gap->undo->insert gap->undo->redo
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    QAbstractButton* redo = GTAction::button("msa_action_redo");

    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTWidget::click(undo);

    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTWidget::click(undo);

    GTWidget::click(redo);

    // Expected state: redo button is disabled
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is enebled");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {  // undo remove selection
    // open file
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // remove selection
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: selection removed
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 1));
    GTKeyboardUtils::copy();
    QString clipdoardText = GTClipboard::text();
    CHECK_SET_ERR(clipdoardText == "ACTT\nCTTA", QString("Expected ACTT\nCTTA, found: %1").arg(clipdoardText));

    // undo
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    GTWidget::click(undo);

    // Expected state: delition undone
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 1));
    GTKeyboardUtils::copy();
    clipdoardText = GTClipboard::text();
    CHECK_SET_ERR(clipdoardText == "TAAG\nTAAG", QString("Expected TAAG\nTAAG, found: %1").arg(clipdoardText));

    // redo
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    GTWidget::click(redo);

    // Expected state: delition redone
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 1));
    GTKeyboardUtils::copy();
    clipdoardText = GTClipboard::text();
    CHECK_SET_ERR(clipdoardText == "ACTT\nCTTA", QString("Expected ACTT\nCTTA, found: %1").arg(clipdoardText));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {  // undo replace_selected_rows_with_reverse-complement
    // In-place reverse complement replace in MSA Editor (0002425)

    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 2));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence changed from TTG -> CAA
    GTKeyboardUtils::copy();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CAA\nTGA\nATC",
                  "Clipboard string and expected MSA string differs");

    //  sequence name changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 6, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L|revcompl") && (nameList[1] == "S|revcompl") && (nameList[2] == "D|revcompl"), "Unexpected sequence names");

    // 3. Undo
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    GTWidget::click(undo);

    // Expected state: sequence changed from CAA -> TTG
    GTKeyboardUtils::copy();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTG\nTCA\nGAT",
                  "Clipboard string and expected MSA string differs");

    //  sequence name changed from L|revcompl ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 3, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L") && (nameList[1] == "S") && (nameList[2] == "D"),
                  "There are unexpected names in nameList");

    // 4. Redo
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    GTWidget::click(redo);

    // Expected state: sequence changed from TTG -> CAA
    GTKeyboardUtils::copy();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CAA\nTGA\nATC",
                  "Clipboard string and expected MSA string differs");

    //  sequence name changed from L -> L|revcompl
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 6, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L|revcompl") && (nameList[1] == "S|revcompl") && (nameList[2] == "D|revcompl"), "Unexpected sequence names");
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {  // undo replace_selected_rows_with_reverse
    // In-place reverse complement replace in MSA Editor (0002425)

    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse"}));
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 2));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence changed from TTG -> GTT
    GTKeyboardUtils::copy();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "GTT\nACT\nTAG",
                  "Clipboard string and expected MSA string differs");

    // sequence name  changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 6, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L|rev") && (nameList[1] == "S|rev") && (nameList[2] == "D|rev"), "Unexpected sequence names");

    // 3. Undo
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    GTWidget::click(undo);

    // Expected state: sequence changed from GTT -> TTG
    GTKeyboardUtils::copy();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTG\nTCA\nGAT",
                  "Clipboard string and expected MSA string differs");

    //  sequence name changed from L|rev ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 3, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L") && (nameList[1] == "S") && (nameList[2] == "D"),
                  "There are unexpected names in nameList");

    // 4. Redo
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    GTWidget::click(redo);

    // Expected state: sequence changed from TTG -> GTT
    GTKeyboardUtils::copy();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "GTT\nACT\nTAG",
                  "Clipboard string and expected MSA string differs");

    //  sequence name changed from L -> L|revcompl
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 6, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L|rev") && (nameList[1] == "S|rev") && (nameList[2] == "D|rev"), "Unexpected sequence names");
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    // undo replace_selected_rows_with_complement
    //  In-place reverse complement replace in MSA Editor (0002425)

    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_complement"}));
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 2));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence changed from TTG -> AAC
    GTKeyboardUtils::copy();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "AAC\nAGT\nCTA",
                  "Clipboard string and expected MSA string differs. Clipboard string is: " + clipboardText);

    //  sequence name  changed from L -> L|compl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 6, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L|compl") && (nameList[1] == "S|compl") && (nameList[2] == "D|compl"), "Unexpected sequence names");

    // 3. Undo
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    GTWidget::click(undo);

    // Expected state: sequence changed from AAC -> TTG
    GTKeyboardUtils::copy();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTG\nTCA\nGAT",
                  "Clipboard string and expected MSA string differs");

    //  sequence name changed from L|rev ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 3, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L") && (nameList[1] == "S") && (nameList[2] == "D"),
                  "There are unexpected names in nameList");

    // 4. Redo
    QAbstractButton* redo = GTAction::button("msa_action_redo");
    GTWidget::click(redo);

    // Expected state: sequence changed from TTG -> AAC
    GTKeyboardUtils::copy();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "AAC\nAGT\nCTA",
                  "Clipboard string and expected MSA string differs");

    //  sequence name  changed from L -> L|revcompl
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 6, "nameList doesn't contain enough strings");
    CHECK_SET_ERR((nameList[0] == "L|compl") && (nameList[1] == "S|compl") && (nameList[2] == "D|compl"),
                  "There are unexpected names in nameList");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // remove columns with 3 or more gaps
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedChangedMsa = {"AAGCTTCTTT", "AAGTTACTAA", "TAG---TTAT", "AAGC---TAT", "TAGTTATTAA", "TAGTTATTAA", "TAGTTATTAA", "AAGCTTT---", "A--AGAATAA", "AAGCTTTTAA"};

    // fill remove columns of gaps dialog
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EDIT, "remove_columns_of_gaps"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Number, 3));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong");

    // undo
    GTUtilsMsaEditor::undo();

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    // redo
    GTUtilsMsaEditor::redo();

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    // remove columns with 15 percents of gaps
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedChangedMsa = {"AAGCCTTT", "AAGTCTAA", "TAG-TTAT", "AAGC-TAT", "TAGTTTAA", "TAGTTTAA", "TAGTTTAA", "AAGCT---", "A--AATAA", "AAGCTTAA"};

    // fill remove columns of gaps dialog
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EDIT, "remove_columns_of_gaps"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Percent, 15));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong:\nChenged MSA:\n" + changedMsa.join("\n") + "\nOriginal MSA:\n" + expectedChangedMsa.join("\n"));

    // undo
    GTUtilsMsaEditor::undo();

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    // redo
    GTUtilsMsaEditor::redo();

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    // remove columns of gaps is tested
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedChangedMsa = {"AAGCTTCTTTTAA", "AAGTTACTAA---", "TAG---TTATTAA", "AAGC---TATTAA", "TAGTTATTAA---", "TAGTTATTAA---", "TAGTTATTAA---", "AAGCTTT---TAA", "A--AGAATAATTA", "AAGCTTTTAA---"};

    // fill remove columns of gaps dialog
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EDIT, "remove_columns_of_gaps"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Column));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong:\nChenged MSA:\n" + changedMsa.join("\n") + "\nOriginal MSA:\n" + expectedChangedMsa.join("\n"));

    // undo
    GTUtilsMsaEditor::undo();

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    // redo
    GTUtilsMsaEditor::redo();

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // remove all gaps is tested
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // save initial state
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedChangedMsa = {"AAGCTTCTTTTAA", "AAGTTACTAA---", "TAGTTATTAA---", "AAGCTATTAA---", "TAGTTATTAA---", "TAGTTATTAA---", "TAGTTATTAA---", "AAGCTTTTAA---", "AAGAATAATTA--", "AAGCTTTTAA---"};

    // fill remove columns of gaps dialog
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "Remove all gaps"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "remove gaps option works wrong:\nChenged MSA:\n" + changedMsa.join("\n") + "\nOriginal MSA:\n" + expectedChangedMsa.join("\n"));

    // undo
    GTUtilsMsaEditor::undo();

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "undo works wrong");

    // redo
    GTUtilsMsaEditor::redo();

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "redo works wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // rename msa is tested
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // rename msa
    GTUtilsDialog::waitForDialog(new PopupChooser({"Rename"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("ma2_gap_col"));
    GTMouseDriver::click(Qt::RightButton);
    GTKeyboardDriver::keySequence("some_name");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected state: msa renamed
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("some_name"));

    // undo
    GTUtilsMsaEditor::undo();

    // Expected state: rename undone
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("ma2_gap_col"));

    // redo
    GTUtilsMsaEditor::redo();

    // Expected state: rename redone
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("some_name"));
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // MUSCLE aligner undo test
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedChangedMsa = {"AAG---AATAATTA", "AAG---TCTATTAA", "AAGACTTCTTTTAA", "AAG---TCTTTTAA", "AAG---CCTTTTAA", "AAG---CTTACTAA", "TAG---TTTATTAA", "TAG---CTTATTAA", "TAG---CTTATTAA", "TAG---CTTATTAA"};

    // Use context {Edit->Align with MUSCLE}
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new MuscleDialogFiller(MuscleDialogFiller::Default, false));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    // undo
    GTUtilsMsaEditor::undo();

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    // redo
    GTUtilsMsaEditor::redo();

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Kalign undo test
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    QStringList expectedChangedMsa = {
        "AAGACTTCTTTTAA",
        "---AAGCTTACTAA",
        "---TAGTTTATTAA",
        "---AAGTCTATTAA",
        "---TAGCTTATTAA",
        "---TAGCTTATTAA",
        "---TAGCTTATTAA",
        "---AAGTCTTTTAA",
        "---AAGAATAATTA",
        "---AAGCCTTTTAA"};

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "alignWithKalignAction"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new KalignDialogFiller());
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    GTUtilsMsaEditor::undo();

    QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    GTUtilsMsaEditor::redo();

    QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // Kalign undo test
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    QStringList expectedChangedMsa = {
        "AAGACTTCTTTTAA",
        "---AAGCTTACTAA",
        "---TAGTTTATTAA",
        "---AAGTCTATTAA",
        "---TAGCTTATTAA",
        "---TAGCTTATTAA",
        "---TAGCTTATTAA",
        "---AAGTCTTTTAA",
        "---AAGAATAATTA",
        "---AAGCCTTTTAA"};

    // Use context {Edit->Align with Kalign}
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "alignWithKalignAction"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new KalignDialogFiller(100));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    GTUtilsMsaEditor::undo();

    QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    GTUtilsMsaEditor::redo();

    QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // ClustalW aligner undo test
    // Open file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();
    const QStringList expectedChangedMsa = {"---AAGACTTCTTTTAA", "---AAGCTT---ACTAA", "---TAGT---TTATTAA", "---AAGTC---TATTAA", "---TAGCTT---ATTAA", "---TAGCTT---ATTAA", "---TAGCTT---ATTAA", "---AAGTCTTT---TAA", "A---AGAAT--AATTA-", "---AAGCCT---TTTAA"};

    // Use context {Edit->Align with Kalign}
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align with ClustalW"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new ClustalWDialogFiller());
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    const QStringList changedMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(changedMsa == expectedChangedMsa, "Unexpected alignment:\n" + changedMsa.join("\n"));

    // undo
    GTUtilsMsaEditor::undo();

    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa, "Undo works wrong:\n" + undoneMsa.join("\n"));

    // redo
    GTUtilsMsaEditor::redo();

    const QStringList redoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(redoneMsa == expectedChangedMsa, "Redo works wrong:\n" + redoneMsa.join("\n"));
}

}  // namespace GUITest_common_scenarios_undo_redo
}  // namespace U2
