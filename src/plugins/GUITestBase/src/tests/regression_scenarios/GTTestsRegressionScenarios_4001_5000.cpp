/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QClipboard>
#include <QFile>
#include <QGroupBox>
#include <QListWidget>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QTableView>
#include <QWebElement>
#include <QTextStream>
#include <QDebug>

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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MSAEditorTreeViewer.h>
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
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsPcr.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsPrimerLibrary.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsSharedDatabaseDocument.h"
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
#include "runnables/ugene/corelibs/U2Gui/ImportToDatabaseDialogFiller.h"
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
#include "runnables/ugene/plugins/external_tools/BlastAllSupportDialogFiller.h"
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
#include "runnables/ugene/ugeneui/DocumentProviderSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SelectDocumentFormatDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_4007) {
    GTLogTracer l;
    //    1. Open file {data/samples/Genbank/murine.gb}
    QDir().mkpath(sandBoxDir + "test_4007");
    GTFile::copy(os, dataDir + "samples/Genbank/murine.gb", sandBoxDir + "test_4007/murine.gb");
    GTFileDialog::openFile(os, sandBoxDir + "test_4007", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open file {data/samples/FASTA/human_T1.fa}
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Drag and drop annotations object to the human_T1 sequence.
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, "NC_001363 features"), GTUtilsAnnotationsTreeView::getTreeWidget(os));


    //    4. Edit "murine.gb" file with an external editor.
    //    Expected state: UGENE offers to reload the changed file.
    //    5. Agree to reload the file.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));

    QFile murineFile(sandBoxDir + "test_4007/murine.gb");
    const bool opened = murineFile.open(QFile::ReadWrite);
    CHECK_SET_ERR(opened, "Can't open the file: " + sandBoxDir + "test_4007/murine.gb");
    murineFile.write("L");
    murineFile.close();

    GTGlobals::sleep(5000);
    GTGlobals::FindOptions murineOptions(false);
    GTUtilsDocument::removeDocument(os, "human_T1.fa");

    QList<U2Region> regs = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(regs.isEmpty(), QString("Annotations are connected to murine.gb %1").arg(regs.isEmpty()));
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");

    //    Expected state: the file is reloaded, annotations object still have an association only with human_T1 sequence (if annotations object exists and has the same name as before reloading),
    //there is no errors in the log.
    GTUtilsProjectTreeView::doubleClickItem(os, "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(5000);
    GTUtilsAnnotationsTreeView::findFirstAnnotation(os);

    CHECK_SET_ERR(!l.hasError(), "There is error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_4008) {
//    1. Open "samples/CLUSTALW/COI.aln".
//    2. Use Alignment Viewer context menu -> View.
//    Expected: "Show offsets" option is enabled.
//    3. Open "_common_data/clustal/big.aln".
//    4. Use Alignment Viewer context menu -> View.
//    Expected: "Show offsets" option is enabled.
//    Current: "Show offsets" option is disabled.

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "MSAE_MENU_VIEW" << "show_offsets",
                                                            PopupChecker::IsEnabled | PopupChecker::IsChecable));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep();

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "MSAE_MENU_VIEW" << "show_offsets",
                                                            PopupChecker::IsEnabled | PopupChecker::IsChecable));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
}

GUI_TEST_CLASS_DEFINITION(test_4009) {
    //1. Open file "_common_data/clustal/big.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //2. Open "Export Consensus" OP tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    //3. Press "Export" button
    GTWidget::click(os, GTWidget::findWidget(os, "exportBtn"));

    //4. Remove "big.aln" document
    GTUtilsDocument::removeDocument(os, "big.aln");

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os)==0, "some tasks were not cancelled")

    //Current state: the task hangs, debug error occured with message "Infinite wait has timed out"
}

GUI_TEST_CLASS_DEFINITION(test_4010) {
//    1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Open the PCR OP tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

//    3. Enter the forward primer: AAGGAAAAAATGCT.
    GTUtilsOptionPanelSequenceView::setForwardPrimer(os, "AAGGAAAAAATGCT");

//    4. Enter the reverse primer: AGCATTTTTTCCTT.
    GTUtilsOptionPanelSequenceView::setReversePrimer(os, "AGCATTTTTTCCTT");

//    5. Click the Primers Details dialog.
//    Expected: the primers are whole dimers, 14 red lines.
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTTextEdit::containsString(os, GTWidget::findExactWidget<QTextEdit *>(os, "textEdit"), "||||||||||||||");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PrimersDetailsDialogFiller(os, new Scenario));
    GTUtilsOptionPanelSequenceView::showPrimersDetails(os);
}

GUI_TEST_CLASS_DEFINITION(test_4011){
    GTLogTracer l;
//    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
//    2. Open sample "Align sequences with MUSCLE"
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
//    3. Align attached file
    GTUtilsWorkflowDesigner::click(os, "Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/regression/4011/human_T1.aln");
    GTUtilsWorkflowDesigner::runWorkflow(os);
//    Current state:
//    Runtime error occured(x86 version of UGENE)
//    Windows hangs(x64 version)
    l.checkMessage("Nothing to write");
}

GUI_TEST_CLASS_DEFINITION(test_4013) {
    GTFileDialog::openFile(os, dataDir+"samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    int columnsNumber = GTUtilsMSAEditorSequenceArea::getNumVisibleBases(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(columnsNumber - 10, 0), QPoint(columnsNumber, 10), GTGlobals::UseMouse);

    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick( Qt::Key_Space);

    GTGlobals::sleep();
    QRect oldRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);

    GTKeyboardDriver::keyClick( 'f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("ACCCTATTTTATACCAACAAACTare");
    GTKeyboardDriver::keyClick( Qt::Key_Enter);
    GTThread::waitForMainThread();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, oldRect);
}

GUI_TEST_CLASS_DEFINITION(test_4022) {
    //1. Download sequence: http://www.ncbi.nlm.nih.gov/nuccore/CM000265.1
    //2. Open the sequence by UGENE.
    //3. Select the whole sequence.
    //4. Use context menu {Copy->Copy sequence}.
    QString sequence;
    for (int i=0; i<1376256; i++) {
        sequence += "AAAACCCCGGGGTTTTAAAACCCCGGGGTTTTAAAACCCCGGGGTTTTAAAACCCCGGGGTTTT";
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(sequence);

    //5. Use menu {File->New document from text...}.
    //Expected state: "Create document" dialog appeared.
    //6. Paste sequence into the dialog.
    //Expected: UGENE does not crash.
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");
            QPlainTextEdit *plainText = dialog->findChild<QPlainTextEdit*>("sequenceEdit");
            CHECK_SET_ERR(plainText != NULL, "plain text not found");
            GTWidget::click(os, plainText);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "amount of data"));
            GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);
            GTGlobals::sleep();

            GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "An error occurred", "ExceptionWarning"));
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes, "amount of data"));
            GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateDocumentFiller(os, new Scenario()));
    GTGlobals::sleep();

    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "New document from text...", GTGlobals::UseKey);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4026) {
    //1. Open "samples/Genbank/sars.gb".
    //Expected: there are a lot of annotations in the panoramic and details views.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Close the MDI window.
    GTKeyboardDriver::keyClick( 'w', Qt::ControlModifier);

    //3. Double click the sequence in the project.
    GTUtilsProjectTreeView::doubleClickItem(os, "NC_004718");
    GTGlobals::sleep(1000);

    //Expected: there is the same amount of annotations in the panoramic and details views.
    //Actual: annotations are now shown in the views. Their locations and qualifier names are deleted.
    QString value = GTUtilsAnnotationsTreeView::getQualifierValue(os, "evidence", "5'UTR");
    CHECK_SET_ERR("not_experimental" == value, QString("Unexpected qualifier value"));
}

GUI_TEST_CLASS_DEFINITION(test_4030) {
    //1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open the "Statistics" tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);

    //3. Edit the alignment.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(1, 1), QPoint(1, 1));
    GTKeyboardDriver::keyClick( Qt::Key_Space);

    //Expected state: hint about reference sequence is hidden
    QWidget *label = GTWidget::findWidget(os, "refSeqWarning");
    CHECK_SET_ERR(!label->isVisible(), "Label is shown");
}

GUI_TEST_CLASS_DEFINITION(test_4033) {
    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            QWidget *cbExistingTable = GTWidget::findWidget(os, "cbExistingTable", dialog);
            QWidget *tbBrowseExistingTable = GTWidget::findWidget(os, "tbBrowseExistingTable", dialog);
            QWidget *leNewTablePath = GTWidget::findWidget(os, "leNewTablePath", dialog);
            QWidget *tbBrowseNewTable = GTWidget::findWidget(os, "tbBrowseNewTable", dialog);

            GTWidget::click(os, GTWidget::findWidget(os, "rbExistingTable", dialog));
            CHECK_SET_ERR(cbExistingTable->isEnabled() && tbBrowseExistingTable->isEnabled(), "Create annotation dialog controls are disabled unexpectedly");
            CHECK_SET_ERR(!leNewTablePath->isEnabled() && !tbBrowseNewTable->isEnabled(), "Create annotation dialog controls are enabled unexpectedly");

            GTWidget::click(os, GTWidget::findWidget(os, "rbCreateNewTable", dialog));
            CHECK_SET_ERR(!cbExistingTable->isEnabled() && !tbBrowseExistingTable->isEnabled(), "Create annotation dialog controls are enabled unexpectedly");
            CHECK_SET_ERR(leNewTablePath->isEnabled() && tbBrowseNewTable->isEnabled(), "Create annotation dialog controls are disabled unexpectedly");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    //1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Ctrl + N.
    //Expected: if the "Existing table" radio button is not checked, then the URL line edit is disabled.
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_4034) {
    //1. Open "samples/Genbank/murine.gb".
    //2. Ctrl + N.
    //There are two check boxes for switching the location type. Usually, when you choose between several actions (switch them), GUI shows you radio buttons, not check boxes.
    //Check boxes are used to switch on/off an option but not to choose between options. In this dialog, you even can't switch off the check box when you click it.
    //It is a wrong behavior for this graphic primitive
    //Solution: replace the check boxes with radio buttons.
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "exon", "annotation", "200..300",
        sandBoxDir + "ann_test_4034.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD" << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4035) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Click the "Build tree" button on the main toolbar.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFillerPhyML(os, false, 10));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    //3. Select the "PhyML" tool, set "Bootstrap" option to 10, build the tree
    GTUtilsTaskTreeView::waitTaskFinished(os);

    int labelsCount = GTUtilsPhyTree::getDistances(os).count();
    CHECK_SET_ERR(labelsCount == 49, QString("Number of distances is incorrect: %1").arg(labelsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4036) {
//     1. Open "_common_data/clustal/gap_column.aln".
//     2. MSA sequence area context menu -> Edit -> Remove columns of gaps.
//     3. Choose "Remove all gap-only columns".
//     4. Click "Remove".
//     UGENE 1.16-dev: it take ~15 minutes to remove gaps.
//     UGENE 1.15.1: it takes ~5 seconds to remove gaps.

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "gap_column.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "remove_columns_of_gaps"));
    GTUtilsDialog::waitForDialog(os, new DeleteGapsDialogFiller(os, 1));

    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4045) {
    //1. Open "murine.gb"
    //2. Find ORFs
    //3. Unload "murine.gb"
    //4. Open "murine.gb"
    //Current state: SAFE_POINT is triggered
    GTLogTracer logTracer;
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os) : Filler(_os, "ORFDialogBase"){}
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );
            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox*>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );

            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        }
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os));
    GTWidget::click(os, GTAction::button(os, "Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::unloadDocument(os, "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::loadDocument(os, "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_4046){
//    Open 'human_T1.fa'
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Create a new annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Add" << "New annotation...");
    GTGlobals::sleep();
//    Current state: new annotation has an empty qualifier 'note'
    QTreeWidgetItem* ann = GTUtilsAnnotationsTreeView::findItem(os, "ann1");
//    Expected state: new annotation does not have any qualifiers
    CHECK_SET_ERR(ann->childCount() == 0, QString("Unexpected qualifier number: %1").arg(ann->childCount()));
}

GUI_TEST_CLASS_DEFINITION(test_4047){
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            GTGlobals::sleep(500);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
#ifdef Q_OS_MAC
            //dirty hack for mac
            GTGlobals::sleep(500);
            if(QApplication::activeModalWidget() != NULL){
                GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
            }
#endif
        }
    };


    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw DNA-Seq Data Processing", new custom()));
    GTUtilsWorkflowDesigner::addSample(os, "Raw DNA-Seq data processing");
    GTGlobals::sleep();
    GTThread::waitForMainThread();

    CHECK_SET_ERR(GTAction::button(os, "Show wizard")->isVisible() == false, "'Show wizard' button should not be at toolbar!");
}

GUI_TEST_CLASS_DEFINITION(test_4059) {
    GTLogTracer l;
    //1. Open "_common_data/text/text.txt".
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "Plain text"));
    GTFileDialog::openFile(os, testDir + "_common_data/text/text.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Delete the "Text" object.
    GTUtilsProjectTreeView::click(os, "text");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    //3. Unload the document.
    MessageBoxDialogFiller *filler = new MessageBoxDialogFiller(os, "No");
    GTUtilsDialog::waitForDialog(os, filler);
    GTUtilsDocument::unloadDocument(os, "text.txt", false);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Load the document.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "text.txt"));
    GTKeyboardDriver::keyClick( Qt::Key_Enter);

    //Expected: no safe points triggered.
    CHECK_SET_ERR(!l.hasError(), "Errors in log");
}

GUI_TEST_CLASS_DEFINITION(test_4064) {
//    1. Copy "_common_data/bam/scerevisiae.bam" and "_common_data/bam/scerevisiae.bam.bai" to separate folder
//    2. Rename "scerevisiae.bam.bai" to "scerevisiae.bai"
//    3. Open "scerevisiae.bam" in UGENE
//    Expected state: "Import BAM file" dialog appeared - there is no "Index is not available" warning message.

    GTFile::copy(os, testDir + "_common_data/bam/scerevisiae.bam", sandBoxDir + "test_4064.bam");

    class CustomImportBAMDialogFiller : public Filler {
    public:
        CustomImportBAMDialogFiller(HI::GUITestOpStatus &os, bool warningExistence)
            : Filler(os, "Import BAM File"),
              warningExistence(warningExistence) {}
        virtual void run() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QLabel *label = qobject_cast<QLabel*>(GTWidget::findWidget(os,"indexNotAvailableLabel",dialog));
            CHECK_SET_ERR(label != NULL, "indexNotAvailableLabel not found");
            CHECK_SET_ERR(label->isVisible() == warningExistence, "Warning message is shown");

            QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button !=NULL, "cancel button is NULL");
            GTWidget::click(os, button);
        }
    private:
        bool warningExistence;
    };

    GTUtilsDialog::waitForDialog(os, new CustomImportBAMDialogFiller(os, true));
    GTFileDialog::openFile(os, sandBoxDir, "test_4064.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFile::copy(os, testDir + "_common_data/bam/scerevisiae.bam.bai", sandBoxDir + "test_4064.bai");
    GTUtilsDialog::waitForDialog(os, new CustomImportBAMDialogFiller(os, false));
    GTFileDialog::openFile(os, sandBoxDir, "test_4064.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4065) {
/* 1. Open _common_data/scenarios/_regression/4065/example_bam.bam
 * 2. Check log for error: "No bam index given, preparing sequential import"
*/
    GTFile::copy(os, testDir + "_common_data/scenarios/_regression/4065/example_bam.bam", sandBoxDir + "example_bam.bam");
    GTFile::copy(os, testDir + "_common_data/scenarios/_regression/4065/example_bam.bam.bai", sandBoxDir + "example_bam.bam.bai");
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "/test_4065.ugenedb"));
    GTFileDialog::openFile(os, sandBoxDir + "example_bam.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool hasMessage = l.checkMessage("No bam index given");
    CHECK_SET_ERR(false == hasMessage , "Error message is found. Bam index file not found.");
}

GUI_TEST_CLASS_DEFINITION(test_4070) {
//    1. Open file "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Switch on collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode(os);

//    3. Expand "Conocephalus_discolor" group.
    GTUtilsMsaEditor::toggleCollapsingGroup(os, "Conocephalus_discolor");

//    Expected state: the overview is calculated and shown.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *graphOverview = GTUtilsMsaEditor::getGraphOverview(os);
    CHECK_SET_ERR(NULL != graphOverview, "Graph overview widget is NULL");

    bool colorFound = false;
    for (int i = 0; i < graphOverview->width() && !colorFound; i++) {
        for (int j = 0; j < graphOverview->height() && !colorFound; j++) {
            if (QColor(0, 0, 0) == GTUtilsMsaEditor::getGraphOverviewPixelColor(os, QPoint(i, j))) {
                colorFound = true;
            }
        }
    }

    CHECK_SET_ERR(colorFound, "The overview doesn't contain white color");
}

GUI_TEST_CLASS_DEFINITION(test_4071) {
    GTLogTracer l;
    //1. Open any file (for example, human_T1.fa).
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select the object "[s] human_T1 (UCSC April 2002 chr7:115977709-117855134)".
    GTUtilsProjectTreeView::click(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    //3. Press "delete" key.
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    //4. Unload and load document.
    MessageBoxDialogFiller *filler = new MessageBoxDialogFiller(os, "No");
    GTUtilsDialog::waitForDialog(os, filler);
    GTUtilsDocument::unloadDocument(os, "human_T1.fa", false);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa"));
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    //Expected: no safe points are triggered.
    CHECK_SET_ERR(!l.hasError(), "Errors in log");
}

GUI_TEST_CLASS_DEFINITION(test_4072) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *hSeqScroll = GTWidget::findWidget(os, "horizontal_sequence_scroll");
    CHECK_SET_ERR(hSeqScroll != NULL, "No scroll bar at the bottom of sequence area");
    CHECK_SET_ERR(hSeqScroll->isVisible(), "Scroll bar at the bottom of sequence area is invisible");

    QWidget *vSeqScroll = GTWidget::findWidget(os, "vertical_sequence_scroll");

    CHECK_SET_ERR(vSeqScroll != NULL, "No scroll bar at the bottom of sequence area");
    CHECK_SET_ERR(!vSeqScroll->isVisible(), "Scroll bar at the rigth side of sequence area is visible");

    QWidget* hNameScroll = GTWidget::findWidget(os, "horizontal_names_scroll");
    CHECK_SET_ERR(hNameScroll != NULL, "No scroll bar at the bottom of name list area");

    QSplitter* splitter = qobject_cast<QSplitter *>(GTWidget::findWidget(os, "msa_editor_horizontal_splitter"));
    CHECK_SET_ERR(splitter != NULL, "MSA Splitter not found");
    QSplitterHandle *handle = splitter->handle(1);
    CHECK_SET_ERR(handle != NULL, "MSA Splitter handle is NULL");

    QWidget *nameList = GTWidget::findWidget(os, "msa_editor_name_list");
    CHECK_SET_ERR(nameList != NULL, "MSA Editor name list not found");

    GTWidget::click(os, handle);
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

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/fungal - all.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);
    hNameScroll = GTWidget::findWidget(os, "horizontal_names_scroll");
    CHECK_SET_ERR(hNameScroll != NULL, "No scroll bar at the bottom of name list area for fungal-all.aln");
    CHECK_SET_ERR(hNameScroll->isVisible(), "Scroll bar at the bottom of name list area is not visible for fungal-all.aln");

}

GUI_TEST_CLASS_DEFINITION(test_4084) {
    //1. Open "_common_data/fasta/human_T1_cutted.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Create any annotation.
    //Expected state: there is an annotation group with an annotation within.
    //Current state: there is an annotation group with two similar annotations within.
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "group", "feature", "50..60"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Add" << "New annotation...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "group  (0, 1)");
    CHECK_SET_ERR(NULL != annotationGroup, "Wrong annotations number");
}

GUI_TEST_CLASS_DEFINITION(test_4087) {
    // 1. Open murine.gb
    // 2. Open Find Pattern tab
    // 3. Input pattern
    // 4. Click Next a few times
    // Expected state: the results are selected one by one from left to right, no random selection

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "U");
    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern(os);
    GTGlobals::sleep(200);

    QLabel *label = dynamic_cast<QLabel*>(GTWidget::findWidget(os, "lblErrorMessage"));

    CHECK_SET_ERR(label->isVisible(), "Warning is not shown 1");
    CHECK_SET_ERR(label->text().contains("Info"), "Warning is not shown 2");
    CHECK_SET_ERR(!label->text().contains("Warning"), "Warning is shown");

}

GUI_TEST_CLASS_DEFINITION(test_4091) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QSet<GObjectType> acceptableTypes;
    acceptableTypes << GObjectTypes::SEQUENCE << GObjectTypes::ANNOTATION_TABLE;
    QMap<QString, QStringList> doc2Objects;
    doc2Objects["human_T1.fa"] << "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    doc2Objects["NC_014267.1.gb"] << "NC_014267" << "NC_014267 features";
    doc2Objects["PBR322.gb"] << "SYNPBR322" << "SYNPBR322 features";
    doc2Objects["sars.gb"] << "NC_004718" << "NC_004718 features";

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, doc2Objects, acceptableTypes,
        ProjectTreeItemSelectorDialogFiller::Separate));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__ADD_OBJECT));
    GTUtilsProjectTreeView::click(os, "CVU55762.gb", Qt::RightButton);

    GTUtilsTaskTreeView::checkTask(os, "Add objects to document");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QModelIndex docIndex = GTUtilsProjectTreeView::findIndex(os, "CVU55762.gb");
    GTUtilsProjectTreeView::checkItem(os, "CVU55762", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "CVU55762 features", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "NC_014267", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "NC_014267 features", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "SYNPBR322", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "SYNPBR322 features", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "NC_004718", docIndex);
    GTUtilsProjectTreeView::checkItem(os, "NC_004718 features", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_4093) {
    //1. Open "human_T1.fa"
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Call context menu on the document
    //{ Export / Import->Import annotations from CSV... }
    //Expected state : "Import Annotations from CSV" dialog appeared
    //3. Select the attached file for reading
    //4. Copy tab to clipboard and insert to "Column separator" field
    //5. Select 3rd column as "Start position", 4th column as "End position" and 5th as "Complement strand mark"
    //6. Press "Run"

    ImportAnnotationsToCsvFiller::RoleParameters r;
    r << ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::StartParameter(false))
        << ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::EndParameter(true))
        << ImportAnnotationsToCsvFiller::RoleColumnParameter(4, new ImportAnnotationsToCsvFiller::StrandMarkParameter(false, ""));

    ImportAnnotationsToCsvFiller *filler = new ImportAnnotationsToCsvFiller(os, testDir + "_common_data/scenarios/_regression/4093/test.xls",
        sandBoxDir + "test_4093.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "	", 0, "", true, false, "misc_feature", r);

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import" << "Import annotations from CSV file..."));
    GTUtilsDialog::waitForDialog(os, filler);
    GTUtilsProjectTreeView::click(os, "human_T1.fa", Qt::RightButton);

    //Expected state: UGENE doesn't crash
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4095) {
/* 1. Open file "test/_common_data/fasta/fa1.fa"
 * 2. Call context menu on the sequence view { Edit sequence -> Reverse sequence }
 *   Expected state: nucleotides order has reversed
 *   Current state: nothing happens
*/
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<ADV_MENU_EDIT<<ACTION_EDIT_RESERVE_SEQUENCE));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //GTCA
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 4));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Select" << "Sequence region"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os,"ADV_single_sequence_widget_0"));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier );
    GTGlobals::sleep(200);
    const QString selectionContent = GTClipboard::text( os );
    CHECK_SET_ERR( "GTCA" == selectionContent, "Sequence reversing is failed" );
}

