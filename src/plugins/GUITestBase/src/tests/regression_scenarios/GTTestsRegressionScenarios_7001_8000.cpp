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
#include <base_dialogs/GTFileDialog.h>
#include <cmath>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMainWindow.h>
#include <primitives/GTMenu.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTUtilsDialog.h>
#include <utils/GTUtilsText.h>
#include <utils/GTUtilsToolTip.h>

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QRadioButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/Notification.h>

#include <U2View/ADVConstants.h>
#include <U2View/TvBranchItem.h>
#include <U2View/TvNodeItem.h>
#include <U2View/TvTextItem.h>

#include "GTTestsRegressionScenarios_7001_8000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMcaEditorStatusWidget.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanelPhyTree.h"
#include "GTUtilsPcr.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsPrimerLibrary.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsQueryDesigner.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "base_dialogs/MessageBoxFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportConsensusDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/temperature/MeltingTemperatureSettingsDialogFiller.h"
#include "runnables/ugene/plugins/annotator/FindAnnotationCollocationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/DNASequenceGeneratorDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/CreateFragmentDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastLocalSearchDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/TrimmomaticDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ImportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/query/AnalyzeWithQuerySchemaDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DatasetNameEditDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/MAFFT/MAFFTSupportRunDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/sitecon/SiteconBuildDialogFiller.h"
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
        void run() override {
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            QString toolPath = testDir + "_common_data/regression/7003/dumb.";
            toolPath += isOsWindows() ? "cmd" : "sh";

            AppSettingsDialogFiller::setExternalToolPath("python", QFileInfo(toolPath).absoluteFilePath());
            CHECK_SET_ERR(!AppSettingsDialogFiller::isExternalToolValid("python"),
                          "Python module is expected to be invalid, but in fact it is valid")

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CheckPythonInvalidation()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_7012) {
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(
        new WizardFiller(
            "Extract Consensus Wizard",
            QStringList(),
            {{"Assembly", testDir + "_common_data/ugenedb/1.bam.ugenedb"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    bool hasUnexpectedLogMessage = lt.hasMessage("Ignored incorrect value of attribute");
    CHECK_SET_ERR(!hasUnexpectedLogMessage, "Found unexpected message in the log");

    // Check that output file contains only empty FASTA entries.
    QStringList fileUrls = GTUtilsDashboard::getOutputFileUrls();
    CHECK_SET_ERR(fileUrls.length() == 1, "Incorrect number of output files: " + QString::number(fileUrls.length()));
    QString fileContent = GTFile::readAll(fileUrls[0]);
    QStringList lines = fileContent.split("\n");
    for (const auto& line : qAsConst(lines)) {
        CHECK_SET_ERR(line.startsWith(">") || line.isEmpty(), "Only FASTA header lines are expected: " + line);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7014) {
    // The test checks 'Save subalignment' in the collapse (virtual groups) mode.
    GTFileDialog::openFile(testDir + "_common_data/nexus", "DQB1_exon4.nexus");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Enable collapsing.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expand collapsed group.
    GTUtilsMsaEditor::toggleCollapsingGroup("LR882519 exotic DQB1");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(5, 4));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(sandBoxDir + "test_7014.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::closeWindow("DQB1_exon4 [DQB1_exon4.nexus]");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    QStringList expectedNameList = {"LR882519 exotic DQB1", "LR882531 local DQB1", "LR882507 local DQB1", "LR882509 local DQB1"};
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));

    int msaLength = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(msaLength == 5, "Unexpected exported alignment length: " + QString::number(msaLength));
}

GUI_TEST_CLASS_DEFINITION(test_7022) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/7022/test_7022.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Turn on "Wrap mode" and click on the first annotation in DetView.
    QAction* wrapMode = GTAction::findActionByText("Wrap sequence");
    if (!wrapMode->isChecked()) {
        GTWidget::click(GTAction::button(wrapMode));
    }
    GTUtilsSequenceView::clickAnnotationDet("Misc. Feature", 2);

    // Copy selected annotation.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy annotation sequence"}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();

    QString expected = "TGTCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGGTTACCCACAAAGGGAAGCCCATCAGAC";
    QString text = GTClipboard::text();
    CHECK_SET_ERR(text == expected, QString("Unexpected annotation, expected: %1, current: %2").arg(expected).arg(text));
}

GUI_TEST_CLASS_DEFINITION(test_7043) {
    // Check that you see 3D struct is rendered correctly.
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto biostructWidget = GTWidget::findWidget("1-1CF7");
    QImage image1 = GTWidget::getImage(biostructWidget);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }

    // Usually 875 colors are drawn for 1CF7.pdb.
    CHECK_SET_ERR(colors.size() > 100, "Biostruct was not drawn or error label wasn't displayed, number of colors: " + QString::number(colors.size()));

    // There must be no error message on the screen.
    auto errorLabel = GTWidget::findLabel("opengl_initialization_error_label", nullptr, {false});
    CHECK_SET_ERR(errorLabel == nullptr, "Found 'Failed to initialize OpenGL' label");
}

GUI_TEST_CLASS_DEFINITION(test_7044) {
    // The test checks 'Save subalignment' in the collapse (virtual groups) mode after reordering.
    GTFileDialog::openFile(testDir + "_common_data/nexus", "DQB1_exon4.nexus");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Enable collapsing.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Rename the last two sequences in 'seqA' and 'seqB'.
    GTUtilsMSAEditorSequenceArea::renameSequence("LR882509 local DQB1", "seqA");
    GTUtilsMSAEditorSequenceArea::renameSequence("LR882503 local DQB1", "seqB");

    // Copy seqA.
    GTUtilsMSAEditorSequenceArea::selectSequence("seqA");
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Select first collapsed mode and 'Paste before'.
    GTUtilsMSAEditorSequenceArea::selectSequence("LR882520 exotic DQB1");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Cut seqB.
    GTUtilsMSAEditorSequenceArea::selectSequence("seqB");
    GTKeyboardDriver::keyClick('x', Qt::ControlModifier);

    // Select the first sequence and 'Paste before'
    GTUtilsMSAEditorSequenceArea::selectSequence("seqA_1");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Select seqB and seqA_1 (a group of seqA_1 and seqA).
    GTUtilsMSAEditorSequenceArea::selectSequence("seqA_1");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::selectSequence("seqB");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Export -> Save subalignment.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    QStringList expectedNameList = {"seqB", "seqA_1", "seqA"};
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7045) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "COI_subalign.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select 's1'.
    GTUtilsMSAEditorSequenceArea::selectSequence("s1");

    // Copy (CTRL C) and Paste (CTRL V) -> new 's1_1' sequence appears.
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Switch collapsing mode on -> 2 collapsed groups: 's1' and' Mecopoda_elongata_Ishigaki_J' are on the screen.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Select 's1'.
    GTUtilsMSAEditorSequenceArea::selectSequence("s1");

    // Call Export -> Save subalignment context menu.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : new alignment where s1, s1_1 and s2 are present.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    QStringList expectedNameList = {"s1", "s1_1", "s2"};
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7091) {
    // The test compares images of UGENE's main window before and after "Preferences" dialog is closed.
    QWidget* mainWindow = QApplication::activeWindow();
    QImage initialImage = GTWidget::getImage(mainWindow);

    // The scenario does nothing and only closes the dialog.
    class NoOpScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new NoOpScenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    QImage currentImage = GTWidget::getImage(mainWindow);
    CHECK_SET_ERR(initialImage == currentImage, "Visual appearance of the dialog should not change.");
}

GUI_TEST_CLASS_DEFINITION(test_7106) {
    // Check that in Tree-Sync mode Drag & Drop of sequences in the MSA name list is disabled.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_7106");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList sequenceList1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    QRect firstRowRect = GTUtilsMsaEditor::getSequenceNameRect(0);
    QRect secondRowRect = GTUtilsMsaEditor::getSequenceNameRect(1);

    GTMouseDriver::click(firstRowRect.center());
    GTMouseDriver::dragAndDrop(firstRowRect.center(), secondRowRect.center());

    QStringList sequenceList2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
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
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto currentCombobox = GTWidget::findComboBox("algorithmBox", dialog);
            GTComboBox::selectItemByText(currentCombobox, "PhyML Maximum Likelihood");

            currentCombobox = GTWidget::findComboBox("subModelCombo", dialog);
            GTComboBox::selectItemByText(currentCombobox, "CpREV");

            GTWidget::click(GTWidget::findButtonByText("Save Settings", dialog));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new SaveSettingsScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFillerPhyML(false));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
}

