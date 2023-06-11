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

#include <GTGlobals.h>
#include <api/GTUtils.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
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
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>
#include <utils/GTUtilsToolTip.h>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTextStream>
#include <QWizard>
#include <QtWidgets/QTextBrowser>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2ObjectDbi.h>

#include <U2Gui/ProjectViewModel.h>
#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/AnnotationsTreeView.h>
#include <U2View/AssemblyBrowser.h>
#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorTreeViewer.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/MaEditorNameList.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTTestsRegressionScenarios_1001_2000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsEscClicker.h"
#include "GTUtilsExternalTools.h"
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
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTGraphicsItem.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/qt/EscapeClicker.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/BuildIndexDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindRepeatsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/GenerateAlignmentProfileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/cap3/CAP3SupportDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportBlastResultDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ImportAnnotationsToCsvFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/CreateFragmentDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/RemoteBLASTDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/TCoffeeDailogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithCommandLineToolFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/MAFFT/MAFFTSupportRunDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/hmm3/HmmerSearchDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/NCBISearchDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SelectDocumentFormatDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {

GUI_TEST_CLASS_DEFINITION(test_1001) {
    GTUtilsProject::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsProject::openFile(testDir + "_common_data/fasta/human_T1_cutted.fa");

    GTUtilsDialog::waitForDialog(new DotPlotFiller(4));
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

    GTGlobals::sleep(15000);
}

GUI_TEST_CLASS_DEFINITION(test_1001_1) {
    GTUtilsProject::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsProject::openFile(testDir + "_common_data/fasta/human_T1_cutted.fa");

    GTUtilsDialog::waitForDialog(new DotPlotFiller(100, 50));
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));
}

GUI_TEST_CLASS_DEFINITION(test_1001_2) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::openFile(testDir + "_common_data/fasta/human_T1_cutted.fa");

    GTUtilsDialog::waitForDialog(new DotPlotFiller(99, 99, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_1001_3) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::openFile(testDir + "_common_data/fasta/human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(99, 99, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_1001_4) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::openFile(testDir + "_common_data/fasta/human_T1_cutted.fa");

    GTUtilsDialog::waitForDialog(new DotPlotFiller(99, 99, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);
    GTUtilsMdi::click(GTGlobals::Close);
}

GUI_TEST_CLASS_DEFINITION(test_1003) {
    // 1. Open "murine.gb", search for all available restriction sites (i.e. click "Select All" in the "Find Restriction Sites" dialog).
    // Expected state: UGENE not hangs at 100% completion of 'Auto-annotation update task'
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class Scenario_test_1003 : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/archive/2013_08_01.bairoch.gz"));
            GTWidget::click(GTWidget::findWidget("enzymesFileButton", dialog));

            auto enzymesSelectorWidget = GTWidget::findWidget("enzymesSelectorWidget");
            GTWidget::click(GTWidget::findWidget("selectAllButton", enzymesSelectorWidget));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished();
            GTThread::waitForMainThread();
        }
    };

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList(), new Scenario_test_1003()));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_1011) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto pw = GTWidget::findWidget("pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QList<QScrollBar*> widgetList = pw->findChildren<QScrollBar*>();
    CHECK_SET_ERR(widgetList.size() == 2, "scroll bars count should be 2");
    foreach (QScrollBar* sbar, widgetList) {
        CHECK_SET_ERR(sbar->maximum() == 0, "maximum is not 0");
        CHECK_SET_ERR(sbar->minimum() == 0, "minimum is not 0");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1013) {
    // 1) Open any MSA (data\samples\MSF\HMA.msf)
    // 2) Right-click on sequences name list.
    // Expected state: "Edit -> replace selected rows with rev-complement" presend in popup menu
    // 3) Start the selection by clicking on any sequence in the alignment area and moving the pointer to the whitespace
    // Expected state: selection is present
    // 4) Start the selection by clicking on the whitespace ("Consensus" widget counts) and moving the pointer to any sequence in the alignment area
    // Expected state: selection is present

    GTFileDialog::openFile(dataDir + "samples/MSF/", "HMA.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(-1, 0), GTGlobals::UseMouse);
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1015) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(3));
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1015_1) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(30, 50));
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

    GTUtilsMdi::click(GTGlobals::Close);
    // GTUtilsMdi::click(GTGlobals::Minimize);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1015_2) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(100, 50, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsMdi::click(GTGlobals::Close);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1015_3) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(100, 50, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);

    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsMdi::click(GTGlobals::Close);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1015_4) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(100, 50, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);

    GTUtilsMdi::click(GTGlobals::Close);
    // GTUtilsMdi::click(GTGlobals::Minimize);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1016) {
    //    1) Open "\test\_common_data\scenarios\_regression\1016\eg1.sam"
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1016/eg1.sam");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: "Import SAM File" dialog appeared, not "select format" dialog
}

GUI_TEST_CLASS_DEFINITION(test_1020) {
    GTLogTracer lt;

    // 1. Open file data / CLUSTALW / COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. In MSA context menu choose "Statistics" > "Generate distance matrix".
    // 3. Try to generate distance matrix with both "Hamming dissimilarity" and "Identity" algorithms.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(true, true, true));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    CHECK_SET_ERR(GTUtilsMdi::activeWindow()->windowTitle() == "Distance matrix for COI", "Unexpected active window name");

    GTUtilsMdi::activateWindow("COI [COI.aln]");

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(false, true, true));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    CHECK_SET_ERR(GTUtilsMdi::activeWindow()->windowTitle() == "Distance matrix for COI", "Unexpected active window name");

    GTUtilsMdi::activateWindow("COI [COI.aln]");

    // 4. Then run this dialog in "Profile mode".
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new GenerateAlignmentProfileDialogFiller());
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTThread::waitForMainThread();

    CHECK_SET_ERR(GTUtilsMdi::activeWindow()->windowTitle() == "Alignment profile for COI", "Unexpected active window name: " + GTUtilsMdi::activeWindow()->windowTitle());

    GTUtilsMdi::activateWindow("COI [COI.aln]");

    // 5. Finally, try to save results as *.html and *.csv files.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(DistanceMatrixDialogFiller::HTML, sandBoxDir + "test_1020.html"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    CHECK_SET_ERR(QFileInfo::exists(sandBoxDir + "test_1020.html"), "Distance matrix file not found");

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_STATISTICS, "Generate distance matrix"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(DistanceMatrixDialogFiller::CSV, sandBoxDir + "test_1020.csv"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    CHECK_SET_ERR(QFileInfo::exists(sandBoxDir + "test_1020.csv"), "Distance matrix file not found");

    // Expected result : Distance matrix is generated and / or saved correctly in all cases.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1021) {
    GTUtilsMdi::click(GTGlobals::Close);
    for (int i = 0; i < 2; i++) {
        // 1) Open data\samples\FASTA\human_T1.fa
        GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
        GTUtilsTaskTreeView::waitTaskFinished();

        // 2) Click "build dotplot" tooltip
        // 3) Click OK in opened dotplot dialog
        GTUtilsDialog::waitForDialog(new DotPlotFiller(100));
        GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

        // 4) Click on human_T1.fa project tree view item
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
        GTMouseDriver::click();

        // 5) Press delete key
        GTKeyboardDriver::keyClick(Qt::Key_Delete);

        // Expected state: there are no empty MDI window opened, no bookmarks
        QTreeWidget* bookmarksTree = GTUtilsBookmarksTreeView::getTreeWidget();
        CHECK_SET_ERR(bookmarksTree != nullptr, "bookmarksTreeWidget is NULL");

        int bookmarksCount = bookmarksTree->topLevelItemCount();
        CHECK_SET_ERR(bookmarksCount == 0, "there are bookmarks");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1021_1) {
    GTUtilsMdi::click(GTGlobals::Close);
    for (int i = 0; i < 3; i++) {
        // 1) Open data\samples\FASTA\human_T1.fa
        GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
        GTUtilsTaskTreeView::waitTaskFinished();

        // 2) Click "build dotplot" tooltip
        // 3) Click OK in opened dotplot dialog
        GTUtilsDialog::add(new DotPlotFiller(100));
        GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

        // 4) Click on human_T1.fa project tree view item
        GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
        GTMouseDriver::click();

        // 5) Press delete key
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
        GTUtilsTaskTreeView::waitTaskFinished();

        QTreeWidget* bookmarksTree = GTUtilsBookmarksTreeView::getTreeWidget();
        CHECK_SET_ERR(bookmarksTree != nullptr, "bookmarksTreeWidget is NULL");

        int bookmarksCount = bookmarksTree->topLevelItemCount();
        CHECK_SET_ERR(bookmarksCount == 0, "there are bookmarks");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1021_2) {
    GTUtilsMdi::click(GTGlobals::Close);
    for (int i = 0; i < 2; i++) {
        // 1) Open data\samples\FASTA\human_T1.fa
        GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
        GTUtilsTaskTreeView::waitTaskFinished();

        // 2) Click "build dotplot" tooltip
        // 3) Click OK in opened dotplot dialog
        GTUtilsDialog::waitForDialog(new DotPlotFiller(100));
        GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

        // 4) Click on human_T1.fa project tree view item
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
        GTMouseDriver::click();

        // 5) Press delete key
        GTKeyboardDriver::keyClick(Qt::Key_Delete);

        QTreeWidget* bookmarksTree = GTUtilsBookmarksTreeView::getTreeWidget();
        CHECK_SET_ERR(bookmarksTree != nullptr, "bookmarksTreeWidget is NULL");

        int bookmarksCount = bookmarksTree->topLevelItemCount();
        CHECK_SET_ERR(bookmarksCount == 0, "there are bookmarks");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1021_3) {
    GTUtilsMdi::click(GTGlobals::Close);
    for (int i = 0; i < 2; i++) {
        // 1) Open data\samples\FASTA\human_T1.fa
        GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
        GTUtilsTaskTreeView::waitTaskFinished();

        // 2) Click "build dotplot" tooltip
        // 3) Click OK in opened dotplot dialog
        GTUtilsDialog::add(new DotPlotFiller(120, 100, true));
        GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));
        GTThread::waitForMainThread();

        if (i == 0) {
            // GTUtilsMdi::click(GTGlobals::Minimize);
            GTGlobals::sleep();
        }

        // 4) Click on human_T1.fa project tree view item
        GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
        GTMouseDriver::click();
        GTThread::waitForMainThread();

        // 5) Press delete key
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
        QTreeWidget* bookmarksTree = GTUtilsBookmarksTreeView::getTreeWidget();
        CHECK_SET_ERR(bookmarksTree != nullptr, "bookmarksTreeWidget is NULL");

        int bookmarksCount = bookmarksTree->topLevelItemCount();
        CHECK_SET_ERR(bookmarksCount == 0, "there are bookmarks");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1021_4) {
    GTUtilsMdi::click(GTGlobals::Close);
    for (int i = 0; i < 2; i++) {
        // 1) Open data\samples\FASTA\human_T1.fa
        GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
        GTUtilsTaskTreeView::waitTaskFinished();

        // 2) Click "build dotplot" tooltip
        // 3) Click OK in opened dotplot dialog
        GTUtilsDialog::add(new DotPlotFiller(110, 100, true));
        GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

        if (i == 0) {
            // GTUtilsMdi::click(GTGlobals::Minimize);
            GTGlobals::sleep();
        }

        // 4) Click on human_T1.fa project tree view item
        GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
        GTMouseDriver::click();

        // 5) Press delete key
        GTKeyboardDriver::keyClick(Qt::Key_Delete);

        QTreeWidget* bookmarksTree = GTUtilsBookmarksTreeView::getTreeWidget();
        CHECK_SET_ERR(bookmarksTree != nullptr, "bookmarksTreeWidget is NULL");

        int bookmarksCount = bookmarksTree->topLevelItemCount();
        CHECK_SET_ERR(bookmarksCount == 0, "there are bookmarks");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1022) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(3));
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();

    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsDialog::add(new PopupChooser({"Dotplot", "Remove"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::showContextMenu(GTWidget::findWidget("dotplot widget"));
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1029) {
    //    1. Open all files from "samples/genbank/" folder in separate mode
    //    2. Close all views except one
    //    3. Add other sequences to existing view using "add to view" from project context menu
    //    4. Close all opened circular views with buttons on sequence view toolbar
    //    5. Open circular views with button "Toggle circular views" on the UGENE toolbar
    //    Expected state: all opened sequences have circular views
    //    6. Close circular views with same button
    //    Expected state: all circular views are closed

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Separate;
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(dataDir + "samples/Genbank/", {"murine.gb", "sars.gb", "CVU55762.gb", "PBR322.gb", "NC_014267.1.gb"}));

    GTMenu::clickMainMenuItem({"File", "Open..."});

    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList windowsNames;
    windowsNames << "NC_001363 [murine.gb]"
                 << "NC_004718 [sars.gb]"
                 << "CVU55762 [CVU55762.gb]"
                 << "SYNPBR322 [PBR322.gb]";

    foreach (const QString& window, windowsNames) {
        GTUtilsMdi::closeWindow(window);
    }
    GTUtilsMdi::closeWindow("Start Page");

    foreach (const QString& window, windowsNames) {
        GTUtilsDialog::waitForDialog(new PopupChooserByText({"Add to view", "Add to view: NC_014267 [NC_014267.1.gb]"}));
        QString seqName = window.left(window.indexOf("[") - 1);
        GTUtilsProjectTreeView::click(seqName, Qt::RightButton);
    }

    class MainThreadScenario : public CustomScenario {
    public:
        void run() override {
            auto scroll = GTWidget::findScrollArea("annotated_DNA_scrollarea");
            int seqNum = GTUtilsSequenceView::getSeqWidgetsNumber();
            for (int i = 0; i < seqNum; i++) {
                ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(i);
                scroll->ensureWidgetVisible(seqWgt);
                if (GTUtilsCv::isCvPresent(seqWgt)) {
                    GTUtilsCv::cvBtn::click(seqWgt);
                }
            }
        }
    };

    GTThread::runInMainThread(new MainThreadScenario());

    GTUtilsCv::commonCvBtn::click();

    int seqNum = GTUtilsSequenceView::getSeqWidgetsNumber();
    for (int i = 0; i < seqNum; i++) {
        ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(i);
        CHECK_SET_ERR(GTUtilsCv::isCvPresent(seqWgt), QString("No CV for %1 single sequence view. First check").arg(i));
    }

    GTUtilsCv::commonCvBtn::click();

    for (int i = 0; i < seqNum; i++) {
        ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(i);
        CHECK_SET_ERR(!GTUtilsCv::isCvPresent(seqWgt), QString("No CV for %1 single sequence view. Second check").arg(i));
    }
}

GUI_TEST_CLASS_DEFINITION(test_1037) {
    //    1) Open any Assembly file (*.bam, *.sam)
    QString ugenedbFileName = testDir + "_common_data/scenarios/sandbox/test_1037.ugenedb";

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(ugenedbFileName));
    GTFileDialog::openFile(testDir + "_common_data/bam", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Open two SNP files (*.snp, *.vcf)
    GTFileDialog::openFile(testDir + "_common_data/snp/simple.snp");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/snp/valid.snp");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    3) Drag-n-drop SNP object files to assembly browser
    QModelIndex simple = GTUtilsProjectTreeView::findIndex("simple.snp");
    QModelIndex chr1 = GTUtilsProjectTreeView::findIndex("chr1", simple);
    GTUtilsProjectTreeView::dragAndDrop(chr1, GTWidget::findWidget("assembly_reads_area"));
    GTWidget::findWidget("AssemblyVariantRow_chr1");

    QModelIndex chr10 = GTUtilsProjectTreeView::findIndex("chr10");
    GTUtilsProjectTreeView::dragAndDrop(chr10, GTWidget::findWidget("assembly_reads_area"));
    GTWidget::findWidget("AssemblyVariantRow_chr10");
    //    Expected state: snp visualization appears one after another under reference sequence visualization
}

GUI_TEST_CLASS_DEFINITION(test_1038) {
    //    1. Open WD
    //    2. Create a scheme with the following elments: "Read NGS Reads Assembly", "Write sequence", "Split assembly into sequences"
    //    3. Set samples/Assembly/chrM.sorted.bam as an input assembly. Set any output sequence file
    //    4. Run the scheme
    //    5. Open chrM in assembly browser
    //    Expected: output sequences file has the same sequences as you can see in Assmbly Browser

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* readAsmbl = GTUtilsWorkflowDesigner::addElement("Read NGS Reads Assembly");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/small.bam.sorted.bam");

    WorkflowProcessItem* split = GTUtilsWorkflowDesigner::addElement("Split Assembly into Sequences");
    WorkflowProcessItem* writeSeq = GTUtilsWorkflowDesigner::addElement("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_1038_seq", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(readAsmbl, split);
    GTUtilsWorkflowDesigner::connect(split, writeSeq);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(sandBoxDir + "test_1038_seq");
    GTUtilsTaskTreeView::waitTaskFinished();

    // get the list of sequences in file and read names in assembly
    Document* seqDoc = GTUtilsDocument::getDocument("test_1038_seq");
    QList<GObject*> seqList = seqDoc->findGObjectByType(GObjectTypes::SEQUENCE, UOF_LoadedAndUnloaded);
    CHECK_SET_ERR(!seqList.isEmpty(), "The list of sequences is empty");
    QList<QByteArray> seqNames;
    foreach (GObject* obj, seqList) {
        CHECK_SET_ERR(obj != nullptr, "GObject is NULL");
        seqNames << obj->getGObjectName().toLatin1();
    }

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_1038_bam"));
    GTFileDialog::openFile(testDir + "_common_data/bam", "small.bam.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    AssemblyBrowserUi* ui = GTUtilsAssemblyBrowser::getView("ref_and_others [test_1038_bam]");
    QSharedPointer<AssemblyModel> model = ui->getModel();

    U2OpStatus2Log u2os;
    U2DbiIterator<U2AssemblyRead>* it = model->getReads(U2Region(0, model->getModelLength(u2os)), u2os);
    CHECK_SET_ERR(it != nullptr, "NULL iterator");

    int matchCount = 0;
    while (it->hasNext()) {
        U2AssemblyRead read = it->next();
        if (seqNames.contains(read->name)) {
            matchCount++;
        }
    }

    CHECK_SET_ERR(matchCount == seqNames.size(), QString("Number of reads and sequences are not matched: got %1, expected %2").arg(matchCount).arg((seqNames.size())));
}

GUI_TEST_CLASS_DEFINITION(test_1044) {
    // 1. Open "data/samples/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. In MSA context menu choose {Align -> Align with MUSCLE…}.
    // Expected state: "Align with MUSCLE" dialog appears.
    // 3. Set checkbox "Translate to amino when aligning".
    // Expected state: The "Translation table" field becomes enabled.
    // 4. Press the "Align" dialog button.
    // Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Default, true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1047) {
    //    1. Open \samples\Assembly\chrM in Assembly Browser
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_1047.ugenedb", dataDir + "samples/Assembly", "chrM.fa"));
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.sam");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Choose a color scheme in options panel of assembly browser. Try zooming with mouse wheel.
    GTWidget::click(GTWidget::findWidget("OP_ASS_SETTINGS"));
    auto colorBox = GTWidget::findComboBox("READS_HIGHLIGHTNING_COMBO");
    GTComboBox::selectItemByText(colorBox, "Strand direction");
    //    Bug state: Zoom and color scheme changing simultaneously.
    QWidget* assembly_reads_area = GTWidget::findWidget("assembly_reads_area");
    QPoint p = assembly_reads_area->mapToGlobal(assembly_reads_area->rect().center());
    GTMouseDriver::moveTo(p);
    GTMouseDriver::scroll(10);
    //    Expected state: only zoom is changed
    QString currText = colorBox->currentText();
    CHECK_SET_ERR(currText == "Strand direction", "Color scheme unexpectidly changed");
}

GUI_TEST_CLASS_DEFINITION(test_1048) {
    //    Open a few assembly views.
    GTFile::copy(testDir + "_common_data/bam/chrM.sorted.bam", testDir + "_common_data/scenarios/sandbox/1.bam");
    GTFile::copy(testDir + "_common_data/bam/chrM.sorted.bam", testDir + "_common_data/scenarios/sandbox/2.bam");
    //    Switch on "Show pop-up hint" option in one of them and switch it off in others.
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/1.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/2.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* act = GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "readHintEnabledAction");
    GTWidget::click(act);

    QWidget* w2 = GTUtilsMdi::activeWindow();
    GTWidget::click(w2);
    for (int i = 0; i < 50; i++) {
        GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);
    }
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    bool vis = GTWidget::findWidget("AssemblyReadsAreaHint", GTUtilsMdi::activeWindow())->isVisible();
    CHECK_SET_ERR(!vis, "hint unexpectidly visiable");

    GTUtilsMdi::activateWindow("chrM [1.ugenedb]");

    QWidget* w1 = GTUtilsMdi::activeWindow();
    GTWidget::click(w1);
    for (int i = 0; i < 50; i++) {
        GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);
    }
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(20, 20));
    vis = GTWidget::findWidget("AssemblyReadsAreaHint", GTUtilsMdi::activeWindow())->isVisible();
    CHECK_SET_ERR(vis, "hint unexpectidly invisiable");

    //    Expected state: Popups are shown in views where it is switched on, and are not shown is views where it is switched off
}

GUI_TEST_CLASS_DEFINITION(test_1049) {
    //    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Click the "Enable collapsing" button on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();
    //    Expected state: some sequences are collapsed into two groups.

    //    3. Click {Statistics->Generate distance matrix} in the context menu.
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    4. Check the "Show group statistics of multiple alignment" checkbox and press the "Generate" button.
            auto groupStatisticsCheck = GTWidget::findCheckBox("groupStatisticsCheck", dialog);
            GTCheckBox::setChecked(groupStatisticsCheck, true);
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(new custom()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_STATISTICS", "Generate distance matrix"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    //    Expected state: the "Generate Distance matrix" dialog appeared.

    //    Expected state: Statistics View opened, it contains two tables: full statistics and additional group statistics.
    QTextBrowser* v = GTUtilsMdi::activeWindow()->findChild<QTextBrowser*>();
    QString text = v->toHtml();
    CHECK_SET_ERR(text.contains("Group statistics of multiple alignment"), text);
}

GUI_TEST_CLASS_DEFINITION(test_1052) {
    qputenv("UGENE_DISABLE_ENZYMES_OVERFLOW_CHECK", "1");  // disable overflow to create a long-running "Find Enzymes task".

    //    1. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open "Find restriction sites" dialog
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    3. Select all sites.
            GTWidget::click(GTWidget::findWidget("selectAllButton", dialog));
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList(), new custom()));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    //    4. Close sequence view immediately
    GTUtilsMdi::click(GTGlobals::Close);

    //    Expected state: UGENE does not crash
    QString title = GTUtilsMdi::activeWindow()->windowTitle();
    CHECK_SET_ERR(title == "Start Page", "unexpected title: " + title);
}

GUI_TEST_CLASS_DEFINITION(test_1058) {
    GTLogTracer lt;
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/1058/", "file.gff"));
    GTUtilsDialog::add(new DocumentFormatSelectorDialogFiller("GFF"));
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTMenu::clickMainMenuItem({"File", "Open as..."});
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_1059) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Place the "Write sequence" element on the scene
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence");

    // 3. Set 'Output file' parameter 'sample'
    GTUtilsWorkflowDesigner::setParameter("Output file", "sample", GTUtilsWorkflowDesigner::textValue);

    // 4. Copy&Paste this worker
    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Write Sequence"));
    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    const QPoint workerCenterPos = GTUtilsWorkflowDesigner::getItemCenter("Write Sequence");
    GTMouseDriver::moveTo(workerCenterPos);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(workerCenterPos + QPoint(100, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    GTLineEdit::setText(GTWidget::findLineEdit("nameEdit"), "Write Sequence 1");

    // 5. Change 'Output file' parameter to 'ssss' at fist worker
    // Expected state : 'Output file' parameter for 2nd worker not 'sssss'
    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Write Sequence"));
    GTUtilsWorkflowDesigner::setParameter("Output file", "sssss", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Write Sequence 1"));
    const QString outputFile = GTUtilsWorkflowDesigner::getParameter("Output file");
    CHECK_SET_ERR(outputFile == "sample", "Unexpected output file parameter value: " + outputFile);
}

GUI_TEST_CLASS_DEFINITION(test_1061) {
    class CreateMarkerDialogFiller : public Filler {
    public:
        CreateMarkerDialogFiller()
            : Filler("EditMarkerDialog") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto markerNameEdit = GTWidget::findLineEdit("markerNameEdit", dialog);
            GTLineEdit::setText(markerNameEdit, "1");

            GTWidget::click(GTWidget::findWidget("containsButton", dialog));
            GTLineEdit::setText(GTWidget::findLineEdit("containsEdit"), "1");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    class AddMarkerDialogFiller : public Filler {
    public:
        AddMarkerDialogFiller()
            : Filler("EditMarkerGroupDialog") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto typeBox = GTWidget::findComboBox("typeBox", dialog);
            GTComboBox::selectItemByText(typeBox, "Qualifier text value markers");

            auto addParamEdit = GTWidget::findLineEdit("addParamEdit", dialog);
            GTLineEdit::setText(addParamEdit, "protein_id");

            GTUtilsDialog::waitForDialog(new CreateMarkerDialogFiller());
            GTWidget::click(GTWidget::findWidget("addButton", dialog));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Create a scheme with "Read sequence", "Write sequence", "Filter", "Sequence marker" element.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Sequence marker");
    GTUtilsWorkflowDesigner::addAlgorithm("Filter", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    // 2. In the "Marker" element create a text qualifier marker containing value "1"
    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Sequence Marker"));
    GTUtilsDialog::waitForDialog(new AddMarkerDialogFiller());
    GTWidget::click(GTWidget::findWidget("addButton"));

    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Filter"));
    GTUtilsWorkflowDesigner::setParameter("Filter by value(s)", "1", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/Genbank/murine.gb");

    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Write Sequence"));
    GTUtilsWorkflowDesigner::setParameter("Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);

    // 3. Connect all elements, setup connections, provide any annotated sequence as an input
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Sequence Marker"));
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Sequence Marker"), GTUtilsWorkflowDesigner::getWorker("Filter"));
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Filter"), GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    // 4. Run the scheme
    // Expected state : UGENE does not crash
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1063) {
    class EnableWdDebuggerFiller : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto tree = GTWidget::findTreeWidget("tree", dialog);

            QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
            foreach (QTreeWidgetItem* item, items) {
                if (item->text(0) == "  Workflow Designer") {
                    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
                    GTMouseDriver::click();
                }
            }

            GTCheckBox::setChecked(GTWidget::findCheckBox("debuggerBox", dialog), true);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 1) Set "Enable debugger" in Settings->WD
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new EnableWdDebuggerFiller()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    // 2) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 3) Create schema{ Read sequence->Write sequence }, set valid input and output files
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");

    // 4) Right - click on Read sequence element, click "Toggle breakpoint"
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Break at element..."}));
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click(Qt::RightButton);

    // 5) Run workflow
    GTUtilsWorkflowDesigner::runWorkflow();

    // Expected state : Workflow is paused, "Pause scheme" button is disabled
    QAbstractButton* pauseButton = GTAction::button("Pause workflow");
    CHECK_SET_ERR(pauseButton->isVisible() && !pauseButton->isEnabled(), "'Pause workflow' button is either invisible or active unexpectedly");

    // 6) Click "Run schema" button
    GTUtilsNotifications::waitForNotification(true, "The task 'Execute workflow' has been finished");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTThread::waitForMainThread();
    // Expected state : run finished successfully
}

GUI_TEST_CLASS_DEFINITION(test_1064) {
    // 1) Open "_common_data\regression\1064\test_data.sam"
    // Expected state: "Import SAM file" dialog opened with Reference lineedit and red text saying that the SAM file doesn't contain the header
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_1064.ugenedb", dataDir + "samples/Assembly", "chrM.fa"));
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.sam");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1065_1) {
    //    Building index with the "Bowtie" tool.
    //    1. Select "Tools->Align to reference->Build index" from the main menu.
    //    Expected state: the "Build index" dialog appeared.
    //    2. Fill the dialog with the next values:
    //        {Align short reads method}    Bowtie
    //        {reference sequence}        _common_data/scenarios/_regression/1065/e_coli_1000.fa
    //        {Index file name}        _tmp/e_coli_1000
    //    Click the "Start" button.
    //    Expected state: task completes successfully, there are six files in the _tmp folder:
    //    e_coli_1000.1.ebwt
    //    e_coli_1000.2.ebwt
    //    e_coli_1000.3.ebwt
    //    e_coli_1000.4.ebwt
    //    e_coli_1000.rev.1.ebwt
    //    e_coli_1000.rev.2.ebwt
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new BuildIndexDialogFiller(testDir + "_common_data/scenarios/_regression/1065/", "e_coli_1000.fa", "Bowtie", false, sandBoxDir, "e_colli_1000"));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Build index for reads mapping..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    QDir dir(sandBoxDir);
    CHECK_SET_ERR(dir.entryList(QDir::Files).count() == 6, QString("Incorrect count of index files: got %1, expected 6").arg(dir.entryList(QDir::Files).count()));
    foreach (const QString& fileName, dir.entryList(QDir::Files)) {
        CHECK_SET_ERR(fileName.endsWith("ebwt"), "Incorrect result file");
    }

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1065_2) {
    //    Align short reads with the "Bowtie" tool by index.
    //    1. Select {Tools->Align to reference->Align short reads} in the main menu.
    //    Expected state: the "Align sequencing reads" dialog appeared.
    //    2. Fill the dialog with next values:
    //        {Alignmnet method}    Bowtie
    //        {Reference sequence}    _common_data/scenarios/_regression/1065/index/e_coli_100
    //        {Result file name}    _tmp/e_coli_1000.sam
    //        {Prebuild index}    checked
    //        {Short reads}        _common_data/scanarios/_regression/1065/e_coli_1000.fq
    //    And click the "Start" button.
    //    Expected state: the Bowtie task successfully completes, the "Import SAM file" dialog appeared.
    //    3. Set the {Destination URL} field with "_tmp/e_coli_1000.sam.ugenedb" value and click "Import" button.
    //    Expected state: SAM importing task successfully finish, the Assembly Viewer opens.
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "/1065_2"));
    AlignShortReadsFiller::Parameters p(testDir + "_common_data/scenarios/_regression/1065/", "e_coli_1000.fa", testDir + "_common_data/scenarios/_regression/1065/", "e_coli_1000.fq", AlignShortReadsFiller::Parameters::Bowtie);
    p.prebuiltIndex = true;
    p.useDefaultResultPath = false;
    p.resultDir = sandBoxDir;
    p.resultFileName = "1065_2";

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(&p));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1065_3) {
    //    Bowtie align without prebuilt index.
    //    1. Select {Tools->Align to reference->Align short reads} in the main menu.
    //    Expected state: the "Align sequencing reads" dialog appeared.
    //    2. Fill the dialog with next values:
    //        {Alignmnet method}    Bowtie
    //        {Reference sequence}    _common_data/scenarios/_regression/1065/e_coli_100.fa
    //        {Result file name}    _tmp/e_coli_1000.sam
    //        {Prebuild index}    unchecked
    //        {Short reads}        _common_data/scanarios/_regression/1065/e_coli_1000.fq
    //    And click the "Start" button.
    //    Expected state: the Bowtie task successfully completes, the "Import SAM file" dialog appeared.
    //    3. Set the {Destination URL} field with "_tmp/e_coli_1000.sam.ugenedb" value and click "Import" button.
    //    Expected state: SAM importing task successfully finish, the Assembly Viewer opens.
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "/1065_3"));
    AlignShortReadsFiller::Parameters p(testDir + "_common_data/scenarios/_regression/1065/", "e_coli_1000.fa", testDir + "_common_data/scenarios/_regression/1065/", "e_coli_1000.fq", AlignShortReadsFiller::Parameters::Bowtie);
    p.useDefaultResultPath = false;
    p.resultDir = sandBoxDir;
    p.resultFileName = "1065_3";

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(&p));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1068) {
    //    1) Open _common_data/scenarios/regression/1068/Oppa.uwl
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/1068/Oppa.uwl"));
    GTWidget::click(GTAction::button("Load workflow"));
    //    Expected state: Workflow was not loaded. Error in log
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_1069) {
    // 1. Open any sequence
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Switch on auto-annotations
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Close the sequence view
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    GTThread::waitForMainThread();

    // 4. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: auto-annotations are switched off
    QTreeWidgetItem* annItem = GTUtilsAnnotationsTreeView::findFirstAnnotation({false});
    CHECK_SET_ERR(annItem == nullptr, "Auto-annotations are switched on");
}