namespace {
    QString getFileContent(const QString &path) {
        QFile file(path);
        CHECK(file.open(QFile::ReadOnly), QString());
        QTextStream fileReader(&file);
        return fileReader.readAll();
    }

}
GUI_TEST_CLASS_DEFINITION(test_4096) {
    // 1. Open "human_T1.fa"
    // 2. Use context menu on sequence object
    // {Export/Import->Export sequences as alignment}
    // Expected state: "Export Sequences as Alignment" dialog appeared
    // 3. Press "Export"
    // Current state: only part of human_T1(128000 nb) exported to alignment

    class ExportSeqsAsMsaScenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");

            QCheckBox *addToProjectBox = qobject_cast<QCheckBox *>(GTWidget::findWidget(os, "addToProjectBox", dialog));
            CHECK_SET_ERR(addToProjectBox->isChecked(), "'Add document to project' checkbox is not set");

            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "fileNameEdit", dialog));
            GTLineEdit::setText(os, lineEdit, sandBoxDir + "test_4096.aln");

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT));
    GTUtilsDialog::waitForDialog(os, new ExportSequenceAsAlignmentFiller(os, new ExportSeqsAsMsaScenario));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::checkItem(os, "test_4096.aln");

    const QString referenceMsaContent = getFileContent(testDir + "_common_data/clustal/test_4096.aln");
    const QString resultMsaContent = getFileContent(sandBoxDir + "test_4096.aln");
    CHECK_SET_ERR(!referenceMsaContent.isEmpty() && referenceMsaContent == resultMsaContent, "Unexpected MSA content");

}
GUI_TEST_CLASS_DEFINITION(test_4097) {
/* 1. Open "_common_data/vector_nti_sequence/unrefined.gb".
 * 2. Export the document somewhere to the vector NTI sequence format.
 *   Expected state: the saved file contains only 8 entries in the COMMENT section and doesn't contain "Vector_NTI_Display_Data_(Do_Not_Edit!)" comment.
*/
    GTFileDialog::openFile(os, testDir + "_common_data/vector_nti_sequence", "unrefined.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, sandBoxDir, "test_4097.gb", ExportDocumentDialogFiller::VectorNTI, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export document"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "unrefined.gb"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QString resultFileContent = getFileContent(sandBoxDir + "test_4097.gb");
    CHECK_SET_ERR(false == resultFileContent.contains("Vector_NTI_Display_Data_(Do_Not_Edit!)",Qt::CaseInsensitive), "Unexpected file content");
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
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4099", "p4228.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os,"CDS");
    CHECK_SET_ERR( 2 == items.length(), "CDS annotations count is not 2");
    foreach (QTreeWidgetItem* item, items) {
        if (item->text(2)=="1656..2450")
        {
            CHECK_SET_ERR("Tn5 neomycin resistance" == GTUtilsAnnotationsTreeView::getQualifierValue(os,"label",item), "Wrong label value");
            break;
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_4100) {
//    1. Create or open a file with a few annotations
//    2. Import that file to shared database and open it
//    Expected state: annotations in database are the same as the annotations from step 1

    ADVSingleSequenceWidget* wgt = GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/genbank/", "JQ040024.1.gb", "JQ040025");
    CHECK_SET_ERR(wgt != NULL, "No ADVSignleSequenceWidget found");
    ADVSequenceObjectContext* ctx = wgt->getActiveSequenceContext();
    CHECK_SET_ERR(ctx != NULL, "Sequence context is NULL");
    QSet<AnnotationTableObject*> annTables = ctx->getAnnotationObjects();

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QList<ImportToDatabaseDialogFiller::Action> actions;
    QVariantMap addFileAction;
    addFileAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, testDir + "_common_data/genbank/JQ040024.1.gb");
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFileAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__add_menu" << "action_project__import_to_database"));
    GTUtilsProjectTreeView::click(os, "ugene_gui_test", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(10000);

    GTUtilsSharedDatabaseDocument::doubleClickItem(os, databaseDoc, "JQ040024.1");
    QModelIndex idx = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, "/JQ040024.1/JQ040025");
    GTUtilsProjectTreeView::doubleClickItem(os, idx);
    GTGlobals::sleep();

    ADVSingleSequenceWidget* wgtDB = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(wgtDB != NULL, "No ADVSingleSequenceWidget fond");
    ctx = wgtDB->getActiveSequenceContext();
    CHECK_SET_ERR(ctx != NULL, "Sequence context is NULL");
    QSet<AnnotationTableObject*> annTablesDB = ctx->getAnnotationObjects();

    CHECK_SET_ERR(annTables.size() == annTablesDB.size(), "Number of annotation tables imported is incorrect");
    CHECK_SET_ERR(annTables.size() == 1, "Number of annotation tables is incorrect");

    CHECK_SET_ERR(annTables.values().first() != NULL, "AnnotationTable is NULL");
    QList<Annotation *> annInitial = annTables.values().first()->getAnnotations();
    CHECK_SET_ERR(annTablesDB.values().first() != NULL, "AnnotationTable is NULL");
    QList<Annotation *> annImported = annTablesDB.values().first()->getAnnotations();

    foreach (Annotation *ann, annInitial) {
        bool found = false;
        foreach (Annotation *aImported, annImported) {
            if (*ann->getData() == *aImported->getData()) {
                found = true;
                break;
            }
        }
        CHECK_SET_ERR(found == true, QString("Annotation '%1' was not imported").arg(ann->getName()));
    }

    GTUtilsSharedDatabaseDocument::doubleClickItem(os, databaseDoc, "JQ040024.1");
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();
    GTUtilsSharedDatabaseDocument::disconnectDatabase(os, databaseDoc);
}

GUI_TEST_CLASS_DEFINITION(test_4104) {
    GTLogTracer l;
    //1. Open the attached workflow file.
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4104/test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Set file "data/samples/Genbank/murine.gb" as input.
    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Genbank/murine.gb");

    //3. Run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow(os);

    //Expected state : a result file has been produced.It's a copy of murine.gb
    //Current state : the "Write Sequence" worker gives the "Nothing to write" error in the log.
    GTUtilsLog::check(os, l);

    HIWebElement button = GTUtilsDashboard::findElement(os, "Dataset 1.gb", "BUTTON");
    GTUtilsDashboard::click(os, button);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QString activeWindowName = GTUtilsMdi::activeWindow(os)->windowTitle();
    CHECK_SET_ERR(activeWindowName == "Dataset 1 [s] NC_001363", "Unexpected active window name: " + activeWindowName);
    GTUtilsProjectTreeView::findIndex(os, "NC_001363");
    GTUtilsProjectTreeView::findIndex(os, "NC_001363 features");
}

GUI_TEST_CLASS_DEFINITION(test_4106){
/* 1. Open "data/samples/CLUSTALW/ty3.aln.gz".
 * 2. Select a sequence that is two sequences above the last visible sequence in the name list area.
 * 3. Press and hold "shift" key.
 * 4. Click "down" key twice.
 *   Expected state: three sequences are selected, the msa is not scrolled down.
 * 5. Click "down" again.
 *   Expected state: four sequences are selected
 *   Current state: the state is not changed.
*/
    GTFileDialog::openFile( os, dataDir + "samples/CLUSTALW", "ty3.aln.gz" );
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    MSAEditorSequenceArea* msaEdistorSequenceArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
    const int endPos = msaEdistorSequenceArea->getEditor()->getUI()->getScrollController()->getLastVisibleRowNumber(msaEdistorSequenceArea->height());

    GTUtilsMSAEditorSequenceArea::click( os, QPoint( -5, endPos-1 ) );
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect( os, QRect( 0, endPos-1, 1234, 1 ) );

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down );
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Down );
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect( os, QRect( 0, endPos-1, 1234, 3 ) );

    GTGlobals::sleep(200);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down );
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect( os, QRect( 0, endPos-1, 1234, 4 ) );

}

GUI_TEST_CLASS_DEFINITION(test_4110){
    GTFile::copy(os, dataDir + "samples/FASTA/human_T1.fa", sandBoxDir + "test_4110_human_T1.fa");
    GTUtilsMdi::click(os, GTGlobals::Close);
//    1. Connect to the shared database "ugene_gui_test_win" located on "ugene-quad-ubuntu".
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
//    2. Open sequence view for sequence from ugene_gui_test_win:/view_test_0001/NC_001363.
    QModelIndex fol = GTUtilsProjectTreeView::findIndex(os, "test_4110");
    QModelIndex seqFol = GTUtilsProjectTreeView::findIndex(os, "NC_001363", fol);
    QModelIndex annFol = GTUtilsProjectTreeView::findIndex(os, "NC_001363 features", fol);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, seqFol));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();
//    Expected state: sequence is opened with its annotations on the same view.
    QList<QTreeWidgetItem*> annotationsInFile = GTTreeWidget::getItems(GTUtilsAnnotationsTreeView::getTreeWidget(os)->invisibleRootItem());
    int num = annotationsInFile.size();
    CHECK_SET_ERR(num == 14, QString("1: unexpected annotations number: %1").arg(num));
//    3. Open file "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, sandBoxDir + "test_4110_human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    4. Drag&drop annotation object "ugene_gui_test_win:/view_test_0001/NC_001363 features" to the sequence view.
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, annFol, GTWidget::findWidget(os, "ADV_single_sequence_widget_0", GTUtilsMdi::activeWindow(os)));
    GTGlobals::sleep(1000);
//    Expected state: annotations successfully associated with a new sequence.
    annotationsInFile = GTTreeWidget::getItems(GTUtilsAnnotationsTreeView::getTreeWidget(os)->invisibleRootItem());
    num = annotationsInFile.size();
    CHECK_SET_ERR(num == 14, QString("2: unexpected annotations number: %1").arg(num));
//    5. Close both sequence views.
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsMdi::click(os, GTGlobals::Close);
//    6. Open a sequence view for "human_T1" again.
    GTUtilsProjectTreeView::doubleClickItem(os, "test_4110_human_T1.fa");
    GTGlobals::sleep();
//    Expected state: it still displays annotations along with the sequence
    annotationsInFile = GTTreeWidget::getItems(GTUtilsAnnotationsTreeView::getTreeWidget(os)->invisibleRootItem());
    num = annotationsInFile.size();
    CHECK_SET_ERR(num == 14, QString("3: unexpected annotations number: %1").arg(num));
//    Current state: only sequence is shown
}

GUI_TEST_CLASS_DEFINITION(test_4111){
    GTLogTracer l;
    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/bowtie/index/",
        "e_coli.1.ebwt",
        testDir + "_common_data/fastq/",
        "short_sample.fastq",
        AlignShortReadsFiller::Parameters::Bowtie);

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_4111.ugenedb"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_4113){
//    1. Connect to a shared DB.
//    2. Import the folder "data/samples/Assembly" to it
//    3. Import ugenedb file
//    Expected state: import is completed successfully

    GTLogTracer l;

    Document* doc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::importDirs(os, doc, "/test_4113", QStringList() << dataDir + "/samples/Assembly/");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::importFiles(os, doc, "/test_4113", QStringList() << testDir + "/_common_data/ugenedb/scerevisiae.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_4116) {
//    1. Open the Primer Library.
    GTUtilsPrimerLibrary::openLibrary(os);

//    2. Click "Import primer(s)".

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
//    Expected: the dialog is modal, the "OK" button is disabled.
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");
            CHECK_SET_ERR(GTUtilsDialog::buttonBox(os, dialog) != NULL, "ButtonBox is NULL");

            QWidget *okButton = GTUtilsDialog::buttonBox(os, dialog)->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(NULL != okButton, "Export button is NULL");
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is unexpectedly enabled");

//    3. Add human_T1.fa.
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/human_T1.fa"));
            GTWidget::click(os, GTWidget::findButtonByText(os, "Add file(s)", dialog));

//    4. Click the added item.
            const QString filePath = QDir::cleanPath(QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath());
            GTListWidget::click(os, GTWidget::findExactWidget<QListWidget *>(os, "lwFiles", dialog), filePath);

//    Expected: the "Remove" button is enabled.
            QWidget *removeButton = GTWidget::findWidget(os, "pbRemoveFile", dialog);
            CHECK_SET_ERR(NULL != removeButton, "Remove button is NULL");
            CHECK_SET_ERR(removeButton->isEnabled(), "Remove button is unexpectedly disabled");

//    5. Choose "Shared database" in the combobox.
            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbSource", dialog), "Shared database");

//    Expected: the "OK" button is disabled.
            CHECK_SET_ERR(!okButton->isEnabled(), "OK button is unexpectedly enabled");

//    6. Choose "Local file(s)" in the combobox.
            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbSource", dialog), "Local file(s)");

//    Expected: the "OK" button is enabled.
            CHECK_SET_ERR(okButton->isEnabled(), "OK button is unexpectedly disabled");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImportPrimersDialogFiller(os, new Scenario));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Import);
}

GUI_TEST_CLASS_DEFINITION(test_4117){
    GTLogTracer l;
    QDir().mkpath(testDir + "_common_data/scenarios/sandbox/space containing dir");
    GTFile::copy(os, testDir + "_common_data/fastq/short_sample.fastq",
                 testDir + "_common_data/scenarios/sandbox/space containing dir/short_sample.fastq");
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Quality control by FastQC");

    GTUtilsWorkflowDesigner::click(os, "FASTQ File List");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/scenarios/sandbox/space containing dir/short_sample.fastq" );
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_4118){
//1. Add sample raw data processing
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    QMap<QString, QVariant> parameters;
    parameters.insert("FASTQ files", QVariant(QDir().absoluteFilePath(testDir + "_common_data/fastq/lymph.fastq")));
    parameters.insert("Adapters", QVariant(""));

    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw RNA-Seq Data Processing", QStringList()<<"Skip mapping"<<"Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw RNA-Seq Data Processing Wizard", QStringList(), parameters));
    GTUtilsWorkflowDesigner::addSample(os, "Raw RNA-Seq data processing");

    GTUtilsWorkflowDesigner::runWorkflow(os);
    //GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);

}

GUI_TEST_CLASS_DEFINITION(test_4121) {
//    1. Open the Primer Library.
//    2. Select two primes.
//    3. Click "Export primer(s)".
//    4. There is no 'Raw' format available(raw format is valid only for one primer export)

    class test_4121 : public CustomScenario {
    public:
        test_4121(bool isRawPresent)
            : isRawPresent(isRawPresent) {}
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");

            QComboBox* cbFormat = dialog->findChild<QComboBox*>("cbFormat");
            CHECK_SET_ERR(cbFormat != NULL, "cbFormat not found");

            if (isRawPresent) {
                CHECK_SET_ERR(cbFormat->findText("Raw sequence") != -1, "raw format is present");
            } else {
                CHECK_SET_ERR(cbFormat->findText("Raw sequence") == -1, "raw format is present");
            }

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    private:
        bool isRawPresent;

    };

    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::addPrimer(os, "primer1", "ACGTA");
    GTUtilsPrimerLibrary::addPrimer(os, "primer2", "GTACG");

    GTUtilsPrimerLibrary::selectPrimers(os, QList<int>() << 0 << 1);

    GTUtilsDialog::waitForDialog(os, new ExportPrimersDialogFiller(os, new test_4121(false)));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Export);

    GTUtilsPrimerLibrary::selectPrimers(os, QList<int>() << 0);

    GTUtilsDialog::waitForDialog(os, new ExportPrimersDialogFiller(os, new test_4121(true)));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Export);
}

GUI_TEST_CLASS_DEFINITION(test_4122) {
/* 1. Open "data/samples/Genbank/murine.gb".
 * 2. Search any existing pattern.
 * 3. Try to create annotations from the search results to the existing annotations table.
 *   Expected state: annotations are created.
 *   Current state: annotations are not created, a safe point is triggered
*/
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "GAGTTCTGAACACCCGGC", true);
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* annotationTable = GTUtilsAnnotationsTreeView::findItem(os, "NC_001363 features [murine.gb] *");
    CHECK_SET_ERR(NULL != annotationTable, "AnnotationTable (NC_001363 features [murine.gb]) is NULL or not changed");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "NC_001363 features [murine.gb] *"));
    GTMouseDriver::click();

    QTreeWidgetItem* newItem = GTUtilsAnnotationsTreeView::findItem(os, "Misc. Feature",annotationTable);
    CHECK_SET_ERR(NULL != newItem, "New annotation is NULL or not created");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "Misc. Feature"));
    GTMouseDriver::click();

}

GUI_TEST_CLASS_DEFINITION(test_4124) {
/* 1. Select {Tools -> NGS data analysis -> Map reads to reference...} menu item in the main menu.
 * 2. Select UGENE genome aligner, use input data samples/FASTA/human_T1.fa, unset "use 'best'-mode" option. The output path should be default. Align reads.
 *   Expected state: there is some result assembly.
 * 3. Remove the result document from the project, remove the result file. Align reads with same parameters again.
 *   Expected state: there is the same result assembly.
 *   Current state: a message box appears: "Failed to detect file format..."
*/

    class Scenario_test_4124: public CustomScenario{
    public:
        virtual void run(HI::GUITestOpStatus &os){
            QWidget* dialog = QApplication::activeModalWidget();
            QComboBox* methodNamesBox = GTWidget::findExactWidget<QComboBox*>(os, "methodNamesBox", dialog);
            GTComboBox::setIndexWithText(os, methodNamesBox, "UGENE Genome Aligner");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/human_T1.fa"));
            QWidget* addRefButton = GTWidget::findWidget(os, "addRefButton", dialog);
            GTWidget::click(os, addRefButton);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/human_T1.fa"));
            QWidget* addShortreadsButton = GTWidget::findWidget(os, "addShortreadsButton", dialog);
            GTWidget::click(os, addShortreadsButton);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils (os, sandBoxDir, "out.ugenedb", GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "setResultFileNameButton", dialog));

            QCheckBox* check = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "firstMatchBox"));
            CHECK_SET_ERR(check != NULL, "firstMatchBox not found!");
            GTCheckBox::setChecked(os, check, 0);

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new Scenario_test_4124()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(200);
    GTUtilsProjectTreeView::click(os, "out.ugenedb");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep(1000);
    QFile::remove(sandBoxDir+"out.ugenedb");
    GTGlobals::sleep();
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new Scenario_test_4124()));
    GTUtilsDialog::waitForDialogWhichMustNotBeRun(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
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
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4127", "merged_document.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    class OrfScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");

            QTabWidget *tabWidget = qobject_cast<QTabWidget*>(GTWidget::findWidget(os, "tabWidget", dialog));
            CHECK_SET_ERR(tabWidget != NULL, "tabWidget not found!");
            GTTabWidget::setCurrentIndex(os, tabWidget, 1);

            QRadioButton* radio = qobject_cast<QRadioButton *>(GTWidget::findWidget(os, "rbCreateNewTable", dialog));
                    //GTRadioButton::getRadioButtonByText(os, "Create new table", dialog);
            CHECK_SET_ERR(radio != NULL, "rbCreateNewTable not found!");
            GTRadioButton::click(os, radio);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new OrfDialogFiller(os, new OrfScenario));
    GTWidget::click(os, GTAction::button(os, "Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4131) {
    //1. Open "data/samples/FASTA/human_T1.fa".
    QString pattern("ATCGTAC");
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Search in Sequence" options panel tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));

    //3. Enter any valid pattern.
    GTKeyboardDriver::keySequence(pattern);
    GTGlobals::sleep(1000);

    //4. Set output annotations name : -=_\,.<>;:[]#()$
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Annotation parameters"));
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leAnnotationName"), "-=_\\,.<>;:[]#()$");

    //Create annotations from results.
    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : annotations are created, they are named exactly as you set.
    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "-=_\\,.<>;:[]#()$  (0, 3)");
    CHECK_SET_ERR(3 == annotationGroup->childCount(), "Unexpected annotations count");
}

GUI_TEST_CLASS_DEFINITION(test_4134) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QString trimBothValue = GTUtilsWizard::getParameter(os, "Trim both ends").toString();

            CHECK_SET_ERR(trimBothValue == "True", "unexpected 'Trim both ends value' : " + trimBothValue);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw DNA-Seq Data Processing Wizard", new custom()));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw DNA-Seq Data Processing", QStringList()<<"Single-end"));
    GTUtilsWorkflowDesigner::addSample(os, "Raw DNA-Seq data processing");
}

GUI_TEST_CLASS_DEFINITION(test_4141) {
    QWidget *appWindow = QApplication::activeWindow();
    //1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open the "Statistics" tab on the Options panel
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);

    //3. Check "Show distances column"
    //Expected state : distances column has appeared between the name list and the sequence area
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "showDistancesColumnCheck"));
    GTWidget::findWidget(os, "msa_editor_similarity_column");
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
    aligners << "BWA" << "Bowtie aligner" << "Bowtie 2 aligner";
    foreach (const QString& al, aligners) {
        GTUtilsExternalTools::removeTool(os, al);
    }

    class Scenario_test_4148: public CustomScenario{
    public:
        virtual void run(HI::GUITestOpStatus &os){
            QWidget* dialog = QApplication::activeModalWidget();

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/human_T1.fa"));
            QWidget* addRefButton = GTWidget::findWidget(os, "addRefButton", dialog);
            GTWidget::click(os, addRefButton);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTA/human_T1.fa"));
            QWidget* addShortreadsButton = GTWidget::findWidget(os, "addShortreadsButton", dialog);
            GTWidget::click(os, addShortreadsButton);

            QStringList aligners;
            aligners << "BWA" << "BWA-SW" << "BWA-MEM" << "Bowtie" << "Bowtie2";
            QComboBox* methodNamesBox = GTWidget::findExactWidget<QComboBox*>(os, "methodNamesBox", dialog);
            foreach (const QString& al, aligners) {
                GTComboBox::setIndexWithText(os, methodNamesBox, al);
                GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
                GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
            }

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new Scenario_test_4148()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
}

GUI_TEST_CLASS_DEFINITION(test_4150) {
    // Connect to the ugene-quad-ubuntu shared DB
    Document *dbDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    // Create a new folder "qwe" there
    GTUtilsSharedDatabaseDocument::createFolder(os, dbDoc, "/", "test_4150");
    GTFile::copy(os, dataDir + "samples/Genbank/murine.gb", sandBoxDir + "test_4150_murine.gb");

    // Open file "data/samples/Genbank/murine.gb"
    GTFileDialog::openFile(os, sandBoxDir, "test_4150_murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Close the sequence view
    GTUtilsMdi::click(os, GTGlobals::Close);

    // Drag&drop the sequence document to "qwe" in the DB
    QModelIndex from = GTUtilsProjectTreeView::findIndex(os, "test_4150_murine.gb");
    QModelIndex to = GTUtilsProjectTreeView::findIndex(os, "test_4150");

    class scenario_4150_proj_selector : public CustomScenario{
    public:
        virtual void run(HI::GUITestOpStatus &os) {
        GTGlobals::sleep(1000);
        QWidget *dialog = QApplication::activeModalWidget();

        QTreeView* treeView = dialog->findChild<QTreeView*>();
        const QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex(os, treeView, "test_4150_murine.gb");
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, treeView, documentIndex));
        GTMouseDriver::click();

        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

        }
    };

    class scenario_4150 : public CustomScenario {
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, new scenario_4150_proj_selector));
            GTWidget::click(os, GTWidget::findWidget(os, "pbAddObjects", dialog));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, new scenario_4150));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__add_menu"
                                                       << "action_project__import_to_database"));
    GTUtilsProjectTreeView::click(os,"test_4150", Qt::RightButton);
    GTGlobals::sleep(5000);
    to = GTUtilsProjectTreeView::findIndex(os, "test_4150");

    // Do double click on the sequence object from the "murine.gb" file
    QModelIndex seqFile = GTUtilsProjectTreeView::findIndex(os, "NC_001363", from);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, seqFile));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // Expected state: sequence view has opened, it contains a single set of annotations
    QList<QTreeWidgetItem*> annotationsInFile = GTTreeWidget::getItems(GTUtilsAnnotationsTreeView::getTreeWidget(os)->invisibleRootItem());
    int num = annotationsInFile.size();
    CHECK_SET_ERR(num == 14, QString("unexpected annotations number: %1").arg(num));

    // Do double click on the sequence object from the "murine.gb" folder
    GTUtilsMdi::click(os, GTGlobals::Close);
    QModelIndex murineFol = GTUtilsProjectTreeView::findIndex(os, "test_4150_murine.gb", to);
    QModelIndex seqFol = GTUtilsProjectTreeView::findIndex(os, "NC_001363", murineFol);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, seqFol));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // Expected state: sequence view has opened, it contains a single set of annotations
    annotationsInFile = GTTreeWidget::getItems(GTUtilsAnnotationsTreeView::getTreeWidget(os)->invisibleRootItem());
    num = annotationsInFile.size();
    CHECK_SET_ERR(num == 14, QString("unexpected annotations number: %1").arg(num));
}

