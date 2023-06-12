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
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QClipboard>
#include <QDebug>
#include <QFile>
#include <QGroupBox>
#include <QMainWindow>
#include <QTextStream>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>
#include <U2View/MSAEditorTreeViewer.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaGraphOverview.h>
#include <U2View/ScrollController.h>

#include "GTTestsRegressionScenarios_4001_5000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsDocument.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
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
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "runnables/ugene/corelibs/U2Gui/AddNewDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportChromatogramFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindRepeatsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportCoverageDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ImportAnnotationsToCsvFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastLocalSearchDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/RemoteBLASTDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SnpEffDatabaseDialogFiller.h"
#include "runnables/ugene/plugins/orf_marker/OrfDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ExportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ImportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/pcr/PrimersDetailsDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithCommandLineToolFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DashboardsManagerDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SelectDocumentFormatDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_4007) {
    GTLogTracer lt;
    //    1. Open file {data/samples/Genbank/murine.gb}
    QDir().mkpath(sandBoxDir + "test_4007");
    GTFile::copy(dataDir + "samples/Genbank/murine.gb", sandBoxDir + "test_4007/murine.gb");
    GTFileDialog::openFile(sandBoxDir + "test_4007", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open file {data/samples/FASTA/human_T1.fa}
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Drag and drop annotations object to the human_T1 sequence.
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(GTUtilsProjectTreeView::findIndex("NC_001363 features"), GTUtilsAnnotationsTreeView::getTreeWidget());

    //    4. Edit "murine.gb" file with an external editor.
    //    Expected state: UGENE offers to reload the changed file.
    //    5. Agree to reload the file.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    QFile murineFile(sandBoxDir + "test_4007/murine.gb");
    bool opened = murineFile.open(QFile::ReadWrite);
    CHECK_SET_ERR(opened, "Can't open the file: " + sandBoxDir + "test_4007/murine.gb");
    murineFile.write("L");
    murineFile.close();
    GTGlobals::sleep(5000);  // Wait until UGENE detects the change.

    GTGlobals::FindOptions murineOptions(false);
    GTUtilsDocument::removeDocument("human_T1.fa");

    QList<U2Region> regs = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    CHECK_SET_ERR(regs.isEmpty(), QString("Annotations are connected to murine.gb %1").arg(regs.isEmpty()));
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");

    //    Expected state: the file is reloaded, annotations object still have an association only with human_T1 sequence (if annotations object exists and has the same name as before reloading),
    // there is no errors in the log.
    GTUtilsProjectTreeView::doubleClickItem("human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTGlobals::sleep(5000);  // Wait until UGENE detects the change.

    GTUtilsAnnotationsTreeView::findFirstAnnotation();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}
GUI_TEST_CLASS_DEFINITION(test_4008) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    //    2. Use Alignment Viewer context menu -> View.
    //    Expected: "Show offsets" option is enabled.
    //    3. Open "_common_data/clustal/big.aln".
    //    4. Use Alignment Viewer context menu -> View.
    //    Expected: "Show offsets" option is enabled.
    //    Current: "Show offsets" option is disabled.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_APPEARANCE, "show_offsets"}, PopupChecker::IsEnabled | PopupChecker::IsCheckable));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTFileDialog::openFile(testDir + "_common_data/clustal/", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_APPEARANCE, "show_offsets"}, PopupChecker::IsEnabled | PopupChecker::IsCheckable));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
}

GUI_TEST_CLASS_DEFINITION(test_4009) {
    // 1. Open file "_common_data/clustal/big.aln"
    GTFileDialog::openFile(testDir + "_common_data/clustal/big.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Open "Export Consensus" OP tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    // 3. Press "Export" button
    GTWidget::click(GTWidget::findWidget("exportBtn"));

    // 4. Remove "big.aln" document
    GTUtilsDocument::removeDocument("big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Bad state: the task hangs, debug error occurred with message "Infinite wait has timed out"
}

GUI_TEST_CLASS_DEFINITION(test_4010) {
    //    1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open the PCR OP tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    //    3. Enter the forward primer: AAAGGAAAAAATGCT.
    GTUtilsOptionPanelSequenceView::setForwardPrimer("AAAGGAAAAAATGCT");

    //    4. Enter the reverse primer: AGCATTTTTTCCTTT.
    GTUtilsOptionPanelSequenceView::setReversePrimer("AGCATTTTTTCCTTT");

    //    5. Click the Primers Details dialog.
    //    Expected: the primers are whole dimers, 14 red lines.
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTTextEdit::containsString(GTWidget::findTextEdit("textEdit"), "||||||||||||||");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new PrimersDetailsDialogFiller(new Scenario));
    GTUtilsOptionPanelSequenceView::showPrimersDetails();
}

GUI_TEST_CLASS_DEFINITION(test_4011) {
    GTLogTracer lt;
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open sample "Align sequences with MUSCLE"
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Align attached file
    GTUtilsWorkflowDesigner::click("Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/regression/4011/human_T1.aln");
    GTUtilsWorkflowDesigner::runWorkflow();
    //    Current state:
    //    Runtime error occurred(x86 version of UGENE)
    //    Windows hangs(x64 version)
    lt.hasMessage("Nothing to write");
}

GUI_TEST_CLASS_DEFINITION(test_4013) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    int columnsNumber = GTUtilsMSAEditorSequenceArea::getNumVisibleBases();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(columnsNumber - 10, 0), QPoint(columnsNumber, 10), GTGlobals::UseMouse);

    GTKeyboardDriver::keyClick(Qt::Key_Space);

    QRect oldRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("ACCCTATTTTATACCAACAAACTare");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(oldRect);
}

GUI_TEST_CLASS_DEFINITION(test_4022) {
    // 1. Put very long sequence into clipboard.
    // 2. Paste it into New document from text dialog and see the warning.
    // Expected: UGENE shows warning & does not crash.

    // 10Mb (40 * 250_000)
    GTClipboard::setText(QString("AAAAAAAAAACCCCCCCCCCGGGGGGGGGGTTTTTTTTTT").repeated(250000));

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findPlainTextEdit("sequenceEdit", dialog));

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "amount of data"));
            GTKeyboardUtils::paste();

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "amount of data"));
            GTKeyboardUtils::paste();

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4026) {
    // 1. Open "samples/Genbank/sars.gb".
    // Expected: there are a lot of annotations in the panoramic and details views.
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Close the MDI window.
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);

    // 3. Double click the sequence in the project.
    GTUtilsProjectTreeView::doubleClickItem("NC_004718");

    // Expected: there is the same amount of annotations in the panoramic and details views.
    // Actual: annotations are now shown in the views. Their locations and qualifier names are deleted.
    GTUtilsAnnotationsTreeView::selectItemsByName({"5'UTR"});
    QString value = GTUtilsAnnotationsTreeView::getQualifierValue("evidence", "5'UTR");
    CHECK_SET_ERR("not_experimental" == value, QString("Unexpected qualifier value"));
}

GUI_TEST_CLASS_DEFINITION(test_4030) {
    // 1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Statistics" tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);

    // 3. Edit the alignment.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(1, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Expected state: hint about reference sequence is hidden
    auto label = GTWidget::findWidget("refSeqWarning");
    CHECK_SET_ERR(!label->isVisible(), "Label is shown");
}

GUI_TEST_CLASS_DEFINITION(test_4033) {
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto cbExistingTable = GTWidget::findWidget("cbExistingTable", dialog);
            auto tbBrowseExistingTable = GTWidget::findWidget("tbBrowseExistingTable", dialog);
            auto leNewTablePath = GTWidget::findWidget("leNewTablePath", dialog);
            auto tbBrowseNewTable = GTWidget::findWidget("tbBrowseNewTable", dialog);

            GTWidget::click(GTWidget::findWidget("rbExistingTable", dialog));
            CHECK_SET_ERR(cbExistingTable->isEnabled() && tbBrowseExistingTable->isEnabled(), "Create annotation dialog controls are disabled unexpectedly");
            CHECK_SET_ERR(!leNewTablePath->isEnabled() && !tbBrowseNewTable->isEnabled(), "Create annotation dialog controls are enabled unexpectedly");

            GTWidget::click(GTWidget::findWidget("rbCreateNewTable", dialog));
            CHECK_SET_ERR(!cbExistingTable->isEnabled() && !tbBrowseExistingTable->isEnabled(), "Create annotation dialog controls are enabled unexpectedly");
            CHECK_SET_ERR(leNewTablePath->isEnabled() && tbBrowseNewTable->isEnabled(), "Create annotation dialog controls are disabled unexpectedly");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Ctrl + N.
    // Expected: if the "Existing table" radio button is not checked, then the URL line edit is disabled.
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new Scenario));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_4034) {
    // 1. Open "samples/Genbank/murine.gb".
    // 2. Ctrl + N.
    // There are two check boxes for switching the location type. Usually, when you choose between several actions (switch them), GUI shows you radio buttons, not check boxes.
    // Check boxes are used to switch on/off an option but not to choose between options. In this dialog, you even can't switch off the check box when you click it.
    // It is a wrong behavior for this graphic primitive
    // Solution: replace the check boxes with radio buttons.

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "exon", "annotation", "200..300", sandBoxDir + "ann_test_4034.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4035) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Click the "Build tree" button on the main toolbar.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFillerPhyML(false, 10));
    GTWidget::click(GTAction::button("Build Tree"));

    // 3. Select the "PhyML" tool, set "Bootstrap" option to 10, build the tree.
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<double> distances = GTUtilsPhyTree::getDistancesValues();
    QList<double> expectedDistances = {0.456, 0.008, 0.227, 0.769, 1.186, 0.277, 0, 0.539, 100};  // Some values from the file.
    for (double expectedDistance : qAsConst(expectedDistances)) {
        CHECK_SET_ERR(distances.contains(expectedDistance), QString("Distances not found: %1").arg(expectedDistance));
    }
}

GUI_TEST_CLASS_DEFINITION(test_4036) {
    //     1. Open "_common_data/clustal/gap_column.aln".
    //     2. MSA sequence area context menu -> Edit -> Remove columns of gaps.
    //     3. Choose "Remove all gap-only columns".
    //     4. Click "Remove".
    //     UGENE 1.16-dev: it take ~15 minutes to remove gaps.
    //     UGENE 1.15.1: it takes ~5 seconds to remove gaps.

    GTFileDialog::openFile(testDir + "_common_data/clustal/", "gap_column.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EDIT, "remove_columns_of_gaps"}));
    GTUtilsDialog::add(new DeleteGapsDialogFiller(1));

    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4045) {
    // 1. Open "murine.gb"
    // 2. Find ORFs
    // 3. Unload "murine.gb"
    // 4. Open "murine.gb"
    // Current state: SAFE_POINT is triggered
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class OkClicker : public Filler {
    public:
        OkClicker()
            : Filler("ORFDialogBase") {
        }
        void run() override {
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new OkClicker());
    GTWidget::click(GTAction::button("Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::unloadDocument("murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::loadDocument("murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4046) {
    //    Open 'human_T1.fa'
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Create a new annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});
    //    Current state: new annotation has an empty qualifier 'note'
    QTreeWidgetItem* ann = GTUtilsAnnotationsTreeView::findItem("ann1");
    //    Expected state: new annotation does not have any qualifiers
    CHECK_SET_ERR(ann->childCount() == 0, QString("Unexpected qualifier number: %1").arg(ann->childCount()));
}

GUI_TEST_CLASS_DEFINITION(test_4047) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class custom : public CustomScenario {
    public:
        void run() override {
            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Raw DNA-Seq Data Processing", new custom()));
    GTUtilsWorkflowDesigner::addSample("Raw DNA-Seq data processing");
    GTThread::waitForMainThread();

    CHECK_SET_ERR(GTAction::button("Show wizard")->isVisible() == false, "'Show wizard' button should not be at toolbar!");
}

GUI_TEST_CLASS_DEFINITION(test_4059) {
    GTLogTracer lt;
    // 1. Open "_common_data/text/text.txt".
    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("Plain text"));
    GTUtilsProject::openFile(testDir + "_common_data/text/text.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Delete the "Text" object.
    GTUtilsProjectTreeView::click("text");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 3. Unload the document.
    MessageBoxDialogFiller* filler = new MessageBoxDialogFiller("No");
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDocument::unloadDocument("text.txt", false);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Load the document.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("text.txt"));
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected: no safe points triggered.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4064) {
    //    1. Copy "_common_data/bam/scerevisiae.bam" and "_common_data/bam/scerevisiae.bam.bai" to separate folder
    //    2. Rename "scerevisiae.bam.bai" to "scerevisiae.bai"
    //    3. Open "scerevisiae.bam" in UGENE
    //    Expected state: "Import BAM file" dialog appeared - there is no "Index is not available" warning message.

    GTFile::copy(testDir + "_common_data/bam/scerevisiae.bam", sandBoxDir + "test_4064.bam");

    class CustomImportBAMDialogFiller : public Filler {
    public:
        CustomImportBAMDialogFiller(bool warningExistence)
            : Filler("Import BAM File"),
              warningExistence(warningExistence) {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto label = GTWidget::findLabel("indexNotAvailableLabel", dialog);
            CHECK_SET_ERR(label->isVisible() == warningExistence, "Warning message is shown");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }

    private:
        bool warningExistence;
    };

    GTUtilsDialog::waitForDialog(new CustomImportBAMDialogFiller(true));
    GTFileDialog::openFile(sandBoxDir, "test_4064.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFile::copy(testDir + "_common_data/bam/scerevisiae.bam.bai", sandBoxDir + "test_4064.bai");
    GTUtilsDialog::waitForDialog(new CustomImportBAMDialogFiller(false));
    GTFileDialog::openFile(sandBoxDir, "test_4064.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4065) {
    /* 1. Open _common_data/scenarios/_regression/4065/example_bam.bam
     * 2. Check log for error: "No bam index given, preparing sequential import"
     */
    GTFile::copy(testDir + "_common_data/scenarios/_regression/4065/example_bam.bam", sandBoxDir + "example_bam.bam");
    GTFile::copy(testDir + "_common_data/scenarios/_regression/4065/example_bam.bam.bai", sandBoxDir + "example_bam.bam.bai");
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "/test_4065.ugenedb"));
    GTFileDialog::openFile(sandBoxDir + "example_bam.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    bool hasMessage = lt.hasMessage("No bam index given");
    CHECK_SET_ERR(false == hasMessage, "Error message is found. Bam index file not found.");
}

GUI_TEST_CLASS_DEFINITION(test_4070) {
    //    1. Open file "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Switch on collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    3. Expand "Conocephalus_discolor" group.
    GTUtilsMsaEditor::toggleCollapsingGroup("Conocephalus_discolor");

    //    Expected state: the overview is calculated and shown.
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* graphOverview = GTUtilsMsaEditor::getGraphOverview();
    CHECK_SET_ERR(nullptr != graphOverview, "Graph overview widget is NULL");

    bool colorFound = false;
    for (int i = 0; i < graphOverview->width() && !colorFound; i++) {
        for (int j = 0; j < graphOverview->height() && !colorFound; j++) {
            if (QColor(0, 0, 0) == GTUtilsMsaEditor::getGraphOverviewPixelColor(QPoint(i, j))) {
                colorFound = true;
            }
        }
    }

    CHECK_SET_ERR(colorFound, "The overview doesn't contain white color");
}

GUI_TEST_CLASS_DEFINITION(test_4071) {
    GTLogTracer lt;
    // 1. Open any file (for example, human_T1.fa).
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the object "[s] human_T1 (UCSC April 2002 chr7:115977709-117855134)".
    GTUtilsProjectTreeView::click("human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    // 3. Press "delete" key.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 4. Unload and load document.
    MessageBoxDialogFiller* filler = new MessageBoxDialogFiller("No");
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDocument::unloadDocument("human_T1.fa", false);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected: no safe points are triggered.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4072) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto hSeqScroll = GTWidget::findWidget("horizontal_sequence_scroll");
    CHECK_SET_ERR(hSeqScroll->isVisible(), "Scroll bar at the bottom of sequence area is invisible");

    auto vSeqScroll = GTWidget::findWidget("vertical_sequence_scroll");
    CHECK_SET_ERR(!vSeqScroll->isVisible(), "Scroll bar at the right side of sequence area is visible");

    auto parent = GTWidget::findWidget("COI [COI.aln]", GTWidget::findWidget("COI [COI.aln]_SubWindow"));
    auto hNameScroll = GTWidget::findWidget("horizontal_names_scroll", parent);

    auto splitter = GTWidget::findSplitter("name_and_sequence_areas_splitter");

    QSplitterHandle* handle = splitter->handle(1);
    CHECK_SET_ERR(handle != nullptr, "MSA Splitter handle is NULL");

    auto nameList = GTWidget::findWidget("msa_editor_name_list");
    GTWidget::click(handle);

    QPoint p = GTMouseDriver::getMousePosition();
    const bool isHorVisible = hNameScroll->isVisible();
    if (isHorVisible) {
        p.setX(p.x() + 3 * nameList->width());
    } else {
        p.setX(p.x() - 2 * nameList->width() / 3);
    }
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p);
    GTMouseDriver::release();

    GTThread::waitForMainThread();

    CHECK_SET_ERR(hNameScroll->isVisible() != isHorVisible, "Scroll bar state at the bottom of name list area isn't changes");

    GTFileDialog::openFile(testDir + "_common_data/clustal/fungal - all.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // remove the longest sequence "MGLR3_Magnaporthe_grisea_AF314" for test stability.
    GTUtilsMsaEditor::removeRows(14, 14);
    GTUtilsProjectTreeView::toggleView();  // Close project view to add om width for the name list.

    parent = GTWidget::findWidget("fungal - all [fungal - all.aln]", GTWidget::findWidget("fungal - all [fungal - all.aln]_SubWindow"));
    hNameScroll = GTWidget::findWidget("horizontal_names_scroll", parent);
    CHECK_SET_ERR(hNameScroll != nullptr, "No scroll bar at the bottom of name list area for fungal-all.aln");
    CHECK_SET_ERR(!hNameScroll->isVisible(), "Scroll bar at the bottom of name list area is visible for fungal-all.aln");
}

GUI_TEST_CLASS_DEFINITION(test_4084) {
    // 1. Open "_common_data/fasta/human_T1_cutted.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta/human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Create any annotation.
    // Expected state: there is an annotation group with an annotation within.
    // Current state: there is an annotation group with two similar annotations within.
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "group", "feature", "50..60"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::findItem("group  (0, 1)");
}

GUI_TEST_CLASS_DEFINITION(test_4086) {
    GTFileDialog::openFile(testDir + "_common_data/fasta/fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("AC\nG\nTG", true);
    GTUtilsTaskTreeView::waitTaskFinished();

    auto label = GTWidget::findLabel("lblErrorMessage");
    CHECK_SET_ERR(label->text().isEmpty(), "There is an error: " + label->text());

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/4"), "Results string does not match");
}

GUI_TEST_CLASS_DEFINITION(test_4087) {
    // Enter "U" to the pattern field.
    // Check "Load patterns from file" option.
    // Expected state: there are no warnings, a search task is not started.

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("U");
    auto label = dynamic_cast<QLabel*>(GTWidget::findWidget("lblErrorMessage"));
    CHECK_SET_ERR(label->isVisible(), "Warning is not shown 1");
    CHECK_SET_ERR(label->text().contains("Warning"), "Warning is not shown 2");

    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();

    if (label->isVisible()) {
        CHECK_SET_ERR(!label->text().contains("Warning"), "Warning is shown");
    }
}

GUI_TEST_CLASS_DEFINITION(test_4091) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::SEQUENCE << GObjectTypes::ANNOTATION_TABLE;
    QMap<QString, QStringList> doc2Objects;
    doc2Objects["human_T1.fa"] << "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    doc2Objects["NC_014267.1.gb"] << "NC_014267"
                                  << "NC_014267 features";
    doc2Objects["PBR322.gb"] << "SYNPBR322"
                             << "SYNPBR322 features";
    doc2Objects["sars.gb"] << "NC_004718"
                           << "NC_004718 features";

    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller(doc2Objects, acceptableTypes, ProjectTreeItemSelectorDialogFiller::Separate));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__ADD_MENU, ACTION_PROJECT__ADD_OBJECT}));
    GTUtilsProjectTreeView::click("CVU55762.gb", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex("CVU55762.gb");
    GTUtilsProjectTreeView::checkItem("CVU55762", docIndex);
    GTUtilsProjectTreeView::checkItem("CVU55762 features", docIndex);
    GTUtilsProjectTreeView::checkItem("human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIndex);
    GTUtilsProjectTreeView::checkItem("NC_014267", docIndex);
    GTUtilsProjectTreeView::checkItem("NC_014267 features", docIndex);
    GTUtilsProjectTreeView::checkItem("SYNPBR322", docIndex);
    GTUtilsProjectTreeView::checkItem("SYNPBR322 features", docIndex);
    GTUtilsProjectTreeView::checkItem("NC_004718", docIndex);
    GTUtilsProjectTreeView::checkItem("NC_004718 features", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_4093) {
    // 1. Open "human_T1.fa"
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Call context menu on the document
    //{ Export / Import->Import annotations from CSV... }
    // Expected state : "Import Annotations from CSV" dialog appeared
    // 3. Select the attached file for reading
    // 4. Copy tab to clipboard and insert to "Column separator" field
    // 5. Select 3rd column as "Start position", 4th column as "End position" and 5th as "Complement strand mark"
    // 6. Press "Run"

    ImportAnnotationsToCsvFiller::RoleParameters r;
    r << ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::StartParameter(false))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::EndParameter(true))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(4, new ImportAnnotationsToCsvFiller::StrandMarkParameter(false, ""));

    ImportAnnotationsToCsvFiller* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/_regression/4093/test.xls", sandBoxDir + "test_4093.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "	", 0, "", true, false, "misc_feature", r);

    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Import annotations from CSV file..."}));
    GTUtilsDialog::add(filler);
    GTUtilsProjectTreeView::click("human_T1.fa", Qt::RightButton);

    // Expected state: UGENE doesn't crash
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4095) {
    /* 1. Open file "test/_common_data/fasta/fa1.fa"
     * 2. Call context menu on the sequence view { Edit sequence -> Reverse sequence }
     *   Expected state: nucleotides order has reversed
     *   Current state: nothing happens
     */
    GTFileDialog::openFile(testDir + "_common_data/fasta", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, ADV_MENU_REPLACE_WHOLE_SEQUENCE, ACTION_EDIT_RESERVE_SEQUENCE}));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTUtilsTaskTreeView::waitTaskFinished();
    // GTCA
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 4));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR("GTCA" == selectionContent, "Sequence reversing is failed");
}

