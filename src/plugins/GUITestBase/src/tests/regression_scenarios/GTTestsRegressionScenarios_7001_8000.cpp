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
#include <primitives/GTLabel.h>
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
#include <primitives/GTTextEdit.h>
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
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QRadioButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/Notification.h>

#include <U2View/ADVConstants.h>
#include <U2View/TvBranchItem.h>
#include <U2View/TvNodeItem.h>
#include <U2View/TvTextItem.h>

#include "GTTestsRegressionScenarios_7001_8000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
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
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsQueryDesigner.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "base_dialogs/MessageBoxFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportConsensusDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/plugins/annotator/FindAnnotationCollocationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/DNASequenceGeneratorDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastLocalSearchDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/TrimmomaticDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DatasetNameEditDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/MAFFT/MAFFTSupportRunDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
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
        void run(GUITestOpStatus& os) override {
            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            QString toolPath = testDir + "_common_data/regression/7003/dumb.";
            toolPath += isOsWindows() ? "cmd" : "sh";

            AppSettingsDialogFiller::setExternalToolPath(os, "python", QFileInfo(toolPath).absoluteFilePath());
            CHECK_SET_ERR(!AppSettingsDialogFiller::isExternalToolValid(os, "python"),
                          "Python module is expected to be invalid, but in fact it is valid")

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new CheckPythonInvalidation()));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."}, GTGlobals::UseMouse);
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

    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7014.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMdi::closeWindow(os, "DQB1_exon4 [DQB1_exon4.nexus]");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Expected state: the saved sub-alignment is opened. Check the content.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = {"LR882519 exotic DQB1", "LR882531 local DQB1", "LR882507 local DQB1", "LR882509 local DQB1"};
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));

    int msaLength = GTUtilsMSAEditorSequenceArea::getLength(os);
    CHECK_SET_ERR(msaLength == 5, "Unexpected exported alignment length: " + QString::number(msaLength));
}

GUI_TEST_CLASS_DEFINITION(test_7022) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/7022/test_7022.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Turn on "Wrap mode" and click on the first annotation in DetView.
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    if (!wrapMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, wrapMode));
    }
    GTUtilsSequenceView::clickAnnotationDet(os, "Misc. Feature", 2);

    // Copy selected annotation.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Copy/Paste", "Copy annotation sequence"}));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString expected = "TGTCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGGTTACCCACAAAGGGAAGCCCATCAGAC";
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text == expected, QString("Unexpected annotation, expected: %1, current: %2").arg(expected).arg(text));
}

GUI_TEST_CLASS_DEFINITION(test_7043) {
    // Check that you see 3D struct is rendered correctly.
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto biostructWidget = GTWidget::findWidget(os, "1-1CF7");
    QImage image1 = GTWidget::getImage(os, biostructWidget);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }

    // Usually 875 colors are drawn for 1CF7.pdb.
    CHECK_SET_ERR(colors.size() > 100, "Biostruct was not drawn or error label wasn't displayed, number of colors: " + QString::number(colors.size()));

    // There must be no error message on the screen.
    auto errorLabel = GTWidget::findLabel(os, "opengl_initialization_error_label", nullptr, {false});
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
    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(os, saveSubalignmentDialogFiller);
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
    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));
    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7044.aln");
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state : new alignment where s1, s1_1 and s2 are present.
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QStringList expectedNameList = {"s1", "s1_1", "s2"};
    CHECK_SET_ERR(nameList == expectedNameList, "Unexpected name list in the exported alignment: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7091) {
    // The test compares images of UGENE's main window before and after "Preferences" dialog is closed.
    QWidget* mainWindow = QApplication::activeWindow();
    QImage initialImage = GTWidget::getImage(os, mainWindow);

    // The scenario does nothing and only closes the dialog.
    class NoOpScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
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
        void run(GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            auto currentCombobox = GTWidget::findComboBox(os, "algorithmBox", dialog);
            GTComboBox::selectItemByText(os, currentCombobox, "PhyML Maximum Likelihood");

            currentCombobox = GTWidget::findComboBox(os, "subModelCombo", dialog);
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

GUI_TEST_CLASS_DEFINITION(test_7126) {
    // Check that MSA re-ordered by tree is copied to clipboard using the visual row order.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7127");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::selectRows(os, 0, 17);

    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList lines = GTClipboard::text(os).split("\n");
    CHECK_SET_ERR(lines[0].startsWith("CLUSTAL W"), "Unexpected line 0: " + lines[0]);
    CHECK_SET_ERR(lines[1].trimmed().isEmpty(), "Unexpected line 1: " + lines[1]);
    CHECK_SET_ERR(lines[2].startsWith("Isophya_altaica_EF540820"), "Unexpected line 2: " + lines[2]);
    CHECK_SET_ERR(lines[3].startsWith("Bicolorana_bicolor_EF540830"), "Unexpected line 3: " + lines[3]);
    CHECK_SET_ERR(lines[4].startsWith("Roeseliana_roeseli"), "Unexpected lines 4: " + lines[4]);
    CHECK_SET_ERR(lines[5].startsWith("Montana_montana"), "Unexpected lines 5: " + lines[5]);
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
        void run(GUITestOpStatus& os) override {
            QString toolPath = AppSettingsDialogFiller::getExternalToolPath(os, "MAFFT");
            GTFile::copyDir(os, toolPath.remove("mafft.bat"), mafftDir);
            AppSettingsDialogFiller::setExternalToolPath(os, "MAFFT", QFileInfo(mafftPath).absoluteFilePath());
            GTUtilsTaskTreeView::waitTaskFinished(os);

            toolPath = AppSettingsDialogFiller::getExternalToolPath(os, "MAFFT");
            bool isValid = AppSettingsDialogFiller::isExternalToolValid(os, "MAFFT");
            CHECK_SET_ERR(isValid, QString("MAFFT with path '%1' is expected to be valid, but in fact it is invalid").arg(toolPath));
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }

    private:
        QString mafftDir;
        QString mafftPath;
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new SetMafft(mafftDirToRemove, mafftPathToRemove)));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."}, GTGlobals::UseMouse);

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTLogTracer logTracer;
    GTFile::removeDir(mafftDirToRemove);
    GTUtilsDialog::waitForDialog(os, new MAFFTSupportRunDialogFiller(os, new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_ALIGN, "Align with MAFFT"}));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);

    GTUtilsLog::checkContainsError(os, logTracer, QString("External tool '%1' doesn't exist").arg(QFileInfo(mafftPathToRemove).absoluteFilePath()));
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

    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, true));
    GTFileDialog::openFileWithDialog(os, dataDir + "samples/ACE", "BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::closeProject(os, true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<QLabel*> labels = GTWidget::findLabelByText(os, "- BL060C3.ace");

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 1; i <= 20; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "test_7151").absoluteFilePath();

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, {"Tools", "Sanger data analysis", "Map reads to reference..."});

    class WaitInSelectFormatDialog : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, new WaitInSelectFormatDialog()));
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

GUI_TEST_CLASS_DEFINITION(test_7154) {
    // 1. Open "_common_data/genbank/Smc3_LOCUS_19_45436_bp_DNA_HTG_4_changed.gbk".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/Smc3_LOCUS_19_45436_bp_DNA_HTG_4_changed.gbk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Create annotation #1
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "grpA", "annA", "complement(10.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // 3. Create annotations #2
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "grpB", "annB", "complement(30.. 40)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // 7. Drag&drop annotation #1 to group #2
    QTreeWidgetItem* annA = GTUtilsAnnotationsTreeView::findItem(os, "annA");
    QTreeWidgetItem* annB = GTUtilsAnnotationsTreeView::findItem(os, "annB");
    QTreeWidgetItem* grpA = annA->parent();
    QTreeWidgetItem* grpB = annB->parent();
    QPoint pointA = GTUtilsAnnotationsTreeView::getItemCenter(os, "annA");
    QPoint pointGrpA = GTTreeWidget::getItemCenter(os, grpA);
    QPoint pointGrpB = GTTreeWidget::getItemCenter(os, grpB);
    GTThread::waitForMainThread();
    GTMouseDriver::dragAndDrop(pointA, pointGrpB);

    // 8. Drag&drop group #1 to group #2
    pointGrpA = GTTreeWidget::getItemCenter(os, grpA);
    pointGrpB = GTTreeWidget::getItemCenter(os, grpB);
    GTThread::waitForMainThread();
    GTMouseDriver::dragAndDrop(pointGrpA, pointGrpB);

    // Expected: group moved successfully, no crash
    GTGlobals::FindOptions findOpt(false, Qt::MatchContains);
    QTreeWidgetItem* itemGrpA = GTUtilsAnnotationsTreeView::findItem(os, "grpA", nullptr, findOpt);
    CHECK_SET_ERR(itemGrpA != nullptr, QString("Can't find item grpA"));
    QTreeWidgetItem* parentGrpA = itemGrpA->parent();
    CHECK_SET_ERR(parentGrpA != nullptr, QString("Parent of the grpA was not found"));
    annA = GTUtilsAnnotationsTreeView::findItem(os, "annA");
    annB = GTUtilsAnnotationsTreeView::findItem(os, "annB");
    grpA = annA->parent();
    grpB = annB->parent();
    CHECK_SET_ERR(grpA == grpB && grpA == parentGrpA,
                  QString("Parent of the grpA, annA, annB must be the same"));
}

GUI_TEST_CLASS_DEFINITION(test_7161) {
    class ItemPopupChooserByPosition : public PopupChooser {
        // for some reason PopupChooser don not work properly, so we choose item by position
    public:
        ItemPopupChooserByPosition(HI::GUITestOpStatus& os, int _pos)
            : PopupChooser(os, {}), pos(_pos) {
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
        ChooseCDSAndCommentsWithin60kRegion(HI::GUITestOpStatus& os)
            : FindAnnotationCollocationsDialogFiller(os) {
        }

        void run() override {
            QToolButton* plusButton = getPlusButton();

            GTUtilsDialog::waitForDialog(os, new ItemPopupChooserByPosition(os, 3));
            GTWidget::click(os, plusButton);

            GTUtilsDialog::waitForDialog(os, new ItemPopupChooserByPosition(os, 3));
            GTWidget::click(os, plusButton);

            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTSpinBox::setValue(os, "regionSpin", 60000, GTGlobals::UseKeyBoard, dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    // 1. Open data/samples/sars.gb
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Use context menu: {Analyze -> Find annotated regions}
    // 3. Click plus button, select "comment", repeat and select "cds"
    // 4. Set "Region size" to 60000
    // 5. Press "Search button"
    // Expected state: no crash or assert on run
    auto* toolbar = GTToolbar::getToolbar(os, "mwtoolbar_activemdi");
    auto* farButton = GTToolbar::getWidgetForActionTooltip(os, toolbar, "Find annotated regions");

    GTUtilsDialog::waitForDialog(os, new ChooseCDSAndCommentsWithin60kRegion(os));
    GTWidget::click(os, farButton);
}

GUI_TEST_CLASS_DEFINITION(test_7183) {
    class ExportSequencesScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTRadioButton::click(os, GTWidget::findRadioButton(os, "bothStrandsButton", dialog));
            GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "translateButton", dialog), true);
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
        GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
        GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, new ExportSequencesScenario()));
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

GUI_TEST_CLASS_DEFINITION(test_7191) {
    /*
     * 1. Open data/samples/sars.gb
     * 2. Delete sequence object
     * 3. Export annotation object
     * Expected state: there is no errors in the log
     */
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::click(os, "NC_004718");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_PROJECT__REMOVE_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "test_7191.gb", ExportAnnotationsFiller::ugenedb, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export/Import", "Export annotations..."}));
    GTUtilsProjectTreeView::callContextMenu(os, "NC_004718 features");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7193) {
    GTUtilsPcr::clearPcrDir(os);
    // 1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "GGAAAAAATGCTAAGGGC" and "CTGGGTTGAAAATTCTTT".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "GGAAAAAATGCTAAGGGC");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "CTGGGTTGAAAATTCTTT");
    // 4. Set both mismatches to 9
    GTUtilsPcr::setMismatches(os, U2Strand::Direct, 9);
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 9);
    // 5. Set 3' perfect match to 3
    auto perfectSpinBox = GTWidget::findSpinBox(os, "perfectSpinBox");
    GTSpinBox::setValue(os, perfectSpinBox, 3, GTGlobals::UseKeyBoard);

    // 6. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsPcr::productsCount(os) == 22, QString("Expected 22 result instead of %1").arg(QString::number(GTUtilsPcr::productsCount(os))));
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
    QString documentName = GTUtils::genUniqueString("PairwiseAlignmentResult");
    GTUtilsOptionPanelMsa::setOutputFile(os, sandBoxDir + documentName + ".aln");

    GTWidget::click(os, GTUtilsOptionPanelMsa::getAlignButton(os));
    GTUtilsOptionPanelMsa::toggleTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::removeDocument(os, documentName);
    GTUtilsProjectTreeView::doubleClickItem(os, "shortened_big.aln");
    GTUtilsOptionPanelMsa::toggleTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTWidget::click(os, GTUtilsOptionPanelMsa::getAlignButton(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7234) {
    class InSilicoWizardScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
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
    QWidget* tabWidget = GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
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
        void run(GUITestOpStatus& os) override {
            GTUtilsWizard::setInputFiles(os, {{dataDir + "samples/FASTA/human_T1.fa"}});
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Apply);
        }
    };
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::add(os, new WizardFiller(os, "Remote BLASTing Wizard", new RemoteBlastWizardScenario()));
    GTUtilsWorkflowDesigner::addSample(os, "Remote BLASTing");
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, "Save"));
    GTUtilsDialog::add(os, new WorkflowMetaDialogFiller(os, testDir + "_common_data/scenarios/sandbox/7247.uwl", "7247"));
    GTUtilsMdi::click(os, GTGlobals::Close);
}

