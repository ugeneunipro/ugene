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
#include <U2Core/DNAAlphabet.h>

#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

#include "GTTestsColorsMSAMultiline.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProject.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline_colors {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    Highlighting scheme options should be saved on the alphabet changing for an amino acid MSA

    //    1. Open "_common_data/fasta/RAW.fa".
    GTUtilsProject::openFileExpectRawSequence(testDir + "_common_data/fasta/RAW.fa", "RAW263");

    //    2. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    3. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select "Conservation level" highlighting scheme.
    GTUtilsOptionPanelMsa::setHighlightingScheme("Conservation level");

    //    5. Set the next highlighting scheme options:
    //        threshold: 70%
    //        comparison: less or equal
    //        use dots: checked.
    int expectedThreshold = 70;
    GTUtilsOptionPanelMsa::ThresholdComparison expectedThresholdComparison = GTUtilsOptionPanelMsa::LessOrEqual;
    bool expectedIsUseDotsOptionsSet = true;

    GTUtilsOptionPanelMsa::setThreshold(expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(expectedIsUseDotsOptionsSet);

    //    6. Drag and drop "RAW263" sequence object from the Project View to the MSA Editor.
    GTUtilsMsaEditor::dragAndDropSequenceFromProject({"RAW.fa", "RAW263"});
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the alignment alphabet is changed to Raw, highlighting scheme options are the same.
    const bool isAlphabetRaw = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isRaw();
    CHECK_SET_ERR(isAlphabetRaw, "Alphabet is not RAW after the symbol replacing");

    int threshold = GTUtilsOptionPanelMsa::getThreshold();
    GTUtilsOptionPanelMsa::ThresholdComparison thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison();
    bool isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet();

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

    GTUtilsOptionPanelMsa::setThreshold(expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(expectedIsUseDotsOptionsSet);

    //    7. Press "Undo" button on the toolbar.
    GTUtilsMsaEditor::undo();

    //    Expected state: the alignment alphabet is changed to Amino Acid, highlighting scheme options are the same.
    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino acid after the undoing");

    threshold = GTUtilsOptionPanelMsa::getThreshold();
    thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison();
    isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet();

    CHECK_SET_ERR(expectedThreshold == threshold,
                  QString("Threshold is incorrect: expected %1, got %2").arg(expectedThreshold).arg(threshold));
    CHECK_SET_ERR(expectedThresholdComparison == thresholdComparison,
                  QString("Threshold comparison is incorrect: expected %1, got %2").arg(expectedThresholdComparison).arg(thresholdComparison));
    CHECK_SET_ERR(expectedIsUseDotsOptionsSet == isUseDotsOptionsSet,
                  QString("Use dots option status is incorrect: expected %1, got %2").arg(expectedIsUseDotsOptionsSet).arg(isUseDotsOptionsSet));

    GTUtilsMsaEditor::setMultilineMode(false);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    Open "Color schemes" dialog.
    //    Open "Create color scheme" dialog.
    //    Set wrong scheme names: space only, empty, with forbidden
    //    characters, duplicating existing scnemes.
    //    Check error hint in dialog

    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator("GUITest_common_scenarios_msa_editor_test_0061", NewColorSchemeCreator::nucl));

    GTUtilsDialog::waitForDialog(new PopupChooser(QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                                << "Custom schemes"
                                                                << "Create new color scheme"));
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

    GTUtilsDialog::waitForDialog(new PopupChooser(QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                                << "Custom schemes"
                                                                << "Create new color scheme"));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    GTUtilsMsaEditor::setMultilineMode(false);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Highlighting scheme options should be saved on the alphabet changing for an amino acid MSA

    //    1. Open "_common_data/fasta/RAW.fa".
    GTUtilsProject::openFileExpectRawSequence(testDir + "_common_data/fasta/RAW.fa", "RAW263");

    //    2. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    3. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select "Conservation level" highlighting scheme.
    GTUtilsOptionPanelMsa::setHighlightingScheme("Conservation level");

    //    5. Set the next highlighting scheme options:
    //        threshold: 70%
    //        comparison: less or equal
    //        use dots: checked.
    int expectedThreshold = 70;
    GTUtilsOptionPanelMsa::ThresholdComparison expectedThresholdComparison = GTUtilsOptionPanelMsa::LessOrEqual;
    bool expectedIsUseDotsOptionsSet = true;

    GTUtilsOptionPanelMsa::setThreshold(expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(expectedIsUseDotsOptionsSet);

    //    6. Drag and drop "RAW263" sequence object from the Project View to the MSA Editor.
    GTUtilsMsaEditor::dragAndDropSequenceFromProject({"RAW.fa", "RAW263"});
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the alignment alphabet is changed to Raw, highlighting scheme options are the same.
    const bool isAlphabetRaw = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isRaw();
    CHECK_SET_ERR(isAlphabetRaw, "Alphabet is not RAW after the symbol replacing");

    int threshold = GTUtilsOptionPanelMsa::getThreshold();
    GTUtilsOptionPanelMsa::ThresholdComparison thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison();
    bool isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet();

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

    GTUtilsOptionPanelMsa::setThreshold(expectedThreshold);
    GTUtilsOptionPanelMsa::setThresholdComparison(expectedThresholdComparison);
    GTUtilsOptionPanelMsa::setUseDotsOption(expectedIsUseDotsOptionsSet);

    //    7. Press "Undo" button on the toolbar.
    GTUtilsMsaEditor::undo();

    //    Expected state: the alignment alphabet is changed to Amino Acid, highlighting scheme options are the same.
    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino acid after the undoing");

    threshold = GTUtilsOptionPanelMsa::getThreshold();
    thresholdComparison = GTUtilsOptionPanelMsa::getThresholdComparison();
    isUseDotsOptionsSet = GTUtilsOptionPanelMsa::isUseDotsOptionSet();

    CHECK_SET_ERR(expectedThreshold == threshold,
                  QString("Threshold is incorrect: expected %1, got %2").arg(expectedThreshold).arg(threshold));
    CHECK_SET_ERR(expectedThresholdComparison == thresholdComparison,
                  QString("Threshold comparison is incorrect: expected %1, got %2").arg(expectedThresholdComparison).arg(thresholdComparison));
    CHECK_SET_ERR(expectedIsUseDotsOptionsSet == isUseDotsOptionsSet,
                  QString("Use dots option status is incorrect: expected %1, got %2").arg(expectedIsUseDotsOptionsSet).arg(isUseDotsOptionsSet));

    GTUtilsMsaEditor::setMultilineMode(false);
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline_colors
}  // namespace U2
