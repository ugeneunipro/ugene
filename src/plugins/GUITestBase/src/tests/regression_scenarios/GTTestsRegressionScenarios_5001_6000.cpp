/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License57
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

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/FontDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
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

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QRadioButton>
#include <QTableView>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>
#include <U2View/MSAEditorTreeViewer.h>
#include <U2View/MaEditorConsensusArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaEditorSelection.h>
#include <U2View/MaGraphOverview.h>

#include "GTTestsRegressionScenarios_5001_6000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
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
#include "GTUtilsOptionPanelMca.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsPcr.h"
#include "GTUtilsPrimerLibrary.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportAPRFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PredictSecondaryStructureDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportCoverageDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastLocalSearchDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/MakeBlastDbDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SnpEffDatabaseDialogFiller.h"
#include "runnables/ugene/plugins/orf_marker/OrfDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ImportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_5004) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/5004/short.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Show DNA Flexibility graph, expected state: no errors in log.
    GTLogTracer lt;
    auto sequenceWidget = GTWidget::findWidget("ADV_single_sequence_widget_0");
    auto graphAction = GTWidget::findWidget("GraphMenuAction", sequenceWidget, false);
    GTUtilsDialog::waitForDialog(new PopupChooser({"DNA Flexibility"}));
    GTWidget::click(graphAction);

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5012) {
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam2.sam");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam3.sam");

    GTUtilsWorkflowDesigner::click("Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/genbank/JQ040024.1.gb");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.fa");

    GTUtilsWorkflowDesigner::click("Call Variants");
    GTUtilsWorkflowDesigner::setParameter("Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5012_1) {
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::click("Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/genbank/JQ040024.1.gb");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.fa");

    GTUtilsWorkflowDesigner::click("Call Variants");
    GTUtilsWorkflowDesigner::setParameter("Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012_1.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_5012_2) {
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::createDataset();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/scerevisiae.bam2.sam");

    GTUtilsWorkflowDesigner::click("Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::click("Call Variants");
    GTUtilsWorkflowDesigner::setParameter("Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012_2.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_5018) {
    QString homePlaceholder = isOsWindows() ? "%UserProfile%" : "~";

    QString homePath = QDir::homePath();
    QString testFilePath = homePath + "/test_5018.fa";

    //    1. Ensure that there is no "test_5018.fa" file in the home dir.
    if (GTFile::check(testFilePath)) {
        QFile(testFilePath).remove();
        CHECK_SET_ERR(!GTFile::check(testFilePath), "File can't be removed");
    }

    //    2. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Call context menu on the sequence object in the Project View, select {Export/Import -> Export sequences...} item.
    //    4. Set output path to "~/test_5018.fa" for *nix and "%HOME_DIR%\test_5018.fa" for Windows. Accept the dialog.
    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Export sequences..."}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(homePlaceholder + "/test_5018.fa"));
    GTUtilsProjectTreeView::click("human_T1 (UCSC April 2002 chr7:115977709-117855134)", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: "test_5018.fa" appears in the home dir.
    CHECK_SET_ERR(GTFile::check(testFilePath), "File was not created");
    GTUtilsDialog::waitForDialog(new MessageBoxNoToAllOrNo());
    QFile(testFilePath).remove();
    GTUtilsDialog::checkNoActiveWaiters(10000);
}

GUI_TEST_CLASS_DEFINITION(test_5026) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Expand the "Mecopoda_elongata_Ishigaki_J" collapsed group.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Mecopoda_elongata__Ishigaki__J");

    // 4. Select an inner sequence in the collapsed group. Remove the sequence
    GTUtilsMSAEditorSequenceArea::removeSequence(QString("Mecopoda_elongata__Sumatra_"));

    // 5. Expected result: only the selected sequence is removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 1, "The number of sequences remained unchanged.");
    CHECK_SET_ERR(!modifiedNames.contains("Mecopoda_elongata__Sumatra_"), "Removed sequence is present in multiple alignment.");
    CHECK_SET_ERR(modifiedNames.contains("Mecopoda_elongata__Ishigaki__J"), "Sequence Mecopoda_elongata__Ishigaki__J is not present in multiple alignment.");
}

GUI_TEST_CLASS_DEFINITION(test_5027_1) {
    // 1. Open preferences and set memory limit per task 500000MB
    // 2. Open WD and compose next scheme "Read File URL(s)" -> "SnpEff annotation and filtration"
    // 3. Run schema.
    // Expected state : there is problem on dashboard "A problem occurred during allocating memory for running SnpEff."
    class MemorySetter : public CustomScenario {
    public:
        MemorySetter(int _memValue)
            : memValue(_memValue) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::Resources);

            auto memSpinBox = GTWidget::findSpinBox("memBox");
            GTSpinBox::setValue(memSpinBox, memValue, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

    private:
        int memValue;
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new MemorySetter(200)));  // 200mb
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("SnpEff");
    GTThread::waitForMainThread();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);  // close wizard

    GTUtilsWorkflowDesigner::click("Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click("Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(new SnpEffDatabaseDialogFiller("hg19"));
    GTUtilsWorkflowDesigner::setParameter("Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::findLabelByText("There is not enough memory to complete the SnpEff execution.", GTUtilsDashboard::getDashboard());
}

GUI_TEST_CLASS_DEFINITION(test_5027_2) {
    // 1. Open preferences and set memory limit per task 512MB
    // 2. Open WD and compose next scheme "Read File URL(s)" -> "SnpEff annotation and filtration"
    // 3. Run schema.
    // Expected state : there is problem on dashboard "There is not enough memory to complete the SnpEff execution."
    class MemorySetter : public CustomScenario {
    public:
        MemorySetter(int memValue)
            : memValue(memValue) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::Resources);

            auto memSpinBox = GTWidget::findSpinBox("memBox");
            GTSpinBox::setValue(memSpinBox, memValue, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

    private:
        int memValue;
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new MemorySetter(256)));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

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
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::findLabelByText("There is not enough memory to complete the SnpEff execution.", GTUtilsDashboard::getDashboard());
}

GUI_TEST_CLASS_DEFINITION(test_5029) {
    GTMenu::clickMainMenuItem({"Settings", "Plugins..."});
    auto tree = GTWidget::findTreeWidget("treeWidget");
    int numPlugins = tree->topLevelItemCount();
    CHECK_SET_ERR(numPlugins > 10, QString("Not all plugins were loaded. Loaded %1 plugins").arg(numPlugins));
}

GUI_TEST_CLASS_DEFINITION(test_5039) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set the consensus type to "Levitsky".
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    auto consensusCombo = GTWidget::findComboBox("consensusType");
    GTComboBox::selectItemByText(consensusCombo, "Levitsky");

    // 3. Add an additional sequence from file : "test/_common_data/fasta/amino_ext.fa".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/amino_ext.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Open the "Export consensus" OP tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    GTLogTracer lt;

    // 5. Press "Undo" button.
    GTUtilsMsaEditor::undo();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 6. Press "Redo" button.
    GTUtilsMsaEditor::redo();
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state : the tab is successfully updated. No error in log.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5052) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Close the opened sequence view.
    GTUtilsSequenceView::getActiveSequenceViewWindow();
    GTGlobals::FindOptions findOptions;
    findOptions.matchPolicy = Qt::MatchContains;
    GTUtilsMdi::closeWindow("NC_", findOptions);

    // 3. Click "murine.gb" on Start Page.
    GTWidget::click(GTWidget::findLabelByText("murine.gb").first());

    // Expected: The file is loaded, the view is opened.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded("murine.gb"), "The file is not loaded");
    QString title = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_5059) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    // 2. Select whole alignment.
    // 3. Press Delete.
    // Expected: notification about impossible operation popped.

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsNotifications::waitForNotification(true, "Impossible to delete whole alignment!");
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_5069) {
    //    1. Load workflow "_common_data/regression/5069/crash.uwl".
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/5069/crash.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Set "data/samples/Genbank/murine.gb" as input.
    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/murine.gb");

    //    3. Launch workflow.
    //    Expected state: UGENE doesn't crash.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Workflow has finished with problems");
}

GUI_TEST_CLASS_DEFINITION(test_5082) {
    GTLogTracer lt;
    // 1. Open "_common_data/clustal/big.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/big.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTWidget::click(GTUtilsMsaEditor::getShowOverviewButton());  // Close 'Overview' to make the test faster.

    // 2. Align it with MUSCLE.
    GTUtilsDialog::add(new PopupChooserByText({"Align", "Align with MUSCLE…"}));
    GTUtilsDialog::add(new MuscleDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Expected: Error notification appears with a correct human-readable error. There is an error in log with memory requirements.
    GTUtilsNotifications::waitForNotification(true, "There is not enough memory to align these sequences with MUSCLE.");
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(lt.hasMessage("Not enough resources for the task"), "No default error in log");
}

GUI_TEST_CLASS_DEFINITION(test_5090) {
    //    1. Open "_common_data/genbank/join_complement_ann.gb".
    //    Expected state: the file is successfully opened;
    //                    a warning appears. It contains next message: "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand."
    //                    there are two annotations: 'just_an_annotation' (40..50) and 'join_complement' (join(10..15,20..25)). // the second one should have another location after UGENE-3423 will be done

    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/genbank/join_complement_ann.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsNotifications::checkNotificationReportText("The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.");

    CHECK_SET_ERR(lt.hasError("The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand."), "Expected error not found");

    GTUtilsMdi::activateWindow("A_SEQ_1 [join_complement_ann.gb]");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    const QString simpleAnnRegion = GTUtilsAnnotationsTreeView::getAnnotationRegionString("just_an_annotation");
    CHECK_SET_ERR("40..50" == simpleAnnRegion, QString("An incorrect annotation region: expected '%1', got '%2'").arg("40..50").arg(simpleAnnRegion));
    const QString joinComplementAnnRegion = GTUtilsAnnotationsTreeView::getAnnotationRegionString("join_complement");
    CHECK_SET_ERR("join(10..15,20..25)" == joinComplementAnnRegion, QString("An incorrect annotation region: expected '%1', got '%2'").arg("join(10..15,20..25)").arg(simpleAnnRegion));
}

GUI_TEST_CLASS_DEFINITION(test_5110) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTTreeWidget::expand(GTUtilsAnnotationsTreeView::findItem("NC_001363 features [murine.gb]"));
    QTreeWidgetItem* cdsGroupItem = GTUtilsAnnotationsTreeView::findItem("CDS  (0, 4)");
    GTTreeWidget::expand(cdsGroupItem);

    QTreeWidgetItem* cdsItem = GTUtilsAnnotationsTreeView::findItem("CDS", cdsGroupItem);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("codon_start", cdsItem) == "1", "wrong qualifier value");

    //    4. Open the "Annotation highlighting" OP widget.
    GTWidget::click(GTWidget::findWidget("OP_ANNOT_HIGHLIGHT"));

    auto showAnnotations = GTWidget::findCheckBox("checkShowHideAnnots");
    GTCheckBox::setChecked(showAnnotations, false);
    GTCheckBox::setChecked(showAnnotations, true);

    QBrush expectedBrush = QApplication::palette().brush(QPalette::Active, QPalette::Foreground);
    QBrush actualBrush = cdsItem->foreground(1);
    CHECK_SET_ERR(expectedBrush == actualBrush, "wrong item color");
}

GUI_TEST_CLASS_DEFINITION(test_5128) {
    // 1. Open any 3D structure.
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Context menu: { Molecular Surface -> * }.
    // 3. Select any model.
    // Current state: crash
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Molecular Surface", "SAS"}));
    GTWidget::click(GTWidget::findWidget("1-1CF7"), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_5130) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto msaEditorView = GTWidget::findWidget("msa_editor_COI_0");
    auto msaWidget = qobject_cast<MsaEditorWgt*>(msaEditorView);
    MaEditorNameList* nameListWidget = msaWidget->getEditorNameList();
    MaEditorConsensusArea* consWidget = msaWidget->getConsensusArea();
    MaEditorSequenceArea* seqAreaWidget = msaWidget->getSequenceArea();

    QFont nameListFontBefore = nameListWidget->getFont(false);
    nameListFontBefore.setItalic(false);

    //    2. press "change font button" on toolbar
    GTUtilsDialog::waitForDialog(new FontDialogFiller());
    QAbstractButton* change_font = GTAction::button("Change Font");
    GTWidget::click(change_font);

    QFont nameListFontAfter = nameListWidget->getFont(false);
    nameListFontAfter.setItalic(false);
    QFont consFontAfter = consWidget->getDrawSettings().font;
    QFont seqAreaFontAfter = seqAreaWidget->getFont();

    CHECK_SET_ERR(nameListFontBefore != nameListFontAfter, "Expected fonts to be NOT equal");
    CHECK_SET_ERR(nameListFontAfter == consFontAfter && consFontAfter == seqAreaFontAfter,
                  "Expected fonts to be equal: NameList: " + nameListFontAfter.toString() +
                      ", Cons: " + consFontAfter.toString() + ", SeqArea: " + seqAreaFontAfter.toString());
}

GUI_TEST_CLASS_DEFINITION(test_5136) {
    // Open the data/samples/PDB/1CF7.PDB.
    // In context menu go to Molecular surface->SAS.
    //    Expected state: molecular surface calculated and showed. Program not crashed.
    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsDialog::waitForDialog(new PopupChooser({"Molecular Surface", "SAS"}));
    auto widget3d = GTWidget::findWidget("1-1CF7");
    GTWidget::click(widget3d, Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_5137) {
    // 1. Open document test/_common_data/clustal/big.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Add a big sequence.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/fasta/", "PF07724_full_family.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");

    // 3. Delete the original alignment and wait for the error in the log.
    GTUtilsNotifications::waitForNotification(true, "A problem occurred during adding sequences. The multiple alignment is no more available.");
    GTUtilsProjectTreeView::click("COI");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished(20000);
}

GUI_TEST_CLASS_DEFINITION(test_5149) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Copy a sequence which contains only gaps to the clipboard.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 0), QPoint(43, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Paste the clipboard data to the alignment.
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: nothing happens, the undo/redo stack hasn't been modified.
    QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button should be disabled");

    QAbstractButton* redo = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_5199) {
    //    1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Set focus to the first sequence.
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber());

    //    3. Click "Predict secondary structure" button on the toolbar;
    //    4. Select "PsiPred" algorithm.
    //    5. Click "OK" button.
    //    Expected state: UGENE doesn't crash, 4 results are found.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmComboBox", dialog), "PsiPred");
            GTUtilsDialog::waitForDialog(new LicenseAgreementDialogFiller());
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished();

            auto resultsTable = GTWidget::findTableWidget("resultsTable", dialog);
            const int resultsCount = resultsTable->rowCount();
            CHECK_SET_ERR(4 == resultsCount, QString("Unexpected results count: expected %1, got %2").arg(4).arg(resultsCount));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new PredictSecondaryStructureDialogFiller(new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Predict secondary structure");
}

GUI_TEST_CLASS_DEFINITION(test_5205) {
    // Check that there is no way to run "primer-search" algorithm with invalid primer settings.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsOptionPanelSequenceView::setForwardPrimer("AACTTG");
    GTUtilsOptionPanelSequenceView::setReversePrimer("CCCTGG");

    auto findButton = GTWidget::findPushButton("findProductButton");
    CHECK_SET_ERR(!findButton->isEnabled(), "Find product(s) must be disabled");

    GTUtilsOptionPanelSequenceView::setForwardPrimer("TTTGGATCCAGCATCACCATCACCATCACGATCAAATAGAAGCAATG");
    GTUtilsOptionPanelSequenceView::setReversePrimer("AAACCTAGGTACGTAGTGGTAGTGGTAGTGCTAGTTTATCTTCGTTAC");
    CHECK_SET_ERR(findButton->isEnabled(), "Find product(s) must be enabled");
}

