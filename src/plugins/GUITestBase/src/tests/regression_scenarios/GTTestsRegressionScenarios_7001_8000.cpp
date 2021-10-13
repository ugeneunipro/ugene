/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QRadioButton>

#include "GTTestsRegressionScenarios_7001_8000.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "api/GTRegionSelector.h"
#include "base_dialogs/MessageBoxFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/DNASequenceGeneratorDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_7003) {
    // 1. Ensure that 'UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY' is not set to "1"
    // 2. Open "UGENE Application Settings", select "External Tools" tab
    // 3. Add the 'dumb.sh' or 'dumb.cmd' as a Python executable
    // 4. Check that validation fails

    qputenv("UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY", "0");

    class CheckPythonInvalidation : public CustomScenario {
        void run(GUITestOpStatus &os) override {
            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            QString toolPath = testDir + "_common_data/regression/7003/dumb.";
            toolPath += isOsWindows() ? "cmd" : "sh";

            AppSettingsDialogFiller::setExternalToolPath(os, "python", QFileInfo(toolPath).absoluteFilePath());
            CHECK_SET_ERR(!AppSettingsDialogFiller::isExternalToolValid(os, "python"),
                          "Python module is expected to be invalid, but in fact it is valid")

            GTUtilsDialog::clickButtonBox(os, GTWidget::getActiveModalWidget(os), QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new CheckPythonInvalidation()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...",
                              GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_7014) {
    // The test checks 'Save subalignment' in the collapse (virtual groups) mode.
    GTFileDialog::openFile(os, testDir + "_common_data/nexus", "DQB1_exon4.nexus");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Enable collapsing.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Expand collapsed group.
    GTUtilsMsaEditor::toggleCollapsingGroup(os, "LR882519 exotic DQB1");

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(1, 1), QPoint(5, 4));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment", GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7014.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMdi::closeWindow(os, "DQB1_exon4 [DQB1_exon4.nexus]");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = QStringList() << "LR882519 exotic DQB1"
                                                 << "LR882531 local DQB1"
                                                 << "LR882507 local DQB1"
                                                 << "LR882509 local DQB1";
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));

    int msaLength = GTUtilsMSAEditorSequenceArea::getLength(os);
    CHECK_SET_ERR(msaLength == 5, "Unexpected exported alignment length: " + QString::number(msaLength));
}

GUI_TEST_CLASS_DEFINITION(test_7022) {
    // 1. Open _common_data/scenarios/_regression/7022/test_7022.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/7022/test_7022.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Turn on "Wrap mode" and click on the first annotation in DetView
    QAction *wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != nullptr, "Cannot find Wrap sequence action");
    if (!wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }
    GTUtilsSequenceView::clickAnnotationDet(os, "Misc. Feature", 2);

    // 3. copy selected annotation
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste"
                                                                              << "Copy annotation sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: TGTCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGGTTACCCACAAAGGGAAGCCCATCAGAC
    QString expected = "TGTCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGGTTACCCACAAAGGGAAGCCCATCAGAC";
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text == expected, QString("Unexpected annotation, expected: %1, current: %2").arg(expected).arg(text));
}

GUI_TEST_CLASS_DEFINITION(test_7043) {
    // Check that you see 3D struct is rendered correctly.
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto biostructWidget = GTWidget::findWidget(os, "1-1CF7");
    QImage image1 = GTWidget::getImage(os, biostructWidget, true);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }
    // Usually 875 colors are drawn for 1CF7.pdb.
    CHECK_SET_ERR(colors.size() > 100, "Biostruct was not drawn or error label wasn't displayed");

    // There must be no error message on the screen.
    QLabel *errorLabel = GTWidget::findLabel(os, "opengl_initialization_error_label", nullptr, {false});
    CHECK_SET_ERR(errorLabel == nullptr, "Found 'Failed to initialize OpenGL' label");
}