GUI_TEST_CLASS_DEFINITION(test_4151) {
    //1. Open samples / Genbank / murine.gb.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Hide all views.
    QWidget* toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363");
    CHECK_SET_ERR(toolbar != NULL, "Cannot find views_tool_bar_NC_001363");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));

    //Expected state: Vertical scroll bar isn't shown.
    QScrollArea *advScrollArea = dynamic_cast<QScrollArea *>(GTWidget::findWidget(os, "annotated_DNA_scrollarea"));
    CHECK_SET_ERR(!advScrollArea->verticalScrollBar()->isVisible(), "Scrollbar is unexpectedly visible");
}

GUI_TEST_CLASS_DEFINITION(test_4153) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Show simple overview"));
    QWidget *overview = GTWidget::findWidget(os, "msa_overview_area_graph");
    CHECK_OP(os, );
    GTWidget::click(os, overview, Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    //    Select different highlighting schemes.
    QComboBox* highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));
    GTComboBox::setIndexWithText(os, highlightingScheme, "Conservation level");

    QSlider* thresholdSlider = GTWidget::findExactWidget<QSlider*>(os, "thresholdSlider");
    CHECK_SET_ERR(thresholdSlider != NULL, "Threshold slider is null");

    GTSlider::setValue(os, thresholdSlider, 78);

    QWidget *simpleOverview = GTWidget::findWidget(os, "msa_overview_area_simple");
    CHECK_SET_ERR(overview != NULL, "Simple overview is null");

    QPoint rightBottom = simpleOverview->rect().topRight();
    rightBottom += QPoint(-3,3);
    QColor curColor = GTWidget::getColor(os, simpleOverview, rightBottom);
    //GTWidget.getColor(os, )
    GTSlider::setValue(os, thresholdSlider, 81);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QColor newColor = GTWidget::getColor(os, simpleOverview, rightBottom);
    CHECK_SET_ERR(curColor != newColor, "Color is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_4156) {
//    1. Open _common_data/query/crash_4156.uql
//    2. Run the scheme with the human_T1.
//    Extected state: erro message appeared

    class scenario_4156 : public CustomScenario {
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);

            QSpinBox* memBox = dialog->findChild<QSpinBox*>("memorySpinBox");
            CHECK_SET_ERR(memBox != NULL, "memorySpinBox not found");
            GTSpinBox::setValue(os, memBox, 256, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    class filler_4156 : public Filler {
    public:
        filler_4156(HI::GUITestOpStatus& os)
            : Filler(os, "RunQueryDlg") {}
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "/samples/FASTA/human_T1.fa"));
            GTWidget::click(os, dialog->findChild<QToolButton*>("tbInFile"));

            QLineEdit* out = dialog->findChild<QLineEdit*>("outFileEdit");
            CHECK_SET_ERR(out != NULL, "outFileEdit not found");
            GTLineEdit::setText(os, out, sandBoxDir + "/test_4156.out");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new scenario_4156()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...");
    GTGlobals::sleep();

    GTFileDialog::openFile(os, testDir + "_common_data/query/crash_4156.uql");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new filler_4156(os));
    QAction* runAction = GTAction::findActionByText(os, "Run Schema...");
    CHECK_SET_ERR(runAction != NULL, "Run action not found");
    GTWidget::click(os, GTAction::button(os, runAction));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.hasError(), "There is no error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_4160) {
    QString pattern("TTGTCAGATTCACCA");
    GTFileDialog::openFile(os, dataDir+"samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));
    GTGlobals::sleep(500);

    GTKeyboardDriver::keySequence(pattern);
    GTGlobals::sleep(1000);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_4164){
    GTLogTracer l;
    QDir().mkpath(testDir + "_common_data/scenarios/sandbox/space dir");
    GTFile::copy(os, testDir + "_common_data/vcf/valid.vcf", testDir + "_common_data/scenarios/sandbox/space dir/valid.vcf");
//1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
//2. Add sample: snpEff
    GTUtilsWorkflowDesigner::addSample(os, "SnpEff");
//3. Set input file which contains spaces in path
    GTUtilsWorkflowDesigner::click(os, "Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/scenarios/sandbox/space dir/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);
//4. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTGlobals::sleep(5000);
    GTUtilsLog::check(os, l);
    GTUtilsTaskTreeView::cancelTask(os, "Execute workflow");
    GTGlobals::sleep();
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

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    GTKeyboardDriver::keyClick('f', Qt::ControlModifier );
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keySequence(">pattern1");
    GTKeyboardDriver::keyClick( Qt::Key_Enter, Qt::ControlModifier );
    GTKeyboardDriver::keySequence("TGGCAAGCT");

    GTUtilsOptionPanelSequenceView::openAnnotationParametersShowHideWidget(os);
    GTGlobals::sleep(1000);

    QLineEdit *annotationNameEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os,"leAnnotationName"));
    CHECK_SET_ERR(annotationNameEdit != NULL, "chbUsePatternNames not found!");
    annotationNameEdit->setText("pat");
    GTGlobals::sleep(1000);

    QCheckBox* check = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "chbUsePatternNames"));
    CHECK_SET_ERR(check != NULL, "chbUsePatternNames not found!");
    GTCheckBox::setChecked(os, check, 1);

    CHECK_SET_ERR(annotationNameEdit->isEnabled() != true, "annotationNameEdit is enabled!");

    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));
    GTGlobals::sleep(500);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTGlobals::sleep(1000);
    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "pattern1");
    QTreeWidgetItem *item2 = GTUtilsAnnotationsTreeView::findItem(os, "pat", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(item1 != NULL, "item1 not found!");
    CHECK_SET_ERR(item2 == NULL, "item2 found!");

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
}

GUI_TEST_CLASS_DEFINITION(test_4179) {
    //1. Open file "data/samples/Genabnk/sars.gb"
    //Current state: Two words are merged into a single one, in the file they are separated by a newline symbol.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem(os,"comment");
    CHECK_SET_ERR(item != NULL, "Can't find \"comment\" item");
    QString qualifier = GTUtilsAnnotationsTreeView::getQualifierValue(os,"1", item);
    CHECK_SET_ERR(qualifier.indexOf("The reference") > 0, "Expected string is not found");
}

GUI_TEST_CLASS_DEFINITION(test_4188_1) {
    //1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select area: from 16 to 26 col, from 6 to 16 seq
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(16, 6), QPoint(26, 16), GTGlobals::UseMouse);

    //3. Move left border to the left
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 3, -2);

    //Expected selection x: 14, y: 6, width: 13, height: 11
    QRect selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(14, 6, 13, 11),
                  QString("Unexpected selection. Expected x: 14, y: 6, width: 13, height: 11. Current x: %1, y: %2, width: %3, height: %4")
                  .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));

    //4. Move top border upper
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 0, -2);

    //Expected selection x: 14, y: 4, width: 13, height: 13
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(14, 4, 13, 13),
        QString("Unexpected selection. Expected x: 14, y: 4, width: 13, height: 13. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));

    //5. Move right border to the left
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 1, -3);

    //Expected selection x: 14, y: 4, width: 10, height: 13
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(14, 4, 10, 13),
        QString("Unexpected selection. Expected x: 14, y: 4, width: 13, height: 10. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));


    //5. Move bottom border upper
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 2, -3);

    //Expected selection x: 14, y: 4, width: 10, height: 10
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(14, 4, 10, 10),
        QString("Unexpected selection. Expected x: 14, y: 4, width: 10, height: 10. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));

    //6. Move top right corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 4, 1);

    //Expected selection x: 14, y: 3, width: 11, height: 11
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(14, 3, 11, 11),
        QString("Unexpected selection. Expected x: 14, y: 3, width: 11, height: 11. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));

    //7. Move bottom right corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 5, -4);

    //Expected selection x: 14, y: 3, width: 7, height: 7
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(14, 3, 7, 7),
        QString("Unexpected selection. Expected x: 14, y: 3, width: 7, height: 7. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));

    //8. Move bottom left corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 6, 2);

    //Expected selection x: 16, y: 3, width: 5, height: 5
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(16, 3, 5, 5),
        QString("Unexpected selection. Expected x: 16, y: 3, width: 5, height: 5. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));


    //9. Move top left corner
    GTUtilsMSAEditorSequenceArea::expandSelectedRegion(os, 7, 3);

    //Expected selection x: 19, y: 6, width: 2, height: 2
    selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selectedRect == QRect(19, 6, 2, 2),
        QString("Unexpected selection. Expected x: 19, y: 6, width: 2, height: 2. Current x: %1, y: %2, width: %3, height: %4")
        .arg(selectedRect.x()).arg(selectedRect.y()).arg(selectedRect.width()).arg(selectedRect.height()));
}

GUI_TEST_CLASS_DEFINITION(test_4188_2) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select region from 40000 to 60000
    GTUtilsSequenceView::selectSequenceRegion(os, 40000, 60000);

    //3. Drag and drop left border to the right in the Zoom View
    PanViewRenderArea* panViewRenderArea = GTUtilsSequenceView::getPanViewByNumber(os)->getRenderArea();
    CHECK_SET_ERR(panViewRenderArea != NULL, "PanViewRenderArea not found");

    const double panScale = panViewRenderArea->getCurrentScale();
    const QPoint startPan((int)(40000 * panScale), panViewRenderArea->height() / 2);
    const QPoint startPanGlobal = panViewRenderArea->mapToGlobal(startPan);
    const QPoint endPanGlobal(startPanGlobal.x() + panViewRenderArea->width() / 2, startPanGlobal.y());
    GTMouseDriver::dragAndDrop(startPanGlobal, endPanGlobal);

    //Expected: one region, start - 60000
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    U2Region sel = selection.first();
    CHECK_SET_ERR(sel.startPos == 60000, QString("Unexpected selected region start, expected: 60000, current: %1").arg(sel.startPos));

    //4. Select several regions
    GTUtilsSequenceView::selectSeveralRegionsByDialog(os, "40000..60000,80000..100000");

    //5. Join regions by moving the right border of the left region to the right in the Zoom View
    const QPoint startMultiplePan((int)(60000 * panScale), panViewRenderArea->height() / 2);
    const QPoint startMultiplePanGlobal = panViewRenderArea->mapToGlobal(startMultiplePan);
    const QPoint endMultiplePanGlobal(startMultiplePanGlobal.x() + (int)(30000 * panScale), startMultiplePanGlobal.y());
    GTMouseDriver::dragAndDrop(startMultiplePanGlobal, endMultiplePanGlobal);

    //Expected: one region, start - 39999, end - 1000000
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    sel = selection.first();
    CHECK_SET_ERR(sel == U2Region(39999, 60001), QString("Unexpected selected region. Expected start: 39999, end: 100000. Current start: %1, end: %2").arg(sel.startPos).arg(sel.endPos()));
}

GUI_TEST_CLASS_DEFINITION(test_4188_3) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Turn off wrap mode
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    GTWidget::click(os, GTAction::button(os, wrapMode));

    //3. Select region from 10 to 20
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);

    //4. Drag and drop left border to the right in the Details View
    DetViewRenderArea* detViewRenderArea = GTUtilsSequenceView::getDetViewByNumber(os)->getDetViewRenderArea();
    CHECK_SET_ERR(detViewRenderArea != NULL, "DetViewRenderArea not found");

    const double detScale = detViewRenderArea->getCurrentScale();
    const QPoint startDet((int)(9 * detScale), detViewRenderArea->height() / 2);
    const QPoint startDetGlobal = detViewRenderArea->mapToGlobal(startDet);
    const QPoint endDetGlobal(startDetGlobal.x() + detViewRenderArea->width() / 2, startDetGlobal.y());

    GTMouseDriver::dragAndDrop(startDetGlobal, endDetGlobal);

    //Expected: one region, start - 20
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Unexpected selected regions quantity, expected 1, current %1").arg(selection.size()));
    U2Region sel = selection.first();
    CHECK_SET_ERR(sel.startPos == 20, QString("Unexpected selected region start, expected: 20, current: %1").arg(sel.startPos));

    //5. Select several regions
    GTUtilsSequenceView::selectSeveralRegionsByDialog(os, "10..20,30..40");

    //6. Join regions by moving the right border of the left region to the right in the Zoom View
    const QPoint startMultipleDet((int)(20 * detScale), detViewRenderArea->height() / 2);
    const QPoint startMultipleDetGlobal = detViewRenderArea->mapToGlobal(startMultipleDet);
    const QPoint endMultipleDetGlobal(startMultipleDetGlobal.x() + (int)(15 * detScale), startMultipleDetGlobal.y());
    GTMouseDriver::dragAndDrop(startMultipleDetGlobal, endMultipleDetGlobal);

    //Expected: one region, start - 9, end - 40
    selection = GTUtilsSequenceView::getSelection(os);
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

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    WorkflowProcessItem* filter = GTUtilsWorkflowDesigner::addElement(os, "Filter Annotations by Name");
    CHECK_SET_ERR( filter != NULL, "Failed to add an element Filter annotations by name");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os,GTAction::button(os, "Validate workflow"));
    GTGlobals::sleep();
    int countErrors = GTUtilsWorkflowDesigner::checkErrorList(os, "At least one of these parameters must be set");
    CHECK_SET_ERR(countErrors == 1, QString("Errors count dont match, should be 1 validation error, but %1 errors now").arg(countErrors));
}

GUI_TEST_CLASS_DEFINITION(test_4209) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4209/", "crash.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::click(os, "Align to Reference");
    GTUtilsWorkflowDesigner::setParameter(os, "Reference URL",testDir + "_common_data/scenarios/_regression/4209/seq1.gb", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::addInputFile(os, "Read Sequence", testDir + "_common_data/reads/e_coli_10000snp.fa");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4218) {
    // Check that "Write Annotations" worker takes into account object names of incoming annotation tables
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/regression/4218/test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::addInputFile(os, "Read Annotations",
        testDir + "_common_data/bedtools/introns.bed");
    GTUtilsWorkflowDesigner::click(os, "Write Annotations");
    const QString outputFilePath = QDir(sandBoxDir).absolutePath() +"/out.bed";
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, sandBoxDir + "out.bed");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::checkItem(os, "chr2 features");
}

GUI_TEST_CLASS_DEFINITION(test_4218_1) {
    // Check that an output annotation object has a default name if incoming annotation objects have different names
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, testDir + "_common_data/regression/4218/test.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::addInputFile(os, "Read Annotations", testDir + "_common_data/regression/4218/test.bed");
    GTUtilsWorkflowDesigner::click(os, "Write Annotations");
    const QString outputFilePath = QDir(sandBoxDir).absolutePath() + "/out.bed";
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, sandBoxDir + "out.bed");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::checkItem(os, "Annotations features");
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
    GTLogTracer logTracer;

    AlignShortReadsFiller::BwaParameters parameters(testDir + "_common_data/reads/eas.fa", testDir + "_common_data/reads/eas.fastq");
    parameters.useDefaultResultPath = false;
    parameters.resultDir = sandBoxDir;
    parameters.resultFileName = "test_4221.sam";
    parameters.indexAlgorithm = AlignShortReadsFiller::BwaParameters::Is;
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_4221.ugenedb"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "test_4221.ugenedb");

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_4232) {
    // 1. Open a file with variations
    GTFileDialog::openFile(os, testDir + "_common_data/vcf/valid.vcf");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Open a file with a sequence
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/illumina.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 3. Open a file with an assembly
    GTFileDialog::openFile(os, testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 4. Drag&drop the sequence object from the project view on the assembly view
    const QModelIndex sequenceDocIndex = GTUtilsProjectTreeView::findIndex(os, "illumina.fa");
    const QModelIndex sequenceObjIndex = sequenceDocIndex.child(0, 0);

    GTUtilsNotifications::waitForNotification(os, true, "It seems that sequence");
    GTUtilsProjectTreeView::dragAndDrop(os, sequenceObjIndex, GTWidget::findWidget(os, "assembly_reads_area"));

    // Expected state: sequence object and document are highlighted in the Project view
    QFont itemFont = GTUtilsProjectTreeView::getFont(os, sequenceObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Sequence object item isn't highlighted in Project view");
    itemFont = GTUtilsProjectTreeView::getFont(os, sequenceDocIndex);
    CHECK_SET_ERR(itemFont.bold(), "Sequence document item isn't highlighted in Project view");

    // 4. Drag&drop the variations object from the project view on the assembly view
    const QModelIndex variationsObjIndex = GTUtilsProjectTreeView::findIndex(os, "II");
    GTUtilsProjectTreeView::dragAndDrop(os, variationsObjIndex, GTWidget::findWidget(os, "assembly_reads_area"));

    // Expected state: variations object is highlighted in the Project view
    itemFont = GTUtilsProjectTreeView::getFont(os, variationsObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Variations object item isn't highlighted in Project view");

    // 5. Remove the variations from the assembly view
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Remove track from the view"));
    GTWidget::click(os, GTWidget::findWidget(os, "AssemblyVariantRow_II"), Qt::RightButton);

    // Expected state: the variations object isn't highlighted in the Project view
    itemFont = GTUtilsProjectTreeView::getFont(os, variationsObjIndex);
    CHECK_SET_ERR(!itemFont.bold(), "Variations object item is unexpectedly highlighted in Project view");

    // 6. Remove the sequence from the assembly view
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Unassociate"));
    GTWidget::click(os, GTWidget::findWidget(os, "Assembly reference sequence area"), Qt::RightButton);

    // Expected state: the sequence object and document aren't highlighted in the Project view
    itemFont = GTUtilsProjectTreeView::getFont(os, sequenceObjIndex);
    CHECK_SET_ERR(!itemFont.bold(), "Sequence object item is unexpectedly highlighted in Project view");
    itemFont = GTUtilsProjectTreeView::getFont(os, sequenceDocIndex);
    CHECK_SET_ERR(!itemFont.bold(), "Sequence document item is unexpectedly highlighted in Project view");
}

GUI_TEST_CLASS_DEFINITION(test_4232_1) {
    // 1. Open file with a chromatogram
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: a chromatogram object and a sequence object are highlighted in the Project view
    const QModelIndex seqObjIndex = GTUtilsProjectTreeView::findIndex(os, "A1#berezikov");
    QFont itemFont = GTUtilsProjectTreeView::getFont(os, seqObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Sequence object item isn't highlighted in Project view");

    const QModelIndex chromatogramObjIndex = GTUtilsProjectTreeView::findIndex(os, "Chromatogram");
    itemFont = GTUtilsProjectTreeView::getFont(os, chromatogramObjIndex);
    CHECK_SET_ERR(itemFont.bold(), "Chromatogram object item isn't highlighted in Project view");
}

GUI_TEST_CLASS_DEFINITION(test_4244){
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QString s;
            for(int i = 0; i<32000; i++){
                s.append("a");
            }
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            QLineEdit* leDescription = GTWidget::findExactWidget<QLineEdit*>(os, "leDescription", dialog);
            GTWidget::click(os, leDescription);
            GTClipboard::setText(os, s);
            GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);
            GTGlobals::sleep();

            QLineEdit* leAnnotationName = GTWidget::findExactWidget<QLineEdit*>(os, "leAnnotationName", dialog);
            GTLineEdit::setText(os, leAnnotationName, "name");
            QLineEdit* leRegionStart = GTWidget::findExactWidget<QLineEdit*>(os, "leRegionStart", dialog);
            GTLineEdit::setText(os, leRegionStart, "10");
            QLineEdit* leRegionEnd = GTWidget::findExactWidget<QLineEdit*>(os, "leRegionEnd", dialog);
            GTLineEdit::setText(os, leRegionEnd, "20");


            uiLog.trace(QString("easy to find %1").arg(leDescription->text().length()));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario()));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep(20000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "test_4244", ExportAnnotationsFiller::gff, os));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "name"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "test_4244.gb", ExportAnnotationsFiller::genbank, os));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "name"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTFileDialog::openFile(os, sandBoxDir + "test_4244.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList()<<"action_project__unload_selected_action",
                                                      PopupChecker::IsEnabled));
    GTUtilsProjectTreeView::click(os, "test_4244.gb", Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4266) {
//    1. Create the workflow: Read Seqeunce --> Write Sequence
//    2. Input data: "_common_data/fasta/Gene.fa"
//    3. Run the workflow
//    Expected state: there is a warning about empty sequences on the dashboard

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement(os, "Read Sequence", true);
    CHECK_SET_ERR( read != NULL, "Failed to add an element");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/Gene.fa");

    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement(os, "Write Sequence", true);
    CHECK_SET_ERR( write != NULL, "Failed to add an element");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QDir(sandBoxDir).absolutePath() + "/test_4266.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(os, read, write);
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDashboard::findElement(os, "Loaded sequences: 139.");
}

GUI_TEST_CLASS_DEFINITION(test_4272){
//    Open any sequence
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* mainTb = GTWidget::findWidget(os, "mwtoolbar_activemdi");
    QWidget* qt_toolbar_ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", mainTb, GTGlobals::FindOptions(false));
    if(qt_toolbar_ext_button != NULL && qt_toolbar_ext_button->isVisible()){
        GTWidget::click(os, qt_toolbar_ext_button);
    }
//    Turn on auto-annotations (e.g. find ORFs)
     GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
     GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
     GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: annotations are displayed in zoom, details and annotations views
     GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 837)");
//    Turn auto-annotations OFF
     GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
     GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
     GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: no annotations are displayed
     QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 837)", GTGlobals::FindOptions(false));
     CHECK_SET_ERR(item == NULL, "orfs are unexpectidly shown");
}

GUI_TEST_CLASS_DEFINITION(test_4276) {
    //1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Use context menu
    //{Add->Sequence from file...}
    //Expected: "Open file with sequences" dialog appeared
    //3. Select file "_common_data/fasta/PF07724_full_family.fa"
    //4. Press "Open"
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/PF07724_full_family.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);

    //5. Delete the alignment object
    GTUtilsProjectTreeView::click(os, "COI");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    bool executed = GTUtilsTaskTreeView::checkTask(os, "Add sequences to alignment task");
    CHECK_SET_ERR(false == executed, "The task is not cancelled");
    //Current state: UGENE crashes
}

GUI_TEST_CLASS_DEFINITION(test_4284){
//    1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Select a sequence that is two sequences above the last visible sequence in the name list area.
    MSAEditorSequenceArea *msaEdistorSequenceArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
    const int endPos = msaEdistorSequenceArea->getEditor()->getUI()->getScrollController()->getLastVisibleRowNumber(msaEdistorSequenceArea->height());

    GTUtilsMsaEditor::clickSequence(os, endPos - 1);
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, endPos - 1, 1234, 1));

//    3. Press and hold "shift" key.
//    4. Click "down" key once.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

//    Expected state: two sequences are selected, the msa is not scrolled down.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, endPos - 1, 1234, 2));

//    4. Click "down" key again.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

//    Expected state: three sequences are selected, the msa is scrolled down for one line.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, endPos - 1, 1234, 3));

//    5. Click "down" key again.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

