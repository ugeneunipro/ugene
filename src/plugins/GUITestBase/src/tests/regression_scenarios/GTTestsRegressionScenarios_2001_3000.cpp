/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWebView.h>
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
#include <QProgressBar>
#include <QPushButton>
#include <QTableWidget>
#include <QWizard>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/Dashboard.h>

#include <U2Gui/ProjectViewModel.h>
#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/AnnotationsTreeView.h>
#include <U2View/AssemblyNavigationWidget.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTDatabaseConfig.h"
#include "GTTestsRegressionScenarios_2001_3000.h"
#include "GTUtilsAnnotationsHighlightingTreeView.h"
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
#include "GTUtilsPhyTree.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsSharedDatabaseDocument.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/qt/EscapeClicker.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/BuildIndexDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ConvertAssemblyToSAMDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindRepeatsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindTandemsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportAPRFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SharedConnectionsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/util/RenameSequenceFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DeleteGapsDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExportHighlightedDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/annotator/FindAnnotationCollocationsDialogFiller.h"
#include "runnables/ugene/plugins/biostruct3d_view/StructuralAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/cap3/CAP3SupportDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportBlastResultDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2SequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ImportAnnotationsToCsvFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastAllSupportDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/ClustalOSupportRunDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/FormatDBDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/RemoteBLASTDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SpadesGenomeAssemblyDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/TCoffeeDailogFiller.h"
#include "runnables/ugene/plugins/weight_matrix/PwmBuildDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/AliasesDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithCommandLineToolFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithScriptDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DashboardsManagerDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DefaultWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/MAFFT/MAFFTSupportRunDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/clustalw/ClustalWDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/hmm3/UHMM3PhmmerDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/ConvertAceToSqliteDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentProviderSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/NCBISearchDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SelectDocumentFormatDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {

GUI_TEST_CLASS_DEFINITION(test_2006) {
    const int MSA_WIDTH = 30;
    const int MSA_HEIGHT = 3;

    // 1. Open "data/samples/CLUSTAL/COI.aln" and save it's part to a string
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(MSA_WIDTH, MSA_HEIGHT));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString initialMsaContent = GTClipboard::text(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select the second symbol in the first line
    const QPoint initialSelectionPos(1, 0);
    GTUtilsMSAEditorSequenceArea::click(os, initialSelectionPos);
    GTGlobals::sleep(200);

    // 3. Drag it to the first symbol in the first line
    const QPoint mouseDragPosition(1, 0);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition);
    GTMouseDriver::press();
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition + QPoint(0, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTGlobals::sleep(200);

    // 4. Check that the content has not been changed
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(MSA_WIDTH, MSA_HEIGHT));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");

    // 5. Check that "Undo" and "Redo" buttons are disabled
    const QAbstractButton *undo = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button is unexpectedly enabled");
    const QAbstractButton *redo = GTAction::button(os, "msa_action_redo");
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2007) {
    // 1. Open "_common_data/scenarios/msa/ma_empty_line.aln (and save it to string)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma_empty_line.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 4));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString initialMsaContent = GTClipboard::text(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select the fourth column of the second, third and fourth lines
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(3, 1), QPoint(3, 3));
    GTGlobals::sleep(200);

    // 3. Drag it to the left.
    const QPoint mouseDragPosition(3, 2);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition);
    GTMouseDriver::press();
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition - QPoint(1, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTGlobals::sleep(200);

    // Expected state: nothing happens
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 4));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has changed unexpectedly!");
}

GUI_TEST_CLASS_DEFINITION(test_2009) {
    //    1) Open Settings/Preferences/External tools
    //    2) Set correct BWA or any other tool path (preferably just executable)
    class BWAInactivation : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    3) Clear set path
            AppSettingsDialogFiller::clearToolPath(os, "BWA");

            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new BWAInactivation()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");
    //    4) Look at UGENE log
    GTUtilsLog::check(os, l);
    //    Expected state: UGENE doesn't write any error to log
}

GUI_TEST_CLASS_DEFINITION(test_2012) {
    //1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open the "General" tab on the options panel.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_GENERAL"));
    GTGlobals::sleep(500);
    QLineEdit *sequencelineEdit = (QLineEdit *)GTWidget::findWidget(os, "sequenceLineEdit");
    CHECK(NULL != sequencelineEdit, );

    GTLineEdit::setText(os, sequencelineEdit, "m");
    GTGlobals::sleep(500);

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);

    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTGlobals::sleep(500);

    QString selectedSequence = sequencelineEdit->text();
    CHECK("Mecopoda_elongata__Sumatra_" == selectedSequence, );

    //3. Set focus to the reference sequence name line edit and enter 'm' into it.
}

GUI_TEST_CLASS_DEFINITION(test_2021_1) {
    // 1. Open document "ma.aln" and save it to string
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString initialMsaContent = GTClipboard::text(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a character in the sequence area
    const QPoint initialSelectionPos(10, 10);
    GTUtilsMSAEditorSequenceArea::click(os, initialSelectionPos);
    GTGlobals::sleep(200);

    // 3. Add gaps by pressing "Space" key
    const int totalShiftCount = 3;
    int shiftCounter = 0;
    QPoint currentSelection = initialSelectionPos;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        GTGlobals::sleep(200);
    }
    currentSelection.rx() += totalShiftCount;

    // 4. Remove gaps with "Backspace" key
    shiftCounter = 0;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
        GTGlobals::sleep(200);
        currentSelection.rx() -= 1;
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(currentSelection, currentSelection));
    }

    // 5. Check that alignment content has returned to initial state
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2021_2) {
    // 1. Open document "ma.aln" and save it to string
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString initialMsaContent = GTClipboard::text(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a region in the sequence area
    const QPoint initialSelectionLeftTop(5, 5);
    const QPoint initialSelectionRightBottom(11, 10);
    const int selectionWidth = initialSelectionRightBottom.x() - initialSelectionLeftTop.x() + 1;
    GTUtilsMSAEditorSequenceArea::selectArea(os, initialSelectionLeftTop, initialSelectionRightBottom);
    GTGlobals::sleep(200);

    // 3. Add gaps by pressing "Space" key
    const int totalShiftCount = 3;
    int shiftCounter = 0;
    QPoint currentSelectionLeftTop = initialSelectionLeftTop;
    QPoint currentSelectionRightBottom = initialSelectionRightBottom;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        GTGlobals::sleep(200);
    }
    currentSelectionLeftTop.rx() += totalShiftCount * selectionWidth;
    currentSelectionRightBottom.rx() += totalShiftCount * selectionWidth;
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(currentSelectionLeftTop, currentSelectionRightBottom));

    // 4. Remove gaps with "Backspace" key
    shiftCounter = 0;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
        GTGlobals::sleep(200);
        currentSelectionLeftTop.rx() -= selectionWidth;
        currentSelectionRightBottom.rx() -= selectionWidth;
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(currentSelectionLeftTop, currentSelectionRightBottom));
    }

    // 5. Check that alignment content has returned to initial state
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2021_3) {
    // 1. Open document "ma2_gap_8_col.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select a column in the sequence area
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::click();
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(os, 9);
    GTGlobals::sleep(200);

    // 3. Remove gap columns by pressing "Backspace" key
    const int totalShiftCount = 6;
    int shiftCounter = 0;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
        GTGlobals::sleep(200);
    }

    // 4. Check that all the empty columns were removed
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(14, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("AAGCTTCTTTTAA--\nAAGTTACTAA-----\nTAG---TTATTAA--\nAAGC---TATTAA--\n"
                  "TAGTTATTAA-----\nTAGTTATTAA-----\nTAGTTATTAA-----\nAAGCTTT---TAA--\n"
                  "A--AGAATAATTA--\nAAGCTTTTAA-----" == finalMsaContent,
                  "Unexpected MSA content has occurred\n expected: \nAAGCTTCTTTTAA--\nAAGTTACTAA-----\nTAG---TTATTAA--\nAAGC---TATTAA--\n"
                  "TAGTTATTAA-----\nTAGTTATTAA-----\nTAGTTATTAA-----\nAAGCTTT---TAA--\n"
                  "A--AGAATAATTA--\nAAGCTTTTAA-----\n actual: \n" +
                      finalMsaContent);
}

GUI_TEST_CLASS_DEFINITION(test_2021_4) {
    // 1. Open document "ma2_gap_8_col.aln" and save it to string
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(20, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString initialMsaContent = GTClipboard::text(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a region of trailing gaps
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(17, 4), QPoint(19, 6));

    // 3. Press "Backspace"
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(200);

    // 4. Check that MSA does not changed
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(20, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");

    // 5. Check that "Undo" and "Redo" buttons are disabled
    const QAbstractButton *undo = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button is unexpectedly enabled");
    const QAbstractButton *redo = GTAction::button(os, "msa_action_redo");
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2021_5) {
    // 1. Open document "ma2_gap_8_col.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select a column in the sequence area
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(9, 0), QPoint(16, 9));

    // 3. Remove gap columns by pressing "Backspace" key
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(200);

    // 4. Check that all the empty columns were removed
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(14, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("AAGCTTCTTTTAA--\nAAGTTACTAA-----\nTAG---TTATTAA--\nAAGC---TATTAA--\n"
                  "TAGTTATTAA-----\nTAGTTATTAA-----\nTAGTTATTAA-----\nAAGCTTT---TAA--\n"
                  "A--AGAATAATTA--\nAAGCTTTTAA-----" == finalMsaContent,
                  "Unexpected MSA content has occurred");
}

GUI_TEST_CLASS_DEFINITION(test_2021_6) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    if (GTUtilsProjectTreeView::isVisible(os)) {
        GTUtilsProjectTreeView::toggleView(os);
    }

    //2. Set cursor to the position 45 of the first line (after gaps).
    //const QPoint initialSelectionPos(44, 0);
    //GTUtilsMSAEditorSequenceArea::click(os, initialSelectionPos);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(44, 0), QPoint(44, 0));

    //3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(200);

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(43, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);

    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("TAAGACTTCTAATTCGAGCCGAATTAGGTCAACCAGGATAC--C" == finalMsaContent,
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2021_7) {
    // 1. Open test/_common_data/scenarios/msa/ma2_gap_col.aln.
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select the 8 and 9 of the third line (two symbols after gaps).
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 2), QPoint(8, 2));

    //3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep();

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 2), QPoint(13, 2));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep();
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("TAG--TTATTAA--" == finalMsaContent,
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2021_8) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    if (GTUtilsProjectTreeView::isVisible(os)) {
        GTUtilsProjectTreeView::toggleView(os);
    }

    //2. Select the 45 and 46 of the second line (two symbols after gaps).
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(44, 1), QPoint(45, 1));

    //3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep();

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 1), QPoint(44, 1));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep();
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("TAAGCTTACTAATCCGGGCCGAATTAGGTCAACCTGGTTAT-CTA" == finalMsaContent,
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2021_9) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    if (GTUtilsProjectTreeView::isVisible(os)) {
        GTUtilsProjectTreeView::toggleView(os);
    }

    //2. Select the 45 and 46 of the second line (two symbols after gaps).
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(44, 2), QPoint(46, 2));
    GTGlobals::sleep();

    //3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep();

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 2), QPoint(46, 2));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep();
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("TTAGTTTATTAATTCGAGCTGAACTAGGTCAACCAGGCTATTTAATT" == finalMsaContent,
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2026) {
    // 1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select Montana_montana in name list
    GTUtilsMSAEditorSequenceArea::selectSequence(os, QString("Montana_montana"));

    // 3. Press shift, click down_arrow 4 times. Release shift.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 4; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        GTGlobals::sleep(500);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: 5 sequences are selected
    CHECK_SET_ERR(5 == GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(os),
                  QString("Unexpected number of selected sequences1. Got %1, Expected %2").arg(GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(os)).arg(5));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(os, QString("Montana_montana")),
                  "Expected sequence is not selected");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(os, QString("Zychia_baranovi")),
                  "Expected sequence is not selected");

    // 4. Release shift. Press shift again and click down arrow
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: 6 sequences selected
    CHECK_SET_ERR(6 == GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(os),
                  QString("Unexpected number of selected sequences2. Got %1, Expected %2").arg(GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(os)).arg(6));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(os, QString("Montana_montana")),
                  "Expected sequence is not selected");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(os, QString("Zychia_baranovi")),
                  "Expected sequence is not selected");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(os, QString("Tettigonia_viridissima")),
                  "Expected sequence is not selected");
}

GUI_TEST_CLASS_DEFINITION(test_2030) {
    // 1. Open {_common_data/scenarios/msa/ma_one_line.aln}
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma_one_line.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Set cursor to the end of the line
    const QPoint endLinePos(11, 0);
    GTUtilsMSAEditorSequenceArea::click(os, endLinePos);
    GTGlobals::sleep(200);

    // 3. Insert four gaps with SPACE.
    for (int i = 0; i < 4; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        GTGlobals::sleep(500);
    }

    // 4. Set cursor to the beginning of the line
    const QPoint begLinePos(0, 0);
    GTUtilsMSAEditorSequenceArea::click(os, begLinePos);
    GTGlobals::sleep(200);

    // 5. Delete one symbol with DELETE
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(200);

    // Expected state: line length is 15.
    CHECK_SET_ERR(15 == GTUtilsMSAEditorSequenceArea::getLength(os),
                  "Unexpected MSA length!");
}

GUI_TEST_CLASS_DEFINITION(test_2032) {
    // 1. Open {_common_data/fasta/abcd.fa} as separate sequences
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(os, testDir, "_common_data/fasta/abcd.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected result: order of sequences in the project view is {d, a, c, b}
    QModelIndex documentItem = GTUtilsProjectTreeView::findIndex(os, "abcd.fa");

    CHECK_SET_ERR(documentItem.child(0, 0).data() == "[s] a", "1. Unexpected name of the object in the project view!");
    CHECK_SET_ERR(documentItem.child(1, 0).data() == "[s] b", "2. Unexpected name of the object in the project view!");
    CHECK_SET_ERR(documentItem.child(2, 0).data() == "[s] c", "3. Unexpected name of the object in the project view!");
    CHECK_SET_ERR(documentItem.child(3, 0).data() == "[s] d", "4. Unexpected name of the object in the project view!");

    // Expected result: order of sequences in the sequences view is {d, a, c, b}
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    ADVSingleSequenceWidget *seq0 = dynamic_cast<ADVSingleSequenceWidget *>(GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    ADVSingleSequenceWidget *seq1 = dynamic_cast<ADVSingleSequenceWidget *>(GTWidget::findWidget(os, "ADV_single_sequence_widget_1"));
    ADVSingleSequenceWidget *seq2 = dynamic_cast<ADVSingleSequenceWidget *>(GTWidget::findWidget(os, "ADV_single_sequence_widget_2"));
    ADVSingleSequenceWidget *seq3 = dynamic_cast<ADVSingleSequenceWidget *>(GTWidget::findWidget(os, "ADV_single_sequence_widget_3"));

    CHECK_SET_ERR(seq0 != NULL, "1. Failed to find a sequence widget!");
    CHECK_SET_ERR(seq1 != NULL, "2. Failed to find a sequence widget!");
    CHECK_SET_ERR(seq2 != NULL, "3. Failed to find a sequence widget!");
    CHECK_SET_ERR(seq3 != NULL, "4. Failed to find a sequence widget!");

    CHECK_SET_ERR(seq0->getSequenceObject()->getSequenceName() == "d", "1. Unexpected sequence name!");
    CHECK_SET_ERR(seq1->getSequenceObject()->getSequenceName() == "a", "2. Unexpected sequence name!");
    CHECK_SET_ERR(seq2->getSequenceObject()->getSequenceName() == "c", "3. Unexpected sequence name!");
    CHECK_SET_ERR(seq3->getSequenceObject()->getSequenceName() == "b", "4. Unexpected sequence name!");
}

GUI_TEST_CLASS_DEFINITION(test_2049) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Codon table"));
    GTGlobals::sleep(500);
    QWidget *w = GTWidget::findWidget(os, "Codon table widget");
    int ititHeight = GTWidget::findWidget(os, "Leucine (Leu, L)", w)->geometry().height();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "3. The Yeast Mitochondrial Code"));
    GTWidget::click(os, GTWidget::findWidget(os, "AminoToolbarButton"));
    GTGlobals::sleep(500);

    w = GTWidget::findWidget(os, "Codon table widget");
    int finalHeight = GTWidget::findWidget(os, "Leucine (Leu, L)", w)->geometry().height();
    CHECK_SET_ERR(ititHeight != finalHeight, "codone table not changed");
}

GUI_TEST_CLASS_DEFINITION(test_2053) {
    //    1. Remove UGENE .ini file
    //    2. Run UGENE
    //    3. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    4. Run some scheme
    WorkflowProcessItem *read = GTUtilsWorkflowDesigner::addElement(os, "Read Alignment");
    WorkflowProcessItem *write = GTUtilsWorkflowDesigner::addElement(os, "Write Alignment");
    GTUtilsWorkflowDesigner::connect(os, read, write);
    GTUtilsWorkflowDesigner::click(os, "Read Alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    //    Expected state: after scheme finish there is the hint on the dashboard -
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QToolButton *loadSchemaButton = GTUtilsDashboard::findLoadSchemaButton(os);
    CHECK_SET_ERR(loadSchemaButton, "loadSchemaButton not found");

    CHECK_SET_ERR(loadSchemaButton->toolTip() == "Open workflow schema", "loadSchemaButton has no hint")
}

GUI_TEST_CLASS_DEFINITION(test_2076) {
    //    1) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2) Run any scheme
    WorkflowProcessItem *read = GTUtilsWorkflowDesigner::addElement(os, "Read Alignment");
    WorkflowProcessItem *write = GTUtilsWorkflowDesigner::addElement(os, "Write Alignment");
    GTUtilsWorkflowDesigner::connect(os, read, write);
    GTUtilsWorkflowDesigner::click(os, read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::saveWorkflowAs(os, sandBoxDir + "test_2076.uwl", "test_2076");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTGlobals::sleep();
    //    Expected state: workflow monitor appeared

    //    3) Click on the 'Dashboards manager' tool button
    QWidget *dmButton = GTAction::button(os, GTAction::findAction(os, "Dashboards manager"));
    GTWidget::click(os, dmButton);
    //    Expected state: 'Dashboards manager' dialog appeared
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    4) Select some dashboards in the dialog
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            //    5) Click on the 'Remove selected' button
            DashboardsManagerDialogFiller::selectDashboards(os, QStringList() << "test_2076 1");
            //    Expected state: 'Removing dashboards' dialog appeared
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Confirm"));
            //    6) Click on the 'Confirm' button
            GTWidget::click(os, GTWidget::findWidget(os, "removeButton", dialog));

            //    Expected state: selected dashboards were removed
            bool pres = DashboardsManagerDialogFiller::isDashboardPresent(os, "test_2076 1");
            CHECK_SET_ERR(!pres, "dashboard unexpectidly present")

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new DashboardsManagerDialogFiller(os, new custom()));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2077) {
    // 1) Open WD

    // 2) Add elements "Read Sequence" and "Write sequence" to the scheme
    // 3) Connect "Read Sequence" to "Write sequence"

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence", true);

    WorkflowProcessItem *seqReader = GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence");
    WorkflowProcessItem *seqWriter = GTUtilsWorkflowDesigner::getWorker(os, "Write Sequence");

    GTUtilsWorkflowDesigner::connect(os, seqReader, seqWriter);

    // 4) Add file "human_T1.fa" to "Read Sequence" twice

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    // 5) Validate scheme
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Workflow is valid"));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
    GTGlobals::sleep(200);

    //Expected state: The "File not found" error has appeared in the "Error list"
    GTUtilsWorkflowDesigner::checkErrorList(os, "file '../human_T1.fa' was specified several times");
}

GUI_TEST_CLASS_DEFINITION(test_2078) {
    //    1. Go to the WD samples. Double click on the Chip-Seq WD sample.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    Expexted: setup dialog appears.
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Cistrome Workflow", QStringList() << "Only treatment tags"));

    class customWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            QWizard *wizard = qobject_cast<QWizard *>(dialog);
            CHECK_SET_ERR(wizard, "activeModalWidget is not wizard");

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QComboBox *combo = GTWidget::findExactWidget<QComboBox *>(os, "Motif database widget", dialog);
            GTComboBox::checkValues(os, combo, QStringList() << "cistrome.xml"
                                                             << "jaspar.xml");

            QList<QLabel *> labelList = wizard->currentPage()->findChildren<QLabel *>();
            bool found = false;
            foreach (QLabel *l, labelList) {
                if (l->text().contains("Use 'cistrome.xml' to descrease the computation time")) {
                    CHECK_SET_ERR(l->isVisible(), "hint is invisiable");
                    found = true;
                    break;
                }
            }
            CHECK_SET_ERR(found, "hint label not found");

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
            dialog = QApplication::activeModalWidget();
            if (dialog != NULL) {
                GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
            }
        }
    };
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-seq Analysis Wizard", new customWizard));
    GTUtilsWorkflowDesigner::addSample(os, "Cistrome");
    GTGlobals::sleep();

    //    4. In 'Motif database' combobox select following databases: 'cistrome.xml', 'jaspar.xml'
    //    Expected state: hint was appeared and contains following text "Use 'cistrome.xml' to decrease computation time..."
}

GUI_TEST_CLASS_DEFINITION(test_2089) {
    // 1. Start UGENE with a new *.ini file.
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os, "!@#$%^&*()_+\";:?/", false));

    // 2. Open WD
    // 3. Set any folder without write access as workflow output folder. Click OK.
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Workflow Designer...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2100_1) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click toolbutton "Enable collapsing"
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "Enable collapsing"));

    //expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are collapsed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(os, QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    //3. Select Mecopoda_sp.__Malaysia_
    GTUtilsMSAEditorSequenceArea::selectSequence(os, QString("Mecopoda_sp.__Malaysia_"));

    //4. Expand Mecopoda_elongata__Ishigaki__J

    //Expected state: Mecopoda_sp.__Malaysia_ is still selected
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(os, QString("Mecopoda_sp.__Malaysia_")),
                  "Expected sequence is not selected");
}

GUI_TEST_CLASS_DEFINITION(test_2100_2) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click toolbutton "Enable collapsing"
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "Enable collapsing"));

    //3. Select Mecopoda_sp.__Malaysia_
    GTUtilsMSAEditorSequenceArea::selectSequence(os, QString("Mecopoda_sp.__Malaysia_"));

    //4. Expand Select Mecopoda_sp.__Malaysia_
    //Expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are selected
}

GUI_TEST_CLASS_DEFINITION(test_2124) {
    // 1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // 2. Call the context menu on the sequence area.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(1, 1));
    const QString colorSchemeName = getName() + "_Scheme";
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                                        << "Custom schemes"
                                                                        << "Create new color scheme"));
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, colorSchemeName, NewColorSchemeCreator::amino));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    // 3. Create a new color scheme for the amino alphabet.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(1, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                                        << "Custom schemes" << colorSchemeName));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_2091) {
    //1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: document are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex(os, "COI.aln");
    QWidget *msaView = GTUtilsMdi::activeWindow(os);
    CHECK(NULL != msaView, );

    //2. Select any sequence.
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(-5, 4));
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    //3. Call context menu on the name list area, select the {Edit -> Remove sequence} menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EDIT << "Remove sequence"));
    GTMouseDriver::click(Qt::RightButton);
    //Expected state: the sequence is removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList(os);

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 1, "The number of sequences remained unchanged.");
    CHECK_SET_ERR(!modifiedNames.contains("Montana_montana"), "Removed sequence is present in multiple alignment.");
}