GUI_TEST_CLASS_DEFINITION(test_7276) {
    // Check that selection and sequence order does not change after KAlign alignment.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Insert space into some sequence to check that the final alignment algorithm is not no-op.
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, {0, 0});
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString unalignedSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    QStringList sequenceNameList1 = GTUtilsMSAEditorSequenceArea::getNameList(os);

    QString sequenceName = "Phaneroptera_falcata";
    GTUtilsMsaEditor::clickSequenceName(os, sequenceName);
    GTUtilsMsaEditor::checkSelectionByNames(os, {sequenceName});

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Sort", "By name"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that the sequence is still selected, but the list is re-ordered.
    GTUtilsMsaEditor::checkSelectionByNames(os, {sequenceName});
    QStringList sequenceNameList2 = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(sequenceNameList2 != sequenceNameList1, "Name list must change as the result of sorting");

    // Align with KAlign now.
    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTUtilsDialog::add(os, new KalignDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that sequence is still selected and the list is not re-ordered.
    GTUtilsMsaEditor::checkSelectionByNames(os, {sequenceName});
    QStringList sequenceNameList3 = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(sequenceNameList3 == sequenceNameList2, "Name list should not change as the result of alignment");

    // Check that the space inserted before was fixed by the alignment algorithm.
    int newIndex = sequenceNameList2.indexOf(sequenceName);
    QString alignedSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, newIndex);
    CHECK_SET_ERR(alignedSequence.left(20) == unalignedSequence.mid(1).left(20), "Aligned sequence must match the original sequence");
}

GUI_TEST_CLASS_DEFINITION(test_7279) {
    // Check that UGENE prints a detailed error message in case if input parameters are invalid.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTLogTracer logTracer;
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "test_7279.nwk", 2, 99.99));
    GTUtilsMsaEditor::clickBuildTreeButton(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(logTracer.getJoinedErrorString().contains("Failed to compute distance matrix: distance matrix contains infinite values"),
                  "Expected error message is not found");
}

GUI_TEST_CLASS_DEFINITION(test_7293) {
    // Open a multi-byte unicode file that triggers format selection dialog with a raw data preview.
    // Check that raw data is shown correctly for both Open... & Open As... dialog (these are 2 different dialogs).

    class CheckDocumentReadingModeSelectorTextScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto textEdit = GTWidget::findPlainTextEdit(os, "previewEdit", dialog);
            QString previewText = textEdit->toPlainText();
            CHECK_SET_ERR(previewText.contains("Первый"), "Expected text is not found in previewEdit");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, new CheckDocumentReadingModeSelectorTextScenario()));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/utf16be.fa"));
    GTMenu::clickMainMenuItem(os, {"File", "Open..."});
    GTUtilsDialog::checkNoActiveWaiters(os);

    // Now check preview text for the second dialog.
    class CheckDocumentFormatSelectorTextScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto textEdit = GTWidget::findPlainTextEdit(os, "previewEdit", dialog);
            QString previewText = textEdit->toPlainText();
            CHECK_SET_ERR(previewText.contains("Первый"), "Expected text is not found in previewEdit");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, new CheckDocumentFormatSelectorTextScenario()));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/utf16be.fa"));
    GTMenu::clickMainMenuItem(os, {"File", "Open as..."});
}

GUI_TEST_CLASS_DEFINITION(test_7338) {
    // 1. Open and import _common_data/bam/NoAssemblies.bam, with "Import empty reads" checked.
    // 2. Close project
    // 3. Repeat step 1
    // Expected state: no crash
    GTUtilsDialog::add(os, new ImportBAMFileFiller(os, sandBoxDir + "test_7338_1.ugenedb", "", "", true));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/NoAssemblies.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::add(os, new ImportBAMFileFiller(os, sandBoxDir + "test_7338_2.ugenedb", "", "", true));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/NoAssemblies.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
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

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "too large"));
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

    QScrollBar* scrollBar = GTUtilsMcaEditor::getVerticalScrollBar(os);
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

GUI_TEST_CLASS_DEFINITION(test_7388) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/align_subalign.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtils::checkExportServiceIsEnabled(os);

    // Export subalignment with only gaps inside.
    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_EXPORT, "Save subalignment"}, GTGlobals::UseMouse));

    auto saveSubalignmentDialogFiller = new ExtractSelectedAsMSADialogFiller(os, sandBoxDir + "test_7388.aln", {"s1", "s2"}, 16, 24);
    saveSubalignmentDialogFiller->setUseDefaultSequenceSelection(true);
    GTUtilsDialog::add(os, saveSubalignmentDialogFiller);
    GTMenu::showContextMenu(os, GTUtilsMsaEditor::getSequenceArea(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select both sequences with only gaps inside.
    GTUtilsMdi::checkWindowIsActive(os, "test_7388");
    GTUtilsMsaEditor::selectRows(os, 0, 1);

    // Check that "Copy" works as expected.
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Copy/Paste", "Copy"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    QString clipboardText1 = GTClipboard::text(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(clipboardText1 == "---------\n---------",
                  "1. Unexpected clipboard text: " + clipboardText1);

    // Check that "Copy (custom format)" works as expected.
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString clipboardText2 = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText2 == "CLUSTAL W 2.0 multiple sequence alignment\n\ns1   --------- 9\ns2   --------- 9\n              \n\n",
                  "2. Unexpected clipboard text: " + clipboardText2);
}

GUI_TEST_CLASS_DEFINITION(test_7390) {
    // 1. Set SPAdes to any file
    // Expected: SPAdes is invalid
    class SetSpades : public CustomScenario {
        void run(GUITestOpStatus& os) override {
            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            QString toolPath = dataDir + "samples/FASTA/human_T1.fa";

            AppSettingsDialogFiller::setExternalToolPath(os, "SPAdes", QFileInfo(toolPath).absoluteFilePath());
            CHECK_SET_ERR(!AppSettingsDialogFiller::isExternalToolValid(os, "SPAdes"),
                          "SPAdes is expected to be invalid, but in fact it is valid");

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new SetSpades()));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."}, GTGlobals::UseMouse);

    // 2. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

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
        void run(HI::GUITestOpStatus& os) override {
            //    Expected state: wizard has appeared.
            QWidget* wizard = GTWidget::getActiveModalWidget(os);
            GTWidget::clickWindowTitle(os, wizard);

            GTUtilsWizard::setInputFiles(os, {{dataDir + "samples/FASTA/human_T1.fa"}});
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTWidget::click(os, GTWidget::findToolButton(os, "trimmomaticPropertyToolButton", wizard));
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Apply);
        }
    };

    GTUtilsDialog::waitForDialog(os, new TrimmomaticDialogFiller(os, steps));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Illumina SE Reads De Novo Assembly Wizard", new ProcessWizard));
    GTUtilsWorkflowDesigner::addSample(os, "De novo assemble Illumina SE reads");

    // 10. Validate workflow
    // Expected: no crash
    GTUtilsWorkflowDesigner::validateWorkflow(os);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

