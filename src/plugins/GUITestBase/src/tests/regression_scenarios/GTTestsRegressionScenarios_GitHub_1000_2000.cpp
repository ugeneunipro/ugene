/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "GTTestsRegressionScenarios_GitHub_1000_2000.h"
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMainWindow.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <utils/GTKeyboardUtils.h>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2ObjectDbi.h>

#include <U2Gui/Theme.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotationsTreeView.h>
#include <U2View/DetView.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MsaEditor.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTDatabaseConfig.h"
#include "GTGlobals.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "primitives/GTAction.h"
#include "primitives/GTLabel.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindTandemsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/GraphSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PredictSecondaryStructureDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportConsensusDialogFiller.h"
#include "runnables/ugene/corelibs/U2View//ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/cap3/CAP3SupportDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ImportAnnotationsToCsvFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/CreateFragmentDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/EditFragmentDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastLocalSearchDialogFiller.h"
#include "runnables/ugene/plugins/weight_matrix/PwmBuildDialogFiller.h"
#include "runnables/ugene/plugins/weight_matrix/PwmSearchDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithCommandLineToolFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithScriptDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/clustalw/ClustalWDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/AnyDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTClipboard.h"
#include "system/GTFile.h"
#include "utils/GTThread.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_regression_scenarios_github_issues {

GUI_TEST_CLASS_DEFINITION(test_1790) {
    /*
     * Open human_T1.fa.
     * Open the "Find restriction sites" dialog.
     * Type "qwerty" to the "Filter by name".
     * Expected: Nothing found.
     * Remove "werty" and leaver only "q".
     * Expected: Some enzymes are visible.
     * Clear filter.
     * Expected: All enzymes are visible.
     **/
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class TryNamesInSearchEdit : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTLineEdit::setText("enzymesFilterEdit", "qwerty", dialog);
            auto enzymesTree = GTWidget::findTreeWidget("tree", GTWidget::findWidget("enzymesSelectorWidget"));
            CHECK_SET_ERR(GTTreeWidget::countVisibleItems(enzymesTree) == 21, "Item list should be empty");
            auto findLEWidget = GTWidget::findWidget("enzymesFilterEdit", dialog);
            GTWidget::click(findLEWidget);
            GTKeyboardDriver::keyClick(Qt::Key_End);
            for (int i = 0; i < 5; i++) {
                GTKeyboardDriver::keyClick(Qt::Key_Backspace);
            }
            const int wEnzymesCount = GTTreeWidget::countVisibleItems(enzymesTree);
            CHECK_SET_ERR(wEnzymesCount > 21, "Item list shouldn't be empty");
            GTWidget::click(findLEWidget);
            GTKeyboardDriver::keyClick(Qt::Key_Backspace);
            CHECK_SET_ERR(GTTreeWidget::countVisibleItems(enzymesTree) > wEnzymesCount, "Item list size should be bigger than with 'w'");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList {}, new TryNamesInSearchEdit()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Analyze", "Find restriction sites..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

}

