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

#include <GTGlobals.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsApp.h>
#include <utils/GTUtilsDialog.h>

#include <U2Core/DNAAlphabet.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "GTTestsMSAEditorColors.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_editor_colors {
using namespace HI;

void checkColor(HI::GUITestOpStatus &os, const QPoint &p, const QString &expectedColor, int Xmove = 0, int Ymove = 0) {
    QWidget *seq = GTWidget::findWidget(os, "msa_editor_sequence_area");
    CHECK_SET_ERR(seq != NULL, "msa_editor_sequence_area widget is NULL");

    GTUtilsMSAEditorSequenceArea::click(os, p);
    QPoint p1 = GTMouseDriver::getMousePosition();
    p1.setY(p1.y() + Ymove);
    p1.setX(p1.x() + Xmove);

    const QImage content = GTWidget::getImage(os, seq);
    const QRgb rgb = content.pixel(seq->mapFromGlobal(p1));
    const QColor color(rgb);

    CHECK_SET_ERR(color.name() == expectedColor , "Expected: " + expectedColor + " ,found: " + color.name());
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Colors->UGENE} in MSA editor area.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList( ) << MSAE_MENU_APPEARANCE << "Colors" << "UGENE"));
    GTMenu::showContextMenu(os, seq);

//    Expected state: background for symbols must be:
//    A - yellow    G - blue    T - red    C - green    gap - no backround
    //check A
    checkColor(os, QPoint(0, 1), "#fdff6a", 5);

    //check G
    checkColor(os, QPoint(2, 2), "#2aa1e1", 5, 3);

    //check T
    checkColor(os, QPoint(0, 2), "#ff7195",5);

    //check C
    checkColor(os, QPoint(4, 0), "#49f949");

    //check gap
    checkColor(os, QPoint(4, 2), "#ffffff",0,5);

}

GUI_TEST_CLASS_DEFINITION(test_0002){
//    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    2. Use context menu {Colors->No Colors} in MSA editor area.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList() << MSAE_MENU_APPEARANCE << "Colors" << "No colors"));
    GTMenu::showContextMenu(os,seq);
//    Expected state: background for symbols must be white
    //check A
    checkColor(os,QPoint(0, 1), "#ffffff",5);

    //check G
    checkColor(os,QPoint(2, 2), "#ffffff",5, 3);

    //check T
    checkColor(os,QPoint(0, 2), "#ffffff",5);

    //check C
    checkColor(os,QPoint(4, 0), "#ffffff");

    //check gap
    checkColor(os,QPoint(4, 2), "#ffffff", 0, 5);
}

GUI_TEST_CLASS_DEFINITION(test_0003){
//1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//2. Use context menu {Colors->Jalview} in MSA editor area.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList() << MSAE_MENU_APPEARANCE << "Colors" << "Jalview"));
    GTMenu::showContextMenu(os,seq);
//Expected state: background for symbols must be:
//A - green G - red T - blue  C - orange gap - no backround
    //check A
    checkColor(os, QPoint(0, 1), "#48f718", 5);

    //check G
    checkColor(os,QPoint(2, 2), "#eb1a17", 5, 3);

    //check T
    checkColor(os,QPoint(0, 2), "#1674ee",5);

    //check C
    checkColor(os,QPoint(4, 0), "#ffa318");

    //check gap
    checkColor(os,QPoint(4, 2), "#ffffff",0,5);
}

GUI_TEST_CLASS_DEFINITION(test_0004){
//    1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    2. Use context menu {Colors->Persentage identity} in MSA editor area.
//    Expected state: Background of the symbol  with the highest number of matches in the column is painted over.
//    Intensity of colour depends on the frequency of appearance in the column.
    QWidget* seq=GTWidget::findWidget(os, "msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList() << MSAE_MENU_APPEARANCE << "Colors" << "Percentage identity"));
    GTMenu::showContextMenu(os,seq);
//    Symbols and columns at the descending order
//    1. A,G,T at 2,3,9
//    2. A at 10
//    3. T at 6
//    4. A,C,A,T,A,T,A at 1,4,7,8,11,12,14

//    columns without colored symbols 5,13
    checkColor(os,QPoint(0, 1), "#a4a4ff",5);
    checkColor(os,QPoint(1, 1), "#3c3cff",5);
    checkColor(os,QPoint(2, 1), "#3c3cff", 5, 3);
    checkColor(os,QPoint(3, 1), "#a4a4ff");
    checkColor(os,QPoint(4, 1), "#ffffff",5);
    checkColor(os,QPoint(5, 1), "#7171ff",5);
    checkColor(os,QPoint(6, 1), "#a4a4ff",5);
    checkColor(os,QPoint(7, 2), "#a4a4ff",5);
    checkColor(os,QPoint(8, 2), "#3c3cff",5);
    checkColor(os,QPoint(9, 2), "#7171ff",5);
    checkColor(os,QPoint(10, 1), "#a4a4ff",5);
    checkColor(os,QPoint(11, 2), "#a4a4ff",5);
    checkColor(os,QPoint(12, 2), "#ffffff",5);
    checkColor(os,QPoint(13, 2), "#a4a4ff",5);
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
    GTUtilsMsaEditor::dragAndDropSequenceFromProject(os, QStringList() << "RAW.fa" << "RAW263");
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

}   // namespace
}   // namespace U2