GUI_TEST_CLASS_DEFINITION(test_1071) {
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTTreeWidget::click(GTUtilsAnnotationsTreeView::findItem("CDS"));

    // 1. Open edit annotation name and region dialog (by F2).
    // 2. Enter an invalid region (e.g. asdfsadf12..25).
    // 3. Click OK.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTRadioButton::click("rbGenbankFormat", dialog);
            GTLineEdit::setText("leLocation", "asdfsadf12..25", dialog);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new EditAnnotationFiller(new Scenario()));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    // expected state: Error message appears, UGENE does not crash.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1078) {  // Need to add the test
    /* 1. File-New Project
     * 2. Open as - HannaRescued.fa(https://ugene.net/tracker/browse/UGENE-1078) (FASTA format. Score:13 (Perfect match) - OK, As separate sequences in sequence viewer - OK)
     * Bug state: Then crush and hung of the program
     * Expected state: Error message with format error
     * System: Ubuntu 12.04
     */
    GTUtilsNotifications::waitForNotification(false);
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/1078/", "HannaRescued.fa"));
    GTUtilsDialog::add(new DocumentFormatSelectorDialogFiller("FASTA"));
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTMenu::clickMainMenuItem({"File", "Open as..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    auto textEdit = GTWidget::findTextEdit("reportTextEdit", GTUtilsMdi::activeWindow());
    CHECK_SET_ERR(textEdit->toPlainText().contains("Loaded sequences: 24."), "Expected message is not found in the report text");
}

GUI_TEST_CLASS_DEFINITION(test_1080) {
    class OkClicker2 : public Filler {
    public:
        OkClicker2()
            : Filler("EditMarkerDialog") {
        }
        void run() override {
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTKeyboardDriver::keySequence("0.001");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    class OkClicker : public Filler {
    public:
        OkClicker()
            : Filler("EditMarkerGroupDialog") {
        }
        void run() override {
            GTUtilsDialog::waitForDialog(new OkClicker2());

            QWidget* w = GTWidget::getActiveModalWidget();

            auto table = GTWidget::findTableView("table", w);
            GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 0, 0));
            GTMouseDriver::click();

            GTWidget::click(GTWidget::findToolButton("editButton", w));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/regression/1080", "blast+marker_new.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::click("Sequence Marker");

    auto table = GTWidget::findTableView("markerTable");
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 0, 0));
    GTMouseDriver::click();

    auto editButton = GTWidget::findToolButton("editButton");

    GTUtilsDialog::waitForDialog(new OkClicker());
    GTWidget::click(editButton);
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/Genbank/PBR322.gb");

    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir().absoluteFilePath(sandBoxDir) + "wd_test_1080.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    // Allow task to start and check there are no errors
    QString taskName = "Execute workflow";
    GTUtilsTaskTreeView::checkTaskIsPresent(taskName);
    QString taskStatus = GTUtilsTaskTreeView::getTaskStatus(taskName);
    CHECK_SET_ERR(taskStatus == "Running", "The task status is incorrect: " + taskStatus);
    GTUtilsTaskTreeView::cancelTask(taskName);  // Cancel task because we don't need the result
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1083) {
    // 1. open files data\samples\FASTA\human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. select sequence oject in projrct tree view. press delete
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: UGENE is not crashed
    GTUtilsProject::checkProject(GTUtilsProject::Empty);
}

GUI_TEST_CLASS_DEFINITION(test_1093) {
    //    1. do menu "tools->Align to refrence->Align short reads"

    //    2. fill next fields in appeared dialog:
    //        Refrence sequence "_common_data\scenarios\_regression\1093\refrence.fa"
    //        SAM output checkbox {set checked}
    //        Short reads add next file: "_common_data\scenarios\_regression\1093\read.fa"

    //    3. Press 'Align'
    //    Expected state: message window appears "The short reads can't be mapped to the reference sequence!"
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/scenarios/_regression/1093/",
                                                 "refrence.fa",
                                                 testDir + "_common_data/scenarios/_regression/1093/",
                                                 "read.fa");
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    // The short reads can't be mapped to the reference sequence!
}

GUI_TEST_CLASS_DEFINITION(test_1107) {
    // 1) Open an MSA file (e.g. _common_data\scenarios\msa\ma2_gapped.aln)
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) Menu File->Close Project
    // 3) Press No in the Save current project dialog
    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    // Expected state: UGENE not crashes
}

GUI_TEST_CLASS_DEFINITION(test_1113) {  // commit AboutDialogController.cpp
    // 1. Open UGENE
    // 2. Press F1
    class EscClicker : public Filler {
    public:
        EscClicker()
            : Filler("AboutDialog") {
        }
        virtual void run() {
            if (isOsMac()) {
                GTKeyboardDriver::keyRelease(GTKeyboardDriver::key[Qt::Key_F1]);
            }

            QWidget* dialog = GTWidget::getActiveModalWidget();
            // getting an info string
            auto w = GTWidget::findWidget("about_widget", dialog);

            QObject* parent = w->findChild<QObject*>("parent");
            CHECK_SET_ERR(parent != nullptr, "parentObject not found");

            QObject* child = parent->findChild<QObject*>();
            CHECK_SET_ERR(child != nullptr, "childObject not found");

            QString text = child->objectName();
            CHECK_SET_ERR(text.contains("64-bit") || text.contains("32-bit"), text);
            if (isOsMac()) {
                GTWidget::click(GTWidget::findWidget("close_button"));
            } else {
                GTKeyboardDriver::keyClick(Qt::Key_Escape);
            }
        }
    };
    GTUtilsDialog::waitForDialog(new EscClicker());
    if (isOsMac()) {
        // hack for mac
        MainWindow* mw = AppContext::getMainWindow();
        CHECK_SET_ERR(mw != nullptr, "MainWindow is NULL");
        QMainWindow* mainWindow = mw->getQMainWindow();
        CHECK_SET_ERR(mainWindow != nullptr, "QMainWindow is NULL");
        auto w = qobject_cast<QWidget*>(mainWindow);
        GTWidget::click(w, Qt::LeftButton, QPoint(5, 5));
    }
    GTKeyboardDriver::keyClick(Qt::Key_F1);
    // Expected state: About dialog appeared, shown info includes platform info (32/64)
}

GUI_TEST_CLASS_DEFINITION(test_1113_1) {  // commit AboutDialogController.cpp
    // 1. Open UGENE
    // 2. Help->About
    class EscClicker : public Filler {
    public:
        EscClicker()
            : Filler("AboutDialog") {
        }
        virtual void run() {
            if (isOsMac()) {
                GTMouseDriver::release();
            }
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // getting an info string
            auto w = GTWidget::findWidget("about_widget", dialog);

            QObject* parent = w->findChild<QObject*>("parent");
            CHECK_SET_ERR(parent != nullptr, "parentObject not found");

            QObject* child = parent->findChild<QObject*>();
            CHECK_SET_ERR(child != nullptr, "childObject not found");

            QString text = child->objectName();
            CHECK_SET_ERR(text.contains("64-bit") || text.contains("32-bit"), text);
            if (isOsMac()) {
                GTWidget::click(GTWidget::findWidget("close_button"));
            } else {
                GTKeyboardDriver::keyClick(Qt::Key_Escape);
            }
        }
    };

    GTUtilsDialog::waitForDialog(new EscClicker());
    GTMenu::clickMainMenuItem({"Help", "About"});
    // Expected state: About dialog appeared, shown info includes platform info (32/64)
}

GUI_TEST_CLASS_DEFINITION(test_1115) {
    GTFileDialog::openFile(testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/genbank/PBR322_blast_annotations.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Add BLAST annotations to the sequence view.
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    const QModelIndex table = GTUtilsProjectTreeView::findIndex("SYNPBR322 features", GTUtilsProjectTreeView::findIndex("PBR322_blast_annotations.gb"));
    GTUtilsProjectTreeView::dragAndDrop(table, GTUtilsAnnotationsTreeView::getTreeWidget());
    GTUtilsDialog::checkNoActiveWaiters();

    //    3. Select two or more BLAST annotations.
    QList<QTreeWidgetItem*> blastResultItems = GTUtilsAnnotationsTreeView::findItems("blast result");
    CHECK_SET_ERR(blastResultItems.size() >= 2, "Not enough BLAST results");

    GTUtilsAnnotationsTreeView::selectItems({blastResultItems.first(), blastResultItems.last()});

    QStringList expectedNames = QStringList() << GTUtilsAnnotationsTreeView::getQualifierValue("accession", blastResultItems.first())
                                              << GTUtilsAnnotationsTreeView::getQualifierValue("accession", blastResultItems.last());

    //    4. Use menu {Export->Export blast result to alignment}.
    //    5. Click "Export".
    QDir().mkpath(sandBoxDir + "test_1115");
    GTUtilsDialog::waitForDialog(new ExportBlastResultDialogFiller(sandBoxDir + "test_1115/test_1115.aln"));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export BLAST result to alignment"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(blastResultItems.first());
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    6. Check that annotations are correctly exported.
    GTUtilsDocument::checkDocument("test_1115.aln", MsaEditorFactory::ID);

    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names == expectedNames, QString("Unexpected msa rows names: expect '%1', got '%2'").arg(expectedNames.join(", ")).arg(names.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_1121) {
    GTLogTracer lt;

    // 1) Open alignment with amino alphabet
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "amino_ext.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) join line with nucl alphabet.
    GTUtilsDialog::add(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/fasta/", "fa1.fa"));
    GTMenu::showContextMenu(GTWidget::findWidget("msa_editor_name_list"));

    // 3) Then delete line with amino alphabet.
    GTUtilsMSAEditorSequenceArea::selectSequence("FOSB_HUMAN");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state : Ugene did not crash on assert
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1122) {
    //    1. Select "Tools->DNA Assembly->Contig assembly with CAP3" from the main menu.
    //    Expected state: the "Contig Assembly With CAP3" dialog appeared.
    //    2. Click "Add" button.
    //    Expected state: the "Add sequences to assembly" dialog appeared.
    //    3. Select two files: "_common_data/scenarios/CAP3/xyz.fa" and "_common_data/scenarios/CAP3/xyz.qual". Click "Open" button.
    //    Expected state: this files had been added to the "Input files" field of the "Contig Assembly With CAP3" dialog. Some path in the "Result contig" field appeared.
    //    4. Click the "Run" button.
    //    Expected state: CAP3 task started. Multiply alignment as a result of the task appeared.
    //    5. Run CAP3 again with "_common_data/scenarios/CAP3/xyz.fastq".
    //    Expected state: Result is the same as in the step 4.

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTUtilsDialog::waitForDialog(new CAP3SupportDialogFiller({testDir + "_common_data/scenarios/CAP3/xyz.fa", testDir + "_common_data/scenarios/CAP3/xyz.qual"}, sandBoxDir + "test_1122_1"));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTUtilsDialog::waitForDialog(new CAP3SupportDialogFiller({testDir + "_common_data/scenarios/CAP3/xyz.fastq"}, sandBoxDir + "test_1122_2"));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTFile::equals(sandBoxDir + "test_1122_1.ace", sandBoxDir + "test_1122_2.ace"), "Files are not equal");

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1123) {
    // 1. Select {Tools->DNA assembly->Contig assembly with CAP3} in the main menu.
    //  Expected state: the "Contig Assembly With CAP3" dialog appeared.
    //  2. Fill the dialog with values:
    //  {Input files}:
    //  "_common_data/abif/19_022.ab1"
    //  "_common_data/abif/39_034.ab1"
    //  {Result contig}: "_tmp/1123_abi.cap.ace"
    //  And click the "Run" button.
    //  Expected state: task complete successfully, result file opens in the MSA Editor (or in the Assembly Viewer).

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_1123"));
    GTUtilsDialog::waitForDialog(new CAP3SupportDialogFiller({testDir + "_common_data/abif/19_022.ab1", testDir + "_common_data/abif/39_034.ab1"}, sandBoxDir + "1123_abi.cap.ace"));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}
GUI_TEST_CLASS_DEFINITION(test_1123_1) {
    // 1. Select {Tools->DNA assembly->Contig assembly with CAP3} in the main menu.
    // Expected state: the "Contig Assembly With CAP3" dialog appeared.
    // 2. Fill the dialog with values:
    // {Input files}:
    // "_common_data/scf/Sequence A.scf"
    // "_common_data/scf/Sequence B.scf"
    // {Result contig}: "_tmp/1123_scf.cap.ace"
    // And click the "Run" button.
    // Expected state: task complete successfully, result file opens in the MSA Editor (or in the Assembly Viewer).

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_1123_1"));
    GTUtilsDialog::waitForDialog(new CAP3SupportDialogFiller({testDir + "_common_data/scf/Sequence A.scf", testDir + "_common_data/scf/Sequence A.scf"}, sandBoxDir + "1123_scf.cap.ace"));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1124) {
    //    1. Select {Tools->DNA assembly->Contig assembly with CAP3} in main menu ("CAP3" external tool must be installed).
    //    Expected state: "Contig Assembly With CAP3" dialog appeared.

    class Scenario : public CustomScenario {
    public:
        void run() {
            GTGlobals::sleep();
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Hover the {Input files (long DNA reads to assembly)} field with mouse and wait the tooltip appeares.
            auto seqList = GTWidget::findListWidget("seqList", dialog);
            GTWidget::click(seqList);
            GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(10, 10));
            //    Expected state: tooltip is presented.
            GTUtilsToolTip::checkExistingToolTip("The quality scores for FASTA sequences can be provided in an additional file");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new CAP3SupportDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});
}

GUI_TEST_CLASS_DEFINITION(test_1133) {
    //     1. Open human_t1.fa
    //     2. Open Smith-Waterman search dialog
    //     3. Paste sequence from text file to pattern field
    //     4. Run search
    //
    //     Expected state: Search successfully perfoms
    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    QString patttern = "ATGAA    GGAAAAA\nA T G CTA AG GG\nCAGC    CAGAG AGAGGTCA GGT";
    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(patttern));
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find pattern [Smith-Waterman]"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    GTUtilsAnnotationsTreeView::findItem("misc_feature  (0, 1)");
}

GUI_TEST_CLASS_DEFINITION(test_1152) {
    // 1. Open human_t1.fa
    // 2. Open Find Pattern bar on the Options Pannel
    // 3. Copy a few subsequences of human_t1 in the pattern area putting each of them on a new line
    // 4. Press Enter
    // 5. All the subsequences are found on right places

    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("TAACG");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);
    GTKeyboardDriver::keySequence("AAAAAA");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);

    // Expected state : 1 pattern is found
    auto resultLabel = GTWidget::findLabel("resultLabel");
    CHECK_SET_ERR(resultLabel->text() == "Results: 1/1328", "Unexpected find algorithm result count");
}

GUI_TEST_CLASS_DEFINITION(test_1152_1) {
    // 1. Open human_t1.fa
    // 2. Open Find Pattern bar on the Options Pannel
    // 3. Copy a few subsequences of human_t1 in any file. Put each puttern on a new line
    // 4. Check "Load Pattern(s) from file"
    // 5. Select the file that you've created in [3]
    // 6. Press "Search"
    // Expected state: All patterns're found on their places

    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(testDir + "_common_data/scenarios/_regression/1285/", "small.fa");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/1"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_1155) {
    // 1. Open corresponding schema (_common_data/scenarios/regression/1155/crash.uwl)
    // 2. Set the correct input sequence file for Read Sequence.
    // 3. Do not the change output file for Write Sequence.
    // 4. Run the schema.
    // Expected state: UGENE not crashed

    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1155", "crash.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/Genbank/sars.gb");

    if (isOsWindows()) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    }
    GTUtilsWorkflowDesigner::runWorkflow();
}

GUI_TEST_CLASS_DEFINITION(test_1154) {
    //    1. Download "read.fa", "read2.fa", "reference.fa" from https://ugene.net/tracker/browse/UGENE-1154 or use other sequences
    //    2. Use menu { Tools -> NGS data analysis -> Map reads to reference... }
    //    Expected state: "Align sequences reads" dialog has appeared
    //    3. Add "read.fa" and "read2.fa" to short reads list in the dialog
    //    4. Set "refrence.fa" as reference sequence
    //    5. Uncheck "Use best-mode"
    //    Expected state: reads are aligned and statistics is correct
    GTLogTracer lt;

    GTFile::copyDir(testDir + "_common_data/regression/1154", sandBoxDir + "1154");

    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(sandBoxDir + "1154/reference.fa", {sandBoxDir + "1154/read.fa", sandBoxDir + "1154/read2.fa"});
    parameters.useBestMode = false;
    parameters.samOutput = false;
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(&parameters));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("reference.ugenedb");

    bool hasMessage = lt.hasMessage("50% reads aligned.");
    CHECK_SET_ERR(hasMessage, "The expected message is not found in the log");
}

GUI_TEST_CLASS_DEFINITION(test_1156) {
    class DigestCircularSequenceScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto availableEnzymeWidget = GTWidget::findListWidget("availableEnzymeWidget", dialog);

            QList<QListWidgetItem*> items = availableEnzymeWidget->findItems("BamHI", Qt::MatchStartsWith);
            CHECK_SET_ERR(items.size() == 1, "Unexpected number of enzymes starting with 'BamHI'");

            QPoint enzymePosition = availableEnzymeWidget->mapToGlobal(availableEnzymeWidget->visualItemRect(items.first()).center());
            GTMouseDriver::moveTo(enzymePosition);
            GTMouseDriver::click();

            GTWidget::click(GTWidget::findWidget("addAllButton"));

            GTCheckBox::setChecked(GTWidget::findCheckBox("circularBox"));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 1. Open human_T1
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use menu{ Analyze->Find restriction sites }.
    // Expected state : "Find restriction sites" dialog has appeared
    // 3. Press "Ok" button in the dialog.
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList("BamHI")));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Use menu{ Cloning->Digest into fragments }.
    // Expected state : "Find restriction sites" dialog has appeared. It contains a checkbox "Circular molecule"
    // 5. Choose "Circular molecule" mode
    // 6. Press "Ok"
    GTUtilsDialog::add(new PopupChooserByText({"Cloning", "Digest into fragments..."}));
    GTUtilsDialog::add(new DigestSequenceDialogFiller(new DigestCircularSequenceScenario));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* fragmentGroupItem = GTUtilsAnnotationsTreeView::findItem("fragments  (0, 24)");
    CHECK_SET_ERR(fragmentGroupItem->childCount() == 24, "Unexpected sequence fragments count");
}

GUI_TEST_CLASS_DEFINITION(test_1157) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* readSequence = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* writeSequence = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);

    QString resultFilePath = testDir + "_common_data/scenarios/sandbox/test_1157.gb";
    GTUtilsWorkflowDesigner::setParameter("Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir().absoluteFilePath(resultFilePath), GTUtilsWorkflowDesigner::textValue);

    WorkflowProcessItem* callocationSearch = GTUtilsWorkflowDesigner::addElement("Collocation Search");
    GTUtilsWorkflowDesigner::setParameter("Result type", "Copy original annotations", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Group of annotations", "mat_peptide, CDS", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Must fit into region", "False", GTUtilsWorkflowDesigner::comboValue);

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/Genbank/sars.gb");

    GTUtilsWorkflowDesigner::connect(readSequence, callocationSearch);
    GTUtilsWorkflowDesigner::connect(callocationSearch, writeSequence);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(resultFilePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::findItem("CDS  (0, 17)");
    GTUtilsAnnotationsTreeView::findItem("mat_peptide  (0, 32)");
}

GUI_TEST_CLASS_DEFINITION(test_1163) {
    // 1. Open file *.ugenedb (for example _common_data\ugenedb\example-alignment.ugenedb) in assembly browser.
    // 2. right click it and choose "Unload selected documents".
    // 3. click "Yes" in appeared message box.
    // Expected state: UGENE not crashes

    GTFileDialog::openFile(testDir + "_common_data/ugenedb/", "example-alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"action_project__unload_selected_action"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsProjectTreeView::click("example-alignment.ugenedb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1165) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. open context menu in msa, "Add"->"Sequence from file...",
    // 3. browse the file "data/samples/FASTA/human_T1.fa".
    auto nameList = GTWidget::findWidget("msa_editor_name_list");

    GTUtilsDialog::add(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTUtilsDialog::add(new GTFileDialogUtils(dataDir + "/samples/FASTA/", "human_T1.fa"));
    GTMenu::showContextMenu(nameList);

    // 4. Then choose any sequence in sequence names area (except that which you've just added), press "Delete"
    // Expected state: UGENE not crashes
    GTWidget::click(nameList);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_1166) {
    // 1. Open alignment
    GTFileDialog::openFile(testDir + "_common_data/clustal", "alignx.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(24, 2), QPoint(29, 3));

    // 2. Select any region in msa with left button, move it left
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(26, 2));
    GTMouseDriver::press();

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(24, 2));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // 3. Drag the region you selected to its original place
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(24, 2));
    GTMouseDriver::press();

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(26, 2));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // 4. Press right mouse button, choose "Align"->"Align with T-Coffee"
    // Expected state: UGENE not crashes
    GTUtilsDialog::waitForDialog(new TCoffeeDailogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with T-Coffee"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1172) {
    // 1. Open file "\data\samples\Stockholm\CBS.sto" or "\data\samples\ACE\BL060C3.ace"
    // 2. Select any area in msa (or just one symbol)
    // 3. Press at selected area (or at selected symbol) again
    // Expected state: UGENE not crashes

    GTFileDialog::openFile(dataDir + "samples/Stockholm/", "CBS.sto");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 2));
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 2));
}

GUI_TEST_CLASS_DEFINITION(test_1175) {
    //     1. Open WD.
    //     2. Create any scheme with element, that correspond to an external tool.
    //     3. Delete path to the extarnal tool if it is selected
    //     4. Press "Validate schema" or "Run scheme"
    //     Expected state: error "Extarnal tool "Name of tool" is not set" appeared

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Gene-by-gene approach");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsExternalTools::removeTool("Blast");

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::click("Local BLAST Search");
    GTUtilsWorkflowDesigner::setParameter("Database Path", sandBoxDir, GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Database Name", "test", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Validate workflow"));

    GTUtilsWorkflowDesigner::checkErrorList("External tool \"BlastN\" is not set.");
}

GUI_TEST_CLASS_DEFINITION(test_1180) {
    // 1) Open Settings, External tools.
    // 2) Set wrong path to the Bowtie tool.
    QString toolPath = QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath();
    GTUtilsExternalTools::setToolUrl("Bowtie aligner", toolPath);
    GTUtilsExternalTools::setToolUrl("Bowtie build indexer", toolPath);

    // 3) Run Bowtie aligning on any data.
    // Expected state: UGENE not crashed.
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/fasta/",
                                                 "Mycobacterium.fna",
                                                 testDir + "_common_data/e_coli/",
                                                 "e_coli_1000.fastq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(&parameters));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_1182) {
    //    1.Create a new output text slot for Grouper element in WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    WorkflowProcessItem* worker = GTUtilsWorkflowDesigner::addElement("Grouper");
    //    2. The slot contains the size of created group which hold a number of element in each group
    WorkflowPortItem* port = GTUtilsWorkflowDesigner::getPortById(worker, "output-data");
    GTMouseDriver::moveTo(GTGraphicsItem::getItemCenter(port));
    GTMouseDriver::click();

    auto paramBox = GTWidget::findWidget("paramBox");
    QTableWidget* table = paramBox->findChild<QTableWidget*>();
    QList<QTableWidgetItem*> tableItems = table->findItems("Group size (by Grouper)", Qt::MatchExactly);
    CHECK_SET_ERR(tableItems.size() == 1, QString("unexpected items number: %1").arg(tableItems.size()));
}

GUI_TEST_CLASS_DEFINITION(test_1184) {
    //    1. Open WD
    //    2. Place worker on the scheme 'Write FASTA', set worker parameters to the next values:
    //        {Output file} file.fa
    //        {Existing file} Overwrite

    //    3. Copy & Paste worker
    //    Expected state: parameters of original worker are equal with parameters on copied worker

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Write FASTA");
    GTUtilsWorkflowDesigner::setParameter("Output file", "file.fa", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Existing file", "Overwrite", GTUtilsWorkflowDesigner::comboValue);

    GTUtilsWorkflowDesigner::click("Write FASTA");
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter("Output file", true) == "file.fa",
                  QString("Incorrect output file parameter [%1]").arg(GTUtilsWorkflowDesigner::getParameter("Output file")));
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter("Existing file", true) == "Overwrite",
                  QString("Incorrect existing file parameter [%1]").arg(GTUtilsWorkflowDesigner::getParameter("Existing file")));
}

GUI_TEST_CLASS_DEFINITION(test_1186_1) {
    //     1) Open "Align Short Read Dialog"
    //     2) Set "Result file name" 'test.sam'
    //     3) Select "Reference sequence" for example human_T1.fa
    //     Expected state: "Result file name" is 'test.sam'
    class Scenario_test_1186_1 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir, "test_1186_1.sam", GTFileDialogUtils::Save, GTGlobals::UseMouse));

            auto setResultFileNameButton = GTWidget::findWidget("setResultFileNameButton", dialog);

            GTWidget::click(setResultFileNameButton);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/", "human_T1.fa"));
            auto addRefButton = GTWidget::findWidget("addRefButton", dialog);
            GTWidget::click(addRefButton);

            auto resultFileNameEdit = GTWidget::findLineEdit("resultFileNameEdit", dialog);
            CHECK_SET_ERR(resultFileNameEdit->text().contains("test_1186_1.sam"), "Incorrect output file");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new Scenario_test_1186_1()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_1186_2) {
    //     1) Open Align Short Read Dialog
    //     2) Uncheck "SAM output" checkbox
    //     3) Set "Result file name" for example 'test.ugenedb'
    //     Expected state: "Result file name" is 'test.ugenedb'

    class Scenario_test_1186_2 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto samBox = GTWidget::findCheckBox("samBox", dialog);

            GTCheckBox::setChecked(samBox, false);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir, "test_1186_2.ugenedb", GTFileDialogUtils::Save, GTGlobals::UseMouse));

            auto setResultFileNameButton = GTWidget::findWidget("setResultFileNameButton", dialog);

            GTWidget::click(setResultFileNameButton);

            auto resultFileNameEdit = GTWidget::findLineEdit("resultFileNameEdit", dialog);
            CHECK_SET_ERR(resultFileNameEdit->text().contains("test_1186_2.ugenedb"), "Incorrect output file");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new Scenario_test_1186_2()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_1189) {
    // 1) Open samples/FASTA/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2) Select any region of the sequence
    GTUtilsSequenceView::selectSequenceRegion(100, 200);

    // 3) Activate search widget with Ctrl+F
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 4) Select "Selected region" in "Region" combobox of "Search in" area.
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    auto regBox = GTWidget::findComboBox("boxRegion");
    GTComboBox::selectItemByIndex(regBox, 2);

    // 5) Ensure that two lineedits became visible and contain correct region
    auto start = GTWidget::findLineEdit("editStart");
    CHECK_SET_ERR(start->isVisible(), "editStart line is not visiable");
    CHECK_SET_ERR(start->text() == "100", "Wrong startValue. Current value is: " + start->text());

    auto end = GTWidget::findLineEdit("editEnd");
    CHECK_SET_ERR(end->isVisible(), "editEnd line is not visiable");
    CHECK_SET_ERR(end->text() == "200", "Wrong endValue. Current value is: " + end->text());
}

GUI_TEST_CLASS_DEFINITION(test_1189_1) {
    // 1) Open samples/FASTA/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2) Select any region of the sequence
    QWidget* panView = GTUtilsSequenceView::getPanOrDetView();
    QPoint centerPos = panView->mapToGlobal(panView->rect().center());
    //    GTWidget::click(centerPos - QPoint(-100, 0));
    GTMouseDriver::dragAndDrop(centerPos + QPoint(-100, 0), centerPos + QPoint(100, 0));
    GTThread::waitForMainThread();

    // 3) Activate search widget with Ctrl+F
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 4) Select "Selected region" in "Region" combobox of "Search in" area.
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    auto regBox = GTWidget::findComboBox("boxRegion");
    GTComboBox::selectItemByIndex(regBox, 2);

    // 5) Ensure that two lineedits became visible and contain correct region
    auto start = GTWidget::findLineEdit("editStart");
    CHECK_SET_ERR(start->isVisible(), "editStart line is not visiable");
    CHECK_SET_ERR(start->text() != "1", "Wrong startValue: 1.");  // 1 is default

    auto end = GTWidget::findLineEdit("editEnd");
    CHECK_SET_ERR(end->isVisible(), "editEnd line is not visiable");
    CHECK_SET_ERR(start->text() != "199950", "Wrong endValue: 199950.");  // 199950 is the length of human_T1 and is default value.
}

GUI_TEST_CLASS_DEFINITION(test_1190) {  // add AlignShortReadsFiller

    // 1) Align shortreads with genome aligner
    // Tools -> Align to reference -> Align short reads
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/fasta/",
                                                 "N.fa",
                                                 testDir + "_common_data/fasta/",
                                                 "RAW.fa");
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(&parameters));
    GTLogTracer lt;

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());

    //}
    // Reference sequence: _common_data/fasta/N.fa  /home/vmalin/ugene/trunk/test/_common_data/fasta/N.ugenedb
    // Short reads: _common_data/reads/shortreads15Mb.fasta

    // Click "Start"

    // 2) wait for dialog to appear, click "OK"

    // repeat these steps 3 times, UGENE shouldn't crash
}

