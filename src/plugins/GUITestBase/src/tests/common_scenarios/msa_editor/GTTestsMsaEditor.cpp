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
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/FontDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <harness/UGUITestBase.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <QApplication>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/TextUtils.h>

#include <U2View/ADVConstants.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>

#include "GTTestsMsaEditor.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/util/RenameSequenceFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExportHighlightedDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/GenerateAlignmentProfileDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/weight_matrix/PwmBuildDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_editor {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    int length = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(length == 14, "Wrong length");

    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBaseIdx == 0, "Wrong first base idx");

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastBaseIdx == 13, "Wrong last base idx");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    int length = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(length == 12, "Wrong length");

    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBaseIdx == 0, "Wrong first base idx");

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastBaseIdx == 11, "Wrong last base idx");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    int length = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(length == 14, "Wrong length");

    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBaseIdx == 0, "Wrong first base idx");

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastBaseIdx == 13, "Wrong last base idx");
}

GUI_TEST_CLASS_DEFINITION(test_0001_3) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "revcompl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    int length = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(length == 6, "Wrong length");

    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBaseIdx == 0, "Wrong first base idx");

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastBaseIdx == 5, "Wrong last base idx");
}

GUI_TEST_CLASS_DEFINITION(test_0001_4) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln", GTFileDialog::Cancel);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln", GTFileDialog::Open);
    GTUtilsTaskTreeView::waitTaskFinished();

    int length = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(length == 3, "Wrong length");

    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBaseIdx == 0, "Wrong first base idx");

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastBaseIdx == 2, "Wrong last base idx");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    bool offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(!offsetsVisible, "Offsets are visible");

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(offsetsVisible, "Offsets are not visible");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    bool offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(!offsetsVisible, "Offsets are visible");

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(offsetsVisible, "Offsets are not visible");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMenu::clickMainMenuItem({"Actions", "Appearance", "Show offsets"});
    GTUtilsTaskTreeView::waitTaskFinished();

    bool offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(!offsetsVisible, "Offsets are visible");

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(offsetsVisible, "Offsets are not visible");
}

GUI_TEST_CLASS_DEFINITION(test_0002_3) {
    GTUtilsMdi::click(GTGlobals::Close);
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "revcompl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* mdiWindow = GTUtilsMdi::activeWindow();
    GTMenu::clickMainMenuItem({"Actions", "Appearance", "Show offsets"});

    bool offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(!offsetsVisible, "Offsets are visible");
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished();

    mdiWindow = GTUtilsMdi::activeWindow(false);
    CHECK_SET_ERR(mdiWindow == nullptr, "There is an MDI window");

    QPoint p = GTUtilsProjectTreeView::getItemCenter("revcompl");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::doubleClick();

    GTMenu::clickMainMenuItem({"Actions", "Appearance", "Show offsets"});
    GTUtilsTaskTreeView::waitTaskFinished();

    offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(offsetsVisible, "Offsets are not visible");
}