GUI_TEST_CLASS_DEFINITION(test_2093_1) {
    //    1. Run a scheme, e.g. "Call variants with SAMtools" from the NGS samples (or any other like read->write).
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // Simple scheme: read file list.
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read File URL(s)");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    GTUtilsWorkflowDesigner::runWorkflow(os);

    GTGlobals::sleep();

    //    2. Select "Load schema" button on the dashboard menu line.
    QToolButton *loadSchemaButton = GTUtilsDashboard::findLoadSchemaButton(os);
    CHECK_SET_ERR(loadSchemaButton, "loadSchemaButton not found");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Discard));
    GTWidget::click(os, loadSchemaButton);
    GTGlobals::sleep();

    //    Expected result: the scheme with parameters is loaded.
    WorkflowProcessItem *wdElement = GTUtilsWorkflowDesigner::getWorker(os, "Read File URL(s)");
    CHECK_SET_ERR(wdElement, "Schema wasn't loaded");
}

GUI_TEST_CLASS_DEFINITION(test_2093_2) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Open any shema with the "Load workflow" button on the toolbar (not the "Open" button!)
    QString schemaPath = testDir + "_common_data/scenarios/workflow designer/222.uwl";
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, schemaPath));

    QToolBar *wdToolbar = GTToolbar::getToolbar(os, "mwtoolbar_activemdi");
    CHECK_SET_ERR(wdToolbar, "Toolbar not found");
    QWidget *loadButton = GTToolbar::getWidgetForActionName(os, wdToolbar, "Load workflow");
    CHECK_SET_ERR(loadButton, "Load button not found");
    GTWidget::click(os, loadButton);

    GTGlobals::sleep();

    // Expected result: the scheme with parameters is loaded.

    WorkflowProcessItem *wdElement = GTUtilsWorkflowDesigner::getWorker(os, "Read sequence");
    CHECK_SET_ERR(wdElement, "Schema wasn't loaded");
}

GUI_TEST_CLASS_DEFINITION(test_2128) {
    // 1. Open document "ma.aln" and save it to string
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString initialMsaContent = GTClipboard::text(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a region in the sequence area
    QRect currentSelection(QPoint(2, 5), QPoint(8, 11));
    GTUtilsMSAEditorSequenceArea::selectArea(os, currentSelection.topLeft(), currentSelection.bottomRight());

    // 3. Add gaps by pressing "Ctrl + Space" key
    const int totalShiftCount = 3;
    for (int shiftCounter = 0; shiftCounter < totalShiftCount; ++shiftCounter) {
        GTKeyboardDriver::keyClick(Qt::Key_Space, Qt::ControlModifier);
        GTGlobals::sleep(200);
        currentSelection.moveRight(currentSelection.right() + 1);
        GTGlobals::sleep(200);
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, currentSelection);
    }

    // 4. Remove gaps with "Ctrl + Backspace" key
    for (int shiftCounter = 0; shiftCounter < totalShiftCount; ++shiftCounter) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace, Qt::ControlModifier);
        GTGlobals::sleep(200);
        currentSelection.moveLeft(currentSelection.left() - 1);
        GTGlobals::sleep(200);
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, currentSelection);
    }

    // 5. Check that alignment content has returned to initial state
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2128_1) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select the six first symbols in the first sequence and press Space.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(5, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(2000);

    // Expected state: The first symbol T is on the 7 position.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    const QPoint initialSelectionPos(6, 0);
    GTUtilsMSAEditorSequenceArea::click(os, initialSelectionPos);
    GTGlobals::sleep(2000);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("T" == finalMsaContent,
                  "Unexpected MSA content has occurred");

    //3. Press the Ctrl + Space.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(6, 0), QPoint(12, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space, Qt::ControlModifier);
    GTGlobals::sleep(200);

    // Expected state: The first symbol T is on the 8 position.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 0), QPoint(7, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    const QString finalMsaContent1 = GTClipboard::text(os);
    CHECK_SET_ERR("T" == finalMsaContent1,
                  "Unexpected MSA content has occurred");

    //4. Press the Ctrl + Backspace.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 0), QPoint(13, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Backspace, Qt::ControlModifier);
    GTGlobals::sleep(200);

    // Expected state: The first symbol T is on the 8 position.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(6, 0), QPoint(6, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    const QString finalMsaContent2 = GTClipboard::text(os);
    CHECK_SET_ERR("T" == finalMsaContent2,
                  "Unexpected MSA content has occurred");

    //5. Press the Backspace.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(6, 0), QPoint(12, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(200);

    // Expected state: The first symbol T is on the 1 position.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(0, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);
    const QString finalMsaContent3 = GTClipboard::text(os);
    CHECK_SET_ERR("T" == finalMsaContent3,
                  "Unexpected MSA content has occurred");
}

GUI_TEST_CLASS_DEFINITION(test_2138) {
    //1. Open _common_data/scenarios/_regression/2138/1.fa
    //2. Press "Join sequences into alignment..." radio button
    //3. Press "OK" button
#ifdef Q_OS_MAC
    //hack for mac
    MainWindow *mw = AppContext::getMainWindow();
    CHECK_SET_ERR(mw != NULL, "MainWindow is NULL");
    QMainWindow *mainWindow = mw->getQMainWindow();
    CHECK_SET_ERR(mainWindow != NULL, "QMainWindow is NULL");
    QWidget *w = qobject_cast<QWidget *>(mainWindow);
    GTWidget::click(os, w, Qt::LeftButton, QPoint(5, 5));
    GTGlobals::sleep(500);
#endif

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/_regression/2138/1.fa");

    GTGlobals::sleep(1000);
    //Expected state: alignment has been opened and whole msa alphabet is amino
    bool isAmino = GTUtilsMSAEditorSequenceArea::hasAminoAlphabet(os);
    CHECK_SET_ERR(isAmino, "Aligment has wrong alphabet type");
}

GUI_TEST_CLASS_DEFINITION(test_2140) {
    //1. Use main menu {Tools->Dna assembly->convert UGENE assembly database to SAM format}
    //2. Select any .fasta file instead of .ugenedb file
    //3. Click "convert"
    //Expected state: UGENE not crashes. Error message is written to log

    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ConvertAssemblyToSAMDialogFiller(os, dataDir + "samples/FASTA/", "human_T1.fa"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Convert UGENE assembly database to SAM...");

    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2144) {
    //    1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open the NGS sample scheme "Call variants with SAM tools".
    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    3. Fill input data, e.g.:
    //        "data/samples/Assembly/chrM.sam" as input to "Read Assembly SAM/BAM" element;
    //        "data/samples/Assembly/chrM.fa" as input to "Read Sequence" element;
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Assembly/chrM.sam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Assembly/chrM.fa");

    //    4. Choose "Estimate" option in tool bar.
    //       "Estimate" option is available only for NGS samples (except "Extract transcript sequence").
    //    Expected state: Estimation dialog appears and provides information about approximate time of workflow run.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Close, "Approximate estimation time of the workflow run is"));
    GTWidget::click(os, GTAction::button(os, "Estimate workflow"));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2150) {
    // 1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    // 2. Open the "Align sequences with MUSCLE" sample scheme.
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Set "data/samples/CLUSTALW/ty3.aln.gz" as the input file.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTGlobals::sleep();

    // 4. Set some name to the result file.
    //    QTableView* table = qobject_cast<QTableView*>(GTWidget::findWidget(os,"table"));
    //    CHECK_SET_ERR(table,"tableView not found");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write alignment"));
    GTMouseDriver::click();
    //    GTMouseDriver::moveTo(GTTableView::getCellPosition(os,table,1,1));
    //    GTMouseDriver::click();
    QString s = QFileInfo(testDir + "_common_data/scenarios/sandbox/").absoluteFilePath();
    GTKeyboardDriver::keySequence(s + "/2150_0001.sto");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", s + "/2150_0001.aln", GTUtilsWorkflowDesigner::textValue);
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));

    // 5. Run the workflow.
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(5000);
    //GTUtilsTaskTreeView::waitTaskFinished(os,1000);

    // 6. During the workflow execution open the "Tasks" panel in the bottom, find in the task tree the "MUSCLE alignment" subtask and cancel it.
    GTUtilsTask::cancelSubTask(os, "MUSCLE alignment");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2152) {
    //1. Open WD
    //2. Create a scheme with the following elements: Read File URL(s), Assembly Sequences with CAP3
    //3. Put _common_data/cap3/region1.fa,
    //       _common_data/cap3/region2.fa,
    //       _common_data/cap3/region3.fa,
    //       _common_data/cap3/region4.fa as an input sequences
    //4. Run the scheme
    //Expected state: the dashboard appears, the WD task has been finished without errors.

    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read File URL(s)");
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Assembly Sequences with CAP3");

    WorkflowProcessItem *fileList = GTUtilsWorkflowDesigner::getWorker(os, "Read File URL(s)");
    WorkflowProcessItem *fileCAP3 = GTUtilsWorkflowDesigner::getWorker(os, "Assembly Sequences with CAP3");

    GTUtilsWorkflowDesigner::connect(os, fileList, fileCAP3);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Assembly Sequences with CAP3"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", QDir().absoluteFilePath(sandBoxDir + "out.ace"), GTUtilsWorkflowDesigner::textValue);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cap3/region1.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cap3/region2.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cap3/region3.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cap3/region4.fa");

    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2156) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::openView(os);
    GTUtilsProjectTreeView::toggleView(os);
    GTGlobals::sleep(500);
    //    2. Select six symbols (45-50) of the first line.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(44, 0), QPoint(49, 0));
    //    3. Press BACKSPACE.
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    //    Expected state: three gaps before the selected area are removed.
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(41, 0), QPoint(44, 0));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipboardText = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardText == "CTAA", QString("Expected: CTAA, found: %1").arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_2157) {
    //    1. Open file "https://ugene.net/tracker/secure/attachment/12864/pdb1a07.ent.gz".
    GTFileDialog::openFile(os, testDir + "_common_data/pdb/", "pdb1a07.ent.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: UGENE displays sequence and 3d structure
    GTUtilsMdi::findWindow(os, "pdb1a07.ent.gz");
}

GUI_TEST_CLASS_DEFINITION(test_2160) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Select whole msa
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(11, 17));

    // 3. Shift msa
    const QPoint mouseDragPosition(7, 7);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition);
    GTMouseDriver::press();
    GTGlobals::sleep(200);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition + QPoint(3, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTGlobals::sleep(200);

    // 4. Press "Delete" key
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(200);

    // 6. Check that alignment has not changed
    CHECK_SET_ERR(15 == GTUtilsMSAEditorSequenceArea::getLength(os),
                  "MSA length unexpectedly changed");
    CHECK_SET_ERR(18 == GTUtilsMSAEditorSequenceArea::getNameList(os).size(),
                  "MSA row count unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2165) {
    GTLogTracer l;
    //1. Open human_t1
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Copy the whole sequence to the clipboard
    GTWidget::click(os, GTWidget::findWidget(os, "annotated_DNA_scrollarea"));
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os));
    GTWidget::click(os, GTWidget::findWidget(os, "select_range_action"));
    GTGlobals::sleep(500);

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    //3. Past the whole sequence to the find pattern field
    GTWidget::click(os, GTWidget::findWidget(os, "OP_FIND_PATTERN"));
    GTWidget::click(os, GTWidget::findWidget(os, "textPattern"));

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTThread::waitForMainThread();

    //4. Press the "Search" button

    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: UGENE finds the sequence or shows a error message
    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2188) {
    class Helper {
    public:
        Helper(const QString &dataDir, const QString &testDir, HI::GUITestOpStatus &os) {
            dir = testDir + "_common_data/scenarios/sandbox/";
            fileName = "regression_test_2188.fa";
            url = dir + fileName;
            bool copied = QFile::copy(dataDir + "samples/FASTA/human_T1.fa", url);
            if (!copied) {
                os.setError("Can not copy the file");
                url = "";
            }
        }

        QString url;
        QString dir;
        QString fileName;
    };

    // 1. Open the file "data/samples/FASTA/human_T1.fa"
    Helper helper(dataDir, testDir, os);
    CHECK_OP(os, );
    GTFileDialog::openFile(os, helper.dir, helper.fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

    // 2. At the same time open the file with a text editor
    // 3. Change something and save
    // Expected state: Dialog suggesting file to reload has appeared in UGENE
    // 4. Press "Yes to All" button
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    QFile file(helper.url);
    bool opened = file.open(QIODevice::Append);
    CHECK_SET_ERR(opened, "Can not open the file for writing");
    qint64 writed = file.write("AAAAAAAAAA");
    CHECK_SET_ERR(10 == writed, "Can not write to the file");
    file.close();
    GTGlobals::sleep(6000);

    // Expected state: All the sequences were reloaded and displayed correctly in sequence view
    int length = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_OP(os, );
    CHECK_SET_ERR(199960 == length, "The file lenght is wrong");
}

GUI_TEST_CLASS_DEFINITION(test_2187) {
    //1. Open "data/samples/FASTA/human_T1.fa"
    //d:\src\ugene\trunk\test\_common_data\scenarios\_regression\2187\seq.fa

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/2187/", "seq.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "seq.fa");

    //2. Open {Actions -> Analyze -> Find tandems...}
    //3. Click ok

    Runnable *tDialog = new FindTandemsDialogFiller(os, testDir + "_common_data/scenarios/sandbox/result_2187.gb");
    GTUtilsDialog::waitForDialog(os, tDialog);

    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Analyze"
                                                << "Find tandem repeats...",
                              GTGlobals::UseMouse);
    GTGlobals::sleep(200);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::getTreeWidget(os);
    QTreeWidgetItem *annotationsRoot = GTUtilsAnnotationsTreeView::findItem(os, "repeat_unit  (0, 5)");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, annotationsRoot->child(0)));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditAnnotationChecker(os, "repeat_unit", "251..251,252..252,253..253,254..254,255..255,256..256,257..257,258..258,259..259");
    GTUtilsDialog::waitForDialog(os, filler);
    static QList<QTreeWidgetItem *> items = GTUtilsAnnotationsTreeView::findItems(os, "repeat_unit");
    foreach (QTreeWidgetItem *item, items) {
        if (item->text(2) == "251..251,252..252,253..253,254..254,255..255,256..256,257..257,258..258,259..259") {
            CHECK_SET_ERR("9" == GTUtilsAnnotationsTreeView::getQualifierValue(os, "num_of_repeats", item), "Wrong num_of_repeats value");
            CHECK_SET_ERR("1" == GTUtilsAnnotationsTreeView::getQualifierValue(os, "repeat_length", item), "Wrong repeat_length value");
            CHECK_SET_ERR("9" == GTUtilsAnnotationsTreeView::getQualifierValue(os, "whole_length", item), "Wrong whole_length value");
            break;
        }
    }

    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTGlobals::sleep();

    GTUtilsMdi::click(os, GTGlobals::Close);
    GTMouseDriver::click();
}

GUI_TEST_CLASS_DEFINITION(test_2192) {
    QString samtoolsPath = "samtools-0.1.19/samtools";
#ifdef Q_OS_WIN
    samtoolsPath = "samtools-0.1.19\\samtools";
#endif
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Open Call Variants sample.
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/chrM.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/call-variations/chrM.fa");
    //    4. Start the scheme.
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    5. Open External Tools tab & copy sam tools path
    GTUtilsDashboard::openTab(os, GTUtilsDashboard::ExternalTools);
    QWidget *samToolsRunNode = GTUtilsDashboard::getCopyButton(os, "actor_call_variants_run_1_tool_SAMtools_run_1");
    GTWidget::click(os, samToolsRunNode);

    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text.contains(samtoolsPath), "Unexpected SAMTools path:\n" + text);
}

GUI_TEST_CLASS_DEFINITION(test_2202) {
    //    1. Open Workflow Designer first time (e.g. remove UGENE.ini before it).
    //    Expected: the dialog appears.

    //    2. Write the path to the folder which does not exist.

    //    3. Click OK.
    //    Expected: the folder is created, there are no error messages about write access.

    QDir workflowOutputDir(testDir + "_common_data/scenarios/sandbox/regression_test_2202/1/2/3/4/");
    CHECK_SET_ERR(!workflowOutputDir.exists(), "Dir already exists");

    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os, workflowOutputDir.absolutePath()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Workflow Designer...");

    GTGlobals::sleep();
    CHECK_SET_ERR(workflowOutputDir.exists(), "Dir wasn't created");
}

GUI_TEST_CLASS_DEFINITION(test_2204) {
    //    1. Go to the WD samples. Double click on the Chip-Seq WD sample.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    Expexted: setup dialog appears.
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Cistrome Workflow", QStringList() << "Only treatment tags"));

    class customWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QComboBox *combo = GTWidget::findExactWidget<QComboBox *>(os, "Motif database widget", dialog);
            GTComboBox::checkValues(os, combo, QStringList() /*<<"cistrome.xml"*/ << "hpdi.xml"
                                                                                  << "jaspar.xml");
            QString s = combo->currentText();
            CHECK_SET_ERR(s == "hpdi.xml,jaspar.xml", "unexpected text: " + s);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
            dialog = QApplication::activeModalWidget();
            if (dialog != NULL) {
                GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
            }
        }
    };
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-seq Analysis Wizard", new customWizard));
    GTUtilsWorkflowDesigner::addSample(os, "Cistrome");
    //    2. Press the setup button.
    //    Expexted: the sample opens and the first wizard page appears.
    //    4. Go to the fifth wizard page by the next button.
    //    5. Change the Motif database parameter: uncheck cistrome.xml and check the hpdi.xml, jaspar.xml
    //    Expected: combobox shows "hpdi.xml,jaspar.xml"
}

GUI_TEST_CLASS_DEFINITION(test_2225) {
    Runnable *filler = new NCBISearchDialogFillerDeprecated(os, "rat", true);

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Search NCBI GenBank...",
                              GTGlobals::UseKey);
    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_2259) {
    MainWindow *mw = AppContext::getMainWindow();
    CHECK_SET_ERR(mw != NULL, "MainWindow is NULL");
    QMainWindow *mainWindow = mw->getQMainWindow();
    CHECK_SET_ERR(mainWindow != NULL, "QMainWindow is NULL");

    QAction *menu = mainWindow->findChild<QAction *>(MWMENU_SETTINGS);
    CHECK_SET_ERR(menu->menu()->actions().size() == 2, "wrong numder of actions");
}

GUI_TEST_CLASS_DEFINITION(test_2266_1) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/ugenedb/Klebsislla.sort.bam.ugenedb");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/ugenedb/Klebsislla_ref.fa");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absolutePath() + "/test_2266_1.vcf", GTUtilsWorkflowDesigner::lineEditWithFileSelector);

    GTWidget::click(os, GTAction::button(os, "Run workflow"));

    GTUtilsTaskTreeView::waitTaskFinished(os, 6000000);

    GTFileDialog::openFile(os, sandBoxDir + "test_2266_1.vcf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::findIndex(os, "pkF70");
    GTUtilsProjectTreeView::findIndex(os, "pkf140");
}
GUI_TEST_CLASS_DEFINITION(test_2267_1) {
    //     1. Open human_T1.fa
    //
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //     2. Select random subsequence, press Ctrl+N, press Enter
    //
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "DDD", "D", "10..16"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();
    //     3. Press Insert, press '1' key until there is no new symbols in lineedit
    //      Current state: no error message for long qualifier
    Runnable *filler = new EditQualifierFiller(os, "111111111111111111111111111111111111111111111111111111111111111111111111111111111", "val", true, false);
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "D"));
    GTMouseDriver::click(Qt::RightButton);

    //     4. Press Enter
    //     Expected state: Edit qualifier window closes
}

GUI_TEST_CLASS_DEFINITION(test_2267_2) {
    //     1. Open human_T1.fa
    //
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //     2. Select random subsequence, press Ctrl+N, press Enter
    //
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "DDD", "D", "10..16"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();
    //     3. Press Insert, set the name of qualifier: '))()((_'
    //
    //     4. Press Enter
    //     Expected state: Error message appears once
    Runnable *filler = new EditQualifierFiller(os, "))()((_", "val", true, true);
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "D"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2268) {
    //    0. Copy t-coffee tool to the place where UGENE has enough permissions to change file permissions;
    //    Set the copied t-coffee tool in preferences.

    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK_SET_ERR(etRegistry, "External tool registry is NULL");

    ExternalTool *tCoffee = etRegistry->getById("USUPP_T_COFFEE");
    CHECK_SET_ERR(tCoffee, "T-coffee tool is NULL");

    const QFileInfo origToolPath(tCoffee->getPath());
    CHECK_SET_ERR(origToolPath.exists(), "T-coffee tool is not set");

    QDir origToolDir = origToolPath.dir();
#ifdef Q_OS_LINUX
    origToolDir.cdUp();    // exit from 'bin' folder
#endif

    GTFile::copyDir(os, origToolDir.absolutePath(), sandBoxDir + "GUITest_regression_scenarios_test_2268/");
#ifdef Q_OS_LINUX
    const QFileInfo newToolPath(sandBoxDir + "GUITest_regression_scenarios_test_2268/bin/t_coffee");
#elif defined(Q_OS_WIN)
    const QFileInfo newToolPath(sandBoxDir + "GUITest_regression_scenarios_test_2268/t_coffee.bat");
#else
    const QFileInfo newToolPath(sandBoxDir + "GUITest_regression_scenarios_test_2268/t_coffee");
#endif

    // Hack, it is better to set the tool path via the preferences dialog
    CHECK_SET_ERR(newToolPath.exists(), "The copied T-coffee tool does not exist");
    tCoffee->setPath(newToolPath.absoluteFilePath());

    QDir newToolDir = origToolPath.dir();
#ifdef Q_OS_LINUX
    newToolDir.cdUp();    // exit from 'bin' folder
#endif

    // 1. Forbid write access to the t-coffee folder recursively (chmod 555 -R %t-coffee-dir%).
    GTFile::setReadOnly(os, newToolDir.path(), true);

    // 2. Open "sample/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Right click on the MSA -> Align -> Align with T-Coffee.
    // 4. Click the "Align" button.
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new TCoffeeDailogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with T-Coffee"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    //    Expected: the t-coffee task started and finished well.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(test_2314) {
    //    1. Open 'COI.aln'
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Scroll sequence area to the last columns
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 604));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    GTGlobals::sleep(500);

    //    3. Select to the whole last column by clicking consensus area.
    QWidget *consArea = GTWidget::findWidget(os, "consArea");
    GTWidget::click(os, consArea, Qt::LeftButton, QPoint(consArea->geometry().right() - 1, consArea->geometry().height() / 2));
    GTGlobals::sleep(500);

    //    3.1 Move the selected column with a mouse to the right. Do not click -> it will reset the selection.
    GTUtilsMSAEditorSequenceArea::moveMouseToPosition(os, QPoint(603, 0));
    QPoint p = GTMouseDriver::getMousePosition();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(p.x() + 30, p.y()));
    GTMouseDriver::release();

    //    Expected state: the column was moved
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength(os) > 604, "The length of the alignment is not changed");

    GTUtilsMsaEditor::undo(os);
    GTGlobals::sleep();

    //    4. Move to the right 1 base region, that is close to the end of alignment
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(600, 5));
    p = GTMouseDriver::getMousePosition();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(p.x() + 30, p.y()));
    GTMouseDriver::release();

    //    Expected state: the region is moved if mouse goes beyond the right border of the alignment
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength(os) > 604, "The length of the alignment is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_2316) {
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, dataDir + "samples/../workflow_samples/Alignment", "basic_align.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep(5000);

    CHECK_SET_ERR(!GTUtilsProjectTreeView::isVisible(os), "Project tree view is visible");

    //     1. Start UGENE
    //
    //     2. Drag an .uwl file inside UGENE to open a workflow
    //     Expected state: now the project view is collapsed if a file.uwl is drag'n'dropped to UGENE when there is no project yet.
}
GUI_TEST_CLASS_DEFINITION(test_2269) {
    //1. Use main menu: {tools->Align short reeds}
    //2. Select Bowtie2 as a mapping tool
    //3. Try to set incorrect value in "Seed lingth" spinbox(Correct boundaries are: >3, <32)
    AlignShortReadsFiller::Bowtie2Parameters parameters(testDir + "_common_data/scenarios/_regression/1093/",
                                                        "refrence.fa",
                                                        testDir + "_common_data/scenarios/_regression/1093/",
                                                        "read.fa");
    parameters.seedLengthCheckBox = true;
    parameters.seedLength = 33;

    class Scenario_test_2269 : public CustomScenario {
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();

            QComboBox *methodNamesBox = GTWidget::findExactWidget<QComboBox *>(os, "methodNamesBox", dialog);
            GTComboBox::setIndexWithText(os, methodNamesBox, "Bowtie2");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/1093/refrence.fa"));
            QWidget *addRefButton = GTWidget::findWidget(os, "addRefButton", dialog);
            GTWidget::click(os, addRefButton);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/1093/read.fa"));
            QWidget *addShortreadsButton = GTWidget::findWidget(os, "addShortreadsButton", dialog);
            GTWidget::click(os, addShortreadsButton);

            QCheckBox *seedCheckBox = GTWidget::findExactWidget<QCheckBox *>(os, "seedlenCheckBox", dialog);
            GTCheckBox::setChecked(os, seedCheckBox, true);

            QSpinBox *seedSpinBox = GTWidget::findExactWidget<QSpinBox *>(os, "seedlenSpinBox", dialog);
            int max = seedSpinBox->maximum();
            CHECK_SET_ERR(max == 31, QString("wrong seed maximim: %1").arg(max));

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new Scenario_test_2269()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Map reads to reference...");

    CHECK_SET_ERR(!os.hasError(), "Uncorrect value is available");
}

GUI_TEST_CLASS_DEFINITION(test_2270) {
    //1. Open file "data/cmdline/snp.uwl"
    //Ecpected state: scheme opened in WD without problems
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTLogTracer lt;
    GTFileDialog::openFile(os, dataDir + "cmdline/", "snp.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(test_2281) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    QTabWidget *tabs = qobject_cast<QTabWidget *>(GTWidget::findWidget(os, "tabs"));
    CHECK_SET_ERR(tabs != NULL, "tabs widget not found");

    //2. Click the "samples" bar. The samples hint is shown
    GTTabWidget::setCurrentIndex(os, tabs, 1);
    GTGlobals::sleep(500);
    QGraphicsView *sceneView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "sceneView"));

    QImage img = GTWidget::getImage(os, sceneView);

    const QColor yc = QColor(255, 255, 160);
    bool found = false;
    for (int i = sceneView->rect().left(); i < sceneView->rect().right(); i += 10) {
        for (int j = sceneView->rect().top(); j < sceneView->rect().bottom(); j += 10) {
            const QRgb rgb = img.pixel(QPoint(i, j));
            const QColor c(rgb);
            if (c == yc) {
                found = true;
                break;
            }
        }
    }
    CHECK_SET_ERR(found, "hint not found");

    //3. Click the "elements" bar.
    GTTabWidget::setCurrentIndex(os, tabs, 0);
    GTGlobals::sleep(500);

    //Expected: the samples hint is hidden
    img = GTWidget::getImage(os, sceneView);
    bool notFound = true;
    for (int i = sceneView->rect().left(); i < sceneView->rect().right(); i += 10) {
        for (int j = sceneView->rect().top(); j < sceneView->rect().bottom(); j += 10) {
            const QRgb rgb = img.pixel(QPoint(i, j));
            const QColor c(rgb);
            if (c == yc) {
                notFound = false;
                break;
            }
        }
    }

    CHECK_SET_ERR(notFound, "hint is found");
}