GUI_TEST_CLASS_DEFINITION(test_7044) {
    // The test checks 'Save subalignment' in the collapse (virtual groups) mode after reordering.
    GTFileDialog::openFile(os, testDir + "_common_data/nexus", "DQB1_exon4.nexus");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Enable collapsing.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Rename the last two sequences in 'seqA' and 'seqB'.
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "LR882509 local DQB1", "seqA");
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "LR882503 local DQB1", "seqB");

    // Copy seqA.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqA");
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Select first collapsed mode and 'Paste before'.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "LR882520 exotic DQB1");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Cut seqB.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqB");
    GTKeyboardDriver::keyClick('x', Qt::ControlModifier);

    // Select the first sequence and 'Paste before'
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqA_1");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Select seqB and seqA_1 (a group of seqA_1 and seqA).
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqA_1");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "seqB");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Export -> Save subalignment.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = {"seqB", "seqA_1", "seqA"};
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7045) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "COI_subalign.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Select 's1'.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "s1");

    // Copy (CTRL C) and Paste (CTRL V) -> new 's1_1' sequence appears.
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Switch collapsing mode on -> 2 collapsed groups: 's1' and' Mecopoda_elongata_Ishigaki_J' are on the screen.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Select 's1'.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "s1");

    // Call Export -> Save subalignment context menu.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment", GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state : new alignment where s1, s1_1 and s2 are present.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = QStringList() << "s1"
                                                 << "s1_1"
                                                 << "s2";
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7091) {
    // The test compares images of UGENE's main window before and after "Preferences" dialog is closed.
    QWidget *mainWindow = QApplication::activeWindow();
    QImage initialImage = GTWidget::getImage(os, mainWindow);

    // The scenario does nothing and only closes the dialog.
    class NoOpScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new NoOpScenario()));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."});

    QImage currentImage = GTWidget::getImage(os, mainWindow);
    CHECK_SET_ERR(initialImage == currentImage, "Visual appearance of the dialog should not change.");
}

GUI_TEST_CLASS_DEFINITION(test_7106) {
    // Check that in Tree-Sync mode Drag & Drop of sequences in the MSA name list is disabled.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7106");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList sequenceList1 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);

    QRect firstRowRect = GTUtilsMsaEditor::getSequenceNameRect(os, 0);
    QRect secondRowRect = GTUtilsMsaEditor::getSequenceNameRect(os, 1);

    GTMouseDriver::click(firstRowRect.center());
    GTMouseDriver::dragAndDrop(firstRowRect.center(), secondRowRect.center());

    QStringList sequenceList2 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    CHECK_SET_ERR(sequenceList2 == sequenceList1, "Sequence order must not change");
}

GUI_TEST_CLASS_DEFINITION(test_7125) {
    // Open data/samples/CLUSTALW/ty3.aln.gz
    // Press the Build Tree button on the toolbar.
    // In the "Build Phylogenetic Tree" dialog select the PhyML Maximum Likelihood method.
    // Select CpREV substitution model.
    // Press "Save Settings".
    // Cancel the dialog.
    // Open data/samples/CLUSTALW/COI.aln
    // Press the Build Tree button on the toolbar.
    // In the "Build Phylogenetic Tree" dialog select the PhyML Maximum Likelihood method.
    // Press "Build".
    //    Expected state: no crash.

    class SaveSettingsScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);

            auto currentCombobox = GTWidget::findExactWidget<QComboBox *>(os, "algorithmBox", dialog);
            GTComboBox::selectItemByText(os, currentCombobox, "PhyML Maximum Likelihood");

            currentCombobox = GTWidget::findExactWidget<QComboBox *>(os, "subModelCombo", dialog);
            GTComboBox::selectItemByText(os, currentCombobox, "CpREV");

            GTWidget::click(os, GTWidget::findButtonByText(os, "Save Settings", dialog));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new SaveSettingsScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFillerPhyML(os, false));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
}

GUI_TEST_CLASS_DEFINITION(test_7127) {
    // Make an alignment ordered by tree and check that the row order shown in the status bar is correct.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7127");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    for (int i = 0; i < 18; i++) {
        GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(0, i));
        QString rowNumber = GTMSAEditorStatusWidget::getRowNumberString(os);
        QString expectedRowNumber = QString::number(i + 1);
        CHECK_SET_ERR(rowNumber == expectedRowNumber, "Unexpected row number! Expected:  " + expectedRowNumber + ", got: " + rowNumber);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7151) {
    // Open data/samples/ACE/BL060C3.ace as MSA.
    // Close project, don't save. These steps are required for BL060C3.ace to appear in the Recent Files.
    // Click Tools->Sanger data analysis->Map reads to reference...
    // Set _common_data/sanger/reference.gb as reference.
    // Add sanger_01.ab1-sanger_20.ab1 as reads.
    // Click Map.
    // While running Sanger, click BL060C3.ace from Recent Files on Start Page.
    // Wait for the Sanger Reads Editor to appears.
    // In Select Document Format dialog click OK.
    //     Expected: no crash.

    class WaitInSelectFormatDialog : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) override {
            GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, true));
    GTFileDialog::openFileWithDialog(os, dataDir + "samples/ACE", "BL060C3.ace");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsProject::closeProject(os);

    QList<QLabel *> labels = GTWidget::findLabelByText(os, "- BL060C3.ace");
    CHECK_SET_ERR(!labels.isEmpty(), "Expected recent files BL060C3.ace on Start Page")

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 1; i <= 20; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "test_7151").absoluteFilePath();

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, {"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, new WaitInSelectFormatDialog));
    GTWidget::click(os, labels.first());
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
}