GUI_TEST_CLASS_DEFINITION(test_7401) {
    // 1. Open human_T1.fa.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select any part of sequence.
    PanView* panView = GTUtilsSequenceView::getPanViewByNumber(os);
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

GUI_TEST_CLASS_DEFINITION(test_7402) {
    // Check that MSA Toolbar→Actions→Export→Export-Selected-Sequences action is present and works as expected.

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/protein.fasta.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Check that menu is present and disabled.
    GTMenu::checkMainMenuItemState(os, {"Actions", "Export", "Move selected rows to another alignment"}, PopupChecker::IsDisabled);

    // Select a row, check that menu is enabled, export the selected row.
    GTUtilsMsaEditor::selectRowsByName(os, {"Whale"});
    GTMenu::clickMainMenuItem(os, {"Actions", "Export", "Move selected rows to another alignment", "COI [COI.aln]"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList sourceNameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(!sourceNameList.contains("Whale"), "Row was not removed from the source MSA");

    GTUtilsMdi::activateWindow(os, "COI.aln");
    QStringList targetNameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(targetNameList.contains("Whale"), "Row was not added to the target MSA");
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

GUI_TEST_CLASS_DEFINITION(test_7410) {
    // Check sequence generation dialog uses correct file extension for MSA and generates MSA, not a sequence.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7410");
    model.formatId = BaseDocumentFormats::CLUSTAL_ALN;
    model.numberOfSequences = 3;
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 3, "Invalid number of sequence in the alignment");

    GTUtilsProjectTreeView::checkItem(os, "test_7410.aln");
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

    auto checkUniformDistribution = [&model, &os]() {
        GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
        GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});

        GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
        QString sequence = GTUtilsSequenceView::getSequenceAsString(os);

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

GUI_TEST_CLASS_DEFINITION(test_7419) {
    // Copy "_common_data/ugenedb/murine.ugenedb" to sandbox
    GTFile::copy(os, testDir + "_common_data/ugenedb/murine.ugenedb", sandBoxDir + "test_7419.ugenedb");

    // Open the copied file
    GTFileDialog::openFile(os, sandBoxDir + "test_7419.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Now remove it from the disk manually
    GTLogTracer lt;
    QFile::remove(sandBoxDir + "test_7419.ugenedb");

    // Expected: the message box about lost database -> click OK -> view is closed.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "The document 'test_7419.ugenedb' was removed from its original folder. Therefore, it will be deleted from the current project"));

    // The document update happens each 3 seconds, the messagebox will appear on the closest update
    GTUtilsDialog::checkNoActiveWaiters(os, 3000);

    // Expected: no safe points
    auto joinedErrorList = lt.getJoinedErrorString();
    CHECK_SET_ERR(!joinedErrorList.contains("Trying to recover from error"), "Unexpected SAFE_POINT has appeared");
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
                  QString("Illegal first result coordinates: " + GTUtilsText::rectToString(selectedRect)));

    // Press 'Next', move to the next result.
    GTUtilsOptionPanelMsa::clickNext(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(21, 0, 3, 1),
                  QString("Illegal second result coordinates: " + GTUtilsText::rectToString(selectedRect)));

    // Enter illegal 'M' character: check that there is a warning and no results in the list.
    auto patternEdit = GTWidget::findTextEdit(os, "textPattern");
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
                  QString("Illegal first (2) result coordinates: " + GTUtilsText::rectToString(selectedRect)));
}

GUI_TEST_CLASS_DEFINITION(test_7448_1) {
    // Check that "Export sequence of selected annotations..." does not generate error messages.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsSequenceView::clickAnnotationPan(os, "misc_feature", 2);

    GTUtilsDialog::waitForDialog(os,
                                 new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                               sandBoxDir + "murine_out.fa",
                                                                               ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                               ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
                                                                               0,
                                                                               true,
                                                                               false,
                                                                               GTGlobals::UseMouse,
                                                                               true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there is no log message "Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries"
    GTLogTracer::checkMessage("Sequences of the selected annotations can't be exported. At least one of the annotations is out of boundaries");
}

GUI_TEST_CLASS_DEFINITION(test_7448_2) {
    // Check that "Export sequence of selected annotations..."  for multi-big-chunks import mode works correctly.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/5mbf.fa.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "", "1..5000000"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsSequenceView::clickAnnotationPan(os, "misc_feature", 1);

    GTUtilsDialog::waitForDialog(os,
                                 new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                               sandBoxDir + "test_7448_2_out.fa",
                                                                               ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                               ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
                                                                               0,
                                                                               true,
                                                                               false,
                                                                               GTGlobals::UseMouse,
                                                                               true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the last 3 symbols are RAG.
    QString currentString = GTUtilsSequenceView::getEndOfSequenceAsString(os, 3);
    CHECK_SET_ERR(currentString == "RAG", "Last 3 symbols expected: RAG, current: " + currentString);
}

GUI_TEST_CLASS_DEFINITION(test_7448_3) {
    // Check that "Export sequence of selected annotations..."  for multi-big-chunks import mode works correctly for complementary mode.

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/5mbf.fa.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "", "complement(1..5000000)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsSequenceView::clickAnnotationPan(os, "misc_feature", 1);

    GTUtilsDialog::waitForDialog(os,
                                 new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                               sandBoxDir + "test_7448_3_out.fa",
                                                                               ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                               ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
                                                                               0,
                                                                               true,
                                                                               false,
                                                                               GTGlobals::UseMouse,
                                                                               true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the first 3 symbols are TPA.
    QString currentString = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 3);
    CHECK_SET_ERR(currentString == "TPA", "Last 3 symbols expected: TPA, current:" + currentString);

    GTUtilsSequenceView::clickMouseOnTheSafeSequenceViewArea(os);

    // Expected: the last 3 symbols are ILD.
    currentString = GTUtilsSequenceView::getEndOfSequenceAsString(os, 3);
    CHECK_SET_ERR(currentString == "ILD", "Last 3 symbols expected: ILD, current: " + currentString);
}

GUI_TEST_CLASS_DEFINITION(test_7448_4) {
    // Check that multi-region/multi-frame DNA annotation is translated correctly (DNA is joined first, translated next).
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "", "join(10..16,18..20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 10);

    GTUtilsDialog::waitForDialog(os,
                                 new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                               sandBoxDir + "test_7448_4_out.fa",
                                                                               ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                               ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
                                                                               0,
                                                                               true,
                                                                               false,
                                                                               GTGlobals::UseMouse,
                                                                               true));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString exportedSequence = GTUtilsSequenceView::getSequenceAsString(os);
    CHECK_SET_ERR(exportedSequence == "SPS", "Sequence not matched: " + exportedSequence);
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

GUI_TEST_CLASS_DEFINITION(test_7454) {
    // Open data/samples/PDB/1CF7.PDB.
    // Increase the width of the Project View.
    //     Expected: the current Sequence View is narrow.
    // Find the action toolbar extension for the first sequence in the Sequence View (">>" button). Press ">>"->
    //         "X Remove sequence".
    //     Expected: no crash.
    GTUtilsProject::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QRect rect = GTWidget::findWidget(os, "project_view")->geometry();
    QPoint splitterCenter =
        GTWidget::findWidget(os, "project_view")->mapToGlobal({rect.right() + 4, rect.center().y()});
    QPoint delta(GTMainWindow::getMainWindowWidgetByName(os, "main_window")->width() * 0.6, 0);
    GTMouseDriver::dragAndDrop(splitterCenter, splitterCenter + delta);

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Remove sequence"}));
    GTWidget::click(os,
                    GTWidget::findWidget(os,
                                         "qt_toolbar_ext_button",
                                         GTWidget::findToolBar(os, "views_tool_bar_1CF7 chain A sequence")));
}

GUI_TEST_CLASS_DEFINITION(test_7455) {
    // 1. Open "_common_data/regression/7455/clipboard.gb"
    GTFileDialog::openFile(os, testDir + "_common_data/regression/7455/clipboard.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Open the "Find restriction sites" dialog, choose "AaaI" (vary first one) only and click OK.
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, {"AaaI"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Right click -> Cloning -> Digest into fragments...
    class DigestScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            // 4. Select "AaaI" and click "Add---->"
            // 5. Go to the "Conserved annotations" tab
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto availableEnzymeWidget = GTWidget::findListWidget(os, "availableEnzymeWidget", dialog);
            QList<QListWidgetItem*> items = availableEnzymeWidget->findItems("AaaI : 2 cut(s)", Qt::MatchExactly);
            CHECK_SET_ERR(items.size() == 1, "Unexpected number of enzymes");

            GTListWidget::click(os, availableEnzymeWidget, "AaaI : 2 cut(s)");
            GTWidget::click(os, GTWidget::findWidget(os, "addButton", dialog));
            GTTabWidget::clickTab(os, "tabWidget", dialog, 1);

            class SelectAnnotationScenario : public CustomScenario {
            public:
                void run(HI::GUITestOpStatus& os) override {
                    // 6. Click "Add", choose the only option and click "OK".
                    QWidget* dialog = GTWidget::getActiveModalWidget(os);
                    auto selectAnnotationsList = GTWidget::findWidgetByType<QListWidget*>(os, dialog, "Cant find the \"Select annotations\" list");
                    auto items = GTListWidget::getItems(os, selectAnnotationsList);
                    CHECK_SET_ERR(items.size() == 1, "Unexpected number of annotations");

                    GTListWidget::click(os, selectAnnotationsList, items.first());
                    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
                }
            };

            GTUtilsDialog::waitForDialog(os, new Filler(os, "select_annotations_dialog", new SelectAnnotationScenario()));

            // 7. Click "OK"
            GTWidget::click(os, GTWidget::findWidget(os, "addAnnBtn", dialog));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os, new DigestScenario()));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Cloning", "Digest into fragments..."}));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the task finished with an error: Conserved annotation misc_feature (2646..3236) is disrupted by the digestion. Try changing the restriction sites.
    GTUtilsLog::checkContainsError(os, lt, "Conserved annotation Misc. Feature (2646..3236) is disrupted by the digestion. Try changing the restriction sites.");
}

GUI_TEST_CLASS_DEFINITION(test_7456) {
    // Check that UGENE can open a FASTA file with a 100k small sequences as an alignment.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7456.fa");
    model.length = 5;
    model.window = 5;
    model.numberOfSequences = 100 * 1000;
    GTUtilsDialog::add(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTUtilsDialog::add(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join), 90000);
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(sequenceCount == model.numberOfSequences, "Invalid sequence count in MSA: " + QString::number(sequenceCount));
}

GUI_TEST_CLASS_DEFINITION(test_7459) {
    // Check that UGENE can fetch remote file as a reference for the generator task.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7459.fa");
    model.referenceUrl = "https://raw.githubusercontent.com/ugeneunipro/ugene/master/data/samples/FASTA/human_T1.fa";
    GTUtilsDialog::add(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
}

GUI_TEST_CLASS_DEFINITION(test_7460) {
    // Check that UGENE can open an alignment of [1_000 x 10_000] fast enough.
    DNASequenceGeneratorDialogFillerModel model(sandBoxDir + "/test_7460.fa");
    model.length = 1000;
    model.window = 1000;
    model.numberOfSequences = 10000;

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsDialog::waitForDialog(os, new DNASequenceGeneratorDialogFiller(os, model));
    GTMenu::clickMainMenuItem(os, {"Tools", "Random sequence generator..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(sequenceCount == model.numberOfSequences, "Invalid sequence count in MSA: " + QString::number(sequenceCount));

    QWidget* overviewWidget = GTUtilsMsaEditor::getOverviewArea(os);
    CHECK_SET_ERR(overviewWidget->isVisible(), "Overview widget ,must be visible, but must be hidden");
    GTUtilsTaskTreeView::waitTaskFinished(os, 10000);  // Check that there is no long-running active tasks.
}

GUI_TEST_CLASS_DEFINITION(test_7463) {
    // Tools->NGS data analysis->Extract consensus from assemblies...
    // Set _common_data/bam/Mycobacterium.sorted.bam as input and run.
    // Repeat steps with data/samples/Assembly/chrM.sorted.bam.
    // When the chrM workflow is over, click "Close dashboard".
    //     Expected: no crash or freeze.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", QStringList(), {{"Assembly", testDir + "_common_data/bam/Mycobacterium.sorted.bam"}}));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow(os);

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", QStringList(), {{"Assembly", dataDir + "samples/Assembly/chrM.sorted.bam"}}));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow(os);

    GTUtilsNotifications::waitForNotification(os);
    GTUtilsDialog::checkNoActiveWaiters(os);
    auto tab = GTTabWidget::getTabBar(os, GTUtilsDashboard::getTabWidget(os));
    GTWidget::click(os, tab->tabButton(tab->currentIndex(), QTabBar::RightSide));
}

GUI_TEST_CLASS_DEFINITION(test_7465) {
    // 1. Open workflow sample "Align sequences with MUSCLE"
    // Expected state: wizard has appeared.
    class AlignSequencesWithMuscleWizardFiller : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            // 2. Set file with many (~1200) sequences as input file and run workflow
            GTUtilsWizard::setInputFiles(os, {{QFileInfo(testDir + "_common_data/regression/7465/big_msa_as_fasta.fa").absoluteFilePath()}});
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Align Sequences with MUSCLE Wizard", new AlignSequencesWithMuscleWizardFiller));
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state: there is a notification about lacking of memory.
    CHECK_SET_ERR(GTUtilsDashboard::getJoinedNotificationsString(os).contains("There is not enough memory to align these sequences with MUSCLE"),
                  "No expected message about lacking of memory in notifications");
}

GUI_TEST_CLASS_DEFINITION(test_7469) {
    // Check that annotation sequence copy action respects 'join' and 'order' location flags.
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/7469.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Check 'order' annotation on the direct strand.
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, false);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "AAGACCCCCCCGTAGG", "1. Unexpected DNA sequence: " + GTClipboard::text(os));
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "KTPP*", "1. Unexpected Amino sequence: " + GTClipboard::text(os));

    // Check 'order' annotation on the complementary strand.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "AAGACCCC-CCCGTAGG", "2. Unexpected DNA sequence: " + GTClipboard::text(os));
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "KT-PV", "2. Unexpected Amino sequence: " + GTClipboard::text(os));

    // Check 'join' annotation on the direct strand.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "TGCCTTGCAAAGTTACTTAAGCTAGCTTG", "3. Unexpected DNA sequence: " + GTClipboard::text(os));
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "CLAKLLKLA", "3. Unexpected Amino sequence: " + GTClipboard::text(os));

    // Check 'join' annotation on the complementary strand.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "TGCCTTGCAAA-GTTACTTAAGCTAGCTTG", "4. Unexpected DNA sequence: " + GTClipboard::text(os));
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    CHECK_SET_ERR(GTClipboard::text(os) == "CLA-VT*ASL", "4. Unexpected Amino sequence: " + GTClipboard::text(os));
}

GUI_TEST_CLASS_DEFINITION(test_7472) {
    // Check that "Build tree" does not start the task if output directory is not writable.
    GTFileDialog::openFile(os, testDir + "_common_data/stockholm/ABC_tran.sto");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    class CheckReadOnlyPathScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            // Create a read-only directory and set a path to a file inside it into the saveLineEdit.
            QString dirPath = QFileInfo(sandBoxDir + GTUtils::genUniqueString("test_7472")).absoluteFilePath();
            CHECK_SET_ERR(QDir().mkpath(dirPath), "Failed to create dir: " + dirPath);
            GTFile::setReadOnly(os, dirPath, false);

            auto saveLineEdit = GTWidget::findLineEdit(os, "fileNameEdit", dialog);
            GTLineEdit::setText(os, saveLineEdit, dirPath + "/tree.nwk");

            // Check that error message is shown.
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Error opening file for writing"));
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new CheckReadOnlyPathScenario()));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::checkNoItem(os, "tree.nwk");
}

GUI_TEST_CLASS_DEFINITION(test_7473_1) {
    // Build an alignment for a read-only alignment file.
    GTFileDialog::openFile(os, testDir + "_common_data/stockholm", "2-Hacid_dh.sto");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsDocument::checkIfDocumentIsLocked(os, "2-Hacid_dh.sto", true);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7443.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that tree view is opened.
    GTUtilsMsaEditor::getTreeView(os);
}

GUI_TEST_CLASS_DEFINITION(test_7473_2) {
    // Build an alignment for a read-only alignment file from options panel.
    GTFileDialog::openFile(os, dataDir + "samples/Stockholm/CBS.sto");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsDocument::checkIfDocumentIsLocked(os, "CBS.sto", true);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, "default", 0, 0, true));
    GTWidget::click(os, GTWidget::findWidget(os, "buildTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that tree view is opened.
    GTUtilsMsaEditor::getTreeView(os);
}

GUI_TEST_CLASS_DEFINITION(test_7476) {
    // Check that IQ-TREE has "Display Options" tab, and it works.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    class EnableCreateNewViewOptionScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);

            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "IQ-TREE");

            GTTabWidget::clickTab(os, "tab_widget", dialog, "Display Options");

            GTRadioButton::click(os, "createNewView", dialog);

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new EnableCreateNewViewOptionScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that tree view is opened.
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os, "collapse_mode_");
}