GUI_TEST_CLASS_DEFINITION(test_0002_4) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "revcompl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    bool offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(!offsetsVisible, "Offsets are visible");

    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished();

    QPoint p = GTUtilsProjectTreeView::getItemCenter("revcompl");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "show_offsets"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    offsetsVisible = GTUtilsMSAEditorSequenceArea::offsetsVisible();
    CHECK_SET_ERR(offsetsVisible, "Offsets are not visible");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma_unsorted.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_name"}));
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList() == QStringList() << "a"
                                                                               << "C"
                                                                               << "d"
                                                                               << "D",
                  "Sort by name failed (ascending)");

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_name_descending"}));
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList() == QStringList() << "d"
                                                                               << "D"
                                                                               << "C"
                                                                               << "a",
                  "Sort by name failed (descending)");

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_length"}));
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList() == QStringList() << "D"
                                                                               << "d"
                                                                               << "a"
                                                                               << "C",
                  "Sort by length failed (ascending)");

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_length_descending"}));
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList() == QStringList() << "C"
                                                                               << "d"
                                                                               << "a"
                                                                               << "D",
                  "Sort by length failed (descending)");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    QWidget* msaWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(6));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTMenu::showContextMenu(msaWindow);
    GTUtilsDialog::checkNoActiveWaiters();

    QRect expectedRect(5, 0, 1, 1);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);

    GTUtilsMsaEditor::gotoWithKeyboardShortcut(6);

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* mdiWindow = GTUtilsMdi::activeWindow();

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(6));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTMenu::showContextMenu(mdiWindow);
    GTUtilsTaskTreeView::waitTaskFinished();

    QRect expectedRect(5, 0, 1, 1);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);

    GTUtilsMsaEditor::gotoWithKeyboardShortcut(6);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* mdiWindow = GTUtilsMdi::activeWindow();

    GTUtilsDialog::waitForDialog(new GoToDialogFiller(6));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTMenu::showContextMenu(mdiWindow);
    GTUtilsTaskTreeView::waitTaskFinished();

    QRect expectedRect(5, 0, 1, 1);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);

    GTUtilsMsaEditor::gotoWithKeyboardShortcut(6);

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Check alignment view status bar coordinates

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Alignment length 14, left offset 1, right offset 14
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength() == 14, "Wrong length");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex() == 0, "Wrong first base idx");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex() == 13, "Wrong last base idx");

    auto msaEditorStatusBar = GTWidget::findWidget("msa_editor_status_bar");
    auto line = GTWidget::findLabel("Line", msaEditorStatusBar);
    auto column = GTWidget::findLabel("Column", msaEditorStatusBar);
    auto selection = GTWidget::findLabel("Selection", msaEditorStatusBar);

    //  Select 1 base.
    GTUtilsMSAEditorSequenceArea::click(QPoint(4, 3));
    CHECK_SET_ERR(column->text() == "Col 5 / 14", "1. Column is " + column->text());
    CHECK_SET_ERR(line->text() == "Seq 4 / 10", "1. Sequence is " + line->text());

    //  Select another base.
    GTUtilsMSAEditorSequenceArea::click(QPoint(1, 8));
    CHECK_SET_ERR(column->text() == "Col 2 / 14", "2. Column is " + column->text());
    CHECK_SET_ERR(line->text() == "Seq 9 / 10", "2. Sequence is " + line->text());

    // Select a vertical 1D range.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(7, 7));
    CHECK_SET_ERR(column->text() == "Col 8 / 14", "3. Column is " + column->text());
    CHECK_SET_ERR(line->text() == "Seq - / 10", "3. Sequence is " + line->text());

    // Select a horizontal 1D range.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 3));
    CHECK_SET_ERR(column->text() == "Col - / 14", "4. Column is " + column->text());
    CHECK_SET_ERR(line->text() == "Seq 4 / 10", "4. Sequence is " + line->text());

    // Select a 2D range.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));
    CHECK_SET_ERR(column->text() == "Col - / 14", "5. Column is " + column->text());
    CHECK_SET_ERR(line->text() == "Seq - / 10", "5. Sequence is " + line->text());

    // Select multi-selection.
    GTUtilsMsaEditor::clearSelection();
    GTUtilsMsaEditor::selectRowsByName({"Phaneroptera_falcata", "Tettigonia_viridissima"});
    CHECK_SET_ERR(column->text() == "Col - / 14", "6. Column is " + column->text());
    CHECK_SET_ERR(line->text() == "Seq - / 10", "6. Sequence is " + line->text());
    CHECK_SET_ERR(selection->text() == "Sel 14 x 2 regions", "6. Selection is " + selection->text());
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength() == 14, "Wrong length");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex() == 0, "Wrong first base idx");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex() == 13, "Wrong last base idx");

    auto msaEditorStatusBar = GTWidget::findWidget("msa_editor_status_bar");
    auto line = GTWidget::findLabel("Line", msaEditorStatusBar);
    auto column = GTWidget::findLabel("Column", msaEditorStatusBar);

    GTUtilsMSAEditorSequenceArea::click(QPoint(4, 3));
    CHECK_SET_ERR(line->text() == "Seq 4 / 10", "1. Sequence is " + line->text());
    CHECK_SET_ERR(column->text() == "Col 5 / 14", "1. Column is " + column->text());

    // CHANGES: close and open MDI window, hide projectTreeView
    GTUtilsMdi::click(GTGlobals::Close);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("ma2_gapped"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::toggleView();
    GTUtilsTaskTreeView::waitTaskFinished();

    msaEditorStatusBar = GTWidget::findWidget("msa_editor_status_bar");

    line = GTWidget::findLabel("Line", msaEditorStatusBar);
    column = GTWidget::findLabel("Column", msaEditorStatusBar);

    GTUtilsMSAEditorSequenceArea::click(QPoint(1, 8));
    CHECK_SET_ERR(line->text() == "Seq 9 / 10", "2. Sequence is " + line->text());
    CHECK_SET_ERR(column->text() == "Col 2 / 14", "2. Column is " + column->text());
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Check rename & undo functionality in MSA Editor.

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::clickSequenceName("Tettigonia_viridissima");

    // Rename Tettigonia_viridissima -> Sequence_a.
    GTUtilsDialog::waitForDialog(new RenameSequenceFiller("Sequence_a", "Tettigonia_viridissima"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Rename Sequence_a -> Sequence_a: must be a no-op.
    GTUtilsDialog::waitForDialog(new RenameSequenceFiller("Sequence_a", "Sequence_a"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Undo.
    GTWidget::click(GTAction::button("msa_action_undo"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Tettigonia_viridissima is renamed back.
    GTUtilsMsaEditor::getSequenceNameRect("Tettigonia_viridissima");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Create bookmark: "start bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "start bookmark");

    int b0 = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    QWidget* mdiWindow = GTUtilsMdi::activeWindow();

    // Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(300));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTMenu::showContextMenu(mdiWindow);

    // Create bookmark: "middle bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "middle bookmark");

    int b300 = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Scroll msa to the end.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(550));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTMenu::showContextMenu(mdiWindow);

    // Create bookmark: "end bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "end bookmark");

    int b550 = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Expected state: clicking on each bookmark will recall corresponding MSA position
    GTUtilsBookmarksTreeView::doubleClickBookmark("start bookmark");
    GTUtilsTaskTreeView::waitTaskFinished();

    int offset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(offset == b0, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark("middle bookmark");
    GTUtilsTaskTreeView::waitTaskFinished();

    offset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(offset == b300, QString("middle bookmark offsets aren't equal to the expected: b300=%1 offset=%2").arg(b300).arg(offset));
    GTUtilsBookmarksTreeView::doubleClickBookmark("end bookmark");
    GTUtilsTaskTreeView::waitTaskFinished();

    offset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(offset == b550, QString("end bookmark offsets aren't equal to the expected: b550=%3 offset=%4").arg(b550).arg(offset));
    //     7. Delete Start bookmark
    GTUtilsBookmarksTreeView::deleteBookmark("start bookmark");

    //     Expected state: start bookmark isn't present
    QTreeWidgetItem* startBookmark = GTUtilsBookmarksTreeView::findItem("start bookmark", {false});
    CHECK_SET_ERR(startBookmark == nullptr, "Start bookmark is not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_0008_1) {
    // CHANGES: mid and end coordinates changed.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Create a bookmark. Rename "New bookmark" to "start bookmark".
    GTUtilsBookmarksTreeView::addBookmark("HIV-1 [HIV-1.aln]", "start bookmark");
    int b0 = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    //  Scroll msa to the middle.
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(600);

    // Create a bookmark. Rename "New bookmark" to "middle bookmark"
    GTUtilsBookmarksTreeView::addBookmark("HIV-1 [HIV-1.aln]", "middle bookmark");
    int b600 = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Scroll msa to the end.
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(1000);

    // Create bookmark. Rename "New bookmark" to "end bookmark".
    GTUtilsBookmarksTreeView::addBookmark("HIV-1 [HIV-1.aln]", "end bookmark");
    int b1000 = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Expected state: click on each bookmark sets corresponding MSA position.
    GTUtilsBookmarksTreeView::doubleClickBookmark("start bookmark");
    int offset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(offset == b0, QString("Start bookmark first base offset does not match: %1 vs %2").arg(offset).arg(b0));

    GTUtilsBookmarksTreeView::doubleClickBookmark("middle bookmark");
    offset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(offset == b600, QString("Middle bookmark first base offset does not match: %1 vs %2").arg(offset).arg(b600));

    GTUtilsBookmarksTreeView::doubleClickBookmark("end bookmark");
    offset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(offset == b1000, QString("End bookmark first base offset does not match: %1 vs %2").arg(offset).arg(b1000));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // 1. Open ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a trailing region length=3 (all gaps) for Isophia_altiacaEF540820
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(11, 1), QPoint(13, 1));

    // 3. Do context menu {Align-> Align with MUSCLE}  use "column range"
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseKey));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Column range = 12-14
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(11, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    QString clipboardText = GTClipboard::text();
    QString expectedMSA = "TAA\n---\nTAA\nTAA\n---\n---\n---\nTAA\nTTA\n---";

    CHECK_SET_ERR(clipboardText == expectedMSA, "Clipboard string and expected MSA string differs\n" + clipboardText);

    // 4. Press Align
    // Expected state: After aligning with 'stable' option the order must not change
}

GUI_TEST_CLASS_DEFINITION(test_0009_1) {
    // 1. Open ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a trailing region length=3 (all gaps) for Isophia_altiacaEF540820
    // CHANGES: selection from right to left
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(13, 1), QPoint(11, 1));

    // 3. Do context menu {Align-> Align with MUSCLE}  use "column range"
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseKey));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Column range = 12-14
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(11, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    QString sequencesInClipboard = GTClipboard::text();
    QString expectedMSA = "TAA\n---\nTAA\nTAA\n---\n---\n---\nTAA\nTTA\n---";

    CHECK_SET_ERR(sequencesInClipboard == expectedMSA, "Clipboard check failed! Expected: '" + expectedMSA + "'\n, got: '" + sequencesInClipboard + "'");

    // 4. Press Align
    // Expected state: After aligning with 'stable' option the order must not change
}

GUI_TEST_CLASS_DEFINITION(test_0009_2) {
    // 1. Open ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a trailing region length=3 (all gaps) for Isophia_altiacaEF540820
    // CHANGES: another region selected
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(11, 4), QPoint(13, 4));

    // 3. Do context menu {Align-> Align with MUSCLE}  use "column range"
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseKey));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Column range = 12-14
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(11, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    QString clipboardText = GTClipboard::text();
    QString expectedMSA = "TAA\n---\nTAA\nTAA\n---\n---\n---\nTAA\nTTA\n---";

    CHECK_SET_ERR(clipboardText == expectedMSA, "Clipboard string and expected MSA string differs");

    // 4. Press Align
    // Expected state: After aligning with 'stable' option the order must not change
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/translations_nucl.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate with default settings
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0010.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected state: every sequense name is the same as its amino translation
    const QString clipboardText = GTClipboard::text();
    const QString expectedMSA = "L\nS\nD\nS\nP\nK";
    CHECK_SET_ERR(clipboardText == expectedMSA, clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0010_1) {
    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/translations_nucl.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate with default settings
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0010_1.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected state: every sequense name the same as it amino translation
    const QString clipboardText = GTClipboard::text();
    const QString expectedMSA = "L\nS\nD\nS\nP\nK";
    CHECK_SET_ERR(clipboardText == expectedMSA, "Clipboard string and expected MSA string differs");
}

GUI_TEST_CLASS_DEFINITION(test_0010_2) {
    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/translations_nucl.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate to amino with default settings
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0010_2.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMsaEditor::getActiveMsaEditorWindow(), Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    // copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected state: every sequence name the same as it amino translation
    QString clipboardText = GTClipboard::text();
    QString expectedMSA = "L\nS\nD\nS\nP\nK";
    CHECK_SET_ERR(clipboardText == expectedMSA, "Clipboard string and expected MSA string are different. Clipboard text: " + clipboardText);

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    QStringList expectedNameList = {"L(translated)", "S(translated)", "D(translated)", "S(translated)", "P(translated)", "K(translated)"};
    CHECK_SET_ERR(nameList == expectedNameList, "Name lists are different. Expected: " + expectedNameList.join(",") + ", actual: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // In-place reverse complement replace in MSA Editor (0002425)

    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first sequence and do context menu {Edit->Replace selected rows with reverse complement}
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 0));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: sequence changed from TTG -> CAA
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    GTUtilsTaskTreeView::waitTaskFinished();
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CAA", "Clipboard string and expected MSA string differs");

    // sequence name  changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 2, "nameList doesn't contain enough strings");
    CHECK_SET_ERR(nameList[0] == "L|revcompl", "There are no 'L|revcompl' in nameList");

    // 3. Do step 2 again
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence changed from CAA -> TTG
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTG", "Clipboard string and expected MSA string differs");

    // sequence name changed from L|revcompl ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 2, "nameList doesn't contain enough strings");
    CHECK_SET_ERR(!nameList.contains("L|revcompl"), "There are 'L|revcompl' in nameList");
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // In-place reverse complement replace in MSA Editor (0002425)

    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 0));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence changed from TTG -> CAA
    // CHANGES: copy by context menu
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CAA", "Clipboard string and expected MSA string differs");

    //                 sequence name  changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 2, "nameList doesn't contain enough strings");
    CHECK_SET_ERR(nameList[0] == "L|revcompl", "There are no 'L|revcompl' in nameList");

    // 3. Do step 2 again
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence changed from CAA -> TTG
    // CHANGES: copy by context menu
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTG", "Clipboard string and expected MSA string differs");

    //                 sequence name changed from L|revcompl ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 2, "nameList doesn't contain enough strings");
    CHECK_SET_ERR(!nameList.contains("L|revcompl"), "There are 'L|revcompl' in nameList");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    // In-place reverse complement replace in MSA Editor (0002425)

    // 1. Open file _common_data\scenarios\msa\translations_nucl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first sequence and do context menu {Edit->Replace selected rows with reverce complement}
    // CHANGES: using main menu
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 0));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace selected rows with reverse-complement"});
    GTUtilsTaskTreeView::waitTaskFinished();
    //  Expected state: sequence changed from TTG -> CAA
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CAA", "Clipboard string and expected MSA string differs" + clipboardText);

    //                 sequence name  changed from L -> L|revcompl
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 2, "nameList doesn't contain enough strings");
    CHECK_SET_ERR(nameList[0] == "L|revcompl", "There are no 'L|revcompl' in nameList");

    // 3. Do step 2 again
    // CHANGES: using main menu
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace selected rows with reverse-complement"});

    // Expected state: sequence changed from CAA -> TTG
    // GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TTG", "Clipboard string and expected MSA string differs");

    //                 sequence name changed from L|revcompl ->
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() >= 2, "nameList doesn't contain enough strings");
    CHECK_SET_ERR(!nameList.contains("L|revcompl"), "There are 'L|revcompl' in nameList");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // Add tests on alignment translation features (0002432)

    // 1. Open file _common_data\scenarios\msa\revcompl.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/revcompl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select all sequences and do context menu {Edit->Replace selected rows with reverce complement}
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTMouseDriver::click(Qt::RightButton);
    GTWidget::click(GTUtilsMdi::activeWindow());

    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: result alignement must be
    // CAA---
    // --TGA-
    // ---ATC
    const QStringList expectedData = {"CAA---", "--TGA-", "---ATC"};
    const QStringList actualData = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(actualData == expectedData, "Clipboard data and expected MSA data differs");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Kalign crashes on amino alignment that was generated from nucleotide alignment (0002658)

    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Convert alignment to amino. Use context menu {Export->Amino translation of alignment rows}
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // 3. Open converted alignment. Use context menu {Align->Align with Kalign}
    GTUtilsDialog::waitForDialog(new KalignDialogFiller());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // Expected state: UGENE not crash
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    // Kalign crashes on amino alignment that was generated from nucleotide alignment (0002658)

    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Convert alignment to amino. Use context menu {Export->Amino translation of alignment rows}
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, testDir + "_common_data/scenarios/sandbox/COI_transl.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMsaEditor::getActiveMsaEditorWindow(), Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    // CHANGES: close and open MDI window
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsMdi::checkWindowIsActive("Start Page");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI_transl.aln"));
    GTMouseDriver::doubleClick();
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 3. Open converted alignment. Use context menu {Align->Align with Kalign}
    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    // Kalign crashes on amino alignment that was generated from nucleotide alignment (0002658)

    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Convert alignment to amino. Use context menu {Export->Amino translation of alignment rows}
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open converted alignment. Use context menu {Align->Align with Kalign}
    GTUtilsDialog::waitForDialog(new KalignDialogFiller());

    // CHANGES: using main menu
    GTMenu::clickMainMenuItem({"Actions", "Align", "Align with Kalign..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // UGENE crashes in malignment editor after aligning (UGENE-6)

    // 1. Do menu tools->multiple alignment->kalign, set input alignment "data/samples/CLUSTALW/COI.aln" and press Align button
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. after kalign finishes and msa opens insert gaps and click in alignment

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::click();

    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    // UGENE crashes in malignment editor after aligning (UGENE-6)

    // 1. Do menu tools->multiple alignment->kalign, set input alignment "data/samples/CLUSTALW/COI.aln" and press Align button
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());

    // CHANGES: using main menu
    GTMenu::clickMainMenuItem({"Actions", "Align", "Align with Kalign..."});

    // 2. after kalign finishes and msa opens insert gaps and click in alignment
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTMouseDriver::click();

    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    // UGENE crashes in malignment editor after aligning (UGENE-6)

    // 1. Do menu tools->multiple alignment->kalign, set input alignment "data/samples/CLUSTALW/COI.aln" and press Align button
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // CHANGES: close and open MDI window, close Project tree view
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::toggleView();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. after kalign finishes and msa opens insert gaps and click in alignment
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::click();

    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // ugene crashes when removing document after kalign (UGENE-36)
    //
    // 1. create empty project
    // 2. do menu {tools->multiple alignment->kalign}, set aligned document samples/CLUSTALW/COI.aln

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/", "COI.aln"));
    GTMenu::clickMainMenuItem({"Tools", "Multiple sequence alignment", "Align with Kalign..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. aligned document opens
    GTUtilsMdi::activeWindow();

    // 4. select document in project and press del
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {
    // ugene crashes when removing document after kalign (UGENE-36)
    //
    // 1. create empty project
    // 2. do menu {tools->multiple alignment->kalign}, set aligned document samples/CLUSTALW/COI.aln

    // CHANGES: opens file, Kalign by popup menu
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. aligned document opens

    // 4. select document in project and press del
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: UGENE not crash
}

GUI_TEST_CLASS_DEFINITION(test_0015_2) {
    // ugene crashes when removing document after kalign (UGENE-36)
    //
    // 1. create empty project
    // 2. do menu {tools->multiple alignment->kalign}, set aligned document samples/CLUSTALW/COI.aln

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/", "COI.aln"));
    GTMenu::clickMainMenuItem({"Tools", "Multiple sequence alignment", "Align with Kalign..."});
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // CHANGES: close MDI window
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsMsaEditor::checkNoMsaEditorWindowIsOpened();

    // 4. select document in project and press del
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: UGENE not crash
    GTThread::waitForMainThread();
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Run Ugene. Open file _common_data\scenarios\msa\ma2_gapped.aln
    GTFile::copy(testDir + "_common_data/scenarios/msa/ma2_gapped.aln", sandBoxDir + "ma2_gapped.aln");
    GTFile::copy(testDir + "_common_data/scenarios/msa/ma2_gapped_edited.aln", sandBoxDir + "ma2_gapped_edited.aln");
    GTFileDialog::openFile(sandBoxDir, "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Open same file in text editor. Change first 3 bases of 'Phaneroptera_falcata'
    //    from 'AAG' to 'CTT' and save file.
    //    Expected state: Dialog suggesting to reload modified document has appeared.
    //    Press 'Yes'.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTFile::copy(sandBoxDir + "ma2_gapped.aln", sandBoxDir + "ma2_gapped_old.aln");
    GTFile::copy(sandBoxDir + "ma2_gapped_edited.aln", sandBoxDir + "ma2_gapped.aln");
    GTGlobals::sleep(10000);  // Wait up to 10 seconds so UGENE will find the changes.

    //    Expected state: document was reloaded, view activated.
    //    'Phaneroptera_falcata' starts with CTT.
    GTUtilsMdi::activeWindow();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CTT", "MSA part differs from expected");
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
    // 1. Run Ugene. Open file _common_data\scenarios\msa\ma2_gapped.aln
    GTFile::copy(testDir + "_common_data/scenarios/msa/ma2_gapped.aln", sandBoxDir + "ma2_gapped.aln");
    GTFileDialog::openFile(sandBoxDir, "ma2_gapped.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // CHANGES: insert gaps in the beginning
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // 2. Open same file in text editor. Change first 3 bases of 'Phaneroptera_falcata' from 'AAG' to 'CTT' and save file.
    //  Expected state: Dialog suggesting to reload modified document has appeared. Press 'Yes'.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTFile::copy(testDir + "_common_data/scenarios/msa/ma2_gapped_edited.aln", sandBoxDir + "ma2_gapped.aln");

    // Wait for the document to reload (1 second granularity).
    GTGlobals::sleep(2000);

    // Expected state: document was reloaded, view activated.
    // 'Phaneroptera_falcata' starts with CTT.
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    // copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));
    GTKeyboardUtils::copy();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CTT", "MSA part differs from expected. Expected: CTT, actual: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
    // 1. Run Ugene. Open file _common_data\scenarios\msa\ma2_gapped.aln
    GTFile::copy(testDir + "_common_data/scenarios/msa/ma2_gapped.aln", sandBoxDir + "ma2_gapped.aln");
    GTFileDialog::openFile(sandBoxDir, "ma2_gapped.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Open same file in text editor. Change first 3 bases of 'Phaneroptera_falcata'
    //    from 'AAG' to 'CTT' and save file.
    // CHANGES: backup old file, copy changed file
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTGlobals::sleep(1000);  // ugene doesn't detect changes whithin one second interval
    GTFile::copy(testDir + "_common_data/scenarios/msa/ma2_gapped_edited.aln", sandBoxDir + "ma2_gapped.aln");

    //    Expected state: Dialog suggesting to reload modified document has appeared.
    // 3. Press 'Yes'.
    GTUtilsDialog::checkNoActiveWaiters();

    //    Expected state: document was reloaded, view activated.
    //    'Phaneroptera_falcata' starts with CTT.

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));
    // copy to clipboard
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTThread::waitForMainThread();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CTT", "MSA part differs from expected. Expected: CTT, actual: " + clipboardText);

    // CHANGES: select item in project tree view and press delete
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("ma2_gapped.aln"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // Add a molecule from project  (UGENE-288)
    //
    // 1. Open file data/samples/Genbank/murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open file data/samples/MSF/HMA.msf
    GTFileDialog::openFile(dataDir + "samples/MSF/", "HMA.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. On MSA editor {Context Menu->Add->Sequence from current project}
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_LOAD, "Sequence from current project"}));

    // 4. Select item dialog appeared
    // Expected state: loaded sequences present in list
    GTUtilsDialog::add(new ProjectTreeItemSelectorDialogFiller("murine.gb", "NC_001363"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_0017_1) {
    // Add a molecule from project  (UGENE-288)
    //
    // 1. Open file data/samples/Genbank/murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open file data/samples/MSF/HMA.msf
    GTFileDialog::openFile(dataDir + "samples/MSF/", "HMA.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. On MSA editor {Context Menu->Add->Sequence from current project}
    // 4. Select item dialog appeared
    // Expected state: loaded sequences present in list
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("murine.gb", "NC_001363"));

    // CHANGES: using main menu instead of popup
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from current project..."});
}

GUI_TEST_CLASS_DEFINITION(test_0017_2) {
    // Add a molecule from project  (UGENE-288)
    //
    // 1. Open file data/samples/Genbank/murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // CHANGES: close MDI window of murine.gb
    GTUtilsMdi::click(GTGlobals::Close);

    // 2. Open file data/samples/MSF/HMA.msf
    GTFileDialog::openFile(dataDir + "samples/MSF/", "HMA.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. On MSA editor {Context Menu->Add->Sequence from current project}
    // 4. Select item dialog appeared
    // Expected state: loaded sequences present in list
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("murine.gb", "NC_001363"));

    // CHANGES: using main menu instead of popup
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from current project..."});
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    // Shifting sequences in the Alignment Editor (UGENE-238)
    //
    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Click on some row in sequence names area
    GTUtilsMsaEditor::clickSequence(2);

    // Expected state: row became selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 2, 604, 1));

    // 3. Click & drag selected row in sequence names area
    QStringList list1 = GTUtilsMSAEditorSequenceArea::getNameList();

    QRect rowNameRect = GTUtilsMsaEditor::getSequenceNameRect(2);
    QRect destinationRowNameRect = GTUtilsMsaEditor::getSequenceNameRect(3);
    GTMouseDriver::dragAndDrop(rowNameRect.center(), destinationRowNameRect.center());

    // Expected state: row order changes respectively
    QStringList list2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(list1 != list2, "Name list wasn't changed 1");

    // 4. Click & drag on unselected area
    rowNameRect = GTUtilsMsaEditor::getSequenceNameRect(0);
    destinationRowNameRect = GTUtilsMsaEditor::getSequenceNameRect(1);
    GTMouseDriver::dragAndDrop(rowNameRect.center(), destinationRowNameRect.center());

    // Expected state: multiple rows selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 604, 2));

    // 5. Click & drag selected block
    rowNameRect = GTUtilsMsaEditor::getSequenceNameRect(0);
    destinationRowNameRect = GTUtilsMsaEditor::getSequenceNameRect(1);
    GTMouseDriver::dragAndDrop(rowNameRect.center(), destinationRowNameRect.center());

    // Expected state: whole selected block shifted
    QStringList list3 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(list2 != list3, "Name list wasn't changed 2");

    // 6. Click on some row in selected block
    GTUtilsMsaEditor::clickSequence(1);

    // Expected state: selection falls back to one row
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 1, 604, 1));
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    // UGENE-79 In MSA editor support rows collapsing mode
    //
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList nameListWithNoGroups = GTUtilsMSAEditorSequenceArea::getVisibleNames(true);
    CHECK_SET_ERR(nameListWithNoGroups.size() == 18, "Wrong sequence count in original mode: " + QString::number(nameListWithNoGroups.size()));

    // 2. Press button Enable collapsing
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ folded together
    QStringList nameListWithCollapsedGroup = GTUtilsMSAEditorSequenceArea::getVisibleNames(true);
    CHECK_SET_ERR(nameListWithCollapsedGroup.size() == 17, "Wrong sequence count in collapsed mode: " + QString::number(nameListWithCollapsedGroup.size()));
    QString groupName = nameListWithCollapsedGroup[13];
    CHECK_SET_ERR(groupName == "[2] Mecopoda_elongata__Ishigaki__J", "Collapsed group has no 'count' badge: " + groupName);
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    // UGENE crashes when all columns in MSAEditor are deleted (UGENE-329)
    //
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select Edit -> remove columns of gaps -> remove columns with number of gaps 1.
    // 3. Click OK
    GTUtilsDialog::waitForDialog(new DeleteGapsDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::checkNoActiveWaiters();

    // Expected state: UGENE not crashes, deletion is not performed
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(0, 9));

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString text = GTClipboard::text();
    QString expected = "A\nA\nT\nA\nT\nT\nT\nA\nA\nA";
    CHECK_SET_ERR(text == expected, "expected: " + expected + "found: " + text);
}

GUI_TEST_CLASS_DEFINITION(test_0020_1) {
    // UGENE crashes when all columns in MSAEditor are deleted (UGENE-329)
    //
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Insert some gaps to the first column. Ensure, that every column has a gap
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    for (int i = 0; i < 6; i++) {
        GTKeyboardDriver::keyClick(' ');
        GTUtilsTaskTreeView::waitTaskFinished();
    }
    // 3. Select Edit -> remove columns of gaps -> remove columns with number of gaps 1.
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(19, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString initial = GTClipboard::text();
    // 4. Click OK
    GTUtilsDialog::waitForDialog(new DeleteGapsDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: UGENE not crashes, deletion is not performed
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(19, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString final = GTClipboard::text();

    CHECK_SET_ERR(initial == final, "msa area was changed");
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    // MSA editor zoom bug (UGENE-520)
    //
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. zoom MSA to maximum
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom In"));
        GTUtilsTaskTreeView::waitTaskFinished();
    }

    // Expected state: top sequence not overlaps with ruler
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom Out"));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0021_1) {
    // MSA editor zoom bug (UGENE-520)
    //
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. zoom MSA to maximum
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom In"));
        GTUtilsTaskTreeView::waitTaskFinished();
    }

    // Expected state: top sequence not overlaps with ruler
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom Out"));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0021_2) {
    // MSA editor zoom bug (UGENE-520)
    //
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. zoom MSA to maximum
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom In"));
        GTUtilsTaskTreeView::waitTaskFinished();
    }

    // Expected state: top sequence not overlaps with ruler
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom Out"));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select character â„–3 in "Phaneroptera_falcata"(G)
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 0));
    auto posLabel = GTWidget::findLabel("Position");
    CHECK_SET_ERR(posLabel->text() == "Pos 3 / 14", "Expected text: Pos 3/14. Found: " + posLabel->text());
    // Expected state: Statistics "Pos" in right bottom is "Pos 3/14"

    // 3. Insert 3 gaps to first three positoons in "Phaneroptera_falcata"
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    for (int i = 0; i < 3; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        GTUtilsTaskTreeView::waitTaskFinished();
    }
    // 4. Select char at 4 position in "Phaneroptera_falcata"(A)
    GTUtilsMSAEditorSequenceArea::click(QPoint(3, 0));
    CHECK_SET_ERR(posLabel->text() == "Pos 1 / 14", "Expected text: Pos 1/14. Found: " + posLabel->text());
    // Expected state: Gaps are inserted, statistics "Pos" in right bottom is "Pos 1/14"
}

GUI_TEST_CLASS_DEFINITION(test_0022_1) {  // DIFFERENCE: Column label is tested
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select character â„–3 in "Phaneroptera_falcata"(G)
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 0));
    auto colLabel = GTWidget::findLabel("Column");
    CHECK_SET_ERR(colLabel->text() == "Col 3 / 14", "Expected text: Col 3/14. Found: " + colLabel->text());
    // Expected state: Statistics "Pos" in right bottom is "Pos 3/14"

    // 3. Insert 3 gaps to first three positoons in "Phaneroptera_falcata"
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    for (int i = 0; i < 3; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        GTUtilsTaskTreeView::waitTaskFinished();
    }
    // 4. Select char at 4 position in "Phaneroptera_falcata"(A)
    GTUtilsMSAEditorSequenceArea::click(QPoint(3, 0));
    CHECK_SET_ERR(colLabel->text() == "Col 4 / 17", "Expected text: Col 4 / 17. Found: " + colLabel->text());
    // Expected state: Gaps are inserted, statistics "Pos" in right bottom is "Pos 1/14"
}

