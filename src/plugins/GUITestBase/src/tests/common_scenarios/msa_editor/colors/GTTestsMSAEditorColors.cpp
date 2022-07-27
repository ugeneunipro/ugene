/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <drivers/GTMouseDriver.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTUtilsDialog.h>

#include <U2Core/DNAAlphabet.h>

#include <U2View/MSAEditor.h>

#include "GTTestsMSAEditorColors.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProject.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_editor_colors {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Use context menu {Colors->UGENE} in MSA editor area.
    auto seq = GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_APPEARANCE, "Colors", "UGENE"}));
    GTMenu::showContextMenu(os, seq);

    //    Expected state: background for symbols must be:
    //    A - yellow    G - blue    T - red    C - green    gap - no background
    // check A
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 1), "#fcff92");

    // check G
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(2, 2), "#4eade1");

    // check T
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 2), "#ff99b1");

    // check C
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 0), "#70f970");

    // check gap
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 2), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Use context menu {Colors->No Colors} in MSA editor area.
    auto seq = GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_APPEARANCE, "Colors", "No colors"}));
    GTMenu::showContextMenu(os, seq);
    //    Expected state: background for symbols must be white
    // check A
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 1), "#ffffff");

    // check G
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(2, 2), "#ffffff");

    // check T
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 2), "#ffffff");

    // check C
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 0), "#ffffff");

    // check gap
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 2), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Use context menu {Colors->Jalview} in MSA editor area.
    auto seq = GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_APPEARANCE, "Colors", "Jalview"}));
    GTMenu::showContextMenu(os, seq);
    // Expected state: background for symbols must be:
    // A - green G - red T - blue  C - orange gap - no background
    // check A
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 1), "#64f73f");

    // check G
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(2, 2), "#EB413C");

    // check T
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 2), "#3C88EE");

    // check C
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 0), "#FFB340");

    // check gap
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 2), "#ffffff");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Use context menu {Colors->Persentage identity} in MSA editor area.
    //    Expected state: Background of the symbol  with the highest number of matches in the column is painted over.
    //    Intensity of colour depends on the frequency of appearance in the column.
    auto seq = GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_APPEARANCE, "Colors", "Percentage identity"}));
    GTMenu::showContextMenu(os, seq);
    //    Symbols and columns at the descending order
    //    1. A,G,T at 2,3,9
    //    2. A at 10
    //    3. T at 6
    //    4. A,C,A,T,A,T,A at 1,4,7,8,11,12,14

    //    columns without colored symbols 5,13
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(0, 1), "#CCCCFF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(1, 1), "#6464FF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(2, 1), "#6464FF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(3, 1), "#CCCCFF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(4, 1), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(5, 1), "#9999FF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(6, 1), "#CCCCFF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(7, 2), "#CCCCFF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(8, 2), "#6464FF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(9, 2), "#9999FF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(10, 1), "#CCCCFF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(11, 2), "#CCCCFF");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(12, 2), "#ffffff");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColor(os, QPoint(13, 2), "#CCCCFF");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    Highlighting scheme options should be saved on the alphabet changing for a DNA MSA

    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    3. Select "Conservation level" highlighting scheme.
    GTUtilsOptionPanelMsa::setHighlightingScheme(os, "Conservation level");

    //    4. Set the next highlighting scheme options:
    //        threshold: 70%
    //        comparison: less or equal
    //        use dots: checked.
    int expectedThreshold = 70;
    GTUtilsOptionPanelMsa::ThresholdComparison expectedThresholdComparison = GTUtilsOptionPanelMsa::LessOrEqual;
    bool expectedIsUseDotsOptionsSet = true;

    GTUtilsOptionPanelMsa::setThreshold(os, expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(os, expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(os, expectedIsUseDotsOptionsSet);

    //    5. Replace any symbol in the MSA to amino acid specific symbols, e.g. to 'Q'.
    GTUtilsMSAEditorSequenceArea::replaceSymbol(os, QPoint(0, 0), 'q');
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the alignment alphabet is changed to Raw, highlighting scheme options are the same.
    bool isAlphabetRaw = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isRaw();
    CHECK_SET_ERR(isAlphabetRaw, "Alphabet is not RAW after the symbol replacing");

    int threshold = GTUtilsOptionPanelMsa::getThreshold(os);
    GTUtilsOptionPanelMsa::ThresholdComparison thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison(os);
    bool isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet(os);

    CHECK_SET_ERR(expectedThreshold == threshold,
                  QString("Threshold is incorrect: expected %1, got %2").arg(expectedThreshold).arg(threshold));
    CHECK_SET_ERR(expectedThresholdComparison == thresholdComparison,
                  QString("Threshold comparison is incorrect: expected %1, got %2").arg(expectedThresholdComparison).arg(thresholdComparison));
    CHECK_SET_ERR(expectedIsUseDotsOptionsSet == isUseDotsOptionsSet,
                  QString("Use dots option status is incorrect: expected %1, got %2").arg(expectedIsUseDotsOptionsSet).arg(isUseDotsOptionsSet));

    //    6. Set the next highlighting scheme options:
    //        threshold: 30%
    //        comparison: greater or equal
    //        use dots: unchecked.
    expectedThreshold = 30;
    expectedThresholdComparison = GTUtilsOptionPanelMsa::GreaterOrEqual;
    expectedIsUseDotsOptionsSet = false;

    GTUtilsOptionPanelMsa::setThreshold(os, expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(os, expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(os, expectedIsUseDotsOptionsSet);

    //    7. Press "Undo" button on the toolbar.
    GTUtilsMsaEditor::undo(os);

    //    Expected state: the alignment alphabet is changed to DNA, highlighting scheme options are the same.
    const bool isAlphabetDna = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isDNA();
    CHECK_SET_ERR(isAlphabetDna, "Alphabet is not DNA after the undoing");

    threshold = GTUtilsOptionPanelMsa::getThreshold(os);
    thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison(os);
    isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet(os);

    CHECK_SET_ERR(expectedThreshold == threshold,
                  QString("Threshold is incorrect: expected %1, got %2").arg(expectedThreshold).arg(threshold));
    CHECK_SET_ERR(expectedThresholdComparison == thresholdComparison,
                  QString("Threshold comparison is incorrect: expected %1, got %2").arg(expectedThresholdComparison).arg(thresholdComparison));
    CHECK_SET_ERR(expectedIsUseDotsOptionsSet == isUseDotsOptionsSet,
                  QString("Use dots option status is incorrect: expected %1, got %2").arg(expectedIsUseDotsOptionsSet).arg(isUseDotsOptionsSet));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    Highlighting scheme options should be saved on the alphabet changing for an amino acid MSA

    //    1. Open "_common_data/fasta/RAW.fa".
    GTUtilsProject::openFileExpectRawSequence(os, testDir + "_common_data/fasta/RAW.fa", "RAW263");

    //    2. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select "Conservation level" highlighting scheme.
    GTUtilsOptionPanelMsa::setHighlightingScheme(os, "Conservation level");

    //    5. Set the next highlighting scheme options:
    //        threshold: 70%
    //        comparison: less or equal
    //        use dots: checked.
    int expectedThreshold = 70;
    GTUtilsOptionPanelMsa::ThresholdComparison expectedThresholdComparison = GTUtilsOptionPanelMsa::LessOrEqual;
    bool expectedIsUseDotsOptionsSet = true;

    GTUtilsOptionPanelMsa::setThreshold(os, expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(os, expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(os, expectedIsUseDotsOptionsSet);

    //    6. Drag and drop "RAW263" sequence object from the Project View to the MSA Editor.
    GTUtilsMsaEditor::dragAndDropSequenceFromProject(os, {"RAW.fa", "RAW263"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the alignment alphabet is changed to Raw, highlighting scheme options are the same.
    const bool isAlphabetRaw = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isRaw();
    CHECK_SET_ERR(isAlphabetRaw, "Alphabet is not RAW after the symbol replacing");

    int threshold = GTUtilsOptionPanelMsa::getThreshold(os);
    GTUtilsOptionPanelMsa::ThresholdComparison thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison(os);
    bool isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet(os);

    CHECK_SET_ERR(expectedThreshold == threshold,
                  QString("Threshold is incorrect: expected %1, got %2").arg(expectedThreshold).arg(threshold));
    CHECK_SET_ERR(expectedThresholdComparison == thresholdComparison,
                  QString("Threshold comparison is incorrect: expected %1, got %2").arg(expectedThresholdComparison).arg(thresholdComparison));
    CHECK_SET_ERR(expectedIsUseDotsOptionsSet == isUseDotsOptionsSet,
                  QString("Use dots option status is incorrect: expected %1, got %2").arg(expectedIsUseDotsOptionsSet).arg(isUseDotsOptionsSet));

    //    6. Set the next highlighting scheme options:
    //        threshold: 30%
    //        comparison: greater or equal
    //        use dots: unchecked.
    expectedThreshold = 30;
    expectedThresholdComparison = GTUtilsOptionPanelMsa::GreaterOrEqual;
    expectedIsUseDotsOptionsSet = false;

    GTUtilsOptionPanelMsa::setThreshold(os, expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(os, expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(os, expectedIsUseDotsOptionsSet);

    //    7. Press "Undo" button on the toolbar.
    GTUtilsMsaEditor::undo(os);

    //    Expected state: the alignment alphabet is changed to Amino Acid, highlighting scheme options are the same.
    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino acid after the undoing");

    threshold = GTUtilsOptionPanelMsa::getThreshold(os);
    thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison(os);
    isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet(os);

    CHECK_SET_ERR(expectedThreshold == threshold,
                  QString("Threshold is incorrect: expected %1, got %2").arg(expectedThreshold).arg(threshold));
    CHECK_SET_ERR(expectedThresholdComparison == thresholdComparison,
                  QString("Threshold comparison is incorrect: expected %1, got %2").arg(expectedThresholdComparison).arg(thresholdComparison));
    CHECK_SET_ERR(expectedIsUseDotsOptionsSet == isUseDotsOptionsSet,
                  QString("Use dots option status is incorrect: expected %1, got %2").arg(expectedIsUseDotsOptionsSet).arg(isUseDotsOptionsSet));
}

}  // namespace GUITest_common_scenarios_msa_editor_colors
}  // namespace U2