GUI_TEST_CLASS_DEFINITION(test_4096) {
    // 1. Open "human_T1.fa"
    // 2. Use context menu on sequence object
    // {Export/Import->Export sequences as alignment}
    // Expected state: "Export Sequences as Alignment" dialog appeared
    // 3. Press "Export"
    // Current state: only part of human_T1(128000 nb) exported to alignment

    class ExportSeqsAsMsaScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto addToProjectBox = GTWidget::findCheckBox("addToProjectBox", dialog);
            CHECK_SET_ERR(addToProjectBox->isChecked(), "'Add document to project' checkbox is not set");

            auto lineEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(lineEdit, sandBoxDir + "test_4096.aln");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(new ExportSequenceAsAlignmentFiller(new ExportSeqsAsMsaScenario));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkItem("test_4096.aln");

    QString referenceMsaContent = GTFile::readAll(testDir + "_common_data/regression/4096/test_4096.aln");
    QString resultMsaContent = GTFile::readAll(sandBoxDir + "test_4096.aln");
    CHECK_SET_ERR(!referenceMsaContent.isEmpty() && referenceMsaContent == resultMsaContent, "Unexpected MSA content");
}

GUI_TEST_CLASS_DEFINITION(test_4097) {
    /* 1. Open "_common_data/vector_nti_sequence/unrefined.gb".
     * 2. Export the document somewhere to the vector NTI sequence format.
     *   Expected state: the saved file contains only 8 entries in the COMMENT section and doesn't contain "Vector_NTI_Display_Data_(Do_Not_Edit!)" comment.
     */
    GTFileDialog::openFile(testDir + "_common_data/vector_nti_sequence", "unrefined.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ExportDocumentDialogFiller(sandBoxDir, "test_4097.gb", ExportDocumentDialogFiller::VectorNTI, false, false));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Export document"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("unrefined.gb"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    QString resultFileContent = GTFile::readAll(sandBoxDir + "test_4097.gb");
    CHECK_SET_ERR(!resultFileContent.contains("Vector_NTI_Display_Data_(Do_Not_Edit!)", Qt::CaseInsensitive), "Unexpected file content");
    QRegExp rx("COMMENT");
    int pos = 0;
    int count = 0;
    while (pos >= 0) {
        pos = rx.indexIn(resultFileContent, pos);
        if (pos >= 0) {
            ++pos;
            ++count;
        }
    }
    CHECK_SET_ERR(8 == count, "The saved file contains more/less then 8 entries in the COMMENT section");
}
GUI_TEST_CLASS_DEFINITION(test_4099) {
    /* 1. Open file _common_data/scenarios/_regression/4099/p4228.gb
     * 2. Select CDS annotation 1656..2450 and select 'label' item
     *   Expected state: Value has "Tn5 neomycin resistance"
     *   Note: The value of the "label" qualifier is displayed both on the circular view and the zoom view as annotation names.
     */
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4099", "p4228.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems("CDS");
    CHECK_SET_ERR(2 == items.length(), "CDS annotations count is not 2");
    foreach (QTreeWidgetItem* item, items) {
        if (item->text(2) == "1656..2450") {
            GTUtilsAnnotationsTreeView::selectItems({item});
            CHECK_SET_ERR("Tn5 neomycin resistance" == GTUtilsAnnotationsTreeView::getQualifierValue("label", item), "Wrong label value");
            break;
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_4104) {
    GTLogTracer lt;
    // 1. Open the attached workflow file.
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4104/test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set file "data/samples/Genbank/murine.gb" as input.
    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/murine.gb");

    // 3. Run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    // Expected state : a result file has been produced.It's a copy of murine.gb
    // Current state : the "Write Sequence" worker gives the "Nothing to write" error in the log.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());

    QAbstractButton* button = GTWidget::findButtonByText("Dataset 1.gb", GTUtilsDashboard::getDashboard());
    GTWidget::click(button);

    GTUtilsTaskTreeView::waitTaskFinished();

    QString activeWindowName = GTUtilsMdi::activeWindow()->windowTitle();
    CHECK_SET_ERR(activeWindowName == "NC_001363 [Dataset 1.gb]", "Unexpected active window name: " + activeWindowName);
    GTUtilsProjectTreeView::findIndex("NC_001363");
    GTUtilsProjectTreeView::findIndex("NC_001363 features");
}

GUI_TEST_CLASS_DEFINITION(test_4106) {
    /* 1. Open "data/samples/CLUSTALW/ty3.aln.gz".
     * 2. Select a sequence that is two sequences above the last visible sequence in the name list area.
     * 3. Press and hold "shift" key.
     * 4. Click "down" key twice.
     *   Expected state: three sequences are selected, the msa is not scrolled down.
     * 5. Click "down" again.
     *   Expected state: four sequences are selected
     *   Current state: the state is not changed.
     */
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    MSAEditorSequenceArea* msaEdistorSequenceArea = GTUtilsMSAEditorSequenceArea::getSequenceArea();
    const int endPos = msaEdistorSequenceArea->getEditor()->getUI()->getUI(0)->getScrollController()->getLastVisibleViewRowIndex(
        msaEdistorSequenceArea->height());

    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, endPos - 1));
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 1));

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 3));

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 4));
}

GUI_TEST_CLASS_DEFINITION(test_4116) {
    //    1. Open the Primer Library.
    GTUtilsPrimerLibrary::openLibrary();

    //    2. Click "Import primer(s)".

    class Scenario : public CustomScenario {
    public:
        void run() override {
            // Expected: the dialog is modal, the "OK" button is disabled.
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QWidget* okButton = GTUtilsDialog::buttonBox(dialog)->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != nullptr, "Export button is NULL");
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is unexpectedly enabled");

            // 3. Add human_T1.fa.
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
            GTWidget::click(GTWidget::findButtonByText("Add file(s)", dialog));

            // 4. Click the added item.
            const QString filePath = QDir::cleanPath(QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath());
            GTListWidget::click(GTWidget::findListWidget("lwFiles", dialog), filePath);

            // Expected: the "Remove" button is enabled.
            auto removeButton = GTWidget::findWidget("pbRemoveFile", dialog);
            CHECK_SET_ERR(removeButton->isEnabled(), "Remove button is unexpectedly disabled");

            // Expected: the "OK" button is enabled.
            CHECK_SET_ERR(okButton->isEnabled(), "OK button is unexpectedly disabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new Scenario));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Import);
}

GUI_TEST_CLASS_DEFINITION(test_4117) {
    GTLogTracer lt;
    QDir().mkpath(testDir + "_common_data/scenarios/sandbox/space containing dir");
    GTFile::copy(testDir + "_common_data/fastq/short_sample.fastq", testDir + "_common_data/scenarios/sandbox/space containing dir/short_sample.fastq");
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Quality control by FastQC");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("FASTQ File List");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/scenarios/sandbox/space containing dir/short_sample.fastq");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4118) {
    // 1. Add sample raw data processing
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    QMap<QString, QVariant> parameters;
    parameters.insert("FASTQ files", QVariant(QDir().absoluteFilePath(testDir + "_common_data/fastq/lymph.fastq")));
    parameters.insert("Adapters", QVariant(""));

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Raw RNA-Seq Data Processing", {"Skip mapping", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Raw RNA-Seq Data Processing Wizard", QStringList(), parameters));
    GTUtilsWorkflowDesigner::addSample("Raw RNA-Seq data processing");

    GTUtilsWorkflowDesigner::runWorkflow();
    // GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4121) {
    //    1. Open the Primer Library.
    //    2. Select two primes.
    //    3. Click "Export primer(s)".
    //    4. There is no 'Raw' format available(raw format is valid only for one primer export)

    class test_4121 : public CustomScenario {
    public:
        test_4121(bool isRawPresent)
            : isRawPresent(isRawPresent) {
        }
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto cbFormat = GTWidget::findComboBox("cbFormat", dialog);

            if (isRawPresent) {
                CHECK_SET_ERR(cbFormat->findText("Raw sequence") != -1, "raw format is present");
            } else {
                CHECK_SET_ERR(cbFormat->findText("Raw sequence") == -1, "raw format is present");
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }

    private:
        bool isRawPresent;
    };

    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::addPrimer("primer1", "ACGTA");
    GTUtilsPrimerLibrary::addPrimer("primer2", "GTACG");

    GTUtilsPrimerLibrary::selectPrimers(QList<int>() << 0 << 1);

    GTUtilsDialog::waitForDialog(new ExportPrimersDialogFiller(new test_4121(false)));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Export);

    GTUtilsPrimerLibrary::selectPrimers(QList<int>() << 0);

    GTUtilsDialog::waitForDialog(new ExportPrimersDialogFiller(new test_4121(true)));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Export);
}

GUI_TEST_CLASS_DEFINITION(test_4122) {
    /* 1. Open "data/samples/Genbank/murine.gb".
     * 2. Search any existing pattern.
     * 3. Try to create annotations from the search results to the existing annotations table.
     *   Expected state: annotations are created.
     *   Current state: annotations are not created, a safe point is triggered
     */
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("GAGTTCTGAACACCCGGC", true);
    GTUtilsOptionPanelSequenceView::clickGetAnnotation();
    GTUtilsTaskTreeView::waitTaskFinished();

    auto annotationTableItem = GTUtilsAnnotationsTreeView::findItem("NC_001363 features [murine.gb] *");
    GTUtilsAnnotationsTreeView::findItem("misc_feature", annotationTableItem);
}

GUI_TEST_CLASS_DEFINITION(test_4124) {
    /* 1. Select {Tools -> NGS data analysis -> Map reads to reference...} menu item in the main menu.
     * 2. Select UGENE genome aligner, use input data samples/FASTA/human_T1.fa, unset "use 'best'-mode" option. The output path should be default. Align reads.
     *   Expected state: there is some result assembly.
     * 3. Remove the result document from the project, remove the result file. Align reads with same parameters again.
     *   Expected state: there is the same result assembly.
     *   Current state: a message box appears: "Failed to detect file format..."
     */

    class Scenario_test_4124 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto methodNamesBox = GTWidget::findComboBox("methodNamesBox", dialog);
            GTComboBox::selectItemByText(methodNamesBox, "UGENE Genome Aligner");

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
            auto addRefButton = GTWidget::findWidget("addRefButton", dialog);
            GTWidget::click(addRefButton);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
            auto addShortreadsButton = GTWidget::findWidget("addShortreadsButton", dialog);
            GTWidget::click(addShortreadsButton);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir, "out.ugenedb", GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("setResultFileNameButton", dialog));

            auto check = GTWidget::findCheckBox("firstMatchBox");
            GTCheckBox::setChecked(check, 0);

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new Scenario_test_4124()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::click("out.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    QFile::remove(sandBoxDir + "out.ugenedb");
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new Scenario_test_4124()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4127) {
    /* 1. Open attached file _common_data/scenarios/_regression/4127/merged_document.gb
     * 2. Press "Find ORFs" tool button
     *   Expected state: "ORF marker" dialog appeared
     * 3. Open "Output" tab in the dialog
     * 4. Press "Create new table" radio button
     * 5. Press "Ok"
     *   Current state: UGENE crashes
     *   Expected state: UGENE does not crash
     */
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4127", "merged_document.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    class OrfScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(dialog != nullptr, "dialog not found");

            auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);
            GTTabWidget::setCurrentIndex(tabWidget, 1);

            auto radio = GTWidget::findRadioButton("rbCreateNewTable", dialog);
            // GTRadioButton::getRadioButtonByText("Create new table", dialog);
            GTRadioButton::click(radio);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new OrfDialogFiller(new OrfScenario));
    GTWidget::click(GTAction::button("Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4131) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    QString pattern("ATCGTAC");
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "Search in Sequence" options panel tab.
    GTWidget::click(GTWidget::findWidget("OP_FIND_PATTERN"));

    // 3. Enter any valid pattern.
    GTKeyboardDriver::keySequence(pattern);

    // 4. Set output annotations name : -=_\,.<>;:[]#()$
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Annotation parameters"));
    GTLineEdit::setText(GTWidget::findLineEdit("leAnnotationName"), "-=_\\,.<>;:[]#()$");

    // Create annotations from results.
    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : annotations are created, they are named exactly as you set.
    QTreeWidgetItem* annotationGroup = GTUtilsAnnotationsTreeView::findItem("-=_\\,.<>;:[]#()$  (0, 3)");
    CHECK_SET_ERR(annotationGroup->childCount() == 3, "Unexpected annotations count");
}

GUI_TEST_CLASS_DEFINITION(test_4134) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    class custom : public CustomScenario {
    public:
        void run() {
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QString trimBothValue = GTUtilsWizard::getParameter("Trim both ends").toString();

            CHECK_SET_ERR(trimBothValue == "True", "unexpected 'Trim both ends value' : " + trimBothValue);

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Raw DNA-Seq Data Processing Wizard", new custom()));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Raw DNA-Seq Data Processing", {"Single-end"}));
    GTUtilsWorkflowDesigner::addSample("Raw DNA-Seq data processing");
}

GUI_TEST_CLASS_DEFINITION(test_4141) {
    QWidget* appWindow = QApplication::activeWindow();
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Statistics" tab on the Options panel
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);

    // 3. Check "Show distances column"
    // Expected state : distances column has appeared between the name list and the sequence area
    GTCheckBox::setChecked(GTWidget::findCheckBox("showDistancesColumnCheck"));
    GTUtilsMSAEditorSequenceArea::getSimilarityColumn(0);
    CHECK_SET_ERR(QApplication::activeWindow() == appWindow, "Active window changed");
}

GUI_TEST_CLASS_DEFINITION(test_4148) {
    //    0. Remove BWA from external tools.
    //    1. Tools -> NGS data analysis -> Map reads to reference.
    //    2. Method: BWA.
    //    3. Reference: samples/FASTA/human_T1.fa.
    //    4. Reads: samples/FASTA/human_T1.fa.
    //    5. Start.
    //    Expected: UGENE offers to choose the path to BWA executables.
    //    Repeat for other aligners: Bowtie, Bowtie2, BWA SW, MWA MEM

    QStringList aligners;
    aligners << "BWA"
             << "Bowtie aligner"
             << "Bowtie 2 aligner";
    foreach (const QString& al, aligners) {
        GTUtilsExternalTools::removeTool(al);
    }

    class Scenario_test_4148 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
            auto addRefButton = GTWidget::findWidget("addRefButton", dialog);
            GTWidget::click(addRefButton);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA/human_T1.fa"));
            auto addShortreadsButton = GTWidget::findWidget("addShortreadsButton", dialog);
            GTWidget::click(addShortreadsButton);

            QStringList aligners;
            aligners << "BWA"
                     << "BWA-SW"
                     << "BWA-MEM"
                     << "Bowtie"
                     << "Bowtie2";
            auto methodNamesBox = GTWidget::findComboBox("methodNamesBox", dialog);
            foreach (const QString& al, aligners) {
                GTComboBox::selectItemByText(methodNamesBox, al);
                GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
                GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
            }

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new Scenario_test_4148()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_4151) {
    // 1. Open samples / Genbank / murine.gb.
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Hide all views.
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363");
    GTWidget::click(GTWidget::findWidget("show_hide_all_views", toolbar));

    // Expected state: Vertical scroll bar isn't shown.
    auto advScrollArea = dynamic_cast<QScrollArea*>(GTWidget::findWidget("annotated_DNA_scrollarea"));
    CHECK_SET_ERR(!advScrollArea->verticalScrollBar()->isVisible(), "Scrollbar is unexpectedly visible");
}

GUI_TEST_CLASS_DEFINITION(test_4153) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooser({"Show simple overview"}));
    auto overview = GTWidget::findWidget("msa_overview_area_graph");

    GTWidget::click(overview, Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("OP_MSA_HIGHLIGHTING"));
    //    Select different highlighting schemes.
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::selectItemByText(highlightingScheme, "Conservation level");

    auto thresholdSlider = GTWidget::findSlider("thresholdSlider");

    GTSlider::setValue(thresholdSlider, 78);

    auto simpleOverview = GTWidget::findWidget("msa_overview_area_simple");

    QPoint rightBottom = simpleOverview->rect().topRight();
    rightBottom += QPoint(-3, 3);
    QColor curColor = GTWidget::getColor(simpleOverview, rightBottom);
    // GTWidget.getColor()
    GTSlider::setValue(thresholdSlider, 81);
    GTUtilsTaskTreeView::waitTaskFinished();
    QColor newColor = GTWidget::getColor(simpleOverview, rightBottom);
    CHECK_SET_ERR(curColor != newColor, "Color is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_4156) {
    //    1. Open _common_data/query/crash_4156.uql
    //    2. Run the scheme with the human_T1.
    //    Extected state: erro message appeared

    class scenario_4156 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::Resources);

            auto memBox = GTWidget::findSpinBox("memBox", dialog);
            GTSpinBox::setValue(memBox, 256, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    class filler_4156 : public Filler {
    public:
        filler_4156()
            : Filler("RunQueryDlg") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "/samples/FASTA/human_T1.fa"));
            GTWidget::click(GTWidget::findToolButton("tbInFile", dialog));

            auto out = GTWidget::findLineEdit("outFileEdit", dialog);
            GTLineEdit::setText(out, sandBoxDir + "/test_4156.out");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new scenario_4156()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    GTFileDialog::openFile(testDir + "_common_data/query/crash_4156.uql");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new filler_4156());
    QAction* runAction = GTAction::findActionByText("Run Schema...");
    CHECK_SET_ERR(runAction != nullptr, "Run action not found");
    GTWidget::click(GTAction::button(runAction));

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_4160) {
    QString pattern("TTGTCAGATTCACCA");
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("OP_FIND_PATTERN"));

    GTKeyboardDriver::keySequence(pattern);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_4164) {
    GTLogTracer lt;
    QDir().mkpath(testDir + "_common_data/scenarios/sandbox/space dir");
    GTFile::copy(testDir + "_common_data/vcf/valid.vcf", testDir + "_common_data/scenarios/sandbox/space dir/valid.vcf");
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Add sample: snpEff
    GTUtilsWorkflowDesigner::addSample("SnpEff");
    GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);

    // 3. Set input file which contains spaces in path
    GTUtilsWorkflowDesigner::click("Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/scenarios/sandbox/space dir/valid.vcf");

    GTUtilsWorkflowDesigner::click("Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(new SnpEffDatabaseDialogFiller("hg19"));
    GTUtilsWorkflowDesigner::setParameter("Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    // 4. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    GTUtilsTaskTreeView::cancelTask("Execute workflow");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4170) {
    /* Annotation pattern line edit should be disabled if "Use pattern name" option checked.
     * 1. Open file data/samples/FASTA/human_T1.fa
     * 2. enter search pattern :
     *   >pattern1
     *   TGGCAAGCT
     * 3. Expand annotation parameters
     * 4. Set annotation name "pat"
     * 5. Set Use "pattern name" option checked
     *   Expected state: "Annotation name" line edit is disabled
     * 6. Press "Create annotations"
     *   Expected state: here is one created annotation in annotation tree view with name "pattern1"
     */

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence(">pattern1");
    GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::ControlModifier);
    GTKeyboardDriver::keySequence("TGGCAAGCT");

    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget();

    auto annotationNameEdit = GTWidget::findLineEdit("leAnnotationName");
    GTLineEdit::setText(annotationNameEdit, "pat");

    GTCheckBox::setChecked(GTWidget::findCheckBox("chbUsePatternNames"));
    CHECK_SET_ERR(!annotationNameEdit->isEnabled(), "annotationNameEdit is enabled!");

    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    GTUtilsAnnotationsTreeView::findItem("pattern1");

    QTreeWidgetItem* item2 = GTUtilsAnnotationsTreeView::findItem("pat", nullptr, {false});
    CHECK_SET_ERR(item2 == nullptr, "item2 found!");
}

static void changeFontAndSize(const QString& fontFamilyStr, int fontSize) {
    auto fontComboBox = GTWidget::findComboBox("fontComboBox");
    GTComboBox::selectItemByText(fontComboBox, fontFamilyStr);
    GTSpinBox::setValue(GTWidget::findSpinBox("fontSizeSpinBox"), fontSize, GTGlobals::UseMouse);
}

static void fontChecker(const QString& expectedFamilyStr, int expectedSize) {
    QString comboText = GTComboBox::getCurrentText("fontComboBox");
    CHECK_SET_ERR(comboText == expectedFamilyStr, "unexpected style: " + comboText);
    int actualSize = GTSpinBox::getValue(GTWidget::findSpinBox("fontSizeSpinBox"));
    CHECK_SET_ERR(actualSize == expectedSize, QString("unexpected point size: %1").arg(QString::number(actualSize)));
}

static void getFontSettings(QString& familyStr, int& size) {
    familyStr = GTComboBox::getCurrentText("fontComboBox");
    size = GTSpinBox::getValue(GTWidget::findSpinBox("fontSizeSpinBox"));
}

GUI_TEST_CLASS_DEFINITION(test_4177) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Let more space for the tree view.

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTWidget::click(GTWidget::findWidget("buildTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QString defaultFontFamily;
    int defaultSize;

    // Open samples/CLUSTALW/COI.aln and build tree for it
    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText("0.009", "0.026");
    // 2. Select node, change font size to 16, also remember default parameters
    GTUtilsPhyTree::clickNode(node);
    getFontSettings(defaultFontFamily, defaultSize);
    changeFontAndSize(defaultFontFamily, 16);

    QString customFontName = isOsLinux() ? "Times New Roman" : "Arial";

    // Click on the parent node for node.
    // Change its font to Arial with size 22.
    TvNodeItem* parentNode = GTUtilsPhyTree::getNodeByBranchText("0.006", "0.104");
    GTUtilsPhyTree::clickNode(parentNode);
    changeFontAndSize(customFontName, 22);
    // 4. Go back to first one node
    // Expected state: its font became 'customFontName' with size 22
    GTUtilsPhyTree::clickNode(node);
    fontChecker(customFontName, 22);
    // 5. Change font to default
    changeFontAndSize(defaultFontFamily, 22);
    // 6. Select parent node again
    // Expected state: font still 'customFontName' with size 22
    GTUtilsPhyTree::clickNode(parentNode);
    fontChecker(customFontName, 22);
    // 7. Change font and size to defaults
    changeFontAndSize(defaultFontFamily, defaultSize);
    // 8. Select first node
    // Expected state: font and size now became default
    GTUtilsPhyTree::clickNode(node);
    fontChecker(defaultFontFamily, defaultSize);
}

GUI_TEST_CLASS_DEFINITION(test_4177_1) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Let more space for the tree view.

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("default", 0, 0, true));
    GTWidget::click(GTWidget::findWidget("buildTreeButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QString defaultFontFamily;
    int defaultSize;

    TvNodeItem* node = GTUtilsPhyTree::getNodeByBranchText("0.009", "0.026");
    GTUtilsPhyTree::clickNode(node);
    getFontSettings(defaultFontFamily, defaultSize);
    changeFontAndSize(defaultFontFamily, 16);
    // 3. Close OP tab
    GTWidget::click(GTWidget::findWidget("OP_MSA_TREES_WIDGET"));
    // 4. Click to empty space near the node to reset selection
    GTThread::waitForMainThread();
    auto treeView = GTWidget::findGraphicsView("treeView");
    QPointF sceneCoord = node->mapToScene(node->boundingRect().topLeft());
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);
    globalCoord += QPoint(node->boundingRect().width() / 2 + 8, node->boundingRect().height() / 2 + 8);
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
    // 5. Open OP tab
    // Expected state: font and size are default
    GTWidget::click(GTWidget::findWidget("OP_MSA_TREES_WIDGET"));
    fontChecker(defaultFontFamily, defaultSize);
}

