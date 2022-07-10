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
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTSplitter.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextStream>
#include <QThreadPool>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ProjectViewModel.h>
#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/AnnotationsTreeView.h>
#include <U2View/AssemblyNavigationWidget.h>
#include <U2View/DetView.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTDatabaseConfig.h"
#include "GTTestsRegressionScenarios_3001_4000.h"
#include "GTUtilsAnnotationsHighlightingTreeView.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/BuildIndexDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindRepeatsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PredictSecondaryStructureDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExportHighlightedDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/plugins/biostruct3d_view/StructuralAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/cap3/CAP3SupportDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2SequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastLocalSearchDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SnpEffDatabaseDialogFiller.h"
#include "runnables/ugene/plugins/weight_matrix/PwmBuildDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/AliasesDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/clustalw/ClustalWDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {

GUI_TEST_CLASS_DEFINITION(test_3014) {
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/3014/pdb2q41.ent.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // In context menu go to 'Molecular surface'->'SES'
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Molecular Surface", "SES"}));
    auto widget3d = GTWidget::findWidget(os, "1-2Q41");
    GTWidget::click(os, widget3d, Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Molecular surface calculated and showed. Program not crached.
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3017) {
    // 1. Open 'HIV-1.aln';
    // 2. Select few columns;
    // 3. Run ClastulW, ClustalO, Mafft or T-Coffee alignment task;
    // 4. Try to change an alignment while the task is running: move region, delete region etc.;
    // Current state: there is no results of your actions because msa is blocked, overview is not recalculated.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os, MuscleDialogFiller::Refine));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with muscle"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(13, 8));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR("S" == clipboardText, "Alignment is not locked" + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_3031) {
    //    Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open "Statistics" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_STATISTICS_WIDGET"));
    //    Set "Montatna_montana" reference sequence.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-4, 4));
    GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));
    //    Delete "Montana_montana" sequence
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-4, 4));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    5. Right click on any sequence name
    //    Expected state: action "Set this sequence as reference" is visible, "Unset reference sequence" is invisible
}

GUI_TEST_CLASS_DEFINITION(test_3034) {
    //    1. Open "samples/FASTA/human_T1.fa".
    //    2. Right click on the document -> Add -> Add object to document.
    //    Expected: the dialog will appear. There are no human_T1 objects.
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, QMap<QString, QStringList>(), QSet<GObjectType>(), ProjectTreeItemSelectorDialogFiller::Separate, 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__ADD_OBJECT));
    QPoint docCenter = GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa");
    GTMouseDriver::moveTo(docCenter);
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3052) {
    GTLogTracer l;

    QString ugenedbFileName = testDir + "_common_data/scenarios/sandbox/test_3052.ugenedb";
    QString docName = "test_3052.ugenedb";

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, ugenedbFileName));
    GTFileDialog::openFile(os, testDir + "_common_data/bam", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMdi::closeWindow(os, "chrM [test_3052.ugenedb]");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Append"));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, ugenedbFileName));
    GTFileDialog::openFile(os, testDir + "_common_data/bam", "scerevisiae.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "chrM"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::removeDocument(os, docName);

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox", "test_3052.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<GObject*> objects = GTUtilsDocument::getDocument(os, docName)->getObjects();
    CHECK_SET_ERR(objects.size() == 1, "Expected 1 object, got: " + QString::number(objects.size()));
    CHECK_SET_ERR(objects[0]->getGObjectName() == "Scmito", "Expected 'Scmito' object name, got: " + objects[0]->getGObjectName());

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3052_1) {
    GTLogTracer l;

    QString ugenedbFileName = testDir + "_common_data/scenarios/sandbox/test_3052.ugenedb";
    QString docName = "test_3052.ugenedb";

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, ugenedbFileName));
    GTFileDialog::openFile(os, testDir + "_common_data/bam", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMdi::closeWindow(os, "chrM [test_3052.ugenedb]");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Append"));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, ugenedbFileName));
    GTFileDialog::openFile(os, testDir + "_common_data/bam", "scerevisiae.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "chrM"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsDocument::unloadDocument(os, docName, true);
    GTUtilsDocument::loadDocument(os, docName);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3073) {
    //    1. Open "human_T1.fa";
    //    2. Create few annotations (new file MyDocument_n.gb appeared);
    //    3. Save the project with these files and relaunch UGENE;
    //    4. Open the project and open either sequence or annotation file;
    //    Expected state: both files are loaded;
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "group", "annotation_1", "10..20", sandBoxDir + "test_3073.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "create_annotation_action"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group", "annotation_2", "10000..100000"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "create_annotation_action"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group", "annotation_3", "120000..180000"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "create_annotation_action"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    GTUtilsDocument::saveDocument(os, "test_3073.gb");
    GTUtilsDialog::waitForDialog(os, new SaveProjectAsDialogFiller(os, "proj_test_3073", sandBoxDir + "/proj_test_3073"));
    GTMenu::clickMainMenuItem(os, {"File", "Save project as..."}, GTGlobals::UseMouse);

    GTMenu::clickMainMenuItem(os, {"File", "Close project"}, GTGlobals::UseMouse);

    GTFileDialog::openFile(os, sandBoxDir, "proj_test_3073.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::loadDocument(os, "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded(os, "test_3073.gb"), "Annotation file is not loaded!");

    GTMenu::clickMainMenuItem(os, {"File", "Close project"}, GTGlobals::UseMouse);

    GTFileDialog::openFile(os, sandBoxDir, "proj_test_3073.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::loadDocument(os, "test_3073.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded(os, "human_T1.fa"), "Sequence file is not loaded!");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3085_1) {
    QFile(testDir + "_common_data/regression/3085/murine.gb").copy(sandBoxDir + "murine_3085_1.gb");

    // 1. Open samples/genbank/murine.gb.
    GTFileDialog::openFile(os, sandBoxDir + "murine_3085_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* sv = GTUtilsMdi::activeWindow(os);

    // 2. Change the sequence outside UGENE.
    // Expected state: dialog about file modification appeared.
    // 3. Click Yes.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    QByteArray data = GTFile::readAll(os, sandBoxDir + "murine_3085_1.gb");

    GTGlobals::sleep(1000);  // wait at least 1 second: UGENE does not detect file changes within 1 second interval.
    QFile file(sandBoxDir + "murine_3085_1.gb");
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
    GTGlobals::sleep(6000);  // Wait until UGENE detects the changes.

    // Expected state: file was updated, the sequence view with annotations is opened and updated.
    QWidget* reloaded1Sv = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(sv != reloaded1Sv, "File is not reloaded 1");

    // 4. Change the annotations file outside UGENE (e.g. change annotation region).
    // Expected state:: dialog about file modification appeared.
    // 5. Click Yes.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    data = GTFile::readAll(os, testDir + "_common_data/regression/3085/murine_2.gb");

    GTGlobals::sleep(1000);  // wait at least 1 second: UGENE does not detect file changes within 1 second interval.
    QFile file1(sandBoxDir + "murine_3085_1.gb");
    file1.open(QIODevice::WriteOnly);
    file1.write(data);
    file1.close();

    GTGlobals::sleep(6000);  // Wait until UGENE detects the changes.

    // Expected state:: file was updated, the sequence view with annotations is opened and updated.
    QWidget* reloaded2Sv = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(reloaded1Sv != reloaded2Sv, "File is not reloaded 2");
}

GUI_TEST_CLASS_DEFINITION(test_3085_2) {
    QFile(testDir + "_common_data/regression/3085/test.gb").copy(sandBoxDir + "murine_3085_2.gb");
    GTLogTracer l;

    // 1. Open "_common_data/regression/test.gb".
    GTFileDialog::openFile(os, sandBoxDir + "murine_3085_2.gb");
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);

    // 2. Append another sequence to the file outside of UGENE.
    // 3. Click "Yes" in the appeared dialog in UGENE.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    QByteArray data = GTFile::readAll(os, testDir + "_common_data/regression/3085/test_1.gb");

    GTGlobals::sleep(1000);  // Wait at least 1 second: UGENE does not detect file changes within 1 second interval.
    QFile file(sandBoxDir + "murine_3085_2.gb");
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    GTUtilsDialog::checkNoActiveWaiters(os);

    // Expected state: document reloaded without errors/warnings.
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_3086) {
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_3086/test_3086.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/3086/", "UGENE-3086.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3092) {
    //    1. Open "data/samples/FASTA/human_T1.fa".

    //    2. Do {Actions -> Analyze -> Query with BLAST...} in the main menu.
    //    Expected state: a dialog "Search using Local BLAST database" appears, UGENE doesn't crash.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BlastLocalSearchDialogFiller(BlastLocalSearchDialogFiller::Parameters(), os));
    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Query with local BLAST..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_3101) {
    // 1. Open "_common_data\scenarios\_regression\3101\enzymes"
    // Expected state : "Document format selection" dialog appeared
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "Newick Standard"));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/_regression/3101/enzymes");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Select "Newick" format
    // Expected state : Task finished with error, but without assert call
}

GUI_TEST_CLASS_DEFINITION(test_3112) {
    // Open big alignment, e.g. "_common_data/clustal/big.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto showOverviewButton = qobject_cast<QToolButton*>(GTAction::button(os, "Show overview"));
    CHECK_SET_ERR(showOverviewButton != nullptr, "Overview button is not found");
    CHECK_SET_ERR(showOverviewButton->isChecked(), "Overview button is not checked");

    // Modify the alignment.
    // Expected state: the overview task starts.
    GTUtilsMsaEditor::removeColumn(os, 5);
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "1: There are no active tasks ");

    // Click the "Overview" button on the main toolbar
    // Expected state: the task is canceled, the overview is hidden.
    GTWidget::click(os, showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os, 2000);
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 0, "2: There are unfinished tasks");

    // Click the "Overview" button again and wait till overview calculation and rendering ends.
    GTWidget::click(os, showOverviewButton);
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "3: There are no active tasks");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Hide the overview.
    GTWidget::click(os, showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os, 2000);
    // Open the overview.
    GTWidget::click(os, showOverviewButton);
    // Expected state: no task starts because nothing have been changed.
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 0, "4: There are unfinished tasks");

    // Hide the overview.
    GTWidget::click(os, showOverviewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os, 2000);

    // Edit the alignment.
    GTUtilsMsaEditor::removeColumn(os, 5);

    // Open the overview.
    // Expected state: overview calculation task starts.
    GTWidget::click(os, showOverviewButton);
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "5: There are no active tasks");
}

GUI_TEST_CLASS_DEFINITION(test_3126) {
    //    1. Open "test/_common_data/ace/ace_test_1.ace".
    //    2. Click "OK" in the import dialog.
    //    Expected: the file is imported, UGENE does not crash.
    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_3126"));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/ace_test_1.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3128) {
    // 1. Open file test/_common_data/cmdline/read-write/read_db_write_gen.uws"
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/read-write/", "read_db_write_gen.uws");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Set parameters:     db="NCBI protein sequence database", id="AAA59172.1"
    GTUtilsWorkflowDesigner::click(os, "Read from remote database", QPoint(-20, -20));
    GTUtilsWorkflowDesigner::setParameter(os, "Database", 2, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Resource ID(s)", "AAA59172.1", GTUtilsWorkflowDesigner::textValue);

    // 3. Launch scheme.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));

    // Expected state: no errors in the log.
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3128_1) {
    // 1. Open any genbank file with a COMMENT section
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Find the "comment" annotation, click it.
    QTreeWidgetItem* item2 = GTUtilsAnnotationsTreeView::findItem(os, "comment");
    QPoint p2 = GTTreeWidget::getItemCenter(os, item2);
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::click();
    // Expected state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_3138) {
    //    1. Open "_common_data/fasta/abcd.fa"
    //    2. Open Find Pattern on the Option Panel
    //    3. Set algorithm to "Regular expression"
    //    4. Find 'A*' pattern
    //    Expected state: founded regions are valid.

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/abcd.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));

    auto algorithmBox = GTWidget::findComboBox(os, "boxAlgorithm");

    GTComboBox::selectItemByText(os, algorithmBox, "Regular expression");

    GTWidget::click(os, GTWidget::findWidget(os, "textPattern"));
    GTKeyboardDriver::keySequence("A*");

    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));

    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    for (const U2Region& r : qAsConst(regions)) {
        CHECK_SET_ERR(r.length > 0, "Invalid annotated region!");
    }
}

GUI_TEST_CLASS_DEFINITION(test_3139) {
    // 1. Do {File -> Open as...} in the main menu.
    // 2. Open "data/samples/FASTA/human_T1.fa" as msa.
    //     Expected state: a MSA Editor is opened.
    //     Current state: a Sequence View is opened.
    GTUtilsDialog::add(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/", "human_T1.fa"));
    GTUtilsDialog::add(os, new DocumentFormatSelectorDialogFiller(os, "FASTA"));
    GTUtilsDialog::add(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTMenu::clickMainMenuItem(os, {"File", "Open as..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::findWidget(os, "msa_editor_sequence_area");
}

GUI_TEST_CLASS_DEFINITION(test_3140) {
    // Open "_common_data/clustal/big.aln".
    GTUtilsTaskTreeView::openView(os);
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select the first symbol of the first line.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(0, 0));

    // Press the Space button and do not unpress it.
    // Expected: the alignment changes on every button press. UGENE does not crash.
    // Unpress the button.
    // Expected: the overview rendering task is finished. The overview is shown.
    for (int i = 0; i < 100; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }

    int renderTasksCount = GTUtilsTaskTreeView::getTopLevelTasksCount(os);
    CHECK_SET_ERR(renderTasksCount == 1, QString("An unexpected overview render tasks count: expect %1, got %2").arg(1).arg(renderTasksCount));

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QColor currentColor = GTUtilsMsaEditor::getGraphOverviewPixelColor(os, QPoint(1, 1));
    QColor expectedColor = QColor("white");
    QString currentColorString = QString("(%1, %2, %3)").arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());
    QString expectedColorString = QString("(%1, %2, %3)").arg(expectedColor.red()).arg(expectedColor.green()).arg(expectedColor.blue());
    CHECK_SET_ERR(expectedColor == currentColor,
                  QString("An unexpected color, maybe overview was not rendered: expected %1, got %2")
                      .arg(expectedColorString)
                      .arg(currentColorString));
}

GUI_TEST_CLASS_DEFINITION(test_3142) {
    //    1. Open "data/samples/CLUSTALW/COI.aln"
    //    2. On the options panel press the "Open tree" button
    //    Expected state: the "Select files to open..." dialog has opened
    //    3. Choose the file "data/samples/Newick/COI.nwk"
    //    Expected state: a tree view has appeared along with MSA view
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_ADD_TREE_WIDGET"));

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Newick/COI.nwk"));
    GTWidget::click(os, GTWidget::findWidget(os, "OpenTreeButton"));

    GTWidget::findWidget(os, "msa_editor_sequence_area");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3143) {
    //    1. Open file data/samples/Assembly/chrM.sorted.bam;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(os, dataDir + "samples/Assembly", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: Showed Import BAM File dialog.
    //    2. Click Import;
    //    Expected state: Imported file opened in Assembly Viewer.
    GTWidget::findWidget(os, "assembly_browser_chrM [chrM.sorted.bam.ugenedb]");
    //    3. Remove this file from project and try to open it again;
    GTUtilsProjectTreeView::click(os, "chrM.sorted.bam.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    //    Expected state: Showed Import BAM File dialog.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Replace"));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(os, dataDir + "samples/Assembly", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    4. Click Import;
    //    Expected state: Showed message box with question about overwriting of existing file..
    //    5. Click Replace;
    GTWidget::findWidget(os, "chrM [chrM.sorted.bam.ugenedb]");
    //    Expected state: Imported file opened in Assembly Viewer without errors.
}

GUI_TEST_CLASS_DEFINITION(test_3155) {
    // 1. Open "human_T1"
    // Expected state: "Circular search" checkbox does not exist
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Press "Find ORFs" tool button
    class CancelClicker : public Filler {
    public:
        CancelClicker(HI::GUITestOpStatus& _os)
            : Filler(_os, "ORFDialogBase") {
        }
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget(os);
            CHECK_SET_ERR(GTWidget::findWidget(os, "ckCircularSearch", dialog, {false}) == nullptr, "ckCircularSearch must not exist");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new CancelClicker(os));
    GTWidget::click(os, GTAction::button(os, "Find ORFs"));
}

class test_3165_messageBoxDialogFiller : public MessageBoxDialogFiller {
public:
    test_3165_messageBoxDialogFiller(HI::GUITestOpStatus& os, QMessageBox::StandardButton _b)
        : MessageBoxDialogFiller(os, _b) {
    }
    void run() override {
        QWidget* activeModal = GTWidget::getActiveModalWidget(os);
        QMessageBox* messageBox = qobject_cast<QMessageBox*>(activeModal);
        CHECK_SET_ERR(messageBox != nullptr, "messageBox is NULL");

        QAbstractButton* button = messageBox->button(b);
        CHECK_SET_ERR(button != nullptr, "There is no such button in messagebox");

        GTWidget::click(os, button);
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Save, "", "permissionBox"));
    }
};

GUI_TEST_CLASS_DEFINITION(test_3165) {
    //    1. Set file read-only: "test/_common_data/scenarios/msa/ma.aln".
    GTFile::copy(os, testDir + "_common_data/scenarios/msa/ma.aln", sandBoxDir + "ma.aln");
    GTFile::setReadOnly(os, sandBoxDir + "ma.aln");
    //    2. Open it with UGENE.
    GTFileDialog::openFile(os, sandBoxDir, "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    3. Change the alignment (e.g. insert a gap).
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(1, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    //    4. Close the project.
    GTUtilsDialog::add(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTUtilsDialog::add(os, new test_3165_messageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsDialog::add(os, new GTFileDialogUtils(os, sandBoxDir, "test_3165_out.aln", GTFileDialogUtils::Save));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
    // GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Save));
    //     Expected state: you are offered to save the project.
    //     5. Do not save the project.
    //     Expected state: you are offered to save the file.
    //     6. Accept the offering.
    //     Expected state: UGENE notices that it can't rewrite the file, it offers you to save the file to another location.
    //     7. Save file anywhere.
    //     Expected state: the project closes, the file is successfully saved, UGENE doesn't crash.
    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + "test_3165_out.aln"), "file not saved");
    //    Current state: file is successfully saved, then UGENE crashes.
}

GUI_TEST_CLASS_DEFINITION(test_3170) {
    // 1. Open human_T1.fa.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    // 2. Select the region [301..350].
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 51, 102));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));

    // 3. Context menu -> Analyze -> Query with BLAST.
    // 5. Select the database.
    // 6. Run.
    BlastLocalSearchDialogFiller::Parameters blastParams;
    blastParams.runBlast = true;
    blastParams.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::waitForDialog(os, new BlastLocalSearchDialogFiller(blastParams, os));
    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Query with local BLAST..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the found annotations don't start from the position 1.
    bool found1 = GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(51, 51));
    CHECK_OP(os, );
    CHECK_SET_ERR(found1, "Can not find the blast result");
    bool found2 = GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(1, 52));
    CHECK_OP(os, );
    CHECK_SET_ERR(!found2, "Wrong blast result");
}

GUI_TEST_CLASS_DEFINITION(test_3175) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the first sequence is "TAAGACTTCTAA".
    const QString firstSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, 0);
    CHECK_SET_ERR("TAAGACTTCTAA" == firstSequence, "MSA changing is failed");
}

GUI_TEST_CLASS_DEFINITION(test_3180) {
    // 1. Open "samples/FASTA/human_T1.fa".
    // 2. Click the "Find restriction sites" button on the main toolbar.
    // 3. Accept the dialog.
    // Expected: the task becomes cancelled.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    GTWidget::click(os, GTWidget::findWidget(os, "AutoAnnotationUpdateAction"));

    GTUtilsTaskTreeView::cancelTask(os, "Auto-annotations update task", false);
}