GUI_TEST_CLASS_DEFINITION(test_5208) {
    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary();
    GTUtilsPrimerLibrary::clearLibrary();

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa"
    //    and accept the dialog.
    class ImportFromMultifasta : public CustomScenario {
        void run() override {
            ImportPrimersDialogFiller::addFile(testDir + "_common_data/fasta/random_primers.fa");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportPrimersDialogFiller(new ImportFromMultifasta));
    GTUtilsPrimerLibrary::clickButton(GTUtilsPrimerLibrary::Import);

    //    4. Check log.
    //    Expected state: the library contains four primers, log contains no errors.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5211) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select the first sequence.
    GTUtilsMsaEditor::clickSequenceName("Phaneroptera_falcata");

    //    3. Copy it to the clipboard.
    GTKeyboardUtils::copy();

    //    4. Press the next key sequence:
    //        Windows and Linux: Shift+Ins
    //        macOS: Meta+Y
    if (!isOsMac()) {
        GTKeyboardUtils::paste();
    } else {
        GTKeyboardDriver::keyClick('y', Qt::MetaModifier);
    }
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: a new sequence is added to the alignment. There are no new objects and documents in the Project View.
    int expectedSequencesCount = 19;
    int sequencesCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(expectedSequencesCount == sequencesCount,
                  QString("Incorrect count of sequences after the first insertion: expected %1, got %2")
                      .arg(expectedSequencesCount)
                      .arg(sequencesCount));

    const int expectedDocumentsCount = 2;
    int documentsCount = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait("", QModelIndex(), 2).size();
    CHECK_SET_ERR(expectedDocumentsCount == documentsCount,
                  QString("Incorrect count of items in the Project View after the first insertion: expected %1, got %2")
                      .arg(expectedDocumentsCount)
                      .arg(documentsCount));

    //    5. Press the next key sequence:
    //        Windows and Linux: Ctrl+V
    //        macOS: Cmd+V
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);  // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: one more new sequence is added to the alignment. There are no new objects and documents in the Project View.
    expectedSequencesCount = 20;
    sequencesCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(expectedSequencesCount == sequencesCount,
                  QString("Incorrect count of sequences after the second insertion: expected %1, got %2")
                      .arg(expectedSequencesCount)
                      .arg(sequencesCount));

    documentsCount = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait("", QModelIndex(), 2).size();
    CHECK_SET_ERR(expectedDocumentsCount == documentsCount,
                  QString("Incorrect count of items in the Project View after the second insertion: expected %1, got %2")
                      .arg(expectedDocumentsCount)
                      .arg(documentsCount));
}

GUI_TEST_CLASS_DEFINITION(test_5220) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::AddTree);

    QDir().mkdir(QFileInfo(sandBoxDir + "test_5220/COI.nwk").dir().absolutePath());
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_5220/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::click("COI.nwk");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI"));
    GTMouseDriver::doubleClick();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);

    QDir().mkdir(QFileInfo(sandBoxDir + "test_5220/COI1.nwk").dir().absolutePath());
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_5220/COI1.nwk", 0, 0, true));

    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    bool isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(GTUtilsOptionPanelMsa::PairwiseAlignment);
    CHECK_SET_ERR(!isTabOpened, "The 'PairwiseAlignment' tab is unexpectedly opened");
}

GUI_TEST_CLASS_DEFINITION(test_5227) {
    GTUtilsPcr::clearPcrDir();

    // 1. Open "samples/Genbank/CVU55762.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Set next parameters:
    //  the first primer : TTCTGGATTCA
    //  the first primer mismatches : 15
    //  the second primer : CGGGTAG
    //  the second primer mismatches : 12
    //  3' perfect match: 10
    //  Maximum product : 100 bp
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTCTGGATTCA");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "CGGGTAG");

    GTUtilsPcr::setMismatches(U2Strand::Direct, 15);
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 12);

    auto perfectSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget("perfectSpinBox"));
    GTSpinBox::setValue(perfectSpinBox, 10, GTGlobals::UseKeyBoard);

    auto productSizeSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget("productSizeSpinBox"));
    GTSpinBox::setValue(productSizeSpinBox, 100, GTGlobals::UseKeyBoard);

    // 4. Find products
    // Expected state: log shouldn't contain errors
    GTLogTracer lt;
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5231) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Run Smith-waterman search using:
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // pattern: "ATCGAT"; note that pattern length is 6.
            GTTextEdit::setText(GTWidget::findTextEdit("teditPattern", dialog), "K*KTPPVGGKLA*VTP");

            GTRadioButton::click("radioTranslation", dialog);

            // 2.1 Choose Classic algorithm
            auto comboRealization = GTWidget::findComboBox("comboRealization", dialog);
            const int swRealizationIndex = comboRealization->findText("Classic 2");
            GTComboBox::selectItemByIndex(comboRealization, swRealizationIndex);

            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);
            // 3. Open tab "Input and output"
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);

            // 4. Chose in the combobox "Multiple alignment"
            GTComboBox::selectItemByText(GTWidget::findComboBox("resultViewVariants", dialog), "Multiple alignment");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::doubleClickItem("P1_NC_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino");
}

GUI_TEST_CLASS_DEFINITION(test_5246) {
    // 1. Open file human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Show ORFs
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Show ORFs"}));
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidget* widget = GTUtilsAnnotationsTreeView::getTreeWidget();
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(widget->invisibleRootItem());
    CHECK_SET_ERR(839 == treeItems.size(), "Unexpected annotation count");

    // 3. Change amino translation
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTWidget::click(GTWidget::findWidget("AminoToolbarButton", GTWidget::findWidget("ADV_single_sequence_widget_0")));
    auto menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    GTMenu::clickMenuItemByName(menu, {"14. The Alternative Flatworm Mitochondrial Code"});
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: orfs recalculated
    treeItems = GTTreeWidget::getItems(widget->invisibleRootItem());
    CHECK_SET_ERR(2023 == treeItems.size(), "Unexpected annotation count");
}

GUI_TEST_CLASS_DEFINITION(test_5249) {
    // 1. Open file "_common_data/pdb/1atp.pdb"
    // Expected state: no crash and no errors in the log
    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/pdb/1atp.pdb");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5252) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open an additional view for the sequence.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Open In", "Open new view: Sequence View"}));
    GTUtilsProjectTreeView::click("murine.gb", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: there are two bookmarks: "murine [s] NC_001363" and "murine [s] NC_001363 2".
    GTUtilsBookmarksTreeView::findItem("NC_001363 [murine.gb]");
    GTUtilsBookmarksTreeView::findItem("NC_001363 [murine.gb] 2");

    //    3. Rename the annotation table object.
    GTUtilsProjectTreeView::rename("NC_001363 features", "test_5252");

    //    Expected state: bookmarks are not renamed.
    GTUtilsBookmarksTreeView::findItem("NC_001363 [murine.gb]");
    GTUtilsBookmarksTreeView::findItem("NC_001363 [murine.gb] 2");
}

GUI_TEST_CLASS_DEFINITION(test_5268) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");

    //    2. Create a custom color scheme for the alignment with aan ppropriate alphabet.
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator("test_5268", NewColorSchemeCreator::nucl));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsDialog::checkNoActiveWaiters(60000);

    //    3. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select the custom color scheme.
    GTUtilsOptionPanelMsa::setColorScheme("test_5268");
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance", "Colors", "Custom schemes", "test_5268"}, PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    5. Open {Settings -> Preferences -> Alignment Color Scheme}.
    //    6. Change color of the custom color scheme and click ok.
    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator("test_5268", NewColorSchemeCreator::nucl, NewColorSchemeCreator::Change));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsDialog::checkNoActiveWaiters(60000);

    //    Expected state: the settings dialog closed, new colors are applied for the opened MSA.
    const QString opColorScheme = GTUtilsOptionPanelMsa::getColorScheme();
    CHECK_SET_ERR(opColorScheme == "test_5268",
                  QString("An incorrect color scheme is set in option panel: expect '%1', got '%2'")
                      .arg("test_5268")
                      .arg(opColorScheme));

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance", "Colors", "Custom schemes", "test_5268"}, PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_5278) {
    // 1. Open file PBR322.gb from samples
    GTFileDialog::openFile(dataDir + "samples/Genbank", "PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Find next restriction sites "AaaI" and "AagI"
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"AaaI", "AagI"}));
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsNotifications::waitForNotification(false);
    // 3. Open report and be sure fragments sorted by length (longest first)
    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller());
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    auto textEdit = dynamic_cast<QTextEdit*>(GTWidget::findWidget("reportTextEdit", GTUtilsMdi::activeWindow()));
    CHECK_SET_ERR(textEdit->toPlainText().contains("1:    From AaaI (944) To AagI (24) - 3442 bp "), "Expected message is not found in the report text");
}

GUI_TEST_CLASS_DEFINITION(test_5295) {
    // Open "_common_data/pdb/Helix.pdb".
    GTFileDialog::openFile(testDir + "_common_data/pdb/Helix.pdb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //  Expected state: UGENE doesn't crash, the 3d structure is shown.
    int minimumExpectedColors = 10;
    auto biostructWidget = GTWidget::findWidget("1-");
    QImage initialImage = GTWidget::getImage(biostructWidget);
    QSet<QRgb> colorSet = GTWidget::countColors(initialImage, minimumExpectedColors);
    CHECK_SET_ERR(colorSet.size() >= minimumExpectedColors, "Ball-and-Stick image has too few colors");

    // Call a context menu, open "Render Style" submenu.
    // Expected state: "Ball-and-Stick" renderer is selected.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Render Style", "Ball-and-Stick"}, PopupChecker::CheckOptions(PopupChecker::IsChecked)));
    GTWidget::click(biostructWidget, Qt::RightButton);

    // Select "Model" renderer. Select "Space Fill".
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Render Style", "Space Fill"}));
    GTWidget::click(biostructWidget, Qt::RightButton);
    QImage spaceFillImage = GTWidget::getImage(biostructWidget);
    CHECK_SET_ERR(spaceFillImage != initialImage, "Space Fill image is the same as Ball-and-Stick!");

    // Select "Model" renderer. Select "Ball-and-stick" again.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Render Style", "Ball-and-Stick"}));
    GTWidget::click(biostructWidget, Qt::RightButton);

    //  Expected state: UGENE doesn't crash, the 3d structure is shown.
    QImage currentImage = GTWidget::getImage(biostructWidget);
    CHECK_SET_ERR(currentImage == initialImage, "Current image is not equal to initial");
}

GUI_TEST_CLASS_DEFINITION(test_5314) {
    // 1. Open "data/samples/Genbank/CVU55762.gb".
    // 2. Search any enzyme on the whole sequence.
    // 3. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(testDir + "_common_data/genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList defaultEnzymes = {"ClaI"};
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(defaultEnzymes));
    GTMenu::showContextMenu(GTWidget::findWidget("det_view_CVU55762"));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/abif/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5330) {
    // Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Enable collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: The MSA object is not marked as modified.
    GTUtilsProjectTreeView::itemModificationCheck(GTUtilsProjectTreeView::findIndex("ma2_gapped.aln"), false);
}

GUI_TEST_CLASS_DEFINITION(test_5334) {
    // Open "_common_data/clustal/amino_ext.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/amino_ext.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select any symbol 'A' in the alignment.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(19, 0), QPoint(19, 0));

    // Click Ctrl + C.
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // Click Ctrl + V.
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: msa alphabet is still AMINO.
    bool isAmino = GTUtilsMSAEditorSequenceArea::hasAminoAlphabet();
    CHECK_SET_ERR(isAmino, "Alignment has wrong alphabet type");
}