GUI_TEST_CLASS_DEFINITION(test_2292) {
    QString destName = testDir + "_common_data/ugenedb/example-alignment.ugenedb";
    GTFileDialog::openFile(os, testDir + "_common_data/ugenedb/", "example-alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_2292.ace.ugenedb"));
    GTUtilsProject::openFile(os, dataDir + "samples/ACE/K26.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2295) {
    //1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, true));
    GTUtilsProject::openFile(os, dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: Alignent is locked
    bool isLocked = GTUtilsMSAEditorSequenceArea::isAlignmentLocked(os);
    CHECK_SET_ERR(isLocked, "Alignment is unexpectably unlocked");

    //2. Export alignment to read-write format
    GTUtilsDialog::waitForDialog(os, new ExportMSA2MSADialogFiller(os, 0, sandBoxDir + "DNA"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_PROJECT__EXPORT_TO_AMINO_ACTION));
    GTUtilsProjectTreeView::callContextMenu(os, "DNA.apr");
    GTGlobals::sleep();

    //Expected: Alignent is locked
    isLocked = GTUtilsMSAEditorSequenceArea::isAlignmentLocked(os);
    CHECK_SET_ERR(!isLocked, "Alignment is unexpectably locked");
}

GUI_TEST_CLASS_DEFINITION(test_2298) {
    //    1. Open the file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Build and show a tree
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Collapse any node in the tree
    QGraphicsItem *node = GTUtilsPhyTree::getNodes(os).at(1);
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    treeView->ensureVisible(node);
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(GTUtilsPhyTree::getGlobalCenterCoord(os, node));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    //    Expected state: the appropriate sequences in the msa view were collapsed into a group as well
    QStringList l = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    int num = l.count();
    CHECK_SET_ERR(num == 3, QString("Unexpected visible sequences count. Expected: 3, actual: %1").arg(num));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2293) {
    //    0. Ensure that Bowtie2 Build index tool is not set. Remove it, if it is.
    //    1. Do {main menu -> Tools -> ALign to reference -> Build index}.
    //    Expected state: a "Build index" dialog appeared.
    //    2. Fill the dialog:
    //        {Align short reads method}: Bowtie2
    //        {Reference sequence}:       data/samples/FASTA/human_T1.fa
    //        {Index file name}:          set any valid data or use default
    //    Click a "Start" button.

    //    Expected state: a message box appeared: an offer to set "Bowtie2 Build index" tool in UGENE preferences.
    GTUtilsExternalTools::removeTool(os, "Bowtie 2 build indexer");

    class CheckBowtie2Filler : public Filler {
    public:
        CheckBowtie2Filler(HI::GUITestOpStatus &os)
            : Filler(os, "BuildIndexFromRefDialog") {
        }
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QComboBox *methodNamesBox = dialog->findChild<QComboBox *>("methodNamesBox");
            for (int i = 0; i < methodNamesBox->count(); i++) {
                if (methodNamesBox->itemText(i) == "Bowtie2") {
                    GTComboBox::setCurrentIndex(os, methodNamesBox, i);
                }
            }

            GTFileDialogUtils *ob = new GTFileDialogUtils(os, dataDir + "samples/FASTA/", "human_T1.fa");
            GTUtilsDialog::waitForDialog(os, ob);
            GTWidget::click(os, GTWidget::findWidget(os, "addRefButton", dialog));

            QDialogButtonBox *box = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
            QPushButton *okButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != NULL, "ok button is NULL");
            GTWidget::click(os, okButton);

            QPushButton *cancelButton = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != NULL, "Cancel button is NULL");
            GTWidget::click(os, cancelButton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CheckBowtie2Filler(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Build index for reads mapping...");
}

GUI_TEST_CLASS_DEFINITION(test_2282) {
    // 1. Open "chrM.sorted.bam" file using {File->Open} menu in UGENE.
    //    Expected state: "Import BAM file" dialog has appeared.
    // 2. Press "Enter".
    //    Expected state:
    //      1) The dialog has been closed.
    //      2) A new project has been created.
    QString assFileName = testDir + "_common_data/scenarios/sandbox/test_2282.chrM.sorted.ugenedb";
    QString assDocName = "test_2282.chrM.sorted.ugenedb";
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, assFileName));
    GTFileDialog::openFile(os, testDir + "_common_data/bam", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3) The Project View with document "chrM.sorted.bam.ugenedb" has been opened.
    GTUtilsProjectTreeView::findIndex(os, assDocName);

    GTUtilsMdi::closeWindow(os, GTUtilsMdi::activeWindowTitle(os));
    GTGlobals::sleep(100);

    // 3. Delete "chrM.sorted.bam.ugenedb" from the file system (i.e. not from UGENE).
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    bool deleteResult = QFile::remove(QFileInfo(assFileName).absoluteFilePath());
    CHECK_SET_ERR(deleteResult, "Unable to remove assembly file");
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_2284) {
    //1. Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Press the "Switch on/off collapsing" button
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "Enable collapsing"));

    //expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are collapsed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(os, QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    //3. Select the first base in last row
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 16), QPoint(0, 16));

    //4. In status bar search field type "AATT"
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTGlobals::sleep(200);
    GTKeyboardDriver::keySequence("AATT");

    //5. Press the "Find forward" button a few times until selection reaches the end of the alignment
    //6. Press the button again
    for (int i = 0; i < 11; i++) {
        GTGlobals::sleep(200);
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2285) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Click the "Switch on/off collapsing" button on the toolbar.
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "Enable collapsing"));
    //    Expected state: Collapsed mode is switched on, there are one collapsed group.
    int visableNamesNum = GTUtilsMSAEditorSequenceArea::getVisibleNames(os).count();
    CHECK_SET_ERR(visableNamesNum == 17,
                  QString("unexpected visable sequences number. Expected 17, actual: %1").arg(visableNamesNum));
    //    3. Expand the collapsed group ("Mecopoda_elongata__Ishigaki__J" is the head sequence).
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(os, "Mecopoda_elongata__Ishigaki__J");
    GTGlobals::sleep(1000);

    //    4. Set the cursor to the 14 line (the "Mecopoda_elongata__Ishigaki__J" sequence), 45 base.

    QWidget *documentTreeWidget = GTWidget::findWidget(os, GTUtilsProjectTreeView::widgetName, NULL, GTGlobals::FindOptions(false));
    if (documentTreeWidget != NULL) {
        GTUtilsProjectTreeView::toggleView(os);
    }
    GTGlobals::sleep();
    GTUtilsMSAEditorSequenceArea::click(os, QPoint(44, 13));
    //    5. Press Backspace.
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    //    Expected state: each sequence in the group loose one gap.
    GTUtilsMSAEditorSequenceArea::click(os);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(43, 13), QPoint(43, 14));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipboardText = GTClipboard::text(os);

    CHECK_SET_ERR(clipboardText == "T\n-", "Unexpected selection. Expected: T\nT, actual: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_2306) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Turn on collapsing mode in MSA
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "Enable collapsing"));

    //expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are collapsed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(os, QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(5, 10), QPoint(15, 15));

    // 3. Shift the region but don't release left mouse button
    const QPoint mouseDragPosition(12, 12);
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(os, mouseDragPosition + QPoint(3, 0));
    GTGlobals::sleep(200);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTThread::waitForMainThread();
    GTGlobals::sleep(200);

    // 4. Call context menu
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(5, 10), QPoint(7, 15));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(200);
    const QString finalMsaContent = GTClipboard::text(os);
    CHECK_SET_ERR("---\n---\n---\n---\n---\n---" == finalMsaContent, "Unexpected MSA content has occurred" + finalMsaContent);
}

GUI_TEST_CLASS_DEFINITION(test_2309) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Build tree for the alignment
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, testDir + "_common_data/scenarios/sandbox/2309.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList initialNames = GTUtilsMSAEditorSequenceArea::getNameList(os);
    QAbstractButton *refresh = GTAction::button(os, "Refresh tree");

    CHECK(NULL != refresh, );
    if (refresh->isVisible()) {
        GTWidget::click(os, refresh);
    } else {
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Refresh tree"));
        QToolBar *toolBar = qobject_cast<QToolBar *>(refresh->parent());
        GTMouseDriver::moveTo(toolBar->mapToGlobal(toolBar->geometry().bottomRight()) - QPoint(5, 15));
        GTMouseDriver::click();
    }

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList newNames = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(newNames == initialNames, "Wrong sequences order");

    GTGlobals::sleep();
    GTWidget::click(os, GTUtilsProjectTreeView::getTreeView(os));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTGlobals::sleep(100);

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::NoToAll));
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2318) {
    class FirstItemPopupChooser : public PopupChooser {
    public:
        FirstItemPopupChooser(HI::GUITestOpStatus &os)
            : PopupChooser(os, QStringList()) {
        }

        virtual void run() {
            GTGlobals::sleep(1000);
            GTMouseDriver::release();
            GTKeyboardDriver::keyClick(Qt::Key_Down);
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };

    class PlusClicker : public FindAnnotationCollocationsDialogFiller {
    public:
        PlusClicker(HI::GUITestOpStatus &os, const QString &_annName)
            : FindAnnotationCollocationsDialogFiller(os),
              annName(_annName) {
        }

        virtual void run() {
            QToolButton *plusButton = getPlusButton();
            CHECK_SET_ERR(plusButton, "First plus toolbutton is NULL");
            firstSize = plusButton->size();

            GTUtilsDialog::waitForDialog(os, new FirstItemPopupChooser(os));
            GTWidget::click(os, plusButton);

            GTGlobals::sleep();
            plusButton = getPlusButton();
            CHECK_SET_ERR(plusButton, "Second plus toolbutton is NULL");
            secondSize = plusButton->size();

            bool result = test();
            CHECK_SET_ERR(result, "Incorrect result");

            FindAnnotationCollocationsDialogFiller::run();
        }

    private:
        bool test() const {
            return firstSize == secondSize;
        }

        QString annName;
        QSize firstSize;
        QSize secondSize;
    };

    //    1) Open {data/samples/murine.gb}
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2) Use context menu: {Analyze -> Find annotated regions}
    //    3) Click plus button, select any annotation
    //    Expected state: plus button changes place, its size wasn't change
    QToolBar *toolbar = GTToolbar::getToolbar(os, "mwtoolbar_activemdi");
    CHECK_SET_ERR(toolbar, "Toolbar is NULL");
    QWidget *farButton = GTToolbar::getWidgetForActionTooltip(os, toolbar, "Find annotated regions");
    CHECK_SET_ERR(farButton, "Find annotated region button is NULL");

    // Expected state is checked in PlusClicker
    GTUtilsDialog::waitForDialog(os, new PlusClicker(os, "CDS"));
    GTWidget::click(os, farButton);
}

GUI_TEST_CLASS_DEFINITION(test_2351) {
    //    1. Run UGENE
    //    2. Press the "New Project" button on the main toolbar
    //    Expected state: the "Create new project" dialog has appeared
    //    3. Press the "Create" button or press the "Enter" key
    //    4. Repeat steps 2 and 3 as fast as you can more than 10 times
    //    Expected state: UGENE does not crash
    class RapidProjectCreator : public Filler {
    public:
        RapidProjectCreator(HI::GUITestOpStatus &os, const QString &_projectName, const QString &_projectFolder, const QString &_projectFile)
            : Filler(os, "CreateNewProjectDialog"),
              projectName(_projectName),
              projectFolder(_projectFolder),
              projectFile(_projectFile) {
        }

        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            if (NULL == dialog) {
                os.setError("Modal dialog not found");
                return;
            }

            QLineEdit *projectNameEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "projectNameEdit", dialog));
            if (NULL == projectNameEdit) {
                os.setError("projectNameEdit not found");
                return;
            }
            projectNameEdit->setText(projectName);

            QLineEdit *projectFileEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "projectFilePathEdit", dialog));
            if (NULL == projectFileEdit) {
                os.setError("projectFileEdit not found");
                return;
            }
            projectFileEdit->setText(projectFolder + "/" + projectFile);

            QDialogButtonBox *box = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");
            QPushButton *button = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(button != NULL, "ok button is NULL");
            GTWidget::click(os, button);
        }

    private:
        const QString projectName;
        const QString projectFolder;
        const QString projectFile;
    };

    const QString projectName = "test_2351";
    const QString projectFolder = testDir + "_common_data/scenarios/sandbox";
    const QString projectFile = "test_2351";

    for (int i = 0; i < 15; ++i) {
        GTUtilsDialog::waitForDialog(os, new RapidProjectCreator(os, projectName, projectFolder, projectFile));
        GTWidget::click(os,
                        GTToolbar::getWidgetForActionName(os,
                                                          GTToolbar::getToolbar(os, MWTOOLBAR_MAIN),
                                                          ACTION_PROJECTSUPPORT__NEW_PROJECT));
    }
}

GUI_TEST_CLASS_DEFINITION(test_2342) {
    //    1. Open WD, open Cistrome sample.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Select the SeqPos element on the scene. Properties of the SeqPos element are displayed the property widget.
    //    GTUtilsDialog::waitForDialog(os, new EscapeClicker(os, "ChIP-seq Analysis Wizard"));
    //    3. Open wizard, go to the SeqPos page.
    //    GTWidget::click(os, GTAction::button(os, "Show wizard"));
    //    4. Change any parameter value(e.g. "Motif database").
    //    5. Apply wizard changes.
    QVariantMap map;
    map.insert("Region width", QVariant(300));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-seq Analysis Wizard", QList<QStringList>(), map));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Cistrome Workflow", QStringList() << "Only treatment tags"));
    GTUtilsWorkflowDesigner::addSample(os, "Cistrome");

    //    Expected state: element on the scene is updated, property widget is updated.
    GTUtilsWorkflowDesigner::click(os, "Collect Motifs with SeqPos");
    QString par = GTUtilsWorkflowDesigner::getParameter(os, "Region width");
    CHECK_SET_ERR(par == "300", "unexpected parameter: " + par);
}

GUI_TEST_CLASS_DEFINITION(test_2343) {
    //    1. Open Workflow designer
    //    2. Add element "Align with ClustalW"
    //    3. Select the element
    //    Expected state: property widget for the element appeared
    //    4. Select "Gap distance" parameter in the property widget
    //    5. Press "Tab" key
    //    Expected state: focus setted to "End Gaps" value

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Align with ClustalW");
    GTUtilsWorkflowDesigner::click(os, "Align with ClustalW");

    GTUtilsWorkflowDesigner::clickParameter(os, "Gap distance");
    GTMouseDriver::doubleClick();
    QWidget *wgt = QApplication::focusWidget();
    CHECK_SET_ERR(wgt != NULL, "No widnget in focus")

    GTKeyboardDriver::keyClick(Qt::Key_Tab);
    GTGlobals::sleep();

    CHECK_SET_ERR(QApplication::focusWidget() != NULL, "No widget in focus");
    CHECK_SET_ERR(wgt != QApplication::focusWidget(), "Focus didn't changed");
}

GUI_TEST_CLASS_DEFINITION(test_2344) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Add the "ChIP-seq analysis with Cistrome tools" sample.
    GTUtilsDialog::waitForDialog(os, new DefaultWizardFiller(os, "ChIP-seq Analysis Wizard"));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Cistrome Workflow", QStringList() << "Only treatment tags"));
    GTUtilsWorkflowDesigner::addSample(os, "Cistrome");
    //    Expected state: the sample wizard appears.
    //    3. Choose the "Treatment tags only" mode and click "Setup".
    //    Expected state: the sample is added. The workflow scale is 90%. The workflow wizard appears.
    //    4. Exit the wizard.
    QComboBox *wdScaleCombo = GTWidget::findExactWidget<QComboBox *>(os, "wdScaleCombo");

    CHECK_SET_ERR(wdScaleCombo->itemText(0) == "25%", "unexpected scale: " + wdScaleCombo->itemText(0));
    CHECK_SET_ERR(wdScaleCombo->itemText(1) == "50%", "unexpected scale: " + wdScaleCombo->itemText(1));
    CHECK_SET_ERR(wdScaleCombo->itemText(2) == "75%", "unexpected scale: " + wdScaleCombo->itemText(2));
    CHECK_SET_ERR(wdScaleCombo->itemText(3) == "90%", "unexpected scale: " + wdScaleCombo->itemText(3));
    CHECK_SET_ERR(wdScaleCombo->itemText(4) == "100%", "unexpected scale: " + wdScaleCombo->itemText(4));
    CHECK_SET_ERR(wdScaleCombo->itemText(5) == "125%", "unexpected scale: " + wdScaleCombo->itemText(5));
    CHECK_SET_ERR(wdScaleCombo->itemText(6) == "150%", "unexpected scale: " + wdScaleCombo->itemText(6));
    CHECK_SET_ERR(wdScaleCombo->itemText(7) == "200%", "unexpected scale: " + wdScaleCombo->itemText(7));
    //    5. Expand the scale combobox on the toolbar.
    //    Expected state: values are sorted in the increasing order.
}

GUI_TEST_CLASS_DEFINITION(test_2352) {
    //1. Close current project or open empty UGENE
    //2. Open menu {Tools->Build dotplot...}
    //3. Select any binary file as first file in dialog
    //Expected state: file is not selected, no crash

    QString bin = QCoreApplication::applicationFilePath();
    GTUtilsDialog::waitForDialog(os, new BuildDotPlotFiller(os, bin, bin, false, false, false, 5, 5, true));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Build dotplot...");

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2360) {
    // 1. Open "data/samples/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Right click on document in project.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));

    // 3. Choose the context menu {Export/Import->Export nucleic alignment to amino translation}.
    // Expected state: Export dialog appears.
    // 4. Set "File format to use" to PHYLIP Sequantial.
    // 5. Click "Export".
    GTUtilsDialog::waitForDialog(os, new ExportMSA2MSADialogFiller(os, 6));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_PROJECT__EXPORT_TO_AMINO_ACTION));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2364) {
    //1. Open WD.'
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Create a workflow: Read sequence -> Write sequence.
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(os, GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence"), GTUtilsWorkflowDesigner::getWorker(os, "Write Sequence"));

    //3. Set the input sequence file: "data/samples/FASTA/human_T1.fa".
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTGlobals::sleep(300);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    //4. Set the output file: "out.fa".
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write Sequence"));
    GTMouseDriver::click();
    GTGlobals::sleep(300);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "out.fa", GTUtilsWorkflowDesigner::textValue);

    //5. Validate the workflow.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
    GTGlobals::sleep(1000);

    //Expected: the workflow has the warning about FASTA format and annotations.
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::checkErrorList(os, "") != 0, "There is no any messages in infoLog");

    //6. Run the workflow.
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(500);

    //Expected: the button "Load schema" is shown.
    // expected button is in dashboard - it can't be checked for now
}

GUI_TEST_CLASS_DEFINITION(test_2373) {
    GTLogTracer logTracer;

    //    1. Open "COI.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Move 'Mecopoda_elongata__Ishigaki__J' and 'Mecopoda_elongata__Sumatra_' to the end of name list
    GTUtilsMsaEditor::replaceSequence(os, "Mecopoda_elongata__Ishigaki__J", 17);
    GTUtilsMsaEditor::replaceSequence(os, "Mecopoda_elongata__Sumatra_", 17);

    //    3. Press 'Switch on/off collapsing mode' tool button
    //    Expected state: collapsing group, that contains two sequences, appeared
    GTUtilsMsaEditor::toggleCollapsingMode(os);

    //    4. Mouse press on the group
    //    Expected state:  group selected
    GTUtilsMsaEditor::clickSequenceName(os, "Mecopoda_elongata__Ishigaki__J");

    //    5. Mouse press under the group in the name list
    //    Expected state:  nothing happens
    GTMouseDriver::moveTo(GTUtilsMsaEditor::getSequenceNameRect(os, "Mecopoda_elongata__Ishigaki__J").center() + QPoint(0, 20));
    GTMouseDriver::click();

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_2374) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Create scheme read alignment->write alignment
    WorkflowProcessItem *read = GTUtilsWorkflowDesigner::addElement(os, "Read Alignment");
    WorkflowProcessItem *write = GTUtilsWorkflowDesigner::addElement(os, "Write Alignment");
    GTUtilsWorkflowDesigner::connect(os, read, write);
    //    3. Set COI.aln as input, run scheme
    GTUtilsWorkflowDesigner::click(os, read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: there is no "External Tools" page on the WD dashboards
    GTWebView::checkElement(os, GTUtilsDashboard::getDashboardWebView(os), "External Tools", "A", false);
}