GUI_TEST_CLASS_DEFINITION(test_3209_1) {
    // BLAST from file
    BlastLocalSearchDialogFiller::Parameters blastParams;
    blastParams.runBlast = true;
    blastParams.programNameText = "blastn";
    blastParams.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    blastParams.withInputFile = true;
    blastParams.inputPath = dataDir + "samples/FASTA/human_T1.fa";
    GTUtilsDialog::waitForDialog(os, new BlastLocalSearchDialogFiller(blastParams, os));
    GTMenu::clickMainMenuItem(os, {"Tools", "BLAST", "BLAST search..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool found = GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(5061, 291));
    CHECK_OP(os, );
    CHECK_SET_ERR(found, "Can not find the blast result");
}

GUI_TEST_CLASS_DEFINITION(test_3214) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Extract Consensus from Alignment as Sequence");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Extract Consensus from Alignment as Sequence"));
    GTMouseDriver::click();

    GTUtilsWorkflowDesigner::setParameter(os, "Threshold", 49, GTUtilsWorkflowDesigner::spinValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Algorithm", 0, GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter(os, "Threshold") == "50", "Wrong parameter");

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Extract Consensus from Alignment as Text");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Extract Consensus from Alignment as Text"));
    GTMouseDriver::click();

    GTUtilsWorkflowDesigner::setParameter(os, "Algorithm", 0, GTUtilsWorkflowDesigner::comboValue);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getAllParameters(os).size() == 1, "Too many parameters");
}

GUI_TEST_CLASS_DEFINITION(test_3216_1) {
    //    1. Open "test/_common_data/genbank/1anot_1seq.gen" file.
    QDir().mkpath(sandBoxDir + "test_3216");
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "test_3216/test_3216_1.gen");
    GTFileDialog::openFile(os, sandBoxDir + "test_3216", "test_3216_1.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add a qualifier with the value "012345678901234567890123456789012345678901234567890123456789".
    QString expectedValue = "012345678901234567890123456789012345678901234567890123456789";

    QTreeWidgetItem* cdsItem = GTUtilsAnnotationsTreeView::findItem(os, "CDS");
    GTUtilsAnnotationsTreeView::createQualifier(os, "test_3216_1", expectedValue, cdsItem);

    //    3. Save the file, reopen the file.
    //    Expected state: the qualifier value is the same.
    GTUtilsDocument::saveDocument(os, "test_3216_1.gen");
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsDocument::removeDocument(os, "test_3216_1.gen");

    GTFileDialog::openFile(os, sandBoxDir + "test_3216", "test_3216_1.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"CDS"});
    QString actualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "test_3216_1", "CDS");
    CHECK_SET_ERR(actualValue == expectedValue, QString("The qualifier value is incorrect: expect '%1', got '%2'").arg(expectedValue).arg(actualValue));
}

GUI_TEST_CLASS_DEFINITION(test_3216_2) {
    //    1. Open "test/_common_data/genbank/1anot_1seq.gen" file.
    QDir().mkpath(sandBoxDir + "test_3216");
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "test_3216/test_3216_2.gen");
    GTFileDialog::openFile(os, sandBoxDir + "test_3216", "test_3216_2.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add a qualifier with the value "012345678901234567890123456789 012345678901234567890123456789".
    QString expectedValue = "012345678901234567890123456789 012345678901234567890123456789";
    QTreeWidgetItem* cdsItem = GTUtilsAnnotationsTreeView::findItem(os, "CDS");
    GTUtilsAnnotationsTreeView::createQualifier(os, "test_3216_2", expectedValue, cdsItem);

    //    3. Save the file, reopen the file.
    //    Expected state: the qualifier value is the same.
    GTUtilsDocument::saveDocument(os, "test_3216_2.gen");
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsDocument::removeDocument(os, "test_3216_2.gen");

    GTFileDialog::openFile(os, sandBoxDir + "test_3216", "test_3216_2.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"CDS"});
    QString actualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "test_3216_2", "CDS");
    CHECK_SET_ERR(actualValue == expectedValue, QString("The qualifier value is incorrect: expect '%1', got '%2'").arg(expectedValue).arg(actualValue));
}

GUI_TEST_CLASS_DEFINITION(test_3216_3) {
    //    1. Open "test/_common_data/genbank/1anot_1seq.gen" file.
    QDir().mkpath(sandBoxDir + "test_3216");
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "test_3216/test_3216_3.gen");
    GTFileDialog::openFile(os, sandBoxDir + "test_3216", "test_3216_3.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add a qualifier with the value "012345678901234567890123456789 0  1 2345678901234567890123456789".
    QString expectedValue = "012345678901234567890123456789 0  1 2345678901234567890123456789";
    QTreeWidgetItem* cdsItem = GTUtilsAnnotationsTreeView::findItem(os, "CDS");
    GTUtilsAnnotationsTreeView::createQualifier(os, "test_3216_3", expectedValue, cdsItem);

    //    3. Save the file, reopen the file.
    //    Expected state: the qualifier value is the same.
    GTUtilsDocument::saveDocument(os, "test_3216_3.gen");
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsDocument::removeDocument(os, "test_3216_3.gen");

    GTFileDialog::openFile(os, sandBoxDir + "test_3216", "test_3216_3.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"CDS"});
    QString actualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "test_3216_3", "CDS");
    CHECK_SET_ERR(actualValue == expectedValue, QString("The qualifier value is incorrect: expect '%1', got '%2'").arg(expectedValue).arg(actualValue));
}

GUI_TEST_CLASS_DEFINITION(test_3218) {
    // 1. Open "test/_common_data/genbank/big_feature_region.gb".
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/", "big_feature_region.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state : file is opened, there are no errors in the log
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3220) {
    // 1. Open human_T1.fa
    //
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Add an annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "DDD", "D", "10..16"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Add", "New annotation..."});
    // 2. Add qualifier with quotes
    GTUtilsDialog::waitForDialog(os, new EditQualifierFiller(os, "newqualifier", "val\"", GTGlobals::UseMouse, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "D"));
    GTMouseDriver::click(Qt::RightButton);

    // 3. Add another qualifier to the same annotation
    GTUtilsDialog::waitForDialog(os, new EditQualifierFiller(os, "newqualifier2", "val\"2", GTGlobals::UseMouse, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "D"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters(os);

    // 4. Save the file and reload it
    GTUtilsDocument::unloadDocument(os, "human_T1.fa", true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::loadDocument(os, "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "D"));
    GTMouseDriver::click(Qt::LeftButton);

    QTreeWidgetItem* generalItem = GTUtilsAnnotationsTreeView::findItem(os, "D");
    AVAnnotationItem* annotation = dynamic_cast<AVAnnotationItem*>(generalItem);
    CHECK_SET_ERR(annotation != nullptr, "Annotation is not found");
    CHECK_SET_ERR(annotation->annotation->findFirstQualifierValue("newqualifier") == "val\"", "Qualifier is not found");
    CHECK_SET_ERR(annotation->annotation->findFirstQualifierValue("newqualifier2") == "val\"2", "Qualifier 2 is not found");
}

GUI_TEST_CLASS_DEFINITION(test_3221) {
    //    1. Open "data/samples/FASTA/human_T1.fa".

    //    2. Open "Find pattern" options panel tab.

    //    3. Enter pattern with long annotation name(>15 characters).

    //    4. Check "Use pattern name" check box

    //    5. Press "Create annotations" button
    //    Expected state: annotations created without errors.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, ">long_annotation_name");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);
    GTKeyboardDriver::keySequence("ACGTAAA");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os, true);

    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbUsePatternNames"), true);
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::findItem(os, "long_annotation_name  (0, 10)");
}

GUI_TEST_CLASS_DEFINITION(test_3223) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    QString pattern = ">zzz\n"
                      "ACCTGAA\n"
                      ">yyy\n"
                      "ATTGACA\n";
    GTUtilsOptionPanelSequenceView::enterPattern(os, pattern, true);
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Annotation parameters"));
    auto chbUsePatternNames = GTWidget::findCheckBox(os, "chbUsePatternNames");
    GTCheckBox::setChecked(os, chbUsePatternNames, true);
    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::findItem(os, "yyy  (0, 32)");
    GTUtilsAnnotationsTreeView::findItem(os, "zzz  (0, 34)");
}

GUI_TEST_CLASS_DEFINITION(test_3226) {
    // 1. Create a workflow with a 'Read File URL(s)' element.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read File URL(s)");

    // 2. Setup alias 'in' for input path.
    QMap<QPoint*, QString> map;
    QPoint p(1, 0);
    map[&p] = "in";
    GTUtilsDialog::waitForDialog(os, new AliasesDialogFiller(os, map));
    GTWidget::click(os, GTAction::button(os, "Set parameter aliases"));

    // 3. Copy and paste the 'Read File URL(s)' element.
    GTUtilsWorkflowDesigner::click(os, "Read File URL(s)");
    GTKeyboardUtils::copy();
    // GTWidget::click(os, GTAction::button(os, "Copy action"));
    GTKeyboardUtils::paste();

    // 4. Save the workflow.
    QString path = sandBoxDir + "test_3226_workflow.uwl";
    GTUtilsDialog::waitForDialog(os, new WorkflowMetaDialogFiller(os, path, ""));
    GTWidget::click(os, GTAction::button(os, "Save workflow action"));

    // 5. Close current workflow.
    GTWidget::click(os, GTAction::button(os, "New workflow action"));

    // 7. Open the saved workflow.
    GTLogTracer l;
    GTUtilsWorkflowDesigner::loadWorkflow(os, path);

    // Expected state: the saved workflow is opened, there are no errors in the log, the alias it set only for the one element.
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3229) {
    //    1. Create the "read sequence -> write sequence" workflow.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement(os, "Read Sequence", true);
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement(os, "Write Sequence", true);
    GTUtilsWorkflowDesigner::connect(os, read, write);

    //    2. Set input a single file human_T1
    GTUtilsWorkflowDesigner::click(os, read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    //    3. Set the output path: ../test.fa or ./test.fa Output file
    GTUtilsWorkflowDesigner::click(os, write);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "./test.fa", GTUtilsWorkflowDesigner::textValue);

    //    4. Run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a single result file on the WD dashboard.
    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles(os);

    const int expectedFilesCount = 1;
    CHECK_SET_ERR(expectedFilesCount == outputFiles.size(),
                  QString("An unexpected count of output files: expected %1, got %2")
                      .arg(expectedFilesCount)
                      .arg(outputFiles.size()));

    const QString expectedFileName = "test.fa";
    CHECK_SET_ERR(expectedFileName == outputFiles.first(),
                  QString("An unexpected result file name: expected '%1', got '%2'")
                      .arg(expectedFileName)
                      .arg(outputFiles.first()));
}

GUI_TEST_CLASS_DEFINITION(test_3245) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // 2. Ensure that there is a single menu item (Create new color scheme) in the {Colors -> Custom schemes}
    // submenu of the context menu. Click it.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::Highlighting);

    auto combo = GTWidget::findComboBox(os, "colorScheme");
    int initialItemsNumber = combo->count();

    // 3. Create a new color scheme, accept the preferences dialog.
    QString colorSchemeName = GTUtils::genUniqueString(name);
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                              << "Custom schemes"
                                                              << "Create new color scheme"));
    GTUtilsDialog::add(os, new NewColorSchemeCreator(os, colorSchemeName, NewColorSchemeCreator::nucl));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // 4. Ensure that the new scheme is added to the context menu. Call the preferences dialog again.
    // 5. Remove the custom scheme and cancel the preferences dialog.
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                              << "Custom schemes" << colorSchemeName));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    combo = GTWidget::findComboBox(os, "colorScheme");
    CHECK_SET_ERR(combo->count() - 1 == initialItemsNumber, "color scheme hasn't been added to the Options Panel");

    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                              << "Custom schemes"
                                                              << "Create new color scheme"));
    GTUtilsDialog::add(os, new NewColorSchemeCreator(os, colorSchemeName, NewColorSchemeCreator::nucl, NewColorSchemeCreator::Delete, true));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Expected state: the scheme presents in the context menu, it is shown in the preferences dialog.
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                              << "Custom schemes" << colorSchemeName));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    combo = GTWidget::findComboBox(os, "colorScheme");
    CHECK_SET_ERR(combo->count() - 1 == initialItemsNumber, "color scheme hasn't been added to the Options Panel");
}

GUI_TEST_CLASS_DEFINITION(test_3253) {
    // Open "data/samples/ABIF/A01.abi".
    // Minimize annotation tree view.
    // Expected state: Chromatogram view is resized.

    GTFileDialog::openFile(os, dataDir + "/samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto chromaView = GTWidget::findWidget(os, "chromatogram_view_A1#berezikov");
    auto annotationTreeWidget = GTWidget::findWidget(os, "annotations_tree_widget");

    QSize startSize = chromaView->size();
    QPoint treeGlobalTopLeft = annotationTreeWidget->mapToGlobal(annotationTreeWidget->pos());
    GTMouseDriver::moveTo(QPoint(treeGlobalTopLeft.x() + 100, treeGlobalTopLeft.y()));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(treeGlobalTopLeft.x() + 100, treeGlobalTopLeft.y() + annotationTreeWidget->height()));
    GTMouseDriver::release();

    QSize endSize = chromaView->size();
    CHECK_SET_ERR(startSize != endSize, "chromatogram_view is not resized");
}

GUI_TEST_CLASS_DEFINITION(test_3253_1) {
    /*  1. Open "data/samples/ABIF/A01.abi".
     *  2. Toggle Show Detail View
     *  3. Resize annotation tree view
     *    Expected state: Detail View view resized
     */
    GTFileDialog::openFile(os, dataDir + "/samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto annotationTreeWidget = GTWidget::findWidget(os, "annotations_tree_widget");

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_A1#berezikov");
    auto showDetView = GTWidget::findToolButton(os, "show_hide_details_view", toolbar);
    if (!showDetView->isChecked()) {
        GTWidget::click(os, showDetView);
    }
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTWidget::click(os, GTWidget::findWidget(os, "CHROMA_ACTION", toolbar));

    QSplitterHandle* splitterHandle = qobject_cast<QSplitterHandle*>(GTWidget::findWidget(os, "qt_splithandle_det_view_A1#berezikov"));
    CHECK_SET_ERR(nullptr != splitterHandle, "splitterHandle is not present");

    auto detView = GTWidget::findWidget(os, "render_area_A1#berezikov");
    QSize startSize = detView->size();
    GTMouseDriver::moveTo(QPoint(annotationTreeWidget->mapToGlobal(annotationTreeWidget->pos()).x() + 100, annotationTreeWidget->mapToGlobal(annotationTreeWidget->pos()).y()));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(annotationTreeWidget->mapToGlobal(annotationTreeWidget->pos()).x() + 100, annotationTreeWidget->mapToGlobal(annotationTreeWidget->pos()).y() - detView->size().height()));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    QSize endSize = detView->size();
    CHECK_SET_ERR(startSize != endSize, "detView is not resized");
}
GUI_TEST_CLASS_DEFINITION(test_3253_2) {
    /*  1. Open "data/samples/ABIF/A01.abi".
     *   2. Open GC Content (%) graph
     *   3. Close the chomatogram view
     *      Expected state: GC Content (%) graph view resized.
     */
    GTFileDialog::openFile(os, dataDir + "/samples/ABIF/", "A01.abi");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction", GTUtilsSequenceView::getSeqWidgetByNumber(os, 0)));
    GTUtilsDialog::checkNoActiveWaiters(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto graphView = GTWidget::findWidget(os, "GSequenceGraphViewRenderArea");
    QSize startSize = graphView->size();

    // Hide the chromatogram.
    GTWidget::click(os, GTWidget::findWidget(os, "CHROMA_ACTION"));

    auto splitter = GTWidget::findSplitter(os, "single_sequence_view_splitter");
    GTSplitter::moveHandle(os, splitter, graphView->height() / 2, 2);
    GTThread::waitForMainThread();

    QSize endSize = graphView->size();
    CHECK_SET_ERR(startSize != endSize, "graphView is not resized, size: " + QString::number(endSize.width()) + "x" + QString::number(endSize.height()));
}

GUI_TEST_CLASS_DEFINITION(test_3255) {
    //    1. Open "data/samples/Assembly/chrM.sam.bam".
    //    Expected state: an import dialog appears.
    //    2. Set any valid output path (or use default), check the "Import unmapped reads" option and click the "Import" button.
    //    Expected state: the task finished without errors.
    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_3255/test_3255.ugenedb", "", "", true));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3263) {
    //    1. Open "_common_data/alphabets/standard_dna_rna_amino_1000.fa"
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/alphabets/standard_dna_rna_amino_1000.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open CV for the first sequence
    auto cvButton1 = GTWidget::findWidget(os, "CircularViewAction", GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    auto cvButton2 = GTWidget::findWidget(os, "CircularViewAction", GTWidget::findWidget(os, "ADV_single_sequence_widget_1"));
    GTWidget::click(os, cvButton2);
    //    3. Open CV for the second sequence
    GTWidget::click(os, cvButton1);
    //    4. Click CV button for the first sequence (turn it off and on again) few times
    auto CV_ADV_single_sequence_widget_1 = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_1");
    QRect geometry = CV_ADV_single_sequence_widget_1->geometry();
    for (int i = 0; i < 5; i++) {
        GTWidget::click(os, cvButton1);
        GTWidget::click(os, cvButton1);
        CHECK_SET_ERR(geometry == CV_ADV_single_sequence_widget_1->geometry(), "geometry changed");
    }
    //    See the result on the attached screenshot.
}

GUI_TEST_CLASS_DEFINITION(test_3270) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open "Search in Sequence" options panel tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    //    3. Open "Annotation parameters" group and check "Use pattern name" option.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    GTUtilsOptionPanelSequenceView::setUsePatternName(os);

    //    4. Set next patterns:
    //    > pattern1
    //    TGGGGGCCAATA

    //    > pattern2
    //    GGCAGAAACC
    QString pattern = "> pattern1\n"
                      "TGGGGGCCAATA\n\n"
                      "> pattern2\n"
                      "GGCAGAAACC";
    GTUtilsOptionPanelSequenceView::enterPattern(os, pattern, true);

    //    Expected state: there is an info message: "annotation name is not set".
    QString hintText = GTUtilsOptionPanelSequenceView::getHintText(os);
    CHECK_SET_ERR(hintText.contains("Info: annotation name is not set"), QString("Incorrect hint text: '%1'").arg(hintText));

    //    5. Click "Create annotations" button.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);

    //    Expected state: there are two annotations with names "pattern1" and "pattern2".
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::findItem(os, "pattern1");
    GTUtilsAnnotationsTreeView::findItem(os, "pattern2");

    //    6. Set next pattern:
    //    >gi|92133205|dbj|BD295338.1| A method for providing and controling the rice fertility, and discerning the presence of the rice restorer gene by using the rice restorer gene to the rice BT type cytoplasmic male sterility
    //    TGGGGATTCT
    pattern = ">gi|92133205|dbj|BD295338.1| A method for providing and controling the rice fertility, and discerning the presence of the rice restorer gene by using the rice restorer gene to the rice BT type cytoplasmic male sterility"
              "\n"
              "TGGGGATTCT";
    GTUtilsOptionPanelSequenceView::enterPattern(os, pattern, true);

    //    Expected state: there are no warnings.
    hintText = GTUtilsOptionPanelSequenceView::getHintText(os);
    CHECK_SET_ERR(!hintText.contains("Warning") && !hintText.contains("annotation name is not set"), QString("Incorrect hint text: '%1'").arg(hintText));

    //    6. Click "Create annotations" button.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);

    //    Expected state: there is an additional annotation with name "gi|92133205|dbj|BD295338.1| A method for providing and controling the rice fertility, and discerning the presence of the rice restorer gene by using the rice restorer gene to the rice BT type cytoplasmic male sterility".
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::findItem(os, "gi|92133205|dbj|BD295338.1| A method for providing and controling the rice fertility, and discerning the presence of the rice restorer gene by using the rice restorer gene to the rice BT type cytoplasmic male sterility");
}

GUI_TEST_CLASS_DEFINITION(test_3274) {
    QStringList expectedNames;
    QList<ADVSingleSequenceWidget*> seqWidgets;
    expectedNames << "seq1"
                  << "seq3"
                  << "seq5";

    seqWidgets = GTUtilsProject::openFileExpectSequences(os,
                                                         testDir + "_common_data/alphabets/",
                                                         "standard_dna_rna_amino_1000.fa",
                                                         expectedNames);
    CHECK_OP_SET_ERR(os, "Failed to open sequences!");

    ADVSingleSequenceWidget* seq1Widget = seqWidgets.at(0);
    ADVSingleSequenceWidget* seq3Widget = seqWidgets.at(1);

    GTUtilsCv::cvBtn::click(os, seq3Widget);
    GTUtilsCv::cvBtn::click(os, seq1Widget);

    auto circularView = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    CHECK_OP_SET_ERR(os, "Failed to open circular view!");

    GTUtilsDialog::waitForDialog(os, new CircularViewExportImage(os, testDir + "_common_data/scenarios/sandbox/image.jpg", "", "seq3"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "Save circular view as image"));

    GTWidget::click(os, circularView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3276) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Build a phylogenetic tree. Check that the tree is synchronized with the alignment.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_3276/COI.wnk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* syncModeButton = GTAction::button(os, "sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/1");
    CHECK_SET_ERR(syncModeButton->isEnabled(), "Sync mode must be enabled/1");

    // Rename the first and the second sequences to "1".
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "Isophya_altaica_EF540820", "1");
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "Bicolorana_bicolor_EF540830", "1");

    // Ensure that sync mode is ON: the tree tracks all sequence rename operations correctly.
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/2");
    CHECK_SET_ERR(syncModeButton->isEnabled(), "Sync mode must be enabled/2");

    // Remove the first sequence.
    GTUtilsMSAEditorSequenceArea::removeSequence(os, "1");

    // Ensure that sync mode is OFF, and can't be  enabled because tree and MSA sequence counts do not match.
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");
    CHECK_SET_ERR(!syncModeButton->isEnabled(), "Sync mode must be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_3277) {
    //    Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto seqArea = GTWidget::findWidget(os, "msa_editor_sequence_area");
    QColor before = GTWidget::getColor(os, seqArea, QPoint(1, 1));
    //    Open the "Highlighting" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    //    Set any reference sequence.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 5));
    GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));
    //    Select different highlighting schemes.
    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingScheme, "Gaps");
    //    Current state: the highlighting doesn't work for all sequences except the reference sequence.

    QColor after = GTWidget::getColor(os, seqArea, QPoint(1, 1));
    CHECK_SET_ERR(before != after, "colors not changed");
}