GUI_TEST_CLASS_DEFINITION(test_5335) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Find ORFs" button on the toolbar.
    class PartialSearchScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    3. Set region to 1..4. Accept the dialog.
            GTLineEdit::setText("end_edit_line", "4", dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new OrfDialogFiller(new PartialSearchScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find ORFs");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: an empty auto-annotation group is added to the auto-annotation table.
    QTreeWidgetItem* orfGroup = GTUtilsAnnotationsTreeView::findItem("orf  (0, 0)");

    //    4. Open the context menu on this group.
    //    Expected state:  there is no "Make auto-annotations persistent" menu item.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Make auto-annotations persistent"}, PopupChecker::NotExists));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(orfGroup);

    //    5. Click "Find ORFs" button on the toolbar.
    //    6. Accept the dialog.
    GTUtilsDialog::waitForDialog(new OrfDialogFiller());
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find ORFs");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the auto-annotation group is now contains some annotations.
    orfGroup = GTUtilsAnnotationsTreeView::findItem("orf  (0, 837)");

    //    7. Open the context menu on this group.
    //    Expected state: there is "Make auto-annotations persistent" menu item, it is enabled.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Make auto-annotations persistent"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(orfGroup);
}

GUI_TEST_CLASS_DEFINITION(test_5346) {
    // 1. Open WD
    // 2. Create the workflow: Read File URL(s) - FastQC Quality Control
    // 3. Set empty input file
    // Expected state: there is an error "The input file is empty"
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    QString emptyFile = sandBoxDir + "test_5346_empty";
    GTFile::create(emptyFile);
    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::addElement("Read File URL(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(emptyFile);

    WorkflowProcessItem* fastqc = GTUtilsWorkflowDesigner::addElement("FastQC Quality Control");
    GTUtilsWorkflowDesigner::connect(fileList, fastqc);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasError(QString("The input file '%1' is empty.").arg(QFileInfo(emptyFile).absoluteFilePath())), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_5352) {
    //    1. Open WD
    //    2. Open any sample (e.g. Align with MUSCLE)
    //    3. Remove some elements and set input data
    //    4. Run the workflow
    //    5. Click "Load dashboard workflow"
    //    Expected state: message box about workflow modification appears
    //    6. Click "Close without saving"
    //    Expected state: the launched workflow is loaded successfully, no errors

    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsTaskTreeView::waitTaskFinished();

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::getWorker("Read alignment");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::getWorker("Write alignment");

    GTUtilsWorkflowDesigner::click("Align with MUSCLE");
    GTUtilsWorkflowDesigner::removeItem("Align with MUSCLE");
    GTUtilsWorkflowDesigner::connect(read, write);

    GTUtilsWorkflowDesigner::click("Read alignment");
    GTUtilsWorkflowDesigner::addInputFile("Read alignment", dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Discard));

    QToolButton* loadSchemaButton = GTUtilsDashboard::findLoadSchemaButton();
    CHECK_SET_ERR(loadSchemaButton, "loadSchemaButton not found");
    GTWidget::click(loadSchemaButton);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5263) {
    // 1. Open _common_data/genbank/pBR322.gb
    // 2. Find the following restriction site: EcoRI
    // Expected state: it is located on the sequence junction point
    // 3. Remove the circular mark
    // Expected state: the restriction sites of sequence junction disappear
    // 4. Set back the circular mark
    // Expected state: 1 auto annotation appeared

    GTFileDialog::openFile(dataDir + "samples/Genbank/PBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller({"EcoRI"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString("EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));
    GTUtilsAnnotationsTreeView::findItem("EcoRI");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("SYNPBR322"));
    GTUtilsDialog::add(new PopupChooserByText({"Mark as circular"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("EcoRI", nullptr, {false}) == nullptr, "'EcoRI' item is found, but should not.");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("SYNPBR322"));
    GTUtilsDialog::add(new PopupChooserByText({"Mark as circular"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    region = GTUtilsAnnotationsTreeView::getAnnotationRegionString("EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));
    GTUtilsAnnotationsTreeView::findItem("EcoRI");
}

GUI_TEST_CLASS_DEFINITION(test_5356) {
    //    1. Open WD
    //    2. Create workflow: "Read FASTQ" --> "Cut Adapter" --> "FastQC"
    //       (open _common_data/regression/5356/cutadapter_and_trim.uwl)
    //    3. Set input data:
    //       reads - _common_data/regression/5356/reads.fastq
    //       adapter file -  _common_data/regression/5356/adapter.fa
    //    4. Run the workflow
    //    Expected state: no errors in the log (empty sequences were skipped by CutAdapter)

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/5356/cutadapt_and_trim.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addInputFile("Read FASTQ Files with Reads 1", testDir + "_common_data/regression/5356/reads.fastq");

    GTUtilsWorkflowDesigner::click("Cut Adapter");
    GTUtilsWorkflowDesigner::setParameter("FASTA file with 3' adapters", QDir(testDir + "_common_data/regression/5356/adapter.fa").absolutePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Output folder", "Custom", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Custom folder", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5360) {
    // 1. Open scheme _common_data / scenarios / _regression / 5360 / 5360.uwl
    //
    // 2. Set input fastq file located with path containing non ASCII symbols
    //
    // 3. Run workflow
    // Expected state : workflow runs without errors.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/scenarios/_regression/5360/5360.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::click("Read FASTQ Files with Reads");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + QString::fromUtf8("_common_data/scenarios/_regression/5360/папка/риды.fastq"), true);

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5363_2) {
    //    1. {Tools --> BLAST --> BLAST make database}
    //    2. Set murine.gb as input file
    //    3. Check nucleotide radiobutton
    //    4. Create database
    //    Expected state: database was successfully created
    //    5. Open murine.gb
    //    6. {Analyze --> Query with local BLAST}
    //    7. Select the created database and accept the dialog
    //    Expected state: blast annotations were found and there is an annotation with the region equal to 'hit-from' and 'hit-to' qualifier values

    MakeBlastDbDialogFiller::Parameters parametersDB;
    parametersDB.inputFilePath = dataDir + "/samples/Genbank/murine.gb";
    parametersDB.outputDirPath = QDir(sandBoxDir).absolutePath();
    GTUtilsDialog::waitForDialog(new MakeBlastDbDialogFiller(parametersDB));
    GTMenu::clickMainMenuItem({"Tools", "BLAST", "BLAST make database..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    BlastLocalSearchDialogFiller::Parameters parametersSearch;
    parametersSearch.runBlast = true;
    parametersSearch.dbPath = sandBoxDir + "/murine.nin";
    U2Region searchRegion = {500, 100};
    parametersSearch.searchRegion = searchRegion;

    GTUtilsDialog::waitForDialog(new BlastLocalSearchDialogFiller(parametersSearch));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Query with local BLAST..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> blastResultItems = GTUtilsAnnotationsTreeView::findItems("blast result");
    CHECK_SET_ERR(blastResultItems.length() > 1, "Expected multiple blast results");

    QString expectedLocationText = QString::number(searchRegion.startPos) + ".." + QString::number(searchRegion.endPos());
    QTreeWidgetItem* wholeRegionItem = nullptr;
    for (auto item : blastResultItems) {
        if (item->text(2).contains(expectedLocationText)) {
            wholeRegionItem = item;
            break;
        }
    }
    CHECK_SET_ERR(wholeRegionItem != nullptr, "Whole region result item not found");

    bool ok;
    int hitFrom = GTUtilsAnnotationsTreeView::getQualifierValue("hit-from", wholeRegionItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-to qualifier value");

    int hitTo = GTUtilsAnnotationsTreeView::getQualifierValue("hit-to", wholeRegionItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-from qualifier value");

    CHECK_SET_ERR(hitFrom == searchRegion.startPos && hitTo == searchRegion.endPos(),
                  QString("Unexpected hit-from/to qualifiers: [%1, %2]").arg(hitFrom).arg(hitTo));
}

GUI_TEST_CLASS_DEFINITION(test_5367) {
    //    1. Open "_common_data/bam/accepted_hits_with_gaps.bam"
    //    2. Export coverage in 'Per base' format
    //    Expected state: gaps are not considered "to cover, the result file is qual to "_common_data/bam/accepted_hits_with_gaps_coverage.txt"

    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "/test_5367.ugenedb"));
    GTFileDialog::openFile(testDir + "_common_data/bam/accepted_hits_with_gaps.bam");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    QList<ExportCoverageDialogFiller::Action> actions = {
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base"),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir(sandBoxDir).absolutePath() + "/test_5367_coverage.txt"),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, QVariant())};

    GTUtilsDialog::add(new PopupChooserByText({"Export coverage..."}));
    GTUtilsDialog::add(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters();

    CHECK_SET_ERR(GTFile::equals(
                      sandBoxDir + "/test_5367_coverage.txt",
                      testDir + "/_common_data/bam/accepted_hits_with_gaps_coverage.txt"),
                  "Exported coverage is wrong!");
}

GUI_TEST_CLASS_DEFINITION(test_5377) {
    //    1. Open file "_common_data/genbank/70Bp_new.gb".
    //    2. Search for restriction site HinFI.
    //    3. Digest into fragments, then reconstruct the original molecule.
    //    Expected state: the result sequence is equal to the original sequence. Fragments annotations have the same positions and lengths.
    GTFileDialog::openFile(testDir + "_common_data/genbank/70Bp_new.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"HinfI"}));
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller());
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion("Fragment 1", U2Region(36, 35)), "Fragment 1 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion("Fragment 2", U2Region(1, 24)), "Fragment 2 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion("Fragment 3", U2Region(28, 5)), "Fragment 3 is incorrect or not found");

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findWidget("takeAllButton"));

            auto tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget("molConstructWidget"));
            GTTreeWidget::click(GTTreeWidget::findItem(tree, "Blunt"));

            GTWidget::click(GTWidget::findWidget("downButton"));
            GTWidget::click(GTWidget::findWidget("downButton"));

            auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);
            GTTabWidget::clickTab(tabWidget, "Output");

            auto linEdit = GTWidget::findLineEdit("filePathEdit");
            GTLineEdit::setText(linEdit, QFileInfo(sandBoxDir + "test_5377").absoluteFilePath());

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Construct molecule..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsSequenceView::getSeqWidgetByNumber()->getSequenceLength() == 70, "The result length of the constructed molecule is wrong");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion("A sequence Fragment 1", U2Region(36, 35)), "Constructed molecule: Fragment 1 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion("A sequence Fragment 2", U2Region(1, 24)), "Constructed molecule: Fragment 2 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion("A sequence Fragment 3", U2Region(28, 5)), "Constructed molecule: Fragment 3 is incorrect or not found");
}

GUI_TEST_CLASS_DEFINITION(test_5371) {
    // 1. Open bam assembly with index with path containing non ASCII symbols
    // Expected state: assembly opened successfully

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "5371.bam.ugenedb"));
    auto ob = new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/5371/папка/", "асс ссембли.bam", GTFileDialogUtils::Open, GTGlobals::UseKey, GTFileDialogUtils::CopyPaste);
    GTUtilsDialog::waitForDialog(ob);
    ob->openFileDialog();

    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5382) {
    // 1. Open an alignment.
    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new ExportMsaImage(testDir + "_common_data/scenarios/sandbox/test_5382/test_5382.png"));

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5412) {
    //    1. Open "/_common_data/reads/wrong_order/align_bwa_mem.uwl"
    //    2. Set input data: e_coli_mess_1.fastq nd e_coli_mess_2.fastq (the folder from step 1)
    //    3. Reference: "/_common_data/e_coli/NC_008253.fa"
    //    4. Set requiered output parameters
    //    5. Set "Filter unpaired reads" to false
    //    6. Run workflow
    //    Expected state: error - BWA MEM tool exits with code 1
    //    7. Go back to the workflow and set the filter parameter back to true
    //    8. Run the workflow
    //    Expected state: there is a warning about filtered reads

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "/_common_data/reads/wrong_order/align_bwa_mem.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addInputFile("File List 1", testDir + "/_common_data/reads/wrong_order/e_coli_mess_1.fastq");
    GTUtilsWorkflowDesigner::addInputFile("File List 2", testDir + "/_common_data/reads/wrong_order/e_coli_mess_2.fastq");

    GTUtilsWorkflowDesigner::click("Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter("Output folder", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Output file name", "test_5412", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Reference genome", testDir + "/_common_data/e_coli/NC_008253.fa", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Filter unpaired reads", false, GTUtilsWorkflowDesigner::comboValue);

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasMessage("exited with code 1"), "No message about failed start of BWA MEM");

    GTToolbar::clickButtonByTooltipOnToolbar("mwtoolbar_activemdi", "Show workflow");

    GTUtilsWorkflowDesigner::click("Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter("Filter unpaired reads", true, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Output file name", "test_5412_1", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasMessage("5 read pairs were mapped, 6 reads without a pair from files"), "No message about filtered reads");
}