GUI_TEST_CLASS_DEFINITION(test_7126) {
    // Check that MSA re-ordered by tree is copied to clipboard using the visual row order.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_7127");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::selectRows(0, 17);

    GTUtilsDialog::add(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList lines = GTClipboard::text().split("\n");
    CHECK_SET_ERR(lines[0].startsWith("CLUSTAL W"), "Unexpected line 0: " + lines[0]);
    CHECK_SET_ERR(lines[1].trimmed().isEmpty(), "Unexpected line 1: " + lines[1]);
    CHECK_SET_ERR(lines[2].startsWith("Isophya_altaica_EF540820"), "Unexpected line 2: " + lines[2]);
    CHECK_SET_ERR(lines[3].startsWith("Bicolorana_bicolor_EF540830"), "Unexpected line 3: " + lines[3]);
    CHECK_SET_ERR(lines[4].startsWith("Roeseliana_roeseli"), "Unexpected lines 4: " + lines[4]);
    CHECK_SET_ERR(lines[5].startsWith("Montana_montana"), "Unexpected lines 5: " + lines[5]);
}

GUI_TEST_CLASS_DEFINITION(test_7127) {
    // Make an alignment ordered by tree and check that the row order shown in the status bar is correct.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_7127");
    GTUtilsTaskTreeView::waitTaskFinished();

    for (int i = 0; i < 18; i++) {
        GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(0, i));
        QString rowNumber = GTMSAEditorStatusWidget::getRowNumberString();
        QString expectedRowNumber = QString::number(i + 1);
        CHECK_SET_ERR(rowNumber == expectedRowNumber, "Unexpected row number! Expected:  " + expectedRowNumber + ", got: " + rowNumber);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7128) {
    // Copy mafft folder in sandbox_dir.
    // Open UGENE, open Preferences...->External Tools.
    // Set MAFFT path as "sandbox_dir/mafft/mafft.bat", wait for validating, click OK.
    // Remove "sandbox_dir/mafft/".
    // Open COI.aln and Align with MAFFT.
    // Expected: the log contains errors like "file "sandbox_dir/mafft/mafft.bat" doesn't exist".

    QString mafftDirToRemove = sandBoxDir + "mafft";
    QString mafftPathToRemove = mafftDirToRemove + "/mafft.bat";

    class SetMafft : public CustomScenario {
    public:
        SetMafft(const QString& mafftDir, const QString& mafftPath)
            : mafftDir(mafftDir), mafftPath(mafftPath) {
        }
        void run() override {
            QString toolPath = AppSettingsDialogFiller::getExternalToolPath("MAFFT");
            GTFile::copyDir(toolPath.remove("mafft.bat"), mafftDir);
            AppSettingsDialogFiller::setExternalToolPath("MAFFT", QFileInfo(mafftPath).absoluteFilePath());
            GTUtilsTaskTreeView::waitTaskFinished();

            toolPath = AppSettingsDialogFiller::getExternalToolPath("MAFFT");
            bool isValid = AppSettingsDialogFiller::isExternalToolValid("MAFFT");
            CHECK_SET_ERR(isValid, QString("MAFFT with path '%1' is expected to be valid, but in fact it is invalid").arg(toolPath));
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }

    private:
        QString mafftDir;
        QString mafftPath;
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new SetMafft(mafftDirToRemove, mafftPathToRemove)));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTLogTracer lt;
    GTFile::removeDir(mafftDirToRemove);
    GTUtilsDialog::waitForDialog(new MAFFTSupportRunDialogFiller(new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with MAFFT"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    QString expectedError = QString("External tool '%1' doesn't exist").arg(QFileInfo(mafftPathToRemove).absoluteFilePath());
    CHECK_SET_ERR(lt.hasError(expectedError), "Expected error is not found");
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

    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTFileDialog::openFileWithDialog(dataDir + "samples/ACE", "BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::closeProject(true);
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QLabel*> labels = GTWidget::findLabelByText("- BL060C3.ace");

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 1; i <= 20; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "test_7151").absoluteFilePath();

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    class WaitInSelectFormatDialog : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsMcaEditor::checkMcaEditorWindowIsActive();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(new WaitInSelectFormatDialog()));
    GTWidget::click(labels.first());
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_7152) {
    // Check that corner characters of an alignment has valid info shown in the status bar.
    GTFileDialog::openFile(testDir + "_common_data/clustal/region.full-gap.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(0, 0));
    QString topLeft = GTMSAEditorStatusWidget::getRowNumberString() + "/" +
                      GTMSAEditorStatusWidget::getColumnNumberString() + "/" +
                      GTMSAEditorStatusWidget::getSequenceUngappedPositionString();
    GTMSAEditorStatusWidget::getColumnNumberString();
    CHECK_SET_ERR(topLeft == "1/1/1", "Top left position is wrong: " + topLeft);

    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(39, 10));
    QString bottomRight = GTMSAEditorStatusWidget::getRowNumberString() + "/" +
                          GTMSAEditorStatusWidget::getColumnNumberString() + "/" +
                          GTMSAEditorStatusWidget::getSequenceUngappedPositionString();
    GTMSAEditorStatusWidget::getColumnNumberString();
    CHECK_SET_ERR(bottomRight == "11/40/35", "Bottom right position is wrong: " + bottomRight);
}

GUI_TEST_CLASS_DEFINITION(test_7154) {
    // 1. Open "_common_data/genbank/Smc3_LOCUS_19_45436_bp_DNA_HTG_4_changed.gbk".
    GTFileDialog::openFile(testDir + "_common_data/genbank/Smc3_LOCUS_19_45436_bp_DNA_HTG_4_changed.gbk");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Create annotation #1
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "grpA", "annA", "complement(10.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // 3. Create annotations #2
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "grpB", "annB", "complement(30.. 40)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // 7. Drag&drop annotation #1 to group #2
    QTreeWidgetItem* annA = GTUtilsAnnotationsTreeView::findItem("annA");
    QTreeWidgetItem* annB = GTUtilsAnnotationsTreeView::findItem("annB");
    QTreeWidgetItem* grpA = annA->parent();
    QTreeWidgetItem* grpB = annB->parent();
    QPoint pointA = GTUtilsAnnotationsTreeView::getItemCenter("annA");
    QPoint pointGrpB = GTTreeWidget::getItemCenter(grpB);
    GTMouseDriver::dragAndDrop(pointA, pointGrpB);

    // 8. Drag&drop group #1 to group #2
    QPoint pointGrpA = GTTreeWidget::getItemCenter(grpA);
    pointGrpA = GTTreeWidget::getItemCenter(grpA);
    pointGrpB = GTTreeWidget::getItemCenter(grpB);
    GTMouseDriver::dragAndDrop(pointGrpA, pointGrpB);

    // Expected: group moved successfully, no crash
    GTGlobals::FindOptions findOpt(false, Qt::MatchContains);
    QTreeWidgetItem* itemGrpA = GTUtilsAnnotationsTreeView::findItem("grpA", nullptr, findOpt);
    CHECK_SET_ERR(itemGrpA != nullptr, QString("Can't find item grpA"));
    QTreeWidgetItem* parentGrpA = itemGrpA->parent();
    CHECK_SET_ERR(parentGrpA != nullptr, QString("Parent of the grpA was not found"));
    annA = GTUtilsAnnotationsTreeView::findItem("annA");
    annB = GTUtilsAnnotationsTreeView::findItem("annB");
    grpA = annA->parent();
    grpB = annB->parent();
    CHECK_SET_ERR(grpA == grpB && grpA == parentGrpA,
                  QString("Parent of the grpA, annA, annB must be the same"));
}

GUI_TEST_CLASS_DEFINITION(test_7161) {
    class ItemPopupChooserByPosition : public PopupChooser {
        // for some reason PopupChooser does not work properly, so we choose item by position
    public:
        ItemPopupChooserByPosition(int _pos)
            : PopupChooser({}), pos(_pos) {
        }

        void run() override {
            for (int i = 0; i < pos; i++) {
                GTKeyboardDriver::keyClick(Qt::Key_Down);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }

    private:
        int pos;
    };

    class ChooseCDSAndCommentsWithin60kRegion : public FindAnnotationCollocationsDialogFiller {
    public:
        ChooseCDSAndCommentsWithin60kRegion()
            : FindAnnotationCollocationsDialogFiller() {
        }

        void run() override {
            QToolButton* plusButton = getPlusButton();

            GTUtilsDialog::waitForDialog(new ItemPopupChooserByPosition(3));
            GTWidget::click(plusButton);

            GTUtilsDialog::waitForDialog(new ItemPopupChooserByPosition(3));
            GTWidget::click(plusButton);

            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTSpinBox::setValue("regionSpin", 60000, GTGlobals::UseKeyBoard, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    // 1. Open data/samples/sars.gb
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use context menu: {Analyze -> Find annotated regions}
    // 3. Click plus button, select "comment", repeat and select "cds"
    // 4. Set "Region size" to 60000
    // 5. Press "Search button"
    // Expected state: no crash or assert on run
    auto* toolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    auto* farButton = GTToolbar::getWidgetForActionTooltip(toolbar, "Find annotated regions");

    GTUtilsDialog::waitForDialog(new ChooseCDSAndCommentsWithin60kRegion());
    GTWidget::click(farButton);
}

GUI_TEST_CLASS_DEFINITION(test_7183) {
    class ExportSequencesScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTRadioButton::click(GTWidget::findRadioButton("bothStrandsButton", dialog));
            GTCheckBox::setChecked(GTWidget::findCheckBox("translateButton", dialog), true);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    // 1. Open file _common_data/fasta/reads.fa as separate sequences.
    QString filePath = testDir + "_common_data/fasta/reads.fa";
    QString fileName = "reads.fa";
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(sandBoxDir + "/" + fileName);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    for (int i = 0; i < 8; i++) {
        GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
        GTUtilsDialog::add(new ExportSelectedRegionFiller(new ExportSequencesScenario()));
        GTUtilsProjectTreeView::click("reads.fa", Qt::RightButton);
        GTUtilsTaskTreeView::waitTaskFinished();
    }
    // 2. Open context menu on reads.fa file in project view. Select "Export/Import -> Export sequences..."
    // 3. Check the "Save both strands" radiobutton
    // 4. Check the "Translate to amino" checkbox
    // 5. Push Export button in the dialog.
    // 6. Repeat steps 2-5 8 times
    // Expected state: UGENE is not crash
}

GUI_TEST_CLASS_DEFINITION(test_7191) {
    /*
     * 1. Open data/samples/sars.gb
     * 2. Delete sequence object
     * 3. Export annotation object
     * Expected state: there is no errors in the log
     */
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::click("NC_004718");

    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "test_7191.gb", ExportAnnotationsFiller::ugenedb));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export/Import", "Export annotations..."}));
    GTUtilsProjectTreeView::callContextMenu("NC_004718 features");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7193) {
    GTUtilsPcr::clearPcrDir();
    // 1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "GGAAAAAATGCTAAGGGC" and "CTGGGTTGAAAATTCTTT".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "GGAAAAAATGCTAAGGGC");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "CTGGGTTGAAAATTCTTT");
    // 4. Set both mismatches to 9
    GTUtilsPcr::setMismatches(U2Strand::Direct, 9);
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 9);
    // 5. Set 3' perfect match to 3
    auto perfectSpinBox = GTWidget::findSpinBox("perfectSpinBox");
    GTSpinBox::setValue(perfectSpinBox, 3, GTGlobals::UseKeyBoard);

    // 6. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsPcr::productsCount() == 22, QString("Expected 22 result instead of %1").arg(QString::number(GTUtilsPcr::productsCount())));
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
    GTFileDialog::openFile(testDir + "_common_data/clustal/shortened_big.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA("seq1");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("seq2");
    QString documentName = GTUtils::genUniqueString("PairwiseAlignmentResult");
    GTUtilsOptionPanelMsa::setOutputFile(sandBoxDir + documentName + ".aln");

    GTWidget::click(GTUtilsOptionPanelMsa::getAlignButton());
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::removeDocument(documentName);
    GTUtilsProjectTreeView::doubleClickItem("shortened_big.aln");
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTWidget::click(GTUtilsOptionPanelMsa::getAlignButton());
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7234) {
    class InSilicoWizardScenario : public CustomScenario {
    public:
        void run() override {
            GTWidget::getActiveModalWidget();

            GTUtilsWizard::setInputFiles({{QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath()}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::setParameter("Primers URL", QFileInfo(dataDir + "primer3/drosophila.w.transposons.txt").absoluteFilePath());

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    // 1. Open WD and choose the "In Silico PCR" sample.
    // 2. Select "Read Sequence", add data\samples\fasta\human_T1.fa
    // 3. Select "In Silico PCR" item, add "add "\data\primer3\drosophila.w.transposons"
    // 4. Run
    // Expected state: no crash
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new InSilicoWizardScenario()));
    GTUtilsWorkflowDesigner::addSample("In Silico PCR");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7246) {
    GTFileDialog::openFile(testDir + "_common_data/clustal/RAW.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Check that alphabet is RAW.
    QWidget* tabWidget = GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    QString alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText();
    CHECK_SET_ERR(alphabet.contains("Raw"), "Alphabet is not RAW/1: " + alphabet);

    // Click convert to Amino button and check the alphabet is 'Amino'.
    GTWidget::click(GTWidget::findButtonByText("Amino", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished();
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText();
    CHECK_SET_ERR(alphabet.contains("amino"), "Alphabet is not Amino: " + alphabet);
    QString sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(0);
    CHECK_SET_ERR(sequence == "UTTSQDLQWLVXPTLIXSMAQSQGQPLASQPPAVDPYDMPGTSYSTPGLSAYSTGGASGS", "Not an Amino sequence: " + sequence);

    GTUtilsMsaEditor::undo();
    GTUtilsTaskTreeView::waitTaskFinished();
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText();
    CHECK_SET_ERR(alphabet.contains("Raw"), "Alphabet is not RAW/2: " + alphabet);

    // Click convert to DNA button and check the alphabet is 'DNA'.
    GTWidget::click(GTWidget::findButtonByText("DNA", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished();
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText();
    CHECK_SET_ERR(alphabet.contains("DNA"), "Alphabet is not DNA: " + alphabet);
    sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(0);
    CHECK_SET_ERR(sequence == "TTTNNNNNNNNNNTNNNNNANNNGNNNANNNNANNNNNNNGTNNNTNGNNANNTGGANGN", "Not a DNA sequence: " + sequence);

    // Click convert to RNA button and check the alphabet is 'RNA'.
    GTWidget::click(GTWidget::findButtonByText("RNA", tabWidget));
    GTUtilsTaskTreeView::waitTaskFinished();
    alphabet = GTUtilsOptionPanelMsa::getAlphabetLabelText();
    CHECK_SET_ERR(alphabet.contains("RNA"), "Alphabet is not RNA: " + alphabet);
    sequence = GTUtilsMSAEditorSequenceArea::getSequenceData(0);
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
        void run() override {
            GTUtilsWizard::setInputFiles({{dataDir + "samples/FASTA/human_T1.fa"}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::add(new WizardFiller("Remote BLASTing Wizard", new RemoteBlastWizardScenario()));
    GTUtilsWorkflowDesigner::addSample("Remote BLASTing");
    GTUtilsDialog::add(new MessageBoxDialogFiller("Save"));
    GTUtilsDialog::add(new WorkflowMetaDialogFiller(testDir + "_common_data/scenarios/sandbox/7247.uwl", "7247"));
    GTUtilsMdi::click(GTGlobals::Close);
}

GUI_TEST_CLASS_DEFINITION(test_7276) {
    // Check that selection and sequence order does not change after KAlign alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Insert space into some sequence to check that the final alignment algorithm is not no-op.
    GTUtilsMSAEditorSequenceArea::clickToPosition({0, 0});
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString unalignedSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(0);
    QStringList sequenceNameList1 = GTUtilsMSAEditorSequenceArea::getNameList();

    QString sequenceName = "Phaneroptera_falcata";
    GTUtilsMsaEditor::clickSequenceName(sequenceName);
    GTUtilsMsaEditor::checkSelectionByNames({sequenceName});

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Sort", "By name"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that the sequence is still selected, but the list is re-ordered.
    GTUtilsMsaEditor::checkSelectionByNames({sequenceName});
    QStringList sequenceNameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(sequenceNameList2 != sequenceNameList1, "Name list must change as the result of sorting");

    // Align with KAlign now.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTUtilsDialog::add(new KalignDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that sequence is still selected and the list is not re-ordered.
    GTUtilsMsaEditor::checkSelectionByNames({sequenceName});
    QStringList sequenceNameList3 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(sequenceNameList3 == sequenceNameList2, "Name list should not change as the result of alignment");

    // Check that the space inserted before was fixed by the alignment algorithm.
    int newIndex = sequenceNameList2.indexOf(sequenceName);
    QString alignedSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(newIndex);
    CHECK_SET_ERR(alignedSequence.left(20) == unalignedSequence.mid(1).left(20), "Aligned sequence must match the original sequence");
}

GUI_TEST_CLASS_DEFINITION(test_7279) {
    // Check that UGENE prints a detailed error message in case if input parameters are invalid.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("test_7279.nwk", 2, 99.99));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.getJoinedErrorString().contains("Failed to compute distance matrix: distance matrix contains infinite values"),
                  "Expected error message is not found");
}

GUI_TEST_CLASS_DEFINITION(test_7293) {
    // Open a multi-byte unicode file that triggers format selection dialog with a raw data preview.
    // Check that raw data is shown correctly for both Open... & Open As... dialog (these are 2 different dialogs).

    class CheckDocumentReadingModeSelectorTextScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto textEdit = GTWidget::findPlainTextEdit("previewEdit", dialog);
            QString previewText = textEdit->toPlainText();
            CHECK_SET_ERR(previewText.contains("Первый"), "Expected text is not found in previewEdit");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(new CheckDocumentReadingModeSelectorTextScenario()));
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/utf16be.fa"));
    GTMenu::clickMainMenuItem({"File", "Open..."});
    GTUtilsDialog::checkNoActiveWaiters();

    // Now check preview text for the second dialog.
    class CheckDocumentFormatSelectorTextScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto textEdit = GTWidget::findPlainTextEdit("previewEdit", dialog);
            QString previewText = textEdit->toPlainText();
            CHECK_SET_ERR(previewText.contains("Первый"), "Expected text is not found in previewEdit");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller(new CheckDocumentFormatSelectorTextScenario()));
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/utf16be.fa"));
    GTMenu::clickMainMenuItem({"File", "Open as..."});
}

GUI_TEST_CLASS_DEFINITION(test_7338) {
    // 1. Open and import _common_data/bam/NoAssemblies.bam, with "Import empty reads" checked.
    // 2. Close project
    // 3. Repeat step 1
    // Expected state: no crash
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "test_7338_1.ugenedb", "", "", true));
    GTFileDialog::openFile(testDir + "_common_data/bam/NoAssemblies.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "test_7338_2.ugenedb", "", "", true));
    GTFileDialog::openFile(testDir + "_common_data/bam/NoAssemblies.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7367) {
    // Generate a large sequence.
    // Check that test does not time-outs and the generated sequence contains expected base distribution.

    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7367.fa");
    model.percentA = 10;
    model.percentC = 20;
    model.percentG = 30;
    model.percentT = 40;
    model.length = 100 * 1000 * 1000;

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QString sequence = GTUtilsSequenceView::getSequenceAsString();
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

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "too large"));
    GTUtilsProjectTreeView::callContextMenu("test_7368.fa");
}

GUI_TEST_CLASS_DEFINITION(test_7380) {
    // Check that "Remove selection" is enabled when whole sequence is selected.
    GTFileDialog::openFile(testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B70");

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Edit", "Remove selection"}, PopupChecker::IsEnabled));
    GTUtilsMcaEditorSequenceArea::callContextMenu();

    // Check that "Trim left end" is disabled when whole sequence is selected.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Edit", "Trim left end"}, PopupChecker::IsDisabled));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
}

GUI_TEST_CLASS_DEFINITION(test_7371) {
    // Check that vertical scrollbar changes on expanding/collapsing all rows in MCA editor.
    GTFileDialog::openFile(testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();

    QScrollBar* scrollBar = GTUtilsMcaEditor::getVerticalScrollBar();
    CHECK_SET_ERR(scrollBar->isVisible(), "Vertical scrollbar must be visible in expanded mode (default)");

    GTUtilsMcaEditor::toggleShowChromatogramsMode();
    CHECK_SET_ERR(!scrollBar->isVisible(), "Vertical scrollbar must not be visible in collapsed mode");

    GTUtilsMcaEditor::toggleShowChromatogramsMode();
    CHECK_SET_ERR(scrollBar->isVisible(), "Vertical scrollbar must be visible in expanded mode (restored)");
}

GUI_TEST_CLASS_DEFINITION(test_7384_1) {
    // Check that multi-series graph does not crash on large sequence.
    GTFileDialog::openFile(testDir + "_common_data/fasta/Mycobacterium.fna");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    for (int i = 0; i < 7; i++) {
        GTUtilsSequenceView::toggleGraphByName("GC Frame Plot");
        GTUtilsTaskTreeView::waitTaskFinished();
    }
}

GUI_TEST_CLASS_DEFINITION(test_7384_2) {
    // Open graph, zoom in, and close. Do not wait until the task is finished. UGENE must not crash.
    GTFileDialog::openFile(testDir + "_common_data/fasta/Mycobacterium.fna");
    for (int i = 0; i < 4; i++) {
        GTUtilsSequenceView::toggleGraphByName("GC Frame Plot");
        GTUtilsSequenceView::zoomIn();
        GTUtilsSequenceView::toggleGraphByName("GC Frame Plot");
    }
}

GUI_TEST_CLASS_DEFINITION(test_7388) {
    GTFileDialog::openFile(testDir + "_common_data/clustal/align_subalign.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtils::checkExportServiceIsEnabled();

    // Export subalignment with only gaps inside.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));

    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(sandBoxDir + "test_7388.aln", {"s1", "s2"}, 16, 24);
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select both sequences with only gaps inside.
    GTUtilsMdi::checkWindowIsActive("test_7388");
    GTUtilsMsaEditor::selectRows(0, 1);

    // Check that "Copy" works as expected.
    GTUtilsDialog::add(new PopupChooserByText({"Copy/Paste", "Copy"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    QString clipboardText1 = GTClipboard::text();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(clipboardText1 == "---------\n---------",
                  "1. Unexpected clipboard text: " + clipboardText1);

    // Check that "Copy (custom format)" works as expected.
    GTUtilsDialog::add(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
    QString clipboardText2 = GTClipboard::text();
    CHECK_SET_ERR(clipboardText2 == "CLUSTAL W 2.0 multiple sequence alignment\n\ns1   --------- 9\ns2   --------- 9\n              \n\n",
                  "2. Unexpected clipboard text: " + clipboardText2);
}

GUI_TEST_CLASS_DEFINITION(test_7390) {
    // 1. Set SPAdes to any file
    // Expected: SPAdes is invalid
    class SetSpades : public CustomScenario {
        void run() override {
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            QString toolPath = dataDir + "samples/FASTA/human_T1.fa";

            AppSettingsDialogFiller::setExternalToolPath("SPAdes", QFileInfo(toolPath).absoluteFilePath());
            CHECK_SET_ERR(!AppSettingsDialogFiller::isExternalToolValid("SPAdes"),
                          "SPAdes is expected to be invalid, but in fact it is valid");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new SetSpades()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);

    // 2. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    using TrimmomaticAddSettings = QPair<TrimmomaticDialogFiller::TrimmomaticSteps, QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant>>;
    QList<TrimmomaticAddSettings> steps;
    steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::ILLUMINACLIP, {}));

    // 3. Open the "De novo assemble Illumina SE reads" sample
    // 4. Set "human_T1.fa" as input
    // 5. Click "Next"
    // 6. Set the "ILLUMINACLIP" Trimmomatic step
    // 7. Click "Next"
    // 8. Click "Next"
    // 9. Click "Apply"
    class ProcessWizard : public CustomScenario {
    public:
        void run() override {
            //    Expected state: wizard has appeared.
            QWidget* wizard = GTWidget::getActiveModalWidget();
            GTWidget::clickWindowTitle(wizard);

            GTUtilsWizard::setInputFiles({{dataDir + "samples/FASTA/human_T1.fa"}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTWidget::click(GTWidget::findToolButton("trimmomaticPropertyToolButton", wizard));
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };

    GTUtilsDialog::waitForDialog(new TrimmomaticDialogFiller(steps));
    GTUtilsDialog::waitForDialog(new WizardFiller("Illumina SE Reads De Novo Assembly Wizard", new ProcessWizard));
    GTUtilsWorkflowDesigner::addSample("De novo assemble Illumina SE reads");

    // 10. Validate workflow
    // Expected: no crash
    GTUtilsWorkflowDesigner::validateWorkflow();
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

GUI_TEST_CLASS_DEFINITION(test_7401) {
    // 1. Open human_T1.fa.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select any part of sequence.
    PanView* panView = GTUtilsSequenceView::getPanViewByNumber();
    auto startPoint = panView->mapToGlobal(panView->rect().center());
    auto endPoint = QPoint(startPoint.x() + 150, startPoint.y());
    GTMouseDriver::dragAndDrop(startPoint, endPoint);

    // 3. Move mouse a bit upper
    endPoint = QPoint(endPoint.x(), endPoint.y() - 20);
    GTMouseDriver::moveTo(endPoint);

    // Only one selection is presented
    auto firstSelection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(firstSelection.size() == 1, QString("Expected first selections: 1, current: %1").arg(firstSelection.size()));

    // 4. Double click and move the cursor to the right (or to the left).
    GTMouseDriver::click();
    GTMouseDriver::press();
    endPoint = QPoint(endPoint.x() + 150, endPoint.y());
    GTMouseDriver::moveTo(endPoint);
    GTMouseDriver::release();

    // Only one selection is presented, and it's been expanded to the right
    auto secondSelection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(secondSelection.size() == 1, QString("Expected second selections: 1, current: %1").arg(secondSelection.size()));

    qint64 firstSelectionEndPos = firstSelection.first().endPos();
    qint64 secondSelectionEndPos = secondSelection.first().endPos();
    CHECK_SET_ERR(firstSelectionEndPos < secondSelectionEndPos,
                  QString("The first selection end pos should be lesser than the second selection end pos: first = %1, second = %2").arg(firstSelectionEndPos).arg(secondSelectionEndPos));
}

GUI_TEST_CLASS_DEFINITION(test_7402) {
    // Check that MSA Toolbar→Actions→Export→Export-Selected-Sequences action is present and works as expected.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/clustal/protein.fasta.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Check that menu is present and disabled.
    GTMenu::checkMainMenuItemState({"Actions", "Export", "Move selected rows to another alignment"}, PopupChecker::IsDisabled);

    // Select a row, check that menu is enabled, export the selected row.
    GTUtilsMsaEditor::selectRowsByName({"Whale"});
    GTMenu::clickMainMenuItem({"Actions", "Export", "Move selected rows to another alignment", "COI [COI.aln]"});
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList sourceNameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(!sourceNameList.contains("Whale"), "Row was not removed from the source MSA");

    GTUtilsMdi::activateWindow("COI.aln");
    QStringList targetNameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(targetNameList.contains("Whale"), "Row was not added to the target MSA");
}

GUI_TEST_CLASS_DEFINITION(test_7403) {
    // Check that there is no crash when generating very large (2Gb) sequences.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7403.fa");
    model.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    model.length = 2147483647;
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    // Give the generator's task some time to warm up & cancel it.
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::cancelTask("Generate sequence task");
    GTUtilsTaskTreeView::waitTaskFinished(20000);
}

GUI_TEST_CLASS_DEFINITION(test_7404_1) {
    // Check sequence generator can produce a sequence percents set to 100 or 0: one non-zero value.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7404_1.fa");
    model.percentA = 100;
    model.percentC = 0;
    model.percentG = 0;
    model.percentT = 0;
    model.length = 1000;

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    QString sequence = GTUtilsSequenceView::getSequenceAsString();
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

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    QString sequence = GTUtilsSequenceView::getSequenceAsString();

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

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    QString error = lt.getJoinedErrorString();
    CHECK_SET_ERR(error.contains(model.referenceUrl), "Expected error message is not found");
}

GUI_TEST_CLASS_DEFINITION(test_7407) {
    // Check that UGENE can generate a single character sequence.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7407.fa");
    model.length = 1;
    model.window = 1;
    model.referenceUrl = testDir + "_common_data/sanger/reference.gb";

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    QString sequence = GTUtilsSequenceView::getSequenceAsString();

    CHECK_SET_ERR(sequence.length() == 1, "Invalid sequence length: " + QString::number(sequence.length()));
    char c = sequence[0].toLatin1();
    CHECK_SET_ERR(c == 'A' || c == 'C' || c == 'G' || c == 'T', "Invalid sequence symbol: " + sequence[0]);
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_7410) {
    // Check sequence generation dialog uses correct file extension for MSA and generates MSA, not a sequence.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7410");
    model.formatId = BaseDocumentFormats::CLUSTAL_ALN;
    model.numberOfSequences = 3;
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 3, "Invalid number of sequence in the alignment");

    GTUtilsProjectTreeView::checkItem("test_7410.aln");
}

GUI_TEST_CLASS_DEFINITION(test_7413) {
    // Check that the distribution is uniform by the Kolmogorov-Smirnov test.
    // https://colab.research.google.com/drive/1-F4pAh-n0BMXeZczQY-te-UcEJeUSP8Y?usp=sharing
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7413.fa");
    model.percentA = 99;
    model.percentC = 1;
    model.percentG = 0;
    model.percentT = 0;
    model.length = 10000;

    auto checkUniformDistribution = [&model]() {
        GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
        GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

        GTUtilsSequenceView::checkSequenceViewWindowIsActive();
        QString sequence = GTUtilsSequenceView::getSequenceAsString();

        QVector<int> empiricalSum;
        int sumNumSeq = 0;
        for (QChar c : qAsConst(sequence)) {
            sumNumSeq += c == 'C' ? 1 : 0;
            empiricalSum << sumNumSeq;
        }
        CHECK_SET_ERR_RESULT(sumNumSeq > 0, "Invalid base content: there is no letter C in the sequence", false);

        double maxDifference = 0;
        for (int i = 0; i < model.length; i++) {
            maxDifference = std::max<double>(maxDifference, std::abs((double(i) + 1) / model.length - double(empiricalSum[i]) / sumNumSeq));
        }

        // https://drive.google.com/file/d/1YFIm8SXb3e-W0JKWWmiTXXh4BU2unHEm/view?usp=sharing
        // 1.61 is the constant from the table for alpha value 0.01.
        return maxDifference < 1.61 / std::sqrt(sumNumSeq);
    };

    for (int i = 0; i < 10; ++i) {
        if (checkUniformDistribution()) {
            return;
        }
        model.url = sandBoxDir + QString("/test_7413_%1.fa").arg(i);
    }
    CHECK_SET_ERR(false, "The generated sequences are not uniform distributed")
}

GUI_TEST_CLASS_DEFINITION(test_7414) {
    // Check that 1 char window selects a random char from the origin.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7414.fa");
    model.length = 1000;
    model.window = 1;
    model.referenceUrl = testDir + "_common_data/sanger/reference.gb";

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    QString sequence = GTUtilsSequenceView::getSequenceAsString();

    CHECK_SET_ERR(sequence.length() == model.length, "Invalid sequence length: " + QString::number(sequence.length()));
    CHECK_SET_ERR(sequence.count('A') > 0, "No 'A' char in the result");
    CHECK_SET_ERR(sequence.count('C') > 0, "No 'C' char in the result");
    CHECK_SET_ERR(sequence.count('G') > 0, "No 'G' char in the result");
    CHECK_SET_ERR(sequence.count('T') > 0, "No 'T' char in the result");
}

GUI_TEST_CLASS_DEFINITION(test_7415_1) {
    // Check that by default Random Sequence generator uses random seed: produces different results on different runs.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7415_1_1.fa");

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    QString sequence1 = GTUtilsSequenceView::getSequenceAsString();

    model.url = sandBoxDir + "/test_7415_1_2.fa";
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    QString sequence2 = GTUtilsSequenceView::getSequenceAsString();

    CHECK_SET_ERR(sequence1.length() == model.length, "Invalid sequence1 length: " + QString::number(sequence1.length()));
    CHECK_SET_ERR(sequence2.length() == model.length, "Invalid sequence2 length: " + QString::number(sequence2.length()));
    CHECK_SET_ERR(sequence1 != sequence2, "Sequences are equal");
}

GUI_TEST_CLASS_DEFINITION(test_7415_2) {
    // Check that for given seed, Random Sequence generator produces same results on different runs.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7415_2_1.fa");
    model.seed = 0;

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    QString sequence1 = GTUtilsSequenceView::getSequenceAsString();

    model.url = sandBoxDir + "/test_7415_2_2.fa";
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    QString sequence2 = GTUtilsSequenceView::getSequenceAsString();

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

    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    QString sequence = GTUtilsSequenceView::getSequenceAsString();

    CHECK_SET_ERR(sequence.length() == model.length, "Invalid sequence length: " + QString::number(sequence.length()));
    QString window1Sequence = sequence.mid(0, model.window);
    QString window2Sequence = sequence.mid(model.window, model.window);

    CHECK_SET_ERR(window1Sequence != window2Sequence, "Sequences are equal");
}

GUI_TEST_CLASS_DEFINITION(test_7419) {
    // Copy "_common_data/ugenedb/murine.ugenedb" to sandbox
    GTFile::copy(testDir + "_common_data/ugenedb/murine.ugenedb", sandBoxDir + "test_7419.ugenedb");

    // Open the copied file
    GTFileDialog::openFile(sandBoxDir + "test_7419.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Now remove it from the disk manually
    GTLogTracer lt;
    QFile::remove(sandBoxDir + "test_7419.ugenedb");

    // Expected: the message box about lost database -> click OK -> view is closed.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "The document 'test_7419.ugenedb' was removed from its original folder. Therefore, it will be deleted from the current project"));

    // The document update happens each 3 seconds, the messagebox will appear on the closest update
    GTUtilsDialog::checkNoActiveWaiters(3000);

    // Expected: no safe points
    auto joinedErrorList = lt.getJoinedErrorString();
    CHECK_SET_ERR(!joinedErrorList.contains("Trying to recover from error"), "Unexpected SAFE_POINT has appeared");
}

GUI_TEST_CLASS_DEFINITION(test_7438) {
    // Checks that selection with Shift does not cause a crash.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // There are 18 sequences in the list, and we are trying to select with SHIFT+KeyDown beyond this range.
    GTUtilsMsaEditor::clickSequence(15);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    QRect selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect.top() == 15, "Illegal start of the selection: " + QString::number(selectedRect.top()));
    CHECK_SET_ERR(selectedRect.bottom() == 17, "Illegal end of the selection: " + QString::number(selectedRect.bottom()));
}

GUI_TEST_CLASS_DEFINITION(test_7447) {
    // Check that search results in MSA Editor are reset when user enters incorrect search pattern.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Enter a valid search pattern: 'ATG'
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);
    GTUtilsOptionPanelMsa::enterPattern("ATG");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(0, 0, 3, 1),
                  QString("Illegal first result coordinates: " + GTUtilsText::rectToString(selectedRect)));

    // Press 'Next', move to the next result.
    GTUtilsOptionPanelMsa::clickNext();
    GTUtilsTaskTreeView::waitTaskFinished();
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(21, 0, 3, 1),
                  QString("Illegal second result coordinates: " + GTUtilsText::rectToString(selectedRect)));

    // Enter illegal 'M' character: check that there is a warning and no results in the list.
    auto patternEdit = GTWidget::findPlainTextEdit("textPattern");
    GTWidget::click(patternEdit);

    GTKeyboardDriver::keyClick('M');
    GTUtilsTaskTreeView::waitTaskFinished();

    QString pattern = GTUtilsOptionPanelMsa::getPattern();
    CHECK_SET_ERR(pattern == "ATGM", "Unexpected pattern, expected: ATGM, got: " + pattern);

    auto nextButton = GTWidget::findPushButton("nextPushButton");
    CHECK_SET_ERR(!nextButton->isEnabled(), "Next button must be disabled");
    GTUtilsOptionPanelMsa::checkResultsText("No results");

    // Delete the last 'M' character. Check that the first 'ATG' result is selected.
    GTWidget::click(patternEdit);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTUtilsTaskTreeView::waitTaskFinished();

    pattern = GTUtilsOptionPanelMsa::getPattern();
    CHECK_SET_ERR(pattern == "ATG", "Illegal pattern, expected: 'ATG', got: " + pattern);

    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(0, 0, 3, 1),
                  QString("Illegal first (2) result coordinates: " + GTUtilsText::rectToString(selectedRect)));
}

GUI_TEST_CLASS_DEFINITION(test_7448_1) {
    // Check that "Export sequence of selected annotations..." does not generate error messages.
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtils::checkExportServiceIsEnabled();

    GTUtilsSequenceView::clickAnnotationPan("misc_feature", 2);

    GTUtilsDialog::waitForDialog(
        new ExportSequenceOfSelectedAnnotationsFiller(
            sandBoxDir + "murine_out.fa",
            ExportSequenceOfSelectedAnnotationsFiller::Fasta,
            ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
            0,
            true,
            false,
            GTGlobals::UseMouse,
            true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there is no log message "Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries"
    CHECK_SET_ERR(!lt.hasMessage("Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries"), "Found unexpected message");
}

GUI_TEST_CLASS_DEFINITION(test_7448_2) {
    // Check that "Export sequence of selected annotations..."  for multi-big-chunks import mode works correctly.
    GTFileDialog::openFile(testDir + "_common_data/fasta/5mbf.fa.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "", "1..5000000"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsSequenceView::clickAnnotationPan("misc_feature", 1);

    GTUtilsDialog::waitForDialog(
        new ExportSequenceOfSelectedAnnotationsFiller(
            sandBoxDir + "test_7448_2_out.fa",
            ExportSequenceOfSelectedAnnotationsFiller::Fasta,
            ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
            0,
            true,
            false,
            GTGlobals::UseMouse,
            true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the last 3 symbols are RAG.
    QString currentString = GTUtilsSequenceView::getEndOfSequenceAsString(3);
    CHECK_SET_ERR(currentString == "RAG", "Last 3 symbols expected: RAG, current: " + currentString);
}

GUI_TEST_CLASS_DEFINITION(test_7448_3) {
    // Check that "Export sequence of selected annotations..."  for multi-big-chunks import mode works correctly for complementary mode.

    GTFileDialog::openFile(testDir + "_common_data/fasta/5mbf.fa.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "", "complement(1..5000000)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsSequenceView::clickAnnotationPan("misc_feature", 1);

    GTUtilsDialog::waitForDialog(
        new ExportSequenceOfSelectedAnnotationsFiller(
            sandBoxDir + "test_7448_3_out.fa",
            ExportSequenceOfSelectedAnnotationsFiller::Fasta,
            ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
            0,
            true,
            false,
            GTGlobals::UseMouse,
            true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the first 3 symbols are TPA.
    QString currentString = GTUtilsSequenceView::getBeginOfSequenceAsString(3);
    CHECK_SET_ERR(currentString == "TPA", "Last 3 symbols expected: TPA, current:" + currentString);

    GTUtilsSequenceView::clickMouseOnTheSafeSequenceViewArea();

    // Expected: the last 3 symbols are ILD.
    currentString = GTUtilsSequenceView::getEndOfSequenceAsString(3);
    CHECK_SET_ERR(currentString == "ILD", "Last 3 symbols expected: ILD, current: " + currentString);
}

GUI_TEST_CLASS_DEFINITION(test_7448_4) {
    // Check that multi-region/multi-frame DNA annotation is translated correctly (DNA is joined first, translated next).
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "", "join(10..16,18..20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 10);

    GTUtilsDialog::waitForDialog(
        new ExportSequenceOfSelectedAnnotationsFiller(
            sandBoxDir + "test_7448_4_out.fa",
            ExportSequenceOfSelectedAnnotationsFiller::Fasta,
            ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
            0,
            true,
            false,
            GTGlobals::UseMouse,
            true));

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString exportedSequence = GTUtilsSequenceView::getSequenceAsString();
    CHECK_SET_ERR(exportedSequence == "SPS", "Sequence not matched: " + exportedSequence);
}

GUI_TEST_CLASS_DEFINITION(test_7451) {
    // Check that a right click on a recent item on the Welcome Screen does not crash UGENE.

    // Copy the test file first to a tmp location: we will need to remove it later.
    GTFile::copy(dataDir + "samples/FASTA/human_T1.fa", testDir + "_common_data/scenarios/sandbox/test_7451.fa");
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/test_7451.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsMdi::closeActiveWindow();
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened();
    GTUtilsStartPage::checkRecentListUrl("test_7451.fa", true);

    // Test a right click on the Welcome Screen for a valid file.
    GTWidget::click(GTWidget::findLabelByText("test_7451.fa").first(), Qt::RightButton);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsMdi::closeActiveWindow();
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened();
    GTUtilsStartPage::checkRecentListUrl("test_7451.fa", true);

    // Test a right click on the Welcome Screen for a removed file.
    // Close the project first to avoid 'missed file' popups.
    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});
    QFile::remove(testDir + "_common_data/scenarios/sandbox/test_7451.fa");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Remove From List"));
    GTWidget::click(GTWidget::findLabelByText("test_7451.fa").first(), Qt::RightButton);

    // Check that there is no removed item in the recent files list and UGENE does not crash.
    GTUtilsStartPage::checkRecentListUrl("test_7451.fa", false);
}

GUI_TEST_CLASS_DEFINITION(test_7454) {
    // Open data/samples/PDB/1CF7.PDB.
    // Increase the width of the Project View.
    //     Expected: the current Sequence View is narrow.
    // Find the action toolbar extension for the first sequence in the Sequence View (">>" button). Press ">>"->
    //         "X Remove sequence".
    //     Expected: no crash.
    GTUtilsProject::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    QRect rect = GTWidget::findWidget("project_view")->geometry();
    QPoint splitterCenter =
        GTWidget::findWidget("project_view")->mapToGlobal({rect.right() + 4, rect.center().y()});
    QPoint delta(GTMainWindow::getMainWindowWidgetByName("main_window")->width() * 0.6, 0);
    GTMouseDriver::dragAndDrop(splitterCenter, splitterCenter + delta);

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove sequence"}));
    GTWidget::click(
        GTWidget::findWidget(
            "qt_toolbar_ext_button",
            GTWidget::findToolBar("views_tool_bar_1CF7 chain A sequence")));
}

GUI_TEST_CLASS_DEFINITION(test_7455) {
    // 1. Open "_common_data/regression/7455/clipboard.gb"
    GTFileDialog::openFile(testDir + "_common_data/regression/7455/clipboard.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Open the "Find restriction sites" dialog, choose "AaaI" (vary first one) only and click OK.
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"AaaI"}));
    GTWidget::click(GTWidget::findWidget("Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Right click -> Cloning -> Digest into fragments...
    class DigestScenario : public CustomScenario {
    public:
        void run() override {
            // 4. Select "AaaI" and click "Add---->"
            // 5. Go to the "Conserved annotations" tab
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto availableEnzymeWidget = GTWidget::findListWidget("availableEnzymeWidget", dialog);
            QList<QListWidgetItem*> items = availableEnzymeWidget->findItems("AaaI : 2 cut(s)", Qt::MatchExactly);
            CHECK_SET_ERR(items.size() == 1, "Unexpected number of enzymes");

            GTListWidget::click(availableEnzymeWidget, "AaaI : 2 cut(s)");
            GTWidget::click(GTWidget::findWidget("addButton", dialog));
            GTTabWidget::clickTab("tabWidget", dialog, 1);

            class SelectAnnotationScenario : public CustomScenario {
            public:
                void run() override {
                    // 6. Click "Add", choose the only option and click "OK".
                    QWidget* dialog = GTWidget::getActiveModalWidget();
                    auto selectAnnotationsList = GTWidget::findWidgetByType<QListWidget*>(dialog, "Cant find the \"Select annotations\" list");
                    auto items = GTListWidget::getItems(selectAnnotationsList);
                    CHECK_SET_ERR(items.size() == 1, "Unexpected number of annotations");

                    GTListWidget::click(selectAnnotationsList, items.first());
                    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
                }
            };

            GTUtilsDialog::waitForDialog(new Filler("select_annotations_dialog", new SelectAnnotationScenario()));

            // 7. Click "OK"
            GTWidget::click(GTWidget::findWidget("addAnnBtn", dialog));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller(new DigestScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Cloning", "Digest into fragments..."}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the task finished with an error: Conserved annotation misc_feature (2646..3236) is disrupted by the digestion. Try changing the restriction sites.
    CHECK_SET_ERR(lt.hasError("Conserved annotation Misc. Feature (2646..3236) is disrupted by the digestion. Try changing the restriction sites."), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_7456) {
    // Check that UGENE can open a FASTA file with a 100k small sequences as an alignment.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7456.fa");
    model.length = 5;
    model.window = 5;
    model.numberOfSequences = 100 * 1000;
    GTUtilsDialog::add(new DNASequenceGeneratorDialogFiller(model));
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join), 90000);
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCount == model.numberOfSequences, "Invalid sequence count in MSA: " + QString::number(sequenceCount));
}

GUI_TEST_CLASS_DEFINITION(test_7459) {
    // Check that UGENE can fetch remote file as a reference for the generator task.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7459.fa");
    model.referenceUrl = "https://raw.githubusercontent.com/ugeneunipro/ugene/master/data/samples/FASTA/human_T1.fa";
    GTUtilsDialog::add(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_7460) {
    // Check that UGENE can open an alignment of [1_000 x 10_000] fast enough.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7460.fa");
    model.length = 1000;
    model.window = 1000;
    model.numberOfSequences = 10000;

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsDialog::waitForDialog(new DNASequenceGeneratorDialogFiller(model));
    GTMenu::clickMainMenuItem({"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCount == model.numberOfSequences, "Invalid sequence count in MSA: " + QString::number(sequenceCount));

    QWidget* overviewWidget = GTUtilsMsaEditor::getOverviewArea();
    CHECK_SET_ERR(overviewWidget->isVisible(), "Overview widget ,must be visible, but must be hidden");
    GTUtilsTaskTreeView::waitTaskFinished(10000);  // Check that there is no long-running active tasks.
}

GUI_TEST_CLASS_DEFINITION(test_7463) {
    // Tools->NGS data analysis->Extract consensus from assemblies...
    // Set _common_data/bam/Mycobacterium.sorted.bam as input and run.
    // Repeat steps with data/samples/Assembly/chrM.sorted.bam.
    // When the chrM workflow is over, click "Close dashboard".
    //     Expected: no crash or freeze.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", QStringList(), {{"Assembly", testDir + "_common_data/bam/Mycobacterium.sorted.bam"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow();

    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", QStringList(), {{"Assembly", dataDir + "samples/Assembly/chrM.sorted.bam"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow();

    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::findLabelByText("The workflow task has been finished", GTUtilsDashboard::getDashboard());

    auto tab = GTTabWidget::getTabBar(GTUtilsDashboard::getTabWidget());
    GTWidget::click(tab->tabButton(tab->currentIndex(), QTabBar::RightSide));
    GTWidget::click(tab->tabButton(tab->currentIndex(), QTabBar::RightSide));
}

GUI_TEST_CLASS_DEFINITION(test_7465) {
    // 1. Open workflow sample "Align sequences with MUSCLE"
    // Expected state: wizard has appeared.
    class AlignSequencesWithMuscleWizardFiller : public CustomScenario {
    public:
        void run() override {
            // 2. Set file with many (~1200) sequences as input file and run workflow
            GTUtilsWizard::setInputFiles({{QFileInfo(testDir + "_common_data/regression/7465/big_msa_as_fasta.fa").absoluteFilePath()}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };
    GTUtilsDialog::waitForDialog(new WizardFiller("Align Sequences with MUSCLE Wizard", new AlignSequencesWithMuscleWizardFiller));
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: there is a notification about lacking of memory.
    CHECK_SET_ERR(GTUtilsDashboard::getJoinedNotificationsString().contains("There is not enough memory to align these sequences with MUSCLE"),
                  "No expected message about lacking of memory in notifications");
}

GUI_TEST_CLASS_DEFINITION(test_7469) {
    // Check that annotation sequence copy action respects 'join' and 'order' location flags.
    GTFileDialog::openFile(testDir + "_common_data/genbank/7469.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Check 'order' annotation on the direct strand.
    GTUtilsAnnotationsTreeView::clickItem("CDS", 1, false);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "AAGACCCCCCCGTAGG", "1. Unexpected DNA sequence: " + GTClipboard::text());
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "KTPP*", "1. Unexpected Amino sequence: " + GTClipboard::text());

    // Check 'order' annotation on the complementary strand.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "AAGACCCC-CCCGTAGG", "2. Unexpected DNA sequence: " + GTClipboard::text());
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "KT-PV", "2. Unexpected Amino sequence: " + GTClipboard::text());

    // Check 'join' annotation on the direct strand.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "TGCCTTGCAAAGTTACTTAAGCTAGCTTG", "3. Unexpected DNA sequence: " + GTClipboard::text());
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "CLAKLLKLA", "3. Unexpected Amino sequence: " + GTClipboard::text());

    // Check 'join' annotation on the complementary strand.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "TGCCTTGCAAA-GTTACTTAAGCTAGCTTG", "4. Unexpected DNA sequence: " + GTClipboard::text());
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text() == "CLA-VT*ASL", "4. Unexpected Amino sequence: " + GTClipboard::text());
}

GUI_TEST_CLASS_DEFINITION(test_7472) {
    // Check that "Build tree" does not start the task if output directory is not writable.
    GTFileDialog::openFile(testDir + "_common_data/stockholm/ABC_tran.sto");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    class CheckReadOnlyPathScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            // Create a read-only directory and set a path to a file inside it into the saveLineEdit.
            QString dirPath = QFileInfo(sandBoxDir + GTUtils::genUniqueString("test_7472")).absoluteFilePath();
            CHECK_SET_ERR(QDir().mkpath(dirPath), "Failed to create dir: " + dirPath);
            GTFile::setReadOnly(dirPath, false);

            auto saveLineEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(saveLineEdit, dirPath + "/tree.nwk");

            // Check that error message is shown.
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Error opening file for writing"));
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new CheckReadOnlyPathScenario()));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkNoItem("tree.nwk");
}

GUI_TEST_CLASS_DEFINITION(test_7473_1) {
    // Build an alignment for a read-only alignment file.
    GTFileDialog::openFile(testDir + "_common_data/stockholm", "2-Hacid_dh.sto");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDocument::checkIfDocumentIsLocked("2-Hacid_dh.sto", true);

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_7443.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that tree view is opened.
    GTUtilsMsaEditor::getTreeView();
}

GUI_TEST_CLASS_DEFINITION(test_7473_2) {
    // Build an alignment for a read-only alignment file from options panel.
    GTFileDialog::openFile(dataDir + "samples/Stockholm/CBS.sto");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDocument::checkIfDocumentIsLocked("CBS.sto", true);

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTWidget::click(GTWidget::findWidget("buildTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that tree view is opened.
    GTUtilsMsaEditor::getTreeView();
}

GUI_TEST_CLASS_DEFINITION(test_7476) {
    // Check that IQ-TREE has "Display Options" tab, and it works.
    GTFileDialog::openFile(testDir + "_common_data/clustal/collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    class EnableCreateNewViewOptionScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();

            GTComboBox::selectItemByText("algorithmBox", dialog, "IQ-TREE");

            GTTabWidget::clickTab("tab_widget", dialog, "Display Options");

            GTRadioButton::click("createNewView", dialog);

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new EnableCreateNewViewOptionScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that tree view is opened.
    GTUtilsPhyTree::checkTreeViewerWindowIsActive("collapse_mode_");
}

GUI_TEST_CLASS_DEFINITION(test_7487_1) {
    // Check that move of the multi-region selection with drag-and-drop works as expected (2 selected regions).
    GTFileDialog::openFile(testDir + "_common_data/clustal/collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Original state: a,b,c,d,e,f,g,h.
    GTUtilsMsaEditor::selectRowsByName({"c", "d", "g", "h"});

    // Drag and drop "g" up to the "e" location.
    QRect movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect("g");
    QRect movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect("e");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "g", "h", "e", "f"}), "1. Unexpected order: " + nameList.join(","));

    // Restore original state: a,b,c,d,e,f,g,h.
    GTUtilsMsaEditor::clearSelection();
    GTUtilsMsaEditor::undo();
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "e", "f", "g", "h"}), "2. Unexpected order: " + nameList.join(","));

    // Drag and drop "b" down to the "d" location.
    GTUtilsMsaEditor::selectRowsByName({"a", "b", "e", "f"});
    movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect("b");
    movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect("d");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"c", "d", "a", "b", "e", "f", "g", "h"}), "3. Unexpected order: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7487_2) {
    // Check that move of the multi-region selection with drag-and-drop works as expected (4 selected regions).
    GTFileDialog::openFile(testDir + "_common_data/clustal/collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Original state: a,b,c,d,e,f,g,h.
    GTUtilsMsaEditor::selectRowsByName({"a", "c", "e", "g"});

    // Drag and drop "e" up to the "d" location.
    QRect movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect("e");
    QRect movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect("d");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"b", "a", "c", "e", "g", "d", "f", "h"}), "1. Unexpected order: " + nameList.join(","));
    GTUtilsMsaEditor::checkSelectionByNames({"a", "c", "e", "g"});

    // Restore original state: a,b,c,d,e,f,g,h. Check that selection is restored too.
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkSelectionByNames({"a", "c", "e", "g"});
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "e", "f", "g", "h"}), "2. Unexpected order: " + nameList.join(","));
    GTUtilsMsaEditor::clearSelection();

    // Drag and drop "d" down to the "e" location.
    GTUtilsMsaEditor::selectRowsByName({"b", "d", "f", "h"});
    movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect("d");
    movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect("e");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"a", "c", "e", "b", "d", "f", "h", "g"}), "3. Unexpected order: " + nameList.join(","));
    GTUtilsMsaEditor::checkSelectionByNames({"b", "d", "f", "h"});

    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::checkSelectionByNames({"b", "d", "f", "h"});
    nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "e", "f", "g", "h"}), "4. Unexpected order: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7490) {
    // Create a multi-selection and check that the current line label in the MCA editor's status bar shows '-'.
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", sandBoxDir + "test_7490.ugenedb");
    GTFileDialog::openFile(sandBoxDir + "test_7490.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMcaEditor::selectReadsByName({"SZYD_Cas9_5B70", "SZYD_Cas9_CR50"});
    QString currentLineNumberText = GTUtilsMcaEditorStatusWidget::getRowNumberString();
    CHECK_SET_ERR(currentLineNumberText == "-", "Unexpected <Ln> string in MCA editor status bar: " + currentLineNumberText);
}

GUI_TEST_CLASS_DEFINITION(test_7491) {
    /*
     * 1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
     * 2. Set "samples/Assembly/chrM.sorted.bam" as an input
     * 3. Click "Run"
     * 4. Close WD
     * 5. Click 'Save' and 'Save anyway' in save dialogs
     * 6. repeat steps 1-4
     * Expected state: no errors in the log
     */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", QStringList(), {{"Assembly", dataDir + "samples/Assembly/chrM.sorted.bam"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsDialog::checkNoActiveWaiters(40000);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Save));
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsDialog::checkNoActiveWaiters(10000);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Save anyway"));
    GTUtilsDialog::checkNoActiveWaiters(10000);

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", QStringList(), {{"Assembly", dataDir + "samples/Assembly/chrM.sorted.bam"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7499) {
    // Create a multi-selection and check that the current line label in the MCA editor's status bar shows '-'.

    GTFileDialog::openFile(testDir + "_common_data/clustal/protein.fasta.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QString sequence1v1 = GTUtilsMSAEditorSequenceArea::getSequenceData(1).left(10);
    QString sequence8v1 = GTUtilsMSAEditorSequenceArea::getSequenceData(8).left(10);

    // Modify 2 sequences first.
    GTUtilsMSAEditorSequenceArea::clickToPosition({1, 1});
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::clickToPosition({8, 8});
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString sequence1v2 = GTUtilsMSAEditorSequenceArea::getSequenceData(1).left(10);
    QString sequence8v2 = GTUtilsMSAEditorSequenceArea::getSequenceData(8).left(10);
    QString expected1v2 = sequence1v1.mid(0, 1) + "-" + sequence1v1.mid(1, 8);
    QString expected8v2 = sequence8v1.mid(0, 8) + "-" + sequence8v1.mid(8, 1);
    CHECK_SET_ERR(sequence1v2 == expected1v2, "Sequence 1 modification is not matched: " + sequence1v2 + ", expected: " + expected1v2);
    CHECK_SET_ERR(sequence8v2 == expected8v2, "Sequence 8 modification is not matched: " + sequence8v2 + ", expected: " + expected8v2);

    QStringList nameListBefore = GTUtilsMSAEditorSequenceArea::getNameList();

    // Align the first sequence to the current alignment.
    GTUtilsMsaEditor::clickSequence(1);
    GTUtilsDialog::waitForDialog(new PopupChooser({"align_selection_to_alignment_muscle"}));
    GTWidget::click(GTAction::button("align_selected_sequences_to_alignment"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // The order of sequences should not change.
    QStringList nameListAfter = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameListBefore == nameListAfter, "Name list changed");

    // The only the first sequence must be changed (alignment back).
    QString sequence1v3 = GTUtilsMSAEditorSequenceArea::getSequenceData(1).left(10);
    QString sequence8v3 = GTUtilsMSAEditorSequenceArea::getSequenceData(8).left(10);
    CHECK_SET_ERR(sequence1v3 == sequence1v1, "Sequence 1 was not aligned as expected.");
    CHECK_SET_ERR(sequence8v3 == sequence8v2, "Sequence 8 was modified as result of alignment");
}

GUI_TEST_CLASS_DEFINITION(test_7504) {
    // Check that multi-region complement(join()) annotation is exported in the correct order.
    GTFileDialog::openFile(testDir + "_common_data/fasta/short.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "", "complement(join(1..1,10..10))"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsDialog::waitForDialog(
        new ExportSequenceOfSelectedAnnotationsFiller(
            sandBoxDir + "test_7504_out.fa",
            ExportSequenceOfSelectedAnnotationsFiller::Fasta,
            ExportSequenceOfSelectedAnnotationsFiller::Merge));

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export sequence of selected annotations..."}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();

    QString exportedSequence = GTUtilsSequenceView::getSequenceAsString();
    CHECK_SET_ERR(exportedSequence == "GA", "Sequence not matched: " + exportedSequence);
}

GUI_TEST_CLASS_DEFINITION(test_7505) {
    // Check that double-click on the sequence name in MSA editor toggles centering of the start/end sequence region.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Delete first sequences so the tested sequence will be scrolled into the view.
    GTUtilsMsaEditor::selectRows(0, 15);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    QString sequenceName = "Pc_Metavir10";
    GTUtilsMsaEditor::clickSequenceName(sequenceName);
    GTUtilsMsaEditor::checkSelectionByNames({sequenceName});

    int firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstVisibleBase == 0, "1. Unexpected first visible base: " + QString::number(firstVisibleBase));

    QRect rect = GTUtilsMsaEditor::getSequenceNameRect(sequenceName);
    GTMouseDriver::moveTo(rect.center());

    GTMouseDriver::doubleClick();
    int expectedCenter = 66;
    firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstVisibleBase < expectedCenter, "2. Unexpected first visible base: " + QString::number(firstVisibleBase));
    int lastVisibleBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastVisibleBase > expectedCenter, "2. Unexpected last visible base: " + QString::number(lastVisibleBase));

    GTMouseDriver::doubleClick();
    expectedCenter = 1220;
    firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstVisibleBase < expectedCenter, "3. Unexpected first visible base: " + QString::number(firstVisibleBase));
    lastVisibleBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastVisibleBase > expectedCenter, "3. Unexpected last visible base: " + QString::number(lastVisibleBase));

    GTMouseDriver::doubleClick();
    expectedCenter = 66;
    firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstVisibleBase < expectedCenter, "4. Unexpected first visible base: " + QString::number(firstVisibleBase));
    lastVisibleBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(lastVisibleBase > expectedCenter, "4. Unexpected last visible base: " + QString::number(lastVisibleBase));
}

GUI_TEST_CLASS_DEFINITION(test_7506) {
    // Check that blast search correctly selects database name from new variants of file.
    BlastLocalSearchDialogFiller::Parameters blastParams;
    blastParams.runBlast = true;
    blastParams.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1_v2_10/human_T1.ndb";
    blastParams.withInputFile = true;
    blastParams.inputPath = dataDir + "samples/FASTA/human_T1.fa";
    GTUtilsDialog::waitForDialog(new BlastLocalSearchDialogFiller(blastParams));
    GTMenu::clickMainMenuItem({"Tools", "BLAST", "BLAST search..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    bool hasExpectedResult = GTUtilsAnnotationsTreeView::findRegion("blast result", U2Region(5061, 291));
    CHECK_SET_ERR(hasExpectedResult, "Can not find the expected blastn result");
}

GUI_TEST_CLASS_DEFINITION(test_7507) {
    // Check that "Worm" rendering mode is available and is enabled by default for '4rte' PDB file.
    GTFileDialog::openFile(testDir + "_common_data/pdb/4rte.pdb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow();
    auto glWidget = GTWidget::findWidget("1-4RTE", sequenceViewWindow);

    GTUtilsDialog::waitForDialog(
        new PopupCheckerByText(
            {"Render Style", "Worms"},
            PopupChecker::CheckOptions(PopupChecker::IsEnabled | PopupChecker::IsChecked)));
    GTMenu::showContextMenu(glWidget);
}

GUI_TEST_CLASS_DEFINITION(test_7508) {
    // Check that no-op "align-selection" action with MUSCLE does not produce "Undo" history.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Try to  align a sequence that is already aligned.
    GTUtilsMsaEditor::clickSequence(1);
    GTUtilsDialog::waitForDialog(new PopupChooser({"align_selection_to_alignment_muscle"}));
    GTWidget::click(GTAction::button("align_selected_sequences_to_alignment"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undoButton->isEnabled(), "Undo button must be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_7509) {
    // Check that MCA editor does not crash when closed in "replace-character" mode.
    GTFileDialog::openFile(dataDir + "samples/Sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();

    // Enable "replace-character" mode.
    GTUtilsMcaEditorSequenceArea::clickToPosition({6374, 0});
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::getSelectedReadChar() == 'C', "Position validation failed!");

    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);
    short mode = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode();
    CHECK_SET_ERR(mode == 1, "Not an edit mode! Mode: " + QString::number(mode));

    // Close MCA editor -> UGENE should not crash.
    GTUtilsMdi::closeActiveWindow();
}

GUI_TEST_CLASS_DEFINITION(test_7511) {
    // Check that Blast Search filters the list of available tool based on the selected file sequence alphabet.
    class BlastToolListCheckScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            auto toolsCombo = GTWidget::findComboBox("programNameComboBox");
            auto selectFileButton = GTWidget::findToolButton("browseInput", dialog);
            GTComboBox::checkValuesPresence(toolsCombo, {"blastn", "blastp", "blastx", "tblastx", "tblastn"});

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
            GTWidget::click(selectFileButton);
            GTUtilsTaskTreeView::waitTaskFinished();
            // Check that the list of tools is updated to nucleic tools.
            GTComboBox::checkValuesPresence(toolsCombo, {"blastn", "blastx", "tblastx"});

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/titin.fa"));
            GTWidget::click(selectFileButton);
            GTUtilsTaskTreeView::waitTaskFinished();
            //  Check that the list of tools is updated to amino tools.
            GTComboBox::checkValuesPresence(toolsCombo, {"blastp", "tblastn"});

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);  // Cancel "Blast" dialog.
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);  // Cancel "Save project" popup.
        }
    };
    GTUtilsDialog::waitForDialog(new BlastLocalSearchDialogFiller(new BlastToolListCheckScenario()));
    GTMenu::clickMainMenuItem({"Tools", "BLAST", "BLAST search..."});
}

GUI_TEST_CLASS_DEFINITION(test_7515) {
    // Run Tools->NGS data analysis->Extract consensus from assemblies... twice and see that UGENE does not crash.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", QStringList(), {{"Assembly", testDir + "_common_data/bam/hg19_chr5_chr10_chr12_chrX.sorted.bam"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow();

    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", QStringList(), {{"Assembly", testDir + "_common_data/bam/hg19_chr5_chr10_chr12_chrX.sorted.bam"}}));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow();

    // Wait up to 10 seconds for the crash and finish tasks if no crash happened.
    GTGlobals::sleep(10000);
    GTUtilsWorkflowDesigner::stopWorkflow();
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Discard));
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsMdi::activateWindow("Extract");
    GTUtilsWorkflowDesigner::stopWorkflow();
}

GUI_TEST_CLASS_DEFINITION(test_7517) {
    // Check that MCA editor does not crash when closed in "replace-character" mode.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsLog::checkMessageWithTextCount("Registering new task: Render overview", 1, "check1");

    auto showOverviewButton = GTUtilsMsaEditor::getShowOverviewButton();
    GTWidget::click(showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsLog::checkMessageWithTextCount("Registering new task: Render overview", 1, "check2");

    GTWidget::click(showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsLog::checkMessageWithTextCount("Registering new task: Render overview", 1, "check3");
}

GUI_TEST_CLASS_DEFINITION(test_7520) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Add "Improve Reads with Trimmomatic" to the scene
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    GTUtilsWorkflowDesigner::addElement(trimmomaticName);

    // 3. Check tooltips for "Palindrome clip threshold" label and value
    // Expected state: they should be correct (different with "simple clip" tooltip)

    class TrimmomaticCustomScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto addButton = GTWidget::findToolButton("buttonAdd", dialog);

            GTWidget::click(addButton);
            for (int i = 0; i < 4; i++) {
                GTKeyboardDriver::keyClick(Qt::Key_Down);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTWidget::click(addButton);

            GTMouseDriver::moveTo(GTWidget::getWidgetCenter(GTWidget::findWidget("palindromeThreshold")));
            QString tooltip = GTUtilsToolTip::getToolTip();
            QString expedtedTooltip("A threshold for palindrome alignment mode. For palindromic matches, a longer alignment is possible."
                                    " Therefore the threshold can be in the range of 30. Even though this threshold is very high"
                                    " (requiring a match of almost 50 bases) Trimmomatic is still able to identify very, very short adapter fragments.");
            CHECK_SET_ERR(tooltip.contains(expedtedTooltip), QString("Actual tooltip not contains expected string. Expected string: %1").arg(expedtedTooltip));

            GTMouseDriver::moveTo(GTWidget::getWidgetCenter(GTWidget::findWidget("palindromeLabel")));
            tooltip = GTUtilsToolTip::getToolTip();
            CHECK_SET_ERR(tooltip.contains(expedtedTooltip), QString("Actual tooltip not contains expected string. Expected string: %1").arg(expedtedTooltip));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new TrimmomaticDialogFiller(new TrimmomaticCustomScenario()));
    GTUtilsWorkflowDesigner::click(trimmomaticName);
    GTUtilsWorkflowDesigner::setParameter("Trimming steps", "", GTUtilsWorkflowDesigner::customDialogSelector);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7521) {
    GTFileDialog::openFile(testDir + "_common_data/clustal/align_subalign.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_7521_nm.png", ExportMsaImage::Settings(false, false, true, false)));
    GTWidget::click(GTAction::button("export_msa_as_image_action"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_7521_mm.png", ExportMsaImage::Settings(false, false, true, true, 70)));
    GTWidget::click(GTAction::button("export_msa_as_image_action"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QPixmap normalMode(sandBoxDir + "test_7521_nm");
    QPixmap multilineMode(sandBoxDir + "test_7521_mm");
    double normalHeight = normalMode.height();
    double multilineHeight = multilineMode.height();
    double normalWidth = normalMode.width();
    double multilineWidth = multilineMode.width();

    double widthDiff = normalWidth / multilineWidth;

    CHECK_SET_ERR(multilineHeight == normalHeight * 2 + 30, "Image height in multiline mode: " + QString::number(normalHeight) + ", image height in multiline mode: " + QString::number(multilineHeight));
    CHECK_SET_ERR(widthDiff > 1 && widthDiff < 2, "Image width in multiline mode: " + QString::number(normalWidth) + ", image width in multiline mode: " + QString::number(multilineWidth));
}

GUI_TEST_CLASS_DEFINITION(test_7531) {
    // Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Ctrl+N" and create the annotation on "80..90"
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "test_7531", "80..90"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Select the created annotation and click "Delete".
    GTUtilsAnnotationsTreeView::clickItem("test_7531", 1, false);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Open the "In silico PCR" tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // Set "TTGTCAGATTCACCAAAGTT" as a forward primer and "CTCTCTTCTGGCCTGTAGGGTTTCTG" as a reverse primer.
    GTUtilsOptionPanelSequenceView::setForwardPrimer("TTGTCAGATTCACCAAAGTT");
    GTUtilsOptionPanelSequenceView::setReversePrimer("CTCTCTTCTGGCCTGTAGGGTTTCTG");

    // Click "Find product(s) anyway".
    GTUtilsOptionPanelSequenceView::pressFindProducts();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the only product has been found.
    const int count = GTUtilsOptionPanelSequenceView::productsCount();
    CHECK_SET_ERR(count == 1, QString("Unexpected products quantity, expected: 1, current: %1").arg(count));

    // Click "Extract primer".
    GTUtilsOptionPanelSequenceView::pressExtractProduct();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: no crash
}

GUI_TEST_CLASS_DEFINITION(test_7535) {
    // Check that UGENE does not crash when tooltip is invoked on non-standard annotations.
    GTFileDialog::openFile(testDir + "_common_data/genbank/zero_length_feature.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto zeroLength0Item = GTUtilsAnnotationsTreeView::findItem("zero_length_0");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(zeroLength0Item));
    QString tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.isEmpty(), "Expected no tooltip for zero-length annotation: " + tooltip);

    auto zeroLengthXItem = GTUtilsAnnotationsTreeView::findItem("zero_length_x");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(zeroLengthXItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.isEmpty(), "Expected no tooltip for out of bound annotation: " + tooltip);

    auto normalLengthItem = GTUtilsAnnotationsTreeView::findItem("normal_length");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(normalLengthItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = TTGCAGAATTC"), "Expected sequence info in tooltip for a normal annotation: " + tooltip);

    auto normalLengthComplementaryItem = GTUtilsAnnotationsTreeView::findItem("normal_length_c");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(normalLengthComplementaryItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = GAATTCTGCAA"), "Expected complementary sequence info in tooltip for a normal annotation: " + tooltip);

    auto joinedItem = GTUtilsAnnotationsTreeView::findItem("joined");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(joinedItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = TCT"), "Expected dna sequence info in tooltip for a joined annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = S"), "Expected amino sequence info in tooltip for a joined annotation: " + tooltip);

    auto joinedComplementaryItem = GTUtilsAnnotationsTreeView::findItem("joined_c");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(joinedComplementaryItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = AGA"), "Expected dna sequence info in tooltip for a joined complementary annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = R"), "Expected amino sequence info in tooltip for a joined complementary annotation: " + tooltip);
}

GUI_TEST_CLASS_DEFINITION(test_7539) {
    // Check that UGENE shows a tooltip when a small 1-char annotation region is hovered in sequence view.
    GTFileDialog::openFile(testDir + "_common_data/genbank/zero_length_feature.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::moveMouseToAnnotationInDetView("joined", 30);
    QString tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = TCT"), "Expected dna sequence info in tooltip for a joined annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = S"), "Expected amino sequence info in tooltip for a joined annotation: " + tooltip);

    GTUtilsSequenceView::moveMouseToAnnotationInDetView("joined_c", 30);
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = AGA"), "Expected dna sequence info in tooltip for a joined complementary annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = R"), "Expected amino sequence info in tooltip for a joined complementary annotation: " + tooltip);
}

GUI_TEST_CLASS_DEFINITION(test_7540) {
    // 1. Open file with two equal annotations.
    GTFileDialog::openFile(testDir + "_common_data/regression/7540/7540.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* miscFeature = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTTreeWidget::click(miscFeature);
    // 2. Select one and cheange it location, then name.
    GTUtilsDialog::waitForDialog(new EditAnnotationFiller("misc_feature", "2..8"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    GTTreeWidget::click(miscFeature);
    GTUtilsDialog::waitForDialog(new EditAnnotationFiller("misc_feature1", "2..8"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    // 3. Open Annotation Highlighting Tab.
    // Expected state: no crash or SAFE_POINT triggering.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
}

GUI_TEST_CLASS_DEFINITION(test_7546) {
    // Check that tree or msa with ambiguous names can't be synchronized.
    GTFileDialog::openFile(testDir + "_common_data/clustal/same_name_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::toggleCollapsingMode();
    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_7546.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: UGENE does not crash and Sync button is OFF.
    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(!syncModeButton->isEnabled(), "Sync mode must be not available");
}

GUI_TEST_CLASS_DEFINITION(test_7548) {
    // Check that UGENE shows correct MSA symbols for huge MSA files when scrolled to large positions.
    GTFileDialog::openFile(testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::clickToPosition({1, 0});  // Move focus and check visually that the selected char is 'C'.
    QString colorOfC = GTUtilsMSAEditorSequenceArea::getColor({1, 0});

    QList<QPoint> positions = {{9999 - 1, 0}, {100000 - 1, 0}, {2000000 - 1, 1}};
    for (const QPoint& position : qAsConst(positions)) {
        GTUtilsMsaEditor::gotoWithKeyboardShortcut(position.x() + 1);  // GoTo accepts visual positions.
        GTUtilsMSAEditorSequenceArea::clickToPosition(position);  // Selected character has some opacity adjustment.

        QString color = GTUtilsMSAEditorSequenceArea::getColor(position);
        CHECK_SET_ERR(color == colorOfC, "Invalid color: " + color + ", position: " + QString::number(position.x()) + ", expected: " + colorOfC);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7550) {
    class Click103TimesScenario : public CustomScenario {
    public:
        void run() override {
            auto stack = AppContext::getMainWindow()->getNotificationStack();
            for (int i = 0; i < 103; i++) {
                stack->addNotification("Notification " + QString::number(i + 1));
                GTGlobals::sleep(200);
            }
        }
    };
    // Create 100+ notifications. Check that UGENE does not crash.
    GTThread::runInMainThread(new Click103TimesScenario());
}

GUI_TEST_CLASS_DEFINITION(test_7555) {
    // Check that among many empty assemblies in the file the one with reads is opened by default.
    GTFileDialog::openFile(testDir + "_common_data/ugenedb/7555-reads-data-removed.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    CHECK_SET_ERR(GTUtilsAssemblyBrowser::getReadsCount() > 0, "No reads in the view");
}

GUI_TEST_CLASS_DEFINITION(test_7556) {
    // Check that IQ-TREE parameter input widgets work in sync with a manual parameters input as text.
    // Check that in Tree-Sync mode Drag & Drop of sequences in the MSA name list is disabled.
    GTFileDialog::openFile(testDir + "_common_data/msf/1.msf");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    class OptionsTestScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("algorithmBox", dialog, "IQ-TREE");

            auto substModelEdit = GTWidget::findLineEdit("substModelEdit", dialog);
            auto ultrafastBootstrapEdit = GTWidget::findLineEdit("ultrafastBootstrapEdit", dialog);
            auto alrtEdit = GTWidget::findLineEdit("alrtEdit", dialog);
            auto ancestralReconstructionCheckBox = GTWidget::findCheckBox("ancestralReconstructionCheckBox", dialog);
            auto extraParametersTextEdit = GTWidget::findPlainTextEdit("extraParametersTextEdit", dialog);

            CHECK_SET_ERR(extraParametersTextEdit->toPlainText().isEmpty(), "extraParametersTextEdit is not empty by default");
            CHECK_SET_ERR(substModelEdit->text().isEmpty(), "substModelEdit is not empty by default");
            CHECK_SET_ERR(ultrafastBootstrapEdit->text().isEmpty(), "ultrafastBootstrapEdit is not empty by default");
            CHECK_SET_ERR(alrtEdit->text().isEmpty(), "alrtEdit is not empty by default");
            CHECK_SET_ERR(!ancestralReconstructionCheckBox->isChecked(), "ancestralReconstructionCheckBox is not unchecked by default");

            // Set values to widgets, check that text is changed.
            GTPlainTextEdit::setText(extraParametersTextEdit, "-custom c1 -m 1 -bb 2 --custom c2 c3 -alrt 3");
            GTLineEdit::setText(substModelEdit, "LM");
            GTLineEdit::setText(ultrafastBootstrapEdit, "1000");
            GTLineEdit::setText(alrtEdit, "1001");
            GTCheckBox::setChecked(ancestralReconstructionCheckBox);
            CHECK_SET_ERR(extraParametersTextEdit->toPlainText() == "-custom c1 --custom c2 c3 -m LM -bb 1000 -alrt 1001 -asr",
                          "extraParametersTextEdit is not updated with values from the inputs");

            // Empty text field - widgets must be also reset.
            GTPlainTextEdit::clear(extraParametersTextEdit);
            CHECK_SET_ERR(substModelEdit->text().isEmpty(), "substModelEdit is not empty");
            CHECK_SET_ERR(ultrafastBootstrapEdit->text().isEmpty(), "ultrafastBootstrapEdit is not empty");
            CHECK_SET_ERR(alrtEdit->text().isEmpty(), "alrtEdit is not empty by default");
            CHECK_SET_ERR(!ancestralReconstructionCheckBox->isChecked(), "ancestralReconstructionCheckBox is not unchecked");

            // Set text with parameters and check the widgets are updated
            GTPlainTextEdit::setText(extraParametersTextEdit, "-m TEST -bb 1000 -alrt 1002 -asr");
            CHECK_SET_ERR(substModelEdit->text() == "TEST", "substModelEdit is not updated");
            CHECK_SET_ERR(ultrafastBootstrapEdit->text() == "1000", "ultrafastBootstrapEdit is not updated");
            CHECK_SET_ERR(alrtEdit->text() == "1002", "alrtEdit is not updated");
            CHECK_SET_ERR(ancestralReconstructionCheckBox->isChecked(), "ancestralReconstructionCheckBox is not checked");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new OptionsTestScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::getTreeView();  // Check that tree view is opened.
}

GUI_TEST_CLASS_DEFINITION(test_7572) {
    // 1. Open HIV-1.aln
    // 2. Click the "Build Tree" button on the toolbar.
    // 3. Start building tree with Likelihood algorithm
    // 4. Cancel Tree building task
    // Expected state: no message about QProcess destructor in details log
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    class PhyMLMaximumLikelihoodScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("algorithmBox", dialog, "PhyML Maximum Likelihood");
            GTLineEdit::setText("fileNameEdit", sandBoxDir + "test_7572.nwk", dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new PhyMLMaximumLikelihoodScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");

    QString taskName = "Calculating Phylogenetic Tree";
    GTUtilsTaskTreeView::checkTaskIsPresent(taskName);
    QString taskStatus = GTUtilsTaskTreeView::getTaskStatus(taskName);
    CHECK_SET_ERR(taskStatus == "Running", "The task status is incorrect: " + taskStatus);
    GTUtilsTaskTreeView::cancelTask(taskName);
    GTUtilsTaskTreeView::waitTaskFinished();
    // We can't put it in macro because it will be auto-triggered by log message from macro itself.
    bool messageNotFound = !lt.hasMessage("QProcess: Destroyed while process");
    CHECK_SET_ERR(messageNotFound, "Message about QProcess destructor found, but shouldn't be.");
}

GUI_TEST_CLASS_DEFINITION(test_7573) {
    // Open data/samples/PDB/1CF7.PDB
    // Right click 3D Model->Molecular Surface->SAS.
    // Press Ctrl+S and save the project.
    // Press Ctrl+S many times.
    //     Expected: UGENE doesn't crash.
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new PopupChooser({"Molecular Surface", "SAS"}));
    GTMenu::showContextMenu(GTWidget::findWidget("1-1CF7"));

    GTUtilsProject::saveProjectAs(sandBoxDir + "7573/A.uprj");
    for (int i = 0; i < 50; i++) {
        GTKeyboardDriver::keyClick('S', Qt::ControlModifier);
    }
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7574) {
    // Original problem:
    // Open _common_data/pdb/1JFA_3.pdb
    // Click context menu 'Models..'
    // Enable Model N3 -> UGENE crashes.
    GTFileDialog::openFile(testDir + "_common_data/pdb/1JFA_3.pdb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooserByText({"Models.."}));
    GTMenu::showContextMenu(GTWidget::findWidget("1-1JFA"));

    auto dialog = GTWidget::findWidget("SelectModelsDialog");
    auto listWidget = GTWidget::findListWidget("modelsList", dialog);
    QStringList itemsBefore = GTListWidget::getItems(listWidget);
    CHECK_SET_ERR(itemsBefore.size() == 3, "1. Expected 3 items, got: " + QString::number(itemsBefore.size()));
    CHECK_SET_ERR(GTListWidget::isItemChecked(listWidget, "1"), "1. Item 1 must be checked");
    CHECK_SET_ERR(!GTListWidget::isItemChecked(listWidget, "2"), "1. Item 2 must not be checked");
    CHECK_SET_ERR(!GTListWidget::isItemChecked(listWidget, "3"), "1. Item 3 must not be checked");
    GTListWidget::checkAllItems(listWidget, true);
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
    GTThread::waitForMainThread();
    // UGENE must not crash.

    // Open the dialog again and check that all 3 items are checked.
    GTUtilsDialog::add(new PopupChooserByText({"Models.."}));
    GTMenu::showContextMenu(GTWidget::findWidget("1-1JFA"));

    dialog = GTWidget::findWidget("SelectModelsDialog");
    listWidget = GTWidget::findListWidget("modelsList", dialog);
    QStringList itemsAfter = GTListWidget::getItems(listWidget);
    CHECK_SET_ERR(itemsBefore.size() == 3, "2. Expected 3 items, got: " + QString::number(itemsBefore.size()));
    CHECK_SET_ERR(GTListWidget::isItemChecked(listWidget, "1"), "2. Item 1 must be checked");
    CHECK_SET_ERR(GTListWidget::isItemChecked(listWidget, "2"), "2. Item 2 must be checked");
    CHECK_SET_ERR(GTListWidget::isItemChecked(listWidget, "3"), "2. Item 3 must be checked");
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

GUI_TEST_CLASS_DEFINITION(test_7575) {
    // Check that reset-zoom action does not crash UGENE.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::zoomIn();
    GTUtilsMsaEditor::zoomIn();
    GTUtilsMsaEditor::zoomIn();

    GTUtilsMSAEditorSequenceArea::scrollToPosition({550, 1});
    GTUtilsMsaEditor::resetZoom();
    // Expected state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_7576) {
    // Check that zoom-to-selection in MSA keeps the selected region within the visible sequence area.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QList<QPoint> topLeftPoints = {{500, 5}, {603, 17}};
    QList<QPoint> bottomRightPoints = {{540, 15}, {603, 17}};

    for (int i = 0; i < topLeftPoints.size(); i++) {
        QPoint topLeft = topLeftPoints[i];
        QPoint bottomRight = bottomRightPoints[i];

        GTUtilsMSAEditorSequenceArea::selectArea(topLeft, bottomRight);
        GTUtilsMsaEditor::zoomToSelection();

        int firstVisibleBaseIndex = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
        int lastVisibleBaseIndex = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
        CHECK_SET_ERR(firstVisibleBaseIndex <= topLeft.x() && lastVisibleBaseIndex >= bottomRight.x(),
                      QString("%1.Invalid visible X range: %2:%3").arg(i).arg(firstVisibleBaseIndex).arg(lastVisibleBaseIndex));

        int firstVisibleRowIndex = GTUtilsMSAEditorSequenceArea::getFirstVisibleRowIndex();
        int lastVisibleRowIndex = GTUtilsMSAEditorSequenceArea::getLastVisibleRowIndex(true);
        CHECK_SET_ERR(firstVisibleRowIndex <= topLeft.y() && lastVisibleRowIndex >= bottomRight.y(),
                      QString("%1.Invalid visible Y range: %2:%3").arg(i).arg(firstVisibleRowIndex).arg(lastVisibleRowIndex));

        GTUtilsMsaEditor::resetZoom();
    }
}

GUI_TEST_CLASS_DEFINITION(test_7582) {
    // Check that UGENE can build a tree for a MSA with non-unique sequence names.
    GTFileDialog::openFile(testDir + "_common_data/clustal/same_name_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    class RunBuildTreeScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("algorithmBox", dialog, "MrBayes");
            GTLineEdit::setText("fileNameEdit", sandBoxDir + "test_7582.nwk", dialog);  // Set output file name.
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new RunBuildTreeScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::getTreeView();  // Check that tree view was opened.
    CHECK_SET_ERR(!lt.hasErrors(), "Found error in the log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7584) {
    // Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class OkClicker : public Filler {
    public:
        OkClicker()
            : Filler("CreateAnnotationDialog") {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(w, QDialogButtonBox::Ok);
        }
    };

    // Select Align->Align sequence to mRNA from context menu
    // In "Select Item" dialog expand  human_T1 and select corresponding sequence
    // Push OK -> "Save result to annotation" dialog appeas
    // Click Create button in "Save result to annotation" dialog
    GTUtilsDialog::waitForDialog(new OkClicker());
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("human_T1.fa", "human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ALIGN", "Align sequence to mRNA"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Remove this file by Del button as quick as possible
    GTUtilsProjectTreeView::click("Annotations");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7607) {
    // Check that UGENE can build a tree for a MSA with non-unique sequence names.
    GTFileDialog::openFile(testDir + "_common_data/clustal/align.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    class BuildTreeWithMrBayesScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("algorithmBox", dialog, "MrBayes");
            GTLineEdit::setText("fileNameEdit", sandBoxDir + "test_7607.nwk", dialog);  // Set output file name.
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new BuildTreeWithMrBayesScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();

    QString expectedTree = GTFile::readAll(testDir + "_common_data/regression/7607/test_7607_expected.nwk");
    QString actualTree = GTFile::readAll(sandBoxDir + "test_7607.nwk");
    CHECK_SET_ERR(actualTree == expectedTree, "Actual tree does not match the expected tree");
}

GUI_TEST_CLASS_DEFINITION(test_7609) {
    // Open _common_data/clustal/non_unique_row_names.aln.
    // Open and close the "Tree Settings" tab of the Options Panel.
    // Select the first sequence in the alignment.
    // Press Delete 2 times.
    // No crash.
    GTFileDialog::openFile(testDir + "_common_data/clustal/non_unique_row_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsMsaEditor::removeRows(0, 0);
    GTUtilsMsaEditor::removeRows(0, 0);
}

GUI_TEST_CLASS_DEFINITION(test_7611) {
    // Check that export 3D struct to PDF works correctly (doesn't fail with error or an empty document).
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    QString pdfFilePath = sandBoxDir + "test_7611.pdf";

    class ExportImageScenario : public CustomScenario {
    public:
        ExportImageScenario(const QString& _pdfFilePath)
            : pdfFilePath(_pdfFilePath) {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText("formatsBox", dialog, "PDF");
            GTLineEdit::setText("fileNameEdit", pdfFilePath, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

        QString pdfFilePath;
    };

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new Filler("ImageExportForm", new ExportImageScenario(pdfFilePath)));
    GTUtilsDialog::waitForDialog(new PopupChooser({"bioStruct3DExportImageAction"}));
    GTMenu::showContextMenu(GTWidget::findWidget("1-1CF7"));

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    qint64 pdfFileSize = GTFile::getSize(pdfFilePath);
    CHECK_SET_ERR(pdfFileSize > 1000 * 1000, "Invalid PDF file size: " + QString::number(pdfFileSize));
}

GUI_TEST_CLASS_DEFINITION(test_7616) {
    // Check that "Open Tree" button in MSA Editor's options panel works correctly.
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    QList<Document*> documents = AppContext::getProject()->getDocuments();
    CHECK_SET_ERR(documents.size() == 1, "Expected 1 document in project");
    Document* initialCoiNwkDocument = documents.first();

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);

    // Try non-tree file. Expected state: nothing is loaded.
    GTLogTracer lt1;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "/samples/CLUSTALW/ty3.aln.gz"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt1.hasMessage("Document contains no tree objects"), "Expected message not found");
    GTUtilsMsaEditor::checkNoTreeView();

    // Try load a tree file that is already in the project. Expected state: the document in the project is reused.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "/samples/Newick/COI.nwk"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check there is an active tree view.
    GTLogTracer lt2;
    GTUtilsMsaEditor::getTreeView();
    CHECK_SET_ERR(!lt2.hasErrors(), "Found errors in log: " + lt2.getJoinedErrorString());

    documents = AppContext::getProject()->getDocuments();
    CHECK_SET_ERR(documents.size() == 2, "Expected 2 document in project");
    CHECK_SET_ERR(documents.contains(initialCoiNwkDocument), "Expected initial tree document to be present in the project and re-used in MSA editor");
}

GUI_TEST_CLASS_DEFINITION(test_7617) {
    // 1. Build dotplot for human_T1.fa
    GTUtilsDialog::waitForDialog(new DotPlotFiller());
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(dataDir + "samples/FASTA/human_T1.fa", "", false, true));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir, "test_7617", GTFileDialogUtils::Save));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));

    // 2. Click "Remove sequence" (gray cross) on sequence widget toolbar
    // 3. Answer "Yes" in "Save dot-plot" dialog and choose valid path
    // Expected state: no crash, no errors in the log
    GTLogTracer lt;
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    QFile f(sandBoxDir + "test_7617");
    CHECK_SET_ERR(f.exists() && f.size() != 0, "Result file is not exists or empty");
}

GUI_TEST_CLASS_DEFINITION(test_7623) {
    GTLogTracer lt;

    // Select "Tools>Workflow Designer"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // Open "Trim and аlign Sanger reads" sample
    class Scenario : public CustomScenario {
    public:
        void run() override {
            // Select Reference .../test/general/_common_data/sanger/reference.gb
            GTUtilsWizard::setParameter("Reference", testDir + "_common_data/sanger/reference.gb");

            // Push Next
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            // On page "Input Sanger reads" add: .../test/general/_common_data/sanger/n_and_gaps.fa and click "Next" button
            GTUtilsWizard::setInputFiles({{testDir + QString("_common_data/sanger/n_and_gaps.fa")}});

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            //  Push Next on "Trim and Filtering" page
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Map Sanger Reads to Reference", new Scenario()));
    GTUtilsWorkflowDesigner::addSample("Trim and Map Sanger reads");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasError("All input reads contain gaps or Ns only, abort"), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_7629) {
    // 1. Open sars.gb
    GTFileDialog::openFile(dataDir + "/samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Copy 1001 symbol
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 1001));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    GTUtilsDialog::waitForDialog(new PopupChooserByText(QStringList() << "Copy/Paste"
                                                                      << "Copy selected sequence"));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();
    // 3. Paste it to project filter
    // Expected: no crash, here is info message in log and warning message box
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "The search pattern is too long. Pattern was truncated to 1000 symbols."));
    auto nameFilterEdit = GTWidget::findLineEdit("nameFilterEdit");
    GTLineEdit::setText(nameFilterEdit, GTClipboard::text(), true, true);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsLog::checkMessageWithWait(lt, "The search pattern is too long. Pattern was truncated to 1000 symbols.", 90000);

    // 4. Copy region with acceptable length 1000 symbols
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 1000));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy selected sequence"}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();
    // 5. Paste it to project filter
    // Expected: no crash, no error in log
    GTLogTracer logTracer2;
    GTUtilsTaskTreeView::openView();
    GTLineEdit::clear(nameFilterEdit);
    GTLineEdit::setText(nameFilterEdit, GTClipboard::text(), true, true);
    GTUtilsTaskTreeView::checkTaskIsPresent("Filtering project content");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!logTracer2.hasErrors(), "Log should not contain errors");
}

GUI_TEST_CLASS_DEFINITION(test_7630) {
    // Open CVU55762.gb and murine.gb in separate sequence mode.
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // In CVU55762 select region 1001-1000.
    SelectSequenceRegionDialogFiller* filler = new SelectSequenceRegionDialogFiller(1001, 1000);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(filler);
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);

    // Copy it (Cmd-C).
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Switch to murine and toggle circular views.
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsCv::commonCvBtn::click();

    // Select region 1000-5833.
    filler = new SelectSequenceRegionDialogFiller(1000, 5830);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(filler);
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);

    // Right click on the sequence->Replace subsequence... "Replace sequence" dialog appears.
    // Paste clipboard into text field (Cmd-V).
    // Press Enter.
    // Dialog closed, sequence changed. Now only 1 annotation remains (5' terminal repeat).
    class ReplaceSequenceScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto plainText = GTWidget::findPlainTextEdit("sequenceEdit", dialog);
            GTWidget::click(plainText);

            // Select the whole sequence and replace it with '='. Try applying the change.
            GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
            GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller(new ReplaceSequenceScenario()));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace subsequence..."});

    // Remove the annotation:
    //     click on it in Sequence View to select it
    //         -> right click it
    //         -> Remove
    //         -> Selected annotations and qualifiers.
    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 2, 0, true);
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_REMOVE, "Selected annotations and qualifiers"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("misc_feature"));
    GTMouseDriver::click(Qt::RightButton);

    // Open "Search in Sequence" Options Panel tab (Cmd-F).
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // Paste clipboard into pattern text field (Cmd-V).
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Was state: crash when "Find in sequence task" progress is 94% (same as in crash report).
    //            no crash handler appeared, but there is error in zsh (Terminal):
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7631) {
    // Check that file buttons on Workflow Dashboard "Inputs" tab for actors not visible by default works as expected (open the file with UGENE).
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE", GTUtilsMdi::activeWindow());
    GTKeyboardDriver::keyClick(Qt::Key_Escape);  // Cancel the wizard.

    // Go to the "Read alignment" element and set an input file.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDashboard::openTab(GTUtilsDashboard::Input);
    GTUtilsDashboard::clickLabelInParametersWidget("Write alignment");

    GTUtilsDashboard::clickFileButtonInParametersWidget("muscle_alignment.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_7633) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Click "Build Tree". Click "Display Options->Display tree in new window".
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI_7633.nwk", 0, 0, false));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");

    // Remove the tree from the project.
    GTUtilsProjectTreeView::click("COI_7633.nwk");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Switch to the COI.aln window.
    GTUtilsProjectTreeView::doubleClickItem("COI.aln");

    // Open the "Tree Setting" tab in the Options Panel and click "Open tree".
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/sandbox/COI_7633.nwk"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));

    // Expected: there is no crash, tree viewer is added.
    // TODO: uncomment after fixing UGENE-7869.
    //  GTUtilsMsaEditor::getTreeView();
}

GUI_TEST_CLASS_DEFINITION(test_7635) {
    // Checks that notification container widget contains all available notifications.
    class Create10NotificationsScenario : public CustomScenario {
    public:
        void run() override {
            auto stack = AppContext::getMainWindow()->getNotificationStack();
            for (int i = 0; i < 10; i++) {
                stack->addNotification("Notification " + QString::number(i + 1));
                GTGlobals::sleep(200);
            }
        }
    };
    GTThread::runInMainThread(new Create10NotificationsScenario());

    QString counterValue = GTUtilsNotifications::getNotificationCounterValue();
    CHECK_SET_ERR(counterValue == "10", "Invalid notification counter value: " + counterValue);

    auto containerWidget = GTUtilsNotifications::openNotificationContainerWidget();
    QList<QWidget*> notifications = GTWidget::findChildren<QWidget>(containerWidget, [](QWidget* w) { return qobject_cast<Notification*>(w) != nullptr; });
    CHECK_SET_ERR(notifications.count() == 10, "Invalid notification widgets count: " + QString::number(notifications.count()));

    // Check that counter value was not reset after the widget is opened.
    counterValue = GTUtilsNotifications::getNotificationCounterValue();
    CHECK_SET_ERR(counterValue == "10", "Invalid notification counter value: " + counterValue);
}

GUI_TEST_CLASS_DEFINITION(test_7644) {
    // Open _common_data/genbank/1seq.gen
    GTFileDialog::openFile(testDir + "_common_data/genbank/1seq.gen");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Open the OP's "Search in Sequence" tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Tabs::Search);

    // Click Actions->Edit->Switch on the editing mode.
    GTUtilsSequenceView::makePanViewVisible(false);
    GTUtilsSequenceView::makeDetViewVisible();
    GTUtilsSequenceView::enableEditingMode();

    // Select the entire sequence.
    GTUtilsSequenceView::selectSequenceRegion(1, 70);

    // Press Backspace.
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes, "Would you like"));
    GTKeyboardDriver::keyClick(Qt::Key::Key_Backspace);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7645) {
    // 1. Open file "/_common_data/primer3/7645.seq"
    // 2. Run Primer 3 with default parameters
    // Expected state: there is only two files in project view - sequence and annotations
    GTFileDialog::openFile(testDir + "/_common_data/primer3", "7645.seq");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(new Primer3DialogFiller());
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsProjectTreeView::countTopLevelItems() == 2, "two opened files expected");
}

GUI_TEST_CLASS_DEFINITION(test_7650) {
    // 1. Open samples/CLUSTALW/COI.aln
    // 2. Press 'Save as', and save file to its own path.
    // Expected state: message box with warinig appears.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsDialog::add(new MessageBoxDialogFiller("Ok"));
    GTUtilsDialog::waitForDialog(new ExportDocumentDialogFiller(dataDir + "samples/CLUSTALW/", "COI.aln", ExportDocumentDialogFiller::CLUSTALW), false, true);
    GTWidget::click(GTAction::button("Save alignment as"));
    GTUtilsProjectTreeView::click("COI.aln");
}

GUI_TEST_CLASS_DEFINITION(test_7652) {
    // 1. Open files samples/CLUSTALW/COI.aln, _common_data/ugenedb/Klebsislla.sort.bam.ugenedb
    // 2. Export consensus from Klebsislla
    // 3. Switch to COI.aln
    // 4. Do menu Actions->Add->Sequence from file...
    // 5. Do not choose file, wait until export task finishes
    // Expected state: Info message 'Unable to open view because of active modal widget.' appears in the log
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/ugenedb/Mycobacterium.sorted.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class SimpleExport : public CustomScenario {
        void run() {
            GTUtilsDialog::clickButtonBox(GTWidget::getActiveModalWidget(), QDialogButtonBox::Ok);
        }
    };
    //    Export consensus
    GTUtilsDialog::waitForDialog(new ExportConsensusDialogFiller(new SimpleExport()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export consensus..."}));
    GTWidget::click(GTWidget::findWidget("Consensus area"), Qt::RightButton);

    class WaitLogMessage : public CustomScenario {
        void run() override {
            GTUtilsTaskTreeView::waitTaskFinished();
            auto targetButton = GTWidget::findButtonByText("Cancel", GTWidget::getActiveModalWidget());
            GTWidget::click(targetButton);
        }
    };

    GTLogTracer lt;
    GTGlobals::sleep(750);  // need pause to redraw/update ui, sometimes test can't preform next action
    GTUtilsMdi::activateWindow("COI [COI.aln]");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(new WaitLogMessage()));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from file..."});
    CHECK_SET_ERR(lt.hasMessage("Unable to open view because of active modal widget."), "Expected message about not opening view not found!");
}

GUI_TEST_CLASS_DEFINITION(test_7659) {
    // 1. Open WD sampe "Call variants
    // 2. Select "Read Assembly (BAM/SAM)" worker
    // 3. Rename dataset "Dataset" -> "NewSet"
    // 4.Select "Read Sequence"
    // Expected state: dataset renamed to "NewSet" too
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Assembly (BAM/SAM)");
    GTUtilsDialog::waitForDialog(new DatasetNameEditDialogFiller("NewSet"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"rename_dataset_action"}));
    QTabBar* barWidget = GTWidget::findWidgetByType<QTabBar*>(GTUtilsWorkflowDesigner::getDatasetsListWidget(), "Can't find QTabBar widget");
    GTWidget::click(barWidget->tabButton(0, QTabBar::RightSide), Qt::RightButton);

    GTUtilsWorkflowDesigner::click("Read Sequence");
    barWidget = GTWidget::findWidgetByType<QTabBar*>(GTUtilsWorkflowDesigner::getDatasetsListWidget(), "Can't find QTabBar widget");
    CHECK_SET_ERR(barWidget->tabText(0) == "NewSet", "Actual dataset name on 'Read Sequence' worker is not expected 'NewSet'.");
}

GUI_TEST_CLASS_DEFINITION(test_7661) {
    // Duplicate _common_data/ugenedb/chrM.sorted.bam.ugenedb.
    QString origFilePath = testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb";
    GTFile::copy(origFilePath, sandBoxDir + "/chrM.sorted.bam.ugenedb");

    // Open duplicate.
    GTFileDialog::openFile(sandBoxDir, "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Type "chr" in the search field in the project view.
    GTUtilsProjectTreeView::filterProject("chr");

    // Wait for the filtration.Found 1 result.Select it.
    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchFlag::MatchContains;
    GTUtilsProjectTreeView::click("chrM", "Object name", Qt::MouseButton::LeftButton, options);

    // Click the cross in the search field in the project view.
    // Filter clearing has the same result
    GTUtilsProjectTreeView::filterProject("");

    // Close the chrM tab.
    GTMenu::clickMainMenuItem({"Actions", "Close active view"}, GTGlobals::UseKey);

    // Rename the file in the storage from "chrM.sorted.bam.ugenedb" to "Renamed.ugenedb".
    // UGENE displays the message "File Modification Detected".Click OK.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("OK", "was removed"));
    QFile f(sandBoxDir + "/chrM.sorted.bam.ugenedb");
    f.rename(sandBoxDir + "/Renamed.ugenedb");
    GTUtilsDialog::checkNoActiveWaiters();

    // Rename the file back to "chrM.sorted.bam.ugenedb".
    f.rename(sandBoxDir + "/chrM.sorted.bam.ugenedb");

    // Open it in UGENE again.
    GTFileDialog::openFile(sandBoxDir, "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: no crash
}

// Clicks the "Run Schema" menu item;
// in the "Run Schema" dialog that appears, sets
//     `inputPath` as "Load sequence" (if `inputPath` is empty, does nothing),
//     "sandbox/7667.gb" as "Save results to" (if the path of the output file is already set, does nothing),
//     checks/unchecks the "Add to project" checkbox depending on `addToProject`;
// accepts the dialog and runs the Query Designer task.
static void runSchema(const QString& inputPath = "", bool addToProject = true) {
    class RunSchemaDialogScenario : public CustomScenario {
        QString input;
        bool addToProj;

    public:
        RunSchemaDialogScenario(const QString& input, bool addToProj)
            : input(input), addToProj(addToProj) {
        }

        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            if (!input.isEmpty()) {
                GTUtilsDialog::waitForDialog(new GTFileDialogUtils(input));
                GTWidget::click(GTWidget::findToolButton("tbInFile", dialog));
            }
            {
                auto out = GTWidget::findLineEdit("outFileEdit", dialog);
                if (GTLineEdit::getText(out).isEmpty()) {
                    GTLineEdit::setText(out, UGUITest::sandBoxDir + "7667.gb");
                }
            }
            GTCheckBox::setChecked("cbAddToProj", addToProj, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::add(new Filler("RunQueryDlg", new RunSchemaDialogScenario(inputPath, addToProject)));
    GTMenu::clickMainMenuItem({"Actions", "Run Schema..."});
}

// Sets the Primer3 algorithm parameter "Number to return" to 3.
static void setNumberToReturn(QTableView* table) {
    GTTableView::click(table, 3, 1);
    GTSpinBox::setValue(GTWidget::findWidgetByType<QSpinBox*>(table, "7667-0"), 3);
}

// Sets the cell (`row`, 1) of `table` to `value`.
static void setDouble(QTableView* table, int row, double value) {
    GTTableView::click(table, row, 1);
    GTDoubleSpinbox::setValue(
        GTWidget::findWidgetByType<QDoubleSpinBox*>(table, "7667-" + QString::number(row)),
        value,
        GTGlobals::UseKeyBoard);
}

GUI_TEST_CLASS_DEFINITION(test_7667_1) {
    // Run 2 tasks one by one with different settings.

    // Open _common_data/primer3/only_primer.uql.
    //     The "Query Designer" window opens.
    // Run Schema.
    //     The "Run Schema" dialog appears.
    // Set:
    //     Load sequence      _common_data/primer3/all_settingsfiles.fa
    //     Save results to    tmp/7667.gb
    // Click Run.
    //     The all_settingsfiles.fa will open with the resulting annotations.
    // Expected: the following annotations:
    //     Result 1    850..869    complement(1022..1041)
    //     Result 2    22..41      complement(199..218)
    //     Result 3    850..869    complement(993..1012)
    //     Result 4    452..471    complement(610..629)
    //     Result 5    610..629    complement(786..805)
    // File->Close project. Don't save it.
    // Return to the "Query Designer" window.
    // Select the subunit of the Primer element on the Scene.
    //     Its parameters appear in the Property Editor.
    // Change the Parameters:
    //     Product size ranges             100-300
    //     Number to return                3
    //     Max repeat mispriming           10
    //     Max template mispriming         10
    //     Max 3' stability                10
    //     Pair max repeat mispriming      20
    //     Pair max template mispriming    20
    // Run Schema.
    //     The "Run Schema" dialog appears.
    // Click Run.
    // Expected: the following annotations:
    //     Result 1    199..218    complement(297..316)
    //     Result 2    40..59      complement(297..316)
    //     Result 3    39..58      complement(297..316)
    GTFileDialog::openFile(testDir + "_common_data/primer3/only_primer.uql");
    GTUtilsTaskTreeView::waitTaskFinished();
    runSchema(testDir + "_common_data/primer3/all_settingsfiles.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 1  (0, 2)", {{850, 869}, {1022, 1041}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 2  (0, 2)", {{22, 41}, {199, 218}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 3  (0, 2)", {{850, 869}, {993, 1012}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 4  (0, 2)", {{452, 471}, {610, 629}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 5  (0, 2)", {{610, 629}, {786, 805}});
    GTUtilsProject::closeProject(true);
    GTUtilsMdi::activateWindow("Query Designer - NewSchema");

    GTWidget::moveToAndClick(GTUtilsQueryDesigner::getItemCenter("Primer"));
    auto table = GTWidget::findTableView("table");
    {  // Product size ranges.
        GTTableView::click(table, 2, 1);
        GTLineEdit::setText(GTWidget::findWidgetByType<QLineEdit*>(table, "7667"), "100-300");
    }
    setNumberToReturn(table);
    setDouble(table, 4, 10);  // Max repeat mispriming.
    setDouble(table, 5, 10);  // Max template mispriming.
    setDouble(table, 6, 10);  // Max 3' stability.
    setDouble(table, 7, 20);  // Pair max repeat mispriming.
    setDouble(table, 8, 20);  // Pair max template mispriming.
    runSchema();
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 1  (0, 2)", {{199, 218}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 2  (0, 2)", {{40, 59}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("Result 3  (0, 2)", {{39, 58}, {297, 316}});
}

GUI_TEST_CLASS_DEFINITION(test_7667_2) {
    // Run 2 tasks at the same time with different settings.

    // Open _common_data/primer3/only_primer.uql.
    //     The "Query Designer" window opens.
    // Select the subunit of the Primer element on the Scene.
    //     Its parameters appear in the Property Editor.
    // Run Schema.
    //     The "Run Schema" dialog appears.
    // Set:
    //     Load sequence      _common_data/bwa/NC_000021.gbk.min.fa
    //     Save results to    tmp/7667.gb
    //     Add to project     ☐
    // Click Run.
    // Don't wait for the task to finish. Change the "Number to return" Property Editor parameter to 3.
    // Run Schema again.
    //     The "Run Schema" dialog appears.
    // Set _common_data/primer3/all_settingsfiles.fa as the "Load sequence".
    // Click Run.
    // Expected: both tasks completed successfully.
    GTFileDialog::openFile(testDir + "_common_data/primer3/only_primer.uql");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::moveToAndClick(GTUtilsQueryDesigner::getItemCenter("Primer"));
    runSchema(testDir + "_common_data/bwa/NC_000021.gbk.min.fa", false);

    setNumberToReturn(GTWidget::findTableView("table"));
    runSchema(testDir + "_common_data/primer3/all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7668) {
    // Open tree in MSA editor and check that sync mode works in all tree layouts.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsProjectTreeView::toggleView();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::add(new GTFileDialogUtils(dataDir + "/samples/Newick/COI.nwk"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto syncModeButton = GTAction::button("sync_msa_action");
    auto layoutCombo = GTWidget::findComboBox("layoutCombo");

    auto collapsedNodeSelector = [&](TvNodeItem* buttonNode) {
        auto branchNode = dynamic_cast<TvBranchItem*>(buttonNode->parentItem());
        auto distanceTextNode = branchNode->getDistanceTextItem();
        return distanceTextNode != nullptr && distanceTextNode->text() == "0.068";
    };
    auto selectNodeToCollapse = [&](QList<TvNodeItem*> nodes) -> TvNodeItem* {
        auto it = std::find_if(nodes.begin(), nodes.end(), collapsedNodeSelector);
        CHECK_SET_ERR_RESULT(it != nodes.end(), "Node to collapse is not found!", nullptr);
        return *it;
    };

    // Rectangular.
    QStringList allInRect = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after opening");

    QList<TvNodeItem*> treeNodes = GTUtilsPhyTree::getNodes();
    TvNodeItem* rectNodeToCollapse = selectNodeToCollapse(treeNodes);
    GTUtilsPhyTree::doubleClickNode(rectNodeToCollapse);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after collapse in Rectangular");
    QStringList collapsedInRect = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    CHECK_SET_ERR(collapsedInRect.length() == allInRect.length() - 2,
                  "Invalid count of nodes after collapse in Rectangular: " + QString::number(collapsedInRect.size()));

    // Circular.
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after switch to Circular");
    QStringList collapsedInCircular = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    CHECK_SET_ERR(collapsedInCircular == collapsedInRect,
                  QString("Nodes in Circular layout do not match: rect: '%1', circular: '%2'").arg(collapsedInRect.join(",")).arg(collapsedInCircular.join(",")));
    treeNodes = GTUtilsPhyTree::getNodes();
    TvNodeItem* circularNodeToCollapse = selectNodeToCollapse(treeNodes);
    GTUtilsPhyTree::doubleClickNode(circularNodeToCollapse);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after collapse op in Circular");
    QStringList allInCircular = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    CHECK_SET_ERR(allInCircular == allInRect, "Invalid nodes after collapse in Circular");

    // Unrooted.
    GTComboBox::selectItemByText(layoutCombo, "Unrooted");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after switch to Unrooted");
    QStringList allInUnrooted = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    CHECK_SET_ERR(allInUnrooted == allInRect, "Nodes in Unrooted layout do not match");
    treeNodes = GTUtilsPhyTree::getNodes();
    TvNodeItem* unrootedNodeToCollapse = selectNodeToCollapse(treeNodes);
    GTUtilsPhyTree::doubleClickNode(unrootedNodeToCollapse);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after collapse op in Unrooted");
    QStringList collapsedInUnrooted = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    CHECK_SET_ERR(collapsedInUnrooted == collapsedInRect, "Invalid nodes after collapse in Unrooted");

    // Rectangular (back).
    GTComboBox::selectItemByText(layoutCombo, "Rectangular");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after switch to Rectangular");
}

GUI_TEST_CLASS_DEFINITION(test_7671) {
    // I made a small file which has the same error as file from the issue,
    // because the file from the issue was almost 100 Mb size

    // Open _common_data/scenarios/_regression/7671/NC_051342_region.gb
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/7671/NC_051342_region.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Call the Primer3 dialog
    // In the Primer3 Designer dialog select PT - PCR tab
    // Check in main checkbox and set Exon range : 1424 - 1606
    // Click Pick primers button
    Primer3DialogFiller::Primer3Settings settings;
    settings.rtPcrDesign = true;
    settings.exonRangeLine = "1424-1606";

    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: no crash
}

GUI_TEST_CLASS_DEFINITION(test_7680) {
    // Check that tree buttons size remains not changed on window resize.
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();
    QList<TvNodeItem*> nodes = GTUtilsPhyTree::getNodes();
    auto node = nodes[5];

    QRect viewRectBefore = GTUtilsPhyTree::getItemViewRect(node);
    GTUtilsProjectTreeView::toggleView();
    QRect viewRectAfter = GTUtilsPhyTree::getItemViewRect(node);

    CHECK_SET_ERR(viewRectBefore.width() > 0 && viewRectBefore.height() > 0, "Initial node size is 0");

    CHECK_SET_ERR(viewRectBefore.width() == viewRectAfter.width(),
                  QString("Width of the node changed: %1 vs %2").arg(viewRectBefore.width()).arg(viewRectAfter.width()));

    CHECK_SET_ERR(viewRectBefore.height() == viewRectAfter.height(),
                  QString("Height of the node changed: %1 vs %2").arg(viewRectBefore.height()).arg(viewRectAfter.height()));
}

GUI_TEST_CLASS_DEFINITION(test_7682) {
    // Check 'curvature' controls for rectangular branches.
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();
    QWidget* optionPanel = GTUtilsOptionPanelPhyTree::openTab();

    // Check 'curvature' slider is enabled and current value is 0.
    QSlider* curvatureSlider = GTWidget::findSlider("curvatureSlider", optionPanel);
    CHECK_SET_ERR(curvatureSlider->isEnabled(), "Slider is not enabled");
    CHECK_SET_ERR(curvatureSlider->value() == 0, "By default there is no curvature");
    CHECK_SET_ERR(curvatureSlider->minimum() == 0, "Incorrect minimum curvature");
    CHECK_SET_ERR(curvatureSlider->maximum() == 100, "Incorrect maximum curvature");

    // Change slider and check that image changes too.
    QImage imageBefore = GTUtilsPhyTree::captureTreeImage();
    GTSlider::setValue(curvatureSlider, 50);
    QImage imageAfter = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageBefore != imageAfter, "Image is not changed");

    auto layoutCombo = GTWidget::findComboBox("layoutCombo", optionPanel);

    // Switch to the 'Circular', check that 'curvature' is disabled.
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    CHECK_SET_ERR(!curvatureSlider->isEnabled(), "Slider must be disabled");

    // Switch back to the 'Rectangular' and check that curvature is enabled again.
    GTComboBox::selectItemByText(layoutCombo, "Rectangular");
    CHECK_SET_ERR(curvatureSlider->isEnabled(), "Slider must be re-enabled");
    CHECK_SET_ERR(curvatureSlider->value() == 50, "Slider value must be restored, current value: " + QString::number(curvatureSlider->value()));
}

GUI_TEST_CLASS_DEFINITION(test_7686) {
    // Check "copy tree image to clipboard".
    GTFileDialog::openFile(testDir + "_common_data/newick/PF05282_full.nhx");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    GTClipboard::clear();
    GTMenu::clickMainMenuItem({"Actions", "Tree image", "Copy to clipboard"});
    GTClipboard::checkHasNonEmptyImage();

    // Zoom so image becomes very large: UGENE should show an error message.
    GTUtilsPhyTree::zoomWithMouseWheel(20);
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("OK", "Image is too large. Please zoom out."));
    GTMenu::clickMainMenuItem({"Actions", "Tree image", "Copy to clipboard"});
}

GUI_TEST_CLASS_DEFINITION(test_7697) {
    // Check that tree settings are saved/restored correctly.
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    auto panel1 = GTUtilsOptionPanelPhyTree::openTab();
    CHECK_SET_ERR(GTCheckBox::getState("showNamesCheck", panel1) == true, "Invalid default showNamesCheck state");
    GTCheckBox::setChecked("showNamesCheck", false, panel1);
    CHECK_SET_ERR(GTCheckBox::getState("showDistancesCheck", panel1) == true, "Invalid default showDistancesCheck state");
    GTCheckBox::setChecked("showDistancesCheck", false, panel1);

    auto curvatureSlider = GTWidget::findSlider("curvatureSlider", panel1);
    CHECK_SET_ERR(curvatureSlider->value() == 0, "Invalid default curvatureSlider value: " + QString::number(curvatureSlider->value()));
    GTSlider::setValue(curvatureSlider, 20);

    CHECK_SET_ERR(GTComboBox::getCurrentText("treeViewCombo", panel1) == "Default", "Invalid default treeViewCombo value");
    GTComboBox::selectItemByText("treeViewCombo", panel1, "Cladogram");

    GTUtilsMdi::closeActiveWindow();

    GTUtilsProjectTreeView::doubleClickItem("COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    auto panel2 = GTUtilsOptionPanelPhyTree::openTab();
    CHECK_SET_ERR(GTCheckBox::getState("showNamesCheck", panel2) == false, "showNamesCheck state is not restored");
    CHECK_SET_ERR(GTCheckBox::getState("showDistancesCheck", panel2) == false, "showDistancesCheck state is not restored");
    CHECK_SET_ERR(GTWidget::findSlider("curvatureSlider", panel2)->value() == 20, "curvatureSlider state is not restored");
    CHECK_SET_ERR(GTComboBox::getCurrentText("treeViewCombo", panel2) == "Cladogram", "treeViewCombo state is not restored");
}

GUI_TEST_CLASS_DEFINITION(test_7712) {
    class FilterShortScaffoldsWizard : public CustomScenario {
    public:
        void run() override {
            GTWidget::getActiveModalWidget();
            GTUtilsWizard::setInputFiles({{testDir + "_common_data/genbank/1anot.gen"}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Filter short sequences", new FilterShortScaffoldsWizard()), 60000);
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Filter short scaffolds..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    QString notification = GTUtilsDashboard::getJoinedNotificationsString();
    CHECK_SET_ERR(notification.contains("Nothing to write"), "Unexpected notification message: " + notification);
}

GUI_TEST_CLASS_DEFINITION(test_7714) {
    // Open the file 1.bam.
    // Check the box "Deselect all", "Import unmapped reads" and import the file.
    // Expected state: UGENE not crashed
    GTLogTracer lt;
    qint64 expectedReads = 10;

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_7714/test_7714.ugenedb", "", "", true, true));
    GTFileDialog::openFile(testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    qint64 assemblyReads1 = GTUtilsAssemblyBrowser::getReadsCount();
    CHECK_SET_ERR(assemblyReads1 == expectedReads, QString("An unexpected assembly reads count: expect  %1, got %2").arg(expectedReads).arg(assemblyReads1));

    GTUtilsMdi::closeActiveWindow();

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_7714/test_7714_1.ugenedb", "", "", true, true));
    GTFileDialog::openFile(testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    qint64 assemblyReads2 = GTUtilsAssemblyBrowser::getReadsCount();
    CHECK_SET_ERR(assemblyReads2 == expectedReads, QString("An unexpected assembly reads count: expect  %1, got %2").arg(expectedReads).arg(assemblyReads2));

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7715) {
    // Open COI.aln.
    //     Expected: no log messages
    //         "QObject::connect(U2::MaEditorWgt, U2::MaGraphOverview): invalid nullptr parameter
    //          QWidget::setMinimumSize: (msa_editor_sequence_area/U2::MSAEditorSequenceArea) Negative sizes (-1,-1)
    //              are not possible".
    // Select the first character.
    // Quickly and abruptly move the subalignment to the right by at least 20 characters.
    //     Expected: the Overview isn't empty, no log messages
    //         "QWidget::setMinimumSize: (msa_editor_name_list/U2::MsaEditorNameList) Negative sizes (-1,-1)
    //              are not possible
    //          QWidget::setMinimumSize: (msa_editor_sequence_area/U2::MSAEditorSequenceArea) Negative sizes (-1,-1)
    //              are not possible".
    // Click "Wrap mode".
    //     Expected: no size messages in the log.
    // Click "Remove all gaps".
    //     Expected: no size messages in the log.
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsMSAEditorSequenceArea::click();

    GTMouseDriver::press();
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(GTWidget::getWidgetCenter(GTWidget::findWidget(GTUtilsOptionPanelMsa::tabsNames[GTUtilsOptionPanelMsa::General])));
    GTMouseDriver::release();
    GTUtilsTaskTreeView::waitTaskFinished();

    // The background is white, the bars are gray, the background in the selection is light gray, the bars
    // in the selection are dark gray, the selection frame is black. Total 5 colors.
    CHECK_SET_ERR(GTWidget::countColors(GTWidget::getImage(GTUtilsMsaEditor::getGraphOverview())).size() == 5,
                  "Overview is empty (white)");

    GTUtilsMsaEditor::setMultilineMode(true);
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove all gaps"});
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasMessage("QObject::connect"), "Found unexpected message/1");
    CHECK_SET_ERR(!lt.hasMessage("QWidget::setMinimumSize)"), "Found unexpected message/2");
}

GUI_TEST_CLASS_DEFINITION(test_7720) {
    GTFileDialog::openFile(testDir + "_common_data/realign_sequences_in_alignment/", "amino_ext.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // ========================== multiline mode is off ===================
    GTUtilsMsaEditor::setMultilineMode(false);

    auto activeWindow = GTUtilsMsaEditor::getEditor()->getUI()->getUI(0);
    CHECK_SET_ERR(activeWindow != nullptr, "Sequence widget is not visible");

    auto splitter = GTWidget::findSplitter("name_and_sequence_areas_splitter", activeWindow);
    CHECK_SET_ERR(splitter != nullptr, "Splitter is not visible");

    QSplitterHandle* handle = splitter->handle(1);
    CHECK_SET_ERR(handle != nullptr, "MSA Splitter handle is NULL");

    QPoint p;
    int seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    int uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    int seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    int baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    int length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    QPoint delta(seqAreaBaseWidth / 5 * 4, 0);
    if (length * baseWidth + 50 < uiWidth) {
        delta = QPoint(seqAreaWidth - length * baseWidth - 50, 0);
    } else if (length * baseWidth < uiWidth) {
        delta = QPoint(seqAreaWidth - length * baseWidth, 0);
    } else if (length * baseWidth - 50 < uiWidth) {
        delta = QPoint(seqAreaWidth - length * baseWidth + 50, 0);
    }

    p = GTWidget::getWidgetCenter(handle);
    GTMouseDriver::dragAndDrop(p, p + delta);
    GTThread::waitForMainThread();
    GTGlobals::sleep(2000);
    GTThread::waitForMainThread();

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    // ========================== multiline mode is on ===================
    GTUtilsMsaEditor::setMultilineMode(true);
    GTGlobals::sleep(2000);

    activeWindow = GTUtilsMsaEditor::getEditor()->getUI()->getUI(0);
    CHECK_SET_ERR(activeWindow != nullptr, "Sequence widget is not visible");

    splitter = GTWidget::findSplitter("name_and_sequence_areas_splitter", activeWindow);
    CHECK_SET_ERR(splitter != nullptr, "Splitter is not visible");

    handle = splitter->handle(1);
    CHECK_SET_ERR(handle != nullptr, "MSA Splitter handle is NULL");

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    delta = QPoint(0, 0);
    int fullLength = length * baseWidth;
    int extLength = 50;

    p = GTWidget::getWidgetCenter(handle);
    GTMouseDriver::dragAndDrop(p, p + QPoint(seqAreaWidth * 5 / 3 - fullLength + 20, 0));
    GTGlobals::sleep(2000);

    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    p = GTWidget::getWidgetCenter(handle);

    if (fullLength + extLength < seqAreaWidth) {
        delta = QPoint(seqAreaWidth - fullLength - extLength, 0);
    } else if (fullLength < seqAreaWidth) {
        delta = QPoint(seqAreaWidth - fullLength, 0);
    } else if (fullLength - extLength < seqAreaWidth) {
        delta = QPoint(seqAreaWidth - fullLength + extLength, 0);
    } else {
        delta = QPoint(seqAreaWidth + extLength - uiWidth, 0);
    }

    p = GTWidget::getWidgetCenter(handle);
    GTMouseDriver::dragAndDrop(p, p + delta);
    GTGlobals::sleep(2000);

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    auto count = GTUtilsMsaEditor::getEditor()->getUI()->getChildrenCount();
    bool allAreVisible = true;
    for (int i = 0; i < count; i++) {
        auto w = GTUtilsMsaEditor::getEditor()->getUI()->getUI(i);
        CHECK_SET_ERR(w != nullptr, "Sequence widget must exist");
        allAreVisible = allAreVisible && w->isVisible();
    }
    CHECK_SET_ERR((count > 1 && !allAreVisible) || (count == 1 && allAreVisible),
                  "Some line of the multiline view must be hidden #1");

    activeWindow = GTUtilsMsaEditor::getEditor()->getUI()->getUI(0);
    CHECK_SET_ERR(activeWindow != nullptr, "Sequence widget must exist");

    splitter = GTWidget::findSplitter("name_and_sequence_areas_splitter", activeWindow);
    CHECK_SET_ERR(splitter != nullptr, "Splitter must exist");

    handle = splitter->handle(1);
    CHECK_SET_ERR(handle != nullptr, "MSA Splitter handle is NULL");

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    delta = QPoint(seqAreaWidth / 5 * 4, 0);
    if (length * baseWidth / 5 < uiWidth) {
        delta = QPoint(seqAreaWidth - length * baseWidth / 5, 0);
    }

    p = GTWidget::getWidgetCenter(handle);
    GTMouseDriver::dragAndDrop(p, p + delta);
    GTGlobals::sleep(2000);

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    count = GTUtilsMsaEditor::getEditor()->getUI()->getChildrenCount();
    allAreVisible = true;
    for (int i = 0; i < count; i++) {
        auto w = GTUtilsMsaEditor::getEditor()->getUI()->getUI(i);
        CHECK_SET_ERR(w != nullptr, "Sequence widget exists");
        allAreVisible = allAreVisible && w->isVisible();
    }
    CHECK_SET_ERR(allAreVisible, "All lines of the multiline view must be visible");

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    delta = QPoint(0, 0);
    fullLength = length * baseWidth;
    if (fullLength + extLength < seqAreaWidth) {
        delta = QPoint(seqAreaWidth - fullLength - extLength, 0);
    } else if (fullLength < seqAreaWidth) {
        delta = QPoint(seqAreaWidth - fullLength, 0);
    } else if (fullLength - extLength < seqAreaWidth) {
        delta = QPoint(seqAreaWidth - fullLength + extLength, 0);
    } else {
        delta = QPoint(seqAreaWidth + extLength - uiWidth, 0);
    }

    p = GTWidget::getWidgetCenter(handle);
    GTMouseDriver::dragAndDrop(p, p + delta);
    GTGlobals::sleep(2000);

    seqAreaBaseWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaBaseWidth(0);
    uiWidth = GTUtilsMsaEditor::getEditor()->getUI()->width();
    seqAreaWidth = GTUtilsMsaEditor::getEditor()->getUI()->getSequenceAreaWidth(0);
    baseWidth = GTUtilsMsaEditor::getEditor()->getColumnWidth();
    length = GTUtilsMsaEditor::getEditor()->getAlignmentLen();

    allAreVisible = true;
    for (int i = 0; i < count; i++) {
        auto w = GTUtilsMsaEditor::getEditor()->getUI()->getUI(i);
        CHECK_SET_ERR(w != nullptr, "Sequence widget must exist");
        allAreVisible = allAreVisible && w->isVisible();
    }
    CHECK_SET_ERR((count > 1 && !allAreVisible) || (count == 1 && allAreVisible),
                  "Some line of the multiline view must be hidden #2");
}

GUI_TEST_CLASS_DEFINITION(test_7730) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Open the OP's "Statistics" tab.
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::Statistics);

    // In this tab click the ">" button.
    GTWidget::click(GTWidget::findWidget("addSeq"));

    // Enable "Wrap mode".
    QAction* wrapMode = GTAction::findActionByText("Wrap mode");
    GTWidget::click(GTAction::button(wrapMode));

    // Сheck the "Show distances column" box.
    GTCheckBox::setChecked("showDistancesColumnCheck", true);

    // Disable "Wrap mode".
    GTWidget::click(GTAction::button(wrapMode));

    // Сheck the "Exclude gaps" box.
    GTCheckBox::setChecked("excludeGapsCheckBox", true);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: UGENE doesn't crash
}

GUI_TEST_CLASS_DEFINITION(test_7740) {
    GTFileDialog::openFile(dataDir + "samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    // Select root node.
    TvNodeItem* rootNode = GTUtilsPhyTree::getRootNode();
    GTUtilsPhyTree::clickNode(rootNode);

    // Swap Siblings button must be disabled.
    QToolBar* toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    auto swapSiblingsButton = GTToolbar::getWidgetForActionObjectName(toolbar, "Swap Siblings");
    CHECK_SET_ERR(!swapSiblingsButton->isEnabled(), "Swap siblings must be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_7744) {
    GTFileDialog::openFile(dataDir + "/samples/Genbank/sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Select "GC Deviation (G-C)/(G+C)"  or "AT Deviation (A-T)/(A+T)" graph
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Deviation (G-C)/(G+C)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new PopupChooser({"AT Deviation (A-T)/(A+T)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_7748) {
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller("", testDir + "_common_data/fasta/broken", "empty_name_multi.fa"));
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.sam");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_7751) {
    GTFileDialog::openFile(dataDir + "samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    // Select any inner node
    GTUtilsPhyTree::clickNode(GTUtilsPhyTree::getNodeByBranchText("0.009", "0.026"));

    QToolBar* toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    auto swapSiblingsButton = GTToolbar::getWidgetForActionObjectName(toolbar, "Swap Siblings");
    CHECK_SET_ERR(swapSiblingsButton->isEnabled(), "Swap siblings must be enabled");

    // Click Swapping Siblings button on the toolbar
    GTWidget::click(swapSiblingsButton);
    CHECK_SET_ERR(swapSiblingsButton->isEnabled(), "Swap siblings must be enabled");
    GTUtilsPhyTree::getNodeByBranchText("0.026", "0.009");

    GTWidget::click(swapSiblingsButton);
    CHECK_SET_ERR(swapSiblingsButton->isEnabled(), "Swap siblings must be enabled");
    GTUtilsPhyTree::getNodeByBranchText("0.009", "0.026");
}

GUI_TEST_CLASS_DEFINITION(test_7753) {
    // 1. Open "data/samples/Assembly/chrM.sorted.bam".
    // 2. Delete bam file
    // 3. Press 'imort' button in dialog
    // Expected state: you got message box with error and error in log
    class DeleteFileBeforeImport : public CustomScenario {
        void run() override {
            QFile::remove(sandBoxDir + "test_7753/chrM.sorted.bam");
            GTUtilsDialog::clickButtonBox(GTWidget::getActiveModalWidget(), QDialogButtonBox::Ok);
        }
    };
    GTLogTracer lt;
    QString sandboxFilePath = sandBoxDir + "test_7753/chrM.sorted.bam";
    QDir().mkpath(sandBoxDir + "test_7753");
    GTFile::copy(dataDir + "samples/Assembly/chrM.sorted.bam", sandboxFilePath);
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(new DeleteFileBeforeImport()));
    GTFileDialog::openFile(sandboxFilePath);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasError(QString("File %1 does not exists. Document was removed.").arg(QFileInfo(sandboxFilePath).absoluteFilePath())), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_7770) {
    GTUtilsDialog::waitForDialog(new SiteconBuildDialogFiller(testDir + "_common_data/clustal/1000_sequences.aln", sandBoxDir + "/test_7770.sitecon"));
    GTMenu::clickMainMenuItem({"Tools", "Search for TFBS", "Build SITECON model..."});
    // It is important to give a time for sitecon to warm up to reproduce the crash.
    GTGlobals::sleep(15000);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);  // Cancel the execution.
    GTUtilsTaskTreeView::waitTaskFinished(5000);  // Check the task is canceled fast enough with no crash.
}

GUI_TEST_CLASS_DEFINITION(test_7785) {
    class InSilicoWizardScenario : public CustomScenario {
    public:
        void run() override {
            GTWidget::getActiveModalWidget();

            GTUtilsWizard::setInputFiles({{QFileInfo(testDir + "_common_data/fasta/chr6.fa").absoluteFilePath()}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::setParameter("Primers URL", QFileInfo(testDir + "_common_data/regression/7785/TheSimplestPrimers.txt").absoluteFilePath());

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    // 1. Open WD and choose the "In Silico PCR" sample.
    // 2. Select "Read Sequence", add _common_data/fasta/chr6.fa
    // 3. Select "In Silico PCR" item, add "add "_common_data/regression/7785/TheSimplestPrimers.txt"
    // 4. Run
    // 5. Wait until the "Multiple In Silico PCR" task progress > 90
    // 6. Click "Stop workflow"
    // Expected state: no crash
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new InSilicoWizardScenario()));
    GTUtilsWorkflowDesigner::addSample("In Silico PCR");

    GTUtilsTaskTreeView::doubleClick("Execute workflow");
    GTUtilsTaskTreeView::doubleClick("Workflow run");

    auto stopButton = GTAction::button("Stop workflow", GTUtilsMdi::activeWindow());
    auto globalPos = stopButton->mapToGlobal(GTWidget::getWidgetVisibleCenter(stopButton));
    GTMouseDriver::moveTo(globalPos);

    GTUtilsTask::waitTaskStart("Multiple In Silico PCR");

    GTUtilsTaskTreeView::waitTaskProgressMoreThan("Multiple In Silico PCR", 90);

    GTMouseDriver::click();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_7781) {
    // Open "_common_data/scenarios/_regression/7781/7781.bam".
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "test_7781.ugenedb", "", "", false));
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/7781/7781.bam");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    auto coveredRegionsLabel = GTWidget::findLabel("CoveredRegionsLabel", GTUtilsMdi::activeWindow());
    QString textFromLabel = coveredRegionsLabel->text();
    CHECK_SET_ERR(textFromLabel.contains(">206<"), "expected coverage value not found: 206");
    CHECK_SET_ERR(textFromLabel.contains(">10<"), "expected coverage value not found: 10");
    CHECK_SET_ERR(textFromLabel.contains(">2<"), "expected coverage value not found: 2");
}

GUI_TEST_CLASS_DEFINITION(test_7789) {
    // Check that show/hide node shape option works.

    GTFileDialog::openFile(testDir + "_common_data/newick/COXII CDS tree.newick");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsPhyTree::clickZoomFitButton();

    GTUtilsOptionPanelPhyTree::openTab();

    GTCheckBox::checkState("showNodeShapeCheck", false);
    GTCheckBox::checkState("showTipShapeCheck", false);

    // ========================== rectangular layout ===================
    QImage originalRectangularImage = GTUtilsPhyTree::captureTreeImage();

    GTCheckBox::setChecked("showNodeShapeCheck", true);
    QImage imageWithNodesShapeRectangularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNodesShapeRectangularLayout != originalRectangularImage, "imageWithNodesShapeRectangularLayout != originalRectangularImage failed");

    GTCheckBox::setChecked("showNodeShapeCheck", false);
    QImage imageWithNoNodesShapeRectangularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNoNodesShapeRectangularLayout == originalRectangularImage, "imageWithNoNodesShapeRectangularLayout == originalRectangularImage failed");

    GTCheckBox::setChecked("showTipShapeCheck", true);
    QImage imageWithTipShapeRectangularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithTipShapeRectangularLayout != originalRectangularImage, "imageWithTipShapeRectangularLayout != originalRectangularImage failed");
    CHECK_SET_ERR(imageWithTipShapeRectangularLayout != imageWithNodesShapeRectangularLayout, "imageWithTipShapeRectangularLayout != imageWithNodesShapeRectangularLayout failed");

    GTCheckBox::setChecked("showTipShapeCheck", false);
    QImage imageWithNoTipShapeRectangularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNoTipShapeRectangularLayout == originalRectangularImage, "imageWithNoTipShapeRectangularLayout == originalImage failed");

    // ========================== circular layout ===================
    GTUtilsDialog::waitForDialog(new PopupChooser({"Circular"}));
    GTWidget::click(GTWidget::findWidget("Layout"));

    QImage originalCircularImage = GTUtilsPhyTree::captureTreeImage();

    GTCheckBox::setChecked("showNodeShapeCheck", true);
    QImage imageWithNodesShapeCircularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNodesShapeCircularLayout != originalCircularImage, "imageWithNodesShapeCircularLayout != originalCircularImage failed");

    GTCheckBox::setChecked("showNodeShapeCheck", false);
    QImage imageWithNoNodesShapeCircularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNoNodesShapeCircularLayout == originalCircularImage, "imageWithNoNodesShapeCircularLayout == originalCircularImage failed");

    GTCheckBox::setChecked("showTipShapeCheck", true);
    QImage imageWithTipShapeCircularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithTipShapeCircularLayout != originalCircularImage, "imageWithTipShapeCircularLayout != originalCircularImage failed");
    CHECK_SET_ERR(imageWithTipShapeCircularLayout != imageWithNodesShapeCircularLayout, "imageWithTipShapeCircularLayout != imageWithNodesShapeCircularLayout failed");

    GTCheckBox::setChecked("showTipShapeCheck", false);
    QImage imageWithNoTipShapeCircularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNoTipShapeCircularLayout == originalCircularImage, "imageWithNoTipShapeRectangularLayout == originalCircularImage failed");

    // ========================== unrooted layout ===================
    GTUtilsDialog::waitForDialog(new PopupChooser({"Unrooted"}));
    GTWidget::click(GTWidget::findWidget("Layout"));

    QImage originalUnrootedImage = GTUtilsPhyTree::captureTreeImage();

    GTCheckBox::setChecked("showNodeShapeCheck", true);
    QImage imageWithNodesShapeUnrootedLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNodesShapeUnrootedLayout != originalUnrootedImage, "imageWithNodesShapeUnrootedLayout != originalUnrootedImage failed");

    GTCheckBox::setChecked("showNodeShapeCheck", false);
    QImage imageWithNoNodesShapeUnrootedLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNoNodesShapeUnrootedLayout == originalUnrootedImage, "imageWithNoNodesShapeUnrootedLayout == originalUnrootedImage failed");

    GTCheckBox::setChecked("showTipShapeCheck", true);
    QImage imageWithTipShapeUnrootedCircularLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithTipShapeUnrootedCircularLayout != originalUnrootedImage, "imageWithTipShapeUnrootedCircularLayout != originalUnrootedImage failed");
    CHECK_SET_ERR(imageWithTipShapeUnrootedCircularLayout != imageWithNodesShapeUnrootedLayout, "imageWithTipShapeUnrootedCircularLayout != imageWithNodesShapeUnrootedLayout failed");

    GTCheckBox::setChecked("showTipShapeCheck", false);
    QImage imageWithNoTipShapeUnrootedLayout = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageWithNoTipShapeUnrootedLayout == originalUnrootedImage, "imageWithNoTipShapeUnrootedLayout == originalUnrootedImage failed");
}

GUI_TEST_CLASS_DEFINITION(test_7792) {
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();
    QWidget* optionPanel = GTUtilsOptionPanelPhyTree::openTab();

    // Check 'curvature' slider is enabled and current value is 0.
    QSlider* curvatureSlider = GTWidget::findSlider("curvatureSlider", optionPanel);
    CHECK_SET_ERR(curvatureSlider->isEnabled(), "1. Slider is not enabled");
    CHECK_SET_ERR(curvatureSlider->value() == 0, "2. By default there is no curvature");

    // Check 'expansion' slider is enabled and current value is 100.
    QSlider* expansionSlider = GTWidget::findSlider("breadthScaleAdjustmentSlider", optionPanel);
    CHECK_SET_ERR(expansionSlider->isEnabled(), "3. Slider is not enabled");
    CHECK_SET_ERR(expansionSlider->value() == 100, QString("4. By default expansion is 100: got %1").arg(expansionSlider->value()));

    // Change curvature slider and check that image changes too.
    QImage imageBefore = GTUtilsPhyTree::captureTreeImage();
    GTSlider::setValue(curvatureSlider, 50);
    QImage imageAfter1 = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageAfter1 != imageBefore, "5. Image is not changed");

    // Double-click on the slider and check that curvative is 0 and check that image changes to the original image
    GTMouseDriver::moveTo(GTWidget::getWidgetVisibleCenterGlobal(GTWidget::findWidget("curvatureSlider")));
    GTMouseDriver::doubleClick();
    CHECK_SET_ERR(curvatureSlider->value() == 0, "6. The curvature should be 0");
    QImage imageAfter2 = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageAfter2 == imageBefore, "7. Image is changed");

    // Change expansion slider and check that image changes too.
    GTSlider::setValue(expansionSlider, 200);
    QImage imageAfter3 = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageAfter3 != imageBefore, "8. Image is not changed");

    // Double-click on the expansion slider and check that expansion is 100 and check that image changes to the original one
    GTMouseDriver::moveTo(GTWidget::getWidgetVisibleCenterGlobal(GTWidget::findWidget("breadthScaleAdjustmentSlider")));
    GTMouseDriver::doubleClick();
    CHECK_SET_ERR(expansionSlider->value() == 100, "9. The expansion should be 100");
    QImage imageAfter4 = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(imageAfter4 == imageBefore, "10. Image is changed");
}

GUI_TEST_CLASS_DEFINITION(test_7797) {
    // Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select from context menu Analyze->Analyze with Query Schema menu item.
    QString fullFilePath = QFileInfo(testDir + "_common_data/query/empty.uql").absoluteFilePath();
    GTUtilsDialog::waitForDialog(new AnalyzeWithQuerySchemaDialogFiller(fullFilePath, true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Analyze", "Analyze with query schema..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: there is no crash.
}

GUI_TEST_CLASS_DEFINITION(test_7806) {
    QDir(sandBoxDir).mkdir("test_7806");
    QDir(sandBoxDir).mkdir("test_7806/1");
    QDir(sandBoxDir).mkdir("test_7806/2");

    GTFile::copy(dataDir + "samples/Assembly/chrM.fa", sandBoxDir + "/test_7806/1/chrM.fa");
    GTFile::copy(dataDir + "samples/Assembly/chrM.sam", sandBoxDir + "/test_7806/2/chrM.sam");
    IOAdapterUtils::writeTextFile(sandBoxDir + "test_7806/2/chrM.fa", "1234");

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller("", sandBoxDir + "/test_7806/1", "chrM.fa"));
    GTFileDialog::openFile(sandBoxDir + "/test_7806/2/chrM.sam");

    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    qint64 size = GTFile::getSize(sandBoxDir + "/test_7806/2/chrM.fa");
    CHECK_SET_ERR(size == 4, "chrM.fa in SAM dir is changed, size: " + QString::number(size));
}

GUI_TEST_CLASS_DEFINITION(test_7823) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsAnnotationsTreeView::expandItem("CDS  (0, 4)");

    GTMouseDriver::dragAndDrop(GTUtilsAnnotationsTreeView::getItemCenter("CDS"),
                               GTUtilsAnnotationsTreeView::getItemCenter("comment  (0, 1)"));

    GTUtilsSequenceView::clickAnnotationPan("CDS", 1042, 0, false);
}

GUI_TEST_CLASS_DEFINITION(test_7824) {
    // 1. Open 1.gb.
    // 2. Double click any annotation
    // Expected: the corresponding sequence has been selected
    //
    // 3. Click right button on the same annotation
    // Expected: the corresponding sequence is still selected
    // Current: sequence selection is gone, only annotation selection left

    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTTreeWidget::doubleClick(GTUtilsAnnotationsTreeView::findItem("B_group  (0, 2)"));
    GTTreeWidget::doubleClick(GTUtilsAnnotationsTreeView::findItem("B"));
    GTTreeWidget::click(GTUtilsAnnotationsTreeView::findItem("B"), -1, Qt::RightButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 1, "Selection size should be 1, but actual size is " + QString::number(selection.size()));
    CHECK_SET_ERR(selection.first() == U2Region(29, 91),
                  QString("Selection doesn't match with 'B' annotation it is (%1, %2) instead of (29, 91).")
                      .arg(QString::number(selection.first().startPos))
                      .arg(QString::number(selection.first().length)));
    GTTreeWidget::doubleClick(GTUtilsAnnotationsTreeView::findItem("C_group  (0, 1)"));
    QPoint cCenter = GTUtilsAnnotationsTreeView::getItemCenter("C");
    QPoint bjCenter = GTUtilsAnnotationsTreeView::getItemCenter("B_joined");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::moveTo(cCenter);
    GTMouseDriver::doubleClick();
    GTMouseDriver::moveTo(bjCenter);
    GTMouseDriver::doubleClick();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 4, "Selection size should be 4, but actual size is " + QString::number(selection.size()));
}

GUI_TEST_CLASS_DEFINITION(test_7825) {
    // 1. Open the attached sequence.
    // 2. Open Primer3 dialog
    // 3. Set "pick_discriminative_primers" task
    // 4. Set Target region to 36, 163 (annotated region)
    // 5. Set region to whole sequence
    // 6. Go to Advance settings tab and click "Pick anyway"
    // 7. Click "Pick"
    // Expected: primers frame the annotation

    GTFileDialog::openFile(testDir + "_common_data/regression/7825/seq.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.filePath = testDir + "_common_data/regression/7825/settings.txt";
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{16, 35}, {199, 218}});
}

GUI_TEST_CLASS_DEFINITION(test_7827) {
    // 1. Open samples/PDB/1CF7.PDB
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open Primer Library
    GTUtilsPrimerLibrary::openLibrary();

    // 3. Click "Import primer(s)"
    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller({}, {{"1CF7.PDB", {"1CF7 chain A sequence", "1CF7 chain B sequence", "1CF7 chain C sequence", "1CF7 chain D sequence"}}}));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Button::Import);

    // Expected: two sequences imported as primers, two declined because of alphabet
    GTUtilsNotifications::checkNotificationReportText({"A sequence: <span style=\" color:#a6392e;\">error", "B sequence: <span style=\" color:#a6392e;\">error", "C sequence: <span style=\" color:#008000;\">success", "D sequence: <span style=\" color:#008000;\">success"});
}

GUI_TEST_CLASS_DEFINITION(test_7830) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Right button-> Statistics-> Generate distance matrix
    // 3. Click "Generate" (save to a new file)
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(DistanceMatrixDialogFiller::SaveFormat::HTML, sandBoxDir + "test_7830.html"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: legend exists, and, in general, the generated report contains some expected part from "_common_data/regression/7830/test.html"
    auto generated = GTFile::readAll(sandBoxDir + "test_7830.html");
    auto expected = GTFile::readAll(testDir + "_common_data/regression/7830/test.html");
    CHECK_SET_ERR(generated.contains(expected), "Distance matfix report does not contain expected text");
}

GUI_TEST_CLASS_DEFINITION(test_7842) {
    // 1. Open "GenBank/murine.gb" and "GenBank/sars.gb"
    // 2. Click right button->Cloning->Construct molecule...
    // 3. Click "From project..."
    // 4. Try select both sequences: it should not be possible because ProjectTreeItemSelector will be instantiated with 'allowMultipleSelection=false'.
    // Expected: only one sequence could be selected, only one "Create Fragment" dialog has appeared, only one fragment added

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::add(new ProjectTreeItemSelectorDialogFiller({{"sars.gb", {"NC_004718"}}, {"murine.gb", {"NC_001363"}}}));
            GTUtilsDialog::add(new CreateFragmentDialogFiller());
            GTWidget::click(GTWidget::findWidget("fromProjectButton"));

            auto fragmentListWidget = GTWidget::findListWidget("fragmentListWidget", dialog);
            CHECK_SET_ERR(fragmentListWidget->count() == 1, QString("Unexpected fragments size, expected: 1, current: %1").arg(fragmentListWidget->count()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Construct molecule..."});
}

GUI_TEST_CLASS_DEFINITION(test_7850) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "my bookmark");

    // Scroll MSA to the middle.
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(550);

    // Update start bookmark.
    GTUtilsBookmarksTreeView::updateBookmark("my bookmark");
    int savedLeftOffset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Scroll MSA to the start.
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(1);

    // Expected state: click on the bookmark restores updated MSA position.
    GTUtilsBookmarksTreeView::doubleClickBookmark("my bookmark");

    int restoredLeftOffset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(restoredLeftOffset == savedLeftOffset,
                  QString("Bad offset: expected %1, current %2").arg(savedLeftOffset).arg(restoredLeftOffset));
}

GUI_TEST_CLASS_DEFINITION(test_7850_1) {
    GTFileDialog::openFile(dataDir + "/samples/Raw/raw.seq");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsBookmarksTreeView::addBookmark("raw [raw.seq]", "raw_seq");

    GTUtilsMdi::closeWindow("raw [raw.seq]");

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({ACTION_UPDATE_BOOKMARK}, PopupChecker::CheckOption(PopupChecker::IsDisabled)));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter("raw_seq"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_7852) {
    /*
    1. Open samples/FASTA/human_T1.fa.
    2. Open the "Statistics" tab, expand Codons.
       Current state: AAA: 16 558.
    3. Open the "Find pattern" tab, click on sequence, "Ctrl + A" -> "Go".
    4. Open the "Statistics" tab.
       Expected: AAA: 5 501.
    5. Open the "Find pattern" tab, click on sequence, "Ctrl + A", set "Min" to 1, "Max" to 100 -> "Go".
    6. Open the "Statistics" tab.
       Expected: AAA: 4
    */
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);

    auto reportPanel = GTWidget::findWidget("options_panel_codons_widget");
    GTWidget::click(reportPanel);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString codonsInfo = GTWidget::findWidgetByType<QLabel*>(reportPanel, "Failed to find label inside codons panel")->text();
    CHECK_SET_ERR(codonsInfo.contains("<td><b>AAA:&nbsp;&nbsp;</b></td><td>16 558 &nbsp;&nbsp;</td>"), "Codons info does not contain desired string 'AAA: 16 558'");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsDialog::add(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(1, 199950));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);

    reportPanel = GTWidget::findWidget("options_panel_codons_widget");
    GTWidget::click(reportPanel);
    GTUtilsTaskTreeView::waitTaskFinished();

    codonsInfo = GTWidget::findWidgetByType<QLabel*>(reportPanel, "Failed to find label inside codons panel")->text();
    CHECK_SET_ERR(codonsInfo.contains("<td><b>AAA:&nbsp;&nbsp;</b></td><td>5 501 &nbsp;&nbsp;</td>"), "Codons info does not contain desired string 'AAA: 5 501'");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsDialog::add(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller(1, 100));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);

    reportPanel = GTWidget::findWidget("options_panel_codons_widget");
    GTWidget::click(reportPanel);
    GTUtilsTaskTreeView::waitTaskFinished();

    codonsInfo = GTWidget::findWidgetByType<QLabel*>(reportPanel, "Failed to find label inside codons panel")->text();
    CHECK_SET_ERR(codonsInfo.contains("<td><b>AAA:&nbsp;&nbsp;</b></td><td>4 &nbsp;&nbsp;</td>"), "Codons info does not contain desired string 'AAA: 4'");
}