GUI_TEST_CLASS_DEFINITION(test_3279) {
    //    Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open "Statistics" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_STATISTICS_WIDGET"));
    //    Set any reference sequence.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 5));
    GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));
    //    Set "Show distances column" option.
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    //    Expected state: the addition column is shown, it contains distances to the reference sequence.
    QString num1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 1);
    QString num3 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 3);
    CHECK_SET_ERR(num1 == "19%", "unexpected sumilarity value an line 1: " + num1);
    CHECK_SET_ERR(num3 == "12%", "unexpected sumilarity value an line 3: " + num3);
    //    Current state: the addition column is shown, it contains sequence names.
}

GUI_TEST_CLASS_DEFINITION(test_3283) {
    //    1. Open "data/Samples/MMDB/1CRN.prt".
    GTFileDialog::openFile(os, dataDir + "samples/MMDB", "1CRN.prt");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Click to any annotation on the panoramic view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CRN chain A annotation"));
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem(os, "sec_struct  (0, 5)");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
}

GUI_TEST_CLASS_DEFINITION(test_3287) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    ImageExportFormFiller::Parameters params;
    params.fileName = testDir + "_common_data/scenarios/sandbox/test_3287.bmp";
    params.format = "BMP";
    GTUtilsDialog::add(os, new PopupChooser(os, {"export_overview_as_image_action"}));
    GTUtilsDialog::add(os, new ImageExportFormFiller(os, params));
    GTWidget::click(os, GTWidget::findWidget(os, "msa_overview_area_graph"), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QImage image(params.fileName);
    CHECK_SET_ERR(image.height() == 70, "Wrong image height");
}

GUI_TEST_CLASS_DEFINITION(test_3288) {
    // 1. Open "data/samples/CLUSTALW/HIV-1.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "HIV-1.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // 2. Click the "Build tree" button on the main toolbar.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFillerPhyML(os, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));

    // 3. Select the "PhyML" tool, set "Equilibrium frequencies" option to "optimized", build the tree
    auto taskProgressBar = GTWidget::findProgressBar(os, "taskProgressBar");
    int percent = 0;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && percent == 0; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        percent = taskProgressBar->text().replace("%", "").toInt();
        CHECK_SET_ERR(percent >= 0 && percent <= 100, "Percent must be within 0 and 100%");
    }
    GTUtilsTaskTreeView::cancelTask(os, "Calculating Phylogenetic Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state: the task progress is correct.
}

GUI_TEST_CLASS_DEFINITION(test_3305) {
    GTLogTracer logTracer;

    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Create an annotation.
    QDir().mkpath(sandBoxDir + "test_3305");
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "misc_feature", "1..5", sandBoxDir + "test_3305/test_3305.gb"));
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "create_annotation_action"));

    //    3. Call context menu on the annotations document, select the {Export/Import -> Export annotations...} menu item.
    //    4. Fill the dialog:
    //        Export to file: any acceptable path;
    //        File format: bed
    //    and accept it.
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << "ep_exportAnnotations2CSV"));
    GTUtilsDialog::add(os, new ExportAnnotationsFiller(sandBoxDir + "test_3305/test_3305.bed", ExportAnnotationsFiller::bed, os));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "test_3305.gb"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the annotation is successfully exported, result file exists, there are no errors in the log.
    QFile bedFile(sandBoxDir + "test_3305/test_3305.bed");
    CHECK_SET_ERR(bedFile.exists() && bedFile.size() != 0, "The result file is empty or does not exist!");

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3306) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::expandItem(os, "CDS  (0, 14)");
    auto geneItem = GTUtilsAnnotationsTreeView::expandItem(os, "gene  (0, 13)");
    GTUtilsAnnotationsTreeView::expandItem(os, "mat_peptide  (0, 16)");
    GTUtilsAnnotationsTreeView::expandItem(os, "misc_feature  (0, 16)");

    GTTreeWidget::click(os, geneItem);

    auto annotTreeWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    auto scrollBar = annotTreeWidget->verticalScrollBar();
    int initialPos = scrollBar->value();
    for (int i = 0; i < 50; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        if (initialPos != scrollBar->value()) {
            break;
        }
    }
    CHECK_SET_ERR(initialPos != scrollBar->value(), "ScrollBar value is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_3308) {
    //    1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Call context menu on the 3dview, select {Structural Alignment -> Align With...} menu item.
    //    3. Accept the dialog.
    //    Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(os, new StructuralAlignmentDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Structural Alignment", "align_with"}));
    auto widget3d = GTWidget::findWidget(os, "1-1CF7");
    GTWidget::click(os, widget3d, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3313) {
    // 1. Open "data/samples/CLUSTALW/ty3.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Open "Statistics" options panel tab.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_STATISTICS_WIDGET"));
    // 3. Set any reference sequence.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 5));
    GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));
    // 4. Check the "Show distances column" option.
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    // 5. Edit the msa fast enough, e.g. insert several gaps somewhere.

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(10, 10));

    for (int i = 0; i < 10; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }
    CHECK_SET_ERR(2 >= GTUtilsTaskTreeView::getTopLevelTasksCount(os), "There are several \"Generate distance matrix\" tasks");
}

GUI_TEST_CLASS_DEFINITION(test_3318) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_OP(os, );

    // 2. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_OP(os, );

    // 3. Drag the sequence to the alignment
    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_LOAD, "Sequence from current project"}));
    GTUtilsDialog::add(os, new ProjectTreeItemSelectorDialogFiller(os, "human_T1.fa", "human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Make the sequence reference.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(-5, 18));
    GTUtilsDialog::add(os, new PopupChooser(os, {"set_seq_as_reference"}));
    GTMouseDriver::click(Qt::RightButton);

    // 5. Change the highlighting mode to "Disagreements"
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    auto highlightingSchemeCombo = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingSchemeCombo, "Disagreements");

    // 6. Use the dots
    auto useDotsCheckBox = GTWidget::findCheckBox(os, "useDots");
    GTCheckBox::setChecked(os, useDotsCheckBox);

    // 7. Drag the reference sequence in the list of sequences.
    QPoint mouseDragPosition(-5, 18);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition);

    GTMouseDriver::click();
    GTGlobals::sleep(1000);  // To avoid double click.
    GTMouseDriver::press();
    for (int i = 0; i < 50; i++) {
        GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(0, -5));
    }
    GTGlobals::sleep(200);
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // Expected result: the highlighting mode is the same, human_T1 is still the reference.
    CHECK_SET_ERR(highlightingSchemeCombo->currentText() == "Disagreements", "Invalid highlighting scheme");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)"), "Unexpected reference sequence");
}

GUI_TEST_CLASS_DEFINITION(test_3319) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Reverse complement sequence
    GTKeyboardDriver::keyClick('r', Qt::ControlModifier | Qt::ShiftModifier);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 51, 102));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString clipboardText = GTClipboard::text(os);

    CHECK_SET_ERR(clipboardText == "TTTAAACCACAGGTCATGACCCAGTAGATGAGGAAATTGGTTTAGTGGTTTA", "unexpected text in clipboard: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_3321) {
    //    Open sequence
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open Circular View
    auto parent = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto CircularViewAction = GTWidget::findWidget(os, "CircularViewAction", parent);
    GTWidget::click(os, CircularViewAction);
    //    Select region that contains zero position
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, "1..10,5823..5833"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTWidget::click(os, GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0"), Qt::RightButton);
    //    Press "Ctrl+C"
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    //    Make sure that buffer contains right region
    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == "AAATGAAAGAGGTCTTTCATT", "unecpected text in clipboard: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_3328) {
    //    1. Open "test/_common_data/fasta/human_T1_cutted.fa".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click the "Find restriction sites" button on the main toolbar.
    //    3. Select a single enzyme: "AbaBGI". Start the search.
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            // 3. Press "Select by length"
            // 4. Input "7" and press "Ok"
            GTUtilsDialog::waitForDialog(os, new InputIntFiller(os, 8));
            GTWidget::click(os, GTWidget::findWidget(os, "selectByLengthButton"));

            GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites..."));

            //    4. Close the sequence view until task has finished.
            GTUtilsMdi::click(os, GTGlobals::Close);

            //    Expected state: the task is canceled.
            CHECK_SET_ERR(0 == GTUtilsTaskTreeView::getTopLevelTasksCount(os), "There are unfinished tasks");
        }
    };

    QThreadPool threadPool(this);
    QEventLoop waiter(this);

    if (GTUtilsTaskTreeView::getTopLevelTasksCount(os) != 0) {
        QString s = GTUtilsTaskTreeView::getTaskStatus(os, "Auto-annotations update task");
        //    Expected state: the task is canceled.
        CHECK_SET_ERR(s == "Canceling...", "Unexpected task status: " + s);
    }
}

GUI_TEST_CLASS_DEFINITION(test_3332) {
    // 1. Open "data/sample/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Select {Edit -> Remove columns of gaps...} menu item from the context menu.
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "remove_columns_of_gaps"));
    // 3. Select the "all-gaps columns" option and accept the dialog.
    GTUtilsDialog::add(os, new DeleteGapsDialogFiller(os, 1));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));
    // Expected state: nothing happens.
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength(os) == 604, "Wrong msa length");
}

GUI_TEST_CLASS_DEFINITION(test_3335) {
    GTLogTracer lt;

    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Create an annotation.
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "test_3335", "misc_feature", "50..100", sandBoxDir + "test_3335/annotationTable.gb"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Add", "New annotation..."});

    //    Expected state: an annotation table object appears in a new document.
    GTUtilsDocument::checkDocument(os, "annotationTable.gb");

    //    3. Rename the sequence object.
    GTUtilsProjectTreeView::rename(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", "renamed sequence");

    //    Expected state: the sequence object is renamed, object relations are correct, there are no errors in the log.
    const QModelIndex sequenceObjectIndex = GTUtilsProjectTreeView::findIndex(os, "renamed sequence");
    CHECK_SET_ERR(sequenceObjectIndex.isValid(), "Can't find the renamed sequence object");

    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsProjectTreeView::doubleClickItem(os, "Annotations");
    QWidget* relatedSequenceView = GTUtilsMdi::findWindow(os, "renamed sequence [human_T1.fa]");
    CHECK_SET_ERR(nullptr != relatedSequenceView, "A view for the related sequence was not opened");

    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(test_3342) {
    //    1. Open "human_T1.fa"
    //    2. Press "Build dotlpot" toolbar button
    //    Expected state: "DotPlot" dialog appeared
    //    3. Press "Ok" button in the dialog
    //    Expected state: Dotplot view has appeared
    //    4. Close dotplot view
    //    Expected state: "Save dot-plot" dialog has appeared
    //    5. Press "Yes" button
    //    Expected state: the view has been closed
    //    Current state: the view can't be closed

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const GTGlobals::FindOptions fo(false);
    auto dotplotWgt = GTWidget::findWidget(os, "dotplot widget", nullptr, fo);

    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os));
    GTWidget::click(os, GTWidget::findWidget(os, "build_dotplot_action_widget"));

    dotplotWgt = GTWidget::findWidget(os, "dotplot widget");

    GTUtilsDialog::waitForDialog(os, new MessageBoxNoToAllOrNo(os));
    GTWidget::click(os, GTWidget::findWidget(os, "exitButton"));

    dotplotWgt = GTWidget::findWidget(os, "dotplot widget", nullptr, fo);
    CHECK_SET_ERR(dotplotWgt == nullptr, "There should be NO dotpot widget");
}

GUI_TEST_CLASS_DEFINITION(test_3344) {
    //    Steps to reproduce:
    //    1. Open "human_T1"
    //    2. Press "Find repeats" tool button
    //    3. Choose following settings in the dialog: region="whole sequence", min repeat length=10bp
    //    4. Press "start"
    //    5. Wait until repeats finding complete
    //    Expected state: repeats finding completed and create annotations task started
    //    6. Delete "repeat_unit" annotations group
    //    Current state: UGENE hangs
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Runnable* tDialog = new FindRepeatsDialogFiller(os, testDir + "_common_data/scenarios/sandbox/test_3344.gb", false, 10);
    GTUtilsDialog::waitForDialog(os, tDialog);

    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Find repeats..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "Annotations [test_3344.gb] *"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "repeat_unit  (0, 3486)"));

    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3346) {
    GTLogTracer lt;

    QFile originalFile(dataDir + "samples/Genbank/murine.gb");
    QString dstPath = sandBoxDir + "murine.gb";
    originalFile.copy(dstPath);

    QFile copiedFile(dstPath);
    CHECK_SET_ERR(copiedFile.exists(), "Unable to copy file");

    GTFileDialog::openFile(os, sandBoxDir, "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    if (!copiedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        os.setError("Unable to open file");
        return;
    }
    QString fileData = copiedFile.readAll();
    copiedFile.close();
    fileData.replace("\"gag polyprotein\"", "\"gag polyprotein");

    if (!copiedFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        os.setError("Unable to open file");
        return;
    }

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));

    GTGlobals::sleep(1000);  // wait at least 1 second: UGENE does not detect file changes within 1 second interval.
    QTextStream out(&copiedFile);
    out << fileData;
    copiedFile.close();

    GTUtilsDialog::checkNoActiveWaiters(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_3348) {
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/", "DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new FindRepeatsDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", true, 10, 75, 100));
    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Find repeats..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "repeat_unit  (0, 39)");
    QTreeWidgetItem* generalItem = annotationGroup->child(36);
    CHECK_SET_ERR(generalItem != nullptr, "Invalid annotation tree item");

    auto annotation = dynamic_cast<AVAnnotationItem*>(generalItem);
    CHECK_SET_ERR(annotation != nullptr, "Annotation tree item not found");
    QString identityQualifierValue = annotation->annotation->findFirstQualifierValue("repeat_identity");
    CHECK_SET_ERR(identityQualifierValue == "76", "Annotation qualifier has invalid value: " + identityQualifierValue);
}