GUI_TEST_CLASS_DEFINITION(test_2375) {
    //    1. Open {_common_data/sam/broken_invalid_cigar.sam}
    //    Expected state: import dialog appears.

    //    2. Fill the import dialog with valid data. Begin the importing.
    //    Expected state: importing fails, UGENE doesn't crash.
    QString destUrl = testDir + "_common_data/scenarios/sandbox/test_2375.ugenedb";
    GTLogTracer logtracer;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, destUrl));
    GTFileDialog::openFile(os, testDir + "_common_data/sam/", "broken_invalid_cigar.sam");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    TaskScheduler *scheduler = AppContext::getTaskScheduler();
    CHECK_SET_ERR(scheduler, "Task scheduler is NULL");
    GTGlobals::sleep(5000);
    while (!scheduler->getTopLevelTasks().isEmpty()) {
        GTGlobals::sleep();
    }

    CHECK_SET_ERR(logtracer.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2377) {
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    const QString assemblyReaderName = "Read NGS Reads Assembly";
    const QString assemblyWriterName = "Write NGS Reads Assembly";

    GTUtilsWorkflowDesigner::addAlgorithm(os, assemblyReaderName);
    GTUtilsWorkflowDesigner::addAlgorithm(os, assemblyWriterName);

    WorkflowProcessItem *assemblyReader = GTUtilsWorkflowDesigner::getWorker(os,
                                                                             assemblyReaderName);
    WorkflowProcessItem *assemblyWriter = GTUtilsWorkflowDesigner::getWorker(os,
                                                                             assemblyWriterName);

    GTUtilsWorkflowDesigner::connect(os, assemblyReader, assemblyWriter);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, assemblyReaderName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/sam/broken_without_reads.sam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, assemblyWriterName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "test", GTUtilsWorkflowDesigner::textValue);

    GTWidget::click(os, GTAction::button(os, "Run workflow"));

    GTGlobals::sleep(5000);
    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2378) {
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, testDir + "_common_data/scenarios/sandbox/test_2378.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/sam/", "scerevisiae.sam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2378_1) {
    GTLogTracer l;
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2. Create scheme: read assembly->write assembly
    // 3. set _common_data\sam\scerevisiae.sam as input file
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read NGS Reads Assembly");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read NGS Reads Assembly"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/sam/scerevisiae.sam");

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write NGS Reads Assembly");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write NGS Reads Assembly"));
    GTMouseDriver::click();
    QString path = QFileInfo(testDir + "_common_data/scenarios/sandbox/").absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", path + "/test_2378_1.bam", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(os, GTUtilsWorkflowDesigner::getWorker(os, "Read NGS Reads Assembly"), GTUtilsWorkflowDesigner::getWorker(os, "Write NGS Reads Assembly"));

    // 4. Run scheme
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(5000);
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2379) {
    class CreateProjectFiller : public Filler {
        // It is a local support class, it is the same as SaveProjectAsDialogFiller,
        // but it clicks the final button with keyboard.
        // I know that it is bad practice to create so useless classes, but I don't need to extend the original class.
        // Do not move it to another place: if you need the same filler than extend the original class.
    public:
        CreateProjectFiller(HI::GUITestOpStatus &_os,
                            const QString &_projectName,
                            const QString &_projectFolder,
                            const QString &_projectFile)
            : Filler(_os, "CreateNewProjectDialog"),
              projectName(_projectName),
              projectFolder(_projectFolder),
              projectFile(_projectFile) {
        }

        virtual void run() {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);

            QLineEdit *projectNameEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "projectNameEdit", dialog));
            GTLineEdit::setText(os, projectNameEdit, projectName);

            QLineEdit *projectFileEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "projectFilePathEdit", dialog));
            GTLineEdit::setText(os, projectFileEdit, projectFolder + "/" + projectFile);

            GTGlobals::sleep(1000);
#ifdef Q_OS_MACX
            GTWidget::click(os, GTWidget::findButtonByText(os, "Create", dialog));
#else
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
#endif
        }

    private:
        const QString projectName;
        const QString projectFolder;
        const QString projectFile;
    };

    // 0. Create a project that will be "existing" in the second step
    const QString projectName = "test_2379";
    const QString projectFolder = testDir + "_common_data/scenarios/sandbox";
    const QString projectFile = "test_2379";

    GTUtilsDialog::waitForDialog(os, new CreateProjectFiller(os, projectName, projectFolder, projectFile));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "New project...");
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Save all");
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");

    // 1. Press "Create new project" button
    // 2. Specify the path to an existing project
    // 3. Press "Create" button by using keyboard
    //    Expected state: only one dialog with warning message appeared
    GTUtilsDialog::waitForDialog(os, new CreateProjectFiller(os, projectName, projectFolder, projectFile));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes, "Project file already exists"));

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "New project...");
}

GUI_TEST_CLASS_DEFINITION(test_2382) {
    // 1. Open "_common_data/ace/capres4.ace".
    // 2. "Import ACE file" dialog appeared. Press ok.
    // Expected state: export succeeded, assembly is displayed.
    GTLogTracer l;

    QString sandboxDir = testDir + "_common_data/scenarios/sandbox/";
    QString assDocName = "test_2382.ugenedb";
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandboxDir + assDocName));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/capres4.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());

    GTUtilsProjectTreeView::findIndex(os, assDocName);
}

GUI_TEST_CLASS_DEFINITION(test_2382_1) {
    // 1. Open "_common_data/ace/test_new_.cap.ace".
    // 2. "Import ACE file" dialog appeared. Press ok.
    // Expected state: export succeeded, assembly is displayed.
    GTLogTracer l;

    QString sandboxDir = testDir + "_common_data/scenarios/sandbox/";
    QString assDocName = "test_2382_1.ugenedb";
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandboxDir + assDocName));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/test_new.cap.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());

    GTUtilsProjectTreeView::findIndex(os, assDocName);
}

GUI_TEST_CLASS_DEFINITION(test_2387) {
    //1) Click Open button in UGENE
    //2) Select any two valid files, for example data/samples/Genbank/ PBR322.gb and sars.gb
    //3) Click Open
    //Expected state: MultipleDocumentsReadingModeDialog appears
    //4) Select merge option
    //5) Replace one of the files with any binary file
    //6) Click OK in MultipleDocumentsReadingModeDialog.
    //Expected state: UGENE not crashed

    QString sandbox = testDir + "_common_data/scenarios/sandbox/";

    GTFile::copy(os, dataDir + "samples/Genbank/PBR322.gb", sandbox + "PBR322.gb");
    GTFile::copy(os, dataDir + "samples/Genbank/sars.gb", sandbox + "sars.gb");

    class SequenceReadingModeDialogUtils : public CustomScenario {
    public:
        //SequenceReadingModeDialogUtils(HI::GUITestOpStatus& _os) : GTSequenceReadingModeDialogUtils(_os){}
        virtual void run(HI::GUITestOpStatus &os) {
            GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
            GTFile::copy(os, testDir + "_common_data/scenarios/_regression/2387/binary.dll", testDir + "_common_data/scenarios/sandbox/sars.gb");
            GTSequenceReadingModeDialogUtils(os).commonScenario();
        }
    };

    //GTSequenceReadingModeDialogUtils "PBR322.gb" "sars.gb"
    GTUtilsDialog::waitForDialog(os, new GTSequenceReadingModeDialogUtils(os, new SequenceReadingModeDialogUtils()));
    GTFileDialog::openFileList(os, sandbox, QStringList() << "PBR322.gb"
                                                          << "sars.gb");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2392) {
    // 1. Open file _common_data/genbank/multi.gb
    // Expected state: Open dialog "Sequence reading options"
    // 2. Select "Join sequences into alignment" option and press OK
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(os, testDir + "_common_data/genbank/multi.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Document opened in MSA view
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(2 == names.size(), "Wrong sequence count");
}

GUI_TEST_CLASS_DEFINITION(test_2400) {
    //    1. Import samples/ACE/k26.ace to  ugenedb (via open file)
    QString fileName = "2400.ugenedb";
    QString ugenedb = sandBoxDir + fileName;
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, ugenedb));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/ace_test_1.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: assembly view for Contig_1 opened with refrence sequence added to it
    bool ref = GTUtilsAssemblyBrowser::hasReference(os, "2400 [as] 1");
    CHECK_SET_ERR(ref, "no reference")
}

GUI_TEST_CLASS_DEFINITION(test_2401) {
    // 1. Open the file "_common_data/ace/ace_test_1.ace".
    // 2. Set the ugenedb path for import: "_common_data/scenarios/sandbox/2401.ugenedb".
    // 3. Click OK
    QString sandbox = testDir + "_common_data/scenarios/sandbox/";
    QString fileName = "2401.ugenedb";
    QString ugenedb = sandbox + fileName;
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, ugenedb));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/ace_test_1.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the file is imported without errors, the assembly is opened.
    // 4. Close the project.
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
#ifdef Q_OS_MAC
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");
#else
    GTKeyboardDriver::keyClick('q', Qt::ControlModifier);
#endif
    GTGlobals::sleep();

    // 5. Open the file "_common_data/ace/ace_test_11_(error).ace".
    // 6. Set the same ugenedb path for import: "_common_data/scenarios/sandbox/2401.ugenedb".
    // 7. Click OK.
    // 8. Click Append.
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, ugenedb));
    GTUtilsProject::openFile(os, testDir + "_common_data/ace/ace_test_11_(error).ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the file is not imported but "2401.ugenedb" still exists.
    QString s = GUrl(ugenedb).getURLString();
    CHECK_SET_ERR(QFile::exists(s), "ugenedb file does not exist");

    // 9. Open the file "2401.ugenedb".
    GTFileDialog::openFile(os, sandbox, fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected: assembly is opened without errors.
    GTUtilsProjectTreeView::findIndex(os, fileName);
}

GUI_TEST_CLASS_DEFINITION(test_2402) {
    QDir outputDir(testDir + "_common_data/scenarios/sandbox");
    const QString outputFilePath = outputDir.absolutePath() + "/test_2402.gb";

    // 1.Open Workflow Designer
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 2.Add 'Read Sequence' element
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    // 3. Add 'Assembly Sequences witn CAP3' element
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Assembly Sequences with CAP3");
    WorkflowProcessItem *readWorker = GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence");
    WorkflowProcessItem *assemblyWorker = GTUtilsWorkflowDesigner::getWorker(os, "Assembly Sequences with CAP3");
    // 4. Connect elements
    GTUtilsWorkflowDesigner::connect(os, readWorker, assemblyWorker);

    //Expected state: all slots are connected
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Genbank/sars.gb");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Assembly Sequences with CAP3"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Well done!"));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_2403) {
    // 1. Open "human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Resize the main UGENE window that not all buttons will be visible on the sequence toolbar.
    QMainWindow *mw = AppContext::getMainWindow()->getQMainWindow();
    GTWidget::showNormal(os, mw);
    GTWidget::resizeWidget(os, mw, QSize(300, mw->size().height()));

    // 3. Click on the "Show full toolbar" button.
    QWidget *toolbarWidget = GTWidget::findWidget(os, "mwtoolbar_activemdi");
    CHECK_SET_ERR(NULL != toolbarWidget, "Toolbar  is not present");
    QWidget *expandWidget = GTWidget::findWidget(os, "qt_toolbar_ext_button", toolbarWidget);
    CHECK_SET_ERR(NULL != expandWidget, "\"Show full toolbar\" button  is not present");

    GTWidget::click(os, expandWidget);
    GTGlobals::sleep();

    //Expected: all toolbar actions appears.
    QWidget *toggleWidget = GTWidget::findWidget(os, "toggleViewButton", toolbarWidget);
    CHECK_SET_ERR(NULL != toggleWidget, "\"Toggle view\" button  is not present");

    GTWidget::click(os, expandWidget);
}

GUI_TEST_CLASS_DEFINITION(test_2404) {
    /*  1. Open human_T1.fa
    2. Open Search in sequence OP tab
    3. Input "AAAAA" pattern to the Search for: field
    4. Expand all available parameters
    Expected: scrollbar appears
    Current: layout breaks
*/
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionsPanel::runFindPatternWithHotKey("AAAAA", os);
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Save annotation(s) to"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Annotation parameters"));
    QScrollArea *sa = qobject_cast<QScrollArea *>(GTWidget::findWidget(os, "OP_SCROLL_AREA"));
    QScrollBar *scroll = sa->verticalScrollBar();
    CHECK_SET_ERR(scroll != NULL, "Scroll bar is NULL");
    CHECK_SET_ERR(scroll->isVisible(), "Scroll bar is invisible!");
}

GUI_TEST_CLASS_DEFINITION(test_2406) {
    //    1. Create the {Read Sequence -> Write Sequence} workflow.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    const QString sequenceReaderName = "Read Sequence";
    const QString sequenceWriterName = "Write Sequence";

    GTUtilsWorkflowDesigner::addAlgorithm(os, sequenceReaderName, true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, sequenceWriterName, true);

    WorkflowProcessItem *sequenceReader = GTUtilsWorkflowDesigner::getWorker(os, sequenceReaderName);
    WorkflowProcessItem *sequenceWriter = GTUtilsWorkflowDesigner::getWorker(os, sequenceWriterName);

    CHECK_SET_ERR(NULL != sequenceReader, "Sequence reader element is NULL");
    CHECK_SET_ERR(NULL != sequenceWriter, "Sequence writer element is NULL");

    GTUtilsWorkflowDesigner::connect(os, sequenceReader, sequenceWriter);

    //    2. Click on the output file field and input "TEST" file name and press Enter.
    //    Expected: TEST file name appears in the output file name field
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, sequenceWriterName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "TEST", GTUtilsWorkflowDesigner::textValue);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    GTGlobals::sleep(500);
    const QString expectedPreValue = "TEST";
    const QString resultPreValue = GTUtilsWorkflowDesigner::getParameter(os, "Output file", true);
    CHECK_SET_ERR(expectedPreValue == resultPreValue,
                  QString("Unexpected value: expected '%1' get '%2'").arg(expectedPreValue).arg(resultPreValue));

    //    3. Change the file format to the genbank
    //    Expected: TEST.gb file name appears in the output file name field
    GTUtilsWorkflowDesigner::setParameter(os, "Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);

    const QString expectedPostValue = "TEST.gb";
    const QString resultPostValue = GTUtilsWorkflowDesigner::getParameter(os, "Output file", true);
    CHECK_SET_ERR(expectedPostValue == resultPostValue,
                  QString("Unexpected value: expected '%1' get '%2'").arg(expectedPostValue).arg(resultPostValue));
}

GUI_TEST_CLASS_DEFINITION(test_2407) {
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "10000_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "10000_sequences.aln"));

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__UNLOAD_SELECTED));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitAllFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "10000_sequences.aln"));
    GTMouseDriver::doubleClick();
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2410) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 166740, 166755));
    GTGlobals::sleep(1000);

    QWidget *sequenceWidget = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_SET_ERR(NULL != sequenceWidget, "sequenceWidget is not present");

    GTWidget::click(os, sequenceWidget);
    GTKeyboardUtils::selectAll(os);

    QWidget *graphAction = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget, false);
    Runnable *chooser = new PopupChooser(os, QStringList() << "GC Content (%)");
    GTUtilsDialog::waitForDialog(os, chooser);
    GTGlobals::sleep(100);

    GTWidget::click(os, graphAction);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTAction::button(os, "action_zoom_in_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    QWidget *renderArea = GTWidget::findWidget(os, "GSequenceGraphViewRenderArea", sequenceWidget);
    const QPoint mouseInitialPos(4 * renderArea->width() / 7, renderArea->height() / 2);
    GTWidget::click(os, renderArea, Qt::LeftButton, mouseInitialPos);
    GTGlobals::sleep(200);

    const QPoint mouseInitialAbsPos = GTMouseDriver::getMousePosition();
    const int rightMouseLimit = mouseInitialAbsPos.x() * 1.3;

    for (int x = mouseInitialAbsPos.x(); x < rightMouseLimit; x += 5) {
        const QPoint currentPos(x, mouseInitialAbsPos.y());
        GTMouseDriver::moveTo(currentPos);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2415) {
    // 1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Right click on the object sequence name in the project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    // 3. Click the menu {Edit -> Rename}.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Rename"));
    GTMouseDriver::click(Qt::RightButton);

    // 4. Enter the new name: "name".
    GTKeyboardDriver::keySequence("name");

    // 5. Press Enter.
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected state: the sequence is renamed.
    GTUtilsProjectTreeView::findIndex(os, "name");
}

GUI_TEST_CLASS_DEFINITION(test_2424) {
    //     1. Open WD. Add Read Sequence, Write sequence and Filter quality (custm element with script section) workers. Make sure Fitler Quality worker's script text is empty.
    QString workflowOutputDirPath(testDir + "_common_data/scenarios/sandbox");
    QDir workflowOutputDir(workflowOutputDirPath);
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Quality Filter Example");

    WorkflowProcessItem *seqReader = GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence");
    WorkflowProcessItem *seqWriter = GTUtilsWorkflowDesigner::getWorker(os, "Write Sequence");
    WorkflowProcessItem *qualFilter = GTUtilsWorkflowDesigner::getWorker(os, "Quality Filter Example");

    GTUtilsWorkflowDesigner::connect(os, seqReader, qualFilter);
    GTUtilsWorkflowDesigner::connect(os, qualFilter, seqWriter);
    //     2. Connect workers into schema, set input data and output data (for example eas.fastq from samples)
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTA/human_T1.fa");

    const QString outputFilePath = workflowOutputDir.absolutePath() + "/test.gb";

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);
    //     3. Launch the schema.
    //     Expected state: schema didnt start, error "Quality filter example"
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep();
    GTUtilsWorkflowDesigner::checkErrorList(os, "Quality Filter Example: Empty script text");
}

GUI_TEST_CLASS_DEFINITION(test_2430) {
    //    1. Check the debug mode checkbox in the applications settings
    //    2. Add an element to the scene
    //    3. Open breakpoints manager
    //    4. Try to add the breakpoint to the element by shortcut (Ctrl+B)
    //    Current: nothing happens
    //    Expected: the breakpoint appears

    GTUtilsWorkflowDesigner::toggleDebugMode(os);
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::toggleBreakpointManager(os);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick('b', Qt::ControlModifier);

    GTGlobals::sleep();
    QStringList breakpoints = GTUtilsWorkflowDesigner::getBreakpointList(os);
    CHECK_SET_ERR(breakpoints.size() == 1, "Wrong amount of breakpoints!");
}

GUI_TEST_CLASS_DEFINITION(test_2431) {
    //1. Check the debug mode checkbox in the applications settings
    //2. Add an element to the scene
    //3. Try to delete the element by delete button
    //Current: nothing happens
    //Expected: the element will be deleted

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::toggleDebugMode(os);
    GTGlobals::sleep(1000);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(1000);
    WorkflowProcessItem *readSeq = GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(readSeq == NULL, "Item didn't deleted");
}

GUI_TEST_CLASS_DEFINITION(test_2432) {
    //    1. Check the debug mode checkbox in the applications settings
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::toggleDebugMode(os);
    GTUtilsWorkflowDesigner::toggleBreakpointManager(os);

    //    2. Add an element to the scene
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read File URL(s)");

    //    3. Add the breakpoint to the element
    GTUtilsWorkflowDesigner::setBreakpoint(os, "Read File URL(s)");

    //    4. Delete the element
    //    Expected: there are no breakpoints in the breakpoints manager
    GTUtilsWorkflowDesigner::removeItem(os, "Read File URL(s)");
    GTGlobals::sleep();
    const QStringList breakpoints = GTUtilsWorkflowDesigner::getBreakpointList(os);
    CHECK_SET_ERR(breakpoints.isEmpty(), "There are breakpoints in the workflow");
}

GUI_TEST_CLASS_DEFINITION(test_2449) {
    //    1. Open "COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Create a phylogenetic tree for the alignment.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_2449.nwk", 0, 0, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Tree"
                                                << "Build Tree");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Open tree options panel widget (it can be opened automatically after tree building).
    //    4. Open font settings on the OP widget.
    GTWidget::click(os, GTWidget::findWidget(os, "lblFontSettings"));

    //    There is a font size spinbox. You can set zero value to it: in this case font has its standard size (on mac), but this value is incorrect.
    QSpinBox *sizeSpinBox = qobject_cast<QSpinBox *>(GTWidget::findWidget(os, "fontSizeSpinBox"));
    CHECK_SET_ERR(NULL != sizeSpinBox, "Size spin box not found");

    GTWidget::setFocus(os, sizeSpinBox);
    int prev = 0;
    while (0 < sizeSpinBox->value()) {
        prev = sizeSpinBox->value();
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        GTGlobals::sleep(100);
        if (prev <= sizeSpinBox->value()) {
            break;
        }
    }

    CHECK_SET_ERR(0 < sizeSpinBox->value(), "Invalid size spin box bound");
}

GUI_TEST_CLASS_DEFINITION(test_2451) {
    //    1. Open Workflow designer
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Open sample {Alignment->Align sequences with MUSCLE}
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    //    Expected state: There is "Show wizard" tool button

    //    3. Press "Show wizard" button

    class customWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            //    4. Select input MSA "samples\CLUSTALW\COI.aln"
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << (QStringList() << dataDir + "samples/CLUSTALW/COI.aln"));
            //    5. Press "Next" button
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            //    6. Press "Run" button
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Align Sequences with MUSCLE Wizard", new customWizard()));
    GTWidget::click(os, GTAction::button(os, "Show wizard"));
    //    Expected state: Align sequences with MUSCLE Wizard appeared

    //    Expected state: Scheme successfully performed
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2459) {
    //    1. Open "COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Set any reference sequence.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Set this sequence as reference"));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton, QPoint(10, 10));

    //    3. Open context menu, open the "Highlighting" submenu, set the "Agreements" type.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Highlighting"
                                                                              << "Agreements"));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);

    //    4. Open context menu again, open the "Export" submenu, choose the "Export highlighted" menu item.
    //    Expected state: the "Export highlighted to file" dialog appears.

    //    5. Set any valid filename, other settings should be default. Click "Export".
    //    Expected state: exporting successfully completes, UGENE doesn't crash.
    //    Current state: UGENE crashes.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export"
                                                                              << "Export highlighted"));
    GTUtilsDialog::waitForDialog(os, new ExportHighlightedDialogFiller(os, sandBoxDir + "test_2459.txt"));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2460) {
    //1. Open "COI.aln".

    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Remove all sequences except the first one.
    QStringList list = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GTUtilsMsaEditor::removeRows(os, 1, list.size() - 1);

    GTGlobals::sleep(500);

    //3. Align the result one-line-msa by kalign with default values.
    GTUtilsDialog::waitForDialog(os, new KalignDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "align_with_kalign"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Kalign task finishes with error. Redo button is disabled.
    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");

    QAbstractButton *redo = GTAction::button(os, "msa_action_redo");
    CHECK_SET_ERR(NULL != redo, "There is no REDO button");
    CHECK_SET_ERR(redo->isEnabled() == false, "REDO button is enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2470) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/2470", "blast_result.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<QTreeWidgetItem *> blastResultItems = GTUtilsAnnotationsTreeView::findItems(os, "blast result");
    GTUtilsAnnotationsTreeView::selectItems(os, blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus &_os, const QString &dbPath, const QString &outputPath)
            : Filler(_os, "BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {
        }
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dbPath));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton", w));
            GTGlobals::sleep();
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "browseOutputButton", w));

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox *>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        };

    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os, testDir + "_common_data/scenarios/_regression/2470/nice_base.nhr", testDir + "_common_data/scenarios/sandbox/2470_fetched.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "fetchMenu"
                                                                        << "fetchSequenceById"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|11 Conocephalus_sp");
    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|4 Montana_montana");
    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|6 Gampsocleis_sedakovii_EF540828");
}