GUI_TEST_CLASS_DEFINITION(test_5417) {
    //      1. Open "data/samples/Genbank/murine.gb".
    //      2. Open "data/samples/Genbank/srs.gb".
    //      3. Build doplot with theese files and try to save it.
    //      Expected state: warning message ox appeared
    GTUtilsDialog::waitForDialog(new DotPlotFiller());
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(dataDir + "samples/Genbank/sars.gb", dataDir + "samples/Genbank/murine.gb"));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});

    GTLogTracer lt;
    GTUtilsDialog::add(new PopupChooser({"Dotplot", "Save/Load", "Save"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTMenu::showContextMenu(GTWidget::findWidget("dotplot widget"));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5421) {
    // 1. Build doplot for "data/samples/Genbank/murine.gb" and "data/samples/Genbank/sars.gb".
    // 2. Remove sars.gb from project
    // Expected state: Save dialog appeared.
    GTUtilsDialog::waitForDialog(new DotPlotFiller());
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(dataDir + "samples/Genbank/sars.gb", dataDir + "samples/Genbank/murine.gb"));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No, "Save dot-plot data before closing?"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("sars.gb"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_5425) {
    // Open de novo assembly dialog
    // Fill it and run
    // Expected result: no errors
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    class TrimmomaticScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 3. Add two "ILLUMINACLIP" steps with adapters with similar filenames located in different directories to Trimmomatic worker.
            GTWidget::click(GTWidget::findWidget("buttonAdd", dialog));
            auto menu = GTWidget::findMenuWidget("stepsMenu", dialog);
            GTMenu::clickMenuItemByName(menu, {"ILLUMINACLIP"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/6118/TruSeq3-SE.fa"));
            GTWidget::click(GTWidget::findWidget("tbBrowse", dialog));

            GTWidget::click(GTWidget::findWidget("buttonAdd", dialog));
            menu = GTWidget::findMenuWidget("stepsMenu", dialog);
            GTMenu::clickMenuItemByName(menu, {"ILLUMINACLIP"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            auto settingsStep1Widget = GTWidget::findWidget("TrimmomaticStepSettingsWidget_step_1", dialog);
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/6118/deeperDir/TruSeq3-SE.fa"));
            GTWidget::click(GTWidget::findWidget("tbBrowse", settingsStep1Widget));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    class IlluminaAssemblyWizardScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* wizard = GTWidget::getActiveModalWidget();
            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq").absoluteFilePath()));

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            // GTUtilsWizard::clickButton

            GTUtilsDialog::waitForDialog(new DefaultDialogFiller("TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));

            GTWidget::click(GTWidget::findWidget("trimmomaticPropertyToolButton", wizard));

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure De Novo Assembly Workflow", {"Illumina SE reads"}));
    GTUtilsDialog::add(new WizardFiller("Illumina SE Reads De Novo Assembly Wizard", new IlluminaAssemblyWizardScenario()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Reads de novo assembly (with SPAdes)..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    // Expected: The dashboard appears
    GTUtilsDashboard::getDashboard();
    // There should be no notifications.
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Unexpected notification");
}
GUI_TEST_CLASS_DEFINITION(test_5425_1) {
    // Open de novo assembly dialog
    // Fill it and run
    // Expected result: no errors

    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 3. Add two "ILLUMINACLIP" steps with adapters with similar filenames located in different directories to Trimmomatic worker.
            GTWidget::click(GTWidget::findWidget("buttonAdd"));

            auto menu = GTWidget::findMenuWidget("stepsMenu");
            GTMenu::clickMenuItemByName(menu, {"ILLUMINACLIP"});

            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/6118/TruSeq3-SE.fa"));
            GTWidget::click(GTWidget::findWidget("tbBrowse", dialog));

            GTWidget::click(GTWidget::findWidget("buttonAdd"));

            menu = GTWidget::findMenuWidget("stepsMenu");
            GTMenu::clickMenuItemByName(menu, {"ILLUMINACLIP"});

            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/6118/deeperDir/TruSeq3-SE.fa"));
            auto settingsStep1Widget = GTWidget::findWidget("TrimmomaticStepSettingsWidget_step_1", dialog);
            GTWidget::click(GTWidget::findWidget("tbBrowse", settingsStep1Widget));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    class custom : public CustomScenario {
    public:
        void run() override {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
            GTUtilsWizard::setInputFiles({{QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq").absoluteFilePath()}});

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsDialog::waitForDialog(new DefaultDialogFiller("TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new Scenario()));

            GTWidget::click(GTWidget::findWidget("trimmomaticPropertyToolButton"));

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure De Novo Assembly Workflow", {"Illumina PE reads"}));
    GTUtilsDialog::add(new WizardFiller("Illumina PE Reads De Novo Assembly Wizard", new custom()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Reads de novo assembly (with SPAdes)..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    // Expected: The dashboard appears
    GTUtilsDashboard::getDashboard();
    // There should be no notifications.
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Unexpected notification");
}

GUI_TEST_CLASS_DEFINITION(test_5425_2) {
    // Open de novo assembly dialog
    // Fill it and run
    // Expected result: no errors

    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class custom : public CustomScenario {
    public:
        void run() override {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq").absoluteFilePath()));
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/scaffolds_001.fasta").absoluteFilePath()));
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure De Novo Assembly Workflow", {"Illumina PE and Nanopore reads"}));
    GTUtilsDialog::add(new WizardFiller("Illumina PE Reads De Novo Assembly Wizard", new custom()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Reads de novo assembly (with SPAdes)..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    // Expected: The dashboard appears
    GTUtilsDashboard::getDashboard();
    // There should be no notifications.
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Unexpected notification");
}

GUI_TEST_CLASS_DEFINITION(test_5431) {
    // Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Remove all columns except the first one.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Toggle collapse by sequence content mode: there will be 2 collapsed groups.
    GTUtilsMsaEditor::toggleCollapsingMode();
    int viewRowCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(viewRowCount == 2, "Wrong visible row count. Expected: 2, got: " + QString::number(viewRowCount));

    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Tettigonia_viridissima"),
                  "1 Tettigonia_viridissima is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_discolor"),
                  "2 Conocephalus_discolor is not collapsed");

    GTUtilsMSAEditorSequenceArea::removeSequence("Phaneroptera_falcata");

    // Expected state: the first group is removed, the second one is collapsed, so we have only 1 visible row.
    viewRowCount = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(viewRowCount == 1, "Wrong visiable row count. Expected: 1, got: " + QString::number(viewRowCount));
}

GUI_TEST_CLASS_DEFINITION(test_5447_1) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open the context menu on the "NC_001363 features" object in the project view.
    //    3. Select "Export/Import" -> "Export annotations..." menu item.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state: an "Export Annotations" dialog opens, "GenBank" format is selected, there is an "Add to project" checkbox, it is enabled and checked.
            GTComboBox::checkCurrentValue(GTWidget::findComboBox("formatsBox", dialog), "GenBank");

            auto addToProjectCheck = GTWidget::findCheckBox("addToProjectCheck", dialog);
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(addToProjectCheck->isEnabled(), "addToProjectCheck is not enabled");
            CHECK_SET_ERR(addToProjectCheck->isChecked(), "addToProjectCheck is not checked by default");

            //    4. Set a valid result file path, accept the dialog.
            GTLineEdit::setText(GTWidget::findLineEdit("fileNameEdit", dialog), sandBoxDir + "test_5447_1.gb");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Export annotations..."}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(new Scenario()));
    GTUtilsProjectTreeView::callContextMenu("NC_001363 features", "murine.gb");

    //    Expected state: the annotations were exported, a new document with an annotations table object was added to the project.
    const qint64 fileSize = GTFile::getSize(sandBoxDir + "test_5447_1.gb");
    CHECK_SET_ERR(0 != fileSize, "Result file is empty");

    const QModelIndex annotationsTableObjectIndex = GTUtilsProjectTreeView::findIndex("NC_001363 features", GTUtilsProjectTreeView::findIndex("test_5447_1.gb"));
    CHECK_SET_ERR(annotationsTableObjectIndex.isValid(), "Annotation object not found");

    //    5. Add the object to the "murine.gb" sequence.
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(annotationsTableObjectIndex, GTUtilsSequenceView::getSeqWidgetByNumber());

    //    Expected state: all annotations are doubled.
    const QStringList oldGroups = GTUtilsAnnotationsTreeView::getGroupNames("NC_001363 features [murine.gb]");
    const QStringList newGroups = GTUtilsAnnotationsTreeView::getGroupNames("NC_001363 features [test_5447_1.gb]");
    bool oldCommentGroupExists = false;
    foreach (const QString& oldGroup, oldGroups) {
        if (oldGroup == "comment  (0, 1)") {
            oldCommentGroupExists = true;
            continue;
        }
        CHECK_SET_ERR(newGroups.contains(oldGroup), QString("'%1' group from the original file is not present in a new file").arg(oldGroup));
    }
    CHECK_SET_ERR(oldGroups.size() - (oldCommentGroupExists ? 1 : 0) == newGroups.size(),
                  QString("Groups count from the original file is not equal to a groups count in a new file (%1 and %2").arg(oldGroups.size()).arg(newGroups.size()));
}

GUI_TEST_CLASS_DEFINITION(test_5447_2) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open the context menu on the "NC_001363 features" object in the project view.
    //    3. Select "Export/Import" -> "Export annotations..." menu item.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state: an "Export Annotations" dialog opens, "GenBank" format is selected, there is an "Add to project" checkbox, it is enabled and checked.
            GTComboBox::checkCurrentValue(GTWidget::findComboBox("formatsBox", dialog), "GenBank");

            auto addToProjectCheck = GTWidget::findCheckBox("addToProjectCheck", dialog);
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(addToProjectCheck->isEnabled(), "addToProjectCheck is not enabled");
            CHECK_SET_ERR(addToProjectCheck->isChecked(), "addToProjectCheck is not checked by default");

            //    4. Select "CSV" format.
            //    Expected state: a "CSV" format is selected, the "Add to project" checkbox is disabled.
            GTComboBox::selectItemByText(GTWidget::findComboBox("formatsBox", dialog), "CSV");
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(!addToProjectCheck->isEnabled(), "addToProjectCheck is unexpectedly enabled");

            //    5. Set a valid result file path, accept the dialog.
            GTLineEdit::setText(GTWidget::findLineEdit("fileNameEdit", dialog), sandBoxDir + "test_5447_2.csv");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Export annotations..."}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(new Scenario()));
    GTUtilsProjectTreeView::callContextMenu("NC_001363 features", "murine.gb");

    //    Expected state: the annotations were exported, there are no new documents in the project.
    const qint64 fileSize = GTFile::getSize(sandBoxDir + "test_5447_2.csv");
    CHECK_SET_ERR(0 != fileSize, "Result file is empty");

    const bool newDocumentExists = GTUtilsProjectTreeView::checkItem("test_5447_2.csv", {false});
    CHECK_SET_ERR(!newDocumentExists, "New document unexpectedly exists");
}

GUI_TEST_CLASS_DEFINITION(test_5447_3) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open the context menu on the "NC_001363 features" object in the project view.
    //    3. Select "Export/Import" -> "Export annotations..." menu item.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state: an "Export Annotations" dialog opens, "GenBank" format is selected, there is an "Add to project" checkbox, it is enabled and checked.
            GTComboBox::checkCurrentValue(GTWidget::findComboBox("formatsBox", dialog), "GenBank");

            auto addToProjectCheck = GTWidget::findCheckBox("addToProjectCheck", dialog);
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(addToProjectCheck->isEnabled(), "addToProjectCheck is not enabled");
            CHECK_SET_ERR(addToProjectCheck->isChecked(), "addToProjectCheck is not checked by default");

            //    4. Select each format.
            //    Expected state: the "Add to project" checkbox becomes disabled only for CSV format.
            const QStringList formats = GTComboBox::getValues(GTWidget::findComboBox("formatsBox", dialog));
            foreach (const QString& format, formats) {
                GTComboBox::selectItemByText(GTWidget::findComboBox("formatsBox", dialog), format);
                CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
                CHECK_SET_ERR(addToProjectCheck->isEnabled() != (format == "CSV"), QString("addToProjectCheck is unexpectedly enabled for format '%1'").arg(format));
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Export annotations..."}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(new Scenario()));
    GTUtilsProjectTreeView::callContextMenu("NC_001363 features", "murine.gb");
}

GUI_TEST_CLASS_DEFINITION(test_5469) {
    // 1. Open two different GenBank sequences in one Sequence view.
    // 2. Select two different annotations (one from the first sequence, and one from the second sequence) using the "Ctrl" keyboard button.
    // Extected state: there is no crash
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Add to view", "Add to view: NC_001363 [murine.gb]"}));
    GTUtilsProjectTreeView::click("NC_004718", Qt::RightButton);

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 2);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::clickAnnotationDet("5'UTR", 1, 1);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAllSelectedItems().size() == 2, QString("Wrong number of selected annotations expect %1, got %2").arg("2").arg(GTUtilsAnnotationsTreeView::getAllSelectedItems().size()));
}

GUI_TEST_CLASS_DEFINITION(test_5492) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMca::openTab(GTUtilsOptionPanelMca::General);

    // 2. Select last symbol of the read and insert some gaps, until reference will increase for a few symbols
    MultipleAlignmentRowData* row = GTUtilsMcaEditor::getMcaRow(0);
    int end = row->getCoreStart() + row->getCoreLength() - 1;
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(end, 0));

    int i = 15;
    while (i != 0) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        i--;
    }

    // 4. Select the last symbol again, press "Insert character" and insert gap
    row = GTUtilsMcaEditor::getMcaRow(0);
    end = row->getCoreStart() + row->getCoreLength() - 1;
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(end, 0));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace character/gap"});
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Expected : all gaps since a place when you started to insert, will turn into trailing
    row = GTUtilsMcaEditor::getMcaRow(0);
    int newRowLength = row->getCoreStart() + row->getCoreLength() - 1;
    CHECK_SET_ERR(newRowLength < end, "Incorrect length");

    int refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    // 5. Press "Remove all coloumns of gaps "
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Remove all columns of gaps"});

    // Expected: Reference will be trimmed
    int newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    CHECK_SET_ERR(newRefLength < refLength, QString("Expected: New ref length is less then old ref length, current: new = %1, old = %2").arg(QString::number(newRefLength)).arg(QString::number(refLength)));

    // 6. Press "undo"
    GTUtilsMcaEditor::undo();

    // Expected: reference will be restored with gaps
    newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    CHECK_SET_ERR(newRefLength == refLength, QString("Expected: New ref length is equal old ref length, current: new = %1, old = %2").arg(QString::number(newRefLength)).arg(QString::number(refLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5495) {
    // 1) Open samples/FASTA/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) Select 100..10 region of the sequence
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto startEdit = GTWidget::findLineEdit("startEdit", dialog);
            auto endEdit = GTWidget::findLineEdit("endEdit", dialog);

            GTLineEdit::setText(startEdit, QString::number(321));
            GTLineEdit::setText(endEdit, QString::number(123));

            auto box = GTWidget::findDialogButtonBox("buttonBox");
            QPushButton* goButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(goButton != nullptr, "Go button not found");
            CHECK_SET_ERR(!goButton->isEnabled(), "Go button is enabled");

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

GUI_TEST_CLASS_DEFINITION(test_5499) {
    //    1. Open txt file (_common_data/text/text.txt).
    //    Expected state: "Select correct document format" dialog appears
    //    2. Select "Choose format manually" with the default ABIF format.
    //    3. Click Ok.
    GTLogTracer lt;

    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/text/text.txt"));
    GTUtilsDialog::add(new DocumentFormatSelectorDialogFiller("ABIF"));
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTMenu::clickMainMenuItem({"File", "Open as..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the message about "not ABIF format" appears, UGENE doesn't crash.
    CHECK_SET_ERR(lt.hasError("Not a valid ABIF file"), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_5517) {
    // 1. Open sequence
    // 2. Open build dotplot dialog
    // 3. Check both checkboxes direct and invert repeats search
    // Expected state: UGENE not crashed
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new DotPlotFiller(100, 0, true));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Build dotplot..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_5520_2) {
    GTFileDialog::openFile(dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "/_common_data/cmdline/external-tool-support/blastall/sars_middle.nhr"));
            GTWidget::click(GTWidget::findWidget("selectDatabasePushButton"));

            auto rbNewTable = GTWidget::findRadioButton("rbCreateNewTable");
            GTRadioButton::click(rbNewTable);

            auto leTablePath = GTWidget::findLineEdit("leNewTablePath");
            GTLineEdit::setText(leTablePath, sandBoxDir + "/test_5520_2.gb");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new BlastLocalSearchDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Query with local BLAST..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_5550) {
    GTFileDialog::openFile(testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there are next values on the statusbar: "Ln - / 2  Col - / 4  Pos - / -".
    QString rowNumberString = GTMSAEditorStatusWidget::getRowNumberString();
    QString rowsCountString = GTMSAEditorStatusWidget::getRowsCountString();
    QString columnNumberString = GTMSAEditorStatusWidget::getColumnNumberString();
    QString columnsCountString = GTMSAEditorStatusWidget::getColumnsCountString();
    QString sequenceUngappedPositionString = GTMSAEditorStatusWidget::getSequenceUngappedPositionString();
    QString sequenceUngappedLengthString = GTMSAEditorStatusWidget::getSequenceUngappedLengthString();

    CHECK_SET_ERR(rowNumberString == "-", QString("An incorrect row number label: expected '%1', got '%2'").arg("-").arg(rowNumberString));
    CHECK_SET_ERR(rowsCountString == "2", QString("An incorrect rows count label: expected '%1', got '%2'").arg("-").arg(rowsCountString));
    CHECK_SET_ERR(columnNumberString == "-", QString("An incorrect column number label: expected '%1', got '%2'").arg("-").arg(columnNumberString));
    CHECK_SET_ERR(columnsCountString == "4", QString("An incorrect columns count label: expected '%1', got '%2'").arg("-").arg(columnsCountString));
    CHECK_SET_ERR(sequenceUngappedPositionString == "-", QString("An incorrect sequence ungapped position label: expected '%1', got '%2'").arg("-").arg(sequenceUngappedPositionString));
    CHECK_SET_ERR(sequenceUngappedLengthString == "-", QString("An incorrect sequence ungapped length label: expected '%1', got '%2'").arg("-").arg(sequenceUngappedLengthString));
}

GUI_TEST_CLASS_DEFINITION(test_5562_1) {
    // 1. Open File "\samples\CLUSTALW\HIV-1.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "Statistics" Options Panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);

    // 3. Click ">" button to set Reference sequence
    GTUtilsOptionPanelMsa::addReference("sf170");

    // 4. Click check box "Show distance coloumn"
    // GTWidget::findComboBox("showDistancesColumnCheck");
    GTCheckBox::setChecked("showDistancesColumnCheck", true);

    // 5. Set combo box value "Hamming dissimilarity"
    GTComboBox::selectItemByText(GTWidget::findComboBox("algoComboBox"), "Hamming dissimilarity");

    // 6. Set radio button value "Percents"
    GTRadioButton::click(GTWidget::findRadioButton("percentsButton"));

    // 7. Click check box "Exclude gaps"
    GTCheckBox::setChecked("excludeGapsCheckBox", true);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Percents near: "ug46" is 6%,
    //                                "primer_ed5" is 0%
    //                                "primer_es7" is 1%
    QString val = GTUtilsMSAEditorSequenceArea::getSimilarityValue(8);
    CHECK_SET_ERR("6%" == val, QString("incorrect similarity: expected %1, got %2").arg("6%").arg(val));
    val = GTUtilsMSAEditorSequenceArea::getSimilarityValue(19);
    CHECK_SET_ERR("0%" == val, QString("incorrect similarity: expected %1, got %2").arg("0%").arg(val));
    val = GTUtilsMSAEditorSequenceArea::getSimilarityValue(21);
    CHECK_SET_ERR("1%" == val, QString("incorrect similarity: expected %1, got %2").arg("1%").arg(val));
}

GUI_TEST_CLASS_DEFINITION(test_5562_2) {
    // 1. Open File "\samples\CLUSTALW\HIV-1.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open context menu in sequence area
    // 3. Click "Statistick->Generate Distance Matrix"
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 4. Set combo box value "Hamming dissimilarity"
            GTComboBox::selectItemByText(GTWidget::findComboBox("algoCombo", dialog), "Hamming dissimilarity");
            // 5. Set radio button value "Percents"
            GTRadioButton::click(GTWidget::findRadioButton("percentsRB", dialog));
            // 6. Click check box "Exclude gaps"
            GTCheckBox::setChecked("checkBox", true, dialog);
            // 7. Click check box "Save profile to file"
            GTGroupBox::setChecked("saveBox", dialog);
            // 8. Set radio button value "Hypertext"
            // 9. Set any valid file name
            GTLineEdit::setText(GTWidget::findLineEdit("fileEdit", dialog), sandBoxDir + "5562_2_HTML.html");
            // 10. Accept the dialog.
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::add(new PopupChooserByText({"Statistics", "Generate distance matrix..."}));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(new Scenario));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : row "ug46", coloumn "sf170" value 26 % ,
    //                  row "sf170", coloumn "ug46" value 6 % ,
    //                  row "primer_ed31", coloumn "sf170" value 7 %
    //                  row "sf170", coloumn "primer_ed31" value 0 %
    QByteArray file = GTFile::readAll(sandBoxDir + "5562_2_HTML.html");
    QByteArray find = "ug46</td><td bgcolor=#60ff00>26%</td><td bgcolor=#ff9c00>23%";
    bool check = file.contains(find);
    CHECK_SET_ERR(check, QString("incorrect similarity"));
    find = "21%</td><td bgcolor=#ff5555>6%</td><td bgcolor=#ff9c00>19%";
    file.contains(find);
    CHECK_SET_ERR(check, QString("incorrect similarity"));
    find = "primer_ed31< / td><td bgcolor = #ff5555>7 % < / td><td bgcolor = #ff5555>7 %";
    file.contains(find);
    CHECK_SET_ERR(check, QString("incorrect similarity"));
    find = "0%</td><td bgcolor=#ff5555>0%</td><td bgcolor=#ff5555>1%";
    file.contains(find);
    CHECK_SET_ERR(check, QString("incorrect similarity"));
}

GUI_TEST_CLASS_DEFINITION(test_5562_3) {
    // 1. Open File "\samples\CLUSTALW\HIV-1.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open context menu in sequence area
    // 3. Click "Statistick->Generate Distance Matrix"
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 4. Set combo box value "Hamming dissimilarity"
            GTComboBox::selectItemByText(GTWidget::findComboBox("algoCombo", dialog), "Hamming dissimilarity");
            // 5. Set radio button value "Percents"
            GTRadioButton::click(GTWidget::findRadioButton("percentsRB", dialog));
            // 6. Click check box "Exclude gaps"
            GTCheckBox::setChecked("checkBox", true, dialog);
            // 7. Click check box "Save profile to file"
            GTGroupBox::setChecked("saveBox", dialog);
            // 8. Set radio button value "Comma Separated"
            GTRadioButton::click(GTWidget::findRadioButton("csvRB", dialog));
            // 9. Set any valid file name
            GTLineEdit::setText(GTWidget::findLineEdit("fileEdit", dialog), sandBoxDir + "5562_3_CSV.csv");
            // 10. Accept the dialog.
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::add(new PopupChooserByText({"Statistics", "Generate distance matrix..."}));
    GTUtilsDialog::add(new DistanceMatrixDialogFiller(new Scenario));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : the file should look like a sample file "_common_data/scenarios/_regression/5562/5562.csv"
    bool check = GTFile::equals(testDir + "_common_data/scenarios/_regression/5562/5562.csv", sandBoxDir + "5562_3_CSV.csv");
    CHECK_SET_ERR(check, QString("files are not equal"));
}

GUI_TEST_CLASS_DEFINITION(test_5588) {
    // 1. Open File "/samples/CLUSTALW/HIV-1.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Select a 6th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(5);
    // 3. Press the Shift key
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    // 4. Select a 15th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(14);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    // Expected state : All columns between 6th and 15th clicks are selected
    QRect rect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(rect == QRect(QPoint(5, 0), QPoint(14, 24)), QString("Incorrect selected area, %1, %2, %3, %4").arg(rect.topLeft().x()).arg(rect.topLeft().y()).arg(rect.bottomRight().x()).arg(rect.bottomRight().y()));

    // 5. Select a 30th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(29);
    // 6. Press the Shift key
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    // 7. Select a 12th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(11);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    // Expected state : All columns between 12th and 30th clicks are selected
    rect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(rect == QRect(QPoint(11, 0), QPoint(29, 24)), QString("Incorrect selected area, %1, %2, %3, %4").arg(rect.topLeft().x()).arg(rect.topLeft().y()).arg(rect.bottomRight().x()).arg(rect.bottomRight().y()));
}

GUI_TEST_CLASS_DEFINITION(test_5594_1) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Select reference pos 15
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(15);

    // 6. Press reference pos 35 with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(35);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: selected length = 20
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection();
    CHECK_SET_ERR(reg.length == 21, QString("Unexpexter selected length, expected: 20, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5594_2) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Push "Show/Hide Chromatograms" button in the main menu
    bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown("SZYD_Cas9_5B70");
    if (isChromatogramShown) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "chromatograms"));
    }

    // 6. Select read "SZYD_Cas9_CR51"
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_CR51");

    // 7. Select read "SZYD_Cas9_CR61" with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_CR61");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: selected length = 8
    U2Region reg = GTUtilsMcaEditorSequenceArea::getSelectedRowsNum();
    CHECK_SET_ERR(reg.length == 8, QString("Unexpexter selected length, expected: 8, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5594_3) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Select reference pos 15
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(15);

    // 6. Press right 5 times with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: selected length = 20
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection();
    CHECK_SET_ERR(reg.length == 6, QString("Unexpexter selected length, expected: 6, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5594_4) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Push "Show/Hide Chromatograms" button in the main menu
    bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown("SZYD_Cas9_5B70");
    if (isChromatogramShown) {
        GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "chromatograms"));
    }

    // 6. Select read "SZYD_Cas9_CR51"
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_CR51");

    // 7. Prss down 5 times with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: selected length = 6
    U2Region reg = GTUtilsMcaEditorSequenceArea::getSelectedRowsNum();
    CHECK_SET_ERR(reg.length == 6, QString("Unexpexter selected length, expected: 6, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5604) {
    // 1. Open Workflow designer
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Open scheme
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/scenarios/_regression/5604/scheme.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Set up input data
    GTUtilsWorkflowDesigner::click("Read FASTQ Files with Reads");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/reads/e_coli_1000.fq", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/reads/e_coli_1000_1.fq", true);

    GTUtilsWorkflowDesigner::click("Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter("Reference genome", testDir + "_common_data/fasta/human_T1_cutted.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsDashboard::getOutputFiles().size() == 1, "Wrong quantaty of output files");
}

GUI_TEST_CLASS_DEFINITION(test_5622) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference_gapped.gb (reference with gaps);
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference_gapped.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: all gaps columns was removed
    qint64 refLengthBeforeGapsRemove = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Remove all columns of gaps"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
    qint64 refLengthAfterGapsRemove = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    CHECK_SET_ERR(refLengthBeforeGapsRemove == refLengthAfterGapsRemove, QString("Equals befor adn after gaps removing not equal, length before: %1, length after: %2").arg(QString::number(refLengthBeforeGapsRemove)).arg(QString::number(refLengthAfterGapsRemove)));
}

GUI_TEST_CLASS_DEFINITION(test_5636) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click Align sequences to alignment->Align sequence to profile with MUSCLE...
    // Select "\samples\CLUSTALW\COI.aln"
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/COI.aln"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("Align sequences to alignment with MUSCLE");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: 18 sequences are added to the msa.
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount() == 36, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_5637) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);

    // Expected: row length must be equal or lesser then reference length
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    MultipleAlignmentRowData* row = GTUtilsMcaEditor::getMcaRow(0);
    qint64 rowLength = row->getRowLengthWithoutTrailing();
    CHECK_SET_ERR(rowLength <= refLength, QString("Expected: row length must be equal or lesser then reference length, current: row lenght = %1, reference length = %2").arg(QString::number(rowLength)).arg(QString::number(refLength)));

    // 2. Select a char in the first row
    QPoint p(5500, 0);
    GTUtilsMcaEditorSequenceArea::clickToPosition(p);

    // 3. insert 6 gaps
    int i = 6;
    while (i != 0) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        i--;
    }

    // Expected: row length must be equal or lesser then reference length
    refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    row = GTUtilsMcaEditor::getMcaRow(1);
    rowLength = row->getRowLengthWithoutTrailing();
    CHECK_SET_ERR(rowLength <= refLength, QString("Expected: row length must be equal or lesser then reference length, current: row lenght = %1, reference length = %2").arg(QString::number(rowLength)).arg(QString::number(refLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5638) {
    // 1. Open File "\samples\CLUSTALW\COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click to position (30, 10)
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(30, 10));

    // 3. Press Ctrl and drag and drop selection to the right for a few symbols
    QList<QVector<U2MsaGap>> startGapModel = GTUtilsMsaEditor::getEditor()->getMaObject()->getGapModel();

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::press();
    QPoint curPos = GTMouseDriver::getMousePosition();
    QPoint moveMouseTo(curPos.x() + 200, curPos.y());
    GTMouseDriver::moveTo(moveMouseTo);

    QList<QVector<U2MsaGap>> gapModel = GTUtilsMsaEditor::getEditor()->getMaObject()->getGapModel();
    if (gapModel.size() < 11) {
        GTMouseDriver::release();
        GTKeyboardDriver::keyRelease(Qt::Key_Control);
        CHECK_SET_ERR(false, "Can't find selected sequence");
    }

    if (gapModel[10].size() != 1) {
        GTMouseDriver::release();
        GTKeyboardDriver::keyRelease(Qt::Key_Control);
        CHECK_SET_ERR(false, QString("Unexpected selected sequence's gap model size, expected: 1, current: %1").arg(gapModel[10].size()));
    }

    // 4. Drag and drop selection to the left to the begining
    GTMouseDriver::moveTo(curPos);
    GTMouseDriver::release();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    QList<QVector<U2MsaGap>> finishGapModel = GTUtilsMsaEditor::getEditor()->getMaObject()->getGapModel();
    CHECK_SET_ERR(finishGapModel == startGapModel, "Unexpected changes of alignment");
}

GUI_TEST_CLASS_DEFINITION(test_5640) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set "Strict" consensus algorithm.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTComboBox::selectItemByText("consensusType", nullptr, "Strict");

    // 3. Set threshold to 1 % .
    GTSpinBox::setValue("thresholdSpinBox", 1, GTGlobals::UseKeyBoard);

    // 4. Remove the last sequence from the MSA.
    GTUtilsMSAEditorSequenceArea::removeSequence("Hetrodes_pupus_EF540832");

    // Expected state : consensus characters in the columns, that consist of gaps, are also gaps.
    auto expectedData = GTFile::readAll(testDir + "_common_data/scenarios/_regression/5640/res.txt");
    GTUtilsMSAEditorSequenceArea::checkConsensus(expectedData);
}

GUI_TEST_CLASS_DEFINITION(test_5657) {
    // 1. Open _common_data/clustal/COI_sub_asterisks.aln
    GTFileDialog::openFile(testDir + "_common_data/clustal/COI_sub_asterisks.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Try to align it with Kalign
    // Expected state: there is messagebox about incompatible alphabet
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "Unable to align this Multiple alignment with Kalign.\r\nPlease, convert alignment from Raw alphabet to supported one and try again."));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_5659) {
    // 1. Open murine.gb
    // 2. Context menu on annotations object
    // Expected state: export annotataions dialog appeared
    // 3. Check format combobox
    // Expected state: BAM format is abcent
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto comboBox = dialog->findChild<QComboBox*>();
            CHECK_SET_ERR(comboBox != nullptr, "ComboBox not found");

            QStringList formats = GTComboBox::getValues(comboBox);
            CHECK_SET_ERR(!formats.contains("BAM"), "BAM format is present in annotations export dialog");

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);

            QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != nullptr, "cancelButton is NULL");
            GTWidget::click(cancelButton);
        }
    };

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(new Scenario()));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("source"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_5660) {
    // Open document test/_common_data/clustal/1000_sequences.aln.fa
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/clustal/1000_sequences.aln.fa");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Statistics->generate distance matrix
    // Expected: HTML content is too large to be safely displayed in UGENE.
    class ClickOkButtonScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new DistanceMatrixDialogFiller(new ClickOkButtonScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_STATISTICS", "Generate distance matrix"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: task report contains "HTML content is too large to be safely displayed in UGENE."
    QWidget* activeWindow = GTUtilsMdi::activeWindow();
    CHECK_SET_ERR(activeWindow->windowTitle() == "Distance matrix for Multiple alignment", "Unexpected active window name");

    auto textBrowser = GTWidget::findTextBrowser("textBrowser", activeWindow);
    QString text = textBrowser->toHtml();
    CHECK_SET_ERR(text.contains("HTML content is too large to be safely displayed in UGENE."), text);
}

GUI_TEST_CLASS_DEFINITION(test_5663) {
    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("1ezg", 3, false, true, false, sandBoxDir));
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsNotifications::waitForNotification(false);
    QWidget* taskReportWindow = GTUtilsMdi::findWindow("Task report [Download remote documents]");
    auto reportEdit = GTWidget::findTextEdit("reportTextEdit", taskReportWindow);
    QString html = reportEdit->toHtml();
    CHECK_SET_ERR(html.contains("Document was successfully downloaded"), "Report contains expected text");
}

GUI_TEST_CLASS_DEFINITION(test_5665) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Document context menu -> Export / Import -> Export sequences.
    // Expected: "Export selected sequences" dialog appears.
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto filepathLineEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(filepathLineEdit, dataDir + "long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_.fa");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    // Expected: the dialog about external modification of documents appears.
    // 5. Click "No".
    // Expected: UGENE does not crash.
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(new Scenario()));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsProjectTreeView::click("human_T1.fa", Qt::RightButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_5681) {
    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto comboBox = GTWidget::findComboBox("", dialog);

            QStringList formats = GTComboBox::getValues(comboBox);
            CHECK_SET_ERR(!formats.contains("BAM"), "BAM format is present in annotations export dialog");

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);

            QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != nullptr, "cancelButton is NULL");
            GTWidget::click(cancelButton);
        }
    };

    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the context menu for the "NC_001363 features" object.
    // 3. Select "Export/Import" -> "Export annotations..." menu item.
    // 4. Set any valid output path, select "UGENE Database" format.
    // 5. Accept the dialog.
    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "murine_annotations.gb", ExportAnnotationsFiller::ugenedb, true, false, false));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export/Import", "Export annotations..."}));
    GTUtilsProjectTreeView::callContextMenu("NC_001363 features");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the imported annotations are loaded. The file extension was fixed according to the document format.
    GTUtilsProjectTreeView::checkItem("murine_annotations.ugenedb");
}

GUI_TEST_CLASS_DEFINITION(test_5696) {
    // 1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 3. Select region with gaps
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 1), QPoint(43, 3));

    // 4. Copy this subalignment
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);  // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);  // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems
    GTUtilsNotifications::waitForNotification(true, "No new rows were inserted: selection contains no valid sequences.");
    GTUtilsDialog::checkNoActiveWaiters();

    GTClipboard::setText("фыва...");
    // GTClipboard::setText("#$%^&*(");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);  // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems

    GTUtilsNotifications::waitForNotification(true, "No new rows were inserted: selection contains no valid sequences.");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_5714_1) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);

    // 5. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2066, 1));

    // 6. Press Ctrl + Shift + Backspace
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace, Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected: row length must be lesser than row length before trim
    qint64 currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    // 7. Press undo
    GTUtilsMcaEditor::undo();

    // Expected: current row length is equal start row length
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength == rowLength, QString("Expected: current row length is equal start row length, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    // 8. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2066, 1));

    // 9. Press Ctrl + Shift + Delete
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace, Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected: row length must be lesser than row length before trim
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5714_2) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);

    // 5. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2066, 1));

    // 6. Press "Trim left end" from the context menu
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Trim left end"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: row length must be lesser than row length before trim
    qint64 currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    // 7. Press undo
    GTUtilsMcaEditor::undo();

    // Expected: current row length is equal start row length
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength == rowLength, QString("Expected: current row length is equal start row length, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    // 8. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2066, 1));

    // 9. Press "Trim right end" from the context menu
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Trim right end"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: row length must be lesser than row length before trim
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5714_3) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);

    // 5. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2066, 1));

    // 6. Press "Trim left end" from the main menu
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Trim left end"});

    // Expected: row length must be lesser than row length before trim
    qint64 currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    // 7. Press undo
    GTUtilsMcaEditor::undo();

    // Expected: current row length is equal start row length
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength == rowLength, QString("Expected: current row length is equal start row length, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    // 8. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2066, 1));

    // 9. Press "Trim right end" from the main menu
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Trim right end"});

    // Expected: row length must be lesser than row length before trim
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5716) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "Export Consensus" options panel tab.
    //    Expected state: UGENE doesn't crash.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    //    3. Set any output file path, set any format.
    const QString expectedOutputPath = QDir::toNativeSeparators(sandBoxDir + "test_5716.txt");
    GTUtilsOptionPanelMsa::setExportConsensusOutputPath(expectedOutputPath);

    //    4. Open "General" options panel tab.
    //    Expected state: UGENE doesn't crash.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    //    5. Open "Export Consensus" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::ExportConsensus);

    //    Expected state: UGENE doesn't crash, the form is filled with values from step 3.
    const QString currentOutputPath = GTUtilsOptionPanelMsa::getExportConsensusOutputPath();
    const QString currentOutputFormat = GTUtilsOptionPanelMsa::getExportConsensusOutputFormat();
    const QString expectedOutputFormat = "Plain text";
    CHECK_SET_ERR(currentOutputPath == expectedOutputPath, QString("Output path is incorrect: expected '%1', got '%2'").arg(expectedOutputPath).arg(currentOutputPath));
    CHECK_SET_ERR(currentOutputFormat == expectedOutputFormat, QString("Output format is incorrect: expected '%1', got '%2'").arg(expectedOutputFormat).arg(currentOutputFormat));
}