//    Expected state: four sequences are selected, the msa is scrolled down for two lines.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, endPos - 1, 1234, 4));

    const int firstVisibleSequence = msaEdistorSequenceArea->getEditor()->getUI()->getScrollController()->getFirstVisibleRowNumber(false);
    CHECK_SET_ERR(firstVisibleSequence == 2, QString("MSA scrolled incorrectly: expected first fully visible sequence %1, got %2").arg(2).arg(firstVisibleSequence));
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
    //clean up
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::removeCmdlineWorkerFromPalette(os, "test_4295");

    // start test
    GTLogTracer logTracer;

    GTUtilsWorkflowDesigner::addElement(os, "Read File URL(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTGlobals::sleep(200);

    GTUtilsWorkflowDesigner::addElement(os, "Write Plain Text");
    GTGlobals::sleep(200);

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4295", "test_4295.etc");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton* button = GTAction::button(os, "AddElementWithCommandLineTool");
    GTWidget::click(os, button);
    GTGlobals::sleep(200);
    WorkflowProcessItem *cmdlineWorker = GTUtilsWorkflowDesigner::getWorker(os, "test_4295");

    GTUtilsWorkflowDesigner::connect(os, GTUtilsWorkflowDesigner::getWorker(os, "Read File URL(s)"), cmdlineWorker);
    GTGlobals::sleep(200);
    GTUtilsWorkflowDesigner::connect(os, cmdlineWorker, GTUtilsWorkflowDesigner::getWorker(os, "Write Plain Text"));
    GTGlobals::sleep(200);
    GTUtilsWorkflowDesigner::click(os, "test_4295");
    GTGlobals::sleep(200);
    QTableWidget* table = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
    GTUtilsWorkflowDesigner::setTableValue(os, "Plain text", "Source URL (by Read File URL(s))", GTUtilsWorkflowDesigner::comboValue, table);
    GTUtilsWorkflowDesigner::click(os, "test_4295");

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os) : Filler(_os, "CreateExternalProcessWorkerDialog"){}
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            QLineEdit *ed = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "templateLineEdit", w));
            GTLineEdit::setText(os, ed, "echo $in > $out");

            QAbstractButton *button = GTWidget::findButtonByText(os, "Finish");
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        }
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "editConfiguration"));
    GTUtilsWorkflowDesigner::click(os, "test_4295",QPoint(0,0),Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, logTracer);

}

GUI_TEST_CLASS_DEFINITION(test_4302_1) {
    //1. Open samples/Genbank/sars.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Select any region
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 4));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "select_range_action"));
    //3. Open main menu "Actions"
    //Expected state: "Replace subsequence" menu item enabled
    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "ACCCT"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Replace subsequence...");
}

GUI_TEST_CLASS_DEFINITION(test_4302_2) {
    //1. Open samples/Genbank/sars.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Select any annotation
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click();
    //3. Open main menu "Actions"
    //Expected state: menu item {Remove->Selected annotation and qualifiers} are enabled
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Remove" << "Selected annotations and qualifiers");
}

GUI_TEST_CLASS_DEFINITION(test_4306_1) {
//    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Build a tree with default parameters.
//    Expected state: a tree view is displayed with the alignment editor.
    QDir().mkpath(sandBoxDir + "test_4306");

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4306/test_4306.nwk", 0, 0, true));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Use context menu on tree view.
//    Expected state: there are "Zoom in", "Zoom out" and "Reset zooming" actions in the menu.
    //    Expected state: there are "Zoom in", "Zoom out" and "Reset zooming" actions in the menu.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Zoom In", PopupChecker::IsEnabled));
    GTWidget::click(os, GTUtilsMsaEditor::getTreeView(os), Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Zoom Out", PopupChecker::IsEnabled));
    GTWidget::click(os, GTUtilsMsaEditor::getTreeView(os), Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Reset Zooming", PopupChecker::IsEnabled));
    GTWidget::click(os, GTUtilsMsaEditor::getTreeView(os), Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4306_2) {
//    1. Open "data/samples/Newick/COI.nwk".
    GTFileDialog::openFile(os, dataDir + "samples/Newick/COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Use context menu on the tree view.
//    Expected state: there are "Zoom in", "Zoom out" and "Reset zooming" actions in the menu.
    QStringList items = QStringList() << "Zoom In"
                                      << "Zoom Out"
                                      << "Reset Zooming";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList(), items));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4308) {
//    1. Open "_common_data/clustal/10000_sequences.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/PF07724_full_family.fa", GTFileDialog::Open, GTGlobals::UseMouse);
    GTUtilsTask::waitTaskStart(os, "Loading documents");

//    2. Remove the document while the file is opening.
    const bool itemExistsBefore = GTUtilsProjectTreeView::checkItem(os, "PF07724_full_family.fa");
    CHECK_SET_ERR(itemExistsBefore, "A loading item not found");

    GTUtilsNotifications::waitForNotification(os, true, "Subtask {Load 'PF07724_full_family.fa'} is canceled Document was removed");
    GTUtilsDocument::removeDocument(os, "PF07724_full_family.fa");
    GTGlobals::sleep(500);

//    Expected state: the document is removed from the project, the loading task is canceled, a notification about the canceled task appears.
    const bool itemExists = GTUtilsProjectTreeView::checkItem(os, "PF07724_full_family.fa");
    CHECK_SET_ERR(!itemExists, "The document is not removed from the project");
    GTUtilsTask::checkNoTask(os, "Loading documents");
}

GUI_TEST_CLASS_DEFINITION(test_4309) {
    // 1. Open sars.gb
    // 2. Context menu on annotations object in project view: {Export/Import --> Export annotations}
    // Expected state: export annotataions dialog appeared
    // 3. Check format combobox
    // Expected state: Vector NTI format is abcent

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    class VectorNTIFormatChecker : public Filler {
    public:
        VectorNTIFormatChecker(HI::GUITestOpStatus &os)
            : Filler(os, "U2__ExportAnnotationsDialog") {}
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");

            QComboBox *comboBox = dialog->findChild<QComboBox*>();
            CHECK_SET_ERR(comboBox != NULL, "ComboBox not found");

            QStringList formats = GTComboBox::getValues(os, comboBox);
            CHECK_SET_ERR(! formats.contains("Vector NTI sequence"), "VectorNTI format is present in annotations export dialog");

            QDialogButtonBox* buttonBox = dialog->findChild<QDialogButtonBox*>("buttonBox");
            CHECK_SET_ERR(buttonBox != NULL, "buttonBox is NULL");

            QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != NULL, "cancelButton is NULL");
            GTWidget::click(os, cancelButton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new VectorNTIFormatChecker(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << "ep_exportAnnotations2CSV"));
    GTUtilsProjectTreeView::click(os, "NC_004718 features", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4309_1) {
    // 1. Open WD
    // 2. Add Write Annotations element
    // Expected state: Vector NTI format is not available in Format parameter combobox

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write annotations");

    QTableView* table = qobject_cast<QTableView*>(GTWidget::findWidget(os,"table"));
    CHECK_SET_ERR(table,"tableView not found");

    QAbstractItemModel* model = table->model();
    int iMax = model->rowCount();
    int row = -1;
    for(int i = 0; i<iMax; i++){
        QString s = model->data(model->index(i,0)).toString();
        if (s.compare("Document format", Qt::CaseInsensitive) == 0){
            row = i;
            break;
        }
    }
    CHECK_SET_ERR(row != -1, QString("Document format parameter not found"));
    table->scrollTo(model->index(row,1));

    GTMouseDriver::moveTo(GTTableView::getCellPosition(os,table,1,row));
    GTMouseDriver::click();
    GTGlobals::sleep(500);

    QComboBox* box = qobject_cast<QComboBox*>(table->findChild<QComboBox*>());
    CHECK_SET_ERR(box, "QComboBox not found. Widget in this cell might be not QComboBox");
    QString vectorNtiFormatName = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::VECTOR_NTI_SEQUENCE)->getFormatName();
    QString genbankFormatName = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK)->getFormatName();
    QStringList boxData = GTComboBox::getValues(os, box);
    CHECK_SET_ERR(boxData.contains(vectorNtiFormatName) == false, "Vector NTI format is present in WriteAnnotations worker");
    CHECK_SET_ERR(boxData.contains(genbankFormatName), "GenBank format isn't present in WriteAnnotations worker");
}

GUI_TEST_CLASS_DEFINITION(test_4323_1) {
    GTLogTracer logTracer;

//    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Click "Align sequence to this alignment" and select "_common_data/database.ini".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/database.ini"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

//    Expected state: load task fails, safe point doesn't trigger.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, logTracer, "Task {Load sequences and add to alignment task} finished with error: There are no sequences to align in the document(s)");
}

GUI_TEST_CLASS_DEFINITION(test_4323_2) {
//    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Click "Align sequence to this alignment" button on the toolbar, select "samples/PDB/1CF7.pdb".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/PDB/1CF7.PDB"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: four sequences are added to the alignment.
    const int count = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(22 == count, QString("Unexpected sequences count: expect %1, got %2")
                  .arg(22).arg(count));

//    3. Open "Pairwise alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

//    Expected state: there is a message that the msa alphabet is no applicable.
    QLabel *errorLabel = GTWidget::findExactWidget<QLabel *>(os, "lblMessage");
    CHECK_SET_ERR(NULL != errorLabel, "Error label is NULL");
    CHECK_SET_ERR(errorLabel->isVisible(), "Error label is invisible");
    CHECK_SET_ERR(errorLabel->text().contains("Pairwise alignment is not available for alignments with \"Raw\" alphabet."),
                  QString("An unexpected error message: '%1'").arg(errorLabel->text()));
}

GUI_TEST_CLASS_DEFINITION(test_4323_3) {
//    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Click "Align sequence to this alignment" button on the toolbar, select "samples/PDB/1CF7.pdb".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/PDB/1CF7.PDB"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: four sequences are added to the alignment.
    const int count = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(22 == count, QString("Unexpected sequences count: expect %1, got %2")
                  .arg(22).arg(count));
}

GUI_TEST_CLASS_DEFINITION(test_4323_4) {
//    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Rename the first two sequences to "1".
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "Phaneroptera_falcata", "1");
    GTUtilsMSAEditorSequenceArea::renameSequence(os, "Isophya_altaica_EF540820", "1");

//    3. Click "Align sequence to this alignment" button on the toolbar, select "samples/FASTQ/eas.fastq".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTQ/eas.fastq"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    4. Do it again.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTQ/eas.fastq"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there are 24 sequences in the alignment; two of them are named "1", two - "EAS54_6_R1_2_1_413_324", two - "EAS54_6_R1_2_1_540_792", two - "EAS54_6_R1_2_1_443_348".
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(24 == names.count(), QString("Unexpected sequences count: expect %1, got %2")
                  .arg(24).arg(names.count()));
    CHECK_SET_ERR(2 == names.count("1"), QString("Unexpected sequences with name '1' count: expect %1, got %2")
                  .arg(2).arg(names.count("1")));
    CHECK_SET_ERR(2 == names.count("EAS54_6_R1_2_1_413_324"), QString("Unexpected sequences with name 'EAS54_6_R1_2_1_413_324' count: expect %1, got %2")
                  .arg(2).arg(names.count("EAS54_6_R1_2_1_413_324")));
    CHECK_SET_ERR(2 == names.count("EAS54_6_R1_2_1_540_792"), QString("Unexpected sequences with name 'EAS54_6_R1_2_1_540_792' count: expect %1, got %2")
                  .arg(2).arg(names.count("EAS54_6_R1_2_1_540_792")));
    CHECK_SET_ERR(2 == names.count("EAS54_6_R1_2_1_443_348"), QString("Unexpected sequences with name 'EAS54_6_R1_2_1_443_348' count: expect %1, got %2")
                  .arg(2).arg(names.count("EAS54_6_R1_2_1_443_348")));
}

GUI_TEST_CLASS_DEFINITION(test_4325) {
    // 1. Open In Silico PCR sample
    // 2. Seqeunce: "data/samples/Genbank/CVU55762.gb"
    //    Primers: "test/_common_data/cmdline/pcr/primers_CVU55762.fa"
    // 3. Run workflow
    // Expected state: the size of the product is 150, and the primers are annotated on both sides

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "In Silico PCR");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "/samples/Genbank/CVU55762.gb");

    GTUtilsWorkflowDesigner::click(os, "In Silico PCR");
    GTUtilsWorkflowDesigner::setParameter(os, "Primers URL", QDir(testDir).absolutePath() + "/_common_data/cmdline/pcr/primers_CVU55762.fa", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click(os, "Write Sequence");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QDir(sandBoxDir).absolutePath() + "/test_4325.gb", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, sandBoxDir, "test_4325.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsSequenceView::getLengthOfSequence(os) == 150, "Product size is incorrect");
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegions(os);
    CHECK_SET_ERR(regions.size() == 2, "Incorrect primers number");
    CHECK_SET_ERR(regions.contains(U2Region(0, 40)), "There is no (1, 40) annotated primer region");
    CHECK_SET_ERR(regions.contains(U2Region(110, 40)), "There is no (111, 150) annotated primer region");
}

GUI_TEST_CLASS_DEFINITION(test_4334) {
    //1. Open "_common_data/fasta/empty.fa" as msa.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer lt;
    //    2. Add human_t1.fa sequence throu context menu {Add->Sequence from file}
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsDialog::waitForDialog(os, ob);
    QAbstractButton *align = GTAction::button(os, "Align sequence to this alignment");
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence to this alignment\" action not found");
    GTWidget::click(os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasError(), "log should not contain errors");
}

GUI_TEST_CLASS_DEFINITION(test_4345) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Close active view", GTGlobals::UseKey);
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir+"samples/Genbank/", "PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4352) {
    //1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Toggle the Circular View for the sequence.
    GTUtilsCv::commonCvBtn::click(os);

    //3. Find some restriction sites.
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "AaaI"));
    GTWidget::click(os, GTWidget::findWidget(os, "Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Select any restriction site in the "Restriction Sites Map" widget.
    QTreeWidget *tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget(os, "restrictionMapTreeWidget"));
    QTreeWidgetItem *item = GTTreeWidget::findItem(os, tree, "89345..89350");
    GTTreeWidget::click(os, item);

    //5. Remove a part of the sequence that contains the selected site.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Remove subsequence..."));
    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "89300..89400"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    //6. Wait while restriction sites recalculates.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //7. Navigate to any restriction site in the restriction site map.
    item = GTTreeWidget::findItem(os, tree, "89231..89236");
    GTTreeWidget::click(os, item);
    //Expected state: UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_4356) {
    
    class Test_4356 : public Filler {
    public:
        Test_4356(HI::GUITestOpStatus& os)
            : Filler(os, "DotPlotDialog") {}
        virtual void run() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Genbank", "murine.gb"));
            GTWidget::click(os, dialog->findChild<QPushButton*>("loadSequenceButton"));
            GTGlobals::sleep();

            QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
            QPushButton* button =  box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(button !=NULL, "Ok button is NULL");
            GTWidget::click(os, button);

        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    
    GTUtilsDocument::unloadDocument(os, "murine.gb", true);
    
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new Test_4356(os));
    GTWidget::click(os, GTWidget::findWidget(os, "build_dotplot_action_widget"));
    GTGlobals::sleep(1000);
    
    Document *doc = GTUtilsDocument::getDocument(os, "murine.gb");
    CHECK_SET_ERR(NULL != doc, "Document is NULL");
    CHECK_SET_ERR(doc->isLoaded(), "Document is unexpectedly unloaded");
}


GUI_TEST_CLASS_DEFINITION(test_4359) {
/* 1. Open human_T1
 * 2. Open Primer3 dialog
 *   Expected state: the "Pick primers" button should be in focus
 */
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class EscClicker : public Filler {
    public:
        EscClicker(HI::GUITestOpStatus& _os) : Filler(_os, "Primer3Dialog"){}
        virtual void run() {
            GTGlobals::sleep();
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            QPushButton* button = qobject_cast<QPushButton*>(GTWidget::findWidget(os, "pickPrimersButton"));
            CHECK(NULL != button, );
            CHECK_SET_ERR(button->isDefault(), "Pick primers button doesn't default");

            GTKeyboardDriver::keyClick( Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new EscClicker(os));
    GTWidget::click(os, GTWidget::findWidget(os, "primer3_action_widget"));
}
GUI_TEST_CLASS_DEFINITION(test_4368) {
/* 1. Open "data/samples/CLUSTALW/COI.aln".
 * 2. Open "Statistics" options panel tab.
 * 3. Set any reference sequence.
 * 4. Set "Show distances column" option.
 */
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_STATISTICS_WIDGET"));

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 5));
    GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));

    QCheckBox* showDistancesColumnCheck = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "showDistancesColumnCheck"));
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    GTGlobals::sleep();

    QGroupBox* groupBox = qobject_cast<QGroupBox*>(GTWidget::findWidget(os,"profileGroup"));
    CHECK_SET_ERR(groupBox != NULL, "groupBox not found!");

    QRadioButton* radio = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "percentsButton", groupBox));
    CHECK_SET_ERR(radio != NULL, "percentsButton not found!");
    GTRadioButton::click(os, radio);
    QLabel* nameLabel = qobject_cast<QLabel*>(GTWidget::findWidget(os, "Distance column name"));
    CHECK_SET_ERR(nameLabel != NULL, "percentsButton not found!");
    CHECK_SET_ERR(nameLabel->text() == "%", "percentsButton not found!");
    GTGlobals::sleep();

    QRadioButton* radio2 = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "countsButton", groupBox));
    CHECK_SET_ERR(radio2 != NULL, "countsButton not found!");
    GTRadioButton::click(os, radio2);
    CHECK_SET_ERR(nameLabel->text() == "score", "percentsButton not found!");
}

GUI_TEST_CLASS_DEFINITION(test_4373) {
    // 1. Open any sequence
    // 2. Open CV
    // 3. Resize CV to make some buttons hidden
    // Expected state: there is a button with an arrows at the bottom of toolbar and it shows the hidden action

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsCv::commonCvBtn::click(os);

    QWidget* splitterHandler = GTWidget::findWidget(os, "qt_splithandle_annotated_DNA_scrollarea");
    CHECK_SET_ERR(splitterHandler != NULL, "SplitterHandle not found");
    GTWidget::click(os, splitterHandler);

    QWidget* mainToolBar = GTWidget::findWidget(os, "mwtoolbar_activemdi");
    CHECK_SET_ERR(mainToolBar != NULL, "mwtoolbar_activemdi not found");

    QPoint point = mainToolBar->geometry().bottomLeft();
    point = mainToolBar->mapToGlobal(point);
    QWidget* cv = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    CHECK_SET_ERR(cv != NULL, "Cannot find CV_ADV_single_sequence_widget_0");
    point.setY( cv->mapToGlobal( cv->geometry().topLeft()).y() + 100 );
    GTGlobals::sleep();

    GTMouseDriver::press();
    GTMouseDriver::moveTo(point);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTGlobals::sleep();
#ifdef Q_OS_LINUX
    GTGlobals::sleep(15000);//splitter is moved slowly on some versions of linux
#endif
    QWidget* toolBar = GTWidget::findWidget(os, "circular_view_local_toolbar");
    CHECK_SET_ERR(toolBar != NULL, "Cannot find circular_view_local_toolbar");

    QWidget* extButton = GTWidget::findWidget(os, "qt_toolbar_ext_button", toolBar);
    CHECK_SET_ERR(extButton != NULL, "Cannot find qt_toolbar_ext_button");
    CHECK_SET_ERR(extButton->isVisible() && extButton->isEnabled(), "qt_toolbar_ext_button is not visible and disabled");

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Show/hide restriction sites map"));
    GTWidget::click(os, extButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4377) {
    // 1. Open "_common_data/fasta/Gene.fa".
    // 2. Choose the separate reading mode.
    // Expected: the warning notification is shown.
    // 3. Click the notification.
    // Expected: the report window is opened. There is the list of sequences with empty names.

    GTUtilsNotifications::waitForNotification(os, false);
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/Gene.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);


    GTGlobals::sleep();
    QTextEdit *textEdit = dynamic_cast<QTextEdit*>(GTWidget::findWidget(os, "reportTextEdit", GTUtilsMdi::activeWindow(os)));
    CHECK_SET_ERR(textEdit->toPlainText().contains("The following sequences are empty:"), "Expected message is not found in the report text");
}

GUI_TEST_CLASS_DEFINITION(test_4383) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMSAEditorSequenceArea::scrollToPosition(os, QPoint(603, 1));

    QWidget* activeWindow = GTUtilsMdi::activeWindow(os);
    MSAEditorSequenceArea *msaEditArea = qobject_cast<MSAEditorSequenceArea*>(GTWidget::findWidget(os, "msa_editor_sequence_area", activeWindow));
    CHECK_SET_ERR(msaEditArea != NULL, "MsaEditorSequenceArea not found");
    QWidget *msaOffsetRight = GTWidget::findWidget(os, "msa_editor_offsets_view_widget_right", activeWindow);
    CHECK_SET_ERR(msaOffsetRight != NULL, "MsaOffset Left not found");

    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 2, 7)));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 52, 50)));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTKeyboardDriver::keyClick( Qt::Key_Space);
    GTThread::waitForMainThread();

    GTGlobals::sleep();
    activeWindow = GTUtilsMdi::activeWindow(os);
    msaOffsetRight = GTWidget::findWidget(os, "msa_editor_offsets_view_widget_right", activeWindow);
    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 2, 77)));
    GTMouseDriver::press();
    GTGlobals::sleep();
    GTMouseDriver::moveTo(msaOffsetRight->mapToGlobal(QPoint(msaOffsetRight->rect().left() - 52, 120)));
    GTMouseDriver::release();
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick( Qt::Key_Space);
}

