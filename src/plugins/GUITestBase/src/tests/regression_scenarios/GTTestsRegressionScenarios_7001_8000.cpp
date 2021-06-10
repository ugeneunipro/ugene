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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QFileInfo>
#include <QRadioButton>

#include "GTTestsRegressionScenarios_7001_8000.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
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

    // 2. Turn on "Wrap mode" and click on the firts annotation in DetView
    QAction *wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
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
    // 1. Open data/samples/PDB/1CF7.pdb
    // 2. Check that you see 3D struct or black screen with text "Failed to initialize OpenGL"
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto biostructWidget = GTWidget::findWidget(os, "1-1CF7");
    const QImage image1 = GTWidget::getImage(os, biostructWidget, true);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }
    bool isPicture = colors.size() > 100;    // Usually 875 colors are drawn for 1CF7.pdb

    auto errorLbl = GTWidget::findLabelByText(os, "Failed to initialize OpenGL", nullptr, GTGlobals::FindOptions(false));
    bool isError = errorLbl.size() > 0;

    // There must be one thing: either a picture or an error
    CHECK_SET_ERR(isPicture != isError, "Biostruct was not drawn or error label wasn't displayed");
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
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment", GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::waitForDialog(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = QStringList() << "seqB"
                                                 << "seqA_1"
                                                 << "seqA";
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

    const QList<QLabel *> labels = GTWidget::findLabelByText(os, "- BL060C3.ace");
    CHECK_SET_ERR(labels.size() > 0, "Expected recent files BL060C3.ace on Start Page")

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
    //1. Open file _common_data/fasta/reads.fa as separate sequences.
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
    //2. Open context menu on reads.fa file in project view. Select "Export/Import -> Export sequences..."
    //3. Check the "Save both strands" radiobutton
    //4. Check the "Translate to amino" checkbox
    //5. Push Export button in the dialog.
    //6. Repeat steps 2-5 8 times
    //Expected state: UGENE is not crash
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

GUI_TEST_CLASS_DEFINITION(test_7246) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/RAW.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Check that alphabet is RAW.
    QWidget *tabWidget = GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    QString alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("Raw"), "Alphabet is not RAW/1: " + alphabet);

    // Click convert to Amino button and check the the alphabet is 'Amino'.
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

    // Click convert to DNA button and check the the alphabet is 'DNA'.
    GTWidget::click(os, GTWidget::findButtonByText(os, "DNA", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("DNA"), "Alphabet is not DNA: " + alphabet);
    sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    CHECK_SET_ERR(sequence == "TTTNNNNNNNNNNTNNNNNANNNGNNNANNNNANNNNNNNGTNNNTNGNNANNTGGANGN", "Not a DNA sequence: " + sequence);

    // Click convert to RNA button and check the the alphabet is 'RNA'.
    GTWidget::click(os, GTWidget::findButtonByText(os, "RNA", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText(os);
    CHECK_SET_ERR(alphabet.contains("RNA"), "Alphabet is not RNA: " + alphabet);
    sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    CHECK_SET_ERR(sequence == "UUUNNNNNNNNNNUNNNNNANNNGNNNANNNNANNNNNNNGUNNNUNGNNANNUGGANGN", "Not a RNA sequence: " + sequence);
}

GUI_TEST_CLASS_DEFINITION(test_7273) {
    // Open _common_data/regression/7273/web-clustalo.aln.
    // Select the symbol(1, 1).
    // Click Del.
    // Press Ctrl-Z(undo).
    //     Expected: no errors in the log.
    GTFileDialog::openFile(os, testDir + "_common_data/regression/7273", "web-clustalo.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, {0, 0});
    GTKeyboardDriver::keyPress(Qt::Key_Delete);

    GTLogTracer lt;
    GTUtilsMsaEditor::undo(os);
    QStringList errors = GTUtilsLog::getErrors(os, lt);
    int errorsNumber = errors.size();
    CHECK_SET_ERR(errorsNumber == 0, QString("Expected: no errors in the log, current: %1 errors in the log:\n%2").
        arg(errorsNumber).arg(errors.join('\n')))
}

}    // namespace GUITest_regression_scenarios

}    // namespace U2