GUI_TEST_CLASS_DEFINITION(test_5718) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);

    // 2. Click reference pos 2071
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2071, 1));

    // 3. Insert gap
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTUtilsOptionPanelMca::openTab(GTUtilsOptionPanelMca::General);
    const int lengthBeforeGapColumnsRemoving = GTUtilsOptionPanelMca::getLength();
    GTUtilsOptionPanelMca::closeTab(GTUtilsOptionPanelMca::General);

    // 4. Remove all columns of gaps
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Remove all columns of gaps"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: before gap column removig < after gap column removig
    GTUtilsOptionPanelMca::openTab(GTUtilsOptionPanelMca::General);
    int lengthAfterGapColumnsRemoving = GTUtilsOptionPanelMca::getLength();
    GTUtilsOptionPanelMca::closeTab(GTUtilsOptionPanelMca::General);
    CHECK_SET_ERR(lengthAfterGapColumnsRemoving < lengthBeforeGapColumnsRemoving, QString("Expected: before gap column removig > after gap column removig, current: before %1, after %2").arg(QString::number(lengthBeforeGapColumnsRemoving)).arg(QString::number(lengthAfterGapColumnsRemoving)));
}

GUI_TEST_CLASS_DEFINITION(test_5728) {
    // Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select the first character in the first row.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(0, 0));

    // Enter the character replacement mode.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    // Press the Space key.
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Select the last character in the last row.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(13, 9), QPoint(13, 9));

    // Press the Delete key (to cause the alignment updating and redrawing).
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: the first character in the first row and the last character in the last row are gaps, the rest characters in the alignment are the same.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(0, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString selectionContent1 = GTClipboard::text();
    CHECK_SET_ERR(selectionContent1 == "-", QString("Incorrect selection content: expected - %1, received - %2").arg("-").arg(selectionContent1));

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(13, 9), QPoint(13, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString selectionContent2 = GTClipboard::text();
    CHECK_SET_ERR(selectionContent2 == "-", QString("Incorrect selection content: expected - %1, received - %2").arg("-").arg(selectionContent2));

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength() == 14, "Wrong msa length");
}

