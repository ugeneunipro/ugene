/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QTableView>
#include <QTableWidget>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>
#include <U2View/MSAEditorTreeViewer.h>
#include <U2View/MaEditorConsensusArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaGraphOverview.h>

#include "GTTestsRegressionScenarios_5001_6000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsDocument.h"
#include "GTUtilsExternalTools.h"
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
#include "GTUtilsSharedDatabaseDocument.h"
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
#include "runnables/ugene/corelibs/U2View/ov_msa/GenerateAlignmentProfileDialogFiller.h"
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
#include "runnables/ugene/plugins/external_tools/BlastAllSupportDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/FormatDBDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SnpEffDatabaseDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SpadesGenomeAssemblyDialogFiller.h"
#include "runnables/ugene/plugins/orf_marker/OrfDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ImportPrimersDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_5004) {
    //1. Open file _common_data/scenarios/_regression/5004/short.fa
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/5004", "short.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_SET_ERR(NULL != sequenceWidget, "sequenceWidget is not present");

    GTWidget::click(os, sequenceWidget);

    GTLogTracer lt;
    // 2. Show DNA Flexibility graph
    // Expected state: no errors in log
    QWidget *graphAction = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget, false);
    Runnable *chooser = new PopupChooser(os, QStringList() << "DNA Flexibility");
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, graphAction);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5012) {
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/scerevisiae.bam2.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/scerevisiae.bam3.sam");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/genbank/JQ040024.1.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Assembly/chrM.fa");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5012_1) {
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/genbank/JQ040024.1.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Assembly/chrM.fa");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012_1.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_5012_2) {
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/scerevisiae.bam2.sam");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012_2.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_5018) {
#ifdef Q_OS_WIN
    const QString homePlaceholder = "%UserProfile%";
#else
    const QString homePlaceholder = "~";
#endif

    //    1. Ensure that there is no "test_5018.fa" file in the home dir.
    const QString homePath = QDir::homePath();
    if (GTFile::check(os, homePath + "/test_5018.fa")) {
        QFile(homePath + "/test_5018.fa").remove();
        CHECK_SET_ERR(!GTFile::check(os, homePath + "/test_5018.fa"), "File can't be removed");
    }

    //    2. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Call context menu on the sequence object in the Project View, select {Export/Import -> Export sequences...} item.
    //    4. Set output path to "~/test_5018.fa" for *nix and "%HOME_DIR%\test_5018.fa" for Windows. Accept the dialog.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export sequences..."));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, homePlaceholder + "/test_5018.fa"));
    GTUtilsProjectTreeView::click(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: "test_5018.fa" appears in the home dir.
    CHECK_SET_ERR(GTFile::check(os, homePath + "/test_5018.fa"), "File was not created");
    GTUtilsDialog::waitForDialog(os, new MessageBoxNoToAllOrNo(os));
    QFile(homePath + "/test_5018.fa").remove();
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_5026) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // 3. Expand the "Mecopoda_elongata_Ishigaki_J" collapsed group.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(os, "Mecopoda_elongata__Ishigaki__J");
    GTGlobals::sleep(1000);

    // 4. Select an inner sequence in the collapsed group. Remove the sequence
    GTUtilsMSAEditorSequenceArea::removeSequence(os, QString("Mecopoda_elongata__Sumatra_"));

    // 5. Expected result: only the selected sequence is removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 1, "The number of sequences remained unchanged.");
    CHECK_SET_ERR(!modifiedNames.contains("Mecopoda_elongata__Sumatra_"), "Removed sequence is present in multiple alignment.");
    CHECK_SET_ERR(modifiedNames.contains("Mecopoda_elongata__Ishigaki__J"), "Sequence Mecopoda_elongata__Ishigaki__J is not present in multiple alignment.");
}

GUI_TEST_CLASS_DEFINITION(test_5027_1) {
    //1. Open preferences and set memory limit per task 500000MB
    //2. Open WD and compose next scheme "Read File URL(s)" -> "SnpEff annotation and filtration"
    //3. Run schema.
    //Expected state : there is problem on dashboard "A problem occurred during allocating memory for running SnpEff."
    class MemorySetter : public CustomScenario {
    public:
        MemorySetter(int _memValue)
            : memValue(_memValue) {
        }
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);

            QSpinBox *memSpinBox = qobject_cast<QSpinBox *>(GTWidget::findWidget(os, "memorySpinBox"));
            CHECK_SET_ERR(memSpinBox != NULL, "No memorySpinBox");
            GTSpinBox::setValue(os, memSpinBox, memValue, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }

    private:
        int memValue;
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MemorySetter(200)));    //200mb
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "SnpEff");
    GTThread::waitForMainThread();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);    // close wizard

    GTUtilsWorkflowDesigner::click(os, "Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::findLabelByText(os, "There is not enough memory to complete the SnpEff execution.", GTUtilsDashboard::getDashboard(os));
}

GUI_TEST_CLASS_DEFINITION(test_5027_2) {
    //1. Open preferences and set memory limit per task 512MB
    //2. Open WD and compose next scheme "Read File URL(s)" -> "SnpEff annotation and filtration"
    //3. Run schema.
    //Expected state : there is problem on dashboard "There is not enough memory to complete the SnpEff execution."
    class MemorySetter : public CustomScenario {
    public:
        MemorySetter(int memValue)
            : memValue(memValue) {
        }
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);

            QSpinBox *memSpinBox = qobject_cast<QSpinBox *>(GTWidget::findWidget(os, "memorySpinBox"));
            CHECK_SET_ERR(memSpinBox != NULL, "No memorySpinBox");
            GTSpinBox::setValue(os, memSpinBox, memValue, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }

    private:
        int memValue;
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MemorySetter(256)));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "SnpEff");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTThread::waitForMainThread();
    GTUtilsWorkflowDesigner::click(os, "Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::findLabelByText(os, "There is not enough memory to complete the SnpEff execution.", GTUtilsDashboard::getDashboard(os));
}

GUI_TEST_CLASS_DEFINITION(test_5029) {
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Plugins...");
    GTGlobals::sleep();
    QTreeWidget *tree = qobject_cast<QTreeWidget *>(GTWidget::findWidget(os, "treeWidget"));
    int numPlugins = tree->topLevelItemCount();
    CHECK_SET_ERR(numPlugins > 10, QString("Not all plugins were loaded. Loaded %1 plugins").arg(numPlugins));
}

GUI_TEST_CLASS_DEFINITION(test_5039) {
    //1. Open "COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Set the consensus type to "Levitsky".
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(200);
    QComboBox *consensusCombo = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "consensusType"));
    CHECK_SET_ERR(consensusCombo != NULL, "consensusCombo is NULL");
    GTComboBox::selectItemByText(os, consensusCombo, "Levitsky");

    //3. Add an additional sequence from file : "test/_common_data/fasta/amino_ext.fa".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/amino_ext.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Open the "Export consensus" OP tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    GTLogTracer l;

    //5. Press "Undo" button.
    GTUtilsMsaEditor::undo(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //6. Press "Redo" button.
    GTUtilsMsaEditor::redo(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state : the tab is successfully updated. No error in log.
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5052) {
    //1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Close the opened sequence view.
    GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    GTGlobals::FindOptions findOptions;
    findOptions.matchPolicy = Qt::MatchContains;
    GTUtilsMdi::closeWindow(os, "NC_", findOptions);

    //3. Click "murine.gb" on Start Page.
    GTWidget::click(os, GTWidget::findLabelByText(os, "murine.gb").first());

    //Expected: The file is loaded, the view is opened.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded(os, "murine.gb"), "The file is not loaded");
    QString title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_5069) {
    //    1. Load workflow "_common_data/regression/5069/crash.uwl".
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/regression/5069/crash.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Set "data/samples/Genbank/murine.gb" as input.
    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Genbank/murine.gb");

    //    3. Launch workflow.
    //    Expected state: UGENE doesn't crash.
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os), "Workflow has finished with problems");
}

GUI_TEST_CLASS_DEFINITION(test_5082) {
    GTLogTracer l;
    // 1. Open "_common_data/clustal/big.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Align it with MUSCLE.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align"
                                                                              << "Align with MUSCLE..."));
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Expected: Error notification appears with a correct human readable error. There is a error in log wit memory requirements.
    GTUtilsNotifications::waitForNotification(os, true, "There is not enough memory to align these sequences with MUSCLE.");
    GTUtilsDialog::waitAllFinished(os);
    CHECK_SET_ERR(l.checkMessage("Not enough resources for the task, resource name:"), "No default error in log");
}

GUI_TEST_CLASS_DEFINITION(test_5090) {
    //    1. Open "_common_data/genbank/join_complement_ann.gb".
    //    Expected state: the file is successfully opened;
    //                    a warning appears. It contains next message: "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand."
    //                    there are two annotations: 'just_an_annotation' (40..50) and 'join_complement' (join(10..15,20..25)). // the second one should have another location after UGENE-3423 will be done

    GTLogTracer logTracer;
    GTUtilsNotifications::waitForNotification(os, false, "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.");

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/join_complement_ann.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, logTracer, "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.");

    GTUtilsMdi::activateWindow(os, "join_complement_ann [s] A_SEQ_1");

    const QString simpleAnnRegion = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "just_an_annotation");
    CHECK_SET_ERR("40..50" == simpleAnnRegion, QString("An incorrect annotation region: expected '%1', got '%2'").arg("40..50").arg(simpleAnnRegion));
    const QString joinComplementAnnRegion = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "join_complement");
    CHECK_SET_ERR("join(10..15,20..25)" == joinComplementAnnRegion, QString("An incorrect annotation region: expected '%1', got '%2'").arg("join(10..15,20..25)").arg(simpleAnnRegion));
}

GUI_TEST_CLASS_DEFINITION(test_5110) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, QString("NC_001363 features [murine.gb]")));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS  (0, 4)"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    QList<QTreeWidgetItem *> items = GTUtilsAnnotationsTreeView::findItems(os, "CDS");
    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "CDS");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue(os, "codon_start", items[0]) == "1", "wrong qualifier value");

    //    4. Open the "Annotation highlighting" OP widget.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT"));

    QCheckBox *showAnnotations = GTWidget::findExactWidget<QCheckBox *>(os, "checkShowHideAnnots");
    GTCheckBox::setChecked(os, showAnnotations, false);
    GTCheckBox::setChecked(os, showAnnotations, true);

    QTreeWidgetItem *item = items[0];
    QBrush expectedBrush = QApplication::palette().brush(QPalette::Active, QPalette::Foreground);
    QBrush actualBrush = item->foreground(1);
    CHECK_SET_ERR(expectedBrush == actualBrush, "wrong item color");
}

GUI_TEST_CLASS_DEFINITION(test_5128) {
    //1. Open any 3D structure.
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //2. Context menu: { Molecular Surface -> * }.
    //3. Select any model.
    //Current state: crash
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Molecular Surface"
                                                                              << "SAS"));
    GTWidget::click(os, GTWidget::findWidget(os, "1-1CF7"), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5130) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *msaEditorView = GTWidget::findWidget(os, "msa_editor_COI");
    MsaEditorWgt *msaWidget = qobject_cast<MsaEditorWgt *>(msaEditorView);
    MaEditorNameList *nameListWidget = msaWidget->getEditorNameList();
    MaEditorConsensusArea *consWidget = msaWidget->getConsensusArea();
    MaEditorSequenceArea *seqAreaWidget = msaWidget->getSequenceArea();

    QFont nameListFontBefore = nameListWidget->getFont(false);
    nameListFontBefore.setItalic(false);

    //    2. press "change font button" on toolbar
    GTUtilsDialog::waitForDialog(os, new FontDialogFiller(os));
    QAbstractButton *change_font = GTAction::button(os, "Change Font");
    GTWidget::click(os, change_font);
    GTGlobals::sleep(500);

    QFont nameListFontAfter = nameListWidget->getFont(false);
    nameListFontAfter.setItalic(false);
    QFont consFontAfter = consWidget->getDrawSettings().font;
    QFont seqAreaFontAfter = seqAreaWidget->getFont();

    CHECK_SET_ERR(nameListFontBefore != nameListFontAfter, "Expected fonts to be NOT equal");
    CHECK_SET_ERR(nameListFontAfter == consFontAfter && consFontAfter == seqAreaFontAfter,
                  "Expected fonts to be equal: NameList: " + nameListFontAfter.toString() +
                      ", Cons: " + consFontAfter.toString() + ", SeqArea: " + seqAreaFontAfter.toString());
}

GUI_TEST_CLASS_DEFINITION(test_5137) {
    // 1. Open document test/_common_data/clustal/big.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Add a big sequence.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/", "PF07724_full_family.fa"));
    QAbstractButton *alignButton = GTAction::button(os, "Align sequence(s) to this alignment");
    CHECK_SET_ERR(alignButton != nullptr, "MSA \"Align sequence(s) to this alignment\" action not found");
    GTWidget::click(os, alignButton);

    // 3. Delete the original alignment and wait for the error in the log.
    GTUtilsNotifications::waitForNotification(os, true, "A problem occurred during adding sequences. The multiple alignment is no more available.");
    GTUtilsProjectTreeView::click(os, "COI");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsDialog::waitAllFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os, 20000);
}