GUI_TEST_CLASS_DEFINITION(test_3357) {
    /*  1. Open file _common_data\alphabets\standard_dna_rna_amino_1000.fa
    2. Click the CV button on seq3 widget
    3. Select document in project view. Press delete key
    Expected state: UGENE doesn't crash.
*/
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/alphabets/standard_dna_rna_amino_1000.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto w = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction", w));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "standard_dna_rna_amino_1000.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_3373) {
    //    1. Launch WD
    //    2. Create the following workflow: "Read Sequence" -> "Reverse Complement" -> "Write Sequence"
    //    3. Set output format "GenBank" (to prevent warnings about annotation support) and the "result.gb" output file name
    //    4. Set input file "test/_common_data/fasta/seq1.fa"
    //    5. Run the workflow
    //    Expected state: workflow is successfully finished. "result.gb" contains reverse complement sequence for "seq1.fa"
    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Reverse Complement");
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence", true);

    WorkflowProcessItem* seqReader = GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence");
    WorkflowProcessItem* revComplement = GTUtilsWorkflowDesigner::getWorker(os, "Reverse Complement");
    WorkflowProcessItem* seqWriter = GTUtilsWorkflowDesigner::getWorker(os, "Write Sequence");

    GTUtilsWorkflowDesigner::connect(os, seqReader, revComplement);
    GTUtilsWorkflowDesigner::connect(os, revComplement, seqWriter);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "result.gb", GTUtilsWorkflowDesigner::textValue);
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/seq1.fa");

    GTWidget::click(os, GTAction::button(os, "Run workflow"));

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3379) {
    // 1. Open "_common_data/fasta/abds.fa" as separate sequences
    QStringList expectedNames;
    QList<ADVSingleSequenceWidget*> seqWidgets;
    expectedNames << "seq1"
                  << "seq3"
                  << "seq5";

    seqWidgets = GTUtilsProject::openFileExpectSequences(os,
                                                         testDir + "_common_data/alphabets/",
                                                         "standard_dna_rna_amino_1000.fa",
                                                         expectedNames);
    CHECK_OP_SET_ERR(os, "Failed to open sequences!");

    // 2. Open a few CV
    ADVSingleSequenceWidget* seq1Widget = seqWidgets.at(0);
    ADVSingleSequenceWidget* seq3Widget = seqWidgets.at(1);

    GTUtilsCv::cvBtn::click(os, seq3Widget);
    GTUtilsCv::cvBtn::click(os, seq1Widget);

    auto circularView = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    CHECK_OP_SET_ERR(os, "Failed to open circular view!");

    // 3.Add more files to the project and open a few more views
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/", "DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Return to 'abcd.fa' view
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "seq1"));
    GTMouseDriver::doubleClick();

    // 5. Try to launch Export dialog using context menu
    GTUtilsDialog::waitForDialog(os, new CircularViewExportImage(os, testDir + "_common_data/scenarios/sandbox/image.jpg"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_EXPORT, "Save circular view as image"}, GTGlobals::UseMouse));

    GTWidget::click(os, circularView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3384) {
    GTLogTracer l;
    //    Open sequence data/samples/Genbank/murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open CV
    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction"));
    //    Insert at least one symbol to the sequence
    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EDIT", "action_edit_insert_sub_sequences"}));
    GTUtilsDialog::add(os, new InsertSequenceFiller(os, "A"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getDetViewByNumber(os));

    //    Select an area on CV that contains zero position
    auto cv = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    GTWidget::click(os, cv);
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(20, -20));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(0, 40));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    //    Current state: SAFE_POINT triggers and selection is "beautiful" (see the attachment)
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3398_1) {
    //    1. Open "_common_data/fasta/broken/data_in_the_name_line.fa".
    //    2. Select "As separate sequences" mode.
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/broken/data_in_the_name_line.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: an unloaded document appears, there are no objects within.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Document* doc = GTUtilsDocument::getDocument(os, "data_in_the_name_line.fa");
    CHECK_SET_ERR(!doc->isLoaded(), "Document is unexpectedly loaded");

    //    3. Call context menu on the document.
    //    Expected state: UGENE doesn't crash, a context menu is shown.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"action_load_selected_documents"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "data_in_the_name_line.fa"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3398_2) {
    //    1. Open "_common_data/fasta/broken/data_in_the_name_line.fa".
    //    2. Select "As separate sequences" mode.
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge, 10));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/broken/data_in_the_name_line.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: an unloaded document appears, there are no objects within.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Document* doc = GTUtilsDocument::getDocument(os, "data_in_the_name_line.fa");
    CHECK_SET_ERR(!doc->isLoaded(), "Document is unexpectedly loaded");

    //    3. Call context menu on the document.
    //    Expected state: UGENE doesn't crash, a context menu is shown.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"action_load_selected_documents"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "data_in_the_name_line.fa"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3398_3) {
    //    1. Open "_common_data/fasta/broken/data_in_the_name_line.fa".
    //    2. Select "Merge into one sequence" mode, set 10 'unknown' symbols.
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge, 0));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/broken/data_in_the_name_line.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: an unloaded document appears, there are no objects within.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Document* doc = GTUtilsDocument::getDocument(os, "data_in_the_name_line.fa");
    CHECK_SET_ERR(!doc->isLoaded(), "Document is unexpectedly loaded");

    //    3. Call context menu on the document.
    //    Expected state: UGENE doesn't crash, a context menu is shown.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"action_load_selected_documents"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "data_in_the_name_line.fa"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3398_4) {
    //    1. Open "_common_data/fasta/broken/data_in_the_name_line.fa".
    //    2. Select "As separate sequences" mode.
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/broken/data_in_the_name_line.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: an unloaded document appears, there are no objects within.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Document* doc = GTUtilsDocument::getDocument(os, "data_in_the_name_line.fa");
    CHECK_SET_ERR(!doc->isLoaded(), "Document is unexpectedly loaded");

    //    3. Call context menu on the document.
    //    Expected state: UGENE doesn't crash, a context menu is shown.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"action_load_selected_documents"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "data_in_the_name_line.fa"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_3437) {
    //    1. Open file test/_common_data/fasta/empty.fa
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected: file opened in msa editor
    GTWidget::findWidget(os, "msa_editor_sequence_area");
}

GUI_TEST_CLASS_DEFINITION(test_3402) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "3000_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);

    //  Call context menu on the "100_sequences" object.
    GTUtilsDialog::waitForDialog(os, new ExportToSequenceFormatFiller(os, sandBoxDir, "test_3402.fa", ExportToSequenceFormatFiller::FASTA, true, true));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"action_project__export_import_menu_action", "action_project__export_as_sequence_action"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "3000_sequences.aln"));
    GTMouseDriver::click(Qt::RightButton);

    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    bool isTaskFound = false;
    while (!isTaskFound) {
        QList<Task*> topLevelTasks = scheduler->getTopLevelTasks();
        if (topLevelTasks.isEmpty()) {
            continue;
        }
        for (const Task* task : qAsConst(topLevelTasks)) {
            if (task != nullptr && task->getTaskName().contains("Opening view")) {
                isTaskFound = true;
                break;
            }
        }
        // TODO: replace this method with a helper method that waits until task by name.
        GTGlobals::sleep(10);
    }

    // Expected state: the fasta document is present in the project, open view task is in progress.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "test_3402.fa"));
    // Delete the fasta document from the project.
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Current state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_3414) {
    /*
     Check time is updated on the dashboard
        - Open WD.
        - Run Align with MUSCLE.
        - Execute workflow.
        - Check elapsed time is changed.
    */
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click(os, "Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/PF07724_full_family.fa");
    GTUtilsWorkflowDesigner::runWorkflow(os);

    auto timeLabel = GTWidget::findLabel(os, "timeLabel", GTUtilsDashboard::getDashboard(os));
    QString timeBefore = timeLabel->text();
    GTGlobals::sleep(3000);  // Wait for label to change.
    QString timeAfter = timeLabel->text();
    CHECK_SET_ERR(timeBefore != timeAfter, "timer is not changed, timeBefore: " + timeBefore + ", timeAfter: " + timeAfter);
    GTUtilsTaskTreeView::cancelTask(os, "Execute workflow");
}

GUI_TEST_CLASS_DEFINITION(test_3428) {
    //    1. Add element with unset parameter to the scene
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Annotations");
    // QRect r = GTUtilsWorkflowDesigner::getItemRect(os, "Read Annotations");
    GTUtilsWorkflowDesigner::clickLink(os, "Read Annotations");
    //    2. Click on the "unset" parameter of the element.
    //    UGENE not crashes
}

GUI_TEST_CLASS_DEFINITION(test_3430) {
    // 1. Open "_common_data/alphabets/standard_dna_rna_amino_1000.fa" as separate sequences
    QStringList expectedNames;
    QList<ADVSingleSequenceWidget*> seqWidgets;
    expectedNames << "seq1"
                  << "seq3"
                  << "seq5";

    seqWidgets = GTUtilsProject::openFileExpectSequences(os,
                                                         testDir + "_common_data/alphabets/",
                                                         "standard_dna_rna_amino_1000.fa",
                                                         expectedNames);
    CHECK_OP_SET_ERR(os, "Failed to open sequences!");

    // 2. Open one circular view
    ADVSingleSequenceWidget* seq1Widget = seqWidgets.at(0);

    GTUtilsCv::cvBtn::click(os, seq1Widget);

    auto circularView1 = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    CHECK_OP_SET_ERR(os, "Failed to open circular view!");

    auto circularView2 = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_1", nullptr, {false});
    CHECK_SET_ERR(nullptr == circularView2, "Unexpected circular view is opened!");

    // 3. Press "Toggle circular views" button

    GTWidget::click(os, GTWidget::findWidget(os, "globalToggleViewAction_widget"));

    circularView1 = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0", nullptr, {false});
    CHECK_SET_ERR(nullptr == circularView1, "Unexpected circular view is opened!");

    circularView2 = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_1", nullptr, {false});
    CHECK_SET_ERR(nullptr == circularView2, "Unexpected circular view is opened!");
    // 4. Press "Toggle circular views" again
    GTWidget::click(os, GTWidget::findWidget(os, "globalToggleViewAction_widget"));

    GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    CHECK_OP_SET_ERR(os, "Failed to open circular view!");

    GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_1");
    CHECK_OP_SET_ERR(os, "Failed to open circular view!");
}

GUI_TEST_CLASS_DEFINITION(test_3439) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Alignment");

    // Validate workflow
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::checkErrorList(os, "Write Alignment") == 1, "Errors count dont match, should be 1 validation error");
}

GUI_TEST_CLASS_DEFINITION(test_3441) {
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Open "Statistics" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_GENERAL"));

    // Sequence count is 2.
    auto seqCountLabel = GTWidget::findLabel(os, "alignmentHeight");
    CHECK_SET_ERR(seqCountLabel->text() == "2", "Sequence count don't match: " + seqCountLabel->text());
}

GUI_TEST_CLASS_DEFINITION(test_3443) {
    GTKeyboardDriver::keyClick('3', Qt::AltModifier);

    auto logViewWidget = GTWidget::findWidget(os, "dock_log_view");
    CHECK_SET_ERR(logViewWidget->isVisible(), "Log view is expected to be visible");

    GTKeyboardDriver::keyClick('3', Qt::AltModifier);
    CHECK_SET_ERR(!logViewWidget->isVisible(), "Log view is expected to be visible");

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto projectViewWidget = GTWidget::findWidget(os, "project_view");

    GTKeyboardDriver::keyClick('1', Qt::AltModifier);

    CHECK_SET_ERR(!projectViewWidget->isVisible(), "Project view is expected to be invisible");

    GTKeyboardDriver::keyClick('1', Qt::AltModifier);

    CHECK_SET_ERR(projectViewWidget->isVisible(), "Project view is expected to be visible");

    GTKeyboardDriver::keyClick('2', Qt::AltModifier);

    auto taskViewWidget = GTWidget::findWidget(os, "dock_task_view");
    CHECK_SET_ERR(taskViewWidget->isVisible(), "Task view is expected to be visible");

    GTKeyboardDriver::keyClick('2', Qt::AltModifier);

    CHECK_SET_ERR(!taskViewWidget->isVisible(), "Task view is expected to be invisible");

    GTKeyboardDriver::keyClick('b', Qt::ControlModifier);

    auto codonTableWidget = GTWidget::findWidget(os, "Codon table widget");
    CHECK_SET_ERR(codonTableWidget->isVisible(), "Codon table is expected to be visible");

    GTKeyboardDriver::keyClick('b', Qt::ControlModifier);

    CHECK_SET_ERR(!codonTableWidget->isVisible(), "Codon table is expected to be invisible");
}

GUI_TEST_CLASS_DEFINITION(test_3450) {
    //    1. Open file "COI.aln"
    //    2. Open "Highlighting" options panel tab
    //    3. Set reference sequence
    //    4. Set highlighting scheme
    //    5. Press "Export" button in the tab
    //    Expected state: "Export highlighted to file" dialog appeared, there is default file in "Export to file"
    //    6. Delete the file path and press "Export" button
    //    Expected state: message box appeared
    //    6. Set the result file and press "Export" button
    //    Current state: file is empty, but error is not appeared

    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    GTWidget::click(os, GTWidget::findWidget(os, "sequenceLineEdit"));
    GTKeyboardDriver::keySequence("Montana_montana");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    auto combo = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, combo, "Agreements");

    auto exportButton = GTWidget::findWidget(os, "exportHighlightning");

    class ExportHighlightedDialogFiller : public Filler {
    public:
        ExportHighlightedDialogFiller(HI::GUITestOpStatus& os)
            : Filler(os, "ExportHighlightedDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto filePath = GTWidget::findLineEdit(os, "fileNameEdit", dialog);
            CHECK_SET_ERR(!GTLineEdit::copyText(os, filePath).isEmpty(), "Default file path is empty");
            GTLineEdit::setText(os, filePath, "");

            QPushButton* exportButton = dialog->findChild<QPushButton*>();
            CHECK_SET_ERR(exportButton != nullptr, "ExportButton is NULL");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Ok"));
            GTWidget::click(os, exportButton);

            GTLineEdit::setText(os, filePath, sandBoxDir + "test_3450_export_hl.txt");
            GTWidget::click(os, exportButton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ExportHighlightedDialogFiller(os));
    GTWidget::click(os, exportButton);

    CHECK_SET_ERR(GTFile::getSize(os, sandBoxDir + "test_3450_export_hl.txt") != 0, "Exported file is empty!");
}

GUI_TEST_CLASS_DEFINITION(test_3451) {
    //    1. Open file "COI.aln"
    //    2. Open "Highlighting" options panel tab
    //    3. Set reference sequence
    //    4. Set highlighting scheme
    //    5. Press "Export" button in the tab
    //    Expected state: "Export highlighted to file" dialog appeared, there is default file in "Export to file"
    //    6. Set the "from" as 5, "to" as 6
    //    7. Set the "to" as 5
    //    Expected state: "from" is 4

    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    GTWidget::click(os, GTWidget::findWidget(os, "sequenceLineEdit"));
    GTKeyboardDriver::keySequence("Montana_montana");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    auto combo = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, combo, "Agreements");

    auto exportButton = GTWidget::findWidget(os, "exportHighlightning");

    class CancelExportHighlightedDialogFiller : public Filler {
    public:
        CancelExportHighlightedDialogFiller(HI::GUITestOpStatus& os)
            : Filler(os, "ExportHighlightedDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto startPos = GTWidget::findSpinBox(os, "startLineEdit", dialog);

            auto endPos = GTWidget::findSpinBox(os, "endLineEdit", dialog);

            GTSpinBox::checkLimits(os, startPos, 1, 604);
            GTSpinBox::checkLimits(os, endPos, 1, 604);

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CancelExportHighlightedDialogFiller(os));
    GTWidget::click(os, exportButton);
}

GUI_TEST_CLASS_DEFINITION(test_3452) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Find the annotation: NC_001363 -> CDS (0, 4) -> CDS.
    // Expected state: qualifiers of CDS are shown.
    QTreeWidgetItem* item1 = GTUtilsAnnotationsTreeView::findItem(os, "CDS");
    QTreeWidgetItem* item2 = GTUtilsAnnotationsTreeView::findItem(os, "comment");
    QPoint p1 = GTTreeWidget::getItemCenter(os, item1);
    QPoint pQual(p1.x(), p1.y() + 80);
    QPoint p2 = GTTreeWidget::getItemCenter(os, item2);

    // 3. Select the annotaions and its several qualifiers.
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::click();
    GTMouseDriver::moveTo(pQual);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // 4. Try to drag selected annotaions.
    GTMouseDriver::dragAndDrop(p1, p2);
    // Expected state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_3455) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Activate samples.
    GTUtilsWorkflowDesigner::setCurrentTab(os, GTUtilsWorkflowDesigner::samples);

    // 3. Choose a sample (but not open it).
    QTreeWidgetItem* sample = GTUtilsWorkflowDesigner::findTreeItem(os, "call variants", GTUtilsWorkflowDesigner::samples);
    sample->parent()->setExpanded(true);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, sample));
    GTMouseDriver::click();
    CHECK_OP(os, );

    // 4. Load any workflow.
    QString schemaPath = testDir + "_common_data/scenarios/workflow designer/222.uwl";
    GTUtilsWorkflowDesigner::loadWorkflow(os, schemaPath);
    CHECK_OP(os, );

    // Expected: the elements tab is active.
    GTUtilsWorkflowDesigner::tab current = GTUtilsWorkflowDesigner::currentTab(os);
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::algorithms == current, "Samples tab is active");
}

GUI_TEST_CLASS_DEFINITION(test_3471) {
    //    1. Open "_common_data\bam\1.bam"
    //    Expected state: "Import BAM file dialog" appeared
    //    2. Press "Import" button in the dialog
    //    Expected state: assembly is empty, there is text "Assembly has no mapped reads. Nothing to visualize."
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_3471/test_3471.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Add bookmark
    //    Expected state: UGENE doesn't crash
    GTUtilsBookmarksTreeView::addBookmark(os, GTUtilsMdi::activeWindow(os)->windowTitle(), "test_3471");
}

GUI_TEST_CLASS_DEFINITION(test_3472) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Conocephalus_discolor");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Conocephalus_sp.");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));

    auto outputFilePathEdit = GTWidget::findLineEdit(os, "outputFileLineEdit");

    GTWidget::setFocus(os, outputFilePathEdit);
#ifndef Q_OS_DARWIN
    GTKeyboardDriver::keyClick(Qt::Key_Home);
#else
    GTKeyboardDriver::keyClick(Qt::LeftArrow, Qt::ControlModifier);
#endif

    GTKeyboardDriver::keySequence("///123/123/123");

    int deleteCounter = 100;
    while (--deleteCounter > 0) {
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
    }

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::setFocus(os, outputFilePathEdit);
#ifndef Q_OS_DARWIN
    GTKeyboardDriver::keyClick(Qt::Key_Home);
#else
    GTKeyboardDriver::keyClick(Qt::LeftArrow, Qt::ControlModifier);
#endif

    GTKeyboardDriver::keySequence(sandBoxDir + "123/123/123/1.aln");

    deleteCounter = 15;
    while (--deleteCounter > 0) {
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
    }

    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));

    QString expected = "TTAGCTTATTAATT\n"
                       "TTAGCTTATTAATT";
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(0, 0), QPoint(13, 1), expected);
}

GUI_TEST_CLASS_DEFINITION(test_3473) {
    /*  1. Open "human_T1"
    2. Press "Show circular view" tool button
        Expected state: circular view appeared and button's hint change to "Remove circular view"
    3. Press the button again
        Expected state: the hint is "Show circular view"
*/
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto a = GTWidget::findToolButton(os, "CircularViewAction");
    CHECK_SET_ERR(a->toolTip() == "Show circular view", QString("Unexpected tooltip: %1, must be %2").arg(a->toolTip()).arg("Show circular view"));
    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction"));
    CHECK_SET_ERR(a->toolTip() == "Remove circular view", QString("Unexpected tooltip: %1, must be %2").arg(a->toolTip()).arg("Remove circular view"));
}

GUI_TEST_CLASS_DEFINITION(test_3477) {
    //    1. Open "data/samples/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Press "Find restriction sites" tool button.
    //    Expected state: "Find restriction sites" dialog appeared.
    //    3. Select enzyme "T(1, 105) -> TaaI" and accept the dialog.
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, {"TaaI"}));
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    4. Select {Tools -> Cloning -> Digest into Fragments...} menu item in the main menu.
    //    Expected state: "Digest sequence into fragments" dialog appeared.
    //    5. Add "TaaI" to selected enzymes and accept the dialog.
    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os));
    GTMenu::clickMainMenuItem(os, {"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    6. Select {Tools -> Cloning -> Construct molecule} menu item in the main menu.
    //    Expected state: "Construct molecule" dialog appeared.
    //    7. Press "Add all" button.
    //    9.Press several times to checkbox "Inverted" for any fragment.
    //    Expected state: checkbox's state updates on every click, UGENE doesn't crash.
    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddAllFragments, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::InvertAddedFragment, "Fragment 1");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickCancel, "");

    GTUtilsDialog::waitForDialog(os, new ConstructMoleculeDialogFiller(os, actions));
    GTMenu::clickMainMenuItem(os, {"Tools", "Cloning", "Construct molecule..."});
}

GUI_TEST_CLASS_DEFINITION(test_3480) {
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));

    GTFileDialog::openFile(os, testDir + "_common_data/bwa/workflow/", "bwa-mem.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Align reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Library", 0, GTUtilsWorkflowDesigner::comboValue);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3484) {
    //    1. Open an alignment
    //    2. Build the tree
    //    3. Unload both documents (alignment and tree)
    //    4. Delete the tree document from project
    //    5. Load alignment
    //    Expected state: only alignment is opened.
    GTFile::copy(os, dataDir + "samples/CLUSTALW/COI.aln", testDir + "_common_data/scenarios/sandbox/COI_3484.aln");

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "COI_3484.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI_3484.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that tree is visible.
    GTWidget::findGraphicsView(os, "treeView");

    GTUtilsDocument::unloadDocument(os, "COI_3484.nwk", false);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::unloadDocument(os, "COI_3484.aln", true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::removeDocument(os, "COI_3484.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::loadDocument(os, "COI_3484.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "COI_3484  .nwk", false) == false, "Unauthorized tree opening!");
}