GUI_TEST_CLASS_DEFINITION(test_7487_1) {
    // Check that move of the multi-region selection with drag-and-drop works as expected (2 selected regions).
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Original state: a,b,c,d,e,f,g,h.
    GTUtilsMsaEditor::selectRowsByName(os, {"c", "d", "g", "h"});

    // Drag and drop "g" up to the "e" location.
    QRect movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect(os, "g");
    QRect movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect(os, "e");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "g", "h", "e", "f"}), "1. Unexpected order: " + nameList.join(","));

    // Restore original state: a,b,c,d,e,f,g,h.
    GTUtilsMsaEditor::clearSelection(os);
    GTUtilsMsaEditor::undo(os);
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "e", "f", "g", "h"}), "2. Unexpected order: " + nameList.join(","));

    // Drag and drop "b" down to the "d" location.
    GTUtilsMsaEditor::selectRowsByName(os, {"a", "b", "e", "f"});
    movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect(os, "b");
    movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect(os, "d");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"c", "d", "a", "b", "e", "f", "g", "h"}), "3. Unexpected order: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7487_2) {
    // Check that move of the multi-region selection with drag-and-drop works as expected (4 selected regions).
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/collapse_mode_1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Original state: a,b,c,d,e,f,g,h.
    GTUtilsMsaEditor::selectRowsByName(os, {"a", "c", "e", "g"});

    // Drag and drop "e" up to the "d" location.
    QRect movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect(os, "e");
    QRect movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect(os, "d");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"b", "a", "c", "e", "g", "d", "f", "h"}), "1. Unexpected order: " + nameList.join(","));
    GTUtilsMsaEditor::checkSelectionByNames(os, {"a", "c", "e", "g"});

    // Restore original state: a,b,c,d,e,f,g,h. Check that selection is restored too.
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkSelectionByNames(os, {"a", "c", "e", "g"});
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "e", "f", "g", "h"}), "2. Unexpected order: " + nameList.join(","));
    GTUtilsMsaEditor::clearSelection(os);

    // Drag and drop "d" down to the "e" location.
    GTUtilsMsaEditor::selectRowsByName(os, {"b", "d", "f", "h"});
    movingSequenceSrcRect = GTUtilsMsaEditor::getSequenceNameRect(os, "d");
    movingSequenceDstRect = GTUtilsMsaEditor::getSequenceNameRect(os, "e");
    GTMouseDriver::dragAndDrop(movingSequenceSrcRect.center(), movingSequenceDstRect.center());

    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"a", "c", "e", "b", "d", "f", "h", "g"}), "3. Unexpected order: " + nameList.join(","));
    GTUtilsMsaEditor::checkSelectionByNames(os, {"b", "d", "f", "h"});

    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::checkSelectionByNames(os, {"b", "d", "f", "h"});
    nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameList == QStringList({"a", "b", "c", "d", "e", "f", "g", "h"}), "4. Unexpected order: " + nameList.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_7490) {
    // Create a multi-selection and check that the current line label in the MCA editor's status bar shows '-'.
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", sandBoxDir + "test_7490.ugenedb");
    GTFileDialog::openFile(os, sandBoxDir + "test_7490.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMcaEditor::selectReadsByName(os, {"SZYD_Cas9_5B70", "SZYD_Cas9_CR50"});
    QString currentLineNumberText = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
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
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", QStringList(), {{"Assembly", dataDir + "samples/Assembly/chrM.sorted.bam"}}));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsDialog::checkNoActiveWaiters(os, 40000);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Save));
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsDialog::checkNoActiveWaiters(os, 10000);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Save anyway"));
    GTUtilsDialog::checkNoActiveWaiters(os, 10000);

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", QStringList(), {{"Assembly", dataDir + "samples/Assembly/chrM.sorted.bam"}}));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7499) {
    // Create a multi-selection and check that the current line label in the MCA editor's status bar shows '-'.

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/protein.fasta.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    QString sequence1v1 = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 1).left(10);
    QString sequence8v1 = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 8).left(10);

    // Modify 2 sequences first.
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, {1, 1});
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, {8, 8});
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString sequence1v2 = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 1).left(10);
    QString sequence8v2 = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 8).left(10);
    QString expected1v2 = sequence1v1.mid(0, 1) + "-" + sequence1v1.mid(1, 8);
    QString expected8v2 = sequence8v1.mid(0, 8) + "-" + sequence8v1.mid(8, 1);
    CHECK_SET_ERR(sequence1v2 == expected1v2, "Sequence 1 modification is not matched: " + sequence1v2 + ", expected: " + expected1v2);
    CHECK_SET_ERR(sequence8v2 == expected8v2, "Sequence 8 modification is not matched: " + sequence8v2 + ", expected: " + expected8v2);

    QStringList nameListBefore = GTUtilsMSAEditorSequenceArea::getNameList(os);

    // Align the first sequence to the current alignment.
    GTUtilsMsaEditor::clickSequence(os, 1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"align_selection_to_alignment_muscle"}));
    GTWidget::click(os, GTAction::button(os, "align_selected_sequences_to_alignment"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // The order of sequences should not change.
    QStringList nameListAfter = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(nameListBefore == nameListAfter, "Name list changed");

    // The only the first sequence must be changed (alignment back).
    QString sequence1v3 = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 1).left(10);
    QString sequence8v3 = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 8).left(10);
    CHECK_SET_ERR(sequence1v3 == sequence1v1, "Sequence 1 was not aligned as expected.");
    CHECK_SET_ERR(sequence8v3 == sequence8v2, "Sequence 8 was modified as result of alignment");
}

GUI_TEST_CLASS_DEFINITION(test_7504) {
    // Check that multi-region complement(join()) annotation is exported in the correct order.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/short.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "", "complement(join(1..1,10..10))"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsDialog::waitForDialog(os,
                                 new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                               sandBoxDir + "test_7504_out.fa",
                                                                               ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                               ExportSequenceOfSelectedAnnotationsFiller::Merge));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export", "Export sequence of selected annotations..."}));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString exportedSequence = GTUtilsSequenceView::getSequenceAsString(os);
    CHECK_SET_ERR(exportedSequence == "GA", "Sequence not matched: " + exportedSequence);
}

GUI_TEST_CLASS_DEFINITION(test_7505) {
    // Check that double-click on the sequence name in MSA editor toggles centering of the start/end sequence region.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Delete first sequences so the tested sequence will be scrolled into the view.
    GTUtilsMsaEditor::selectRows(os, 0, 15);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    QString sequenceName = "Pc_Metavir10";
    GTUtilsMsaEditor::clickSequenceName(os, sequenceName);
    GTUtilsMsaEditor::checkSelectionByNames(os, {sequenceName});

    int firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstVisibleBase == 0, "1. Unexpected first visible base: " + QString::number(firstVisibleBase));

    QRect rect = GTUtilsMsaEditor::getSequenceNameRect(os, sequenceName);
    GTMouseDriver::moveTo(rect.center());

    GTMouseDriver::doubleClick();
    int expectedCenter = 66;
    firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstVisibleBase < expectedCenter, "2. Unexpected first visible base: " + QString::number(firstVisibleBase));
    int lastVisibleBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    CHECK_SET_ERR(lastVisibleBase > expectedCenter, "2. Unexpected last visible base: " + QString::number(lastVisibleBase));

    GTMouseDriver::doubleClick();
    expectedCenter = 1220;
    firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstVisibleBase < expectedCenter, "3. Unexpected first visible base: " + QString::number(firstVisibleBase));
    lastVisibleBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    CHECK_SET_ERR(lastVisibleBase > expectedCenter, "3. Unexpected last visible base: " + QString::number(lastVisibleBase));

    GTMouseDriver::doubleClick();
    expectedCenter = 66;
    firstVisibleBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstVisibleBase < expectedCenter, "4. Unexpected first visible base: " + QString::number(firstVisibleBase));
    lastVisibleBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    CHECK_SET_ERR(lastVisibleBase > expectedCenter, "4. Unexpected last visible base: " + QString::number(lastVisibleBase));
}

GUI_TEST_CLASS_DEFINITION(test_7506) {
    // Check that blast search correctly selects database name from new variants of file.
    BlastLocalSearchDialogFiller::Parameters blastParams;
    blastParams.runBlast = true;
    blastParams.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1_v2_10/human_T1.ndb";
    blastParams.withInputFile = true;
    blastParams.inputPath = dataDir + "samples/FASTA/human_T1.fa";
    GTUtilsDialog::waitForDialog(os, new BlastLocalSearchDialogFiller(blastParams, os));
    GTMenu::clickMainMenuItem(os, {"Tools", "BLAST", "BLAST search..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool hasExpectedResult = GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(5061, 291));
    CHECK_SET_ERR(hasExpectedResult, "Can not find the expected blastn result");
}

GUI_TEST_CLASS_DEFINITION(test_7507) {
    // Check that "Worm" rendering mode is available and is enabled by default for '4rte' PDB file.
    GTFileDialog::openFile(os, testDir + "_common_data/pdb/4rte.pdb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto glWidget = GTWidget::findWidget(os, "1-4RTE", sequenceViewWindow);

    GTUtilsDialog::waitForDialog(os,
                                 new PopupCheckerByText(os,
                                                        {"Render Style", "Worms"},
                                                        PopupChecker::CheckOptions(PopupChecker::IsEnabled | PopupChecker::IsChecked)));
    GTMenu::showContextMenu(os, glWidget);
}

GUI_TEST_CLASS_DEFINITION(test_7508) {
    // Check that no-op "align-selection" action with MUSCLE does not produce "Undo" history.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Try to  align a sequence that is already aligned.
    GTUtilsMsaEditor::clickSequence(os, 1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"align_selection_to_alignment_muscle"}));
    GTWidget::click(os, GTAction::button(os, "align_selected_sequences_to_alignment"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto undoButton = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undoButton->isEnabled(), "Undo button must be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_7509) {
    // Check that MCA editor does not crash when closed in "replace-character" mode.
    GTFileDialog::openFile(os, dataDir + "samples/Sanger/alignment.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    // Enable "replace-character" mode.
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, {6374, 0});
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os) == 'C', "Position validation failed!");

    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);
    short mode = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(mode == 1, "Not an edit mode! Mode: " + QString::number(mode));

    // Close MCA editor -> UGENE should not crash.
    GTUtilsMdi::closeActiveWindow(os);
}

GUI_TEST_CLASS_DEFINITION(test_7511) {
    // Check that Blast Search filters the list of available tool based on the selected file sequence alphabet.
    class BlastToolListCheckScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            auto toolsCombo = GTWidget::findComboBox(os, "programNameComboBox");
            auto selectFileButton = GTWidget::findToolButton(os, "browseInput", dialog);
            GTComboBox::checkValuesPresence(os, toolsCombo, {"blastn", "blastp", "blastx", "tblastx", "tblastn"});

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/human_T1.fa"));
            GTWidget::click(os, selectFileButton);
            GTUtilsTaskTreeView::waitTaskFinished(os);
            // Check that the list of tools is updated to nucleic tools.
            GTComboBox::checkValuesPresence(os, toolsCombo, {"blastn", "blastx", "tblastx"});

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/titin.fa"));
            GTWidget::click(os, selectFileButton);
            GTUtilsTaskTreeView::waitTaskFinished(os);
            //  Check that the list of tools is updated to amino tools.
            GTComboBox::checkValuesPresence(os, toolsCombo, {"blastp", "tblastn"});

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);  // Cancel "Blast" dialog.
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);  // Cancel "Save project" popup.
        }
    };
    GTUtilsDialog::waitForDialog(os, new BlastLocalSearchDialogFiller(os, new BlastToolListCheckScenario()));
    GTMenu::clickMainMenuItem(os, {"Tools", "BLAST", "BLAST search..."});
}

GUI_TEST_CLASS_DEFINITION(test_7517) {
    // Check that MCA editor does not crash when closed in "replace-character" mode.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkMessageWithTextCount(os, "Registering new task: Render overview", 1, "check1");

    auto showOverviewButton = GTUtilsMsaEditor::getShowOverviewButton(os);
    GTWidget::click(os, showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkMessageWithTextCount(os, "Registering new task: Render overview", 1, "check2");

    GTWidget::click(os, showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkMessageWithTextCount(os, "Registering new task: Render overview", 1, "check3");
}

GUI_TEST_CLASS_DEFINITION(test_7520) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Add "Improve Reads with Trimmomatic" to the scene
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    GTUtilsWorkflowDesigner::addElement(os, trimmomaticName);

    // 3. Check tooltips for "Palindrome clip threshold" label and value
    // Expected state: they should be correct (different with "simple clip" tooltip)

    class TrimmomaticCustomScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto addButton = GTWidget::findToolButton(os, "buttonAdd", dialog);

            GTWidget::click(os, addButton);
            for (int i = 0; i < 4; i++) {
                GTKeyboardDriver::keyClick(Qt::Key_Down);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTWidget::click(os, addButton);

            GTMouseDriver::moveTo(GTWidget::getWidgetCenter(GTWidget::findWidget(os, "palindromeThreshold")));
            QString tooltip = GTUtilsToolTip::getToolTip();
            QString expedtedTooltip("A threshold for palindrome alignment mode. For palindromic matches, a longer alignment is possible."
                                    " Therefore the threshold can be in the range of 30. Even though this threshold is very high"
                                    " (requiring a match of almost 50 bases) Trimmomatic is still able to identify very, very short adapter fragments.");
            CHECK_SET_ERR(tooltip.contains(expedtedTooltip), QString("Actual tooltip not contains expected string. Expected string: %1").arg(expedtedTooltip));

            GTMouseDriver::moveTo(GTWidget::getWidgetCenter(GTWidget::findWidget(os, "palindromeLabel")));
            tooltip = GTUtilsToolTip::getToolTip();
            CHECK_SET_ERR(tooltip.contains(expedtedTooltip), QString("Actual tooltip not contains expected string. Expected string: %1").arg(expedtedTooltip));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new TrimmomaticDialogFiller(os, new TrimmomaticCustomScenario()));
    GTUtilsWorkflowDesigner::click(os, trimmomaticName);
    GTUtilsWorkflowDesigner::setParameter(os, "Trimming steps", "", GTUtilsWorkflowDesigner::customDialogSelector);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7531) {
    // Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Ctrl+N" and create the annotation on "80..90"
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "test_7531", "80..90"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Select the created annotation and click "Delete".
    GTUtilsAnnotationsTreeView::clickItem(os, "test_7531", 1, false);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Open the "In silico PCR" tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // Set "TTGTCAGATTCACCAAAGTT" as a forward primer and "CTCTCTTCTGGCCTGTAGGGTTTCTG" as a reverse primer.
    GTUtilsOptionPanelSequenceView::setForwardPrimer(os, "TTGTCAGATTCACCAAAGTT");
    GTUtilsOptionPanelSequenceView::setReversePrimer(os, "CTCTCTTCTGGCCTGTAGGGTTTCTG");

    // Click "Find product(s) anyway".
    GTUtilsOptionPanelSequenceView::pressFindProducts(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the only product has been found.
    const int count = GTUtilsOptionPanelSequenceView::productsCount(os);
    CHECK_SET_ERR(count == 1, QString("Unexpected products quantity, expected: 1, current: %1").arg(count));

    // Click "Extract primer".
    GTUtilsOptionPanelSequenceView::pressExtractProduct(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: no crash
}

GUI_TEST_CLASS_DEFINITION(test_7535) {
    // Check that UGENE does not crash when tooltip is invoked on non-standard annotations.
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/zero_length_feature.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto zeroLength0Item = GTUtilsAnnotationsTreeView::findItem(os, "zero_length_0");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, zeroLength0Item));
    QString tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.isEmpty(), "Expected no tooltip for zero-length annotation: " + tooltip);

    auto zeroLengthXItem = GTUtilsAnnotationsTreeView::findItem(os, "zero_length_x");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, zeroLengthXItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.isEmpty(), "Expected no tooltip for out of bound annotation: " + tooltip);

    auto normalLengthItem = GTUtilsAnnotationsTreeView::findItem(os, "normal_length");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, normalLengthItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = TTGCAGAATTC"), "Expected sequence info in tooltip for a normal annotation: " + tooltip);

    auto normalLengthComplementaryItem = GTUtilsAnnotationsTreeView::findItem(os, "normal_length_c");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, normalLengthComplementaryItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = GAATTCTGCAA"), "Expected complementary sequence info in tooltip for a normal annotation: " + tooltip);

    auto joinedItem = GTUtilsAnnotationsTreeView::findItem(os, "joined");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, joinedItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = TCT"), "Expected dna sequence info in tooltip for a joined annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = S"), "Expected amino sequence info in tooltip for a joined annotation: " + tooltip);

    auto joinedComplementaryItem = GTUtilsAnnotationsTreeView::findItem(os, "joined_c");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, joinedComplementaryItem));
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = AGA"), "Expected dna sequence info in tooltip for a joined complementary annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = R"), "Expected amino sequence info in tooltip for a joined complementary annotation: " + tooltip);
}