GUI_TEST_CLASS_DEFINITION(test_5138_1) {
    //1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/msa/big_aln.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Do MSA area context menu->Statistics->generate distance matrix
    //    Expected state: notification about low memory has appeared
    Runnable *dis = new DistanceMatrixDialogFiller(os, DistanceMatrixDialogFiller::NONE, testDir + "_common_data/scenarios/sandbox/matrix.html");
    GTUtilsDialog::waitForDialog(os, dis);
    Runnable *pop = new PopupChooser(os, QStringList() << MSAE_MENU_STATISTICS << "Generate distance matrix", GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, pop);
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsNotifications::waitForNotification(os, true, "not enough memory");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5138_2) {
    //    1. Open document test/_common_data/clustal/big.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/5138", "big_5138.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Do MSA area context menu->Statistics->generate grid profile
    //    Expected state: notification about low memory has appeared
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_STATISTICS << "Generate grid profile", GTGlobals::UseKey));
    GTUtilsDialog::waitForDialog(os, new GenerateAlignmentProfileDialogFiller(os, true, GenerateAlignmentProfileDialogFiller::NONE, testDir + "_common_data/scenarios/sandbox/stat.html"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsNotifications::waitForNotification(os, true, "not enough memory");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5199) {
    //    1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Set focus to the first sequence.
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    //    3. Click "Predict secondary structure" button on the toolbar;
    //    4. Select "PsiPred" algorithm.
    //    5. Click "OK" button.
    //    Expected state: UGENE doesn't crash, 4 results are found.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "algorithmComboBox", dialog), "PsiPred");
            GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new LicenseAgreementDialogFiller(os));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished(os);

            QTableWidget *resultsTable = GTWidget::findExactWidget<QTableWidget *>(os, "resultsTable", dialog);
            GTGlobals::sleep();
            CHECK_SET_ERR(NULL != resultsTable, "resultsTable is NULL");
            const int resultsCount = resultsTable->rowCount();
            CHECK_SET_ERR(4 == resultsCount, QString("Unexpected results count: expected %1, got %2").arg(4).arg(resultsCount));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PredictSecondaryStructureDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Predict secondary structure");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5208) {
    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa"
    //    and accept the dialog.
    class ImportFromMultifasta : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");
            ImportPrimersDialogFiller::setImportTarget(os, ImportPrimersDialogFiller::LocalFiles);
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/fasta/random_primers.fa");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new ImportPrimersDialogFiller(os, new ImportFromMultifasta));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Import);

    //    4. Check log.
    //    Expected state: the library contains four primers, log contains no errors.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5211) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Select the first sequence.
    GTUtilsMsaEditor::clickSequenceName(os, "Phaneroptera_falcata");

    //    3. Copy it to the clipboard.
    GTKeyboardUtils::copy(os);

//    4. Press the next key sequence:
//        Windows and Linux: Shift+Ins
//        macOS: Meta+Y
#ifndef Q_OS_MAC
    GTKeyboardUtils::paste(os);
#else
    GTKeyboardDriver::keyClick('y', Qt::MetaModifier);
#endif
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: a new sequence is added to the alignment. There are no new objects and documents in the Project View.
    int expectedSequencesCount = 19;
    int sequencesCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(expectedSequencesCount == sequencesCount,
                  QString("Incorrect count of sequences after the first insertion: expected %1, got %2")
                      .arg(expectedSequencesCount)
                      .arg(sequencesCount));

    const int expectedDocumentsCount = 2;
    int documentsCount = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait(os, "", QModelIndex(), 2).size();
    CHECK_SET_ERR(expectedDocumentsCount == documentsCount,
                  QString("Incorrect count of items in the Project View after the first insertion: expected %1, got %2")
                      .arg(expectedDocumentsCount)
                      .arg(documentsCount));

    //    5. Press the next key sequence:
    //        Windows and Linux: Ctrl+V
    //        macOS: Cmd+V
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);    // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: one more new sequence is added to the alignment. There are no new objects and documents in the Project View.
    expectedSequencesCount = 20;
    sequencesCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(expectedSequencesCount == sequencesCount,
                  QString("Incorrect count of sequences after the second insertion: expected %1, got %2")
                      .arg(expectedSequencesCount)
                      .arg(sequencesCount));

    documentsCount = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait(os, "", QModelIndex(), 2).size();
    CHECK_SET_ERR(expectedDocumentsCount == documentsCount,
                  QString("Incorrect count of items in the Project View after the second insertion: expected %1, got %2")
                      .arg(expectedDocumentsCount)
                      .arg(documentsCount));
}

GUI_TEST_CLASS_DEFINITION(test_5216) {
    // 1. Connect to the public database
    //GTUtilsSharedDatabaseDocument::connectToUgenePublicDatabase(os);
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTLogTracer lt;
    // 2. Type to the project filter field "acct" then "acctt"
    GTUtilsProjectTreeView::filterProjectSequental(os, QStringList() << "acct"
                                                                     << "accttt",
                                                   true);
    GTGlobals::sleep(3500);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //GTUtilsProjectTreeView::filterProject(os, "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
}

GUI_TEST_CLASS_DEFINITION(test_5220) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::TreeSettings);

    QDir().mkdir(QFileInfo(sandBoxDir + "test_5220/COI.nwk").dir().absolutePath());
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_5220/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::click(os, "COI.nwk");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(1000);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    QDir().mkdir(QFileInfo(sandBoxDir + "test_5220/COI1.nwk").dir().absolutePath());
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_5220/COI1.nwk", 0, 0, true));

    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool isTabOpened = GTUtilsOptionPanelMsa::isTabOpened(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    CHECK_SET_ERR(!isTabOpened, "The 'PairwiseAlignment' tab is unexpectedly opened");
}

GUI_TEST_CLASS_DEFINITION(test_5227) {
    GTUtilsPcr::clearPcrDir(os);

    //1. Open "samples/Genbank/CVU55762.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    //3. Set next parameters:
    // the first primer : TTCTGGATTCA
    // the first primer mismatches : 15
    // the second primer : CGGGTAG
    // the second primer mismatches : 12
    // 3' perfect match: 10
    // Maximum product : 100 bp
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTCTGGATTCA");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "CGGGTAG");

    GTUtilsPcr::setMismatches(os, U2Strand::Direct, 15);
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 12);

    QSpinBox *perfectSpinBox = dynamic_cast<QSpinBox *>(GTWidget::findWidget(os, "perfectSpinBox"));
    GTSpinBox::setValue(os, perfectSpinBox, 10, GTGlobals::UseKeyBoard);

    QSpinBox *productSizeSpinBox = dynamic_cast<QSpinBox *>(GTWidget::findWidget(os, "productSizeSpinBox"));
    GTSpinBox::setValue(os, productSizeSpinBox, 100, GTGlobals::UseKeyBoard);

    //4. Find products
    //Expected state: log shouldn't contain errors
    GTLogTracer lt;
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5231) {
    //1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Run Smith-waterman search using:
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");
            //pattern: "ATCGAT"; note that pattern length is 6.
            GTTextEdit::setText(os, GTWidget::findExactWidget<QTextEdit *>(os, "teditPattern", dialog), "K*KTPPVGGKLA*VTP");

            GTRadioButton::click(os, "radioTranslation", dialog);

            //2.1 Choose Classic algorithm
            QComboBox *comboRealization = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "comboRealization", dialog));
            const int swRealizationIndex = comboRealization->findText("Classic 2");
            GTComboBox::selectItemByIndex(os, comboRealization, swRealizationIndex);

            GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);
            //3. Open tab "Input and output"
            GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);

            //4. Chose in the combobox "Multiple alignment"
            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "resultViewVariants", dialog), "Multiple alignment");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Analyze"
                                                << "Find pattern [Smith-Waterman]...",
                              GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::doubleClickItem(os, "P1_NC_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino");
}

GUI_TEST_CLASS_DEFINITION(test_5246) {
    //1. Open file human_t1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Show ORFs
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Show ORFs"));
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidget *widget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    QList<QTreeWidgetItem *> treeItems = GTTreeWidget::getItems(widget->invisibleRootItem());
    CHECK_SET_ERR(839 == treeItems.size(), "Unexpected annotation count");

    //3. Change amino translation
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTWidget::click(os, GTWidget::findWidget(os, "AminoToolbarButton", GTWidget::findWidget(os, "ADV_single_sequence_widget_0")));
    QMenu *menu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
    GTMenu::clickMenuItemByName(os, menu, QStringList() << "14. The Alternative Flatworm Mitochondrial Code");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state: orfs recalculated
    treeItems = GTTreeWidget::getItems(widget->invisibleRootItem());
    CHECK_SET_ERR(2023 == treeItems.size(), "Unexpected annotation count");
}

GUI_TEST_CLASS_DEFINITION(test_5249) {
    // 1. Open file "_common_data/pdb/1atp.pdb"
    // Expected state: no crash and no errors in the log
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/pdb/1atp.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5252) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open an additional view for the sequence.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Open view"
                                                                              << "Open new view: Sequence View"));
    GTUtilsProjectTreeView::click(os, "murine.gb", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there are two bookmarks: "murine [s] NC_001363" and "murine [s] NC_001363 2".
    GTUtilsBookmarksTreeView::findItem(os, "murine [s] NC_001363");
    GTUtilsBookmarksTreeView::findItem(os, "murine [s] NC_001363 2");

    //    3. Rename the annotation table object.
    GTUtilsProjectTreeView::rename(os, "NC_001363 features", "test_5252");

    //    Expected state: bookmarks are not renamed.
    GTUtilsBookmarksTreeView::findItem(os, "murine [s] NC_001363");
    GTUtilsBookmarksTreeView::findItem(os, "murine [s] NC_001363 2");
}

GUI_TEST_CLASS_DEFINITION(test_5268) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");

    //    2. Create a custom color scheme for the alignment with aan ppropriate alphabet.
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, "test_5268", NewColorSchemeCreator::nucl));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");
    GTUtilsDialog::waitAllFinished(os, 60000);

    //    3. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::Highlighting);

    //    4. Select the custom color scheme.
    GTUtilsOptionPanelMsa::setColorScheme(os, "test_5268");
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Appearance"
                                                                              << "Colors"
                                                                              << "Custom schemes"
                                                                              << "test_5268",
                                                            PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(500);
    //    5. Open {Settings -> Preferences -> Alignment Color Scheme}.
    //    6. Change color of the custom color scheme and click ok.
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, "test_5268", NewColorSchemeCreator::nucl, NewColorSchemeCreator::Change));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");
    GTUtilsDialog::waitAllFinished(os, 60000);

    //    Expected state: the settings dialog closed, new colors are applied for the opened MSA.
    const QString opColorScheme = GTUtilsOptionPanelMsa::getColorScheme(os);
    CHECK_SET_ERR(opColorScheme == "test_5268",
                  QString("An incorrect color scheme is set in option panel: expect '%1', got '%2'")
                      .arg("test_5268")
                      .arg(opColorScheme));

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Appearance"
                                                                              << "Colors"
                                                                              << "Custom schemes"
                                                                              << "test_5268",
                                                            PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_5278) {
    //1. Open file PBR322.gb from samples
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Find next restriction sites "AaaI" and "AagI"
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "AaaI"
                                                                                   << "AagI"));
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsNotifications::waitForNotification(os, false);
    //3. Open report and be sure fragments sorted by length (longest first)
    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Cloning"
                                                << "Digest into fragments...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    QTextEdit *textEdit = dynamic_cast<QTextEdit *>(GTWidget::findWidget(os, "reportTextEdit", GTUtilsMdi::activeWindow(os)));
    CHECK_SET_ERR(textEdit->toPlainText().contains("1:    From AaaI (944) To AagI (24) - 3442 bp "), "Expected message is not found in the report text");
}

GUI_TEST_CLASS_DEFINITION(test_5295) {
    //    1. Open "_common_data/pdb/Helix.pdb".
    GTFileDialog::openFile(os, testDir + "_common_data/pdb/Helix.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: UGENE doesn't crash, the 3d structure is shown.
    QWidget *biostructWidget = GTWidget::findWidget(os, "1-");
    const QImage image1 = GTWidget::getImage(os, biostructWidget);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }
    CHECK_SET_ERR(colors.size() > 1, "Biostruct was not drawn");

    //    2. Call a context menu, open "Render Style" submenu.
    //    Expected state: "Ball-and-Stick" renderer is selected.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Render Style"
                                                                              << "Ball-and-Stick",
                                                            PopupChecker::CheckOptions(PopupChecker::IsChecked)));
    GTWidget::click(os, biostructWidget, Qt::RightButton);

    //    3. Select "Model" renderer. Select "Ball-and-Stick" again.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Render Style"
                                                                              << "Space Fill"));
    GTWidget::click(os, biostructWidget, Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Render Style"
                                                                              << "Ball-and-Stick"));
    GTWidget::click(os, biostructWidget, Qt::RightButton);

    //    Expected state: UGENE doesn't crash, the 3d structure is shown.
    const QImage image2 = GTWidget::getImage(os, biostructWidget);
    colors.clear();
    for (int i = 0; i < image2.width(); i++) {
        for (int j = 0; j < image2.height(); j++) {
            colors << image2.pixel(i, j);
        }
    }
    CHECK_SET_ERR(colors.size() > 1, "Biostruct was not drawn after renderer change");
}