GUI_TEST_CLASS_DEFINITION(test_4386_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Change this alignment by adding random gaps.
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(5, 5));
    GTKeyboardDriver::keyClick( ' ');

    //    3. Align with Muscle (or other algorithm).
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align" << "Align with MUSCLE..."));
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    4. Select some sequences in project view and click "Align sequence to this alignment".
    GTUtilsProject::openMultiSequenceFileAsSequences(os, dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsMdi::activateWindow(os, "COI [m] COI");

    GTUtilsProjectTreeView::click(os, "EAS54_6_R1_2_1_413_324");
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: sequences are aligned to alignment.
    const int rowsCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(19 == rowsCount, QString("Unexpected rows count: expect %1, got %2").arg(19).arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4386_2) {
//    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Rename the alignment, a new name should contain spaces.
    GTUtilsProjectTreeView::rename(os, "COI", "C O I");

//    3. Click "Align sequence to this alignment" and select any file with sequence.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTQ/eas.fastq"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: sequence is aligned to alignment.
    const int rowsCount = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(21 == rowsCount, QString("Unexpected rows count: expect %1, got %2").arg(19).arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4391) {
    GTLogTracer logTracer;

//    1. Create workflow { Read File URL(s) -> Cut Adapter }.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    WorkflowProcessItem *fileList = GTUtilsWorkflowDesigner::addElement(os, "Read File URL(s)");
    WorkflowProcessItem *cutAdapter = GTUtilsWorkflowDesigner::addElement(os, "Cut Adapter");
    GTUtilsWorkflowDesigner::connect(os, fileList, cutAdapter);

//    2. Set "_common_data/fastq/illumina.fastq" as input.
    GTUtilsWorkflowDesigner::addInputFile(os, "Read File URL(s)", testDir + "_common_data/fastq/illumina.fastq");

//    3. Run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: there are no errors neither in log nor in dashboard.
    const bool prolemsExist = GTUtilsDashboard::areThereProblems(os);
    CHECK_SET_ERR(!prolemsExist, "There are problems on the dashboard");
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_4400) {
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/VectorNTI_CAN_READ.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* commentItem = GTUtilsAnnotationsTreeView::findItem(os, "comment");
    QString qualValue = GTUtilsAnnotationsTreeView::getQualifierValue(os, "Original database", commentItem);
    CHECK_SET_ERR( qualValue == "GenBank", "ORIGDB comment was parced incorreclty");
}

GUI_TEST_CLASS_DEFINITION(test_4434) {
    GTLogTracer l;
    //1. Open murine.gb and human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Drag the annotations from murine to human_t1
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, "NC_001363 features"), GTUtilsAnnotationsTreeView::getTreeWidget(os));
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findFirstAnnotation(os) != NULL, "Annotations are connected to human_T1.fa");

    //3. Save the project
    GTUtilsProject::saveProjectAs(os, sandBoxDir + "test_4434.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Close the project.
    GTUtilsProject::closeProject(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Open the saved project.
    GTFileDialog::openFile(os, sandBoxDir + "test_4434.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //6. Current state : human_t1 is loaded with annotations
    GTUtilsProjectTreeView::doubleClickItem(os, "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findFirstAnnotation(os) != NULL, "Annotations are connected to human_T1.fa");
}

GUI_TEST_CLASS_DEFINITION(test_4439) {
//    1. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Call context menu on "NC_004718 features [sars.gb]" item in the Annotations tree view, select "Find qualifier..." menu item.

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    Expected state: a "Find Qualifier" dialog appears, search buttons are disabled.
            QDialogButtonBox *buttonBox = GTWidget::findExactWidget<QDialogButtonBox *>(os, "buttonBox", dialog);
            CHECK_SET_ERR(NULL != buttonBox, "buttonBox is NULL");
            QPushButton *nextButton = buttonBox->button(QDialogButtonBox::Ok);
            QPushButton *allButton = buttonBox->button(QDialogButtonBox::Yes);
            CHECK_SET_ERR(!nextButton->isEnabled(), "'Next' button is enabled");
            CHECK_SET_ERR(!allButton->isEnabled(), "'Select all' button is enabled");

//    3. Enter "1" as qualifier name. Click "Select all" button. Close the dialog.
//    Expected state: search buttons are enabled, a comment annotation with its qualifier are selected.
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "nameEdit", dialog), "1");

            CHECK_SET_ERR(nextButton->isEnabled(), "'Next' button is disabled");
            CHECK_SET_ERR(allButton->isEnabled(), "'Select all' button is disabled");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Yes);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Find qualifier..."));
    GTUtilsDialog::waitForDialog(os, new FindQualifierFiller(os, new Scenario));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, "NC_004718 features [sars.gb]");

    QList<QTreeWidgetItem *> selectedItems = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(2 == selectedItems.size(), QString("Unexpected count of selected items: expect 2, got %1").arg(selectedItems.size()));
    CHECK_SET_ERR("comment" == selectedItems.first()->text(0), QString("Unexpected annotation name: expect '%1', got '%2'")
                  .arg("comment").arg(selectedItems.first()->text(0)));
    CHECK_SET_ERR("1" == selectedItems.last()->text(0), QString("Unexpected qualifier name: expect '%1', got '%2'")
                  .arg("1").arg(selectedItems.first()->text(0)));
}

GUI_TEST_CLASS_DEFINITION(test_4440) {
//    1. Open COI.aln
//    2. Align with MUSCLE
//    Expected state: object should be named "COI".

    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os,"COI.aln");
    GTUtilsProjectTreeView::checkItem(os, GTUtilsProjectTreeView::getTreeView(os), "COI", idx);

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align" << "Align with MUSCLE..."));
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::checkItem(os, GTUtilsProjectTreeView::getTreeView(os), "COI", idx);
}

GUI_TEST_CLASS_DEFINITION(test_4463) {
//    1. Open attached file in "GenBank" format
//    2. Modify sequence
//    3. Unload document
//    Expected state: "Save document" dialog appeared
//    4. Press "Yes"
//    Expected state: UGENE does not crash
//    5. Load the document again
//    Expected state: the document is succesfully loaded

    GTFile::copy(os, testDir + "_common_data/genbank/gbbct131.gb.gz", sandBoxDir + "/test_4463.gb.gz");

    GTFileDialog::openFile(os, sandBoxDir, "test_4463.gb.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "10..20"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Remove subsequence..."));
    GTWidget::click(os, GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsMdi::closeWindow(os, "test_4463.gb.gz");
    GTUtilsDocument::unloadDocument(os, "test_4463.gb.gz", true);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::loadDocument(os, "test_4463.gb.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(NULL != GTUtilsSequenceView::getSeqWidgetByNumber(os), "Can't find sequence view widget");
}

GUI_TEST_CLASS_DEFINITION(test_4486) {
//    1. Open "data/samples/Assembly/chrM.sorted.bam".
//    2. Import with default settings.
    QDir().mkpath(sandBoxDir + "test_4486");
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_4486/test_4486.ugenedb"));
    GTFileDialog::openFile(os, dataDir + "samples/Assembly/chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Zoom in assembly view while reads are not visible.
    bool readsAreVisible = !GTUtilsAssemblyBrowser::isWelcomeScreenVisible(os);
    for (int i = 0; i < 100 && !readsAreVisible; i++) {
        GTUtilsAssemblyBrowser::zoomIn(os);
        readsAreVisible = !GTUtilsAssemblyBrowser::isWelcomeScreenVisible(os);
    }
    CHECK_SET_ERR(readsAreVisible, "Can't zoom to reads");

//    4. Use context menu on reads area:
//    {Export->visible reads}
//    5. Export dialog appeared. Press "Export"
//    Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export" << "Visible reads as sequences"));
    GTUtilsDialog::waitForDialog(os, new ExportReadsDialogFiller(os, sandBoxDir + "test_4486/reads.fa"));
    GTUtilsAssemblyBrowser::callContextMenu(os, GTUtilsAssemblyBrowser::Reads);

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4488) {
    //1. Open COI.aln.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Set a sequence as reference.
    GTUtilsMsaEditor::setReference(os, "Phaneroptera_falcata");

    //3. Open the "Statistics" OP tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);

    //4. Check "Show distances column".
    QCheckBox* showDistancesColumnCheck = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "showDistancesColumnCheck"));
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);

    //5. Switch off auto updating.
    QCheckBox *autoUpdateCheck = GTWidget::findExactWidget<QCheckBox*>(os, "autoUpdateCheck");
    GTCheckBox::setChecked(os, autoUpdateCheck, false);

    //6. Alignment context menu -> Add -> Sequence from file.
    //7. Select "data/samples/Assembly/chrM.fa".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Assembly/chrM.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "MSAE_MENU_LOAD_SEQ" << "Sequence from file"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);

    //UGENE crashes
}

GUI_TEST_CLASS_DEFINITION(test_4489) {
    //1. Open COI.aln.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Select some region
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(5, 5), QPoint(10, 10));
    //3. Move it to the right with a mouse.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(7, 7));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(12, 7));
    GTMouseDriver::release();
    GTGlobals::sleep();

    //Bug state : Overview is not recalculated.There is the "Waiting..." state.
    const QColor currentColor = GTUtilsMsaEditor::getGraphOverviewPixelColor(os, QPoint(GTUtilsMsaEditor::getGraphOverview(os)->width() - 5, 5));
    const QColor expectedColor = QColor("white");
    const QString currentColorString = QString("(%1, %2, %3)").arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());
    const QString expectedColorString = QString("(%1, %2, %3)").arg(expectedColor.red()).arg(expectedColor.green()).arg(expectedColor.blue());
    CHECK_SET_ERR(expectedColor == currentColor, QString("An unexpected color, maybe overview was not rendered: expected %1, got %2")
        .arg(expectedColorString).arg(currentColorString));
}

GUI_TEST_CLASS_DEFINITION(test_4505) {
//    1. Open "test/_common_data/scenarios/msa/Chikungunya_E1.fasta".
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    2. Delete any column
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(os, 1);
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //GTUtilsTaskTreeView::waitTaskFinished(os);
//    3. Press "Undo"
    GTUtilsMsaEditor::undo(os);
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Bug state: Error appeared in log: "[ERROR][19:02] Failed to create a multiple alignment row!"
    GTUtilsLog::check(os, l);
//    4. Click right button on MSA
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList()<<"Consensus mode"));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);
//    Bug state: UGENE crashes
    GTGlobals::sleep();

}

GUI_TEST_CLASS_DEFINITION(test_4508) {
    GTLogTracer logTracer;

//    1. Open "_common_data/fasta/400000_symbols_msa.fasta".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/400000_symbols_msa.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2.  Click "Export as image" button on the toolbar.
//    Expected state: an "Export Image" dialog appears.
//    3. Set SVG format.
//    Expected state: a warning appears, the dialog can't be accepted.

    class Scenario1 : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), "SVG", GTGlobals::UseMouse);

            QLabel *hintLabel = GTWidget::findExactWidget<QLabel *>(os, "hintLabel", dialog);
            CHECK_SET_ERR(NULL != hintLabel, "hintLabel is NULL");
            CHECK_SET_ERR(hintLabel->isVisible(), "hintLabel is invisible");
            const QString expectedSubstring = "selected region is too big";
            CHECK_SET_ERR(hintLabel->text().contains(expectedSubstring), QString("An expected substing not found: substing - '%1', text - '%2'")
                          .arg(expectedSubstring).arg(hintLabel->text()));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    for (int i = 0; i < 6; i++) {
        QWidget* zoomOutAction = GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Zoom Out");
        CHECK_BREAK(zoomOutAction->isEnabled());
        GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Zoom Out"));
        GTGlobals::sleep(500);
    }

    GTUtilsDialog::waitForDialog(os, new ExportImage(os, new Scenario1));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Export as image");

//    4. Cancel the dialog. Remove the first column. Call the dialog again and set SVG format.
//    Expected state: there are no warnings, dialog can be accepted.
//    5. Accept the dialog.

    GTUtilsMsaEditor::removeColumn(os, 1);
    GTThread::waitForMainThread();

    class Scenario2 : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "formatsBox", dialog), "SVG", GTGlobals::UseMouse);

            QLabel *hintLabel = GTWidget::findExactWidget<QLabel *>(os, "hintLabel", dialog);
            CHECK_SET_ERR(NULL != hintLabel, "hintLabel is NULL");
            CHECK_SET_ERR(!hintLabel->isVisible(), "hintLabel is visible");

            QDir().mkpath(sandBoxDir + "test_4508");
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "fileNameEdit", dialog), sandBoxDir + "test_4508/test_4508.svg");
            GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "exportSeqNames", dialog));
            GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "exportConsensus", dialog));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ExportImage(os, new Scenario2));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Export as image");

//    Expected state: the msa is successfully exported, there are no errors in the log.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + "test_4508/test_4508.svg"), QString("File '%1' doesn't exist")
        .arg(sandBoxDir + "test_4508/test_4508.svg"));
    CHECK_SET_ERR(0 < GTFile::getSize(os, sandBoxDir + "test_4508/test_4508.svg"), QString("File '%1' has zero size")
        .arg(sandBoxDir + "test_4508/test_4508.svg"));
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_4515) {

    GTFileDialog::openFile(os, dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "K");

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 0/0"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_4523) {
//    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Open "data/samples/FASTQ/eas.fastq".
    GTUtilsProject::openMultiSequenceFileAsSequences(os, dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Call context menu on the "human_T1.fa" document, select {Add -> Add object to document...} menu item. Add any sequence object.
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "eas.fastq", "EAS54_6_R1_2_1_413_324"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__ADD_OBJECT));
    GTUtilsProjectTreeView::callContextMenu(os, "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: "human_T1.fa" document contains two sequence objects.
    QModelIndex oldIndex = GTUtilsProjectTreeView::findIndex(os, QStringList() << "human_T1.fa" << "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QModelIndex addedIndex = GTUtilsProjectTreeView::findIndex(os, QStringList() << "human_T1.fa" << "EAS54_6_R1_2_1_413_324");

//    4. Rename both objects in "human_T1.fa" document to "123".
    GTUtilsProjectTreeView::rename(os, addedIndex, "123");
    GTGlobals::sleep(500);

    GTUtilsProjectTreeView::rename(os, oldIndex, "123");
    GTGlobals::sleep(500);

//    Expected state: "human_T1.fa" document contains two sequence objects with the same names.
    // Ok, I'm sure in the renaming method, skip this check

//    5. Open the second object.
    GTUtilsProjectTreeView::doubleClickItem(os, addedIndex);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the second object is opened. The second object is highlighted in the Project view.
    GTUtilsSequenceView::checkSequence(os, "CCCTTCTTGTCTTCAGCGTTTCTCC");

//    6. Open the first object.
    GTUtilsProjectTreeView::doubleClickItem(os, oldIndex);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the first object is opened. The first object is highlighted in the Project view.
    const QString sequenceBeginning = GTUtilsSequenceView::getBeginOfSequenceAsString(os, 25);
    const QString expectedSequenceBeginning = "TTGTCAGATTCACCAAAGTTGAAAT";
    CHECK_SET_ERR(expectedSequenceBeginning == sequenceBeginning,
                  QString("The sequence first 25 bases are incorrect: expected '%1', got '%2'")
                  .arg(expectedSequenceBeginning).arg(sequenceBeginning));
}

GUI_TEST_CLASS_DEFINITION(test_4524) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Remove the first sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "Phaneroptera_falcata");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    // Export the msa to SVG.
    GTUtilsDialog::waitForDialog(os, new ExportMsaImage(os, sandBoxDir + "test_4524.svg", "SVG", 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export" << "Export as image"));
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(10, 10));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(5000);

    const qint64 imageFileSize = GTFile::getSize(os, sandBoxDir + "test_4524.svg");
    CHECK_SET_ERR(imageFileSize > 0, "Export MSA to image failed. Unexpected image file size");

    // Current state : "undo" action becomes disabled.
    QWidget *undoButton = GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is disabled unexpectedly");
}

GUI_TEST_CLASS_DEFINITION(test_4536) {
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

//    2. Open "general" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);

//    Expected state: there are next values: Length: 0, Sequences: 0.
    const int length = GTUtilsOptionPanelMsa::getLength(os);
    const int height = GTUtilsOptionPanelMsa::getHeight(os);
    CHECK_SET_ERR(0 == length, QString("An incorrect alignment length is on the options panel: expected %1, got %2").arg(0).arg(length));
    CHECK_SET_ERR(0 == height, QString("An incorrect alignment height is on the options panel: expected %1, got %2").arg(0).arg(height));
}

GUI_TEST_CLASS_DEFINITION(test_4537) {
    //1. Open it in UGENE with Ctrl + Shift + O.
    class Scenario : public CustomScenario{
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            QComboBox *userSelectedFormat = GTWidget::findExactWidget<QComboBox*>(os, "userSelectedFormat", dialog);
            QStringList values = GTComboBox::getValues(os, userSelectedFormat);

            //Expected: there is no format SAM in the formats combobox.
            CHECK_SET_ERR(!values.contains("SAM"), "SAM format is in the combo box");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    //Expected: format selector dialog appears.
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, new Scenario()));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Assembly/chrM.sam"));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Open as...");
}

GUI_TEST_CLASS_DEFINITION(test_4552){

    // Open .
    // Align the sequences with MUSCLE.
    // While MUSCLE is running, open the "Tree" context menu.
    // Expected state: the "Build tree" action is disabled while the modification is not finished.
    // Current state: the "Build tree" action is enabled.

    GTFileDialog::openFile(os, testDir + "_common_data/clustal/1000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align" << "Align with MUSCLE..."));
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << MSAE_MENU_TREES << "Build Tree", PopupChecker::IsDisabled));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));
}


GUI_TEST_CLASS_DEFINITION(test_4557){
//    1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Open the PCR OP tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

//    3. Enter the forward primer: TTGTCAGATTCACCAAAGTT.
    GTUtilsOptionPanelSequenceView::setForwardPrimer(os, "AAATCAGATTCACCAAAGTT");
    GTUtilsPcr::setMismatches(os, U2Strand::Direct, 3);

//    4. Enter the reverse primer: ACCTCTCTCTGGCTGCCCTT.
    GTUtilsOptionPanelSequenceView::setReversePrimer(os, "GGGTCTCTCTGGCTGCCCTT");
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 3);

    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "extractProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString product = GTUtilsSequenceView::getSequenceAsString(os);
    QString expected = "AAATCAGATTCACCAAAGTTGAAATGAAGGAAAAAATGCTAAGGGCAGCCAGAGAGACCC";

    CHECK_SET_ERR(product == expected, "Unexpected product: " + product)

}

GUI_TEST_CLASS_DEFINITION(test_4563) {
    // 1. Set memory limit to 200 mb.
    class MemoryLimitSetScenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);
            GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "memorySpinBox", dialog), 200);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MemoryLimitSetScenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...");
    // 2. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    // 3. Open the "Align sequences with MUSCLE" sample scheme.
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");

    // 4. Set "_common_data/scenarios/_regression/4563/test_ma.fa" as the input file.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/scenarios/_regression/4563/test_ma.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/scenarios/_regression/4563/test_ma_1.fa");
    GTLogTracer l;
    // 5. Run the workflow.
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(5000);

    // 6. check log message
    GTUtilsTaskTreeView::waitTaskFinished(os);
    l.checkMessage("Can't allocate enough memory");
}

GUI_TEST_CLASS_DEFINITION(test_4587) {
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_4587"));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4587/", "extended_dna.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::checkObjectTypes(os,
        QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT,
        GTUtilsProjectTreeView::findIndex(os, "Contig1"));
    CHECK_SET_ERR(!l.hasError(), "logfile shouldn't contain errors");
}

GUI_TEST_CLASS_DEFINITION(test_4588) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4588", "4588.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<QTreeWidgetItem *> blastResultItems = GTUtilsAnnotationsTreeView::findItems(os, "blast result");
    GTUtilsAnnotationsTreeView::selectItems(os, blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os, const QString &dbPath, const QString &outputPath)
            : Filler(_os, "BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {};
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dbPath));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton", w));
            GTGlobals::sleep();
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "browseOutputButton", w));

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox*>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        };
    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os, testDir + "_common_data/scenarios/_regression/4588/BLAST/4588.00.nhr",
        testDir + "_common_data/scenarios/sandbox/4588_fetched.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "fetchMenu" << "fetchSequenceById"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|24489 shortread24489");
}

GUI_TEST_CLASS_DEFINITION(test_4588_1) {
    GTUtilsExternalTools::removeTool(os, "BlastAll");
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4588", "4588_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<QTreeWidgetItem *> blastResultItems = GTUtilsAnnotationsTreeView::findItems(os, "blast result");
    GTUtilsAnnotationsTreeView::selectItems(os, blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os, const QString &dbPath, const QString &outputPath)
            : Filler(_os, "BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {}
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dbPath));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton", w));
            GTGlobals::sleep();
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "browseOutputButton", w));

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox*>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        }
    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os, testDir + "_common_data/scenarios/_regression/4588/BLAST_plus/4588.00.nhr",
        testDir + "_common_data/scenarios/sandbox/4588_1_fetched.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "fetchMenu" << "fetchSequenceById"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|24481 shortread24481");
}

GUI_TEST_CLASS_DEFINITION(test_4588_2) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4588", "4588_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<QTreeWidgetItem *> blastResultItems = GTUtilsAnnotationsTreeView::findItems(os, "blast result");
    GTUtilsAnnotationsTreeView::selectItems(os, blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus& _os, const QString &dbPath, const QString &outputPath)
            : Filler(_os, "BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {}
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dbPath));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton", w));
            GTGlobals::sleep();
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "browseOutputButton", w));

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox*>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTWidget::click(os, button);
            GTGlobals::sleep(8000);
            button = buttonBox->button(QDialogButtonBox::Cancel);
            GTWidget::click(os, button);
        }
    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os, testDir + "_common_data/scenarios/_regression/4588/4588_1.gb",
        testDir + "_common_data/scenarios/sandbox/4588_1_fetched.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "fetchMenu" << "fetchSequenceById"));
    GTMouseDriver::click(Qt::RightButton);
}


GUI_TEST_CLASS_DEFINITION(test_4589) {

    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Call context menu on the "sars.gb" document and select {Add -> Add object to document} menu item.
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "murine.gb", "NC_001363 features"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__ADD_OBJECT));
    GTUtilsProjectTreeView::callContextMenu(os, "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::findIndex(os, QStringList() << "sars.gb" << "NC_001363 features");
    GTUtilsProjectTreeView::findIndex(os, QStringList() << "sars.gb" << "NC_004718 features");

}


GUI_TEST_CLASS_DEFINITION(test_4591) {

    //1. Open a circular sequence of length N.
    GTFileDialog::openFile(os, dataDir  + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    SelectSequenceRegionDialogFiller* filler = new SelectSequenceRegionDialogFiller(os, 140425, 2);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick( 'a', Qt::ControlModifier);
    //2. Open "Region selection" dialog {Ctrl+a} fill it with next data:
    //        {Single range selection} checked
    //        {Region:} 140425..2

    //3. Press 'Go' button
    //Expected state: this regions are selected on the view
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<ADV_MENU_COPY<<"Copy sequence"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);
    GTGlobals::sleep(500);
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text == "ATTG", "unexpected selection: " + text);
}


GUI_TEST_CLASS_DEFINITION(test_4591_1) {
    //1) Open samples/FASTA/human_T1.fa
        GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
        GTUtilsTaskTreeView::waitTaskFinished(os);

    //2) Select 100..10 region of the sequence
        class Scenario : public CustomScenario {
        public:
            void run(HI::GUITestOpStatus &os) {
                QWidget *dialog = QApplication::activeModalWidget();

                QLineEdit *startEdit = dialog->findChild<QLineEdit*>("startEdit");
                QLineEdit *endEdit = dialog->findChild<QLineEdit*>("endEdit");
                CHECK_SET_ERR(startEdit != NULL, "QLineEdit \"startEdit\" not found");
                CHECK_SET_ERR(endEdit != NULL, "QLineEdit \"endEdit\" not found");

                GTLineEdit::setText(os, startEdit, QString::number(321));
                GTLineEdit::setText(os, endEdit, QString::number(123));

                QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox"));
                QPushButton* goButton = box->button(QDialogButtonBox::Ok);
                CHECK_SET_ERR(goButton!= NULL, "Go button not found");
                CHECK_SET_ERR(!goButton->isEnabled(), "Go button is enabled");

                GTLineEdit::setText(os, startEdit, QString::number(123));
                GTLineEdit::setText(os, endEdit, QString::number(321));
                CHECK_SET_ERR(goButton!= NULL, "Go button not found");
                CHECK_SET_ERR(goButton->isEnabled(), "Go button is notenabled");

                GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
            }
        };
        GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, new Scenario));
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Select" << "Sequence region"));
        GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));

}

GUI_TEST_CLASS_DEFINITION(test_4591_2) {

    //1. Open a circular sequence of length N.
    GTFileDialog::openFile(os, dataDir  + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    SelectSequenceRegionDialogFiller* filler = new SelectSequenceRegionDialogFiller(os, 3, 3);
    filler->setCircular(true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick( 'a', Qt::ControlModifier);
    //2. Open "Region selection" dialog {Ctrl+a} fill it with next data:
    //        {Single range selection} checked
    //        {Region:} 140425..2

    //3. Press 'Go' button
    //Expected state: this regions are selected on the view
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<ADV_MENU_COPY<<"Copy sequence"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);
    GTGlobals::sleep(500);
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text == "G", "unexpected selection: " + text);
}