GUI_TEST_CLASS_DEFINITION(test_4179) {
    // 1. Open file "data/samples/Genabnk/sars.gb"
    // Current state: Two words are merged into a single one, in the file they are separated by a newline symbol.
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::selectItemsByName({"comment"});
    QString qualifier = GTUtilsAnnotationsTreeView::getQualifierValue("1", "comment");
    CHECK_SET_ERR(qualifier.indexOf("The reference") > 0, "Expected string is not found");
}

GUI_TEST_CLASS_DEFINITION(test_4188_1) {
    // 1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select area: from 16 to 26 col, from 6 to 16 seq
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(16, 6), QPoint(26, 16), GTGlobals::UseMouse);

    // 3. Move left border to the left
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(3, -2);

    // Expected selection x: 14, y: 6, width: 13, height: 11
    QRect selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(14, 6, 13, 11),
                  QString("Unexpected selection. Expected x: 14, y: 6, width: 13, height: 11. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 4. Move top border upper
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(0, -2);

    // Expected selection x: 14, y: 4, width: 13, height: 13
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(14, 4, 13, 13),
                  QString("Unexpected selection. Expected x: 14, y: 4, width: 13, height: 13. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 5. Move right border to the left
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(1, -3);

    // Expected selection x: 14, y: 4, width: 10, height: 13
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(14, 4, 10, 13),
                  QString("Unexpected selection. Expected x: 14, y: 4, width: 13, height: 10. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 5. Move bottom border upper
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(2, -3);

    // Expected selection x: 14, y: 4, width: 10, height: 10
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(14, 4, 10, 10),
                  QString("Unexpected selection. Expected x: 14, y: 4, width: 10, height: 10. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 6. Move top right corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(4, 1);

    // Expected selection x: 14, y: 3, width: 11, height: 11
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(14, 3, 11, 11),
                  QString("Unexpected selection. Expected x: 14, y: 3, width: 11, height: 11. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 7. Move bottom right corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(5, -4);

    // Expected selection x: 14, y: 3, width: 7, height: 7
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(14, 3, 7, 7),
                  QString("Unexpected selection. Expected x: 14, y: 3, width: 7, height: 7. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 8. Move bottom left corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(6, 2);

    // Expected selection x: 16, y: 3, width: 5, height: 5
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(16, 3, 5, 5),
                  QString("Unexpected selection. Expected x: 16, y: 3, width: 5, height: 5. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));

    // 9. Move top left corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(7, 3);

    // Expected selection x: 19, y: 6, width: 2, height: 2
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selectedRect == QRect(19, 6, 2, 2),
                  QString("Unexpected selection. Expected x: 19, y: 6, width: 2, height: 2. Current x: %1, y: %2, width: %3, height: %4")
                      .arg(selectedRect.x())
                      .arg(selectedRect.y())
                      .arg(selectedRect.width())
                      .arg(selectedRect.height()));
}

GUI_TEST_CLASS_DEFINITION(test_4188_2) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select region from 40000 to 60000
    GTUtilsSequenceView::selectSequenceRegion(40000, 60000);

    // 3. Drag and drop left border to the right in the Zoom View
    PanViewRenderArea* panViewRenderArea = GTUtilsSequenceView::getPanViewByNumber()->getRenderArea();
    CHECK_SET_ERR(panViewRenderArea != nullptr, "PanViewRenderArea not found");

    const double panScale = panViewRenderArea->getCurrentScale();
    const QPoint startPan((int)(40000 * panScale), panViewRenderArea->height() / 2);
    const QPoint startPanGlobal = panViewRenderArea->mapToGlobal(startPan);
    const QPoint endPanGlobal(startPanGlobal.x() + panViewRenderArea->width() / 2, startPanGlobal.y());
    GTMouseDriver::dragAndDrop(startPanGlobal, endPanGlobal);

    // Expected: one region, start - 60000
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    U2Region sel = selection.first();
    CHECK_SET_ERR(sel.startPos == 60000, QString("Unexpected selected region start, expected: 60000, current: %1").arg(sel.startPos));

    // 4. Select several regions
    GTUtilsSequenceView::selectSeveralRegionsByDialog("40000..60000,80000..100000");

    // 5. Join regions by moving the right border of the left region to the right in the Zoom View
    const QPoint startMultiplePan((int)(60000 * panScale), panViewRenderArea->height() / 2);
    const QPoint startMultiplePanGlobal = panViewRenderArea->mapToGlobal(startMultiplePan);
    const QPoint endMultiplePanGlobal(startMultiplePanGlobal.x() + (int)(30000 * panScale), startMultiplePanGlobal.y());
    GTMouseDriver::dragAndDrop(startMultiplePanGlobal, endMultiplePanGlobal);

    // Expected: one region, start - 39999, end - 1000000
    selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    sel = selection.first();
    CHECK_SET_ERR(sel == U2Region(39999, 60001), QString("Unexpected selected region. Expected start: 39999, end: 100000. Current start: %1, end: %2").arg(sel.startPos).arg(sel.endPos()));
}

GUI_TEST_CLASS_DEFINITION(test_4188_3) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Turn off wrap mode
    QAction* wrapMode = GTAction::findActionByText("Wrap sequence");
    CHECK_SET_ERR(wrapMode != nullptr, "Cannot find Wrap sequence action");
    GTWidget::click(GTAction::button(wrapMode));

    // 3. Select region from 10 to 20
    GTUtilsSequenceView::selectSequenceRegion(10, 20);

    // 4. Drag and drop left border to the right in the Details View
    DetViewRenderArea* detViewRenderArea = GTUtilsSequenceView::getDetViewByNumber()->getDetViewRenderArea();
    CHECK_SET_ERR(detViewRenderArea != nullptr, "DetViewRenderArea not found");

    const double detScale = detViewRenderArea->getCurrentScale();
    const QPoint startDet((int)(9 * detScale), detViewRenderArea->height() / 2);
    const QPoint startDetGlobal = detViewRenderArea->mapToGlobal(startDet);
    const QPoint endDetGlobal(startDetGlobal.x() + detViewRenderArea->width() / 2, startDetGlobal.y());

    GTMouseDriver::dragAndDrop(startDetGlobal, endDetGlobal);

    // Expected: one region, start - 20
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    U2Region sel = selection.first();
    CHECK_SET_ERR(sel.startPos == 20, QString("Unexpected selected region start, expected: 20, current: %1").arg(sel.startPos));

    // 5. Select several regions
    GTUtilsSequenceView::selectSeveralRegionsByDialog("10..20,30..40");

    // 6. Join regions by moving the right border of the left region to the right in the Zoom View
    const QPoint startMultipleDet((int)(20 * detScale), detViewRenderArea->height() / 2);
    const QPoint startMultipleDetGlobal = detViewRenderArea->mapToGlobal(startMultipleDet);
    const QPoint endMultipleDetGlobal(startMultipleDetGlobal.x() + (int)(15 * detScale), startMultipleDetGlobal.y());
    GTMouseDriver::dragAndDrop(startMultipleDetGlobal, endMultipleDetGlobal);

    // Expected: one region, start - 9, end - 40
    selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    sel = selection.first();
    CHECK_SET_ERR(sel == U2Region(9, 31), QString("Unexpected selected region. Expected start: 9, end: 40. Current start: %1, end: %2").arg(sel.startPos).arg(sel.endPos()));
}

GUI_TEST_CLASS_DEFINITION(test_4194) {
    /* 1. Open WD
     * 2. Add element "Filter Annotations by Name"
     * Expected state: It should have two parameters: "Annotation names" and "Annotation names file"
     * 3. Click the toolbar button "Validate workflow"
     * Expected state: Showed error message about empty fields
     */

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    WorkflowProcessItem* filter = GTUtilsWorkflowDesigner::addElement("Filter Annotations by Name");
    CHECK_SET_ERR(filter != nullptr, "Failed to add an element Filter annotations by name");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Validate workflow"));
    int countErrors = GTUtilsWorkflowDesigner::checkErrorList("At least one of these parameters must be set");
    CHECK_SET_ERR(countErrors == 1, QString("Errors count dont match, should be 1 validation error, but %1 errors now").arg(countErrors));
}

GUI_TEST_CLASS_DEFINITION(test_4209) {
    // Run a task with 10k reads to align (total run time is 20-30 minutes).
    // Check that the task runs correctly.
    // Cancel the task: check that UI is not frozen and the task can be canceled correctly.
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());  // Workflow dir selector.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4209/", "crash.uwl");
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();

    GTUtilsWorkflowDesigner::click("Align to Reference");
    GTUtilsWorkflowDesigner::setParameter("Reference URL", testDir + "_common_data/scenarios/_regression/4209/seq1.gb", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Result alignment URL", QDir(sandBoxDir).absolutePath() + "/4209.ugenedb", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", testDir + "_common_data/reads/e_coli_10000snp.fa");

    GTUtilsWorkflowDesigner::runWorkflow();

    // Wait for some period to ensure that the long-running sub-task is started with no crash and cancel it next.
    GTGlobals::sleep(10000);
    GTUtilsTaskTreeView::cancelTask("Execute workflow");
}

GUI_TEST_CLASS_DEFINITION(test_4209_1) {
    // Run a task with 1k reads to align (total run time is 2-3 minutes).
    // Check that the task finishes with no errors.
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4209/crash.uwl");
    GTUtilsWorkflowDesigner::checkWorkflowDesignerWindowIsActive();

    GTUtilsWorkflowDesigner::click("Align to Reference");
    GTUtilsWorkflowDesigner::setParameter("Reference URL", testDir + "_common_data/scenarios/_regression/4209/seq1.gb", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Result alignment URL", QDir(sandBoxDir).absolutePath() + "/4209.ugenedb", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", testDir + "_common_data/reads/e_coli_1000.fa");

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // OMG!
    // The only error we have today is the error about missed chromatogram.
    // The error is not correct: "Align to Reference" with BLAST does not need/use a chromatogram at all.
    // The error was introduced during the time the test was suppressed and made impossible to run "Align to Reference" for reads with no chromatograms.
    // See: UGENE-5423: Use ChromObject in Sanger algorithm.
    // This problem will be addressed in the separate bug and logTracer will be checked for no errors.
    CHECK_SET_ERR(lt.errorMessages.size() == 1 && lt.errorMessages[0].contains("The related chromatogram not found"),
                  "Got unexpected error: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4218) {
    // Check that "Write Annotations" worker takes into account object names of incoming annotation tables
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/regression/4218/test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addInputFile("Read Annotations", testDir + "_common_data/bedtools/introns.bed");
    GTUtilsWorkflowDesigner::click("Write Annotations");
    QString outputFilePath = QDir(sandBoxDir).absolutePath() + "/out.bed";
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir + "out.bed");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkItem("ann features");
}

GUI_TEST_CLASS_DEFINITION(test_4218_1) {
    // Check that an output annotation object has a default name if incoming annotation objects have different names
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/regression/4218/test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addInputFile("Read Annotations", testDir + "_common_data/regression/4218/test.bed");
    GTUtilsWorkflowDesigner::click("Write Annotations");
    QString outputFilePath = QDir(sandBoxDir).absolutePath() + "/out.bed";
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir + "out.bed");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkItem("chr2 features");
    GTUtilsProjectTreeView::checkItem("chr5 features");
}

GUI_TEST_CLASS_DEFINITION(test_4221) {
    //    1. Use main menu
    //    {tools->NGS data analysis->Map reads to reference}
    //    2.Fill the dialog:
    //    Mapping tool: BWA
    //    Index algorithm: is
    //    Select any valid input data
    //    Press start
    //    Expected state: UGENE doesn't crash.
    GTLogTracer lt;

    AlignShortReadsFiller::BwaParameters parameters(testDir + "_common_data/reads/eas.fa", testDir + "_common_data/reads/eas.fastq");
    parameters.useDefaultResultPath = false;
    parameters.resultDir = sandBoxDir;
    parameters.resultFileName = "test_4221.sam";
    parameters.indexAlgorithm = AlignShortReadsFiller::BwaParameters::Is;
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "test_4221.ugenedb"));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("test_4221.ugenedb");

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4232) {
    // 1. Open a file with variations
    GTFileDialog::openFile(testDir + "_common_data/vcf/valid.vcf");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open a file with a sequence
    GTFileDialog::openFile(testDir + "_common_data/fasta/illumina.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 3. Open a file with an assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Drag&drop the sequence object from the project view on the assembly view
    const QModelIndex sequenceDocIndex = GTUtilsProjectTreeView::findIndex("illumina.fa");
    const QModelIndex sequenceObjIndex = sequenceDocIndex.model()->index(0, 0, sequenceDocIndex);

    GTUtilsProjectTreeView::dragAndDrop(sequenceObjIndex, GTWidget::findWidget("assembly_reads_area"));

    // Expected state: sequence object and document are highlighted in the Project view
    QFont itemFont = GTUtilsProjectTreeView::getFont(sequenceObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Sequence object item isn't highlighted in Project view");
    itemFont = GTUtilsProjectTreeView::getFont(sequenceDocIndex);
    CHECK_SET_ERR(itemFont.bold(), "Sequence document item isn't highlighted in Project view");

    // 4. Drag&drop the variations object from the project view on the assembly view
    const QModelIndex variationsObjIndex = GTUtilsProjectTreeView::findIndex("II");
    GTUtilsProjectTreeView::dragAndDrop(variationsObjIndex, GTWidget::findWidget("assembly_reads_area"));

    // Expected state: variations object is highlighted in the Project view
    itemFont = GTUtilsProjectTreeView::getFont(variationsObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Variations object item isn't highlighted in Project view");

    // 5. Remove the variations from the assembly view
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove track from the view"}));
    GTWidget::click(GTWidget::findWidget("AssemblyVariantRow_II"), Qt::RightButton);

    // Expected state: the variations object isn't highlighted in the Project view
    itemFont = GTUtilsProjectTreeView::getFont(variationsObjIndex);
    CHECK_SET_ERR(!itemFont.bold(), "Variations object item is unexpectedly highlighted in Project view");

    // 6. Remove the sequence from the assembly view
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Unassociate"}));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);

    // Expected state: the sequence object and document aren't highlighted in the Project view
    itemFont = GTUtilsProjectTreeView::getFont(sequenceObjIndex);
    CHECK_SET_ERR(!itemFont.bold(), "Sequence object item is unexpectedly highlighted in Project view");
    itemFont = GTUtilsProjectTreeView::getFont(sequenceDocIndex);
    CHECK_SET_ERR(!itemFont.bold(), "Sequence document item is unexpectedly highlighted in Project view");
}

GUI_TEST_CLASS_DEFINITION(test_4232_1) {
    // 1. Open file with a chromatogram
    GTFileDialog::openFile(dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: a chromatogram object and a sequence object are highlighted in the Project view
    const QModelIndex seqObjIndex = GTUtilsProjectTreeView::findIndex("A1#berezikov");
    QFont itemFont = GTUtilsProjectTreeView::getFont(seqObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Sequence object item isn't highlighted in Project view");

    const QModelIndex chromatogramObjIndex = GTUtilsProjectTreeView::findIndex("Chromatogram");
    itemFont = GTUtilsProjectTreeView::getFont(chromatogramObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Chromatogram object item isn't highlighted in Project view");
}

GUI_TEST_CLASS_DEFINITION(test_4244) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();

            auto leDescription = GTWidget::findLineEdit("leDescription", dialog);
            GTWidget::click(leDescription);
            GTClipboard::setText(QString('a').repeated(32000));
            GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

            auto leAnnotationName = GTWidget::findLineEdit("leAnnotationName", dialog);
            GTLineEdit::setText(leAnnotationName, "name");

            auto leRegionStart = GTWidget::findLineEdit("leRegionStart", dialog);
            GTLineEdit::setText(leRegionStart, "10");

            auto leRegionEnd = GTWidget::findLineEdit("leRegionEnd", dialog);
            GTLineEdit::setText(leRegionEnd, "20");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(new Scenario()));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "test_4244", ExportAnnotationsFiller::gff));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("name"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "test_4244.gb", ExportAnnotationsFiller::genbank));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("name"));
    GTMouseDriver::click(Qt::RightButton);

    GTFileDialog::openFile(sandBoxDir + "test_4244.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChecker({"action_project__unload_selected_action"}, PopupChecker::IsEnabled));
    GTUtilsProjectTreeView::click("test_4244.gb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4266) {
    //    1. Create the workflow: Read Sequence --> Write Sequence
    //    2. Input data: "_common_data/fasta/Gene.fa"
    //    3. Run the workflow
    //    Expected state: there is a warning about empty sequences on the dashboard

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    CHECK_SET_ERR(read != nullptr, "Failed to add an element");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/Gene.fa");

    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);
    CHECK_SET_ERR(write != nullptr, "Failed to add an element");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_4266.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(read, write);
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::findLabelByText("Loaded sequences: 139.", GTUtilsDashboard::getDashboard());
}

GUI_TEST_CLASS_DEFINITION(test_4272) {
    //    Open any sequence
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto mainTb = GTWidget::findWidget("mwtoolbar_activemdi");
    auto qt_toolbar_ext_button = GTWidget::findWidget("qt_toolbar_ext_button", mainTb, {false});
    if (qt_toolbar_ext_button != nullptr && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(qt_toolbar_ext_button);
    }
    //    Turn on auto-annotations (e.g. find ORFs)
    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: annotations are displayed in zoom, details and annotations views
    GTUtilsAnnotationsTreeView::findItem("orf  (0, 837)");
    //    Turn auto-annotations OFF
    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: no annotations are displayed
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("orf  (0, 837)", nullptr, {false});
    CHECK_SET_ERR(item == nullptr, "orfs are unexpectidly shown");
}

GUI_TEST_CLASS_DEFINITION(test_4276) {
    // 1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use context menu
    //{Add->Sequence from file...}
    // Expected: "Open file with sequences" dialog appeared
    // 3. Select file "_common_data/fasta/PF07724_full_family.fa"
    // 4. Press "Open"
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/PF07724_full_family.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // 5. Delete the alignment object: current state: UGENE crashes.
    GTUtilsProjectTreeView::click("COI");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsTaskTreeView::checkTaskIsPresent("Add sequences to alignment task", false);
}

GUI_TEST_CLASS_DEFINITION(test_4284) {
    //    1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select a sequence that is two sequences above the last visible sequence in the name list area.
    MSAEditorSequenceArea* msaEdistorSequenceArea = GTUtilsMSAEditorSequenceArea::getSequenceArea();
    const int endPos = msaEdistorSequenceArea->getEditor()->getUI()->getUI(0)->getScrollController()->getLastVisibleViewRowIndex(
        msaEdistorSequenceArea->height());

    GTUtilsMsaEditor::clickSequence(endPos - 1);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 1));

    //    3. Press and hold "shift" key.
    //    4. Click "down" key once.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    //    Expected state: two sequences are selected, the msa is not scrolled down.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 2));

    //    4. Click "down" key again.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    //    Expected state: three sequences are selected, the msa is scrolled down for one line.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 3));

    //    5. Click "down" key again.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    //    Expected state: four sequences are selected, the msa is scrolled down for two lines.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, endPos - 1, 1234, 4));

    const int firstVisibleSequence = msaEdistorSequenceArea->getEditor()->getUI()->getUI(0)->getScrollController()->getFirstVisibleViewRowIndex(
        false);
    CHECK_SET_ERR(firstVisibleSequence == 2, QString("MSA scrolled incorrectly: expected first fully visible sequence %1, got %2").arg(2).arg(firstVisibleSequence));
}