GUI_TEST_CLASS_DEFINITION(test_5314) {
    //1. Open "data/samples/Genbank/CVU55762.gb".
    //2. Search any enzyme on the whole sequence.
    //3. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList defaultEnzymes = QStringList() << "ClaI";
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
                                                                        << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, defaultEnzymes));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "det_view_CVU55762"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer lt;
    GTFileDialog::openFile(os, testDir + "_common_data/abif/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5335) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click "Find ORFs" button on the toolbar.
    class PartialSearchScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modla widget is NULL");

            //    3. Set region to 1..4. Accept the dialog.
            GTLineEdit::setText(os, "end_edit_line", "4", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new OrfDialogFiller(os, new PartialSearchScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find ORFs");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: an empty auto-annotation group is added to the auto-annotation table.
    QTreeWidgetItem *orfGroup = GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 0)");

    //    4. Open the context menu on this group.
    //    Expected state:  there is no "Make auto-annotations persistent" menu item.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Make auto-annotations persistent", PopupChecker::NotExists));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, orfGroup);

    //    5. Click "Find ORFs" button on the toolbar.
    //    6. Accept the dialog.
    GTUtilsDialog::waitForDialog(os, new OrfDialogFiller(os));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find ORFs");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the auto-annotation group is now contains some annotations.
    orfGroup = GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 837)");

    //    7. Open the context menu on this group.
    //    Expected state: there is "Make auto-annotations persistent" menu item, it is enabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Make auto-annotations persistent"));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, orfGroup);

    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_5346) {
    // 1. Open WD
    // 2. Create the workflow: Read File URL(s) - FastQC Quality Control
    // 3. Set empty input file
    // Expected state: there is an error "The input file is empty"
    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    QString emptyFile = sandBoxDir + "test_5346_empty";
    GTFile::create(os, emptyFile);
    WorkflowProcessItem *fileList = GTUtilsWorkflowDesigner::addElement(os, "Read File URL(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, emptyFile);

    WorkflowProcessItem *fastqc = GTUtilsWorkflowDesigner::addElement(os, "FastQC Quality Control");
    GTUtilsWorkflowDesigner::connect(os, fileList, fastqc);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, l, QString("The input file '%1' is empty.").arg(QFileInfo(emptyFile).absoluteFilePath()));
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

    GTLogTracer logTracer;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    WorkflowProcessItem *read = GTUtilsWorkflowDesigner::getWorker(os, "Read alignment");
    WorkflowProcessItem *write = GTUtilsWorkflowDesigner::getWorker(os, "Write alignment");

    GTUtilsWorkflowDesigner::click(os, "Align with MUSCLE");
    GTUtilsWorkflowDesigner::removeItem(os, "Align with MUSCLE");
    GTUtilsWorkflowDesigner::connect(os, read, write);

    GTUtilsWorkflowDesigner::click(os, "Read alignment");
    GTUtilsWorkflowDesigner::addInputFile(os, "Read alignment", dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Discard));

    QToolButton *loadSchemaButton = GTUtilsDashboard::findLoadSchemaButton(os);
    CHECK_SET_ERR(loadSchemaButton, "loadSchemaButton not found");
    GTWidget::click(os, loadSchemaButton);

    CHECK_SET_ERR(!logTracer.hasErrors(), "Errors in log: " + logTracer.getJoinedErrorString());
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

    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/regression/5356/cutadapt_and_trim.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::addInputFile(os, "Read FASTQ Files with Reads 1", testDir + "_common_data/regression/5356/reads.fastq");

    GTUtilsWorkflowDesigner::click(os, "Cut Adapter");
    GTGlobals::sleep(200);
    GTUtilsWorkflowDesigner::setParameter(os, "FASTA file with 3' adapters", QDir(testDir + "_common_data/regression/5356/adapter.fa").absolutePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output folder", "Custom", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Custom folder", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(200);
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5360) {
    //1. Open scheme _common_data / scenarios / _regression / 5360 / 5360.uwl
    //
    //2. Set input fastq file located with path containing non ASCII symbols
    //
    //3. Run workflow
    //Expected state : workflow runs without errors.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/scenarios/_regression/5360/5360.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read FASTQ Files with Reads");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + QString::fromUtf8("_common_data/scenarios/_regression/5360/папка/риды.fastq"), true);

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5363_2) {
    //    1. {Tools --> BLAST --> BLAST+ make database}
    //    2. Set murine.gb as input file
    //    3. Check nucleotide radiobutton
    //    4. Create database
    //    Expected state: database was successfully created
    //    5. Open murine.gb
    //    6. {Analyze --> Query with local BLAST+}
    //    7. Select the created database and accept the dialog
    //    Expected state: blast annotations were found and the annotations locations are equal to 'hit-from' and 'hit-to' qualifier values

    FormatDBSupportRunDialogFiller::Parameters parametersDB;
    parametersDB.inputFilePath = dataDir + "/samples/Genbank/murine.gb";
    parametersDB.outputDirPath = QDir(sandBoxDir).absolutePath();
    GTUtilsDialog::waitForDialog(os, new FormatDBSupportRunDialogFiller(os, parametersDB));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "BLAST"
                                                << "BLAST+ make database...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    BlastAllSupportDialogFiller::Parameters parametersSearch;
    parametersSearch.runBlast = true;
    parametersSearch.dbPath = sandBoxDir + "/murine.nin";

    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(parametersSearch, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Analyze"
                                                << "Query with local BLAST+...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *treeItem = GTUtilsAnnotationsTreeView::findItem(os, "blast result");
    CHECK_SET_ERR(treeItem != NULL, "blast result annotations not found");
    bool ok;
    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "blast result");
    int hitFrom = GTUtilsAnnotationsTreeView::getQualifierValue(os, "hit-to", treeItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-to qualifier value");

    int hitTo = GTUtilsAnnotationsTreeView::getQualifierValue(os, "hit-from", treeItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-from qualifier value");

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(hitFrom, hitTo - hitFrom)),
                  QString("Cannot find blast result [%1, %2]").arg(hitFrom).arg(hitTo));
}

GUI_TEST_CLASS_DEFINITION(test_5367) {
    //    1. Open "_common_data/bam/accepted_hits_with_gaps.bam"
    //    2. Export coverage in 'Per base' format
    //    Expected state: gaps are not considered "to cover, the result file is qual to "_common_data/bam/accepted_hits_with_gaps_coverage.txt"

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "/test_5367.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/accepted_hits_with_gaps.bam");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<ExportCoverageDialogFiller::Action> actions;
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir(sandBoxDir).absolutePath() + "/test_5367_coverage.txt");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, QVariant());

    GTUtilsDialog::waitForDialog(os, new ExportCoverageDialogFiller(os, actions));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export coverage..."));
    GTUtilsAssemblyBrowser::callContextMenu(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTFile::equals(os, sandBoxDir + "/test_5367_coverage.txt", testDir + "/_common_data/bam/accepted_hits_with_gaps_coverage.txt"), "Exported coverage is wrong!");
}

GUI_TEST_CLASS_DEFINITION(test_5377) {
    //    1. Open file "_common_data/genbank/70Bp_new.gb".
    //    2. Search for restriction site HinFI.
    //    3. Digest into fragments, then reconstruct the original molecule.
    //    Expected state: the result sequence is equal to the original sequence. Fragments annotations have the same positions and lengths.
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/70Bp_new.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "HinfI"));
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Cloning"
                                                << "Digest into fragments...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "Fragment 1", U2Region(36, 35)), "Fragment 1 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "Fragment 2", U2Region(1, 24)), "Fragment 2 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "Fragment 3", U2Region(28, 5)), "Fragment 3 is incorrect or not found");

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            GTWidget::click(os, GTWidget::findWidget(os, "takeAllButton"));

            QTreeWidget *tree = dynamic_cast<QTreeWidget *>(GTWidget::findWidget(os, "molConstructWidget"));
            GTTreeWidget::click(os, GTTreeWidget::findItem(os, tree, "Blunt"));

            GTWidget::click(os, GTWidget::findWidget(os, "downButton"));
            GTWidget::click(os, GTWidget::findWidget(os, "downButton"));

            QTabWidget *tabWidget = GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog);
            CHECK_SET_ERR(tabWidget != NULL, "tabWidget not found");
            GTTabWidget::clickTab(os, tabWidget, "Output");

            QLineEdit *linEdit = GTWidget::findExactWidget<QLineEdit *>(os, "filePathEdit");
            CHECK_SET_ERR(linEdit != NULL, "filePathEdit not found");
            GTLineEdit::setText(os, linEdit, QFileInfo(sandBoxDir + "test_5377").absoluteFilePath());

            GTUtilsDialog::clickButtonBox(os, QApplication::activeModalWidget(), QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ConstructMoleculeDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Cloning"
                                                << "Construct molecule...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsSequenceView::getSeqWidgetByNumber(os)->getSequenceLength() == 70, "The result length of the constructed molecule is wrong");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "A sequence Fragment 1", U2Region(36, 35)), "Constructed molecule: Fragment 1 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "A sequence Fragment 2", U2Region(1, 24)), "Constructed molecule: Fragment 2 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "A sequence Fragment 3", U2Region(28, 5)), "Constructed molecule: Fragment 3 is incorrect or not found");
}

GUI_TEST_CLASS_DEFINITION(test_5371) {
    //1. Open bam assembly with index with path containing non ASCII symbols
    //Expected state: assembly opened successfully

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "5371.bam.ugenedb"));
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/5371/папка/", "асс ссембли.bam", GTFileDialogUtils::Open, GTGlobals::UseKey, GTFileDialogUtils::CopyPaste);
    GTUtilsDialog::waitForDialog(os, ob);
    ob->openFileDialog();

    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5382) {
    //1. Open an alignment.
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Export as image"));
    GTUtilsDialog::waitForDialog(os, new ExportMsaImage(os, testDir + "_common_data/scenarios/sandbox/test_5382/test_5382.png"));

    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
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

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "/_common_data/reads/wrong_order/align_bwa_mem.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::addInputFile(os, "File List 1", testDir + "/_common_data/reads/wrong_order/e_coli_mess_1.fastq");
    GTUtilsWorkflowDesigner::addInputFile(os, "File List 2", testDir + "/_common_data/reads/wrong_order/e_coli_mess_2.fastq");

    GTUtilsWorkflowDesigner::click(os, "Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Output folder", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file name", "test_5412", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Reference genome", testDir + "/_common_data/e_coli/NC_008253.fa", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Filter unpaired reads", false, GTUtilsWorkflowDesigner::comboValue);

    GTLogTracer l;
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    CHECK_SET_ERR(l.checkMessage("exited with code 1"), "No message about failed start of BWA MEM");

    GTToolbar::clickButtonByTooltipOnToolbar(os, "mwtoolbar_activemdi", "Show workflow");

    GTUtilsWorkflowDesigner::click(os, "Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Filter unpaired reads", true, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file name", "test_5412_1", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.checkMessage("5 pairs are complete, 6 reads without a pair were found in files"), "No message about filtered reads");
}

GUI_TEST_CLASS_DEFINITION(test_5417) {
    //      1. Open "data/samples/Genbank/murine.gb".
    //      2. Open "data/samples/Genbank/srs.gb".
    //      3. Build doplot with theese files and try to save it.
    //      Expected state: warning message ox appeared
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os));
    Runnable *filler2 = new BuildDotPlotFiller(os, dataDir + "samples/Genbank/sars.gb", dataDir + "samples/Genbank/murine.gb");
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Build dotplot...");

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Dotplot"
                                                                        << "Save/Load"
                                                                        << "Save"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "dotplot widget"));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5425) {
    //1. Open de novo assembly dialog
    //2. Fill it and run
    //3. Open dialog again
    //Expected state: all settings except files with reads was saved from previous run
    class SpadesDialogSettingsChecker : public SpadesGenomeAssemblyDialogFiller {
    public:
        SpadesDialogSettingsChecker(HI::GUITestOpStatus &os, QString lib, QString datasetType, QString runningMode, QString kmerSizes, int numThreads, int memLimit)
            : SpadesGenomeAssemblyDialogFiller(os, lib, QStringList(), QStringList(), "", datasetType, runningMode, kmerSizes, numThreads, memLimit) {
        }
        virtual void commonScenario() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QComboBox *combo = GTWidget::findExactWidget<QComboBox *>(os, "modeCombo", dialog);
            CHECK_SET_ERR(combo->currentText() == runningMode, "running mode doesn't match");

            combo = GTWidget::findExactWidget<QComboBox *>(os, "typeCombo", dialog);
            CHECK_SET_ERR(combo->currentText() == datasetType, "type mode doesn't match");

            QLineEdit *lineEdit = GTWidget::findExactWidget<QLineEdit *>(os, "kmerEdit", dialog);
            CHECK_SET_ERR(lineEdit->text() == kmerSizes, "kmer doesn't match");

            QSpinBox *spinbox = GTWidget::findExactWidget<QSpinBox *>(os, "memlimitSpin", dialog);
            CHECK_SET_ERR(spinbox->text() == QString::number(memLimit), "memlimit doesn't match");

            spinbox = GTWidget::findExactWidget<QSpinBox *>(os, "numThreadsSpinbox", dialog);
            CHECK_SET_ERR(spinbox->text() == QString::number(numThreads), "threads doesn't match");

            combo = GTWidget::findExactWidget<QComboBox *>(os, "libraryComboBox", dialog);
            CHECK_SET_ERR(combo->currentText() == library, QString("library doesn't match, expected %1, actual:%2.").arg(library).arg(combo->currentText()));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new SpadesGenomeAssemblyDialogFiller(os, "Paired-end (Interlaced)", QStringList() << testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq", QStringList(), sandBoxDir, "Single Cell", "Error correction only", "aaaaa", 1, 228));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Reads de novo assembly (with SPAdes)...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SpadesDialogSettingsChecker(os, "Paired-end (Interlaced)", "Single Cell", "Error correction only", "aaaaa", 1, 228));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Reads de novo assembly (with SPAdes)...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5431) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    // 2. Remove all columns except the first one.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(1, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();

    GTUtilsMsaEditor::toggleCollapsingMode(os);

    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed(os, "Tettigonia_viridissima"),
                  "1 Tettigonia_viridissima is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed(os, "Conocephalus_discolor"),
                  "2 Conocephalus_discolor is not collapsed");

    GTUtilsMSAEditorSequenceArea::removeSequence(os, "Phaneroptera_falcata");

    // 3. Expected state: first group is removed
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 4, "Wrong rows number");
}

GUI_TEST_CLASS_DEFINITION(test_5447_1) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open the context menu on the "NC_001363 features" object in the project view.
    //    3. Select "Export/Import" -> "Export annotations..." menu item.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal dialog is NULL");

            //    Expected state: an "Export Annotations" dialog opens, "GenBank" format is selected, there is an "Add to project" checkbox, it is enabled and checked.
            GTComboBox::checkCurrentValue(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), "GenBank");

            QCheckBox *addToProjectCheck = GTWidget::findExactWidget<QCheckBox *>(os, "addToProjectCheck", dialog);
            CHECK_SET_ERR(NULL != addToProjectCheck, "addToProjectCheck is NULL");
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(addToProjectCheck->isEnabled(), "addToProjectCheck is not enabled");
            CHECK_SET_ERR(addToProjectCheck->isChecked(), "addToProjectCheck is not checked by default");

            //    4. Set a valid result file path, accept the dialog.
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "fileNameEdit", dialog), sandBoxDir + "test_5447_1.gb");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export annotations..."));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, new Scenario()));
    GTUtilsProjectTreeView::callContextMenu(os, "NC_001363 features", "murine.gb");

    //    Expected state: the annotations were exported, a new document with an annotations table object was added to the project.
    const qint64 fileSize = GTFile::getSize(os, sandBoxDir + "test_5447_1.gb");
    CHECK_SET_ERR(0 != fileSize, "Result file is empty");

    const QModelIndex annotationsTableObjectIndex = GTUtilsProjectTreeView::findIndex(os, "NC_001363 features", GTUtilsProjectTreeView::findIndex(os, "test_5447_1.gb"));
    CHECK_SET_ERR(annotationsTableObjectIndex.isValid(), "Annotation object not found");

    //    5. Add the object to the "murine.gb" sequence.
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, annotationsTableObjectIndex, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    GTGlobals::sleep(1000);

    //    Expected state: all annotations are doubled.
    const QStringList oldGroups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [murine.gb]");
    const QStringList newGroups = GTUtilsAnnotationsTreeView::getGroupNames(os, "NC_001363 features [test_5447_1.gb]");
    bool oldCommentGroupExists = false;
    foreach (const QString &oldGroup, oldGroups) {
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
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open the context menu on the "NC_001363 features" object in the project view.
    //    3. Select "Export/Import" -> "Export annotations..." menu item.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal dialog is NULL");

            //    Expected state: an "Export Annotations" dialog opens, "GenBank" format is selected, there is an "Add to project" checkbox, it is enabled and checked.
            GTComboBox::checkCurrentValue(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), "GenBank");

            QCheckBox *addToProjectCheck = GTWidget::findExactWidget<QCheckBox *>(os, "addToProjectCheck", dialog);
            CHECK_SET_ERR(NULL != addToProjectCheck, "addToProjectCheck is NULL");
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(addToProjectCheck->isEnabled(), "addToProjectCheck is not enabled");
            CHECK_SET_ERR(addToProjectCheck->isChecked(), "addToProjectCheck is not checked by default");

            //    4. Select "CSV" format.
            //    Expected state: a "CSV" format is selected, the "Add to project" checkbox is disabled.
            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), "CSV");
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(!addToProjectCheck->isEnabled(), "addToProjectCheck is unexpectedly enabled");

            //    5. Set a valid result file path, accept the dialog.
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "fileNameEdit", dialog), sandBoxDir + "test_5447_2.csv");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export annotations..."));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, new Scenario()));
    GTUtilsProjectTreeView::callContextMenu(os, "NC_001363 features", "murine.gb");

    //    Expected state: the annotations were exported, there are no new documents in the project.
    const qint64 fileSize = GTFile::getSize(os, sandBoxDir + "test_5447_2.csv");
    CHECK_SET_ERR(0 != fileSize, "Result file is empty");

    const bool newDocumentExists = GTUtilsProjectTreeView::checkItem(os, "test_5447_2.csv", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(!newDocumentExists, "New document unexpectedly exists");
}

