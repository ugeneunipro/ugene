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
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDir>
#include <QGraphicsItem>
#include <QMainWindow>

#include <U2Core/AppContext.h>

#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

#include "GTTestsOptionPanelMSAMultiline.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline_options {
using namespace HI;

namespace {
void setHighlightingType(const QString& type) {
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::selectItemByText(highlightingScheme, type);
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(general_test_0002) {
    const QString seqName = "Phaneroptera_falcata";

    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    1.1. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //    3. Use button to add Phaneroptera_falcata as referene
    GTUtilsOptionPanelMsa::addReference(seqName);
    //    Expected state:
    //    reference sequence line edit is empty
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");
    //    Expected state: Phaneroptera_falcata highlighted as reference

    //    4. Use button to remove reference
    GTUtilsOptionPanelMsa::removeReference();
    //    Expected state:
    //    reference sequence line edit contains "select and add"
    //    Phaneroptera_falcata is not highlighted as reference
    text = sequenceLineEdit->text();
    CHECK_SET_ERR(text.isEmpty(), QString("sequenceLineEdit contains %1, no text expected").arg(text));
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");

    GTUtilsMsaEditor::setMultilineMode(false);
}

GUI_TEST_CLASS_DEFINITION(general_test_0003) {
    // UGENE-7591

    const QString seqName = "IXI_234";

    //    1. Open file test/_common_data/clustal/align.aln
    GTFileDialog::openFile(testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select seq.
    GTUtilsMsaEditor::selectRowsByName({seqName});

    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    // 3. Copy seq
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 4. Insert seq from clipboard
    QPoint p = GTUtilsProjectTreeView::getItemCenter("align.aln");
    p.setY(p.y() + 44);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    GTKeyboardUtils::paste();

    // 5. Select new item
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView();
    GTGlobals::FindOptions options = GTGlobals::FindOptions(true, Qt::MatchStartsWith);
    QModelIndex index = GTUtilsProjectTreeView::findIndex(treeView, "clipboard_", options);
    GTUtilsProjectTreeView::scrollToIndexAndMakeExpanded(treeView, index);
    p = GTUtilsProjectTreeView::getItemCenter(index);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 6. Delete item
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Must not crash

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::setMultilineMode(false);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(statistic_test_0001) {
    // UGENE-7588

    const QString seqName = "IXI_234";

    //    1. Open file test/_common_data/clustal/align.aln
    GTFileDialog::openFile(testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    //    3. Use button to add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(seqName, GTUtilsOptionPanelMsa::Completer);
    //    Expected state:
    //    reference sequence line edit contains IXI_234
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(seqName), "sequence not highlighted");
    //    IXI_234 highlighted as reference

    //    4. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);

    //    5. Switch multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    6. uncheck showDistancesColumn checkbox
    GTCheckBox::setChecked(showDistancesColumnCheck, false);
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    GTCheckBox::setChecked(showDistancesColumnCheck, false);

    //    7. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(false);

    //    8. uncheck showDistancesColumn checkbox
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    GTCheckBox::setChecked(showDistancesColumnCheck, false);
    GTCheckBox::setChecked(showDistancesColumnCheck, true);

    //    9. Switch multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    10. uncheck showDistancesColumn checkbox
    GTCheckBox::setChecked(showDistancesColumnCheck, false);
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    GTCheckBox::setChecked(showDistancesColumnCheck, false);

    //    Expected state:
    // Must not crash

    GTUtilsMsaEditor::setMultilineMode(false);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001) {
    // UGENE-7603

    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    auto w = GTUtilsMsaEditor::getSequenceArea();
    QImage initImg = GTWidget::getImage(w);

    // 3. Check "use dots" checkbox
    setHighlightingType("Agreements");
    auto useDots = GTWidget::findCheckBox("useDots");
    GTCheckBox::setChecked(useDots, true);

    // Expected state: no effect
    QImage img = GTWidget::getImage(w);
    CHECK_SET_ERR(img == initImg, "sequence area unexpectedly changed");

    // 4. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    // Expected state: not highlighted changed to dots
    img = GTWidget::getImage(w);
    CHECK_SET_ERR(img != initImg,
                  "image not changed");  // no way to check dots. Can only check that sequence area changed

    // 5. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 4. Remove Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::removeReference();
    GTUtilsTaskTreeView::waitTaskFinished();

    w = GTUtilsMsaEditor::getSequenceArea();
    initImg = GTWidget::getImage(w);

    // 6. Check "use dots" checkbox
    setHighlightingType("Agreements");
    useDots = GTWidget::findCheckBox("useDots");
    GTCheckBox::setChecked(useDots, true);

    // Expected state: no effect
    img = GTWidget::getImage(w);
    CHECK_SET_ERR(img == initImg, "sequence area unexpectedly changed");

    // 4. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    // Expected state: not highlighted changed to dots
    img = GTWidget::getImage(w);
    CHECK_SET_ERR(img != initImg,
                  "image not changed");  // no way to check dots. Can only check that sequence area changed

    GTUtilsMsaEditor::setMultilineMode(false);
}

GUI_TEST_CLASS_DEFINITION(search_test_0001) {
    // UGENE-7525

    // Open file test/_common_data/clustal/align.aln
    GTFileDialog::openFile(testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open the OP's "General" tab to enable the "Wrap mode" button.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // Open search option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // Set search string
    GTUtilsOptionPanelMsa::enterPattern("RHR");

    // Check selection
    QRect expectedRect(66, 0, 3, 1);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expectedRect);

    // Check visible bases and selection
    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(0);
    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(
        GTUtilsMsaEditor::getEditor()->getUI()->getChildrenCount() - 1);

    CHECK_SET_ERR(firstBaseIdx < 66 && 68 < lastBaseIdx,
                  "Selection must be between fist and last bases");

    GTUtilsMsaEditor::setMultilineMode(false);
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline_options
}  // namespace U2