GUI_TEST_CLASS_DEFINITION(test_2470_1) {
    GTUtilsExternalTools::removeTool(os, "BlastAll");
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/2470", "blast_result.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<QTreeWidgetItem *> blastResultItems = GTUtilsAnnotationsTreeView::findItems(os, "blast result");
    GTUtilsAnnotationsTreeView::selectItems(os, blastResultItems);

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus &_os, const QString &dbPath, const QString &outputPath)
            : Filler(_os, "BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {};
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dbPath));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton", w));
            GTGlobals::sleep();
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, outputPath, GTGlobals::UseMouse, GTFileDialogUtils::Save));
            GTWidget::click(os, GTWidget::findWidget(os, "browseOutputButton", w));

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox *>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        };

    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(os, new OkClicker(os, testDir + "_common_data/scenarios/_regression/2470/nice_base.nhr", testDir + "_common_data/scenarios/sandbox/2470_1_fetched.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "fetchMenu"
                                                                        << "fetchSequenceById"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|11 Conocephalus_sp");
    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|4 Montana_montana");
    GTUtilsProjectTreeView::getItemCenter(os, "gnl|BL_ORD_ID|6 Gampsocleis_sedakovii_EF540828");
}

GUI_TEST_CLASS_DEFINITION(test_2475) {
    //1. Open WD.
    //2. Open Single-sample (NGS samples).
    //3. Set proper input data.
    //4. Validate scheme.
    //Expected state: validation passed.

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsDialog::waitForDialog(os, new DefaultWizardFiller(os, "Tuxedo Wizard"));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Single-sample"
                                                                                                                  << "Single-end"));
    GTUtilsWorkflowDesigner::addSample(os, "RNA-seq analysis with Tuxedo tools");
    GTGlobals::sleep();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read RNA-seq Short Reads"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bowtie2/reads_1.fq");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bowtie2/reads_2.fq");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Find Splice Junctions with TopHat"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter(os, "Bowtie index folder", testDir + "_common_data/bowtie2/index", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Bowtie index basename", "human_T1_cutted", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_2482) {
    // 1. Open "COI.nwk".
    GTFileDialog::openFile(os, dataDir + "samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Change the tree layout to unrooted.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Unrooted"));
    GTWidget::click(os, GTWidget::findWidget(os, "Layout"));

    // 3. Select any node in the tree that is not a leaf.
    QList<GraphicsButtonItem *> items = GTUtilsPhyTree::getNodes(os);
    CHECK_SET_ERR(items.size() >= 4, "Incorrect tree size");

    QPoint nodeCoords = GTUtilsPhyTree::getGlobalCenterCoord(os, items.at(3));
    GTMouseDriver::moveTo(nodeCoords);

    GTMouseDriver::click();

    // 4. Call context menu on the Tree Viewer.
    // Expected state: menu items "Swap Sublings" and "Reroot tree" are disabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Reroot tree", PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Swap Siblings", PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2487) {
    // 1. Open "COI.nwk".
    // 2. Select root-node.
    // 3. Call context menu.
    // 'Reroot tree' and 'Collapse ' options must be disabled

    GTFileDialog::openFile(os, dataDir + "samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<GraphicsButtonItem *> items = GTUtilsPhyTree::getNodes(os);
    CHECK_SET_ERR(items.size() != 0, "Tree is empty");

    QPoint rootCoords = GTUtilsPhyTree::getGlobalCenterCoord(os, items.first());
    GTMouseDriver::moveTo(rootCoords);
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Reroot tree", PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Collapse", PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2496) {
    // 1. Open file testDir/_common_data/scenarios/assembly/example-alignment.bam
    // Expected state: import bam dialog appeared

    GTUtilsDialog::waitForDialog(os,
                                 new ImportBAMFileFiller(os, testDir + "_common_data/scenarios/sandbox/example-alignment.bam.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/assembly/", "example-alignment.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_2498) {
    // 1. Open the /test/_common_data/fasta/empty.fa empty msa file.
    // 2. Open context menu on the sequence area. Go to the {Export -> amino translation}
    //
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "empty.fa");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << MSAE_MENU_EXPORT << "amino_translation_of_alignment_rows", PopupChecker::IsDisabled, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    //TODO: Expected state: this menu item is disabled for the empty msa.
}

GUI_TEST_CLASS_DEFINITION(test_2506) {
    // 1. Open file _common_data/vcf_consensus/vcf_cons_out_damaged_1.vcf
    // Expected state: error message appears.
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/vcf_consensus/", "vcf_cons_out_damaged_1.vcf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2506_1) {
    // 1. Open file _common_data/vcf_consensus/vcf_cons_out_damaged_2.vcf
    // Expected state: error message appears.
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/vcf_consensus/", "vcf_cons_out_damaged_2.vcf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2513) {
    //    Open COI.nwk.
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Switch to the circular layout on the tree view.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_TREES_WIDGET"));
    QComboBox *layoutCombo = GTWidget::findExactWidget<QComboBox *>(os, "layoutCombo");
    GTComboBox::setIndexWithText(os, layoutCombo, "Circular");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Select the last node, then call a context menu for it. It contains two menu items: "swap siblings" and "reroot".
    //The first one should be always disabled (for the tree leafs), the second one should be always enabled.
    QList<GraphicsButtonItem *> nodes = GTUtilsPhyTree::getNodes(os);
    CHECK_SET_ERR(!nodes.isEmpty(), "Nodes list is empty");

    GTUtilsPhyTree::clickNode(os, nodes[25]);
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Swap Siblings", PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Reroot tree", PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsPhyTree::clickNode(os, nodes[22]);
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes(os).isEmpty(), "A clicked node wasn't selected");
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Swap Siblings", PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Reroot tree", PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2519) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Go to position 20000.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 20000));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);

    // 3. Call context menu, select menu item {Edit sequence -> Remove subsequence...}.
    // Expected state: a "Remove subsequence" dialog appears.
    // 4. Set the region (1..190950), other settings should be default, click the "Remove" button.
    // Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REMOVE_SUBSEQUENCE));
    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1..190950"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTGlobals::sleep(2000);

    // 5. Call this dialog again, remove region (1..8999).
    // Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << ACTION_EDIT_REMOVE_SUBSEQUENCE));
    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1..8999"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_2538) {
    //    1. Open file "_common_data/scenarios/tree_view/COI.nwk"
    GTFileDialog::openFile(os, dataDir + "/samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Call context menu on node just near root. Click "Collapse"
    GTMouseDriver::moveTo(GTUtilsPhyTree::getGlobalCenterCoord(os, GTUtilsPhyTree::getNodes(os).at(1)));
    GTMouseDriver::click();
    GTGlobals::sleep(1000);

    QWidget *treeView = GTWidget::findWidget(os, "treeView");

    const QImage initImg = GTWidget::getImage(os, treeView);

    GTMouseDriver::doubleClick();
    GTGlobals::sleep(1000);

    //    3. Call context menu on node just near root. Click "Expand"
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(1000);

    //    Expected state: tree has the same view as at the beginning
    const QImage finalImg = GTWidget::getImage(os, treeView);

    //images have several pixels differ. so sizes are compared
    CHECK_SET_ERR(initImg.size() == finalImg.size(), "different images");
}

GUI_TEST_CLASS_DEFINITION(test_2540) {
    //    1. Forbid to write to the dir with the source bam(sam) file.
    QDir().mkpath(sandBoxDir + "test_2540");
    GTFile::copy(os, testDir + "_common_data/bam/chrM.sorted.bam", sandBoxDir + "test_2540/chrM.sorted.bam");
    GTFile::setReadOnly(os, sandBoxDir + "test_2540");
    //    2. Try to open this file with UGENE.
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(os, sandBoxDir + "test_2540/chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: an import dialog appeared.
    //    3. Set the destination location with enough permissions.
    //    4. Click "Import" button.
    //    Expected state: file is successfully imported.
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2542) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Lock the document
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_document_lock"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::click(Qt::RightButton);

    // 3. Do pairwise alignment with any sequences.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(1, 1));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_PAIRALIGN"));
    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Output settings"));

    QCheckBox *check = qobject_cast<QCheckBox *>(GTWidget::findWidget(os, "inNewWindowCheckBox"));
    CHECK_SET_ERR(check != NULL, "inNewWindowCheckBox not found!");
    GTCheckBox::setChecked(os, check, false);

    // State:
    // Align button not active if file locked for writing (user locking or format didn't support writing) and selected aligning in current file option
    QWidget *button = GTWidget::findWidget(os, "alignButton");

    CHECK_SET_ERR(!button->isEnabled(), "Align button is enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2543) {
    class BuildTreeDialogFiller_test_2543 : public Filler {
    public:
        BuildTreeDialogFiller_test_2543(HI::GUITestOpStatus &os, QString _saveTree = "default")
            : Filler(os, "CreatePhyTree"),
              saveTree(_saveTree) {
        }
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            if (saveTree != "default") {
                QLineEdit *saveLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "fileNameEdit"));
                GTLineEdit::setText(os, saveLineEdit, saveTree);
            }

            GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new LicenseAgreementDialogFiller(os));
            //Expected state: UGENE does not allow to create tree, the message dialog appears
            GTUtilsNotifications::waitForNotification(os, true);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }

    private:
        QString saveTree;
    };

    //1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click the "Build tree" button on the toolbar.
    //Expected state: a "Build Phylogenetic Tree" dialog appeared.
    //3. Set the output file location to any read-only folder.
    const QString outputFilePath = testDir + "_common_data/scenarios/sandbox/gui_regr_2543";
    QDir sandboxDir(testDir + "_common_data/scenarios/sandbox");
    sandboxDir.mkdir("gui_regr_2543");
    GTFile::setReadOnly(os, outputFilePath);
    GTGlobals::sleep(2000);

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller_test_2543(os, outputFilePath + "/test.nwk"));

    QAbstractButton *tree = GTAction::button(os, "Build Tree");
    GTWidget::click(os, tree);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(2000);
}

GUI_TEST_CLASS_DEFINITION(test_2544) {
    //    1. Open "data/samples/FASTA/human_T1.fa"
    GTFile::copy(os, dataDir + "samples/FASTA/human_T1.fa", sandBoxDir + "test_2544.fa");
    GTFileDialog::openFile(os, sandBoxDir + "test_2544.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    2. Use context menu { Edit sequence -> Remove subsequence... }
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT << "action_edit_remove_sub_sequences"));
    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "10..20"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getPanOrDetView(os));

    //    Expected state: "Remove subsequence" dialog has appeared
    //    3. Set string "10..20" to the "Region to remove" field, press the "Remove" button
    //    Expected state: the dialog has disappeared, subsequence has been removed

    //    4. Change permissions to the file to read-only
    GTFile::setReadOnly(os, sandBoxDir + "test_2544.fa");
    //    5. Use context menu on the document item in project view { Save selected documents }

    //    Expected state: message box has appeared
    //    6. Press "Save" button
    //    Expected state: "Save as" dialog has appeared
    //    7. Choose the same file, press "Save"
    //    Expected state: message box has appeared
    //    8. Press "Save"

    class InnerMessageBoxFiller : public MessageBoxDialogFiller {
    public:
        InnerMessageBoxFiller(HI::GUITestOpStatus &os)
            : MessageBoxDialogFiller(os, QMessageBox::Yes) {
        }
        void run() {
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Cancel, "", "permissionBox"));
            MessageBoxDialogFiller::run();
        }
    };
    class CustomSaver : public GTFileDialogUtils {
    public:
        CustomSaver(HI::GUITestOpStatus &os)
            : GTFileDialogUtils(os, sandBoxDir, "test_2544.fa", GTFileDialogUtils::Save) {
        }
        void commonScenario() {
            fileDialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::waitForDialog(os, new InnerMessageBoxFiller(os));
            setName();
            clickButton(button);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__SAVE_DOCUMENT));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Save, "", "permissionBox"));
    GTUtilsDialog::waitForDialog(os, new CustomSaver(os));
    GTUtilsProjectTreeView::click(os, "test_2544.fa", Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_2545) {
    //1. Remove the "spidey" external tool, if it exists.
    GTUtilsExternalTools::removeTool(os, "Spidey");

    //2. Open "human_t1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Call context menu, select the {Align -> Align sequence to mRna} menu item.
    //Expected state: UGENE offers to select the "spidey" external tool.
    //4. Refuse the offering.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ALIGN"
                                                                        << "Align sequence to mRNA"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    //Expected state: align task is cancelled.
    CHECK_SET_ERR(0 == GTUtilsTaskTreeView::getTopLevelTasksCount(os), "The task is not canceled");
}

GUI_TEST_CLASS_DEFINITION(test_2549) {
    //this needed to ensure that log view has text
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/gtf/invalid", "AB375112_annotations.gtf"));
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "GTF"));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Open as...");
    GTGlobals::sleep();

    GTKeyboardDriver::keyClick('3', Qt::AltModifier);
    GTGlobals::sleep();
    GTWidget::click(os, GTWidget::findWidget(os, "dock_log_view"));
    GTKeyboardDriver::keySequence("this sequence needed to ensure that log view has text");

    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    const QString clipboardContent = GTClipboard::text(os);
    CHECK_SET_ERR(!clipboardContent.isEmpty(), "Clipboard is empty");
}

GUI_TEST_CLASS_DEFINITION(test_2565) {
    //    1. Open "samples/Genbank/murine.gb".
    //    2. Press Ctrl+F.
    //    3. Insert the pattern "GCTAGCTTAAGTAACGCCACTTTT".
    //    4. Click "Search".
    //    Expected: the pattern is not found. Notification with this information appears.
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionsPanel::runFindPatternWithHotKey("GCTAGCTTAAGTAACGCCACTTTT", os);
    CHECK_SET_ERR(l.checkMessage(QString("Searching patterns in sequence task: No results found.")),
                  "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_2566) {
    //1. Open "samples/Genbank/murine.gb".
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Press Ctrl+F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTGlobals::sleep();

    //3. Enter the pattern: GCTAGCTTAAGTAACGCCAC
    QWidget *patternInputLine = QApplication::focusWidget();
    CHECK_SET_ERR(NULL != patternInputLine && patternInputLine->objectName() == "textPattern", "Focus is not on FindPattern widget");

    GTKeyboardDriver::keySequence("GCTAGCTTAAGTAACGCCAC");
    GTGlobals::sleep(1000);

    //4. Choose the algorithm: Substitute.
    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Substitute");

    //5. Enter the "Should match" value: 97%
    GTUtilsOptionPanelSequenceView::setMatchPercentage(os, 97);

    //6. Click "Search".
    // There is no "Search buttons since UGENE-3483 was done, the task is already launched
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: 97% is entered.
    const int matchPercentage = GTUtilsOptionPanelSequenceView::getMatchPercentage(os);
    CHECK_SET_ERR(97 == matchPercentage, "Entered and actual values don't match");

    //Expected state: the task finished successfully.
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2567) {
    //1. Open "samples/Genbank/murine.gb".
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Press Ctrl+F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTGlobals::sleep();

    //3. Enter the pattern: GCTAGCTTAAGTAACGCCAC
    QWidget *patternInputLine = QApplication::focusWidget();
    CHECK_SET_ERR(NULL != patternInputLine && patternInputLine->objectName() == "textPattern", "Focus is not on FindPattern widget");

    GTKeyboardDriver::keySequence("GCTAGCTTAAGTAACGCCAC");
    GTGlobals::sleep(1000);

    //4. Choose the algorithm: Substitute.
    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Substitute");

    //5. Enter the "Should match" value: 30%
    GTUtilsOptionPanelSequenceView::setMatchPercentage(os, 30);

    //6. Click "Search".
    // There is no "Search buttons since UGENE-3483 was done, the task is already launched
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: the task finished successfully.
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2568) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Add the "Call Variants" sample
    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Run the wizard

    class customFileDialog : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *d = QApplication::activeModalWidget();
            CHECK_SET_ERR(d, "activeModalWidget is NULL");
            QFileDialog *dialog = qobject_cast<QFileDialog *>(d);
            CHECK_SET_ERR(dialog, "activeModalWidget is not file dialog");

            QString name = dialog->directory().dirName();
            CHECK_SET_ERR(name == "Assembly", "unexpectyed dir name: " + name);
            GTWidget::click(os, GTWidget::findButtonByText(os, "Cancel", dialog));
        }
    };

    class customWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            //    4. Click to browse a reference file and choose a reference
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/Assembly/chrM.fa"));
            QWidget *browse = GTWidget::findWidget(os, "browseButton", GTWidget::findWidget(os, "Reference sequence file labeledWidget", dialog));
            GTWidget::click(os, browse);
            //    Expected: the file is chosen. (The file's folder is DIR)

            //    5. Click to browse a BAM/SAM file

            //    Expected: the open file dialog is opened with the folder DIR
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, new customFileDialog()));
            GTWidget::click(os, GTWidget::findWidget(os, "addFileButton"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Call Variants Wizard", new customWizard()));
    GTWidget::click(os, GTAction::button(os, "Show wizard"));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2569) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Add the Call Variants sample.
    GTUtilsWorkflowDesigner::addSample(os, "Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/chrM.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/call-variations/chrM.fa");
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    4. Click "External Tools" on the appeared Dashboard.
    GTUtilsDashboard::openTab(os, GTUtilsDashboard::ExternalTools);
    //    5. Expand "SAMtools run"
    auto runNode = GTUtilsDashboard::getExternalToolNodeByText(os, "SAMtools run");
    GTUtilsDashboard::clickNodeTitle(os, runNode);
    //    6. Check that "Command" node exists.
    GTUtilsDashboard::getExternalToolNodeByText(os, runNode, "Command");
}

GUI_TEST_CLASS_DEFINITION(test_2577) {
    //1. Open {data/samples/CLUSTALW/COI.aln}.
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Call context menu somewhere on the alignment, select {Consenssus mode...} menu item.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Consensus mode", GTGlobals::UseMouse));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));

    //Expected state: options panel "General" tab opens.
    QWidget *panel = GTWidget::findWidget(os, "OP_MSA_GENERAL");
    CHECK_SET_ERR(NULL != panel, "General OP tab does not appear");
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_2578) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open options panel 'Highlighting' tab.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));

    //    3. Select 'agreements' highlighting scheme.
    QComboBox *combo = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "highlightingScheme"));
    CHECK_SET_ERR(combo != NULL, "highlightingScheme not found!");
    GTComboBox::setIndexWithText(os, combo, "Agreements");

    QWidget *exportButton = GTWidget::findWidget(os, "exportHighlightning");
    CHECK_SET_ERR(NULL != exportButton, "exportButton not found");
    CHECK_SET_ERR(!exportButton->isEnabled(), "exportButton is enabled unexpectedly");

    //    4. Select any reference sequence.
    //    Expected state: the "Export" button is active.
    GTWidget::click(os, GTWidget::findWidget(os, "sequenceLineEdit"));
    GTKeyboardDriver::keySequence("Montana_montana");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    CHECK_SET_ERR(exportButton->isEnabled(), "exportButton is disabled unexpectedly");

    //    5. Select 'General' tab, then select 'Highlighting' tab again.
    //    Expected state: the "Export" button is active.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_GENERAL"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));

    exportButton = GTWidget::findWidget(os, "exportHighlightning");
    CHECK_SET_ERR(NULL != exportButton, "exportButton not found");
    CHECK_SET_ERR(exportButton->isEnabled(), "exportButton is disabled unexpectedly");
}

GUI_TEST_CLASS_DEFINITION(test_2579) {
    //  Linux:
    //  1. Click the menu Settings -> Preferences -> External Tools.
    //  2. Find the MAFFT item and if it has the active red cross, click it.
    //  3. Click the MAFFT's browse button and add the MAFFT executable from UGENE external tools package.
    //  Expected: there are no errors in the log.

    class MafftInactivation : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QString path = AppSettingsDialogFiller::getExternalToolPath(os, "MAFFT");
            AppSettingsDialogFiller::clearToolPath(os, "MAFFT");
            AppSettingsDialogFiller::setExternalToolPath(os, "MAFFT", path);

            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MafftInactivation()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");
    GTGlobals::sleep();
    GTUtilsLog::check(os, l);
}
GUI_TEST_CLASS_DEFINITION(test_2581) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with MUSCLE }
    // Expected state: the "Align with MUSCLE" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os, MuscleDialogFiller::Default));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with muscle"));

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2581_1) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with ClustalW }
    // Expected state: the "Align with ClustalW" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash

    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new ClustalWDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with ClustalW", GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2581_2) {
    //    1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped_same_names.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Use context menu { Align -> Align with ClustalO }
    //    Expected state: the "Align with Clustal Omega" dialog has appeared

    //    3. Press the "Align" button
    //    Expected state: after a few seconds alignment has finished, UGENE does not crash
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align"
                                                                              << "Align with ClustalO..."));
    GTUtilsDialog::waitForDialog(os, new ClustalOSupportRunDialogFiller(os));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);

    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_2581_3) {
    //    1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "ma2_gapped_same_names.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Use context menu { Align -> Align with MAFFT }
    //    Expected state: the "Align with MAFFT" dialog has appeared

    //    3. Press the "Align" button
    //    Expected state: after a few seconds alignment has finished, UGENE does not crash
    MAFFTSupportRunDialogFiller::Parameters parameters;
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align"
                                                                              << "Align with MAFFT..."));
    GTUtilsDialog::waitForDialog(os, new MAFFTSupportRunDialogFiller(os, &parameters));
    GTWidget::click(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os), Qt::RightButton);

    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_2581_4) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with T-Coffee }
    // Expected state: the "Align with T-Coffee" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash

    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new TCoffeeDailogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with T-Coffee", GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2581_5) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with Kalign }
    // Expected state: the "Align with Kalign" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash

    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new KalignDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "align_with_kalign", GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2583) {
    //    1. Open file data/samples/EMBL/AL000263.emb
    GTFileDialog::openFile(os, dataDir + "samples/EMBL/AL000263.emb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Open file test/_common_data/regression/2583/My_Document_2.gb
    GTFileDialog::openFile(os, testDir + "_common_data/regression/2583/MyDocument_2.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    3. Drag "AB000263 standart annotations" AL000263.emb sequence view
    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os, "AB000263 standard annotations");
    QWidget *sequence = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(sequence != NULL, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idx, sequence);
    //    "Edit objct relations" dialog appeared. Click "OK"
    //    4. Select all "blast result" tree items in annotation tree view
    QList<QTreeWidgetItem *> blastResultItems = GTUtilsAnnotationsTreeView::findItems(os, "blast result");
    GTUtilsAnnotationsTreeView::selectItems(os, blastResultItems);
    //    5. Use context menu: "Export"->"Export BLAST result to alignment"

    GTUtilsDialog::waitForDialog(os, new ExportBlastResultDialogFiller(os, sandBoxDir + "test_2583/test_2583.aln", true));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_EXPORT"
                                                                        << "export_BLAST_result_to_alignment"));
    GTMouseDriver::click(Qt::RightButton);
    //    "Export BLAST result to alignment" dialog appeared. Set some output file.
    //    check "add reference to alignment" checkBox
    //    Click "Export"
    //    Expected state: aligned parts are different with same parts in ref sequence.//Kirill can give more comments
    GTUtilsProjectTreeView::openView(os);
    GTUtilsProjectTreeView::toggleView(os);
    GTUtilsMSAEditorSequenceArea::checkSelection(os, QPoint(30, 1), QPoint(41, 1), "TGCGGCTGCTCT");
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_2605) {
    GTLogTracer logTracer;
    // 1. Open file _common_data/fasta/multy_fa.fa as multiple alignment
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Export subalignment from this msa to any MSA format
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment"));
    GTUtilsDialog::waitForDialog(os, new ExtractSelectedAsMSADialogFiller(os, testDir + "_common_data/scenarios/sandbox/2605.aln", QStringList() << "SEQUENCE_1", 6, 237));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_editor_sequence_area"));

    // Expected state: export successfull, no any messages in log like "There is no sequence objects in given file, unable to convert it in multiple alignment"
    CHECK_SET_ERR(!logTracer.hasErrors(), "Errors in log: " + logTracer.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2612) {
    // 1. Open sequence "samples/fasta/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Search for 20 first nucleotides (TTGTCAGATTCACCAAAGTT) using Find Pattern.
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTGTCAGATTCACCAAAGTT", os);
    GTGlobals::sleep(1000);
    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));
    GTGlobals::sleep(500);
    // Expected state: the annotation with pattern created and shown in sequence view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTGlobals::sleep(1000);
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "Misc. Feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    // 3. Delete annotation from annotation editor.
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: there is no annotation in sequence view.
    GTGlobals::sleep(100);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "Misc. Feature", GTGlobals::FindOptions(false)) == NULL, "Annotations document not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_2619) {
    //    1. Open file samples/genbank/sars.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open context menu for any qualifier on annotation table view.
    //    Expected state: submenu "Copy" didn't contains items "Edit qualifier" and "Add 'evidence' column"
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_COPY << "edit_qualifier_action", PopupChecker::NotExists));
    GTUtilsAnnotationsTreeView::callContextMenuOnQualifier(os, "5'UTR", "evidence");
    GTGlobals::sleep(500);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_COPY << "toggle_column", PopupChecker::NotExists));
    GTUtilsAnnotationsTreeView::callContextMenuOnQualifier(os, "5'UTR", "evidence");
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_2622) {
    // 1. Open "_common_data/fasta/multy_fa.fa".
    // 2. Choose "Merge sequences ..." with 100 bases.
    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge, 100));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Press Ctrl + F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTGlobals::sleep();

    // 4. Choose "Regular expression" algorithm.
    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Regular expression");

    // 5. Write "X+" in the pattern string.
    QWidget *textPattern = GTWidget::findWidget(os, "textPattern");
    GTWidget::click(os, textPattern);
    GTKeyboardDriver::keyClick('X');
    GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);

    // UGENE does not hang.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2622_1) {
    // 1. Open "_common_data/fasta/multy_fa.fa".
    // 2. Choose "Merge sequences ..." with 100 bases.
    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge, 100));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Press Ctrl + F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTGlobals::sleep();

    // 4. Choose "Regular expression" algorithm.
    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Regular expression");

    // 5. Write "X+" in the pattern string.
    QWidget *textPattern = GTWidget::findWidget(os, "textPattern");
    GTWidget::click(os, textPattern);
    GTKeyboardDriver::keyClick('X');
    GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);

    // 6. Check "Results no longer than" and set the value 1.
    QCheckBox *boxUseMaxResultLen = qobject_cast<QCheckBox *>(GTWidget::findWidget(os, "boxUseMaxResultLen"));
    GTCheckBox::setChecked(os, boxUseMaxResultLen, true);

    QSpinBox *boxMaxResultLen = qobject_cast<QSpinBox *>(GTWidget::findWidget(os, "boxMaxResultLen"));
    GTSpinBox::setValue(os, boxMaxResultLen, 1, GTGlobals::UseKeyBoard);

    // UGENE does not hang and all results are 1 bp length (100 results).
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, l);

    QLabel *resultLabel = qobject_cast<QLabel *>(GTWidget::findWidget(os, "resultLabel"));
    CHECK_SET_ERR(resultLabel->text() == "Results: 1/100", "Unexpected find algorithm results");
}