GUI_TEST_CLASS_DEFINITION(test_5447_3) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open the context menu on the "NC_001363 features" object in the project view.
    //    3. Select "Export/Import" -> "Export annotations..." menu item.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal dialog is NULL");

            //    Expected state: an "Export Annotations" dialog opens, "GenBank" format is selected, there is an "Add to project" checkbox, it is enabled and checked.
            GTComboBox::checkCurrentValue(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), "GenBank");

            QCheckBox *addToProjectCheck = GTWidget::findExactWidget<QCheckBox *>(os, "addToProjectCheck", dialog);
            CHECK_SET_ERR(NULL != addToProjectCheck, "addToProjectCheck is NULL");
            CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
            CHECK_SET_ERR(addToProjectCheck->isEnabled(), "addToProjectCheck is not enabled");
            CHECK_SET_ERR(addToProjectCheck->isChecked(), "addToProjectCheck is not checked by default");

            //    4. Select each format.
            //    Expected state: the "Add to project" checkbox becomes disabled only for CSV format.
            const QStringList formats = GTComboBox::getValues(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog));
            foreach (const QString &format, formats) {
                GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), format);
                CHECK_SET_ERR(addToProjectCheck->isVisible(), "addToProjectCheck is not visible");
                CHECK_SET_ERR(addToProjectCheck->isEnabled() != (format == "CSV"), QString("addToProjectCheck is unexpectedly enabled for format '%1'").arg(format));
            }

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export annotations..."));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, new Scenario()));
    GTUtilsProjectTreeView::callContextMenu(os, "NC_001363 features", "murine.gb");
}

GUI_TEST_CLASS_DEFINITION(test_5469) {
    // 1. Open two different GenBank sequences in one Sequence view.
    // 2. Select two different annotations (one from the first sequence, and one from the second sequence) using the "Ctrl" keyboard button.
    // Extected state: there is no crash
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Add to view"
                                                                              << "Add to view: murine [s] NC_001363"));
    GTUtilsProjectTreeView::click(os, "NC_004718", Qt::RightButton);
    GTGlobals::sleep();

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::clickAnnotationDet(os, "5'UTR", 1, 1);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size() == 2, QString("Wrong number of selected annotations expect %1, got %2").arg("2").arg(GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size()));
}

GUI_TEST_CLASS_DEFINITION(test_5492) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);

    //2. Select last symbol of the read and insert some gaps, until reference will increase for a few symbols
    MultipleAlignmentRowData *row = GTUtilsMcaEditor::getMcaRow(os, 0);
    int end = row->getCoreStart() + row->getCoreLength() - 1;
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(end, 0));

    int i = 15;
    while (i != 0) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        GTGlobals::sleep(300);
        i--;
    }

    //4. Select the last symbol again, press "Insert character" and insert gap
    row = GTUtilsMcaEditor::getMcaRow(os, 0);
    end = row->getCoreStart() + row->getCoreLength() - 1;
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(end, 0));
    GTGlobals::sleep(1000);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Replace character/gap");
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(1000);

    //Expected : all gaps since a place when you started to insert, will turn into trailing
    row = GTUtilsMcaEditor::getMcaRow(os, 0);
    int newRowLength = row->getCoreStart() + row->getCoreLength() - 1;
    CHECK_SET_ERR(newRowLength < end, "Incorrect length");

    int refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    //5. Press "Remove all coloumns of gaps "
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Remove all columns of gaps");

    //Expected: Reference will be trimmed
    int newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(newRefLength < refLength, QString("Expected: New ref length is less then old ref length, current: new = %1, old = %2").arg(QString::number(newRefLength)).arg(QString::number(refLength)));

    //6. Press "undo"
    GTUtilsMcaEditor::undo(os);

    //Expected: reference will be restored with gaps
    newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(newRefLength == refLength, QString("Expected: New ref length is equal old ref length, current: new = %1, old = %2").arg(QString::number(newRefLength)).arg(QString::number(refLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5495) {
    //1) Open samples/FASTA/human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2) Select 100..10 region of the sequence
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();

            QLineEdit *startEdit = dialog->findChild<QLineEdit *>("startEdit");
            QLineEdit *endEdit = dialog->findChild<QLineEdit *>("endEdit");
            CHECK_SET_ERR(startEdit != NULL, "QLineEdit \"startEdit\" not found");
            CHECK_SET_ERR(endEdit != NULL, "QLineEdit \"endEdit\" not found");

            GTLineEdit::setText(os, startEdit, QString::number(321));
            GTLineEdit::setText(os, endEdit, QString::number(123));

            QDialogButtonBox *box = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox"));
            QPushButton *goButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(goButton != NULL, "Go button not found");
            CHECK_SET_ERR(!goButton->isEnabled(), "Go button is enabled");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_human_T1(UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, new Scenario));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Select"
                                                                        << "Sequence region"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
}

GUI_TEST_CLASS_DEFINITION(test_5499) {
    //    1. Open txt file (_common_data/text/text.txt).
    //    Expected state: "Select correct document format" dialog appears
    //    2. Select "Choose format manually" with the default ABIF format.
    //    3. Click Ok.
    GTLogTracer logTracer;

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/text/text.txt"));
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "ABIF"));
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Open as...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the message about "not ABIF format" appears, UGENE doesn't crash.
    GTUtilsLog::checkContainsError(os, logTracer, "Not a valid ABIF file");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5517) {
    //1. Open sequence
    //2. Open build dotplot dialog
    //3. Check both checkboxes direct and invert repeats search
    //Expected state: UGENE not crashed
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 100, 0, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Analyze"
                                                << "Build dotplot...",
                              GTGlobals::UseMouse);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5520_2) {
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "/_common_data/cmdline/external-tool-support/blastall/sars_middle.nhr"));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton"));

            QRadioButton *rbNewTable = GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable");
            CHECK_SET_ERR(rbNewTable != NULL, "rbCreateNewTable not found");
            GTRadioButton::click(os, rbNewTable);
            GTGlobals::sleep();

            QLineEdit *leTablePath = GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath");
            CHECK_SET_ERR(leTablePath != NULL, "leNewTablePath not found");
            GTLineEdit::setText(os, leTablePath, sandBoxDir + "/test_5520_2.gb");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Analyze"
                                                << "Query with local BLAST+...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5550) {
    //    1. Open "_common_data/fasta/empty.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: there are next values on the statusbar: "Ln - / 0  Col - / 0  Pos - / -".
    const QString rowNumberString = GTMSAEditorStatusWidget::getRowNumberString(os);
    const QString rowsCountString = GTMSAEditorStatusWidget::getRowsCountString(os);
    const QString columnNumberString = GTMSAEditorStatusWidget::getColumnNumberString(os);
    const QString columnsCountString = GTMSAEditorStatusWidget::getColumnsCountString(os);
    const QString sequenceUngappedPositionString = GTMSAEditorStatusWidget::getSequenceUngappedPositionString(os);
    const QString sequenceUngappedLengthString = GTMSAEditorStatusWidget::getSequenceUngappedLengthString(os);

    CHECK_SET_ERR("-" == rowNumberString, QString("An incorrect row number label: expected '%1', got '%2'").arg("-").arg(rowNumberString));
    CHECK_SET_ERR("0" == rowsCountString, QString("An incorrect rows count label: expected '%1', got '%2'").arg("-").arg(rowsCountString));
    CHECK_SET_ERR("-" == columnNumberString, QString("An incorrect column number label: expected '%1', got '%2'").arg("-").arg(columnNumberString));
    CHECK_SET_ERR("0" == columnsCountString, QString("An incorrect columns count label: expected '%1', got '%2'").arg("-").arg(columnsCountString));
    CHECK_SET_ERR("-" == sequenceUngappedPositionString, QString("An incorrect sequence ungapped position label: expected '%1', got '%2'").arg("-").arg(sequenceUngappedPositionString));
    CHECK_SET_ERR("-" == sequenceUngappedLengthString, QString("An incorrect sequence ungapped length label: expected '%1', got '%2'").arg("-").arg(sequenceUngappedLengthString));
}

GUI_TEST_CLASS_DEFINITION(test_5562_1) {
    //1. Open File "\samples\CLUSTALW\HIV-1.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Statistics" Options Panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);

    //3. Click ">" button to set Reference sequence
    GTUtilsOptionPanelMsa::addReference(os, "sf170");

    //4. Click check box "Show distance coloumn"
    //GTWidget::findExactWidget<QComboBox*>(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, "showDistancesColumnCheck", true);

    //5. Set combo box value "Hamming dissimilarity"
    GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "algoComboBox"), "Hamming dissimilarity");

    //6. Set radio button value "Percents"
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "percentsButton"));

    //7. Click check box "Exclude gaps"
    GTCheckBox::setChecked(os, "excludeGapsCheckBox", true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Percents near: "ug46" is 6%,
    //                               "primer_ed5" is 0%
    //                               "primer_es7" is 1%
    QString val = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 8);
    CHECK_SET_ERR("6%" == val, QString("incorrect similarity: expected %1, got %2").arg("6%").arg(val));
    val = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 19);
    CHECK_SET_ERR("0%" == val, QString("incorrect similarity: expected %1, got %2").arg("0%").arg(val));
    val = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 21);
    CHECK_SET_ERR("1%" == val, QString("incorrect similarity: expected %1, got %2").arg("1%").arg(val));
}

GUI_TEST_CLASS_DEFINITION(test_5562_2) {
    //1. Open File "\samples\CLUSTALW\HIV-1.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open context menu in sequence area
    //3. Click "Statistick->Generate Distance Matrix"
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");
            //4. Set combo box value "Hamming dissimilarity"
            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "algoCombo", dialog), "Hamming dissimilarity");
            //5. Set radio button value "Percents"
            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "percentsRB", dialog));
            //6. Click check box "Exclude gaps"
            GTCheckBox::setChecked(os, "checkBox", true, dialog);
            //7. Click check box "Save profile to file"
            GTGroupBox::setChecked(os, "saveBox", dialog);
            //8. Set radio button value "Hypertext"
            //9. Set any valid file name
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "fileEdit", dialog), sandBoxDir + "5562_2_HTML.html");
            //10. Accept the dialog.
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Statistics"
                                                                              << "Generate distance matrix..."));
    GTUtilsDialog::waitForDialog(os, new DistanceMatrixDialogFiller(os, new Scenario));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : row "ug46", coloumn "sf170" value 26 % ,
    //                 row "sf170", coloumn "ug46" value 6 % ,
    //                 row "primer_ed31", coloumn "sf170" value 7 %
    //                 row "sf170", coloumn "primer_ed31" value 0 %
    QByteArray file = GTFile::readAll(os, sandBoxDir + "5562_2_HTML.html");
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
    //1. Open File "\samples\CLUSTALW\HIV-1.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open context menu in sequence area
    //3. Click "Statistick->Generate Distance Matrix"
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");
            //4. Set combo box value "Hamming dissimilarity"
            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "algoCombo", dialog), "Hamming dissimilarity");
            //5. Set radio button value "Percents"
            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "percentsRB", dialog));
            //6. Click check box "Exclude gaps"
            GTCheckBox::setChecked(os, "checkBox", true, dialog);
            //7. Click check box "Save profile to file"
            GTGroupBox::setChecked(os, "saveBox", dialog);
            //8. Set radio button value "Comma Separated"
            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "csvRB", dialog));
            //9. Set any valid file name
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "fileEdit", dialog), sandBoxDir + "5562_3_CSV.csv");
            //10. Accept the dialog.
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Statistics"
                                                                              << "Generate distance matrix..."));
    GTUtilsDialog::waitForDialog(os, new DistanceMatrixDialogFiller(os, new Scenario));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : the file should look like a sample file "_common_data/scenarios/_regression/5562/5562.csv"
    bool check = GTFile::equals(os, testDir + "_common_data/scenarios/_regression/5562/5562.csv", sandBoxDir + "5562_3_CSV.csv");
    CHECK_SET_ERR(check, QString("files are not equal"));
}

GUI_TEST_CLASS_DEFINITION(test_5588) {
    //1. Open File "/samples/CLUSTALW/HIV-1.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Select a 6th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(os, 5);
    //3. Press the Shift key
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    //4. Select a 15th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(os, 14);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    //Expected state : All columns between 6th and 15th clicks are selected
    QRect rect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(rect == QRect(QPoint(5, 0), QPoint(14, 24)), QString("Incorrect selected area, %1, %2, %3, %4").arg(rect.topLeft().x()).arg(rect.topLeft().y()).arg(rect.bottomRight().x()).arg(rect.bottomRight().y()));

    //5. Select a 30th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(os, 29);
    //6. Press the Shift key
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    //7. Select a 12th column with a mouse click to the consensus area
    GTUtilsMsaEditor::clickColumn(os, 11);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    // Expected state : All columns between 12th and 30th clicks are selected
    rect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(rect == QRect(QPoint(11, 0), QPoint(29, 24)), QString("Incorrect selected area, %1, %2, %3, %4").arg(rect.topLeft().x()).arg(rect.topLeft().y()).arg(rect.bottomRight().x()).arg(rect.bottomRight().y()));
}