GUI_TEST_CLASS_DEFINITION(test_7152) {
    // Check that corner characters of an alignment has valid info shown in the status bar.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/region.full-gap.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(0, 0));
    QString topLeft = GTMSAEditorStatusWidget::getRowNumberString(os) + "/" +
                      GTMSAEditorStatusWidget::getColumnNumberString(os) + "/" +
                      GTMSAEditorStatusWidget::getSequenceUngappedPositionString(os);
    GTMSAEditorStatusWidget::getColumnNumberString(os);
    CHECK_SET_ERR(topLeft == "1/1/1", "Top left position is wrong: " + topLeft);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(39, 10));
    QString bottomRight = GTMSAEditorStatusWidget::getRowNumberString(os) + "/" +
                          GTMSAEditorStatusWidget::getColumnNumberString(os) + "/" +
                          GTMSAEditorStatusWidget::getSequenceUngappedPositionString(os);
    GTMSAEditorStatusWidget::getColumnNumberString(os);
    CHECK_SET_ERR(bottomRight == "11/40/35", "Bottom right position is wrong: " + bottomRight);
}

GUI_TEST_CLASS_DEFINITION(test_7183) {
    class ExportSequencesScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "bothStrandsButton", dialog));
            GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "translateButton", dialog), true);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    // 1. Open file _common_data/fasta/reads.fa as separate sequences.
    QString filePath = testDir + "_common_data/fasta/reads.fa";
    QString fileName = "reads.fa";
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, sandBoxDir + "/" + fileName);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    for (int i = 0; i < 8; i++) {
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
        GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, new ExportSequencesScenario()));
        GTUtilsProjectTreeView::click(os, "reads.fa", Qt::RightButton);
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
    // 2. Open context menu on reads.fa file in project view. Select "Export/Import -> Export sequences..."
    // 3. Check the "Save both strands" radiobutton
    // 4. Check the "Translate to amino" checkbox
    // 5. Push Export button in the dialog.
    // 6. Repeat steps 2-5 8 times
    // Expected state: UGENE is not crash
}

GUI_TEST_CLASS_DEFINITION(test_7212) {
    // Open _common_data/clustal/shortened_big.aln.
    // Click the Pairwise Alignment tab of the Options Panel.
    // Select two sequence from the original alignment and click on the Align button.
    // Until the task completes, click the Pairwise Alignment tab again.
    // Wait for task finish. A new document "PairwiseAlignmentResult.aln" has been added to the project.
    //
    // Remove PairwiseAlignmentResult.aln from project.
    // Return to shortened_big.aln and click the Pairwise Alignment tab of the Options Panel.
    // Click Align.
    //     Expected state: no crash.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/shortened_big.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsOptionPanelMsa::toggleTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "seq1");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "seq2");
    GTWidget::click(os, GTUtilsOptionPanelMsa::getAlignButton(os));
    GTUtilsOptionPanelMsa::toggleTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::removeDocument(os, "PairwiseAlignmentResult.aln");
    GTUtilsProjectTreeView::doubleClickItem(os, "shortened_big.aln");
    GTUtilsOptionPanelMsa::toggleTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTWidget::click(os, GTUtilsOptionPanelMsa::getAlignButton(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7234) {
    class InSilicoWizardScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            GTWidget::getActiveModalWidget(os);

            GTUtilsWizard::setInputFiles(os, {{QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath()}});
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsWizard::setParameter(os, "Primers URL", QFileInfo(dataDir + "primer3/drosophila.w.transposons.txt").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    // 1. Open WD and choose the "In Silico PCR" sample.
    // 2. Select "Read Sequence", add data\samples\fasta\human_T1.fa
    // 3. Select "In Silico PCR" item, add "add "\data\primer3\drosophila.w.transposons"
    // 4. Run
    // Expected state: no crash
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "In Silico PCR", new InSilicoWizardScenario()));
    GTUtilsWorkflowDesigner::addSample(os, "In Silico PCR");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7246) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/RAW.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Check that alphabet is RAW.
    QWidget *tabWidget = GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    QString alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("Raw"), "Alphabet is not RAW/1: " + alphabet);

    // Click convert to Amino button and check the alphabet is 'Amino'.
    GTWidget::click(os, GTWidget::findButtonByText(os, "Amino", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("amino"), "Alphabet is not Amino: " + alphabet);
    QString sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    CHECK_SET_ERR(sequence == "UTTSQDLQWLVXPTLIXSMAQSQGQPLASQPPAVDPYDMPGTSYSTPGLSAYSTGGASGS", "Not an Amino sequence: " + sequence);

    GTUtilsMsaEditor::undo(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("Raw"), "Alphabet is not RAW/2: " + alphabet);

    // Click convert to DNA button and check the alphabet is 'DNA'.
    GTWidget::click(os, GTWidget::findButtonByText(os, "DNA", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("DNA"), "Alphabet is not DNA: " + alphabet);
    sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    CHECK_SET_ERR(sequence == "TTTNNNNNNNNNNTNNNNNANNNGNNNANNNNANNNNNNNGTNNNTNGNNANNTGGANGN", "Not a DNA sequence: " + sequence);

    // Click convert to RNA button and check the alphabet is 'RNA'.
    GTWidget::click(os, GTWidget::findButtonByText(os, "RNA", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("RNA"), "Alphabet is not RNA: " + alphabet);
    sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    CHECK_SET_ERR(sequence == "UUUNNNNNNNNNNUNNNNNANNNGNNNANNNNANNNNNNNGUNNNUNGNNANNUGGANGN", "Not a RNA sequence: " + sequence);
}

GUI_TEST_CLASS_DEFINITION(test_7247) {
    // Open WD.
    // Load scenario sample "Remote BLASTing".
    // Add any fasta file as input.
    // Click Next, Next, Apply.
    // Close WD tab.
    // Question appears: "The workflow has been modified. Do you want to save changes?". Click Save.
    // In "Workflow properties" dialog add location. Click OK.
    //    Expected: UGENE doesn't crash.
    class RemoteBlastWizardScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus &os) override {
            GTUtilsWizard::setInputFiles(os, {{dataDir + "samples/FASTA/human_T1.fa"}});
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Apply);
        }
    };
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Remote BLASTing Wizard", new RemoteBlastWizardScenario()));
    GTUtilsWorkflowDesigner::addSample(os, "Remote BLASTing");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Save"));
    GTUtilsDialog::waitForDialog(os, new WorkflowMetaDialogFiller(os, testDir + "_common_data/scenarios/sandbox/7247.uwl", "7247"));
    GTUtilsMdi::click(os, GTGlobals::Close);
}