GUI_TEST_CLASS_DEFINITION(test_2632) {
    //    1. Opened the full tuxedo wizard
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            //    2. Go to the second page
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    3. Set bowtie index and a known transcript file.
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/bowtie/index/e_coli.1.ebwt"));
            GTWidget::click(os, GTWidget::findButtonByText(os, "Select\nbowtie index file", dialog));
            GTUtilsWizard::setParameter(os, "Known transcript file", QVariant(QDir().absoluteFilePath(testDir + "_common_data/gtf/valid.gtf")));

            //    4. Revern to first page, then click next again
            //    UGENE crashes
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Back);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Full"
                                                                                                                  << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Tuxedo Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample(os, "RNA-seq analysis with Tuxedo tools");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2638) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Select "tuxedo" sample
    //    3. Set proper input data.
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_1000.fastq"), map));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Single-sample"
                                                                                                                  << "Single-end"));
    GTUtilsWorkflowDesigner::addSample(os, "RNA-seq analysis with Tuxedo tools");
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::click(os, "Assemble Transcripts with Cufflinks");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(200);

    //    Launch pipeline
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    4. Open "input" tab on dashboard
    QString initTitle = GTUtilsMdi::activeWindowTitle(os);
    GTUtilsDashboard::openTab(os, GTUtilsDashboard::Input);
    GTWebView::traceAllWebElements(os, GTUtilsDashboard::getDashboardWebView(os));
    GTUtilsDashboard::click(os, GTUtilsDashboard::findWebElement(os, "Map RNA-Seq Reads with TopHat", "LI"));
    GTUtilsDashboard::click(os, GTUtilsDashboard::findWebElement(os, "index", "BUTTON"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: "Bowtie index folder" parameter's value is folder, it is not tried to be opened bu UGENE when clicking
    QString finalTitle = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(initTitle == finalTitle, "New window was opened unexpectidly: " + finalTitle);
}

GUI_TEST_CLASS_DEFINITION(test_2640) {
    //    0. Set CPU optimisation in settings dialog
    GTGlobals::sleep();
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "dialog is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);
            QSpinBox *cpuBox = GTWidget::findExactWidget<QSpinBox *>(os, "cpuBox", dialog);
            GTSpinBox::setValue(os, cpuBox, 94, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new custom));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings"
                                                << "Preferences...");
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Select "tuxedo" sample
    //    3. Set proper input data
    QString expected = "tophat-2.1.1/tophat -p 94 --output-dir";
    ;

    GTLogTracer l(expected);
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_1000.fastq"), map));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Single-sample"
                                                                                                                  << "Single-end"));
    GTUtilsWorkflowDesigner::addSample(os, "RNA-seq analysis with Tuxedo tools");
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::click(os, "Assemble Transcripts with Cufflinks");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTThread::waitForMainThread();

    //    Launch pipeline
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state: tophat launched with argument -p
    GTUtilsLog::checkContainsMessage(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2651) {
    // 1. File->Search NCBI GenBank...
    // 2. In the search field paste
    // AB797204.1 AB797210.1 AB797201.1
    // 3. Click Search
    // 4. Select three results and download them
    // 5. Close the dialog
    GTLogTracer l;

    QList<int> resultNumbersToSelect;
    resultNumbersToSelect << 0 << 1 << 2;
    const QVariant variantNumbers = QVariant::fromValue<QList<int>>(resultNumbersToSelect);
    const QVariant searchField = QVariant::fromValue<QPair<int, QString>>(QPair<int, QString>(0, "AB797204.1 AB797210.1 AB797201.1"));

    QList<DownloadRemoteFileDialogFiller::Action> remoteDialogActions;
    remoteDialogActions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::ClickOk, QVariant());
    const QVariant remoteDialogActionsVariant = QVariant::fromValue<QList<DownloadRemoteFileDialogFiller::Action>>(remoteDialogActions);

    QList<NcbiSearchDialogFiller::Action> actions;
    actions << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::SetTerm, searchField)
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::ClickSearch, QVariant())
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::WaitTasksFinish, QVariant())
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::SelectResultsByNumbers, variantNumbers)
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::ClickDownload, remoteDialogActionsVariant)
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::ClickClose, QVariant());
    GTUtilsDialog::waitForDialog(os, new NcbiSearchDialogFiller(os, actions));

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Search NCBI GenBank...");

    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 6. With Ctrl pressed, select all three annotation objects in the project view
    GTUtilsProjectTreeView::openView(os);
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    GTUtilsProjectTreeView::click(os, "AB797210 features");
    GTUtilsProjectTreeView::click(os, "AB797204 features");
    GTUtilsProjectTreeView::click(os, "AB797201 features");

    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // 7. delete this objects through context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__remove_selected_action"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : the objects are deleted, the popup is shown
    GTGlobals::FindOptions safeOptions(false);
    const QModelIndex firstIndex = GTUtilsProjectTreeView::findIndex(os, "AB797210 features", safeOptions);
    CHECK_SET_ERR(!firstIndex.isValid(), "The \"AB797210 features\" item has not been deleted");
    const QModelIndex secondIndex = GTUtilsProjectTreeView::findIndex(os, "AB797204 features", safeOptions);
    CHECK_SET_ERR(!secondIndex.isValid(), "The \"AB797204 features\" item has not been deleted");
    const QModelIndex thirdIndex = GTUtilsProjectTreeView::findIndex(os, "AB797201 features", safeOptions);
    CHECK_SET_ERR(!thirdIndex.isValid(), "The \"AB797201 features\" item has not been deleted");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2656) {
    //    0. Create a file with an empty sequence. A FASTA file with the first line ">seq1" and the empty second line
    //    1. Open any sequence
    //    2. On the toolbar, press "Build dotplot"
    //    3. Press "Load sequence" in the dialog. Load the empty sequence
    //    4. Select the empty sequence as the second sequence in the combobox

    //    Bug: UGENE crashes
    //    Expected: an error message is shown
    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class DotplotLoadSequenceFiller : public Filler {
    public:
        DotplotLoadSequenceFiller(HI::GUITestOpStatus &os, const QString seqPath, const QString seqName)
            : Filler(os, "DotPlotDialog"), seqPath(seqPath), seqName(seqName) {
        }
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QPushButton *loadSeq = qobject_cast<QPushButton *>(GTWidget::findWidget(os, "loadSequenceButton", dialog));
            CHECK_SET_ERR(loadSeq != NULL, "Load sequence button no found");
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, seqPath, seqName));
            GTWidget::click(os, loadSeq);

            QDialogButtonBox *box = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox", dialog));
            QPushButton *button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button != NULL, "Cancel button is NULL");
            GTWidget::click(os, button);
        }

    private:
        QString seqPath;
        QString seqName;
    };

    GTUtilsDialog::waitForDialog(os, new DotplotLoadSequenceFiller(os, testDir + "_common_data/fasta", "empty_2.fa"));
    GTWidget::click(os, GTWidget::findWidget(os, "build_dotplot_action_widget"));
    GTThread::waitForMainThread();

    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2662) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Open Call Variants sample.
    GTUtilsWorkflowDesigner::addSample(os, "Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bam/chrM.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/call-variations/chrM.fa");
    //    4. Start the scheme.
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    5. Open External Tools tab.
    GTUtilsDashboard::openTab(os, GTUtilsDashboard::ExternalTools);
    //    Expected state: vcfTools executible file is /usr/bin/perl path/to/vcfutils.pl
    //    Actual: vcfTools executible file is /usr/bin/perl

    auto node = GTUtilsDashboard::getExternalToolNodeByText(os, "vcfutils run");
    GTWidget::click(os, node);

#ifdef Q_OS_WIN
    GTUtilsDashboard::getExternalToolNodeByText(os, "samtools-0.1.19\\vcfutils.pl", false);
#else
    GTUtilsDashboard::getExternalToolNodeByText(os, "samtools-0.1.19/vcfutils.pl", false);
#endif
}

GUI_TEST_CLASS_DEFINITION(test_2667) {
    //    1. Open {/data/samples/genbank/murine.gb}.
    //    Expected state: a document was added, it contains two object: an annotation and a sequence
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Select the annotation object in the project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::click();

    //    3. Press "delete" key.
    //    Expected state: the annotation object is removed from the document.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    GTUtilsProjectTreeView::findIndex(os, "NC_001363 features", options);
}

GUI_TEST_CLASS_DEFINITION(test_2683) {
    //    1. Open Tuxedo sample scheme in WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            QString title = GTUtilsWizard::getPageTitle(os);
            CHECK_SET_ERR(title == "Cuffdiff Samples", "unexpected title: " + title);

            GTWidget::findWidget(os, "Sample1", dialog);
            GTWidget::findWidget(os, "Sample2", dialog);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Full"
                                                                                                                  << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Tuxedo Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample(os, "RNA-seq analysis with Tuxedo tools");
    //    Expected state: in appeared wizard there is a page "Cuffdiff Samples"
    //    where user can divide input datasets into samples for Cuffdiff tool.
}

GUI_TEST_CLASS_DEFINITION(test_2690) {
    //    1. Open "human_t1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Create an annotation: Group name - "1", location - "1..1".
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "1", "ann1", "1..1"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");

    //    3. Create an annotation: Group name - "2", location - "5..5, 6..7".
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "2", "ann2", "5..5, 6..7"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");

    //    4. Open the "Annotation highlighting" OP widget.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT"));
    //    5. Select the first annotation.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "ann1"));
    GTMouseDriver::click();
    //    6. Click the "next annotation" button.
    QWidget *nextAnnotationButton = GTWidget::findWidget(os, "nextAnnotationButton");
    GTWidget::click(os, nextAnnotationButton);
    GTGlobals::sleep(500);
    //    Expected state: the second annotation is selected.
    QString str = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(str == "ann2", "unexpected selected annotation after click: " + str);
    CHECK_SET_ERR(!nextAnnotationButton->isEnabled(), "nextAnnotationButton is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2701) {
    //    1. Open {/data/samples/genbank/CVU55762.gb}.
    //    Expected state: a document was added, circular view is opened

    //    2. Click on Save circular view as image.
    //    Expected state: Opened Export Image dialog

    //    3. Select vector format (svg, pdf or ps)
    //    Expected state: Quality tuning slider is not showed.

    //    4. Select jpg format
    //    Expected state: Quality tuning slider is showed.
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    ADVSingleSequenceWidget *wgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(wgt != NULL, "No sequence widget");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, wgt), "No CV opened");

    class ImageQualityChecker : public Filler {
    public:
        ImageQualityChecker(HI::GUITestOpStatus &os)
            : Filler(os, "ImageExportForm") {
        }
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QComboBox *formatsBox = dialog->findChild<QComboBox *>("formatsBox");
            QWidget *spin = dialog->findChild<QSpinBox *>("qualitySpinBox");

            GTComboBox::setIndexWithText(os, formatsBox, "SVG");
            CHECK_SET_ERR(!spin->isVisible(), "Quality spin box is visible!");
            GTComboBox::setIndexWithText(os, formatsBox, "PDF");
            CHECK_SET_ERR(!spin->isVisible(), "Quality spin box is visible!");
            GTComboBox::setIndexWithText(os, formatsBox, "PS");
            CHECK_SET_ERR(!spin->isVisible(), "Quality spin box is visible!");

            GTComboBox::setIndexWithText(os, formatsBox, "JPG");
            CHECK_SET_ERR(spin->isVisible(), "Quality spin box not visible!");

            QDialogButtonBox *box = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");
            QPushButton *button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button != NULL, "Cancel button is NULL");
            GTWidget::click(os, button);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ImageQualityChecker(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "Save circular view as image", GTGlobals::UseMouse));

    QWidget *circularView = GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0");
    CHECK_OP_SET_ERR(os, "Failed to open circular view!");
    GTWidget::click(os, circularView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2709) {
    //    1. Open tuxedo sample "no-new-transcripts"

    class test_2709_canceler : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            GTGlobals::sleep();
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "No-new-transcripts"
                                                                                                                  << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Tuxedo Wizard", new test_2709_canceler()));
    GTUtilsWorkflowDesigner::addSample(os, "tuxedo");
    GTGlobals::sleep();
    //    Expected state: "No novel junctions" tophat parameter set to true by default
    GTUtilsWorkflowDesigner::click(os, "Map RNA-Seq Reads with TopHat");
    QString result = GTUtilsWorkflowDesigner::getParameter(os, "No novel junctions");
    CHECK_SET_ERR(result == "True", "No novel junctions parameter is " + result);
}

GUI_TEST_CLASS_DEFINITION(test_2713) {
    //    1. Open file {data/samples/Genbank/murine.gb}
    GTFile::copy(os, dataDir + "samples/Genbank/murine.gb", sandBoxDir + "test_2713.gb");
    GTFileDialog::openFile(os, sandBoxDir, "test_2713.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open file {data/samples/FASTA/human_T1.fa}
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Drag and drop annotation object "NC_001363 features" from project view to sequence view
    //    Expected state: the "Edit Object Relations" dialog has appeared
    //    4. Press "OK"
    //    Expected state: annotations has appeared on the sequence view
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTWidget::click(os, GTUtilsProjectTreeView::getTreeView(os));

    QPoint point = GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features");
    point.setX(point.x() + 1);
    point.setY(point.y() + 1);
    GTMouseDriver::moveTo(point);
    GTMouseDriver::click();

    QPoint endPoint = GTWidget::getWidgetCenter(GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTMouseDriver::dragAndDrop(point, endPoint);

    GTUtilsDialog::waitAllFinished(os);

    //    5. Open file {data/samples/Genbank/murine.gb} with text editor, then make some modification and save file
    //    Expected state: dialog about detected file modification has appeared in UGENE window
    //    6. Press "Yes"
    //    Expected state: "human_T1" view has disappeared from the "Bookmarks" list, "murine.gb" has been reloaded.
    const GUIDialogWaiter::WaitSettings waitSettings("");
    GTUtilsDialog::waitForDialog(os,
                                 new MessageBoxDialogFiller(os,
                                                            QMessageBox::Yes,
                                                            "Document 'test_2713.gb' was modified. Do you want to reload it?",
                                                            ""),
                                 waitSettings);

    QFile file(sandBoxDir + "/test_2713.gb");
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
    CHECK_SET_ERR(opened, "Can't open the file: " + sandBoxDir + "test_2713.gb");
    QByteArray fileData = file.readAll();
    file.close();

    fileData.replace("gag polyprotein", "ggg_polyprotein");

    GTGlobals::sleep(1000);    // wait at least 1 second: UGENE does not detect file changes within 1 second interval.
    opened = file.open(QIODevice::WriteOnly);
    CHECK_SET_ERR(opened, "Can't open the file: " + sandBoxDir + "test_2713.gb");
    file.write(fileData);
    file.close();

    GTUtilsDialog::waitAllFinished(os);
    //    7. Open "human_T1" sequence view
    //    Expected state: annotations from "murine.gb" present on the sequence view
    GTUtilsProjectTreeView::doubleClickItem(os, "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::findFirstAnnotation(os);
}

GUI_TEST_CLASS_DEFINITION(test_2721) {
    //Note: Make sure first that "data" contains "cistrome" folder with proper data.

    //1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //2. Select Cistrome example.
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //Expected state: wizard appeared - on the first page "Cistrome data folder" is set to "/cistrome".
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "activeModalWidget is NULL");

            QLineEdit *urlLineEdit = GTWidget::findExactWidget<QLineEdit *>(os, "urlLineEdit", dialog);
            QString url = urlLineEdit->text();
            CHECK_SET_ERR(url.contains("/cistrome"), "unexpected url: " + url);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Cistrome Workflow", new custom()));
    GTUtilsWorkflowDesigner::addSample(os, "Cistrome");
}

GUI_TEST_CLASS_DEFINITION(test_2726) {
    //1. Open "COI.aln".
    //2. Select the second symbol at the first line.
    //3. Press backspace twice.
    //Expected state: undo and redo buttons are disabled.
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(1, 0), QPoint(1, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    //Expected state : MSA view has opened, the "Undo" button is disabled.
    QAbstractButton *undoButton = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");
    //Expected state : MSA view has opened, the "Redo" button is disabled.
    QAbstractButton *redoButton = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!redoButton->isEnabled(), "'Redo' button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2729) {
    //    1. Open {_common_data/fasta/AMINO.fa}
    //    Expected state: there is no a "Graphs" button on the sequence toolbar for amino, it is invisible and disabled.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "AMINO.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QAbstractButton *graphsButton = GTAction::button(os, "GraphMenuAction", GTUtilsSequenceView::getSeqWidgetByNumber(os));
    CHECK_SET_ERR(NULL != graphsButton, "Graphs button is NULL");
    CHECK_SET_ERR(!graphsButton->isEnabled(), "Graphs button is unexpectedly enabled");

    //    2. Click the "Graphs" button.
    //    Expected state: menu is shown.
    //    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()));
    //    GTWidget::click(os, graphsButton);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2730) {
    /*  1. Open "_common_data/fasta/abcd.fa" as separate sequences in the sequence viewer.
    2. Click the "Automatic Annotation Highlighting" button on the first sequence's toolbar.
    3. Click the "Plasmid features" button.
    Expected: UGENE does not crash.
*/
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/abcd.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);
    QWidget *parent = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_SET_ERR(parent != NULL, "ADV_single_sequence_widget_0 not found!");

    QWidget *menuAction = GTWidget::findWidget(os, "AutoAnnotationUpdateAction", parent);
    CHECK_SET_ERR(menuAction != NULL, "AutoAnnotationUpdateAction not found!");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Plasmid features"));
    GTWidget::click(os, menuAction);
    GTGlobals::sleep(500);
    //Close file - UGENE does not crash.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "abcd.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_2737) {
    //1. Open any sequence without annotations (e.g. "_common_data/fasta/AMINO.fa")
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "AMINO.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Add few annotations with different names.
    GTUtilsAnnotationsTreeView::createAnnotation(os, "group", "name1", "1..10");
    GTUtilsAnnotationsTreeView::createAnnotation(os, "group", "name2", "11..20", false);
    GTUtilsAnnotationsTreeView::createAnnotation(os, "group", "name3", "21..30", false);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Open Annotation Highlighting tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    //4. Delete all annotations one by one.
    GTUtilsAnnotationsTreeView::deleteItem(os, "name1");
    GTUtilsAnnotationsTreeView::deleteItem(os, "name2");
    GTUtilsAnnotationsTreeView::deleteItem(os, "name3");

    //Expected state: there is no annotations is annotation tree.
    QWidget *annotationsTree = GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(!annotationsTree->isVisible(), "Annotations tree is shown");
}

GUI_TEST_CLASS_DEFINITION(test_2737_1) {
    GTLogTracer l;
    //1. Open "murine.gb";
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //1.1. Open Annotation Highlighting tab.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    //2. Delete all annotations in random order;
    QTreeWidgetItem *annotation = NULL;
    while (NULL != (annotation = GTUtilsAnnotationsTreeView::findFirstAnnotation(os, GTGlobals::FindOptions(false)))) {
        uiLog.trace("annotation text is: " + annotation->text(0));
        GTUtilsAnnotationsTreeView::deleteItem(os, annotation);
        GTGlobals::sleep();
    }

    //Expected state: no errors in the log
    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2754) {
    //1. Open "murine.gb";
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "activeModalWidget is NULL");

            QAbstractButton *next = GTWidget::findButtonByText(os, "Next", dialog);
            CHECK_SET_ERR(!next->isEnabled(), "Next button is unexpectidly enabled");

            QAbstractButton *selectAll = GTWidget::findButtonByText(os, "Select all", dialog);
            CHECK_SET_ERR(!selectAll->isEnabled(), "Select all button is unexpectidly enabled");

            GTWidget::click(os, GTWidget::findButtonByText(os, "Close", dialog));
        }
    };

    GTUtilsDialog::waitForDialog(os, new FindQualifierFiller(os, new custom()));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "find_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2761_1) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    QDir().mkpath(sandBoxDir + "test_2761_1");
    GTFile::setReadOnly(os, sandBoxDir + "test_2761_1");
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Select some area in the MSA.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(2, 2), QPoint(5, 5));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment"));
    //    3. Context menu of the area -> Export -> Save subalignment.

    class customFiller : public ExtractSelectedAsMSADialogFiller {
    public:
        customFiller(HI::GUITestOpStatus &os)
            : ExtractSelectedAsMSADialogFiller(os, testDir + "_common_data/scenarios/sandbox/test_2761_1/2761.aln", QStringList() << "Bicolorana_bicolor_EF540830"
                                                                                                                                  << "Roeseliana_roeseli") {
        }
        void run() {
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "No write permission to"));
            ExtractSelectedAsMSADialogFiller::run();
            GTGlobals::sleep(1000);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new customFiller(os));
    GTMouseDriver::click(Qt::RightButton);
    //    4. Set the destination path to the dir without write permissions.
    //    5. Click "Extract".
    //    Expected: the message about write permissions to the dir appears. The extraction task is not run.
}

GUI_TEST_CLASS_DEFINITION(test_2761_2) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Select some area in the MSA.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(2, 2), QPoint(5, 5));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_EXPORT << "Save subalignment"));
    //    3. Context menu of the area -> Export -> Save subalignment.

    class customFiller : public ExtractSelectedAsMSADialogFiller {
    public:
        customFiller(HI::GUITestOpStatus &os)
            : ExtractSelectedAsMSADialogFiller(os, testDir + "_common_data/scenarios/sandbox/test_2761_2/2761.aln", QStringList() << "Bicolorana_bicolor_EF540830"
                                                                                                                                  << "Roeseliana_roeseli") {
        }
        void run() {
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Folder to save does not exist"));
            ExtractSelectedAsMSADialogFiller::run();
            GTGlobals::sleep(1000);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new customFiller(os));
    GTMouseDriver::click(Qt::RightButton);
    //    4. Set the destination path to the dir that does not exists
    //    5. Click "Extract".
    //    Expected: the message about write permissions to the dir appears. The extraction task is not run.
}