GUI_TEST_CLASS_DEFINITION(test_1199) {
    // 1. Open any samples/PDB/1CF7.pdb file.
    GTFileDialog::openFile(dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select document in project tree view. Press 'Delete'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7.PDB"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: document removed, UGENE not crashes.
    GTUtilsProject::checkProject(GTUtilsProject::Empty);
}

GUI_TEST_CLASS_DEFINITION(test_1203_1) {
    //    Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::setCurrentTab(GTUtilsWorkflowDesigner::algorithms);

    //    Print "mer" in line "Name filter"
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("Remote BLAST");

    //    Place "Remote BLAST" element on the scheme
    GTUtilsWorkflowDesigner::addAlgorithm("Remote BLAST", true);
    GTUtilsWorkflowDesigner::click("Remote BLAST");

    //    Select "Database" to "ncbi-blastn"
    //    Expected state: "Entrez query" lineedit is enabled
    GTUtilsWorkflowDesigner::setParameter("Program", "ncbi-blastn", GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterEnabled("Entrez query"), "Parameter is unexpectedly disabled");

    //    Select "Database" to "ncbi-blastp"
    //    Expected state: "Entrez query" lineedit is enabled
    GTUtilsWorkflowDesigner::setParameter("Program", "ncbi-blastp", GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterEnabled("Entrez query"), "Parameter is unexpectedly disabled");

    //    Select "Database" to "ncbi-cdd"
    //    Expected state: "Entrez query" lineedit is not visible
    GTUtilsWorkflowDesigner::setParameter("Program", "ncbi-cdd", GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isParameterVisible("Entrez query"), "Parameter is unexpectedly visible");

    //    Select "Database" to "ncbi-blastn"
    //    Expected state: "Entrez query" lineedit is enabled
    GTUtilsWorkflowDesigner::clickParameter("BLAST output");
    GTUtilsWorkflowDesigner::setParameter("Program", "ncbi-blastn", GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isParameterEnabled("Entrez query"), "Parameter is unexpectedly disabled");
}

GUI_TEST_CLASS_DEFINITION(test_1203_2) {
    //    1) Open data\samples\FASTA\human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2) Use context menu {Analyze -> Search NCBI BLAST database}

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    3) Set "Search the search type" to "blastn"
            //    Expected state: "Entrez query" presents on "Advanced options" tab
            GTComboBox::selectItemByText(GTWidget::findComboBox("dataBase", dialog), "blastn");
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("optionsTab", dialog), 1);
            auto entrezQueryEdit = GTWidget::findLineEdit("entrezQueryEdit", dialog);
            CHECK_SET_ERR(entrezQueryEdit->isVisible(), "entrezQueryEdit is unexpectedly not visible");

            //    4) Set "Search the search type" to "blastp"
            //    Expected state: "Entrez query" presents on "Advanced options" tab
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("optionsTab", dialog), 0);
            GTComboBox::selectItemByText(GTWidget::findComboBox("dataBase", dialog), "blastp");
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("optionsTab", dialog), 1);
            entrezQueryEdit = GTWidget::findLineEdit("entrezQueryEdit", dialog);
            CHECK_SET_ERR(entrezQueryEdit->isVisible(), "entrezQueryEdit is unexpectedly not visible");

            //    5) Set "Search the search type" to "cdd"
            //    Expected state: "Advanced options" tab is disabled
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("optionsTab", dialog), 0);
            GTComboBox::selectItemByText(GTWidget::findComboBox("dataBase", dialog), "cdd");
            auto tabWidget = GTWidget::findTabWidget("optionsTab", dialog);
            CHECK_SET_ERR(!tabWidget->isTabEnabled(1), "'Advanced options' tab is unexpectedly enabled");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Analyze", "Query NCBI BLAST database..."}));
    GTUtilsDialog::add(new RemoteBLASTDialogFiller(new Scenario));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_1204) {
    // 1) Open files data\samples\FASTA\human_T1.fa
    // 2) Use context menu {Analyze -> Query NCBI BLAST database}
    // Expected state: "max hits" spinbox can be set to 5000

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
    public:
        void run() {
            auto maxResultsSpinBox = GTWidget::findSpinBox("quantitySpinBox");
            GTSpinBox::setValue(maxResultsSpinBox, 5000, GTGlobals::UseKeyBoard);
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };
    GTUtilsDialog::add(new PopupChooserByText({"Analyze", "Query NCBI BLAST database..."}));
    GTUtilsDialog::add(new RemoteBLASTDialogFiller(new Scenario));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_1209) {
    //    1. Open workflow sample "Call variants with SAMtools"
    //    2. Substitute "Read assembly (BAM/SAM)" element with "Read Sequence" element
    //    3. Set any input sequences for "Read sequence" elements.
    //    4. Select "Call Variants" element with mouse
    //    5. Set "Source URL (by Read Sequence 1)" as "Source URL" in "Input data" area in workflow editor
    //    6. Run the scheme
    //    Expected state: Error notification appears, UGENE doesn't crash
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Call Variants");

    QTableWidget* table1 = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    GTUtilsWorkflowDesigner::setTableValue("Source URL", "<empty>", GTUtilsWorkflowDesigner::comboValue, table1);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Validate workflow"));
    GTUtilsWorkflowDesigner::checkErrorList("Empty input slot");
}

GUI_TEST_CLASS_DEFINITION(test_1210) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* readSequence = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* readAlignment = GTUtilsWorkflowDesigner::addElement("Read Alignment");
    WorkflowProcessItem* callVariants = GTUtilsWorkflowDesigner::addElement("Call Variants with SAMtools");

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::addInputFile("Read Alignment", dataDir + "samples/Assembly/chrM.sam");

    GTUtilsWorkflowDesigner::connect(readSequence, callVariants);
    GTUtilsWorkflowDesigner::connect(readAlignment, callVariants);
}

GUI_TEST_CLASS_DEFINITION(test_1212) {
    //    1. Open any sequence. (human_t1.fa)
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do "Select->Sequence Region..."
    //    3. In single selection mode enter any region
    //    4. Press "Go" and UGENE hangs up/crashes
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("100..200"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    //    Expected: region is selected
    ADVSingleSequenceWidget* w = (ADVSingleSequenceWidget*)GTWidget::findWidget("ADV_single_sequence_widget_0");
    CHECK_SET_ERR(!w->getSequenceSelection()->isEmpty(), "No selected region");
}

GUI_TEST_CLASS_DEFINITION(test_1212_1) {
    //    1. Open any sequence. (human_t1.fa)
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Do "Select->Sequence Region..."
    //    3. In single selection mode enter any region
    //    4. Press "Go" and UGENE hangs up/crashes
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("1..199950"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    //    Expected: region is selected
    ADVSingleSequenceWidget* w = (ADVSingleSequenceWidget*)GTWidget::findWidget("ADV_single_sequence_widget_0");
    CHECK_SET_ERR(!w->getSequenceSelection()->isEmpty(), "No selected region");
}

GUI_TEST_CLASS_DEFINITION(test_1219) {
    //    1. Open any sequence. (human_t1.fa)
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open Smith-Waterman search dialog
    //    Excepted state: default value of combobox "Save result as" is "Annotations"
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTTextEdit::setText(GTWidget::findTextEdit("teditPattern", dialog), "CTAAGGG");

            //    3. Open tab "Input and output"
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);

            //    4. Chose in the combobox "Multiple alignment"
            GTComboBox::selectItemByText(GTWidget::findComboBox("resultViewVariants", dialog), "Multiple alignment");

            //    5. Check that "pattern sequence name" is "PN", where "N" is number of SW search launch.
            GTLineEdit::checkText(GTWidget::findLineEdit("patternSequenceName", dialog), "P1");

            //    6. Click "Align" button
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find pattern [Smith-Waterman]");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Excepted state: new *.aln documents (with specified names) should be created and contain founded subsequences for
    //    one pair in each [m]object. Few documents must be loaded to current project, others will stay unloaded
    //    7. Check that numeration of founded sequences starting from 1
    //    8. Check that there are some separators between template values to default strings in files names and sequences names
    GTUtilsDocument::checkDocument("P1_human_T1_1.aln");
    GTUtilsDocument::checkDocument("P1_human_T1_2.aln");
    GTUtilsDocument::checkDocument("P1_human_T1_3.aln");
    GTUtilsDocument::checkDocument("P1_human_T1_4.aln");
    GTUtilsDocument::checkDocument("P1_human_T1_5.aln");
    GTUtilsDocument::checkDocument("P1_human_T1_6.aln", GTUtilsDocument::DocumentUnloaded);
    GTUtilsDocument::checkDocument("P1_human_T1_7.aln", GTUtilsDocument::DocumentUnloaded);

    const QSet<GObjectType> acceptableTypes = QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    GTUtilsProjectTreeView::checkObjectTypes(acceptableTypes, GTUtilsProjectTreeView::findIndex("P1_human_T1_1.aln"));
    GTUtilsProjectTreeView::checkObjectTypes(acceptableTypes, GTUtilsProjectTreeView::findIndex("P1_human_T1_2.aln"));
    GTUtilsProjectTreeView::checkObjectTypes(acceptableTypes, GTUtilsProjectTreeView::findIndex("P1_human_T1_3.aln"));
    GTUtilsProjectTreeView::checkObjectTypes(acceptableTypes, GTUtilsProjectTreeView::findIndex("P1_human_T1_4.aln"));
    GTUtilsProjectTreeView::checkObjectTypes(acceptableTypes, GTUtilsProjectTreeView::findIndex("P1_human_T1_5.aln"));

    GTUtilsProjectTreeView::doubleClickItem("P1_human_T1_1");

    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    const QStringList expectedNames = {"human_T1_39_45", "P1_1_7"};
    CHECK_SET_ERR(expectedNames == names, QString("There are unexpected sequence names in the msa: expect '%1', got '%2'").arg(expectedNames.join(", ")).arg(names.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_1220) {
    //    1) Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Run Smith-waterman search using:
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    pattern: "ATCGAT"; note that pattern length is 6.
            GTTextEdit::setText(GTWidget::findTextEdit("teditPattern", dialog), "ATCGAT");
            //    min: 100%
            auto spinScorePercent = GTWidget::findSpinBox("spinScorePercent", dialog);
            GTSpinBox::setValue(spinScorePercent, 100);
            //    {input-output tab} "Add qualifier...": checked
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);
            auto addPatternContentQualifierCheck = GTWidget::findCheckBox("addPatternContentQualifierCheck", dialog);
            GTCheckBox::setChecked(addPatternContentQualifierCheck, true);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: misc_feature annotations created with pattern subsequence length qualifiers set to 6
    GTUtilsAnnotationsTreeView::selectItemsByName({"misc_feature"});
    QString val = GTUtilsAnnotationsTreeView::getQualifierValue("pattern_match_len", "misc_feature");
    CHECK_SET_ERR(val == "6", "unexpected value: " + val);
    //    Current state: "pattern_subseq_length" qualifiers created and set to 5.
}

GUI_TEST_CLASS_DEFINITION(test_1229) {
    // 1. Open two sequences with same names in two documents.For instance, you can copy a file with a sequence to do that.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1229", "1.txt");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1229", "2.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. The sequences in different documents have identical names. Rename both sequence objects. Now the sequences have different name.
    const QModelIndex firstDoc = GTUtilsProjectTreeView::findIndex("1.txt");
    const QModelIndex secondDoc = GTUtilsProjectTreeView::findIndex("2.txt");

    GTUtilsProjectTreeView::rename(GTUtilsProjectTreeView::findIndex("tub", firstDoc), "tub_1", GTGlobals::UseMouse);
    GTUtilsProjectTreeView::rename(GTUtilsProjectTreeView::findIndex("tub", secondDoc), "tub_2", GTGlobals::UseMouse);

    // 3. Select both sequence objects and export them as multiple alignment.
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("tub_1"));
    GTMouseDriver::click();

    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(new ExportSequenceAsAlignmentFiller(sandBoxDir, "test_1229.aln", ExportSequenceAsAlignmentFiller::Clustalw, true));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Sequences in the alignment have old names(identical).
    // Expected state : sequence in the alignment renamed properly.
    const QStringList msaNames = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(2 == msaNames.size(), "Unexpected sequence count in MSA");
    CHECK_SET_ERR(msaNames.contains("tub_1") && msaNames.contains("tub_2"), "Unexpected sequences names in MSA");
}

GUI_TEST_CLASS_DEFINITION(test_1234) {
    /* 1. Select a sequence region.
     * 2. Do {Export->Export selected sequence region...}
     * 3. Check "Translate to amino alphabet" and "Save all amino frames"
     *   Expected state: no bad characters at the end of the frames. Sequences are translated correctly.
     */
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsSequenceView::selectSequenceRegion(100, 120);
    // DLSAETL
    // ISRQKP
    // SLGRNP

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(sandBoxDir, "test_1234.fa", true));

    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    auto parent = GTWidget::findWidget("test_1234.fa");

    auto seq0 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_0", parent));
    auto seq1 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_1", parent));
    auto seq2 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_2", parent));

    CHECK_SET_ERR(nullptr != seq0, "Failed to find a sequence widget for seq0!");
    CHECK_SET_ERR(nullptr != seq1, "Failed to find a sequence widget for seq1!");
    CHECK_SET_ERR(nullptr != seq2, "Failed to find a sequence widget for seq2!");

    U2OpStatus2Log u2os;

    CHECK_SET_ERR("DLSAETL" == QString(seq0->getSequenceObject()->getWholeSequenceData(u2os)), QString("Unexpected sequence. Expected %1, Actual %2").arg("DLSAETL").arg(QString(seq0->getSequenceObject()->getWholeSequenceData(u2os))));
    CHECK_SET_ERR("ISRQKP" == QString(seq1->getSequenceObject()->getWholeSequenceData(u2os)), QString("Unexpected sequence. Expected %1, Actual %2").arg("ISRQKP").arg(QString(seq1->getSequenceObject()->getWholeSequenceData(u2os))));
    CHECK_SET_ERR("SLGRNP" == QString(seq2->getSequenceObject()->getWholeSequenceData(u2os)), QString("Unexpected sequence. Expected %1, Actual %2").arg("SLGRNP").arg(QString(seq2->getSequenceObject()->getWholeSequenceData(u2os))));
}

GUI_TEST_CLASS_DEFINITION(test_1241) {
    GTLogTracer lt;

    // 1. Open file _common_data/scenarios/regression/1241/tt.fa.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1241/tt.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select ma959 sequence.
    GTUtilsMsaEditor::clickSequenceName("ma959");

    // 3. Press "Enable collapsing".
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected: UGENE doesn't crash, no errors in log.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1245) {
    //    1. Open the file human_T1.fa.
    //    2. Click on the document context menu "Export Document" in the project view. The Export Document dialog appears.
    //    3. Be sure what 'FASTA' format is choosen
    //    4. Press on the browse button. Select file location dialog appears. Set the saving file: "test" and click "Save".
    //    Expected state: In the "Save to file" field in the Export Document dialog path of the document "test" appears with "fa" extension

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class ExportDocumentCustomFiller : public Filler {
    public:
        ExportDocumentCustomFiller()
            : Filler("ExportDocumentDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto comboBox = GTWidget::findComboBox("formatCombo", dialog);
            int index = comboBox->findText("FASTA");

            CHECK_SET_ERR(index != -1, QString("item \"FASTA\" in combobox not found"));
            GTComboBox::selectItemByIndex(comboBox, index);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir, "test_1245", GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("browseButton"));

            auto lineEdit = GTWidget::findLineEdit("fileNameEdit");
            CHECK_SET_ERR(GTLineEdit::copyText(lineEdit).endsWith(".fa"), "Wrong extension");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ExportDocumentCustomFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"Export document"}));
    GTUtilsProjectTreeView::click("human_T1.fa", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1246) {
    //    1. Open the file data/samples/Assembly/chrM.sorted.bam
    //    2. Select file in the project view and click on the document context menu "Export Document" in the project view.
    //    Expected state: the Export Document dialog appears.
    //    3. Press on the browse button.
    //    Expected state: Select file location dialog appears.
    //    4. Fill this dialog with:
    //        {Save to file:} anything
    //        {File format:} SAM.
    //    5. Click the Export button.
    //    Expected state: UGENE does not crash.

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "/test_1246.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly/", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    class ExportDocumentCustomFiller : public Filler {
    public:
        ExportDocumentCustomFiller()
            : Filler("ExportDocumentDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir, "test_1246", GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("browseButton"));

            auto comboBox = GTWidget::findComboBox("formatCombo", dialog);
            int index = comboBox->findText("SAM");

            CHECK_SET_ERR(index != -1, QString("item \"SAM\" in combobox not found"));
            GTComboBox::selectItemByIndex(comboBox, index);
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "/test_1246_2"));

    GTUtilsDialog::waitForDialog(new ExportDocumentCustomFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"Export document"}));
    GTUtilsProjectTreeView::click("test_1246.ugenedb", Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1249) {
    // 1. Open human_T1.fa.
    // 2. Use menu {Analyze->Find restriction sites}.
    // 3. Press "Enzymes file.."
    // 4. Select file "_common_data\archive\2013_08_01.bairoch.gz".
    // Expected state: total number of enzymes is 4862(Enzymes with unknown sequence field are removed from list)

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class Scenario_test_1249 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/archive/2013_08_01.bairoch.gz"));
            GTWidget::click(GTWidget::findWidget("enzymesFileButton", dialog));
            auto totalNumberOfEnzymesLabel = GTWidget::findLabel("statusLabel");
            QString labelText = totalNumberOfEnzymesLabel->text();
            QString s = QString("4862");
            CHECK_SET_ERR(labelText.contains(s), QString("label text: %1. It does not contais %2").arg(labelText).arg(s));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList(), new Scenario_test_1249()));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_1252) {
    //    1. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Find any pattern. A new annotation document is created
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTTTTAAAAA");

    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
    //    3. Delete found annotations from human_t1 annotations tree
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    4. Delete created annotations document
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    QList<QString> keys = GTUtilsProjectTreeView::getDocuments().keys();
    QString name;
    foreach (const QString& key, keys) {
        if (key.startsWith("MyDocument")) {
            name = key;
            break;
        }
    }
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(name));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    // check delition of annotation document
    GTUtilsProjectTreeView::findIndex("Annotations", {false});
    //    5. Click search again

    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    item = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
    // delete annotations manually to cache MessageBox
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(name));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    //    Expected: pattern is found and annotation is stored in a new document
}

GUI_TEST_CLASS_DEFINITION(test_1252_1) {
    // DIFFERENCE: DEL KEY IS USED TO DELETE ANNOTATION DOCUMENT
    //     1. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Find any pattern. A new annotation document is created
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTTTTAAAAA");
    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
    //    3. Delete found annotations from human_t1 annotations tree
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    4. Delete created annotations document
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    QList<QString> keys = GTUtilsProjectTreeView::getDocuments().keys();
    QString name;
    foreach (const QString& key, keys) {
        if (key.startsWith("MyDocument")) {
            name = key;
            break;
        }
    }
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(name));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // check delition of annotation document
    GTUtilsProjectTreeView::findIndex("Annotations", {false});  // checks inside
    //    5. Click search again
    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    item = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
    // delete annotations manually to cache MessageBox
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(name));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    //    Expected: pattern is found and annotation is stored in a new document
}

GUI_TEST_CLASS_DEFINITION(test_1252_real) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // Add Read Sequence(RS).
    WorkflowProcessItem* reader = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);

    // Add ORF Finder(OF).
    WorkflowProcessItem* orfMarker = GTUtilsWorkflowDesigner::addElement("ORF Marker", true);

    // Add Write Sequence(WS).
    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);

    // Connect RS with OF.
    GTUtilsWorkflowDesigner::connect(reader, orfMarker);

    // Connect OF with WS.
    GTUtilsWorkflowDesigner::connect(orfMarker, writer);

    // Excepted state: Input "Annotations" slot of WS is not empty and contains annotations from ORF Finder
    GTUtilsWorkflowDesigner::click("Write Sequence");

    QList<QPair<QString, bool>> items = GTUtilsWorkflowDesigner::getCheckableComboboxValuesFromInputPortTable(0, "Set of annotations");
    bool found = false;
    QString expectedText = "Set of annotations (by ORF Marker)";
    for (const auto& item : qAsConst(items)) {
        if (item.first == expectedText) {
            found = true;
            CHECK_SET_ERR(item.second, QString("'%1' is not checked").arg(expectedText));
        }
    }
    CHECK_SET_ERR(found, QString("'%1' is not found among the values").arg(expectedText));
}

GUI_TEST_CLASS_DEFINITION(test_1253) {
    //    1. Open Find Pattern on the Options Panel
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    //    2. Input any valid pattern
    GTUtilsOptionPanelSequenceView::enterPattern("AAAAAAA", true);
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Annotation parameters"));
    //    3. Rename annotation and annotation group
    auto leGroupName = GTWidget::findLineEdit("leGroupName");
    GTLineEdit::setText(leGroupName, "groupName");
    auto leAnnotationName = GTWidget::findLineEdit("leAnnotationName");
    GTLineEdit::setText(leAnnotationName, "annName");
    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    //    4. Run search
    //    Expected state: check annotations and group names in results
    GTUtilsAnnotationsTreeView::findItem("groupName  (0, 787)");
    GTUtilsAnnotationsTreeView::findItem("annName");
}

GUI_TEST_CLASS_DEFINITION(test_1257) {
    // 1. Open Find Pattern on the Options Panel
    // 2. Provide the widget with wrong input (no annotation name, bad region)
    // Expected: Search button is disabled

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    GTKeyboardDriver::keySequence(">S");

    auto prevButton = GTWidget::findWidget("prevPushButton");
    CHECK_SET_ERR(!prevButton->isEnabled(), "prevPushButton is unexpectidly enabled")
}

GUI_TEST_CLASS_DEFINITION(test_1259) {
    //    1. Open FindPattern on the Options Panel
    //    2. (Using ctrl+enter once) enter the following pattern:
    //    >S
    //    H
    //    3. Remove H (with backspace)
    //    Expected state: UGENE doesn't crash and the symbol is removed

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    GTKeyboardDriver::keySequence(">S");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);
    GTKeyboardDriver::keySequence("H");
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    auto textEdit = GTWidget::findPlainTextEdit("textPattern");
    QString text = textEdit->toPlainText();
    CHECK_SET_ERR(text == ">S\n", "Wrong pattern: " + text);
}

GUI_TEST_CLASS_DEFINITION(test_1260) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1260/", "51.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(testDir + "_common_data/scenarios/sandbox/1260.sto", {"Isophya_altaica_EF540820", "Phaneroptera_falcata"}, 1, 51, true, false, false, false, true));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1262) {
    // 1. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Find any pattern. A new annotation document is created
    GTUtilsOptionsPanel::runFindPatternWithHotKey("AGGAAAAAATGCTAAGGGCAGCCAGAGAGAGGTCAGG");

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));

    // 3. Delete created annotations document
    QList<QString> keys = GTUtilsProjectTreeView::getDocuments().keys();
    QString name;
    foreach (const QString& key, keys) {
        if (key.startsWith("MyDocument")) {
            name = key;
            break;
        }
    }
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(name));
    GTMouseDriver::click(Qt::RightButton);
    // 4. Click search again

    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    GTUtilsTaskTreeView::waitTaskFinished();
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));

    // delete new doc
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__REMOVE_SELECTED}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(name));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1263) {
    // Open "human_T1"
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Use tool button "Primers3"
    GTUtilsDialog::waitForDialog(new Primer3DialogFiller());
    GTWidget::click(GTWidget::findWidget("primer3_action_widget"));

    // Press button "Pick Primers"
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select pair of primers
    QTreeWidgetItem* parent = GTUtilsAnnotationsTreeView::findItem("pair 1  (0, 2)");
    QTreeWidgetItem* first = parent->child(0);
    QTreeWidgetItem* second = parent->child(1);

    // Use context menu {Cloning->Create PCR product}
    GTUtilsAnnotationsTreeView::selectItems({first, second});
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cloning", "Create PCR product"}));
    GTMouseDriver::click(Qt::RightButton);

    // Press 'Ok'
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Excepted state: PCR product has been created
    GTUtilsAnnotationsTreeView::findItem("Fragment (22172-22388)");
}

GUI_TEST_CLASS_DEFINITION(test_1266) {
    //    1. Open "Call variants" sample pipleine from the "NGS" category
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            auto w = GTWidget::findWidget("Disable BAQ computation label", dialog);
            auto parent = qobject_cast<QWidget*>(w->parent());
            QString s = parent->toolTip();
            CHECK_SET_ERR(s.contains("<html>Disable"), "unexpected tooltip: " + s);

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    //    2. Run its wizzard
    GTUtilsDialog::waitForDialog(new WizardFiller("Call Variants Wizard", new custom()));
    GTWidget::click(GTAction::button("Show wizard"));
    //    Expected state: all parameters of the wizzard have tooltips with their descriptions
}

GUI_TEST_CLASS_DEFINITION(test_1274) {
    //    1. Select "Tabbed documents" mode in the Application Settings.
    GTMenu::clickMainMenuItem({"Window", "Window layout", "Tabbed documents"});

    //    2. Open a document
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    3. Click the cross button of the window to close it
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    //    Expected state: UGENE not crashes
}

GUI_TEST_CLASS_DEFINITION(test_1273) {
    // 1) Open "_common_data/genbank/JQ040024.1.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/JQ040024.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) Switch the windows layout to the tabbed documents mode
    //(Settings -> Preferences -> General -> Windows Layout -> Tabbed documents -> OK)
    GTMenu::clickMainMenuItem({"Window", "Window layout", "Tabbed documents"});

    // Expected: the name of the sequence view tab starts with "JQ040024.1", but not with "JQ040024".
    QTabBar* tabs = AppContext::getMainWindow()->getQMainWindow()->findChild<QTabBar*>("");
    CHECK_SET_ERR(nullptr != tabs, "No tab bar");
    CHECK_SET_ERR(tabs->tabText(1).startsWith("JQ040025"), "Wrong tab name");
}

GUI_TEST_CLASS_DEFINITION(test_1285) {
    // 1. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 3. Click {show more options}
    // 4. Check comboboxes: use pattern name, load pattern from file
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(testDir + "_common_data/scenarios/_regression/1285/", "small.fa");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/1"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_1288) {
    //    1) Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Open Tab "Elements".

    //    3) Create element "Basic Analysis->Find Pattern".
    WorkflowProcessItem* fp = GTUtilsWorkflowDesigner::addElement("Find Pattern");
    //    4) Create element "Data Readers->Read Sequence".
    WorkflowProcessItem* rs = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    //    5) Connect "Read Sequence" to "Find Pattern".
    GTUtilsWorkflowDesigner::connect(rs, fp);
    //    Wrong state: "Plain text" slot in "Find Pattern" has value "Dataset".
    GTUtilsWorkflowDesigner::click("Find Pattern");
    //    Expected: "Plain text" slot in "Find Pattern" has value "<empty>".
    QTableWidget* tw = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    CHECK_SET_ERR(tw != nullptr, "InputPortsTable is NULL");
    QString s = GTUtilsWorkflowDesigner::getCellValue("Plain text", tw);

    CHECK_SET_ERR(s == "<empty>", "unexpected value: " + s);
}

GUI_TEST_CLASS_DEFINITION(test_1289) {
    //    1) Open samples/Genbank/murine.gb.
    //    2) Open Workflow Designer.
    //    3) Menu: Settings->Preferences.
    //    4) "General" tab: switch on "Tabbed documents" window layout.
    //    5) Press Ok.

    //    Expected state:
    //    UGENE not crashed
    //    Tabs have red cross icons.

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTMenu::clickMainMenuItem({"Window", "Window layout", "Tabbed documents"});
}

GUI_TEST_CLASS_DEFINITION(test_1295) {
    class CustomBuildTreeDialogFiller : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto algorithmBox = GTWidget::findComboBox("algorithmBox", dialog);
            GTComboBox::selectItemByText(algorithmBox, "MrBayes");

            auto saveLineEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(saveLineEdit, sandBoxDir + "1295.nwk");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    // 1. Open file "data/samples/MSF/HMA.msf".
    GTFileDialog::openFile(dataDir + "samples/MSF/", "HMA.msf");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Call context menu on MSA area.
    // 3. Choose { "Tree" -> "Build Tree" }.
    // Expected state: "Build Phylogenetic Tree" dialog appears.
    // 4. Set "Tree building method" to "MrBayes".
    // 5. Press "Build" button.
    // Expected state: Tree is built (can take a while), notification error message doesn't appear.

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new CustomBuildTreeDialogFiller()));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected: the tree appears synchronized with the MSA Editor.

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1299) {
    /* 1) Open WD, load "Find substrings in sequences" sample
     * 2) Click on "Find substrings", "Text" slot
     *  Expected state: Popup menu should appear, contains bold "Additional" item menu
     */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addSample("Find substrings in sequences");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::click("Find Substrings");

    QTableWidget* tw = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    CHECK_SET_ERR(tw != nullptr, "InputPortsTable is NULL");

    GTUtilsWorkflowDesigner::setTableValue("Plain text", "Source URL (by Read Sequence)", GTUtilsWorkflowDesigner::comboValue, tw);

    int row = -1;
    for (int i = 0; i < tw->rowCount(); i++) {
        QString s = tw->item(i, 0)->text();
        if (s == "Plain text") {
            row = i;
            break;
        }
    }
    QRect rect = tw->visualItemRect(tw->item(row, 1));
    QPoint globalP = tw->viewport()->mapToGlobal(rect.center());
    GTMouseDriver::moveTo(globalP);
    GTMouseDriver::click();

    auto box = qobject_cast<QComboBox*>(tw->findChild<QComboBox*>());

    CHECK_SET_ERR(box->itemText(3) != "Additional", "Additional item not found");

    GTWidget::click(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_1300_1) {
    //    Show the opened view.

    //    1. Open "murine.gb".
    //    Expected state: the sequence view opens.
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    bool sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    //    2. Open "COI.aln".
    //    Expected state: the MSA Editor opens, sequence view is opened, but is not shown.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    bool msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(msaEditorIsVisible, "Msa editor is unexpectedly not visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    //    3. Double click to the sequence of "murine.gb" in the project view ([s] NC_001363).
    //    Expected state: the sequence view is shown, the MSA Editor is no  visible.
    GTUtilsProjectTreeView::doubleClickItem("NC_001363");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    //    4. Double click to the msa object in the project view ([m] COI).
    //    Expected state: the MSA Editor is shown.
    GTUtilsProjectTreeView::doubleClickItem("COI");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(msaEditorIsVisible, "Msa editor is unexpectedly not visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    //    5. Double click to the annotation object of "murine.gb" in the project view ([a] NC_001363 features).
    //    Expected state: the sequence View is shown.
    GTUtilsProjectTreeView::doubleClickItem("NC_001363 features");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    //    6. Double click to the COI document name in the project view (COI.aln).
    //    Expected state: the MSA Editor is shown.
    GTUtilsProjectTreeView::doubleClickItem("COI.aln");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(msaEditorIsVisible, "Msa editor is unexpectedly not visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    //    7. Double click to the murine document name in the project view (murine.gb).
    //    Expected state: the sequence view is shown.
    GTUtilsProjectTreeView::doubleClickItem("murine.gb");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");
}

GUI_TEST_CLASS_DEFINITION(test_1300_2) {
    //    Opening new views, opening the first view of several views.

    //    1. Open "murine.gb".
    //    Expected state: the sequence view opens.
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    bool sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    //    2. Close sequence view with murine.gb.
    //    Expected state: there is the "murine.gb" document in the project view, no views are opened.
    GTUtilsMdi::click(GTGlobals::Close);

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    //    3. Open "COI.aln".
    //    Expected state: the MSA Editor opens.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    bool msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(msaEditorIsVisible, "Msa editor is unexpectedly not visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    //    4. Right click to the COI document in the project view, select {Open view->Open new view: alignment editor} from the context menu.
    //    Expected state: there are two MSA Editors (the second one is active) and no sequence views.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Open In", "Open new view: Multiple Alignment Editor"}));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    bool msaEditor2IsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln] 2");
    CHECK_SET_ERR(msaEditor2IsVisible, "Msa editor is unexpectedly not visible");

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    //    5. Double click to the murine.gb (document, sequence object, annotation object - different tests?) in the project view.
    //    Expected state: the sequecne view opens. There are two MSA Editors and one sequence view (active).
    //    5.1 document
    GTUtilsProjectTreeView::doubleClickItem("murine.gb");
    GTThread::waitForMainThread();

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    msaEditor2IsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln] 2");
    CHECK_SET_ERR(!msaEditor2IsVisible, "Msa editor is unexpectedly visible");

    //    5.2 sequence
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsProjectTreeView::doubleClickItem("NC_001363");
    GTThread::waitForMainThread();

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    msaEditor2IsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI 2 [COI.aln]");
    CHECK_SET_ERR(!msaEditor2IsVisible, "Msa editor is unexpectedly visible");

    //    5.3 sequence
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsProjectTreeView::doubleClickItem("NC_001363 features");
    GTThread::waitForMainThread();

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(sequenceViewIsVisible, "Sequence view is unexpectedly not visible");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(!msaEditorIsVisible, "Msa editor is unexpectedly visible");

    msaEditor2IsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI 2 [COI.aln]");
    CHECK_SET_ERR(!msaEditor2IsVisible, "Msa editor is unexpectedly visible");

    //    6. Double click to the COI in the project view.
    //    Expected state: a popup menu is shown to select a MSA Editor to show. Select the first. There are two MSA Editors (the first one is active) and one sequence view.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Activate view: COI [COI.aln]"}));
    GTUtilsProjectTreeView::doubleClickItem("COI");
    GTThread::waitForMainThread();

    sequenceViewIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("NC_001363 [murine.gb]");
    CHECK_SET_ERR(!sequenceViewIsVisible, "Sequence view is unexpectedly visible");

    msaEditorIsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI [COI.aln]");
    CHECK_SET_ERR(msaEditorIsVisible, "Msa editor is unexpectedly not visible");

    msaEditor2IsVisible = GTUtilsMdi::isAnyPartOfWindowVisible("COI 2 [COI.aln]");
    CHECK_SET_ERR(!msaEditor2IsVisible, "Msa editor is unexpectedly visible");
}

GUI_TEST_CLASS_DEFINITION(test_1310) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto algorithmBox = GTWidget::findComboBox("algorithmBox", dialog);
            GTComboBox::selectItemByText(algorithmBox, "PHYLIP Neighbor Joining");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new Scenario()));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1315_1) {
    // 1. open murine.gb
    // 2. open Primer3 dialog
    // 3. Deselect "Pick left primer"
    // Expected state: 5 primers are found
    // Bug state: no primers are found
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.pickLeft = false;

    GTUtilsDialog::waitForDialog(new Primer3DialogFiller(settings));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Primer3..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::findItem("top_primers  (0, 5)");
}