GUI_TEST_CLASS_DEFINITION(test_0022_2) {  // DIFFERENCE: Line label is tested
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the thirs character in "Phaneroptera_falcata"(G)
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 0));

    // Expected state: Statistics "Seq" in right bottom is "Seq 1 / 10"
    auto lineLabel = GTWidget::findLabel("Line");
    CHECK_SET_ERR(lineLabel->text() == "Seq 1 / 10", "Expected text: Seq 1 / 10. Found: " + lineLabel->text());

    // 3. Select and delete 5 lines
    GTUtilsMsaEditor::selectRows(3, 7);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 4. Select char at 4 position in "Phaneroptera_falcata"(A)
    GTUtilsMSAEditorSequenceArea::click(QPoint(3, 0));
    // Expected state: Gaps are inserted, statistics "Seq" in right bottom is "Seq 1 / 5"
    CHECK_SET_ERR(lineLabel->text() == "Seq 1 / 5", "Expected text: Seq 1 / 5. Found: " + lineLabel->text());
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    // Test that an external sequence can be added to an alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Call context menu->Add->sequence from file.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/", "CVU55762.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    // Check that the sequence is present.
    GTUtilsMsaEditor::clickSequenceName("CVU55762");
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. select first symbol of first sequence
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click();
    // 3. press toolbar button "zoom to selection"
    int initOffset = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    // offsets are used to check zooming
    QAbstractButton* zoom_to_sel = GTAction::button("Zoom To Selection");
    GTWidget::click(zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished();

    int finOffset = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(initOffset >= (finOffset * 2 - 8), "inital offset: " + QString().setNum(initOffset) + " final offset: " + QString().setNum(finOffset));
    // Expected state: MSA is zoomed

    // 4. press toolbar button "Reset zoom"
    QAbstractButton* resetZoom = GTAction::button("Reset Zoom");
    GTWidget::click(resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex() == initOffset, "MSA is not zoomed back");
    // Expected state: MSA is zoomed back
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    // Note: the test depends on the fact that the first font in the Font Selection dialog is not the current font used by UGENE.

    // Open document samples/CLUSTALW/COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    auto msaEditor = GTWidget::findExactWidget<MsaEditorWgt*>("msa_editor_COI_0")->getEditor();
    QString initialFont = msaEditor->getFont().toString();

    // Click "change font button" on the toolbar.
    GTUtilsDialog::waitForDialog(new FontDialogFiller());
    GTWidget::click(GTAction::button("Change Font"));

    // Check that the font was changed and is not equal to the initial.
    QString currentFont = msaEditor->getFont().toString();
    CHECK_SET_ERR(currentFont != initialFont, "Expected font to be changed, initial: " + initialFont + ", current: " + currentFont);
}

// windows test
GUI_TEST_CLASS_DEFINITION(test_0025_1) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. press "change font button" on toolbar
    GTUtilsDialog::waitForDialog(new FontDialogFiller());
    GTWidget::click(GTAction::button("Change Font"));

    auto nameListWidget = GTWidget::findWidget("msa_editor_COI_0");
    auto ui = qobject_cast<MsaEditorWgt*>(nameListWidget);

    QFont f = ui->getEditor()->getFont();
    QString expectedFont = "Verdana,10,-1,5,50,0,0,0,0,0";

    CHECK_SET_ERR(f.toString().startsWith(expectedFont), "Expected: " + expectedFont + ", found: " + f.toString());
    //    Expected state: change font dialog appeared

    //    3. choose some font, press OK
    //    Expected state: font is changed
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    // Open document samples/CLUSTALW/COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Press "export as image" on toolbar.
    QString filePath = testDir + "_common_data/scenarios/sandbox/image.bmp";
    GTUtilsDialog::waitForDialog(new ExportImage(filePath));
    GTWidget::click(GTAction::button("export_msa_as_image_action"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: image is exported.
    bool isFileExist = GTFile::check(filePath);
    CHECK_SET_ERR(isFileExist, "Image file not found: " + filePath);
}

GUI_TEST_CLASS_DEFINITION(test_0026_1) {  // DIFFERENCE: context menu is used
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. press "export as image" on toolbar
    QString filePath = testDir + "_common_data/scenarios/sandbox/image.bmp";
    GTUtilsDialog::waitForDialog(new ExportImage(filePath));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Expected state: image is exported.
    bool isFileExist = GTFile::check(filePath);
    CHECK_SET_ERR(isFileExist, "Image file not found: " + filePath);
}

GUI_TEST_CLASS_DEFINITION(test_0026_2) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    //    2. press "export as image" on toolbar
    GTUtilsDialog::waitForDialog(new ExportImage(testDir + "_common_data/scenarios/sandbox/bigImage.bmp", "JPG", 100));
    QAbstractButton* saveImage = GTAction::button("export_msa_as_image_action");
    GTWidget::click(saveImage);

    //    Expected state: export dialog appeared
    GTUtilsDialog::waitForDialog(new ExportImage(testDir + "_common_data/scenarios/sandbox/smallImage.bmp", "JPG", 50));
    GTWidget::click(saveImage);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. fill dialog:
    //    file name: test/_common_data/scenarios/sandbox/image.bmp
    //    press OK
    qint64 bigSize = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/bigImage.jpg");
    qint64 smallSize = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/smallImage.jpg");
    CHECK_SET_ERR(bigSize > 3 * 1000 * 1000 && bigSize < 7 * 1000 * 1000, "Invalid big image size: " + QString::number(bigSize));
    CHECK_SET_ERR(smallSize > 700 * 1000 && smallSize < 2 * 1000 * 1000, "Invalid small image size: " + QString::number(smallSize));
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. select element 4 in sequence 3
    GTUtilsMSAEditorSequenceArea::click(QPoint(3, 2));

    //    3. Move selected left using mouse by 6
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection(QPoint(3, 2), QPoint(9, 2));

    //    Expected state: area is moved,position 4-9 filled with gaps
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 2), QPoint(8, 2));
    GTKeyboardUtils::copy();
    const QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "------", "Expected: ------ Found: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0027_1) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. select element 4 in sequences 2 and 3
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 2), QPoint(3, 3));

    //    3. Move selected left using mouse by 6
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection(QPoint(3, 2), QPoint(9, 2));

    //    Expected stste: area is moved,position 4-9 filled with gaps
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 2), QPoint(8, 3));
    GTKeyboardUtils::copy();
    const QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "------\n------", "Expected: ------\n------ Found: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0027_2) {
    // Drag & drop of selection should be disabled in multi-selection mode.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select region 1.
    GTUtilsMSAEditorSequenceArea::selectArea({3, 3}, {10, 5});

    // Select region 2.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequence(7);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected state: 2 rows are selected.
    GTUtilsMsaEditor::checkSelection({{3, 3, 8, 3}, {3, 7, 8, 1}});

    // Check that selection can't be moved:
    // try moving the selected region && check that instead of moving
    // a new selection was created from the drag range.
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection({4, 4}, {10, 8});
    GTUtilsMsaEditor::checkSelection({{4, 4, 7, 5}});
}