GUI_TEST_CLASS_DEFINITION(test_3484_1) {
    //    1. Open alignment
    //    2. Build the tree
    //    Current state: tree and alignment are both in the project, loaded and visualized.
    //    3. Save the file and the project, relaunch UGENE (or close and reload project)
    //    4. Open project, open alignment
    //    Current state: tree and alignment are both in the project, loaded and visualized.
    //    5. Delete the tree document from the project
    //    6. Save the alignment
    //    7. Save the project
    //    8. Relaunch UGENE and open the project
    //    9. Load the alignment
    //    Current state: tree document is added to the project, both documents are loaded.
    //    Expected state: only alignment is loaded.

    GTFile::copy(os, dataDir + "samples/CLUSTALW/COI.aln", testDir + "_common_data/scenarios/sandbox/COI_3484_1.aln");

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "COI_3484_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/COI_3484_1.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SaveProjectAsDialogFiller(os, "proj_3484_1", testDir + "_common_data/scenarios/sandbox/proj_3484_1"));
    GTMenu::clickMainMenuItem(os, {"File", "Save project as..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/sandbox/", "proj_3484_1.uprj"));
    GTMenu::clickMainMenuItem(os, {"File", "Open..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::loadDocument(os, "COI_3484_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::removeDocument(os, "COI_3484_1.nwk");
    GTMenu::clickMainMenuItem(os, {"File", "Save all"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/sandbox/", "proj_3484_1.uprj"));
    GTMenu::clickMainMenuItem(os, {"File", "Open..."});

    GTUtilsDocument::loadDocument(os, "COI_3484_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "COI_3484_1.nwk", {false}) == false, "Unauthorized tree opening!");
}

GUI_TEST_CLASS_DEFINITION(test_3504) {
    //    1. Open COI.aln
    //    2. Build the tree and open it with the alignment.
    //    Expected state: Tree view has horizontal scroll bar
    //    3. Change tree layout to unrooted.
    //    4. Change layout back to rectangular
    //    Bug state: tree view has no horizontal scroll bar.
    //    Expected state: horizontal scroll bar is present

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "COI_3504.nwk", 0, 0, true));
    QAbstractButton* tree = GTAction::button(os, "Build Tree");
    GTWidget::click(os, tree);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QScrollBar* scroll = treeView->horizontalScrollBar();
    CHECK_SET_ERR(scroll != nullptr, "TreeView does not have a horisontal scroll bar");
    CHECK_SET_ERR(scroll->isVisible(), "Horisontal scroll bar is hidden");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Unrooted"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Layout"));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Rectangular"}));
    GTWidget::click(os, GTWidget::findWidget(os, "Layout"));

    scroll = treeView->horizontalScrollBar();
    CHECK_SET_ERR(scroll != nullptr, "TreeView does not have a horisontal scroll bar");
    CHECK_SET_ERR(scroll->isVisible(), "Horisontal scroll bar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_3518) {
    //    1. Select {Tools -> Weight matrix -> Build Weight Matrix} menu item in the main menu.
    //    2. Set file "data/samples/GFF/5prime_utr_intron_A20.gff" as input.
    //    Expected state: the dialog process the file and notify user if the file is inappropriate, UGENE doesn't crash.
    QList<PwmBuildDialogFiller::Action> actions;
    actions << PwmBuildDialogFiller::Action(PwmBuildDialogFiller::ExpectInvalidFile, "");
    actions << PwmBuildDialogFiller::Action(PwmBuildDialogFiller::SelectInput, dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    actions << PwmBuildDialogFiller::Action(PwmBuildDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(os, new PwmBuildDialogFiller(os, actions));

    GTMenu::clickMainMenuItem(os, {"Tools", "Search for TFBS", "Build weight matrix..."});
}

GUI_TEST_CLASS_DEFINITION(test_3519_1) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open a "SITECON Search" dialog, fill it and start the search.
    //    Expected state: the dialog is opened, there are search results.
    //    3. Click the "Save as annotations" button and then click the "Search" button again before all annotations are drawn.
    //    4. Try to close the dialog.
    //    Expected state: the dialog is closed, the search task is canceled.
    //    Current state: GUI waits until all annotations are drawn, then react on the button click.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class SiteconCustomFiller : public Filler {
    public:
        SiteconCustomFiller(HI::GUITestOpStatus& os)
            : Filler(os, "SiteconSearchDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "/sitecon_models/eukaryotic", "CLOCK.sitecon.gz"));
            auto modelButton = GTWidget::findWidget(os, "pbSelectModelFile", dialog);
            GTWidget::click(os, modelButton);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsTaskTreeView::waitTaskFinished(os);

            GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "sitecon_ann", ""));
            auto saveButton = GTWidget::findWidget(os, "pbSaveAnnotations", dialog);
            GTWidget::click(os, saveButton);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SiteconCustomFiller(os));
    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Find TFBS with SITECON..."}, GTGlobals::UseMouse);

    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 0, "Some task is still running");
}

GUI_TEST_CLASS_DEFINITION(test_3519_2) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Run "Auto-annotations update" task, e.g. find tandems with default parameters.
    //    3. Open a "SITECON Search" dialog before "auto-annotations update" task finish, fill it and start the search.
    //    Current state: a deadlock occurs: "auto-annotations update" task wait until the dialog close,
    //                   dialog can't be closed until the search task finish,
    //                   the search task waits until the "auto-annotation update" task finish.

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "Mycobacterium.fna");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class SiteconCustomFiller : public Filler {
    public:
        SiteconCustomFiller(HI::GUITestOpStatus& os)
            : Filler(os, "SiteconSearchDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "/sitecon_models/eukaryotic", "CLOCK.sitecon.gz"));
            auto modelButton = GTWidget::findWidget(os, "pbSelectModelFile", dialog);
            GTWidget::click(os, modelButton);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 2, QString("Incorrect top-level task counts: %1").arg(GTUtilsTaskTreeView::getTopLevelTasksCount(os)));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    qputenv("UGENE_DISABLE_ENZYMES_OVERFLOW_CHECK", "1");  // disable overflow to create a long running "Find Enzymes task".
    class AllEnzymesSearchScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto selectAllButton = GTWidget::findWidget(os, "selectAllButton", dialog);
            GTWidget::click(os, selectAllButton);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList(), new AllEnzymesSearchScenario()));
    GTWidget::click(os, GTWidget::findWidget(os, "Find restriction sites_widget"));

    GTGlobals::sleep(1000);  // Wait for the task to run.

    GTUtilsTaskTreeView::openView(os);
    GTUtilsDialog::waitForDialog(os, new SiteconCustomFiller(os));
    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Find TFBS with SITECON..."});

    GTUtilsTaskTreeView::checkTaskIsPresent(os, "SITECON search", false);
    GTUtilsTaskTreeView::cancelTask(os, "Auto-annotations update task");
    GTUtilsTaskTreeView::waitTaskFinished(os, 60000);
}

GUI_TEST_CLASS_DEFINITION(test_3545) {
    //    Open "_common_data\scenarios\msa\big.aln"
    GTFile::copy(os, testDir + "_common_data/scenarios/msa/big_3.aln", sandBoxDir + "big_3.aln");
    GTFileDialog::openFile(os, sandBoxDir, "big_3.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //    Use context menu
    //    {Add->Sequence from file}
    //    Expected state: "Open file with sequence" dialog appeared
    //    Select sequence "_common_data\fasta\NC_008253.fna" and press "Open"
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta", "NC_008253.fna"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));

    // Close MSAEditor
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Save document "big.aln": expected state: UGENE does not crash.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__SAVE_DOCUMENT));
    GTUtilsProjectTreeView::click(os, "big_3.aln", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3552) {
    // Check that "Render overview" task has a meaningful progress info: NN% instead of ? symbol).
    // Open a large alignment.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/10000_sequences.aln");

    // Wait until 'Render overview' task is started.
    auto statusBar = GTWidget::findWidget(os, "taskStatusBar");
    GTWidget::findLabelByText(os, "Running task: Render overview", statusBar);

    // Check progress bar text.
    QString taskProgressBarText = GTWidget::findProgressBar(os, "taskProgressBar", statusBar)->text();
    CHECK_SET_ERR(taskProgressBarText.contains("%"), "Unexpected progress bar text: " + taskProgressBarText);
    GTUtilsTaskTreeView::waitTaskFinished(os, 10000);
}

GUI_TEST_CLASS_DEFINITION(test_3553) {
    // 1. Open "_common_data/clustal/big.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select both sequences.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(1, 0), QPoint(1, 1));

    // 3. Open the "Pairwise Alignment" OP tab. Wait for overview to re-render.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Press the "Align" button several times(~5).
    for (int i = 0; i < 5; i++) {
        GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));
    }
    // Expected: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_3555) {
    //    1. Open "_common_data\muscul4\prefab_1_ref.aln"
    //    2. Press "Switch on/off collapsing" tool button
    //    3. Scroll down sequences
    //    Expected state: scrolling will continue until the last sequence becomes visible
    //    Current state: see the attachment
    //    4. Click on the empty space below "1a0cA" sequence
    //    Current state: SAFE_POINT in debug mode and incorrect selection in release(see the attachment)
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/muscul4/", "prefab_1_ref.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    GTUtilsMSAEditorSequenceArea::scrollToBottom(os);

    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor* editor = mw->findChild<MSAEditor*>();
    CHECK_SET_ERR(editor != nullptr, "MsaEditor not found");

    MaEditorNameList* nameList = editor->getUI()->getEditorNameList();
    CHECK_SET_ERR(nameList != nullptr, "MSANameList is empty");
    GTWidget::click(os, nameList, Qt::LeftButton, QPoint(10, nameList->height() - 1));

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3556) {
    // 1. Open "_common_data/muscul4/prefab_1_ref.aln".
    // 2. Press the "Switch on/off collapsing" tool button.
    // 3. Select the sequence "1a0cA".
    // 4. Context menu: {Set with sequence as reference}.
    // Expected state: the sequence became reference.

    GTFileDialog::openFile(os, testDir + "_common_data/muscul4/", "prefab_1_ref.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::toggleCollapsingMode(os);

    GTUtilsMSAEditorSequenceArea::selectSequence(os, "1a0dA");
    GTKeyboardDriver::keyClick(Qt::Key_End, Qt::ControlModifier);
    GTUtilsMsaEditor::clickSequenceName(os, "1a0cA");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"set_seq_as_reference"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);

    QString referenceName = GTLineEdit::getText(os, "sequenceLineEdit");
    CHECK_SET_ERR(referenceName == "1a0cA", "Wrong reference sequence: " + referenceName);
}

GUI_TEST_CLASS_DEFINITION(test_3557) {
    GTFileDialog::openFile(os, testDir + "_common_data/muscul4/", "prefab_1_ref.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // "Switch on/off collapsing" tool button.
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Switch on/off collapsing");

    GTUtilsMsaEditor::clickSequenceName(os, "1a0dA");

    // Scroll to end of the list.
    GTKeyboardDriver::keyClick(Qt::Key_End, Qt::ControlModifier);

    // Select another sequence.
    GTUtilsMsaEditor::clickSequenceName(os, "2|1a0cA|gi|32470780");

    // Hold SHIFT & select +1 sequence.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName(os, "1a0cA");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    // Expected: "2|1a0cA|gi|32470780" and "1a0cA" are in the OP.
    QString firstRowName = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(os, 1);
    QString secondRowName = GTUtilsOptionPanelMsa::getSeqFromPAlineEdit(os, 2);
    QString expectedFirstRowName = "2|1a0cA|gi|32470780";
    QString expectedSecondRowName = "1a0cA";
    CHECK_SET_ERR(firstRowName == expectedFirstRowName, QString("Wrong first sequence: expected '%1', got '%2'").arg(expectedFirstRowName).arg(firstRowName));
    CHECK_SET_ERR(secondRowName == expectedSecondRowName, QString("Wrong second sequence: expected '%1', got '%2'").arg(expectedSecondRowName).arg(secondRowName));
}

GUI_TEST_CLASS_DEFINITION(test_3563_1) {
    //    1. Open an alignment
    //    2. Build the tree
    //    3. Unload both documents (alignment and tree)
    //    4. Load alignment
    //    Expected state: no errors in the log
    GTLogTracer logTracer;

    GTFile::copy(os, testDir + "_common_data/clustal/dna.fasta.aln", testDir + "_common_data/scenarios/sandbox/test_3563_1.aln");
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "test_3563_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::buildPhylogeneticTree(os, testDir + "_common_data/scenarios/sandbox/test_3563_1.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::unloadDocument(os, "test_3563_1.nwk", false);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::unloadDocument(os, "test_3563_1.aln", true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::loadDocument(os, "test_3563_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3563_2) {
    //    1. Open "human_T1.fa"
    //    2. Open "GFF/5prime_utr_intron_A21.gff"
    //    3. Drag and drop "Ca21 chr5 features" to "human_T1"
    //    4. Unload both documents
    //    5. Load "human_T1.fa" document
    //    6. Load "GFF/5prime_utr_intron_A21.gff" document
    //    Expected state: no errors in the log
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/GFF/", "5prime_utr_intron_A21.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex idxGff = GTUtilsProjectTreeView::findIndex(os, "Ca21chr5 features");
    CHECK_SET_ERR(idxGff.isValid(), "Can not find 'Ca21 chr5 features' object");
    auto seqArea = GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Yes"));
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idxGff, seqArea);

    GTUtilsDocument::unloadDocument(os, "human_T1.fa");
    GTUtilsDocument::unloadDocument(os, "5prime_utr_intron_A21.gff", false);

    GTUtilsDocument::loadDocument(os, "human_T1.fa");
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded(os, "5prime_utr_intron_A21.gff"),
                  "Connection between documents was lost");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3571_1) {
    // 1. Open file "test/_common_data/fasta/numbers_in_the_middle.fa" in sequence view
    class Custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto separateRB = GTWidget::findRadioButton(os, "separateRB", dialog);
            GTRadioButton::click(os, separateRB);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
        }
    };
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, new Custom()));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/numbers_in_the_middle.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select first sequence
    ADVSingleSequenceWidget* firstSeqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os, 0);
    GTWidget::click(os, firstSeqWidget);

    // 3. Open statistics option panel tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    // Expected state : only length info appears
    GTWidget::findWidget(os, "ArrowHeader_Common Statistics");

    GTGlobals::FindOptions widgetFindSafeOptions(false);
    auto charOccurWidget = GTWidget::findWidget(os, "ArrowHeader_Characters Occurrence", nullptr, widgetFindSafeOptions);
    CHECK_SET_ERR(!charOccurWidget->isVisible(), "Character Occurrence section is unexpectedly visible");

    // 4. Select second sequence
    ADVSingleSequenceWidget* secondSeqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os, 1);
    GTWidget::click(os, secondSeqWidget);

    // Expected state : length and characters occurrence info appears
    GTWidget::findWidget(os, "ArrowHeader_Common Statistics");
    charOccurWidget = GTWidget::findWidget(os, "ArrowHeader_Characters Occurrence");
}

GUI_TEST_CLASS_DEFINITION(test_3571_2) {
    // 1. Open file test/_common_data/fasta/numbers_in_the_middle.fa in sequence view
    class Custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto separateRB = GTWidget::findRadioButton(os, "separateRB", dialog);
            GTRadioButton::click(os, separateRB);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
        }
    };
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, new Custom()));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/numbers_in_the_middle.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select second sequence
    ADVSingleSequenceWidget* secondSeqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os, 1);
    GTWidget::click(os, secondSeqWidget);

    // 3. Open statistics option panel tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    // Expected state : length and characters occurrence info appears
    GTWidget::findWidget(os, "ArrowHeader_Common Statistics");
    auto charOccurWidget = GTWidget::findWidget(os, "ArrowHeader_Characters Occurrence");

    // 4. Select first sequence
    ADVSingleSequenceWidget* firstSeqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os, 0);
    GTWidget::click(os, firstSeqWidget);

    // Expected state : only length info appears
    GTWidget::findWidget(os, "ArrowHeader_Common Statistics");
    GTGlobals::FindOptions widgetFindSafeOptions(false);
    charOccurWidget = GTWidget::findWidget(os, "ArrowHeader_Characters Occurrence", nullptr, widgetFindSafeOptions);
    CHECK_SET_ERR(!charOccurWidget->isVisible(), "Character Occurrence section is unexpectedly visible");
}

GUI_TEST_CLASS_DEFINITION(test_3589) {
    // 0. Copy "data/samples/Assembly/chrM.sam" to a new folder to avoid UGENE conversion cache.
    // 1. Create a workflow: Read assembly.
    // 2. Set an input file: that copied chrM.sam.
    // 3. Run the workflow.
    // Expected state: there are warnings about header in log and dashboard.

    QString dirPath = sandBoxDir + "test_3589_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH.mm.ss") + "/";
    QDir().mkpath(dirPath);
    GTFile::copy(os, dataDir + "samples/Assembly/chrM.sam", dirPath + "chrM.sam");

    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement(os, "Read NGS Reads Assembly");
    CHECK_SET_ERR(read != nullptr, "Added workflow element is NULL");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dirPath + "chrM.sam");

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.getJoinedErrorString().contains("There is no header in the SAM file"), "No warnings about header");
    int errorsCount = GTUtilsLog::getErrors(os, l).size();
    // Initial warning and dashboard problem
    CHECK_SET_ERR(errorsCount == 2, "Invalid errors count. Expected 2, got: " + QString::number(errorsCount) + "\nErrors: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_3603) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Open "Find Pattern" options panel tab.
    //    3. Set "Selected" region type.
    //    4. Call context menu on the sequence view, and select "Select sequence regions...".
    //    5. Accept the dialog with default values (a single region, from min to max).
    //    Expected state: the region selector widget contains "Selected" region type, region is (1..199950).
    //    Current state: the region selector widget contains "Selected" region type, region is (1..199951).
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));
    auto regionComboBox = GTWidget::findComboBox(os, "boxRegion");
    if (!regionComboBox->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    }
    GTComboBox::selectItemByText(os, regionComboBox, "Selected region");

    GTWidget::click(os, GTUtilsSequenceView::getPanOrDetView(os));

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);

    auto startEdit = GTWidget::findLineEdit(os, "editStart");
    auto endEdit = GTWidget::findLineEdit(os, "editEnd");
    CHECK_SET_ERR(startEdit->text() == "1" && endEdit->text() == "199950", "Selection is wrong!");
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3609_1) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call context menu on the sequence view, select {Edit sequence -> Remove subsequence...} menu item.
    //    3. Remove region (5000..199950).
    //    4. Enter position 50000 to the "goto" widget on the tool bar, click the "Go" button.
    //    Expected state: you can't enter this position.
    //    Current state: you can enter this position, an error message appears in the log after button click (safe point triggers in the debug mode).
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    QWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os);

    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REMOVE_SUBSEQUENCE));
    GTUtilsDialog::add(os, new RemovePartFromSequenceDialogFiller(os, "5000..199950"));
    GTWidget::click(os, seqWidget, Qt::RightButton);

    auto goToPosLineEdit = GTWidget::findLineEdit(os, "go_to_pos_line_edit");
    bool inputResult = GTLineEdit::tryToSetText(os, goToPosLineEdit, "50000");
    CHECK_SET_ERR(inputResult == false, "Invalid goToPosition is accepted");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3609_2) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call context menu on the sequence view, select {Edit sequence -> Insert subsequence...} menu item.
    //    3. Insert any subsequence long enough.
    //    4. Enter position 199960 to the "goto" widget on the tool bar, click the "Go" button.
    //    Expected state: you can enter this position, view shows the position.
    //    Current state: you can't enter this position.
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    QWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os);

    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_INSERT_SUBSEQUENCE));
    GTUtilsDialog::add(os, new InsertSequenceFiller(os, "AAACCCTTTGGGAAA"));
    GTWidget::click(os, seqWidget, Qt::RightButton);

    auto goToPosLineEdit = GTWidget::findLineEdit(os, "go_to_pos_line_edit");

    GTLineEdit::setText(os, goToPosLineEdit, "199960");

    auto goBtn = GTWidget::findWidget(os, "Go!");
    GTWidget::click(os, goBtn);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3609_3) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Call context menu on the sequence view, select {Edit sequence -> Insert subsequence...} menu item.
    //    3. Insert any subsequence long enough.
    //    4. Call context menu, select {Go to position...} menu item.
    //    5. Enter position 199960 and accept the dialog.
    //    Expected state: view shows the position, there are no errors in the log.
    //    Current state: view shows the position, there is an error in the log (safe point triggers in the debug mode).
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    QWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ADV_MENU_EDIT, ACTION_EDIT_INSERT_SUBSEQUENCE}));
    GTUtilsDialog::add(os, new InsertSequenceFiller(os, "AAACCCTTTGGGAAA"));
    GTWidget::click(os, seqWidget, Qt::RightButton);

    GTUtilsDialog::add(os, new PopupChooser(os, {ADV_GOTO_ACTION}));
    GTUtilsDialog::add(os, new GoToDialogFiller(os, 199960));
    GTWidget::click(os, seqWidget, Qt::RightButton);

    GTUtilsLog::check(os, l);
}
GUI_TEST_CLASS_DEFINITION(test_3610) {
    // Open "data/samples/FASTA/human_T1.fa".
    // Select whole sequence.
    // Call context menu, select {Edit sequence -> Replace subsequence...} menu item.
    // Replace whole sequence with any inappropriate symbol, e.g. '='. Accept the dialog, agree with message box.
    // Expected state: UGENE doesn't crash.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 199950));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMouseDriver::click(Qt::RightButton);

    class ReplaceSequenceScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            auto plainText = GTWidget::findPlainTextEdit(os, "sequenceEdit", dialog);
            GTWidget::click(os, plainText);

            // Select the whole sequence and replace it with '='. Try applying the change.
            GTKeyboardDriver::keyClick('A', Qt::ControlModifier);
            GTKeyboardDriver::keyClick('=');
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            // Exit from the warning dialog with Escape. Click "OK" on "Input sequence is empty" notification.
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Ok"));
            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            // Close the dialog. There is no other way to close it except 'Cancel'.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, new ReplaceSequenceScenario()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
}