GUI_TEST_CLASS_DEFINITION(test_4293) {
    // Check that context menu in Tree Viewer has expected action & states.
    GTFileDialog::openFile(testDir + "_common_data/newick/sample5.newick");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();

    TvNodeItem* rootNode = GTUtilsPhyTree::getRootNode();
    TvNodeItem* childNode = GTUtilsPhyTree::getNodeByBranchText("0.336", "0.061");

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Reroot Tree"}, PopupChecker::IsDisabled));
    GTUtilsPhyTree::clickNode(rootNode, Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Collapse"}, PopupChecker::IsDisabled));
    GTUtilsPhyTree::clickNode(rootNode, Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    QList<TvNodeItem*> selectedNodes = GTUtilsPhyTree::getSelectedNodes();
    CHECK_SET_ERR(selectedNodes.size() == 12, QString("1. Unexpected number of selected nodes: %1").arg(selectedNodes.size()));

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Reroot tree"}));
    GTUtilsPhyTree::clickNode(childNode, Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Collapse"}));
    GTUtilsPhyTree::clickNode(childNode, Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Expand"}));
    GTUtilsPhyTree::clickNode(childNode, Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    selectedNodes = GTUtilsPhyTree::getSelectedNodes();
    CHECK_SET_ERR(selectedNodes.size() == 7, QString("2. Unexpected number of selected nodes: %1").arg(selectedNodes.size()));
    CHECK_SET_ERR(!rootNode->isSelected(), "Root not must not be selected");
    CHECK_SET_ERR(childNode->isSelected(), "Child node must be selected");
}

GUI_TEST_CLASS_DEFINITION(test_4295) {
    /* 1. Open Workflow Designer
     * 2. Add elements Read File URL(s) and Write Plain Text
     * 3. Set as input file human_T1.fa
     * 3. Add "Element with CMD" _common_data/scenarios/_regression/4295/test_4295.etc
     * 4. Connect elements Read File URL(s) > Element with CMD > Write Plain Text
     * 5. Run workflow
     * Expected state: no errors in log
     */
    // clean up
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::removeCmdlineWorkerFromPalette("test_4295");
    GTFile::copy(testDir + "_common_data/scenarios/_regression/4295/test_4295.etc", sandBoxDir + "test_4295.etc");

    // start test
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::addElement("Read File URL(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    GTUtilsWorkflowDesigner::addElement("Write Plain Text");

    GTFileDialogUtils* ob = new GTFileDialogUtils(sandBoxDir, "test_4295.etc");
    GTUtilsDialog::waitForDialog(ob);

    QAbstractButton* button = GTAction::button("AddElementWithCommandLineTool");
    GTWidget::click(button);

    GTUtilsWorkflowDesigner::click("test_4295");

    WorkflowProcessItem* element = GTUtilsWorkflowDesigner::getWorker("test_4295");
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read File URL(s)"), element);
    GTUtilsWorkflowDesigner::connect(element, GTUtilsWorkflowDesigner::getWorker("Write Plain Text"));
    GTUtilsWorkflowDesigner::click("test_4295");
    QTableWidget* table = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    GTUtilsWorkflowDesigner::setTableValue("Plain text", "Source URL (by Read File URL(s))", GTUtilsWorkflowDesigner::comboValue, table);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4302_1) {
    // 1. Open samples/Genbank/sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select any region
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 4));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("select_range_action"));
    // 3. Open main menu "Actions"
    // Expected state: "Replace subsequence" menu item enabled
    GTUtilsDialog::waitForDialog(new ReplaceSubsequenceDialogFiller("ACCCT"));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace subsequence..."});
}

GUI_TEST_CLASS_DEFINITION(test_4302_2) {
    // 1. Open samples/Genbank/sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select any annotation
    GTUtilsTaskTreeView::waitTaskFinished();
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("CDS"));
    GTMouseDriver::click();
    // 3. Open main menu "Actions"
    // Expected state: menu item {Remove->Selected annotation and qualifiers} are enabled
    GTMenu::clickMainMenuItem({"Actions", "Remove", "Selected annotations and qualifiers"});
}

GUI_TEST_CLASS_DEFINITION(test_4306_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Build a tree with default parameters.
    //    Expected state: a tree view is displayed with the alignment editor.
    QDir().mkpath(sandBoxDir + "test_4306");

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4306/test_4306.nwk", 0, 0, true));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Use context menu on tree view.
    //    Expected state: there are "Zoom in", "Zoom out" and "Reset zooming" actions in the menu.
    //    Expected state: there are "Zoom in", "Zoom out" and "Reset zooming" actions in the menu.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Zoom In"}, PopupChecker::IsEnabled));
    GTWidget::click(GTUtilsMsaEditor::getTreeView(), Qt::RightButton);

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Zoom Out"}, PopupChecker::IsEnabled));
    GTWidget::click(GTUtilsMsaEditor::getTreeView(), Qt::RightButton);

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Reset Zooming"}, PopupChecker::IsEnabled));
    GTWidget::click(GTUtilsMsaEditor::getTreeView(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4306_2) {
    //    1. Open "data/samples/Newick/COI.nwk".
    GTFileDialog::openFile(dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Use context menu on the tree view.
    //    Expected state: there are "Zoom in", "Zoom out" and "Reset zooming" actions in the menu.
    QStringList items = {"Zoom In", "Zoom Out", "Reset Zooming"};
    GTUtilsDialog::waitForDialog(new PopupCheckerByText(QStringList(), items));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4308) {
    //    1. Open "_common_data/clustal/10000_sequences.aln".
    GTFileDialog::openFile(testDir + "_common_data/fasta/PF07724_full_family.fa", GTFileDialog::Open, GTGlobals::UseMouse);
    GTUtilsTask::waitTaskStart("Loading documents");

    //    2. Remove the document while the file is opening.
    GTUtilsProjectTreeView::checkItem("PF07724_full_family.fa");
    GTUtilsNotifications::waitForNotification(true, "Document was removed");
    GTUtilsDocument::removeDocument("PF07724_full_family.fa");

    //    Expected state: the document is removed from the project, the loading task is canceled, a notification about the canceled task appears.
    bool itemExists = GTUtilsProjectTreeView::checkItem("PF07724_full_family.fa", {false});
    CHECK_SET_ERR(!itemExists, "The document is not removed from the project");
    GTUtilsTask::checkNoTask("Loading documents");
}

GUI_TEST_CLASS_DEFINITION(test_4309) {
    // 1. Open sars.gb
    // 2. Context menu on annotations object in project view: {Export/Import --> Export annotations}
    // Expected state: export annotataions dialog appeared
    // 3. Check format combobox
    // Expected state: Vector NTI format is abcent

    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class VectorNTIFormatChecker : public Filler {
    public:
        VectorNTIFormatChecker()
            : Filler("U2__ExportAnnotationsDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QComboBox* comboBox = dialog->findChild<QComboBox*>();
            CHECK_SET_ERR(comboBox != nullptr, "ComboBox not found");

            QStringList formats = GTComboBox::getValues(comboBox);
            CHECK_SET_ERR(!formats.contains("Vector NTI sequence"), "VectorNTI format is present in annotations export dialog");

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);

            QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != nullptr, "cancelButton is NULL");
            GTWidget::click(cancelButton);
        }
    };

    GTUtilsDialog::waitForDialog(new VectorNTIFormatChecker());
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "ep_exportAnnotations2CSV"}));
    GTUtilsProjectTreeView::click("NC_004718 features", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4309_1) {
    // 1. Open WD
    // 2. Add Write Annotations element
    // Expected state: Vector NTI format is not available in Format parameter combobox

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Write annotations");

    auto table = GTWidget::findTableView("table");

    QAbstractItemModel* model = table->model();
    int iMax = model->rowCount();
    int row = -1;
    for (int i = 0; i < iMax; i++) {
        QString s = model->data(model->index(i, 0)).toString();
        if (s.compare("Document format", Qt::CaseInsensitive) == 0) {
            row = i;
            break;
        }
    }
    CHECK_SET_ERR(row != -1, QString("Document format parameter not found"));
    table->scrollTo(model->index(row, 1));

    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, row));
    GTMouseDriver::click();

    auto box = qobject_cast<QComboBox*>(table->findChild<QComboBox*>());
    CHECK_SET_ERR(box, "QComboBox not found. Widget in this cell might be not QComboBox");
    QString vectorNtiFormatName = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::VECTOR_NTI_SEQUENCE)->getFormatName();
    QString genbankFormatName = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK)->getFormatName();
    QStringList boxData = GTComboBox::getValues(box);
    CHECK_SET_ERR(boxData.contains(vectorNtiFormatName) == false, "Vector NTI format is present in WriteAnnotations worker");
    CHECK_SET_ERR(boxData.contains(genbankFormatName), "GenBank format isn't present in WriteAnnotations worker");
}

GUI_TEST_CLASS_DEFINITION(test_4323_1) {
    GTLogTracer lt;

    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "align_new_sequences_to_alignment_action" and select "_common_data/database.ini".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/database.ini"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: load task fails, safe point doesn't trigger.

    CHECK_SET_ERR(lt.hasError("Task {Load sequences and add to alignment task} finished with error: Data from the \"database.ini\" file can't be alignment to the \"COI\" alignment - there are no sequences to align in the document(s)"), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_4323_2) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "align_new_sequences_to_alignment_action" button on the toolbar, select "samples/PDB/1CF7.pdb".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/PDB/1CF7.PDB"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: four sequences are added to the alignment.
    const int count = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(22 == count, QString("Unexpected sequences count: expect %1, got %2").arg(22).arg(count));

    //    3. Open "Pairwise alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);

    //    Expected state: there is a message that the msa alphabet is no applicable.
    auto errorLabel = GTWidget::findLabel("lblMessage");
    CHECK_SET_ERR(errorLabel->isVisible(), "Error label is invisible");
    CHECK_SET_ERR(errorLabel->text().contains("Pairwise alignment is not available for alignments with \"Raw\" alphabet."),
                  QString("An unexpected error message: '%1'").arg(errorLabel->text()));
}

GUI_TEST_CLASS_DEFINITION(test_4323_3) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "align_new_sequences_to_alignment_action" button on the toolbar, select "samples/PDB/1CF7.pdb".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/PDB/1CF7.PDB"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: four sequences are added to the alignment.
    const int count = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(22 == count, QString("Unexpected sequences count: expect %1, got %2").arg(22).arg(count));
}

GUI_TEST_CLASS_DEFINITION(test_4323_4) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Rename the first two sequences to "1".
    GTUtilsMSAEditorSequenceArea::renameSequence("Phaneroptera_falcata", "1");
    GTUtilsMSAEditorSequenceArea::renameSequence("Isophya_altaica_EF540820", "1");

    //    3. Click "align_new_sequences_to_alignment_action" button on the toolbar, select "samples/FASTQ/eas.fastq".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTQ/eas.fastq"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Do it again.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTQ/eas.fastq"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: there are 24 sequences in the alignment; two of them are named "1", two - "EAS54_6_R1_2_1_413_324", two - "EAS54_6_R1_2_1_540_792", two - "EAS54_6_R1_2_1_443_348".
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(24 == names.count(), QString("Unexpected sequences count: expect %1, got %2").arg(24).arg(names.count()));
    CHECK_SET_ERR(2 == names.count("1"), QString("Unexpected sequences with name '1' count: expect %1, got %2").arg(2).arg(names.count("1")));
    CHECK_SET_ERR(2 == names.count("EAS54_6_R1_2_1_413_324"), QString("Unexpected sequences with name 'EAS54_6_R1_2_1_413_324' count: expect %1, got %2").arg(2).arg(names.count("EAS54_6_R1_2_1_413_324")));
    CHECK_SET_ERR(2 == names.count("EAS54_6_R1_2_1_540_792"), QString("Unexpected sequences with name 'EAS54_6_R1_2_1_540_792' count: expect %1, got %2").arg(2).arg(names.count("EAS54_6_R1_2_1_540_792")));
    CHECK_SET_ERR(2 == names.count("EAS54_6_R1_2_1_443_348"), QString("Unexpected sequences with name 'EAS54_6_R1_2_1_443_348' count: expect %1, got %2").arg(2).arg(names.count("EAS54_6_R1_2_1_443_348")));
}

GUI_TEST_CLASS_DEFINITION(test_4325) {
    // 1. Open In Silico PCR sample
    // 2. Seqeunce: "data/samples/Genbank/CVU55762.gb"
    //    Primers: "test/_common_data/cmdline/pcr/primers_CVU55762.fa"
    // 3. Run workflow
    // Expected state: the size of the product is 150, and the primers are annotated on both sides

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("In Silico PCR");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "/samples/Genbank/CVU55762.gb");

    GTUtilsWorkflowDesigner::click("In Silico PCR");
    GTUtilsWorkflowDesigner::setParameter("Primers URL", QDir(testDir).absolutePath() + "/_common_data/cmdline/pcr/primers_CVU55762.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_4325.gb", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir, "test_4325.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsSequenceView::getLengthOfSequence() == 150, "Product size is incorrect");
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    CHECK_SET_ERR(regions.size() == 2, "Incorrect primers number");
    CHECK_SET_ERR(regions.contains(U2Region(0, 40)), "There is no (1, 40) annotated primer region");
    CHECK_SET_ERR(regions.contains(U2Region(110, 40)), "There is no (111, 150) annotated primer region");
}

GUI_TEST_CLASS_DEFINITION(test_4334) {
    // Open "_common_data/fasta/empty.fa" as msa.
    GTFileDialog::openFile(testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTLogTracer lt;
    // Add human_t1.fa sequence through the context menu {Add->Sequence from file}
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTA", "human_T1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4345) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTMenu::clickMainMenuItem({"Actions", "Close active view"}, GTGlobals::UseKey);
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4352) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Toggle the Circular View for the sequence.
    GTUtilsCv::commonCvBtn::click();

    // 3. Find some restriction sites.
    FindEnzymesDialogFillerSettings settings;
    settings.enzymes = QStringList{ "AaaI" };
    settings.clickSelectAllSuppliers = true;
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTWidget::click(GTWidget::findWidget("Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Select any restriction site in the "Restriction Sites Map" widget.
    auto tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget("restrictionMapTreeWidget"));
    QTreeWidgetItem* item = GTTreeWidget::findItem(tree, "89345..89350");
    GTTreeWidget::click(item);

    // 5. Remove a part of the sequence that contains the selected site.
    GTUtilsDialog::add(new PopupChooserByText({"Edit", "Remove subsequence..."}));
    GTUtilsDialog::add(new RemovePartFromSequenceDialogFiller("89300..89400"));
    GTMenu::showContextMenu(GTUtilsSequenceView::getDetViewByNumber());

    // 6. Wait while restriction sites recalculates.
    GTUtilsTaskTreeView::waitTaskFinished();

    // 7. Navigate to any restriction site in the restriction site map.
    item = GTTreeWidget::findItem(tree, "89231..89236");
    GTTreeWidget::click(item);
    // Expected state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_4356) {
    class Test_4356 : public Filler {
    public:
        Test_4356()
            : Filler("DotPlotDialog") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/murine.gb"));
            GTWidget::click(GTWidget::findPushButton("loadSequenceButton", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::unloadDocument("murine.gb", true);

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new Test_4356());
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));
    GTUtilsTaskTreeView::waitTaskFinished();

    Document* doc = GTUtilsDocument::getDocument("murine.gb");
    CHECK_SET_ERR(doc->isLoaded(), "Document is unexpectedly unloaded");
}

GUI_TEST_CLASS_DEFINITION(test_4359) {
    /* 1. Open human_T1
     * 2. Open Primer3 dialog
     *   Expected state: the "Pick primers" button should be in focus
     */
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class EscClicker : public Filler {
    public:
        EscClicker()
            : Filler("Primer3Dialog") {
        }
        virtual void run() {
            QWidget* w = GTWidget::getActiveModalWidget();
            auto button = GTWidget::findPushButton("pickPrimersButton", w);
            CHECK_SET_ERR(button->isDefault(), "Pick primers button doesn't default");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(new EscClicker());
    GTWidget::click(GTWidget::findWidget("primer3_action_widget"));
}
GUI_TEST_CLASS_DEFINITION(test_4368) {
    /* 1. Open "data/samples/CLUSTALW/COI.aln".
     * 2. Open "Statistics" options panel tab.
     * 3. Set any reference sequence.
     * 4. Set "Show distances column" option.
     */
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("OP_SEQ_STATISTICS_WIDGET"));

    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, 5));
    GTWidget::click(GTWidget::findWidget("addSeq"));

    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);

    auto groupBox = GTWidget::findGroupBox("profileGroup");

    auto radio = GTWidget::findRadioButton("percentsButton", groupBox);
    GTRadioButton::click(radio);
    auto nameLabel = GTWidget::findLabel("Distance column name");
    CHECK_SET_ERR(nameLabel->text() == "%", "percentsButton not found!");

    auto radio2 = GTWidget::findRadioButton("countsButton", groupBox);
    GTRadioButton::click(radio2);
    CHECK_SET_ERR(nameLabel->text() == "score", "percentsButton not found!");
}

GUI_TEST_CLASS_DEFINITION(test_4373) {
    // 1. Open any sequence
    // 2. Open CV
    // 3. Resize CV to make some buttons hidden
    // Expected state: there is a button with an arrows at the bottom of toolbar and it shows the hidden action

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsCv::commonCvBtn::click();

    auto splitterHandler = GTWidget::findWidget("qt_splithandle_annotated_DNA_scrollarea");
    GTWidget::click(splitterHandler);

    auto mainToolBar = GTWidget::findWidget("mwtoolbar_activemdi");

    QPoint point = mainToolBar->geometry().bottomLeft();
    point = mainToolBar->mapToGlobal(point);
    auto cv = GTWidget::findWidget("CV_ADV_single_sequence_widget_0");
    point.setY(cv->mapToGlobal(cv->geometry().topLeft()).y() + 100);

    GTMouseDriver::press();
    GTMouseDriver::moveTo(point);
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    auto toolBar = GTWidget::findWidget("circular_view_local_toolbar");

    auto extButton = GTWidget::findWidget("qt_toolbar_ext_button", toolBar);
    CHECK_SET_ERR(extButton->isVisible() && extButton->isEnabled(), "qt_toolbar_ext_button is not visible and disabled");

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Show/hide restriction sites map"}));
    GTWidget::click(extButton);
}

GUI_TEST_CLASS_DEFINITION(test_4377) {
    // 1. Open "_common_data/fasta/Gene.fa".
    // 2. Choose the separate reading mode.
    // Expected: the warning notification is shown.
    // 3. Click the notification.
    // Expected: the report window is opened. There is the list of sequences with empty names.

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/Gene.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsNotifications::checkNotificationReportText("The following sequences are empty:");
}

GUI_TEST_CLASS_DEFINITION(test_4383) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::scrollToPosition(QPoint(603, 1));

    QWidget* activeWindow = GTUtilsMdi::activeWindow();
    GTWidget::findExactWidget<MSAEditorSequenceArea*>("msa_editor_sequence_area", activeWindow);
    auto msaOffsetRight = GTWidget::findWidget("msa_editor_offsets_view_widget_right", activeWindow);

    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 2, 7)));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 52, 50)));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTThread::waitForMainThread();

    activeWindow = GTUtilsMdi::activeWindow();
    msaOffsetRight = GTWidget::findWidget("msa_editor_offsets_view_widget_right", activeWindow);
    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 2, 77)));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 52, 120)));
    GTMouseDriver::release();
    GTKeyboardDriver::keyClick(Qt::Key_Space);
}

GUI_TEST_CLASS_DEFINITION(test_4386_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Change this alignment by adding random gaps.
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(5, 5));
    GTKeyboardDriver::keyClick(' ');

    //    3. Align with Muscle (or other algorithm).
    GTUtilsDialog::add(new PopupChooserByText({"Align", "Align with MUSCLE…"}));
    GTUtilsDialog::add(new MuscleDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Select some sequences in project view and click "align_new_sequences_to_alignment_action".
    GTUtilsProject::openMultiSequenceFileAsSequences(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsMdi::activateWindow("COI [COI.aln]");

    GTUtilsProjectTreeView::click("EAS54_6_R1_2_1_413_324");
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: sequences are aligned to alignment.
    int rowsCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(rowsCount == 19, QString("Unexpected rows count: expect %1, got %2").arg(19).arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4386_2) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Rename the alignment, a new name should contain spaces.
    GTUtilsProjectTreeView::rename("COI", "C O I");

    //    3. Click "align_new_sequences_to_alignment_action" and select any file with sequence.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTQ/eas.fastq"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: sequence is aligned to alignment.
    int rowsCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(rowsCount == 21, QString("Unexpected rows count: expect %1, got %2").arg(19).arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4391) {
    GTLogTracer lt;

    //    1. Create workflow { Read File URL(s) -> Cut Adapter }.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::addElement("Read File URL(s)");
    WorkflowProcessItem* cutAdapter = GTUtilsWorkflowDesigner::addElement("Cut Adapter");
    GTUtilsWorkflowDesigner::connect(fileList, cutAdapter);

    //    2. Set "_common_data/fastq/illumina.fastq" as input.
    GTUtilsWorkflowDesigner::addInputFile("Read File URL(s)", testDir + "_common_data/fastq/illumina.fastq");

    //    3. Run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: there are no errors neither in log nor in dashboard.
    bool hasErrorNotifications = GTUtilsDashboard::hasNotifications();
    CHECK_SET_ERR(!hasErrorNotifications, "There are error notifications on the dashboard");
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4400) {
    GTFileDialog::openFile(testDir + "_common_data/genbank/VectorNTI_CAN_READ.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* commentItem = GTUtilsAnnotationsTreeView::findItem("comment");
    GTUtilsAnnotationsTreeView::selectItemsByName({"comment"});
    QString qualValue = GTUtilsAnnotationsTreeView::getQualifierValue("Original database", commentItem);
    CHECK_SET_ERR(qualValue == "GenBank", "ORIGDB comment was parced incorreclty");
}

GUI_TEST_CLASS_DEFINITION(test_4434) {
    GTLogTracer lt;
    // 1. Open murine.gb and human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Drag the annotations from murine to human_t1
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(GTUtilsProjectTreeView::findIndex("NC_001363 features"), GTUtilsAnnotationsTreeView::getTreeWidget());
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findFirstAnnotation() != nullptr, "Annotations are connected to human_T1.fa");

    // 3. Save the project
    GTUtilsProject::saveProjectAs(sandBoxDir + "test_4434.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Close the project.
    GTUtilsProject::closeProject(false);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Open the saved project.
    GTFileDialog::openFile(sandBoxDir + "test_4434.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 6. Current state : human_t1 is loaded with annotations
    GTUtilsProjectTreeView::doubleClickItem("human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findFirstAnnotation() != nullptr, "Annotations are connected to human_T1.fa");
}

GUI_TEST_CLASS_DEFINITION(test_4439) {
    //    1. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on "NC_004718 features [sars.gb]" item in the Annotations tree view, select "Find qualifier..." menu item.

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state: a "Find Qualifier" dialog appears, search buttons are disabled.
            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* nextButton = buttonBox->button(QDialogButtonBox::Ok);
            QPushButton* allButton = buttonBox->button(QDialogButtonBox::Yes);
            CHECK_SET_ERR(!nextButton->isEnabled(), "'Next' button is enabled");
            CHECK_SET_ERR(!allButton->isEnabled(), "'Select all' button is enabled");

            //    3. Enter "1" as qualifier name. Click "Select all" button. Close the dialog.
            //    Expected state: search buttons are enabled, a comment annotation with its qualifier are selected.
            GTLineEdit::setText(GTWidget::findLineEdit("nameEdit", dialog), "1");

            CHECK_SET_ERR(nextButton->isEnabled(), "'Next' button is disabled");
            CHECK_SET_ERR(allButton->isEnabled(), "'Select all' button is disabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Yes);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Find qualifier..."}));
    GTUtilsDialog::add(new FindQualifierFiller(new Scenario));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem("NC_004718 features [sars.gb]");

    QList<QTreeWidgetItem*> selectedItems = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    CHECK_SET_ERR(selectedItems.size() == 2, QString("Unexpected count of selected items: expect 2, got %1").arg(selectedItems.size()));
    CHECK_SET_ERR(selectedItems.first()->text(0) == "comment", QString("Unexpected annotation name: expect '%1', got '%2'").arg("comment").arg(selectedItems.first()->text(0)));
    CHECK_SET_ERR(selectedItems.last()->text(0) == "1", QString("Unexpected qualifier name: expect '%1', got '%2'").arg("1").arg(selectedItems.first()->text(0)));
}

GUI_TEST_CLASS_DEFINITION(test_4440) {
    //    1. Open COI.aln
    //    2. Align with MUSCLE
    //    Expected state: object should be named "COI".

    GTFileDialog::openFile(dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QModelIndex idx = GTUtilsProjectTreeView::findIndex("COI.aln");
    GTUtilsProjectTreeView::checkItem(GTUtilsProjectTreeView::getTreeView(), "COI", idx);

    GTUtilsDialog::add(new PopupChooserByText({"Align", "Align with MUSCLE…"}));
    GTUtilsDialog::add(new MuscleDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkItem(GTUtilsProjectTreeView::getTreeView(), "COI", idx);
}

GUI_TEST_CLASS_DEFINITION(test_4463) {
    //    1. Open attached file in "GenBank" format
    //    2. Modify sequence
    //    3. Unload document
    //    Expected state: "Save document" dialog appeared
    //    4. Press "Yes"
    //    Expected state: UGENE does not crash
    //    5. Load the document again
    //    Expected state: the document is successfully loaded

    GTFile::copy(testDir + "_common_data/genbank/gbbct131.gb.gz", sandBoxDir + "/test_4463.gb.gz");

    GTFileDialog::openFile(sandBoxDir, "test_4463.gb.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new RemovePartFromSequenceDialogFiller("10..20"));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Remove subsequence..."}));
    GTWidget::click(GTUtilsAnnotationsTreeView::getTreeWidget());
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsMdi::closeWindow("test_4463.gb.gz");
    GTUtilsDocument::unloadDocument("test_4463.gb.gz", true);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::loadDocument("test_4463.gb.gz");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(nullptr != GTUtilsSequenceView::getSeqWidgetByNumber(), "Can't find sequence view widget");
}

GUI_TEST_CLASS_DEFINITION(test_4483) {
    // Open "samples/CLUSTALW/ty3.aln.gz".
    // Click "Export as image".
    // Choose SVG.
    // Export.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "Zoom Out"));
    }
    GTUtilsDialog::waitForDialog(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test.svg", QString("SVG")));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    qint64 fileSize = GTFile::getSize(testDir + "_common_data/scenarios/sandbox/test.svg");
    CHECK_SET_ERR(fileSize > 7000000 && fileSize < 80000000, "Current size: " + QString().setNum(fileSize));
}

GUI_TEST_CLASS_DEFINITION(test_4486) {
    //    1. Open "data/samples/Assembly/chrM.sorted.bam".
    //    2. Import with default settings.
    QDir().mkpath(sandBoxDir + "test_4486");
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_4486/test_4486.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Zoom in assembly view while reads are not visible.
    bool readsAreVisible = !GTUtilsAssemblyBrowser::isWelcomeScreenVisible();
    for (int i = 0; i < 100 && !readsAreVisible; i++) {
        GTUtilsAssemblyBrowser::zoomIn();
        readsAreVisible = !GTUtilsAssemblyBrowser::isWelcomeScreenVisible();
    }
    CHECK_SET_ERR(readsAreVisible, "Can't zoom to reads");

    //    4. Use context menu on reads area:
    //    {Export->visible reads}
    //    5. Export dialog appeared. Press "Export"
    //    Expected state: UGENE doesn't crash.
    GTUtilsDialog::add(new PopupChooserByText({"Export", "Visible reads as sequences"}));
    GTUtilsDialog::add(new ExportReadsDialogFiller(sandBoxDir + "test_4486/reads.fa"));
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Reads);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4488) {
    // 1. Open COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set a sequence as reference.
    GTUtilsMsaEditor::setReference("Phaneroptera_falcata");

    // 3. Open the "Statistics" OP tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);

    // 4. Check "Show distances column".
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);

    // 5. Switch off auto updating.
    auto autoUpdateCheck = GTWidget::findCheckBox("autoUpdateCheck");
    GTCheckBox::setChecked(autoUpdateCheck, false);

    // 6. Alignment context menu -> Add -> Sequence from file.
    // 7. Select "data/samples/Assembly/chrM.fa".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Assembly/chrM.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // UGENE crashes
}

GUI_TEST_CLASS_DEFINITION(test_4489) {
    // 1. Open COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select some region
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 5), QPoint(10, 10));
    // 3. Move it to the right with a mouse.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(7, 7));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(12, 7));
    GTMouseDriver::release();

    // Bug state : Overview is not recalculated.There is the "Waiting..." state.
    const QColor currentColor = GTUtilsMsaEditor::getGraphOverviewPixelColor(QPoint(GTUtilsMsaEditor::getGraphOverview()->width() - 5, 5));
    const QColor expectedColor = QColor("white");
    const QString currentColorString = QString("(%1, %2, %3)").arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());
    const QString expectedColorString = QString("(%1, %2, %3)").arg(expectedColor.red()).arg(expectedColor.green()).arg(expectedColor.blue());
    CHECK_SET_ERR(expectedColor == currentColor, QString("An unexpected color, maybe overview was not rendered: expected %1, got %2").arg(expectedColorString).arg(currentColorString));
}