GUI_TEST_CLASS_DEFINITION(test_0028_linux) {
    //    1. Open document "samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Context menu -- "Export as image"
    GTUtilsDialog::waitForDialog(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test.svg", QString("SVG")));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    //    3. Fill dialog: svg format, output file
    qint64 fileSize = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/test.svg");
    CHECK_SET_ERR(fileSize > 6500000 && fileSize < 7500000, "Current size: " + QString::number(fileSize));
    //    Expected state:  SVG is exported
}

GUI_TEST_CLASS_DEFINITION(test_0028_windows) {
    //    1. Open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Context menu -- "Export as image"
    GTUtilsDialog::waitForDialog(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test.svg", QString("SVG")));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    //    3. Fill dialog: svg format, output file
    qint64 fileSize = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/test.svg");
    CHECK_SET_ERR(fileSize > 6500000 && fileSize < 9800000, "Current size: " + QString().setNum(fileSize));
    //    Expected state:  SVG is exported
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTUtilsProject::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    //    2. Select first sequence
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "exportSelectedMsaRowsToSeparateFilesAction"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new ExportSelectedSequenceFromAlignment(testDir + "_common_data/scenarios/sandbox/", ExportSelectedSequenceFromAlignment::FASTA, true));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Phaneroptera_falcata.fa"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Phaneroptera_falcata"));

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_Phaneroptera_falcata");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(42, 44));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_COPY, "Copy sequence"}));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "---", "Expected: TAGTTTATTAA, Found: " + clipboardText);
    //    3. use MSA area context menu->export->save sequence
    //    Exptcted state: Export sequence dialog appeared

    //    4. fill dialog:
    //    Export to file: test/_common_data/scenarios/sandbox/sequence.fa(use other extensions is branches)
    //    Add to project: checked
    //    Gap characters: keep
    //    Expectes state: sequence added to project
}

GUI_TEST_CLASS_DEFINITION(test_0029_1) {  // DIFFERENCE:gaps are trimmed, FASTQ format is used
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Select first sequence
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 2));
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "exportSelectedMsaRowsToSeparateFilesAction"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new ExportSelectedSequenceFromAlignment(testDir + "_common_data/scenarios/sandbox/", ExportSelectedSequenceFromAlignment::FASTQ, false));
    GTMouseDriver::click(Qt::RightButton);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Bicolorana_bicolor_EF540830.fastq"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Bicolorana_bicolor_EF540830"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy sequence"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "TAGTTTATTAA", "Expected: TAGTTTATTAA, Found: " + clipboardText);
    //    3. use MSA area context menu->export->save sequence
    //    Exptcted state: Export sequence dialog appeared

    //    4. fill dialog:
    //    Export to file: test/_common_data/scenarios/sandbox/sequence.fa(use other extensions is branches)
    //    Add to project: checked
    //    Gap characters: keep
    //    Expectes state: sequence added to project
}

GUI_TEST_CLASS_DEFINITION(test_0029_2) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Select first sequence
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 2));
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "exportSelectedMsaRowsToSeparateFilesAction"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new ExportSelectedSequenceFromAlignment(testDir + "_common_data/scenarios/sandbox/", ExportSelectedSequenceFromAlignment::Genbank, true, false));
    GTMouseDriver::click(Qt::RightButton);

    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "Bicolorana_bicolor_EF540830.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    3. use MSA area context menu->export->save sequence
    //    Exptcted state: Export sequence dialog appeared

    //    4. fill dialog:
    //    Export to file: test/_common_data/scenarios/sandbox/sequence.fa(use other extensions is branches)
    //    Add to project: checked
    //    Gap characters: keep
    //    Expectes state: sequence added to project
}

GUI_TEST_CLASS_DEFINITION(test_0031) {  // TODO: check statistic result
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller());
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Exptcted state: generate alignment profile dialog appeared

    //    3. Fill dialog: Profile mode:Counts. Click "Generate"
    GTWidget::findWidget("Alignment profile for ma2_gapped");
    //    Expected state: Alignment profile generated
}

GUI_TEST_CLASS_DEFINITION(test_0031_1) {  // DIFFERENCE: Percentage is used
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller(false));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Exptcted state: generate alignment profile dialog appeared

    //    3. Fill dialog: Profile mode:Counts. Click "Generate"
    GTWidget::findWidget("Alignment profile for ma2_gapped");
    //    Expected state: Alignment profile generated
}

GUI_TEST_CLASS_DEFINITION(test_0031_2) {  // TODO: check statistic result
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller(true, false, false));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Exptcted state: generate alignment profile dialog appeared

    //    3. Fill dialog: Profile mode:Counts. Click "Generate"
    GTWidget::findWidget("Alignment profile for ma2_gapped");
    //    Expected state: Alignment profile generated
}

GUI_TEST_CLASS_DEFINITION(test_0031_3) {  // TODO: check statistic result
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller(false, true, false));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Exptcted state: generate alignment profile dialog appeared

    //    3. Fill dialog: Profile mode:Counts. Click "Generate"
    GTWidget::findWidget("Alignment profile for ma2_gapped");
    //    Expected state: Alignment profile generated
}

GUI_TEST_CLASS_DEFINITION(test_0031_4) {  // TODO: check statistic result
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller(false, false, true));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Exptcted state: generate alignment profile dialog appeared

    //    3. Fill dialog: Profile mode:Counts. Click "Generate"
    GTWidget::findWidget("Alignment profile for ma2_gapped");
    //    Expected state: Alignment profile generated
}

GUI_TEST_CLASS_DEFINITION(test_0032) {
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    //    Exptcted state: generata alignment profile dialog appeared
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller(true, GenerateAlignmentProfileDialogFiller::HTML, testDir + "_common_data/scenarios/sandbox/stat.html"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 size = 0;
    size = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/stat.html");
    CHECK_SET_ERR(size > 0, "file not found");
    //    3. Fill dialog: Profile mode:Counts
    //            Save profile to file: checked
    //            file path: test/_common_data/scenarios/sandbox/stat.html(stat.csv)
    //            Click "Generate"
    //    Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0032_1) {  // DIFFERENCE: csv format is used
    //    1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do MSA area context menu->Statistics->generate grid profile
    //    Exptcted state: generata alignment profile dialog appeared
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller(true, GenerateAlignmentProfileDialogFiller::CSV, testDir + "_common_data/scenarios/sandbox/stat.html"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 size = 0;
    size = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/stat.csv");
    CHECK_SET_ERR(size > 0, "file not found");
    //    3. Fill dialog:Profile mode:Counts
    //            Save profile to file: checked
    //            file path: test/_common_data/scenarios/sandbox/stat.html(stat.csv)
    //            Click "Generate"
    //    Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0033) {
    // 1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate distance matrix
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(true, true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    // Exptcted state: generata distance matrix dialog appeared

    // 3. Fill dialog: Distance Algorithm: Hamming dissimilarity(Simple similiraty)
    //         Profile mode: Counts
    //         Exclude gakls: checked
    //         Click "Generate"
    GTWidget::findWidget("Distance matrix for ma2_gapped");
    // Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0033_1) {
    // 1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate distance matrix
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(false, true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    // Exptcted state: generata distance matrix dialog appeared

    // 3. Fill dialog: Distance Algorithm: Hamming dissimilarity(Simple similiraty)
    //         Profile mode: Counts
    //         Exclude gakls: checked
    //         Click "Generate"
    GTWidget::findWidget("Distance matrix for ma2_gapped");
    // Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
    // 1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate distance matrix
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(true, false, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    // Exptcted state: generata distance matrix dialog appeared

    // 3. Fill dialog: Distance Algorithm: Hamming dissimilarity
    //         Profile mode: Counts(Percents)
    //         Exclude gakls: checked(unchecked)
    //         Click "Generate"
    GTWidget::findWidget("Distance matrix for ma2_gapped");
    // Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0034_1) {
    // 1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate distance matrix
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(true, true, false));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    // Exptcted state: generata distance matrix dialog appeared

    // 3. Fill dialog: Distance Algorithm: Hamming dissimilarity
    //         Profile mode: Counts(Percents)
    //         Exclude gakls: checked(unchecked)
    //         Click "Generate"
    GTWidget::findWidget("Distance matrix for ma2_gapped");
    // Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    // 1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate distance matrix
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(DistanceMatrixDialogFiller::HTML, testDir + "_common_data/scenarios/sandbox/matrix.html"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    // Exptcted state: generata distance matrix dialog appeared

    // 3. Fill dialog: Distance Algorithm: Hamming dissimilarity
    //         Profile mode: Counts
    //         Exclude gakls: checked
    //         Save profile to file: checked
    //         File path: test/_common_data/scenarios/sandbox/matrix.html(matrix.csv)
    //         Click "Generate"
    qint64 size = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/matrix.html");
    CHECK_SET_ERR(size != 0, "file not created");
    // Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0035_1) {
    // 1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate distance matrix
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(DistanceMatrixDialogFiller::CSV, testDir + "_common_data/scenarios/sandbox/matrix.html"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    // Exptcted state: generata distance matrix dialog appeared

    // 3. Fill dialog: Distance Algorithm: Hamming dissimilarity
    //         Profile mode: Counts
    //         Exclude gakls: checked
    //         Save profile to file: checked
    //         File path: test/_common_data/scenarios/sandbox/matrix.html(matrix.csv)
    //         Click "Generate"
    qint64 size = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/matrix.csv");
    CHECK_SET_ERR(size != 0, "file not created");
    // Expected state: Alignment profile file created
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84(Kimura/Jukes-Cantor/LogDet)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0036_1) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    // Expected state: build tree dialog appeared
    // 3. Fill dialog:
    //     Distanse matrix model: F84(Kimura/Jukes-Cantor/LogDet)
    //     Press "Build"
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 1));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: tree appeared
    GTWidget::findGraphicsView("treeView");
}

GUI_TEST_CLASS_DEFINITION(test_0036_2) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 2));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84(Kimura/Jukes-Cantor/LogDet)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0036_3) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 3));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84(Kimura/Jukes-Cantor/LogDet)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0, 0.5));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84
    //     Gamma distributed rates across sites: checked
    //     Coefficient of variation: 0.50(50.00/99.00)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0037_1) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0, 50));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84
    //     Gamma distributed rates across sites: checked
    //     Coefficient of variation: 0.50(50.00/99.00)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0037_2) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0, 99));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84
    //     Gamma distributed rates across sites: checked
    //     Coefficient of variation: 0.50(50.00/99.00)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(100, testDir + "_common_data/scenarios/sandbox/COI.nwk", 5, BuildTreeDialogFiller::MAJORITYEXT));

    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();  // some time is needed to build tree
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84
    //     Gamma distributed rates across sites: unchecked
    //     Bootstrapping and consensus tree: checked
    //     Number of replications: 100
    //     Seed: 5
    //     Consensus type: Majority Rule extended(Strict/Majority Rule/M1)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0038_1) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(100, testDir + "_common_data/scenarios/sandbox/COI.nwk", 5, BuildTreeDialogFiller::STRICTCONSENSUS));

    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();  // some time is needed to build tree
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distance matrix model: F84
    //     Gamma distributed rates across sites: unchecked
    //     Bootstrapping and consensus tree: checked
    //     Number of replications: 100
    //     Seed: 5
    //     Consensus type: Majority Rule extended(Strict/Majority Rule/M1)
    //     Press "Build"
    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0038_2) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(100, testDir + "_common_data/scenarios/sandbox/COI.nwk", 5, BuildTreeDialogFiller::MAJORITY));

    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distanse matrix model: F84
    //     Gamma distributed rates across sites: unchecked
    //     Bootatraping and consensus tree: checked
    //     Number of replications: 100
    //     Seed: 5
    //     Consensus type: Majority Rule extended(Strict/Majority Rule/M1)
    //     Press "Build"

    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0038_3) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(100, testDir + "_common_data/scenarios/sandbox/COI.nwk", 5, BuildTreeDialogFiller::M1));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distanse matrix model: F84
    //     Gamma distributed rates across sites: unchecked
    //     Bootatraping and consensus tree: checked
    //     Number of replications: 100
    //     Seed: 5
    //     Consensus type: Majority Rule extended(Strict/Majority Rule/M1)
    //     Press "Build"
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

GUI_TEST_CLASS_DEFINITION(test_0038_4) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Press "build tree" button on toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(100, testDir + "_common_data/scenarios/sandbox/COI.nwk", 5, BuildTreeDialogFiller::M1, 1));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    // Expected state: build tree dialog appeared

    // 3. Fill dialog:
    //     Distanse matrix model: F84
    //     Gamma distributed rates across sites: unchecked
    //     Bootatraping and consensus tree: checked
    //     Number of replications: 100
    //     Seed: 5
    //     Consensus type: Majority Rule extended(Strict/Majority Rule/M1)
    //     Press "Build"

    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::findGraphicsView("treeView");
    // Expected state: tree appeared
}

void test_0039_function(int comboNum, const QString& extension) {
    // 1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(UGUITest::dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use project tree context menu->Export/Import->Export Nucleic Alignment to Amino Translation
    // Expected state: Export Nucleic Alignment to Amino Translation dialog appeared
    // 3.Fill dialog:
    //     File name: test/_common_data/scenarios/sandbox/transl.aln
    //     File format: CLUSTALW(use other formats too, check extension change)
    //     Amino translation: Standard genetic code
    //     Add document to project: checked
    GTUtilsDialog::add(new PopupChooser({"action_project__export_import_menu_action", "action_project__export_to_amino_action"}));
    GTUtilsDialog::add(new ExportMSA2MSADialogFiller(comboNum, UGUITest::testDir + "_common_data/scenarios/sandbox/COI_transl.aln"));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: transl.aln appeared in project
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI_transl." + extension));
}

