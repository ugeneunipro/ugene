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
#include "GTTestsMSAEditorConsensus.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <U2View/MSAEditor.h>

#include "GTGlobals.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "utils/GTUtilsDialog.h"
namespace U2 {

namespace GUITest_common_scenarios_msa_editor_consensus {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    Check consensus in MSA editor
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Use context menu {Consensus mode} in MSA editor area.
    //    Expected state: consensus representation dialog appeared

    //    3. Select ClustalW consensus type. Click OK.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "ClustalW");

    GTUtilsMSAEditorSequenceArea::checkConsensus("              ");
    //    Expected state: consensus must be empty
    auto thresholdSlider = GTWidget::findWidget("thresholdSlider");
    CHECK_SET_ERR(!thresholdSlider->isEnabled(), "thresholdSlider is unexpectedly enabled");

    auto thresholdSpinBox = GTWidget::findWidget("thresholdSpinBox");
    CHECK_SET_ERR(!thresholdSpinBox->isEnabled(), "thresholdSpinBox is unexpectedly enabled");
}
GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Default consensus type. Set 100% threshold
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Default");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 100);

    GTUtilsMSAEditorSequenceArea::checkConsensus("aagc+tattaataa");
    // Expected state: consensus must be aagc+tattaataa

    // 4. Set 1% threshold.
    GTSpinBox::setValue(thresholdSpinBox, 1, GTGlobals::UseKeyBoard);
    GTUtilsMSAEditorSequenceArea::checkConsensus("AAGC+TATTAATAA");
    // Expected state: consensus must be AAGC+TATTAATAA
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Default consensus type. Set 100% threshold
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Default");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 30, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("AAGC+TATTAATAA");
    // Expected state: consensus must be AAGC+TATTAATAA
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Default consensus type. Set 100% threshold
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Default");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 60, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("AAGc+TaTTAAtaa");
    // Expected state: consensus must be AAGc+TaTTAAtaa
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Strict consensus type. Set 100% threshold.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Strict");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 100, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("--------------");
    // Expected state: consensus must be --------------
    GTSpinBox::setValue(thresholdSpinBox, 50, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("AAGC-TATTAAT-A");
    // 4. Set 50% threshold.
    // Expected state: consensus must be AAGC-TATTAAT-A
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Strict consensus type. Set 100% threshold.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Strict");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 30, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("AAGCTTATTAATAA");
    // Expected state: consensus must be AAGCTTATTAATAA
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Strict consensus type. Set 100% threshold.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    // GTUtilsDialog::waitForDialog(new ConsensusSelectionDialogFiller(3,60));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Strict");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 60, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("AAG--T-TTAA---");
    // Expected state: consensus must be AAG--T-TTAA---
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Levitsky consensus type. Set 90% threshold.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Levitsky");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 90, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("WAGHH--HTWW---");
    // Expected state: consensus must be WAGHH--HTWW---
    GTSpinBox::setValue(thresholdSpinBox, 60, GTGlobals::UseKeyBoard);
    GTUtilsMSAEditorSequenceArea::checkConsensus("AAGMYTWTTAA---");
    // 4. Set 60% threshold.
    // Expected state: consensus must be AAGMYTWTTAA---
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Levitsky consensus type. Set 90% threshold.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Levitsky");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 70, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("WAGYYTWYTAW---");
    // Expected state: consensus must be WAGYYTWYTAW---
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    // Check consensus in MSA editor
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu {Consensus mode} in MSA editor area.
    // Expected state: consensus representation dialog appeared
    // 3. Select Levitsky consensus type. Set 90% threshold.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(seq);

    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Levitsky");

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 100, GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus("W-------------");
    // Expected state: consensus must be W-------------
}

void checkLimits(int minVal, int maxVal) {
    auto thresholdSlider = GTWidget::findSlider("thresholdSlider");
    int actualSliderMin = thresholdSlider->minimum();
    int actualSliderMax = thresholdSlider->maximum();
    CHECK_SET_ERR(actualSliderMin == minVal, QString("wrong minimal value for slider. Expected: %1, actual: %2").arg(minVal).arg(actualSliderMin));
    CHECK_SET_ERR(actualSliderMax == maxVal, QString("wrong maximum value for slider. Expected: %1, actual: %2").arg(maxVal).arg(actualSliderMin));

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    int actualSpinMin = thresholdSpinBox->minimum();
    int actualSpinMax = thresholdSpinBox->maximum();
    CHECK_SET_ERR(actualSpinMin == minVal, QString("wrong minimal value for spin. Expected: %1, actual: %2").arg(minVal).arg(actualSpinMin));
    CHECK_SET_ERR(actualSpinMax == maxVal, QString("wrong maximum value for spin. Expected: %1, actual: %2").arg(maxVal).arg(actualSpinMin));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // check thresholdSpinBox and thresholdSlider limits
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    auto consensusType = GTWidget::findComboBox("consensusType");
    // 3. Select "Default" consensus mode. Limits are 1-100
    GTComboBox::selectItemByText(consensusType, "Default");
    checkLimits(1, 100);
    // 4. Select "Levitsky" consensus mode. Limits are 50-100
    GTComboBox::selectItemByText(consensusType, "Levitsky");
    checkLimits(50, 100);
    // 4. Select "Strict" consensus mode. Limits are 50-100
    GTComboBox::selectItemByText(consensusType, "Strict");
    checkLimits(1, 100);
}

void checkValues(int expected) {
    auto thresholdSlider = GTWidget::findSlider("thresholdSlider");
    int actualSliderValue = thresholdSlider->value();
    CHECK_SET_ERR(actualSliderValue == expected, QString("wrong value for slider. Executed: %1, actual: %2").arg(expected).arg(actualSliderValue));

    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    int actualSpinValue = thresholdSpinBox->value();
    CHECK_SET_ERR(actualSpinValue == expected, QString("wrong value for Spin. Executed: %1, actual: %2").arg(expected).arg(actualSpinValue));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // check reset button
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto consensusType = GTWidget::findComboBox("consensusType");
    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    auto thresholdResetButton = GTWidget::findWidget("thresholdResetButton");
    // 3. Select "Default" consensus mode
    GTComboBox::selectItemByText(consensusType, "Default");
    GTSpinBox::setValue(thresholdSpinBox, 10, GTGlobals::UseKeyBoard);
    GTWidget::click(thresholdResetButton);
    checkValues(100);
    // 3. Select "Levitsky" consensus mode
    GTComboBox::selectItemByText(consensusType, "Levitsky");
    GTSpinBox::setValue(thresholdSpinBox, 70, GTGlobals::UseKeyBoard);
    GTWidget::click(thresholdResetButton);
    checkValues(90);
    // 3. Select "Strict" consensus mode
    GTComboBox::selectItemByText(consensusType, "Strict");
    GTSpinBox::setValue(thresholdSpinBox, 10, GTGlobals::UseKeyBoard);
    GTWidget::click(thresholdResetButton);
    checkValues(100);
}
}  // namespace GUITest_common_scenarios_msa_editor_consensus
}  // namespace U2