GUI_TEST_CLASS_DEFINITION(test_7293) {
    // Open a multi-byte unicode file that triggers format selection dialog with a raw data preview.
    // Check that raw data is shown correctly for both Open... & Open As... dialog (these are 2 different dialogs).

    class CheckDocumentReadingModeSelectorTextScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            auto textEdit = GTWidget::findExactWidget<QPlainTextEdit *>(os, "previewEdit", dialog);
            QString previewText = textEdit->toPlainText();
            CHECK_SET_ERR(previewText.contains("Первый"), "Expected text is not found in previewEdit");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, new CheckDocumentReadingModeSelectorTextScenario()));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/utf16be.fa"));
    GTMenu::clickMainMenuItem(os, {"File", "Open..."});
    GTUtilsDialog::waitAllFinished(os);

    // Now check preview text for the second dialog.
    class CheckDocumentFormatSelectorTextScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            auto textEdit = GTWidget::findExactWidget<QPlainTextEdit *>(os, "previewEdit", dialog);
            QString previewText = textEdit->toPlainText();
            CHECK_SET_ERR(previewText.contains("Первый"), "Expected text is not found in previewEdit");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, new CheckDocumentFormatSelectorTextScenario()));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/utf16be.fa"));
    GTMenu::clickMainMenuItem(os, {"File", "Open as..."});
}

#ifdef SW2_BUILD_WITH_CUDA
GUI_TEST_CLASS_DEFINITION(test_7360) {
    // Open _common_data/fasta/fa1.fa.
    // Call Smith-Waterman dialog:
    //     Pattern: A,
    //     Search in: Translation,
    //     Region: 1-1,
    //     Algorithm version: CUDA.
    // Search.
    //     Expected: no crash.

    // Call Smith-Waterman dialog:
    //     Pattern: AA,
    //     Search in: Translation,
    //     Region: Whole sequence,
    //     Algorithm version: CUDA.
    // Search.
    //     Expected: no crash.
    class SwCudaScenario : public CustomScenario {
    public:
        SwCudaScenario(const QString &pattern, bool isWholeSequence)
            : pattern(pattern), region() {
            if (!isWholeSequence) {
                region = GTRegionSelector::RegionSelectorSettings(1, 1);
            }
        }

        void run(GUITestOpStatus &os) override {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTTextEdit::setText(os, GTWidget::findExactWidget<QTextEdit *>(os, "teditPattern", dialog), pattern);
            GTRadioButton::click(os, "radioTranslation", dialog);
            GTRegionSelector::setRegion(os, GTWidget::findExactWidget<RegionSelector *>(os, "range_selector", dialog), region);
            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "comboRealization", dialog), "CUDA");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }

    private:
        QString pattern;
        GTRegionSelector::RegionSelectorSettings region;
    };
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/fa1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    const GTLogTracer logA;
    GTUtilsDialog::waitForDialog(os, new Filler(os, "SmithWatermanDialogBase", new SwCudaScenario("A", false)));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsLog::checkContainsError(os, logA, "Pattern length (1) is longer than search sequence length (0).");

    const GTLogTracer logAa;
    GTUtilsDialog::waitForDialog(os, new Filler(os, "SmithWatermanDialogBase", new SwCudaScenario("AA", true)));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsLog::checkContainsError(os, logAa, "Pattern length (2) is longer than search sequence length (1).");
}
#endif  // SW2_BUILD_WITH_CUDA