GUI_TEST_CLASS_DEFINITION(test_3612) {
    //    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Turn on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    //    3. Expand "Conocephalus_discolor" group.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(os, "Conocephalus_discolor");

    //    4. Open "Pairwise alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    //    5. Set parameters:
    //        First sequence: Tettigonia_viridissima
    //        Second sequence: Conocephalus_discolor
    //        Algorithm: Smith-Waterman
    //        Gap open penalty: 1
    //        In new window: unchecked
    //    and start the align task.
    //    Expected state: these two sequences are aligned, the same changes are applied to whole collapsing group.
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Tettigonia_viridissima");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Conocephalus_discolor");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Algorithm settings"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "algorithmListComboBox"), "Smith-Waterman");
    GTSpinBox::setValue(os, GTWidget::findSpinBox(os, "gapOpen"), 1);
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "inNewWindowCheckBox"), false);
    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QString firstSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, "Conocephalus_discolor");
    const QString secondSequence = GTUtilsMSAEditorSequenceArea::getSequenceData(os, "Conocephalus_sp.");
    CHECK_SET_ERR(firstSequence == "TT-AGCT-TATTAA", "Unexpected selection. Expected: TT-AGCT-TATTAA");
    CHECK_SET_ERR(secondSequence == "TTAGCTTATTAA--", "Unexpected selection. Expected: TTAGCTTATTAA--");
}

GUI_TEST_CLASS_DEFINITION(test_3613) {
    //    1. Open any assembly.
    //    2. Call a context menu on any read, select {Export -> Current Read} menu item.
    //    3, Set any appropriate destination and apply the dialog.
    //    Expected state: a new document is added to the project, a view opens (MSA Editor or Sequence view, it should be clarified, see the documentation).
    //    Current state: a new unloaded document is added to the project. If you force it to open it will load but "open view" task will fail with an error: "Multiple alignment object not found".
    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os,
                                 new ImportBAMFileFiller(os, sandBoxDir + "test_3613.bam.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/assembly/", "example-alignment.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAssemblyBrowser::zoomToMax(os);

    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Export", "Current read"}));
    GTUtilsDialog::add(os, new ExportReadsDialogFiller(os, sandBoxDir + "test_3613.fa"));
    auto readsArea = GTWidget::findWidget(os, "assembly_reads_area");
    GTWidget::click(os, readsArea, Qt::RightButton);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3619) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    auto progressLabel = GTWidget::findWidget(os, "progressLabel");
    auto resultLabel = GTWidget::findWidget(os, "resultLabel");

    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Regular expression");
    GTUtilsOptionPanelSequenceView::enterPattern(os, ".");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(progressLabel->isHidden() && resultLabel->isVisible(), "Wrong hide, show conditions 2");
}

GUI_TEST_CLASS_DEFINITION(test_3622) {
    //    1. Open "data/samples/FASTA/human_T1.fa".

    //    2. Open "Find pattern" options panel tab.

    //    3. Enter any valid pattern.
    //    Expected state: a new search task is launched.

    //    4. Set "InsDel" algorithm.
    //    Expected state: a new search task is launched.

    //    5. Set any another match value.
    //    Expected state: a new search task is launched.

    //    6. Set "Substitute" algorithm.
    //    Expected state: a new search task is launched.

    //    7. Set any another match value.
    //    Expected state: a new search task is launched.

    //    8. Turn on "Search with ambiguous bases" option.
    //    Expected state: a new search task is launched.

    //    9. Select any sequence region.

    //    10. Set "Selected region" region type.
    //    Expected state: a new search task is launched, the region type is set to "Custom region", the region is the same as the selected one.

    //    11. Change the region.
    //    Expected state: a new search task is launched.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    SchedulerListener listener;

    GTUtilsOptionPanelSequenceView::enterPattern(os, "ACGT");
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "InsDel");
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    GTUtilsOptionPanelSequenceView::setMatchPercentage(os, 80);
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Substitute");
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    GTUtilsOptionPanelSequenceView::setMatchPercentage(os, 90);
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases(os);
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    GTUtilsSequenceView::selectSequenceRegion(os, 100, 200);
    GTUtilsOptionPanelSequenceView::setRegionType(os, "Selected region");
    CHECK_SET_ERR(0 < listener.getRegisteredTaskCount(), "The search task wasn't registered");
    listener.reset();

    const QString currentRegionType = GTUtilsOptionPanelSequenceView::getRegionType(os);
    CHECK_SET_ERR("Selected region" == currentRegionType, QString("An unexpected region type: expect '%1', got '%2'").arg("Selected region").arg(currentRegionType));

    const QPair<int, int> currentRegion = GTUtilsOptionPanelSequenceView::getRegion(os);
    CHECK_SET_ERR(qMakePair(100, 200) == currentRegion, QString("An unexpected region: expect [%1, %2], got [%3, %4]").arg(100).arg(200).arg(currentRegion.first).arg(currentRegion.second));

    GTUtilsOptionPanelSequenceView::setRegion(os, 500, 1000);
    CHECK_SET_ERR(listener.getRegisteredTaskCount() > 0, "The search task wasn't registered");
}

GUI_TEST_CLASS_DEFINITION(test_3623) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAAAAAAAAAAAAAAAAAAAAAAAA");
    GTUtilsOptionPanelSequenceView::toggleSaveAnnotationsTo(os);
    GTUtilsOptionPanelSequenceView::enterFilepathForSavingAnnotations(os, sandBoxDir + "op_seqview_test_0001.gb");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::isPrevNextEnabled(os), "Next and prev buttons are disabled");
}

GUI_TEST_CLASS_DEFINITION(test_3625) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "ACACACACACACACACACACACACACAC", true);

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/33"), "Results string not match. Expected 33.");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
    auto removeOverlapsBox = GTWidget::findCheckBox(os, "removeOverlapsBox");
    GTWidget::click(os, removeOverlapsBox);

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/7"), "Results string not match. Expected 7.");

    GTWidget::click(os, removeOverlapsBox);

    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Regular expression");
    GTUtilsOptionPanelSequenceView::enterPattern(os, "(AAAAAAAAAAAAAAAAAAAAA)+", true);

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/28"), "Results string not match. Expected 28.");

    GTWidget::click(os, removeOverlapsBox);

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/11"), "Results string not match. Expected 11.");
}

GUI_TEST_CLASS_DEFINITION(test_3629) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Select {Add->New annotation...} menu item in the context menu.
    //    Expected state: "Create Annotation" dialog appeared, "create new table" option is selected.

    //    3. Set any location and press "Create".
    //    Expected state: new annotation object was created.
    QDir().mkpath(sandBoxDir + "test_3629");
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "misc_feature", "1..5", sandBoxDir + "test_3629/test_3629.gb"));
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "New annotation"));
    GTUtilsProjectTreeView::checkItem(os, "test_3629.gb");

    //    4. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    5. Add the annotation object to "sars" sequence.
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, "Annotations"), GTUtilsAnnotationsTreeView::getTreeWidget(os));

    //    6. Switch view to "human_T1".
    //    Expected state: there are no attached annotations.
    GTUtilsProjectTreeView::doubleClickItem(os, "human_T1.fa");
    GTThread::waitForMainThread();
    QList<QTreeWidgetItem*> list = GTUtilsAnnotationsTreeView::findItems(os, "misc_feature", {false});
    CHECK_SET_ERR(list.isEmpty(), QString("%1 annotation(s) unexpectidly found").arg(list.count()));
}

GUI_TEST_CLASS_DEFINITION(test_3645) {
    // checking results with diffirent algorithms
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::isTabOpened(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern(os);
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(os, testDir + "_common_data/FindAlgorithm/", "find_pattern_op_2.fa");

    GTUtilsOptionPanelSequenceView::clickNext(os);
    GTUtilsOptionPanelSequenceView::clickNext(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY << "Copy sequence", GTGlobals::UseMouse));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    QString clipStr = GTClipboard::text(os);
    CHECK_SET_ERR(clipStr == "GGGGG", "Found sequence don't match");
}

GUI_TEST_CLASS_DEFINITION(test_3634) {
    // 1. File -> Open as -> "_common_data/gtf/invalid/AB375112_annotations.gtf".
    // Expected: the "Select correct document format" dialog appears.
    // 2. Choose "GTF" in the combobox.
    // 3. Click OK.
    //  Expected: the file is not opened.
    GTLogTracer l;
    GTUtilsDialog::add(os, new GTFileDialogUtils(os, testDir + "_common_data/gtf/invalid", "AB375112_annotations.gtf"));
    GTUtilsDialog::add(os, new DocumentFormatSelectorDialogFiller(os, "GTF"));
    GTMenu::clickMainMenuItem(os, {"File", "Open as..."});
    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_3649) {
    // 1. Open "_common_data/smith-waterman2/simple/05/search.txt".
    GTFileDialog::openFile(os, testDir + "_common_data/smith_waterman2/simple/05", "search.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Export the sequence object as alignment.
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT));
    GTUtilsDialog::add(os, new ExportSequenceAsAlignmentFiller(os, testDir + "_common_data/scenarios/sandbox", "test_3649.aln", ExportSequenceAsAlignmentFiller::Clustalw, true));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "S"));
    GTMouseDriver::click(Qt::RightButton);

    // 3. Add a sequence from the file "_common_data/smith-waterman2/simple/05/query.txt" in the alignment.
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTUtilsDialog::add(os, new GTFileDialogUtils(os, testDir + "_common_data/smith_waterman2/simple/05", "query.txt"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Select both sequences.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(1, 1));

    // 5. Open pairwise alignment option panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));

    // 6. Align using the Smith-Waterman algorithm.
    GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(os, "Smith-Waterman");
    GTWidget::click(os, GTUtilsOptionPanelMsa::getAlignButton(os));

    // Expected: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_3658) {
    //    1. Open the WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Add "Call Varinats with Samtools" workflow element
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Annotations");
    //    3. Press on the toolbar
    //    {Scripting mode->Show scripting options}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show scripting options"}));
    GTWidget::click(os, GTAction::button(os, GTAction::findActionByText(os, "Scripting mode")));
    //    4. Click on the workflow element
    GTUtilsWorkflowDesigner::click(os, "Write Annotations");
    //    Expected state: property editor appeared
    //    5. Press on the toolbar
    //    {Scripting mode->Hide scripting options}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Hide scripting options"}));
    GTWidget::click(os, GTAction::button(os, GTAction::findActionByText(os, "Scripting mode")));
    //    Expected state: scripting column is hidden
    auto table = GTWidget::findTableView(os, "table");
    int count = table->model()->columnCount();
    CHECK_SET_ERR(count == 2, QString("wrong columns number. expected 2, actual: %1").arg(count));
}
GUI_TEST_CLASS_DEFINITION(test_3675) {
    /*  1. Open file COI.aln
    2. Press "build tree" button on main toolbar
    3. In build tree dialog set path like this: some_existing_folder/some_not_existing_folder/COI.nwk
    4. Press build.
    Expected state: new folder created, COI.nwk writter there
    Actual state: error messagebox appeares: "you don't have permission to write to this folder"
*/
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "some_not_existing_folder/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, GTAction::findAction(os, "Build Tree")));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(QFile::exists(sandBoxDir + "some_not_existing_folder/COI.nwk"), "File sandBoxDir/some_not_existing_folder/COI.nwk does not exist");
}

GUI_TEST_CLASS_DEFINITION(test_3676) {
    // 1. Open human_T1.fa
    // 2. Context menu {Analyze --> Primer3}
    // Expected state: 'Primer Designer' dialog appeared
    // 3. Go to Result Settings tab
    // 4. Set group name and annotation name
    // 5. Pick primers
    // Current state: a group name is correct, but annotations name is 'primer'
    // Expected state: all items have corresponding values from the dialog.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Primer3DialogFiller::Primer3Settings settings;
    settings.primersName = "testPrimer";
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));
    GTWidget::click(os, GTWidget::findWidget(os, "primer3_action_widget"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::findItem(os, "testPrimer");
}

GUI_TEST_CLASS_DEFINITION(test_3687_1) {
    // 1. Open file "_common_data/NGS_tutorials/RNA-Seq_Analysis/Prepare_Raw_Data/lymph.fastq".
    // Expected state: "Sequence Reading Options" dialog appeared.
    // 2. Select "As separate sequences in sequence viewer" in the dialog.
    // 3. Press "Ok".
    // Expected: the finishes with error about sequences amount.
    qputenv("UGENE_MAX_OBJECTS_PER_DOCUMENT", "100");
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(os, testDir + "_common_data/fastq/lymph_min.fastq");
    GTUtilsNotifications::waitForNotification(os, true, "contains too many sequences to be displayed");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3687_2) {
    // 1. Open file "_common_data/NGS_tutorials/RNA-Seq_Analysis/Prepare_Raw_Data/lymph.fastq".
    // Expected state: "Sequence Reading Options" dialog appeared.
    // 2. Select "Join sequences into alignment" in the dialog.
    // 3. Press "Ok".
    // Expected: the finishes with error about sequences amount.
    qputenv("UGENE_MAX_OBJECTS_PER_DOCUMENT", "100");
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(os, testDir + "_common_data/fastq/lymph_min.fastq");
    GTUtilsNotifications::waitForNotification(os, true, "contains too many sequences to be displayed");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3690) {
    //    1. Open human_T1.fa
    //    Expected state: there are two opened windows - start page and human_T1
    //    2. Use short cut - Ctrl+Tab
    //    Expected state: current active MDI window is changed to start page
    //    3. Use short cut - Ctrl+Shift+Tab
    //    Expected state: current active MDI window is changed back to human_T1

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* wgt = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(wgt != nullptr, "ActiveWindow is NULL");
    CHECK_SET_ERR(wgt->windowTitle() == "human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1.fa]", "human_T1.fa should be opened!");

    GTKeyboardDriver::keyClick(Qt::Key_Tab, Qt::ControlModifier);

    wgt = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(wgt != nullptr, "ActiveWindow is NULL");
    CHECK_SET_ERR(wgt->windowTitle() == "Start Page", "Start Page should be opened!");

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Tab, Qt::ControlModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    wgt = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(wgt != nullptr, "ActiveWindow is NULL");
    CHECK_SET_ERR(wgt->windowTitle() == "human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1.fa]", "human_T1.fa should be opened!");
}

GUI_TEST_CLASS_DEFINITION(test_3702) {
    // 1. Open human_T1.fa
    // 2. Drag'n' drop it from the project to welcome screen
    // Expected state: sequence view is opened

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsMdi::closeWindow(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1.fa]");
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);

    GTUtilsMdi::checkWindowIsActive(os, "Start Page");

    QPoint centerOfWelcomePage = AppContext::getMainWindow()->getQMainWindow()->geometry().center();
    GTMouseDriver::dragAndDrop(GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa"), centerOfWelcomePage);

    QWidget* window = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    CHECK_SET_ERR(window->windowTitle() == "human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1.fa]", "human_T1.fa should be opened!");
}

GUI_TEST_CLASS_DEFINITION(test_3710) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsNotifications::waitForNotification(os, false, "Report for task: 'Export highlighting'");
    GTUtilsDialog::waitForDialog(os, new ExportHighlightedDialogFiller(os, sandBoxDir + "export_test_3710"));

    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingScheme, "Agreements");
    GTWidget::click(os, GTWidget::findWidget(os, "exportHighlightning"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTFile::getSize(os, sandBoxDir + "export_test_3710") != 0, "Exported file is empty!");
}

GUI_TEST_CLASS_DEFINITION(test_3715) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Activate samples.
    GTUtilsWorkflowDesigner::setCurrentTab(os, GTUtilsWorkflowDesigner::samples);

    // 3. Choose a sample
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTKeyboardDriver::keyClick('r', Qt::ControlModifier);

    CHECK_SET_ERR(GTUtilsWorkflowDesigner::checkErrorList(os, "Read Assembly") != 0, "Workflow errors list cant be empty");
}

GUI_TEST_CLASS_DEFINITION(test_3717) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Click the last sequence in the name list area.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 17));
    //    3. Press Shift and click the second sequence in the name list area.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 1));
    //    Expected: sequences [2; last] are selected, the selection frame in the name list is shown.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(0, 1), QPoint(603, 17)));
    //    3. Press Shift and click the first sequence in the name list area.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 0));
    //    Expected: the selection frame in the name list area is still shown and bound all sequences.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(0, 0), QPoint(603, 17)));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
}

GUI_TEST_CLASS_DEFINITION(test_3723) {
    //    1. Open simultaneously two files: "_common_data/fasta/fa1.fa.gz" and "_common_data/fasta/fa3.fa.gz".
    //    Expected state: "Multiple Sequence Reading Mode" dialog appears.
    //    2. Open them with "Merge" option.
    //    Expected state: sequences are merged and opened, there are no errors in the log.
    GTLogTracer logTracer;

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsDialog::waitForDialog(os, new GTSequenceReadingModeDialogUtils(os));
    GTFileDialog::openFileList(os, testDir + "_common_data/fasta", {"fa1.fa.gz", "fa3.fa.gz"});

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "merged_document.gb");
    GTUtilsDocument::isDocumentLoaded(os, "merged_document.gb");
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3724) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Use sequence area context menu:
    //    { Statistics -> Generate distance matrix }
    //    Expected state: the "Generate Distance Matrix" dialog has appeared.

    //    3. Click "Generate".
    //    Expected state: the "Multiple Sequence Alignment Distance Matrix" view has appeared.
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Statistics", "Generate distance matrix..."}));
    GTUtilsDialog::add(os, new DistanceMatrixDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
}