GUI_TEST_CLASS_DEFINITION(test_1794) {
    // Open "samples/Assembly/chrM.sam" and import it to ugenedb.
    // Click to the first visible position (6950).
    // Click right mose button -> Export -> Assembly region.
    // Expected: Region - visible and start and end are about 6000-10000
    // Current: Region - visible and start and end are 1-16570
    // Switch "Visible" to "Custom region" and back to "Visible">
    // Current: SAFE_POINT

    class Scenario : public CustomScenario {
    public:
        void run() override {
            GTLogTracer lt;
            QWidget* dialog = GTWidget::getActiveModalWidget();
            int start = GTLineEdit::getText("start_edit_line", dialog).toInt();
            int end = GTLineEdit::getText("end_edit_line", dialog).toInt();
            CHECK_SET_ERR(start >= 6000 && end < 10000, QString("Unexpected region: start: %1, end: %2").arg(start).arg(end));

            GTComboBox::selectItemByText("region_type_combo", dialog, "Custom region");
            GTComboBox::selectItemByText("region_type_combo", dialog, "Visible");
            CHECK_SET_ERR(!lt.hasErrors(), QString("Unexpected errors"));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "/chrM.sorted.bam.ugenedb");
    GTFileDialog::openFile(sandBoxDir + "/chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAssemblyBrowser::zoomToMax();
    GTUtilsDialog::add(new PopupChooserByText({"Export", "Assembly region"}));
    GTUtilsDialog::add(new AnyDialogFiller("ExtractAssemblyRegionDialog", new Scenario()));
    GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Reads);
}

GUI_TEST_CLASS_DEFINITION(test_1810) {
    /*
     * 1. Open COI.aln.
     * 2. Build a tree and make it open in a new window in Display Options tab -> COI.hwk is created in new window.
     * 3. Select opened COI.aln in the view.
     * 4. In Option View select tree tab
     * 5. Open created COI.nwk from COI.aln options panel
     * Expected state: previously opened view activated
     */

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI_test_1810.nwk", 0, 0, false));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::activateWindow("COI [COI.aln]");

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/sandbox/COI_test_1810.nwk"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsMdi::activeWindowTitle() == "Tree [COI_test_1810.nwk]", "Unexpected active window title");
}

GUI_TEST_CLASS_DEFINITION(test_1812) {
    // Open murine.gb
    // Click Shift + T
    // Expected: no new annotations created
    // Select "5' terminal repeat" and "3' terminal repeat" annotations
    // Click Shift + T
    // Expecte 2 new primer annotations created
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto annotatedRegions1 = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    GTKeyboardDriver::keyClick('T', Qt::ShiftModifier);
    auto annotatedRegions2 = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    CHECK_SET_ERR(annotatedRegions1.size() == annotatedRegions2.size(), QString("No new annotations should be created"));

    GTUtilsSequenceView::clickAnnotationPan("misc_feature", 2);
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationPan("misc_feature", 5245);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('T', Qt::ShiftModifier);
    auto annotatedRegions3 = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    CHECK_SET_ERR(annotatedRegions3.size() == annotatedRegions2.size() + 2, QString("Two new annotations should be created"));
}

GUI_TEST_CLASS_DEFINITION(test_1818) {
    /*
     * 1. Open "data/samples/Genbank/human_T1.fa".
     * 2. Open "Search in Sequence" options panel tab.
     * 3. Insert "AYCG" pattern.
     * 4. Open section "Search agorithm", set algorithm - Substitute, check - Search with ambiguous bases.
     * 5. Set algorithm - Exact
     * Expected result: 2738 results are found.
     */

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");
    GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases();
    GTUtilsOptionPanelSequenceView::enterPattern("AYCG");
    GTUtilsOptionPanelSequenceView::setAlgorithm("Exact");
    CHECK_SET_ERR(GTWidget::findLabel("lblErrorMessage")->isVisible(), QString("Warning label should be visible"));
}

GUI_TEST_CLASS_DEFINITION(test_1819) {
    // Open human_T1.fa
    // Open empty_ugene_name.gb
    // Attach annotations from the second file to the first file.
    // Expected: no errors
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTFileDialog::openFile(testDir + "_common_data/genbank/", "empty_ugene_name.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject("Annotation features");
    CHECK_SET_ERR(!lt.hasErrors(), QString("Unexpected errors"));
}

GUI_TEST_CLASS_DEFINITION(test_1829) {
    /*
     * 1. Open COI.aln
     * 2. Open highlighting option panel tab, check "Use dots" checkbox
     * 3. Switch to multiline mode
     * Expected state: "Use dots" still checked
     * 4. Switch back from multilone mode
     * Expected state: "Use dots" still checked
     **/
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTCheckBox::setChecked("useDots", true);
    GTUtilsMsaEditor::setMultilineMode(true);
    GTCheckBox::checkState("useDots", true);
    GTUtilsMsaEditor::setMultilineMode(false);
    GTCheckBox::checkState("useDots", true);
}

GUI_TEST_CLASS_DEFINITION(test_1831) {
    /*
     * 1. Open COI.aln
     * 2. Select "Zychia_baranovi" sequence
     * 3. Open Options panel, General tab
     * 4. Select Plain text format
     * 5. Press Copy button
     * Expected state: no error
     **/
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::selectRowsByName({"Zychia_baranovi"});
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTLogTracer lt;
    GTComboBox::selectItemByText(GTWidget::findComboBox("copyType"), "Plain text");
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    CHECK_SET_ERR(!lt.hasErrors(), QString("Unexpected errors"));
}

GUI_TEST_CLASS_DEFINITION(test_1842) {
    // Click "Create Sequence".
    // Type "AYT" as sequence, check "Custom settings", set "Standard amino acid" as alphabet.
    // Set path to the output file and click "Create".
    // Expected: [amino] alpabet.

    auto filler = new CreateDocumentFiller(
        "AYT",
        true,
        CreateDocumentFiller::StandardAmino,
        true,
        false,
        "",
        testDir + "_common_data/scenarios/sandbox/test_1842.fa",
        CreateDocumentFiller::FASTA,
        "test_1842");

    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    auto label = GTWidget::findLabel("nameLabel", GTWidget::findWidget("ADV_single_sequence_widget_0"));
    CHECK_SET_ERR(label->text().contains("[amino]"), QString("Unexpected label of sequence name: %1, must contain %2").arg(label->text()).arg("[amino]"));
}

GUI_TEST_CLASS_DEFINITION(test_1857) {
    /*
     * 1. Open a sequence in the Sequence View.
     * 2. Open "Search in Sequence" tab in the options panel.
     * 3. Check "Load patterns from file", start manually input a string in the "Path" field.
     * Expected state: tip field contains error "Please input a valid file with patterns.
     **/
    //usePatternFromFileRadioButton filePathLineEdit
    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();
    GTLineEdit::setText("filePathLineEdit", "sasasas");
    auto lblErrorMessage = GTWidget::findLabel("lblErrorMessage");
    auto errorText = lblErrorMessage->text();
    CHECK_SET_ERR(errorText.contains("Error: please input a valid file with patterns."), QString("Unexpected or empty error: '%1'").arg(errorText));
}

GUI_TEST_CLASS_DEFINITION(test_1871) {
    // Open murine.gb.
    // Open the "Search pattern" tab.
    // Expand show/hide "Save annotation's to".
    // Expected: murine.gb [NC_001363 features]
    // Select "[a] NC_001363 features" in the prohect view, click F2 (rename) and set name 'Test".
    // Expected: "murine.gb [NC_001363 features]" renamed to "murine.gb [Test]" on the the "Search pattern" tab.
    // Expand show/hide "Annotation parameters".
    // Click on the blue star button in front of "Group name".
    // Expected: no SAFE_POINT and errors in the log.

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::openSaveAnnotationToShowHideWidget();
    auto item = GTComboBox::getCurrentText("cbExistingTable");
    CHECK_SET_ERR(item == "murine.gb [NC_001363 features]", "Expected item 'NC_001363 features' is not found in the list");

    GTUtilsProjectTreeView::rename("NC_001363 features", "Test");
    item = GTComboBox::getCurrentText("cbExistingTable");
    CHECK_SET_ERR(item == "murine.gb [Test]", "Expected item 'Test' is not found in the list");

    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget();
    GTLogTracer lt;
    GTWidget::click(GTWidget::findWidget("tbSelectGroupName"));
    lt.assertNoErrors();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
  
GUI_TEST_CLASS_DEFINITION(test_1873) {
    /*
     * 1. Open general/_common_data/fasta/abcd.fa1.gb, push OK in Sequence Reading Options dialog
     * 2. Select Dotplot dialog, push OK
     * 3. Select Dotplot dialog once more, push OK
     * 4. Select Remove-> Selected sequence from second fitplot view from context menu x3 times
     * Crash!
     **/

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/abcd.fa1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    
    GTUtilsDialog::waitForDialog(new DotPlotFiller());
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);

    GTUtilsDialog::waitForDialog(new DotPlotFiller());
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);
    
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    for (int i = 0; i < 3; i++) {
        GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove", "Selected sequence from view"}));
        GTWidget::click(GTWidget::findWidget("DotPlotWidget1"), Qt::RightButton);
    }
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_1877) {
    /*
     * 1. Load corrupted ugenedb
     * Expected state: Loading documents finished with error
     * 2. Select context menu on it and select "Open In->Open in Sanger Editor"
     * Expected state: no crash, error message in log
     **/
    GTFileDialog::openFile(testDir + "_common_data/regression/1877/sanger_wrong.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Open In", "Open new view: Sanger Reads Editor"}));
    GTUtilsProjectTreeView::click("sanger_wrong.ugenedb", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasError("Document can't be loaded"), "Expected message is not found");
}