GUI_TEST_CLASS_DEFINITION(test_5730) {
    GTLogTracer lt;
    QFile originalFile(dataDir + "samples/Genbank/murine.gb");
    QString dstPath = sandBoxDir + "/5730_murine.gb";
    originalFile.copy(dstPath);
    QFile copiedFile(dstPath);
    CHECK_SET_ERR(copiedFile.exists(), "Unable to copy file");

    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir, "5730_murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Sequence + annotations
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::YesToAll));
    GTUtilsDialog::waitForDialog(new ExportSelectedRegionFiller(sandBoxDir, "5730_murine.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export/Import", "Export sequences..."}));
    GTUtilsProjectTreeView::callContextMenu("NC_004718");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters(10000);

    CHECK_SET_ERR(lt.hasMessage("is already added to the project"), "Expected message not found in the log");

    // Other objects
    QFile originalFile2(dataDir + "samples/CLUSTALW/COI.aln");
    dstPath = sandBoxDir + "/5730_COI.aln";
    originalFile2.copy(dstPath);

    QFile copiedFile2(dstPath);
    CHECK_SET_ERR(copiedFile2.exists(), "Unable to copy file");

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir, "5730_COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Export object..."}));
    GTUtilsDialog::add(new ExportDocumentDialogFiller(sandBoxDir, "5730_COI.aln", ExportDocumentDialogFiller::CLUSTALW, false, true));
    GTUtilsProjectTreeView::callContextMenu("HIV-1", "HIV-1.aln");

    CHECK_SET_ERR(lt.hasMessage("is already added to the project, it will be overwritten."), "Expected message not found in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5739) {
    class AddReadsWithReferenceScenario : public CustomScenario {
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();

            // Expected state: "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox", dialog);
            CHECK_SET_ERR(minReadIdentity == 80, QString("Incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state: "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox", dialog);
            CHECK_SET_ERR(quality == 30, QString("Incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state: "Add to project" option is checked by default
            bool isAddToProject = GTCheckBox::getState("addToProjectCheckbox", dialog);
            CHECK_SET_ERR(isAddToProject, QString("Incorrect addToProject state: expected true, got false"));

            // Expected state: "Result alignment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit", dialog);
            CHECK_SET_ERR(!output.isEmpty(), "Incorrect output line: is empty");

            // Select reference  _common_data/sanger/reference.gb.
            GTLineEdit::setText("referenceLineEdit", testDir + "_common_data/sanger/reference_short.gb", dialog);

            // Select Reads:  _common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                reads << "sanger_" + num + ".ab1";
            }
            GTUtilsTaskTreeView::waitTaskFinished();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/sanger", reads));
            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // Push "Align" button.
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // Select "Tools > Sanger data analysis > Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new AddReadsWithReferenceScenario()));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();  // This task may take up to 1 minute. checkMcaEditorWindowIsActive waits only for 30 seconds.
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMca::openTab(GTUtilsOptionPanelMca::General);

    // Move mouse cursor to the position 6372 at the reference sequence (first half of the char).
    GTUtilsMcaEditorSequenceArea::moveCursorToReferencePositionCenter(6372, QPoint(-5, 0));

    // Select all chars in the reference from here to the end.
    QPoint currentPos = GTMouseDriver::getMousePosition();
    int newXPos = GTUtilsMdi::activeWindow()->mapToGlobal(GTUtilsMdi::activeWindow()->rect().topRight()).x() - 1;
    GTMouseDriver::dragAndDrop(currentPos, QPoint(newXPos, currentPos.y()));

    // Expected: selected length = 4.
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection();
    CHECK_SET_ERR(reg.length == 4, QString("Unexpected selection length, expected: 4, got: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5747) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select any sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Gampsocleis_sedakovii_EF540828");

    // 3. Call contest menu -> Edit -> Edit sequence name
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Edit sequence name"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // 4. Set new name and press enter
    GTKeyboardDriver::keySequence("New name");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // 5. Select another sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Conocephalus_sp.");

    // 6. Edit name by HotKey F2
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    // 7. Set new name and press enter
    GTKeyboardDriver::keySequence("New name 2");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

GUI_TEST_CLASS_DEFINITION(test_5750) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Rename the first sequence to '1'.
    GTUtilsMSAEditorSequenceArea::renameSequence("Phaneroptera_falcata", "1");

    //    3. Export the alignment object to MSF format.
    GTLogTracer lt;

    GTUtilsDialog::add(new PopupChooserByText({"Export/Import", "Export object..."}));
    GTUtilsDialog::add(new ExportDocumentDialogFiller(sandBoxDir, "test_5750.msf", ExportDocumentDialogFiller::MSF, false, true));
    GTUtilsProjectTreeView::callContextMenu("COI", "COI.aln");

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the exported file is opened in UGENE. The first sequence is named "1".
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;

    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(!names.isEmpty(), "Names list is empty");

    const QString expectedName = "1";
    CHECK_SET_ERR(expectedName == names[0], QString("The first sequecne name is incorrect: expected '%1', got '%2'").arg(expectedName).arg(names[0]));
}

GUI_TEST_CLASS_DEFINITION(test_5751) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            GTUtilsTaskTreeView::waitTaskFinished();
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(path));
            GTWidget::click(GTWidget::findToolButton("setOutputButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Call a context menu in the Project view on the opened MCA document.
    // 6. Select "Lock document for editing" menu item.
    GTUtilsProjectTreeView::click("Sanger.ugenedb");
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Lock document for editing"}));
    GTUtilsProjectTreeView::click("Sanger.ugenedb", Qt::RightButton);

    // 7. Call a context menu in the MCA Editor.
    // Expected state : "Remove all columns of gaps" is disabled.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Edit", "Remove all columns of gaps"}, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyPress(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_5752) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, "Incorrect addToProject state: expected true, got false");

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/sanger/", reads));
            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Select any symbol.
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2120, 1));

    // 6. Press Trim left end
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Trim left end"});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 7. Press Trim right end.
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Trim right end"});
    GTUtilsTaskTreeView::waitTaskFinished();

    int readNum = GTUtilsMcaEditor::getReadsNames().size();
    // 8. Press Replace symbol / character and press space.
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace character/gap"});
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Expected : Can't replace with GAP the last non-gap symbol in the read.
    // UGENE does not support empty reads today.
    int newReadNum = GTUtilsMcaEditor::getReadsNames().size();
    CHECK_SET_ERR(newReadNum == 16 && readNum == 16, QString("Incorrect reads num, expected 16, got %1 and %2").arg(readNum).arg(newReadNum));
}

GUI_TEST_CLASS_DEFINITION(test_5753) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            GTUtilsTaskTreeView::waitTaskFinished();
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(path));
            GTWidget::click(GTWidget::findToolButton("setOutputButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Make changes
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2120, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // 6. Close document
    GTUtilsProjectTreeView::click("Sanger.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected: there is no "Save document" messageBox
}

GUI_TEST_CLASS_DEFINITION(test_5755) {
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox", dialog);
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox", dialog);
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox", dialog);
            CHECK_SET_ERR(addToProject, "incorrect addToProject state: expected true, got false");

            // Expected state : "Result alignment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit", dialog);
            CHECK_SET_ERR(!output.isEmpty(), "incorrect output line: is empty");

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText("referenceLineEdit", testDir + "_common_data/sanger/reference_need_gaps.gb", dialog);

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i <= 20; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(readDir, reads));
            GTWidget::click(GTWidget::findPushButton("addReadButton", dialog));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment".
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected : Trailing gaps were inserted into the end of reference.
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength();
    QString refReg = GTUtilsMcaEditorSequenceArea::getReferenceReg(refLength - 20, 20);
    bool isGap = std::all_of(refReg.begin(), refReg.end(), [](const auto& c) { return c == U2Mca::GAP_CHAR; });
    CHECK_SET_ERR(isGap, "Expected only gaps, got: " + refReg);
}

GUI_TEST_CLASS_DEFINITION(test_5758) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            GTUtilsTaskTreeView::waitTaskFinished();
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(path));
            GTWidget::click(GTWidget::findToolButton("setOutputButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Remove a row
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B70");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 6. Close the view
    GTUtilsProjectTreeView::click("Sanger.ugenedb");
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Unload selected document(s)"}));
    GTUtilsProjectTreeView::click("Sanger.ugenedb", Qt::RightButton);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // 7. Open a new view
    GTUtilsProjectTreeView::click("Sanger.ugenedb");
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Load selected document(s)"}));
    GTUtilsProjectTreeView::click("Sanger.ugenedb", Qt::RightButton);

    // 8. Hide chromatograms
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar("mwtoolbar_activemdi"), "chromatograms"));

    // 9. Change the state of the last row
    bool isShownFirstState = GTUtilsMcaEditorSequenceArea::isChromatogramShown("SZYD_Cas9_CR66");
    GTUtilsMcaEditorSequenceArea::clickCollapseTriangle("SZYD_Cas9_CR66", isShownFirstState);
    bool isShownSecondState = GTUtilsMcaEditorSequenceArea::isChromatogramShown("SZYD_Cas9_CR66");

    // Expected: States befor and aftef changing are different
    CHECK_SET_ERR(isShownFirstState != isShownSecondState, "Incorrect state");
}

GUI_TEST_CLASS_DEFINITION(test_5759) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Collapse all rows except the second one.
    GTUtilsMcaEditor::toggleShowChromatogramsMode();

    // 3. Select the "SZYD_Cas9_5B70" sequence.
    GTUtilsMcaEditor::clickReadName(QString("SZYD_Cas9_5B70"));

    // 4. Click arrow down
    GTKeyboardDriver::keyClick(Qt::Key_Down);

    GTKeyboardDriver::keyClick(Qt::Key_Right);

    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::isChromatogramShown(QString("SZYD_Cas9_5B71")),
                  "Required sequence is collapsed");
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    GTUtilsMcaEditor::removeRead(QString("SZYD_Cas9_5B70"));
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::isChromatogramShown(QString("SZYD_Cas9_5B71")),
                  "Required sequence is collapsed");
}

GUI_TEST_CLASS_DEFINITION(test_5761) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMca::openTab(GTUtilsOptionPanelMca::General);

    GTLogTracer lt;
    // 2. Select the last char of the first row
    MultipleAlignmentRowData* row = GTUtilsMcaEditor::getMcaRow(0);
    int end = row->getCoreStart() + row->getCoreLength() - 1;
    QPoint p(end, 0);
    GTUtilsMcaEditorSequenceArea::clickToPosition(p);
    QPoint curPos = GTMouseDriver::getMousePosition();
    QPoint moveMouseTo(curPos.x() + 140, curPos.y());

    // 3. Press left button and move mouse to the right (add some gaps)
    GTMouseDriver::press();
    GTMouseDriver::moveTo(moveMouseTo);
    int i = 10;
    while (i != 0) {
        int minus = (i % 2 == 0) ? 1 : -1;
        int moving = minus * 3 * ((i % 2) + 1);
        QPoint perturbation(moveMouseTo.x(), moveMouseTo.y() + moving);
        GTMouseDriver::moveTo(perturbation);
        i--;
    }
    GTMouseDriver::release();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5769_1) {
    class Scenario : public CustomScenario {
        void run() override {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            CHECK_SET_ERR(minReadIdentity == 80, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            CHECK_SET_ERR(quality == 30, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool isAddToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(isAddToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            GTUtilsTaskTreeView::waitTaskFinished();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/sanger/", reads));
            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Select read "SZYD_Cas9_5B71"
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B71");

    // 6. click 'down' two times
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Down);

    // Expected: selected read "SZYD_Cas9_CR51"
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 1, QString("1. Unexpected selection! Expected selection size == 1, actual selection size == %1").arg(name.size()));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_CR51", QString("Unexpected selected read, expected: SZYD_Cas9_CR51, current: %1").arg(name[0]));

    // 7. Remove selected read
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 8. click 'down' two times
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Down);

    // Expected: selected read "SZYD_Cas9_CR54"
    name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 1, QString("2. Unexpected selection! Expected selection size == 1, actual selection size == %1").arg(name.size()));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_CR54", QString("Unexpected selected read, expected: SZYD_Cas9_CR54, current: %1").arg(name[0]));
}

GUI_TEST_CLASS_DEFINITION(test_5769_2) {
    class Scenario : public CustomScenario {
        void run() {
            // Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue("minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            // Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue("qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            // Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState("addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            // Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText("outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            // 2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            // 3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished();
            auto ob = new GTFileDialogUtils_list(readDir, reads);
            GTUtilsDialog::waitForDialog(ob);

            GTWidget::click(GTWidget::findPushButton("addReadButton"));

            // 4. Push "Align" button
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    // 1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Select read "SZYD_Cas9_5B71"
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_CR50");

    // 6. click 'up'
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    // Expected: selected read "SZYD_Cas9_5B71"
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 1, QString("Unexpected selection? expected sel == 1< cerrent sel == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_5B71", QString("Unexpected selected read, expected: SZYD_Cas9_5B71, current: %1").arg(name[0]));

    // 7. Remove selected read
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 8. click 'up'
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    // Expected: selected read "SZYD_Cas9_5B70"
    name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 1, QString("Unexpected selection? expected sel == 1< cerrent sel == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_5B70", QString("Unexpected selected read, expected: SZYD_Cas9_5B70, current: %1").arg(name[0]));
}

GUI_TEST_CLASS_DEFINITION(test_5770) {
    QString filePath = testDir + "_common_data/sanger/alignment.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select read "SZYD_Cas9_5B70"
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B70");

    // 3. Hold the _Shift_ key and press the _down arrow_ key.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B71");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected: the selection is expanded.
    QStringList names = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(names.size() == 2, QString("Incorrect selection. Expected: 2 selected rows, current: %1 selected rows").arg(names.size()));
}

GUI_TEST_CLASS_DEFINITION(test_5773) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::filterProject("GTTCTCGGG");

    GTUtilsProjectTreeView::checkFilteredGroup("Sanger read content", QStringList(), {"Aligned reads", "ugene_gui_test"}, {"HIV-1.aln"});

    GTUtilsProjectTreeView::checkFilteredGroup("Sanger reference content", QStringList(), {"Aligned reads", "ugene_gui_test"}, {"HIV-1.aln"});

    GTUtilsProjectTreeView::filterProject("KM0");
    GTUtilsProjectTreeView::checkFilteredGroup("Sanger reference name", QStringList(), {"Aligned reads", "ugene_gui_test"}, {"HIV-1.aln"});
}

GUI_TEST_CLASS_DEFINITION(test_5775) {
    // 1. Open "data/samples/FASTQ/eas.fastq".
    // Expected state: the "Sequence Reading Options" dialog has appeared.
    // Select the "Merge sequences into a single sequence to show in sequence viewer" option. Accept the dialog.
    GTUtilsProject::openMultiSequenceFileAsMergedSequence(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Search in Sequence" options panel tab.
    // Check the "Load patterns from file" option, select "data/samples/FASTQ/eas.fastq" as the file with patterns.
    // Expected state: the search task is launched automatically. After it is finished, there are 3 results.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(true);
    GTCheckBox::setChecked(GTWidget::findCheckBox("chbUsePatternNames"), true);

    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();

    GTUtilsOptionPanelSequenceView::enterPatternFromFile(dataDir + "samples/FASTQ/", "eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/3"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_5781) {
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Align));
    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(testDir + "_common_data/fasta/ref2.fa", QStringList());
    parameters.samOutput = false;
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    // GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "can't be mapped"));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/COI2.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChecker({"unassociateReferenceAction"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_5783) {
    // Open "samples/FASTA/human_T1.fa"
    // Create an annotation, set e.g. "ann" annotation name and "200..300" region.
    // Add "gene_id" and "transcript_id" qualifiers.
    // Export the annotation to the GTF format. Make sure the "Add to project" option is checked in the export dialog.
    // Expected state: The export has finished without errors. The document has been added to the project.

    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "ann", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);
    GTWidget::click(GTUtilsAnnotationsTreeView::getTreeWidget());
    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "XCV", "ann");
    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "TR321", "ann");

    GTUtilsAnnotationsTreeView::selectItemsByName({"ann"});

    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTMouseDriver::click(Qt::RightButton);
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5786_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Build Tree" button on the toolbar.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            //    3. Select "PhyML Maximum Likelihood" tree building method.
            GTComboBox::selectItemByText("algorithmBox", dialog, "PhyML Maximum Likelihood");

            //    4. Open "Branch Support" tab.
            GTTabWidget::clickTab("twSettings", dialog, "Branch Support");

            //    Expected state: "Use fast likelihood-based method" radionbutton is selected, "Use fast likelihood-based method" combobox is enabled, "Perform bootstrap" spinbox is disabled.
            auto rbFastMethod = GTWidget::findRadioButton("fastMethodCheckbox", dialog);
            CHECK_SET_ERR(rbFastMethod->isChecked(), "fastMethodCheckbox is not checked");
            GTWidget::checkEnabled("fastMethodCombo", true, dialog);
            GTWidget::checkEnabled("bootstrapSpinBox", false, dialog);

            //    5. Select "Perform bootstrap" radiobutton.
            GTRadioButton::click("bootstrapRadioButton", dialog);

            //    Expected state: "Use fast likelihood-based method" combobox is disabled, "Perform bootstrap" spinbox is enabled.
            GTWidget::checkEnabled("fastMethodCombo", false, dialog);
            GTWidget::checkEnabled("bootstrapSpinBox", true, dialog);

            //    6. Select "Use fast likelihood-based method" radionbutton.
            GTRadioButton::click("fastMethodCheckbox", dialog);

            //    Expected state: "Use fast likelihood-based method" combobox is enabled, "Perform bootstrap" spinbox is disabled.
            GTWidget::checkEnabled("fastMethodCombo", true, dialog);
            GTWidget::checkEnabled("bootstrapSpinBox", false, dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
}