GUI_TEST_CLASS_DEFINITION(test_3730) {
    //  1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //  2. Create new custom nucleotide color scheme.
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, "test_3730_scheme_1", NewColorSchemeCreator::nucl);

    //  3. Go to Highlighting Options Panel tab and select this new color scheme.
    GTUtilsOptionPanelMsa::setColorScheme(os, "test_3730_scheme_1");

    //  4. Go to Preferences again and create new amino color scheme.
    //  5. Accept Application Settings dialog.
    //  Expected state: UGENE doesn't crash, color scheme is not changed.
    GTUtilsMSAEditorSequenceArea::createColorScheme(os, "test_3730_scheme_2", NewColorSchemeCreator::amino);

    QString colorScheme = GTUtilsOptionPanelMsa::getColorScheme(os);
    CHECK_SET_ERR(colorScheme == "test_3730_scheme_1", "The color scheme was unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_3731) {
    // 1. Open /data/sample/PDB/1CRN.pdb
    // Expected state: Sequence is opened
    // 2. Do context menu "Analyze - Predict Secondary Structure"
    // Expected state: Predict Secondary Structure dialog is appeared
    // 3. Set "Range Start" 20, "Range End": 46, set any prediction algorithm
    // 4. Press "Start prediction" button
    // Expected state: you get annotation(s) in range 20..46
    // Current state for GOR IV: you get annotations with ranges 11..15 and 24..25
    GTFileDialog::openFile(os, dataDir + "samples/MMDB", "1CRN.prt");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QPoint itemCenter = GTUtilsAnnotationsTreeView::getItemCenter(os, "1CRN chain A annotation [1CRN.prt]");
    itemCenter.setX(itemCenter.x() + 10);
    GTMouseDriver::moveTo(itemCenter);
    GTMouseDriver::click();
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_REMOVE << "Selected objects with annotations from view"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new PredictSecondaryStructureDialogFiller(os, 20, 46));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ANALYSE << "Predict secondary structure"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    QList<U2Region> annotatedRegions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    for (const U2Region& curRegion : qAsConst(annotatedRegions)) {
        CHECK_SET_ERR(curRegion.startPos >= 20, "Incorrect annotated region");
    }
}

GUI_TEST_CLASS_DEFINITION(test_3732) {
    //    1. Open UGENE preferences, open "Resources" tab, set UGENE memory limit to 200Mb.
    class MemoryLimitSetScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);
            auto memBox = GTWidget::findSpinBox(os, "memBox", dialog);
            GTSpinBox::setValue(os, memBox, 200, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MemoryLimitSetScenario));
    GTMenu::clickMainMenuItem(os, {"Settings", "Preferences..."});

    //    2. Open file "_common_data/scenarios/_regression/1688/sr100.000.fa" as separate sequences.
    //    Expected state: there is an error in the log: "MemoryLocker - Not enough memory error, 41 megabytes are required".
    GTLogTracer logTracer;

    GTUtilsProject::openMultiSequenceFileAsMalignment(os, testDir + "_common_data/scenarios/_regression/1688", "sr100.000.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(logTracer.checkMessage("MemoryLocker - Not enough memory error, 41 megabytes are required"), "An expected error message not found");
}

GUI_TEST_CLASS_DEFINITION(test_3736) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Regular expression");
    GTUtilsOptionPanelSequenceView::enterPattern(os, "A{5,6}", true);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/3973"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_3738) {
    //    Select {DNA Assembly -> Contig assembly with CAP3}
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_3738.ugenedb"));
    GTUtilsDialog::waitForDialog(os, new CAP3SupportDialogFiller(os, QStringList() << testDir + "_common_data/scf/Sequence A.scf" << testDir + "_common_data/scf/Sequence B.scf", sandBoxDir + "test_3738.ace"));
    GTMenu::clickMainMenuItem(os, {"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});

    //    menu item in the main menu.
    //    Set sequences "_common_data/scf/Sequence A.scf" and "_common_data/scf/Sequence B.scf" as input, set any valid output path and run the task.
    //    Expected state: user is asked to select the view.
    //    Select "Open in Assembly Browser with ACE importer format" and import the assembly anywhere.
    //    Expected state: the assembly is successfully imported.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
    //    Current state: the assembly is not imported, there is an error in the log: Task {CAP3 run and open result task}
}

GUI_TEST_CLASS_DEFINITION(test_3744) {
    // 1. Open "data/samples/FASTA/human_T1.fa"
    // 2. Open the Find Pattern options panel tab
    // 3. Set the "Regular expression" search algorithm
    // 4. Paste to the pattern field the following string: "ACT.G"
    // Expected state: some results have been found
    // 5. Select the pattern by mouse or pressing "Shift + Home"
    // 6. Delete the pattern by pressing a backspace
    // Expected state: "Previous" and "Next" buttons are disabled

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));

    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Regular expression");

    GTUtilsOptionPanelSequenceView::enterPattern(os, "ACG.T", true);

    auto createButton = GTWidget::findWidget(os, "getAnnotationsPushButton");

    GTUtilsOptionPanelSequenceView::enterPattern(os, "", true);

    CHECK_SET_ERR(!createButton->isEnabled(), "prevPushButton is unexpectidly enabled")
}
GUI_TEST_CLASS_DEFINITION(test_3749) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select any base.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(10, 10));
    GTMouseDriver::click(Qt::LeftButton);

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            // GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() - QPoint(5, 0));
            GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(1, 10));
            GTMouseDriver::click();
            QWidget* contextMenu = QApplication::activePopupWidget();
            CHECK_SET_ERR(nullptr == contextMenu, "There is an unexpected context menu");
        }
    };

    // 3. Move the mouse to another base and click the right button.
    // Expected state: a context menu appears, a single base from the previous step is selected.
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new Scenario));
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(10, 9));
    GTMouseDriver::click(Qt::RightButton);

    // 4. Move the mouse to the any third base and click the left button.
    // Expected state: the context menu closes, the first selected base is the only selected base.
    // Current state: the context menu closes, there is a selection from the base from the second step to the base from the last step.
    GTMouseDriver::click(Qt::LeftButton);

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(1, 10), QPoint(1, 10)));
}

GUI_TEST_CLASS_DEFINITION(test_3755) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto seqArea = GTWidget::findWidget(os, "msa_editor_sequence_area");
    QColor before = GTWidget::getColor(os, seqArea, QPoint(2, 1));
    //    Open the "Highlighting" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    //    Select different highlighting schemes.
    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingScheme, "Conservation level");
    auto w = GTWidget::findWidget(os, "thresholdSlider");
    QSlider* slider = qobject_cast<QSlider*>(w);
    GTSlider::setValue(os, slider, 80);
    QColor after = GTWidget::getColor(os, seqArea, QPoint(2, 1));
    // check color change
    CHECK_SET_ERR(before != after, "colors not changed");
}

GUI_TEST_CLASS_DEFINITION(test_3757) {
    // Open some specific PDB file and ensure that UGENE doesn't crash
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/pdb/", "water.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "water.pdb", QModelIndex());
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3760) {
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/phylip/", "Three Kingdoms.phy");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "Three Kingdoms.phy", QModelIndex());
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3768) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    // 2. Click "Find ORFs" button on the main toolbar.
    // 3. Uncheck "Must start with init codon" option and accept the dialog.
    // 4. Call context menu, select {Edit sequence -> Remove subsequence...}
    // 5. Fill the dialog:
    // Region to remove: 2..199950;
    // Annotation region resolving mode: Crop corresponding annotation
    // and accept the dialog.
    // Current state: UGENE crashes.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os)
            : Filler(_os, "ORFDialogBase") {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget(os);
            GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "ckInit", w), false);
            GTUtilsDialog::clickButtonBox(os, w, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os));
    GTWidget::click(os, GTAction::button(os, "Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Edit", "Remove subsequence..."}));
    GTUtilsDialog::add(os, new RemovePartFromSequenceDialogFiller(os, "2..199950"));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
}

GUI_TEST_CLASS_DEFINITION(test_3770) {
    //    1. Select {File -> Access remote database...} menu item in the main menu.
    //    2. Fill the dialog:
    //       Resource ID: NW_003943623;
    //       Database: NCBI GenBank (DNA sequence);
    //       Force download the appropriate sequence: checked
    //    and accept the dialog.
    //    3. Try to cancel the task.
    //    Expected state: the task cancels within a half of a minute.
    //    Current state: the task doesn't cancel.

    GTUtilsDialog::waitForDialog(os, new RemoteDBDialogFillerDeprecated(os, "NW_003943623", 0, true, true, false, sandBoxDir));
    GTMenu::clickMainMenuItem(os, {"File", "Access remote database..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::cancelTask(os, "Download remote documents");

    CHECK_SET_ERR(GTUtilsTaskTreeView::countTasks(os, "Download remote documents") == 0, "Task was not canceled");
}

GUI_TEST_CLASS_DEFINITION(test_3772) {
    // 1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Ctrl + F.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    // 3. Type "X".
    GTUtilsOptionPanelSequenceView::enterPattern(os, "X");

    // Expected:
    // a) The alphabets warning appears.
    // b) The pattern text area is red.
    QLabel* label = dynamic_cast<QLabel*>(GTWidget::findWidget(os, "lblErrorMessage"));
    CHECK_SET_ERR(label->isVisible(), "Warning is not shown 1");
    CHECK_SET_ERR(label->text().contains("Warning"), "Warning is not shown 2");

    // 4. Remove the character.
    GTUtilsOptionPanelSequenceView::enterPattern(os, "");

    // Expected:
    // a) The alphabets warning disappears.
    // b) The pattern text area is white.
    if (label->isVisible()) {
        CHECK_SET_ERR(!label->text().contains("Warning"), "Warning is shown");
    }
}

GUI_TEST_CLASS_DEFINITION(test_3773) {
    /* An easier way to reproduce an error:
     * 1. Open HMM profile
     * 2. Remove it from the project
     *   Expected state: Log not have errors
     */
    GTLogTracer logTracer;
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "Plain text"));
    GTUtilsProject::openFile(os, dataDir + "samples/HMM/aligment15900.hmm");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "aligment15900.hmm");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3773_1) {
    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os)
            : Filler(_os, "HmmerBuildDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTLineEdit::setText(os, "outHmmfileEdit", "37773_1_out.hmm", dialog);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer logTracer;
    // QMenu* menu = GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));
    // GTMenu::clickMenuItemByName(os, menu, {"Build HMMER3 profile"});
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Advanced", "Build HMMER3 profile"}));
    GTUtilsDialog::add(os, new OkClicker(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os, QPoint(5, 5));
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3778) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Open Circular View.
    auto button = GTWidget::findWidget(os, "globalToggleViewAction_widget");
    GTWidget::click(os, button);
    // 3. Context menu -> Export -> Save circular view as image.
    // Expected state: the "Export Image" dialog appears.
    // 4. Press "Export".
    // Expected state: the message about file name appears, the dialog is not closed (the export task does not start).
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto fileNameEdit = GTWidget::findLineEdit(os, "fileNameEdit", dialog);
            QString badFileName = isOsWindows()
                                      ? sandBoxDir + "circular_human_T1 (UCSC April 2002 chr7:115977709-117855134).png"
                                      : "///bad-name";
            GTLineEdit::setText(os, fileNameEdit, badFileName);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "cannot be created"));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CircularViewExportImage(os, new Scenario()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "Save circular view as image", GTGlobals::UseMouse));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
}

GUI_TEST_CLASS_DEFINITION(test_3779) {
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "regression_test_3779.ugenedb"));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/ace_test_4.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool assemblyOverviewFound = !AppContext::getMainWindow()->getQMainWindow()->findChildren<CoveredRegionsLabel*>().isEmpty();
    CHECK_SET_ERR(assemblyOverviewFound, "Assembly overview not found");

    GTUtilsAssemblyBrowser::zoomToMax(os);
    GTUtilsAssemblyBrowser::zoomToMin(os);

    assemblyOverviewFound = !AppContext::getMainWindow()->getQMainWindow()->findChildren<CoveredRegionsLabel*>().isEmpty();
    CHECK_SET_ERR(assemblyOverviewFound, "Assembly overview not found");
}

GUI_TEST_CLASS_DEFINITION(test_3785_1) {
    // 1. Open "_common_data/clustal/fungal - all.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/fungal - all.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Align with ClustalW.
    GTUtilsDialog::waitForDialog(os, new ClustalWDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_ALIGN, "Align with ClustalW"}));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);

    // Expected: task started.
    GTGlobals::sleep(1000);
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "Task did not started");

    // 3. Close the alignment view.
    GTUtilsMdi::closeWindow(os, GTUtilsMdi::activeWindow(os)->objectName());

    // Expected: task is still running.
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "Task is cancelled");

    // 4. Delete the document from the project.
    GTUtilsProjectTreeView::click(os, "fungal - all.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: task is cancelled.
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 0, "Task is not cancelled");
}

GUI_TEST_CLASS_DEFINITION(test_3785_2) {
    // 1. Open "_common_data/clustal/fungal - all.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/fungal - all.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Align with ClustalW.
    GTUtilsDialog::waitForDialog(os, new ClustalWDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_ALIGN, "Align with ClustalW"}));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);
    GTGlobals::sleep(1000);

    // Expected: task started.
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "Task did not started");

    // 3. Close the alignment view.
    GTUtilsMdi::closeWindow(os, GTUtilsMdi::activeWindow(os)->objectName());

    // Expected: task is still running.
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 1, "Task is cancelled");

    // 4. Delete the object from the document.
    GTUtilsProjectTreeView::click(os, "fungal - all");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os, 3000);

    // Expected: task is cancelled.
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 0, "Task is not cancelled");
}

GUI_TEST_CLASS_DEFINITION(test_3788) {
    GTLogTracer logTracer;

    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Create an annotation on region 199950..199950.
    GTUtilsAnnotationsTreeView::createAnnotation(os, "<auto>", "misc_feature", "199950..199950");

    //    3. Call context menu, select {Edit sequence -> Remove subsequence...} menu item.
    //    4. Remove region 2..199950, corresponding annotations should be cropped.
    //    Expected result: sequence has length 1, there are no annotations.
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Edit", "Remove subsequence..."}));
    GTUtilsDialog::add(os, new RemovePartFromSequenceDialogFiller(os, "2..199950"));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QList<U2Region> annotatedRegions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(annotatedRegions.isEmpty(), "There are annotations unexpectedly");

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3797) {
    // Open "data/samples/CLUSTALW/COI.aln".
    // Toggle the collapsing mode.
    // Select some sequence in the name area.
    // Click the "end" or "page down" key.

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::toggleCollapsingMode(os);
    GTUtilsMSAEditorSequenceArea::selectSequence(os, QString("Mecopoda_sp.__Malaysia_"));

    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
}

GUI_TEST_CLASS_DEFINITION(test_3805) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Save the initial content
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 51, 102));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialContent = GTClipboard::text(os);

    // 3. Reverse sequence
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace the whole sequence by", "Reverse (3'-5') sequence"}, GTGlobals::UseKey);

    // 4. Complement sequence
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace the whole sequence by", "Complementary (3'-5') sequence"}, GTGlobals::UseKey);

    // 5. Reverse complement sequence
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace the whole sequence by", "Complementary (5'-3') sequence"}, GTGlobals::UseKey);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 51, 102));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString newContent = GTClipboard::text(os);

    CHECK_SET_ERR(initialContent == newContent, "Result of actions is incorrect. Expected: " + initialContent + ", found: " + newContent);
}

GUI_TEST_CLASS_DEFINITION(test_3809) {
    GTFileDialog::openFile(os, testDir + "_common_data/regression/3809/zF849G6-6a01.p1k.scf.ab1");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3815) {
    GTLogTracer l;
    // 1. Open "_common_data/fasta/cant_translate.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "cant_translate.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Context menu of the document -> Export/Import -> Export sequences.
    // Expected state: the "Export Selected Sequences" dialog appears.
    // 3. Check "Translate to amino alphabet" and press "Export".
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "test_3815.fa", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "cant_translate.fa"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: task has finished with error, no output file has been produced.
    CHECK_SET_ERR(l.checkMessage("No sequences have been produced"), "No error");
}

GUI_TEST_CLASS_DEFINITION(test_3816) {
    // Open some specific file with a tree and ensure that UGENE doesn't crash
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/newick/", "arb-silva.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "arb-silva.nwk", QModelIndex());
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3817) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    GTKeyboardDriver::keySequence("ACTGCT");

    GTUtilsOptionPanelSequenceView::openSearchInShowHideWidget(os);

    auto boxRegion = GTWidget::findComboBox(os, "boxRegion");

    GTComboBox::selectItemByText(os, boxRegion, "Custom region");

    auto editStart = GTWidget::findLineEdit(os, "editStart");
    auto editEnd = GTWidget::findLineEdit(os, "editEnd");

    GTLineEdit::setText(os, editStart, "123");
    GTLineEdit::setText(os, editEnd, "1000");

    GTComboBox::selectItemByText(os, boxRegion, "Whole sequence");
    CHECK_SET_ERR(!editStart->isVisible() && !editEnd->isVisible(), "Region boundary fields are unexpectedly visible");

    GTComboBox::selectItemByText(os, boxRegion, "Custom region");
    CHECK_SET_ERR(editStart->isVisible() && editEnd->isVisible(), "Region boundary fields are unexpectedly invisible");

    GTComboBox::selectItemByText(os, boxRegion, "Selected region");
    CHECK_SET_ERR(boxRegion->currentText() == "Selected region", QString("Region type value is unexpected: %1. Expected: Selected region").arg(boxRegion->currentText()));
}

GUI_TEST_CLASS_DEFINITION(test_3821) {
    // 1. Open any genbank file with a COMMENT section
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 2));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence region"}));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);

    Primer3DialogFiller::Primer3Settings settings;
    settings.shortRegion = true;
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));
    GTWidget::click(os, GTWidget::findWidget(os, "primer3_action_widget"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3829) {
    //    Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    Open "data/samples/GFF/5prime_utr_intron_A20.gff".
    GTFileDialog::openFile(os, dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Attach the first annotation object to the sequence.
    QModelIndex index = GTUtilsProjectTreeView::findIndex(os, "Ca20Chr1 features");
    //    Expected state: UGENE warning about annotation is out of range.
    class scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto buttonBox = GTWidget::findDialogButtonBox(os, "buttonBox", dialog);
            QAbstractButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != nullptr, "okButton is NULL");
            //    Agree with warning.
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
            GTWidget::click(os, okButton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os, new scenario()));
    GTUtilsProjectTreeView::dragAndDrop(os, index, GTUtilsSequenceView::getPanOrDetView(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there is a sequence with attached annotation table object, there is an annotation that is located beyond the sequence.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "5_prime_UTR_intron"));
    GTMouseDriver::click();

    //    Select the annotation in the tree view. Open "Statistics" options panel tab or try to find something in the selected region.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));
    GTUtilsOptionPanelSequenceView::checkTabIsOpened(os, GTUtilsOptionPanelSequenceView::Statistics);
    //    Expected state: you can't set region that is not inside the sequence.
    //    Current state: an incorrect selected region is set, crashes and safe points are possible with the region.
}