GUI_TEST_CLASS_DEFINITION(test_7367) {
    // Generate a large sequence.
    // Check that test does not time-outs and the generated sequence contains expected base distribution.

    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7367.fa");
    model.percentA = 10;
    model.percentC = 20;
    model.percentG = 30;
    model.percentT = 40;
    model.length = 100 * 1000 * 1000;

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QString sequence = GTUtilsSequenceView::getSequenceAsString(os);
    CHECK_SET_ERR(sequence.length() == model.length, "Invalid sequence length: " + QString::number(sequence.length()));
    qint64 countA = sequence.count('A');
    qint64 countC = sequence.count('C');
    qint64 countG = sequence.count('G');
    qint64 countT = sequence.count('T');
    CHECK_SET_ERR(countA + countC + countG + countT == model.length,
                  QString("A+C+G+T != sequence length, %1 != %2").arg(countA + countC + countG + countT).arg(sequence.length()));
    qint64 percentA = countA * 100 / sequence.length();
    qint64 percentC = countC * 100 / sequence.length();
    qint64 percentG = countG * 100 / sequence.length();
    qint64 percentT = countT * 100 / sequence.length();

    int diff = 2;  // Allow 2% deviation. With a such big size (100M) the distribution should be within this deviation.
    CHECK_SET_ERR(percentA >= model.percentA - diff && percentA <= model.percentA + diff, "Invalid percent of A: " + QString::number(percentA));
    CHECK_SET_ERR(percentC >= model.percentC - diff && percentC <= model.percentC + diff, "Invalid percent of C: " + QString::number(percentC));
    CHECK_SET_ERR(percentG >= model.percentG - diff && percentG <= model.percentG + diff, "Invalid percent of G: " + QString::number(percentG));
    CHECK_SET_ERR(percentT >= model.percentT - diff && percentT <= model.percentT + diff, "Invalid percent of T: " + QString::number(percentT));
}

GUI_TEST_CLASS_DEFINITION(test_7368) {
    // Generate a large sequence (>=100mb).
    // Check that error dialog is shown when such a big sequence is exported as an alignment.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7368.fa");
    model.length = 100 * 1000 * 1000;

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "too large"));
    GTUtilsProjectTreeView::callContextMenu(os, "test_7368.fa");
}

GUI_TEST_CLASS_DEFINITION(test_7380) {
    // Check that "Remove selection" is enabled when whole sequence is selected.
    GTFileDialog::openFile(os, testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B70");

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, {"Edit", "Remove selection"}, PopupChecker::IsEnabled));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);

    // Check that "Trim left end" is disabled when whole sequence is selected.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, {"Edit", "Trim left end"}, PopupChecker::IsDisabled));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
}