GUI_TEST_CLASS_DEFINITION(test_7853_1) {
    // 1. Open Primer Library
    // 2. Click "Import primer(s)"
    // Expected: "Add Object(s)" button is disabled
    GTUtilsPrimerLibrary::openLibrary();

    class Custom : public CustomScenario {
        void run() override {
            CHECK_SET_ERR(!GTWidget::findWidget("pbAddObject")->isEnabled(), "Add object(s) should be disabled'");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new Custom()));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Button::Import);
}

GUI_TEST_CLASS_DEFINITION(test_7853_2) {
    // 1. Open any file (e.g. human_T1.fa)
    // 2. Open Primer Library
    // 3. Click "Import primer(s)"
    // Expected: "Add Object(s)" button is enabled
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsPrimerLibrary::openLibrary();

    class Custom : public CustomScenario {
        void run() override {
            CHECK_SET_ERR(GTWidget::findWidget("pbAddObject")->isEnabled(), "Add object(s) should be enabled'");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new Custom()));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Button::Import);
}

GUI_TEST_CLASS_DEFINITION(test_7858) {
    GTFileDialog::openFile(testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();

    GTUtilsBookmarksTreeView::addBookmark("Aligned reads [alignment.ugenedb]", "Bookmark1");
    GTUtilsBookmarksTreeView::clickBookmark("Bookmark1");
    // Expected state: no crash.
}

GUI_TEST_CLASS_DEFINITION(test_7860) {
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    // Press zoom out twice.
    auto treeView = GTWidget::findWidget("treeView");

    GTUtilsPhyTree::clickZoomOutButton();
    GTUtilsPhyTree::clickZoomOutButton();
    QImage savedImage = GTWidget::getImage(treeView);

    // Create a bookmark.
    GTUtilsBookmarksTreeView::addBookmark("Tree [COI.nwk]", "Zoom-2");
    // Press Reset zoom.
    GTUtilsPhyTree::clickZoom100Button();
    // Double-click on the bookmark.
    GTUtilsBookmarksTreeView::doubleClickBookmark("Zoom-2");

    QImage restoredImage = GTWidget::getImage(treeView);

    // Expected: the tree is zoomed out twice.
    CHECK_SET_ERR(restoredImage == savedImage, "Bookmarked image is not equal expected image")
}

GUI_TEST_CLASS_DEFINITION(test_7861) {
    // Open COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Press PageDown.
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);

    // Goto 1.
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(1);

    // Expected: position 1 is visible.
    int leftOffset = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(leftOffset == 0, QString("Bad offset: expected 0, current %1").arg(leftOffset));
}