GUI_TEST_CLASS_DEFINITION(test_5786_2) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Build Tree" button on the toolbar.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            //    3. Select "PhyML Maximum Likelihood" tree building method.
            GTComboBox::selectItemByText("algorithmBox", dialog, "PhyML Maximum Likelihood");

            GTWidget::checkEnabled("tranSpinBox", false, dialog);

            //    4. Select "Transition / transversion ratio" "fixed" radiobutton.
            GTRadioButton::click("transFixedRb", dialog);

            GTWidget::checkEnabled("tranSpinBox", true, dialog);

            //    5. Open "Branch Support" tab.
            GTTabWidget::clickTab("twSettings", dialog, "Branch Support");

            //    6. Select "Perform bootstrap" radiobutton.
            GTRadioButton::click("bootstrapRadioButton", dialog);

            //    7. Open the "Substitution Model" tab.
            GTTabWidget::clickTab("twSettings", dialog, "Substitution Model");

            //    Expected state: Expected state: the "Transition / transversion ratio" spinbox is enabled.
            GTWidget::checkEnabled("tranSpinBox", true, dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
}

GUI_TEST_CLASS_DEFINITION(test_5786_3) {
    GTLogTracer lt;

    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click "Build Tree" button on the toolbar.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            //    3. Select "PhyML Maximum Likelihood" tree building method.
            GTComboBox::selectItemByText("algorithmBox", dialog, "PhyML Maximum Likelihood");

            //    4. Open "Branch Support" tab.
            GTTabWidget::clickTab("twSettings", dialog, "Branch Support");

            //    5. Select "Perform bootstrap" radiobutton.
            GTRadioButton::click("bootstrapRadioButton", dialog);

            //    6. Set "Perform bootstrap" spinbox value to 5.
            GTSpinBox::setValue("bootstrapSpinBox", 5, dialog);

            //    7. Select "Use fast likelihood-based method" radiobutton.
            GTRadioButton::click("fastMethodCheckbox", dialog);

            //    8. Set "Use fast likelihood-based method" combobox value to "Chi2-based".
            GTComboBox::selectItemByText("fastMethodCombo", dialog, "Chi2-based");

            //    9. Set other necessary values and accept the dialog.
            GTLineEdit::setText("fileNameEdit", sandBoxDir + "test_5786_3.nwk", dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");

    //    Expected state: there is an only "-b" parameter in the phyML arguments, it is equal to "-2".
    CHECK_SET_ERR(!lt.hasMessage("-b 5"), "Found unexpected message");
    CHECK_SET_ERR(lt.hasMessage("-b -2"), "Expected message is not found");
}

GUI_TEST_CLASS_DEFINITION(test_5789_1) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", sandBoxDir + "test_5789.ugenedb");
    GTFileDialog::openFile(sandBoxDir + "test_5789.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: both "Undo" and "Redo" buttons are disabled.
    bool isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled();
    bool isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled();
    CHECK_SET_ERR(!isUndoEnabled, "Undo button is unexpectedly enabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    2. Edit the MCA somehow.
    GTUtilsMcaEditor::removeRead("SZYD_Cas9_5B70");

    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled();
    isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled();
    CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    3. Close and open the view again.
    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    //    4. Repeat the previous state several times.
    for (int i = 0; i < 5; i++) {
        GTUtilsMdi::closeActiveWindow();
        GTUtilsProjectTreeView::doubleClickItem("test_5789.ugenedb");
        GTUtilsTaskTreeView::waitTaskFinished();

        isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled();
        isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled();
        CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
        CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");
    }
}

GUI_TEST_CLASS_DEFINITION(test_5789_2) {
    //    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: both "Undo" and "Redo" buttons are disabled.
    bool isUndoEnabled = GTUtilsMsaEditor::isUndoEnabled();
    bool isRedoEnabled = GTUtilsMsaEditor::isRedoEnabled();
    CHECK_SET_ERR(!isUndoEnabled, "Undo button is unexpectedly enabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    2. Edit the MSA somehow.
    GTUtilsMsaEditor::removeRows(0, 0);
    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled();
    isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled();
    CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    3. Close and open the view again.
    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    //    4. Repeat the previous state several times.
    for (int i = 0; i < 5; i++) {
        GTUtilsMdi::closeActiveWindow();
        GTUtilsProjectTreeView::doubleClickItem("ma.aln");
        GTUtilsTaskTreeView::waitTaskFinished();

        isUndoEnabled = GTUtilsMsaEditor::isUndoEnabled();
        isRedoEnabled = GTUtilsMsaEditor::isRedoEnabled();
        CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
        CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");
    }
}

GUI_TEST_CLASS_DEFINITION(test_5790) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_5790.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    // GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B71");
    // 2. Click to position on read
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2120, 1));

    // 3. Enter edit mode
    GTKeyboardDriver::keyClick('i', Qt::ShiftModifier);
    // 4. Click escape
    // Expected state: selection still present
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::getCharacterModificationMode() == 0, "MCA is not in view mode");

    // 5. Click escape
    // Expected state: selection disappeared
    QRect emptyselection = QRect();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::getSelectedRect() == emptyselection, "Selection isn't empty but should be");
}

GUI_TEST_CLASS_DEFINITION(test_5798_1) {
    // 1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: DNA.apr in the project view
    GTUtilsProjectTreeView::checkItem("DNA.apr");
    GTUtilsProjectTreeView::checkObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(QStringList() << "DNA.apr"));
    GTUtilsDocument::checkIfDocumentIsLocked("DNA.apr", true);
}

GUI_TEST_CLASS_DEFINITION(test_5798_2) {
    // 1. Convert samples/APR/DNA.apr to fasta
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(false, sandBoxDir + "DNA", "FASTA"));
    GTUtilsProject::openFile(dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: DNA.fa in the project view
    GTUtilsProjectTreeView::checkItem("DNA.fa");
    GTUtilsProjectTreeView::checkObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(QStringList() << "DNA.fa"));
    GTUtilsDocument::checkIfDocumentIsLocked("DNA.fa", false);
}

GUI_TEST_CLASS_DEFINITION(test_5798_3) {
    // 1. Convert samples/APR/DNA.apr to clustaw
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(false, sandBoxDir + "DNA", "CLUSTALW"));
    GTUtilsProject::openFile(dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: DNA.aln in the project view
    GTUtilsProjectTreeView::checkItem("DNA.aln");
    GTUtilsProjectTreeView::checkObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(QStringList() << "DNA.aln"));
    GTUtilsDocument::checkIfDocumentIsLocked("DNA.aln", false);
}