GUI_TEST_CLASS_DEFINITION(test_5594_1) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Select reference pos 15
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(os, 15);

    //6. Press reference pos 35 with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(os, 35);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep();

    //Expected: selected length = 20
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    CHECK_SET_ERR(reg.length == 21, QString("Unexpexter selected length, expected: 20, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5594_2) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Push "Show/Hide Chromatograms" button in the main menu
    bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_5B70");
    if (isChromatogramShown) {
        GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "chromatograms"));
    }

    //6. Select read "SZYD_Cas9_CR51"
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR51");

    //7. Select read "SZYD_Cas9_CR61" with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR61");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep();

    //Expected: selected length = 8
    U2Region reg = GTUtilsMcaEditorSequenceArea::getSelectedRowsNum(os);
    CHECK_SET_ERR(reg.length == 8, QString("Unexpexter selected length, expected: 8, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5594_3) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Select reference pos 15
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(os, 15);

    //6. Press right 5 times with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right);
        GTGlobals::sleep(200);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep();

    //Expected: selected length = 20
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    CHECK_SET_ERR(reg.length == 6, QString("Unexpexter selected length, expected: 6, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5594_4) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Push "Show/Hide Chromatograms" button in the main menu
    bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_5B70");
    if (isChromatogramShown) {
        GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "chromatograms"));
    }

    //6. Select read "SZYD_Cas9_CR51"
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR51");

    //7. Prss down 5 times with shift modifier
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        GTGlobals::sleep(200);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep();

    //Expected: selected length = 6
    U2Region reg = GTUtilsMcaEditorSequenceArea::getSelectedRowsNum(os);
    CHECK_SET_ERR(reg.length == 6, QString("Unexpexter selected length, expected: 6, current: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5604) {
    //1. Open Workflow designer
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Open scheme
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/scenarios/_regression/5604/scheme.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Set up input data
    GTUtilsWorkflowDesigner::click(os, "Read FASTQ Files with Reads");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/reads/e_coli_1000.fq", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/reads/e_coli_1000_1.fq", true);

    GTUtilsWorkflowDesigner::click(os, "Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Reference genome", testDir + "_common_data/fasta/human_T1_cutted.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsDashboard::getOutputFiles(os).size() == 1, "Wrong quantaty of output files");
}

GUI_TEST_CLASS_DEFINITION(test_5622) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference_gapped.gb (reference with gaps);
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference_gapped.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: all gaps columns was removed
    qint64 refLengthBeforeGapsRemove = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Remove all columns of gaps"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    qint64 refLengthAfterGapsRemove = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLengthBeforeGapsRemove == refLengthAfterGapsRemove, QString("Equals befor adn after gaps removing not equal, length before: %1, length after: %2").arg(QString::number(refLengthBeforeGapsRemove)).arg(QString::number(refLengthAfterGapsRemove)));
}

GUI_TEST_CLASS_DEFINITION(test_5636) {
    //1. Open File "\samples\CLUSTALW\COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click Actions->Align->Align sequence to profile with MUSCLE...
    //3. Select "\samples\CLUSTALW\COI.aln"
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align sequences to profile with MUSCLE..."));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/CLUSTALW/COI.aln"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: 18 sequences are added to the msa.
    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 36, "Incorrect sequences count");
}

GUI_TEST_CLASS_DEFINITION(test_5637) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);

    //Expected: row length must be equal or lesser then reference length
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    MultipleAlignmentRowData *row = GTUtilsMcaEditor::getMcaRow(os, 0);
    qint64 rowLength = row->getRowLengthWithoutTrailing();
    CHECK_SET_ERR(rowLength <= refLength, QString("Expected: row length must be equal or lesser then reference length, current: row lenght = %1, reference length = %2").arg(QString::number(rowLength)).arg(QString::number(refLength)));

    //2. Select a char in the first row
    QPoint p(5500, 0);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, p);

    //3. insert 6 gaps
    int i = 6;
    while (i != 0) {
        GTGlobals::sleep(1000);
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        i--;
    }

    //Expected: row length must be equal or lesser then reference length
    refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    row = GTUtilsMcaEditor::getMcaRow(os, 1);
    rowLength = row->getRowLengthWithoutTrailing();
    CHECK_SET_ERR(rowLength <= refLength, QString("Expected: row length must be equal or lesser then reference length, current: row lenght = %1, reference length = %2").arg(QString::number(rowLength)).arg(QString::number(refLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5638) {
    //1. Open File "\samples\CLUSTALW\COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click to position (30, 10)
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(30, 10));

    //3. Press Ctrl and drag and drop selection to the right for a few symbols
    U2MsaListGapModel startGapModel = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getGapModel();

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::press();
    QPoint curPos = GTMouseDriver::getMousePosition();
    QPoint moveMouseTo(curPos.x() + 200, curPos.y());
    GTMouseDriver::moveTo(moveMouseTo);

    GTGlobals::sleep();
    U2MsaListGapModel gapModel = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getGapModel();
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

    GTGlobals::sleep();
    U2MsaListGapModel finishGapModel = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getGapModel();
    CHECK_SET_ERR(finishGapModel == startGapModel, "Unexpected changes of alignment");
}

GUI_TEST_CLASS_DEFINITION(test_5659) {
    // 1. Open murine.gb
    // 2. Context menu on annotations object
    // Expected state: export annotataions dialog appeared
    // 3. Check format combobox
    // Expected state: BAM format is abcent
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal dialog is NULL");

            QComboBox *comboBox = dialog->findChild<QComboBox *>();
            CHECK_SET_ERR(comboBox != NULL, "ComboBox not found");

            QStringList formats = GTComboBox::getValues(os, comboBox);
            CHECK_SET_ERR(!formats.contains("BAM"), "BAM format is present in annotations export dialog");

            QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox *>("buttonBox");
            CHECK_SET_ERR(buttonBox != NULL, "buttonBox is NULL");

            QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != NULL, "cancelButton is NULL");
            GTWidget::click(os, cancelButton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, new Scenario()));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "source"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5663) {
    GTUtilsDialog::waitForDialog(os, new RemoteDBDialogFillerDeprecated(os, "1ezg", 3, false, true, false, sandBoxDir));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Access remote database...",
                              GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsNotifications::waitForNotification(os, false);
    QWidget *taskReportWindow = GTUtilsMdi::findWindow(os, "Task report [Download remote documents]");
    QTextEdit *reportEdit = qobject_cast<QTextEdit *>(GTWidget::findWidget(os, "reportTextEdit", taskReportWindow));
    CHECK_SET_ERR(reportEdit != nullptr, "reportTextEdit is not found");
    QString html = reportEdit->toHtml();
    CHECK_SET_ERR(html.contains("Document was successfully downloaded"), "Report contains expected text");
}

GUI_TEST_CLASS_DEFINITION(test_5665) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Document context menu -> Export / Import -> Export sequences.
    //Expected: "Export selected sequences" dialog appears.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            QLineEdit *filepathLineEdit = GTWidget::findExactWidget<QLineEdit *>(os, "fileNameEdit", dialog);
            GTLineEdit::setText(os, filepathLineEdit, dataDir + "long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_long_file_name_more_then_250_.fa");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    //Expected: the dialog about external modification of documents appears.
    //5. Click "No".
    //Expected: UGENE does not crash.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, new Scenario()));
    GTUtilsProjectTreeView::click(os, "human_T1.fa", Qt::RightButton);
    GTGlobals::sleep(3000);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_5681) {
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal dialog is NULL");

            QComboBox *comboBox = dialog->findChild<QComboBox *>();
            CHECK_SET_ERR(comboBox != NULL, "ComboBox not found");

            QStringList formats = GTComboBox::getValues(os, comboBox);
            CHECK_SET_ERR(!formats.contains("BAM"), "BAM format is present in annotations export dialog");

            QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox *>("buttonBox");
            CHECK_SET_ERR(buttonBox != NULL, "buttonBox is NULL");

            QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != NULL, "cancelButton is NULL");
            GTWidget::click(os, cancelButton);
        }
    };

    //1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open the context menu for the "NC_001363 features" object.
    //3. Select "Export/Import" -> "Export annotations..." menu item.
    //4. Set any valid output path, select "UGENE Database" format.
    //5. Accept the dialog.
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, sandBoxDir + "murine_annotations.gb", ExportAnnotationsFiller::ugenedb, true, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export annotations..."));
    GTUtilsProjectTreeView::callContextMenu(os, "NC_001363 features");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: No safe point
    GTUtilsProjectTreeView::checkItem(os, "murine_annotations.gb");
}

GUI_TEST_CLASS_DEFINITION(test_5696) {
    //1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //3. Select region with gaps
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(41, 1), QPoint(43, 3));

    //4. Copy this subalignment
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);    // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);    // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems
    GTUtilsNotifications::waitForNotification(os, true, "No new rows were inserted: selection contains no valid sequences.");
    GTUtilsDialog::waitAllFinished(os);

    // TODO: can't use Russian text today: PasteController uses UTF-8 to save the text, but TextFormat uses local8Bit to read it and fails.
    // This makes 2 concurrent popups to appear.
    // GTClipboard::setText(os, "фыва...");
    GTClipboard::setText(os, "#$%^&*(");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);    // Qt::ControlModifier is for Cmd on Mac and for Ctrl on other systems

    GTUtilsNotifications::waitForNotification(os, true, "No new rows were inserted: selection contains no valid sequences.");
    GTUtilsDialog::waitAllFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5714_1) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);

    //5. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2066, 1));

    //6. Press Ctrl + Shift + Backspace
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace, Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //Expected: row length must be lesser than row length before trim
    qint64 currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    //7. Press undo
    GTUtilsMcaEditor::undo(os);

    //Expected: current row length is equal start row length
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength == rowLength, QString("Expected: current row length is equal start row length, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    //8. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2066, 1));

    //9. Press Ctrl + Shift + Delete
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace, Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //Expected: row length must be lesser than row length before trim
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5714_2) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);

    //5. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2066, 1));

    //6. Press "Trim left end" from the context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Trim left end"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: row length must be lesser than row length before trim
    qint64 currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    //7. Press undo
    GTUtilsMcaEditor::undo(os);

    //Expected: current row length is equal start row length
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength == rowLength, QString("Expected: current row length is equal start row length, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    //8. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2066, 1));

    //9. Press "Trim right end" from the context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Trim right end"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: row length must be lesser than row length before trim
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5714_3) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);

    //5. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2066, 1));

    //6. Press "Trim left end" from the main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Trim left end");

    //Expected: row length must be lesser than row length before trim
    qint64 currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    //7. Press undo
    GTUtilsMcaEditor::undo(os);

    //Expected: current row length is equal start row length
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength == rowLength, QString("Expected: current row length is equal start row length, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));

    //8. Select position 2066 of the second read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2066, 1));

    //9. Press "Trim right end" from the main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Trim right end");

    //Expected: row length must be lesser than row length before trim
    currentLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(currentLength < rowLength, QString("Expected: row length must be lesser than row length before trim, cureent: start length %1, current length %2").arg(QString::number(rowLength)).arg(QString::number(currentLength)));
}

GUI_TEST_CLASS_DEFINITION(test_5716) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open "Export Consensus" options panel tab.
    //    Expected state: UGENE doesn't crash.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    //    3. Set any output file path, set any format.
    const QString expectedOutputPath = QDir::toNativeSeparators(sandBoxDir + "test_5716.txt");
    GTUtilsOptionPanelMsa::setExportConsensusOutputPath(os, expectedOutputPath);

    //    4. Open "General" options panel tab.
    //    Expected state: UGENE doesn't crash.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);

    //    5. Open "Export Consensus" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    //    Expected state: UGENE doesn't crash, the form is filled with values from step 3.
    const QString currentOutputPath = GTUtilsOptionPanelMsa::getExportConsensusOutputPath(os);
    const QString currentOutputFormat = GTUtilsOptionPanelMsa::getExportConsensusOutputFormat(os);
    const QString expectedOutputFormat = "Plain text";
    CHECK_SET_ERR(currentOutputPath == expectedOutputPath, QString("Output path is incorrect: expected '%1', got '%2'").arg(expectedOutputPath).arg(currentOutputPath));
    CHECK_SET_ERR(currentOutputFormat == expectedOutputFormat, QString("Output format is incorrect: expected '%1', got '%2'").arg(expectedOutputFormat).arg(currentOutputFormat));
}

GUI_TEST_CLASS_DEFINITION(test_5718) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);

    //2. Click reference pos 2071
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2071, 1));
    GTGlobals::sleep(1000);

    //3. Insert gap
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTUtilsOptionPanelMca::openTab(os, GTUtilsOptionPanelMca::General);
    const int lengthBeforeGapColumnsRemoving = GTUtilsOptionPanelMca::getLength(os);
    GTUtilsOptionPanelMca::closeTab(os, GTUtilsOptionPanelMca::General);

    //4. Remove all columns of gaps
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Remove all columns of gaps"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: before gap column removig < after gap column removig
    GTUtilsOptionPanelMca::openTab(os, GTUtilsOptionPanelMca::General);
    int lengthAfterGapColumnsRemoving = GTUtilsOptionPanelMca::getLength(os);
    GTUtilsOptionPanelMca::closeTab(os, GTUtilsOptionPanelMca::General);
    CHECK_SET_ERR(lengthAfterGapColumnsRemoving < lengthBeforeGapColumnsRemoving, QString("Expected: before gap column removig > after gap column removig, current: before %1, after %2").arg(QString::number(lengthBeforeGapColumnsRemoving)).arg(QString::number(lengthAfterGapColumnsRemoving)));
}

GUI_TEST_CLASS_DEFINITION(test_5739) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            CHECK_SET_ERR(minReadIdentity == 80, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            CHECK_SET_ERR(quality == 30, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool isAddToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(isAddToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), "Incorrect output line: is empty");

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, "Incorrect output line: do not contain default path");

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference_short.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                reads << ("sanger_" + num + ".ab1");
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //5. Move mouse cursor to the position 6372 at the reference sequence (first half of the char).
    GTUtilsMcaEditorSequenceArea::moveCursorToReferencePositionCenter(os, 6372, QPoint(-4, 0));

    //6. Select all chars in the reference from here to the end
    QPoint currentPos = GTMouseDriver::getMousePosition();
    int newXPos = GTUtilsMdi::activeWindow(os)->mapToGlobal(GTUtilsMdi::activeWindow(os)->rect().topRight()).x();
    GTMouseDriver::dragAndDrop(currentPos, QPoint(newXPos, currentPos.y()));
    GTThread::waitForMainThread();

    //Expected: selected length = 4
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    CHECK_SET_ERR(reg.length == 4, QString("Unexpected selection length, expected: 4, got: %1").arg(reg.length));
}