GUI_TEST_CLASS_DEFINITION(test_1315_2) {
    // 1. open murine.gb
    // 2. open Primer3 dialog
    // 3. Deselect "Pick right primer"
    // Expected state: 5 primers are found
    // Bug state: no primers are found
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    Primer3DialogFiller::Primer3Settings settings;
    settings.pickRight = false;

    GTUtilsDialog::waitForDialog(new Primer3DialogFiller(settings));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Primer3..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::findItem("top_primers  (0, 5)");
}

GUI_TEST_CLASS_DEFINITION(test_1319) {
    //    1) Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Add "Read sequence" on the scene.
    WorkflowProcessItem* item = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    //    3) Click the element.
    GTUtilsWorkflowDesigner::click(item);
    //    Expected state: bottom datasets panel is visible.
    //    4) Add one input file.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");
    //    Expected state: the element's doc has the blue link to this file.
    //    6) Right click on the link.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Open document(s)"}));
    GTUtilsWorkflowDesigner::clickLink("Read Sequence", Qt::RightButton);
    //    Expected state: a context menu with one action "Open document(s)" must appear.
    //    7) Click on "Open document(s)" menu item.
    //    Expected state: Input file should open in Project View.
    GTUtilsProjectTreeView::checkItem("human_T1.fa");
}

GUI_TEST_CLASS_DEFINITION(test_1319_1) {
    //    1) Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Add "Read sequence" on the scene.
    WorkflowProcessItem* item = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    //    3) Click the element.
    GTUtilsWorkflowDesigner::click(item);
    //    Expected state: bottom datasets panel is visible.
    //    4) Add folder as input files.
    GTUtilsWorkflowDesigner::setDatasetInputFolder(dataDir + "samples/FASTA");
    //    Expected state: the element's doc has the blue link to this folder.
    //    6) Right click on the link.
    GTUtilsWorkflowDesigner::clickLink("Read Sequence", Qt::RightButton);
    //    Expected state: a context menu not showed.
}

GUI_TEST_CLASS_DEFINITION(test_1319_2) {
    //    1) Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Add "Read sequence" on the scene.
    WorkflowProcessItem* item = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    //    3) Click the element.
    GTUtilsWorkflowDesigner::click(item);
    //    Expected state: bottom datasets panel is visible.
    //    4) Add two input files.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/murine.gb");
    //    Expected state: the element's doc has the blue link to this files.
    //    6) Right click on the link.
    GTUtilsDialog::add(new PopupChooserByText({"Open document(s)"}));
    GTUtilsDialog::add(new GTSequenceReadingModeDialogUtils());
    GTUtilsWorkflowDesigner::clickLink("Read Sequence", Qt::RightButton);
    //    Expected state: a context menu with one action "Open document(s)" must appear.
    //    7) Click on "Open document(s)" menu item.
    //    Expected state: All files should open in Project View.
    GTUtilsProjectTreeView::checkItem("human_T1.fa");
    GTUtilsProjectTreeView::checkItem("murine.gb");
}

GUI_TEST_CLASS_DEFINITION(test_1321_1) {
    //    This scenario is about crash found during fixing current bug
    //    1. Open file _common_data/scenarios/_regression/2187/seq.fa
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/2187", "seq.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open 'find repeats' dialog, and set next parameters
    //        {Minimim repeat length} 20bp
    //        {Repeats identity} 80%
    //    3. Press 'Start'
    //    Expected state: one repeat unit was found, with region join(991..1011,1161..1181), and repeat homology 85%
    QDir().mkpath(sandBoxDir + "test_1321_1");
    GTUtilsDialog::waitForDialog(new FindRepeatsDialogFiller(sandBoxDir + "test_1321_1", false, 20, 80));
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find repeats"));

    GTUtilsTaskTreeView::waitTaskFinished();

    const int annotationsCount = GTUtilsAnnotationsTreeView::findItems("repeat_unit").size();
    CHECK_SET_ERR(1 == annotationsCount, QString("Unexpected annotations count: expect '%1', got '%2'").arg(1).arg(annotationsCount));

    GTUtilsAnnotationsTreeView::selectItemsByName({"repeat_unit"});
    const QString homology = GTUtilsAnnotationsTreeView::getQualifierValue("repeat_identity", "repeat_unit");
    CHECK_SET_ERR("85" == homology, QString("Unexpected repeat homology: expect '%1', got '%2'").arg(85).arg(homology));

    const QString annotationRegions = GTUtilsAnnotationsTreeView::getAnnotationRegionString("repeat_unit");
    CHECK_SET_ERR("join(991..1011,1161..1181)" == annotationRegions, QString("Unexpected annotation region: expect '%1', got '%2'").arg("join(991..1011,1161..1181)").arg(annotationRegions));
}

GUI_TEST_CLASS_DEFINITION(test_1321_2) {
    //    1. Open "\samples\FASTA\human_T1.fa"
    //    Expected state: sequence view window appeared
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Press 'Find tandems' tool button
    //    Expected state: 'Find tandems' dialog appeared
    //    3. Go to the 'Advanced' tab of the dialog
    //    Expected state: 'Advanced' tab displayed, there is 'Advanced parameters' groupbox without 'Repeats identity' parameter
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget"), 1);

            GTWidget::findCheckBox("algoCheck", dialog);
            GTWidget::findComboBox("algoCombo", dialog);
            GTWidget::findCheckBox("annotationFitCheck", dialog);
            GTWidget::findLineEdit("annotationFitEdit", dialog);
            GTWidget::findToolButton("annotationFitButton", dialog);
            GTWidget::findCheckBox("annotationAroundKeepCheck", dialog);
            GTWidget::findLineEdit("annotationAroundKeepEdit", dialog);
            GTWidget::findToolButton("annotationAroundKeepButton", dialog);
            GTWidget::findCheckBox("annotationAroundFilterCheck", dialog);
            GTWidget::findLineEdit("annotationAroundFilterEdit", dialog);
            GTWidget::findToolButton("annotationAroundFilterButton", dialog);
            GTWidget::findComboBox("filterAlgorithms", dialog);
            GTWidget::findCheckBox("invertCheck", dialog);
            GTWidget::findCheckBox("excludeTandemsBox", dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindRepeatsDialogFiller(new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find repeats");
}

GUI_TEST_CLASS_DEFINITION(test_1323) {
    // 1. Open \test\_common_data\_regression\1323\sample.bad
    // Expected state: document with file added to project

    GTFileDialog::openFile(testDir + "_common_data/regression/1323/", "sample.bed");
    GTUtilsTaskTreeView::waitTaskFinished();
}
GUI_TEST_CLASS_DEFINITION(test_1324) {
    // 1. Open WD
    // 2. Add "Search for TFBS with SITECON" element
    // 3. Make sure it is possible to enter 0.0001 for Min Err1

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Search for TFBS with SITECON");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Search for TFBS with SITECON"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Min Err1", "0.00010", GTUtilsWorkflowDesigner::textValue);
}

GUI_TEST_CLASS_DEFINITION(test_1325) {
    //    1. Open _common_data\regression\1325\long_gff.gff (choosing GFF format)
    //    2. UGENE will no open the file with error
    //    3. Double click on the unloaded document crashes UGENE

    //    Expected: UGENE does not crash

    GTLogTracer l1;

    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("GFF"));
    GTUtilsProject::openFile(testDir + "_common_data/regression/1325/long_gff.gff");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(l1.hasErrors(), "Expected to have errors in the log, but no errors found");

    GTLogTracer l2;
    GTUtilsDocument::loadDocument("long_gff.gff");
    CHECK_SET_ERR(l2.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_1326) {
    class CallVariantsWizardFiller : public Filler {
    public:
        CallVariantsWizardFiller()
            : Filler("Call Variants Wizard") {
        }
#define GT_CLASS_NAME "GTUtilsDialog::CallVariantsWizardFiller"
#define GT_METHOD_NAME "run"
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QSizePolicy actualPolicy = dialog->sizePolicy();
            QSizePolicy expectedPolicy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            QPoint bottomRight = dialog->mapToGlobal(dialog->rect().bottomRight());
            CHECK_SET_ERR(actualPolicy == expectedPolicy, "size policy dont match");
            QSize prevSize = dialog->size();
            QPoint newBottomRight = QPoint(bottomRight.x() + 5, bottomRight.y() + 5);
            GTMouseDriver::moveTo(bottomRight);
            GTMouseDriver::press();
            GTMouseDriver::moveTo(newBottomRight);
            GTMouseDriver::release();
            GTThread::waitForMainThread();
            CHECK_SET_ERR(prevSize == dialog->size(), "size should not change");
            GTWidget::click(GTWidget::findButtonByText("Cancel"));
        }
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
    };
    GTUtilsDialog::waitForDialog(new CallVariantsWizardFiller());
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("call variants");
}

GUI_TEST_CLASS_DEFINITION(test_1337) {
    // 1) Click "Open file" button.
    // 2) Choose two sequence files with different alphabets (e.g. "_common_data/fasta/DNA.fa" and "_common_data/fasta/amino_multy.fa") and click "Open" button.
    // Expected state: the dialog appears.
    // 3) Choose "Merge sequence mode" and click "OK" button.
    // Expected state: the warning about different alphabets appears.
    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsDialog::add(new GTSequenceReadingModeDialogUtils());
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTFileDialog::openFileList(testDir + "_common_data/fasta/", {"DNA.fa", "amino_multy.fa"});

    GTUtilsDialog::add(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    // 4) Close the project and repeat these steps.
    // Current state: the warning does not appear.
    GTUtilsDialog::add(new GTSequenceReadingModeDialogUtils());
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTFileDialog::openFileList(testDir + "_common_data/fasta/", {"DNA.fa", "amino_multy.fa"});
}

GUI_TEST_CLASS_DEFINITION(test_1338) {
    // 1. Add the "Write annotation" element (or another element with a property in combo box)
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Write Annotations");

    const QString initialText = GTUtilsWorkflowDesigner::getWorkerText("Write Annotations");
    CHECK_SET_ERR(initialText.contains("GenBank"), "Worker item doesn't contain format name");

    // 2. Select another document format
    GTUtilsWorkflowDesigner::setParameter("Document format", "GFF", GTUtilsWorkflowDesigner::comboValue);

    // 3. Click on the scene
    // Expected state : the file format is changed in the description of the element
    const QString textAfter = GTUtilsWorkflowDesigner::getWorkerText("Write Annotations");
    CHECK_SET_ERR(textAfter != initialText && textAfter.contains("GFF"), "Worker item didn't change its content");
}

GUI_TEST_CLASS_DEFINITION(test_1342) {
    class CustomPopupChecker : public CustomScenario {
        void run() {
            auto activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            CHECK_SET_ERR(nullptr != activePopupMenu, "Active popup menu is NULL");

            GTMenu::clickMenuItemByText(activePopupMenu, {"Add element"});

            activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            QAction* dataReadersAction = GTMenu::getMenuItem(activePopupMenu, "Data Readers", true);
            CHECK_SET_ERR(dataReadersAction == nullptr, "Data Readers item is unexpectly found");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::setCurrentTab(GTUtilsWorkflowDesigner::algorithms);

    // 2. print "mer" in line "Name filter"
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("mer");

    // 3. use context menu at the edit field : Add element->Data readers->Read File URL(s)
    // Expected : there is no "Read File URL(s)" element in the menu. UGENE doesn't crash
    GTUtilsDialog::waitForDialog(new PopupChecker(new CustomPopupChecker));
    GTWidget::click(GTWidget::findWidget("sceneView"), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1347) {
    //    1. Run Ugene. Open file _common_data\scenarios\msa\ma2_gapped.aln
    //    2. Select some symbols(for example first three symbols of first sequence)
    //    3. Press ctrl+c
    //    Expected state: selection is copied into clipboard

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "AAGA", QString("unexpected clipboard text: %1").arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_1348) {
    //    1) Create "Element with command line tool" with name "test" and any slots.
    //    2) Use context menu on "test" element in "Custom Elements with External Tools" in "Elements", click "Remove"
    //    3) Use context menu on WD main window, add element -> "Custom Elements with External Tools". Select "test", UGENE DOES NOT crash.

    //    Expected state: There shouldn't be "test" element on the step 3 after removing it

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "Element_1348";

    QList<CreateElementWithCommandLineToolFiller::InOutData> input;
    CreateElementWithCommandLineToolFiller::InOutDataType inOutDataType;
    inOutDataType.first = CreateElementWithCommandLineToolFiller::Sequence;
    inOutDataType.second = "FASTA";
    input << CreateElementWithCommandLineToolFiller::InOutData("in1",
                                                               inOutDataType);
    settings.input = input;
    settings.command = "./ugenem $in1";

    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    QAbstractButton* createElement = GTAction::button("createElementWithCommandLineTool");
    GTWidget::click(createElement);

    GTUtilsWorkflowDesigner::setCurrentTab(GTUtilsWorkflowDesigner::algorithms);
    QTreeWidgetItem* treeItem = GTUtilsWorkflowDesigner::findTreeItem(settings.elementName, GTUtilsWorkflowDesigner::algorithms);
    CHECK_SET_ERR(treeItem != nullptr, "Element not found");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "", "Remove element"));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove"}));
    GTTreeWidget::click(treeItem);
    GTMouseDriver::click(Qt::RightButton);

    const QString groupName = "Custom Elements with External Tools";
    const QStringList groups = GTUtilsWorkflowDesigner::getPaletteGroupNames();
    if (groups.contains(groupName)) {
        CHECK_SET_ERR(!GTUtilsWorkflowDesigner::getPaletteGroupEntriesNames(groupName).contains(settings.elementName), "Element was not removed");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1358) {
    // 1. Open file "test/_common_data/scenarios/workflow designer/222.uwl"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/1358/test_0001.uwl");

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "/samples/Genbank/murine.gb");
    // 2. Press "Run schema"

    // Expected state: UGENE doesn't crash
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1360) {
    //    1) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Connect a reader element with some writer element.
    GTUtilsWorkflowDesigner::addAlgorithm("Read alignment");
    GTUtilsWorkflowDesigner::addAlgorithm("Write alignment");
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::getWorker("Read Alignment");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::getWorker("Write Alignment");
    GTUtilsWorkflowDesigner::connect(read, write);
    //    3) Disconnect them.
    GTUtilsWorkflowDesigner::disconect(read, write);
    //    4) Connect them again.
    GTUtilsWorkflowDesigner::connect(read, write);
    //    5) Specify any input data
    GTUtilsWorkflowDesigner::click("Read Alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln");

    QString s = read->getProcess()->getDescription()->toPlainText();
    CHECK_SET_ERR(s.contains("COI.aln"), "unexpected worker text: " + s);
    //    Expected state: data from dataset s displayed on the read worker
}

GUI_TEST_CLASS_DEFINITION(test_1362) {
    // 1) Open "_common_data/edit_alignment/COI_sub_same_with_gaps.fa".
    GTFileDialog::openFile(testDir + "_common_data/edit_alignment/COI_sub_same_with_gaps.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) MSA context menu: { Statistics -> Generate distance matrix }.
    // 3) Fill in Generate distance matrix dialog:
    //     Distance algorithm : Identity.
    //     Profile mode: Percents.
    //     Exclude gaps: unchecked.
    // 4) Run the task.
    GTUtilsDialog::add(new PopupChooserByText({"Statistics", "Generate distance matrix..."}));
    DistanceMatrixDialogFiller* filler = new DistanceMatrixDialogFiller(false, false, false);
    filler->saveToFile = true;
    filler->format = DistanceMatrixDialogFiller::CSV;
    filler->path = sandBoxDir + "test_1362.csv";
    GTUtilsDialog::add(filler);
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //  Generated report's table should contain "100%" in needed places (where sequences are the same).
    QFile result(sandBoxDir + "test_1362.csv");
    result.open(QIODevice::ReadOnly);
    QByteArray data = result.readAll();
    result.close();

    CHECK_SET_ERR(data.contains("Zychia_baranovi,100%,86%,86%,86%"), "Wrong matrix content 1");
    CHECK_SET_ERR(data.contains("Tettigonia_viridissima,86%,100%,100%,100%"), "Wrong matrix content 2");
    CHECK_SET_ERR(data.contains("Conocephalus_discolor,86%,100%,100%,100%"), "Wrong matrix content 3");
    CHECK_SET_ERR(data.contains("Conocephalus_sp.,86%,100%,100%,100%"), "Wrong matrix content 4");
}

GUI_TEST_CLASS_DEFINITION(test_1364) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add "Read Sequence" element on the scene.
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    //    3. Click the element.
    //    Expected: Bottom datasets panel appears.
    GTUtilsWorkflowDesigner::click("Read Sequence");
    //    4. Click "Add file" button.
    //    Expected: The file dialog appears with some folder A.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
    //    5. Choose some file from some folder B (A != B) and click "Open".
    GTWidget::click(GTWidget::findWidget("addFileButton"));
    //    6. Click "Add file" button again.
    class customFileDialog : public CustomScenario {
    public:
        void run() {
            QWidget* d = GTWidget::getActiveModalWidget();
            auto dialog = qobject_cast<QFileDialog*>(d);
            CHECK_SET_ERR(dialog, "activeModalWidget is not file dialog");

            QString name = dialog->directory().dirName();
            CHECK_SET_ERR(name == "FASTA", "unexpectyed dir name: " + name);
            GTWidget::click(GTWidget::findButtonByText("Cancel", dialog));
        }
    };
    //    Expected: The file dialog opens with the folder B.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(new customFileDialog()));
    GTWidget::click(GTWidget::findWidget("addFileButton"));
}

GUI_TEST_CLASS_DEFINITION(test_1365) {
    // 1. Open file "data/samples/COI.aln" in alignment editor
    // Expected state: "Save all" button on main toolbar is enabled
    // 2. Open Workflow Designer
    // Expected state: "Save all" button is still enabled

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Save all"}, GTGlobals::UseKey);

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Save all"}, GTGlobals::UseKey);
}

GUI_TEST_CLASS_DEFINITION(test_1368) {
    /* 1. Open _common_data\sam\crash.sam
     * Expected state: UGENE not crashes
     */
    QString ugenedbFileName = testDir + "_common_data/scenarios/sandbox/test_1368.ugenedb";
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(ugenedbFileName));
    GTFileDialog::openFile(testDir + "_common_data/sam", "crash.sam");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1371) {
    //    1. Open file "data/samples/ACE/BL060C3.ace" as msa.
    //    Expected state: there are 2 MSA objects in document.
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkItem("Contig1");
    GTUtilsProjectTreeView::checkItem("Contig2");
    GTUtilsProjectTreeView::checkObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT}, GTUtilsProjectTreeView::findIndex("BL060C3.ace"));

    //    2. Open file "data/samples/ACE/BL060C3.ace" as assembly.
    //    Expected state: there are 2 assembly objects in document.

    GTUtilsDocument::removeDocument("BL060C3.ace");
    QDir().mkpath(sandBoxDir + "test_1371");

    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_1371.ugenedb"));
    GTUtilsProject::openFile(dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkItem("Contig1");
    GTUtilsProjectTreeView::checkItem("Contig1_ref");
    GTUtilsProjectTreeView::checkItem("Contig2");
    GTUtilsProjectTreeView::checkItem("Contig2_ref");
    GTUtilsProjectTreeView::checkObjectTypes(
        {GObjectTypes::ASSEMBLY, GObjectTypes::SEQUENCE},
        GTUtilsProjectTreeView::findIndex("test_1371.ugenedb"));
}

GUI_TEST_CLASS_DEFINITION(test_1376) {
    // 1. Open data/position_weight_matrix/JASPAR/fungi/MA0276.1.pfm.
    GTFileDialog::openFile(dataDir + "position_weight_matrix/JASPAR/fungi/MA0276.1.pfm");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: PFM matrix view with logo and weight table is opened.
    // QWidget *mdi = GTUtilsMdi::findWindow("Matrix Viewer");
    GTWidget::findWidget("logoWidget");
    GTWidget::findWidget("tableWidget");
}

GUI_TEST_CLASS_DEFINITION(test_1386) {
    //    1) Open a document (e.g. COI.aln)
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Modify it (e.g. insert a gap)
    GTUtilsMSAEditorSequenceArea::click(QPoint(5, 5));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    //    3) Close the view of the document
    GTUtilsMdi::click(GTGlobals::Close);
    //    4) Select "Unload selected documents" for the document
    GTUtilsDialog::add(new PopupChooser({"action_project__unload_selected_action"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);
    //    5) Select "No" not to save the document
    GTUtilsDocument::isDocumentLoaded("COI.aln");
    GTUtilsProjectTreeView::itemModificationCheck(GTUtilsProjectTreeView::findIndex("COI.aln"), false);
    //    Expected state: the document is unloaded, not marked as modified (blue).
}

GUI_TEST_CLASS_DEFINITION(test_1387) {
    // 1) Open _common_data\regression\1387\col_of_gaps.aln
    // 2) Use context menu: {Edit -> Remove columns of gaps}
    // 3) In the appeared dialog select "Remove columns with percentage of gaps" and click the "Remove" button.
    // Expected state: UGENE not crashed

    GTFileDialog::openFile(testDir + "_common_data/regression/1387/", "col_of_gaps.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EDIT, "remove_columns_of_gaps"}));

    GTUtilsDialog::add(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Percent, 15));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
}
GUI_TEST_CLASS_DEFINITION(test_1390) {
    /*  1. Open some assembly with assembly browser
    2. Open the "Assembly Browser Settings" tab in options panel
    3. Choose the "Difference" type of "Reads highlighting" (if not chosen by default)
        Expected state: the hint about setting reference sequence is displayed
    4. Set any other value for "Reads highlighting"
        Expected state: the hint has vanished
    5. Set the "Difference" again from assembly context menu { Reads highlighting -> Difference }
        Expected state: the hint has appeared again in options panel
*/

    QString ugenedbFileName = testDir + "_common_data/scenarios/sandbox/test_1390.ugenedb";
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(ugenedbFileName));
    GTFileDialog::openFile(testDir + "_common_data/bam", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("OP_ASS_SETTINGS"));
    auto hint = GTWidget::findLabel("HINT_HIGHLIGHTNING");
    CHECK_SET_ERR(!hint->text().isEmpty(), "Hint is empty, but must not be");

    auto highlightingBox = GTWidget::findComboBox("READS_HIGHLIGHTNING_COMBO");
    GTComboBox::selectItemByText(highlightingBox, "Nucleotide");
    CHECK_SET_ERR(hint->text().isEmpty(), "Hint is not empty, but must be");

    GTComboBox::selectItemByText(highlightingBox, "Difference");
    CHECK_SET_ERR(!hint->text().isEmpty(), "Hint is empty, but must not be");
}

GUI_TEST_CLASS_DEFINITION(test_1393) {
    class ExportSeqsAsMsaScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto addToProjectBox = GTWidget::findCheckBox("addToProjectBox", dialog);
            CHECK_SET_ERR(addToProjectBox->isChecked(), "'Add document to project' checkbox is not set");

            auto lineEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(lineEdit, sandBoxDir + "test_1393.aln");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Open file "_common_data/fasta/trim_fa.fa"
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/fasta/trim_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Choose{ Export->Export sequences as alignment } in context menu of project view
    // Expected state : "Export sequences as alignment" dialog appears
    // 3. Make sure that "Add document to project" checkbox is set and press "Export" button.
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(new ExportSequenceAsAlignmentFiller(new ExportSeqsAsMsaScenario));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("trim_fa.fa"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : New *.aln file appears in project view.Names of sequences in consensus area
    // are the same as if you open the *.aln file in text editor.
    GTUtilsProjectTreeView::checkItem("test_1393.aln");

    QString referenceMsaContent = GTFile::readAll(testDir + "_common_data/regression/1393/test_1393.aln");
    QString resultMsaContent = GTFile::readAll(sandBoxDir + "test_1393.aln");
    CHECK_SET_ERR(!referenceMsaContent.isEmpty() && referenceMsaContent == resultMsaContent, "Unexpected MSA content");
}

GUI_TEST_CLASS_DEFINITION(test_1396) {
    /* 1. Open file test/_common_data/scenarios/_regression/1396/empty_rows.fa
     * Expected state: empty alignment and UGENE not crashes
     */
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1396", "empty_rows.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    int rowCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(rowCount == 2, "Wrong rows number: " + QString::number(rowCount));
}

GUI_TEST_CLASS_DEFINITION(test_1405) {
    // 1) Open _common_data/scenarios/msa/ma2_gap_col.aln
    // 2) Try to delete columns with gaps (first option, 1 gap).
    // Expected state: Safe_point was not triggered, i.e. no error messages in log

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EDIT, "remove_columns_of_gaps"}));

    GTUtilsDialog::add(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Number, 1));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());
}

GUI_TEST_CLASS_DEFINITION(test_1408) {
    //    1) Open "data/samples/FASTA/human_T1.fa"
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Right click on "human_T1.fa" in the project tab

    class innerScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::findWidget("groupRB", dialog);
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    class outerScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto readFileLineEdit = GTWidget::findLineEdit("readFileName", dialog);
            GTLineEdit::setText(readFileLineEdit, testDir + "_common_data/scenarios/annotations_import/anns1.csv");

            GTWidget::click(GTWidget::findWidget("guessButton", dialog));

            auto previewTable = GTWidget::findTableWidget("previewTable", dialog);
            QRect rect = previewTable->visualItemRect(previewTable->item(1, 1));
            GTUtilsDialog::waitForDialog(new RoleFiller(new innerScenario()));
            GTWidget::click(previewTable, Qt::LeftButton, rect.center());

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportAnnotationsToCsvFiller(new outerScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"action_project__export_import_menu_action", "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("human_T1.fa", Qt::RightButton);
    //    3) Use menu {Export/Import->Import annotaions from CSV file}
    //    Expected state: "Import annotations from CSV" dialog is appeared

    //    4) Choose any CSV file for read
    //    Expected state: table in "Results preview" is appeared

    //    5) Left click on name of any column
    //    Expected state: "Select the role of the column" dialog is appeared

    //    6) Check that there is role "Annotation group"
}

GUI_TEST_CLASS_DEFINITION(test_1409) {
    //    1. Open "_common_data/genbank/murine_sarcoma.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/murine_sarcoma.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: sequence viewer had opened.

    //    2. Click on some annotation in the sequence view (not in the annotation tree).
    GTUtilsSequenceView::clickAnnotationDet("CDS", 1042, 0, true);
    //    Expected state: the clicked annotation is selected.

    const QPoint pos = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(QPoint(pos.x(), pos.y() - 100));
    GTMouseDriver::click();
    GTUtilsAnnotationsTreeView::clickItem("CDS", 1, true);

    //    3. Press F2 on the keyboard.
    GTUtilsDialog::waitForDialog(new EditAnnotationFiller("CDS", "1042..2658"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    //    Expected state: the "edit annotation" dialog had opened.
}

GUI_TEST_CLASS_DEFINITION(test_1419) {
    // 1. Open "_common_data/scenarios/msa/big.aln".
    // Expected state : UGENE does not crash.File opened successfully.
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1420) {
    //    1. Select {Tools->ALign to reference->Align short reads} from the main menu.
    //    Expected state: the "Align sequencing reads" dialog appeared.
    //    2. Fill this dialog with:
    //        {Mapping:}    BWA-SW
    //        {Reference sequence:}    _common_data/fasta/NC_008253.fna
    //        {Result file name:}    somewhere in the temp folder
    //        {Short reads:}        _common_data/reads/long_reads.fasta
    //        other options: default
    //    3. Press the "Start" button.
    //    Expected state: after the task's end the "Import SAM file" appeared.
    //    4. Set {destination URL} and press the "Import" button.
    //    Expected state: the Assembly browser with alignment result opened.

    QDir().mkpath(sandBoxDir + "test_1402");
    AlignShortReadsFiller::BwaSwParameters parameters(testDir + "_common_data/fasta", "NC_008253.fna", testDir + "_common_data/reads", "long_reads.fasta");
    parameters.resultDir = sandBoxDir + "test_1402";
    parameters.resultFileName = "test_1402.sam";

    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "test_1402/test_1402.ugenedb"));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 expectedLength = 4938920;
    qint64 expectedReads = 269;
    qint64 assemblyLength = GTUtilsAssemblyBrowser::getLength();
    qint64 assemblyReads = GTUtilsAssemblyBrowser::getReadsCount();
    CHECK_SET_ERR(expectedLength == assemblyLength, QString("An unexpected assembly length: expect  %1, got %2").arg(expectedLength).arg(assemblyLength));
    CHECK_SET_ERR(expectedReads == assemblyReads, QString("An unexpected assembly reads count: expect  %1, got %2").arg(expectedReads).arg(assemblyReads));
}

GUI_TEST_CLASS_DEFINITION(test_1424) {
    // 1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    // 2. Edit sequence area(insert gap or something else)
    // 3. Delete any sequence(or several sequences)
    // 4. Press {undo} button
    // Expected state: Sequence deletion undone
    // 5. Press {undo} button
    // 6. delete sequence again.
    // 7. And again.
    // 8. press undo button twice.
    // Expected state: UGENE not crashes

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 10));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTUtilsMsaEditor::clickSequenceName("Conocephalus_sp.");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsMsaEditor::undo();

    GTUtilsMsaEditor::clickSequenceName("Conocephalus_sp.");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsMsaEditor::clickSequenceName("Deracantha_deracantoides_EF540");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::undo();
    GTUtilsMsaEditor::undo();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 18,
                  "Incorrect sequences number");
}

GUI_TEST_CLASS_DEFINITION(test_1426) {
    // 1. Open WD.

    // 2. Add "Read HMM profile" element to the scene.

    // 3. Specify the input file name in property editor.
    // Expected result: "add" button appears in property editor.

    // 4. Delete the element from the scene.

    // 5. Add the element of the same type to the scene again.
    // Expected state: there is no "add" button in property editor.

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Read HMM2 profile");

    CHECK_SET_ERR(GTWidget::findWidget("addButton", nullptr, {false}) == nullptr, "addButton is shown");

    GTUtilsWorkflowDesigner::click("Read HMM2 Profile");

    auto table = GTWidget::findTableView("table");

    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 0));
    GTMouseDriver::click();

    auto line = qobject_cast<QLineEdit*>(table->findChild<QLineEdit*>());
    CHECK_SET_ERR(line, "QLineEdit not found. Widget in this cell might be not QLineEdit");
    GTLineEdit::setText(line, dataDir + "samples/FASTA/HMM/aligment15900.hmm");
    if (isOsMac()) {
        GTMouseDriver::doubleClick();
    }
    CHECK_SET_ERR(GTWidget::findWidget("addButton") != nullptr, "addButton is hiden");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Discard));
    GTUtilsMdi::click(GTGlobals::Close);

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Read HMM2 Profile");
    GTUtilsWorkflowDesigner::click("Read HMM2 Profile");

    CHECK_SET_ERR(GTWidget::findWidget("addButton", nullptr, {false}) == nullptr, "addButton is shown");
}

GUI_TEST_CLASS_DEFINITION(test_1427) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1427/", "text");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkItem("text");
}