GUI_TEST_CLASS_DEFINITION(test_5798_4) {
    // 1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: DNA.apr in the project view
    GTUtilsProjectTreeView::checkItem("DNA.apr");
    GTUtilsProjectTreeView::checkObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(QStringList() << "DNA.apr"));
    GTUtilsDocument::checkIfDocumentIsLocked("DNA.apr", true);

    // 2. Convert document to clustalw from project view
    GTUtilsDialog::waitForDialog(new ExportDocumentDialogFiller(sandBoxDir, "DNA.aln", ExportDocumentDialogFiller::CLUSTALW, false, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Export document"}));
    GTUtilsProjectTreeView::callContextMenu("DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: DNA.aln in the project view
    GTUtilsProjectTreeView::checkItem("DNA.aln");
    GTUtilsProjectTreeView::checkObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(QStringList() << "DNA.aln"));
    GTUtilsDocument::checkIfDocumentIsLocked("DNA.aln", false);

    // 3. Export object to MEGA format from project view
    GTUtilsDialog::waitForDialog(new ExportDocumentDialogFiller(sandBoxDir, "DNA.meg", ExportDocumentDialogFiller::MEGA, false, true));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export/Import", "Export object..."}));
    GTUtilsProjectTreeView::callContextMenu("DNA", "DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: DNA.meg is in the project view
    GTUtilsProjectTreeView::checkItem("DNA.meg");
    GTUtilsProjectTreeView::checkObjectTypes(QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(QStringList() << "DNA.meg"));
    GTUtilsDocument::checkIfDocumentIsLocked("DNA.meg", false);
}

GUI_TEST_CLASS_DEFINITION(test_5798_5) {
    // 1. Open Workflow designer
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Open sample {Convert alignments to ClustalW}
    GTUtilsWorkflowDesigner::addSample("Convert alignments to ClustalW");
    // Expected state: There is "Show wizard" tool button
    // 3. Press "Show wizard" button

    class customWizard : public CustomScenario {
    public:
        void run() override {
            // 4. Select input MSA "samples/APR/DNA.apr"
            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << dataDir + "samples/APR/DNA.apr"));

            // 5. Press "Next" button
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            // GTUtilsWizard::setParameter("Result ClustalW file", "DNA.aln");

            // 6. Press "Run" button
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Convert alignments to ClustalW Wizard", new customWizard()));
    GTWidget::click(GTAction::button("Show wizard"));
    // Expected state: Align sequences with MUSCLE Wizard appeared

    // Expected state: Scheme successfully performed
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5815) {
    // 1. Open a short alignment, e.g "test_common_data\scenarios\msa\ma2_gapped.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click on the empty area at the right side of the consensus
    GTUtilsMsaEditor::moveToColumn(13);
    QPoint p = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(QPoint(p.x() + 100, p.y()));
    GTMouseDriver::click();

    // Expected: no crash
}

GUI_TEST_CLASS_DEFINITION(test_5818_1) {
    // 1. Open samples/ACE/BL060C3.ace in read-only mode
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: BL060C3.ace in the project view
    GTUtilsProjectTreeView::checkItem("BL060C3.ace");
}

GUI_TEST_CLASS_DEFINITION(test_5818_2) {
    // 1. Convert samples/ACE/BL060C3.ace.ugenedb to fasta
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "BL060C3.ace.ugenedb"));
    GTUtilsProject::openFile(dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: BL060C3.ace.ugenedb in the project view
    GTUtilsProjectTreeView::checkItem("BL060C3.ace.ugenedb");
}

GUI_TEST_CLASS_DEFINITION(test_5832) {
    // 1. Open "test/_common_data/fasta/empty.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;

    // 2. Click on the sequence area.
    GTUtilsMSAEditorSequenceArea::click(QPoint(5, 5));

    // Expected: no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5833) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select 440 base on the second read (the position is ungapped).
    GTUtilsMcaEditorSequenceArea::clickToPosition(QPoint(2506, 1));
    GTUtilsMcaEditorSequenceArea::getSelectedReadChar();

    //    Expected state: the status bar contains the next labels: "Ln 2/16, RefPos 2500/11878, ReadPos 440/1173".
    QString rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString();
    QString rowsCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString();
    QString referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString();
    QString referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString();
    QString readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString();
    QString readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString();
    CHECK_SET_ERR("2" == rowNumberString, QString("Unexepected row number label: expected '%1', got '%2'").arg("2").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowsCountString, QString("Unexepected rows count label: expected '%1', got '%2'").arg("16").arg(rowsCountString));
    CHECK_SET_ERR("2500" == referencePositionString, QString("Unexepected reference position label: expected '%1', got '%2'").arg("2500").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("Unexepected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("440" == readPositionString, QString("Unexepected read position label: expected '%1', got '%2'").arg("440").arg(readPositionString));
    CHECK_SET_ERR("1173" == readLengthString, QString("Unexepected read length label: expected '%1', got '%2'").arg("1173").arg(readLengthString));

    //    3. Call a context menu, select "Edit" -> "Insert character/gap" menu item.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit", "Insert character/gap"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Click 'A' key.
    GTKeyboardDriver::keyClick('A');

    //    Expected state: the new base has been inserted, the status bar contains the next labels: "Ln 2/16, RefPos gap/11878, ReadPos 440/1174".
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString();
    rowsCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString();
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString();
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString();
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString();
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString();
    CHECK_SET_ERR("2" == rowNumberString, QString("Unexepected row number label: expected '%1', got '%2'").arg("2").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowsCountString, QString("Unexepected rows count label: expected '%1', got '%2'").arg("16").arg(rowsCountString));
    CHECK_SET_ERR("gap" == referencePositionString, QString("Unexepected reference position label: expected '%1', got '%2'").arg("gap").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("Unexepected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("440" == readPositionString, QString("Unexepected read position label: expected '%1', got '%2'").arg("440").arg(readPositionString));
    CHECK_SET_ERR("1174" == readLengthString, QString("Unexepected read length label: expected '%1', got '%2'").arg("1174").arg(readLengthString));
}

GUI_TEST_CLASS_DEFINITION(test_5837) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTUtilsProject::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    //    2. Select first sequence
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "exportSelectedMsaRowsToSeparateFilesAction"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new ExportSelectedSequenceFromAlignment(testDir + "_common_data/scenarios/sandbox/", ExportSelectedSequenceFromAlignment::Ugene_db, true));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("Phaneroptera_falcata.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_5840) {
    QString fileName = "sanger_alignment.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(testDir + "_common_data/sanger/alignment_short.ugenedb", sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);

    // 2. Select a read ""
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B71");

    // 3. Select a document in the Project View and press the Delete key.
    GTUtilsProjectTreeView::click("Aligned reads");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected: The document has been deleted.
    bool isFound = GTUtilsProjectTreeView::checkItem("Aligned reads", {false});
    CHECK_SET_ERR(!isFound, "The document has not been deleted")
}

GUI_TEST_CLASS_DEFINITION(test_5842) {
    // 1. Open "_common_data/sanger/aligment.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Unload document.
    GTUtilsDocument::unloadDocument("alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;
    // 3. Open the view from the context menu.
    GTUtilsDialog::waitForDialog(new PopupChooser({"openInMenu", "action_open_view"}));
    GTUtilsProjectTreeView::click("alignment.ugenedb", Qt::RightButton);

    // Expected state: the view is opened without errors.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5847) {
    // 1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select any sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("HS11791");

    GTLogTracer lt;

    // 3 Press "delete"
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected: no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5849) {
    GTFileDialog::openFile(testDir + "_common_data/fasta", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click the "align_new_sequences_to_alignment_action" button on the toolbar.
    // Expected state: the file selection dialog is opened.
    // Select "..\samples\CLUSTALW\COI.aln" in the dialog.

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/COI.aln"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a sequence.
    GTUtilsMSAEditorSequenceArea::click(QPoint(2, 2));
    QAbstractButton* undoButton = GTAction::button("msa_action_undo");

    // Click the "Undo" button.
    GTWidget::click(undoButton);
    auto msaEditorStatusBar = GTWidget::findWidget("msa_editor_status_bar");

    // Expected state: the selection has been cleared.
    auto line = GTWidget::findLabel("Line", msaEditorStatusBar);
    auto column = GTWidget::findLabel("Column", msaEditorStatusBar);
    auto position = GTWidget::findLabel("Position", msaEditorStatusBar);
    auto selection = GTWidget::findLabel("Selection", msaEditorStatusBar);

    CHECK_SET_ERR(line->text() == "Seq - / 2", "Sequence is " + line->text());
    CHECK_SET_ERR(column->text() == "Col - / 4", "Column is " + column->text());
    CHECK_SET_ERR(position->text() == "Pos - / -", "Position is " + position->text());
    CHECK_SET_ERR(selection->text() == "Sel none", "Selection is " + selection->text());
}

GUI_TEST_CLASS_DEFINITION(test_5851) {
    //    1. Set the temporary dir path to the folder with the spaces in the path.
    QDir().mkpath(sandBoxDir + "test_5851/t e m p");

    class SetTempDirPathScenario : public CustomScenario {
    public:
        void run() override {
            AppSettingsDialogFiller::setTemporaryDirPath(sandBoxDir + "test_5851/t e m p");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new SetTempDirPathScenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    GTLogTracer lt;

    //    2. Select "Tools" -> Sanger data analysis" -> "Map reads to reference...".
    //    3. Set "_common_data/sanger/reference.gb" as reference, "_common_data/sanger/sanger_*.ab1" as reads. Accept the dialog.
    //    Expected state: the task fails.
    //    4. After the task finish open the report.
    //    Expected state: there is an error message in the report: "The task uses a temporary folder to process the data. The folder path is required not to have spaces. Please set up an appropriate path for the "Temporary files" parameter on the "Directories" tab of the UGENE Application Settings.".
    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AlignToReferenceBlastDialogFiller::setReference(testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList reads;
            for (int i = 1; i < 21; i++) {
                reads << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(reads, dialog);
            AlignToReferenceBlastDialogFiller::setDestination(sandBoxDir + "test_5851/test_5851.ugenedb", dialog);

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasMessage("Your \"Temporary files\" directory contains spaces, \"makeblastdb\" external tool can't correct process it."
                                " Please change it in Preferences on the Directories page, restart UGENE and try again. Current problem path is:"),
                  "Expected message not found");
}

GUI_TEST_CLASS_DEFINITION(test_5853) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the "Tettigonia_viridissima" sequence in the Name List area.
    GTUtilsMsaEditor::clickSequence(9);

    // 3. Press the Esc key.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Expected state : the selection is cleared.
    int numSelSeq = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelSeq == 0, QString("First check, incorrect num of selected sequences, expected: 0, current : %1").arg(numSelSeq));

    // 4. Press the down arrow key.
    GTKeyboardDriver::keyClick(Qt::Key_Down);

    // Expected: nothing should be selected
    numSelSeq = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelSeq == 0, QString("Second checdk, incorrect num of selected sequences, expected: 0, current : %1").arg(numSelSeq));
}

GUI_TEST_CLASS_DEFINITION(test_5854) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select "Mecopoda_elongata__Ishigaki__J" sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Mecopoda_elongata__Ishigaki__J");

    MSAEditorSequenceArea* seqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea();
    QRect sel = seqArea->getEditor()->getSelection().toRect();
    int index = seqArea->getRowIndex(sel.y()) + 1;

    // Expected:: current index 14
    CHECK_SET_ERR(index == 14, QString("Unexpected index, expected: 14, current: %1").arg(index));

    // 4. Select "Mecopoda_sp.__Malaysia_" sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Mecopoda_sp.__Malaysia_");

    // Expected:: current index 16
    sel = seqArea->getEditor()->getSelection().toRect();
    index = seqArea->getRowIndex(sel.y()) + 1;
    CHECK_SET_ERR(index == 16, QString("Unexpected index, expected: 16, current: %1").arg(index));
}

GUI_TEST_CLASS_DEFINITION(test_5855) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: a collapsing group appears.
    GTUtilsMSAEditorSequenceArea::isCollapsed("Mecopoda_elongata__Sumatra_");

    // Select three rows: the header row of the collapsed group, the row above it and the row below it.
    GTUtilsMSAEditorSequenceArea::selectSequence("Conocephalus_percaudata");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::selectSequence("Mecopoda_elongata__Ishigaki__J");
    GTUtilsMSAEditorSequenceArea::selectSequence("Mecopoda_sp.__Malaysia_");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Click the arrow nearby the group header row to expand the collapsed group.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Mecopoda_elongata__Ishigaki__J");

    // Expected state: the group is expanded, four rows are selected: three rows that you've selected manually and the row that was hidden.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 12, 604, 4));

    // Press the up arrow key on the keyboard.
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    // Expected state: the selection is moved up to one row, it's height is still 4.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 11, 604, 4));
}

GUI_TEST_CLASS_DEFINITION(test_5872) {
    GTLogTracer lt;

    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    3. Select two first rows in the Name List Area.
    GTUtilsMsaEditor::selectRows(0, 1, GTGlobals::UseMouse);

    //    4. Click to the position (3, 3).
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(2, 2));

    //    Expected state: there is no message in the log starting with 'ASSERT: "!isInRange'.
    CHECK_SET_ERR(!lt.hasMessage("ASSERT"), "Unexpected log message");
}

GUI_TEST_CLASS_DEFINITION(test_5898) {
    //    1. Open the sequence and the corresponding annotations in separate file:
    //        primer3/NM_001135099_no_anns.fa
    //        primer3/NM_001135099_annotations.gb
    //    2. Add opened annotaions to the sequence
    //    3. Open Primer3 dialog
    //    4. Check RT-PCR and pick primers
    //    Expected state: no error in the log, exon annotations in separate file were successfully found
    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "/_common_data/primer3", "NM_001135099_no_anns.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTFileDialog::openFile(testDir + "/_common_data/primer3", "NM_001135099_annotations.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NM_001135099");
    if (!GTUtilsSequenceView::getPanOrDetView()->isVisible()) {
        GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));
    }

    QModelIndex idx = GTUtilsProjectTreeView::findIndex("NM_001135099 features");
    QWidget* sequence = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(sequence != nullptr, "Sequence widget not found");

    GTUtilsDialog::add(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(idx, sequence);

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    Primer3DialogFiller::Primer3Settings settings;
    settings.rtPcrDesign = true;
    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5899) {
    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("NM_001135099", 0));

    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    Primer3DialogFiller::Primer3Settings settings;
    settings.rtPcrDesign = true;
    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::findItem("pair 1  (0, 2)");
    GTUtilsAnnotationsTreeView::findItem("pair 2  (0, 2)");
    GTUtilsAnnotationsTreeView::findItem("pair 3  (0, 2)");
    GTUtilsAnnotationsTreeView::findItem("pair 4  (0, 2)");
    GTUtilsAnnotationsTreeView::findItem("pair 5  (0, 2)");
}

GUI_TEST_CLASS_DEFINITION(test_5903) {
    // 1. Open 'human_T1.fa'
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTKeyboardDriver::keyClick('p');
            GTKeyboardDriver::keyClick('r');
            GTKeyboardDriver::keyClick('o');
            GTKeyboardDriver::keyClick('p');

            GTRadioButton::click(GTWidget::findRadioButton("rbGenbankFormat", dialog));
            GTLineEdit::setText(GTWidget::findLineEdit("leAnnotationName", dialog), "NewAnn");

            GTLineEdit::setText(GTWidget::findLineEdit("leLocation", dialog), "100..200");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 2. Create annotation with "propertide" type.
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // Expected type - propeptide
    QString type = GTUtilsAnnotationsTreeView::getAnnotationType("NewAnn");
    CHECK_SET_ERR(type == "propeptide", QString("incorrect type, expected: Propeptide, current: %1").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_5905) {
    //    1. Open 'human_T1.fa'
    //    2. Launch Primer3 search (set results count to 50)
    //    Expected state: check GC content of the first result pair, it should be 55 and 33

    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    ADVSingleSequenceWidget* wgt = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(wgt != nullptr, "ADVSequenceWidget is NULL");

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(new Primer3DialogFiller());
    GTWidget::click(wgt, Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems("top_primers");
    CHECK_SET_ERR(items.size() >= 2, "Wrong annotations count");
    GTUtilsAnnotationsTreeView::selectItems({items[0]});
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("gc%", items[0]) == "55", "wrong gc percentage");
    GTUtilsAnnotationsTreeView::selectItems({items[1]});
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("gc%", items[1]) == "55", "wrong gc percentage");
}

GUI_TEST_CLASS_DEFINITION(test_5941) {
    GTFileDialog::openFile(testDir + "_common_data/regression/5941/5941.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    FindEnzymesDialogFiller::selectEnzymes({"DraI"});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::checkNoAnnotations();

    GTUtilsSequenceView::insertSubsequence(3, "A");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that DraI annotation is present.
    GTUtilsAnnotationsTreeView::findItem("DraI");
}

GUI_TEST_CLASS_DEFINITION(test_5947) {
    //    1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Set focus to the first sequence.
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber());

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto startLineEdit = GTWidget::findLineEdit("start_edit_line", dialog);
            GTLineEdit::setText(startLineEdit, "10");

            auto endLineEdit = GTWidget::findLineEdit("end_edit_line", dialog);
            GTLineEdit::setText(endLineEdit, "50");

            GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmComboBox", dialog), "PsiPred");
            GTUtilsDialog::waitForDialog(new LicenseAgreementDialogFiller());
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished();

            auto resultsTable = GTWidget::findTableWidget("resultsTable", dialog);
            const int resultsCount = resultsTable->rowCount();
            CHECK_SET_ERR(resultsCount == 3, QString("Unexpected results count: expected %1, got %2").arg(4).arg(resultsCount));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new PredictSecondaryStructureDialogFiller(new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Predict secondary structure");
}

GUI_TEST_CLASS_DEFINITION(test_5948) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");

    // 2. Make sure the editing mode is switched off.
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    QAction* editMode = GTAction::findActionByText("Switch on the editing mode");
    CHECK_SET_ERR(editMode != nullptr, "Cannot find Edit mode action");
    if (editMode->isChecked()) {
        GTWidget::click(GTAction::button(editMode));
    }

    // 3. Copy a sequence region
    GTUtilsSequenceView::selectSequenceRegion(10, 20);
    GTKeyboardUtils::copy();

    // 4. "Copy/Paste > Paste sequence" is disabled in the context menu.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste", "Paste sequence"}, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    MWMDIWindow* mdiWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_5950) {
    //    1. Open 'human_T1.fa'
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Switch on the editing mode.
    QAction* editMode = GTAction::findActionByText("Switch on the editing mode");
    CHECK_SET_ERR(editMode != nullptr, "Cannot find Edit mode action");
    GTWidget::click(GTAction::button(editMode));

    QPoint point = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(QPoint(point.x() + 100, point.y()));
    GTMouseDriver::press();

    for (int i = 0; i < 2; i++) {
        for (int j = 1; j < 5; j++) {
            point = GTMouseDriver::getMousePosition();
            int multiplier = i == 0 ? 1 : (-1);
            GTMouseDriver::moveTo(QPoint(point.x() + multiplier * 16, point.y()));
            QVector<U2Region> selection = GTUtilsSequenceView::getSelection();
            CHECK_SET_ERR(selection.size() == 1, "Incorrect selection");

            U2Region sel = selection.first();
            CHECK_SET_ERR(sel.length != 0, "Selection length is 0");
        }
    }

    GTMouseDriver::release();
}

GUI_TEST_CLASS_DEFINITION(test_5970) {
    // 1. Open "_common_data/clustal/amino_from_wikipedia.aln".

    GTFileDialog::openFile(testDir + "_common_data/clustal/amino_from_wikipedia.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Expected state: it has the "Standard amino acid alphabet" (see the "General" OP tab).
    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not Amino!");

    // 3. Select (8; 1) (8; 3) rectangle.

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 0), QPoint(7, 2));

    // 4. Press Ctrl + C.
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // 5. Press Ctrl + V.
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // 6. Expected state: the alphabet is "Standard amino acid"
    const bool isAlphabetAminoAfter = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAminoAfter, "Alphabet is not Amino!");
}

GUI_TEST_CLASS_DEFINITION(test_5972_1) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 1. Open file _common_data/regression/5972/5972_1.uwl
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/5972/5972_1.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set input file _common_data/regression/5972/seq_with_orfs.fa
    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/regression/5972/seq_with_orfs.fa");

    // 3. Set output file sandBoxDir "/test_5972_1.csv"
    GTUtilsWorkflowDesigner::click("Write Annotations");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_5972_1.csv", GTUtilsWorkflowDesigner::textValue);

    GTLogTracer tr;
    // 4. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: There are no errors in the log
    CHECK_SET_ERR(!tr.hasErrors(), "Errors in log: " + tr.getJoinedErrorString());

    // Expected: The result file is equal to "_common_data/regression/5972/seq_with_orfs_1.csv"
    bool check = GTFile::equals(testDir + "_common_data/regression/5972/seq_with_orfs_1.csv", QDir(sandBoxDir).absolutePath() + "/test_5972_1.csv");
    CHECK_SET_ERR(check, QString("files are not equal"));
}

GUI_TEST_CLASS_DEFINITION(test_5972_2) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 1. Open file _common_data/regression/5972/5972_2.uwl
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/5972/5972_2.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set input file _common_data/regression/5972/seq_with_orfs.fa
    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/regression/5972/seq_with_orfs.fa");

    // 3. Set output file sandBoxDir "/test_5972_1.csv"
    GTUtilsWorkflowDesigner::click("Write Annotations");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_5972_2.csv", GTUtilsWorkflowDesigner::textValue);

    GTLogTracer lt;

    // 4. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: Error in the log "Sequence names were not saved, the input slot 'Sequence' is empty."
    CHECK_SET_ERR(lt.hasError("Sequence names were not saved, the input slot 'Sequence' is empty."), "Expected error not found");

    // Expected: The result file is equal to "_common_data/regression/5972/seq_with_orfs_1.csv"
    bool check = GTFile::equals(testDir + "_common_data/regression/5972/seq_with_orfs_2.csv", QDir(sandBoxDir).absolutePath() + "/test_5972_2.csv");
    CHECK_SET_ERR(check, QString("files are not equal"));
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
