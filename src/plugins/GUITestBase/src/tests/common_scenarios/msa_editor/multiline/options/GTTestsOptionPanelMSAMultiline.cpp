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
#include <utils/GTThread.h>

#include <QApplication>
#include <QDir>
#include <QGraphicsItem>
#include <QMainWindow>

#include <U2Core/AppContext.h>

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
#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline_options {
using namespace HI;

namespace {
void setHighlightingType(HI::GUITestOpStatus &os, const QString &type)
{
    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingScheme, type);
}
}

GUI_TEST_CLASS_DEFINITION(general_test_0002) {
    const QString seqName = "Phaneroptera_falcata";

    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    1.1. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    //    3. Use button to add Phaneroptera_falcata as referene
    GTUtilsOptionPanelMsa::addReference(os, seqName);
    //    Expected state:
    //    reference sequence line edit is empty
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");
    //    Expected state: Phaneroptera_falcata highlighted as reference

    //    4. Use button to remove reference
    GTUtilsOptionPanelMsa::removeReference(os);
    //    Expected state:
    //    reference sequence line edit contains "select and add"
    //    Phaneroptera_falcata is not highlighted as reference
    text = sequenceLineEdit->text();
    CHECK_SET_ERR(text.isEmpty(), QString("sequenceLineEdit contains %1, no text expected").arg(text));
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(general_test_0003)
{
    // UGENE-7591

    const QString seqName = "IXI_234";

    //    1. Open file test/_common_data/clustal/align.aln
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select seq.
    GTUtilsMsaEditor::selectRowsByName(os, {seqName});

    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);

    // 3. Copy seq
    GTUtilsDialog::waitForDialog(os,
                                 new PopupChooserByText(os, {"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 4. Insert seq from clipboard
    QPoint p = GTUtilsProjectTreeView::getItemCenter(os, "align.aln");
    p.setY(p.y() + 44);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // 5. Select new item
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    GTGlobals::FindOptions options = GTGlobals::FindOptions(true, Qt::MatchStartsWith);
    QModelIndex index = GTUtilsProjectTreeView::findIndex(os, treeView, "clipboard_", options);
    GTUtilsProjectTreeView::scrollToIndexAndMakeExpanded(os, treeView, index);
    p = GTUtilsProjectTreeView::getItemCenter(os, index);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 6. Delete item
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Must not crash

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMsaEditor::setMultilineMode(os, false);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(statistic_test_0001)
{
    // UGENE-7588

    const QString seqName = "IXI_234";

    //    1. Open file test/_common_data/clustal/align.aln
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);
    //    3. Use button to add Phaneroptera_falcata as reference
    GTUtilsOptionPanelMsa::addReference(os, seqName, GTUtilsOptionPanelMsa::Completer);
    //    Expected state:
    //    reference sequence line edit contains IXI_234
    auto sequenceLineEdit = GTWidget::findLineEdit(os, "sequenceLineEdit");
    QString text = sequenceLineEdit->text();
    CHECK_SET_ERR(text == seqName, QString("sequenceLineEdit contains %1, expected: %2").arg(text).arg(seqName));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, seqName), "sequence not highlighted");
    //    IXI_234 highlighted as reference

    //    4. check showDistancesColumn checkbox
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);

    //    5. Switch multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    6. uncheck showDistancesColumn checkbox
    GTCheckBox::setChecked(os, showDistancesColumnCheck, false);
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    GTCheckBox::setChecked(os, showDistancesColumnCheck, false);

    //    7. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, false);

    //    8. uncheck showDistancesColumn checkbox
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    GTCheckBox::setChecked(os, showDistancesColumnCheck, false);
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);

    //    9. Switch multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    10. uncheck showDistancesColumn checkbox
    GTCheckBox::setChecked(os, showDistancesColumnCheck, false);
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    GTCheckBox::setChecked(os, showDistancesColumnCheck, false);

    //    Expected state:
    // Must not crash

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(highlighting_test_0001)
{
    // UGENE-7603

    //    1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    auto w = GTUtilsMsaEditor::getSequenceArea(os);
    QImage initImg = GTWidget::getImage(os, w);

    // 3. Check "use dots" checkbox
    setHighlightingType(os, "Agreements");
    auto useDots = GTWidget::findCheckBox(os, "useDots");
    GTCheckBox::setChecked(os, useDots, true);

    // Expected state: no effect
    QImage img = GTWidget::getImage(os, w);
    CHECK_SET_ERR(img == initImg, "sequence area unexpectedly changed");

    // 4. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    // Expected state: not highlighted changed to dots
    img = GTWidget::getImage(os, w);
    CHECK_SET_ERR(img != initImg,
                  "image not changed"); // no way to check dots. Can only check that sequence area changed

    // 5. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 4. Remove Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::removeReference(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    w = GTUtilsMsaEditor::getSequenceArea(os);
    initImg = GTWidget::getImage(os, w);

    // 6. Check "use dots" checkbox
    setHighlightingType(os, "Agreements");
    useDots = GTWidget::findCheckBox(os, "useDots");
    GTCheckBox::setChecked(os, useDots, true);

    // Expected state: no effect
    img = GTWidget::getImage(os, w);
    CHECK_SET_ERR(img == initImg, "sequence area unexpectedly changed");

    // 4. Select Phaneroptera_falcata as reference.
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    // Expected state: not highlighted changed to dots
    img = GTWidget::getImage(os, w);
    CHECK_SET_ERR(img != initImg,
                  "image not changed"); // no way to check dots. Can only check that sequence area changed

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(search_test_0001)
{
    // UGENE-7525

    // Open file test/_common_data/clustal/align.aln
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "align.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Open the OP's "General" tab to enable the "Wrap mode" button.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // Open search option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Search);

    // Set search string
    GTUtilsOptionPanelMsa::enterPattern(os, "RHR");

    // Check selection
    QRect expectedRect(66, 0, 3, 1);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, expectedRect);

    // Check visible bases and selection
    int firstBaseIdx = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os, 0);
    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os,
                            GTUtilsMsaEditor::getEditor(os)->getUI()->getChildrenCount() - 1);

    CHECK_SET_ERR(firstBaseIdx < 66 && 68 < lastBaseIdx,
                  "Selection must be between fist and last bases");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline_options
}  // namespace U2