GUI_TEST_CLASS_DEFINITION(test_5747) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select any sequence
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Gampsocleis_sedakovii_EF540828");

    //3. Call contest menu -> Edit -> Edit sequence name
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Edit sequence name"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    //4. Set new name and press enter
    GTKeyboardDriver::keySequence("New name");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    //5. Select another sequence
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Conocephalus_sp.");

    //6. Edit name by HotKey F2
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    //7. Set new name and press enter
    GTKeyboardDriver::keySequence("New name 2");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_5750) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Rename the first sequence to '1'.
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "Phaneroptera_falcata", "1");

    //    3. Export the alignment object to MSF format.
    GTLogTracer logTracer;

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export object..."));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, sandBoxDir, "test_5750.msf", ExportDocumentDialogFiller::MSF, false, true));
    GTUtilsProjectTreeView::callContextMenu(os, "COI", "COI.aln");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: the exported file is opened in UGENE. The first sequence is named "1".
    GTUtilsLog::check(os, logTracer);

    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(!names.isEmpty(), "Names list is empty");

    const QString expectedName = "1";
    CHECK_SET_ERR(expectedName == names[0], QString("The first sequecne name is incorrect: expected '%1', got '%2'").arg(expectedName).arg(names[0]));
}

GUI_TEST_CLASS_DEFINITION(test_5751) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Call a context menu in the Project view on the opened MCA document.
    //6. Select "Lock document for editing" menu item.
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb");
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Lock document for editing"));
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb", Qt::RightButton);

    //7. Call a context menu in the MCA Editor.
    //Expected state : "Remove all columns of gaps" is disabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Edit"
                                                                              << "Remove all columns of gaps",
                                                            PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTKeyboardDriver::keyPress(Qt::Key_Escape);
    GTGlobals::sleep(100);
}

GUI_TEST_CLASS_DEFINITION(test_5752) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Select any symbol
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));

    //6. Press Trim left end
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Trim left end");

    //7. Press Trim right end
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Trim right end");

    int readNum = GTUtilsMcaEditor::getReadsNames(os).size();
    //8. Press Replace symbol / character and press space
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Replace character/gap");
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep();

    //Expected : the read must be deleted.If this read is last this option must be blocked
    int newReadNum = GTUtilsMcaEditor::getReadsNames(os).size();
    CHECK_SET_ERR(newReadNum == 15 && 16 == readNum, QString("Incorrect reads num, expected 20 and 19, current %1 and %2").arg(QString::number(readNum)).arg(QString::number(newReadNum)));
}

GUI_TEST_CLASS_DEFINITION(test_5753) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Make changes
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    //6. Close document
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();

    //Expected: there is no "Save document" messageBox
}

GUI_TEST_CLASS_DEFINITION(test_5755) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference_need_gaps.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected : Trailing gaps were inserted into the end of reference
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    QString refReg = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, refLength - 20, 20);
    bool isGaps = true;
    foreach (QChar c, refReg) {
        if (c != U2Mca::GAP_CHAR) {
            isGaps = false;
            break;
        }
    }
    CHECK_SET_ERR(isGaps, "Incorrect characters");
}

GUI_TEST_CLASS_DEFINITION(test_5758) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Remove a row
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B70");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //6. Close the view
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb");
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Unload selected document(s)"));
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb", Qt::RightButton);
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    //7. Open a new view
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb");
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Load selected document(s)"));
    GTUtilsProjectTreeView::click(os, "Sanger.ugenedb", Qt::RightButton);
    GTGlobals::sleep(1000);

    //8. Hide chromatograms
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "chromatograms"));

    //9. Change the state of the last row
    bool isShownFirstState = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_CR66");
    GTUtilsMcaEditorSequenceArea::clickCollapseTriangle(os, "SZYD_Cas9_CR66", isShownFirstState);
    bool isShownSecondState = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_CR66");

    //Expected: States befor and aftef changing are different
    CHECK_SET_ERR(isShownFirstState != isShownSecondState, "Incorrect state");
}

GUI_TEST_CLASS_DEFINITION(test_5759) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Collapse all rows except the second one.
    GTUtilsMcaEditor::toggleShowChromatogramsMode(os);

    // 3. Select the "SZYD_Cas9_5B70" sequence.
    GTUtilsMcaEditor::clickReadName(os, QString("SZYD_Cas9_5B70"));
    GTGlobals::sleep(1000);

    // 4. Click arrow down
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(1000);

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTGlobals::sleep(1000);

    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, QString("SZYD_Cas9_5B71")),
                  "Required sequence is collapsed");
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTGlobals::sleep(1000);

    GTUtilsMcaEditor::removeRead(os, QString("SZYD_Cas9_5B70"));
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, QString("SZYD_Cas9_5B71")),
                  "Required sequence is collapsed");
}

GUI_TEST_CLASS_DEFINITION(test_5761) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);

    GTLogTracer trace;
    //2. Select the last char of the first row
    MultipleAlignmentRowData *row = GTUtilsMcaEditor::getMcaRow(os, 0);
    int end = row->getCoreStart() + row->getCoreLength() - 1;
    QPoint p(end, 0);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, p);
    QPoint curPos = GTMouseDriver::getMousePosition();
    QPoint moveMouseTo(curPos.x() + 140, curPos.y());

    //3. Press left button and move mouse to the right (add some gaps)
    GTMouseDriver::press();
    GTMouseDriver::moveTo(moveMouseTo);
    int i = 10;
    while (i != 0) {
        int minus = (i % 2 == 0) ? 1 : -1;
        int moving = minus * 3 * ((i % 2) + 1);
        QPoint perturbation(moveMouseTo.x(), moveMouseTo.y() + moving);
        GTMouseDriver::moveTo(perturbation);
        i--;
        GTGlobals::sleep(300);
    }
    GTMouseDriver::release();
    QStringList errors = GTUtilsLog::getErrors(os, trace);

    //Expected: no errors in the log
    CHECK_SET_ERR(errors.isEmpty(), "Some errors in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5762) {
    // 1. Connect to a shared database.
    // 2. Right click on the document->Add->Import to the database.
    // 3. Click "Add files".
    // 4. Choose "data/samples/ABIF/A01.abi".
    // 5. Click "Import".
    // Expected state : the file is imported, there are no errors in the log.
    GTLogTracer logTracer;
    Document *databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, "/regression5761", QStringList() << dataDir + "samples/ABIF/A01.abi");
    GTUtilsNotifications::waitForNotification(os, false, "Aligned reads (16)");
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_5769_1) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Select read "SZYD_Cas9_5B71"
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");

    //6. click 'down' two times
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);

    //Expected: selected read "SZYD_Cas9_CR51"
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames(os);
    CHECK_SET_ERR(name.size() == 1, QString("1. Unexpected selection! Expected selection size == 1, actual selection size == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_CR51", QString("Unexpected selected read, expected: SZYD_Cas9_CR51, current: %1").arg(name[0]));

    //7. Remove selected read
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //8. click 'down' two times
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);

    //Expected: selected read "SZYD_Cas9_CR54"
    name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames(os);
    CHECK_SET_ERR(name.size() == 1, QString("2. Unexpected selection! Expected selection size == 1, actual selection size == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_CR54", QString("Unexpected selected read, expected: SZYD_Cas9_CR54, current: %1").arg(name[0]));
}

GUI_TEST_CLASS_DEFINITION(test_5769_2) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
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
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Select read "SZYD_Cas9_5B71"
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR50");

    //6. click 'up'
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTGlobals::sleep(500);

    //Expected: selected read "SZYD_Cas9_5B71"
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames(os);
    CHECK_SET_ERR(name.size() == 1, QString("Unexpected selection? expected sel == 1< cerrent sel == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_5B71", QString("Unexpected selected read, expected: SZYD_Cas9_5B71, current: %1").arg(name[0]));

    //7. Remove selected read
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //8. click 'up'
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTGlobals::sleep(500);

    //Expected: selected read "SZYD_Cas9_5B70"
    name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames(os);
    CHECK_SET_ERR(name.size() == 1, QString("Unexpected selection? expected sel == 1< cerrent sel == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_5B70", QString("Unexpected selected read, expected: SZYD_Cas9_5B70, current: %1").arg(name[0]));
}

GUI_TEST_CLASS_DEFINITION(test_5770) {
    QString filePath = testDir + "_common_data/sanger/alignment.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select read "SZYD_Cas9_5B70"
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B70");
    GTGlobals::sleep();

    //3. Hold the _Shift_ key and press the _down arrow_ key.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep(500);

    //Expected: the selection is expanded.
    QStringList names = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames(os);
    CHECK_SET_ERR(names.size() == 2, QString("Incorrect selection. Expected: 2 selected rows, current: %1 selected rows").arg(names.size()));
}

GUI_TEST_CLASS_DEFINITION(test_5773) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(100);

    GTUtilsProjectTreeView::filterProject(os, "GTTCTCGGG");

    GTUtilsProjectTreeView::checkFilteredGroup(os, "Sanger read content", QStringList(), QStringList() << "Aligned reads"
                                                                                                       << "ugene_gui_test",
                                               QStringList() << "HIV-1.aln");

    GTUtilsProjectTreeView::checkFilteredGroup(os, "Sanger reference content", QStringList(), QStringList() << "Aligned reads"
                                                                                                            << "ugene_gui_test",
                                               QStringList() << "HIV-1.aln");

    GTUtilsProjectTreeView::filterProject(os, "KM0");
    GTUtilsProjectTreeView::checkFilteredGroup(os, "Sanger reference name", QStringList(), QStringList() << "Aligned reads"
                                                                                                         << "ugene_gui_test",
                                               QStringList() << "HIV-1.aln");
}

GUI_TEST_CLASS_DEFINITION(test_5775) {
    // 1. Open "data/samples/FASTQ/eas.fastq".
    // Expected state: the "Sequence Reading Options" dialog has appeared.
    // Select the "Merge sequences into a single sequence to show in sequence viewer" option. Accept the dialog.
    GTUtilsProject::openMultiSequenceFileAsMergedSequence(os, dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the "Search in Sequence" options panel tab.
    // Check the "Load patterns from file" option, select "data/samples/FASTQ/eas.fastq" as the file with patterns.
    // Expected state: the search task is launched automatically. After it is finished, there are 3 results.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os, true);
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbUsePatternNames"), true);

    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern(os);

    GTUtilsOptionPanelSequenceView::enterPatternFromFile(os, dataDir + "samples/FASTQ/", "eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/3"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_5781) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Align));
    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(testDir + "_common_data/fasta/ref2.fa", QStringList());
    parameters.samOutput = false;
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    //GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "can't be mapped"));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/COI2.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "unassociateReferenceAction", PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "Assembly reference sequence area"), Qt::RightButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5783) {
    // Open "samples/FASTA/human_T1.fa"
    // Create an annotation, set e.g. "ann" annotation name and "200..300" region.
    // Add "gene_id" and "transcript_id" qualifiers.
    // Export the annotation to the GTF format. Make sure the "Add to project" option is checked in the export dialog.
    // Expected state: The export has finished without errors. The document has been added to the project.

    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_human_T1(UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);
    GTWidget::click(os, GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "XCV", "ann");
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "TR321", "ann");

    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "ann");

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_5786_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click "Build Tree" button on the toolbar.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);

            //    3. Select "PhyML Maximum Likelihood" tree building method.
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

            //    4. Open "Branch Support" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Branch Support");

            //    Expected state: "Use fast likelihood-based method" radionbutton is selected, "Use fast likelihood-based method" combobox is enabled, "Perform bootstrap" spinbox is disabled.
            QRadioButton *rbFastMethod = GTWidget::findExactWidget<QRadioButton *>(os, "fastMethodCheckbox", dialog);
            CHECK_SET_ERR(NULL != rbFastMethod, "fastMethodCheckbox is NULL");
            CHECK_SET_ERR(rbFastMethod->isChecked(), "fastMethodCheckbox is not checked");
            GTWidget::checkEnabled(os, "fastMethodCombo", true, dialog);
            GTWidget::checkEnabled(os, "bootstrapSpinBox", false, dialog);

            //    5. Select "Perform bootstrap" radiobutton.
            GTRadioButton::click(os, "bootstrapRadioButton", dialog);

            //    Expected state: "Use fast likelihood-based method" combobox is disabled, "Perform bootstrap" spinbox is enabled.
            GTWidget::checkEnabled(os, "fastMethodCombo", false, dialog);
            GTWidget::checkEnabled(os, "bootstrapSpinBox", true, dialog);

            //    6. Select "Use fast likelihood-based method" radionbutton.
            GTRadioButton::click(os, "fastMethodCheckbox", dialog);

            //    Expected state: "Use fast likelihood-based method" combobox is enabled, "Perform bootstrap" spinbox is disabled.
            GTWidget::checkEnabled(os, "fastMethodCombo", true, dialog);
            GTWidget::checkEnabled(os, "bootstrapSpinBox", false, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5786_2) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click "Build Tree" button on the toolbar.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);

            //    3. Select "PhyML Maximum Likelihood" tree building method.
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

            GTWidget::checkEnabled(os, "tranSpinBox", false, dialog);

            //    4. Select "Transition / transversion ratio" "fixed" radiobutton.
            GTRadioButton::click(os, "transFixedRb", dialog);

            GTWidget::checkEnabled(os, "tranSpinBox", true, dialog);

            //    5. Open "Branch Support" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Branch Support");

            //    6. Select "Perform bootstrap" radiobutton.
            GTRadioButton::click(os, "bootstrapRadioButton", dialog);

            //    7. Open the "Substitution Model" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Substitution Model");

            //    Expected state: Expected state: the "Transition / transversion ratio" spinbox is enabled.
            GTWidget::checkEnabled(os, "tranSpinBox", true, dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5786_3) {
    GTLogTracer logTracerNegative("-b 5");
    GTLogTracer logTracerPositive("-b -2");

    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click "Build Tree" button on the toolbar.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);

            //    3. Select "PhyML Maximum Likelihood" tree building method.
            GTComboBox::selectItemByText(os, "algorithmBox", dialog, "PhyML Maximum Likelihood");

            //    4. Open "Branch Support" tab.
            GTTabWidget::clickTab(os, "twSettings", dialog, "Branch Support");

            //    5. Select "Perform bootstrap" radiobutton.
            GTRadioButton::click(os, "bootstrapRadioButton", dialog);

            //    6. Set "Perform bootstrap" spinbox value to 5.
            GTSpinBox::setValue(os, "bootstrapSpinBox", 5, dialog);

            //    7. Select "Use fast likelihood-based method" radiobutton.
            GTRadioButton::click(os, "fastMethodCheckbox", dialog);

            //    8. Set "Use fast likelihood-based method" combobox value to "Chi2-based".
            GTComboBox::selectItemByText(os, "fastMethodCombo", dialog, "Chi2-based");

            //    9. Set other necessary values and accept the dialog.
            GTLineEdit::setText(os, "fileNameEdit", sandBoxDir + "test_5786_3.nwk", dialog);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTGlobals::sleep();

    //    Expected state: there is an only "-b" parameter in the phyML arguments, it is equal to "-2".
    GTUtilsLog::checkContainsMessage(os, logTracerNegative, false);
    GTUtilsLog::checkContainsMessage(os, logTracerPositive, true);
}