GUI_TEST_CLASS_DEFINITION(test_7539) {
    // Check that UGENE shows a tooltip when a small 1-char annotation region is hovered in sequence view.
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/zero_length_feature.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::moveMouseToAnnotationInDetView(os, "joined", 30);
    QString tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = TCT"), "Expected dna sequence info in tooltip for a joined annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = S"), "Expected amino sequence info in tooltip for a joined annotation: " + tooltip);

    GTUtilsSequenceView::moveMouseToAnnotationInDetView(os, "joined_c", 30);
    tooltip = GTUtilsToolTip::getToolTip();
    CHECK_SET_ERR(tooltip.contains("<b>Sequence</b> = AGA"), "Expected dna sequence info in tooltip for a joined complementary annotation: " + tooltip);
    CHECK_SET_ERR(tooltip.contains("<b>Translation</b> = R"), "Expected amino sequence info in tooltip for a joined complementary annotation: " + tooltip);
}

GUI_TEST_CLASS_DEFINITION(test_7540) {
    // 1. Open file with two equal annotations.
    GTFileDialog::openFile(os, testDir + "_common_data/regression/7540/7540.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* miscFeature = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature");
    GTTreeWidget::click(os, miscFeature);
    // 2. Select one and cheange it location, then name.
    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, "misc_feature", "2..8"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    GTTreeWidget::click(os, miscFeature);
    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, "misc_feature1", "2..8"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    // 3. Open Annotation Highlighting Tab.
    // Expected state: no crash or SAFE_POINT triggering.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
}

GUI_TEST_CLASS_DEFINITION(test_7546) {
    // Check that tree or msa with ambiguous names can't be synchronized.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/same_name_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::toggleCollapsingMode(os);
    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_7546.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected result: UGENE does not crash and Sync button is OFF.
    QAbstractButton* syncModeButton = GTAction::button(os, "sync_msa_action");
    CHECK_SET_ERR(!syncModeButton->isEnabled(), "Sync mode must be not available");
}

GUI_TEST_CLASS_DEFINITION(test_7548) {
    // Check that UGENE shows correct MSA symbols for huge MSA files when scrolled to large positions.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, {1, 0});  // Move focus and check visually that the selected char is 'C'.
    QString colorOfC = GTUtilsMSAEditorSequenceArea::getColor(os, {1, 0});

    QList<QPoint> positions = {{9999 - 1, 0}, {100000 - 1, 0}, {2000000 - 1, 1}};
    for (const QPoint& position : qAsConst(positions)) {
        GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, position.x() + 1));  // GoTo accepts visual positions.
        GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
        GTUtilsMSAEditorSequenceArea::clickToPosition(os, position);  // Selected character has some opacity adjustment.

        QString color = GTUtilsMSAEditorSequenceArea::getColor(os, position);
        CHECK_SET_ERR(color == colorOfC, "Invalid color: " + color + ", position: " + QString::number(position.x()) + ", expected: " + colorOfC);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7550) {
    class Click103TimesScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus&) override {
            auto stack = AppContext::getMainWindow()->getNotificationStack();
            for (int i = 0; i < 103; i++) {
                stack->addNotification("Notification " + QString::number(i + 1));
                GTGlobals::sleep(200);
            }
        }
    };
    // Create 100+ notifications. Check that UGENE does not crash.
    GTThread::runInMainThread(os, new Click103TimesScenario());
}

GUI_TEST_CLASS_DEFINITION(test_7555) {
    // Check that among many empty assemblies in the file the one with reads is opened by default.
    GTFileDialog::openFile(os, testDir + "_common_data/ugenedb/7555-reads-data-removed.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
    CHECK_SET_ERR(GTUtilsAssemblyBrowser::getReadsCount(os) > 0, "No reads in the view");
}

GUI_TEST_CLASS_DEFINITION(test_7556) {
    // Check that IQ-TREE parameter input widgets work in sync with a manual parameters input as text.
    // Check that in Tree-Sync mode Drag & Drop of sequences in the MSA name list is disabled.
    GTFileDialog::openFile(os, testDir + "_common_data/msf/1.msf");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    class OptionsTestScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "IQ-TREE");

            auto substModelEdit = GTWidget::findLineEdit(os, "substModelEdit", dialog);
            auto ultrafastBootstrapEdit = GTWidget::findLineEdit(os, "ultrafastBootstrapEdit", dialog);
            auto alrtEdit = GTWidget::findLineEdit(os, "alrtEdit", dialog);
            auto ancestralReconstructionCheckBox = GTWidget::findCheckBox(os, "ancestralReconstructionCheckBox", dialog);
            auto extraParametersTextEdit = GTWidget::findPlainTextEdit(os, "extraParametersTextEdit", dialog);

            CHECK_SET_ERR(extraParametersTextEdit->toPlainText().isEmpty(), "extraParametersTextEdit is not empty by default");
            CHECK_SET_ERR(substModelEdit->text().isEmpty(), "substModelEdit is not empty by default");
            CHECK_SET_ERR(ultrafastBootstrapEdit->text().isEmpty(), "ultrafastBootstrapEdit is not empty by default");
            CHECK_SET_ERR(alrtEdit->text().isEmpty(), "alrtEdit is not empty by default");
            CHECK_SET_ERR(!ancestralReconstructionCheckBox->isChecked(), "ancestralReconstructionCheckBox is not unchecked by default");

            // Set values to widgets, check that text is changed.
            GTPlainTextEdit::setPlainText(os, extraParametersTextEdit, "-custom c1 -m 1 -bb 2 --custom c2 c3 -alrt 3");
            GTLineEdit::setText(os, substModelEdit, "LM");
            GTLineEdit::setText(os, ultrafastBootstrapEdit, "1000");
            GTLineEdit::setText(os, alrtEdit, "1001");
            GTCheckBox::setChecked(os, ancestralReconstructionCheckBox);
            CHECK_SET_ERR(extraParametersTextEdit->toPlainText() == "-custom c1 --custom c2 c3 -m LM -bb 1000 -alrt 1001 -asr",
                          "extraParametersTextEdit is not updated with values from the inputs");

            // Empty text field - widgets must be also reset.
            GTPlainTextEdit::clear(os, extraParametersTextEdit);
            CHECK_SET_ERR(substModelEdit->text().isEmpty(), "substModelEdit is not empty");
            CHECK_SET_ERR(ultrafastBootstrapEdit->text().isEmpty(), "ultrafastBootstrapEdit is not empty");
            CHECK_SET_ERR(alrtEdit->text().isEmpty(), "alrtEdit is not empty by default");
            CHECK_SET_ERR(!ancestralReconstructionCheckBox->isChecked(), "ancestralReconstructionCheckBox is not unchecked");

            // Set text with parameters and check the widgets are updated
            GTPlainTextEdit::setPlainText(os, extraParametersTextEdit, "-m TEST -bb 1000 -alrt 1002 -asr");
            CHECK_SET_ERR(substModelEdit->text() == "TEST", "substModelEdit is not updated");
            CHECK_SET_ERR(ultrafastBootstrapEdit->text() == "1000", "ultrafastBootstrapEdit is not updated");
            CHECK_SET_ERR(alrtEdit->text() == "1002", "alrtEdit is not updated");
            CHECK_SET_ERR(ancestralReconstructionCheckBox->isChecked(), "ancestralReconstructionCheckBox is not checked");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new OptionsTestScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::getTreeView(os);  // Check that tree view is opened.
}

GUI_TEST_CLASS_DEFINITION(test_7572) {
    // 1. Open HIV-1.aln
    // 2. Click the "Build Tree" button on the toolbar.
    // 3. Start building tree with Likelihood algorithm
    // 4. Cancel Tree building task
    // Expected state: no message about QProcess destructor in details log
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class PhyMLMaximumLikelihoodScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "test_7572.nwk", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new PhyMLMaximumLikelihoodScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");

    QString taskName = "Calculating Phylogenetic Tree";
    GTUtilsTaskTreeView::checkTaskIsPresent(os, taskName);
    QString taskStatus = GTUtilsTaskTreeView::getTaskStatus(os, taskName);
    CHECK_SET_ERR(taskStatus == "Running", "The task status is incorrect: " + taskStatus);
    GTUtilsTaskTreeView::cancelTask(os, taskName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // We can't put it in macro because it will be auto-triggered by log message from macro itself.
    bool messageNotFound = !U2::GTLogTracer::checkMessage("QProcess: Destroyed while process");
    CHECK_SET_ERR(messageNotFound, "Message about QProcess destructor found, but shouldn't be.");
}

GUI_TEST_CLASS_DEFINITION(test_7573) {
    // Open data/samples/PDB/1CF7.PDB
    // Right click 3D Model->Molecular Surface->SAS.
    // Press Ctrl+S and save the project.
    // Press Ctrl+S many times.
    //     Expected: UGENE doesn't crash.
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Molecular Surface", "SAS"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "1-1CF7"));

    GTUtilsProject::saveProjectAs(os, sandBoxDir + "7573/A.uprj");
    for (int i = 0; i < 50; i++) {
        GTKeyboardDriver::keyClick('S', Qt::ControlModifier);
    }
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7574) {
    // Original problem:
    // Open _common_data/pdb/1JFA_3.pdb
    // Click context menu 'Models..'
    // Enable Model N3 -> UGENE crashes.
    GTFileDialog::openFile(os, testDir + "_common_data/pdb/1JFA_3.pdb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Models.."}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "1-1JFA"));

    auto dialog = GTWidget::findWidget(os, "SelectModelsDialog");
    auto listWidget = GTWidget::findListWidget(os, "modelsList", dialog);
    QStringList itemsBefore = GTListWidget::getItems(os, listWidget);
    CHECK_SET_ERR(itemsBefore.size() == 3, "1. Expected 3 items, got: " + QString::number(itemsBefore.size()));
    CHECK_SET_ERR(GTListWidget::isItemChecked(os, listWidget, "1"), "1. Item 1 must be checked");
    CHECK_SET_ERR(!GTListWidget::isItemChecked(os, listWidget, "2"), "1. Item 2 must not be checked");
    CHECK_SET_ERR(!GTListWidget::isItemChecked(os, listWidget, "3"), "1. Item 3 must not be checked");
    GTListWidget::checkAllItems(os, listWidget, true);
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
    GTThread::waitForMainThread();
    // UGENE must not crash.

    // Open the dialog again and check that all 3 items are checked.
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Models.."}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "1-1JFA"));

    dialog = GTWidget::findWidget(os, "SelectModelsDialog");
    listWidget = GTWidget::findListWidget(os, "modelsList", dialog);
    QStringList itemsAfter = GTListWidget::getItems(os, listWidget);
    CHECK_SET_ERR(itemsBefore.size() == 3, "2. Expected 3 items, got: " + QString::number(itemsBefore.size()));
    CHECK_SET_ERR(GTListWidget::isItemChecked(os, listWidget, "1"), "2. Item 1 must be checked");
    CHECK_SET_ERR(GTListWidget::isItemChecked(os, listWidget, "2"), "2. Item 2 must be checked");
    CHECK_SET_ERR(GTListWidget::isItemChecked(os, listWidget, "3"), "2. Item 3 must be checked");
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