GUI_TEST_CLASS_DEFINITION(test_4606) {
    //1. Create custom WD element
    //2. Do not fill "Description" and "Parameters description" fields
    //3. Create workflow with created element and tun it
    //Expected state: no safepoint triggered

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "Element_4606";

    QList<CreateElementWithCommandLineToolFiller::InOutData> input;
    CreateElementWithCommandLineToolFiller::InOutDataType inOutDataType;
    inOutDataType.first = CreateElementWithCommandLineToolFiller::Sequence;
    inOutDataType.second = "FASTA";
    input << CreateElementWithCommandLineToolFiller::InOutData("in1",
        inOutDataType);
    settings.input = input;
    settings.executionString = "echo";

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Continue"));

    GTUtilsDialog::waitForDialog(os, new CreateElementWithCommandLineToolFiller(os, settings));
    QAbstractButton *createElement = GTAction::button(os, "createElementWithCommandLineTool");
    GTWidget::click(os, createElement);
    GTGlobals::sleep();

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement(os, "Read Sequence",true);
    CHECK_SET_ERR(read != NULL, "Failed to add an element");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    WorkflowProcessItem* customWorker = GTUtilsWorkflowDesigner::getWorker(os, "Element_4606");

    GTUtilsWorkflowDesigner::connect(os, read, customWorker);
    GTLogTracer l;

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_4620) {
//    1. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Call a context menu on the chromatogram, select {Edit new sequence} menu item.
//    Expected state: an "Add new document" dialog appears.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    3. Ensure that there is no "Database connection" format. Ensure that there are no formats with "DocumentFormatFlag_Hidden" flag.
            const QStringList formats = GTComboBox::getValues(os, GTWidget::findExactWidget<QComboBox *>(os, "documentTypeCombo", dialog));
            CHECK_SET_ERR(!formats.contains("Database connection"), "'Database connection' format isavailable");

            QList<DocumentFormatId> registeredFormatsIds = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();
            QMap<QString, DocumentFormat *> formatsWithNames;
            foreach (const DocumentFormatId &formatId, registeredFormatsIds) {
                DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
                CHECK_SET_ERR(NULL != format, QString("Can't get document format by ID: '%1'").arg(formatId));
                formatsWithNames[format->getFormatName()] = format;
            }

            foreach (const QString &formatName, formats) {
                DocumentFormat *format = formatsWithNames.value(formatName, NULL);
                CHECK_SET_ERR(NULL != format, QString("An unknown format: '%1'").arg(formatName));
                CHECK_SET_ERR(!format->getFlags().testFlag(DocumentFormatFlag_Hidden), "A hidden format is offered to choose");
            }

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit new sequence"));
    GTUtilsDialog::waitForDialog(os, new AddNewDocumentDialogFiller(os, new Scenario));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4621) {
    //1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Find some restriction sites.
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            QWidget *enzymesSelectorWidget = GTWidget::findWidget(os, "enzymesSelectorWidget");
            CHECK_SET_ERR(NULL != enzymesSelectorWidget, "enzymesSelectorWidget is NULL");

            GTWidget::click(os, GTWidget::findWidget(os, "selectAllButton", enzymesSelectorWidget));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList(), new Scenario()));
    GTWidget::click(os, GTWidget::findWidget(os, "Find restriction sites_widget"));
    GTGlobals::sleep(500);

    //3. Delete sequence object
    GTUtilsProjectTreeView::click(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state: UGENE does not crash.
}
GUI_TEST_CLASS_DEFINITION(test_4624) {
    //1. Open assembly with DNA extended alphabet
    GTFileDialog::openFile(os, testDir + "_common_data/ugenedb", "extended_dna.ace.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<ExportCoverageDialogFiller::Action> actions;

    //2. Export coverage with bases quantity info
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, QVariant("Per base"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SelectFile, sandBoxDir + "test_4624.txt");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetExportBasesQuantity, QVariant(true));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, QVariant());

    GTUtilsDialog::waitForDialog(os, new ExportCoverageDialogFiller(os, actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog(os);

    //3. Check the coverage
    QString templateCoverage = getFileContent(testDir + "_common_data/scenarios/_regression/4624/4624.txt");
    QString resCoverage = getFileContent(sandBoxDir + "test_4624.txt");
    CHECK_SET_ERR(templateCoverage == resCoverage, "Incorrect coverage has been exported");
}

GUI_TEST_CLASS_DEFINITION(test_4628) {
    GTUtilsNotifications::waitForNotification(os, false);
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4628", "cow.chr13.repeats.shifted.bed");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    QTextEdit *textEdit = dynamic_cast<QTextEdit*>(GTWidget::findWidget(os, "reportTextEdit", GTUtilsMdi::activeWindow(os)));
    CHECK_SET_ERR(textEdit->toPlainText().contains("incorrect strand value '+379aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa...' at line 5333"),
        "Expected message is not found in the report text");

}

GUI_TEST_CLASS_DEFINITION(test_4674) {
    // 1. Open COI.aln
    // 2. Build the tree and synchronize it with the alignment
    // 3. Delete one sequence
    // Expected state: message box appears
    // 4. Cancel message box
    // Expected state: the edit is undone and the tree is still connected with the alignment
    // 5. Delete one sequence one more time
    // Expected state: message box appears
    // 6. Confirm the modification
    // Expected state: the connection with the tree is broken, the sequence is removed

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4674", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int seqNumber = GTUtilsMsaEditor::getSequencesCount(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTUtilsMsaEditor::clickSequenceName(os, "Zychia_baranovi");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    MSAEditorTreeViewerUI* ui = qobject_cast<MSAEditorTreeViewerUI*>( GTUtilsPhyTree::getTreeViewerUi(os) );
    CHECK_SET_ERR(ui != NULL, "Cannot find the tree");
    CHECK_SET_ERR(ui->isCurTreeViewerSynchronized(), "The connection with the tree is lost");
    CHECK_SET_ERR(seqNumber == GTUtilsMsaEditor::getSequencesCount(os), "The sequence removal was not undone");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsMsaEditor::clickSequenceName(os, "Zychia_baranovi");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    CHECK_SET_ERR(!ui->isCurTreeViewerSynchronized(), "The connection with the tree is still there");
    CHECK_SET_ERR(seqNumber != GTUtilsMsaEditor::getSequencesCount(os), "The sequence was not removed");
}

GUI_TEST_CLASS_DEFINITION(test_4674_1) {
    // 1. Open COI.aln
    // 2. Align the sequence to alignment
    // 3. Build the tree and synchronize it with the alignment
    // 4. Click Undo
    // Expected state: message box appears
    // 4. Cancel message box
    // Expected state: the undoing is undone and the tree is still connected with the alignment
    // 5. Click Undo one more time
    // Expected state: message box appears
    // 6. Confirm the modification
    // Expected state: the connection with the tree is broken, the aligned sequence is removed

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button( os, "Align sequence to this alignment" );
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence to this alignment\" action not found");
    GTWidget::click( os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4674_1", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int seqNumber = GTUtilsMsaEditor::getSequencesCount(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTUtilsMsaEditor::undo(os);
    GTThread::waitForMainThread();

    MSAEditorTreeViewerUI* ui = qobject_cast<MSAEditorTreeViewerUI*>( GTUtilsPhyTree::getTreeViewerUi(os) );
    CHECK_SET_ERR(ui != NULL, "Cannot find the tree");
    CHECK_SET_ERR(ui->isCurTreeViewerSynchronized(), "The connection with the tree is lost");
    CHECK_SET_ERR(seqNumber == GTUtilsMsaEditor::getSequencesCount(os), "Undo was not undone 1");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsMsaEditor::undo(os);
    GTUtilsMsaEditor::undo(os);
    GTThread::waitForMainThread();

    CHECK_SET_ERR(!ui->isCurTreeViewerSynchronized(), "The connection with the tree is still there");
    CHECK_SET_ERR(seqNumber != GTUtilsMsaEditor::getSequencesCount(os), "Undo was not undone 2");
}

GUI_TEST_CLASS_DEFINITION(test_4674_2) {
    // 1. Open COI.aln
    // 2. Build the tree and synchronize it with the alignment
    // 3. Insert a gap
    // Expected state: message box appears
    // 4. Cancel message box
    // Expected state: the insertion is undone and the tree is still connected with the alignment
    // 5. Insert a gap again
    // Expected state: message box appears
    // 6. Confirm the modification
    // Expected state: the connection with the tree is broken, the gap is removed
    // 7. Delete some character
    // Expected state: no message box

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4674_2", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int alnLen = GTUtilsMSAEditorSequenceArea::getLength(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(10, 10));
    GTKeyboardDriver::keyClick( Qt::Key_Space);
    GTGlobals::sleep();

    MSAEditorTreeViewerUI* ui = qobject_cast<MSAEditorTreeViewerUI*>( GTUtilsPhyTree::getTreeViewerUi(os) );
    CHECK_SET_ERR(ui != NULL, "Cannot find the tree");
    CHECK_SET_ERR(ui->isCurTreeViewerSynchronized(), "The connection with the tree is lost");
    CHECK_SET_ERR(alnLen == GTUtilsMSAEditorSequenceArea::getLength(os), "Undo was not undone");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(10, 10));
    GTKeyboardDriver::keyClick( Qt::Key_Space);
    GTGlobals::sleep();

    CHECK_SET_ERR(!ui->isCurTreeViewerSynchronized(), "The connection with the tree is still there");
    CHECK_SET_ERR(alnLen != GTUtilsMSAEditorSequenceArea::getLength(os), "Undo was not undone");

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(10, 10));
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_4682) {
    // Check find from status bar

    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/" , "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *msaEditorStatusBar = GTWidget::findWidget(os, "msa_editor_status_bar");
    CHECK_SET_ERR(msaEditorStatusBar != NULL, "MSAEditorStatusBar is NULL");

    QLineEdit *searchEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "searchEdit", msaEditorStatusBar));
    QWidget *findForward = GTWidget::findWidget(os, "Find forward", msaEditorStatusBar);

    // 2. Put '34c---t A' in text field at status bar. Click Find next button.
    GTLineEdit::setText(os, searchEdit, "34c---t A", true);
    CHECK_SET_ERR(searchEdit->text() == "CTA", "Wrong search pattern, validator does not work.");

    GTWidget::click(os, findForward);
    // Expected state: find result sequence Isophya_altaica_EF540820 region 8..10
    GTGlobals::sleep();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(7, 1, 3, 1));
}

GUI_TEST_CLASS_DEFINITION(test_4682_1) {
    // Check find from status bar

    // 1. Open document data/samples/Stockholm/CBS.sto
    GTFileDialog::openFile(os, dataDir + "samples/Stockholm" , "CBS.sto");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *msaEditorStatusBar = GTWidget::findWidget(os, "msa_editor_status_bar");
    CHECK_SET_ERR(msaEditorStatusBar != NULL, "MSAEditorStatusBar is NULL");

    QLineEdit *searchEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "searchEdit", msaEditorStatusBar));
    QWidget *findForward = GTWidget::findWidget(os, "Find forward", msaEditorStatusBar);

    // 2. Put ' Nn--G34..f' in text field at status bar. Click Find next button.
    GTLineEdit::setText(os, searchEdit, " Nn--G34..f", true);
    CHECK_SET_ERR(searchEdit->text() == "NNGF", "Wrong search pattern, validator does not work.");

    GTWidget::click(os, findForward);
    // Expected state: find result sequence O31698/88-139 region 25..30
    GTGlobals::sleep();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(24, 3, 6, 1));
}

GUI_TEST_CLASS_DEFINITION(test_4682_2) {
    // Check find from status bar

    // 1. Open document _common_data\scenarios\msa\ma2_gapped.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/" , "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Add AMINO.fa to ma2_gapped.aln
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/fasta/" , "AMINO.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<MSAE_MENU_LOAD<<"Sequence from file"));
    GTMenu::showContextMenu(os,GTWidget::findWidget(os, "msa_editor_sequence_area"));

    QWidget *msaEditorStatusBar = GTWidget::findWidget(os, "msa_editor_status_bar");
    CHECK_SET_ERR(msaEditorStatusBar != NULL, "MSAEditorStatusBar is NULL");

    QLineEdit *searchEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "searchEdit", msaEditorStatusBar));
    QWidget *findForward = GTWidget::findWidget(os, "Find forward", msaEditorStatusBar);

    // 3. Put ' eE--F f' in text field at status bar. Click Find next button.
    GTLineEdit::setText(os, searchEdit, " eE--F f", true);
    CHECK_SET_ERR(searchEdit->text() == "EEFF", "Wrong search pattern, validator does not work.(EEFF)");

    GTWidget::click(os, findForward);
    // Expected state: find result sequence AMINO263 region 39..42
    GTGlobals::sleep();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(38, 10, 4, 1));

    // 4. Undo changes
    GTKeyboardDriver::keyClick( 'z', Qt::ControlModifier);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(searchEdit->text() == "", "Search field should be cleared.");

    // 5. Put '34c---t A' in text field at status bar. Click Find next button.
    GTLineEdit::setText(os, searchEdit, "34c---t A", true);
    CHECK_SET_ERR(searchEdit->text() == "CTA", "Wrong search pattern, validator does not work.(CTA)");

    GTWidget::click(os, findForward);
}

GUI_TEST_CLASS_DEFINITION(test_4687) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Open OP and select pairwice alignment tab, select sequences to align
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTGlobals::sleep(500);
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");

    //3. Press "Align sequence to this alignment" and add next sequence _common_data/fasta/amino_ext.fa
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/fasta/", "amino_ext.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button(os, "Align sequence to this alignment");
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence to this alignment\" action not found");
    GTWidget::click(os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Check warning message visibility
    QWidget *label = GTWidget::findWidget(os, "lblMessage");
    CHECK_SET_ERR(label->isVisible(), "Label should be visible");

    //4. Undo changes
    GTKeyboardDriver::keyClick( 'z', Qt::ControlModifier);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Check warning message invisibility
    CHECK_SET_ERR(!label->isVisible(), "Label should be invisible");
}

GUI_TEST_CLASS_DEFINITION(test_4689_1) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    QComboBox* consensusType = GTWidget::findExactWidget<QComboBox*>(os, "consensusType");
    QSpinBox* thresholdSpinBox = GTWidget::findExactWidget<QSpinBox*>(os, "thresholdSpinBox");

    //    3. Set "Strict" consensus algorithm
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");
    GTComboBox::setIndexWithText(os, consensusType, "Strict");
    GTSpinBox::setValue(os, thresholdSpinBox, 50, GTGlobals::UseKeyBoard);

    QLineEdit* sequenceLineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "sequenceLineEdit");
    consensusType = GTWidget::findExactWidget<QComboBox*>(os, "consensusType");
    thresholdSpinBox = GTWidget::findExactWidget<QSpinBox*>(os, "thresholdSpinBox");

    CHECK_SET_ERR(sequenceLineEdit->text() == "Phaneroptera_falcata", QString("unexpected reference: %1").arg(sequenceLineEdit->text()));
    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    CHECK_SET_ERR(thresholdSpinBox->value() == 50, QString("unexpected threshold value: %1").arg(thresholdSpinBox->value()));

    //    4. Add amino extended sequence
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/amino_ext.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTGlobals::sleep();
    //    5. Check that algorithm is "Strict", but set of algorithms correspont to raw alphabet
    consensusType = GTWidget::findExactWidget<QComboBox*>(os, "consensusType");

    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    CHECK_SET_ERR(consensusType->count() == 2, QString("Incorrect consensus algorithms count: %1").arg(consensusType->count()));
}

GUI_TEST_CLASS_DEFINITION(test_4689_2) {
    //    1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add amino extended sequence
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/amino_ext.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep();
    //    3. Press "Undo"
    GTUtilsMsaEditor::undo(os);
    GTGlobals::sleep();

    //    4. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(500);
    //    5. Set consensus algorithm "Levitsky"
    QComboBox* consensusType = GTWidget::findExactWidget<QComboBox*>(os, "consensusType");
    GTComboBox::setIndexWithText(os, consensusType, "Levitsky");
    //    6. Close the tab
    GTUtilsOptionPanelMsa::closeTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(500);

    //    7. Press "Redo"
    GTUtilsMsaEditor::redo(os);
    GTGlobals::sleep();

    //    8. Open general option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(500);

    //    9. Check that algorithm is "ClustalW"
    consensusType = GTWidget::findExactWidget<QComboBox*>(os, "consensusType");
    CHECK_SET_ERR(consensusType->currentText() == "ClustalW", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    //    10. Change algorithm
    GTComboBox::setIndexWithText(os, consensusType, "Strict");
    CHECK_SET_ERR(consensusType->currentText() == "Strict", QString("unexpected consensus: %1").arg(consensusType->currentText()));
    //Expected: UGENE does not crash
}

GUI_TEST_CLASS_DEFINITION(test_4694) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Open OP and select pairwice alignment tab, select sequences to align, set "in new window" parameter to "false"
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTGlobals::sleep(500);
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");

    QWidget* widget = GTWidget::findWidget(os, "outputContainerWidget");
    CHECK_SET_ERR(widget != NULL, QString("%1 not found").arg("outputContainerWidget"));
    if (widget->isHidden()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));
    }
    QCheckBox* inNewWindowCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "inNewWindowCheckBox"));
    CHECK_SET_ERR(inNewWindowCheckBox != NULL, "inNewWindowCheckBox not found");
    GTCheckBox::setChecked(os, inNewWindowCheckBox, false);
    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));

    //4. Undo changes
    GTKeyboardDriver::keyClick( 'z', Qt::ControlModifier);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state "Undo" button is disabled
    QAbstractButton *undo = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Button should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_4699) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "AaaI"));
    GTWidget::click(os, GTWidget::findWidget(os, "Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidget *tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget(os, "restrictionMapTreeWidget"));
    QTreeWidgetItem *item = GTTreeWidget::findItem(os, tree, "76105..76110");
    GTTreeWidget::click(os, item);

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "AacLI"));
    GTWidget::click(os, GTWidget::findWidget(os, "Find restriction sites_widget"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidget *newtree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget(os, "restrictionMapTreeWidget"));
    QTreeWidgetItem *newitem = GTTreeWidget::findItem(os, newtree, "10101..10106");
    GTTreeWidget::click(os, newitem);
}

GUI_TEST_CLASS_DEFINITION(test_4700) {
    //1. Open assembly
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4700/", "almost-empty.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
    //2. Find area without reads
    for (int i = 0;i < 24;i++) {
        GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);
        GTGlobals::sleep(100);
    }
    GTGlobals::sleep(2000);

    GTKeyboardDriver::keyClick(Qt::Key_Home);
    GTGlobals::sleep(2000);
    //3. Export visible reads
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export" << "Visible reads as sequences"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsAssemblyBrowser::callContextMenu(os, GTUtilsAssemblyBrowser::Reads);
}

GUI_TEST_CLASS_DEFINITION(test_4702_1) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    // 2. Select one annotation
    QList<QTreeWidgetItem *> commentItem = GTUtilsAnnotationsTreeView::findItems(os, "comment");
    GTUtilsAnnotationsTreeView::selectItems(os, commentItem);
    int selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(1 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(1).arg(selectedItemsCount));
    // 3. Use context menu on AnnotationsTreeView {Invert annotations selection}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "invert_selection_action"));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: all annotations other besides the originally selected annotation are selected
    selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(359 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(359).arg(selectedItemsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4702_2) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    // 2. Select two annotation from different groups (denote them as "A" and "B").
    QList<QTreeWidgetItem *> annotationItems = GTUtilsAnnotationsTreeView::findItems(os, "comment");
    annotationItems << GTUtilsAnnotationsTreeView::findItems(os, "source");
    GTUtilsAnnotationsTreeView::selectItems(os, annotationItems);
    int selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(2 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(2).arg(selectedItemsCount));
    // 3. Use context menu on AnnotationsTreeView {Invert annotations selection}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "invert_selection_action"));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: All annotations, except comment and source, are selected.
    selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(358 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(358).arg(selectedItemsCount));
}


GUI_TEST_CLASS_DEFINITION(test_4702_3) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    // 2. Make sure nothing is selected in the Annotations Editor.
    int selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(0 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(0).arg(selectedItemsCount));
    // 3. Use context menu on AnnotationsTreeView {Invert annotations selection}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "invert_selection_action"));
    QPoint p = GTUtilsAnnotationsTreeView::getItemCenter(os, "comment");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: All annotations are selected.
    selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(359 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(359).arg(selectedItemsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4702_4) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    // 2. Select an annotations group in the Annotations Editor.
    QList<QTreeWidgetItem *> annotationGroup;
    annotationGroup  << GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 139)");
    CHECK_SET_ERR(1 == annotationGroup.size(), QString("CDS group is not found"));
    GTUtilsAnnotationsTreeView::selectItems(os, annotationGroup);
    int selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(1 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(1).arg(selectedItemsCount));
    // 3. Use context menu on AnnotationsTreeView {Invert annotations selection}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "invert_selection_action"));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: All annotations are selected.
    selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(361 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(361).arg(selectedItemsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4702_5) {
    // 1. Open "samples/Genbank/NC_014267.1.gb"
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    // 2. Select an annotations document in the Annotations Editor.
    QList<QTreeWidgetItem *> annotationGroup;
    annotationGroup << GTUtilsAnnotationsTreeView::findItem(os, "NC_014267 features [NC_014267.1.gb]");
    CHECK_SET_ERR(1 == annotationGroup.size(), QString("NC_014267 features group is not found"));
    GTUtilsAnnotationsTreeView::selectItems(os, annotationGroup);
    int selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(1 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(1).arg(selectedItemsCount));
    // 3. Use context menu on AnnotationsTreeView {Invert annotations selection}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "invert_selection_action"));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: All annotations are selected.
    selectedItemsCount = GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size();
    CHECK_SET_ERR(361 == selectedItemsCount, QString("Incorrect selected annotations count: expected - %1, obtained - %2 ").arg(361).arg(selectedItemsCount));
}

GUI_TEST_CLASS_DEFINITION(test_4710){
//    1) Make sure to have several dashboards for different workflows runs.
//    2) Run a workflow, e.g. I ran "Variation annotation with SnpEff" with the tutorial data.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "SnpEff");
    GTThread::waitForMainThread();
    GTUtilsWorkflowDesigner::click(os, "Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTGlobals::sleep();

    QTabWidget* tabs = GTUtilsDashboard::getTabWidget(os);
    //int initialIndex = tabs->count();
    QString initTabName = GTTabWidget::getTabName(os, tabs, tabs->currentIndex());
//    3) During the workflow run open the "Dashboard Manager", select one of the several dashboard, and click "Remove selected."
    QWidget* dmButton = GTAction::button(os, GTAction::findAction(os, "Dashboards manager"));

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    4) Select some dashboards in the dialog
            GTGlobals::sleep(500);
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QTreeWidget* listWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "listWidget", dialog);
            GTTreeWidget::click(os, listWidget->invisibleRootItem()->child(0));
            GTGlobals::sleep(500);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Confirm"));
            GTWidget::click(os, GTWidget::findWidget(os, "removeButton", dialog));
            GTGlobals::sleep(500);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new DashboardsManagerDialogFiller(os, new custom()));
    GTWidget::click(os, dmButton);
//    Expected result: the selected dashboard was removed, the current workflow execution is proceeding and its dashboard is shown.
//    Actual result: the workflow execution is proceeding, but the current dashboard is removed.
    GTGlobals::sleep();

    //int finalIndex = tabs->count();
    QString finalTabName = GTTabWidget::getTabName(os, tabs, tabs->currentIndex());

    GTGlobals::sleep();
    CHECK_SET_ERR(initTabName == finalTabName, "tab name changed. Initial: " + initTabName + ", actual: " + finalTabName);
    AppContext::getTaskScheduler()->cancelAllTasks();
    GTUtilsTaskTreeView::waitTaskFinished(os, 60000);
}

GUI_TEST_CLASS_DEFINITION(test_4710_1){
//    1) Run "Align with MUSCLE" with "_common_data\fasta\PF07724_full_family.fa".
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTUtilsWorkflowDesigner::click(os, "Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/PF07724_full_family.fa");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTGlobals::sleep();
//    Expected state: Close dashboard tab button is disabled

    QTabWidget* dashboardWidget = GTUtilsDashboard::getTabWidget(os);
    QWidget* corner = GTTabWidget::getTabCornerWidget(os, dashboardWidget, dashboardWidget->currentIndex());
    CHECK_SET_ERR(!corner->isEnabled(), "close tab button is unexpectidly enabled");

//    2) Stop the workflow execution
    GTWidget::click(os, GTAction::button(os, "Stop workflow"));
    GTGlobals::sleep(500);
    corner = GTTabWidget::getTabCornerWidget(os, dashboardWidget, dashboardWidget->currentIndex());
    CHECK_SET_ERR(corner->isEnabled(), "close tab button is unexpectidly disabled");
//    Expected result: Close dashboard tab button is enabled
}