GUI_TEST_CLASS_DEFINITION(test_5789_1) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", sandBoxDir + "test_5789.ugenedb");
    GTFileDialog::openFile(os, sandBoxDir + "test_5789.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: both "Undo" and "Redo" buttons are disabled.
    bool isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled(os);
    bool isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled(os);
    CHECK_SET_ERR(!isUndoEnabled, "Undo button is unexpectedly enabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    2. Edit the MCA somehow.
    GTUtilsMcaEditor::removeRead(os, "SZYD_Cas9_5B70");

    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled(os);
    isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled(os);
    CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    3. Close and open the view again.
    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    //    4. Repeat the previous state several times.
    for (int i = 0; i < 5; i++) {
        GTUtilsMdi::closeActiveWindow(os);
        GTUtilsProjectTreeView::doubleClickItem(os, "test_5789.ugenedb");
        GTUtilsTaskTreeView::waitTaskFinished(os);

        isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled(os);
        isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled(os);
        CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
        CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");
    }
}

GUI_TEST_CLASS_DEFINITION(test_5789_2) {
    //    1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: both "Undo" and "Redo" buttons are disabled.
    bool isUndoEnabled = GTUtilsMsaEditor::isUndoEnabled(os);
    bool isRedoEnabled = GTUtilsMsaEditor::isRedoEnabled(os);
    CHECK_SET_ERR(!isUndoEnabled, "Undo button is unexpectedly enabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    2. Edit the MSA somehow.
    GTUtilsMsaEditor::removeRows(os, 0, 0);
    GTGlobals::sleep(100);
    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    isUndoEnabled = GTUtilsMcaEditor::isUndoEnabled(os);
    isRedoEnabled = GTUtilsMcaEditor::isRedoEnabled(os);
    CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
    CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");

    //    3. Close and open the view again.
    //    Expected state: the "Undo" button is enabled, the "Redo" button is disabled.
    //    4. Repeat the previous state several times.
    for (int i = 0; i < 5; i++) {
        GTUtilsMdi::closeActiveWindow(os);
        GTUtilsProjectTreeView::doubleClickItem(os, "ma.aln");
        GTUtilsTaskTreeView::waitTaskFinished(os);

        isUndoEnabled = GTUtilsMsaEditor::isUndoEnabled(os);
        isRedoEnabled = GTUtilsMsaEditor::isRedoEnabled(os);
        CHECK_SET_ERR(isUndoEnabled, "Undo button is unexpectedly disabled");
        CHECK_SET_ERR(!isRedoEnabled, "Redo button is unexpectedly enabled");
    }
}

GUI_TEST_CLASS_DEFINITION(test_5790) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_5790.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");
    //2. Click to position on read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTGlobals::sleep(500);

    //3. Enter edit mode
    GTKeyboardDriver::keyClick('i', Qt::ShiftModifier);
    GTGlobals::sleep();
    //4. Click escape
    //Expected state: selection still present
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os) == 0, "MCA is not in view mode");

    //5. Click escape
    //Expected state: selection disappeared
    QRect emptyselection = QRect();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::getSelectedRect(os) == emptyselection, "Selection isn't empty but should be");
}

GUI_TEST_CLASS_DEFINITION(test_5798_1) {
    //1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, true));
    GTUtilsProject::openFile(os, dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: DNA.apr in the project view
    GTUtilsProjectTreeView::checkItem(os, "DNA.apr");
    GTUtilsProjectTreeView::checkObjectTypes(os, QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(os, QStringList() << "DNA.apr"));
    GTUtilsDocument::checkIfDocumentIsLocked(os, "DNA.apr", true);
}

GUI_TEST_CLASS_DEFINITION(test_5798_2) {
    //1. Convert samples/APR/DNA.apr to fasta
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, false, sandBoxDir + "DNA", "FASTA"));
    GTUtilsProject::openFile(os, dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: DNA.fa in the project view
    GTUtilsProjectTreeView::checkItem(os, "DNA.fa");
    GTUtilsProjectTreeView::checkObjectTypes(os, QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(os, QStringList() << "DNA.fa"));
    GTUtilsDocument::checkIfDocumentIsLocked(os, "DNA.fa", false);
}

GUI_TEST_CLASS_DEFINITION(test_5798_3) {
    //1. Convert samples/APR/DNA.apr to clustaw
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, false, sandBoxDir + "DNA", "CLUSTALW"));
    GTUtilsProject::openFile(os, dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: DNA.aln in the project view
    GTUtilsProjectTreeView::checkItem(os, "DNA.aln");
    GTUtilsProjectTreeView::checkObjectTypes(os, QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(os, QStringList() << "DNA.aln"));
    GTUtilsDocument::checkIfDocumentIsLocked(os, "DNA.aln", false);
}

GUI_TEST_CLASS_DEFINITION(test_5798_4) {
    //1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, true));
    GTUtilsProject::openFile(os, dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: DNA.apr in the project view
    GTUtilsProjectTreeView::checkItem(os, "DNA.apr");
    GTUtilsProjectTreeView::checkObjectTypes(os, QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(os, QStringList() << "DNA.apr"));
    GTUtilsDocument::checkIfDocumentIsLocked(os, "DNA.apr", true);

    //2. Convert document to clustalw from project view
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, sandBoxDir, "DNA.aln", ExportDocumentDialogFiller::CLUSTALW, false, true));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export document"));
    GTUtilsProjectTreeView::callContextMenu(os, "DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: DNA.aln in the project view
    GTUtilsProjectTreeView::checkItem(os, "DNA.aln");
    GTUtilsProjectTreeView::checkObjectTypes(os, QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(os, QStringList() << "DNA.aln"));
    GTUtilsDocument::checkIfDocumentIsLocked(os, "DNA.aln", false);

    //3. Export object to MEGA format from project view
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, sandBoxDir, "DNA.meg", ExportDocumentDialogFiller::MEGA, false, true));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import"
                                                                              << "Export object..."));
    GTUtilsProjectTreeView::callContextMenu(os, "DNA", "DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: DNA.meg is in the project view
    GTUtilsProjectTreeView::checkItem(os, "DNA.meg");
    GTUtilsProjectTreeView::checkObjectTypes(os, QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GTUtilsProjectTreeView::findIndex(os, QStringList() << "DNA.meg"));
    GTUtilsDocument::checkIfDocumentIsLocked(os, "DNA.meg", false);
}

GUI_TEST_CLASS_DEFINITION(test_5798_5) {
    //1. Open Workflow designer
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Open sample {Convert alignments to ClustalW}
    GTUtilsWorkflowDesigner::addSample(os, "Convert alignments to ClustalW");
    //Expected state: There is "Show wizard" tool button
    //3. Press "Show wizard" button

    class customWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            //4. Select input MSA "samples/APR/DNA.apr"
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << (QStringList() << dataDir + "samples/APR/DNA.apr"));

            //5. Press "Next" button
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            //GTUtilsWizard::setParameter(os, "Result ClustalW file", "DNA.aln");

            //6. Press "Run" button
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Convert alignments to ClustalW Wizard", new customWizard()));
    GTWidget::click(os, GTAction::button(os, "Show wizard"));
    //Expected state: Align sequences with MUSCLE Wizard appeared

    //Expected state: Scheme successfully performed
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_5815) {
    //1. Open a short alignment, e.g "test_common_data\scenarios\msa\ma2_gapped.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click on the empty area at the right side of the consensus
    GTUtilsMsaEditor::moveToColumn(os, 13);
    QPoint p = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(QPoint(p.x() + 100, p.y()));
    GTMouseDriver::click();

    //Expected: no crash
}

GUI_TEST_CLASS_DEFINITION(test_5818_1) {
    //1. Open samples/ACE/BL060C3.ace in read-only mode
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, true));
    GTUtilsProject::openFile(os, dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: BL060C3.ace in the project view
    GTUtilsProjectTreeView::checkItem(os, "BL060C3.ace");
}

GUI_TEST_CLASS_DEFINITION(test_5818_2) {
    //1. Convert samples/ACE/BL060C3.ace.ugenedb to fasta
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "BL060C3.ace.ugenedb"));
    GTUtilsProject::openFile(os, dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: BL060C3.ace.ugenedb in the project view
    GTUtilsProjectTreeView::checkItem(os, "BL060C3.ace.ugenedb");
}