GUI_TEST_CLASS_DEFINITION(test_4500) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open Remove Subsequence dialog
    //  - Fill the dialog : region to remove – 1..1000
    //  - check Save to new file
    //  - check Merge annotations to this file

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 1000));
    GTUtilsSequenceView::clickMouseOnTheSafeSequenceViewArea();
    GTKeyboardUtils::selectAll();

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REMOVE_SUBSEQUENCE}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new RemovePartFromSequenceDialogFiller(RemovePartFromSequenceDialogFiller::Remove, true, sandBoxDir + "4500_result.gb", RemovePartFromSequenceDialogFiller::Genbank));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();
    // 3. Open result file
    // Expected state annotation locations changed accordingly
    GTUtilsSequenceView::openSequenceView("4500_result.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList newRegions({"42..1658", "join(1970..2413,2412..2873)", "2875..3999", "4048..4203"});
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems("CDS");
    for (const QTreeWidgetItem* item : qAsConst(items)) {
        if (!newRegions.contains(item->text(2))) {
            CHECK_SET_ERR(false, "Unexpected CDS location " + item->text(2));
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_4505) {
    //    1. Open "test/_common_data/scenarios/msa/Chikungunya_E1.fasta".
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Delete any column
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(1);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished();
    // GTUtilsTaskTreeView::waitTaskFinished();
    //     3. Press "Undo"
    GTUtilsMsaEditor::undo();
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Bug state: Error appeared in log: "[ERROR][19:02] Failed to create a multiple alignment row!"
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    //    4. Click right button on MSA
    GTUtilsDialog::waitForDialog(new PopupChecker({"Consensus mode"}));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);
    //    Bug state: UGENE crashes
}

GUI_TEST_CLASS_DEFINITION(test_4508) {
    GTLogTracer lt;

    //    1. Open "_common_data/fasta/400000_symbols_msa.fasta".
    GTFileDialog::openFile(testDir + "_common_data/fasta/400000_symbols_msa.fasta");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2.  Click "Export as image" button on the toolbar.
    //    Expected state: an "Export Image" dialog appears.
    //    3. Set SVG format.
    //    Expected state: a warning appears, the dialog can't be accepted.

    class Scenario1 : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText(GTWidget::findComboBox("formatsBox", dialog), "SVG", GTGlobals::UseMouse);
            auto hintLabel = GTWidget::findLabel("hintLabel", dialog);
            CHECK_SET_ERR(hintLabel->isVisible(), "hintLabel is invisible");
            QString expectedSubstring = "selected region is too big";
            CHECK_SET_ERR(hintLabel->text().contains(expectedSubstring), QString("An expected substring not found: substring - '%1', text - '%2'").arg(expectedSubstring).arg(hintLabel->text()));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    for (int i = 0; i < 6; i++) {
        QWidget* zoomOutAction = GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Zoom Out");
        CHECK_BREAK(zoomOutAction->isEnabled());
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Zoom Out"));
    }

    GTUtilsDialog::waitForDialog(new ExportImage(new Scenario1));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Export as image");

    //    4. Cancel the dialog. Remove the first column. Call the dialog again and set SVG format.
    //    Expected state: there are no warnings, dialog can be accepted.
    //    5. Accept the dialog.

    GTUtilsMsaEditor::removeColumn(1);
    GTThread::waitForMainThread();
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Zoom Out"));

    class Scenario2 : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText(GTWidget::findComboBox("formatsBox", dialog), "SVG", GTGlobals::UseMouse);
            auto hintLabel = GTWidget::findLabel("hintLabel", dialog);
            CHECK_SET_ERR(!hintLabel->isVisible(), "hintLabel is visible");

            QDir().mkpath(sandBoxDir + "test_4508");
            GTLineEdit::setText(GTWidget::findLineEdit("fileNameEdit", dialog), sandBoxDir + "test_4508/test_4508.svg");
            GTCheckBox::setChecked(GTWidget::findCheckBox("exportSeqNames", dialog));
            GTCheckBox::setChecked(GTWidget::findCheckBox("exportConsensus", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ExportImage(new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Export as image");

    //    Expected state: the msa is successfully exported, there are no errors in the log.
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTFile::check(sandBoxDir + "test_4508/test_4508.svg"), QString("File '%1' doesn't exist").arg(sandBoxDir + "test_4508/test_4508.svg"));
    CHECK_SET_ERR(GTFile::getSize(sandBoxDir + "test_4508/test_4508.svg") > 0, QString("File '%1' has zero size").arg(sandBoxDir + "test_4508/test_4508.svg"));
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4515) {
    GTFileDialog::openFile(dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("K");

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: -/0"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_4522) {
    // Check that circular & unrooted tree layout image changes when switching from Default to non Default (Cladogram) image.
    GTFileDialog::openFile(dataDir + "/samples/Newick/COI.nwk");
    GTUtilsPhyTree::checkTreeViewerWindowIsActive();
    GTUtilsOptionPanelPhyTree::openTab();

    QImage rectDefaultImage = GTUtilsPhyTree::captureTreeImage();

    GTUtilsOptionPanelPhyTree::changeTreeLayout("Circular");
    GTUtilsOptionPanelPhyTree::checkBranchDepthScaleMode("Default");
    QImage circularDefaultImage = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(circularDefaultImage != rectDefaultImage, "circularDefaultImage != rectDefaultImage check failed");

    GTUtilsOptionPanelPhyTree::changeBranchDepthScaleMode("Cladogram");
    QImage circularCladogramImage = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(circularCladogramImage != circularDefaultImage, "circularCladogramImage != circularDefaultImage check failed");

    GTUtilsOptionPanelPhyTree::changeTreeLayout("Unrooted");
    GTUtilsOptionPanelPhyTree::checkBranchDepthScaleMode("Cladogram");
    QImage unrootedCladogramImage = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(unrootedCladogramImage != circularCladogramImage, "unrootedCladogramImage != circularCladogramImage check failed");

    GTUtilsOptionPanelPhyTree::changeBranchDepthScaleMode("Default");
    QImage unrootedDefaultImage = GTUtilsPhyTree::captureTreeImage();
    CHECK_SET_ERR(unrootedDefaultImage != unrootedCladogramImage, "unrootedDefaultImage != unrootedCladogramImage check failed");
}

GUI_TEST_CLASS_DEFINITION(test_4523) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "data/samples/FASTQ/eas.fastq".
    GTUtilsProject::openMultiSequenceFileAsSequences(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Call context menu on the "human_T1.fa" document, select {Add -> Add object to document...} menu item. Add any sequence object.
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("eas.fastq", "EAS54_6_R1_2_1_413_324"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__ADD_MENU, ACTION_PROJECT__ADD_OBJECT}));
    GTUtilsProjectTreeView::callContextMenu("human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: "human_T1.fa" document contains two sequence objects.
    QModelIndex oldIndex = GTUtilsProjectTreeView::findIndex({"human_T1.fa", "human_T1 (UCSC April 2002 chr7:115977709-117855134)"});
    QModelIndex addedIndex = GTUtilsProjectTreeView::findIndex({"human_T1.fa", "EAS54_6_R1_2_1_413_324"});

    //    4. Rename both objects in "human_T1.fa" document to "123".
    GTUtilsProjectTreeView::rename(addedIndex, "123");

    GTUtilsProjectTreeView::rename(oldIndex, "123");

    //    Expected state: "human_T1.fa" document contains two sequence objects with the same names.
    // Ok, I'm sure in the renaming method, skip this check

    //    5. Open the second object.
    GTUtilsProjectTreeView::doubleClickItem(addedIndex);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the second object is opened. The second object is highlighted in the Project view.
    GTUtilsSequenceView::checkSequence("CCCTTCTTGTCTTCAGCGTTTCTCC");

    //    6. Open the first object.
    GTUtilsProjectTreeView::doubleClickItem(oldIndex);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the first object is opened. The first object is highlighted in the Project view.
    const QString sequenceBeginning = GTUtilsSequenceView::getBeginOfSequenceAsString(25);
    const QString expectedSequenceBeginning = "TTGTCAGATTCACCAAAGTTGAAAT";
    CHECK_SET_ERR(expectedSequenceBeginning == sequenceBeginning,
                  QString("The sequence first 25 bases are incorrect: expected '%1', got '%2'")
                      .arg(expectedSequenceBeginning)
                      .arg(sequenceBeginning));
}

GUI_TEST_CLASS_DEFINITION(test_4524) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Remove the first sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Export the msa to SVG.
    GTUtilsDialog::waitForDialog(new ExportMsaImage(sandBoxDir + "test_4524.svg", "SVG", 0));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(10, 10));
    GTMouseDriver::click(Qt::RightButton);

    qint64 imageFileSize = GTFile::getSize(sandBoxDir + "test_4524.svg");
    CHECK_SET_ERR(imageFileSize > 0, "Export MSA to image failed. Unexpected image file size");

    // Current state : "undo" action becomes disabled.
    QWidget* undoButton = GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_undo");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is disabled unexpectedly");
}

GUI_TEST_CLASS_DEFINITION(test_4536) {
    GTFileDialog::openFile(testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there are the following values in the status-bar: "Ln - / 2  Col - / 4  Pos - / -".
    QString rowNumberString = GTMSAEditorStatusWidget::getRowNumberString();
    QString rowsCountString = GTMSAEditorStatusWidget::getRowsCountString();
    QString columnNumberString = GTMSAEditorStatusWidget::getColumnNumberString();
    QString columnsCountString = GTMSAEditorStatusWidget::getColumnsCountString();
    QString sequenceUngappedPositionString = GTMSAEditorStatusWidget::getSequenceUngappedPositionString();
    QString sequenceUngappedLengthString = GTMSAEditorStatusWidget::getSequenceUngappedLengthString();

    CHECK_SET_ERR(rowNumberString == "-", QString("Incorrect row number label: expected '-', got '%1'").arg(rowNumberString));
    CHECK_SET_ERR(rowsCountString == "2", QString("Incorrect rows count label: expected '2', got '%1'").arg(rowsCountString));
    CHECK_SET_ERR(columnNumberString == "-", QString("Incorrect column number label: expected '-', got '%1'").arg(columnNumberString));
    CHECK_SET_ERR(columnsCountString == "4", QString("Incorrect columns count label: expected '4', got '%1'").arg(columnsCountString));
    CHECK_SET_ERR(sequenceUngappedPositionString == "-", QString("Incorrect sequence ungapped position label: expected '-', got '%1'").arg(sequenceUngappedPositionString));
    CHECK_SET_ERR(sequenceUngappedLengthString == "-", QString("Incorrect sequence ungapped length label: expected '-', got '%1'").arg(sequenceUngappedLengthString));

    // Open "general" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    // Expected state: there are next values: Length: 4, Sequences: 2.
    int length = GTUtilsOptionPanelMsa::getLength();
    int height = GTUtilsOptionPanelMsa::getHeight();
    CHECK_SET_ERR(length == 4, QString("Incorrect alignment length is on the options panel: expected 4, got %1").arg(length));
    CHECK_SET_ERR(height == 2, QString("Incorrect alignment height is on the options panel: expected 2, got %1").arg(height));
}

GUI_TEST_CLASS_DEFINITION(test_4537) {
    // 1. Open it in UGENE with Ctrl + Shift + O.
    class Scenario : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto userSelectedFormat = GTWidget::findComboBox("userSelectedFormat", dialog);
            QStringList values = GTComboBox::getValues(userSelectedFormat);

            // Expected: there is no format SAM in the formats combobox.
            CHECK_SET_ERR(!values.contains("SAM"), "SAM format is in the combo box");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    // Expected: format selector dialog appears.
    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller(new Scenario()));
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Assembly/chrM.sam"));
    GTMenu::clickMainMenuItem({"File", "Open as..."});
}

GUI_TEST_CLASS_DEFINITION(test_4557) {
    //    1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open the PCR OP tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    //    3. Enter the forward primer: TTGTCAGATTCACCAAAGTT.
    GTUtilsOptionPanelSequenceView::setForwardPrimer("AAATCAGATTCACCAAAGTT");
    GTUtilsPcr::setMismatches(U2Strand::Direct, 3);

    //    4. Enter the reverse primer: ACCTCTCTCTGGCTGCCCTT.
    GTUtilsOptionPanelSequenceView::setReversePrimer("GGGTCTCTCTGGCTGCCCTT");
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 3);

    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));

    GTWidget::click(GTWidget::findWidget("extractProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QString product = GTUtilsSequenceView::getSequenceAsString();
    QString expected = "AAATCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGACCC";

    CHECK_SET_ERR(product == expected, "Unexpected product: " + product)
}

GUI_TEST_CLASS_DEFINITION(test_4587) {
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_4587"));
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/4587/extended_dna.ace");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkObjectTypes(
        QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT,
        GTUtilsProjectTreeView::findIndex("Contig1"));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4588) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4588/4588.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> blastResultItems = GTUtilsAnnotationsTreeView::findItems("blast result");
    GTUtilsAnnotationsTreeView::selectItems(blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(const QString& _dbPath, const QString& _outputPath)
            : Filler("BlastDBCmdDialog"), dbPath(_dbPath), outputPath(_outputPath) {};

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dbPath));
            GTWidget::click(GTWidget::findWidget("selectDatabasePushButton", dialog));

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("browseOutputButton", dialog));

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
            GTWidget::click(buttonBox->button(QDialogButtonBox::Ok));
        };

    private:
        QString dbPath;
        QString outputPath;
    };

    GTUtilsDialog::waitForDialog(
        new OkClicker(
            testDir + "_common_data/scenarios/_regression/4588/BLAST/4588.00.nhr",
            testDir + "_common_data/scenarios/sandbox/4588_fetched.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"fetchMenu", "fetchSequenceById"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::getItemCenter("shortread24489");
}

GUI_TEST_CLASS_DEFINITION(test_4588_1) {
    GTUtilsExternalTools::removeTool("BlastAll");

    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4588/4588_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> blastResultItems = GTUtilsAnnotationsTreeView::findItems("blast result");
    GTUtilsAnnotationsTreeView::selectItems(blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(const QString& _dbPath, const QString& _outputPath)
            : Filler("BlastDBCmdDialog"), dbPath(_dbPath), outputPath(_outputPath) {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dbPath));
            GTWidget::click(GTWidget::findWidget("selectDatabasePushButton", dialog));

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("browseOutputButton", dialog));

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
            GTWidget::click(buttonBox->button(QDialogButtonBox::Ok));
        }

    private:
        QString dbPath;
        QString outputPath;
    };

    GTUtilsDialog::waitForDialog(
        new OkClicker(
            testDir + "_common_data/scenarios/_regression/4588/BLAST_plus/4588.00.nhr",
            testDir + "_common_data/scenarios/sandbox/4588_1_fetched.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"fetchMenu", "fetchSequenceById"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::getItemCenter("shortread24481");
}

GUI_TEST_CLASS_DEFINITION(test_4588_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4588", "4588_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    QList<QTreeWidgetItem*> blastResultItems = GTUtilsAnnotationsTreeView::findItems("blast result");
    GTUtilsAnnotationsTreeView::selectItems(blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(const QString& dbPath, const QString& outputPath)
            : Filler("BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dbPath));
            GTWidget::click(GTWidget::findWidget("selectDatabasePushButton", w));
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(GTWidget::findWidget("browseOutputButton", w));

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(w, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(w, QDialogButtonBox::Cancel);
        }

    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(new OkClicker(testDir + "_common_data/scenarios/_regression/4588/4588_1.gb", testDir + "_common_data/scenarios/sandbox/4588_1_fetched.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"fetchMenu", "fetchSequenceById"}));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4589) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Call context menu on the "sars.gb" document and select {Add -> Add object to document} menu item.
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("murine.gb", "NC_001363 features"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__ADD_MENU, ACTION_PROJECT__ADD_OBJECT}));
    GTUtilsProjectTreeView::callContextMenu("sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::findIndex({"sars.gb", "NC_001363 features"});
    GTUtilsProjectTreeView::findIndex({"sars.gb", "NC_004718 features"});
}

GUI_TEST_CLASS_DEFINITION(test_4591) {
    // 1. Open a circular sequence of length N.
    GTFileDialog::openFile(dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    SelectSequenceRegionDialogFiller* filler = new SelectSequenceRegionDialogFiller(140425, 2);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(filler);
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    // 2. Open "Region selection" dialog {Ctrl+a} fill it with next data:
    //         {Single range selection} checked
    //         {Region:} 140425..2

    // 3. Press 'Go' button
    // Expected state: this regions are selected on the view
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy sequence"}));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);
    QString text = GTClipboard::text();
    CHECK_SET_ERR(text == "ATTG", "unexpected selection: " + text);
}

GUI_TEST_CLASS_DEFINITION(test_4591_1) {
    // 1) Open samples/FASTA/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) Select 100..10 region of the sequence
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto startEdit = GTWidget::findLineEdit("startEdit", dialog);
            auto endEdit = GTWidget::findLineEdit("endEdit", dialog);

            GTLineEdit::setText(startEdit, QString::number(321));
            GTLineEdit::setText(endEdit, QString::number(123));

            auto box = GTWidget::findDialogButtonBox("buttonBox");
            QPushButton* goButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!goButton->isEnabled(), "Go button is enabled");

            GTLineEdit::setText(startEdit, QString::number(123));
            GTLineEdit::setText(endEdit, QString::number(321));
            CHECK_SET_ERR(goButton != nullptr, "Go button not found");
            CHECK_SET_ERR(goButton->isEnabled(), "Go button is notenabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(new Scenario));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));
}

GUI_TEST_CLASS_DEFINITION(test_4591_2) {
    // 1. Open a circular sequence of length N.
    GTFileDialog::openFile(dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    SelectSequenceRegionDialogFiller* filler = new SelectSequenceRegionDialogFiller(3, 3);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(filler);
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    // 2. Open "Region selection" dialog {Ctrl+a} fill it with next data:
    //         {Single range selection} checked
    //         {Region:} 140425..2

    // 3. Press 'Go' button
    // Expected state: this regions are selected on the view
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy sequence"}));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);
    QString text = GTClipboard::text();
    CHECK_SET_ERR(text == "G", "unexpected selection: " + text);
}

GUI_TEST_CLASS_DEFINITION(test_4606) {
    // 1. Create custom WD element
    // 2. Do not fill "Description" and "Parameters description" fields
    // 3. Create workflow with created element and tun it
    // Expected state: no safepoint triggered

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::removeCmdlineWorkerFromPalette("Element_4606");

    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "Element_4606";

    QList<CreateElementWithCommandLineToolFiller::InOutData> input;
    CreateElementWithCommandLineToolFiller::InOutDataType inOutDataType;
    inOutDataType.first = CreateElementWithCommandLineToolFiller::Sequence;
    inOutDataType.second = "FASTA";
    input << CreateElementWithCommandLineToolFiller::InOutData("in1",
                                                               inOutDataType);
    settings.input = input;
    settings.command = "echo";

    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    QAbstractButton* createElement = GTAction::button("createElementWithCommandLineTool");
    GTWidget::click(createElement);

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    CHECK_SET_ERR(read != nullptr, "Failed to add an element");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    WorkflowProcessItem* customWorker = GTUtilsWorkflowDesigner::getWorker("Element_4606");

    GTUtilsWorkflowDesigner::connect(read, customWorker);
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4620) {
    GTFileDialog::openFile(dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Call a context menu on the chromatogram, select {Edit new sequence} menu item.
    // Expected state: an "Add new document" dialog appears.
    class CheckFormatsScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // Check that there is no "Database connection" format. Ensure that there are no formats with "DocumentFormatFlag_Hidden" flag.
            QStringList formatNames = GTComboBox::getValues(GTWidget::findComboBox("documentTypeCombo", dialog));
            CHECK_SET_ERR(!formatNames.contains("Database connection"), "'Database connection' format is not available");

            QList<DocumentFormatId> registeredFormatsIds = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();
            QMap<QString, DocumentFormat*> formatByName;
            for (const DocumentFormatId& formatId : qAsConst(registeredFormatsIds)) {
                DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
                formatByName[format->getFormatName()] = format;
            }

            for (const QString& formatName : qAsConst(formatNames)) {
                DocumentFormat* format = formatByName.value(formatName, nullptr);
                CHECK_SET_ERR(!format->getFlags().testFlag(DocumentFormatFlag_Hidden), "A hidden format is offered to choose");
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Edit new sequence"}));
    GTUtilsDialog::add(new AddNewDocumentDialogFiller(new CheckFormatsScenario()));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4621) {
    qputenv("UGENE_DISABLE_ENZYMES_OVERFLOW_CHECK", "1");  // disable overflow to create a long running "Find Enzymes task".

    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Find some restriction sites.
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto enzymesSelectorWidget = GTWidget::findWidget("enzymesSelectorWidget");
            CHECK_SET_ERR(nullptr != enzymesSelectorWidget, "enzymesSelectorWidget is NULL");

            GTWidget::click(GTWidget::findWidget("selectAllButton", enzymesSelectorWidget));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList(), new Scenario()));
    GTWidget::click(GTWidget::findWidget("Find restriction sites_widget"));

    // 3. Delete sequence object
    GTUtilsProjectTreeView::click("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: UGENE does not crash.
}
GUI_TEST_CLASS_DEFINITION(test_4624) {
    // 1. Open assembly with DNA extended alphabet
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "extended_dna.ace.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<ExportCoverageDialogFiller::Action> actions;

    // 2. Export coverage with bases quantity info
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, QVariant("Per base"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SelectFile, sandBoxDir + "test_4624.txt");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetExportBasesQuantity, QVariant(true));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, QVariant());

    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();

    // 3. Check the coverage
    QString expectedFileContent = GTFile::readAll(testDir + "_common_data/scenarios/_regression/4624/4624.txt");
    QString resultFileContent = GTFile::readAll(sandBoxDir + "test_4624.txt");
    CHECK_SET_ERR(resultFileContent == expectedFileContent, "Incorrect coverage has been exported");
}

GUI_TEST_CLASS_DEFINITION(test_4628) {
    GTUtilsNotifications::waitForNotification(false);
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4628", "cow.chr13.repeats.shifted.bed");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* reportWindow = GTUtilsMdi::checkWindowIsActive("Report");
    auto textEdit = GTWidget::findTextEdit("reportTextEdit", reportWindow);
    CHECK_SET_ERR(textEdit->toPlainText().contains("incorrect strand value '+379aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa...' at line 5333"),
                  "Expected message is not found in the report text");
}

GUI_TEST_CLASS_DEFINITION(test_4674) {
    // 1. Open COI.aln
    // 2. Build the tree and synchronize it with the alignment
    // 3. Delete one sequence
    //    Expected state: tree is not in sync anymore and can't be synced.
    // 4. Undo.
    //    Expected state: tree can be synced again but is not in sync.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4674", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");
    int sequenceCount1 = GTUtilsMsaEditor::getSequencesCount();

    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    int sequenceCount2 = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCount2 == sequenceCount1 - 1, "Sequence was not deleted/1");
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/1");
    CHECK_SET_ERR(!syncModeButton->isEnabled(), "Sync mode button must not be enabled");

    GTUtilsMsaEditor::undo();
    GTThread::waitForMainThread();

    int sequenceCount3 = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCount3 == sequenceCount1, "Sequence was not restored");
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/2");
    CHECK_SET_ERR(syncModeButton->isEnabled(), "Sync mode button must be enabled");
}

GUI_TEST_CLASS_DEFINITION(test_4674_1) {
    // 1. Open COI.aln.
    // 2. Align the sequence to alignment.
    // 3. Build the tree and synchronize it with the alignment.
    // 4. Click Undo.
    //    Expected state: the sequence is removed, tree is not in sync anymore and sync action is disabled.
    // 5. Click Redo
    // 6. Sync action is enabled again.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/", "murine.gb"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4674_1", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON");
    int sequenceCount = GTUtilsMsaEditor::getSequencesCount();

    GTUtilsMsaEditor::undo();
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/1");
    CHECK_SET_ERR(!syncModeButton->isEnabled(), "Sync mode must be not available");
    CHECK_SET_ERR(sequenceCount - 1 == GTUtilsMsaEditor::getSequencesCount(), "Undo must remove 1 sequence from the MSA");

    GTUtilsMsaEditor::redo();
    CHECK_SET_ERR(sequenceCount == GTUtilsMsaEditor::getSequencesCount(), "Redo must return 1 sequence back to the MSA");
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/2");
    CHECK_SET_ERR(syncModeButton->isEnabled(), "Sync mode must be available again");
}

GUI_TEST_CLASS_DEFINITION(test_4674_2) {
    // Open COI.aln.
    // Build the tree and check that it is synchronized with MSA.
    // Insert a gap.
    //   Expected state: tree is still in sync.
    // Change sequences order (sort by length).
    //   Expected state: tree is not in sync anymore.
    // Sync tree.
    //  Expected state: tree is in sync again.
    // Delete some character
    //  Expected state: Tree is still in sync mode.

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_4674_2");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::TreeOptions);

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/1");

    // Add gap to the alignment. The tree must keep sync mode, because sync mode is by the name list order only and the name list is not changed.
    GTUtilsMSAEditorSequenceArea::click(QPoint(10, 10));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/2");

    // Change sequences order by re-sorting.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_length"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF");

    // Enable sync mode again.
    GTWidget::click(syncModeButton);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/3");

    // Delete gap from the alignment. The tree must keep sync mode, because sync mode is by the name list order only and the name list is not changed.
    GTUtilsMSAEditorSequenceArea::click(QPoint(10, 10));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/4");
}

GUI_TEST_CLASS_DEFINITION(test_4676_1) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Do any change in the msa.
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(5, 5));
    GTKeyboardDriver::keyClick(' ');

    // Expected state: undo action is enabled, redo action is disabled.
    QAbstractButton* undo1 = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undo1->isEnabled(), "Undo button should be enabled");

    QAbstractButton* redo1 = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(!redo1->isEnabled(), "Redo button should be disabled");

    // Close the view.
    GTUtilsMdi::click(GTGlobals::Close);

    // Open the view again.
    GTUtilsDialog::waitForDialog(new PopupChooser({"openInMenu", "action_open_view"}));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);

    // Expected state: undo action is enabled.
    QAbstractButton* undo2 = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undo2->isEnabled(), "Undo button should be enabled");
}