GUI_TEST_CLASS_DEFINITION(test_1428) {
    // 1. Open human_T1.fa.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the document and the sequence object itself in the Project View.
    QModelIndex docIdx = GTUtilsProjectTreeView::findIndex("human_T1.fa", QModelIndex());
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(docIdx));
    GTMouseDriver::click();
    QModelIndex seqIdx = GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIdx);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(seqIdx));
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // 3. Use context menu {Export/Import->Export Sequences} and export sequence to any file.
    GTUtilsDialog::waitForDialog(new ExportSelectedRegionFiller(sandBoxDir, "test_1428.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there is only one sequence in the file (it's the same as in human_t1.fa).
    QModelIndex expIdx = GTUtilsProjectTreeView::findIndex("test_1428.fa", QModelIndex());
    int objCount = GTUtilsProjectTreeView::getTreeView()->model()->rowCount(expIdx);
    CHECK_SET_ERR(1 == objCount, "Wrong exported sequence count");
}

GUI_TEST_CLASS_DEFINITION(test_1429) {
    //    0. Ensure that Bowtie2 Build index tool is not set. Remove it, if it is.
    //    1. Do {main menu -> Tools -> ALign to reference -> Build index}.
    //    Expected state: a "Build index" dialog appeared.
    //    2. Fill the dialog:
    //        {Align short reads method}: Bowtie
    //        {Reference sequence}:       _common_data/fasta/amino_multy.fa
    //        {Index file name}:          set any valid data or use default
    //    Click a "Start" button.

    //    Expected state: there are no errors in the log, index files appeared in the destination folder.
    GTUtilsExternalTools::removeTool("Bowtie 2 build indexer");

    GTLogTracer lt;
    class CheckBowtie2Filler : public Filler {
    public:
        CheckBowtie2Filler()
            : Filler("BuildIndexFromRefDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto methodNamesBox = GTWidget::findComboBox("methodNamesBox", dialog);
            for (int i = 0; i < methodNamesBox->count(); i++) {
                if (methodNamesBox->itemText(i) == "Bowtie") {
                    GTComboBox::selectItemByIndex(methodNamesBox, i);
                }
            }

            GTFileDialogUtils* ob = new GTFileDialogUtils(testDir + "_common_data/fasta/", "multy_fa.fa");
            GTUtilsDialog::waitForDialog(ob);
            GTWidget::click(GTWidget::findWidget("addRefButton", dialog));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new CheckBowtie2Filler());
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Build index for reads mapping..."});
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1432) {
    //    1. Open WD
    //    2. Add worker "sequence marker"
    //    3. Click button "add" in Parameters
    //    Expected state: create marker group dialog appeared
    //    4. Add several similar markers
    //    Expected state: each new marker has a number after name

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Sequence Marker");

    auto addButton = GTWidget::findToolButton("addButton");

    class OkClicker : public Filler {
    public:
        OkClicker()
            : Filler("EditMarkerGroupDialog") {
        }
        void run() override {
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new OkClicker());
    GTWidget::click(addButton);

    auto groupTable = GTWidget::findTableView("markerTable");
    for (int i = 1; i < 3; i++) {
        GTUtilsDialog::waitForDialog(new OkClicker());
        GTWidget::click(addButton);
        GTWidget::click(groupTable);

        QString name = GTTableView::data(groupTable, i, 0);
        CHECK_SET_ERR(name.endsWith(QString::number(i)), QString("Wrong %1 marker group name: %2").arg(i).arg(name));
    }
}

GUI_TEST_CLASS_DEFINITION(test_1434_1) {
    // 1. Open data / samples / FASTA / human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open find pattern option panel
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Use these settings :
    // search for : > header
    //              ATTCACCAAAGTTGAA*TGAAGGAAAAAATGCT
    // Algorithm : Regular expression
    // Region : custom region(1 - 1000)

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));

    auto algorithmBox = GTWidget::findComboBox("boxAlgorithm");
    GTComboBox::selectItemByText(algorithmBox, "Regular expression");

    GTWidget::click(GTWidget::findWidget("textPattern"));
    GTKeyboardDriver::keySequence("> header");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);
    GTKeyboardDriver::keySequence("ATTCACCAAAGTTGAA");
    GTKeyboardDriver::keyClick('8', Qt::ShiftModifier);
    GTKeyboardDriver::keySequence("TGAAGGAAAAAATGCT");

    GTUtilsOptionPanelSequenceView::setRegionType("Custom region");
    GTLineEdit::setText(GTWidget::findLineEdit("editStart"), "1");
    GTLineEdit::setText(GTWidget::findLineEdit("editEnd"), "1000");

    // Expected state : 1 pattern is found
    auto resultLabel = GTWidget::findLabel("resultLabel");
    CHECK_SET_ERR(resultLabel->text() == "Results: 1/1", "Unexpected find algorithm result count");
}

GUI_TEST_CLASS_DEFINITION(test_1434_2) {
    // 1. Open data / samples / FASTA / human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open find pattern option panel
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Use these settings :
    //     search for:ATTCACCAAAGTTGAA*TGAAGGAAAAAATGCT
    //  ; comment
    // Algorithm : Regular expression
    // Region : custom region(1 - 1000)

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));

    auto algorithmBox = GTWidget::findComboBox("boxAlgorithm");
    GTComboBox::selectItemByText(algorithmBox, "Regular expression");

    GTWidget::click(GTWidget::findWidget("textPattern"));
    GTKeyboardDriver::keySequence("ATTCACCAAAGTTGAA");
    GTKeyboardDriver::keyClick('8', Qt::ShiftModifier);
    GTKeyboardDriver::keySequence("TGAAGGAAAAAATGCT");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);
    GTKeyboardDriver::keyClick(';');
    GTKeyboardDriver::keySequence(" comment");

    GTUtilsOptionPanelSequenceView::setRegionType("Custom region");
    GTLineEdit::setText(GTWidget::findLineEdit("editStart"), "1");
    GTLineEdit::setText(GTWidget::findLineEdit("editEnd"), "1000");

    // Expected state : 1 pattern is found
    auto resultLabel = GTWidget::findLabel("resultLabel");
    CHECK_SET_ERR(resultLabel->text() == "Results: 1/1", "Unexpected find algorithm result count");
}

GUI_TEST_CLASS_DEFINITION(test_1446) {
    //    1) Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Use context menu for COI.aln in project tree view {Export/Import->Export nucleic alignment to amino}
    GTUtilsDialog::add(new PopupChooser({"action_project__export_import_menu_action", "action_project__export_to_amino_action"}));
    GTUtilsDialog::add(new ExportMSA2MSADialogFiller(-1, sandBoxDir + "test_1446.aln"));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: none of sequences starts from (translated), only sequence names have (translated) in the end
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    QString first = names[0];
    CHECK_SET_ERR(first.endsWith("(translated)"), "unexpected name end: " + first);
    foreach (QString name, names) {
        CHECK_SET_ERR(!name.startsWith("(translated)"), "unexpected name start: " + name);
    }
}

GUI_TEST_CLASS_DEFINITION(test_1455) {
    GTLogTracer lt;
    // 1. Create the following scheme in WD:
    //"Read Sequence" -> "Dump Sequence Info" -> "Write Plain Text"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Dump Sequence Info");
    GTUtilsWorkflowDesigner::addAlgorithm("Write Plain Text");
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Dump Sequence Info"));
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Dump Sequence Info"), GTUtilsWorkflowDesigner::getWorker("Write Plain Text"));

    // 2. Save it somewhere using the "Save as..." action
    GTUtilsWorkflowDesigner::saveWorkflowAs(sandBoxDir + "dump_sequence.uwl", "Dump Sequence Info");
    // 3. Close WD
    GTUtilsMdi::click(GTGlobals::Close);
    // GTMouseDriver::click();
    // 4. Reopen the scheme's file
    //    Expected result: scheme is loaded completely without any error messages in log
    GTFileDialog::openFile(sandBoxDir + "dump_sequence.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1457) {
    //    1. Open "_common_data/ugenedb/example-alignment.ugenedb".
    //    Expected state: assymbly viewer had opened.
    GTFileDialog::openFile(testDir + "_common_data/ugenedb/example-alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Find navigation tool on the toolbar.
    //    Expected state: navigation tool is disabled.
    QToolBar* mwtoolbar_activemdi = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    auto go_to_pos_line_edit = GTWidget::findWidget("go_to_pos_line_edit", mwtoolbar_activemdi);
    auto go = GTWidget::findWidget("goButton", mwtoolbar_activemdi);
    CHECK_SET_ERR(!go_to_pos_line_edit->isEnabled(), "go_to_pos_line_edit on toolbar is enabled");
    CHECK_SET_ERR(!go->isEnabled(), "go button on toolbar is enabled");
    //    3. Open navigation tab on the options panel.
    GTWidget::click(GTWidget::findWidget("OP_ASS_NAVIGATION"));
    //    Expected state: navigation tool on the options panel is disabled.
    auto OP_OPTIONS_WIDGET = GTWidget::findWidget("OP_OPTIONS_WIDGET");
    auto go_to_pos_line_edit_op = GTWidget::findWidget("go_to_pos_line_edit", OP_OPTIONS_WIDGET);
    auto go_op = GTWidget::findWidget("goButton", OP_OPTIONS_WIDGET);
    CHECK_SET_ERR(!go_to_pos_line_edit_op->isEnabled(), "go_to_pos_line_edit on option panel is enabled");
    CHECK_SET_ERR(!go_op->isEnabled(), "go button on option panel is enabled");
    //    4. Zoom to any location.
    QWidget* zoomInButton = GTToolbar::getWidgetForActionTooltip(mwtoolbar_activemdi, "Zoom in");
    GTWidget::click(zoomInButton);
    //    Expected state: navigation was enabled both on the toolbar and on the options panel.
    CHECK_SET_ERR(go_to_pos_line_edit->isEnabled(), "go_to_pos_line_edit on toolbar is not enabled");
    CHECK_SET_ERR(go->isEnabled(), "go button on toolbar is not enabled");

    CHECK_SET_ERR(go_to_pos_line_edit_op->isEnabled(), "go_to_pos_line_edit on option panel is not enabled");
    CHECK_SET_ERR(go_op->isEnabled(), "go button on option panel is not enabled");
}

GUI_TEST_CLASS_DEFINITION(test_1458) {
    // 1. Open document "../Samples/ACE/BL060C3.ace"
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_1458.ace.ugenedb"));
    GTUtilsProject::openFile(dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Right click on the document in project tab
    // Excepted state: popup menu has been appeared
    // 3. Select "Export document"
    // 4. Check, that for all output file formats export work correctly
    GTUtilsDialog::add(new PopupChooser({"Export document"}));
    GTUtilsDialog::add(new ExportDocumentDialogFiller(sandBoxDir, "test_1458.fa", ExportDocumentDialogFiller::UGENEDB, false, true));
    GTUtilsProjectTreeView::click("test_1458.ace.ugenedb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1435) {
    //    1) Open WD
    //    2) Click Create element with command line tool
    //    3) input name test
    //    4) input data in1 and in2 of FASTA
    //    5) output data out1 and out2 of FASTA
    //    6) Execution string any
    //    Expected state created element with two input and two output ports

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "Element_1435";

    QList<CreateElementWithCommandLineToolFiller::InOutData> input;
    CreateElementWithCommandLineToolFiller::InOutDataType inOutDataType;
    inOutDataType.first = CreateElementWithCommandLineToolFiller::Sequence;
    inOutDataType.second = "FASTA";

    input << CreateElementWithCommandLineToolFiller::InOutData("in1",
                                                               inOutDataType);
    input << CreateElementWithCommandLineToolFiller::InOutData("in2",
                                                               inOutDataType);
    settings.input = input;

    QList<CreateElementWithCommandLineToolFiller::InOutData> output;
    output << CreateElementWithCommandLineToolFiller::InOutData("out1",
                                                                inOutDataType);
    output << CreateElementWithCommandLineToolFiller::InOutData("out2",
                                                                inOutDataType);
    settings.output = output;

    settings.command = "./ugenem $in1 $in2 $out1 $out2";

    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    QAbstractButton* createElement = GTAction::button("createElementWithCommandLineTool");
    GTWidget::click(createElement);

    GTUtilsWorkflowDesigner::click("Element_1435");
    WorkflowProcessItem* element = GTUtilsWorkflowDesigner::getWorker("Element_1435");
    CHECK_SET_ERR(element != nullptr, "Worker not found");
    int portCount = GTUtilsWorkflowDesigner::getPorts(element).size();
    CHECK_SET_ERR(portCount == 3,
                  QString("Port number is wrong. Expected: 3. Current %1").arg(portCount));
}

GUI_TEST_CLASS_DEFINITION(test_1439) {
    //    1. Open _common_data\scenarios\regression\1439\NC_000964_multi_region.fa in MSA Editor.
    //    2. Do {Align->Align sequences to profile with MUSCLE...} in context menu, and use NC_000964.fa as alignment profile.
    //    3. Select "NC_000964.fa" as profile.
    //    Expected state: there is no error with following message
    //                    "Task {MUSCLE align 'NC_000964.fa' by profile 'NC_000964_multi_region.fa'} finished with error:
    //                    Subtask {MUSCLE add to profile 'NC_000964_multi_region.fa'} is failed:
    //                    Subtask {MUSCLE alignment} is failed:
    //                    Internal MUSCLE error: Internal error MSA::ExpandCache, ColCount changed"
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/1439/NC_000964_multi_region.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/1439", "NC_000964.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align sequences to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
    QString expectedError = "Can't align sequences that are longer than 100000 bp.";

    CHECK_SET_ERR(lt.getJoinedErrorString().contains(expectedError), "Wrong error in the log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1442_1) {
    //    1. Open file "data/position_weight_matrix/JASPAR/fungi/MA0265.1.pfm"
    //    Expected state: Opened only window with position frequency matrix and it's Logo.
    //    In Project View not added any items.

    GTFileDialog::openFile(dataDir + "position_weight_matrix/JASPAR/fungi", "MA0265.1.pfm");
    GTUtilsTaskTreeView::waitTaskFinished();

    const int projectViewItemsCount = GTUtilsProjectTreeView::getTreeView()->model()->rowCount();
    CHECK_SET_ERR(0 == projectViewItemsCount, "Unexpected project view items count");

    GTWidget::findWidget("Matrix viewer");  // check that matrix view is presented

    auto logoWidget = GTWidget::findWidget("logoWidget", nullptr);
    CHECK_SET_ERR(logoWidget->isVisible(), "Logo widget is unexpectedly invisible");
}

GUI_TEST_CLASS_DEFINITION(test_1442_2) {
    // 1. Open file "data/position_weight_matrix/UniPROBE/Cell08/Alx3_3418.2.pwm"
    // Expected state : Opened only window with position weight matrix.
    // In Project View not added any items.
    GTFileDialog::openFile(dataDir + "/position_weight_matrix/UniPROBE/Cell08/", "Alx3_3418.2.pwm");
    GTUtilsTaskTreeView::waitTaskFinished();

    const int projectViewItemsCount = GTUtilsProjectTreeView::getTreeView()->model()->rowCount();
    CHECK_SET_ERR(0 == projectViewItemsCount, "Unexpected project view items count");

    GTWidget::findWidget("Matrix viewer");  // check that matrix view is presented

    auto logoWidget = GTWidget::findWidget("logoWidget", nullptr);
    CHECK_SET_ERR(!logoWidget->isVisible(), "Logo widget is unexpectedly visible");
}

GUI_TEST_CLASS_DEFINITION(test_1442_3) {
    //    1. Open file "data/position_weight_matrix/UniPROBE/NBT06/Cbf1.pwm"
    GTFileDialog::openFile(dataDir + "position_weight_matrix/UniPROBE/NBT06/Cbf1.pwm");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: Opened only window with position weight matrix.
    GTWidget::findWidget("MatrixAndLogoWidget");
    //    In Project View not added any items.
    int num = GTUtilsProjectTreeView::getTreeView()->model()->rowCount();
    CHECK_SET_ERR(num == 0, QString("%1 document(s) unexpectidly present in project view").arg(num))
}

GUI_TEST_CLASS_DEFINITION(test_1443) {
    //    1. Open the file human_T1.fa.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Use popup menu {Cloning->Construct molecule}

    class InnerScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto start_edit_line = GTWidget::findLineEdit("start_edit_line", dialog);
            CHECK_SET_ERR(start_edit_line->text() == "1", "unexpected start text " + start_edit_line->text());
            auto end_edit_line = GTWidget::findLineEdit("end_edit_line", dialog);
            CHECK_SET_ERR(end_edit_line->text() == "199950", "unexpected end text " + end_edit_line->text());
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    4. Select human_t1 sequence object
            //    5. In the dialog "Create DNA Fragment" make sure the region is 1..199950 and click "Ok"
            GTUtilsDialog::waitForDialog(new CreateFragmentDialogFiller(new InnerScenario()));
            GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("human_T1.fa", "human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
            //    3. Click "From Project" button
            GTWidget::click(GTWidget::findWidget("fromProjectButton"));

            //    6. Select the only available fragment and click "Add"
            auto fragmentListWidget = GTWidget::findListWidget("fragmentListWidget", dialog);
            GTListWidget::click(fragmentListWidget, "human_T1 (UCSC April 2002 chr7:115977709-117855134) (human_T1.fa) Fragment (1-199950)");
            GTWidget::click(GTWidget::findWidget("takeButton", dialog));
            //    7. Uncheck "Force blunt and omit all overhangs"
            auto makeBluntBox = GTWidget::findCheckBox("makeBluntBox", dialog);
            GTCheckBox::setChecked(makeBluntBox, false);
            //    8. Check "Make circular"
            auto makeCircularBox = GTWidget::findCheckBox("makeCircularBox", dialog);
            GTCheckBox::setChecked(makeCircularBox, true);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(new Scenario()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cloning", "CLONING_CONSTRUCT"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    //    Expected: Ugene not crashes
}

GUI_TEST_CLASS_DEFINITION(test_1445) {
    //    1. Open "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Choose last sequence (i.e. in bottom) with mouse in sequences area
    GTUtilsMsaEditor::clickSequence(17);
    const int numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelectedSequences == 1, "There is no selection in MSA, but expected");

    //    3. Choose { Edit -> Remove sequence } in context menu
    //    Expected state: UGENE doesn't crash
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "Remove sequence"}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1461_1) {
    //    1. Open "_common_data/fasta/fa1.fa".
    //    Expected state: sequence viewer had opened.
    GTFileDialog::openFile(testDir + "_common_data/fasta/fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click on toolbar "Find pattern [Smith-Waterman]".
    //    Expected state: "Smith-Waterman Search" dialog is opened.
    //    3. Check "Scoring matrix" field
    //    Expected state: "Scoring matrix" field not contain "rna" value.
    class Scenario : public CustomScenario {
        void run() {
            auto comboMatrix = GTWidget::findComboBox("comboMatrix", GTWidget::getActiveModalWidget());
            for (int i = 0; i < comboMatrix->count(); i++) {
                CHECK_SET_ERR(!comboMatrix->itemText(i).contains("rna", Qt::CaseInsensitive),
                              QString("'rna' item unexpectidly found at index: %1, text is %2").arg(i).arg(comboMatrix->itemText(i)));
            }
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new Scenario));
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find pattern [Smith-Waterman]"));
}

GUI_TEST_CLASS_DEFINITION(test_1461_2) {
    //    1. Open "_common_data/fasta/RNA_1_seq.fa".
    //    Expected state: sequence viewer had opened.
    GTFileDialog::openFile(testDir + "_common_data/fasta/RNA_1_seq.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click on toolbar "Find pattern [Smith-Waterman]".
    //    Expected state: "Smith-Waterman Search" dialog is opened.
    //    3. Check "Scoring matrix" field
    //    Expected state: "Scoring matrix" field contain only "rna" value.
    class Scenario : public CustomScenario {
        void run() {
            auto comboMatrix = GTWidget::findComboBox("comboMatrix", GTWidget::getActiveModalWidget());
            GTComboBox::selectItemByText(comboMatrix, "rna");
            CHECK_SET_ERR(1 == comboMatrix->count(), "There are several unexpected matrices");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new Scenario));
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find pattern [Smith-Waterman]"));
}

GUI_TEST_CLASS_DEFINITION(test_1463) {
    // 1. Open "human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click the "Find restriction sites" button on the toolbar.
    // Expected state: the "Find restriction sites" dialog had appeared.
    // 3. Click the "OK" button.
    // Expected state: new auto annotations had been added.
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"BamHI", "XmaI", "DraI", "ClaI"}));
    GTWidget::click(GTWidget::findWidget("Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Click the "Show circular view" button on the Sequence Viewer's toolbar.
    // Expected state: additional widget with circular view had opened.
    GTWidget::click(GTWidget::findWidget("CircularViewAction"));

    // 5. Look at the "Restrictions Sites Map" widget to the right of the circular view.
    auto tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget("restrictionMapTreeWidget"));

    // Expected state: enzymes in this widget are ordered alphabetically.
    QString item1 = tree->topLevelItem(0)->text(0);
    QString item2 = tree->topLevelItem(1)->text(0);
    QString item3 = tree->topLevelItem(2)->text(0);
    QString item4 = tree->topLevelItem(3)->text(0);
    CHECK_SET_ERR((item1 < item2) && (item2 < item3) && (item3 < item4), "Wrong order");
}

GUI_TEST_CLASS_DEFINITION(test_1475) {
    GTUtilsDialog::waitForDialog(new SelectDocumentFormatDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/raw_sequence/NC_000117.txt");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1483) {
    //    1. Open "data/COI.aln" in MSA view
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Press tool button "Enable collapsing"
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    3. Start selection in sequences name list and then go for the lower boundary of the list
    //    Expected state: Ugene doesn't crashes
    GTUtilsMsaEditor::selectRows(5, 20, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_1491) {
    // 1. Open UGENE
    // 2. Press File->Open
    // 3. Select more than three sequences (for instance all the sequences from samples/Genbank)
    class Scenario : public CustomScenario {
    public:
        void run() {
            // 4. Select "Join sequences..." mode
            GTRadioButton::click(dynamic_cast<QRadioButton*>(GTWidget::findWidget("join2alignmentMode", GTWidget::getActiveModalWidget())));

            // 5. Select a sequence
            auto list = dynamic_cast<QListWidget*>(GTWidget::findWidget("listDocuments", GTWidget::getActiveModalWidget()));
            GTListWidget::click(list, "3. murine.gb");

            // 6. Press "Up" or "Down" arrow.
            GTWidget::click(GTWidget::findWidget("upperButton", GTWidget::getActiveModalWidget()));

            // Expected state:
            //     1) the sequence goes up or down correspondingly
            QListWidgetItem* murine = list->item(1);
            CHECK_SET_ERR(murine->text() == "2. murine.gb", "Wrong order file");

            //    2) it is still selected
            QList<QListWidgetItem*> selection = list->selectedItems();
            CHECK_SET_ERR(selection.contains(murine), "Wrong selection");
            CHECK_SET_ERR(1 == selection.size(), "Wrong selection size");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils(new Scenario()));
    GTFileDialog::openFileList(dataDir + "samples/Genbank", {"PBR322.gb", "sars.gb", "murine.gb", "NC_014267.1.gb"});
}

GUI_TEST_CLASS_DEFINITION(test_1497) {
    GTKeyboardDriver::keyClick('3', Qt::AltModifier);
    uiLog.error("log should contain some text for test 1497");

    // 1. Create or open some scheme in WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a few items(elements, links) in the scheme.
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Align with MUSCLE"));
    GTMouseDriver::click();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write alignment"));
    GTMouseDriver::click();

    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // 3. Try to copy something from the log widget to the clipboard by hotkey(Ctrl + C).
    // Expected state : you've got in the clipboard selected text from the log widget.
    uiLog.error("log must contain some text");
    auto logView = GTWidget::findWidget("dock_log_view");
    GTWidget::click(logView);

    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    const QString clipboardContent = GTClipboard::text();

    QPlainTextEdit* logTextEdit = logView->findChild<QPlainTextEdit*>();
    CHECK_SET_ERR(nullptr != logTextEdit, "Log view text edit field is not found")

    const QString logTextEditContent = logTextEdit->toPlainText();
    CHECK_SET_ERR(logTextEditContent == clipboardContent,
                  QString("Clipboard content differs from what is in the log widget. Clipboard: %1, log widget: %2").arg(clipboardContent).arg(logTextEditContent));
}

GUI_TEST_CLASS_DEFINITION(test_1499) {
    class CustomBuildTreeDialogFiller : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto algorithmBox = GTWidget::findComboBox("algorithmBox", dialog);
            GTComboBox::selectItemByText(algorithmBox, "MrBayes");

            auto saveLineEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(saveLineEdit, sandBoxDir + "1499.nwk");

            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);

            QPushButton* button = box->button(QDialogButtonBox::Ok);
            GTWidget::click(button);
        }
    };

    GTLogTracer lt;
    // Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Close opened project tree view to make all icons on the toolbar visible with no overflow.
    GTUtilsProjectTreeView::toggleView();

    // Click the "Build Tree" button on the toolbar.
    // Choose MrBayes tree building method.
    // Choose "Display tree with alignment editor".
    // Build.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new CustomBuildTreeDialogFiller()));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::TreeOptions);

    // Expected: the tree appears synchronized with the MSA Editor.
    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");
    const QStringList msaSequences0 = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    // Break sync mode by sorting sequences in the MSA.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_name"}));
    GTMenu::showContextMenu(GTUtilsMsaEditor::getSequenceArea());
    GTUtilsDialog::checkNoActiveWaiters();

    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");
    const QStringList msaSequences1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(msaSequences1 != msaSequences0, "MSA is not changed");

    // Click the "Sync" button on the Tree View toolbar.
    // = > UGENE does not crash.
    GTWidget::click(syncModeButton);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");

    const QStringList msaSequences2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(msaSequences0 == msaSequences2, "MSA is not synchronized with tree.");

    // Check that there are no errors in the log.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1506) {
    //    1) Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Click the "Build Tree" button on the toolbar.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/1548.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    //    3) Choose "Display tree with alignment editor".
    //    4) Build.
    //    Expected: the tree appears synchronized with the MSA Editor. Clustering blue line is shown.
    //    5) Click the "Layout" button on the Tree View toolbar and choose the circular or unrooted layout.
    auto layoutCombo = GTWidget::findComboBox("layoutCombo");
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    //    6) Zoom in the tree using mouse scroll.
    GTWidget::click(GTWidget::findWidget("treeView"));
    for (int i = 0; i < 10; i++) {
        GTMouseDriver::scroll(1);
    }
    //    Expected: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_1508) {
    // 1. Open COI2.fa as an alignment
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/COI2.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. {MSA Editor context menu} -> Align -> Align with MUSCLE
    // 3. Choose the mode "Refine only"
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Refine));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1510) {
    //    1. Open workflow sample "Call variants with SAMtools"
    //    2. Substitute "Read assembly (BAM/SAM)" element with "Read Sequence" element
    //    3. Set any input sequences for "Read sequence" elements.
    //    4. Select "Call Variants" element with mouse
    //    5. Set "Source URL (by Read Sequence 1)" as "Source URL" in "Input data" area in workflow editor
    //    6. Run the scheme
    //    Expected state: Error notification appears, UGENE doesn't crash
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::removeItem("Read Assembly (BAM/SAM)");
    WorkflowProcessItem* toBam = GTUtilsWorkflowDesigner::getWorker("To BAM");
    CHECK_SET_ERR(toBam != nullptr, "\'To BAM\' element not found");

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);

    WorkflowProcessItem* readSeq = GTUtilsWorkflowDesigner::getWorker("Read Sequence 1");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence 1"));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(0, -200));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(readSeq != nullptr, "\'Read Sequence 1\' element not found");
    GTUtilsWorkflowDesigner::connect(readSeq, toBam);

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::click("Read Sequence 1");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "/samples/FASTA/human_T1.fa");

    GTUtilsWorkflowDesigner::click("Call Variants");

    QTableWidget* w = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    GTUtilsWorkflowDesigner::setTableValue("Source URL", "Source URL (by Read Sequence 1)", GTUtilsWorkflowDesigner::comboValue, w);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_1511) {
    //    1. Open "data/COI.aln" in MSA view
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select some region in Sequence Area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 2), QPoint(15, 6));
    int numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelectedSequences == 5, "There is no selection in MSA, but expected (check #1)");

    //    3. Press Esc key
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    Expected state: selection is removed
    numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelectedSequences == 0, "There is selection in MSA, but not expected (check #1)");

    //    4. Select some sequences in Name Area
    GTUtilsMsaEditor::selectRows(2, 6);
    numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelectedSequences == 5, "There is no selection in MSA, but expected (check #2)");

    //    5. Press Esc key
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    Expected state: selection is removed
    numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelectedSequences == 0, "There is selection in MSA, but not expected(check #2)");
}

GUI_TEST_CLASS_DEFINITION(test_1514) {
    // Check that zoom-in and zoom-out changes tree view image in MSA editor.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto treeView = GTWidget::findWidget("treeView");
    QAbstractButton* zoomOut = GTAction::button("Zoom Out");
    QAbstractButton* zoomIn = GTAction::button("Zoom In");
    QAbstractButton* resetZoom = GTAction::button("Reset Zoom");

    GTWidget::click(resetZoom);
    QImage initialImage = GTWidget::getImage(treeView);

    // Make a series of 2-step zoom-outs and check that image was changed (reason why 2: fonts may be incomplete, example: size 9 == size 10).
    for (int i = 0; i < 2; i++) {
        QImage imageBefore = GTWidget::getImage(treeView);
        CHECK_SET_ERR(zoomOut->isEnabled(), QString::number(i) + ". Zoom out must be enabled.");
        GTWidget::click(zoomOut);
        CHECK_SET_ERR(zoomOut->isEnabled(), QString::number(i) + ". Zoom out must be enabled.");
        GTWidget::click(zoomOut);
        QImage imageAfter = GTWidget::getImage(treeView);
        CHECK_SET_ERR(imageBefore != imageAfter, QString::number(i) + ". Images are unexpectedly equal on zoom out");
    }

    GTWidget::click(resetZoom);
    QImage resetImage = GTWidget::getImage(treeView);
    //    Expected state: sizes of the tree and alignment reset.
    CHECK_SET_ERR(initialImage.height() == resetImage.height(), "Reset zoom action failed")

    // Make a series of 2-step zoom-ins and check that image was changed (reason why 2: fonts may be incomplete, example: size 9 == size 10).
    for (int i = 0; i < 2; i++) {
        QImage imageBefore = GTWidget::getImage(treeView);
        CHECK_SET_ERR(zoomIn->isEnabled(), QString::number(i) + ". Zoom in must be enabled.");
        GTWidget::click(zoomIn);
        CHECK_SET_ERR(zoomIn->isEnabled(), QString::number(i) + ". Zoom in must be enabled.");
        GTWidget::click(zoomIn);
        QImage imageAfter = GTWidget::getImage(treeView);
        CHECK_SET_ERR(imageBefore != imageAfter, QString::number(i) + ". Images are unexpectedly equal on zoom in");
    }
}

GUI_TEST_CLASS_DEFINITION(test_1515) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Press {build tree} button. Call tree file COI.nwk
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    //    Press {build tree} again. Call tree COI.nwk
    GTUtilsProjectTreeView::doubleClickItem("COI.aln");
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI.nwk"));
    tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    //    UGENE crashes
}

GUI_TEST_CLASS_DEFINITION(test_1527) {
    // 1. Open COI2.aln as an alignment
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. {MSA Editor context menu} -> Align -> Align profile to profile with MUSCLE
    // 3. Select empty "test.aln" in the profile browsing dialog.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/regression/1527/", "test.aln"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_1527_1) {
    // 1. Open COI2.aln as an alignment
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. {MSA Editor context menu} -> Align -> Align sequences to profile with MUSCLE
    // 3. Select empty "test.aln" in the profile browsing dialog.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align sequences to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/regression/1527/", "test.aln"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_1528) {
    //    1. Open "Assembly\chrM.sorted.bam" in UGENE
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Make the "chrM.sorted.bam.ugenedb" read-only

    //    3. Open "chrM.fa" in UGENE

    //    4. Drag and drop "chrM.fa" sequence object to the assembly
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "This action requires changing file:"));
    GTUtilsProjectTreeView::click("chrM", "chrM.fa");
    GTFile::setReadOnly(sandBoxDir + "chrM.sorted.bam.ugenedb");
    GTMenu::clickMainMenuItem({"Actions", "Set reference"});

    //    Expected state: This action requires changing file:
    //    Warning with following text has been appeared "This action requires changing file ..."
}