GUI_TEST_CLASS_DEFINITION(test_5832) {
    //1. Open "test/_common_data/fasta/empty.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTLogTracer l;

    //2. Click on the sequence area.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(5, 5));

    //Expected: no errors in the log
    QStringList errorList = GTUtilsLog::getErrors(os, l);
    CHECK_SET_ERR(errorList.isEmpty(), "Unexpected errors in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5833) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Select 440 base on the second read (the position is ungapped).
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2506, 1));
    GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    GTGlobals::sleep(100);

    //    Expected state: the status bar contains the next labels: "Ln 2/16, RefPos 2500/11878, ReadPos 440/1173".
    QString rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    QString rowsCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    QString referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    QString referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    QString readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    QString readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("2" == rowNumberString, QString("Unexepected row number label: expected '%1', got '%2'").arg("2").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowsCountString, QString("Unexepected rows count label: expected '%1', got '%2'").arg("16").arg(rowsCountString));
    CHECK_SET_ERR("2500" == referencePositionString, QString("Unexepected reference position label: expected '%1', got '%2'").arg("2500").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("Unexepected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("440" == readPositionString, QString("Unexepected read position label: expected '%1', got '%2'").arg("440").arg(readPositionString));
    CHECK_SET_ERR("1173" == readLengthString, QString("Unexepected read length label: expected '%1', got '%2'").arg("1173").arg(readLengthString));

    //    3. Call a context menu, select "Edit" -> "Insert character/gap" menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Insert character/gap"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

    //    4. Click 'A' key.
    GTKeyboardDriver::keyClick('A');
    GTGlobals::sleep(500);

    //    Expected state: the new base has been inserted, the status bar contains the next labels: "Ln 2/16, RefPos gap/11878, ReadPos 440/1174".
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowsCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("2" == rowNumberString, QString("Unexepected row number label: expected '%1', got '%2'").arg("2").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowsCountString, QString("Unexepected rows count label: expected '%1', got '%2'").arg("16").arg(rowsCountString));
    CHECK_SET_ERR("gap" == referencePositionString, QString("Unexepected reference position label: expected '%1', got '%2'").arg("gap").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("Unexepected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("440" == readPositionString, QString("Unexepected read position label: expected '%1', got '%2'").arg("440").arg(readPositionString));
    CHECK_SET_ERR("1174" == readLengthString, QString("Unexepected read length label: expected '%1', got '%2'").arg("1174").arg(readLengthString));
}

GUI_TEST_CLASS_DEFINITION(test_5837) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTUtilsProject::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    //    2. Select first sequence
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(0, 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save sequence", GTGlobals::UseKey));
    Runnable *r = new ExportSelectedSequenceFromAlignment(os, testDir + "_common_data/scenarios/sandbox/", ExportSelectedSequenceFromAlignment::Ugene_db, true);
    GTUtilsDialog::waitForDialog(os, r);

    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "Phaneroptera_falcata.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5840) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);

    //2. Select a read ""
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");

    //3. Select a document in the Project View and press the Delete key.
    GTUtilsProjectTreeView::click(os, "Aligned reads");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(1000);

    //Expected: The document has been deleted.
    bool isExited = GTUtilsProjectTreeView::checkItem(os, "Aligned reads");
    CHECK_SET_ERR(!isExited, "The document has not been deleted")
}

GUI_TEST_CLASS_DEFINITION(test_5842) {
    //1. Open "_common_data/sanger/aligment.ugenedb".
    GTFileDialog::openFile(os, testDir + "_common_data/sanger/alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Unload document.
    GTUtilsDocument::unloadDocument(os, "alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTLogTracer l;
    //3. Open the view from the context menu.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Open View"
                                                                        << "action_open_view"));
    GTUtilsProjectTreeView::click(os, "alignment.ugenedb", Qt::RightButton);

    //Expected state: the view is opened without errors.
    QStringList errorList = GTUtilsLog::getErrors(os, l);
    CHECK_SET_ERR(errorList.isEmpty(), "Unexpected errors in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5847) {
    //1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, true));
    GTUtilsProject::openFile(os, dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select any sequence
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "HS11791");
    GTGlobals::sleep(1000);

    GTLogTracer l;

    //3 Press "delete"
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //Expected: no errors in the log
    QStringList errorList = GTUtilsLog::getErrors(os, l);
    CHECK_SET_ERR(errorList.isEmpty(), "Unexpected errors in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5849) {
    //CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");

    // 1. Open "..\general_common_data\fasta\empty.fa".

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Click the "Align sequence(s) to this alignment" button on the toolbar.
    // Expected state: the file selection dialog is opened.
    // Select "..\samples\CLUSTALW\COI.aln" in the dialog.

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/CLUSTALW/COI.aln"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");

    // 3. Select a sequence.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(2, 2));
    GTGlobals::sleep();
    QAbstractButton *undoButton = GTAction::button(os, "msa_action_undo");

    // 4. Click the "Undo" button.
    GTWidget::click(os, undoButton);
    QWidget *msaEditorStatusBar = GTWidget::findWidget(os, "msa_editor_status_bar");
    CHECK_SET_ERR(msaEditorStatusBar != NULL, "MSAEditorStatusBar is NULL");

    // Expected state: the selection has been cleared.
    QLabel *line = qobject_cast<QLabel *>(GTWidget::findWidget(os, "Line", msaEditorStatusBar));
    CHECK_SET_ERR(line != NULL, "Line of MSAEditorStatusBar is NULL");
    QLabel *column = qobject_cast<QLabel *>(GTWidget::findWidget(os, "Column", msaEditorStatusBar));
    CHECK_SET_ERR(column != NULL, "Column of MSAEditorStatusBar is NULL");
    QLabel *position = qobject_cast<QLabel *>(GTWidget::findWidget(os, "Position", msaEditorStatusBar));
    CHECK_SET_ERR(position != NULL, "Position of MSAEditorStatusBar is NULL");
    QLabel *selection = qobject_cast<QLabel *>(GTWidget::findWidget(os, "Selection", msaEditorStatusBar));
    CHECK_SET_ERR(selection != NULL, "Selection of MSAEditorStatusBar is NULL");

    CHECK_SET_ERR(line->text() == "Seq - / 0", "Sequence is " + line->text());
    CHECK_SET_ERR(column->text() == "Col - / 0", "Column is " + column->text());
    CHECK_SET_ERR(position->text() == "Pos - / -", "Position is " + position->text());
    CHECK_SET_ERR(selection->text() == "Sel none", "Selection is " + selection->text());
}

GUI_TEST_CLASS_DEFINITION(test_5851) {
    //    1. Set the temporary dir path to the folder with the spaces in the path.
    QDir().mkpath(sandBoxDir + "test_5851/t e m p");

    class SetTempDirPathScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            AppSettingsDialogFiller::setTemporaryDirPath(os, sandBoxDir + "test_5851/t e m p");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new SetTempDirPathScenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");

    GTLogTracer logTracer("The task uses a temporary folder to process the data. "
                          "The folder path is required not to have spaces. "
                          "Please set up an appropriate path for the \"Temporary files\" "
                          "parameter on the \"Directories\" tab of the UGENE Application Settings.");

    //    2. Select "Tools" -> Sanger data analysis" -> "Map reads to reference...".
    //    3. Set "_common_data/sanger/reference.gb" as reference, "_common_data/sanger/sanger_*.ab1" as reads. Accept the dialog.
    //    Expected state: the task fails.
    //    4. After the task finish open the report.
    //    Expected state: there is an error message in the report: "The task uses a temporary folder to process the data. The folder path is required not to have spaces. Please set up an appropriate path for the "Temporary files" parameter on the "Directories" tab of the UGENE Application Settings.".
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            AlignToReferenceBlastDialogFiller::setReference(os, testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList reads;
            for (int i = 1; i < 21; i++) {
                reads << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(os, reads, dialog);
            AlignToReferenceBlastDialogFiller::setDestination(os, sandBoxDir + "test_5851/test_5851.ugenedb", dialog);

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsMessage(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_5853) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select the "Tettigonia_viridissima" sequence in the Name List area.
    GTUtilsMsaEditor::clickSequence(os, 9);
    GTGlobals::sleep();

    //3. Press the Esc key.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep();

    //Expected state : the selection is cleared.
    int numSelSeq = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(os);
    CHECK_SET_ERR(numSelSeq == 0, QString("First check, incorrect num of selected sequences, expected: 0, current : %1").arg(numSelSeq));

    //4. Press the down arrow key.
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep();

    //Expected: nothing should be selected
    numSelSeq = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(os);
    CHECK_SET_ERR(numSelSeq == 0, QString("Second checdk, incorrect num of selected sequences, expected: 0, current : %1").arg(numSelSeq));
}

GUI_TEST_CLASS_DEFINITION(test_5854) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    //3. Select "Mecopoda_elongata__Ishigaki__J" sequence
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Mecopoda_elongata__Ishigaki__J");

    MSAEditorSequenceArea *seqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
    MaEditorSelection sel = seqArea->getSelection();
    int index = seqArea->getRowIndex(sel.y()) + 1;

    //Expected:: current index 14
    CHECK_SET_ERR(index == 14, QString("Unexpected index, expected: 14, current: %1").arg(index));
    GTGlobals::sleep();

    //4. Select "Mecopoda_sp.__Malaysia_" sequence
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Mecopoda_sp.__Malaysia_");

    //Expected:: current index 16
    sel = seqArea->getSelection();
    index = seqArea->getRowIndex(sel.y()) + 1;
    CHECK_SET_ERR(index == 16, QString("Unexpected index, expected: 16, current: %1").arg(index));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5855) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    // Expected state: a collapsing group appears.
    GTUtilsMSAEditorSequenceArea::isCollapsed(os, "Mecopoda_elongata__Sumatra_");

    // Select three rows: the header row of the collapsed group, the row above it and the row below it.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Conocephalus_percaudata");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Mecopoda_elongata__Ishigaki__J");
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Mecopoda_sp.__Malaysia_");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Click the arrow nearby the group header row to expand the collapsed group.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(os, "Mecopoda_elongata__Ishigaki__J");

    // Expected state: the group is expanded, four rows are selected: three rows that you've selected manually and the row that was hidden.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, 12, 604, 4));

    // Press the up arrow key on the keyboard.
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    // Expected state: the selection is moved up to one row, it's height is still 4.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, 11, 604, 4));
}

GUI_TEST_CLASS_DEFINITION(test_5872) {
    GTLogTracer logTracer("ASSERT: \"!isInRange");

    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    //    3. Select two first rows in the Name List Area.
    GTUtilsMsaEditor::selectRows(os, 0, 1, GTGlobals::UseMouse);

    //    4. Click to the position (3, 3).
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(2, 2));

    //    Expected state: there is no message in the log starting with 'ASSERT: "!isInRange'.
    GTUtilsLog::checkContainsMessage(os, logTracer, false);
}

GUI_TEST_CLASS_DEFINITION(test_5898) {
    //    1. Open the sequence and the corresponding annotations in separate file:
    //        primer3/NM_001135099_no_anns.fa
    //        primer3/NM_001135099_annotations.gb
    //    2. Add opened annotaions to the sequence
    //    3. Open Primer3 dialog
    //    4. Check RT-PCR and pick primers
    //    Expected state: no error in the log, exon annotations in separate file were successfully found
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "NM_001135099_no_anns.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "NM_001135099_annotations.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NM_001135099");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NM_001135099");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os, "NM_001135099 features");
    QWidget *sequence = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(sequence != NULL, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idx, sequence);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
                                                                        << "primer3_action"));
    Primer3DialogFiller::Primer3Settings settings;
    settings.rtPcrDesign = true;

    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_5903) {
    //1. Open 'human_T1.fa'
    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTKeyboardDriver::keyClick('p');
            GTGlobals::sleep(50);
            GTKeyboardDriver::keyClick('r');
            GTGlobals::sleep(50);
            GTKeyboardDriver::keyClick('o');
            GTGlobals::sleep(50);
            GTKeyboardDriver::keyClick('p');
            GTGlobals::sleep(500);

            GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbGenbankFormat", dialog));
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leAnnotationName", dialog), "NewAnn");
            GTGlobals::sleep(50);

            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leLocation", dialog), "100..200");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //2. Create annotation with "propertide" type
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    //Expected type - propeptide
    QString type = GTUtilsAnnotationsTreeView::getAnnotationType(os, "NewAnn");
    CHECK_SET_ERR(type == "Propeptide", QString("incorrect type, expected: Propeptide, current: %1").arg(type));
}

GUI_TEST_CLASS_DEFINITION(test_5905) {
    //    1. Open 'human_T1.fa'
    //    2. Launch Primer3 search (set results count to 50)
    //    Expected state: check GC content of the first result pair, it should be 55 and 33

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_human_T1(UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));

    ADVSingleSequenceWidget *wgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(wgt != NULL, "ADVSequenceWidget is NULL");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
                                                                        << "primer3_action"));
    Primer3DialogFiller::Primer3Settings settings;

    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));
    GTWidget::click(os, wgt, Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<QTreeWidgetItem *> items = GTUtilsAnnotationsTreeView::findItems(os, "top_primers");
    CHECK_SET_ERR(items.size() >= 2, "Wrong annotations count");
    GTUtilsAnnotationsTreeView::selectItems(os, QList<QTreeWidgetItem *>() << items[0]);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue(os, "gc%", items[0]) == "55", "wrong gc percentage");
    GTUtilsAnnotationsTreeView::selectItems(os, QList<QTreeWidgetItem *>() << items[1]);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue(os, "gc%", items[1]) == "35", "wrong gc percentage");
}

GUI_TEST_CLASS_DEFINITION(test_5947) {
    //    1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Set focus to the first sequence.
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            QLineEdit *startLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "start_edit_line", dialog));
            //GT_CHECK(startLineEdit != NULL, "Start lineEdit is NULL");
            GTLineEdit::setText(os, startLineEdit, "10");

            QLineEdit *endLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "end_edit_line", dialog));
            //GT_CHECK(endLineEdit != NULL, "Start lineEdit is NULL");
            GTLineEdit::setText(os, endLineEdit, "50");

            GTComboBox::selectItemByText(os, GTWidget::findExactWidget<QComboBox *>(os, "algorithmComboBox", dialog), "PsiPred");
            GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new LicenseAgreementDialogFiller(os));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished(os);

            QTableWidget *resultsTable = GTWidget::findExactWidget<QTableWidget *>(os, "resultsTable", dialog);
            GTGlobals::sleep();
            CHECK_SET_ERR(resultsTable != NULL, "resultsTable is NULL");
            const int resultsCount = resultsTable->rowCount();
            CHECK_SET_ERR(resultsCount == 3, QString("Unexpected results count: expected %1, got %2").arg(4).arg(resultsCount));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PredictSecondaryStructureDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Predict secondary structure");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5948) {
    //1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");

    //2. Make sure the editing mode is switched off.
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    QAction *editMode = GTAction::findActionByText(os, "Switch on the editing mode");
    CHECK_SET_ERR(editMode != NULL, "Cannot find Edit mode action");
    if (editMode->isChecked()) {
        GTWidget::click(os, GTAction::button(os, editMode));
    }

    //3. Copy a sequence region
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);
    GTKeyboardUtils::copy(os);

    //4. "Copy/Paste > Paste sequence" is disabled in the context menu.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Copy/Paste"
                                                                              << "Paste sequence",
                                                            PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    MWMDIWindow *mdiWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_5950) {
    //    1. Open 'human_T1.fa'
    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Switch on the editing mode.
    QAction *editMode = GTAction::findActionByText(os, "Switch on the editing mode");
    CHECK_SET_ERR(editMode != NULL, "Cannot find Edit mode action");
    GTWidget::click(os, GTAction::button(os, editMode));

    QPoint point = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(QPoint(point.x() + 100, point.y()));
    GTMouseDriver::press();

    for (int i = 0; i < 2; i++) {
        for (int j = 1; j < 5; j++) {
            point = GTMouseDriver::getMousePosition();
            int multiplier = i == 0 ? 1 : (-1);
            GTMouseDriver::moveTo(QPoint(point.x() + multiplier * 16, point.y()));
            QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
            CHECK_SET_ERR(selection.size() == 1, "Incorrect selection");

            U2Region sel = selection.first();
            CHECK_SET_ERR(sel.length != 0, "Selection length is 0");

            GTGlobals::sleep(200);
        }
    }

    GTMouseDriver::release();
}

GUI_TEST_CLASS_DEFINITION(test_5970) {
    //1. Open "_common_data/clustal/amino_from_wikipedia.aln".

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/amino_from_wikipedia.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Expected state: it has the "Standard amino acid alphabet" (see the "General" OP tab).
    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not Amino!");

    //3. Select (8; 1) (8; 3) rectangle.

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 0), QPoint(7, 2));

    //4. Press Ctrl + C.
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    //5. Press Ctrl + V.
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    //6. Expected state: the alphabet is "Standard amino acid"
    const bool isAlphabetAminoAfter = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAminoAfter, "Alphabet is not Amino!");
}

GUI_TEST_CLASS_DEFINITION(test_5972_1) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //1. Open file _common_data/regression/5972/5972_1.uwl
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/regression/5972/5972_1.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Set input file _common_data/regression/5972/seq_with_orfs.fa
    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/regression/5972/seq_with_orfs.fa");

    //3. Set output file sandBoxDir "/test_5972_1.csv"
    GTUtilsWorkflowDesigner::click(os, "Write Annotations");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QDir(sandBoxDir).absolutePath() + "/test_5972_1.csv", GTUtilsWorkflowDesigner::textValue);

    GTLogTracer tr;
    //4. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: There are no errors in the log
    CHECK_SET_ERR(!tr.hasErrors(), "Errors in log: " + tr.getJoinedErrorString());

    //Expected: The result file is equal to "_common_data/regression/5972/seq_with_orfs_1.csv"
    bool check = GTFile::equals(os, testDir + "_common_data/regression/5972/seq_with_orfs_1.csv", QDir(sandBoxDir).absolutePath() + "/test_5972_1.csv");
    CHECK_SET_ERR(check, QString("files are not equal"));
}

GUI_TEST_CLASS_DEFINITION(test_5972_2) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //1. Open file _common_data/regression/5972/5972_2.uwl
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/regression/5972/5972_2.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Set input file _common_data/regression/5972/seq_with_orfs.fa
    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/regression/5972/seq_with_orfs.fa");

    //3. Set output file sandBoxDir "/test_5972_1.csv"
    GTUtilsWorkflowDesigner::click(os, "Write Annotations");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QDir(sandBoxDir).absolutePath() + "/test_5972_2.csv", GTUtilsWorkflowDesigner::textValue);

    GTLogTracer tr;

    //4. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: Error in the log "Sequence names were not saved, the input slot 'Sequence' is empty."
    GTUtilsLog::checkContainsError(os, tr, QString("Sequence names were not saved, the input slot 'Sequence' is empty."));

    //Expected: The result file is equal to "_common_data/regression/5972/seq_with_orfs_1.csv"
    bool check = GTFile::equals(os, testDir + "_common_data/regression/5972/seq_with_orfs_2.csv", QDir(sandBoxDir).absolutePath() + "/test_5972_2.csv");
    CHECK_SET_ERR(check, QString("files are not equal"));
}

}    // namespace GUITest_regression_scenarios

}    // namespace U2
