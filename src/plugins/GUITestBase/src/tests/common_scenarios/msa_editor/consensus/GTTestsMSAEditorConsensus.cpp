/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTKeyboardUtils.h"
#include <primitives/GTWidget.h>
#include <base_dialogs/GTFileDialog.h>
#include "primitives/GTMenu.h"
#include <primitives/GTTreeWidget.h>
#include <primitives/GTSpinBox.h>
#include "GTGlobals.h"
#include <primitives/GTComboBox.h>
#include "utils/GTUtilsApp.h"
#include "utils/GTUtilsDialog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "primitives/PopupChooser.h"
#include "GTUtilsMdi.h"
#include "GTUtilsTaskTreeView.h"

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>
namespace U2 {

namespace GUITest_common_scenarios_msa_editor_consensus {


GUI_TEST_CLASS_DEFINITION(test_0001){
//    Check consensus in MSA editor
//    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    2. Use context menu {Consensus mode} in MSA editor area.
//    Expected state: consensus representstion dialog appeared

//    3. Select ClustalW consensus type. Click OK.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);
    GTGlobals::sleep(500);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "ClustalW");

    GTUtilsMSAEditorSequenceArea::checkConsensus(os,"              ");
//    Expected state: consensus must be empty
    QWidget* thresholdSlider = GTWidget::findWidget(os, "thresholdSlider");
    CHECK_SET_ERR(thresholdSlider != NULL, "thresholdSlider is NULL");
    CHECK_SET_ERR(!thresholdSlider->isEnabled(), "thresholdSlider is unexpectidly enabled");

    QWidget* thresholdSpinBox = GTWidget::findWidget(os, "thresholdSpinBox");
    CHECK_SET_ERR(thresholdSpinBox != NULL, "thresholdSpinBox is NULL");
    CHECK_SET_ERR(!thresholdSpinBox->isEnabled(), "thresholdSpinBox is unexpectidly enabled");
}
GUI_TEST_CLASS_DEFINITION(test_0002){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Default consensus type. Set 100% treshhold
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Default");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,100);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "aagc+tattaataa");
//Expected state: consensus must be aagc+tattaataa

//4. Set 1% treshhold.
    GTSpinBox::setValue(os,thresholdSpinBox,1,GTGlobals::UseKeyBoard);
    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAGC+TATTAATAA");
//Expected state: consensus must be AAGC+TATTAATAA
}

GUI_TEST_CLASS_DEFINITION(test_0002_1){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Default consensus type. Set 100% treshhold
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Default");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,30,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAGC+TATTAATAA");
//Expected state: consensus must be AAGC+TATTAATAA
}

GUI_TEST_CLASS_DEFINITION(test_0002_2){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Default consensus type. Set 100% treshhold
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Default");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,60,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAGc+TaTTAAtaa");
//Expected state: consensus must be AAGc+TaTTAAtaa
}

GUI_TEST_CLASS_DEFINITION(test_0003){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Strict consensus type. Set 100% treshhold.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Strict");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,100,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "--------------");
//Expected state: consensus must be --------------
    GTSpinBox::setValue(os,thresholdSpinBox,50,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAGC-TATTAAT-A");
//4. Set 50% treshhold.
//Expected state: consensus must be AAGC-TATTAAT-A
}

GUI_TEST_CLASS_DEFINITION(test_0003_1){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Strict consensus type. Set 100% treshhold.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Strict");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,30,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAGCTTATTAATAA");
//Expected state: consensus must be AAGCTTATTAATAA
}

GUI_TEST_CLASS_DEFINITION(test_0003_2){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Strict consensus type. Set 100% treshhold.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    //GTUtilsDialog::waitForDialog(os, new ConsensusSelectionDialogFiller(os,3,60));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Strict");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,60,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAG--T-TTAA---");
//Expected state: consensus must be AAG--T-TTAA---
}

GUI_TEST_CLASS_DEFINITION(test_0004){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Levitsky consensus type. Set 90% treshhold.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Levitsky");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,90,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "WAGHH--HTWW---");
//Expected state: consensus must be WAGHH--HTWW---
    GTSpinBox::setValue(os,thresholdSpinBox,60,GTGlobals::UseKeyBoard);
    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "AAGMYTWTTAA---");
//4. Set 60% treshhold.
//Expected state: consensus must be AAGMYTWTTAA---
}

GUI_TEST_CLASS_DEFINITION(test_0004_1){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Levitsky consensus type. Set 90% treshhold.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Levitsky");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,70,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "WAGYYTWYTAW---");
//Expected state: consensus must be WAGYYTWYTAW---
}