GUI_TEST_CLASS_DEFINITION(test_1883) {
    /*
     * 1. Load big fasta with many sequences
     * 2. Open with default option
     * Expected state: file not loaded
     * 3. Open it in alignment editor by calling context menu with "Open In" item
     * 4. Delete file from project while it loading
     * Expected state: loading interrupted, error message in the log, no crash
     **/
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/GSM1313963_S1-21d-KMB17+HAVH2.cluster.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Open In", "Open new view: Multiple Alignment Editor"}));
    GTUtilsProjectTreeView::click("GSM1313963_S1-21d-KMB17+HAVH2.cluster.fa", Qt::RightButton);
    GTUtilsDialog::add(new PopupChooserByText({"Remove selected items"}));
    GTUtilsProjectTreeView::click("GSM1313963_S1-21d-KMB17+HAVH2.cluster.fa", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasError("Multiple alignment object not found"), "Expected message is not found");    
}
  
GUI_TEST_CLASS_DEFINITION(test_1887) {
    /*
     * 1. Open a sequence.
     * 2. Create a new annotation with the following group name inserted by copy/paste: "1//2/3".
     * Expected state: text isn't pasted
     * 3. Create a new annotation with the following group entered by keyboard: "1//2/3".
     * Expected state: because of filter group name should be 123, no errors in the log.
     **/

    class FillGroupName : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QLineEdit* groupNameLE = qobject_cast<QLineEdit*>(GTWidget::findWidget("leGroupName", dialog));
            CHECK_SET_ERR(groupNameLE != nullptr, "groupNameLE is null!");
            GTRadioButton::click(GTWidget::findRadioButton("rbGenbankFormat", dialog));
            GTLineEdit::setText("leLocation", "10..20", dialog);
            for (bool byCopyPaste : {true, false}) {
                GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Group name can't contain"));
                GTLineEdit::setText(groupNameLE, QString("1//2/3"), true, byCopyPaste);
                GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new FillGroupName()));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    lt.assertNoErrors();
}

GUI_TEST_CLASS_DEFINITION(test_1896) {
    // Open big.aln
    // Select sequence "seq1"
    // Click "Realign sequence(s) to other sequences" -> "Align selected sequences to alignment with MAFFT"
    // Remove big.aln from the project
    // The error notification appeared: The multiple alignment is no more available.
    GTFileDialog::openFile(testDir + "_common_data/clustal", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::selectRowsByName({"seq1"});
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Align selected sequences to alignment with MAFFT"}, GTGlobals::UseMouse, Qt::MatchFlag::MatchContains));
    GTWidget::click(GTAction::button("align_selected_sequences_to_alignment"));

    GTUtilsProjectTreeView::click("big.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::checkNotificationDialogText("The multiple alignment is no more available.");
}

}  // namespace GUITest_regression_scenarios_github_issues

}  // namespace U2