GUI_TEST_CLASS_DEFINITION(test_1529) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    QFile sourceFile(dataDir + "samples/CLUSTALW/COI.aln");
    sourceFile.copy(sandBoxDir + "COI.aln");

    GTFileDialog::openFile(sandBoxDir, "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    // 2. Use context menu{ Export->Amino translation... }.
    // Expected state : "Export Amino Translation" dialog has appeared.
    // 3. Press the "Export" button.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(10, 10));
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : A new file with MSA was created, added to the project.The MSA object name is "COI_transl.aln".
    QModelIndex docIndex = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait("COI_transl.aln").first();
    int objCount = docIndex.model()->rowCount(docIndex);
    CHECK_SET_ERR(1 == objCount, QString("Unexpected child object count in the project. Expected %1, found %2").arg(1).arg(objCount));
}

GUI_TEST_CLASS_DEFINITION(test_1531) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    // 1. Open "samples/CLUSTALW/COI.aln".
    // 2. Activate the "Statistics" options panel.
    // 3. Click "Show distances column"
    // Expected: the hint about reference absence is shown.
    // 4. Right click "Phaneroptera_falcata" -> Set this sequence as reference.
    // Expected: the hint about reference absence is not shown.
    // 5. Activate the "General" options panel.
    // 6. Click "Clear".
    // 7. Activate the "Statistics" options panel.
    // Expected: the hint about reference absence is shown.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 17), QPoint(0, 17));

    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::Statistics);
    GTWidget::click(GTWidget::findWidget("addSeq"));
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);

    auto warningMessage = GTWidget::findWidget("refSeqWarning");
    CHECK_SET_ERR(warningMessage->isHidden(), QString("Reference sequence warning must be hidden"));

    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::General);
    GTWidget::click(GTWidget::findWidget("deleteSeq"));
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::Statistics);
    warningMessage = GTWidget::findWidget("refSeqWarning");
    CHECK_SET_ERR(warningMessage->isVisible(), QString("Reference sequence warning must be visible"));
}

GUI_TEST_CLASS_DEFINITION(test_1537) {
    //    1. Open "_common_data/clustal/10000_sequences.aln".
    GTFileDialog::openFile(testDir + "_common_data/fasta/PF07724_full_family.fa", GTFileDialog::Open, GTGlobals::UseMouse);
    //    2. Cancel the loading task.
    GTUtilsTaskTreeView::cancelTask("Loading documents");
    //    Expected: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_1548) {
    // Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QStringList originalNameList = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    // Close opened project tree view to make all icons on the toolbar visible with no overflow.
    GTUtilsProjectTreeView::toggleView();

    // Build tree for the alignment
    GTUtilsMsaEditor::buildPhylogeneticTree(testDir + "_common_data/scenarios/sandbox/1548.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::TreeOptions);

    // Ensure that the "Sync" mode is ON and 'Mecopoda_elongata_Sumatra' and 'Mecopoda_elongata_Ishigaki_J' are in the correct order.
    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");

    QStringList syncModeNameList = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(syncModeNameList != originalNameList, "Name list must be updated in sync mode");

    int ishigakiIndex = syncModeNameList.indexOf("Mecopoda_elongata__Ishigaki__J");
    CHECK_SET_ERR(ishigakiIndex == 12, "Wrong order for 'Mecopoda_elongata__Ishigaki__J': " + QString::number(ishigakiIndex));
    int sumatraIndex = syncModeNameList.indexOf("Mecopoda_elongata__Sumatra_");
    CHECK_SET_ERR(sumatraIndex == 13, "Wrong order for 'Mecopoda_elongata__Sumatra_': " + QString::number(sumatraIndex));

    // Disable 'sync' mode for the tree.
    // The name list must keep the current order, because there may be other trees in sync. The current tree must exit the sync mode.
    GTWidget::click(syncModeButton);
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");

    QStringList nameList = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(nameList == originalNameList, "Name list must be restored to the original sync mode is turned OFF.");
}

GUI_TEST_CLASS_DEFINITION(test_1551) {
    //    (Reproduced on MAC OS X)
    //    1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select any sequence.
    GTUtilsMsaEditor::clickSequenceName("Phaneroptera_falcata");

    //    3. Call context menu in the name list widget.
    //    4. Move the mouse cursor away from context menu and press the left mouse button.
    //    5. Press right mouse button on the same place.
    //    Expected state: there is no rename sequence dialog appeared.
    //    GTUtilsDialog::waitForDialogWhichMustNotBeRun(new MessageBoxDialogFiller());
    class Scenario : public CustomScenario {
        void run() {
            GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() - QPoint(5, 0));
            GTMouseDriver::click();
            QWidget* contextMenu = QApplication::activePopupWidget();
            CHECK_SET_ERR(contextMenu == nullptr, "There is an unexpected context menu");
        }
    };

    GTUtilsDialog::waitForDialog(new PopupChecker(new Scenario()));
    GTWidget::click(GTUtilsMsaEditor::getNameListArea(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_1554) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click a "Build Tree" button on the toolbar.
    //    Expected state: a "Build Phylogenetic Tree" dialog appears.
    //    3. Set the out file location and click a "Build" button.
    //    Expected state: a tree view synchronized with msa appears.
    QDir().mkpath(sandBoxDir + "test_1554");
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_1554/COI.nwk", 0, 0, true));
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Call a context menu of the tree view by the right mouse button clicking.
    //    Expected state: Tree view contect menu appears.
    GTWidget::click(GTUtilsMsaEditor::getTreeView(), Qt::RightButton);
    QWidget* contextMenu = QApplication::activePopupWidget();
    CHECK_SET_ERR(nullptr != contextMenu, "There is no expected context menu");

    //    5. Click somewhere on the tree view to close menu.
    //    Expected state: The context menu closes, there are not any another menus.
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() - QPoint(5, 0));
    GTMouseDriver::click(Qt::LeftButton);
    contextMenu = QApplication::activePopupWidget();
    CHECK_SET_ERR(contextMenu == nullptr, "There is an unexpected context menu");
}

GUI_TEST_CLASS_DEFINITION(test_1560) {
    //    1. Open "test/_common_data/regression/1560/toy_ref.fa" in UGENE
    GTFileDialog::openFile(testDir + "_common_data/regression/1560/toy_ref.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "test/_common_data/regression/1560/toy1.bam" (convert to ugenedb)
    QString ugenedbFileName = testDir + "_common_data/scenarios/sandbox/test_1560.ugenedb";

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(ugenedbFileName));
    GTFileDialog::openFile(testDir + "_common_data/regression/1560/toy1.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Add "toy_ref.fa" as a reference sequence to "toy1.bam"
    GTUtilsAssemblyBrowser::addRefFromProject("ref2");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Current state: UGENE crashes
    GTUtilsAssemblyBrowser::hasReference();
}

GUI_TEST_CLASS_DEFINITION(test_1567) {
    //    1. Start WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open the "RNA-seq analysis with Tuxedo tools" sample.

    class WizardScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QString tophat = GTUtilsWizard::getParameter("Tophat").toString();
            QString cufflinks = GTUtilsWizard::getParameter("Cufflinks").toString();
            QString cuffmerge = GTUtilsWizard::getParameter("Cuffmerge").toString();
            QString cuffdiff = GTUtilsWizard::getParameter("Cuffdiff").toString();

            CHECK_SET_ERR(tophat == "tools_output", "unexpected tophat value: " + tophat);
            CHECK_SET_ERR(cufflinks == "tools_output", "unexpected cufflinks value: " + cufflinks);
            CHECK_SET_ERR(cuffmerge == "tools_output", "unexpected cuffmerge value: " + cuffmerge);
            CHECK_SET_ERR(cuffdiff == "tools_output", "unexpected cuffdiff value: " + cuffdiff);

            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Paired-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new WizardScenario()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    Expected state: a wizard for the scheme appears.

    //    3. Click the "Setup" button, then click the  "Next" button until tou get to the last page: "Output data".
    //    Expected state: the last page has four lineedits, every lineedit has predefined value ("tools_output").
}

GUI_TEST_CLASS_DEFINITION(test_1568) {
    //    1. Open "COI.aln".
    //    2. Add existing tree or build tree and display it in MSAEditor.
    //    3. Close MSAEditor.
    //    4. Unload document with tree.
    //    5. Open "COI.aln" from the Project View.
    //    6. Open document with tree.
    //    Expected state: UGENE doesn't crash.
    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_1568.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button("Build Tree");
    CHECK_SET_ERR(tree != nullptr, "Build Tree action not found");
    GTWidget::click(tree);

    GTUtilsMdi::closeWindow("COI [COI.aln]");

    GTUtilsDocument::unloadDocument("test_1568.nwk", false);
    GTUtilsProjectTreeView::doubleClickItem("COI.aln");

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1573) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select some sequences in the NameList area.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 2), QPoint(2, 6));

    // 3. Click to any sequence in this selection (in the NameList area).
    GTUtilsMSAEditorSequenceArea::selectSequence("Montana_montana");

    // Expected state: only one sequence is selected (the clicked one).
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(0, 4), QPoint(11, 4)));
}

GUI_TEST_CLASS_DEFINITION(test_1574) {
    //    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Turn on the collapsing mode with the "Switch on/off collapsing" button on the toolbar.
    //    Expected state: there are two collapsed groups.
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    3. Try to select some area in the Sequence area (selection start point must be in the white space under sequences).
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 15), QPoint(2, 0), GTGlobals::UseMouse);

    //    Expected state: A region from the alignment bottom to the selection end point is selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(2, 0), QPoint(2, 13)));

    //    4. Try to click to the white space under sequences.
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 15));

    //    Expected state: Nothing is selected (see UGENE-6654).
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(0, 0), QPoint(-1, -1)));

    //    5. Try to select some area in the NameList area (selection must start from the next row under the last row).
    GTUtilsMsaEditor::selectRows(14, 10, GTGlobals::UseMouse);

    //    Expected state: A region from the alignment bottom to the selection end point is selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(0, 10), QPoint(11, 13)));

    //    6. Try to select some area in the NameList area (selection must start from the bottom of widget).
    GTUtilsMsaEditor::selectRows(30, 10, GTGlobals::UseMouse);

    //    Expected state: A region from the alignment bottom to the selection end point is selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(0, 10), QPoint(11, 13)));
}

GUI_TEST_CLASS_DEFINITION(test_1575) {
    //    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click the "Enable collapsing" button on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    3. Open any group and try to edit any sequence:
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Conocephalus_discolor");

    //    3.1 Insert gap by pressing SPACE.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 10));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    //    Expected state: gap was inserted in every sequence of this group.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 10), QPoint(0, 12));
    GTKeyboardUtils::copy();
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "-\nT\nT", "Unexpected selection: " + clipboardText);

    //    3.2 Select some region of the grouped sequences in the Sequence area and drag this selection to the right.
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 11));
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection(QPoint(2, 11), QPoint(3, 11));

    //    Expected state: all sequences in the group are changed simultaneously.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 10), QPoint(2, 12));
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "T\n-\nA", "Unexpected selection 2: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_1576) {
    // 1. Open {_common_data/scenarios/regression/1576/test.uwl}.
    // Expected state: the scema doesn't loaded, an error is in the log:
    // "Cannot bind convert-alignment-to-sequence:out-sequence to sequences-to-msa:in-sequence"
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1576", "test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
    QString error = lt.getJoinedErrorString();
    QString expectedError = "Cannot bind convert-alignment-to-sequence:out-sequence to sequences-to-msa:in-sequence";

    CHECK_SET_ERR(error.contains(expectedError), "actual error is " + error);
}
GUI_TEST_CLASS_DEFINITION(test_1576_1) {
    // 1. Open {_common_data/scenarios/regression/1576/test2.uwl}.
    // Expected state: the scema doesn't loaded, an error is in the log:
    // "Cannot bind sequences-to-msa:out-msa to convert-alignment-to-sequence:in-msa"
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1576", "test2.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
    QString error = lt.getJoinedErrorString();
    QString expectedError = "Cannot bind sequences-to-msa:out-msa to convert-alignment-to-sequence:in-msa";

    CHECK_SET_ERR(error.contains(expectedError), "actual error is " + error);
}

GUI_TEST_CLASS_DEFINITION(test_1584) {
    //    1. Check the first line of "_common_data/genbank/pBR322.gb".
    //    Expected state:
    //    LOCUS       SYNPBR322               4361 bp    DNA     circular SYN 30-SEP-2008
    //    Look to the spaces between "bp" - "DNA" and "DNA" - "circular". There are must be 4 and 5 spaces by specification.
    //    2. Open this file in UGENE.
    //    3. Add any annotation.
    //    Expected state: new annotation was added.
    //    4. Export this document as genbank file.
    //    5. Check the first line of the exported file.
    //    Expected state: the first line of the exported file is the same as in original file.

    QFile f1(testDir + "_common_data/genbank/pBR322.gb");
    f1.open(QIODevice::ReadOnly);
    QByteArray firstLine = f1.read(64);  // after 64 position the date of file modification is located,
                                         // so meaningfull part is before it
    f1.close();

    GTFileDialog::openFile(testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::createAnnotation("group", "annotation", "1..3", false);

    GTUtilsDialog::add(new PopupChooser({"Export document"}));
    GTUtilsDialog::add(new ExportDocumentDialogFiller(sandBoxDir, "test_1584.gb"));
    GTUtilsProjectTreeView::click("pBR322.gb", Qt::RightButton);

    QFile f2(sandBoxDir + "test_1584.gb");
    f2.open(QIODevice::ReadOnly);
    QByteArray exportedFirstLine = f2.read(64);

    CHECK_SET_ERR(firstLine == exportedFirstLine, QString("First line had been changed! Expected: '%1'. Current: '%2'").arg(firstLine.data()).arg(exportedFirstLine.data()));
    f2.close();
}

GUI_TEST_CLASS_DEFINITION(test_1585) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Enable collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select a sequence area including collapsed rows, sequences above and below them.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 9), QPoint(10, 12));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    const QString selection1 = GTClipboard::text();

    // 4. Shift selected region.
    // Expected state : all sequences shifted simultaneously.If group is half - selected, the unselected sequences shifts too.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(7, 10));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(10, 10));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(6, 9), QPoint(13, 12), selection1);

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Conocephalus_discolor");
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(6, 9), QPoint(11, 10));

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(9, 9));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(11, 9));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(8, 9), QPoint(13, 10), "GTCTAT\nGCTTAT");

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(6, 11), QPoint(14, 12));
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(6, 11), QPoint(14, 12), "GCTTATTAA\nGCTTATTAA");
}

GUI_TEST_CLASS_DEFINITION(test_1586) {
    //    1. Open file test/_common_data/scenarios/msa/ma2_gapped.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Save the initial content
    const QStringList originalMsa = GTUtilsMsaEditor::getWholeData();

    //    2. Use context menu {align->align with MUSCLE}
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Default));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Align", "Align with MUSCLE…"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. press undo toolbar button
    GTUtilsMsaEditor::undo();
    GTThread::waitForMainThread();

    //    Expected state: alignment is similar to initial
    //    Bug state: alignments are not similar
    // Deselect alignment
    GTUtilsMsaEditor::clearSelection();

    // Check the undone state
    const QStringList undoneMsa = GTUtilsMsaEditor::getWholeData();
    CHECK_SET_ERR(undoneMsa == originalMsa,
                  "Undo works wrong. Found text is:\n" + undoneMsa.join("\n"));
}

GUI_TEST_CLASS_DEFINITION(test_1587) {
    /*  1. Open WD and create Read sequence -> Write sequence scheme
    2. Set any output file
    3. Use human_t1 and any image file
    4. Run the scheme

    Expected state: human_t1 is written to the output file, there is a message in log about unsupported doucment format for the image file. The scheme is finished with successful report
    Bug state: the scheme is finished with the error report
*/
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* reader = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);
    GTUtilsWorkflowDesigner::connect(reader, writer);

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", testDir + "_common_data/regression/1587/some_image.png");
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");

    GTUtilsWorkflowDesigner::click("Write Sequence");
    QFile outputFile(sandBoxDir + "out.fa");
    const QString outputFilePath = QFileInfo(outputFile).absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasMessage("Unsupported document format"), "The image file has been processed by Workflow Designer");
    CHECK_SET_ERR(outputFile.exists() && outputFile.size() > 0, "Workflow output file is invalid");
}

GUI_TEST_CLASS_DEFINITION(test_1595) {
    //    The scenario is the following:
    //    1) Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Add "Read sequence" to the scene.
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    //    3) Click it.
    //    Expected: datasets widget appears.
    //    4) Add several files.
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");
    //    5) Select some added items in the list.
    auto datasetWidget = GTWidget::findWidget("DatasetWidget");
    auto items = GTWidget::findListWidget("itemsArea", datasetWidget);
    GTListWidget::click(items, "sars.gb");
    //    6) Press Delete button.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    Expected: the selected files has been removed from the list.
    GTUtilsWorkflowDesigner::click("Read Sequence");
    //    Actual: the element is removed from the scene.
}

GUI_TEST_CLASS_DEFINITION(test_1597) {
    // 1. Open a sequence in UGENE.
    // 2. Input a pattern and search for it.
    // 3. Select "Create new table" in the "Save annotations(s)" group. You may also modify the pattern. Search again.
    // Expected result: the new table is created and found annotations are written to it.

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("ACAATGTATGCCTCTTGGTTTCTTCTATC");

    auto obj = GTWidget::findLabel("ArrowHeader_Save annotation(s) to");
    GTWidget::click(obj);

    auto newTable = GTWidget::findRadioButton("rbCreateNewTable");
    GTWidget::click(newTable);

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("ACGTCGTCGTCGTCAATGTATGCCTCTTGGTTTCTTCTATC");
}

GUI_TEST_CLASS_DEFINITION(test_1600_1) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off")
    //    3. Choose in MSA context menu { Edit -> Remove columns of gaps... }
    GTUtilsDialog::waitForDialog(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Number, 3));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    //    Expected state: The "Remove columns of gaps" dialog has appeared

    //    4. Check the "Remove columns with number of gaps" radio button and set "3" in the appropriate spinbox

    //    5. Press the "Remove" button
    //    Expected state: Four columns have been removed from alignment, collapsing mode has been switched off
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(seq == "AAGCTTCTTT", "unexpected sequence: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == false, "collapsing mode is unexpectidly on");
}

GUI_TEST_CLASS_DEFINITION(test_1600_2) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off")
    //    3. Choose in MSA context menu { Edit -> Remove columns of gaps... }
    GTUtilsDialog::waitForDialog(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Percent, 30));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    //    Expected state: The "Remove columns of gaps" dialog has appeared

    //    4. Check the "Remove columns with percentage of gaps" radio button and set "30" in the appropriate spinbox

    //    5. Press the "Remove" button
    //    Expected state: Four columns have been removed from alignment, collapsing mode has been switched off
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(seq == "AAGCTTCTTT", "unexpected sequence: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == false, "collapsing mode is unexpectidly on");
}

GUI_TEST_CLASS_DEFINITION(test_1600_3) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");
    //    3. Choose in MSA context menu { Edit -> Remove columns of gaps... }
    GTUtilsDialog::waitForDialog(new RemoveGapColsDialogFiller(RemoveGapColsDialogFiller::Column));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "remove_columns_of_gaps"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    //    Expected state: The "Remove columns of gaps" dialog has appeared

    //    4. Check the "Remove all gap-only columns" radio button

    //    5. Press the "Remove" button
    //    Expected state: One column has been removed from alignment, collapsing mode has been switched off
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(seq == "AAGCTTCTTTTAA", "unexpected sequence: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == false, "collapsing mode is unexpectidly on");
}

GUI_TEST_CLASS_DEFINITION(test_1600_4) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");
    //    Expected state: One collapsible item has appeared in MSA
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    //    3. Choose in MSA context menu { Edit -> Remove all gaps }
    //    Expected state: All internal gaps have been removed from alignment, collapsing mode should be on as before removing gap
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Isophya_altaica_EF540820");
    CHECK_SET_ERR(seq == "AAGTTACTAA---", "unexpected sequence1: " + seq);
    QString seq1 = GTUtilsMSAEditorSequenceArea::getSequenceData("Podisma_sapporensis");
    CHECK_SET_ERR(seq1 == "AAGAATAATTA--", "unexpected sequence2: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");
}

GUI_TEST_CLASS_DEFINITION(test_1600_5) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");
    //    Expected state: One collapsible item has appeared in MSA

    //    3. Choose some sequence by left mouse button
    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, 1));
    //    4. Use the context menu in the name list area { Edit -> Remove sequence }
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove sequence"}));
    GTMouseDriver::click(Qt::RightButton);
    //    Expected state: The chosen sequence has been removed from alignment, collapsing mode has been switched on
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    int num = names.size();
    CHECK_SET_ERR(num == 9, QString("unexpected sequence number: %1").arg(num));
    CHECK_SET_ERR(!names.contains("Isophya_altaica_EF540820"), "Isophya_altaica_EF540820 was not removed");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off")
}

GUI_TEST_CLASS_DEFINITION(test_1600_6) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");
    //    Expected state: One collapsible item has appeared in MSA

    //    3. Choose in MSA context menu { Align -> Align with MUSCLE… }.
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller());

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: The "Align with MUSCLE" dialog has been appeared

    //    4. Press the "Align" button

    //    Expected state: Alignment has been changed, collapsing mode has been switched off
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(seq == "AAGCTTCTTTTAA", "unexpected sequence1: " + seq);
    QString seq1 = GTUtilsMSAEditorSequenceArea::getSequenceData("Isophya_altaica_EF540820");
    CHECK_SET_ERR(seq1 == "AAG---TTACTAA", "unexpected sequence2: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == false, "collapsing mode is unexpectidly on");
}

GUI_TEST_CLASS_DEFINITION(test_1600_7) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    Expected state: One collapsible item has appeared in MSA
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");

    //    3. Select some area in MSA by left mouse button and press "Space" button
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    Expected state: New gaps have been added, collapsible item has retained
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(seq == "-AAG-CTTCTTTTAA", "unexpected sequence1: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off 1");

    //    4. Select some area in MSA by left mouse button, then drag the area to the right by mouse
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 1));
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection(QPoint(0, 1), QPoint(1, 1));

    //    Expected state: New gaps have been added, collapsible item has retained

    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Isophya_altaica_EF540820");
    CHECK_SET_ERR(seq == "-AAG-TTACTAA---", "unexpected sequence1: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off 2");
}

GUI_TEST_CLASS_DEFINITION(test_1600_8) {
    //    1. Open file "test/_common_data/scenarios/msa/ma2_gap_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Turn the collapsing mode on by the "Switch on/off collapsing" button on the main toolbar
    GTUtilsMsaEditor::toggleCollapsingMode();

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off");
    //    Expected state: One collapsible item has appeared in MSA

    //    3. Select some area in MSA by left mouse button, then press "Del" button
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    Expected state: Selected area has been removed, collapsible item has retained
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    QString seq = GTUtilsMSAEditorSequenceArea::getSequenceData("Phaneroptera_falcata");
    CHECK_SET_ERR(seq == "AG-CTTCTTTTAA-", "unexpected sequence1: " + seq);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::collapsingMode() == true, "collapsing mode is unexpectidly off 1");
}

GUI_TEST_CLASS_DEFINITION(test_1603) {
    // 1. Open "_data/samples/CLUSTALW/COI.aln"
    // 2. Add a tree to the alignment by creating a new one
    // 3. Close MSA View
    // 4. Unload tree from project
    // 5. Open "COI.aln" from the Project View
    // Expected state: MSA Editor opens with the Tree View inside

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/1603.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::closeWindow("COI [COI.aln]");
    QString docName = "1603.nwk";

    GTUtilsDocument::unloadDocument(docName, false);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::doubleClick();
    // TODO: check the expected state
}

GUI_TEST_CLASS_DEFINITION(test_1606) {
    //    1.  File->Access remote database...
    //    2.  Fill    "Resource ID": 1ezg
    //                "Database": PDB
    //        Open.
    //    3.  Build SAS molecular surface.
    //    4.  Close UGENE.
    //    Expected state: UGENE doesn't crash on closing.
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("1ezg", 3, true, true, false, sandBoxDir));
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooser({"Molecular Surface", "SES"}));
    auto widget3d = GTWidget::findWidget("1-1EZG");
    GTWidget::click(widget3d, Qt::RightButton);
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1607) {
    GTLogTracer lt;
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Create schema read variations->write variants
    WorkflowProcessItem* reader = GTUtilsWorkflowDesigner::addElement("Read Variants");
    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::addElement("Write Variants");
    GTUtilsWorkflowDesigner::connect(reader, writer);

    // 3. Use input file "_common_data/vcf/correct_chr_name.vcf"
    GTUtilsWorkflowDesigner::addInputFile("Read Variants", testDir + "_common_data/vcf/correct_chr_name.vcf");

    GTUtilsWorkflowDesigner::click("Write Variants");
    QFile outputFile(sandBoxDir + "out.vcf");
    const QString outputFilePath = QFileInfo(outputFile).absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    // 4. Run schema
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : output file not empty
    CHECK_SET_ERR(outputFile.exists() && outputFile.size() > 0, "Workflow output file is invalid");
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    outputFile.remove();
}

GUI_TEST_CLASS_DEFINITION(test_1609) {
    // 1) Open any file in UGENE
    QFile::copy(dataDir + "samples/FASTA/human_T1.fa", sandBoxDir + "human_T1.fa");
    GTFileDialog::openFile(sandBoxDir, "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CustomFileDialogUtils : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
            GTWidget::click(GTWidget::findButtonByText("Cancel", dialog));
        }
    };

    // 2) Delete that file from the file system
    // 3) Press "Yes" in appeared UGENE "Do you wish to save" dialog
    // 4) Press "Cancel" in appeared "Save as" dialog
    // Expected state : "Do you wish to save" dialog appeared
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsDialog::add(new GTFileDialogUtils(new CustomFileDialogUtils()));
    QFile::remove(sandBoxDir + "human_T1.fa");
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_1616) {
    // 1. Open "COI.aln".
    // 2. Select any symbol in the last sequence.
    // 3. Click the "Switch on\off collapsing" button on the toolbar.
    // Expected state: UGENE not crashes

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 17), QPoint(0, 17));

    GTUtilsMsaEditor::toggleCollapsingMode();
    GTUtilsMsaEditor::toggleCollapsingMode();
}

GUI_TEST_CLASS_DEFINITION(test_1622) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Save the initial content
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialContent = GTClipboard::text();

    // 2.1. Remove selection
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect());

    // 3. Select a region in the sequence area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 4), QPoint(10, 12));

    // 4. Shift the region
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(6, 7), QPoint(11, 7));

    // 5. Obtain undo button
    QAbstractButton* undo = GTAction::button("msa_action_undo");

    // 6. Undo shifting, e.g. alignment should restore to the init state
    GTWidget::click(undo);

    // 7. Check the undone state
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString undoneContent = GTClipboard::text();
    CHECK_SET_ERR(undoneContent == initialContent,
                  "Undo works wrong. Found text is: " + undoneContent);
}

GUI_TEST_CLASS_DEFINITION(test_1626) {
    // TEST UPDATED: for now it is impossible to run KALIGN on raw alphabet alignment
    // 1. Open file _common_data/regression/1626/1626.fa
    GTFileDialog::openFile(testDir + "_common_data/regression/1626", "1626.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Change color scheme
    // Expected state: color scheme changed successfully
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Tailor"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_1627) {
    GTLogTracer lt;

    //    1. Select {Tools->Build dotplot...} in the main menu.
    //    Expected state: the "Build dotplot from the sequences" dialog appeared.

    //    2. Fill next fields of the dialog and click the "Next" button:
    //        {File with first sequence:} _common_data/scenarios/dp_view/dpm1.fa
    //        {File with second sequence:} _common_data/scenarios/dp_view/dpm2.fa
    //    Expected state: the dotplot settings dialog appeared.
    GTUtilsDialog::add(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dpm1.fa", testDir + "_common_data/scenarios/dp_view/dpm2.fa"));

    //    3. Set next fields of the "Dotplot" dilog and click the "OK" button:
    //        {Minimum repeat length} 8bp
    //        {Repeats identity} 80%
    GTUtilsDialog::add(new DotPlotFiller(8, 80));

    //    Expected state: dotplot appeared, there is not any errors in the log window.
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1628) {
    // CORRECT DISPLAYUNG OF RUSSIAN LATTERS CAN NOT BE TESTED

    //    1. Open COI.aln
    //    2. Renames any row in an alignment, use a non-english characters in the alignment name.
    //    3. Undo and redo.
    //    Expected state: renamed named with name given at step 2

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::renameSequence("Montana_montana", "йцукен123");

    QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undo != nullptr, "Undo button is NULL");
    GTWidget::click(undo);

    QAbstractButton* redo = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(redo != nullptr, "Redo button is NULL");
    GTWidget::click(redo);

    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(names.contains("йцукен123") && !names.contains("Montana_montana"), "Undo-redo worked incorrectly");
}

GUI_TEST_CLASS_DEFINITION(test_1629) {
    // 1. Open UGENE.
    // 2. Close currect project if it is opened.
    // 3. Click the menu item "File".
    // Expected: "Open As" sub-item exists and is available to click.
    GTMenu::checkMainMenuItemState({"File", "Open as..."}, PopupChecker::IsEnabled);
}

GUI_TEST_CLASS_DEFINITION(test_1631) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "Tree Settings" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);

    //    3. Click "Open tree" button.
    //    4. Select "data/samples/Newick/COI.nwk".
    //    Expected state: the tree is added to the MSA Editor.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Newick", "COI.nwk"));
    GTWidget::click(GTWidget::findWidget("openTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::getTreeView();

    //    5. Reopen the view.
    //    Expected state: the tree is still shown with the msa.
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsProjectTreeView::doubleClickItem("COI");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::getTreeView();
}

GUI_TEST_CLASS_DEFINITION(test_1640) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click the MSA Editor.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(4, 3), QPoint(4, 3));

    // 3. Press ESCAPE arrow to remove the selection.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 4. Press and hold a bit shift+right arrow.
    // Qt::Key_Shift
    for (int i = 0; i < 12; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right, Qt::ShiftModifier);
    }

    // Expected state: all sequences of each selected column are selected
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString chars = GTClipboard::text();
    CHECK_SET_ERR(chars == "TCTATTAA", "Wrong selection: " + QString("Wrong selection : %1").arg(chars));
}