GUI_TEST_CLASS_DEFINITION(test_7371) {
    // Check that vertical scrollbar changes on expanding/collapsing all rows in MCA editor.
    GTFileDialog::openFile(os, testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    QScrollBar *scrollBar = GTUtilsMcaEditor::getVerticalScrollBar(os);
    CHECK_SET_ERR(scrollBar->isVisible(), "Vertical scrollbar must be visible in expanded mode (default)");

    GTUtilsMcaEditor::toggleShowChromatogramsMode(os);
    CHECK_SET_ERR(!scrollBar->isVisible(), "Vertical scrollbar must not be visible in collapsed mode");

    GTUtilsMcaEditor::toggleShowChromatogramsMode(os);
    CHECK_SET_ERR(scrollBar->isVisible(), "Vertical scrollbar must be visible in expanded mode (restored)");
}

GUI_TEST_CLASS_DEFINITION(test_7384_1) {
    // Check that multi-series graph does not crash on large sequence.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/Mycobacterium.fna");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    for (int i = 0; i < 7; i++) {
        GTUtilsSequenceView::toggleGraphByName(os, "GC Frame Plot");
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7384_2) {
    // Open graph, zoom in, and close. Do not wait until the task is finished. UGENE must not crash.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/Mycobacterium.fna");
    for (int i = 0; i < 4; i++) {
        GTUtilsSequenceView::toggleGraphByName(os, "GC Frame Plot");
        GTUtilsSequenceView::zoomIn(os);
        GTUtilsSequenceView::toggleGraphByName(os, "GC Frame Plot");
    }
}

GUI_TEST_CLASS_DEFINITION(test_7401) {
    // 1. Open human_T1.fa.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select any part of sequence.
    PanView *panView = GTUtilsSequenceView::getPanViewByNumber(os);
    auto startPoint = panView->mapToGlobal(panView->rect().center());
    auto endPoint = QPoint(startPoint.x() + 150, startPoint.y());
    GTMouseDriver::dragAndDrop(startPoint, endPoint);

    // 3. Move mouse a bit upper
    endPoint = QPoint(endPoint.x(), endPoint.y() - 20);
    GTMouseDriver::moveTo(endPoint);

    // Only one selection is presented
    auto firstSelection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(firstSelection.size() == 1, QString("Expected first selections: 1, current: %1").arg(firstSelection.size()));

    // 4. Double click and move the cursor to the right (or to the left).
    GTMouseDriver::click();
    GTMouseDriver::press();
    endPoint = QPoint(endPoint.x() + 150, endPoint.y());
    GTMouseDriver::moveTo(endPoint);
    GTMouseDriver::release();

    // Only one selection is presented, and it's been expanded to the right
    auto secondSelection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(secondSelection.size() == 1, QString("Expected second selections: 1, current: %1").arg(secondSelection.size()));

    qint64 firstSelectionEndPos = firstSelection.first().endPos();
    qint64 secondSelectionEndPos = secondSelection.first().endPos();
    CHECK_SET_ERR(firstSelectionEndPos < secondSelectionEndPos,
                  QString("The first selection end pos should be lesser than the second selection end pos: first = %1, second = %2").arg(firstSelectionEndPos).arg(secondSelectionEndPos));
}

GUI_TEST_CLASS_DEFINITION(test_7403) {
    // Check that there is no crash when generating very large (2Gb) sequences.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7403.fa");
    model.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    model.length = 2147483647;
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    // Give the generator's task some time to warm up & cancel it.
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::cancelTask(os, "Generate sequence task");
    GTUtilsTaskTreeView::waitTaskFinished(os, 20000);
}

GUI_TEST_CLASS_DEFINITION(test_7404_1) {
    // Check sequence generator can produce a sequence percents set to 100 or 0: one non-zero value.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7404_1.fa");
    model.percentA = 100;
    model.percentC = 0;
    model.percentG = 0;
    model.percentT = 0;
    model.length = 1000;

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    QString sequence = GTUtilsSequenceView::getSequenceAsString(os);
    CHECK_SET_ERR(sequence.count('A') == model.length, "Percent of A is not equal to 100%");
}

GUI_TEST_CLASS_DEFINITION(test_7404_2) {
    // Check sequence generator can produce a sequence percents set to 100 or 0: two non-zero values.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7404_1.fa");
    model.url = sandBoxDir + "/test_7404_2.fa";
    model.percentA = 50;
    model.percentC = 50;
    model.percentG = 0;
    model.percentT = 0;
    model.length = 1000;

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    QString sequence = GTUtilsSequenceView::getSequenceAsString(os);

    int countA = sequence.count('A');
    int countC = sequence.count('C');
    int countG = sequence.count('G');
    int countT = sequence.count('T');

    int deviation = 100;  // +- 100 bases (10%).

    int minValue = model.length / 2 - deviation;
    int maxValue = model.length / 2 + deviation;
    CHECK_SET_ERR(sequence.length() == model.length, "Invalid sequence length: " + QString::number(sequence.length()));
    CHECK_SET_ERR(countA >= minValue && countA <= maxValue, "Invalid count of A: " + QString::number(countA));
    CHECK_SET_ERR(countC >= minValue && countC <= maxValue, "Invalid count of C: " + QString::number(countC));
    CHECK_SET_ERR(countG == 0, "Invalid count of G: " + QString::number(countG));
    CHECK_SET_ERR(countT == 0, "Invalid count of T: " + QString::number(countT));
}

GUI_TEST_CLASS_DEFINITION(test_7405) {
    // Check that UGENE does not crash when incorrect reference sequence name is used.

    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7405.fa");
    model.referenceUrl = "/some-wrong-url";
    model.length = 100 * 1000 * 1000;

    GTLogTracer logTracer;
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString error = logTracer.getJoinedErrorString();
    CHECK_SET_ERR(error.contains(model.referenceUrl), "Expected error message is not found");
}

GUI_TEST_CLASS_DEFINITION(test_7407) {
    // Check that UGENE can generate a single character sequence.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7407.fa");
    model.length = 1;
    model.window = 1;
    model.referenceUrl = testDir + "_common_data/sanger/reference.gb";

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    QString sequence = GTUtilsSequenceView::getSequenceAsString(os);

    CHECK_SET_ERR(sequence.length() == 1, "Invalid sequence length: " + QString::number(sequence.length()));
    char c = sequence[0].toLatin1();
    CHECK_SET_ERR(c == 'A' || c == 'C' || c == 'G' || c == 'T', "Invalid sequence symbol: " + sequence[0]);
    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(test_7414) {
    // Check that 1 char window selects a random char from the origin.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7414.fa");
    model.length = 1000;
    model.window = 1;
    model.referenceUrl = testDir + "_common_data/sanger/reference.gb";

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    QString sequence = GTUtilsSequenceView::getSequenceAsString(os);

    CHECK_SET_ERR(sequence.length() == model.length, "Invalid sequence length: " + QString::number(sequence.length()));
    CHECK_SET_ERR(sequence.count('A') > 0, "No 'A' char in the result");
    CHECK_SET_ERR(sequence.count('C') > 0, "No 'C' char in the result");
    CHECK_SET_ERR(sequence.count('G') > 0, "No 'G' char in the result");
    CHECK_SET_ERR(sequence.count('T') > 0, "No 'T' char in the result");
}

GUI_TEST_CLASS_DEFINITION(test_7415_1) {
    // Check that by default Random Sequence generator uses random seed: produces different results on different runs.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7415_1_1.fa");

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString sequence1 = GTUtilsSequenceView::getSequenceAsString(os);

    model.url = sandBoxDir + "/test_7415_1_2.fa";
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString sequence2 = GTUtilsSequenceView::getSequenceAsString(os);

    CHECK_SET_ERR(sequence1.length() == model.length, "Invalid sequence1 length: " + QString::number(sequence1.length()));
    CHECK_SET_ERR(sequence2.length() == model.length, "Invalid sequence2 length: " + QString::number(sequence2.length()));
    CHECK_SET_ERR(sequence1 != sequence2, "Sequences are equal");
}

GUI_TEST_CLASS_DEFINITION(test_7415_2) {
    // Check that for given seed, Random Sequence generator produces same results on different runs.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7415_2_1.fa");
    model.seed = 0;

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString sequence1 = GTUtilsSequenceView::getSequenceAsString(os);

    model.url = sandBoxDir + "/test_7415_2_2.fa";
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString sequence2 = GTUtilsSequenceView::getSequenceAsString(os);

    CHECK_SET_ERR(sequence1.length() == model.length, "Invalid sequence1 length: " + QString::number(sequence1.length()));
    CHECK_SET_ERR(sequence2.length() == model.length, "Invalid sequence2 length: " + QString::number(sequence2.length()));
    CHECK_SET_ERR(sequence1 == sequence2, "Sequences are not equal");
}

GUI_TEST_CLASS_DEFINITION(test_7415_3) {
    // Check that by default Random Sequence generator does not generate identical windows.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7415_3.fa");
    model.seed = 10;
    model.window = 50;
    model.length = 2 * model.window;

    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString sequence = GTUtilsSequenceView::getSequenceAsString(os);

    CHECK_SET_ERR(sequence.length() == model.length, "Invalid sequence length: " + QString::number(sequence.length()));
    QString window1Sequence = sequence.mid(0, model.window);
    QString window2Sequence = sequence.mid(model.window, model.window);

    CHECK_SET_ERR(window1Sequence != window2Sequence, "Sequences are equal");
}

GUI_TEST_CLASS_DEFINITION(test_7438) {
    // Checks that selection with Shift does not cause a crash.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // There are 18 sequences in the list, and we are trying to select with SHIFT+KeyDown beyond this range.
    GTUtilsMsaEditor::clickSequence(os, 15);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    QRect selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect.top() == 15, "Illegal start of the selection: " + QString::number(selectedRect.top()));
    CHECK_SET_ERR(selectedRect.bottom() == 17, "Illegal end of the selection: " + QString::number(selectedRect.bottom()));
}

GUI_TEST_CLASS_DEFINITION(test_7447) {
    // Check that search results in MSA Editor are reset when user enters incorrect search pattern.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Enter a valid search pattern: 'ATG'
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Search);
    GTUtilsOptionPanelMsa::enterPattern(os, "ATG");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(0, 0, 3, 1),
                  QString("Illegal first result coordinates: " + GTUtils::rectToString(selectedRect)));

    // Press 'Next', move to the next result.
    GTUtilsOptionPanelMsa::clickNext(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(21, 0, 3, 1),
                  QString("Illegal second result coordinates: " + GTUtils::rectToString(selectedRect)));

    // Enter illegal 'M' character: check that there is a warning and no results in the list.
    QTextEdit *patternEdit = GTWidget::findTextEdit(os, "textPattern");
    GTWidget::click(os, patternEdit);

    GTKeyboardDriver::keyClick('M');
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString pattern = GTUtilsOptionPanelMsa::getPattern(os);
    CHECK_SET_ERR(pattern == "ATGM", "Unexpected pattern, expected: ATGM, got: " + pattern);

    auto nextButton = GTWidget::findPushButton(os, "nextPushButton");
    CHECK_SET_ERR(!nextButton->isEnabled(), "Next button must be disabled");
    GTUtilsOptionPanelMsa::checkResultsText(os, "No results");

    // Delete the last 'M' character. Check that the first 'ATG' result is selected.
    GTWidget::click(os, patternEdit);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    pattern = GTUtilsOptionPanelMsa::getPattern(os);
    CHECK_SET_ERR(pattern == "ATG", "Illegal pattern, expected: 'ATG', got: " + pattern);

    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(0, 0, 3, 1),
                  QString("Illegal first (2) result coordinates: " + GTUtils::rectToString(selectedRect)));
}