GUI_TEST_CLASS_DEFINITION(test_4676_2) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Do any change in the msa.
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(5, 5));
    GTKeyboardDriver::keyClick(' ');

    // Undo the modification.
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);

    // Expected state: undo action is disabled, redo action is enabled.
    QAbstractButton* undo1 = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undo1->isEnabled(), "Undo button should be disabled");

    QAbstractButton* redo1 = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(redo1->isEnabled(), "Redo button should be enabled");

    // Close the view.
    GTUtilsMdi::click(GTGlobals::Close);

    // Open the view again.
    GTUtilsDialog::waitForDialog(new PopupChooser({"openInMenu", "action_open_view"}));
    GTUtilsProjectTreeView::click("COI.aln", Qt::RightButton);

    // Expected state: redo action is enabled.
    QAbstractButton* redo2 = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(redo2->isEnabled(), "Redo button should be enabled");
}

GUI_TEST_CLASS_DEFINITION(test_4687) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open OP and select pairwice alignment tab, select sequences to align
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");

    // 3. Press "align_new_sequences_to_alignment_action" and add next sequence _common_data/fasta/amino_ext.fa
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/", "amino_ext.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check warning message visibility
    auto label = GTWidget::findWidget("lblMessage");
    CHECK_SET_ERR(label->isVisible(), "Label should be visible");

    // 4. Undo changes
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check warning message invisibility
    CHECK_SET_ERR(!label->isVisible(), "Label should be invisible");
}

GUI_TEST_CLASS_DEFINITION(test_4689_1) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    auto consensusType = GTWidget::findComboBox("consensusType");
    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");

    //    3. Set "Strict" consensus algorithm
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");
    GTComboBox::selectItemByText(consensusType, "Strict");
    GTSpinBox::setValue(thresholdSpinBox, 50, GTGlobals::UseKeyBoard);

    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    consensusType = GTWidget::findComboBox("consensusType");
    thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");

    CHECK_SET_ERR(sequenceLineEdit->text() == "Phaneroptera_falcata", QString("unexpected reference: %1").arg(sequenceLineEdit->text()));
    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    CHECK_SET_ERR(thresholdSpinBox->value() == 50, QString("unexpected threshold value: %1").arg(thresholdSpinBox->value()));

    //    4. Add amino extended sequence
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/amino_ext.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    //    5. Check that algorithm is "Strict", but set of algorithms correspont to raw alphabet
    consensusType = GTWidget::findComboBox("consensusType");

    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    CHECK_SET_ERR(consensusType->count() == 2, QString("Incorrect consensus algorithms count: %1").arg(consensusType->count()));
}

GUI_TEST_CLASS_DEFINITION(test_4689_2) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Add amino extended sequence
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/amino_ext.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    //    3. Press "Undo"
    GTUtilsMsaEditor::undo();

    //    4. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    //    5. Set consensus algorithm "Levitsky"
    auto consensusType = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusType, "Levitsky");
    //    6. Close the tab
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::General);

    //    7. Press "Redo"
    GTUtilsMsaEditor::redo();

    //    8. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    //    9. Check that algorithm is "ClustalW"
    consensusType = GTWidget::findComboBox("consensusType");
    CHECK_SET_ERR(consensusType->currentText() == "ClustalW", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    //    10. Change algorithm
    GTComboBox::selectItemByText(consensusType, "Strict");
    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    // Expected: UGENE does not crash
}

GUI_TEST_CLASS_DEFINITION(test_4694) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open OP and select pairwice alignment tab, select sequences to align, set "in new window" parameter to "false"
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");

    auto widget = GTWidget::findWidget("outputContainerWidget");
    if (widget->isHidden()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Output settings"));
    }
    auto inNewWindowCheckBox = GTWidget::findCheckBox("inNewWindowCheckBox");
    GTCheckBox::setChecked(inNewWindowCheckBox, false);
    GTWidget::click(GTWidget::findWidget("alignButton"));

    // 4. Undo changes
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state "Undo" button is disabled
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Button should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_4699) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    FindEnzymesDialogFillerSettings settings;
    settings.clickSelectAllSuppliers = true;
    settings.enzymes = QStringList{ "AaaI" };
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(settings));
    GTWidget::click(GTWidget::findWidget("Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget("restrictionMapTreeWidget"));
    QTreeWidgetItem* item = GTTreeWidget::findItem(tree, "76105..76110");
    GTTreeWidget::click(item);

    settings.enzymes = QStringList{ "AacLI" };
    settings.clickSelectAllSuppliers = true;
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(settings));
    GTWidget::click(GTWidget::findWidget("Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished();

    auto newtree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget("restrictionMapTreeWidget"));
    QTreeWidgetItem* newitem = GTTreeWidget::findItem(newtree, "10101..10106");
    GTTreeWidget::click(newitem);
}

GUI_TEST_CLASS_DEFINITION(test_4700) {
    // 1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4700/", "almost-empty.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTUtilsMdi::activeWindow());
    // 2. Find area without reads
    for (int i = 0; i < 24; i++) {
        GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);
    }

    GTKeyboardDriver::keyClick(Qt::Key_Home);
    // 3. Export visible reads
    GTUtilsDialog::add(new PopupChooserByText({"Export", "Visible reads as sequences"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Reads);
}
GUI_TEST_CLASS_DEFINITION(test_4701) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch on collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: the button is checked.
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "1 Mecopoda_elongata__Sumatra_ is not collapsed");

    // Press the Remove All Gaps button.
    auto seq = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTMenu::showContextMenu(seq);

    // Expected state: the button is checked.
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "1 Mecopoda_elongata__Sumatra_ is not collapsed");
}
GUI_TEST_CLASS_DEFINITION(test_4701_1) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch on collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: two sequences are collapsed.
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "1 Mecopoda_elongata__Sumatra_ is not collapsed");

    // Edit the alignment.
    GTUtilsMsaEditor::removeColumn(5);

    // Press Undo.
    GTUtilsMsaEditor::undo();

    // Expected state: the collapsing mode stays turned on.
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "1 Mecopoda_elongata__Sumatra_ is not collapsed");
}
GUI_TEST_CLASS_DEFINITION(test_4702) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Give the test more space (for debugging).
    GTUtilsCv::commonCvBtn::click();  // Close circular view.

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAllSelectedItems().isEmpty(), "No annotation must be selected by default");

    QTreeWidget* treeWidget = GTUtilsAnnotationsTreeView::getTreeWidget();
    int totalItemCount = GTTreeWidget::getItems(treeWidget->invisibleRootItem()).size() - 5;  // - 2: source & group, -2: comment & group, -1: empty auto annotations doc.

    // Check inversion of 1 annotation.
    GTUtilsAnnotationsTreeView::selectItemsByName({"ncRNA"});
    int selectedItemCount = GTUtilsAnnotationsTreeView::getAllSelectedItems().size();
    CHECK_SET_ERR(selectedItemCount == 1,
                  QString("1. Incorrect selected annotations count: expected - %1, got - %2 ").arg(1).arg(selectedItemCount));

    GTUtilsDialog::waitForDialog(new PopupChooser({"invert_selection_action"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem("ncRNA");
    int invertedSelectedItemCount = GTUtilsAnnotationsTreeView::getAllSelectedItems().size();
    CHECK_SET_ERR(invertedSelectedItemCount == totalItemCount - 8,  // -1 annotation, -1 group, -1 document, -5 for qualifiers.
                  QString("2. Incorrect selected annotations count: expected - %1, got - %2 ").arg(totalItemCount - 8).arg(invertedSelectedItemCount));

    // Check inversion of multiple annotations.
    GTUtilsAnnotationsTreeView::selectItemsByName({"repeat_region"});
    selectedItemCount = GTUtilsAnnotationsTreeView::getAllSelectedItems().size();
    CHECK_SET_ERR(selectedItemCount == 2,  // 2 annotations.
                  QString("3. Incorrect selected annotations count: expected - %1, got - %2 ").arg(2).arg(selectedItemCount));

    GTUtilsDialog::waitForDialog(new PopupChooser({"invert_selection_action"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem("repeat_region");
    invertedSelectedItemCount = GTUtilsAnnotationsTreeView::getAllSelectedItems().size();
    CHECK_SET_ERR(invertedSelectedItemCount == totalItemCount - 9,  // -2 annotations, -1 group, -1 document, -5 ncRNA qualifiers.
                  QString("4. Incorrect selected annotations count: expected - %1, got - %2 ").arg(totalItemCount - 9).arg(invertedSelectedItemCount));
}

GUI_TEST_CLASS_DEFINITION(test_4710) {
    //    1) Make sure to have several dashboards for different workflows runs.
    //    2) Run a workflow, e.g. I ran "Variation annotation with SnpEff" with the tutorial data.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("SnpEff");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTThread::waitForMainThread();
    GTUtilsWorkflowDesigner::click("Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click("Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(new SnpEffDatabaseDialogFiller("hg19"));
    GTUtilsWorkflowDesigner::setParameter("Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow();

    QTabWidget* tabs = GTUtilsDashboard::getTabWidget();
    // int initialIndex = tabs->count();
    QString initTabName = GTTabWidget::getTabName(tabs, tabs->currentIndex());
    //    3) During the workflow run open the "Dashboard Manager", select one of the several dashboard, and click "Remove selected."
    QWidget* dmButton = GTAction::button(GTAction::findAction("Dashboards manager"));

    class custom : public CustomScenario {
    public:
        void run() {
            //    4) Select some dashboards in the dialog
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto listWidget = GTWidget::findTreeWidget("listWidget", dialog);
            GTTreeWidget::click(listWidget->invisibleRootItem()->child(0));

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Confirm"));
            GTWidget::click(GTWidget::findWidget("removeButton", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new custom()));
    GTWidget::click(dmButton);
    //    Expected result: the selected dashboard was removed, the current workflow execution is proceeding and its dashboard is shown.
    //    Actual result: the workflow execution is proceeding, but the current dashboard is removed.

    // int finalIndex = tabs->count();
    QString finalTabName = GTTabWidget::getTabName(tabs, tabs->currentIndex());

    CHECK_SET_ERR(initTabName == finalTabName, "tab name changed. Initial: " + initTabName + ", actual: " + finalTabName);
    AppContext::getTaskScheduler()->cancelAllTasks();
    GTUtilsTaskTreeView::waitTaskFinished(60000);
}

GUI_TEST_CLASS_DEFINITION(test_4710_1) {
    //    1) Run "Align with MUSCLE" with "_common_data\fasta\PF07724_full_family.fa".
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/PF07724_full_family.fa");
    GTUtilsWorkflowDesigner::runWorkflow();
    //    Expected state: Close dashboard tab button is disabled

    QTabWidget* dashboardWidget = GTUtilsDashboard::getTabWidget();
    QWidget* corner = GTTabWidget::getTabCornerWidget(dashboardWidget, dashboardWidget->currentIndex());
    CHECK_SET_ERR(!corner->isEnabled(), "close tab button is unexpectidly enabled");

    //    2) Stop the workflow execution
    GTWidget::click(GTAction::button("Stop workflow"));
    corner = GTTabWidget::getTabCornerWidget(dashboardWidget, dashboardWidget->currentIndex());
    CHECK_SET_ERR(corner->isEnabled(), "close tab button is unexpectidly disabled");
    //    Expected result: Close dashboard tab button is enabled
}

GUI_TEST_CLASS_DEFINITION(test_4714_1) {
    //    1. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call a context menu on the chromatogram, select {Edit new sequence} menu item.
    GTUtilsDialog::add(new PopupChooserByText({"Edit new sequence"}));
    GTUtilsDialog::add(new AddNewDocumentDialogFiller("FASTA", sandBoxDir + "test_4714_1.fa"));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: a new sequence is added to the project and to the current sequence view.
    //    3. Lock the added document.
    GTUtilsDocument::lockDocument("test_4714_1.fa");

    //    4. Call a context menu on the chromatogram, select {Remove edited sequence} menu item.
    GTUtilsDialog::add(new PopupChooserByText({"Remove edited sequence"}));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);

    //    Expected state: the sequence is removed from the view,
    //                    context menu contains "Edit new sequence" and "Edit existing sequence" items
    //                    and doesn't contain "Remove edited sequence" and "Undo changes" actions.
    int sequencesCount = GTUtilsSequenceView::getSeqWidgetsNumber();
    CHECK_SET_ERR(1 == sequencesCount, QString("An incorrect vount of sequences in the view: expect %1, got %2").arg(1).arg(sequencesCount));

    const QStringList visibleItems = {"Edit new sequence", "Edit existing sequence"};
    GTUtilsDialog::add(new PopupCheckerByText(QStringList(), visibleItems));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);

    const QStringList invisibleItems = {"Remove edited sequence", "Undo changes"};
    GTUtilsDialog::add(new PopupCheckerByText(QStringList(), invisibleItems, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4714_2) {
    //    1. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call a context menu on the chromatogram, select {Edit new sequence} menu item.
    GTUtilsDialog::add(new PopupChooserByText({"Edit new sequence"}));
    GTUtilsDialog::add(new AddNewDocumentDialogFiller("FASTA", sandBoxDir + "test_4714_2.fa"));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: a new sequence is added to the project and to the current sequence view.
    //    3. Lock the added document.
    GTUtilsDocument::lockDocument("test_4714_2.fa");
    GTUtilsDialog::checkNoActiveWaiters();

    //    4. Remove the added document from the project.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save document:"));
    GTUtilsDocument::removeDocument("test_4714_2.fa", GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the sequence is removed from the view,
    //                    context menu contains "Edit new sequence" and "Edit existing sequence" items
    //                    and doesn't contain "Remove edited sequence" and "Undo changes" actions.
    int sequencesCount = GTUtilsSequenceView::getSeqWidgetsNumber();
    CHECK_SET_ERR(sequencesCount == 1, QString("An incorrect count of sequences in the view: expect %1, got %2").arg(1).arg(sequencesCount));

    const QStringList visibleItems = {"Edit new sequence", "Edit existing sequence"};
    GTUtilsDialog::add(new PopupCheckerByText(QStringList(), visibleItems));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);

    const QStringList invisibleItems = {"Remove edited sequence", "Undo changes"};
    GTUtilsDialog::add(new PopupCheckerByText(QStringList(), invisibleItems, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4718) {
    GTLogTracer lt;

    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "Search in Sequence" options panel tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    //    3. Close the tab.
    GTUtilsOptionPanelSequenceView::closeTab(GTUtilsOptionPanelSequenceView::Search);

    //    4. Open the tab again.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    //    Expected state: log does not contain errors.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4719_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    3. Click "align_new_sequences_to_alignment_action" and select "_common_data/fasta/amino_ext.fa".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/amino_ext.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::checkCurrentUserDataValue(colorScheme, MsaColorScheme::UGENE_NUCL);
    GTComboBox::checkCurrentUserDataValue(highlightingScheme, MsaHighlightingScheme::EMPTY);

    //    4. Undo changes
    GTUtilsMsaEditor::undo();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    colorScheme = GTWidget::findComboBox("colorScheme");
    highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::checkCurrentUserDataValue(colorScheme, MsaColorScheme::UGENE_NUCL);
    GTComboBox::checkCurrentUserDataValue(highlightingScheme, MsaHighlightingScheme::EMPTY);
}

GUI_TEST_CLASS_DEFINITION(test_4719_2) {
    //    1. Open "_common_data/clustal/amino_ext.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/amino_ext.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    3. Click "align_new_sequences_to_alignment_action" and select "_common_data/fasta/fa1.fa" (base DNA alphabet).
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/fa1.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: "UGENE" color scheme is selected, "UGENE" highlight scheme is selected
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::checkCurrentUserDataValue(colorScheme, MsaColorScheme::UGENE_AMINO);
    GTComboBox::checkCurrentUserDataValue(highlightingScheme, MsaHighlightingScheme::EMPTY);

    //    4. Undo changes
    GTUtilsMsaEditor::undo();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    colorScheme = GTWidget::findComboBox("colorScheme");
    highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::checkCurrentUserDataValue(colorScheme, MsaColorScheme::UGENE_AMINO);
    GTComboBox::checkCurrentUserDataValue(highlightingScheme, MsaHighlightingScheme::EMPTY);
}

GUI_TEST_CLASS_DEFINITION(test_4719_3) {
    //    1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "align_new_sequences_to_alignment_action" and select "data/samples/Genbank/PBR322.gb".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/PBR322.gb"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Open/close highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::closeTab(GTUtilsOptionPanelMsa::Highlighting);

    //    4. Undo changes
    GTUtilsMsaEditor::undo();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    5. Open/close highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");

    GTComboBox::checkCurrentUserDataValue(colorScheme, MsaColorScheme::UGENE_AMINO);
    GTComboBox::checkCurrentUserDataValue(highlightingScheme, MsaHighlightingScheme::EMPTY);
}

GUI_TEST_CLASS_DEFINITION(test_4721) {
    // Build 2 trees and check that layout settings are preserved while switching between trees.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("tree_test_4721_1", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    auto layoutCombo = GTWidget::findComboBox("layoutCombo");
    auto treeViewCombo = GTWidget::findComboBox("treeViewCombo");
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    GTComboBox::selectItemByText(treeViewCombo, "Cladogram");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller("tree_test_4721_2", 0, 0, true));
    GTUtilsMsaEditor::clickBuildTreeButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    layoutCombo = GTWidget::findComboBox("layoutCombo");
    treeViewCombo = GTWidget::findComboBox("treeViewCombo");
    GTComboBox::selectItemByText(layoutCombo, "Unrooted");
    GTComboBox::selectItemByText(treeViewCombo, "Phylogram");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to the first tree and check the settings are restored.
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0);
    layoutCombo = GTWidget::findComboBox("layoutCombo");
    treeViewCombo = GTWidget::findComboBox("treeViewCombo");
    GTComboBox::checkCurrentValue(layoutCombo, "Circular");
    GTComboBox::checkCurrentValue(treeViewCombo, "Cladogram");

    // Switch to the second tree and check the settings are restored.
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 1);
    layoutCombo = GTWidget::findComboBox("layoutCombo");
    treeViewCombo = GTWidget::findComboBox("treeViewCombo");
    GTComboBox::checkCurrentValue(layoutCombo, "Unrooted");
    GTComboBox::checkCurrentValue(treeViewCombo, "Phylogram");
}

GUI_TEST_CLASS_DEFINITION(test_4728) {
    // 1. Open 'fa1.fa'
    GTFileDialog::openFile(testDir + "_common_data/fasta", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select all sequence by mouse
    ADVSingleSequenceWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber();

    QPoint startPos, endPos;
    int indent = 50;
    qreal widgetCenterHeight = seqWidget->rect().height() / 2;
    startPos = seqWidget->mapToGlobal(QPoint(indent, widgetCenterHeight));
    endPos = seqWidget->mapToGlobal(QPoint(seqWidget->rect().width() - indent, widgetCenterHeight));

    GTMouseDriver::moveTo(startPos);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(endPos);
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // Expected state: all 4 symbols are selected
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selection.size() == 1, "Incorrect number of regions in selection is detected");
    CHECK_SET_ERR(selection.at(0).length == 4, "Incorrect selection length is detected");
}

GUI_TEST_CLASS_DEFINITION(test_4732) {
    QFile::copy(dataDir + "samples/FASTA/human_T1.fa", sandBoxDir + "test_4732.fa");
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(sandBoxDir + "test_4732.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Document context menu -> Export / Import -> Export sequences.
    // Expected: "Export selected sequences" dialog appears.
    class Scenario : public CustomScenario {
    public:
        Scenario()
            : filler(nullptr) {
        }
        void setFiller(ExportSelectedRegionFiller* value) {
            filler = value;
        }
        void run() override {
            // 3. Delete "human_T1.fa" document from the file system.
            bool removed = QFile::remove(sandBoxDir + "test_4732.fa");
            CHECK_SET_ERR(removed, "Can't remove the file");

            // Expected: the dialog about external modification of documents does not appear.
            GTGlobals::sleep(5000);  // Wait until UGENE detects the change.
            CHECK_SET_ERR(filler != nullptr, "NULL filler");
            filler->setPath(sandBoxDir);
            filler->setName("test_4732_out.fa");

            // 4. Click "Export".
            filler->commonScenario();
        }

    private:
        ExportSelectedRegionFiller* filler;
    };
    // Expected: the dialog about external modification of documents appears.
    // 5. Click "No".
    // Expected: UGENE does not crash.
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
    Scenario* scenario = new Scenario();
    auto filler = new ExportSelectedRegionFiller(scenario);
    scenario->setFiller(filler);
    GTUtilsDialog::add(filler);
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTUtilsProjectTreeView::click("test_4732.fa", Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();  // wait for all GTUtilsDialog::waitForDialog waiters are finished (file removed message box).
}

GUI_TEST_CLASS_DEFINITION(test_4734) {
    //    1. Open file {data/samples/FASTA/human_T1.fa}
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open {Context menu -> Analyze menu} and check menu item "Show circular view" is not present there

    class AllPopupChecker : public CustomScenario {
        void run() {
            auto activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            CHECK_SET_ERR(nullptr != activePopupMenu, "Active popup menu is NULL");
            GTMenu::clickMenuItemByText(activePopupMenu, {"Analyze"});
            activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            QAction* showCircular = GTMenu::getMenuItem(activePopupMenu, "globalToggleViewAction", false);
            CHECK_SET_ERR(showCircular == nullptr, "'Toggle circular view' menu item should be NULL");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::waitForDialog(new PopupChecker(new AllPopupChecker));
    GTMenu::showContextMenu(GTWidget::findWidget("ADV_single_sequence_widget_0"));
}

GUI_TEST_CLASS_DEFINITION(test_4735) {
    GTFileDialog::openFile(testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Enable "Simple overview".
    GTUtilsDialog::waitForDialog(new PopupChooser({"Show simple overview"}));
    GTMenu::showContextMenu(GTWidget::findWidget("msa_overview_area"));
    GTThread::waitForMainThread();

    auto simpleOverviewWidget = GTWidget::findWidget("msa_overview_area_simple");
    CHECK_SET_ERR(simpleOverviewWidget->isVisible(), "simple overview is not visiable");

    // Check Simple Overview has an empty gray color.
    QImage img = GTWidget::getImage(simpleOverviewWidget);
    QRgb rgb = img.pixel(simpleOverviewWidget->rect().bottomLeft() + QPoint(10, -10));
    QColor c(rgb);
    CHECK_SET_ERR(c.name() == "#ededed", "1. Simple overview has wrong color: " + c.name());

    // 3. Append sequence eas.fastq to alignment.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/fastq/", "eas.fastq"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check the overview has non-empty color now.
    img = GTWidget::getImage(simpleOverviewWidget);
    rgb = img.pixel(simpleOverviewWidget->rect().bottomLeft() + QPoint(10, -10));
    c = QColor(rgb);
    CHECK_SET_ERR(c.name() == "#b9d5e4", "2. Simple overview has wrong color: " + c.name());

    // Undo changes.
    GTUtilsMsaEditor::undo();
    GTThread::waitForMainThread();

    // Check simple overview has an empty gray color again.
    img = GTWidget::getImage(simpleOverviewWidget);
    rgb = img.pixel(simpleOverviewWidget->rect().bottomLeft() + QPoint(10, -10));
    c = QColor(rgb);
    CHECK_SET_ERR(c.name() == "#ededed", "3. Simple overview has wrong color: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(test_4779) {
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4779/ReadAlignmentMuscleElement.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4779/ReadAlignmentMusclePortAlias.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4764_1) {
    // 1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Add some gaps
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(5, 5));
    GTKeyboardDriver::keyClick(' ');
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(5, 6));
    GTKeyboardDriver::keyClick(' ');
    GTKeyboardDriver::keyClick(' ');
    GTKeyboardDriver::keyClick(' ');

    // 3. Select region with edited sequences, one of sequences should starts with gap
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 5), QPoint(16, 9));

    // 4. Copy this subalignment
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor* editor = mw->findChild<MSAEditor*>();
    QWidget* nameListWidget = editor->getUI()->getUI(0)->getEditorNameList();

    // 5. Open conext menu by right clicking "Name list area". Paste this subaliment throu context menu {Copy/Paste->Paste}
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Paste"}));
    GTWidget::click(nameListWidget, Qt::RightButton);

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList().size() == 23, "Number of sequences should be 23");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 10), QPoint(11, 14), GTGlobals::UseMouse);

    QString expectedClipboard = "-CTACTAATTCG\n---TTATTAATT\nTTGCTAATTCGA\nTTATTAATCCGG\nCTATTAATTCGA";

    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedClipboard, "expected test didn't equal to actual");

    // Expected state subalignment pasted correctly
    GTKeyboardUtils::copy();
    clipboardText = GTClipboard::text();
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea());
    CHECK_SET_ERR(clipboardText == expectedClipboard, "expected test didn't equal to actual");
}

GUI_TEST_CLASS_DEFINITION(test_4764_2) {
    // 1. Select one sequence in the alignment
    // 2. Copy and paste it
    // 3. Expected state : sequence added to the end of file
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4764", "4764.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor* editor = mw->findChild<MSAEditor*>();
    QWidget* sequenceAreaWidget = editor->getUI()->getUI(0)->getSequenceArea();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(15, 0), GTGlobals::UseMouse);
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Paste"}));
    GTWidget::click(sequenceAreaWidget, Qt::RightButton);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 7, "Sequence count should be 7");
}

GUI_TEST_CLASS_DEFINITION(test_4764_3) {
    // 1. Select sub - alignment with a few lines full of gaps
    // 2. Copy and paste it
    // 3. Expected state : gapped only sequences not added to msa
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4764", "4764.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor* editor = mw->findChild<MSAEditor*>();
    QWidget* sequenceAreaWidget = editor->getUI()->getUI(0)->getSequenceArea();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 0), QPoint(5, 4));
    GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu();

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Paste"}));
    GTWidget::click(sequenceAreaWidget, Qt::RightButton);

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 8, "Sequence count should be 7");
}