GUI_TEST_CLASS_DEFINITION(test_1643) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Do not select any sequence, click the right mouse button at the name list area (or at the sequence area).
    //    Expected state: context menu appeared. It contains an item "Set this sequence as reference" and does not contain "unset the reference sequence".

    //    3. Select the "Set this sequence as reference" item in the context manu.
    //    Expected state: the clicked in the 2 step sequence was set as reference.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Set this sequence as reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(5, 5));
    QString currentReference = GTUtilsMsaEditor::getReferenceSequenceName();
    CHECK_SET_ERR("Metrioptera_japonica_EF540831" == currentReference, QString("An unexpected reference sequence is set: expect '%1', got '%2'").arg("Metrioptera_japonica_EF540831").arg(currentReference));

    //    4. Select any sequence. Call context menu on any another sequence (not on the reference).
    //    Expected sate: the content menu contains both set and unset reference items.
    GTUtilsDialog::waitForDialog(new PopupChecker({"unset_reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 6));
    GTUtilsDialog::waitForDialog(new PopupChecker({"set_seq_as_reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 6));

    //    5. Select the "Set this sequence as reference" item in the context manu.
    //    Expected state: the clicked in the 4 step sequence was set as reference (not the selected).
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Set this sequence as reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 6));
    currentReference = GTUtilsMsaEditor::getReferenceSequenceName();
    CHECK_SET_ERR("Gampsocleis_sedakovii_EF540828" == currentReference, QString("An unexpected reference sequence is set: expect '%1', got '%2'").arg("Gampsocleis_sedakovii_EF540828").arg(currentReference));

    //    6. Call context menu below all sequences, on the white space.
    //    Expected state: there is the "unset" item on the context menu, but no "set" item.
    GTUtilsDialog::waitForDialog(new PopupChecker({"set_seq_as_reference"}, PopupChecker::NotExists));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 20));
    GTUtilsDialog::waitForDialog(new PopupChecker({"unset_reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 20));

    //    7. Select the "unset" menu item.
    //    Expected state: There is no reference sequences, context menu does contains the "set" item, but does not contain the "unset" item.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Unset reference sequence"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 20));

    //    8. Call context menu below all sequences, on the white space.
    //    Expected state: there is neither "set" no "unset" items on the context menu.
    GTUtilsDialog::waitForDialog(new PopupChecker({"set_seq_as_reference"}, PopupChecker::NotExists));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 20));
    GTUtilsDialog::waitForDialog(new PopupChecker({"unset_reference"}, PopupChecker::NotExists));
    GTUtilsMSAEditorSequenceArea::callContextMenu(QPoint(6, 20));
}

GUI_TEST_CLASS_DEFINITION(test_1645) {
    // 1. Open "_common_data/fasta/base_ext_nucl_all_symb.fa".

    // 2. Select the "Join sequences into alignment and open in multiple alignment viewer" option and click "OK" in the appeared "Sequence reading options" dialog.
    // Expected state: The file opens in the MSA Editor.

    // 3. Right click to the loaded document in the project view and select "Unload selected objects" from the context menu.
    // Expected state: The message box appeares.

    // 4. Agree to close an opened view in the appeared message box.
    // Expected state: the document unloads.
    // GTFileDialog::openFile(testDir + "_common_data/fasta/", "base_ext_nucl_all_symb.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/base_ext_nucl_all_symb.fa");
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("base_ext_nucl_all_symb.fa"));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__UNLOAD_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    CHECK_SET_ERR(!GTUtilsDocument::isDocumentLoaded("base_ext_nucl_all_symb.fa"), "Document should't be loaded");
}

GUI_TEST_CLASS_DEFINITION(test_1651) {
    //    1. Open the "Access remote database" dialog.
    class custom : public CustomScenario {
    public:
        void run() {
            GTGlobals::sleep();
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto hintLabel = GTWidget::findLabel("hintLabel", dialog);
            //    2. There are sample IDs in the hints
            GTWidget::clickLabelLink(hintLabel, 20, 6);

            //    3. Clicking on a sample ID must out in in the LineEdit automatically
            auto idLineEdit = GTWidget::findLineEdit("idLineEdit", dialog);
            CHECK_SET_ERR(idLineEdit->text() == "NC_001363", "Unexpected lineEdit text: " + idLineEdit->text());
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new DownloadRemoteFileDialogFiller(new custom()));
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);
}

GUI_TEST_CLASS_DEFINITION(test_1653) {
    /*  1. Open some sequence
    2. Open find pattern option panel
    3. click "Show more options"
    Expected state: Algorithm - exact
    "should match" spinbox is not shown
    Actual: spinbox is shown
*/
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("OP_FIND_PATTERN"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));
    auto spin = GTWidget::findSpinBox("spinBoxMatch");
    CHECK_SET_ERR(spin->isHidden(), "Warning spinbox is unexpectedly visible");
}

GUI_TEST_CLASS_DEFINITION(test_1654) {
    // 1. Open some sequence
    // 2. Copy some subsequence(for example first 10 symbols)
    // 3. Open find pattern option panel
    // 4. Use ctrl+v to paste subsequence to find pattern text area
    // 5. Collapse option panel
    // 6. Expand it and paste subsequence again
    // Expected state: {Search} button is enabled
    // Actual: {Search} button is disabled

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 15));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    GTWidget::click(GTWidget::findWidget("OP_FIND_PATTERN"));

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    auto createAnnotationsButton = GTWidget::findWidget("getAnnotationsPushButton");
    CHECK_SET_ERR(!createAnnotationsButton->isEnabled(), "Create annotations button is enabled!");
}

GUI_TEST_CLASS_DEFINITION(test_1658) {
    //    1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Press the "Build Tree" button on the main toolbar
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/COI_1658.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button("Build Tree");
    GTWidget::click(tree);
    GTUtilsTaskTreeView::waitTaskFinished();
    //    3. Make sure that the "Display tree with alignment editor" button is checked
    //    4. Press the "Build" button
    //    Expected state: tree view appears, it's syncronized with MSA view

    //    5. Close alignment view
    GTUtilsMdi::click(GTGlobals::Close);
    //    6. Delete the created file with tree "data/samples/CLUSTALW/COI.nwk"
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::NoToAll));
    QFile(testDir + "_common_data/scenarios/sandbox/COI_1658.nwk").remove();
    GTGlobals::sleep(5000);
    //    Expected state: Warning dialog appears

    //    7. Press "No to all" button or press "Yes" and save it in another folder

    //    8. Make double click on "COI.aln" item in project view
    GTUtilsProjectTreeView::doubleClickItem("COI.aln");
    GTGlobals::sleep(500);
    //    Expected state: MSA view opens without tree view, no error messages in log appear

    auto treeWidget = GTWidget::findWidget("treeView", nullptr, {false});
    CHECK_SET_ERR(treeWidget == nullptr, "tree view unexpectidly presents");
}

GUI_TEST_CLASS_DEFINITION(test_1660) {
    // 1. Open any sequence
    // 2. Open find pattern option panel
    // 3. Enter any valid pattern
    // 4. Click "Show more options" and select "custom region" in "Region" combobox
    // 5. Delete default value from right edge line edit
    // Expected state: lineedit became red Actual: UGENE crashes

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 15));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // TODO setSearchInregion method checking
}
GUI_TEST_CLASS_DEFINITION(test_1661) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open find pattern option panel
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Enter pattern ACAATGTATGCCTCTTGGTTTCTTCTATC
    GTKeyboardDriver::keySequence("ACAATGTATGCCTCTTGGTTTCTTCTATC");

    // 4. Use settings : Region - custom region; 1 - 10000.
    GTUtilsOptionPanelSequenceView::setRegionType("Custom region");
    GTLineEdit::setText(GTWidget::findLineEdit("editStart"), "1");
    GTLineEdit::setText(GTWidget::findLineEdit("editEnd"), "10000");

    // Expected state : nothing found
    auto resultLabel = GTWidget::findLabel("resultLabel");
    CHECK_SET_ERR(resultLabel->text() == "Results: -/0", "Unexpected find algorithm result count");

    // 5. Use settings : Region - Whole sequence.
    GTUtilsOptionPanelSequenceView::setRegionType("Whole sequence");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : One match found
    CHECK_SET_ERR(resultLabel->text() == "Results: 1/1", "Unexpected find algorithm result count");
}

GUI_TEST_CLASS_DEFINITION(test_1662) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add sample: Multiple dataset tuxedo: Single-end
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index/"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq" << testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq") << (QStringList() << testDir + "_common_data/e_coli/e_coli_reads/e_coli_2_1.fastq" << testDir + "_common_data/e_coli/e_coli_reads/e_coli_2_2.fastq"), map));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Click {show wizard} toolbar button
    //    4. Add several files to each dataset. Fill other fields with proper data
    //    5. Run schema
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: Tophat tool ran 2 times
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);

    GTUtilsDashboard::getExternalToolNode("actor_tophat_run_1_tool_TopHat_run_1");
    GTUtilsDashboard::getExternalToolNode("actor_tophat_run_2_tool_TopHat_run_1");

    int topHatRunCount = GTUtilsDashboard::getExternalToolNodesByText(nullptr, "TopHat run").size();
    CHECK_SET_ERR(topHatRunCount == 2, "Unexpected topHatRuns count. Expected 2, got: " + QString::number(topHatRunCount));
}

GUI_TEST_CLASS_DEFINITION(test_1664) {
    //    1. Open COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Select something in the sequence area.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(5, 5));
    //    3. Try to modify the selection with SHIFT+arrows.
    GTKeyboardDriver::keyClick(Qt::Key_Right, Qt::ShiftModifier);
    //    Expected state: selection changes its size.
    QRect expected = QRect(0, 0, 7, 6);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(expected);
    //    Bug state: UGENE crashes.
}

GUI_TEST_CLASS_DEFINITION(test_1668) {
    //    1) Open "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2) Activate the PWA tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    3) Click "Phaneroptera_falcata" sequence name on the MSA Editor and click the first ">" button
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    //    4) Click "Isophya_altaica_EF540820" sequence name on the MSA Editor and click the second ">" button
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");
    //    5) Deactivate the PWA tab
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    6) Activate the PWA tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: chosen sequences and their order are saved
    QString name1 = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(1);
    QString name2 = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(2);
    CHECK_SET_ERR(name1 == "Phaneroptera_falcata", "unexpected first seq: " + name1);
    CHECK_SET_ERR(name2 == "Isophya_altaica_EF540820", "unexpected second seq: " + name2);
    //    7) Deactivate the PWA tab
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    8) Select some bunch of sequences (more than two) in the name list area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(-5, 2), QPoint(-5, 6), GTGlobals::UseMouse);
    //    9) Activate the PWA tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: previosly chosen two sequences and their order are saved
    name1 = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(1);
    name2 = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(2);
    CHECK_SET_ERR(name1 == "Phaneroptera_falcata", "unexpected first seq: " + name1);
    CHECK_SET_ERR(name2 == "Isophya_altaica_EF540820", "unexpected second seq: " + name2);
    //    10) Deactivate the PWA tab
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    11) Select exactly two sequences in the name list area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(-5, 7), QPoint(-5, 8), GTGlobals::UseMouse);
    //    12) Activate the PWA tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    //    Expected state: last chosen two sequences has appeared on PWA tab
    name1 = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(1);
    name2 = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(2);
    CHECK_SET_ERR(name1 == "Deracantha_deracantoides_EF540", "unexpected first seq: " + name1);
    CHECK_SET_ERR(name2 == "Zychia_baranovi", "unexpected second seq: " + name2);
}

GUI_TEST_CLASS_DEFINITION(test_1672) {
    // 1. Open "COI.aln".
    // 2. Set the "Zychia_baranovi" sequence as reference.
    // 3. Open the "Statistics" options panel tab.
    // 4. Set checked "Show distance algorithm" box, select the "Identity" algorithm.
    // Expected state: the reference sequence has 100% match with itself.
    // Bug state: the reference sequence has 103% match with itself.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("OP_SEQ_STATISTICS_WIDGET"));
    GTUtilsMSAEditorSequenceArea::click(QPoint(-8, 8));
    GTWidget::click(GTWidget::findWidget("addSeq"));
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    auto algoCombo = GTWidget::findComboBox("algoComboBox");
    GTComboBox::selectItemByText(algoCombo, "Similarity");
    QString num1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(8);
    CHECK_SET_ERR(num1 == "100%", "unexpected similarity value an line 1: " + num1);
}

GUI_TEST_CLASS_DEFINITION(test_1673) {
    //    1. Open file COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select a options panel tab header
    //    Expected result: the tab has been opened
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::General);
    bool isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::General);
    CHECK_SET_ERR(isTabOpened, "The 'General' tab is unexpectedly closed");

    //    3. Select the same tab header again
    //    Expected result: the tab has been closed
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::General);
    isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::General);
    CHECK_SET_ERR(!isTabOpened, "The 'General' tab is unexpectedly opened");

    //    4. Select the same tab header again
    //    Expected result: the tab has been opened again
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::General);
    isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::General);
    CHECK_SET_ERR(isTabOpened, "The 'General' tab is unexpectedly closed");
}

GUI_TEST_CLASS_DEFINITION(test_1673_2) {
    //    1. Open file COI.aln
    //    2. Select a options panel tab header
    //    Expected result: the tab has been opened
    //    3. Select different tab
    //    Expected result: only one selected group is shown at a time
    //    4. Select different tabs, holding "Ctrl"
    //    Expected result: only one selected group is shown at a time

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::General);
    bool isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::General);
    CHECK_SET_ERR(isTabOpened, "The 'General' tab is unexpectedly closed");

    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::Statistics);
    isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::General);
    CHECK_SET_ERR(!isTabOpened, "The 'General' tab is unexpectedly opened");
    isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::Statistics);
    CHECK_SET_ERR(isTabOpened, "The 'Statictics' tab is unexpectedly closed");

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::ExportConsensus);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::General);
    CHECK_SET_ERR(!isTabOpened, "The 'General' tab is unexpectedly opened");
    isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::ExportConsensus);
    CHECK_SET_ERR(isTabOpened, "The 'Export Consensus' tab is unexpectedly closed");
}

GUI_TEST_CLASS_DEFINITION(test_1673_3) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Press "Ctrl+F"
    // Expected result: "Search in Sequence" tab has been opened, the pattern field is in focus
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    QWidget* wgt = QApplication::focusWidget();
    auto edit = qobject_cast<QPlainTextEdit*>(wgt);
    CHECK_SET_ERR(edit != nullptr, "Text edit is not in focus");
}

GUI_TEST_CLASS_DEFINITION(test_1673_4) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "Annotations Highlighting" tab of the Options Panel, press "Ctrl+F"
    GTWidget::click(GTWidget::findWidget("OP_ANNOT_HIGHLIGHT"));
    GTWidget::findWidget("prevAnnotationButton");
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // Expected result: "Search in Sequence" tab has been opened, other tabs have been closed
    auto prevButton = GTWidget::findWidget("prevAnnotationButton", nullptr, {false});
    CHECK_SET_ERR(prevButton == nullptr, "Annotations options panel is not closed");
    GTWidget::findWidget("FindPatternWidget");
    CHECK_SET_ERR(GTWidget::findWidget("textPattern")->hasFocus(), "Find pattern field has no focus");
}

GUI_TEST_CLASS_DEFINITION(test_1673_5) {
    //    1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open Sequence View, "Search in Sequence" tab, remove focus from the pattern field, press "Ctrl+F"
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber());
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    //    Expected result: "Search in Sequence" tab is still opened, the pattern field is in focus
    QString name = QApplication::focusWidget()->objectName();
    CHECK_SET_ERR(name == "textPattern", "unexpected focus widget: " + name);
}

namespace {
class customFileDialog_1681 : public GTFileDialogUtils {
public:
    customFileDialog_1681(const QString& path)
        : GTFileDialogUtils(path) {
    }
    void commonScenario() {
        GTFileDialogUtils::commonScenario();
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    }
};
}  // namespace

GUI_TEST_CLASS_DEFINITION(test_1677) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add sample: "Main Tuxedo: Paired-end"
    class customWizard : public CustomScenario {
    public:
        void run() {
            GTGlobals::sleep();
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            GTUtilsDialog::waitForDialog(new customFileDialog_1681(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq"));
            QList<QWidget*> list = wizard->currentPage()->findChildren<QWidget*>("addFileButton");
            foreach (QWidget* w, list) {
                if (w->isVisible()) {
                    GTWidget::click(w);
                    break;
                }
            }

            QMap<QString, QVariant> map;
            map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index/"));
            map.insert("Bowtie index basename", "e_coli");
            map.insert("Bowtie version", "Bowtie1");
            map.insert("Input transcripts annotations", QDir().absoluteFilePath(testDir + "_common_data/e_coli/e_coli_1000.gff"));
            GTUtilsWizard::setAllParameters(map);

            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Paired-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new customWizard()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Press {show widget} toolbutton
    //    Expected state: wizard appears
    //    4. Add file {test/_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq} to {First short  reads files} window
    //    Expected state:Adding e_coli_1_2.fastq offer has appeared

    //    5. Fill all other fields with proper values except second dataset. Press {finish} at the end
    //    6. Press {validate schema} toolbar button
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Validate workflow"));
    //    Expected state: warnings about empty datasets appeared
    GTUtilsWorkflowDesigner::checkErrorList("Dataset");
}

GUI_TEST_CLASS_DEFINITION(test_1680) {
    //    For Mac only

    //    1. Open "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Choose some sequence by mouse in the name list area
    GTUtilsMsaEditor::clickSequenceName("Tettigonia_viridissima");

    //    3. Press "Shift"+"Arrow Up" a few times, then "Shift"+"Arrow Down" a few times
    //    Expected state: The selection changes on every keystroke, for "Arrow Up" it grows to
    //    the top of sequences list, for "Arrow Down" it does to the bottom
    GTKeyboardDriver::keyPress(Qt::Key_Shift);

    int expectedSelectionSize = 1;
    for (int i = 0; i < 9; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Up);
        expectedSelectionSize++;
        const int currentSelectionSize = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
        CHECK_SET_ERR(expectedSelectionSize == currentSelectionSize, QString("An unexpected selection size: expect %1, got %2").arg(expectedSelectionSize).arg(currentSelectionSize));
    }

    {
        GTKeyboardDriver::keyClick(Qt::Key_Up);
        const int currentSelectionSize = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
        CHECK_SET_ERR(10 == currentSelectionSize, QString("An unexpected selection size: expect %1, got %2").arg(10).arg(currentSelectionSize));
    }

    for (int i = 0; i < 9; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        expectedSelectionSize--;
        const int currentSelectionSize = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
        CHECK_SET_ERR(expectedSelectionSize == currentSelectionSize, QString("An unexpected selection size: expect %1, got %2").arg(expectedSelectionSize).arg(currentSelectionSize));
    }

    for (int i = 0; i < 8; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        expectedSelectionSize++;
        const int currentSelectionSize = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
        CHECK_SET_ERR(expectedSelectionSize == currentSelectionSize, QString("An unexpected selection size: expect %1, got %2").arg(expectedSelectionSize).arg(currentSelectionSize));
    }

    {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        const int currentSelectionSize = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
        CHECK_SET_ERR(9 == currentSelectionSize, QString("An unexpected selection size: expect %1, got %2").arg(9).arg(currentSelectionSize));
    }

    {
        GTKeyboardDriver::keyClick(Qt::Key_Up);
        const int currentSelectionSize = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
        CHECK_SET_ERR(8 == currentSelectionSize, QString("An unexpected selection size: expect %1, got %2").arg(8).arg(currentSelectionSize));
    }

    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
}

GUI_TEST_CLASS_DEFINITION(test_1681) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add sample: Multiple dataset tuxedo: Single-end
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    map.insert("Input transcripts annotations", QDir().absoluteFilePath(testDir + "_common_data/e_coli/e_coli_1000.gff"));
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"No-new-transcripts", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq") << (QStringList() << testDir + "_common_data/e_coli/e_coli_reads/e_coli_2_1.fastq"), map));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Click {show wizard} toolbar button
    //    4. Fill wizard with proper data
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "Workflow is valid."));
    GTWidget::click(GTAction::button("Validate workflow"));

    //    5. Run schema
    //    Expected state: Pipeline executed without errors
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1681_1) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add sample: Multiple dataset tuxedo: Paired-end

    class customWizard : public CustomScenario {
    public:
        void run() {
            GTGlobals::sleep();
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            GTUtilsDialog::waitForDialog(new customFileDialog_1681(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq"));
            QList<QWidget*> list = wizard->currentPage()->findChildren<QWidget*>("addFileButton");
            foreach (QWidget* w, list) {
                if (w->isVisible()) {
                    GTWidget::click(w);
                    break;
                }
            }

            QTabWidget* tabWidget = dialog->findChild<QTabWidget*>();
            CHECK_SET_ERR(tabWidget != nullptr, "tabWidget not found");
            GTTabWidget::setCurrentIndex(tabWidget, 1);

            GTUtilsDialog::waitForDialog(new customFileDialog_1681(testDir + "_common_data/e_coli/e_coli_reads/e_coli_2_1.fastq"));
            list = wizard->currentPage()->findChildren<QWidget*>("addFileButton");
            foreach (QWidget* w, list) {
                if (w->isVisible()) {
                    GTWidget::click(w);
                    break;
                }
            }

            QMap<QString, QVariant> map;
            map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index/"));
            map.insert("Bowtie index basename", "e_coli");
            map.insert("Bowtie version", "Bowtie1");
            map.insert("Input transcripts annotations", QDir().absoluteFilePath(testDir + "_common_data/e_coli/e_coli_1000.gff"));
            GTUtilsWizard::setAllParameters(map);

            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"No-new-transcripts", "Paired-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new customWizard()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Click {show wizard} toolbar button
    //    4. Fill wizard with proper data
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "Workflow is valid."));
    GTWidget::click(GTAction::button("Validate workflow"));
    //    5. Run schema
    //    Expected state: Pipeline executed without errors
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1681_2) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add sample: Single dataset tuxedo: Single-end
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index/"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", {testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq"}, map));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Click {show wizard} toolbar button
    //    4. Fill wizard with proper data
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "Workflow is valid."));
    GTWidget::click(GTAction::button("Validate workflow"));

    //    5. Run schema
    //    Expected state: Pipeline executed without errors
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1681_3) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add sample: Single dataset tuxedo: Paired-end
    class customWizard : public CustomScenario {
    public:
        void run() {
            GTGlobals::sleep();
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            GTUtilsDialog::waitForDialog(new customFileDialog_1681(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq"));
            QList<QWidget*> list = wizard->currentPage()->findChildren<QWidget*>("addFileButton");
            foreach (QWidget* w, list) {
                if (w->isVisible()) {
                    GTWidget::click(w);
                    break;
                }
            }

            QMap<QString, QVariant> map;
            map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index/"));
            map.insert("Bowtie index basename", "e_coli");
            map.insert("Bowtie version", "Bowtie1");
            GTUtilsWizard::setAllParameters(map);

            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Paired-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new customWizard()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Click {show wizard} toolbar button
    //    4. Fill wizard with proper data
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "Workflow is valid."));
    GTWidget::click(GTAction::button("Validate workflow"));
    //    5. Run schema
    //    Expected state: Pipeline executed without errors
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1686) {
    //    1. Select { Tools -> Build dotplot... } in the main menu.
    GTUtilsDialog::waitForDialog(new DotPlotFiller(8, 80, false, false));
    Runnable* filler2 = new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dpm1.fa", testDir + "_common_data/scenarios/dp_view/dpm2.fa");
    GTUtilsDialog::waitForDialog(filler2);

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    //    2. Set the "_common_data/scenarios/dp_view/dpm1.fa" as the first sequence,
    //    the "_common_data/scenarios/dp_view/dpm2.fa" as the second sequence.
    //    Make sure that the "Join all sequences found in the file" checkboxes are checked. Click the "Next" button.

    //    Expected state: sequences are loaded into sequence view. "Dotplot" dialog has appeared.

    //    3. Set the {Minimum repeat length} : 200, {Repeats identity} : 50%. Click the "OK" button.

    //    Expected state: dotplot view has appeared.

    //    4. Zoom in to the maximum and move the dotplot screen to the right bottom corner of the dotplot with the minimap.

    //    Expected state: Coords of the right bottom corner is (200, 200).
}

GUI_TEST_CLASS_DEFINITION(test_1687) {
    // 1. open samples / clustalW / COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open 'statistic tab'
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);

    // 3. Be sure here is no reference sequence selected.
    auto refSeqEdit = GTWidget::findLineEdit("sequenceLineEdit");
    CHECK_SET_ERR(refSeqEdit->text().isEmpty(), "Unexpected reference sequence in MSA");

    auto refSeqWarning = GTWidget::findLabel("refSeqWarning");
    CHECK_SET_ERR(refSeqWarning->isHidden(), "Warning label is unexpectedly visible");

    // 4. Set 'Set distance column' checked
    auto check = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(check);

    // Expected state : hint with green text appears at the bottom of the tab.
    CHECK_SET_ERR(refSeqWarning->isVisible(), "Warning label is unexpectedly invisible");
    CHECK_SET_ERR(!refSeqWarning->text().isEmpty(), "Warning label contains no text");
}

GUI_TEST_CLASS_DEFINITION(test_1688) {
    // Open file "_common_data/scenarios/_regression/1688/sr100.000.fa" (100k sequences, open as separate)
    // Expected state: UGENE shows error about too many sequences and does not crash.
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/1688/sr100.000.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::checkNotificationDialogText("contains too many sequences");
}

GUI_TEST_CLASS_DEFINITION(test_1693) {
    //    1. Open WD.
    //    2. Launch tuxedo pipeline. Not actually need 'Tuxedo pipeline', so 'Remote BLASTing' are used.
    //    3. Try to open any other sample while pipeline is running.
    //    Expected state: UGENE doesn't crash.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Remote BLASTing");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Sequence(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    GTUtilsWorkflowDesigner::runWorkflow();

    int tasksCount = GTUtilsTaskTreeView::getTopLevelTasksCount();
    CHECK_SET_ERR(tasksCount == 1, QString("An unexpected top level tasks count: expect %1, got %2. Workflow didn't launch?").arg(1).arg(tasksCount));

    GTUtilsWorkflowDesigner::returnToWorkflow();
    auto samplesWidget = GTWidget::findWidget("samples");
    CHECK_SET_ERR(!samplesWidget->isEnabled(), "Samples widget is unexpectedly enabled");
    GTUtilsTaskTreeView::cancelTask("Execute workflow");
}

GUI_TEST_CLASS_DEFINITION(test_1700) {
    //    1. Open file "https://ugene.net/tracker/secure/attachment/12864/pdb1a07.ent.gz".
    GTFileDialog::openFile(testDir + "_common_data/pdb/", "pdb1a07.ent.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: UGENE displays sequence and 3d structure
    GTUtilsMdi::findWindow("pdb1a07.ent.gz");

    // 2. In context menu of 3d view: {Render style -> Ball-and-Stick}
    GTUtilsDialog::waitForDialog(new PopupChooser({"Render Style", "Ball-and-Stick"}));
    GTMenu::showContextMenu(GTWidget::findWidget("1-1A07"));

    // 3) In context menu of 3d view: {Coloring scheme -> <Any different from current scheme>}
    // Repeat step 3) many times

    bool curSchemeFlag = true;
    for (int i = 0; i < 10; i++) {
        if (curSchemeFlag) {
            GTUtilsDialog::waitForDialog(new PopupChooser({"Coloring Scheme", "Chemical Elements"}));
            curSchemeFlag = false;
        } else {
            GTUtilsDialog::waitForDialog(new PopupChooser({"Coloring Scheme", "Secondary Structure"}));
            curSchemeFlag = true;
        }
        GTMenu::showContextMenu(GTWidget::findWidget("1-1A07"));
    }
}

GUI_TEST_CLASS_DEFINITION(test_1701) {
    // 1. Open 2 PDB files("_common_data/pdb/1A5H.pdb" and "_common_data/pdb/1CF7.pdb")
    // 2. In each of them set {Render style -> Ball and Stick}
    // 3. Close one of the views
    // Expected state: UGENE works fine. The opened view is displayed correctly(no black screen instead of molecule).

    GTFileDialog::openFile(testDir + "_common_data/pdb", "1A5H.pdb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/pdb", "1CF7.pdb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow();
    auto pdb2Widget = GTWidget::findWidget("2-1CF7", sequenceViewWindow);

    // PDB 2 is active -> update 3d rendering settings.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Render Style", "Ball-and-Stick"}));
    GTMenu::showContextMenu(pdb2Widget);

    QImage pdb2ImageBefore = GTWidget::getImage(pdb2Widget);

    // Activate PDB 1 and update 3d rendering settings too.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1A5H.pdb"));
    GTMouseDriver::doubleClick();
    GTThread::waitForMainThread();

    sequenceViewWindow = GTUtilsSequenceView::getActiveSequenceViewWindow();
    auto pdb1Widget = GTWidget::findWidget("1-1A5H");
    GTUtilsDialog::waitForDialog(new PopupChooser({"Render Style", "Ball-and-Stick"}));
    GTMenu::showContextMenu(pdb1Widget);
    // Close PDB 1 view.
    GTMenu::clickMainMenuItem({"Actions", "Close active view"}, GTGlobals::UseKey);
    GTUtilsDialog::checkNoActiveWaiters();
    GTThread::waitForMainThread();

    // Check that PDB 2 image was not changed.
    QImage pdb2ImageAfter = GTWidget::getImage(pdb2Widget);
    CHECK_SET_ERR(pdb2ImageBefore == pdb2ImageAfter, "PDB2 3D image is changed");
}

GUI_TEST_CLASS_DEFINITION(test_1703) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select some row in the name list area
    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, 6));
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 6, 12, 1));

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    // 3. Select the upper row
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 5, 12, 2));

    // 4. Select the bottom row
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 6, 12, 2));
}

GUI_TEST_CLASS_DEFINITION(test_1704) {
    //    1. Open _common_data\_regession\1704\lrr_test_new.gb
    GTFileDialog::openFile(testDir + "_common_data/regression/1704", "lrr_test_new.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Find HMM3 signals with _common_data\_regession\1704\LRR_4.hmm model
    GTUtilsDialog::waitForDialog(new HmmerSearchDialogFiller(testDir + "_common_data/regression/1704/LRR_4.hmm", sandBoxDir + "1704.gb"));

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find HMM signals with HMMER3..."});
    //    3. Export document with annotations into genbank format. Do not add to the project
    GTUtilsDialog::add(new PopupChooser({"Export document"}));
    GTUtilsDialog::add(new ExportDocumentDialogFiller(sandBoxDir, "1704_exported.gb"));
    GTUtilsProjectTreeView::click("1704.gb", Qt::RightButton);
    //    4. Open the exported file with UGENE
    GTFileDialog::openFile(sandBoxDir, "1704_exported.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::doubleClickItem("1704_exported.gb");
    //    Expected state: all the annotations of the new file are identical to annotations of the old file
    GTUtilsAnnotationsTreeView::findItem("hmm_signal  (0, 27)");
    //    Bug state: only the first annotations without qualifiers is opened
}

GUI_TEST_CLASS_DEFINITION(test_1708) {
    // 1. Open COI.aln or HIV-1.aln from samples
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(9, 1));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString initAln = GTClipboard::text();

    QString expectedAln("TAAGACTT-C\n"
                        "TAAG-CTTAC");

    // 2. Align with KAlign
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new KalignDialogFiller(10));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(9, 1));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString changedAln = GTClipboard::text();
    CHECK_SET_ERR(changedAln == expectedAln, "Unexpected alignment\n" + changedAln);

    QAbstractButton* undo = GTAction::button("msa_action_undo");

    // 3. Press Undo
    GTWidget::click(undo);
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(9, 1));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    changedAln = GTClipboard::text();

    CHECK_SET_ERR(changedAln == initAln, "Undo works wrong\n" + changedAln);
}

GUI_TEST_CLASS_DEFINITION(test_1710_1) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Add the "Find Patterns" sample.
    GTUtilsWorkflowDesigner::addSample("Find patterns");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Setup inputs and outputs.
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::click("Find Pattern");
    GTUtilsWorkflowDesigner::setParameter("Pattern(s)", "AAA", GTUtilsWorkflowDesigner::textValue);

    // 4) Press the validate button.
    // Expected state: The message appeared saying "Well done!".
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Well done!"));
    GTWidget::click(GTAction::button("Validate workflow"));
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_1710_2) {
    // 1. Open Settings -> Preferences -> External Tools.
    // 2. Select any invalid file for BLAST -> blastn.
    GTUtilsExternalTools::setToolUrl("blastn", dataDir + "this-file-does-not-exist");

    // 3. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 4. Add the "Gene-by-gene approach" sample.
    GTUtilsWorkflowDesigner::addSample("Gene-by-gene approach");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 5. Setup inputs and outputs.
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::click("Local BLAST Search");
    GTUtilsWorkflowDesigner::setParameter("Database Path", sandBoxDir, GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Database Name", "test", GTUtilsWorkflowDesigner::textValue);

    // 6. Press the validate button.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "non-critical warnings"));
    GTWidget::click(GTAction::button("Validate workflow"));
    GTUtilsDialog::checkNoActiveWaiters();

    // Expected state: there must be a warning "External tool is invalid. UGENE may not support this version of the tool or a wrong path to the tools is selected".
    GTUtilsWorkflowDesigner::checkErrorList("External tool \"blastn\" is invalid. UGENE may not support this version of the tool or a wrong path to the tools is selected");
}

GUI_TEST_CLASS_DEFINITION(test_1720) {
    GTLogTracer lt;

    //    1. Load a remote document
    //    2. Load the document with the same ID again
    //    Expected state: the view of the document is opened
    //    Bug state: error message is shown

    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("D11266", 0));
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsMdi::closeActiveWindow();

    // Load again
    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("D11266", 0));
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("D11266.gb"));
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    // Expected state: project view with document "D11266.gb", no error messages in log appear
}

