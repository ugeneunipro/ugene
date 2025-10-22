/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
    GTFileDialog::openFile(testDir + "_common_data/primer3/custom_primers.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsAnnotationsTreeView::clickItem("primer1", 1, false);
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsAnnotationsTreeView::clickItem("primer2", 1, false);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    GTKeyboardDriver::keyClick('t', Qt::ShiftModifier);

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{50, 79}, {400, 435}});
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

}  // namespace GUITest_regression_scenarios_github_issues

}  // namespace U2