GUI_TEST_CLASS_DEFINITION(test_4782) {
    //    1. Open "data/samples/genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: a Sequence View for "sars.gb" is active.
    const QString sarsMdiTitle = "NC_004718 [sars.gb]";
    QString activeMdiTitle = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(sarsMdiTitle == activeMdiTitle, QString("An incorrect MDI is active: expected '%1', got '%2'").arg(sarsMdiTitle).arg(activeMdiTitle));

    //    3. Click "Build dotplot" button on the toolbar.
    //    4. Click "OK" button in the opened dialog.
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("DotPlotDialog"));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build dotplot");

    //    Expected state: a Sequence View for "sars.gb" is active, it contains a dotplot and two sequences.
    CHECK_SET_ERR(sarsMdiTitle == activeMdiTitle, QString("An incorrect MDI is active: expected '%1', got '%2'").arg(sarsMdiTitle).arg(activeMdiTitle));
    int sequenceWidgetsNumber = GTUtilsSequenceView::getSeqWidgetsNumber();
    CHECK_SET_ERR(2 == sequenceWidgetsNumber, QString("Expected 2 sequence widgets, got %2").arg(sequenceWidgetsNumber));
    GTWidget::findWidget("dotplot widget", GTUtilsMdi::activeWindow());

    //    5. Activate the Sequence View for "murine.gb", that was opened on the file opening.
    const QString murineMdiTitle = "NC_001363 [murine.gb]";
    GTUtilsMdi::activateWindow(murineMdiTitle);

    //    Expected state: active view is "murine.gb" sequence view (without dotplot).
    activeMdiTitle = GTUtilsMdi::activeWindowTitle();

    CHECK_SET_ERR(murineMdiTitle == activeMdiTitle, QString("An incorrect MDI is active: expected '%1', got '%2'").arg(murineMdiTitle).arg(activeMdiTitle));
    sequenceWidgetsNumber = GTUtilsSequenceView::getSeqWidgetsNumber();
    CHECK_SET_ERR(1 == sequenceWidgetsNumber, QString("Expected 1 sequence widget, got %2").arg(sequenceWidgetsNumber));
    GTGlobals::FindOptions findOptions(false);
    auto dotplotWidget = GTWidget::findWidget("dotplot widget", GTUtilsMdi::activeWindow(), findOptions);
    CHECK_SET_ERR(dotplotWidget == nullptr, "A dotplot widget unexpectedly found");

    //    6. Select all documents in project. Press delete.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: UGENE doesn't crash, all views are closed, all documents are removed from the project.
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::checkProject(GTUtilsProject::Empty);

    findOptions.matchPolicy = Qt::MatchContains;
    QWidget* sarsMdi = GTUtilsMdi::findWindow(sarsMdiTitle, findOptions);
    CHECK_SET_ERR(sarsMdi == nullptr, "'sars.gb' Sequence View is not closed");

    QWidget* murineMdi = GTUtilsMdi::findWindow(murineMdiTitle, findOptions);
    CHECK_SET_ERR(murineMdi == nullptr, "'murine.gb' Sequence View is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_4783) {
    // 1. Open _common_data/scenarios/_regression/4783/4783.aln
    // 2. Open general option panel tab, set consensus algorithm "Levitsky" and treshold 90
    // Expected state: consensus is BA
    // 3. Remove "2" sequence
    // Expected state: consensus is -A
    // 4. Close view, and open again
    // Expected state: consensus is -A
    // 5. Remove "1" sequence
    // Expected state: consensus is BB

    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4783/4783.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    auto consensusType = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusType, "Levitsky");
    auto thresholdSpinBox = GTWidget::findSpinBox("thresholdSpinBox");
    GTSpinBox::setValue(thresholdSpinBox, 90, GTGlobals::UseKeyBoard);
    GTUtilsMSAEditorSequenceArea::checkConsensus("-H");

    GTUtilsMsaEditor::clickSequenceName("2");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::checkConsensus("-A");

    GTUtilsMdi::closeWindow("4783 [4783.aln]");

    GTUtilsProjectTreeView::doubleClickItem("4783.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::checkConsensus("-A");

    GTWidget::click(GTAction::button("msa_action_undo"));

    GTUtilsMsaEditor::clickSequenceName("1");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::checkConsensus("BB");
}

GUI_TEST_CLASS_DEFINITION(test_4784_2) {
    QFile::copy(testDir + "_common_data/fasta/chr6.fa", sandBoxDir + "regression_test_4784_2.fa");

    // 1. Open "_common_data/fasta/chr6.fa".
    GTFileDialog::openFile(sandBoxDir + "regression_test_4784_2.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Sequence context menu Analyze -> Query with local BLAST...
    // 3. Press "Select a database file".
    // 4. Choose "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr".
    // 6. Press "Search".
    BlastLocalSearchDialogFiller::Parameters settings;
    settings.runBlast = true;
    settings.inputPath = sandBoxDir + "regression_test_4784_2.fa";
    settings.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::add(new BlastLocalSearchDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Query with local BLAST..."}, GTGlobals::UseMouse);

    // 5. Delete "chr6.fa" in file browser.
    // 7. Click "No" in the appeared message box.
    // Expected result: An error notification appears - "A problem occurred during doing BLAST. The sequence is no more available".
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No, "was removed from"));
    QFile::remove(sandBoxDir + "regression_test_4784_2.fa");
    GTGlobals::sleep(5000);  // Wait until UGENE detects the change.
    GTUtilsNotifications::waitForNotification(true, "The sequence is no more available");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4785_1) {
    // 1. Open an alignment.
    GTFile::copy(testDir + "_common_data/clustal/3000_sequences.aln", sandBoxDir + "test_4785.aln");
    GTFileDialog::openFile(sandBoxDir, "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use context menu { Align->Align profile to profile with MUSCLE }
    // 3. Select any alignment and press "Ok"
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}));
    GTUtilsDialog::add(new GTFileDialogUtils(dataDir + "samples/CLUSTALW", "COI.aln"));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::checkTaskIsPresent("MUSCLE");

    // 4. Delete "test_4785.aln"
    // Expected result : a problem occurred during aligning profile to profile with MUSCLE.The original alignment is no more available.
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No, "was removed from"));
    QFile::remove(sandBoxDir + "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4785_2) {
    // Difference with previous scenario is to delete alignment before alignment, selected in dialog box, will be loaded.
    // 1. Open "1000_sequences.aln"
    // Expected state : a file browser appeared
    GTFile::copy(testDir + "_common_data/clustal/1000_sequences.aln", sandBoxDir + "test_4785.aln");
    GTFileDialog::openFile(sandBoxDir, "test_4785.aln");
    GTUtilsMsaEditor::getActiveMsaEditorWindow();

    // 2. Use context menu { Align->Align profile to profile with MUSCLE }
    // 3. Select any alignment and press "Ok"
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/clustal/", "1000_sequences.aln"));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsDialog::checkNoActiveWaiters();

    // 4. Delete "test_4785.aln"
    // Expected result : An error notification appears :
    // A problem occurred during aligning profile to profile with MUSCLE.The original alignment is no more available.
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No, "was removed from"));
    QFile::remove(sandBoxDir + "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished(60000);
    GTUtilsNotifications::waitForNotification(true, "A problem occurred during aligning profile to profile with MUSCLE. The original alignment is no more available.");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4795) {
    //    1. Open "_common_data/clustal/amino_ext.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta/amino_ext.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QModelIndex aminoExtIdx = GTUtilsProjectTreeView::findIndex("amino_ext");
    GTUtilsProjectTreeView::dragAndDrop(aminoExtIdx, GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    auto highlightingScheme = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::checkCurrentValue(colorScheme, "UGENE    ");
    GTComboBox::checkCurrentValue(highlightingScheme, "No highlighting    ");
}

GUI_TEST_CLASS_DEFINITION(test_4799) {
    GTFileDialog::openFile(testDir + "_common_data/fasta/amino_ext.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // In the context menu of the sequences area select "Add->Sequence from current project".
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("amino_ext.fa", "amino_ext"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from current project..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check the result.
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 19, QString("Sequence count mismatch. Expected: 19. Actual: %1").arg(names.size()));
    CHECK_SET_ERR(names.last() == "amino_ext", QString("Inserted sequence name mismatch. Expected: amino_ext. Actual: %1").arg(names.last()));
}

GUI_TEST_CLASS_DEFINITION(test_4803_1) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Call context menu on tree tab.Press "Close tab" menu item
    // bug state : tab is colsed, but tree view is empty, and tree settings on options panel still present.Any change of tree settings causes crash
    GTUtilsDialog::waitForDialog(new PopupChooser({"Close tab"}));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);
    CHECK_SET_ERR(GTWidget::findTabWidget("MsaEditorTreeTab", nullptr, {false}) == nullptr, "Msa editor tree tab widget is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_4803_2) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Call context menu on tree tab. Check that there are not items "Add horizontal splitter" or "Add vertical splitter"
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Add horizontal splitter"}, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Add vertical splitter"}, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);
    // 3. Call context menu on tree tab. Check that "Close other tabs" is disabled
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Close other tabs"}, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4803_3) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Call context menu on tree tab.Press "Close other tabs" menu item
    // bug state : UGENE hangs up
    GTUtilsDialog::waitForDialog(new PopupChooser({"Close other tabs"}));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);

    // 5. Call context menu on tree tab. Check that "Close other tabs" item is disabled.
    // Bug state : both tabs are closed
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Close other tabs"}, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);

    // 6. Call context menu on tree tab. Press "Close tab".
    // Expected state: tree tab widget is closed
    GTUtilsDialog::waitForDialog(new PopupChooser({"Close tab"}));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);

    CHECK_SET_ERR(GTWidget::findTabWidget("MsaEditorTreeTab", nullptr, {false}) == nullptr, "Msa editor tree tab widget is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_4803_4) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Call context menu on tree tab. Check that there are not items "Refresh tab" and "Refresh all tabs"
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Refresh tab"}, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Refresh all tabs"}, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(GTWidget::findTabWidget("MsaEditorTreeTab"), 0, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4804_1) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_dna.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_dna.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Add dna extended sequence via context menu {Add->Sequence from file}
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Extended DNA\"");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804/ext_dna.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Add dna extended sequence via context menu {Add->Sequence from file}
    GTUtilsNotifications::waitForNotification(true, "from \"Extended DNA\" to \"Raw\"");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804/ext_rna.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4804_2) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_rna.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_rna.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Add rna extended sequence via menu {Actions->Add->Sequence from file}
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804/ext_rna.fa"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from file..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::waitForNotification(true, "from \"Standard RNA\" to \"Extended RNA\"");
    GTUtilsDialog::checkNoActiveWaiters();

    //    3. Add dna extended sequence via context menu {Add->Sequence from file}
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804/standard_amino.fa"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from file..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::waitForNotification(true, "from \"Extended RNA\" to \"Raw\"");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4804_3) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_amino.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "ext_dna.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Add  extended amino sequence by drag and drop
    QModelIndex toDragNDrop = GTUtilsProjectTreeView::findIndex("ext_amino_seq");
    GTUtilsProjectTreeView::dragAndDrop(toDragNDrop, GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::checkNoActiveWaiters();

    //    3. Add  extended DNA sequence by drag and drop
    toDragNDrop = GTUtilsProjectTreeView::findIndex("ext_dna_seq");
    GTUtilsProjectTreeView::dragAndDrop(toDragNDrop, GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    GTUtilsNotifications::waitForNotification(true, "from \"Extended amino acid\" to \"Raw\"");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4804_4) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_dna.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Use 'align_new_sequences_to_alignment_action' toolbar button to align Extended rna sequence to alignment
    // Expected state: corresponding notification message has appeared
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Raw\"");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_rna.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4804_5) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_rna.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Use 'align_new_sequences_to_alignment_action' toolbar button to align Extended rna sequence to alignment
    // Expected state: corresponding notification message has appeared
    GTUtilsNotifications::waitForNotification(true, "from \"Standard RNA\" to \"Raw\". Use \"Undo\", if you'd like to restore the original alignment.");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_dna.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4804_6) {
    // Open DNA sequence.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_dna.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Add amino sequence and check the alphabet change notification.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804/ext_amino.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_LOAD, "Sequence from file"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Raw\"");
    GTUtilsDialog::checkNoActiveWaiters();

    //   Undo the changes: no notification is expected.
    GTUtilsMsaEditor::undo();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::checkNoVisibleNotifications();

    //   Redo the changes and check the notification again.
    GTUtilsMsaEditor::redo();
    GTUtilsNotifications::waitForNotification(true, "from \"Standard DNA\" to \"Raw\"");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4833_1) {
    // Add sequence from current project by main menu
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "ext_dna.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("ext_dna.fa", "ext_dna_seq"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from current project..."});
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Raw\"");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4833_2) {
    // Add sequence from current project by context menu
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("ext_amino.fa", "ext_amino_seq"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from current project"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4833_3) {
    // align sequences to profile by MUSCLE by main menu
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align sequences to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4833_4) {
    // align sequences to profile by MUSCLE by context menu
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align sequences to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4833_5) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("Align sequences to alignment with MUSCLE");
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4833_6) {
    // align sequences to profile by MUSCLE by main menu
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4833_7) {
    // align sequences to profile by MUSCLE by context menu
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}, GTGlobals::UseMouse));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4833_8) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("Align alignment to alignment with MUSCLE");
    GTUtilsNotifications::waitForNotification(true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_4839_1) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: document are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex("COI.aln");

    // 2. Select some sequences on sequence view.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 4), QPoint(2, 6));
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 3. Call context menu on the name list area, select the {Edit -> Remove sequence} menu item.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "Remove sequence"}));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: the sequences are removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 3, "The number of sequences remained unchanged.");
}