GUI_TEST_CLASS_DEFINITION(test_0039) {
    //    QMap<int,QString> extMap;
    //    extMap[0] = "aln";
    //    extMap[1] = "fa";
    //    extMap[2] = "msf";
    //    extMap[3] = "meg";
    //    extMap[4] = "nex";
    //    extMap[5] = "phy";
    //    extMap[6] = "phy";
    //    extMap[7] = "sto";
    test_0039_function(0, "aln");
}

GUI_TEST_CLASS_DEFINITION(test_0039_1) {
    test_0039_function(1, "fa");
}

GUI_TEST_CLASS_DEFINITION(test_0039_2) {
    test_0039_function(2, "meg");
}

GUI_TEST_CLASS_DEFINITION(test_0039_3) {
    test_0039_function(3, "msf");
}

GUI_TEST_CLASS_DEFINITION(test_0039_4) {
    test_0039_function(4, "nex");
}

GUI_TEST_CLASS_DEFINITION(test_0039_5) {
    test_0039_function(5, "phy");
}

GUI_TEST_CLASS_DEFINITION(test_0039_6) {
    test_0039_function(6, "phy");
}

GUI_TEST_CLASS_DEFINITION(test_0039_7) {
    test_0039_function(7, "sto");
}

GUI_TEST_CLASS_DEFINITION(test_0040) {  // UGENE crashes when opening several files
    QFile human_T1(dataDir + "/samples/FASTA/human_T1.fa");
    human_T1.copy(dataDir + "/samples/CLUSTALW/human_T1.fa");
    GTFileDialog::openFileList(dataDir + "samples/CLUSTALW/", {"COI.aln", "human_T1.fa"});

    GTUtilsProjectTreeView::findIndex("human_T1.fa");  // checks inside
    GTUtilsProjectTreeView::findIndex("COI.aln");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));

    QFile(dataDir + "/samples/CLUSTALW/human_T1.fa").remove();
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    // Shifting region in the Alignment Editor (UGENE-2127)
    //
    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    int oldLength = GTUtilsMSAEditorSequenceArea::getLength();
    // 2. Select the first column.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);
    // Expected state: column became selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 1, 18));

    // 3. Drag the selection with mouse to 5 bases to the right.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(5, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    // Expected state: alignment moved to 5 bases to the right.

    // 4. Drag the selection with mouse to one base to the left.
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(4, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    // Expected state: alignment  moved to one bases to the left.

    // Check results
    int newLength = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(4 == newLength - oldLength, QString("Wrong length of changed alignment"));
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
    // default msa export
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_0042.png"));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0042_1) {
    // "all included" export
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_0042_1.png", ExportMsaImage::Settings(true, true, true) /*include all*/));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0042_2) {
    // slightly modified export
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_0042_1", ExportMsaImage::Settings(true, false, true) /*include all*/, true, false, RegionMsa(), "BMP"));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
    // select a few sequences
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtils::checkExportServiceIsEnabled();

    QStringList sequences = {"Montana_montana", "Conocephalus_percaudata", "Podisma_sapporensis"};

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(
        new ExportMsaImage(
            testDir + "_common_data/scenarios/sandbox/test_0043.png",
            ExportMsaImage::Settings(),
            false,
            false,
            RegionMsa(U2Region(1, 594), sequences)));

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0044) {
    // export selected region
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 2), QPoint(25, 8));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_0044.png", ExportMsaImage::Settings(true, true, true), false, true));

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0045) {
    // check the connection between export comboBox and selectRegion dialog
    // there should be no selection
    class ExportDialogChecker : public Filler {
    public:
        ExportDialogChecker()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new DefaultDialogFiller("SelectSubalignmentDialog", QDialogButtonBox::Cancel));
            auto exportType = GTWidget::findComboBox("comboBox", dialog);
            GTComboBox::selectItemByText(exportType, "Custom region", GTGlobals::UseMouse);

            CHECK_SET_ERR(exportType->currentText() == "Whole alignment", "Wrong combo box text!");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportDialogChecker());

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0045_1) {
    // check the connection between export comboBox and selectRegion dialog
    // there should be no selection

    class ExportChecker : public Filler {
    public:
        ExportChecker()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto exportType = GTWidget::findComboBox("comboBox", dialog);

            GTUtilsDialog::waitForDialog(
                new SelectSubalignmentFiller(
                    RegionMsa(U2Region(1, 593),
                              {"Montana_montana", "Conocephalus_percaudata"})));
            auto select = GTWidget::findPushButton("selectRegionButton", dialog);
            GTWidget::click(select);

            CHECK_SET_ERR(exportType->currentText() == "Custom region", "Wrong combo box text!");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportChecker());

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0046) {
    // check quality
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_0046", "JPG", 50));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0047) {
    // check select subalignment dialog

    class SelectSubalignmentChecker : public Filler {
    public:
        SelectSubalignmentChecker()
            : Filler("SelectSubalignmentDialog") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* ok = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(ok != nullptr, "ok button is NULL");

            auto startLineEdit = GTWidget::findSpinBox("startLineEdit", dialog);
            GTSpinBox::setValue(startLineEdit, 10);

            auto endLineEdit = GTWidget::findSpinBox("endLineEdit", dialog);
            GTSpinBox::setValue(endLineEdit, 5);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTWidget::click(ok);

            GTSpinBox::setValue(endLineEdit, 15);
            auto noneButton = GTWidget::findWidget("noneButton", dialog);
            GTWidget::click(noneButton);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTWidget::click(ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    class ExportChecker : public Filler {
    public:
        ExportChecker()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new SelectSubalignmentChecker());
            auto select = GTWidget::findPushButton("selectRegionButton", dialog);
            GTWidget::click(select);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(1, 1));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportChecker());
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0048) {
    // fail to export big alignment
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CustomFiller_0048 : public Filler {
    public:
        CustomFiller_0048()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto exportType = GTWidget::findComboBox("comboBox", dialog);
            CHECK_SET_ERR(exportType->currentText() == "Whole alignment", "Wrong combo box text!");

            auto hintLabel = GTWidget::findLabel("hintLabel", dialog);
            CHECK_SET_ERR(hintLabel->isVisible(), "Warning message is hidden!");

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(exportButton != nullptr, "Cannot find Export button");
            CHECK_SET_ERR(!exportButton->isEnabled(), "Export button is enabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new CustomFiller_0048());

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0049) {
    // save alignment buttons test
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new ExportDocumentDialogFiller(sandBoxDir, "COI_test_0049.aln", ExportDocumentDialogFiller::CLUSTALW));
    GTWidget::click(GTAction::button("Save alignment as"));
    GTUtilsProjectTreeView::click("COI.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTFileDialog::openFile(sandBoxDir, "COI_test_0049.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(10, 10));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTAction::button("Save alignment"));
    GTUtilsProjectTreeView::click("COI_test_0049.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir, "COI_test_0049.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(10, 0), "ATTCGAGCCGA");
}

GUI_TEST_CLASS_DEFINITION(test_0050) {
    //    1. Open "COI.aln"
    //    2. Set any reference sequence
    //    3. Open context menu, open the "Highlighting" submenu, set the "Agreements" type
    //    4. Open context menu again, open the "Export" submenu, choose the "Export highlighted" menu item
    //    Expected state: the "Export highlighted to file" dialog appears - there is a checkbox 'transpose output'
    //    5. Click "Export"
    //    Expected state: result file contain columns of sequences
    //    6. Repeat 3-4
    //    7. Deselect 'Transpose output' and click 'Export'
    //    Expected state: result file contain rowa of sequences

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::add(new PopupChooserByText({"Set this sequence as reference"}));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton, QPoint(10, 10));

    GTUtilsDialog::add(new PopupChooserByText({"Appearance", "Highlighting", "Agreements"}));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);

    GTUtilsDialog::add(new PopupChooserByText({"Export", "Export highlighted"}));
    GTUtilsDialog::add(new ExportHighlightedDialogFiller(sandBoxDir + "common_msa_test_0050_1.txt"));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTFile::equals(sandBoxDir + "common_msa_test_0050_1.txt", testDir + "_common_data/clustal/COI_highlighted_1"),
                  "Transposed export is incorrect");

    GTUtilsDialog::add(new PopupChooserByText({"Export", "Export highlighted"}));
    GTUtilsDialog::add(new ExportHighlightedDialogFiller(sandBoxDir + "common_msa_test_0050_2.txt", false));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTFile::equals(sandBoxDir + "common_msa_test_0050_2.txt", testDir + "_common_data/clustal/COI_highlighted_2"),
                  "Export is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_0052) {
    //    1. Open "_common_data/clustal/3000_sequences.aln"
    //    2. Context menu -- Export as ImageExport
    //    Expected state: export dialog appeared, there is a warning message and Export button is disabled
    //    3. Select a small region
    //    Expected state: warning is gone, export is enabled
    GTFileDialog::openFile(testDir + "_common_data/clustal/3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CustomFiller_0052 : public Filler {
    public:
        CustomFiller_0052()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto exportType = GTWidget::findComboBox("comboBox", dialog);
            CHECK_SET_ERR(exportType->currentText() == "Whole alignment", "Wrong combo box text!");

            auto hintLabel = GTWidget::findLabel("hintLabel", dialog);
            CHECK_SET_ERR(hintLabel->isVisible(), "Warning message is hidden!");

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(exportButton != nullptr, "Cannot find Export button");
            CHECK_SET_ERR(!exportButton->isEnabled(), "Export button is enabled");

            GTUtilsDialog::waitForDialog(
                new SelectSubalignmentFiller(
                    RegionMsa(U2Region(1, 593),
                              {"Sequence__1", "Sequence__2", "Sequnce__3", "Sequence__4"})));

            auto select = GTWidget::findPushButton("selectRegionButton", dialog);
            GTWidget::click(select);

            CHECK_SET_ERR(exportType->currentText() == "Custom region", "Wrong combo box text!");

            CHECK_SET_ERR(!hintLabel->isVisible(), "Warning is visible");
            CHECK_SET_ERR(exportButton->isEnabled(), "Export button is disabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new CustomFiller_0052());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_0053) {
    // Copied formatted (context menu)
    // 1. Open amples\CLUSTALW\COI.aln
    // 2. Select the first three letters TAA
    // 3. Context menue {Copy-><<Copy formatted}
    // Expected state: the buffer contatin the sequence in CLUSTALW format
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "copy_formatted"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString clipboardText = GTClipboard::text();

    CHECK_SET_ERR(clipboardText.contains("TAA"), clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0053_1) {
    // Copied formatted (context menu), the format is changable
    // 1. Open samples\CLUSTALW\COI.aln
    // 2. Select the first three letters TAA
    // 3. In the general tab of the options panel find the Copy Type combobox and select the Mega format
    // 4. Context menu {Copy->Copy formatted}
    // Expected state: the buffer contatin the sequence in Mega format
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto copyType = GTWidget::findComboBox("copyType");

    GTComboBox::selectItemByText(copyType, "Mega");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "copy_formatted"}));
    GTMouseDriver::click(Qt::RightButton);

    QString clipboardText = GTClipboard::text();

    CHECK_SET_ERR(clipboardText.contains("mega"), clipboardText);
    CHECK_SET_ERR(clipboardText.contains("TAA"), clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0053_2) {
    // Copied formatted (toolbar), the format is changable
    // 1. Open samples\CLUSTALW\COI.aln
    // 2. Select the first three letters TAA
    // 3. In the general tab of the options panel find the Copy Type combobox and select the CLUSTALW format
    // 4. Toolbar {Copy->Copy formatted}
    // Expected state: the buffer contatin the sequence in CLUSTALW format
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto copyType = GTWidget::findComboBox("copyType");

    GTComboBox::selectItemByText(copyType, "CLUSTALW");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));

    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "copy_formatted"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QString clipboardText = GTClipboard::text();

    CHECK_SET_ERR(clipboardText.contains("CLUSTAL W 2.0 multiple sequence alignment"), clipboardText);
    CHECK_SET_ERR(clipboardText.contains("TAA"), clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0053_3) {
    // Copied formatted (context menu) for a big alignment
    // 1. Open _common_data/clustal/100_sequences.aln
    // 2. Select the whole alignment
    // 3. Context menue {Copy->Copy formatted}
    // Expected state: the buffer contatin the sequences in CLUSTALW format
    GTFileDialog::openFile(testDir + "_common_data/clustal/100_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(!names.isEmpty(), "the alignment is empty");
    GTUtilsMSAEditorSequenceArea::selectSequence(names.first());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "copy_formatted"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString clipboardText = GTClipboard::text();

    CHECK_SET_ERR(clipboardText.contains("ACCAGGCTTGGCAATGCGTATC"), clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0053_4) {
    // Copied formatted (action is disabled when no selection
    // 1. Open samples\CLUSTALW\COI.aln
    // 2. Try context menue {Copy->Copy formatted}
    // Expected state: the action is disabled
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* w = GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "copy_formatted");
    CHECK_SET_ERR(w != nullptr, "no copy action on the toolbar");
    CHECK_SET_ERR(w->isEnabled() == false, "selection is empty but the action is enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0053_5) {
    // Copied formatted (toolbar), the format is changable to RTF
    // 1. Open samples\CLUSTALW\COI.aln
    // 2. Select the first three letters TAA
    // 3. In the general tab of the options panel find the Copy Type combobox and select the RTF format
    // 4. Toolbar {Copy->Copy formatted}
    // Expected state: the buffer contatin the sequence in RTF format
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto copyType = GTWidget::findComboBox("copyType");

    GTComboBox::selectItemByText(copyType, "Rich text (HTML)");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 0));

    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "copy_formatted"));

    QString clipboardText = GTClipboard::text();

    CHECK_SET_ERR(clipboardText.contains("<span style=\"font-size:10pt; font-family:Verdana;\">"), clipboardText);
    CHECK_SET_ERR(clipboardText.contains("<p><span style=\"background-color:#ff99b1;\">T</span><span style=\"background-color:#fcff92;\">A</span><span style=\"background-color:#fcff92;\">A</span></p>"), clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0053_6) {
    // Test copying of spatial selection, whole rows mode & column range mode.

    GTFileDialog::openFile(testDir + "_common_data/clustal/region.full-gap.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea({5, 0}, {10, 1});  // 2 rows.

    // Add an extra row to the selection.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequence(10);
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    GTUtilsMsaEditor::checkSelection({{5, 0, 6, 2}, {5, 10, 6, 1}});

    // Test standard copy: only column range must be copied
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "copy_selection"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    QString clipboardText = GTClipboard::text();
    QString expectedText = "LRPSSS\n"
                           "LRPSSS\n"
                           "WKMSNA";
    CHECK_SET_ERR(clipboardText == expectedText, "Unexpected text: " + QString(clipboardText).replace("\n", "$"));

    // Test copy formatted: only column range must be copied
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "copy_formatted"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    clipboardText = GTClipboard::text();
    expectedText = "CLUSTAL W 2.0 multiple sequence alignment\n\n"
                   "Tcn2           LRPSSS 6\n"
                   "Tcn5           LRPSSS 6\n"
                   "Pc_Metavir9    WKMSNA 6\n"
                   "                : *.:\n\n";
    CHECK_SET_ERR(clipboardText == expectedText, "Unexpected formatted text: " + QString(clipboardText).replace("\n", "$"));

    // Test copying of whole row.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "copy_whole_row"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    clipboardText = GTClipboard::text();
    expectedText = "RTAGRLRPSSSPWAAPAFLIKKENGKFRFLCDFRGLNSVT\n"
                   "REAGRLRPSSSPWAAPAFLVKKENGKFRFIC---------\n"
                   "LRSGRWKMSNARNTSPMLL-----SGIRDIPPRLRCVFDL";
    CHECK_SET_ERR(clipboardText == expectedText, "Unexpected full row text: " + QString(clipboardText).replace("\n", "$"));
}

GUI_TEST_CLASS_DEFINITION(test_0054) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Call 'Align with MUSCLE'. Use 'translate to amino' option.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}));
    GTUtilsDialog::add(new MuscleDialogFiller(MuscleDialogFiller::Default, true, true));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    QString actual = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(actual.startsWith("TAAGACTTCTAATTCGAGCCGAATTAGGTCAACCAGGATACC---TAATTGGAGATGATCAAATTTATAATGTAATTGT"), "unexpected sequence: " + actual);
}