GUI_TEST_CLASS_DEFINITION(test_3843) {
    // 1. Open file "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Turn the collapsing mode on.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // 3. Expand one of the collapsed sequences.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(os, "Conocephalus_discolor");

    // 4. Select some region within a sequence from the chosen collapsed group.
    // 5. Click "Ctrl+C"
    // Expected state : clipboard contains a selected string
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(4, 11), QPoint(10, 11), "CTTATTA");
}

GUI_TEST_CLASS_DEFINITION(test_3850) {
    GTLogTracer l;

    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open "Search in Sequence" options panel tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    // 5. Check "Use pattern name" checkbox.
    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os, true);
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "chbUsePatternNames"), true);

    // 3. Check "Load patterns from file" checkbox.
    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern(os);

    // 4. Set "_common_data/fasta/shuffled.fa" as input file.
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(os, testDir + "_common_data/fasta", "shuffled.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: there are 1802 results found.
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/1802"), "Results string not match");

    // 6. Click "Create annotations" button.
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: 1802 annotations are created, each has the same name as the pattern has.
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3862) {
    // 1. Open any sequence
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Switch on auto-annotations
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3868) {
    // 1. Open "VectorNTI_CAN_READ.gb"
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/", "VectorNTI_CAN_READ.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Open the "Annotation highlighting" OP widget.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT"));
    // Expected state: labels from genbank features are shown in annotations widgets
    GTMouseDriver::moveTo(GTUtilsAnnotHighlightingTreeView::getItemCenter(os, "rep_origin"));
    GTMouseDriver::click();

    auto qualifiersEdit = GTWidget::findLineEdit(os, "editQualifiers");
    CHECK_SET_ERR(qualifiersEdit->text().contains("label"), "Label must be shown in annotation widget");
}

GUI_TEST_CLASS_DEFINITION(test_3869) {
    // check comments for vector-nti format
    GTFileDialog::openFile(os, testDir + "_common_data/vector_nti_sequence/unrefined.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"comment"});
    QString name = GTUtilsAnnotationsTreeView::getQualifierValue(os, "Author name", "comment");
    CHECK_SET_ERR(name == "Demo User", "unexpected qualifier value: " + name)
}

GUI_TEST_CLASS_DEFINITION(test_3870) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int length = GTUtilsMSAEditorSequenceArea::getLength(os);

    // 2. Insert gaps
    int columnsNumber = GTUtilsMSAEditorSequenceArea::getNumVisibleBases(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(columnsNumber - 10, 0), QPoint(columnsNumber, 10), GTGlobals::UseMouse);

    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // 3. Export sequences with terminal gaps to FASTA
    // Expected state: terminal gaps are not cut off
    length = GTUtilsMSAEditorSequenceArea::getLength(os);
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment"));
    GTUtilsDialog::add(os, new ExtractSelectedAsMSADialogFiller(os, testDir + "_common_data/scenarios/sandbox/3870.fa", GTUtilsMSAEditorSequenceArea::getNameList(os), length - 60, length - 1, true, false, false, false, true, "FASTA"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));

    // QFile resFile(testDir + "_common_data/scenarios/sandbox/3870.fa");
    // QFile templateFile(testDir + "_common_data/scenarios/_regression/3870/3870.fa");
    // CHECK_SET_ERR(resFile.size() == templateFile.size(), "Result file is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_3886) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class TestWizardFiller : public Filler {
    public:
        TestWizardFiller(HI::GUITestOpStatus& os)
            : Filler(os, "Extract Alignment Consensus as Sequence") {
        }

        void run() override {
            // Click Next.
            // Expected: UGENE does not crash.
            GTWidget::click(os, GTWidget::findWidget(os, "__qt__passive_wizardbutton1"));
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new TestWizardFiller(os));

    // Open 'Extract consensus as sequence' sample. I will automatically show the wizard.
    GTUtilsWorkflowDesigner::addSample(os, "Extract consensus as sequence");

    // Wait until wizard is closed.
    GTUtilsDialog::checkNoActiveWaiters(os);
}

GUI_TEST_CLASS_DEFINITION(test_3895) {
    // 1. Open '_common_data/genbank/pBR322.gb' (file contains circular marker)
    GTFileDialog::openFile(os, testDir + "_common_data/genbank", "pBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Export document to genbank format
    // Current state: there are two circular markers in the first line
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, sandBoxDir, "test_3895.gb", ExportDocumentDialogFiller::Genbank, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Export document"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "pBR322.gb"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QFile exportedDoc(sandBoxDir + "test_3895.gb");
    bool isOpened = exportedDoc.open(QFile::ReadOnly);
    CHECK_SET_ERR(isOpened, QString("Can not open file: \"%1\"").arg(exportedDoc.fileName()));
    QTextStream fileReader(&exportedDoc);
    QString firstLine = fileReader.readLine();

    int firstIndex = firstLine.indexOf("circular", Qt::CaseInsensitive);
    int lastIndex = firstLine.indexOf("circular", firstIndex + 1, Qt::CaseInsensitive);
    CHECK_SET_ERR(lastIndex < 0, "There are several circular markers");
}

GUI_TEST_CLASS_DEFINITION(test_3901) {
    // 1. Open "_common_data/fasta/human_T1_cutted.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Toggle circular view.
    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction"));

    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != nullptr, "Cannot find Wrap sequence action");
    GTWidget::click(os, GTAction::button(os, wrapMode));

    // 3. Create an annotation with region: join(50..60,20..30,80..90).
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "group", "feature", "join(50..60,20..30,80..90)"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Add", "New annotation..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Click to the arrow (80..90).
    GTUtilsSequenceView::clickAnnotationDet(os, "feature", 80, 0, true);
    // Expected state: the arrow's region is selected.
    ADVSingleSequenceWidget* w = (ADVSingleSequenceWidget*)GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QVector<U2Region> selection = w->getSequenceSelection()->getSelectedRegions();
    CHECK_SET_ERR(selection.size() == 1, "No selected region");
    CHECK_SET_ERR(selection.first() == U2Region(79, 11), "Wrong selected region");
}

GUI_TEST_CLASS_DEFINITION(test_3902) {
    // Open "data/samples/Genbank/murine.gb" and
    // "data/samples/Genbank/sars.gb" as separate sequences (in different views).
    // Select these two documents in the project view and unload them. Agree to close views.
    // Expected state: both documents are unloaded, there are no errors in the log.

    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsDocument::unloadDocument(os, "murine.gb", false);
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsDocument::unloadDocument(os, "sars.gb", false);

    GTUtilsLog::check(os, l);
}
GUI_TEST_CLASS_DEFINITION(test_3903) {
    /*
    1. Open any sequence
    2. Open and close Find Pattern tab
    3. Remove sub-sequence
    4. Press Ctrl+F
    5. Input e.g. 'A'
    Expected state: Log shouldn't contain errors
    Current state: SAFE_POINT is triggered
    or
    Current state: the warning appeared "there is no pattern to search"
    (The problem is in 'Region to search' parameter)

*/

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));

    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Edit", "Remove subsequence..."}));
    GTUtilsDialog::add(os, new RemovePartFromSequenceDialogFiller(os, "100..199950"));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTLogTracer lt;
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("A");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_3904) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Phaneroptera_falcata");
    GTLogTracer lt;
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "remove_columns_of_gaps"));
    GTUtilsDialog::add(os, new RemoveGapColsDialogFiller(os, RemoveGapColsDialogFiller::Percent, 10));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_3905) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Remove the first sequence.
    GTUtilsMSAEditorSequenceArea::removeSequence(os, "Phaneroptera_falcata");

    //    3. Undo removing.
    GTUtilsMsaEditor::undo(os);

    //    4. Set the first sequence as reference.
    GTUtilsMsaEditor::setReference(os, "Phaneroptera_falcata");

    //    5. Redo removing.
    //    Expected state: the reference sequence is unset.
    GTUtilsMsaEditor::redo(os);
    const QString& referenceName = GTUtilsMsaEditor::getReferenceSequenceName(os);
    CHECK_SET_ERR(referenceName.isEmpty(), "A reference sequence was not reset");
}

GUI_TEST_CLASS_DEFINITION(test_3920) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    //    2. Click "find ORFs" button on the toolbar.
    //       Set custom region that differs from the whole sequence region and accept the dialog.
    //    Expected state: ORFs are found in the set region.
    //    Current state: ORFs on the whole sequence are found.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class ORFDialogFiller : public Filler {
    public:
        ORFDialogFiller(HI::GUITestOpStatus& _os)
            : Filler(_os, "ORFDialogBase") {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget(os);
            GTLineEdit::setText(os, "start_edit_line", "1000", w);
            GTLineEdit::setText(os, "end_edit_line", "4000", w);
            GTUtilsDialog::clickButtonBox(os, w, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ORFDialogFiller(os));
    GTWidget::click(os, GTAction::button(os, "Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    for (const U2Region& r : qAsConst(regions)) {
        CHECK_SET_ERR((r.startPos >= 1000 && r.startPos <= 4000 &&
                       r.endPos() >= 1000 && r.endPos() <= 4000),
                      "Invalid annotated region!");
    }
}

GUI_TEST_CLASS_DEFINITION(test_3924) {
    // check comments for vector-nti format
    GTFileDialog::openFile(os, testDir + "_common_data/vector_nti_sequence/unrefined.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"CDS"});
    QString name = GTUtilsAnnotationsTreeView::getQualifierValue(os, "vntifkey", "CDS");
    CHECK_SET_ERR(name == "4", "unexpected qualifier value: " + name)
}

GUI_TEST_CLASS_DEFINITION(test_3927) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Enable collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);
    // 3. Remove the first sequence. x3
    GTUtilsMSAEditorSequenceArea::removeSequence(os, "Phaneroptera_falcata");
    GTUtilsMSAEditorSequenceArea::removeSequence(os, "Isophya_altaica_EF540820");
    GTUtilsMSAEditorSequenceArea::removeSequence(os, "Bicolorana_bicolor_EF540830");
    // Expected state safe point didn't triggered
}

GUI_TEST_CLASS_DEFINITION(test_3928) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    // 2. Set any sequence as a reference via a context menu
    // 3. Open the "Statistics" tab in the Options panel
    // Expected state: reference sequence is shown in the "Reference sequence" section
    // 4. Close the Options panel
    // 5. Set any other sequence as a reference via a context menu
    // 6. Open the "Statistics" tab in the Options panel
    // Expected state: new reference sequence is shown in the "Reference sequence" section

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Set this sequence as reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os, QPoint(5, 5));
    QString currentReference = GTUtilsMsaEditor::getReferenceSequenceName(os);
    CHECK_SET_ERR("Metrioptera_japonica_EF540831" == currentReference, QString("An unexpected reference sequence is set: expect '%1', got '%2'").arg("Metrioptera_japonica_EF540831").arg(currentReference));

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"unset_reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os, QPoint(6, 6));
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"set_seq_as_reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os, QPoint(6, 6));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Set this sequence as reference"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os, QPoint(6, 6));
    currentReference = GTUtilsMsaEditor::getReferenceSequenceName(os);
    CHECK_SET_ERR("Gampsocleis_sedakovii_EF540828" == currentReference, QString("An unexpected reference sequence is set: expect '%1', got '%2'").arg("Gampsocleis_sedakovii_EF540828").arg(currentReference));
}

GUI_TEST_CLASS_DEFINITION(test_3938) {
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Variation annotation with SnpEff");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsWorkflowDesigner::addInputFile(os, "Input Variations File", testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "ebola_zaire"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QString error = lt.getJoinedErrorString();
    CHECK_SET_ERR(!error.contains("finished with error", Qt::CaseInsensitive), "Unexpected error message in the log: " + error);
}

GUI_TEST_CLASS_DEFINITION(test_3950) {
    // 1. Build BWA MEM index for "_common_data/bwa/NC_000021.gbk.fa
    // 2. Open "_common_data/bwa/workflow/bwa-mem.uwl"
    // 3. Set NC_000021.gbk.fa as reference
    // 4. Reads: nrsf-chr21.fastq, control-chr21.fastq
    // 5. Run workflow
    // Expected state: no error (code 1)

    GTLogTracer l;

    GTFile::copy(os, testDir + "_common_data/bwa/NC_000021.gbk.min.fa", sandBoxDir + "test_3950.fa");

    GTUtilsDialog::waitForDialog(os, new BuildIndexDialogFiller(os, sandBoxDir, "test_3950.fa", "BWA MEM"));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Build index for reads mapping..."});
    GTUtilsDialog::checkNoActiveWaiters(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/bwa/workflow/bwa-mem.uwl");
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive(os);

    GTUtilsWorkflowDesigner::click(os, "File List");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bwa/nrsf-chr21.fastq");
    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bwa/control-chr21.fastq");

    GTUtilsWorkflowDesigner::click(os, "Align reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Reference genome", sandBoxDir + "test_3950.fa", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output folder", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os, 40000);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3953) {
    /*
    1. Open "human_T1.fa"
    2. Open "find pattern" tab
    3. Insert "TTGTCAGATTCACCA" into find pattern field
    4. Put cursor to the beginning of find pattern field
    5. Pres "delete" key, until all symbols are deleted
    Expected state: "create annotation" button is disabled
    Actual: "create annotation" button is enabled
    */
    QString pattern = "TTGTCAGATTCACCA";
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));

    GTKeyboardDriver::keySequence(pattern);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto getAnnotationsButton = GTWidget::findPushButton(os, "getAnnotationsPushButton");
    GTWidget::checkEnabled(os, getAnnotationsButton, true);

    GTKeyboardDriver::keyClick(Qt::Key_Home);
    for (int i = pattern.length(); --i >= 0;) {
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
        GTUtilsTaskTreeView::waitTaskFinished(os);
        GTWidget::checkEnabled(os, getAnnotationsButton, i > 0);
    }
}

GUI_TEST_CLASS_DEFINITION(test_3959) {
    // 1. { File -> New document from text... }
    // Expected state: the "Create document" dialog has appeared
    // 2. Set sequence "AAAA", set some valid document path and click "Create".
    // Expected state: the Sequence view has opened, the "Zoom out" button is disabled.
    // 3. Call context menu { Edit sequence -> Insert subsequence... }
    // Expected state: the "Insert Sequence" dialog has appeared
    // 4. Set sequence "AAAA", position to insert - 5 and click "OK".
    // Expected state: the same sequence region is displayed, scrollbar has shrunk, "Zoom out" has enabled.

    Runnable* filler = new CreateDocumentFiller(os,
                                                "AAAA",
                                                false,
                                                CreateDocumentFiller::StandardRNA,
                                                true,
                                                false,
                                                "",
                                                testDir + "_common_data/scenarios/sandbox/result",
                                                CreateDocumentFiller::FASTA,
                                                "result",
                                                true);
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // QToolBar* mwtoolbar_activemdi = GTToolbar::getToolbar(os, MWTOOLBAR_MAIN);
    // QWidget* zoomOutButton = GTToolbar::getWidgetForActionTooltip(os, mwtoolbar_activemdi, "Zoom Out");
    // CHECK_SET_ERR(!zoomOutButton->isEnabled(), "zoomOutButton button on toolbar is not disabled");

    Runnable* filler1 = new InsertSequenceFiller(os,
                                                 "AAAA");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseKey);
}

GUI_TEST_CLASS_DEFINITION(test_3960) {
    /* 1. Open _common_data/scenarios/_regression/3960/all.gb
     *   Expected state: No error messages in log
     */
    GTLogTracer logTracer;
    GTUtilsDialog::add(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/3960", "all.gb"));
    GTUtilsDialog::add(os, new DocumentFormatSelectorDialogFiller(os, "GenBank"));
    GTUtilsDialog::add(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge));
    GTMenu::clickMainMenuItem(os, {"File", "Open as..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!logTracer.hasErrors(), "Errors in log: " + logTracer.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_3975) {
    // 1. Open _common_data/gff/noheader.gff
    // 2. Connect it with some sequence to observe annotations
    // 3. Open the file in some text editor
    // Current state: there is annotation "chromosome"
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, testDir + "_common_data/gff/", "noheader.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex idxGff = GTUtilsProjectTreeView::findIndex(os, "scaffold_1 features");
    CHECK_SET_ERR(idxGff.isValid(), "Can not find 'scaffold_1 features' object");
    auto seqArea = GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Yes"));
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idxGff, seqArea);

    GTUtilsAnnotationsTreeView::findItem(os, "chromosome  (0, 1)");
}

GUI_TEST_CLASS_DEFINITION(test_3983) {
    //    1. Open file "_common_data\fasta\amino_multy.aln"
    //    2. Open "Pairwise Alignment" OP tab
    //    3. Select first two sequences in the tab
    //    4. Run alignment
    //    Current state: UGENE crashes

    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "amino_multy.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "chr1_gl000191_random_Amino_translation_");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(1, 1));
    GTWidget::click(os, GTUtilsOptionPanelMsa::getAddButton(os, 2));
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    GTWidget::click(os, GTUtilsOptionPanelMsa::getAlignButton(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_3988) {
    //    1. Open "_common_data/zero".
    //    Expected state: a message box appear, it contains a message: "File is empty: ...", the log contains the same message.
    GTLogTracer logTracer;

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "File is empty:"));
    GTUtilsProject::openFileExpectNoProject(os, testDir + "_common_data/zero");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, logTracer, "File is empty:");
}

GUI_TEST_CLASS_DEFINITION(test_3994) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::findWidget(os, "msa_editor_sequence_area");
    QColor before = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(1, 0));
    //    Open the "Highlighting" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    //    Select different highlighting schemes.
    auto highlightingScheme = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, highlightingScheme, "Conservation level");
    auto w = GTWidget::findWidget(os, "thresholdLessRb");
    GTRadioButton::click(os, qobject_cast<QRadioButton*>(w));
    QColor after = GTUtilsMSAEditorSequenceArea::getColor(os, QPoint(1, 0));
    // check color change
    CHECK_SET_ERR(before != after, "colors not changed");
}

GUI_TEST_CLASS_DEFINITION(test_3995) {
    GTLogTracer logTracer;

    //    1. Open "human_T1.fa"
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open circular view
    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction"));

    //    3. Use context menu for exporting view as image
    GTUtilsDialog::add(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "Save circular view as image"));
    GTUtilsDialog::add(os, new DefaultDialogFiller(os, "ImageExportForm"));
    GTWidget::click(os, GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0"), Qt::RightButton);

    //    Expected state: "Export Image" dialog appeared
    //    4. Press "Export" button

    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Bug state: Error message appears: "File path contains illegal characters or too long"

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_3996) {
    GTLogTracer l;

    // 1. Open "samples/ABIF/A01.abi"
    GTFileDialog::openFile(os, dataDir + "/samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Zoom In
    GTWidget::click(os, GTAction::button(os, "action_zoom_in_A1#berezikov"));

    // 3. Click on the chroma view.
    auto chromaView = GTWidget::findWidget(os, "chromatogram_view_A1#berezikov");
    const QRect chromaViewRect = chromaView->rect();

    GTMouseDriver::moveTo(chromaView->mapToGlobal(chromaViewRect.center() - QPoint(20, 0)));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(chromaView->mapToGlobal(chromaViewRect.center() + QPoint(20, 0)));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_3997) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "3000_sequences.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::removeDocument(os, "3000_sequences.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_3998) {
    // Check that UGENE does not crash on 'Replace the whole sequence action'
    GTLogTracer logTracer;

    GTFileDialog::openFile(os, testDir + "_common_data/regression/3998/sequence.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace the whole sequence by", "Complementary (5'-3') sequence"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "sequence.fasta");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/regression/3998/sequence.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace the whole sequence by", "Reverse (3'-5') sequence"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "sequence.fasta");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/regression/3998/sequence.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace the whole sequence by", "Complementary (5'-3') sequence"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "sequence.fasta");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that there are no errors in the log.
    GTUtilsLog::check(os, logTracer);
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