GUI_TEST_CLASS_DEFINITION(test_7575) {
    // Check that reset-zoom action does not crash UGENE.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsMsaEditor::zoomIn(os);
    GTUtilsMsaEditor::zoomIn(os);
    GTUtilsMsaEditor::zoomIn(os);

    GTUtilsMSAEditorSequenceArea::scrollToPosition(os, {550, 1});
    GTUtilsMsaEditor::resetZoom(os);
    // Expected state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_7576) {
    // Check that zoom-to-selection in MSA keeps the selected region within the visible sequence area.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    QList<QPoint> topLeftPoints = {{500, 5}, {603, 17}};
    QList<QPoint> bottomRightPoints = {{540, 15}, {603, 17}};

    for (int i = 0; i < topLeftPoints.size(); i++) {
        QPoint topLeft = topLeftPoints[i];
        QPoint bottomRight = bottomRightPoints[i];

        GTUtilsMSAEditorSequenceArea::selectArea(os, topLeft, bottomRight);
        GTUtilsMsaEditor::zoomToSelection(os);

        int firstVisibleBaseIndex = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
        int lastVisibleBaseIndex = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
        CHECK_SET_ERR(firstVisibleBaseIndex <= topLeft.x() && lastVisibleBaseIndex >= bottomRight.x(),
                      QString("%1.Invalid visible X range: %2:%3").arg(i).arg(firstVisibleBaseIndex).arg(lastVisibleBaseIndex));

        int firstVisibleRowIndex = GTUtilsMSAEditorSequenceArea::getFirstVisibleRowIndex(os);
        int lastVisibleRowIndex = GTUtilsMSAEditorSequenceArea::getLastVisibleRowIndex(os, true);
        CHECK_SET_ERR(firstVisibleRowIndex <= topLeft.y() && lastVisibleRowIndex >= bottomRight.y(),
                      QString("%1.Invalid visible Y range: %2:%3").arg(i).arg(firstVisibleRowIndex).arg(lastVisibleRowIndex));

        GTUtilsMsaEditor::resetZoom(os);
    }
}

GUI_TEST_CLASS_DEFINITION(test_7582) {
    // Check that UGENE can build a tree for a MSA with non-unique sequence names.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/same_name_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    class RunBuildTreeScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "MrBayes");
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "test_7582.nwk", dialog);  // Set output file name.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTLogTracer logTracer;
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new RunBuildTreeScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMsaEditor::getTreeView(os);  // Check that tree view was opened.
    CHECK_SET_ERR(!logTracer.hasErrors(), "Found error in the log: " + logTracer.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_7584) {
    // Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os)
            : Filler(_os, "CreateAnnotationDialog") {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::clickButtonBox(os, w, QDialogButtonBox::Ok);
        }
    };

    // Select Align->Align sequence to mRNA from context menu
    // In "Select Item" dialog expand  human_T1 and select corresponding sequence
    // Push OK -> "Save result to annotation" dialog appeas
    // Click Create button in "Save result to annotation" dialog
    GTUtilsDialog::waitForDialog(os, new OkClicker(os));
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "human_T1.fa", "human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ADV_MENU_ALIGN", "Align sequence to mRNA"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    // Remove this file by Del button as quick as possible
    GTUtilsProjectTreeView::click(os, "Annotations");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7607) {
    // Check that UGENE can build a tree for a MSA with non-unique sequence names.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/align.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    class BuildTreeWithMrBayesScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "MrBayes");
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "test_7607.nwk", dialog);  // Set output file name.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new BuildTreeWithMrBayesScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString expectedTree = GTFile::readAll(os, testDir + "_common_data/regression/7607/test_7607_expected.nwk");
    QString actualTree = GTFile::readAll(os, sandBoxDir + "test_7607.nwk");
    CHECK_SET_ERR(actualTree == expectedTree, "Actual tree does not match the expected tree");
}

GUI_TEST_CLASS_DEFINITION(test_7609) {
    // Open _common_data/clustal/non_unique_row_names.aln.
    // Open and close the "Tree Settings" tab of the Options Panel.
    // Select the first sequence in the alignment.
    // Press Delete 2 times.
    // No crash.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/non_unique_row_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsOptionPanelMsa::closeTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsMsaEditor::removeRows(os, 0, 0);
    GTUtilsMsaEditor::removeRows(os, 0, 0);
}

GUI_TEST_CLASS_DEFINITION(test_7611) {
    // Check that export 3D struct to PDF works correctly (doesn't fail with error or an empty document).
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString pdfFilePath = sandBoxDir + "test_7611.pdf";

    class ExportImageScenario : public CustomScenario {
    public:
        ExportImageScenario(const QString& _pdfFilePath)
            : pdfFilePath(_pdfFilePath) {
        }

        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTComboBox::selectItemByText(os, "formatsBox", dialog, "PDF");
            GTLineEdit::setText(os, "fileNameEdit", pdfFilePath, dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }

        QString pdfFilePath;
    };

    GTLogTracer logTracer;

    GTUtilsDialog::waitForDialog(os, new Filler(os, "ImageExportForm", new ExportImageScenario(pdfFilePath)));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"bioStruct3DExportImageAction"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "1-1CF7"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!logTracer.hasErrors(), "Errors in log: " + logTracer.getJoinedErrorString());
    qint64 pdfFileSize = GTFile::getSize(os, pdfFilePath);
    CHECK_SET_ERR(pdfFileSize > 1000 * 1000, "Invalid PDF file size: " + QString::number(pdfFileSize));
}

GUI_TEST_CLASS_DEFINITION(test_7616) {
    // Check that "Open Tree" button in MSA Editor's options panel works correctly.
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);

    QList<Document*> documents = AppContext::getProject()->getDocuments();
    CHECK_SET_ERR(documents.size() == 1, "Expected 1 document in project");
    Document* initialCoiNwkDocument = documents.first();

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);

    // Try non-tree file. Expected state: nothing is loaded.
    GTLogTracer logTracer1("Document contains no tree objects");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "/samples/CLUSTALW/ty3.aln.gz"));
    GTWidget::click(os, GTWidget::findWidget(os, "openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkContainsMessage(os, logTracer1);
    GTUtilsMsaEditor::checkNoTreeView(os);

    // Try load a tree file that is already in the project. Expected state: the document in the project is reused.
    GTLogTracer logTracer2;
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "/samples/Newick/COI.nwk"));
    GTWidget::click(os, GTWidget::findWidget(os, "openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check there is an active tree view.
    GTUtilsMsaEditor::getTreeView(os);
    GTUtilsLog::check(os, logTracer2);  // Check there is no error in the log.

    documents = AppContext::getProject()->getDocuments();
    CHECK_SET_ERR(documents.size() == 2, "Expected 2 document in project");
    CHECK_SET_ERR(documents.contains(initialCoiNwkDocument), "Expected initial tree document to be present in the project and re-used in MSA editor");
}

GUI_TEST_CLASS_DEFINITION(test_7623) {
    GTLogTracer logTracer;

    // Select "Tools>Workflow Designer"
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // Open "Trim and аlign Sanger reads" sample
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            // Select Reference .../test/general/_common_data/sanger/reference.gb
            GTUtilsWizard::setParameter(os, "Reference", testDir + "_common_data/sanger/reference.gb");

            // Push Next
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            // On page "Input Sanger reads" add: .../test/general/_common_data/sanger/n_and_gaps.fa and click "Next" button
            GTUtilsWizard::setInputFiles(os, {{testDir + QString("_common_data/sanger/n_and_gaps.fa")}});

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //  Push Next on "Trim and Filtering" page
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario()));
    GTUtilsWorkflowDesigner::addSample(os, "Trim and Map Sanger reads");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkContainsError(os, logTracer, "All input reads contain gaps or Ns only, abort");
}

GUI_TEST_CLASS_DEFINITION(test_7629) {
    // 1. Open sars.gb
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Copy 1001 symbol
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 1001));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste"
                                                                              << "Copy selected sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 3. Paste it to project filter
    // Expected: no crash, here is info message in log and warning message box
    GTLogTracer logTracer;
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "The search pattern is too long. Pattern was truncated to 1000 symbols."));
    auto nameFilterEdit = GTWidget::findLineEdit(os, "nameFilterEdit");
    GTLineEdit::setText(os, nameFilterEdit, GTClipboard::text(os), true, true);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkMessageWithWait(os, logTracer, "The search pattern is too long. Pattern was truncated to 1000 symbols.", 90000);

    // 4. Copy region with acceptable length 1000 symbols
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 1000));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Copy/Paste", "Copy selected sequence"}));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 5. Paste it to project filter
    // Expected: no crash, no error in log
    GTLogTracer logTracer2;
    GTUtilsTaskTreeView::openView(os);
    GTLineEdit::clear(os, nameFilterEdit);
    GTLineEdit::setText(os, nameFilterEdit, GTClipboard::text(os), true, true);
    GTUtilsTaskTreeView::checkTaskIsPresent(os, "Filtering project content");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!logTracer2.hasErrors(), "Log should not contain errors");
}

GUI_TEST_CLASS_DEFINITION(test_7630) {
    // Open CVU55762.gb and murine.gb in separate sequence mode.
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // In CVU55762 select region 1001-1000.
    SelectSequenceRegionDialogFiller* filler = new SelectSequenceRegionDialogFiller(os, 1001, 1000);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);

    // Copy it (Cmd-C).
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Switch to murine and toggle circular views.
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsCv::commonCvBtn::click(os);

    // Select region 1000-5833.
    filler = new SelectSequenceRegionDialogFiller(os, 1000, 5830);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);

    // Right click on the sequence->Replace subsequence... "Replace sequence" dialog appears.
    // Paste clipboard into text field (Cmd-V).
    // Press Enter.
    // Dialog closed, sequence changed. Now only 1 annotation remains (5' terminal repeat).
    class ReplaceSequenceScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            auto plainText = GTWidget::findPlainTextEdit(os, "sequenceEdit", dialog);
            GTWidget::click(os, plainText);

            // Select the whole sequence and replace it with '='. Try applying the change.
            GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
            GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, new ReplaceSequenceScenario()));
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace subsequence..."});

    // Remove the annotation:
    //     click on it in Sequence View to select it
    //         -> right click it
    //         -> Remove
    //         -> Selected annotations and qualifiers.
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_REMOVE, "Selected annotations and qualifiers"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "misc_feature"));
    GTMouseDriver::click(Qt::RightButton);

    // Open "Search in Sequence" Options Panel tab (Cmd-F).
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // Paste clipboard into pattern text field (Cmd-V).
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Was state: crash when "Find in sequence task" progress is 94% (same as in crash report).
    //            no crash handler appeared, but there is error in zsh (Terminal):
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7631) {
    // Check that file buttons on Workflow Dashboard "Inputs" tab for actors not visible by default works as expected (open the file with UGENE).
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE", GTUtilsMdi::activeWindow(os));
    GTKeyboardDriver::keyClick(Qt::Key_Escape);  // Cancel the wizard.

    // Go to the "Read alignment" element and set an input file.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDashboard::openTab(os, GTUtilsDashboard::Input);
    GTUtilsDashboard::clickLabelInParametersWidget(os, "Write alignment");

    GTUtilsDashboard::clickFileButtonInParametersWidget(os, "muscle_alignment.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
}

GUI_TEST_CLASS_DEFINITION(test_7635) {
    // Checks that notification container widget contains all available notifications.
    class Create10NotificationsScenario : public CustomScenario {
    public:
        void run(GUITestOpStatus&) override {
            auto stack = AppContext::getMainWindow()->getNotificationStack();
            for (int i = 0; i < 10; i++) {
                stack->addNotification("Notification " + QString::number(i + 1));
                GTGlobals::sleep(200);
            }
        }
    };
    GTThread::runInMainThread(os, new Create10NotificationsScenario());

    QString counterValue = GTUtilsNotifications::getNotificationCounterValue(os);
    CHECK_SET_ERR(counterValue == "10", "Invalid notification counter value: " + counterValue);

    auto containerWidget = GTUtilsNotifications::openNotificationContainerWidget(os);
    QList<QWidget*> notifications = GTWidget::findChildren<QWidget>(os, containerWidget, [](QWidget* w) { return qobject_cast<Notification*>(w) != nullptr; });
    CHECK_SET_ERR(notifications.count() == 10, "Invalid notification widgets count: " + QString::number(notifications.count()));

    // Check that counter value was not reset after the widget is opened.
    counterValue = GTUtilsNotifications::getNotificationCounterValue(os);
    CHECK_SET_ERR(counterValue == "10", "Invalid notification counter value: " + counterValue);
}

GUI_TEST_CLASS_DEFINITION(test_7644) {
    // Open _common_data/genbank/1seq.gen
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/1seq.gen");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Open the OP's "Search in Sequence" tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Tabs::Search);

    // Click Actions->Edit->Switch on the editing mode.
    GTUtilsSequenceView::makePanViewVisible(os, false);
    GTUtilsSequenceView::makeDetViewVisible(os);
    GTUtilsSequenceView::enableEditingMode(os);

    // Select the entire sequence.
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 70);

    // Press Backspace.
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, QMessageBox::Yes, "Would you like"));
    GTKeyboardDriver::keyClick(Qt::Key::Key_Backspace);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7645) {
    // 1. Open file "/_common_data/primer3/7645.seq"
    // 2. Run Primer 3 with default parameters
    // Expected state: there is only two files in project view - sequence and annotations
    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "7645.seq");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(os, new Primer3DialogFiller(os));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsProjectTreeView::countTopLevelItems(os) == 2, "two opened files expected");
}