GUI_TEST_CLASS_DEFINITION(test_0054_1) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Use context menu:
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTUtilsDialog::add(new KalignDialogFiller(0, true));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();
    QString actual = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(actual.startsWith("TAAGACTTCTAATTCGAGCCGAATTAGGTCAAC---CAGGATACCTAATTGGAGATGATCAAATTTATAATG"), "unexpected sequence: " + actual);

    //    {Align->Align with MUSCLE}
    //    Check "Translate to amino when aligning" checkbox
    //    Align
}

GUI_TEST_CLASS_DEFINITION(test_0055) {
    //    Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Select some area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 2), QPoint(8, 8));
    //    Use context menu:
    //    {Export->Export subalignment}
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto filepathEdit = GTWidget::findLineEdit("filepathEdit", dialog);
            GTLineEdit::setText(filepathEdit, dataDir + "samples/CLUSTALW/COI.aln");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(new custom()));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTUtilsNotifications::waitForNotification(true, "Document is locked:");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0056) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Export sequence as alignment. In export dialog check

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto fileNameEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(fileNameEdit, sandBoxDir + "murine.aln");

            auto genbankBox = GTWidget::findCheckBox("genbankBox", dialog);
            GTCheckBox::setChecked(genbankBox, true);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ExportSequenceAsAlignmentFiller(new custom()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"action_project__export_import_menu_action", "export sequences as alignment"}));
    GTUtilsProjectTreeView::click("murine.gb", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    "Use Genbank "SOURCE" tags..." checkbox
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() == 1, QString("unexpected number of names: %1").arg(nameList.size()));
    CHECK_SET_ERR(nameList.first() == "Murine_sarcoma_virus.", "unexpected sequence name: " + nameList.first());
}

GUI_TEST_CLASS_DEFINITION(test_0057) {
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto join2alignmentMode = GTWidget::findRadioButton("join2alignmentMode", dialog);
            GTRadioButton::click(join2alignmentMode);
            GTUtilsTaskTreeView::waitTaskFinished();

            auto newDocUrl = GTWidget::findLineEdit("newDocUrl", dialog);
            GTLineEdit::setText(newDocUrl, sandBoxDir + "test_0057.aln");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils(new custom()));
    GTFileDialog::openFileList(dataDir + "samples/Genbank", {"murine.gb", "sars.gb"});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(10, 1), "AAATGAAAGAC\nATATTAGGTTT");
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto logoWidget = GTWidget::findWidget("logoWidget", dialog);
            int initHeight = logoWidget->geometry().height();
            CHECK_SET_ERR(initHeight == 0, QString("logoWidget has too big height: %1").arg(initHeight));

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/clustal/test_1393.aln"));
            GTWidget::click(GTWidget::findWidget("inputButton", dialog));

            int finalHeight = logoWidget->geometry().height();
            CHECK_SET_ERR(finalHeight == 150, QString("logoWidget has wrong height after choosing file: %1").arg(finalHeight));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new PwmBuildDialogFiller(new custom()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"TFBS_MENU", "TFBS_WEIGHT"}));
    GTMenu::showMainMenu(MWMENU_TOOLS);
    //    Use main menu {Tools->Search for TFBS->Build weigth mantix}
    //    In "Weight matrix" dialog set input amino alignment
    //    shorter then 50.
    //    Expected state: weight matrix logo appeared in dialog
    //    Change input file
    //    Expected state: logo updated
}

GUI_TEST_CLASS_DEFINITION(test_0059) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Create new color scheme. Set some new color for some

    //    character.
    //    Press "Clear" button. check state

    class customColorSelector : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto alphabetColorsFrame = GTWidget::findWidget("alphabetColorsFrame", dialog);

            int cellWidth = alphabetColorsFrame->geometry().width() / 6;
            QStringList initialColors;
            initialColors << "#ffffff"
                          << "#fcff92"
                          << "#70f970"
                          << "#4eade1"
                          << "#fcfcfc"
                          << "#ff99b1";
            QString finalColor = "#ffffff";

            GTWidget::click(GTWidget::findWidget("clearButton", dialog));
            for (double i = 0; i < 6; i++) {
                QPoint p = QPoint((i + 0.5) * cellWidth, 10);
                QColor c = GTWidget::getColor(dialog, alphabetColorsFrame->mapTo(dialog, p));
                CHECK_SET_ERR(c.name() == finalColor, QString("unexpected color at cell %1 after clearing: %2").arg(i).arg(c.name()));
                uiLog.trace(c.name());
            }

            GTWidget::click(GTWidget::findWidget("restoreButton", dialog));
            for (double i = 0; i < 6; i++) {
                QPoint p = QPoint((i + 0.5) * cellWidth, 10);
                QColor c = GTWidget::getColor(dialog, alphabetColorsFrame->mapTo(dialog, p));
                CHECK_SET_ERR(c.name() == initialColors[i], QString("unexpected color at cell %1 after clearing: %2, expected: %3").arg(i).arg(c.name()).arg(initialColors[i]));
                uiLog.trace(c.name());
            }

            GTUtilsDialog::waitForDialog(new ColorDialogFiller(255, 0, 0));
            QPoint cell2 = QPoint(1.5 * cellWidth, 10);
            GTMouseDriver::moveTo(alphabetColorsFrame->mapToGlobal(cell2));
            GTMouseDriver::click();
            QColor cell2Color = GTWidget::getColor(dialog, alphabetColorsFrame->mapTo(dialog, cell2));
            CHECK_SET_ERR(cell2Color.name() == "#ff0000", "color was chanded wrong: " + cell2Color.name());

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    class customColorSchemeCreator : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto schemeName = GTWidget::findLineEdit("schemeName", dialog);
            GTLineEdit::setText(schemeName, "GUITest_common_scenarios_msa_editor_test_0059_scheme");

            auto alphabetComboBox = (GTWidget::findComboBox("alphabetComboBox", dialog));
            GTComboBox::selectItemByText(alphabetComboBox, "Nucleotide");

            GTUtilsDialog::waitForDialog(new ColorSchemeDialogFiller(new customColorSelector()));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    class customAppSettingsFiller : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new CreateAlignmentColorSchemeDialogFiller(new customColorSchemeCreator()));
            GTWidget::click(GTWidget::findWidget("addSchemaButton", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new customAppSettingsFiller()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Open "Color schemes" dialog.
    class customAppSettingsFiller : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(QFileInfo(sandBoxDir).absoluteFilePath(), "", GTFileDialogUtils::Choose));
            GTWidget::click(GTWidget::findWidget("colorsDirButton", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new customAppSettingsFiller()));

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    //    Select some color scheme folder. Check state
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator("GUITest_common_scenarios_msa_editor_test_0060", NewColorSchemeCreator::nucl));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTFile::check(sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0060.csmsa");

    class customAppSettingsFiller1 : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto colorsDirEdit = GTWidget::findLineEdit("colorsDirEdit", dialog);
            QString path = colorsDirEdit->text();
            CHECK_SET_ERR(path.contains("_common_data/scenarios/sandbox"), "unexpected color folder: " + path);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new customAppSettingsFiller1()));

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    //    Select some color scheme folder. Check state
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
}

GUI_TEST_CLASS_DEFINITION(test_0061) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Open "Color schemes" dialog.
    //    Open "Create color scheme" dialog.
    //    Set wrong scheme names: space only, empty, with forbidden
    //    characters, duplicating existing scnemes.
    //    Check error hint in dialog

    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator("GUITest_common_scenarios_msa_editor_test_0061", NewColorSchemeCreator::nucl));

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    class customColorSchemeCreator : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto validLabel = GTWidget::findLabel("validLabel", dialog);
            auto schemeName = GTWidget::findLineEdit("schemeName", dialog);

            GTLineEdit::setText(schemeName, "   ");
            CHECK_SET_ERR(validLabel->text() == "Warning: Name can't contain only spaces.", "unexpected hint: " + validLabel->text());
            GTLineEdit::setText(schemeName, "");
            CHECK_SET_ERR(validLabel->text() == "Warning: Name of scheme is empty.", "unexpected hint: " + validLabel->text());
            GTLineEdit::setText(schemeName, "name*");
            CHECK_SET_ERR(validLabel->text() == "Warning: Name has to consist of letters, digits, spaces<br>or underscore symbols only.", "unexpected hint: " + validLabel->text());
            GTLineEdit::setText(schemeName, "GUITest_common_scenarios_msa_editor_test_0061");
            CHECK_SET_ERR(validLabel->text() == "Warning: Color scheme with the same name already exists.", "unexpected hint: " + validLabel->text());

            auto alphabetComboBox = (GTWidget::findComboBox("alphabetComboBox", dialog));
            GTComboBox::selectItemByText(alphabetComboBox, "Nucleotide");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    class customAppSettingsFiller : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new CreateAlignmentColorSchemeDialogFiller(new customColorSchemeCreator()));

            GTWidget::click(GTWidget::findWidget("addSchemaButton", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new customAppSettingsFiller()));

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QDir().mkpath(sandBoxDir + "read_only_dir");
    GTFile::setReadOnly(sandBoxDir + "read_only_dir");

    //    Open "Export subalignment" dialog
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto filepathEdit = GTWidget::findLineEdit("filepathEdit", dialog);
            //    Check wrong parameters:
            //    Dir to save does not exists
            GTLineEdit::setText(filepathEdit, sandBoxDir + "some_dir/subalignment.aln");
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Export folder does not exist"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            //    No permission  to write to folder
            GTLineEdit::setText(filepathEdit, sandBoxDir + "read_only_dir/subalignment.aln");
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "No write permission"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            //    Empty file path
            GTLineEdit::setText(filepathEdit, "");
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "No path specified"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            //    Filename is empty
            GTLineEdit::setText(filepathEdit, sandBoxDir);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Export file name is empty"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            //    Select 0 sequences
            GTLineEdit::setText(filepathEdit, sandBoxDir + "subalignment.aln");

            GTWidget::click(GTWidget::findWidget("noneButton", dialog));
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "No selected sequence found"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            //    Start pos > end pos
            auto startLineEdit = GTWidget::findLineEdit("startLineEdit", dialog);
            GTLineEdit::setText(startLineEdit, "50");
            auto endLineEdit = GTWidget::findLineEdit("endLineEdit", dialog);
            GTLineEdit::setText(endLineEdit, "40");

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Illegal column range!"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ExtractSelectedAsMSADialogFiller(new custom()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTFile::setReadWrite(sandBoxDir + "read_only_dir");
}

GUI_TEST_CLASS_DEFINITION(test_0063) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CheckActivePopupMenuScenario : public CustomScenario {
    public:
        CheckActivePopupMenuScenario(const QStringList& _actionNames)
            : actionNames(_actionNames) {
        }

        void run() override {
            QList<QAction*> menuActions = GTWidget::getActivePopupMenu()->actions();
            int nonSeparatorMenuActionCount = 0;
            for (const QAction* action : qAsConst(menuActions)) {
                if (!action->isSeparator()) {
                    nonSeparatorMenuActionCount++;
                    CHECK_SET_ERR(actionNames.contains(action->objectName()),
                                  "[" + action->objectName() + "/" + action->text() + " is not found in 'Align' menu");
                }
            }
            CHECK_SET_ERR(actionNames.size() == nonSeparatorMenuActionCount,
                          QString("Unexpected number of actions in menu with the first action = '%1', menu size: %2")
                              .arg(menuActions.isEmpty() ? "<empty>" : menuActions[0]->text())
                              .arg(nonSeparatorMenuActionCount));

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }

        QStringList actionNames;
    };

    GTUtilsDialog::waitForDialog(new PopupChecker(new CheckActivePopupMenuScenario({
        "Align with muscle",
        "Align with ClustalW",
        "Align with ClustalO",
        "Align with MAFFT",
        "Align with T-Coffee",
        "align_with_kalign",
    })));
    GTWidget::click(GTAction::button("Align"));

    GTUtilsDialog::waitForDialog(new PopupChecker(new CheckActivePopupMenuScenario({
        "align_to_alignment_ugene",
        "align_to_alignment_mafft",
        "Align sequences to profile with MUSCLE",
        "Align profile to profile with MUSCLE",
        "align-alignment-to-alignment-clustalo",
    })));
    GTWidget::click(GTAction::button("align_new_sequences_to_alignment_action"));

    GTUtilsMsaEditor::selectRows(1, 2);
    GTUtilsDialog::waitForDialog(new PopupChecker(new CheckActivePopupMenuScenario({
        "align_selection_to_alignment_mafft",
        "align_selection_to_alignment_muscle",
    })));
    GTWidget::click(GTAction::button("align_selected_sequences_to_alignment"));
}