GUI_TEST_CLASS_DEFINITION(test_7863) {
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    // Switch "tree view" setting from "Default" to "Phylogram".
    GTUtilsOptionPanelPhyTree::openTab();
    auto treeView = GTWidget::findWidget("treeView");

    GTUtilsOptionPanelPhyTree::changeTreeLayout("Circular");
    GTUtilsOptionPanelPhyTree::changeBranchDepthScaleMode("Phylogram");
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage savedImage = GTWidget::getImage(treeView);

    // Create a bookmark.
    GTUtilsBookmarksTreeView::addBookmark("Tree [COI.nwk]", "Circular Phylogram");

    // Switch branch mode back to "Default".
    GTUtilsOptionPanelPhyTree::changeBranchDepthScaleMode("Default");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Activate the "Circular Phylogram" bookmark.
    GTUtilsBookmarksTreeView::doubleClickBookmark("Circular Phylogram");
    QImage restoredImage = GTWidget::getImage(treeView);

    // Expected: tree view is changed to "Phylogram"
    CHECK_SET_ERR(restoredImage == savedImage, "Bookmarked image is not equal expected image")
}

GUI_TEST_CLASS_DEFINITION(test_7867) {
    // Open In Silico PCR element in Workflow Designer
    // Melting temperature by default is Primer3 in Option Panel instead of Rough like in 46.0
    // Open Melting temperature dialog, select Rough algorithm, push OK
    // Dialog is closed, but  Primer3 is still displayed, field is selected
    // Click on Melting temperature label
    // "Rough-tm-algorithm" is displayed on the screen. It's correct.
    // Again click on "Rough-tm-algorithm"
    // Expected: Rough
    class InSilicoWizardScenario : public CustomScenario {
    public:
        void run() override {
            GTWidget::getActiveModalWidget();
            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new InSilicoWizardScenario()));
    GTUtilsWorkflowDesigner::addSample("In Silico PCR");

    GTUtilsWorkflowDesigner::click("In Silico PCR");
    auto tsPar = GTUtilsWorkflowDesigner::getParameter("Temperature settings");
    CHECK_SET_ERR(tsPar == "Primer 3", "Incorrect parameter, expected \"Primer 3\"");

    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {{GTUtilsMeltingTemperature::Parameter::Algorithm, "Rough"}};
    GTUtilsDialog::waitForDialog(new MeltingTemperatureSettingsDialogFiller(parameters));
    GTUtilsWorkflowDesigner::setParameter("Temperature settings", "", GTUtilsWorkflowDesigner::customDialogSelector);

    tsPar = GTUtilsWorkflowDesigner::getParameter("Temperature settings");
    CHECK_SET_ERR(tsPar == "Rough", "Incorrect parameter, expected \"Rough\"");
}

GUI_TEST_CLASS_DEFINITION(test_7885) {
    // Open _common_data/scenarios/_regression/7885/test_7885.aln
    // Select sequencef from 2 to the last one
    // Click Ctrl + x
    // Expected: nothing has been cut, error in the log
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/7885/", "test_7885.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::clickSequenceName("seq2");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    GTUtilsMsaEditor::clickSequenceName("seq2_1_5_2_1_1");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTLogTracer lt;
    GTKeyboardDriver::keyClick('x', Qt::ControlModifier);
    CHECK_SET_ERR(lt.hasError("Block size is too big and can't be copied into the clipboard"), "No expected error");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum() != 0, "No selected sequences");
}

}  // namespace GUITest_regression_scenarios
}  // namespace U2