GUI_TEST_CLASS_DEFINITION(test_7650) {
    // 1. Open samples/CLUSTALW/COI.aln
    // 2. Press 'Save as', and save file to its own path.
    // Expected state: message box with warinig appears.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, "Ok"));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, dataDir + "samples/CLUSTALW/", "COI.aln", ExportDocumentDialogFiller::CLUSTALW), false, true);
    GTWidget::click(os, GTAction::button(os, "Save alignment as"));
    GTUtilsProjectTreeView::click(os, "COI.aln");
}

GUI_TEST_CLASS_DEFINITION(test_7652) {
    // 1. Open files samples/CLUSTALW/COI.aln, _common_data/ugenedb/Klebsislla.sort.bam.ugenedb
    // 2. Export consensus from Klebsislla
    // 3. Switch to COI.aln
    // 4. Do menu Actions->Add->Sequence from file...
    // 5. Do not choose file, wait until export task finishes
    // Expected state: Info message 'Unable to open view because of active modal widget.' appears in the log
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTFileDialog::openFile(os, testDir + "_common_data/ugenedb/Mycobacterium.sorted.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class SimpleExport : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            GTUtilsDialog::clickButtonBox(os, GTWidget::getActiveModalWidget(os), QDialogButtonBox::Ok);
        }
    };
    //    Export consensus
    GTUtilsDialog::waitForDialog(os, new ExportConsensusDialogFiller(os, new SimpleExport()));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Export consensus..."}));
    GTWidget::click(os, GTWidget::findWidget(os, "Consensus area"), Qt::RightButton);

    class WaitLogMessage : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            GTUtilsTaskTreeView::waitTaskFinished(os);
            auto targetButton = GTWidget::findButtonByText(os, "Cancel", GTWidget::getActiveModalWidget(os));
            GTWidget::click(os, targetButton);
        }
    };

    GTLogTracer logTracer;
    GTGlobals::sleep(750);  // need pause to redraw/update ui, sometimes test can't preform next action
    GTUtilsMdi::activateWindow(os, "COI [COI.aln]");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, new WaitLogMessage()));
    GTMenu::clickMainMenuItem(os, {"Actions", "Add", "Sequence from file..."});
    CHECK_SET_ERR(logTracer.checkMessage("Unable to open view because of active modal widget."), "Expected message about not opening view not found!");
}

GUI_TEST_CLASS_DEFINITION(test_7659) {
    // 1. Open WD sampe "Call variants
    // 2. Select "Read Assembly (BAM/SAM)" worker
    // 3. Rename dataset "Dataset" -> "NewSet"
    // 4.Select "Read Sequence"
    // Expected state: dataset renamed to "NewSet" too
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");
    GTUtilsDialog::waitForDialog(os, new DatasetNameEditDialogFiller(os, "NewSet"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"rename_dataset_action"}));
    QTabBar* barWidget = GTWidget::findWidgetByType<QTabBar*>(os, GTUtilsWorkflowDesigner::getDatasetsListWidget(os), "Can't find QTabBar widget");
    GTWidget::click(os, barWidget->tabButton(0, QTabBar::RightSide), Qt::RightButton);

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    barWidget = GTWidget::findWidgetByType<QTabBar*>(os, GTUtilsWorkflowDesigner::getDatasetsListWidget(os), "Can't find QTabBar widget");
    CHECK_SET_ERR(barWidget->tabText(0) == "NewSet", "Actual dataset name on 'Read Sequence' worker is not expected 'NewSet'.");
}