GUI_TEST_CLASS_DEFINITION(test_7448) {
    QString filePath = sandBoxDir + "test_7448.fa";
    GTFile::copy(os, dataDir + "samples/Genbank/murine.gb", filePath);

    // 1. Open "murine.gb".
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Click on any annotation.
    GTUtilsSequenceView::clickAnnotationPan(os, "misc_feature", 2);

    GTLogTracer lt;
    // 3. Click right mouse button, choose "Export -> Export sequence of selected annotations...".
    // 4. In the appeared dialog check "Translate to amino acid" and click "Export".
    GTUtilsDialog::waitForDialog(os,
        new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                      sandBoxDir + "test_7448_out.fa",
                                                      ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                      ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
                                                      0,
                                                      true,
                                                      false,
                                                      GTGlobals::UseMouse,
                                                      true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, { "Export", "Export sequence of selected annotations..." }));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there is no log message "Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries"
    lt.checkMessage("Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries");
}

GUI_TEST_CLASS_DEFINITION(test_7451) {
    // Check that a right click on a recent item on the Welcome Screen does not crash UGENE.

    // Copy the test file first to a tmp location: we will need to remove it later.
    GTFile::copy(os, dataDir + "samples/FASTA/human_T1.fa", testDir + "_common_data/scenarios/sandbox/test_7451.fa");
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/test_7451.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsMdi::closeActiveWindow(os);
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);
    GTUtilsStartPage::checkRecentListUrl(os, "test_7451.fa", true);

    // Test a right click on the Welcome Screen for a valid file.
    GTWidget::click(os, GTWidget::findLabelByText(os, "test_7451.fa").first(), Qt::RightButton);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsMdi::closeActiveWindow(os);
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);
    GTUtilsStartPage::checkRecentListUrl(os, "test_7451.fa", true);

    // Test a right click on the Welcome Screen for a removed file.
    // Close the project first to avoid 'missed file' popups.
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
    QFile::remove(testDir + "_common_data/scenarios/sandbox/test_7451.fa");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Remove From List"));
    GTWidget::click(os, GTWidget::findLabelByText(os, "test_7451.fa").first(), Qt::RightButton);

    // Check that there is no removed item in the recent files list and UGENE does not crash.
    GTUtilsStartPage::checkRecentListUrl(os, "test_7451.fa", false);
}

GUI_TEST_CLASS_DEFINITION(test_7456) {
    // Check that UGENE can open a FASTA file with a 100k small sequences as an alignment.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7456.fa");
    model.length = 5;
    model.window = 5;
    model.numberOfSequences = 100 * 1000;

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(sequenceCount == model.numberOfSequences, "Invalid sequence count in MSA: " + QString::number(sequenceCount));
}

GUI_TEST_CLASS_DEFINITION(test_7460) {
    // Check that UGENE can open an alignment of [1_000 x 10_000] and no MSA overview is shown for a such big alignment.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7460.fa");
    model.length = 1000;
    model.window = 1000;
    model.numberOfSequences = 10000;

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, { "Tools", "Random sequence generator..." });
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(sequenceCount == model.numberOfSequences, "Invalid sequence count in MSA: " + QString::number(sequenceCount));

    QWidget* overviewWidget = GTUtilsMsaEditor::getOverviewArea(os);
    CHECK_SET_ERR(overviewWidget->isHidden(), "Overview widget is visible, but must be hidden");
    GTUtilsTaskTreeView::waitTaskFinished(os, 10000);  // Check that there is no long-running active tasks.
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