GUI_TEST_CLASS_DEFINITION(test_0064) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Open "Statistics" OP tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    //    Set some reference sequence
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    //    Click "Show distance column". Check state
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    QString val1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    QString val2 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(2);
    CHECK_SET_ERR(val1 == "0%", "1: unexpected value 1: " + val1);
    CHECK_SET_ERR(val2 == "20%", "1: unexpected value 2: " + val2);
    //    Click "Show distance column". Check state
    GTCheckBox::setChecked(showDistancesColumnCheck, false);
    auto column = GTUtilsMSAEditorSequenceArea::getSimilarityColumn(0);
    CHECK_SET_ERR(!column->isVisible(), "similarity column unexpectedly found");
    //    Click "Show distance column". Check state
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    val1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    val2 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(2);
    CHECK_SET_ERR(val1 == "0%", "2: unexpected value 1: " + val1);
    CHECK_SET_ERR(val2 == "20%", "2: unexpected value 2: " + val2);
}

GUI_TEST_CLASS_DEFINITION(test_0065) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Use context menu: {Copy->Copy consensus with gaps}
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_COPY, "Copy consensus with gaps"}));

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    //    Check clipboard
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText.startsWith("TaAGttTatTaATtCGagCtGAAtTagG+CAaCCaGGtTat---+TaATT"), "unexpected consensus was exported: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_0066) {
    //    Open COI.aln consArea
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Select some area on consensus with mouse
    GTUtilsMsaEditor::selectColumns(1, 10, GTGlobals::UseMouse);

    //    Check selection on consensus and alignment
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(1, 0), QPoint(10, 17)));
}

GUI_TEST_CLASS_DEFINITION(test_0067) {
    // TODO: write this test when UGENE-4803 is fixed
    //     Open COI.aln
    //     Build tree displayed with msa
    //     Use context menu on tree tab(in tabWidget)
    //     Check all actions in popup menu
    CHECK_SET_ERR(false, "The test is not implemented");
}

GUI_TEST_CLASS_DEFINITION(test_0069) {
    //    Open COI.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Press on some sequence in nameList
    GTUtilsMsaEditor::clickSequence(2);
    auto hscroll = GTWidget::findScrollBar("horizontal_names_scroll");
    auto vscroll = GTWidget::findScrollBar("vertical_sequence_scroll");

    //    Check keys:
    //    right,
    for (int i = 0; i < 3; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right);
    }
    int value = GTScrollBar::getValue(hscroll);
    CHECK_SET_ERR(value == 3, QString("right key works wrong. Scrollbar has value: %1").arg(value));

    //    left
    for (int i = 0; i < 2; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Left);
    }
    value = GTScrollBar::getValue(hscroll);
    CHECK_SET_ERR(value == 1, QString("left key works wrong. Scrollbar has value: %1").arg(value));

    //    page down
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    value = GTScrollBar::getValue(vscroll);
    CHECK_SET_ERR(value > 20, QString("page down key works wrong: %1").arg(value));

    //    page up
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    CHECK_SET_ERR(GTScrollBar::getValue(vscroll) == 0, QString("page up key works wrong: %1").arg(vscroll->value()));

    //    end
    GTKeyboardDriver::keyClick(Qt::Key_End);
    value = GTScrollBar::getValue(vscroll);
    CHECK_SET_ERR(value > 1650, QString("end key works wrong: %1").arg(value));

    //    home
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    value = GTScrollBar::getValue(vscroll);
    CHECK_SET_ERR(value == 0, QString("end key works wrong: %1").arg(value));

    //    mouse wheel
    for (int i = 0; i < 3; i++) {
        GTMouseDriver::scroll(-1);
    }
    value = GTScrollBar::getValue(vscroll);
    CHECK_SET_ERR(value > 0, QString("scroll down works wrong. Scrollbar has value: %1").arg(value));

    for (int i = 0; i < 2; i++) {
        GTMouseDriver::scroll(1);
    }
    int value2 = GTScrollBar::getValue(vscroll);
    CHECK_SET_ERR(value2 > 0 && value2 < value, QString("scroll up works wrong. Scrollbar has value: %1, old value: %2").arg(value2).arg(value));
}

GUI_TEST_CLASS_DEFINITION(test_0070) {
    //    Open empty alignment
    GTFileDialog::openFile(testDir + "_common_data/fasta", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Press on nameList area
    GTWidget::click(GTWidget::findWidget("msa_editor_name_list"));
    //    Check state
}

GUI_TEST_CLASS_DEFINITION(test_0071) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Click on some character on sequence area
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 2));
    //    Press on other character with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::click(QPoint(8, 8));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    //    Expected state: selection is created on these characters
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(2, 2), QPoint(8, 8)));
}

GUI_TEST_CLASS_DEFINITION(test_0072) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "Chikungunya_E1.fasta");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();  // wait for overview rendering to finish.

    GTUtilsMSAEditorSequenceArea::click(QPoint(5, 5));
    //    Check keys: arrows
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 4), QPoint(5, 4)));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 4), QPoint(4, 4)));

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 5), QPoint(4, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 5), QPoint(5, 5)));

    //    shift + arrows
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 4), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 4), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 5), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTThread::waitForMainThread();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 5), QPoint(5, 5)));
    //    end
    auto hbar = GTWidget::findScrollBar("horizontal_sequence_scroll");
    GTKeyboardDriver::keyClick(Qt::Key_End);
    CHECK_SET_ERR(hbar->value() == hbar->maximum(), QString("end key scrollbar value: %1").arg(hbar->value()))
    //    home
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    CHECK_SET_ERR(hbar->value() == 0, QString("home key works wrong. Scrollbar value: %1").arg(hbar->value()))
    //    page down
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    CHECK_SET_ERR(hbar->value() > 20, QString("page down key works wrong. Scrollbar value: %1").arg(hbar->value()))
    //    page up
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    CHECK_SET_ERR(hbar->value() == 0, QString("page down key works wrong. Scrollbar value: %1").arg(hbar->value()))
    //  end+shift
    auto vbar = GTWidget::findScrollBar("vertical_sequence_scroll");
    GTKeyboardDriver::keyClick(Qt::Key_End, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() == vbar->maximum(), QString("shift + end key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  home+shift
    GTKeyboardDriver::keyClick(Qt::Key_Home, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() == 0, QString("shift + home key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  page down+shift
    GTKeyboardDriver::keyClick(Qt::Key_PageDown, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() > 20, QString("shift + page down key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  page up + shift
    GTKeyboardDriver::keyClick(Qt::Key_PageUp, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() == 0, QString("shift + page down key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  wheel event
    for (int i = 0; i < 3; i++) {
        GTMouseDriver::scroll(-1);
        GTThread::waitForMainThread();
    }

    int scrollBarOffset = hbar->value();
    int minCharWidth = 12;
    int maxCharWidth = 24;
    CHECK_SET_ERR(scrollBarOffset % 3 == 0 && scrollBarOffset >= 3 * minCharWidth && scrollBarOffset <= 3 * maxCharWidth,
                  QString("scroll down works wrong. Scrollbar has value: %1").arg(hbar->value()));

    for (int i = 0; i < 2; i++) {
        GTMouseDriver::scroll(1);
        GTThread::waitForMainThread();
    }
    scrollBarOffset = hbar->value();
    CHECK_SET_ERR(scrollBarOffset >= minCharWidth && scrollBarOffset <= maxCharWidth, QString("scroll up works wrong. Scrollbar has value: %1").arg(hbar->value()));
}

GUI_TEST_CLASS_DEFINITION(test_0073) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Unload document
    GTUtilsDialog::add(new PopupChooser({"action_project__unload_selected_action"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);
    //    Use context menu on object: {Open view -> Open new view: Alignment editor}
    GTUtilsDialog::add(new PopupChooser({"openInMenu", "action_open_view"}));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);
    //    Expected: view is opened, document is loaded
    GTUtilsMdi::findWindow("COI [COI.aln]");
}

GUI_TEST_CLASS_DEFINITION(test_0074) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(0, 5));
    //    Open "Export subalignment" dialog
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            QStringList list = ExtractSelectedAsMSADialogFiller::getSequences(true);
            CHECK_SET_ERR(list.first() == "Phaneroptera_falcata", "unexpected first sequence: " + list.first());
            CHECK_SET_ERR(list.last() == "Metrioptera_japonica_EF540831", "unexpected last sequence: " + list.last());
            CHECK_SET_ERR(list.size() == 6, QString("Unexpected initial list size: %1").arg(list.size()));
            //    Press "Invert selection" button. Expected: selection is inverted
            GTWidget::click(GTWidget::findWidget("invertButton", dialog));
            list = ExtractSelectedAsMSADialogFiller::getSequences(true);
            CHECK_SET_ERR(list.first() == "Gampsocleis_sedakovii_EF540828", "unexpected first sequence(inverted): " + list.first());
            CHECK_SET_ERR(list.last() == "Hetrodes_pupus_EF540832", "unexpected last sequence(inverted): " + list.last());
            CHECK_SET_ERR(list.size() == 12, QString("Unexpected initial list size: %1").arg(list.size()));
            //    Press "Select all" button. Expected: all sequences selected
            GTWidget::click(GTWidget::findWidget("allButton", dialog));
            list = ExtractSelectedAsMSADialogFiller::getSequences(true);
            CHECK_SET_ERR(list.first() == "Phaneroptera_falcata", "unexpected first sequence(all): " + list.first());
            CHECK_SET_ERR(list.last() == "Hetrodes_pupus_EF540832", "unexpected last sequence(all): " + list.last());
            CHECK_SET_ERR(list.size() == 18, QString("Unexpected initial list size: %1").arg(list.size()));

            GTWidget::click(GTWidget::findWidget("noneButton", dialog));
            list = ExtractSelectedAsMSADialogFiller::getSequences(true);
            CHECK_SET_ERR(list.isEmpty(), QString("list is not cleared: %1").arg(list.size()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ExtractSelectedAsMSADialogFiller(new custom()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
}

GUI_TEST_CLASS_DEFINITION(test_0075) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto overview = GTWidget::findWidget("msa_overview_area_graph");

    QImage initialImage = GTWidget::getImage(overview);

    //  Use context menu on overview: {Calculation method->Clustal}
    GTUtilsDialog::waitForDialog(new PopupChooser({"Calculation method", "Clustal"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(overview);

    //  Check that image was changed.
    QImage clustalModeImage = GTWidget::getImage(overview);
    CHECK_SET_ERR(initialImage != clustalModeImage, "overview was not changed (clustal)");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to 'histogram' mode.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Display settings", "Graph type", "Histogram"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(overview);
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage histogramModeImage = GTWidget::getImage(overview);
    CHECK_SET_ERR(histogramModeImage != clustalModeImage, "overview was not changed (histogram)");
}

GUI_TEST_CLASS_DEFINITION(test_0076) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto overview = GTWidget::findWidget("msa_overview_area_graph");
    //    Show simple overview
    GTUtilsDialog::waitForDialog(new PopupChooser({"Show simple overview"}));
    GTMenu::showContextMenu(overview);

    auto simple = GTWidget::findWidget("msa_overview_area_simple");
    QColor initColor = GTWidget::getColor(simple, simple->geometry().center());
    QString initColorS = initColor.name();
    //    Press on overview with mouse left button

    GTWidget::click(overview);
    QColor finalColor = GTWidget::getColor(simple, simple->geometry().center());
    QString finalColorS = finalColor.name();
    CHECK_SET_ERR(initColorS != finalColorS, "color was not changed(1)");
    //    Expected state: visible range moved
    //    Drag visible range with mouse
    QColor initColor1 = GTWidget::getColor(simple, simple->geometry().topLeft() + QPoint(5, 5));
    QString initColorS1 = initColor1.name();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(10, GTMouseDriver::getMousePosition().y()));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    //    Expected state: visible range dragged
    QColor finalColor1 = GTWidget::getColor(simple, simple->geometry().topLeft() + QPoint(5, 5));
    QString finalColorS1 = finalColor1.name();
    CHECK_SET_ERR(initColorS1 != finalColorS1, "color was not changed(2)")
}

GUI_TEST_CLASS_DEFINITION(test_0077) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Open tree with msa
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Press "refresh tree" button on tree view toolbar
    QWidget* button = GTAction::button("Refresh tree");
    bool vis = button->isVisible();
    if (vis) {
        GTWidget::click(button);
    } else {
        auto extButton = GTWidget::findWidget("qt_toolbar_ext_button", GTWidget::findWidget("msa_editor_tree_view_container_widget"));
        GTUtilsDialog::waitForDialog(new PopupChooser({"Refresh tree"}));
        GTWidget::click(extButton);
    }
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: tree refreshed
}

GUI_TEST_CLASS_DEFINITION(test_0078) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Open tree with msa
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Zoom in the tree to make horizontal scroll bar visible.
    QWidget* treeViewer = GTWidget::findWidget("treeView");
    auto parent = GTWidget::findWidget("qt_scrollarea_hcontainer", treeViewer);
    auto horizontalScrollbar = parent->findChild<QScrollBar*>();
    int valueBefore = GTScrollBar::getValue(horizontalScrollbar);

    GTWidget::click(treeViewer);
    GTUtilsPhyTree::zoomWithMouseWheel(treeViewer, 10);

    // Check that scroll bar is shifted to the center: the value is increased.
    int valueAfter = GTScrollBar::getValue(horizontalScrollbar);
    CHECK_SET_ERR(valueAfter > valueBefore, QString("Unexpected scroll value: %1, original value: %2").arg(valueAfter).arg(valueBefore));
}

GUI_TEST_CLASS_DEFINITION(test_0079) {
    // Open an alignment with some alphabet.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");

    // Do the action for a sequence (or sequences) of the same alphabet.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTACG\r\n");

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "paste"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();

    // The sequence was added to the 8th position of the alignment.
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[8] == "human_T1", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0080) {
    // Open an alignment with some alphabet.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");

    // Use a sequence of another alphabet.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTACS\r\n");

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "paste"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // A warning notification appears:
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Extended DNA\"");

    QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();

    // The sequence was added to the bottom of the alignment.
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[8] == "human_T1", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0081) {
    // Open an alignment with some alphabet.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");

    // Use a sequence of another alphabet.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTACS\r\n>human_T2\r\nACGTAC\r\n");

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "paste"}));

    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // A warning notification appears:
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Extended DNA\"");

    QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();

    // The sequence was added to the bottom of the alignment.
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[9] == "human_T2", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0082) {
    // Open an alignment with some alphabet.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Use a sequence of another alphabet.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTAC\r\n>human_T2\r\nACGTACS\r\n>human_T3\r\nACGTAC\r\n");

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "paste"}));

    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // A warning notification appears:
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Extended DNA\"");

    QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();

    // The sequence was added to the bottom of the alignment.
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[10] == "human_T3", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0083) {
    // Open an alignment with some alphabet.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Use a sequence of another alphabet.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTAC\r\n>human_T2\r\nACGTACS\r\n>human_T3\r\nQQ\r\n");

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "paste"}));

    GTMouseDriver::click(Qt::RightButton);

    // A warning notification appears:
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Raw\"");

    GTUtilsDialog::checkNoActiveWaiters();

    QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();

    // The sequence was added to the bottom of the alignment.
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[10] == "human_T3", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0090) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QWidget* sequenceAreaWidget = GTUtilsMsaEditor::getSequenceArea();

    // Check that sequence area cell contains a text character up until the cell size is > 7px.
    // 7px is a hardcoded constant in the MA editor.
    int minWidthToShowText = 7;
    QRect prevRect(0, 0, 10000, 10000);
    while (true) {
        QRect globalRect = GTUtilsMSAEditorSequenceArea::getPositionRect(QPoint(1, 1));  // Using 1,1 but not 0,0 because 0,0 has a focus frame drawing artifacts.
        QRect msaAreaCellRect(sequenceAreaWidget->mapFromGlobal(globalRect.topLeft()), sequenceAreaWidget->mapFromGlobal(globalRect.bottomRight()));
        // Using '-1' because cellImageRect may contain border-line pixels from the next base.
        QRect msaAreaCellRectToCheck(msaAreaCellRect.x(), msaAreaCellRect.y(), msaAreaCellRect.width() - 1, msaAreaCellRect.height() - 1);
        QImage sequenceAreaImage = GTWidget::getImage(sequenceAreaWidget);
        QImage cellImage = GTWidget::createSubImage(sequenceAreaImage, msaAreaCellRectToCheck);
        bool hasOnlyBgColor = GTWidget::hasSingleFillColor(cellImage, "#FCFF92");
        bool hasTextInTheCell = !hasOnlyBgColor;
        if (globalRect.width() >= minWidthToShowText) {
            CHECK_SET_ERR(hasTextInTheCell, "Expected to have text with the given zoom range");
        } else {
            CHECK_SET_ERR(!hasTextInTheCell, "Expected to have no text with the given zoom range");
            break;
        }
        // Check that at least one rect dimension was reduced. Some fonts on Windows may have equal width on "Zoom Out" but in this case they always have different height.
        bool isWidthReduced = globalRect.width() < prevRect.width();
        bool isHeightReduced = globalRect.height() < prevRect.height();
        CHECK_SET_ERR(isWidthReduced || isHeightReduced, "Zoom Out had no effect");
        prevRect = globalRect;

        GTUtilsMsaEditor::zoomOut();
    }
}