GUI_TEST_CLASS_DEFINITION(test_4712) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            GTGlobals::sleep(1000);
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog was not found");

            QTreeView* treeView = dialog->findChild<QTreeView*>();
            int visibleItemCount = 0;
            for (int i = 0; i < treeView->model()->rowCount(); ++i) {
                if (Qt::NoItemFlags != treeView->model()->flags(treeView->model()->index(i, 0))) {
                    ++visibleItemCount;
                }
            }
            CHECK_SET_ERR(visibleItemCount == 0, "Should be zero items");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    //    1. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open "data/samples/ABIF/A01.abi".
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    //    3. Click context menu item "Edit existing sequence"
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit existing sequence"));
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, new Scenario()));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4714_1) {
//    1. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Call a context menu on the chromatogram, select {Edit new sequence} menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit new sequence"));
    GTUtilsDialog::waitForDialog(os, new AddNewDocumentDialogFiller(os, "FASTA", sandBoxDir + "test_4714_1.fa"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new sequence is added to the project and to the current sequence view.
//    3. Lock the added document.
    GTUtilsDocument::lockDocument(os, "test_4714_1.fa");

//    4. Call a context menu on the chromatogram, select {Remove edited sequence} menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Remove edited sequence"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

//    Expected state: the sequence is removed from the view,
//                    context menu contains "Edit new sequence" and "Edit existing sequence" items
//                    and doesn't contain "Remove edited sequence" and "Undo changes" actions.
    int sequencesCount = GTUtilsSequenceView::getSeqWidgetsNumber(os);
    CHECK_SET_ERR(1 == sequencesCount, QString("An incorrect vount of sequences in the view: expect %1, got %2")
                  .arg(1).arg(sequencesCount));

    const QStringList visibleItems = QStringList() << "Edit new sequence"
                                                   << "Edit existing sequence";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList(), visibleItems));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

    const QStringList invisibleItems = QStringList() << "Remove edited sequence"
                                                     << "Undo changes";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList(), invisibleItems, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4714_2) {
//    1. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Call a context menu on the chromatogram, select {Edit new sequence} menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit new sequence"));
    GTUtilsDialog::waitForDialog(os, new AddNewDocumentDialogFiller(os, "FASTA", sandBoxDir + "test_4714_2.fa"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a new sequence is added to the project and to the current sequence view.
//    3. Lock the added document.
    GTUtilsDocument::lockDocument(os, "test_4714_2.fa");

//    4. Remove the added document from the project.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "Save document:"));
    GTUtilsDocument::removeDocument(os, "test_4714_2.fa", GTGlobals::UseMouse);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the sequence is removed from the view,
//                    context menu contains "Edit new sequence" and "Edit existing sequence" items
//                    and doesn't contain "Remove edited sequence" and "Undo changes" actions.
    int sequencesCount = GTUtilsSequenceView::getSeqWidgetsNumber(os);
    CHECK_SET_ERR(1 == sequencesCount, QString("An incorrect vount of sequences in the view: expect %1, got %2")
                  .arg(1).arg(sequencesCount));

    const QStringList visibleItems = QStringList() << "Edit new sequence"
                                                   << "Edit existing sequence";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList(), visibleItems));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

    const QStringList invisibleItems = QStringList() << "Remove edited sequence"
                                                     << "Undo changes";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList(), invisibleItems, PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4718) {
    GTLogTracer logTracer;

//    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Open "Search in Sequence" options panel tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTGlobals::sleep(500);

//    3. Close the tab.
    GTUtilsOptionPanelSequenceView::closeTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTGlobals::sleep(500);

//    4. Open the tab again.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

//    Expected state: log does not contain errors.
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_4719_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    3. Click "Align sequence to this alignment" and select "_common_data/fasta/amino_ext.fa".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/amino_ext.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    QComboBox* colorScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "colorScheme"));
    QComboBox* highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));
    GTComboBox::checkCurrentValue(os, colorScheme, "UGENE    ");
    GTComboBox::checkCurrentValue(os, highlightingScheme, "No highlighting    ");

    //    4. Undo changes
    GTUtilsMsaEditor::undo(os);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    colorScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "colorScheme"));
    highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));
    GTComboBox::checkCurrentValue(os, colorScheme, "UGENE");
    GTComboBox::checkCurrentValue(os, highlightingScheme, "No highlighting");

}

GUI_TEST_CLASS_DEFINITION(test_4719_2) {
    //    1. Open "_common_data/clustal/amino_ext.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/amino_ext.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    3. Click "Align sequence to this alignment" and select "_common_data/fasta/amino_ext.fa".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/fa1.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: "UGENE" color scheme is selected, "UGENE" highlight scheme is selected
    QComboBox* colorScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "colorScheme"));
    QComboBox* highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));
    GTComboBox::checkCurrentValue(os, colorScheme, "UGENE    ");
    GTComboBox::checkCurrentValue(os, highlightingScheme, "No highlighting    ");

    //    4. Undo changes
    GTUtilsMsaEditor::undo(os);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    colorScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "colorScheme"));
    highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));
    GTComboBox::checkCurrentValue(os, colorScheme, "UGENE");
    GTComboBox::checkCurrentValue(os, highlightingScheme, "No highlighting");
}

GUI_TEST_CLASS_DEFINITION(test_4719_3) {
    //    1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click "Align sequence to this alignment" and select "data/samples/Genbank/PBR322.gb".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Genbank/PBR322.gb"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Open/close highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::closeTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    4. Undo changes
    GTUtilsMsaEditor::undo(os);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    5. Open/close highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    QComboBox* colorScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "colorScheme"));
    QComboBox* highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));

    GTComboBox::checkCurrentValue(os, colorScheme, "UGENE");
    GTComboBox::checkCurrentValue(os, highlightingScheme, "No highlighting");
}

GUI_TEST_CLASS_DEFINITION(test_4728) {
    //1. Open 'fa1.fa'
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

    //2. Select all sequence by mouse
    ADVSingleSequenceWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(NULL != seqWidget, "Can not find ADVSingleSequenceWidget");

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
    GTGlobals::sleep(200);

    //Expected state: all 4 symbols are selected
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, "Incorrect number of regions in selection is detected");
    CHECK_SET_ERR(selection.at(0).length == 4, "Incorrect selection length is detected");
}


GUI_TEST_CLASS_DEFINITION(test_4732) {
    QFile::copy(dataDir + "samples/FASTA/human_T1.fa", sandBoxDir + "test_4732.fa");
    //1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, sandBoxDir + "test_4732.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Document context menu -> Export / Import -> Export sequences.
    //Expected: "Export selected sequences" dialog appears.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
    class Scenario : public CustomScenario {
    public:
        Scenario() : filler(NULL) {}
        void setFiller(ExportSelectedRegionFiller *value) { filler = value; }
        void run(HI::GUITestOpStatus &os) {
            //3. Delete "human_T1.fa" document from the file system.
            bool removed = QFile::remove(sandBoxDir + "test_4732.fa");
            CHECK_SET_ERR(removed, "Can't remove the file");

            //Expected: the dialog about external modification of documents does not appear.
            GTGlobals::sleep(5000);
            CHECK_SET_ERR(NULL != filler, "NULL filler");
            filler->setPath(sandBoxDir);
            filler->setName("test_4732_out.fa");

            //4. Click "Export".
            filler->commonScenario();
        }
    private:
        ExportSelectedRegionFiller *filler;
    };
    //Expected: the dialog about external modification of documents appears.
    //5. Click "No".
    //Expected: UGENE does not crash.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    Scenario *scenario = new Scenario();
    ExportSelectedRegionFiller *filler = new ExportSelectedRegionFiller(os, scenario);
    scenario->setFiller(filler);
    GTUtilsDialog::waitForDialog(os, filler);
    GTUtilsProjectTreeView::click(os, "test_4732.fa", Qt::RightButton);
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_4734) {
    //    1. Open file {data/samples/FASTA/human_T1.fa}
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open {Context menu -> Analyze menu} and check menu item "Show circular view" is not present there

    class AllPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QMenu *activePopupMenu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
            CHECK_SET_ERR(NULL != activePopupMenu, "Active popup menu is NULL");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Analyze");
            activePopupMenu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
            QAction* showCircular = GTMenu::getMenuItem(os, activePopupMenu, "globalToggleViewAction", false);
            CHECK_SET_ERR(showCircular == NULL, "'Toggle circular view' menu item should be NULL");

            GTKeyboardDriver::keyClick( Qt::Key_Escape);
            GTGlobals::sleep(200);
            GTKeyboardDriver::keyClick( Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new AllPopupChecker));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
}

GUI_TEST_CLASS_DEFINITION(test_4735) {
    //1. Open "_common_data/fasta/empty.fa" as msa.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "Simple overview"
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Show simple overview"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));
    GTGlobals::sleep();
    GTThread::waitForMainThread();
    QWidget* simple = GTWidget::findWidget(os, "msa_overview_area_simple");
    CHECK_SET_ERR(simple->isVisible(), "simple overveiw is not visiable");

    //Check empty simple overview gray color
    QPixmap pixmap = GTWidget::getPixmap(os, simple);
    QImage img = pixmap.toImage();
    QRgb rgb = img.pixel(simple->rect().topLeft() + QPoint(5, 5));
    QColor c(rgb);
    CHECK_SET_ERR(c.name() == "#ededed", "First check: simple overview has wrong color. Expected: #ededed, Found: " + c.name());
    //3. Add sequence eas.fastq to alignment
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/fastq/", "eas.fastq");
    GTUtilsDialog::waitForDialog(os, ob);
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //check not empty overview color
    pixmap = GTWidget::getPixmap(os, simple);
    img = pixmap.toImage();
    rgb = img.pixel(simple->rect().topLeft() + QPoint(5, 5));
    c = QColor(rgb);
    CHECK_SET_ERR(c.name() == "#c3ebc3", "simple overview has wrong color. Expected: #c3ebc3, Found: " + c.name());

    //4. Undo changes
    GTUtilsMsaEditor::undo(os);
    GTGlobals::sleep();
    GTThread::waitForMainThread();

    //Check empty simple overview gray color again
    pixmap = GTWidget::getPixmap(os, simple);
    img = pixmap.toImage();
    rgb = img.pixel(simple->rect().topLeft() + QPoint(5, 5));
    c = QColor(rgb);
    CHECK_SET_ERR(c.name() == "#ededed", "Second check: simple overview has wrong color. Expected: #ededed, Found: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(test_4764_1) {
    //1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Add some gaps
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(5, 5));
    GTKeyboardDriver::keyClick(' ');
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(5, 6));
    GTKeyboardDriver::keyClick(' ');
    GTKeyboardDriver::keyClick(' ');
    GTKeyboardDriver::keyClick(' ');

    //3. Select region with edited sequences, one of sequences should starts with gap
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(5, 5), QPoint(16, 9));

    //4. Copy this subalignment
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy formatted"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep(500);

    QMainWindow *mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor *editor = mw->findChild<MSAEditor *>();
    QWidget *nameListWidget = editor->getUI()->getEditorNameList();

    //5. Open conext menu by right clicking "Name list area". Paste this subaliment throu context menu {Copy/Paste->Paste}
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Paste"));
    GTWidget::click(os, nameListWidget, Qt::RightButton);
    GTGlobals::sleep(500);

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList(os).size() == 23, "Number of sequences should be 23");

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 18), QPoint(11, 27), GTGlobals::UseMouse);

    QString expectedClipboard = "-CTACTAATTCG\n---TTATTAATT\nTTGCTAATTCGA\nTTATTAATCCGG\nCTATTAATTCGA";

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy selection"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep(500);

    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == expectedClipboard, "expected test didn't equal to actual");

    //Expected state subalignment pasted correctly
    GTKeyboardUtils::copy(os);
    GTGlobals::sleep(200);
    clipboardText = GTClipboard::text(os);
    GTWidget::click(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));
    CHECK_SET_ERR(clipboardText == expectedClipboard, "expected test didn't equal to actual");
}

GUI_TEST_CLASS_DEFINITION(test_4764_2) {
    //1. Select one sequence in the alignment
    //2. Copy and paste it
    //3. Expected state : sequence added to the end of file
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4764", "4764.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor* editor = mw->findChild<MSAEditor*>();
    QWidget *sequenceAreaWidget = editor->getUI()->getSequenceArea();

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(15, 0), GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy selection"));
    GTWidget::click(os, sequenceAreaWidget, Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Paste"));
    GTWidget::click(os, sequenceAreaWidget, Qt::RightButton);
    GTGlobals::sleep();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 7, "Sequence count should be 7");
}

GUI_TEST_CLASS_DEFINITION(test_4764_3) {
    //1. Select sub - alignment with a few lines full of gaps
    //2. Copy and paste it
    //3. Expected state : gapped only sequences not added to msa
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4764", "4764.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    MSAEditor* editor = mw->findChild<MSAEditor*>();
    QWidget *sequenceAreaWidget = editor->getUI()->getSequenceArea();

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(3, 0), QPoint(5, 4));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy selection"));
    GTWidget::click(os, sequenceAreaWidget, Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Paste"));
    GTWidget::click(os, sequenceAreaWidget, Qt::RightButton);
    GTGlobals::sleep();

    CHECK_SET_ERR(GTUtilsMsaEditor::getSequencesCount(os) == 8, "Sequence count should be 7");
}

GUI_TEST_CLASS_DEFINITION(test_4782) {
//    1. Open "data/samples/genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Open "data/samples/Genbank/sars.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: a Sequence View for "sars.gb" is active.
    const QString sarsMdiTitle = "sars [s] NC_004718";
    QString activeMdiTitle = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(sarsMdiTitle == activeMdiTitle, QString("An incorrect MDI is active: expected '%1', got '%2'").arg(sarsMdiTitle).arg(activeMdiTitle));

//    3. Click "Build dotplot" button on the toolbar.
//    4. Click "OK" button in the opened dialog.
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "DotPlotDialog"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build dotplot");

//    Expected state: a Sequence View for "sars.gb" is active, it contains a dotplot and two sequences.
    CHECK_SET_ERR(sarsMdiTitle == activeMdiTitle, QString("An incorrect MDI is active: expected '%1', got '%2'").arg(sarsMdiTitle).arg(activeMdiTitle));
    int sequenceWidgetsNumber = GTUtilsSequenceView::getSeqWidgetsNumber(os);
    CHECK_SET_ERR(2 == sequenceWidgetsNumber, QString("Expected 2 sequence widgets, got %2").arg(sequenceWidgetsNumber));
    GTWidget::findWidget(os, "dotplot widget", GTUtilsMdi::activeWindow(os));

//    5. Activate the Sequence View for "murine.gb", that was opened on the file opening.
    const QString murineMdiTitle = "murine [s] NC_001363";
    GTUtilsMdi::activateWindow(os, murineMdiTitle);
    GTGlobals::sleep(500);

//    Expected state: active view is "murine.gb" sequence view (without dotplot).
    activeMdiTitle = GTUtilsMdi::activeWindowTitle(os);

    CHECK_SET_ERR(murineMdiTitle == activeMdiTitle, QString("An incorrect MDI is active: expected '%1', got '%2'").arg(murineMdiTitle).arg(activeMdiTitle));
    sequenceWidgetsNumber = GTUtilsSequenceView::getSeqWidgetsNumber(os);
    CHECK_SET_ERR(1 == sequenceWidgetsNumber, QString("Expected 1 sequence widget, got %2").arg(sequenceWidgetsNumber));
    GTGlobals::FindOptions findOptions(false);
    QWidget *dotplotWidget = GTWidget::findWidget(os, "dotplot widget", GTUtilsMdi::activeWindow(os), findOptions);
    CHECK_SET_ERR(NULL == dotplotWidget, "A dotplot widget unexpectedly found");

//    6. Select all documents in project. Press delete.
    GTWidget::click(os, GTUtilsProjectTreeView::getTreeView(os));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

//    Expected state: UGENE doesn't crash, all views are closed, all documents are removed from the project.
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::checkProject(os, GTUtilsProject::Empty);

    findOptions.matchPolicy = Qt::MatchContains;
    QWidget *sarsMdi = GTUtilsMdi::findWindow(os, sarsMdiTitle, findOptions);
    CHECK_SET_ERR(NULL == sarsMdi, "'sars.gb' Sequence View is not closed");

    QWidget *murineMdi = GTUtilsMdi::findWindow(os, murineMdiTitle, findOptions);
    CHECK_SET_ERR(NULL == murineMdi, "'murine.gb' Sequence View is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_4784_1) {
    QFile::copy(testDir + "_common_data/fasta/chr6.fa", sandBoxDir + "regression_test_4784_1.fa");

    //1. Click the menu Tools -> BLAST-> BLAST Search...
    //2. Select "_common_data/fasta/chr6,fa" as input file.
    //3. Press "Select a database file".
    //4. Choose "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr".
    //6. Press "Search".
    BlastAllSupportDialogFiller::Parameters settings;
    settings.runBlast = true;
    settings.withInputFile = true;
    settings.inputPath = sandBoxDir + "regression_test_4784_1.fa";
    settings.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "BLAST" << "BLAST search...");
    GTGlobals::sleep(3000);
    //5. Delete "chr6.fa" in file browser.
    //7. Click "No" in the appeared message box.
    //Expected result: An error notification appears - "A problem occurred during doing BLAST. The sequence is no more available".
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "was removed from"));
    GTUtilsNotifications::waitForNotification(os, true, "The sequence is no more available");
    QFile::remove(sandBoxDir + "regression_test_4784_1.fa");
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_4784_2) {
    QFile::copy(testDir + "_common_data/fasta/chr6.fa", sandBoxDir + "regression_test_4784_2.fa");

    //1. Open "_common_data/fasta/chr6.fa".
    GTFileDialog::openFile(os, sandBoxDir + "regression_test_4784_2.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Sequence context menu Analyze -> Query with local BLAST+...
    //3. Press "Select a database file".
    //4. Choose "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr".
    //6. Press "Search".
    BlastAllSupportDialogFiller::Parameters settings;
    settings.runBlast = true;
    settings.inputPath = sandBoxDir + "regression_test_4784_2.fa";
    settings.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Query with local BLAST+...", GTGlobals::UseMouse);

    //5. Delete "chr6.fa" in file browser.
    //7. Click "No" in the appeared message box.
    //Expected result: An error notification appears - "A problem occurred during doing BLAST. The sequence is no more available".
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "was removed from"));
    GTUtilsNotifications::waitForNotification(os, true, "The sequence is no more available");
    QFile::remove(sandBoxDir + "regression_test_4784_2.fa");
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_4784_3) {
    QFile::copy(testDir + "_common_data/fasta/chr6.fa", sandBoxDir + "regression_test_4784_3.fa");

    //1. Open "_common_data/fasta/chr6.fa".
    GTFileDialog::openFile(os, sandBoxDir + "regression_test_4784_3.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Sequence context menu Analyze -> Query with local BLAST...
    //3. Press "Select a database file".
    //4. Choose "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr".
    //5. Press "Search".
    BlastAllSupportDialogFiller::Parameters settings;
    settings.runBlast = true;
    settings.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Query with local BLAST...", GTGlobals::UseMouse);

    //6. Remove "chr6.fa" from project.
    //Expected result: An error notification appears - "A problem occurred during doing BLAST. The sequence is no more available".
    GTUtilsNotifications::waitForNotification(os, true, "The sequence is no more available");
    GTUtilsDocument::removeDocument(os, "regression_test_4784_3.fa");
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_4784_4) {
    QFile::copy(testDir + "_common_data/fasta/chr6.fa", sandBoxDir + "regression_test_4784_4.fa");

    //1. Click the menu Tools -> BLAST-> BLAST+ Search...
    //2. Select "_common_data/fasta/chr6" as input file.
    //3. Press "Select a database file".
    //4. Choose "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr".
    //5. Press "Search".
    BlastAllSupportDialogFiller::Parameters settings;
    settings.runBlast = true;
    settings.withInputFile = true;
    settings.inputPath = sandBoxDir + "regression_test_4784_4.fa";
    settings.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "BLAST" << "BLAST+ search...");

    //6. Remove "chr6.fa" from project.
    //Expected result: An error notification appears - "A problem occurred during doing BLAST. The sequence is no more available".
    GTUtilsNotifications::waitForNotification(os, true, "The sequence is no more available");
    GTUtilsDocument::removeDocument(os, "regression_test_4784_4.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4785_1) {
    //1. Open "COI.aln"
    //Expected state : a file browser appeared
    GTFile::copy(os, testDir + "_common_data/clustal/3000_sequences.aln", sandBoxDir + "test_4785.aln");
    GTFileDialog::openFile(os, sandBoxDir, "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //2. Use context menu { Align->Align profile to profile with MUSCLE }
    //3. Select any alignment and press "Ok"
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align profile to profile with MUSCLE"));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/CLUSTALW", "COI.aln"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep(500);

    //4. Delete "test_4785.aln"
    //Expected result : An error notification appears :
    //A problem occurred during aligning profile to profile with MUSCLE.The original alignment is no more available.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "was removed from"));
    GTUtilsNotifications::waitForNotification(os, true, "A problem occurred during aligning profile to profile with MUSCLE. The original alignment is no more available.");
    QFile::remove(sandBoxDir + "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4785_2) {
    //Difference with previous scenario is to delete alignment before alignment, selected in dialog box, will be loaded.
    //1. Open "1000_sequences.aln"
    //Expected state : a file browser appeared
    GTFile::copy(os, testDir + "_common_data/clustal/1000_sequences.aln", sandBoxDir + "test_4785.aln");
    GTFileDialog::openFile(os, sandBoxDir, "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //2. Use context menu { Align->Align profile to profile with MUSCLE }
    //3. Select any alignment and press "Ok"
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align profile to profile with MUSCLE"));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/clustal/","1000_sequences.aln"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep(500);

    //4. Delete "test_4785.aln"
    //Expected result : An error notification appears :
    //A problem occurred during aligning profile to profile with MUSCLE.The original alignment is no more available.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "was removed from"));
    GTUtilsNotifications::waitForNotification(os, true, "A problem occurred during aligning profile to profile with MUSCLE. The original alignment is no more available.");
    QFile::remove(sandBoxDir + "test_4785.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}


GUI_TEST_CLASS_DEFINITION(test_4795) {
    //    1. Open "_common_data/clustal/amino_ext.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/amino_ext.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QModelIndex aminoExtIdx = GTUtilsProjectTreeView::findIndex(os, "amino_ext");
    GTUtilsProjectTreeView::dragAndDrop(os, aminoExtIdx, GTWidget::findWidget(os, "msa_editor_sequence_area"));

    //    3. Open highlighting option panel tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

    //    Expected state: "UGENE" color scheme is selected, "No highlighting" highlight scheme is selected
    QComboBox* colorScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "colorScheme"));
    QComboBox* highlightingScheme = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "highlightingScheme"));
    GTComboBox::checkCurrentValue(os, colorScheme, "UGENE    ");
    GTComboBox::checkCurrentValue(os, highlightingScheme, "No highlighting    ");
}

GUI_TEST_CLASS_DEFINITION(test_4803_1) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Call context menu on tree tab.Press "Close tab" menu item
    //bug state : tab is colsed, but tree view is empty, and tree settings on options panel still present.Any change of tree settings causes crash
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Close tab"));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);
    GTGlobals::sleep();
    CHECK_SET_ERR(NULL == GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab", NULL, GTGlobals::FindOptions(false)), "Msa editor tree tab widget is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_4803_2) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Call context menu on tree tab. Check that there are not items "Add horizontal splitter" or "Add vertical splitter"
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Add horizontal splitter", PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Add vertical splitter", PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);
    //3. Call context menu on tree tab. Check that "Close other tabs" is disabled
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Close other tabs", PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4803_3) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Call context menu on tree tab.Press "Close other tabs" menu item
    //bug state : UGENE hangs up
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Close other tabs"));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);

    //5. Call context menu on tree tab. Check that "Close other tabs" item is disabled.
    //Bug state : both tabs are closed
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Close other tabs", PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);

    //6. Call context menu on tree tab. Press "Close tab".
    //Expected state: tree tab widget is closed
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Close tab"));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(NULL == GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab", NULL, GTGlobals::FindOptions(false)), "Msa editor tree tab widget is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_4803_4) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Build tree, display it with msa.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_4803/COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Call context menu on tree tab. Check that there are not items "Refresh tab" and "Refresh all tabs"
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Refresh tab", PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Refresh all tabs", PopupChecker::CheckOptions(PopupChecker::NotExists)));
    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "MsaEditorTreeTab"), 0, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4804_1) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_dna.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_dna.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add dna extended sequence throu context menu {Add->Sequence from file}
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard DNA\" to \"Extended DNA\"");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804/ext_dna.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep();

    //    3. Add dna extended sequence throu context menu {Add->Sequence from file}
    GTUtilsNotifications::waitForNotification(os, true, "from \"Extended DNA\" to \"Raw\"");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804/ext_rna.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4804_2) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_rna.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_rna.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add rna extended sequence throu menu {Actions->Add->Sequence from file}
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804/ext_rna.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard RNA\" to \"Extended RNA\"");
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Add" << "Sequence from file...");
    GTGlobals::sleep();

    //    3. Add dna extended sequence throu context menu {Add->Sequence from file}
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804/standard_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Extended RNA\" to \"Raw\"");
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Add" << "Sequence from file...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4804_3) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_amino.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "ext_dna.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add  extended amino sequence by drag and drop
    QModelIndex toDragNDrop = GTUtilsProjectTreeView::findIndex(os, "ext_amino_seq");
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsProjectTreeView::dragAndDrop(os, toDragNDrop, GTWidget::findWidget(os, "msa_editor_sequence_area"));
    GTGlobals::sleep();

    //    3. Add  extended DNA sequence by drag and drop
    toDragNDrop = GTUtilsProjectTreeView::findIndex(os, "ext_dna_seq");
    GTUtilsNotifications::waitForNotification(os, true, "from \"Extended amino acid\" to \"Raw\"");
    GTUtilsProjectTreeView::dragAndDrop(os, toDragNDrop, GTWidget::findWidget(os, "msa_editor_sequence_area"));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_4804_4) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_dna.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_dna.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Check what MAFFT tool is set up
    //    3. Use 'Align sequence to this alignment' toolbar button to align Extended rna sequence to alignment
    //Expected state: corresponding notification message has appeared
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_rna.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard DNA\" to \"Raw\"");

    QAbstractButton *align = GTAction::button(os, "Align sequence to this alignment");
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence to this alignment\" action not found");
    GTWidget::click(os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4804_5) {
    //    1. Check what MAFFT tool is not set up
    //    2. Open _common_data/scenarios/_regression/4804/standard_rna.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_rna.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    3. Use 'Align sequence to this alignment' toolbar button to align Extended rna sequence to alignment
    //Expected state: corresponding notification message has appeared
    GTUtilsExternalTools::removeTool(os, "MAFFT");
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_dna.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard RNA\" to \"Raw\". Use \"Undo\", if you'd like to restore the original alignment.");

    QAbstractButton *align = GTAction::button(os, "Align sequence to this alignment");
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence to this alignment\" action not found");
    GTWidget::click(os, align);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4804_6) {
    //    1. Open _common_data/scenarios/_regression/4804/standard_dna.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_dna.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Add amino extended sequence throu context menu {Add->Sequence from file}
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804/ext_amino.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_LOAD << "Sequence from file"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTGlobals::sleep();

    //   4. Undo changes
    GTUtilsMsaEditor::undo(os);
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //   5. Redo changes and check appearing notifications
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard DNA\" to \"Raw\". Use \"Undo\", if you'd like to restore the original alignment.");
    GTUtilsMsaEditor::redo(os);
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_1) {
    //Add sequence from curent project by main menu
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "ext_dna.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ext_dna.fa", "ext_dna_seq"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Raw\"");
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Add" << "Sequence from current project...");
}