GUI_TEST_CLASS_DEFINITION(test_0004_2){
//Check consensus in MSA editor
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Consensus mode} in MSA editor area.
//Expected state: consensus representstion dialog appeared
//3. Select Levitsky consensus type. Set 90% treshhold.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<"Consensus mode",GTGlobals::UseMouse));
    GTMenu::showContextMenu(os,seq);

    QComboBox *consensusCombo=qobject_cast<QComboBox*>(GTWidget::findWidget(os,"consensusType"));
    CHECK_SET_ERR(consensusCombo!=NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os,consensusCombo, "Levitsky");

    QSpinBox *thresholdSpinBox=qobject_cast<QSpinBox*>(GTWidget::findWidget(os,"thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox!=NULL, "consensusCombo is NULL");
    GTSpinBox::setValue(os,thresholdSpinBox,100,GTGlobals::UseKeyBoard);

    GTUtilsMSAEditorSequenceArea::checkConsensus(os, "W-------------");
//Expected state: consensus must be W-------------
}

void checkLimits(HI::GUITestOpStatus &os, int minVal, int maxVal){
    QSlider* thresholdSlider = qobject_cast<QSlider*>(GTWidget::findWidget(os, "thresholdSlider"));
    CHECK_SET_ERR(thresholdSlider != NULL, "thresholdSlider not found");
    int actualSliderMin = thresholdSlider->minimum();
    int actualSliderMax = thresholdSlider->maximum();
    CHECK_SET_ERR(actualSliderMin == minVal, QString("wrong minimal value for slider. Expected: %1, actual: %2").arg(minVal).arg(actualSliderMin));
    CHECK_SET_ERR(actualSliderMax == maxVal, QString("wrong maximim value for slider. Expected: %1, actual: %2").arg(maxVal).arg(actualSliderMin));

    QSpinBox* thresholdSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox != NULL, "thresholdSpin not found");
    int actualSpinMin = thresholdSpinBox->minimum();
    int actualSpinMax = thresholdSpinBox->maximum();
    CHECK_SET_ERR(actualSpinMin == minVal, QString("wrong minimal value for spin. Expected: %1, actual: %2").arg(minVal).arg(actualSpinMin));
    CHECK_SET_ERR(actualSpinMax == maxVal, QString("wrong maximim value for spin. Expected: %1, actual: %2").arg(maxVal).arg(actualSpinMin));
}

GUI_TEST_CLASS_DEFINITION(test_0005){
//check thresholdSpinBox and thresholdSlider limits
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(200);
    QComboBox* consensusType = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "consensusType"));
    CHECK_SET_ERR(consensusType != NULL, "consensus combobox not found");
//3. Select "Default" consensus mode. Limits are 1-100
    GTComboBox::setIndexWithText(os, consensusType, "Default");
    checkLimits(os, 1, 100);
//4. Select "Levitsky" consensus mode. Limits are 50-100
    GTComboBox::setIndexWithText(os, consensusType, "Levitsky");
    checkLimits(os, 50, 100);
//4. Select "Strict" consensus mode. Limits are 50-100
    GTComboBox::setIndexWithText(os, consensusType, "Strict");
    checkLimits(os, 1, 100);
}

void checkValues(HI::GUITestOpStatus &os, int expected){
    QSlider* thresholdSlider = qobject_cast<QSlider*>(GTWidget::findWidget(os, "thresholdSlider"));
    CHECK_SET_ERR(thresholdSlider != NULL, "thresholdSlider not found");
    int actualSliderValue = thresholdSlider->value();
    CHECK_SET_ERR(actualSliderValue == expected, QString("wrong value for slider. Exected: %1, actual: %2").arg(expected).arg(actualSliderValue));

    QSpinBox* thresholdSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox != NULL, "thresholdSpin not found");
    int actualSpinValue = thresholdSpinBox->value();
    CHECK_SET_ERR(actualSpinValue == expected, QString("wrong value for Spin. Exected: %1, actual: %2").arg(expected).arg(actualSpinValue));
}

GUI_TEST_CLASS_DEFINITION(test_0006){
//check reset button
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(200);

    QComboBox* consensusType = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "consensusType"));
    CHECK_SET_ERR(consensusType != NULL, "consensus combobox not found");
    QSpinBox* thresholdSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox != NULL, "thresholdSpin not found");
    QWidget* thresholdResetButton = GTWidget::findWidget(os, "thresholdResetButton");
    CHECK_SET_ERR(thresholdResetButton != NULL, "thresholdResetButton not found");
//3. Select "Default" consensus mode
    GTComboBox::setIndexWithText(os, consensusType, "Default");
    GTSpinBox::setValue(os, thresholdSpinBox, 10, GTGlobals::UseKeyBoard);
    GTWidget::click(os, thresholdResetButton);
    GTGlobals::sleep(500);
    checkValues(os, 100);
//3. Select "Levitsky" consensus mode
    GTComboBox::setIndexWithText(os, consensusType, "Levitsky");
    GTSpinBox::setValue(os, thresholdSpinBox, 70, GTGlobals::UseKeyBoard);
    GTWidget::click(os, thresholdResetButton);
    GTGlobals::sleep(500);
    checkValues(os, 90);
//3. Select "Strict" consensus mode
    GTComboBox::setIndexWithText(os, consensusType, "Strict");
    GTSpinBox::setValue(os, thresholdSpinBox, 10, GTGlobals::UseKeyBoard);
    GTWidget::click(os, thresholdResetButton);
    GTGlobals::sleep(500);
    checkValues(os, 100);
}
} // namespace
} // namespace U2