GUI_TEST_CLASS_DEFINITION(test_4839_2) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: document are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex("COI.aln");

    // 2. Select some sequences on sequence view.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 4), QPoint(2, 6));
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 3. Use main menu item {Actions -> Edit -> Remove sequence}.
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove sequence(s)"});
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: the sequences are removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 3, "The number of sequences remained unchanged.");
}

GUI_TEST_CLASS_DEFINITION(test_4841) {
    GTFileDialog::openFile(dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelPhyTree::openTab();

    TvNodeItem* midNode = GTUtilsPhyTree::getNodeByBranchText("0.033", "0.069");
    TvNodeItem* childNode = GTUtilsPhyTree::getNodeByBranchText("0.016", "0.017");
    TvNodeItem* parentNode = GTUtilsPhyTree::getNodeByBranchText("0.068", "0.007");

    GTUtilsPhyTree::clickNode(midNode);
    int originalFontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    int newFontSize1 = originalFontSize + 2;
    int newFontSize2 = originalFontSize + 4;
    GTUtilsOptionPanelPhyTree::setFontSize(newFontSize1);

    GTUtilsPhyTree::clickNode(parentNode);
    int fontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    CHECK_SET_ERR(fontSize == originalFontSize, QString("1. 'parentNode' font must not change: %1, expected: %2").arg(fontSize).arg(originalFontSize));

    GTUtilsPhyTree::clickNode(midNode);
    fontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    CHECK_SET_ERR(fontSize == newFontSize1, QString("2. 'midNode' font does not match: %1, expected: %2").arg(fontSize).arg(newFontSize1));

    GTUtilsPhyTree::clickNode(childNode);
    fontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    CHECK_SET_ERR(fontSize == newFontSize1, QString("3. 'childNode' font does not match: %1, expected: %2").arg(fontSize).arg(newFontSize1));

    // Collapse subtree and change font again.
    GTUtilsPhyTree::doubleClickNode(midNode);
    GTUtilsOptionPanelPhyTree::setFontSize(newFontSize2);

    GTUtilsPhyTree::clickNode(parentNode);
    fontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    CHECK_SET_ERR(fontSize == originalFontSize, QString("4. 'parentNode' font must not change: %1, expected: %2").arg(fontSize).arg(originalFontSize));

    GTUtilsPhyTree::clickNode(midNode);
    fontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    CHECK_SET_ERR(fontSize == newFontSize2, QString("5. 'midNode' font does not match: %1, expected: %2").arg(fontSize).arg(newFontSize1));

    GTUtilsPhyTree::doubleClickNode(midNode);
    GTUtilsPhyTree::clickNode(childNode);
    fontSize = GTUtilsOptionPanelPhyTree::getFontSize();
    CHECK_SET_ERR(fontSize == newFontSize2, QString("6. 'childNode' font does not match: %1, expected: %2").arg(fontSize).arg(newFontSize1));
}

GUI_TEST_CLASS_DEFINITION(test_4852) {
    // 1. Open _common_data/genome_aligner/chrY.fa
    GTFileDialog::openFile(testDir + "_common_data/genome_aligner", "chrY.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Run Find repeats with default parameters
    QDir().mkpath(sandBoxDir + "test_4852");
    GTUtilsDialog::waitForDialog(new FindRepeatsDialogFiller(sandBoxDir + "test_4852"));
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find repeats"));

    // 3. Delete chrY.fa document from project view
    GTUtilsProjectTreeView::click("chrY.fa");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: 'find repeats' task cancelled
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount() == 0, "Running task count should be 0");
}

GUI_TEST_CLASS_DEFINITION(test_4860) {
    // 1. Open murine.gb
    // 2. Open Find Pattern tab
    // 3. Input pattern
    // 4. Click Next a few times
    // Expected state: the results are selected one by one from left to right, no random selection

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("AAAAA");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto next = GTWidget::findWidget("nextPushButton");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection().size() == 1, "Incorrect selection: selected region should be only one");
    qint64 startPosPrev = GTUtilsSequenceView::getSelection().first().startPos;
    qint64 startPosNext = -1;
    for (int i = 0; i < 10; i++) {
        GTWidget::click(next);

        CHECK_SET_ERR(GTUtilsSequenceView::getSelection().size() == 1, "Incorrect selection: selected region should be only one");
        startPosNext = GTUtilsSequenceView::getSelection().first().startPos;

        CHECK_SET_ERR(startPosPrev < startPosNext, "Search results are disordered");
        startPosPrev = startPosNext;
    }
}

GUI_TEST_CLASS_DEFINITION(test_4871) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Open OP and select pairwice alignment tab, select sequences to align, set "in new window" parameter to "false"
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);
    // 3. Set pairwise algorithm "Smith-Waterman"
    GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm("Smith-Waterman");
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Phaneroptera_falcata");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Isophya_altaica_EF540820");

    auto widget = GTWidget::findWidget("outputContainerWidget");
    if (widget->isHidden()) {
        GTWidget::click(GTWidget::findWidget("ArrowHeader_Output settings"));
    }
    auto inNewWindowCheckBox = GTWidget::findCheckBox("inNewWindowCheckBox");
    GTCheckBox::setChecked(inNewWindowCheckBox, false);
    GTWidget::click(GTWidget::findWidget("alignButton"));

    // 4. Undo changes
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state "Undo" button is disabled
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Button should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_4885_1) {
    //    1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Wait until overview finishes rendering.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Doubleclick the first symbol of the first sequence.
    //    Expected state: overview doesn't start recalculation.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::doubleClick();

    QWidget* graphOverview = GTUtilsMsaEditor::getGraphOverview();
    CHECK_SET_ERR(nullptr != graphOverview, "Graph overview is NULL");
    const QColor actualColor = GTUtilsMsaEditor::getGraphOverviewPixelColor(QPoint(graphOverview->width() / 2, 2));
    CHECK_SET_ERR("#ffffff" == actualColor.name(), QString("Incorrect color of the graph overview ('%1'). Does it render now?").arg(actualColor.name()));
}

GUI_TEST_CLASS_DEFINITION(test_4885_2) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Build a tree. The result tree should be synchronised with the msa.
    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_4885_2.nwk");

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::doubleClick();
    // Test will fail if there is any opened dialog (like 'Alignment is modified').
}

GUI_TEST_CLASS_DEFINITION(test_4885_3) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFile::copy(dataDir + "samples/CLUSTALW/COI.aln", sandBoxDir + "test_4885_3.aln");
    GTFileDialog::openFile(sandBoxDir + "test_4885_3.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Build a tree. The result tree should be synchronised with the msa.
    GTUtilsMsaEditor::buildPhylogeneticTree(sandBoxDir + "test_4885_3.nwk");

    //    3. Save the project.
    GTUtilsProject::saveProjectAs(sandBoxDir + "test_4885_3.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Close the project.
    GTUtilsProject::closeProject(false);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    5. Open the saved project.
    //    Expected state: there are two unloaded documents in a project: COI.aln and its tree.
    GTFileDialog::openFile(sandBoxDir + "test_4885_3.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    6. Doubleclick the COI.aln document.
    GTUtilsProjectTreeView::doubleClickItem("test_4885_3.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: MSA Editor opens, there is an attached tree in it. The msa object in the project view is not modified.
    GTUtilsProjectTreeView::itemModificationCheck("test_4885_3", false);
}

GUI_TEST_CLASS_DEFINITION(test_4886) {
    GTFileDialog::openFile(dataDir + "samples/SCF/", "90-JRI-07.scf");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTLogTracer lt;
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView();
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("90-JRI-07.scf"));
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CHROMATOGRAM}));
    GTUtilsDialog::add(new ExportChromatogramFiller(testDir + "_common_data/scenarios/sandbox/", "90-JRI-07.scf", ExportChromatogramFiller::SCF, false, false, true));
    GTMouseDriver::click(Qt::RightButton);
    auto parent = GTWidget::findWidget("90-JRI-07 sequence [90-JRI-07.scf] 2");
    GTWidget::findWidget("ADV_single_sequence_widget_0", parent);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4908) {
    // 1. Open s file with multiple sequences
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::selectSequenceRegion(1, 199950);
    GTKeyboardUtils::copy();

    GTFileDialog::openFile(testDir + "_common_data/fasta/", "seq5.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(testDir + "_common_data/fasta/", "seq2.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("SEQUENCE_WITH_A_ENTRY_2", Qt::LeftButton);

    QWidget* detView = GTUtilsSequenceView::getDetViewByNumber();
    QPoint detPos = detView->mapToGlobal(detView->rect().center());

    GTMouseDriver::dragAndDrop(GTMouseDriver::getMousePosition(), detPos);

    GTUtilsSequenceView::enableEditingMode(true, 0);
    GTUtilsSequenceView::enableEditingMode(true, 1);

    // 2. Select the first sequence and add data to the clipboard
    DetView* firstSeqWidget = GTUtilsSequenceView::getDetViewByNumber(0);
    GTWidget::click(firstSeqWidget);
    GTKeyboardUtils::paste();

    // 3. While the data is been pasted, select the second sequence
    DetView* secondSeqWidget = GTUtilsSequenceView::getDetViewByNumber(1);
    GTWidget::click(secondSeqWidget);

    // 4. Check if the data is pasted to the first sequence
    int len = GTUtilsSequenceView::getSequenceAsString(0).length();
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_4913) {
    /* 1. Open an amino acid sequence: data/samples/Swiss-Prot/P16152.txt
     * 2. Open the NCBI BLAST dialog.
     *   Expected result: the "blastp" tool is set up.
     * 3. Open the "Advanced options" tab.
     *   Expected result: The "Word size" parameter is set to "6".
     * 4. Press "Esc" button.
     */
    GTUtilsDialog::waitForDialog(new SelectDocumentFormatDialogFiller());
    GTUtilsProject::openFile(dataDir + "samples/Swiss-Prot/P16152.txt");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CheckWordSizeScenario : public CustomScenario {
    public:
        CheckWordSizeScenario() {
        }
        void run() {
            auto comboAlg = GTWidget::findComboBox("dataBase");
            CHECK_SET_ERR(comboAlg->currentText() == "blastp", QString("Value of dataBase not equal blastp, it has other default value: %1!").arg(comboAlg->currentText()));

            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("optionsTab"), 1);
            auto combo = GTWidget::findComboBox("wordSizeComboBox");
            CHECK_SET_ERR(combo->currentText() == "6", QString("Value of wordSizeComboBox not equal 6, it has other default value: %1!").arg(combo->currentText()));

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    CheckWordSizeScenario* scenario = new CheckWordSizeScenario();
    RemoteBLASTDialogFiller* filler = new RemoteBLASTDialogFiller(scenario);

    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ANALYSE", "Query NCBI BLAST database"}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getSeqWidgetByNumber());
}

GUI_TEST_CLASS_DEFINITION(test_4918) {
    // 1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Align->Align profile to profile with MUSCLE" in the MSAEditor context menu
    // Expected state : "Select file with alignment" dialog is appeared
    // 3. Select file "_common_data\clustal\1000_sequences.aln"
    // Current state : UGENE crashes
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/clustal", "1000_sequences.aln"));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4918_1) {
    // 1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Align->Align profile to profile with MUSCLE" in the MSAEditor context menu
    // Expected state : "Select file with alignment" dialog is appeared
    // 3. Select file "_common_data\genbank\DNA.gen"
    // Current state : UGENE crashes
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align profile to profile with MUSCLE"}));
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/genbank", "DNA.gen"));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_4934) {
    // 1. Open samples/CLUSTALW/ty3.aln.gz
    GTLogTracer lt;
    // GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/1798", "1.4k.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Align with Kalign
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new KalignDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    // 3. while aligning lock document for editing
    GTUtilsDocument::lockDocument("1.4k.aln");

    GTUtilsTaskTreeView::waitTaskFinished();
    // 4. Unlock document after alignment finished
    GTUtilsDocument::unlockDocument("1.4k.aln");

    // 5. Align with Kalign again
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new KalignDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasError("Object '1.4k.aln' is locked"), "Expected error not found");
    int errorNum = lt.errorMessages.size();
    CHECK_SET_ERR(errorNum == 1, QString("Too many errors in log: %1").arg(errorNum));
}

GUI_TEST_CLASS_DEFINITION(test_4936) {
    GTLogTracer lt;

    //    1. Open "data/samples/Swiss-Prot/D0VTW9.txt".
    GTFile::copy(dataDir + "samples/Swiss-Prot/D0VTW9.txt", sandBoxDir + "test_4936.sw");
    GTFileDialog::openFile(sandBoxDir + "test_4936.sw");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    //    2. Edit the file directly.
    //    Expected state: UGENE offers to reload the file.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "Do you want to reload it?"));

    QByteArray data = GTFile::readAll(sandBoxDir + "test_4936.sw");
    data.replace("D0VTW9_9INFA", "00VTW9_9INFA");

    // UGENE does not detect file modification without 1 second interval: sleep 1 second.
    QFile file(sandBoxDir + "test_4936.sw");
    file.open(QFile::WriteOnly);
    file.write(data);
    file.close();

    GTUtilsDialog::checkNoActiveWaiters();

    //    3. Accept the offer.
    //    Expected state: the document is successfully reloaded, there are no errors in the log.
    GTUtilsProjectTreeView::findIndex("00VTW9_9INFA");
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_4938) {
    // 1. Open murine.gb
    // 2. Open Annotations Highlighting tab
    // 3. Remove all annotation groups one by one
    // Expected state: annotations table from Options Panel is not visible, no errors
    // 4. Add annotation
    // Expected state: annotation table is visible

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    auto annTree = GTWidget::findWidget("OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is not visible")

    GTUtilsAnnotationsTreeView::deleteItem("CDS  (0, 4)");
    GTUtilsAnnotationsTreeView::deleteItem("misc_feature  (0, 2)");
    GTUtilsAnnotationsTreeView::deleteItem("comment  (0, 1)");
    GTUtilsAnnotationsTreeView::deleteItem("source  (0, 1)");
    CHECK_SET_ERR(!annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is still visible")

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "group", "feature", "50..60"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});
    CHECK_SET_ERR(annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is not visible")
}

GUI_TEST_CLASS_DEFINITION(test_4938_1) {
    // 1. Open murine.gb
    // 2. Open Annotations Highlighting tab
    // 3. Click "Show all annotations"
    // 4. Remove annotation group, e.g. CDS
    // Expected state: CDS is still peresent in annotation table on Options Panel

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    auto annTree = GTWidget::findTreeWidget("OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is not visible");
    GTWidget::click(GTWidget::findWidget("show_all_annotation_types"));
    CHECK_SET_ERR(!annTree->findItems("CDS", Qt::MatchExactly).isEmpty(), "Cannot find CDS item in OP_ANNOT_HIGHLIGHT_TREE");

    GTUtilsAnnotationsTreeView::deleteItem("CDS  (0, 4)");
    CHECK_SET_ERR(!annTree->findItems("CDS", Qt::MatchExactly).isEmpty(), "CDS item is missing in OP_ANNOT_HIGHLIGHT_TREE");
}

GUI_TEST_CLASS_DEFINITION(test_4965) {
    // 1. Open any assembly
    // 2. Right button click on the assembly object in the project view
    // 3. Export/Import --> Export object
    // Expected state: 'Compress' checkbox is disabled for all assembly formats

    GTFileDialog::openFile(testDir + "_common_data/ugenedb/toy.sam.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CheckCompressFiller : public Filler {
    public:
        CheckCompressFiller()
            : Filler("ExportDocumentDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto comboBox = GTWidget::findComboBox("formatCombo", dialog);

            auto compressCheckBox = GTWidget::findCheckBox("compressCheck", dialog);

            QStringList checkFormats;
            checkFormats << "BAM"
                         << "SAM"
                         << "UGENE Database";
            foreach (const QString& format, checkFormats) {
                GTComboBox::selectItemByText(comboBox, format);
                CHECK_SET_ERR(!compressCheckBox->isEnabled(),
                              QString("Compress checkbox is unexpectedly enabled for '%1' format!").arg(format));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new CheckCompressFiller());
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export/Import", "Export object..."}));
    GTUtilsProjectTreeView::click("ref", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4966) {
    // GTLogTracer lt;
    //     1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open sample "Align sequences with MUSCLE"
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    3. Align "_common_data\clustal\big.aln"
    GTUtilsWorkflowDesigner::click("Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/big.aln");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state:
    //    Workflow finished with error "Not enough resources for the task"
    GTUtilsWorkflowDesigner::checkErrorList("Not enough resources for the task");
}

GUI_TEST_CLASS_DEFINITION(test_4969_1) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    // 2. Open Start Page.
    GTUtilsStartPage::openStartPage();
    // 3. Click the main menu item: "File" -> "Recent files" -> ".../samples/Genbank/murine.gb".
    GTMenu::clickMainMenuItem({"File", "Recent files", QFileInfo(dataDir + "samples/Genbank/murine.gb").absoluteFilePath()});
    // Expected: sequence view is activated.
    QString title = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_4969_2) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    // 2. Unload the sequence.
    GTUtilsDocument::unloadDocument("murine.gb");
    // 3. Click "murine.gb" on Start Page.
    GTWidget::click(GTWidget::findLabelByText("murine.gb").first());
    // Expected: The file is loaded, the view is opened.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded("murine.gb"), "The file is not loaded");
    QString title = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_4983) {
    // 1. Open file : "data/samples/PDB/1CF7.PDB"
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Use context menu on 3D view : {"Coloring schemes"->"Molecular chains"}
    GTUtilsDialog::waitForDialog(new PopupChooser({"Coloring Scheme", "Molecular Chains"}));
    auto widget3d = GTWidget::findWidget("1-1CF7");
    GTWidget::click(widget3d, Qt::RightButton);

    // Expected: the color scheme is changed without errors.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_4985) {
    // Create a file _common_data/scenarios/sandbox/A.fa with text "A".
    // Open this file.
    // Delete this file from hard disk.
    // "File Modification Detected" dialog appears. Click "No".
    // On Start Page open recent file A.fa.
    // Expected: error message box with text "File doesn't exist: _common_data\scenarios\sandbox\a.fa" appears

    QString filePath = testDir + "_common_data/scenarios/sandbox/A.fa";
    IOAdapterUtils::writeTextFile(filePath, "A");

    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new MessageBoxNoToAllOrNo());
    QFile(filePath).remove();
    GTThread::waitForMainThread();
    GTGlobals::sleep(8000);

    GTUtilsStartPage::openStartPage();
    GTThread::waitForMainThread();
    GTGlobals::sleep(8000);

    QString expected = "does not exist";
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("OK", expected));
    GTWidget::click(GTWidget::findLabelByText("- A.fa").first());
}

GUI_TEST_CLASS_DEFINITION(test_4986) {
    //    1. Open "data/samples/Genbank/murine.gb".
    //    2. Open "data/samples/GFF/5prime_utr_intron_A20.gff".
    //    3. Drag and drop any annotation table object from the GFF document to the sequence.
    //    Expected state: you are notified, that some annotations are out of boundaries.
    //    4. Select an annotation which is outside the sequence.
    //    5. Call a context menu, select "Export > Export sequence of selected annotations".
    //    6. Accept the export dialog.
    //    Expected state: there is an error in the log

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(GTUtilsProjectTreeView::findIndex("Ca20Chr1 features"), GTWidget::findWidget("render_area_NC_001363"));

    GTLogTracer lt;
    GTUtils::checkExportServiceIsEnabled();
    GTUtilsDialog::waitForDialog(new ExportSequenceOfSelectedAnnotationsFiller(sandBoxDir + "test_4986.fa", ExportSequenceOfSelectedAnnotationsFiller::Fasta, ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "action_export_sequence_of_selected_annotations"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(GTUtilsAnnotationsTreeView::findItem("5_prime_UTR_intron"));

    GTThread::waitForMainThread();
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_4990) {
    // 1. Open file "_common_data/clustal/big.aln"
    GTFileDialog::openFile(testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTAction::button("Show overview"));

    // 2. Open "Export Consensus" OP tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    // 3. Press "Export" button 3x times
    for (int i = 0; i < 24; i++) {
        GTWidget::click(GTWidget::findWidget("exportBtn"));
    }

    // 4. Remove "big.aln" document
    GTUtilsDocument::removeDocument("big.aln");
    qDebug() << QString("");
}

GUI_TEST_CLASS_DEFINITION(test_4996) {
    // 1. Open "_common_data/fasta/fa1.fa".
    // 2. Open "Search in sequence" options panel tab. Select "RegExp" algorithm.
    // 3. Enter next regexp: (

    GTFileDialog::openFile(testDir + "_common_data/fasta/", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    GTUtilsOptionPanelSequenceView::setAlgorithm("Regular expression");

    auto textPattern = GTWidget::findWidget("textPattern");
    GTWidget::click(textPattern);
    GTKeyboardDriver::keyClick('(');

    // Expected state: the pattern enter field becomes red.

    auto editPatterns = GTWidget::findPlainTextEdit("textPattern");
    QString style0 = editPatterns->styleSheet();
    CHECK_SET_ERR(style0 == "background-color: " + GUIUtils::WARNING_COLOR.name() + ";", "unexpected styleSheet: " + style0);

    // Remove entered pattern, enter a valid pattern:
    //.

    auto textPattern1 = GTWidget::findWidget("textPattern");
    GTWidget::click(textPattern1);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick('.');

    QString style1 = editPatterns->styleSheet();
    CHECK_SET_ERR(style1 == "background-color: " + GUIUtils::OK_COLOR.name() + ";", "unexpected styleSheet: " + style1);
}

GUI_TEST_CLASS_DEFINITION(test_5000) {
    // Check that MSA object can be added to another MSA object via context menu.

    GTFileDialog::openFile(testDir + "_common_data/clustal/dna.fasta.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/clustal/collapse_mode_1.aln"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from file..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that sequences appended to the file.
    QStringList nameList1 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList1.size() == 18, "1. Invalid sequence count: " + QString::number(nameList1.size()));

    // Check insertion into the middle (after the first row).
    GTUtilsMsaEditor::selectRows(0, 0);
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/clustal/collapse_mode_1.aln"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from file..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList nameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList2.size() == 26, "2. Invalid sequence count: " + QString::number(nameList2.size()));
    CHECK_SET_ERR(nameList2[1] == "a_1", "Invalid sequence name: " + nameList2[1]);
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