GUI_TEST_CLASS_DEFINITION(test_7661) {
    // Duplicate _common_data/ugenedb/chrM.sorted.bam.ugenedb.
    QString origFilePath = testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb";
    GTFile::copy(os, origFilePath, sandBoxDir + "/chrM.sorted.bam.ugenedb");

    // Open duplicate.
    GTFileDialog::openFile(os, sandBoxDir, "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Type "chr" in the search field in the project view.
    GTUtilsProjectTreeView::filterProject(os, "chr");

    // Wait for the filtration.Found 1 result.Select it.
    GTGlobals::FindOptions options;
    options.matchPolicy = Qt::MatchFlag::MatchContains;
    GTUtilsProjectTreeView::click(os, "chrM", "Object name", Qt::MouseButton::LeftButton, options);

    // Click the cross in the search field in the project view.
    // Filter clearing has the same result
    GTUtilsProjectTreeView::filterProject(os, "");

    // Close the chrM tab.
    GTMenu::clickMainMenuItem(os, {"Actions", "Close active view"}, GTGlobals::UseKey);

    // Rename the file in the storage from "chrM.sorted.bam.ugenedb" to "Renamed.ugenedb".
    // UGENE displays the message "File Modification Detected".Click OK.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "OK", "was removed"));
    QFile f(sandBoxDir + "/chrM.sorted.bam.ugenedb");
    f.rename(sandBoxDir + "/Renamed.ugenedb");
    GTUtilsDialog::checkNoActiveWaiters(os);

    // Rename the file back to "chrM.sorted.bam.ugenedb".
    f.rename(sandBoxDir + "/chrM.sorted.bam.ugenedb");

    // Open it in UGENE again.
    GTFileDialog::openFile(os, sandBoxDir, "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: no crash
}

// Clicks the "Run Schema" menu item;
// in the "Run Schema" dialog that appears, sets
//     `inputPath` as "Load sequence" (if `inputPath` is empty, does nothing),
//     "sandbox/7667.gb" as "Save results to" (if the path of the output file is already set, does nothing),
//     checks/unchecks the "Add to project" checkbox depending on `addToProject`;
// accepts the dialog and runs the Query Designer task.
static void runSchema(GUITestOpStatus& os, const QString& inputPath = "", bool addToProject = true) {
    class RunSchemaDialogScenario : public CustomScenario {
        QString input;
        bool addToProj;

    public:
        RunSchemaDialogScenario(const QString& input, bool addToProj)
            : input(input), addToProj(addToProj) {
        }

        void run(GUITestOpStatus& os) override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            if (!input.isEmpty()) {
                GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, input));
                GTWidget::click(os, GTWidget::findToolButton(os, "tbInFile", dialog));
            }
            {
                auto out = GTWidget::findLineEdit(os, "outFileEdit", dialog);
                if (GTLineEdit::getText(os, out).isEmpty()) {
                    GTLineEdit::setText(os, out, UGUITest::sandBoxDir + "7667.gb");
                }
            }
            GTCheckBox::setChecked(os, "cbAddToProj", addToProj, dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::add(os, new Filler(os, "RunQueryDlg", new RunSchemaDialogScenario(inputPath, addToProject)));
    GTMenu::clickMainMenuItem(os, {"Actions", "Run Schema..."});
}

// Sets the Primer3 algorithm parameter "Number to return" to 3.
static void setNumberToReturn(GUITestOpStatus& os, QTableView* table) {
    GTTableView::click(os, table, 3, 1);
    GTSpinBox::setValue(os, GTWidget::findWidgetByType<QSpinBox*>(os, table, "7667-0"), 3);
}

// Sets the cell (`row`, 1) of `table` to `value`.
static void setDouble(GUITestOpStatus& os, QTableView* table, int row, double value) {
    GTTableView::click(os, table, row, 1);
    GTDoubleSpinbox::setValue(os,
                              GTWidget::findWidgetByType<QDoubleSpinBox*>(os, table, "7667-" + QString::number(row)),
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
    GTFileDialog::openFile(os, testDir + "_common_data/primer3/only_primer.uql");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    runSchema(os, testDir + "_common_data/primer3/all_settingsfiles.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 1  (0, 2)", {{850, 869}, {1022, 1041}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 2  (0, 2)", {{22, 41}, {199, 218}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 3  (0, 2)", {{850, 869}, {993, 1012}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 4  (0, 2)", {{452, 471}, {610, 629}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 5  (0, 2)", {{610, 629}, {786, 805}});
    GTUtilsProject::closeProject(os, true);
    GTUtilsMdi::activateWindow(os, "Query Designer - NewSchema");

    GTWidget::moveToAndClick(GTUtilsQueryDesigner::getItemCenter(os, "Primer"));
    auto table = GTWidget::findTableView(os, "table");
    {  // Product size ranges.
        GTTableView::click(os, table, 2, 1);
        GTLineEdit::setText(os, GTWidget::findWidgetByType<QLineEdit*>(os, table, "7667"), "100-300");
    }
    setNumberToReturn(os, table);
    setDouble(os, table, 4, 10);  // Max repeat mispriming.
    setDouble(os, table, 5, 10);  // Max template mispriming.
    setDouble(os, table, 6, 10);  // Max 3' stability.
    setDouble(os, table, 7, 20);  // Pair max repeat mispriming.
    setDouble(os, table, 8, 20);  // Pair max template mispriming.
    runSchema(os);
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 1  (0, 2)", {{199, 218}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 2  (0, 2)", {{40, 59}, {297, 316}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "Result 3  (0, 2)", {{39, 58}, {297, 316}});
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
    GTFileDialog::openFile(os, testDir + "_common_data/primer3/only_primer.uql");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::moveToAndClick(GTUtilsQueryDesigner::getItemCenter(os, "Primer"));
    runSchema(os, testDir + "_common_data/bwa/NC_000021.gbk.min.fa", false);

    setNumberToReturn(os, GTWidget::findTableView(os, "table"));
    runSchema(os, testDir + "_common_data/primer3/all_settingsfiles.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_7668) {
    // Open tree in MSA editor and check that sync mode works in all tree layouts.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsProjectTreeView::toggleView(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);
    GTUtilsDialog::add(os, new GTFileDialogUtils(os, dataDir + "/samples/Newick/COI.nwk"));
    GTWidget::click(os, GTWidget::findWidget(os, "openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto syncModeButton = GTAction::button(os, "sync_msa_action");
    auto layoutCombo = GTWidget::findComboBox(os, "layoutCombo");

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
    QStringList allRows = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after opening");

    QList<TvNodeItem*> treeNodes = GTUtilsPhyTree::getNodes(os);
    TvNodeItem* rectNodeToCollapse = selectNodeToCollapse(treeNodes);
    GTUtilsPhyTree::doubleClickNode(os, rectNodeToCollapse);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after collapse in Rectangular");
    QStringList rowsInRectCollapse = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    rowsInRectCollapse.sort();
    CHECK_SET_ERR(rowsInRectCollapse.length() == allRows.length() - 2,
                  "Invalid count of nodes after collapse in Rectangular: " + QString::number(rowsInRectCollapse.size()));

    // Circular.
    GTComboBox::selectItemByText(os, layoutCombo, "Circular");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after switch to Circular");
    QStringList rowsInSyncModeCircular = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    CHECK_SET_ERR(rowsInSyncModeCircular == allRows, "Nodes in Circular layout do not match");
    treeNodes = GTUtilsPhyTree::getNodes(os);
    TvNodeItem* circularNodeToCollapse = selectNodeToCollapse(treeNodes);
    GTUtilsPhyTree::doubleClickNode(os, circularNodeToCollapse);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after collapse in Circular");
    QStringList rowsInCircularCollapse = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    rowsInCircularCollapse.sort();
    CHECK_SET_ERR(rowsInCircularCollapse == rowsInRectCollapse, "Invalid nodes after collapse in Circular");

    // Unrooted.
    GTComboBox::selectItemByText(os, layoutCombo, "Unrooted");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after switch to Unrooted");
    QStringList rowsInSyncModeUnrooted = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    CHECK_SET_ERR(rowsInSyncModeUnrooted == allRows, "Nodes in Unrooted layout do not match");
    treeNodes = GTUtilsPhyTree::getNodes(os);
    TvNodeItem* unrootedNodeToCollapse = selectNodeToCollapse(treeNodes);
    GTUtilsPhyTree::doubleClickNode(os, unrootedNodeToCollapse);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after collapse in Unrooted");
    QStringList rowsInUnrootedCollapse = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    rowsInUnrootedCollapse.sort();
    CHECK_SET_ERR(rowsInUnrootedCollapse == rowsInRectCollapse, "Invalid nodes after collapse in Unrooted");

    // Rectangular (back).
    GTComboBox::selectItemByText(os, layoutCombo, "Rectangular");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON after switch to Rectangular");
}

GUI_TEST_CLASS_DEFINITION(test_7671) {
    // I made a small file which has the same error as file from the issue,
    // because the file from the issue was almoust 100 Mb size

    // Open _common_data/scenarios/_regression/7671/NC_051342_region.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/7671/NC_051342_region.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Call the Primer3 dialog
    // In the Primer3 Designer dialog select PT - PCR tab
    // Check in main checkbox and set Exon range : 1424 - 1606
    // Click Pick primers button
    Primer3DialogFiller::Primer3Settings settings;
    settings.rtPcrDesign = true;
    settings.exonRangeLine = "1424-1606";

    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: no crash
}

GUI_TEST_CLASS_DEFINITION(test_7680) {
    // Check that tree buttons size remains not changed on window resize.
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);
    QList<TvNodeItem*> nodes = GTUtilsPhyTree::getNodes(os);
    auto node = nodes[5];

    QRect viewRectBefore = GTUtilsPhyTree::getItemViewRect(os, node);
    GTUtilsProjectTreeView::toggleView(os);
    QRect viewRectAfter = GTUtilsPhyTree::getItemViewRect(os, node);

    CHECK_SET_ERR(viewRectBefore.width() > 0 && viewRectBefore.height() > 0, "Initial node size is 0");

    CHECK_SET_ERR(viewRectBefore.width() == viewRectAfter.width(),
                  QString("Width of the node changed: %1 vs %2").arg(viewRectBefore.width()).arg(viewRectAfter.width()));

    CHECK_SET_ERR(viewRectBefore.height() == viewRectAfter.height(),
                  QString("Height of the node changed: %1 vs %2").arg(viewRectBefore.height()).arg(viewRectAfter.height()));
}

GUI_TEST_CLASS_DEFINITION(test_7682) {
    // Check 'curvature' controls for rectangular branches.
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);
    QWidget* optionPanel = GTUtilsOptionPanelPhyTree::openTab(os);

    // Check 'curvature' slider is enabled and current value is 0.
    QSlider* curvatureSlider = GTWidget::findSlider(os, "curvatureSlider", optionPanel);
    CHECK_SET_ERR(curvatureSlider->isEnabled(), "Slider is not enabled");
    CHECK_SET_ERR(curvatureSlider->value() == 0, "By default there is no curvature");
    CHECK_SET_ERR(curvatureSlider->minimum() == 0, "Incorrect minimum curvature");
    CHECK_SET_ERR(curvatureSlider->maximum() == 100, "Incorrect maximum curvature");

    // Change slider and check that image changes too.
    QImage imageBefore = GTUtilsPhyTree::captureTreeImage(os);
    GTSlider::setValue(os, curvatureSlider, 50);
    QImage imageAfter = GTUtilsPhyTree::captureTreeImage(os);
    CHECK_SET_ERR(imageBefore != imageAfter, "Image is not changed");

    auto layoutCombo = GTWidget::findComboBox(os, "layoutCombo", optionPanel);

    // Switch to the 'Circular', check that 'curvature' is disabled.
    GTComboBox::selectItemByText(os, layoutCombo, "Circular");
    CHECK_SET_ERR(!curvatureSlider->isEnabled(), "Slider must be disabled");

    // Switch back to the 'Rectangular' and check that curvature is enabled again.
    GTComboBox::selectItemByText(os, layoutCombo, "Rectangular");
    CHECK_SET_ERR(curvatureSlider->isEnabled(), "Slider must be re-enabled");
    CHECK_SET_ERR(curvatureSlider->value() == 50, "Slider value must be restored, current value: " + QString::number(curvatureSlider->value()));
}

GUI_TEST_CLASS_DEFINITION(test_7686) {
    // Check "copy tree image to clipboard".
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);

    GTClipboard::clear(os);
    GTMenu::clickMainMenuItem(os, {"Actions", "Tree image", "Copy to clipboard"});
    GTClipboard::checkHasNonEmptyImage(os);

    // Zoom so image becomes very large: UGENE should show an error message.
    GTUtilsPhyTree::zoomWithMouseWheel(os, 20);
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "OK", "Image is too large. Please zoom out."));
    GTMenu::clickMainMenuItem(os, {"Actions", "Tree image", "Copy to clipboard"});
}

GUI_TEST_CLASS_DEFINITION(test_7697) {
    // Check that tree settings are saved/restored correctly.
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);

    auto panel1 = GTUtilsOptionPanelPhyTree::openTab(os);
    CHECK_SET_ERR(GTCheckBox::getState(os, "showNamesCheck", panel1) == true, "Invalid default showNamesCheck state");
    GTCheckBox::setChecked(os, "showNamesCheck", false, panel1);
    CHECK_SET_ERR(GTCheckBox::getState(os, "showDistancesCheck", panel1) == true, "Invalid default showDistancesCheck state");
    GTCheckBox::setChecked(os, "showDistancesCheck", false, panel1);

    auto curvatureSlider = GTWidget::findSlider(os, "curvatureSlider", panel1);
    CHECK_SET_ERR(curvatureSlider->value() == 0, "Invalid default curvatureSlider value: " + QString::number(curvatureSlider->value()));
    GTSlider::setValue(os, curvatureSlider, 20);

    CHECK_SET_ERR(GTComboBox::getCurrentText(os, "treeViewCombo", panel1) == "Default", "Invalid default treeViewCombo value");
    GTComboBox::selectItemByText(os, "treeViewCombo", panel1, "Cladogram");

    GTUtilsMdi::closeActiveWindow(os);

    GTUtilsProjectTreeView::doubleClickItem(os, "COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);

    auto panel2 = GTUtilsOptionPanelPhyTree::openTab(os);
    CHECK_SET_ERR(GTCheckBox::getState(os, "showNamesCheck", panel2) == false, "showNamesCheck state is not restored");
    CHECK_SET_ERR(GTCheckBox::getState(os, "showDistancesCheck", panel2) == false, "showDistancesCheck state is not restored");
    CHECK_SET_ERR(GTWidget::findSlider(os, "curvatureSlider", panel2)->value() == 20, "curvatureSlider state is not restored");
    CHECK_SET_ERR(GTComboBox::getCurrentText(os, "treeViewCombo", panel2) == "Cladogram", "treeViewCombo state is not restored");
}

GUI_TEST_CLASS_DEFINITION(test_7700) {
    // Create a 250 Unicode character path. See
    //     https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=registry
    // Open _common_data/scenarios/_regression/7700/bwa.uwl
    //     The "Choose Output Directory" dialog appears.
    // Set the created folder as the required directory in this dialog.
    // Click OK.
    //     The Workflow window appears.
    // Set _common_data/bwa/control-chr21.fastq and nrsf-chr21.fastq as input to the "Read File URL(s)" element.
    // Set _common_data/bwa/NC_000021.gbk.fa as the "Reference genome" of the "Map Reads with BWA" element.
    // Run workflow.
    //     Expected: the workflow task finished successfully with one output file "output.sam".
    // Open the output.sam
    //     The "Import SAM File" dialog appears.
    // Click "Import".
    //     Expected: the ugenedb file is successfully created in the default directory and opens without problems, the
    //         Assembly Browser shows position 45 890 375 with coverage 2316 as the first well-covered region.
    QString sandboxPath = QFileInfo(sandBoxDir).canonicalFilePath();
    int requiredNumOfChars = 250 - sandboxPath.size();
    QString longPath = sandboxPath + QString::fromWCharArray(L"/\u221E").repeated(requiredNumOfChars / 2);
    CHECK_SET_ERR(QDir().mkpath(longPath), "Failed to create dir '" + longPath + "'");

    class WorkflowOutputScenario : public CustomScenario {
        QString path;

    public:
        explicit WorkflowOutputScenario(const QString& path)
            : path(path) {
        }
        void run(GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTLineEdit::setText(os, "pathEdit", path, dialog, false, true);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new Filler(os, "StartupDialog", new WorkflowOutputScenario(longPath)));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/7700/bwa.uwl");
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive(os);

    GTUtilsWorkflowDesigner::click(os, "Read File URL(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFiles(
        os, {testDir + "_common_data/bwa/control-chr21.fastq", testDir + "_common_data/bwa/nrsf-chr21.fastq"});

    GTUtilsWorkflowDesigner::click(os, "Map Reads with BWA");
    GTUtilsWorkflowDesigner::setParameter(os,
                                          "Reference genome",
                                          testDir + "_common_data/bwa/NC_000021.gbk.fa",
                                          GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));
    GTUtilsDashboard::clickOutputFile(os, "out.sam");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList positions = GTLabel::getText(os, "CoveredRegionsLabel")
                                .section("href=\"0\">", 1, 1)
                                .split("</a></td><td align=\"center\">");
    QString positionStr = positions.first();
    QString coverageStr = positions[1].section(
        "</td></tr><tr><td align='right'>2&nbsp;&nbsp;</td><td><a href=\"1\">", 0, 0);
    auto toInt = [](QString str) {
        return str.remove(' ').toInt();
    };
    CHECK_SET_ERR(toInt(positionStr) == 45'890'375 && toInt(coverageStr) == 2'316,
                  QString("The first well-covered region: expected 45 890 375 -- 2 316, current %1 -- %2")
                      .arg(positionStr, coverageStr));
}

GUI_TEST_CLASS_DEFINITION(test_7712) {
    class FilterShortScaffoldsWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            GTWidget::getActiveModalWidget(os);
            GTUtilsWizard::setInputFiles(os, {{testDir + "_common_data/genbank/1anot.gen"}});
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Filter short sequences", new FilterShortScaffoldsWizard()));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Filter short scaffolds..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString notification = GTUtilsDashboard::getJoinedNotificationsString(os);
    CHECK_SET_ERR(notification.contains("Nothing to write"), "Unexpected notification message: " + notification);
}

GUI_TEST_CLASS_DEFINITION(test_7714) {
    // Open the file 1.bam.
    // Check the box "Deselect all", "Import unmapped reads" and import the file.
    // Expected state: UGENE not crashed
    GTLogTracer l;
    qint64 expectedReads = 10;

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_7714/test_7714.ugenedb", "", "", true, true));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
    qint64 assemblyReads1 = GTUtilsAssemblyBrowser::getReadsCount(os);
    CHECK_SET_ERR(assemblyReads1 == expectedReads, QString("An unexpected assembly reads count: expect  %1, got %2").arg(expectedReads).arg(assemblyReads1));

    GTUtilsMdi::closeActiveWindow(os);

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_7714/test_7714_1.ugenedb", "", "", true, true));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
    qint64 assemblyReads2 = GTUtilsAssemblyBrowser::getReadsCount(os);
    CHECK_SET_ERR(assemblyReads2 == expectedReads, QString("An unexpected assembly reads count: expect  %1, got %2").arg(expectedReads).arg(assemblyReads2));

    GTUtilsLog::check(os, l);
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
    GTLogTracer ltConnect("QObject::connect(U2::MaEditorWgt, U2::MaGraphOverview): invalid nullptr parameter");
    GTLogTracer ltSize("QWidget::setMinimumSize: (msa_editor_sequence_area/U2::MSAEditorSequenceArea) Negative sizes");
    GTLogTracer ltSizeNameList("QWidget::setMinimumSize: (msa_editor_name_list/U2::MsaEditorNameList) Negative sizes");
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsMSAEditorSequenceArea::click(os);

    GTMouseDriver::press();
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(GTWidget::getWidgetCenter(GTWidget::findWidget(
        os, GTUtilsOptionPanelMsa::tabsNames[GTUtilsOptionPanelMsa::General])));
    GTMouseDriver::release();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // The background is white, the bars are gray, the background in the selection is light gray, the bars
    // in the selection are dark gray, the selection frame is black. Total 5 colors.
    CHECK_SET_ERR(GTWidget::countColors(GTWidget::getImage(os, GTUtilsMsaEditor::getGraphOverview(os))).size() == 5,
                  "Overview is empty (white)");

    GTUtilsMsaEditor::setMultilineMode(os, true);
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Remove all gaps"});
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkContainsMessage(os, ltConnect, false);
    GTUtilsLog::checkContainsMessage(os, ltSize, false);
    GTUtilsLog::checkContainsMessage(os, ltSizeNameList, false);
}

GUI_TEST_CLASS_DEFINITION(test_7740) {
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);

    // Select root node.
    TvNodeItem* rootNode = GTUtilsPhyTree::getRootNode(os);
    GTUtilsPhyTree::clickNode(os, rootNode);

    // Swap Siblings button must be disabled.
    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    auto swapSiblingsButton = GTToolbar::getWidgetForActionObjectName(os, toolbar, "Swap Siblings");
    CHECK_SET_ERR(!swapSiblingsButton->isEnabled(), "Swap siblings must be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_7744) {
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Select "GC Deviation (G-C)/(G+C)"  or "AT Deviation (A-T)/(A+T)" graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Deviation (G-C)/(G+C)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTUtilsDialog::checkNoActiveWaiters(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"AT Deviation (A-T)/(A+T)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTUtilsDialog::checkNoActiveWaiters(os);
}

GUI_TEST_CLASS_DEFINITION(test_7748) {
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, "", testDir + "_common_data/fasta/broken", "empty_name_multi.fa"));
    GTFileDialog::openFile(os, dataDir + "samples/Assembly/chrM.sam");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
}

GUI_TEST_CLASS_DEFINITION(test_7751) {
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive(os);

    // Select any inner node
    GTUtilsPhyTree::clickNode(os, GTUtilsPhyTree::getNodeByBranchText(os, "0.009", "0.026"));

    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    auto swapSiblingsButton = GTToolbar::getWidgetForActionObjectName(os, toolbar, "Swap Siblings");
    CHECK_SET_ERR(swapSiblingsButton->isEnabled(), "Swap siblings must be enabled");

    // Click Swapping Siblings button on the toolbar
    GTWidget::click(os, swapSiblingsButton);
    CHECK_SET_ERR(swapSiblingsButton->isEnabled(), "Swap siblings must be enabled");
    GTUtilsPhyTree::getNodeByBranchText(os, "0.026", "0.009");

    GTWidget::click(os, swapSiblingsButton);
    CHECK_SET_ERR(swapSiblingsButton->isEnabled(), "Swap siblings must be enabled");
    GTUtilsPhyTree::getNodeByBranchText(os, "0.009", "0.026");
}

}  // namespace GUITest_regression_scenarios
}  // namespace U2