GUI_TEST_CLASS_DEFINITION(test_4833_2) {
    //Add sequence from curent project by context menu
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "ext_amino.fa", "ext_amino_seq"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "MSAE_MENU_LOAD_SEQ" << "Sequence from current project"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_3) {
    //align sequences to profile by MUSCLE by main menu
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align sequences to profile with MUSCLE", GTGlobals::UseMouse));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_4) {
    //align sequences to profile by MUSCLE by context menu
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align sequences to profile with MUSCLE", GTGlobals::UseMouse));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_5) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Align sequences to profile with MUSCLE", GTGlobals::UseMouse));
    QAbstractButton* button = GTAction::button(os, "Align");
    GTWidget::click(os, button);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_6) {
    //align sequences to profile by MUSCLE by main menu
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align profile to profile with MUSCLE", GTGlobals::UseMouse));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_7) {
    //align sequences to profile by MUSCLE by context menu
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align profile to profile with MUSCLE", GTGlobals::UseMouse));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4833_8) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/4804", "standard_amino.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/4804", "ext_amino.fa"));
    GTUtilsNotifications::waitForNotification(os, true, "from \"Standard amino acid\" to \"Extended amino acid\"");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Align profile to profile with MUSCLE", GTGlobals::UseMouse));
    QAbstractButton* button = GTAction::button(os, "Align");
    GTWidget::click(os, button);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4839_1) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: document are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex(os, "COI.aln");
    QWidget *msaView = GTUtilsMdi::activeWindow(os);
    CHECK(NULL != msaView, );

    //2. Select some sequences on sequence view.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(2, 4), QPoint(2, 6));
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    //3. Call context menu on the name list area, select the {Edit -> Remove sequence} menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "Remove sequence"));
    GTMouseDriver::click(Qt::RightButton);
    //Expected state: the sequences are removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 3, "The number of sequences remained unchanged.");
}

GUI_TEST_CLASS_DEFINITION(test_4839_2) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: document are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex(os, "COI.aln");
    QWidget *msaView = GTUtilsMdi::activeWindow(os);
    CHECK(NULL != msaView, );

    //2. Select some sequences on sequence view.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(2, 4), QPoint(2, 6));
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    //3. Use main menu item {Actions -> Edit -> Remove sequence}.
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Edit" << "Remove sequence(s)");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    //Expected state: the sequences are removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 3, "The number of sequences remained unchanged.");
}

GUI_TEST_CLASS_DEFINITION(test_4852) {
    //1. Open _common_data/genome_aligner/chrY.fa
    GTFileDialog::openFile(os, testDir + "_common_data/genome_aligner", "chrY.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Run Find repeats with default parameters
    QDir().mkpath(sandBoxDir + "test_4852");
    GTUtilsDialog::waitForDialog(os, new FindRepeatsDialogFiller(os, sandBoxDir + "test_4852"));
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find repeats"));

    //3. Delete chrY.fa document from project view
    GTUtilsProjectTreeView::click(os, "chrY.fa");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    //Expected state: 'find repeats' task cancelled
    GTGlobals::sleep(1000);
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount(os) == 0, "Running task count should be 0");
}

GUI_TEST_CLASS_DEFINITION(test_4860) {
    // 1. Open murine.gb
    // 2. Open Find Pattern tab
    // 3. Input pattern
    // 4. Click Next a few times
    // Expected state: the results are selected one by one from left to right, no random selection

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAA");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* next = GTWidget::findWidget(os, "nextPushButton");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection(os).size() == 1, "Incorrect selection: selected region should be only one");
    int startPosPrev = GTUtilsSequenceView::getSelection(os).first().startPos;
    int startPosNext = -1;
    for (int i = 0; i < 10; i++) {
        GTWidget::click(os, next);

        CHECK_SET_ERR(GTUtilsSequenceView::getSelection(os).size() == 1, "Incorrect selection: selected region should be only one");
        startPosNext = GTUtilsSequenceView::getSelection(os).first().startPos;

        CHECK_SET_ERR(startPosPrev < startPosNext, "Search results are disordered");
        startPosPrev = startPosNext;
    }
}

GUI_TEST_CLASS_DEFINITION(test_4871) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Open OP and select pairwice alignment tab, select sequences to align, set "in new window" parameter to "false"
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);
    //3. Set pairwise algorithm "Smith-Waterman"
    GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(os, "Smith-Waterman");
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Phaneroptera_falcata");
    GTGlobals::sleep(500);
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Isophya_altaica_EF540820");

    QWidget* widget = GTWidget::findWidget(os, "outputContainerWidget");
    CHECK_SET_ERR(widget != NULL, QString("%1 not found").arg("outputContainerWidget"));
    if (widget->isHidden()) {
        GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));
    }
    QCheckBox* inNewWindowCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "inNewWindowCheckBox"));
    CHECK_SET_ERR(inNewWindowCheckBox != NULL, "inNewWindowCheckBox not found");
    GTCheckBox::setChecked(os, inNewWindowCheckBox, false);
    GTWidget::click(os, GTWidget::findWidget(os, "alignButton"));

    //4. Undo changes
    GTKeyboardDriver::keyClick( 'z', Qt::ControlModifier);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state "Undo" button is disabled
    QAbstractButton *undo = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Button should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_4885_1) {
//    1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Wait until overview finishes rendering.
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Doubleclick the first symbol of the first sequence.
//    Expected state: overview doesn't start recalculation.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::doubleClick();

    QWidget *graphOverview = GTUtilsMsaEditor::getGraphOverview(os);
    CHECK_SET_ERR(NULL != graphOverview, "Graph overview is NULL");
    const QColor actualColor = GTUtilsMsaEditor::getGraphOverviewPixelColor(os, QPoint(graphOverview->width() / 2, 2));
    CHECK_SET_ERR("#ffffff" == actualColor.name(), QString("Incorrect color of the graph overview ('%1'). Does it render now?").arg(actualColor.name()));
}

GUI_TEST_CLASS_DEFINITION(test_4885_2) {
//    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Build a tree. The result tree should be synchronised with the msa.
    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_4885_2.nwk");

//    3. Doubleclick the first symbol of the first sequence.
//    Expected state: UGENE doesn't ask about confirmation of the modification.
    GTUtilsDialog::waitForDialogWhichMustNotBeRun(os, new MessageBoxDialogFiller(os, QMessageBox::Cancel, "The alignment has been modified"));

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::doubleClick();

    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_4885_3) {
//    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFile::copy(os, dataDir + "samples/CLUSTALW/COI.aln", sandBoxDir + "test_4885_3.aln");
    GTFileDialog::openFile(os, sandBoxDir + "test_4885_3.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Build a tree. The result tree should be synchronised with the msa.
    GTUtilsMsaEditor::buildPhylogeneticTree(os, sandBoxDir + "test_4885_3.nwk");

//    3. Save the project.
    GTUtilsProject::saveProjectAs(os, sandBoxDir + "test_4885_3.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    4. Close the project.
    GTUtilsProject::closeProject(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    5. Open the saved project.
//    Expected state: there are two unloaded documents in a project: COI.aln and its tree.
    GTFileDialog::openFile(os, sandBoxDir + "test_4885_3.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    6. Doubleclick the COI.aln document.
    GTUtilsProjectTreeView::doubleClickItem(os, "test_4885_3.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: MSA Editor opens, there is an attached tree in it. The msa object in the project view is not modified.
    GTUtilsProjectTreeView::itemModificationCheck(os, "test_4885_3", false);
}

GUI_TEST_CLASS_DEFINITION(test_4886) {
    GTFileDialog::openFile(os, dataDir + "samples/SCF/", "90-JRI-07.scf");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer lt;
    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "90-JRI-07.scf"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_CHROMATOGRAM));
    GTUtilsDialog::waitForDialog(os, new ExportChromatogramFiller(os, testDir + "_common_data/scenarios/sandbox/", "90-JRI-07.scf",
        ExportChromatogramFiller::SCF, false, false, true));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(5000);

    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(test_4908) {
    //1. Open s file with multiple sequences
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 199950);
    GTKeyboardUtils::copy(os);

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "seq5.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "seq2.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "SEQUENCE_WITH_A_ENTRY_2", Qt::LeftButton);

    MWMDIWindow *mdiWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    QPoint detPos = mdiWindow->mapToGlobal(mdiWindow->rect().center());

    GTMouseDriver::dragAndDrop(GTMouseDriver::getMousePosition(), detPos);

    GTGlobals::sleep();

    GTUtilsSequenceView::enableEditingMode(os, true, 0);
    GTUtilsSequenceView::enableEditingMode(os, true, 1);
    GTGlobals::sleep();

    //2. Select the first sequence and add data to the clipboard
    DetView *firstSeqWidget = GTUtilsSequenceView::getDetViewByNumber(os, 0);
    GTWidget::click(os, firstSeqWidget);
    GTKeyboardUtils::paste(os);

    //3. While the data is been pasted, select the second sequence
    DetView *secondSeqWidget = GTUtilsSequenceView::getDetViewByNumber(os, 1);
    GTWidget::click(os, secondSeqWidget);
    GTGlobals::sleep(5000);

    //4. Check if the data is pasted to the first sequence
    int len = GTUtilsSequenceView::getSequenceAsString(os, 0).length();
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
    GTUtilsDialog::waitForDialog(os, new SelectDocumentFormatDialogFiller(os));
    GTFileDialog::openFile(os, dataDir + "samples/Swiss-Prot", "P16152.txt");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class CheckWordSizeScenario : public CustomScenario {
    public:
        CheckWordSizeScenario() {}
        void run(HI::GUITestOpStatus &os) {
            QComboBox* comboAlg = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "dataBase"));
            CHECK_SET_ERR(comboAlg != NULL, "dataBase not found!");
            CHECK_SET_ERR(comboAlg->currentText() == "blastp", QString("Value of dataBase not equal blastp, it has other default value: %1!").arg(comboAlg->currentText()));

            GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "optionsTab"), 1);
            GTGlobals::sleep(200);
            QComboBox* combo = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "wordSizeComboBox"));
            CHECK_SET_ERR(combo != NULL, "wordSizeComboBox not found!");
            CHECK_SET_ERR(combo->currentText() == "6", QString("Value of wordSizeComboBox not equal 6, it has other default value: %1!").arg(combo->currentText()));

            GTKeyboardDriver::keyClick( Qt::Key_Escape);
        }
    };

    CheckWordSizeScenario *scenario = new CheckWordSizeScenario();
    RemoteBLASTDialogFiller *filler = new RemoteBLASTDialogFiller(os, scenario);

    GTUtilsDialog::waitForDialog(os, filler);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
        << "Query NCBI BLAST database"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
}

GUI_TEST_CLASS_DEFINITION(test_4918) {
    //1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //2. Select "Align->Align profile to profile with MUSCLE" in the MSAEditor context menu
    //Expected state : "Select file with alignment" dialog is appeared
    //3. Select file "_common_data\clustal\1000_sequences.aln"
    //Current state : UGENE crashes
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align profile to profile with MUSCLE"));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/clustal", "1000_sequences.aln"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4918_1) {
    //1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //2. Select "Align->Align profile to profile with MUSCLE" in the MSAEditor context menu
    //Expected state : "Select file with alignment" dialog is appeared
    //3. Select file "_common_data\genbank\DNA.gen"
    //Current state : UGENE crashes
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align profile to profile with MUSCLE"));
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/genbank", "DNA.gen"));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_4934) {
    //1. Open samples/CLUSTALW/ty3.aln.gz
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Align with Kalign
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "align_with_kalign", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new KalignDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    //3. while aligning lock document for editing
    GTUtilsDocument::lockDocument(os, "ty3.aln.gz");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    //4. Unlock document after alignment finished
    GTUtilsDocument::unlockDocument(os, "ty3.aln.gz");

    //5. Align with Kalign again
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "align_with_kalign", GTGlobals::UseMouse));
    GTUtilsDialog::waitForDialog(os, new KalignDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, l, "Object 'ty3.aln.gz' removed");
    int errorNum = GTUtilsLog::getErrors(os, l).size();
    CHECK_SET_ERR(errorNum==1, QString("Too many errors in log: %1").arg(errorNum));
}

GUI_TEST_CLASS_DEFINITION(test_4936) {
    GTLogTracer logTracer;

//    1. Open "data/samples/Swiss-Prot/D0VTW9.txt".
    GTFile::copy(os, dataDir + "samples/Swiss-Prot/D0VTW9.txt", sandBoxDir + "test_4936.sw");
    GTFileDialog::openFile(os, sandBoxDir + "test_4936.sw");

//    2. Edit the file directly.
//    Expected state: UGENE offers to reload the file.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes, "Do you want to reload it?"));

    QByteArray data = GTFile::readAll(os, sandBoxDir + "test_4936.sw");
    data.replace("D0VTW9_9INFA", "00VTW9_9INFA");

    QFile file(sandBoxDir + "test_4936.sw");
    file.open(QFile::WriteOnly);
    file.write(data);
    file.close();

    GTGlobals::sleep(5000);

//    3. Accept the offer.
//    Expected state: the document is successfully reloaded, there are no errors in the log.
    GTUtilsProjectTreeView::findIndex(os, "00VTW9_9INFA");
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_4938) {
    // 1. Open murine.gb
    // 2. Open Annotations Highlighting tab
    // 3. Remove all annotation groups one by one
    // Expected state: annotations table from Options Panel is not visible, no errors
    // 4. Add annotation
    // Expected state: annotation table is visible

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    QWidget* annTree = GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(annTree != NULL, "Cannot find OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is not visible")

    GTUtilsAnnotationsTreeView::deleteItem(os, "CDS  (0, 4)");
    GTUtilsAnnotationsTreeView::deleteItem(os, "misc_feature  (0, 2)");
    GTUtilsAnnotationsTreeView::deleteItem(os, "comment  (0, 1)");
    GTUtilsAnnotationsTreeView::deleteItem(os, "source  (0, 1)");
    GTGlobals::sleep();
    CHECK_SET_ERR(!annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is still visible")

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group", "feature", "50..60"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Add" << "New annotation...");
    CHECK_SET_ERR(annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is not visible")
}

GUI_TEST_CLASS_DEFINITION(test_4938_1) {
    // 1. Open murine.gb
    // 2. Open Annotations Highlighting tab
    // 3. Click "Show all annotations"
    // 4. Remove annotation group, e.g. CDS
    // Expected state: CDS is still peresent in annotation table on Options Panel

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    QTreeWidget* annTree = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT_TREE"));
    CHECK_SET_ERR(annTree != NULL, "Cannot find OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(annTree->isVisible(), "OP_ANNOT_HIGHLIGHT_TREE is not visible");
    GTWidget::click(os, GTWidget::findWidget(os, "show_all_annotation_types"));
    CHECK_SET_ERR(!annTree->findItems("CDS", Qt::MatchExactly).isEmpty(), "Cannot find CDS item in OP_ANNOT_HIGHLIGHT_TREE");

    GTUtilsAnnotationsTreeView::deleteItem(os, "CDS  (0, 4)");
    GTGlobals::sleep();
    CHECK_SET_ERR(!annTree->findItems("CDS", Qt::MatchExactly).isEmpty(), "CDS item is missing in OP_ANNOT_HIGHLIGHT_TREE");
}

GUI_TEST_CLASS_DEFINITION(test_4965) {
    // 1. Open any assembly
    // 2. Right button click on the assembly object in the project view
    // 3. Export/Import --> Export object
    // Expected state: 'Compress' checkbox is disabled for all assembly formats

    GTFileDialog::openFile(os, testDir + "_common_data/ugenedb/toy.sam.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class CheckCompressFiller : public Filler {
    public:
        CheckCompressFiller(GUITestOpStatus &os)
            : Filler(os, "ExportDocumentDialog") {}
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog not found");

            QComboBox *comboBox = dialog->findChild<QComboBox*>("formatCombo");
            CHECK_SET_ERR(comboBox != NULL, "ComboBox not found");

            QCheckBox *compressCheckBox = dialog->findChild<QCheckBox*>(QString::fromUtf8("compressCheck"));
            CHECK_SET_ERR(compressCheckBox != NULL, "Check box not found");

            QStringList checkFormats;
            checkFormats << "BAM" << "SAM" << "UGENE Database";
            foreach (const QString& format, checkFormats) {
                GTComboBox::setIndexWithText(os, comboBox, format, true);
                CHECK_SET_ERR(!compressCheckBox->isEnabled(),
                              QString("Compress checkbox is unexpectedly enabled for '%1' format!").arg(format));
            }

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }

    };

    GTUtilsDialog::waitForDialog(os, new CheckCompressFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import" << "Export object..."));
    GTUtilsProjectTreeView::click(os, "ref", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_4966) {
    //GTLogTracer l;
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Open sample "Align sequences with MUSCLE"
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    //    3. Align "_common_data\clustal\big.aln"
    GTUtilsWorkflowDesigner::click(os, "Read alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/clustal/big.aln");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state:
    //    Workflow finished with error "Not enough resources for the task"
    GTUtilsWorkflowDesigner::checkErrorList(os, "Not enough resources for the task");
}

GUI_TEST_CLASS_DEFINITION(test_4969_1) {
    //1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    //2. Open Start Page.
    GTUtilsStartPage::openStartPage(os);
    //3. Click the main menu item: "File" -> "Recent files" -> ".../samples/Genbank/murine.gb".
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Recent files" << QFileInfo(dataDir + "samples/Genbank/murine.gb").absoluteFilePath());
    //Expected: sequence view is activated.
    QString title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_4969_2) {
    //1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    //2. Unload the sequence.
    GTUtilsDocument::unloadDocument(os, "murine.gb");
    //3. Click "murine.gb" on Start Page.
    GTUtilsStartPage::clickResentDocument(os, "murine.gb");
    //Expected: The file is loaded, the view is opened.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded(os, "murine.gb"), "The file is not loaded");
    QString title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_4983) {
    //1. Open file : "data/samples/PDB/1CF7.PDB"
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    //2. Use context menu on 3D view : {"Coloring schemes"->"Molecular chains"}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Coloring Scheme" << "Molecular Chains"));
    QWidget *widget3d = GTWidget::findWidget(os, "1-1CF7");
    GTWidget::click(os, widget3d, Qt::RightButton);
    GTGlobals::sleep();

    //Expected: the color scheme is changed without errors.
    GTUtilsLog::check(os, l);
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

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, "Ca20Chr1 features"), GTWidget::findWidget(os, "render_area_NC_001363"));
    GTGlobals::sleep();

    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ExportSequenceOfSelectedAnnotationsFiller(os, sandBoxDir + "test_4986.fa", ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                                   ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_sequence_of_selected_annotations"));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, GTUtilsAnnotationsTreeView::findItem(os, "5_prime_UTR_intron"));

    GTThread::waitForMainThread();
    CHECK_SET_ERR(l.hasError(), "There is no error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_4990) {
    //1. Open file "_common_data/clustal/big.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    GTWidget::click(os, GTAction::button(os, "Show overview"));

    //2. Open "Export Consensus" OP tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    //3. Press "Export" button 3x times
    for(int i = 0; i < 24; i++){
        GTWidget::click(os, GTWidget::findWidget(os, "exportBtn"));
    }

    //4. Remove "big.aln" document
    GTUtilsDocument::removeDocument(os, "big.aln");
    qDebug()<<QString("");
}

} // namespace GUITest_regression_scenarios

} // namespace U2