GUI_TEST_CLASS_DEFINITION(test_2762) {
    /*  1. Open something, e.g. "human_T1.fa".
    2. Close the project.
        Expected state: a dialog will appear that offer you to save the project.
    3. Press escape key.
        Expected state: the dialog will closed as canceled.
*/
    class EscClicker : public Filler {
    public:
        EscClicker(HI::GUITestOpStatus &_os)
            : Filler(_os, "SaveProjectDialog") {
        }
        virtual void run() {
            GTGlobals::sleep();
#ifdef Q_OS_MAC
            QDialogButtonBox *buttonBox = qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox"));
            QAbstractButton *cancel = buttonBox->button(QDialogButtonBox::Cancel);
            GTWidget::click(os, cancel);
#else
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
#endif
        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new EscClicker(os));
    GTGlobals::sleep(1000);
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");

    GTUtilsProject::checkProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_2770) {
    //1. File -> New document from text.
    //2. Data: TTTTTTTTTTTTTTTTTTTTTTTAAATTTTTTTTTTTTTTTTTTTTTTT
    //Location: set the valid output file.
    //3. Create.
    //4. Ctrl + F.
    //5. Pattern: TTTTTTTTTTTTTTTTTTTTTTTAATTTTTTTTTTTTTTTTTTTTTTT
    //Algorithm: InsDel
    //Should match: 30%
    //6. Search.
    //Expected: two annotations are found.
    Runnable *filler = new CreateDocumentFiller(os,
                                                "TTTTTTTTTTTTTTTTTTTTTTTAAATTTTTTTTTTTTTTTTTTTTTTT",
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

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "New document from text...",
                              GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(5000);

    GTUtilsDocument::checkDocument(os, "result");

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, "TTTTTTTTTTTTTTTTTTTTTTTAATTTTTTTTTTTTTTTTTTTTTTT", true);

    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "InsDel");

    GTUtilsOptionPanelSequenceView::setMatchPercentage(os, 30);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem *annotationGroup = GTUtilsAnnotationsTreeView::findItem(os, "Misc. Feature  (0, 2)");
    CHECK_SET_ERR(NULL != annotationGroup, "Annotations have not been found");
}

GUI_TEST_CLASS_DEFINITION(test_2773) {
    // 1. Open file test/_common_data/cmdline/custom-script-worker-functions/translateTest/translateTest.uwl
    // 2. Set parameters:
    // input: _common_data/cmdline/DNA.fa
    // offset: sss
    // out: some/valid/path
    // 3. run sheme.
    // Expected state: UGENE doesn't crash, error message appears.

    //need to copy enlement to data dir
#ifdef Q_OS_LINUX
    QFile::copy(testDir + "_common_data/cmdline/_proto/translateTest.usa", "../../data/workflow_samples/users/translateTest.usa");
#else
    GTFile::copy(os, testDir + "_common_data/cmdline/_proto/translateTest.usa", dataDir + "/workflow_samples/users/translateTest.usa");
#endif

    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/cmdline/custom-script-worker-functions/translateTest/translateTest.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/DNA.fa");

    GTUtilsWorkflowDesigner::click(os, "translateTest");
    GTUtilsWorkflowDesigner::setParameter(os, "offset", "sss", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click(os, "Write sequence");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", "_common_data/cmdline/result_test_offset", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTGlobals::sleep(1000);

    GTUtilsLog::check(os, l);
}
GUI_TEST_CLASS_DEFINITION(test_2778) {
    //1. Use main menu : tools->align to reference->align short reads
    //2. Set input parameters
    //input sequence : _common_data / genome_aligner / chrY.fa
    //short reads : _common_data / genome_aligner / shortreads15Mb.fasta
    //mismatches allowed : checked
    //3. Press start

    //Expected state : the task should be finished without errors.

    GTLogTracer l;

    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(testDir + "_common_data/genome_aligner/",
                                                               "chrY.fa",
                                                               testDir + "_common_data/genome_aligner/",
                                                               "shortreads1Mb.fasta",
                                                               true);
    parameters.samOutput = false;
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2784) {
    GTLogTracer lt;

    //1. Open the file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : MSA view has opened, the "Undo" button is disabled.
    QAbstractButton *undoButton = GTAction::button(os, "msa_action_undo");
    CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");

    //2. Choose in the context menu{ Align->Align with MUSCLE... }
    //Expected state : The "Align with MUSCLE" dialog has appeared
    //3. Check the "Translation to amino when aligning" checkbox and press "Align"
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialRegionContent = GTClipboard::text(os);

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os, MuscleDialogFiller::Default, true, true));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with muscle", GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(10, 5));
    GTMouseDriver::click(Qt::RightButton);

    //Expected state : Alignment task has started.After some time it finishes without errors
    //and alignment gets changed somehow.The "Undo" button becomes active
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::check(os, lt);
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectedly disabled");

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString alignedRegionContent = GTClipboard::text(os);
    CHECK_SET_ERR(alignedRegionContent != initialRegionContent, "Alignment content has not been changed");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //4. Click on the "Undo" button
    GTWidget::click(os, undoButton);

    //Expected state : Alignment has been restored to its initial state.The "Undo" button gets disabled,
    //the "Redo" has been enabled
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString undoneRegionContent = GTClipboard::text(os);
    CHECK_SET_ERR(undoneRegionContent == initialRegionContent, "Undo hasn't reverted changes");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");

    QAbstractButton *redoButton = GTAction::button(os, "msa_action_redo");
    CHECK_SET_ERR(redoButton->isEnabled(), "'Redo' button is unexpectedly disabled");

    //5. Click on the "Redo" button
    GTWidget::click(os, redoButton);

    //Expected state : Alignment has been changed.The "Redo" button gets disabled,
    //the "Undo" has been enabled
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString redoneRegionContent = GTClipboard::text(os);
    CHECK_SET_ERR(redoneRegionContent == alignedRegionContent, "Redo hasn't changed the alignment");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    CHECK_SET_ERR(!redoButton->isEnabled(), "'Redo' button is unexpectedly enabled");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectedly disabled");
}

GUI_TEST_CLASS_DEFINITION(test_2796) {
    //1. Open file "_common_data/fasta/fa2.fa"
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "fa2.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Use main menu { Actions -> Analyse -> Find pattern[Smith-Waterman]... }
    //Expected state: the "Smith-Waterman Search" dialog has appeared
    //3. Press "Cancel" in the dialog
    //Expected state: dialog has closed
    SmithWatermanDialogFiller *filler = new SmithWatermanDialogFiller(os);
    filler->button = SmithWatermanDialogFiller::Cancel;
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Analyze"
                                                << "Find pattern [Smith-Waterman]...",
                              GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_2801) {
    //1. Open {_common_data/clustal/100_sequences.aln}.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Start MAFFT with default values.
    GTUtilsDialog::waitForDialog(os, new MAFFTSupportRunDialogFiller(os, new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with MAFFT"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTGlobals::sleep(5000);
    //3. Cancel the align task.
    GTUtilsTaskTreeView::openView(os);
    GTUtilsTaskTreeView::checkTask(os, "Run MAFFT alignment task");
    GTUtilsTaskTreeView::cancelTask(os, "Run MAFFT alignment task");
    //Expected state: the task is cancelled, there is no MAFFT processes with its subprocesses (check for the "disttbfast" process)
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setReadChannelMode(QProcess::MergedChannels);
    process.start("wmic.exe /OUTPUT:STDOUT PROCESS get Caption");

    process.waitForStarted(1000);
    process.waitForFinished(1000);

    QByteArray list = process.readAll();
    CHECK_SET_ERR(!list.contains("disttbfast"), "disttbfast is presents in process lise");
}

GUI_TEST_CLASS_DEFINITION(test_2801_1) {
    //1. Open {_common_data/clustal/100_sequences.aln}.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(30000);
    //2. Start MAFFT with default values.
    GTUtilsDialog::waitForDialog(os, new MAFFTSupportRunDialogFiller(os, new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_ALIGN << "Align with MAFFT", GTGlobals::UseMouse));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTGlobals::sleep(20000);
    //3. Cancel the align task.
    GTUtilsTaskTreeView::openView(os);
    GTUtilsTaskTreeView::checkTask(os, "Run MAFFT alignment task");
    GTUtilsTaskTreeView::cancelTask(os, "Run MAFFT alignment task");
    //Expected state: the task is cancelled, there is no MAFFT processes with its subprocesses (check for the "disttbfast" process)
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.setReadChannelMode(QProcess::MergedChannels);
    process.start("wmic.exe /OUTPUT:STDOUT PROCESS get Caption");

    process.waitForStarted(1000);
    process.waitForFinished(1000);

    QByteArray list = process.readAll();
    CHECK_SET_ERR(!list.contains("disttbfast"), "disttbfast is presents in process lise");
}

GUI_TEST_CLASS_DEFINITION(test_2808) {
    //    1. Open WD.
    //    2. Add "Sequence Marker" element to the scene, select it.
    //    Expected state: there are buttons on the parameters widget: "add", "edit" and "remove". The "add" button is enabled, other buttons are disabled.
    //    3. Add a new marker group (click the "add" button and fill the dialog).
    //    Expected state: a new group was added, there is no selection in the marker group list, the "add" button is enabled, other buttons are disabled.
    //    4. Select the added group.
    //    Expected state: the group is selected, all buttons are enabled.
    //    5. Click the "remove" button.
    //    Expected state: the group is removed (the list is empty), the "add" button is enabled, other buttons are disabled.

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Sequence Marker");
    GTUtilsWorkflowDesigner::click(os, "Sequence Marker");

    QToolButton *addButton = qobject_cast<QToolButton *>(GTWidget::findWidget(os, "addButton"));
    CHECK_SET_ERR(addButton != NULL, "AddButton not found!");

    QToolButton *editButton = qobject_cast<QToolButton *>(GTWidget::findWidget(os, "editButton"));
    CHECK_SET_ERR(editButton != NULL, "EditButton not found!");

    QToolButton *removeButton = qobject_cast<QToolButton *>(GTWidget::findWidget(os, "removeButton"));
    CHECK_SET_ERR(removeButton != NULL, "RemoveButton not found!");

    CHECK_SET_ERR(addButton->isEnabled(), "AddButton is disabled!");
    CHECK_SET_ERR(!editButton->isEnabled(), "EditButton is enabled!");
    CHECK_SET_ERR(!removeButton->isEnabled(), "AddButton is enabled!");

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus &_os)
            : Filler(_os, "EditMarkerGroupDialog") {
        }
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );
            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox *>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        }
    };
    GTUtilsDialog::waitForDialog(os, new OkClicker(os));
    GTWidget::click(os, addButton);
    GTGlobals::sleep(2000);

    CHECK_SET_ERR(addButton->isEnabled(), "AddButton is disabled!");
    CHECK_SET_ERR(!editButton->isEnabled(), "EditButton is enabled!");
    CHECK_SET_ERR(!removeButton->isEnabled(), "AddButton is enabled!");

    QTableView *groupTable = qobject_cast<QTableView *>(GTWidget::findWidget(os, "markerTable"));
    CHECK_SET_ERR(groupTable != NULL, "MerkerTable not found");
    GTWidget::click(os, groupTable);

    QPoint p = GTTableView::getCellPosition(os, groupTable, 0, 0);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();

    CHECK_SET_ERR(addButton->isEnabled(), "AddButton is disabled!");
    CHECK_SET_ERR(editButton->isEnabled(), "EditButton is disabled!");
    CHECK_SET_ERR(removeButton->isEnabled(), "AddButton is disabled!");

    GTWidget::click(os, removeButton);

    CHECK_SET_ERR(groupTable->model() != NULL, "Abstract table model is NULL");
    CHECK_SET_ERR(groupTable->model()->rowCount() == 0, "Marker table is not empty!");
}

GUI_TEST_CLASS_DEFINITION(test_2809) {
    //    1. Open WD.
    //    2. Add a "Sequence Marker" element to the scene, select it.
    //    3. Add several items to marker group list on the parameters widget.
    //    Expected state: if all items are visible, there is no vertical scroll bar.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addAlgorithm(os, "Sequence Marker");
    GTUtilsWorkflowDesigner::click(os, "Sequence Marker");

    QToolButton *addButton = qobject_cast<QToolButton *>(GTWidget::findWidget(os, "addButton"));
    CHECK_SET_ERR(addButton != NULL, "AddButton not found!");

    class OkClicker : public Filler {
    public:
        OkClicker(HI::GUITestOpStatus &_os)
            : Filler(_os, "EditMarkerGroupDialog") {
        }
        virtual void run() {
            QWidget *w = QApplication::activeWindow();
            CHECK(NULL != w, );
            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox *>(QString::fromUtf8("buttonBox"));
            CHECK(NULL != buttonBox, );
            QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
            CHECK(NULL != button, );
            GTWidget::click(os, button);
        }
    };
    GTUtilsDialog::waitForDialog(os, new OkClicker(os));
    GTWidget::click(os, addButton);

    QTableView *groupTable = qobject_cast<QTableView *>(GTWidget::findWidget(os, "markerTable"));
    CHECK_SET_ERR(groupTable != NULL, "MerkerTable not found");
    GTWidget::click(os, groupTable);

    QScrollBar *scroll = groupTable->verticalScrollBar();
    CHECK_SET_ERR(scroll != NULL, "Scroll bar is NULL");
    CHECK_SET_ERR(!scroll->isVisible(), "Scroll bar is visible!");
}

GUI_TEST_CLASS_DEFINITION(test_2811) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::toggleDebugMode(os);
    GTGlobals::sleep(1000);
    GTUtilsWorkflowDesigner::toggleBreakpointManager(os);

    //    2. Open any workflow, create a breakpoint for any element.
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep();
    GTUtilsWorkflowDesigner::setBreakpoint(os, "Align with MUSCLE");

    //    3. Open another workflow.
    //    Expected state: breakpoints list is cleared.
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep();
    QStringList breakpointList = GTUtilsWorkflowDesigner::getBreakpointList(os);
    CHECK_SET_ERR(breakpointList.isEmpty(), "There are unexpected breakpoints");
}

GUI_TEST_CLASS_DEFINITION(test_2829) {
    //1) Open files "data/samples/Genbank/murine.gb" and "data/samples/Genbank/sars.gb" in separated views
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2) Click on toolbar 'Build dotplot' button
    //3) In opened dialog click 'OK' button
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os));
    GTWidget::click(os, GTWidget::findWidget(os, "build_dotplot_action_widget"));
    GTGlobals::sleep();

    //4) Choose some annotation by left mouse button on the upper sequence view
    //Expected state: horisontal or vertical selection is shown on DotPlot
    QList<QTreeWidgetItem *> geneItems = GTUtilsAnnotationsTreeView::findItems(os, "gene", GTGlobals::FindOptions(false));
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, geneItems.at(1)));
    GTMouseDriver::click();

    //5) In second sequence view click Remove sequence on the toolbar
    //Expected state: DotPlot closed and UGENE didn't crash
    GTUtilsMdi::activateWindow(os, "murine [s] NC_001363");

    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363", GTUtilsMdi::activeWindow(os));
    CHECK_SET_ERR(toolbar != NULL, "Cannot find views_tool_bar_NC_001363");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
}

GUI_TEST_CLASS_DEFINITION(test_2853) {
    Runnable *filler = new NCBISearchDialogFillerDeprecated(os, "rat");

    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Search NCBI GenBank...",
                              GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_2863) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Add "Read File URL(s)".
    WorkflowProcessItem *fileList = GTUtilsWorkflowDesigner::addElement(os, "Read File URL(s)");
    //    3. Add "File Format Conversion".
    WorkflowProcessItem *conversion = GTUtilsWorkflowDesigner::addElement(os, "File Format Conversion");
    //    4. Connect the elements.
    GTUtilsWorkflowDesigner::connect(os, fileList, conversion);
    //    Expected: the converter's input slot "Source URL" is binded with the "Source URL" slot of the Read File URL(s).
    GTUtilsWorkflowDesigner::click(os, conversion);
    QTableWidget *table = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
    QString s1 = table->item(0, 0)->text();
    QString s2 = table->item(0, 1)->text();
    CHECK_SET_ERR(s1 == "Source URL", "unexpected first value: " + s1);
    CHECK_SET_ERR(s2 == "Source URL (by Read File URL(s))", "unexpected second value: " + s2)

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2866) {
    //    1. Use main menu { Tools -> Align to reference -> Align short reads }
    //    Expected state: the "Align Sequencing Reads" dialog has appeared
    //    2. Fill dialog: mapping tool: Bowtie
    //                    reference sequence: _common_data/e_coli/NC_008253.gff
    //                    short reads: "_common_data/e_coli/e_coli_1000.fastq"
    //       Click start button
    //    Expected state: message box with "These files have the incompatible format" has appeared
    //    3. Click "Yes"
    //    Expected state: UGENE assembles reads without errors and the "Import SAM File" dialog has appeared
    GTLogTracer l;

    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/e_coli/",
                                                 "NC_008253.gff",
                                                 testDir + "_common_data/e_coli/",
                                                 "e_coli_1000.fastq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &parameters));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "Yes"));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "test_2866.ugenedb"));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Map reads to reference...");
    GTGlobals::sleep(5000);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2884) {
    //1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //2. Place Cuffdiff element on the scene
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Test for Diff. Expression with Cuffdiff");
    CHECK_OP(os, );

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Test for Diff. Expression with Cuffdiff"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter(os, "Multi read correct") == "False", "'Mate inner distance', Parameter value doesn't amtch");
}

GUI_TEST_CLASS_DEFINITION(test_2887) {
    //1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //2. Place Tophat element on the scene
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Map RNA-Seq Reads with TopHat");
    CHECK_OP(os, );
    //3. check "Mate inner distance" parameter is 50
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Map RNA-Seq Reads with TopHat"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter(os, "Mate inner distance") == "50", "'Mate inner distance', Parameter value doesn't amtch");
}

GUI_TEST_CLASS_DEFINITION(test_2891) {
    // 1. Open file "data/samples/workflow_samples/NGS/cistrome/chip_seq.uwl"
    // 2. Set input file for the "Read Tags" element to "test/_common_data/bed/valid_input/tophat_output.bed"
    // 3. Press the "Validate workflow" button on the main toolbar
    // Expected state: the message box about workflow errors has appeared. The "Error list" tab has appeared below the workflow
    // 4. Press "OK"
    // Expected state: there is no messages about the "Read tags" element on the "Error list" tab
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, dataDir + "/workflow_samples/NGS/cistrome/", "chip_seq.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::click(os, "Read Tags");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/bed/valid_input/tophat_output.bed");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::checkErrorList(os, "Read Tags") == 0, "Errors count dont match, should be 0 validation errors");
}
GUI_TEST_CLASS_DEFINITION(test_2891_1) {
    // 1. Open file "data/samples/workflow_samples/NGS/cistrome/chip_seq.uwl"
    // 2. Set input file for the "Read Tags" element to "test/_common_data/regression/1587/some_image.png"
    // 3. Press the "Validate workflow" button on the main toolbar
    // Expected state: the message box about workflow errors has appeared. The "Error list" tab has appeared below the workflow
    // 4. Press "OK"
    // Expected state: there is a warning about possible incompatibilities of the "Read tags" element on the "Error list" tab
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new StartupDialogFiller(os));
    GTFileDialog::openFile(os, dataDir + "/workflow_samples/NGS/cistrome/", "chip_seq.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::click(os, "Read Tags");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/regression/1587/some_image.png");

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTWidget::click(os, GTAction::button(os, "Validate workflow"));
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::checkErrorList(os, "Read Tags") == 1, "Errors count dont match, should be 1 validation error");
}

GUI_TEST_CLASS_DEFINITION(test_2894) {
    //    1. Open {_common_data/clustal/100_sequences.aln}.
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "100_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Use context menu {Tree->Build Tree}.
    //    Expected state: "Build phylogenetic tree" dialog has been appeared.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_2894_COI.nwk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    //    3. Run "Phylip Neighbor Joining" with default parameters.
    //    Expected state: tree view has been appeared.
    GTWidget::findWidget(os, "treeView");
    QWidget *qt_toolbar_ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "100_sequences [m] 100_sequences"), GTGlobals::FindOptions(false));
    //    4. Press refresh tree button on the tree's toolbar.
    //    Expected state: "Calculating Phylogenetic Tree" task has been started.
    if (qt_toolbar_ext_button != NULL && qt_toolbar_ext_button->isVisible()) {
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Refresh tree"));
        GTWidget::click(os, qt_toolbar_ext_button);
    } else {
        GTWidget::click(os, GTAction::button(os, "Refresh tree"));
    }

    GTUtilsTask::checkTask(os, "Calculating Phylogenetic Tree");
    //    5. Press refresh button again.
    //    Expected state: a new refresh task is not started, the old one is in process.
    if (qt_toolbar_ext_button != NULL && qt_toolbar_ext_button->isVisible()) {
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Refresh tree"));
        GTWidget::click(os, qt_toolbar_ext_button);
    } else {
        GTWidget::click(os, GTAction::button(os, "Refresh tree"));
    }
    GTGlobals::sleep(100);

    int num = GTUtilsTaskTreeView::countTasks(os, "Calculating Phylogenetic Tree");
    CHECK_SET_ERR(num == 1, QString("Wrong tasks number. Expected 1, actual: ").arg(num));
    //    6. Close the tree view while the task is performed.
    //    Expected state: UGENE doesn't crash, view is closed, task cancels.
    GTUtilsProjectTreeView::click(os, "test_2894_COI.nwk");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_2895) {
    //1. Open "_common_data/fasta/amino_multy.fa" as multiple alignment.
    //2. Open "_common_data/fasta/amino_multy_ext.fa" as separate sequences.
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/amino_multy_ext.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/amino_multy.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //3. Try to add to the amino_multy.fa document, any sequence from the amino_multy_ext.fa document.
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "amino_multy_ext.fa", "chr1_gl000191_random Amino translation 0 direct"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "Sequence from current project...");
    GTGlobals::sleep();
    //Expected: an object will be added.
}

GUI_TEST_CLASS_DEFINITION(test_2897) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    //    2. Open options panel 'Highlighting' tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::Highlighting);

    QComboBox *combo = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "highlightingScheme"));
    CHECK_SET_ERR(combo != NULL, "highlightingScheme not found!");
    int oldItemsNumber = combo->count();

    //    3. Create a new custom nucleotide color scheme.
    const QString colorSchemeName = getName() + "_NewScheme";
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                                        << "Custom schemes"
                                                                        << "Create new color scheme"));
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, colorSchemeName, NewColorSchemeCreator::nucl));
    MSAEditorSequenceArea *msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
    GTMenu::showContextMenu(os, msaSeqArea);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_APPEARANCE << "Colors"
                                                                        << "Custom schemes" << colorSchemeName));
    GTMenu::showContextMenu(os, msaSeqArea);

    combo = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "highlightingScheme"));
    CHECK_SET_ERR(combo != NULL, "highlightingScheme not found!");
    int newItemsNumber = combo->count();

    CHECK_SET_ERR(newItemsNumber == oldItemsNumber, "exportButton is disabled unexpectedly");
}