GUI_TEST_CLASS_DEFINITION(test_0091) {
    // 1. Open file _common_data\scenarios\msa\nucl_with_leading_gaps.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/nucl_with_leading_gaps.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate with "Include gaps"
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0091.aln", true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected: TAVS\nXXVS
    const QString clipboardText = GTClipboard::text();
    const QString expectedMSA = "TAVS\nXXVS";
    CHECK_SET_ERR(clipboardText == expectedMSA, QString("Expected: %1, current: %2").arg(expectedMSA).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0092) {
    // 1. Open file _common_data\scenarios\msa\nucl_with_leading_gaps.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/nucl_with_leading_gaps.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate with "Include gaps", click on the "Gap" radio button
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0092.aln", true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected: TAVS\n--VS
    const QString clipboardText = GTClipboard::text();
    const QString expectedMSA = "TAVS\n--VS";
    CHECK_SET_ERR(clipboardText == expectedMSA, QString("Expected: %1, current: %2").arg(expectedMSA).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0093_1) {
    // 1. Open file _common_data\scenarios\msa\nucl_with_leading_gaps.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/nucl_with_leading_gaps.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate with "frame" 2
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0093.aln", false, false, 2));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected: PPCP\nCP--
    const QString clipboardText = GTClipboard::text();
    const QString expectedMSA = "PPCP\nCP--";
    CHECK_SET_ERR(clipboardText == expectedMSA, QString("Expected: %1, current: %2").arg(expectedMSA).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0093_2) {
    // 1. Open file _common_data\scenarios\msa\nucl_with_leading_gaps.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/nucl_with_leading_gaps.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Do document context menu {Export->Export aligniment to amino format}
    // 3. Translate with "frame" -3
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "GUITest_common_scenarios_msa_editor_test_0093.aln", false, false, -3));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Copy to clipboard
    GTUtilsMSAEditorSequenceArea::selectArea();
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    // Expected: GGHG\nHG--
    const QString clipboardText = GTClipboard::text();
    const QString expectedMSA = "GHGG\nGH--";

    CHECK_SET_ERR(clipboardText == expectedMSA, QString("Expected: %1, current: %2").arg(expectedMSA).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_0094) {
    // Check that sort by group size works correctly.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Check that group is in the middle (original).
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getVisibleNames(true);
    CHECK_SET_ERR(originalNames.size() == 17, "Wrong sequence count in collapsed mode: " + QString::number(originalNames.size()));
    QString expectedGroupName = "[2] Mecopoda_elongata__Ishigaki__J";
    CHECK_SET_ERR(originalNames[13] == expectedGroupName, "Group is not found at index 13. Found: " + expectedGroupName[13]);

    QStringList originalNamesWithNoGroup = originalNames;
    originalNamesWithNoGroup.removeAt(13);

    // Sort by group size ascending.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_groups_by_size_ascending"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    QStringList ascendingNames = GTUtilsMSAEditorSequenceArea::getVisibleNames(true);
    CHECK_SET_ERR(ascendingNames.size() == 17, "Wrong sequence count after ascending sort: " + QString::number(ascendingNames.size()));
    CHECK_SET_ERR(ascendingNames[16] == expectedGroupName, "Group is not found at index 16. Found: " + ascendingNames[16]);

    // Check that order of other sequences is not changed.
    QStringList ascendingNamesWithNoGroup = ascendingNames;
    ascendingNamesWithNoGroup.removeAt(16);
    CHECK_SET_ERR(ascendingNamesWithNoGroup == originalNamesWithNoGroup,
                  "Ascending order was changed for non-group sequences : " + ascendingNamesWithNoGroup.join(",") +
                      " Original: " + originalNamesWithNoGroup.join(","));

    // Sort by group size descending.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_groups_by_size_descending"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    QStringList descendingNames = GTUtilsMSAEditorSequenceArea::getVisibleNames(true);
    CHECK_SET_ERR(descendingNames.size() == 17, "Wrong sequence count after descending sort: " + QString::number(descendingNames.size()));
    CHECK_SET_ERR(descendingNames[0] == expectedGroupName, "Group is not found at index 0. Found: " + descendingNames[0]);

    // Check that order of other sequences is not changed.
    QStringList descendingNamesWithNoGroup = descendingNames;
    descendingNamesWithNoGroup.removeAt(0);
    CHECK_SET_ERR(descendingNamesWithNoGroup == originalNamesWithNoGroup,
                  "Descending order was changed for non-group sequences: " + descendingNamesWithNoGroup.join(",") +
                      " Original: " + originalNamesWithNoGroup.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_0095) {
    // Check that sequences can be moved from one alignment into another.
    QString sourceFile = "align.aln";  // {"IXI_234", "IXI_236", "IXI_237", "IXI_235"}
    QString targetFile = "amino_from_wikipedia.aln";  // {"CYS1_DICDI", "ALEU_HORVU", "CATH_HUMAN"}

    GTFileDialog::openFile(testDir + "_common_data/clustal/" + sourceFile);
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Check that 'Move' menu is disabled (no active selection).
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object"}, PopupChecker::IsDisabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Select a couple of sequences and check that 'Move' menu is enabled now and have a disabled "No other objects" item.
    GTUtilsMsaEditor::selectRowsByName({"IXI_234", "IXI_235"});
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object", "no_other_objects_item"}, PopupChecker::IsDisabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object", "move_selection_to_new_file"}, PopupChecker::IsEnabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Open another file. Check that sequences can be moved now. Move them.
    GTFileDialog::openFile(testDir + "_common_data/clustal/" + targetFile);
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMdi::activateWindow(sourceFile);
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "move_selection_to_another_object", targetFile}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    Document* sourceDoc = GTUtilsDocument::getDocument(sourceFile);
    Document* targetDoc = GTUtilsDocument::getDocument(targetFile);
    CHECK_SET_ERR(sourceDoc->isModified(), "sourceDoc must be marked as modified");
    CHECK_SET_ERR(targetDoc->isModified(), "targetDoc must be marked as modified");

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"IXI_236", "IXI_237"}), "Unexpected source msa name list: " + nameList.join(","));

    GTUtilsMdi::activateWindow(targetFile);
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"CYS1_DICDI", "ALEU_HORVU", "CATH_HUMAN", "IXI_234", "IXI_235"}),
                  "Unexpected target msa name list: " + nameList.join(","));

    // Make the source document read-only. Check that menu is disabled.
    GTUtilsMdi::activateWindow(sourceFile);
    GTUtilsDocument::lockDocument(sourceFile);
    GTUtilsMsaEditor::selectRowsByName({"IXI_236"});
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object"}, PopupChecker::IsDisabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Make the target file read-only and the source not. Check that menu is enabled but has no object items.
    GTUtilsDocument::lockDocument(targetFile);
    GTUtilsDocument::unlockDocument(sourceFile);
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object", "no_other_objects_item"}, PopupChecker::IsDisabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object", targetFile}, PopupChecker::NotExists));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object", "move_selection_to_new_file"}, PopupChecker::IsEnabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Make the target file not read-only. Check that menu is back again.
    GTUtilsDocument::unlockDocument(targetFile);
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "move_selection_to_another_object", targetFile}, PopupChecker::IsEnabled));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
}

GUI_TEST_CLASS_DEFINITION(test_0096) {
    // Check that sequences can be moved to a new MSA document.
    QString sourceFile = "align.aln";  // {"IXI_234", "IXI_236", "IXI_237", "IXI_235"}
    QString targetAlnFile = "test_0096.aln";
    QString targetStoFile = "test_0096.sto";

    GTFileDialog::openFile(testDir + "_common_data/clustal/" + sourceFile);
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select a couple of sequences and check that 'Move' menu is enabled now and have a disabled "No other objects" item.
    GTUtilsMsaEditor::selectRowsByName({"IXI_234", "IXI_237"});
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "move_selection_to_another_object", "move_selection_to_new_file"}));
    GTUtilsDialog::add(new GTFileDialogUtils(sandBoxDir, targetAlnFile, GTFileDialogUtils::Save));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::activateWindow(sourceFile);
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"IXI_236", "IXI_235"}), "Unexpected source msa name list: " + nameList.join(","));

    GTUtilsMdi::activateWindow(targetAlnFile);
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"IXI_234", "IXI_237"}), "Unexpected targetAln msa name list: " + nameList.join(","));

    // Now export using Stockholm format.
    GTUtilsMsaEditor::selectRowsByName({"IXI_237"});
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "move_selection_to_another_object", "move_selection_to_new_file"}));
    GTUtilsDialog::add(new GTFileDialogUtils(sandBoxDir, targetStoFile, GTFileDialogUtils::Save));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"IXI_237"}), "Unexpected targetSto msa name list: " + nameList.join(","));

    // Check modification flags & formats.
    Document* sourceDoc = GTUtilsDocument::getDocument(sourceFile);
    Document* targetAlnDoc = GTUtilsDocument::getDocument(targetAlnFile);
    Document* targetStoDoc = GTUtilsDocument::getDocument(targetStoFile);
    CHECK_SET_ERR(sourceDoc->isModified(), "sourceDoc must be marked as modified");
    CHECK_SET_ERR(targetAlnDoc->isModified(), "targetAlnDoc must be marked as modified");
    CHECK_SET_ERR(!targetStoDoc->isModified(), "targetStoDoc must not be marked as modified");

    CHECK_SET_ERR(targetAlnDoc->getDocumentFormatId() == BaseDocumentFormats::CLUSTAL_ALN, "targetAlnDoc's format must be CLUSTALW");
    CHECK_SET_ERR(targetStoDoc->getDocumentFormatId() == BaseDocumentFormats::STOCKHOLM, "targetStoDoc's format must be Stockholm");
}

}  // namespace GUITest_common_scenarios_msa_editor
}  // namespace U2