GUI_TEST_CLASS_DEFINITION(test_1731) {
    // 1. Open \data\samples\CLUSTALW\ty3.aln.gz

    // 2. Select CopCinTy34 as a reference

    // 3. Enable the distances column

    // 4. Select the Identity algorithm
    // Expected state: identity of CopCinTy34 with each sequence is shown in the column

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("OP_SEQ_STATISTICS_WIDGET"));
    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, 5));
    GTWidget::click(GTWidget::findWidget("addSeq"));
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    GTUtilsTaskTreeView::waitTaskFinished(200000);
    QString num1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(1);
    QString num3 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(3);
    CHECK_SET_ERR(num1 != "100%", "unexpected sumilarity value an line 1: " + num1);
    CHECK_SET_ERR(num3 != "100%", "unexpected sumilarity value an line 3: " + num3);
}

GUI_TEST_CLASS_DEFINITION(test_1733) {
    // 1) Run UGENE
    // 2) Open Workflow Designer
    // 3) Open Call Variant Pipeline scheme from the samples
    // 4) Try to specify parameters (using wizard or wd standard interface) with files
    // (for example specify any of file from "data/samples/Genbank" folder as "Bed or position list file" on page #2 of wizard )
    // Expected state: UGENE doesn't ask to specify folder
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.fa");
}

GUI_TEST_CLASS_DEFINITION(test_1734) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Select "Call variants sample", wizard opens automatically

    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bam/small.bam.sorted.bam"));
            GTWidget::click(GTWidget::findWidget("addFileButton", wizard->currentPage()));
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bam/small.bam.sorted.bam"));
            GTWidget::click(GTWidget::findWidget("addFileButton", wizard->currentPage()));
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bam/small.bam.sorted.bam"));
            GTWidget::click(GTWidget::findWidget("addFileButton", wizard->currentPage()));

            auto itemsArea = GTWidget::findListWidget("itemsArea", wizard);
            CHECK_SET_ERR(itemsArea->count() == 3, "unexpected items number");

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new WizardFiller("Call Variants Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    //    Expected state: adding several bams is allowed
}

GUI_TEST_CLASS_DEFINITION(test_1735) {
    //    1. Open Workflow Designer.
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Open﻿"Call variants with SAMtools" sample.
    //    Expected state: the sample is opened, a wizard appears.

    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            //    3. Set "_common_data/cmdline/call-variations/chrM.fa" as reference; "_common_data/bam/chrM.sorted.bam" as input assembly.
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/cmdline/call-variations/chrM.fa"));
            GTWidget::click(GTWidget::findWidget("browseButton", GTWidget::findWidget("Reference sequence file labeledWidget", dialog)));

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bam/chrM.sorted.bam"));
            GTWidget::click(GTWidget::findWidget("addFileButton", wizard->currentPage()));

            //    4. Go to the fourth page of the wizard.
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            //    Expected state: the page title is "SAMtools vcfutils varFilter parameters".
            QString title = GTUtilsWizard::getPageTitle();
            CHECK_SET_ERR(title == "SAMtools <i>vcfutils varFilter</i> parameters", "unexpected title: " + title);

            //    5. Go to the last page of the wizard, click to the "Run" button.
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(new EscapeClicker("Call Variants Wizard"));
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");

    GTUtilsDialog::waitForDialog(new WizardFiller("Call Variants Wizard", new custom()));
    GTToolbar::clickButtonByTooltipOnToolbar("mwtoolbar_activemdi", "Show wizard");

    //    Expected state: there are no errors when this pipeline scheme is running.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1738) {
    // 1. Open WD and load "Call variants with SAMtools" scheme from samples
    // 2. Set files "_common_data/fasta/Mycobacterium.fna" and "_common_data/bam/Mycobacterium.sorted.bam" as input reference
    // and assembly respectively.
    // 2. Run the Call Variants pipeline with standard options
    // 3. Press "Stop scheme" button on the main toolbar
    // Expected state: the pipeline is stopped almost immediately

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/Mycobacterium.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/Mycobacterium.fna");

    GTWidget::click(GTAction::button("Run workflow"));

    GTWidget::click(GTAction::button("Stop workflow"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto timeLabel = GTWidget::findLabel("timeLabel", GTUtilsDashboard::getDashboard());
    CHECK_SET_ERR(timeLabel->text().contains("00:00:0"), "Workflow is not stopped. Execution time is > 10 seconds");
}

GUI_TEST_CLASS_DEFINITION(test_1747) {
    // 1. Open \data\samples\CLUSTALW\ty3.aln.gz
    // 2. Enable the distances column in options panel or create distances matrix by using menu {statistics->Generate distance matrix}
    // Expected state: progress for "Generete distance matrix" correctly displays current state of calculation
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::add(new PopupChooserByText({"Statistics", "Generate distance matrix..."}));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    auto taskProgressBar = GTWidget::findProgressBar("taskProgressBar");
    QString text = taskProgressBar->text();
    CHECK_SET_ERR(text.contains("%"), "unexpected text: " + text);
    text = text.left(text.length() - 1);
    bool isNumber = false;
    int progress = text.toInt(&isNumber);
    CHECK_SET_ERR(isNumber, QString("The progress must be a number: %1").arg(text));
    CHECK_SET_ERR(progress >= 0 && progress <= 100, QString("Incorrect progress: %1").arg(progress));
    int oldProgress = progress;

    GTGlobals::sleep(5000);
    text = taskProgressBar->text();
    CHECK_SET_ERR(text.contains("%"), "unexpected text: " + text);
    text = text.left(text.length() - 1);
    isNumber = false;
    progress = text.toInt(&isNumber);
    CHECK_SET_ERR(isNumber, QString("The progress must be a number: %1").arg(text));
    CHECK_SET_ERR(progress >= 0 && progress <= 100, QString("Incorrect progress: %1").arg(progress));

    CHECK_SET_ERR(progress > oldProgress, "Progress didn't groving up");
}

GUI_TEST_CLASS_DEFINITION(test_1751) {
    // 1. Prepend dot symbol to some valid file name, for instance "data/samples/COI.aln" => "data/samples/.COI.aln"
    // 2. Open it
    GTLogTracer lt;
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/1751/.COI.aln");

    // Expected state: Check log for errors
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1759) {
    //    1. Open file "data/workflow_samples/NGS/tuxedo/tuxedo_main.uwl"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Press "Show wizard" button on the main toolbar

    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            //    2. Go to the second page
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            if (GTUtilsWizard::getPageTitle() != "Tophat settings") {
                GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            }
            auto version = GTWidget::findWidget("Bowtie version widget", dialog);
            CHECK_SET_ERR(version->isVisible(), "version widget is not visiable");

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    // single
    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"No-new-transcripts", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    // paired
    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Paired-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Paired-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"No-new-transcripts", "Paired-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    Expected state: "Tuxedo Wizard" dialog has appeared

    //    3. Press the "Next" button

    //    Expected state: the "TopHat input" groupbox contains the following fields: "Bowtie index folder",
    //    "Bowtie index basename", "Bowtie version".

    //    4. Repeat 2nd and 3rd steps for all the versions of the Tuxedo pipeline
}

GUI_TEST_CLASS_DEFINITION(test_1763_1) {
    //    Improve dashboards: It should be possible to rename a run tab.

    //    1. Create Read->Write workflow.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);
    GTUtilsWorkflowDesigner::connect(read, write);
    //    2. Set any input/output files
    GTUtilsWorkflowDesigner::click(read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");
    //    3. Start workflow
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: Workflow dasboard tab opened.
    auto tabView = GTWidget::findTabWidget("WorkflowTabView");
    //    4. On opened tab click right mouse button
    class custom : public CustomScenario {
    public:
        void run() {
            //    5. Click on "Rename" action
            //    Expected state: Showed "Rename Dashboard" dialog
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            QLineEdit* line = dialog->findChild<QLineEdit*>();
            //    6. Change name and press "Ok" button
            GTLineEdit::setText(line, "new_name");
            GTWidget::click(GTWidget::findButtonByText("Ok", dialog));
        }
    };
    //    Expected state: Showed popup menu with action "Rename"
    GTUtilsDialog::add(new PopupChooserByText({"Rename"}));
    GTUtilsDialog::add(new DefaultDialogFiller("", QDialogButtonBox::Ok, new custom()));
    GTTabWidget::clickTab(tabView, tabView->currentIndex(), Qt::RightButton);
    //    Expected state: Workflow dasboard tab renamed.
    QString newName = GTTabWidget::getTabName(tabView, tabView->currentIndex());
    CHECK_SET_ERR(newName == "new_name", "unexpected tab name: " + newName);
}

GUI_TEST_CLASS_DEFINITION(test_1764) {
    //    1) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Create schema {Read sequence -> Write sequence}
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);
    GTUtilsWorkflowDesigner::connect(read, write);
    //    3) Set input sequence to "human_T1.fa" from "data/samples/FASTA", set output filename to "readed_fasta.fa"
    GTUtilsWorkflowDesigner::click(read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::click(write);
    GTUtilsWorkflowDesigner::setParameter("Output file", "readed_fasta.fa", GTUtilsWorkflowDesigner::textValue);
    //    4) Run workflow, click on dashboard "readed_fasta.fa"
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    QWidget* button = GTWidget::findButtonByText("readed_fasta.fa", GTUtilsDashboard::getDashboard());
    GTWidget::click(button);
    //    Expected state: "readed_fasta.fa" is opened in UGENE
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    5) Click "Return to workflow", repeat step 4
    GTUtilsMdi::activateWindow("Workflow Designer - New workflow");
    GTWidget::click(GTWidget::findButtonByText("To Workflow Designer"));
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    button = GTWidget::findButtonByText("readed_fasta.fa", GTUtilsDashboard::getDashboard());
    GTWidget::click(button);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    //    Expected state: opened fasta files have different file path in tooltips

    QList<QModelIndex> docs = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait("readed_fasta.fa");
    CHECK_SET_ERR(docs.size() == 2, QString("unexpected documents number: %1").arg(docs.size()));
    QString toolTip0 = docs[0].data(Qt::ToolTipRole).toString();
    QString toolTip1 = docs[1].data(Qt::ToolTipRole).toString();

    CHECK_SET_ERR(toolTip0 != toolTip1, "tooltips are equal, first: " + toolTip0 + ", second: " + toolTip1);
}

GUI_TEST_CLASS_DEFINITION(test_1771) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Select tuxedo sample
    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            //    2. Go to the second page
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            //    3. Set bowtie index and a known transcript file.
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bowtie/index/e_coli.1.ebwt"));
            GTWidget::click(GTWidget::findButtonByText("Select\nbowtie index file", dialog));

            QString name = GTUtilsWizard::getParameter("Bowtie index basename").toString();
            QString version = GTUtilsWizard::getParameter("Bowtie version").toString();
            CHECK_SET_ERR(name == "e_coli", "unexpected name: " + name);
            CHECK_SET_ERR(version == "Bowtie1", "unexpected bowtie version: " + version);

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. Select some configuration
    //    Expected state: wizard appeared
    //    4. go to "tophat settings" page. click button "Select bowtie index file"
    //    5. select file _common_data/bowtie/index/e_coli.1.ebwt
    //    Expected state: bowtie index basename is set to "chr6", bowtie version is set to "bowtie1"
}

GUI_TEST_CLASS_DEFINITION(test_1784) {
    // 1. Open murine.gb
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Export it as alignment
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(new ExportSequenceAsAlignmentFiller(testDir + "_common_data/scenarios/sandbox", "test_1784.aln", ExportSequenceAsAlignmentFiller::Clustalw, true));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363"));
    GTMouseDriver::click(Qt::RightButton);
    // 3. Drag and drop one more murine sequence object to the alignment
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("murine.gb", "NC_001363"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from current project..."});
    // 4. Select the first sequence as reference
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTUtilsDialog::waitForDialog(new PopupChooser({"set_seq_as_reference"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 1));
    GTUtilsDialog::waitForDialog(new PopupChooser({"set_seq_as_reference"}));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: The only selected sequence is the reference.
}

GUI_TEST_CLASS_DEFINITION(test_1797) {
    // 1) Open _common_data\scenarios\formats\test_1797 file
    // 2) Choose "BED" format at appeared format dialog
    // Expected state: UGENE doesn't crash - it shows error: "BED parsing error: incorrect number of fields at line 1!"

    GTUtilsDialog::waitForDialog(new SelectDocumentFormatDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/formats/test_1797.svg");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1798) {
    // 1. Open file _common_data\scenarios\_regression\1798\1.4k.aln.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1798", "1.4k.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Start MAFFT with default values.
    GTUtilsDialog::waitForDialog(new MAFFTSupportRunDialogFiller(new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with MAFFT"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // 3. Open Tasks tab
    //    Expected state: check tasks progress percentage is correct
    auto taskProgressBar = GTWidget::findProgressBar("taskProgressBar");
    QString text = taskProgressBar->text();
    CHECK_SET_ERR(text.contains("%"), "unexpected text: " + text);
    text = text.left(text.length() - 1);
    bool isNumber = false;
    int progress = text.toInt(&isNumber);
    CHECK_SET_ERR(isNumber, QString("The progress must be a number: %1").arg(text));
    CHECK_SET_ERR(progress >= 0 && progress <= 100, QString("Incorrect progress: %1").arg(progress));
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_1808) {
    QDir outputDir(testDir + "_common_data/scenarios/sandbox");
    const QString outputFilePath = outputDir.absolutePath() + "/test_1808.gb";

    // 1.Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2.Create schema "Read Annotations"->"Write annotations"
    GTUtilsWorkflowDesigner::addAlgorithm("Read Annotations");
    GTUtilsWorkflowDesigner::addAlgorithm("Write Annotations");
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::getWorker("Read Annotations");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::getWorker("Write Annotations");
    GTUtilsWorkflowDesigner::connect(read, write);

    // 3.Select any input and output file
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Annotations"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Annotations"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    // 4.Validate schema
    // Expected state: No warning about empty input sequence slot
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Well done!"));
    GTWidget::click(GTAction::button("Validate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_1811_1) {
    GTUtilsDialog::add(new RemoteDBDialogFillerDeprecated("A0N8V2", 5));
    GTUtilsDialog::add(new SelectDocumentFormatDialogFiller());
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::isDocumentLoaded("A0N8V2.txt");
}

GUI_TEST_CLASS_DEFINITION(test_1821) {
    QString workflowOutputDirPath(testDir + "_common_data/scenarios/sandbox");
    QDir workflowOutputDir(workflowOutputDirPath);

    // 1. Open WD
    GTUtilsDialog::waitForDialog(new StartupDialogFiller(workflowOutputDir.absolutePath()));
    GTMenu::clickMainMenuItem({"Tools", "Workflow Designer..."});
    GTUtilsMdi::checkWindowIsActive("Workflow Designer");

    // 2. Select "Align sequences with MUSCLE"
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Change the value of the scale spinbox. E.g. set it to 75%
    auto scaleCombo = dynamic_cast<QComboBox*>(GTWidget::findWidget("wdScaleCombo"));
    CHECK_SET_ERR(nullptr != scaleCombo, "Unable to find scale combobox!");
    GTComboBox::selectItemByText(scaleCombo, "75%");

    // 4. Store the scheme to some file using "Save scheme as" button
    GTUtilsDialog::waitForDialog(new WorkflowMetaDialogFiller(workflowOutputDir.absolutePath() + "/" + "test.uwl", "Scheme"));
    GTMenu::clickMainMenuItem({"Actions", "Save workflow"}, GTGlobals::UseKey);

    // 5. Close WD
    GTUtilsMdi::click(GTGlobals::Close);
    GTMouseDriver::click();

    // 6. Open the file containing the saved scheme using "Open" button
    GTFileDialog::openFile(workflowOutputDirPath, "test.uwl");

    // Expected state: scheme is opened in WD, its scale is 75%
    scaleCombo = dynamic_cast<QComboBox*>(GTWidget::findWidget("wdScaleCombo"));
    CHECK_SET_ERR(nullptr != scaleCombo, "Unable to find scale combobox!");
    CHECK_SET_ERR(scaleCombo->currentText() == "75%", "Unexpected scale value!");
}

GUI_TEST_CLASS_DEFINITION(test_1831) {
    // 1) Create a schema with shrunk elements state.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Align with MUSCLE"));
    GTMouseDriver::doubleClick();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write alignment"));
    GTMouseDriver::doubleClick();

    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isWorkerExtended("Align with MUSCLE"), "\"Align with MUSCLE\" unexpectedly has extended style");
    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isWorkerExtended("Write alignment"), "\"Write alignment\" unexpectedly has extended style");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isWorkerExtended("Read alignment"), "\"Read alignment\" unexpectedly has simple style");

    // 2) Save the schema.
    GTUtilsDialog::waitForDialog(new WorkflowMetaDialogFiller(sandBoxDir + "test.uwl", "Workflow"));
    GTMenu::clickMainMenuItem({"Actions", "Save workflow"}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMenu::clickMainMenuItem({"Window", "Close all windows"}, GTGlobals::UseKey);

    // 3) Reopen UGENE WD.
    GTFileDialog::openFile(sandBoxDir, "test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the state is saved.
    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isWorkerExtended("Align with MUSCLE"), "\"Align with MUSCLE\" unexpectedly has extended style");
    CHECK_SET_ERR(!GTUtilsWorkflowDesigner::isWorkerExtended("Write alignment"), "\"Write alignment\" unexpectedly has extended style");
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::isWorkerExtended("Read alignment"), "\"Read alignment\" unexpectedly has simple style");
}

GUI_TEST_CLASS_DEFINITION(test_1834) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Drag the "Read Alignment" and "File Format Conversion" elements to the scene and link them.
    WorkflowProcessItem* reader = GTUtilsWorkflowDesigner::addElement("Read Alignment");
    WorkflowProcessItem* converter = GTUtilsWorkflowDesigner::addElement("File Format Conversion");
    GTUtilsWorkflowDesigner::connect(reader, converter);

    // 3. Set the input file for the "Read Alignment" element to "data/samples/COI.aln".
    GTUtilsWorkflowDesigner::addInputFile("Read Alignment", dataDir + "samples/CLUSTALW/COI.aln");

    // 4. Set the "Document format" parameter of the "File Format Conversion" element to "Mega".
    GTUtilsWorkflowDesigner::click("File Format Conversion");
    GTUtilsWorkflowDesigner::setParameter("Document format", "Mega", GTUtilsWorkflowDesigner::comboValue, GTGlobals::UseMouse);
    GTUtilsWorkflowDesigner::setParameter("Output folder", 0, GTUtilsWorkflowDesigner::comboValue, GTGlobals::UseMouse);
    GTUtilsWorkflowDesigner::setParameter("Custom folder", QDir().absoluteFilePath(sandBoxDir + "regression_1834"), GTUtilsWorkflowDesigner::textValue);

    // 5. Run the scheme.
    GTWidget::click(GTAction::button("Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* button = GTWidget::findButtonByText("COI.aln.meg", GTUtilsDashboard::getDashboard());
    GTWidget::click(button);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("COI.aln.meg");

    // Expected state: Scheme ran successfully, the "COI.aln.mega" output file has appeared on the "Output Files" panel of the dashboard.
    // The output file is valid, might be opened with MSA editor and has the same content as the source file.
    CHECK_SET_ERR(QFile::exists(sandBoxDir + "regression_1834/COI.aln.meg"), "File does not exist");
}

GUI_TEST_CLASS_DEFINITION(test_1859) {
    QString workflowOutputDirPath(testDir + "_common_data/scenarios/sandbox");
    QDir workflowOutputDir(workflowOutputDirPath);
    const QString outputFilePath = workflowOutputDir.absolutePath() + "/test.gb";

    // 1) Open Workflow Designer
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    const QString annReaderName = "Read Annotations";
    const QString annWriterName = "Write Annotations";

    // 2) Build workflow of elements: "Write Annotations" and "Read Annotations"
    GTUtilsWorkflowDesigner::addAlgorithm(annReaderName);
    GTUtilsWorkflowDesigner::addAlgorithm(annWriterName);

    WorkflowProcessItem* annReader = GTUtilsWorkflowDesigner::getWorker(annReaderName);
    WorkflowProcessItem* annWriter = GTUtilsWorkflowDesigner::getWorker(annWriterName);

    GTUtilsWorkflowDesigner::connect(annReader, annWriter);

    // 3) Set output file

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(annWriterName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    // 4) Set input file
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(annReaderName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");

    GTLogTracer lt;

    // 5) Run workflow
    GTWidget::click(GTAction::button("Run workflow"));

    // 6) Block file for writing
    GTFile::setReadOnly(outputFilePath);

    // 7) Run workflow again
    GTWidget::click(GTAction::button("Run workflow"));

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_1860) {
    // 1) Open Workflow Designer
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    const QString textReaderName = "Read Plain Text";
    const QString textWriterName = "Write Plain Text";

    // 2) Add elements: "Write plain text" and "Read plain text"
    GTUtilsWorkflowDesigner::addAlgorithm(textReaderName);
    GTUtilsWorkflowDesigner::addAlgorithm(textWriterName);

    WorkflowProcessItem* textReader = GTUtilsWorkflowDesigner::getWorker(textReaderName);
    WorkflowProcessItem* textWriter = GTUtilsWorkflowDesigner::getWorker(textWriterName);

    // 3) Connect them to each other
    GTUtilsWorkflowDesigner::connect(textReader, textWriter);

    // 4) Try to set output file

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(textWriterName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", "test", GTUtilsWorkflowDesigner::textValue);
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(textReaderName));
    GTMouseDriver::click();
}

GUI_TEST_CLASS_DEFINITION(test_1865) {
    // 1) Open Workflow Designer
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    const QString sequenceWriterName = "Write Sequence";

    // 2) Add elements: "Write sequence"
    GTUtilsWorkflowDesigner::addAlgorithm(sequenceWriterName);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(sequenceWriterName));
    GTMouseDriver::click();
}

GUI_TEST_CLASS_DEFINITION(test_1883) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select any area in sequence view
    const int startRowNumber = 6;
    const int alignmentLength = 12;
    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, startRowNumber));
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, startRowNumber, alignmentLength, 1));
    // 3. Use context menu: "Edit -> Replace selected rows with complement" or "reverse" or "reverse-complement"
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse"}));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: the bases in the selected area were replaced accordingly to the chosen variant
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR("AATTATTAGACT" == selectionContent, "MSA changing is failed");
    // 4. Press "Ctrl + Z"
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    // Expected result: all rows in the selection were restored
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    selectionContent = GTClipboard::text();
    CHECK_SET_ERR("TCAGATTATTAA" == selectionContent, "MSA changing is failed");
}

GUI_TEST_CLASS_DEFINITION(test_1884) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select some row in the name list area
    const int startRowNumber = 6;
    const int alignmentLength = 12;
    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, startRowNumber));
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, startRowNumber, alignmentLength, 1));

    // 3. Select all the upper rows and make some extra "selection"
    const int extraUpperSelectionCount = 3;
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < extraUpperSelectionCount; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Up);
    }
    const int upperSequencesCount = 4;
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, startRowNumber - extraUpperSelectionCount, alignmentLength, upperSequencesCount));

    // 4. Decrease the selection
    const int deselectionCount = extraUpperSelectionCount - 1;
    for (int i = 0; i < deselectionCount; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, startRowNumber - 1, alignmentLength, upperSequencesCount - deselectionCount));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
}

GUI_TEST_CLASS_DEFINITION(test_1886_1) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a region in the sequence area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 4), QPoint(10, 12));

    // 3. Shift the region but don't release left mouse button
    const QPoint mouseDragPosition(7, 7);
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(3, 0));

    // 4. Call context menu
    GTUtilsDialog::waitForDialog(new GTUtilsEscClicker("msa sequence area context menu"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(8, 4), QPoint(13, 12)));
}

GUI_TEST_CLASS_DEFINITION(test_1886_2) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a region in the sequence area
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 4), QPoint(10, 12));

    // 3. Shift the region but don't release left mouse button
    const QPoint mouseDragPosition(7, 7);
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(1, 0));
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(2, 0));
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(3, 0));

    // 4. Replace selected rows with reverse
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse"}));
    GTMouseDriver::click(Qt::RightButton);

    // 5. Obtain selection
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR("--TGAC\n--TGAT\n--AGAC\n--AGAT\n--AGAT\n"
                  "--TGAA\n--CGAT\n--CGAT\n--CGAT" == selectionContent,
                  "MSA changing is failed");
}

GUI_TEST_CLASS_DEFINITION(test_1897) {
    // 1) Run UGENE
    // 2) Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3) Selected any region on alignment view using mouse or keyboard
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(40, 4), QPoint(50, 12));

    // 4) Choose Highlighting/Gaps action from context-menu
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Highlighting", "Gaps"}));
    GTMouseDriver::click(Qt::RightButton);

    // 5) Look at Hightlighting/Gaps action again
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_APPEARANCE, "Highlighting", "Gaps"}, PopupChecker::IsChecked));
    GTMouseDriver::click(Qt::RightButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_1908) {
    // 1. Open WD
    // 2. Add "Call variants" sample
    // Expected state: datasets are avaluable

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.fa");
}

GUI_TEST_CLASS_DEFINITION(test_1918) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Place to the empty scheme "Read File URL(s)" and "File Conversion" elements
    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::addElement("Read File URL(s)");
    WorkflowProcessItem* converter = GTUtilsWorkflowDesigner::addElement("File Format Conversion");

    // 3. Bind the elements
    GTUtilsWorkflowDesigner::connect(fileList, converter);

    // 4. Add the following input files to the "Read File URL(s)" dataset: "test/_common_data/mega/MegaTest_1.meg",
    //                                                              "test/_common_data/mega/MegaTest_2.meg",
    //                                                              "test/_common_data/clustal/align.aln"
    GTUtilsWorkflowDesigner::addInputFile("Read File URL(s)", testDir + "_common_data/mega/MegaTest_1.meg");
    GTUtilsWorkflowDesigner::addInputFile("Read File URL(s)", testDir + "_common_data/mega/MegaTest_2.meg");
    GTUtilsWorkflowDesigner::addInputFile("Read File URL(s)", testDir + "_common_data/clustal/align.aln");

    // 5. Set the following parameters of the "File Conversion" element: { Document format : NEXUS },
    //                                                                   { Excluded formats : CLUSTALW }
    GTUtilsWorkflowDesigner::click("File Format Conversion");
    GTUtilsWorkflowDesigner::setParameter("Document format", "NEXUS", GTUtilsWorkflowDesigner::comboValue, GTGlobals::UseMouse);
    GTUtilsWorkflowDesigner::setParameter("Excluded formats", {"clustal"}, GTUtilsWorkflowDesigner::ComboChecks);  // ComboChecks works by itemData.
    GTUtilsWorkflowDesigner::setParameter("Output folder", 0, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Custom folder", QDir().absoluteFilePath(sandBoxDir + "regression_1918"), GTUtilsWorkflowDesigner::textValue);

    // 6. Run the scheme.
    GTWidget::click(GTAction::button("Run workflow"));

    // Expected state: After workflow finish there are two output files in the dashboard: "MegaTest_1.meg.nexus",
    //                                                                                    "MegaTest_2.meg.nexus"
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(QFile::exists(sandBoxDir + "regression_1918/MegaTest_1.meg.nex"), "File 1 does not exist");
    CHECK_SET_ERR(QFile::exists(sandBoxDir + "regression_1918/MegaTest_2.meg.nex"), "File 2 does not exist");
    CHECK_SET_ERR(!QFile::exists(sandBoxDir + "regression_1918/align.aln.nex"), "File 3 exists");
}

GUI_TEST_CLASS_DEFINITION(test_1919) {
    // 1) Create the WD scheme: Read File URL(s) -> File conversions.
    // 2) Set input file: a BAM file (e.g _common_data/bam/scerevisiae.bam).
    // 3) Set the result format of the converter: BAM
    // 4) Run the scheme.
    // Expected: the scheme is finished well, no errors appeared.

    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read File URL(s)");
    GTUtilsWorkflowDesigner::addAlgorithm("File Format Conversion");

    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::getWorker("Read File URL(s)");
    WorkflowProcessItem* fileConversion = GTUtilsWorkflowDesigner::getWorker("File Format Conversion");

    GTUtilsWorkflowDesigner::connect(fileList, fileConversion);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("File Format Conversion"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Document format", 2, GTUtilsWorkflowDesigner::comboValue);

    // add setting source url in input data

    GTWidget::click(GTAction::button("Run workflow"));

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_1921) {
    //    1. Click rapidly on the "Show/hide log" button several (20) times (or use the Alt+3 hotkey).

    for (int i = 0; i < 20; i++) {
        GTKeyboardDriver::keyClick('3', Qt::AltModifier);
    }
    //    Expected state: UGENE not crashes.
}

GUI_TEST_CLASS_DEFINITION(test_1924) {
    // 1. Open any sequence
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use context menu on the sequence     {Edit sequence -> Insert subsequence}
    // 3. Fill in "atcgtac" or any valid sequence containing lower case
    Runnable* filler = new InsertSequenceFiller(
        "atcgtac");
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseKey);

    // 4. Click OK
    // Expected state: subsequence inserted
    // Bug state: Warning message is shown first

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 199957, "Sequence length is " + QString::number(sequenceLength) + ", expected 199957");

    QString sequenceBegin = GTUtilsSequenceView::getBeginOfSequenceAsString(7);
    CHECK_SET_ERR(sequenceBegin == "ATCGTAC", "Sequence starts with <" + sequenceBegin + ">, expected ATCGTAC");
}

GUI_TEST_CLASS_DEFINITION(test_1946) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add tuxedo scheme from samples
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index/"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    map.insert("Input transcripts annotations", QDir().absoluteFilePath(testDir + "_common_data/e_coli/e_coli_1000.gff"));
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"No-new-transcripts", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq") << (QStringList() << testDir + "_common_data/e_coli/e_coli_reads/e_coli_2_1.fastq"), map));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    3. fill all needed parameters and run schema
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: there are more then 10 result files and they are grouped into sublists
}

GUI_TEST_CLASS_DEFINITION(test_1984) {
    //    1) Run UGENE
    //    2) Open Settings/Preferences/External tools
    //    3) Set incorrect path for any external tool.
    //    4) Press OK
    //    Expected state: UGENE doesn't show any warning to user. Error should be at UGENE log ("Details" columns should be enabled)

    class CuffDiffIncorrectPath : public CustomScenario {
    public:
        void run() {
            AppSettingsDialogFiller::setExternalToolPath("Cuffdiff", "./");

            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CuffDiffIncorrectPath()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    CHECK_SET_ERR(lt.hasMessage("Cuffdiff validate task failed: Tool does not start."), "No error in the log!");
}

GUI_TEST_CLASS_DEFINITION(test_1986) {
    // Download a sequence from NCBI. Use "limit" for results.
    GTUtilsDialog::waitForDialog(new NCBISearchDialogSimpleFiller("rabbit", false, 10, "Organism"));
    GTMenu::clickMainMenuItem({"File", "Search NCBI GenBank..."});

    // Expected state: the chosen sequence has been downloaded, saved in FASTA format and displayed in sequence view
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkItem(".fasta", GTGlobals::FindOptions(true, Qt::MatchContains));
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