GUI_TEST_CLASS_DEFINITION(test_2899) {
    //    1. Start WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    //    2. Open "RNA-seq analysis with Tuxedo tools" sample.
    //    3. Click the "Setup" button (default values: Full, Single-end).
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Full"
                                                                                                                  << "Single-end"));

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    4. Go to the "Cuffmerge settings" page and click the "Defaults" button.
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "activeModalWidget is NULL");

            for (int i = 0; i < 4; i++) {
                GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            }
            CHECK_SET_ERR(GTUtilsWizard::getPageTitle(os) == "Cuffmerge settings", "unexpected page");
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Defaults);

            //    5. Return to the "Tophat settings" page and set any file as Bowtie index (it might be valid Bowtie index or not).
            for (int i = 0; i < 2; i++) {
                GTUtilsWizard::clickButton(os, GTUtilsWizard::Back);
            }

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/bowtie2/index/human_T1_cutted.2.bt2"));
            GTWidget::click(os, GTWidget::findButtonByText(os, "Select\nbowtie index file", dialog));
            //    Expected state: index is set if it is valid (file has valid extension), no crash
            bool basename = GTUtilsWizard::getParameter(os, "Bowtie index basename").toString() == "human_T1_cutted";
            bool dir = GTUtilsWizard::getParameter(os, "Bowtie index folder").toString().contains("_common_data/bowtie2/index");
            CHECK_SET_ERR(basename, "unexpected basename");
            CHECK_SET_ERR(dir, "unexpected dir");
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Tuxedo Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample(os, "RNA-seq analysis with Tuxedo tools");
}

GUI_TEST_CLASS_DEFINITION(test_2900) {
    //    1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    //    2. Sequence view context menu -> Analyze -> Restriction sites.
    //    3. Press "OK".
    //    Expected: the 8 regions of auto-annotations are created.
    const QStringList defaultEnzymes = QStringList() << "BamHI"
                                                     << "BglII"
                                                     << "ClaI"
                                                     << "DraI"
                                                     << "EcoRI"
                                                     << "EcoRV"
                                                     << "HindIII"
                                                     << "PstI"
                                                     << "SalI"
                                                     << "SmaI"
                                                     << "XmaI";
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
                                                                        << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, defaultEnzymes));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
    const int firstAnnotationsCount = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "enzyme  (8, 0)").size();

    //    4. Repeast 2-3.
    //    Expected: there are still 8 regions of auto-annotations. Old regions are removed, new ones are added.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
                                                                        << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, defaultEnzymes));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);
    const int secondAnnotationsCount = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(os, "enzyme  (8, 0)").size();

    CHECK_SET_ERR(firstAnnotationsCount == secondAnnotationsCount, QString("Annotations count differs: %1 annotations in the first time, %2 annotations in the second time").arg(firstAnnotationsCount).arg(secondAnnotationsCount));
}

GUI_TEST_CLASS_DEFINITION(test_2903) {
    //    1. Open the attached file
    GTLogTracer l;
    GTFileDialog::openFile(os, testDir + "_common_data/regression/2903", "unknown_virus.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            GTWidget::getActiveModalWidget(os);
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };

    //    2. Click on the Analyze->Query NCBI BLAST database context menu
    //    3. Click on the Search button
    //    Expected state: the task starts with no errors
    //    Current state: the following error appears: 'RemoteBLASTTask' task failed: Database couldn't prepare the response
    GTUtilsDialog::waitForDialog(os, new RemoteBLASTDialogFiller(os, new Scenario));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE"
                                                                        << "Query NCBI BLAST database"));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ren"
                                                         "der_area_virus_X"));
    QString blastTaskName = "RemoteBLASTTask";
    GTUtilsTaskTreeView::checkTask(os, blastTaskName);
    GTGlobals::sleep(10000);

    // Cancel the task. If not cancelled the run may last too long to trigger timeout in nightly tests.
    bool isTaskRunning = GTUtilsTaskTreeView::checkTask(os, blastTaskName);
    if (isTaskRunning) {
        GTUtilsTaskTreeView::cancelTask(os, blastTaskName);
    }

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2907) {
    //1. Open file _common_data/genbank/pBR322.gb
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/", "pBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. In annotations tree view go to element Auto - annotations->enzyme->EcoRI(0, 1)->EcoRI
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    QWidget *qt_toolbar_ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "mwtoolbar_activemdi"), GTGlobals::FindOptions(false));
    if (qt_toolbar_ext_button != NULL && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(os, qt_toolbar_ext_button);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "EcoRI"));

    //3. Stop cursor on EcoRI qualifier.Wait for tooltip
    //Expected state : tooltip will appear
    GTUtilsToolTip::checkExistingToolTip(os, "db_xref");
}

GUI_TEST_CLASS_DEFINITION(test_2910) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Press Ctrl+A.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 10000..15000
    // 4. Click the "Go" button.

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, "10000..15000"));
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);
    GTGlobals::sleep();

    // Expected state: the region is selected, there is a label "[5001 bp]" on the pan view.
    QVector<U2Region> reg = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(reg.size() == 1, QString("unexpected number of selected regions: ").arg(reg.size()));
    CHECK_SET_ERR(reg.first().length == 5001, QString("unexpected selection length: ").arg(reg.first().length));
    CHECK_SET_ERR(reg.first().startPos == 9999, QString("unexpected selection start pos: ").arg(reg.first().startPos));
}
GUI_TEST_CLASS_DEFINITION(test_2910_1) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Press Ctrl+A.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 2000..5000,100000..110000
    // 4. Click the "Go" button.

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, "2000..5000,100000..110000"));
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);
    GTGlobals::sleep(500);

    // Expected state: the region is selected, there is a label "[3001 bp]" and "[10001 bp]" on the pan view.
    QVector<U2Region> reg = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(reg.size() == 2, QString("unexpected number of selected regions: ").arg(reg.size()));
    CHECK_SET_ERR(reg.first().length == 3001, QString("unexpected selection length: ").arg(reg.first().length));
    CHECK_SET_ERR(reg.first().startPos == 1999, QString("unexpected selection start pos: ").arg(reg.first().startPos));

    CHECK_SET_ERR(reg.at(1).length == 10001, QString("unexpected selection length: ").arg(reg.at(1).length));
    CHECK_SET_ERR(reg.at(1).startPos == 99999, QString("unexpected selection start pos: ").arg(reg.at(1).startPos));
}

GUI_TEST_CLASS_DEFINITION(test_2910_2) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(2000);

    // 2. Press Ctrl+A.
    // Expected state: a "Region Selection" dialog appeared.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 0..5000
    // Expected state: the region is invalid, a "Go" button is disabled.
    class CancelClicker : public Filler {
    public:
        CancelClicker(HI::GUITestOpStatus &_os)
            : Filler(_os, "RangeSelectionDialog") {
        }
        virtual void run() {
            GTGlobals::sleep(1000);
            QWidget *w = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != w, "active modal widget is null");
            QRadioButton *multipleButton = w->findChild<QRadioButton *>("miltipleButton");
            CHECK_SET_ERR(multipleButton != NULL, "RadioButton \"miltipleButton\" not found");
            GTRadioButton::click(os, multipleButton);

            QLineEdit *regionEdit = w->findChild<QLineEdit *>("multipleRegionEdit");
            CHECK_SET_ERR(regionEdit != NULL, "QLineEdit \"multipleRegionEdit\" not foud");
            GTLineEdit::setText(os, regionEdit, "0..5000");

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox *>(QString::fromUtf8("buttonBox"));
            CHECK_SET_ERR(NULL != buttonBox, "button box is null");
            QPushButton *button = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(NULL != button, "cancel button is null");
            QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!okButton->isEnabled(), "OK button is unexpectedly enabled");
            GTWidget::click(os, button);
        }
    };
    GTUtilsDialog::waitForDialog(os, new CancelClicker(os));
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);
    GTGlobals::sleep(500);
}
GUI_TEST_CLASS_DEFINITION(test_2910_3) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(2000);

    // 2. Press Ctrl+A.
    // Expected state: a "Region Selection" dialog appeared.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 1..199951
    // Expected state: the region is invalid, a "Go" button is disabled.
    class CancelClicker : public Filler {
    public:
        CancelClicker(HI::GUITestOpStatus &_os)
            : Filler(_os, "RangeSelectionDialog") {
        }
        virtual void run() {
            GTGlobals::sleep(1000);
            QWidget *w = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != w, "active modal widget is null");
            QRadioButton *multipleButton = w->findChild<QRadioButton *>("miltipleButton");
            CHECK_SET_ERR(multipleButton != NULL, "RadioButton \"miltipleButton\" not found");
            GTRadioButton::click(os, multipleButton);
            GTGlobals::sleep();

            QLineEdit *regionEdit = w->findChild<QLineEdit *>("multipleRegionEdit");
            CHECK_SET_ERR(regionEdit != NULL, "QLineEdit \"multipleRegionEdit\" not foud");
            GTLineEdit::setText(os, regionEdit, "1..199951");

            QDialogButtonBox *buttonBox = w->findChild<QDialogButtonBox *>(QString::fromUtf8("buttonBox"));
            CHECK_SET_ERR(NULL != buttonBox, "button box is null");
            QPushButton *button = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(NULL != button, "cancel button is null");
            QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!okButton->isEnabled(), "OK button is unexpectedly enabled");
            GTWidget::click(os, button);
        }
    };
    GTUtilsDialog::waitForDialog(os, new CancelClicker(os));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2923) {
    //1. Open "samples/FASTA/human_T1.fa".
    //Expected: the sequence view is opened.
    //2. Press Ctrl+W.
    //Expected: the sequence view is closed.

    //Close active view
    GTUtilsMdi::closeActiveWindow(os);
    GTGlobals::sleep();

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMdi::activeWindow(os);

    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    GTGlobals::sleep();
    QWidget *mdi = GTUtilsMdi::activeWindow(os, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == mdi, "Sequence view is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_2924) {
    //1. Open "_common_data/scenarios/_regression/2924/human_T1_cutted.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/2924/", "human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "_common_data/scenarios/_regression/2924/MyDocument_3.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/2924/", "MyDocument_3.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Drag'n'drop the annotation object from the project to the sequence view.
    //Expected: the dialog appears.
    //4. Click "OK".
    //Expected : the annotations are shown on the sequence view.
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, "Annotations"), GTUtilsAnnotationsTreeView::getTreeWidget(os));

    //5. Click the "Shown circular view" button on the sequence toolbar.
    //Expected : the circular view is shown.
    ADVSingleSequenceWidget *seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    GTUtilsCv::cvBtn::click(os, seqWidget);

    //6. Click the "Zoom in" button several times until it becomes disabled.
    //Expected : UGENE does not crash.
    QWidget *zoomInButton = GTAction::button(os, "tbZoomIn_human_T1_cutted [s] human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(os, zoomInButton);
        GTGlobals::sleep(500);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2927) {
    //    1. Open file "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Open file "data/samples/PDB/1CRN.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CRN.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    3. Remove first document with delete key.
    GTUtilsProjectTreeView::click(os, "1CF7.PDB");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    4. Press "Open" toolBar button.
    //    Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/PDB/1CF7.PDB"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_MAIN, "Open");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2929) {
    //    1.    Open "human_T1.fa".
    //    2.    Click the "Find TFBS with SITECON" button on the main toolbar
    //        Expected state: a dialog appeared, model is not selected, threshold is not set.
    //    3.    Click the "Search" button.
    //        Expected state: an error message box appeared.
    //    4.    Click the "Ok" button.
    //        Expected state: the message box is closed, dialog is not accepted.
    //        Current state: the message box is closed, UGENE crashed.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class SiteconCustomFiller : public Filler {
    public:
        SiteconCustomFiller(HI::GUITestOpStatus &os)
            : Filler(os, "SiteconSearchDialog") {
        }
        virtual void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            QLineEdit *modelFileEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "modelFileEdit"));
            CHECK_SET_ERR(modelFileEdit != NULL, "modelFileEdit not found!");
            CHECK_SET_ERR(modelFileEdit->text().isEmpty(), "Model is set!");

            QComboBox *errComboBox = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "errLevelBox"));
            CHECK_SET_ERR(errComboBox->currentText().isEmpty(), "Threshold is set!");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SiteconCustomFiller(os));
    GTWidget::click(os, GTWidget::findWidget(os, "SITECON_widget"));
}

GUI_TEST_CLASS_DEFINITION(test_2930) {
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_2930"));
    GTUtilsProject::openFile(os, dataDir + "samples/ACE/K26.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_2931) {
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_2931"));
    GTUtilsProject::openFile(os, dataDir + "samples/ACE/K26.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new ConvertAssemblyToSAMDialogFiller(os, sandBoxDir, "test_2931"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Convert UGENE assembly database to SAM...");

    CHECK_SET_ERR(!l.hasErrors(), "Errors in log: " + l.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2945) {
    //    1.    Open file with annotations, e.g. murine.db
    //    2.    Open Circular View(CV)
    //    3.    Move the splitter between CV and Sequence View(SV) up to hide CV (CV action button is still active)
    //    4.    Move the splitter between Sequence View and Annotation Tree View down.
    //    expected state: CV appeared. all parts of CV are visiable

    // New state of UGENE: seq.view is resizable now, so CV will not be visible by dragging splitter between SV and AE

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTAction::button(os, "CircularViewAction"));
    QWidget *zoomIn = GTAction::button(os, "tbZoomIn_murine [s] NC_001363");
    CHECK_SET_ERR(zoomIn != NULL, "zoomIn action on CV not found");

    QWidget *splitterHandler = GTWidget::findWidget(os, "qt_splithandle_annotated_DNA_scrollarea");
    CHECK_SET_ERR(splitterHandler != NULL, "SplitterHandle not found");
    GTWidget::click(os, splitterHandler);

    QWidget *mainToolBar = GTWidget::findWidget(os, "mwtoolbar_activemdi");
    CHECK_SET_ERR(mainToolBar != NULL, "mwtoolbar_activemdi not found");
    QPoint bottomLeftToolBar = mainToolBar->geometry().bottomLeft();
    bottomLeftToolBar = mainToolBar->mapToGlobal(bottomLeftToolBar);
    GTGlobals::sleep();

    GTMouseDriver::press();
    GTMouseDriver::moveTo(bottomLeftToolBar);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTGlobals::sleep(15000);
    QPoint handlePosition = splitterHandler->pos();

    QAbstractButton *cvButton = GTAction::button(os, "CircularViewAction");
    CHECK_SET_ERR(cvButton->isChecked(), "CV button is not checked!");

    QSplitter *splitter = qobject_cast<QSplitter *>(GTWidget::findWidget(os, "annotated_DNA_splitter"));
    CHECK_SET_ERR(splitter != NULL, "annotated_DNA_splitter not found");
    int idx = splitter->indexOf(GTWidget::findWidget(os, "annotations_tree_view"));
    QSplitterHandle *handle = splitter->handle(idx);
    CHECK_SET_ERR(handle != NULL, "SplitterHadle not found");

    GTWidget::click(os, handle);
    GTGlobals::sleep();

    QPoint p = GTMouseDriver::getMousePosition();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p + QPoint(0, 50));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTGlobals::sleep(15000);

    CHECK_SET_ERR(handlePosition == splitterHandler->pos(), QString("Handler was moved: expected: %1, actual: %2").arg(splitter->pos().y()).arg(handlePosition.y()));
}

GUI_TEST_CLASS_DEFINITION(test_2951) {
    //1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Click the "Scripting mode" tool button -> Show scripting options.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Show scripting options"));
    GTWidget::click(os, GTAction::button(os, GTAction::findActionByText(os, "Scripting mode")));

    //3. Create the workflow: "Read sequence" -> "Write sequence".
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm(os, "Write Sequence", true);
    GTUtilsWorkflowDesigner::connect(os, GTUtilsWorkflowDesigner::getWorker(os, "Read Sequence"), GTUtilsWorkflowDesigner::getWorker(os, "Write Sequence"));

    //4. Set the input sequence: _common_data/fasta/abcd.fa.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/fasta/abcd.fa");

    //5. Set the correct output sequence.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Write Sequence"));
    GTMouseDriver::click();
    QString outFile = GUrl(sandBoxDir + "test_2951.gb").getURLString();
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", outFile, GTUtilsWorkflowDesigner::textValue);

    //6. Edit a script for "Sequence count limit":
    //    printToLog("test");
    //    1
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(os, "Read Sequence"));
    GTMouseDriver::click();
    GTUtilsDialog::waitForDialog(os, new ScriptEditorDialogFiller(os, "", "printToLog(\"test message\");\n1"));
    GTUtilsWorkflowDesigner::setParameterScripting(os, "Sequence count limit", "user script");

    //Expected: the result file contains only the first sequence from the input file; there is a message "test" in the script details log.
    GTWidget::click(os, GTAction::button(os, "Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    bool printed = GTLogTracer::checkMessage("test message");
    CHECK_SET_ERR(printed, "No message in the log");

    GTUtilsProject::openFile(os, outFile);
    QModelIndex objIdx = GTUtilsProjectTreeView::findIndex(os, "d");
    QTreeView *tree = GTUtilsProjectTreeView::getTreeView(os);
    int objectsCount = tree->model()->rowCount(objIdx.parent());
    CHECK_SET_ERR(1 == objectsCount, "More than one objects in the result");
}

GUI_TEST_CLASS_DEFINITION(test_2962_1) {
    //    1. Open "_common_data/scenarios/_regression/2924/human_T1_cutted.fa".
    //    2. Click the "Shown circular view" button on the sequence toolbar.
    //    Expected: the circular view is shown.
    //    3. Unload project.
    //    4. Repeat 1,2.
    //    Expected: UGENE does not crash.

    GTLogTracer l;
    ADVSingleSequenceWidget *seqWidget =
        GTUtilsProject::openFileExpectSequence(os,
                                               testDir + "_common_data/scenarios/_regression/2924",
                                               "human_T1_cutted.fa",
                                               "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTUtilsCv::cvBtn::click(os, seqWidget);

    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project",
                              GTGlobals::UseMouse);

    seqWidget = GTUtilsProject::openFileExpectSequence(os,
                                                       testDir + "_common_data/scenarios/_regression/2924",
                                                       "human_T1_cutted.fa",
                                                       "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(os, seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "Unexpected state of CV widget!");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2962_2) {
    //    Open "_common_data/fasta/DNA.fa".
    //    Remove the second sequence object from the document.
    //    Click the "Toggle circular views" button on the main toolbar.
    //    Expected state: the circular view is toggled it is possible.
    //    Current state: UGENE crashes.

    GTLogTracer l;

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QPoint p = GTUtilsProjectTreeView::getItemCenter(os, "GXL_141618");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();

    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsCv::commonCvBtn::click(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_2971) {
    /*  1. Open "COI.aln".
    2. Context menu -> Add -> Sequence from file...
    3. Choose a file with a large sequence (~50mb), e.g human chr21.
    Expected: UGENE does not crash.
*/
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

    GTWidget::click(os, GTAction::button(os, "Show overview"));    //needed to swith off rendering overview
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/regression/2971", "hg18_21.fa"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "MSAE_MENU_LOAD_SEQ"
                                                                        << "Sequence from file"));
    GTWidget::click(os, GTUtilsMdi::activeWindow(os), Qt::RightButton);
    GTGlobals::sleep(500);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Close file - UGENE does not crash.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "COI.aln"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_2972) {
    //    1. Open "samples/FASTA/human_T1.fa".
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Click the menu Tools -> HMMER tools -> HMM3 -> Search with HMM3 phmmer.
    GTUtilsDialog::waitForDialog(os, new UHMM3PhmmerDialogFiller(os, dataDir + "samples/Newick/COI.nwk"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "HMMER tools"
                                                << "Search with phmmer...");
    GTGlobals::sleep();

    CHECK_SET_ERR(l.hasErrors(), "Expected to have errors in the log, but no errors found");
    QString error = l.getJoinedErrorString();
#ifdef Q_OS_WIN
    QString expectedError = "Task {Search with phmmer} finished with error: Subtask {PHMMER search tool} is failed";
#else
    QString expectedError = "is empty or misformatted";
#endif

    CHECK_SET_ERR(error.contains(expectedError), "actual error is " + error);
    //    3. Choose the query sequence file: any non-sequence format file (e.g. *.mp3).
    //    4. Click search.
    //    Expected state: the search task fails, UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_2975) {
    //    1. Open "_common_data/fasta/all_alphabet.fa" as multiple alignment.
    GTUtilsProject::openMultiSequenceFileAsMalignment(os, testDir + "_common_data/fasta", "all_and_raw_alphabets.fa");

    //    2. Open the "Pairwise alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::PairwiseAlignment);

    //    3. Set the first sequence as the first sequence and the second sequence as the second sequence, select the "Smith-Waterman" algorithm.
    //    Expected state: align button is blocked
    GTUtilsOptionPanelMsa::addFirstSeqToPA(os, "Dna");
    GTUtilsOptionPanelMsa::addSecondSeqToPA(os, "Raw");
    GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm(os, "Smith-Waterman");

    QPushButton *alignButton = GTUtilsOptionPanelMsa::getAlignButton(os);
    CHECK_SET_ERR(NULL != alignButton, "Align button is NULL");
    CHECK_SET_ERR(!alignButton->isEnabled(), "Align button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2981) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Click a "Build Tree" button on the main toolbar.
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "test_3276_COI.wnk", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Expected state: a "Build Phyligenetic Tree" dialog appears.

    //    3. Set any acceptable path and build a tree with default parameters.
    //    Expected state: the tree is shown in the MSA Editor.

    //    4. Click a "Layout" button on the tree view toolbar, select a "Circular" menu item.
    QComboBox *layoutCombo = GTWidget::findExactWidget<QComboBox *>(os, "layoutCombo");
    GTComboBox::setIndexWithText(os, layoutCombo, "Circular");
    GTGlobals::sleep(500);
    //    Expected state: the tree becomes circular.
    QGraphicsView *treeView = GTWidget::findExactWidget<QGraphicsView *>(os, "treeView");
    int initW = treeView->rect().width();
    GTGlobals::sleep(500);
    //    5. Hide/show a project view.
    GTKeyboardDriver::keyClick('1', Qt::AltModifier);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick('1', Qt::AltModifier);
    GTGlobals::sleep(500);
    //    Expected state: the tree size is not changed.
    int finalW = treeView->rect().width();
    CHECK_SET_ERR(finalW == initW, QString("initial: %1, final: %2").arg(initW).arg(finalW));
}

GUI_TEST_CLASS_DEFINITION(test_2987) {
    //      1. Open "_common_data/fasta/RAW.fa".
    //      2. Create a complement annotation.
    //      3. Select {Actions -> Export -> Export annotations...} in the main menu.
    //      4. Fill the dialog:
    //      "Export to file" - any valid file;
    //      "File format" - csv;
    //      "Save sequences under annotations" - checked
    //      and click the "Ok" button.
    //      Expected state: annotations are successfully exported.
    GTLogTracer logTracer;

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "RAW2.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QDir().mkpath(sandBoxDir + "test_3305");
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "misc_feature", "complement(1..5)", sandBoxDir + "test_2987/test_2987.gb"));
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "create_annotation_action"));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << "ep_exportAnnotations2CSV"));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "test_2987/test_2987", ExportAnnotationsFiller::csv, os));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "test_2987.gb"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    const QFile csvFile(sandBoxDir + "test_2987/test_2987");
    CHECK_SET_ERR(!(csvFile.exists() && csvFile.size() == 0), "An empty file exists");
}

GUI_TEST_CLASS_DEFINITION(test_2991) {
    /*  1. Open file _common_data/alphabets/extended_amino_1000.fa
    Expected state: Alphabet of opened sequence must be [amino ext]
*/
    GTFileDialog::openFile(os, testDir + "_common_data/alphabets/", "extended_amino_1000.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);
    QWidget *w = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QLabel *label = qobject_cast<QLabel *>(GTWidget::findWidget(os, "nameLabel", w));
    CHECK_SET_ERR(label->text().contains("[amino ext]"), QString("Unexpected label of sequence name: %1, must contain %2").arg(label->text()).arg("[amino ext]"));
}

GUI_TEST_CLASS_DEFINITION(test_2998) {
    // 1. Open human_T1.fa
    GTLogTracer l;
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    // 2. Find any pattern
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTTTTAAAAA", os);

    // Expected state: the task will finished without errors.
    GTUtilsLog::check(os, l);
}

}    // namespace GUITest_regression_scenarios

}    // namespace U2
