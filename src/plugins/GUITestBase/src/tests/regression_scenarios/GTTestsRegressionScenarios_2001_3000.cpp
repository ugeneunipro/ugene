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
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <runnables/ugene/plugins/external_tools/TCoffeeDailogFiller.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>
#include <utils/GTUtilsToolTip.h>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QPushButton>
#include <QTableWidget>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2ObjectDbi.h>

#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/AnnotationsTreeView.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/TvBranchItem.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTTestsRegressionScenarios_2001_3000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
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
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ConvertAssemblyToSAMDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindQualifierDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindTandemsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportAPRFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExportHighlightedDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/annotator/FindAnnotationCollocationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportBlastResultDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2MSADialogFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/ClustalOSupportRunDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/RemoteBLASTDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithScriptDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DashboardsManagerDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/DefaultWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/MAFFT/MAFFTSupportRunDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/clustalw/ClustalWDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/hmm3/UHMM3PhmmerDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/kalign/KalignDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/NCBISearchDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {

GUI_TEST_CLASS_DEFINITION(test_2006) {
    const int MSA_WIDTH = 30;
    const int MSA_HEIGHT = 3;

    // 1. Open "data/samples/CLUSTAL/COI.aln" and save it's part to a string
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(MSA_WIDTH, MSA_HEIGHT));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialMsaContent = GTClipboard::text();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select the second symbol in the first line
    const QPoint initialSelectionPos(1, 0);
    GTUtilsMSAEditorSequenceArea::click(initialSelectionPos);

    // 3. Drag it to the first symbol in the first line
    const QPoint mouseDragPosition(1, 0);
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(0, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // 4. Check that the content has not been changed
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(MSA_WIDTH, MSA_HEIGHT));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");

    // 5. Check that "Undo" and "Redo" buttons are disabled
    const QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button is unexpectedly enabled");
    const QAbstractButton* redo = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2007) {
    // 1. Open "_common_data/scenarios/msa/ma_empty_line.aln (and save it to string)
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma_empty_line.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 4));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialMsaContent = GTClipboard::text();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select the fourth column of the second, third and fourth lines
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 1), QPoint(3, 3));

    // 3. Drag it to the left.
    const QPoint mouseDragPosition(3, 2);
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition - QPoint(1, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // Expected state: nothing happens
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 4));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has changed unexpectedly!");
}

GUI_TEST_CLASS_DEFINITION(test_2009) {
    //    1) Open Settings/Preferences/External tools
    //    2) Set correct BWA or any other tool path (preferably just executable)
    class BWAInactivation : public CustomScenario {
    public:
        void run() {
            //    3) Clear set path
            AppSettingsDialogFiller::clearToolPath("BWA");

            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new BWAInactivation()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    //    4) Look at UGENE log
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    //    Expected state: UGENE doesn't write any error to log
}

GUI_TEST_CLASS_DEFINITION(test_2012) {
    // 1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "General" tab on the options panel.
    GTWidget::click(GTWidget::findWidget("OP_MSA_GENERAL"));
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    GTLineEdit::setText(sequenceLineEdit, "m");

    GTKeyboardDriver::keyClick(Qt::Key_Down);

    GTKeyboardDriver::keyClick(Qt::Key_Down);

    GTKeyboardDriver::keyClick(Qt::Key_Down);

    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    QString selectedSequence = sequenceLineEdit->text();
    CHECK_SET_ERR(selectedSequence == "Mecopoda_elongata__Sumatra_", "Unexpected selected sequence: " + selectedSequence);

    // 3. Set focus to the reference sequence name line edit and enter 'm' into it.
}

GUI_TEST_CLASS_DEFINITION(test_2021_1) {
    // 1. Open document "ma.aln" and save it to string
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialMsaContent = GTClipboard::text();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a character in the sequence area
    const QPoint initialSelectionPos(10, 10);
    GTUtilsMSAEditorSequenceArea::click(initialSelectionPos);

    // 3. Add gaps by pressing "Space" key
    const int totalShiftCount = 3;
    int shiftCounter = 0;
    QPoint currentSelection = initialSelectionPos;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }
    currentSelection.rx() += totalShiftCount;

    // 4. Remove gaps with "Backspace" key
    shiftCounter = 0;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
        currentSelection.rx() -= 1;
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(currentSelection, currentSelection));
    }

    // 5. Check that alignment content has returned to initial state
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2021_2) {
    // 1. Open document "ma.aln" and save it to string
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardUtils::copy();
    QString initialMsaContent = GTClipboard::text();

    // 2. Select a region in the sequence area
    QPoint initialSelectionLeftTop(5, 5);
    QPoint initialSelectionRightBottom(11, 10);
    GTUtilsMSAEditorSequenceArea::selectArea(initialSelectionLeftTop, initialSelectionRightBottom);

    // 3. Add gaps by pressing "Space" key
    int totalShiftCount = 3;
    int shiftCounter = 0;
    QPoint currentSelectionLeftTop = initialSelectionLeftTop;
    QPoint currentSelectionRightBottom = initialSelectionRightBottom;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }
    currentSelectionLeftTop.rx() += totalShiftCount;
    currentSelectionRightBottom.rx() += totalShiftCount;
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(currentSelectionLeftTop, currentSelectionRightBottom));

    // 4. Remove gaps with "Backspace" key
    shiftCounter = 0;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
        currentSelectionLeftTop.rx() -= 1;
        currentSelectionRightBottom.rx() -= 1;
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(currentSelectionLeftTop, currentSelectionRightBottom));
    }

    // 5. Check that alignment content has returned to initial state
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardUtils::copy();
    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent == initialMsaContent, "MSA has unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2021_3) {
    // 1. Open document "ma2_gap_8_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a column in the sequence area
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click();
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(9);

    // 3. Remove gap columns by pressing "Backspace" key
    const int totalShiftCount = 6;
    int shiftCounter = 0;
    while (shiftCounter++ < totalShiftCount) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    }

    // 4. Check that all the empty columns were removed
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(14, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString finalMsaContent = GTClipboard::text();
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(20, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialMsaContent = GTClipboard::text();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Select a region of trailing gaps
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(17, 4), QPoint(19, 6));

    // 3. Press "Backspace"
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // 4. Check that MSA does not changed
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(20, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");

    // 5. Check that "Undo" and "Redo" buttons are disabled
    const QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button is unexpectedly enabled");
    const QAbstractButton* redo = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(!redo->isEnabled(), "Redo button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2021_5) {
    // 1. Open document "ma2_gap_8_col.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select a column in the sequence area.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(9, 0), QPoint(16, 9));

    // 3. Remove gap columns by pressing "Backspace" key.
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // 4. Check that all the empty columns were removed
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(14, 9));
    GTKeyboardUtils::copy();
    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent == "AAGCTTCTTTTAA--\n"
                                     "AAGTTACTAA-----\n"
                                     "TAG---TTATTAA--\n"
                                     "AAGC---TATTAA--\n"
                                     "TAGTTATTAA-----\n"
                                     "TAGTTATTAA-----\n"
                                     "TAGTTATTAA-----\n"
                                     "AAGCTTT---TAA--\n"
                                     "A--AGAATAATTA--\n"
                                     "AAGCTTTTAA-----",
                  "Unexpected MSA content has occurred: " + finalMsaContent);
}

GUI_TEST_CLASS_DEFINITION(test_2021_6) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    if (GTUtilsProjectTreeView::isVisible()) {
        GTUtilsProjectTreeView::toggleView();
    }

    // 2. Set cursor to the position 45 of the first line (after gaps).
    // const QPoint initialSelectionPos(44, 0);
    // GTUtilsMSAEditorSequenceArea::click(initialSelectionPos);
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(44, 0), QPoint(44, 0));

    // 3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(43, 0));
    GTKeyboardUtils::copy();

    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent == "TAAGACTTCTAATTCGAGCCGAATTAGGTCAACCAGGATAC--C",
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2021_7) {
    // 1. Open test/_common_data/scenarios/msa/ma2_gap_col.aln.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the 8 and 9 of the third line (two symbols after gaps).
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 2), QPoint(8, 2));

    // 3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 2), QPoint(13, 2));
    GTKeyboardUtils::copy();
    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent == "TAG--TTATTAA--",
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2021_8) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    if (GTUtilsProjectTreeView::isVisible()) {
        GTUtilsProjectTreeView::toggleView();
    }

    // 2. Select the 45 and 46 of the second line (two symbols after gaps).
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(44, 1), QPoint(45, 1));

    // 3. Press BACKSPACE
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // 4. Expected state: the gap was deleted, selection moves to the previous symbol.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 1), QPoint(44, 1));
    GTKeyboardUtils::copy();
    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent == "TAAGCTTACTAATCCGGGCCGAATTAGGTCAACCTGGTTAT-CTA",
                  QString("Unexpected MSA content has occurred: got %1").arg(finalMsaContent));
}

GUI_TEST_CLASS_DEFINITION(test_2026) {
    // 1. Open data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select Montana_montana in name list
    GTUtilsMSAEditorSequenceArea::selectSequence(QString("Montana_montana"));

    // 3. Press shift, click down_arrow 4 times. Release shift.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    for (int i = 0; i < 4; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: 5 sequences are selected
    CHECK_SET_ERR(5 == GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(),
                  QString("Unexpected number of selected sequences1. Got %1, Expected %2").arg(GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum()).arg(5));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Montana_montana")),
                  "Expected sequence is not selected");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Zychia_baranovi")),
                  "Expected sequence is not selected");

    // 4. Release shift. Press shift again and click down arrow
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: 6 sequences selected
    CHECK_SET_ERR(6 == GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(),
                  QString("Unexpected number of selected sequences2. Got %1, Expected %2").arg(GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum()).arg(6));
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Montana_montana")),
                  "Expected sequence is not selected");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Zychia_baranovi")),
                  "Expected sequence is not selected");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Tettigonia_viridissima")),
                  "Expected sequence is not selected");
}

GUI_TEST_CLASS_DEFINITION(test_2030) {
    // 1. Open {_common_data/scenarios/msa/ma_one_line.aln}
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma_one_line.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Set cursor to the end of the line
    const QPoint endLinePos(11, 0);
    GTUtilsMSAEditorSequenceArea::click(endLinePos);

    // 3. Insert four gaps with SPACE.
    for (int i = 0; i < 4; ++i) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }

    // 4. Set cursor to the beginning of the line
    const QPoint begLinePos(0, 0);
    GTUtilsMSAEditorSequenceArea::click(begLinePos);

    // 5. Delete one symbol with DELETE
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Expected state: line length is 15.
    CHECK_SET_ERR(15 == GTUtilsMSAEditorSequenceArea::getLength(),
                  "Unexpected MSA length!");
}

GUI_TEST_CLASS_DEFINITION(test_2032) {
    // 1. Open {_common_data/fasta/abcd.fa} as separate sequences
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(testDir, "_common_data/fasta/abcd.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: order of sequences in the project view is {d, a, c, b}
    QModelIndex documentItem = GTUtilsProjectTreeView::findIndex("abcd.fa");

    CHECK_SET_ERR(documentItem.model()->index(0, 0, documentItem).data() == "[s] a", "1. Unexpected name of the object in the project view!");
    CHECK_SET_ERR(documentItem.model()->index(1, 0, documentItem).data() == "[s] b", "2. Unexpected name of the object in the project view!");
    CHECK_SET_ERR(documentItem.model()->index(2, 0, documentItem).data() == "[s] c", "3. Unexpected name of the object in the project view!");
    CHECK_SET_ERR(documentItem.model()->index(3, 0, documentItem).data() == "[s] d", "4. Unexpected name of the object in the project view!");

    // Expected result: order of sequences in the sequences view is {d, a, c, b}
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    auto seq0 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    auto seq1 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_1"));
    auto seq2 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_2"));
    auto seq3 = dynamic_cast<ADVSingleSequenceWidget*>(GTWidget::findWidget("ADV_single_sequence_widget_3"));

    CHECK_SET_ERR(seq0 != nullptr, "1. Failed to find a sequence widget!");
    CHECK_SET_ERR(seq1 != nullptr, "2. Failed to find a sequence widget!");
    CHECK_SET_ERR(seq2 != nullptr, "3. Failed to find a sequence widget!");
    CHECK_SET_ERR(seq3 != nullptr, "4. Failed to find a sequence widget!");

    CHECK_SET_ERR(seq0->getSequenceObject()->getSequenceName() == "d", "1. Unexpected sequence name!");
    CHECK_SET_ERR(seq1->getSequenceObject()->getSequenceName() == "a", "2. Unexpected sequence name!");
    CHECK_SET_ERR(seq2->getSequenceObject()->getSequenceName() == "c", "3. Unexpected sequence name!");
    CHECK_SET_ERR(seq3->getSequenceObject()->getSequenceName() == "b", "4. Unexpected sequence name!");
}

GUI_TEST_CLASS_DEFINITION(test_2049) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Codon table"));
    auto codonTableWidget = GTWidget::findWidget("Codon table widget");
    auto labelBefore = GTWidget::findLabel("row_6_column_2", codonTableWidget);
    CHECK_SET_ERR(labelBefore->text().contains("Leucine (Leu, L)"), "1. Invalid cell text: " + labelBefore->text());
    int heightBefore = labelBefore->geometry().height();

    GTUtilsDialog::waitForDialog(new PopupChooser({"3. The Yeast Mitochondrial Code"}));
    GTWidget::click(GTWidget::findWidget("AminoToolbarButton"));

    codonTableWidget = GTWidget::findWidget("Codon table widget");
    auto labelAfter = GTWidget::findLabel("row_6_column_2", codonTableWidget);
    CHECK_SET_ERR(labelAfter->text().contains("Threonine (Thr, T)"), "2. Invalid cell text: " + labelAfter->text());
    int heightAfter = labelAfter->geometry().height();

    CHECK_SET_ERR(heightBefore == heightAfter, "Codon table layout is changed");
}

GUI_TEST_CLASS_DEFINITION(test_2053) {
    //    1. Remove UGENE .ini file
    //    2. Run UGENE
    //    3. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    4. Run some scheme
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Alignment");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Alignment");
    GTUtilsWorkflowDesigner::connect(read, write);
    GTUtilsWorkflowDesigner::click("Read Alignment");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::runWorkflow();
    //    Expected state: after scheme finish there is the hint on the dashboard -
    GTUtilsTaskTreeView::waitTaskFinished();

    QToolButton* loadSchemaButton = GTUtilsDashboard::findLoadSchemaButton();
    CHECK_SET_ERR(loadSchemaButton, "loadSchemaButton not found");

    CHECK_SET_ERR(loadSchemaButton->toolTip() == "Open workflow schema", "loadSchemaButton has no hint")
}

GUI_TEST_CLASS_DEFINITION(test_2076) {
    //    1) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Run any scheme
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Alignment");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Alignment");
    GTUtilsWorkflowDesigner::connect(read, write);
    GTUtilsWorkflowDesigner::click(read);

    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::saveWorkflowAs(sandBoxDir + "test_2076.uwl", "test_2076");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: workflow monitor appeared

    //    3) Click on the 'Dashboards manager' tool button
    //    Expected state: 'Dashboards manager' dialog appeared
    class custom : public CustomScenario {
    public:
        void run() override {
            //    4) Select some dashboards in the dialog
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    5) Click on the 'Remove selected' button
            DashboardsManagerDialogFiller::selectDashboards({"test_2076 1"});
            //    Expected state: 'Removing dashboards' dialog appeared
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Confirm"));
            //    6) Click on the 'Confirm' button
            GTWidget::click(GTWidget::findWidget("removeButton", dialog));

            //    Expected state: selected dashboards were removed
            bool pres = DashboardsManagerDialogFiller::isDashboardPresent("test_2076 1");
            CHECK_SET_ERR(!pres, "dashboard is unexpectedly present")

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new DashboardsManagerDialogFiller(new custom()));
    GTWidget::click(GTAction::button(GTAction::findAction("Dashboards manager")));
}

GUI_TEST_CLASS_DEFINITION(test_2077) {
    // 1) Open WD

    // 2) Add elements "Read Sequence" and "Write sequence" to the scheme
    // 3) Connect "Read Sequence" to "Write sequence"

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    WorkflowProcessItem* seqReader = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    WorkflowProcessItem* seqWriter = GTUtilsWorkflowDesigner::getWorker("Write Sequence");

    GTUtilsWorkflowDesigner::connect(seqReader, seqWriter);

    // 4) Add file "human_T1.fa" to "Read Sequence" twice

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    // 5) Validate scheme
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Workflow is valid"));
    GTWidget::click(GTAction::button("Validate workflow"));

    // Expected state: The "File not found" error has appeared in the "Error list"
    GTUtilsWorkflowDesigner::checkErrorList("file '../human_T1.fa' was specified several times");
}

GUI_TEST_CLASS_DEFINITION(test_2089) {
    // 1. Start UGENE with a new *.ini file.
    GTUtilsDialog::waitForDialog(new StartupDialogFiller("!@#$%^&*()_+\";:?/", false));

    // 2. Open WD
    // 3. Set any folder without write access as workflow output folder. Click OK.
    GTMenu::clickMainMenuItem({"Tools", "Workflow Designer..."});
}

GUI_TEST_CLASS_DEFINITION(test_2100_1) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click toolbutton "Enable collapsing"
    GTUtilsMsaEditor::toggleCollapsingMode();

    // expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are collapsed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    // 3. Select Mecopoda_sp.__Malaysia_
    GTUtilsMSAEditorSequenceArea::selectSequence(QString("Mecopoda_sp.__Malaysia_"));

    // 4. Expand Mecopoda_elongata__Ishigaki__J

    // Expected state: Mecopoda_sp.__Malaysia_ is still selected
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Mecopoda_sp.__Malaysia_")),
                  "Expected sequence is not selected");
}

GUI_TEST_CLASS_DEFINITION(test_2100_2) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click toolbutton "Enable collapsing"
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select Mecopoda_sp.__Malaysia_
    GTUtilsMSAEditorSequenceArea::selectSequence(QString("Mecopoda_sp.__Malaysia_"));

    // 4. Expand Select Mecopoda_sp.__Malaysia_
    // Expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are selected
}

GUI_TEST_CLASS_DEFINITION(test_2124) {
    // 1. Open "data/samples/CLUSTALW/ty3.aln.gz".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "ty3.aln.gz");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Call the context menu on the sequence area.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(1, 1));
    QString colorSchemeName = name + "_Scheme";
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTUtilsDialog::add(new NewColorSchemeCreator(colorSchemeName, NewColorSchemeCreator::amino));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    // 3. Create a new color scheme for the amino alphabet.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(1, 1));
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", colorSchemeName}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_2091) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: document are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex("COI.aln");

    // 2. Select any sequence.
    GTUtilsMSAEditorSequenceArea::click(QPoint(-5, 4));
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 3. Call context menu on the name list area, select the {Edit -> Remove sequence} menu item.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "Remove sequence"}));
    GTMouseDriver::click(Qt::RightButton);
    // Expected state: the sequence is removed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 1, "The number of sequences remained unchanged.");
    CHECK_SET_ERR(!modifiedNames.contains("Montana_montana"), "Removed sequence is present in multiple alignment.");
}

GUI_TEST_CLASS_DEFINITION(test_2093_1) {
    //    1. Run a scheme, e.g. "Call variants with SAMtools" from the NGS samples (or any other like read->write).
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // Simple scheme: read file list.
    GTUtilsWorkflowDesigner::addAlgorithm("Read File URL(s)");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    GTUtilsWorkflowDesigner::runWorkflow();

    //    2. Select "Load schema" button on the dashboard menu line.
    QToolButton* loadSchemaButton = GTUtilsDashboard::findLoadSchemaButton();
    CHECK_SET_ERR(loadSchemaButton, "loadSchemaButton not found");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Discard));
    GTWidget::click(loadSchemaButton);

    //    Expected result: the scheme with parameters is loaded.
    WorkflowProcessItem* wdElement = GTUtilsWorkflowDesigner::getWorker("Read File URL(s)");
    CHECK_SET_ERR(wdElement, "Schema wasn't loaded");
}

GUI_TEST_CLASS_DEFINITION(test_2093_2) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Open any shema with the "Load workflow" button on the toolbar (not the "Open" button!)
    QString schemaPath = testDir + "_common_data/scenarios/workflow designer/222.uwl";
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(schemaPath));

    QToolBar* wdToolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    CHECK_SET_ERR(wdToolbar, "Toolbar not found");
    QWidget* loadButton = GTToolbar::getWidgetForActionObjectName(wdToolbar, "Load workflow");
    CHECK_SET_ERR(loadButton, "Load button not found");
    GTWidget::click(loadButton);

    // Expected result: the scheme with parameters is loaded.

    WorkflowProcessItem* wdElement = GTUtilsWorkflowDesigner::getWorker("Read sequence");
    CHECK_SET_ERR(wdElement, "Schema wasn't loaded");
}

GUI_TEST_CLASS_DEFINITION(test_2128) {
    // 1. Open document "ma.aln" and save it to string
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardUtils::copy();
    QString initialMsaContent = GTClipboard::text();

    // 2. Select a region in the sequence area.
    QRect currentSelection(QPoint(2, 5), QPoint(8, 11));
    GTUtilsMSAEditorSequenceArea::selectArea(currentSelection.topLeft(), currentSelection.bottomRight());

    // 3. Add gaps by pressing "Space" key.
    int totalShiftCount = 3;
    for (int shiftCounter = 0; shiftCounter < totalShiftCount; ++shiftCounter) {
        GTKeyboardDriver::keyClick(Qt::Key_Space);
        currentSelection.moveRight(currentSelection.right() + 1);
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(currentSelection);
    }

    // 4. Remove gaps with "Backspace" key.
    for (int shiftCounter = 0; shiftCounter < totalShiftCount; ++shiftCounter) {
        GTKeyboardDriver::keyClick(Qt::Key_Backspace);
        currentSelection.moveLeft(currentSelection.left() - 1);
        GTUtilsMSAEditorSequenceArea::checkSelectedRect(currentSelection);
    }

    // 5. Check that alignment content has returned to initial state
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));
    GTKeyboardUtils::copy();
    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(initialMsaContent == finalMsaContent, "MSA has unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2128_1) {
    // 1. Open "data/samples/CLUSTAL/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the six first symbols in the first sequence and press Space.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(5, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Expected state: The first symbol T is on the 2nd position.
    GTUtilsMSAEditorSequenceArea::click({1, 0});
    GTKeyboardUtils::copy();
    QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent == "T", "1. Unexpected MSA content");

    // 3. Press the Space.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(4, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Expected state: The first symbol T is on the 3rd position.
    GTUtilsMSAEditorSequenceArea::click({2, 0});
    GTKeyboardUtils::copy();
    QString finalMsaContent1 = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent1 == "T", "2. Unexpected MSA content");

    // 4. Press the Backspace.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 0), QPoint(5, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // Expected state: The first symbol T is on the 2nd position.
    GTUtilsMSAEditorSequenceArea::click({1, 0});
    GTKeyboardUtils::copy();
    QString finalMsaContent2 = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent2 == "T", "3. Unexpected MSA content");

    // 5. Press the Backspace.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(5, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // Expected state: The first symbol T is on the 1 position.
    GTUtilsMSAEditorSequenceArea::click({0, 0});
    GTKeyboardUtils::copy();
    QString finalMsaContent3 = GTClipboard::text();
    CHECK_SET_ERR(finalMsaContent3 == "T", "Unexpected MSA content has occurred");
}

GUI_TEST_CLASS_DEFINITION(test_2138) {
    // 1. Open _common_data/scenarios/_regression/2138/1.fa
    // 2. Press "Join sequences into alignment..." radio button
    // 3. Press "OK" button
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/2138/1.fa");

    // Expected state: alignment has been opened and whole msa alphabet is amino
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    bool isAmino = GTUtilsMSAEditorSequenceArea::hasAminoAlphabet();
    CHECK_SET_ERR(isAmino, "Alignment has wrong alphabet type");
}

GUI_TEST_CLASS_DEFINITION(test_2140) {
    // 1. Use main menu {Tools->Dna assembly->convert UGENE assembly database to SAM format}
    // 2. Select any .fasta file instead of .ugenedb file
    // 3. Click "convert"
    // Expected state: UGENE not crashes. Error message is written to log

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ConvertAssemblyToSAMDialogFiller(dataDir + "samples/FASTA/", "human_T1.fa"));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Convert UGENE assembly database to SAM..."});

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2144) {
    //    1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Open the NGS sample scheme "Call variants with SAM tools".
    GTUtilsWorkflowDesigner::addSample("Call variants with SAMtools");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    3. Fill input data, e.g.:
    //        "data/samples/Assembly/chrM.sam" as input to "Read Assembly SAM/BAM" element;
    //        "data/samples/Assembly/chrM.fa" as input to "Read Sequence" element;
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.fa");

    //    4. Choose "Estimate" option in tool bar.
    //       "Estimate" option is available only for NGS samples (except "Extract transcript sequence").
    //    Expected state: Estimation dialog appears and provides information about approximate time of workflow run.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Close, "Approximate estimation time of the workflow run is"));
    GTWidget::click(GTAction::button("Estimate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_2150) {
    // 1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Open the "Align sequences with MUSCLE" sample scheme.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Set "data/samples/CLUSTALW/ty3.aln.gz" as the input file.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");

    // 4. Set some name to the result file.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write alignment"));
    GTMouseDriver::click();

    QString s = QFileInfo(testDir + "_common_data/scenarios/sandbox/").absoluteFilePath();
    GTKeyboardDriver::keySequence(s + "/2150_0001.sto");
    GTUtilsWorkflowDesigner::setParameter("Output file", s + "/2150_0001.aln", GTUtilsWorkflowDesigner::textValue);
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 5. Run the workflow.
    GTWidget::click(GTAction::button("Run workflow"));
    GTUtilsTask::waitTaskStart("MUSCLE alignment", 10000);

    // 6. During the workflow execution open the "Tasks" panel in the bottom, find in the task tree the "MUSCLE alignment" subtask and cancel it.
    GTUtilsTaskTreeView::cancelTask("MUSCLE alignment", true, {"Execute workflow", "Workflow run", "Wrapper task for: \"MUSCLE alignment\""});
}

GUI_TEST_CLASS_DEFINITION(test_2152) {
    // 1. Open WD
    // 2. Create a scheme with the following elements: Read File URL(s), Assembly Sequences with CAP3
    // 3. Put _common_data/cap3/region1.fa,
    //        _common_data/cap3/region2.fa,
    //        _common_data/cap3/region3.fa,
    //        _common_data/cap3/region4.fa as an input sequences
    // 4. Run the scheme
    // Expected state: the dashboard appears, the WD task has been finished without errors.

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read File URL(s)");
    GTUtilsWorkflowDesigner::addAlgorithm("Assembly Sequences with CAP3");

    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::getWorker("Read File URL(s)");
    WorkflowProcessItem* fileCAP3 = GTUtilsWorkflowDesigner::getWorker("Assembly Sequences with CAP3");

    GTUtilsWorkflowDesigner::connect(fileList, fileCAP3);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Assembly Sequences with CAP3"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir().absoluteFilePath(sandBoxDir + "out.ace"), GTUtilsWorkflowDesigner::textValue);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cap3/region1.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cap3/region2.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cap3/region3.fa");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cap3/region4.fa");

    GTWidget::click(GTAction::button("Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2156) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::openView();
    GTUtilsProjectTreeView::toggleView();
    //    2. Select six symbols (45-50) of the first line.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(44, 0), QPoint(49, 0));
    //    3. Press BACKSPACE 4 times.
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    //    Expected state: three gaps before the selected area are removed.
    GTWidget::click(GTUtilsMdi::activeWindow());
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 0), QPoint(44, 0));
    GTKeyboardUtils::copy();
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == "CTAA", QString("Expected: CTAA, found: %1").arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_2157) {
    //    1. Open file "https://ugene.net/tracker/secure/attachment/12864/pdb1a07.ent.gz".
    GTFileDialog::openFile(testDir + "_common_data/pdb/", "pdb1a07.ent.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: UGENE displays sequence and 3d structure
    GTUtilsMdi::findWindow("pdb1a07.ent.gz");
}

GUI_TEST_CLASS_DEFINITION(test_2160) {
    // 1. Open document "ma.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select whole msa
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(11, 17));

    // 3. Shift msa
    const QPoint mouseDragPosition(7, 7);
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(3, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    // 4. Press "Delete" key
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 6. Check that alignment has not changed
    CHECK_SET_ERR(15 == GTUtilsMSAEditorSequenceArea::getLength(),
                  "MSA length unexpectedly changed");
    CHECK_SET_ERR(18 == GTUtilsMSAEditorSequenceArea::getNameList().size(),
                  "MSA row count unexpectedly changed");
}

GUI_TEST_CLASS_DEFINITION(test_2165) {
    // 1. Open human_t1
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Copy the whole sequence to the clipboard
    GTWidget::click(GTUtilsSequenceView::getPanOrDetView());
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller());
    GTWidget::click(GTWidget::findWidget("select_range_action"));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // 3. Past the whole sequence to the find pattern field
    GTWidget::click(GTWidget::findWidget("OP_FIND_PATTERN"));
    GTWidget::click(GTWidget::findWidget("textPattern"));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    GTUtilsNotifications::checkNotificationDialogText("too long");
}

GUI_TEST_CLASS_DEFINITION(test_2188) {
    class Helper {
    public:
        Helper(const QString& dataDir, const QString& testDir) {
            dir = testDir + "_common_data/scenarios/sandbox/";
            fileName = "regression_test_2188.fa";
            url = dir + fileName;
            bool copied = QFile::copy(dataDir + "samples/FASTA/human_T1.fa", url);
            if (!copied) {
                GT_FAIL("Can not copy the file", );
            }
        }

        QString url;
        QString dir;
        QString fileName;
    };

    // 1. Open the file "data/samples/FASTA/human_T1.fa"
    Helper helper(dataDir, testDir);

    GTFileDialog::openFile(helper.dir, helper.fileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. At the same time open the file with a text editor
    // 3. Change something and save
    // Expected state: Dialog suggesting file to reload has appeared in UGENE
    // 4. Press "Yes to All" button
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    QFile file(helper.url);
    bool opened = file.open(QIODevice::Append);
    CHECK_SET_ERR(opened, "Can not open the file for writing");
    qint64 writed = file.write("AAAAAAAAAA");
    CHECK_SET_ERR(10 == writed, "Can not write to the file");
    file.close();
    GTGlobals::sleep(6000);  // Wait for the dialog.

    // Expected state: All the sequences were reloaded and displayed correctly in sequence view
    int length = GTUtilsSequenceView::getLengthOfSequence();

    CHECK_SET_ERR(199960 == length, "The file length is wrong");
}

GUI_TEST_CLASS_DEFINITION(test_2187) {
    // 1. Open "data/samples/FASTA/human_T1.fa"
    // d:\src\ugene\trunk\test\_common_data\scenarios\_regression\2187\seq.fa

    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/2187/", "seq.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("seq.fa");

    // 2. Open {Actions -> Analyze -> Find tandems...}
    // 3. Click ok

    GTUtilsDialog::waitForDialog(new FindTandemsDialogFiller(testDir + "_common_data/scenarios/sandbox/result_2187.gb"));

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find tandem repeats..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* annotationsRoot = GTUtilsAnnotationsTreeView::findItem("repeat_unit  (0, 5)");
    GTTreeWidget::expand(annotationsRoot);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(annotationsRoot->child(0)));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditAnnotationChecker("repeat_unit", "251..251,252..252,253..253,254..254,255..255,256..256,257..257,258..258,259..259"));
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems("repeat_unit");
    for (QTreeWidgetItem* item : qAsConst(items)) {
        if (item->text(2) == "251..251,252..252,253..253,254..254,255..255,256..256,257..257,258..258,259..259") {
            CHECK_SET_ERR("9" == GTUtilsAnnotationsTreeView::getQualifierValue("num_of_repeats", item), "Wrong num_of_repeats value");
            CHECK_SET_ERR("1" == GTUtilsAnnotationsTreeView::getQualifierValue("repeat_length", item), "Wrong repeat_length value");
            CHECK_SET_ERR("9" == GTUtilsAnnotationsTreeView::getQualifierValue("whole_length", item), "Wrong whole_length value");
            break;
        }
    }

    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsMdi::click(GTGlobals::Close);
}

GUI_TEST_CLASS_DEFINITION(test_2192) {
    QString samtoolsPath = isOsWindows() ? "samtools\\samtools" : "samtools/samtools";
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open Call Variants sample.
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/chrM.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cmdline/call-variations/chrM.fa");
    //    4. Start the scheme.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    5. Open External Tools tab & copy sam tools path
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);
    QWidget* samToolsRunNode = GTUtilsDashboard::getCopyButton("actor_call_variants_run_1_tool_SAMtools_run_1");
    GTWidget::click(samToolsRunNode);

    QString text = GTClipboard::text();
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

    GTUtilsDialog::waitForDialog(new StartupDialogFiller(workflowOutputDir.absolutePath()));
    GTMenu::clickMainMenuItem({"Tools", "Workflow Designer..."});

    CHECK_SET_ERR(workflowOutputDir.exists(), "Dir wasn't created");
}

GUI_TEST_CLASS_DEFINITION(test_2225) {
    GTUtilsDialog::waitForDialog(new NCBISearchDialogSimpleFiller("rat", true));
    GTMenu::clickMainMenuItem({"File", "Search NCBI GenBank..."}, GTGlobals::UseKey);
}

GUI_TEST_CLASS_DEFINITION(test_2259) {
    MainWindow* mw = AppContext::getMainWindow();
    CHECK_SET_ERR(mw != nullptr, "MainWindow is NULL");
    QMainWindow* mainWindow = mw->getQMainWindow();
    CHECK_SET_ERR(mainWindow != nullptr, "QMainWindow is NULL");

    QAction* menu = mainWindow->findChild<QAction*>(MWMENU_SETTINGS);
    CHECK_SET_ERR(menu->menu()->actions().size() == 2, "wrong number of actions");
}

GUI_TEST_CLASS_DEFINITION(test_2266_1) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/ugenedb/Klebsislla.sort.bam.ugenedb");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/ugenedb/Klebsislla_ref.fa");

    GTUtilsWorkflowDesigner::click("Call Variants");
    GTUtilsWorkflowDesigner::setParameter("Output variants file", QDir(sandBoxDir).absolutePath() + "/test_2266_1.vcf", GTUtilsWorkflowDesigner::lineEditWithFileSelector);

    GTWidget::click(GTAction::button("Run workflow"));

    GTUtilsTaskTreeView::waitTaskFinished(6000000);

    GTFileDialog::openFile(sandBoxDir + "test_2266_1.vcf");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::findIndex("pkF70");
    GTUtilsProjectTreeView::findIndex("pkf140");
}
GUI_TEST_CLASS_DEFINITION(test_2267_1) {
    //     1. Open human_T1.fa
    //
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //     2. Select random subsequence, press Ctrl+N, press Enter
    //
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "DDD", "D", "10..16"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});
    //     3. Press Insert, press '1' key until there is no new symbols in lineedit
    //      Current state: no error message for long qualifier
    Runnable* filler = new EditQualifierFiller("111111111111111111111111111111111111111111111111111111111111111111111111111111111", "val", true, false);
    GTUtilsDialog::waitForDialog(filler);

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("D"));
    GTMouseDriver::click(Qt::RightButton);

    //     4. Press Enter
    //     Expected state: Edit qualifier window closes
}

GUI_TEST_CLASS_DEFINITION(test_2267_2) {
    //     1. Open human_T1.fa
    //
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //     2. Select random subsequence, press Ctrl+N, press Enter
    //
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "DDD", "D", "10..16"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});
    //     3. Press Insert, set the name of qualifier: '))()((_'
    //
    //     4. Press Enter
    //     Expected state: Error message appears once
    Runnable* filler = new EditQualifierFiller("))()((_", "val", true, true);
    GTUtilsDialog::waitForDialog(filler);

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("D"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2268) {
    //    0. Copy t-coffee tool to the place where UGENE has enough permissions to change file permissions;
    //    Set the copied t-coffee tool in preferences.

    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    CHECK_SET_ERR(etRegistry, "External tool registry is NULL");

    ExternalTool* tCoffee = etRegistry->getById("USUPP_T_COFFEE");
    CHECK_SET_ERR(tCoffee, "T-coffee tool is NULL");

    const QFileInfo origToolPath(tCoffee->getPath());
    CHECK_SET_ERR(origToolPath.exists(), "T-coffee tool is not set");

    QDir origToolDir = origToolPath.dir();
    if (isOsLinux()) {
        origToolDir.cdUp();  // exit from 'bin' folder
    }

    QString newToolDir = sandBoxDir + GTUtils::genUniqueString("test_2268") + "/";
    GTFile::copyDir(origToolDir.absolutePath(), newToolDir);
    QFileInfo newToolPath(newToolDir + (isOsLinux() ? "bin/t_coffee" : (isOsWindows() ? "t_coffee.bat" : "t_coffee")));

    // Hack, it is better to set the tool path via the preferences dialog
    CHECK_SET_ERR(newToolPath.exists(), "The copied T-coffee tool does not exist");
    tCoffee->setPath(newToolPath.absoluteFilePath());

    // 1. Forbid write access to the t-coffee folder recursively (chmod 555 -R %t-coffee-dir%).
    GTFile::setReadOnly(newToolDir, true);

    // 2. Open "_common_data/clustal/align.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/align.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Right click on the MSA -> Align -> Align with T-Coffee.
    // 4. Click the "Align" button.
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new TCoffeeDailogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with T-Coffee"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    //    Expected: the t-coffee task started and finished well.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2314) {
    //    1. Open 'COI.aln'
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Scroll sequence area to the last columns
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(604);

    //    3. Select to the whole last column by clicking consensus area.
    auto consArea = GTWidget::findWidget("consArea");
    GTWidget::click(consArea, Qt::LeftButton, QPoint(consArea->geometry().right() - 1, consArea->geometry().height() / 2));

    //    3.1 Move the selected column with a mouse to the right. Do not click -> it will reset the selection.
    GTUtilsMSAEditorSequenceArea::moveMouseToPosition(QPoint(603, 0));
    QPoint p = GTMouseDriver::getMousePosition();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(p.x() + 30, p.y()));
    GTMouseDriver::release();

    //    Expected state: the column was moved
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength() > 604, "The length of the alignment is not changed");

    GTUtilsMsaEditor::undo();

    //    4. Move to the right 1 base region, that is close to the end of alignment
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(600, 5));
    p = GTMouseDriver::getMousePosition();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(p.x() + 30, p.y()));
    GTMouseDriver::release();

    //    Expected state: the region is moved if mouse goes beyond the right border of the alignment
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength() > 604, "The length of the alignment is not changed");
}

GUI_TEST_CLASS_DEFINITION(test_2316) {
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTFileDialog::openFile(dataDir + "samples/../workflow_samples/Alignment", "basic_align.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!GTUtilsProjectTreeView::isVisible(), "Project tree view is visible");

    //     1. Start UGENE
    //
    //     2. Drag an .uwl file inside UGENE to open a workflow
    //     Expected state: now the project view is collapsed if a file.uwl is drag'n'dropped to UGENE when there is no project yet.
}
GUI_TEST_CLASS_DEFINITION(test_2269) {
    // 1. Use main menu: {tools->Align short reeds}
    // 2. Select Bowtie2 as a mapping tool
    // 3. Try to set incorrect value in "Seed lingth" spinbox(Correct boundaries are: >3, <32)
    AlignShortReadsFiller::Bowtie2Parameters parameters(testDir + "_common_data/scenarios/_regression/1093/",
                                                        "refrence.fa",
                                                        testDir + "_common_data/scenarios/_regression/1093/",
                                                        "read.fa");
    parameters.seedLengthCheckBox = true;
    parameters.seedLength = 33;

    class Scenario_test_2269 : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto methodNamesBox = GTWidget::findComboBox("methodNamesBox", dialog);
            GTComboBox::selectItemByText(methodNamesBox, "Bowtie2");

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/1093/refrence.fa"));
            auto addRefButton = GTWidget::findWidget("addRefButton", dialog);
            GTWidget::click(addRefButton);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/1093/read.fa"));
            auto addShortreadsButton = GTWidget::findWidget("addShortreadsButton", dialog);
            GTWidget::click(addShortreadsButton);

            auto seedCheckBox = GTWidget::findCheckBox("seedlenCheckBox", dialog);
            GTCheckBox::setChecked(seedCheckBox, true);

            auto seedSpinBox = GTWidget::findSpinBox("seedlenSpinBox", dialog);
            int max = seedSpinBox->maximum();
            CHECK_SET_ERR(max == 31, QString("wrong seed maximim: %1").arg(max));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new Scenario_test_2269()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_2270) {
    // 1. Open file "data/cmdline/snp.uwl"
    // Ecpected state: scheme opened in WD without problems
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "cmdline/", "snp.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2281) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    auto tabs = GTWidget::findTabWidget("tabs");

    // 2. Click the "samples" bar. The samples hint is shown
    GTTabWidget::setCurrentIndex(tabs, 1);
    auto sceneView = GTWidget::findGraphicsView("sceneView");

    QImage img = GTWidget::getImage(sceneView);

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

    // 3. Click the "elements" bar.
    GTTabWidget::setCurrentIndex(tabs, 0);

    // Expected: the samples hint is hidden
    img = GTWidget::getImage(sceneView);
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
    GTFileDialog::openFile(testDir + "_common_data/ugenedb/", "example-alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_2292.ace.ugenedb"));
    GTUtilsProject::openFile(dataDir + "samples/ACE/K26.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_2295) {
    // 1. Open samples/APR/DNA.apr in read-only mode
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/APR/DNA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: Alignent is locked
    bool isLocked = GTUtilsMSAEditorSequenceArea::isAlignmentLocked();
    CHECK_SET_ERR(isLocked, "Alignment is unexpectably unlocked");

    // 2. Export alignment to read-write format
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(0, sandBoxDir + "DNA"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_PROJECT__EXPORT_TO_AMINO_ACTION}));
    GTUtilsProjectTreeView::callContextMenu("DNA.apr");

    // Expected: Alignent is locked
    isLocked = GTUtilsMSAEditorSequenceArea::isAlignmentLocked();
    CHECK_SET_ERR(!isLocked, "Alignment is unexpectably locked");
}

GUI_TEST_CLASS_DEFINITION(test_2298) {
    //    1. Open the file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Let more space for the tree view.

    //    2. Build and show a tree
    GTUtilsDialog::add(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/2298.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Collapse any node in the tree
    auto node = GTUtilsPhyTree::getNodeByBranchText("0.008", "0.003");
    auto treeView = GTWidget::findGraphicsView("treeView");
    treeView->ensureVisible(node);
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(GTUtilsPhyTree::getGlobalCenterCoord(node));
    GTMouseDriver::doubleClick();

    //    Expected state: the appropriate sequences in the msa view were collapsed into a group as well
    QStringList visibleNames = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    int num = visibleNames.count();
    CHECK_SET_ERR(num == 3, QString("Unexpected visible sequences count. Expected: 3, actual: %1").arg(num));
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
    GTUtilsExternalTools::removeTool("Bowtie 2 build indexer");

    class CheckBowtie2Filler : public Filler {
    public:
        CheckBowtie2Filler()
            : Filler("BuildIndexFromRefDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto methodNamesBox = GTWidget::findComboBox("methodNamesBox", dialog);
            for (int i = 0; i < methodNamesBox->count(); i++) {
                if (methodNamesBox->itemText(i) == "Bowtie2") {
                    GTComboBox::selectItemByIndex(methodNamesBox, i);
                }
            }

            GTFileDialogUtils* ob = new GTFileDialogUtils(dataDir + "samples/FASTA/", "human_T1.fa");
            GTUtilsDialog::waitForDialog(ob);
            GTWidget::click(GTWidget::findWidget("addRefButton", dialog));

            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("No"));
            QPushButton* okButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != nullptr, "ok button is NULL");
            GTWidget::click(okButton);

            QPushButton* cancelButton = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(cancelButton != nullptr, "Cancel button is NULL");
            GTWidget::click(cancelButton);
        }
    };

    GTUtilsDialog::waitForDialog(new CheckBowtie2Filler());
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Build index for reads mapping..."});
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
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(assFileName));
    GTFileDialog::openFile(testDir + "_common_data/bam", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3) The Project View with document "chrM.sorted.bam.ugenedb" has been opened.
    GTUtilsProjectTreeView::findIndex(assDocName);

    GTUtilsMdi::closeWindow(GTUtilsMdi::activeWindowTitle());

    // 3. Delete "chrM.sorted.bam.ugenedb" from the file system (i.e. not from UGENE).
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    bool deleteResult = QFile::remove(QFileInfo(assFileName).absoluteFilePath());
    CHECK_SET_ERR(deleteResult, "Unable to remove assembly file");
    GTGlobals::sleep(5000);  // Wait for the dialog.
}

GUI_TEST_CLASS_DEFINITION(test_2284) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press the "Switch on/off collapsing" button
    GTUtilsMsaEditor::toggleCollapsingMode();

    // expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are collapsed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    // 3. Select the first base in last row
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 16), QPoint(0, 16));

    // 4. In status bar search field type "AATT"
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keySequence("AATT");

    // 5. Press the "Find forward" button a few times until selection reaches the end of the alignment
    // 6. Press the button again
    for (int i = 0; i < 11; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2285) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click the "Switch on/off collapsing" button on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();
    //    Expected state: Collapsed mode is switched on, there are one collapsed group.
    int visableNamesNum = GTUtilsMSAEditorSequenceArea::getVisibleNames().count();
    CHECK_SET_ERR(visableNamesNum == 17,
                  QString("unexpected visable sequences number. Expected 17, actual: %1").arg(visableNamesNum));
    //    3. Expand the collapsed group ("Mecopoda_elongata__Ishigaki__J" is the head sequence).
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Mecopoda_elongata__Ishigaki__J");

    //    4. Set the cursor to the 14 line (the "Mecopoda_elongata__Ishigaki__J" sequence), 45 base.

    auto documentTreeWidget = GTWidget::findWidget(GTUtilsProjectTreeView::widgetName, nullptr, {false});
    if (documentTreeWidget != nullptr) {
        GTUtilsProjectTreeView::toggleView();
    }
    GTUtilsMSAEditorSequenceArea::click(QPoint(44, 13));
    //    5. Press Backspace.
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    //    Expected state: each sequence in the group loose one gap.
    GTUtilsMSAEditorSequenceArea::click();
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(43, 13), QPoint(43, 14));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipboardText = GTClipboard::text();

    CHECK_SET_ERR(clipboardText == "T\n-", "Unexpected selection. Expected: T\nT, actual: " + clipboardText);
}

GUI_TEST_CLASS_DEFINITION(test_2306) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Turn on collapsing mode in MSA
    GTUtilsMsaEditor::toggleCollapsingMode();

    // expected state: Mecopoda_elongata__Ishigaki__J and Mecopoda_elongata__Sumatra_ are collapsed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 10), QPoint(15, 15));

    // 3. Shift the region but don't release left mouse button
    const QPoint mouseDragPosition(12, 12);
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition);
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(mouseDragPosition + QPoint(3, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTThread::waitForMainThread();

    // 4. Call context menu
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 10), QPoint(7, 15));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString finalMsaContent = GTClipboard::text();
    CHECK_SET_ERR("---\n---\n---\n---\n---\n---" == finalMsaContent, "Unexpected MSA content has occurred" + finalMsaContent);
}

GUI_TEST_CLASS_DEFINITION(test_2309) {
    // 1. Open file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    // 2. Build tree for the alignment
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(testDir + "_common_data/scenarios/sandbox/2309.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::toggleView();  // Hide project tree view to ensure that all buttons on the toolbar are visible.

    QStringList initialNames = GTUtilsMSAEditorSequenceArea::getNameList();

    GTWidget::click(GTAction::button("Refresh tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList newNames = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(newNames == initialNames, "Wrong sequences order");
}

GUI_TEST_CLASS_DEFINITION(test_2318) {
    class FirstItemPopupChooser : public PopupChooser {
    public:
        FirstItemPopupChooser()
            : PopupChooser(QStringList()) {
        }

        virtual void run() {
            GTMouseDriver::release();
            GTKeyboardDriver::keyClick(Qt::Key_Down);
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };

    class PlusClicker : public FindAnnotationCollocationsDialogFiller {
    public:
        PlusClicker(const QString& _annName)
            : FindAnnotationCollocationsDialogFiller(),
              annName(_annName) {
        }

        virtual void run() {
            QToolButton* plusButton = getPlusButton();
            CHECK_SET_ERR(plusButton, "First plus toolbutton is NULL");
            firstSize = plusButton->size();

            GTUtilsDialog::waitForDialog(new FirstItemPopupChooser());
            GTWidget::click(plusButton);

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
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2) Use context menu: {Analyze -> Find annotated regions}
    //    3) Click plus button, select any annotation
    //    Expected state: plus button changes place, its size wasn't change
    QToolBar* toolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    CHECK_SET_ERR(toolbar, "Toolbar is NULL");
    QWidget* farButton = GTToolbar::getWidgetForActionTooltip(toolbar, "Find annotated regions");
    CHECK_SET_ERR(farButton, "Find annotated region button is NULL");

    // Expected state is checked in PlusClicker
    GTUtilsDialog::waitForDialog(new PlusClicker("CDS"));
    GTWidget::click(farButton);
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
        RapidProjectCreator(const QString& _projectName, const QString& _projectFolder, const QString& _projectFile)
            : Filler("CreateNewProjectDialog"),
              projectName(_projectName),
              projectFolder(_projectFolder),
              projectFile(_projectFile) {
        }

        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTLineEdit::setText("projectNameEdit", projectName, dialog);
            GTLineEdit::setText("projectFilePathEdit", projectFolder + "/" + projectFile, dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

    private:
        QString projectName;
        QString projectFolder;
        QString projectFile;
    };

    QString projectName = "test_2351";
    QString projectFolder = testDir + "_common_data/scenarios/sandbox";
    QString projectFile = "test_2351";

    for (int i = 0; i < 10; ++i) {
        GTUtilsDialog::waitForDialog(new RapidProjectCreator(projectName, projectFolder, projectFile));
        GTWidget::click(
            GTToolbar::getWidgetForActionObjectName(
                GTToolbar::getToolbar(MWTOOLBAR_MAIN),
                ACTION_PROJECTSUPPORT__NEW_PROJECT));
    }
}

GUI_TEST_CLASS_DEFINITION(test_2343) {
    //    1. Open Workflow designer
    //    2. Add element "Align with ClustalW"
    //    3. Select the element
    //    Expected state: property widget for the element appeared
    //    4. Select "Gap distance" parameter in the property widget
    //    5. Press "Tab" key
    //    Expected state: focus setted to "End Gaps" value

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Align with ClustalW");
    GTUtilsWorkflowDesigner::click("Align with ClustalW");

    GTUtilsWorkflowDesigner::clickParameter("Gap distance");
    GTMouseDriver::doubleClick();
    QWidget* wgt = QApplication::focusWidget();
    CHECK_SET_ERR(wgt != nullptr, "No widnget in focus")

    GTKeyboardDriver::keyClick(Qt::Key_Tab);

    CHECK_SET_ERR(QApplication::focusWidget() != nullptr, "No widget in focus");
    CHECK_SET_ERR(wgt != QApplication::focusWidget(), "Focus didn't changed");
}

GUI_TEST_CLASS_DEFINITION(test_2352) {
    // 1. Close current project or open empty UGENE
    // 2. Open menu {Tools->Build dotplot...}
    // 3. Select any binary file as first file in dialog
    // Expected state: file is not selected, no crash

    QString randomBinaryFile = QCoreApplication::applicationFilePath();

    GTUtilsDialog::add(new BuildDotPlotFiller(randomBinaryFile, randomBinaryFile, false, false, false, 5, 5, true));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
}

GUI_TEST_CLASS_DEFINITION(test_2360) {
    // 1. Open "data/samples/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Right click on document in project.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));

    // 3. Choose the context menu {Export/Import->Export nucleic alignment to amino translation}.
    // Expected state: Export dialog appears.
    // 4. Set "File format to use" to PHYLIP Sequantial.
    // 5. Click "Export".
    GTUtilsDialog::waitForDialog(new ExportMSA2MSADialogFiller(6));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_PROJECT__EXPORT_TO_AMINO_ACTION}));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2364) {
    // 1. Open WD.'
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Create a workflow: Read sequence -> Write sequence.
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    // 3. Set the input sequence file: "data/samples/FASTA/human_T1.fa".
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    // 4. Set the output file: "out.fa".
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", "out.fa", GTUtilsWorkflowDesigner::textValue);

    // 5. Validate the workflow.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Validate workflow"));

    // Expected: the workflow has the warning about FASTA format and annotations.
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::checkErrorList("") != 0, "There is no any messages in infoLog");

    // 6. Run the workflow.
    GTWidget::click(GTAction::button("Run workflow"));

    // Expected: the button "Load schema" is shown.
    //  expected button is in dashboard - it can't be checked for now
}

GUI_TEST_CLASS_DEFINITION(test_2373) {
    GTLogTracer lt;

    //    1. Open "COI.aln"
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Move 'Mecopoda_elongata__Ishigaki__J' and 'Mecopoda_elongata__Sumatra_' to the end of name list
    GTUtilsMsaEditor::replaceSequence("Mecopoda_elongata__Ishigaki__J", 17);
    GTUtilsMsaEditor::replaceSequence("Mecopoda_elongata__Sumatra_", 17);

    //    3. Press 'Switch on/off collapsing mode' tool button
    //    Expected state: collapsing group, that contains two sequences, appeared
    GTUtilsMsaEditor::toggleCollapsingMode();

    //    4. Mouse press on the group
    //    Expected state:  group selected
    GTUtilsMsaEditor::clickSequenceName("Mecopoda_elongata__Ishigaki__J");

    //    5. Mouse press under the group in the name list
    //    Expected state:  nothing happens
    GTMouseDriver::moveTo(GTUtilsMsaEditor::getSequenceNameRect("Mecopoda_elongata__Ishigaki__J").center() + QPoint(0, 20));
    GTMouseDriver::click();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2374) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Create scheme read alignment->write alignment
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Alignment");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Alignment");
    GTUtilsWorkflowDesigner::connect(read, write);
    //    3. Set COI.aln as input, run scheme
    GTUtilsWorkflowDesigner::click(read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: there is no "External Tools" page on the WD dashboards
    CHECK_SET_ERR(!GTUtilsDashboard::hasTab(GTUtilsDashboard::ExternalTools), "External tools tab exists, but is not expected!");
}

GUI_TEST_CLASS_DEFINITION(test_2375) {
    //    1. Open {_common_data/sam/broken_invalid_cigar.sam}
    //    Expected state: import dialog appears.

    //    2. Fill the import dialog with valid data. Begin the importing.
    //    Expected state: importing fails, UGENE doesn't crash.
    QString destUrl = testDir + "_common_data/scenarios/sandbox/test_2375.ugenedb";
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(destUrl));
    GTFileDialog::openFile(testDir + "_common_data/sam/", "broken_invalid_cigar.sam");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2377) {
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    const QString assemblyReaderName = "Read NGS Reads Assembly";
    const QString assemblyWriterName = "Write NGS Reads Assembly";

    GTUtilsWorkflowDesigner::addAlgorithm(assemblyReaderName);
    GTUtilsWorkflowDesigner::addAlgorithm(assemblyWriterName);

    WorkflowProcessItem* assemblyReader = GTUtilsWorkflowDesigner::getWorker(
        assemblyReaderName);
    WorkflowProcessItem* assemblyWriter = GTUtilsWorkflowDesigner::getWorker(
        assemblyWriterName);

    GTUtilsWorkflowDesigner::connect(assemblyReader, assemblyWriter);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(assemblyReaderName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/sam/broken_without_reads.sam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(assemblyWriterName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", "test", GTUtilsWorkflowDesigner::textValue);

    GTWidget::click(GTAction::button("Run workflow"));

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2378) {
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(testDir + "_common_data/scenarios/sandbox/test_2378.ugenedb"));
    GTFileDialog::openFile(testDir + "_common_data/sam/", "scerevisiae.sam");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2378_1) {
    GTLogTracer lt;
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Create scheme: read assembly->write assembly
    // 3. set _common_data\sam\scerevisiae.sam as input file
    GTUtilsWorkflowDesigner::addAlgorithm("Read NGS Reads Assembly");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read NGS Reads Assembly"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/sam/scerevisiae.sam");

    GTUtilsWorkflowDesigner::addAlgorithm("Write NGS Reads Assembly");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write NGS Reads Assembly"));
    GTMouseDriver::click();
    QString path = QFileInfo(testDir + "_common_data/scenarios/sandbox/").absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter("Output file", path + "/test_2378_1.bam", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read NGS Reads Assembly"), GTUtilsWorkflowDesigner::getWorker("Write NGS Reads Assembly"));

    // 4. Run scheme
    GTWidget::click(GTAction::button("Run workflow"));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2379) {
    class CreateProjectFiller : public Filler {
        // It is a local support class, it is the same as SaveProjectAsDialogFiller,
        // but it clicks the final button with keyboard.
        // I know that it is bad practice creating so useless classes, but I don't need to extend the original class.
        // Do not move it to another place: if you need the same filler than extend the original class.
    public:
        CreateProjectFiller(
            const QString& _projectName,
            const QString& _projectFolder,
            const QString& _projectFile)
            : Filler("CreateNewProjectDialog"),
              projectName(_projectName),
              projectFolder(_projectFolder),
              projectFile(_projectFile) {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto projectNameEdit = GTWidget::findLineEdit("projectNameEdit", dialog);
            GTLineEdit::setText(projectNameEdit, projectName);

            auto projectFileEdit = GTWidget::findLineEdit("projectFilePathEdit", dialog);
            GTLineEdit::setText(projectFileEdit, projectFolder + "/" + projectFile);

            if (isOsMac()) {
                GTWidget::click(GTWidget::findButtonByText("Create", dialog));
            } else {
                GTKeyboardDriver::keyClick(Qt::Key_Enter);
            }
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

    GTUtilsDialog::waitForDialog(new CreateProjectFiller(projectName, projectFolder, projectFile));
    GTMenu::clickMainMenuItem({"File", "New project..."});
    GTMenu::clickMainMenuItem({"File", "Save all"});
    GTMenu::clickMainMenuItem({"File", "Close project"});

    // 1. Press "Create new project" button
    // 2. Specify the path to an existing project
    // 3. Press "Create" button by using keyboard
    //    Expected state: only one dialog with warning message appeared
    GTUtilsDialog::add(new CreateProjectFiller(projectName, projectFolder, projectFile));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes, "Project file already exists"));

    GTMenu::clickMainMenuItem({"File", "New project..."});
}

GUI_TEST_CLASS_DEFINITION(test_2382) {
    // 1. Open "_common_data/ace/capres4.ace".
    // 2. "Import ACE file" dialog appeared. Press ok.
    // Expected state: export succeeded, assembly is displayed.
    GTLogTracer lt;

    QString sandboxDir = testDir + "_common_data/scenarios/sandbox/";
    QString assDocName = "test_2382.ugenedb";
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandboxDir + assDocName));
    GTUtilsProject::openFile(testDir + "_common_data/ace/capres4.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());

    GTUtilsProjectTreeView::findIndex(assDocName);
}

GUI_TEST_CLASS_DEFINITION(test_2382_1) {
    // 1. Open "_common_data/ace/test_new_.cap.ace".
    // 2. "Import ACE file" dialog appeared. Press ok.
    // Expected state: export succeeded, assembly is displayed.
    GTLogTracer lt;

    QString sandboxDir = testDir + "_common_data/scenarios/sandbox/";
    QString assDocName = "test_2382_1.ugenedb";
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandboxDir + assDocName));
    GTUtilsProject::openFile(testDir + "_common_data/ace/test_new.cap.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());

    GTUtilsProjectTreeView::findIndex(assDocName);
}

GUI_TEST_CLASS_DEFINITION(test_2387) {
    // 1) Click Open button in UGENE
    // 2) Select any two valid files, for example data/samples/Genbank/ PBR322.gb and sars.gb
    // 3) Click Open
    // Expected state: MultipleDocumentsReadingModeDialog appears
    // 4) Select merge option
    // 5) Replace one of the files with any binary file
    // 6) Click OK in MultipleDocumentsReadingModeDialog.
    // Expected state: UGENE not crashed

    QString sandbox = testDir + "_common_data/scenarios/sandbox/";

    GTFile::copy(dataDir + "samples/Genbank/PBR322.gb", sandbox + "PBR322.gb");
    GTFile::copy(dataDir + "samples/Genbank/sars.gb", sandbox + "sars.gb");

    class SequenceReadingModeDialogUtils : public CustomScenario {
    public:
        // SequenceReadingModeDialogUtils() : GTSequenceReadingModeDialogUtils(_os){}
        virtual void run() {
            GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
            GTFile::copy(testDir + "_common_data/scenarios/_regression/2387/binary.dll", testDir + "_common_data/scenarios/sandbox/sars.gb");
            GTSequenceReadingModeDialogUtils().commonScenario();
        }
    };

    // GTSequenceReadingModeDialogUtils "PBR322.gb" "sars.gb"
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils(new SequenceReadingModeDialogUtils()));
    GTFileDialog::openFileList(sandbox, {"PBR322.gb", "sars.gb"});
}

GUI_TEST_CLASS_DEFINITION(test_2392) {
    // 1. Open file _common_data/genbank/multi.gb
    // Expected state: Open dialog "Sequence reading options"
    // 2. Select "Join sequences into alignment" option and press OK
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/genbank/multi.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Document opened in MSA view
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(2 == names.size(), "Wrong sequence count");
}

GUI_TEST_CLASS_DEFINITION(test_2400) {
    //    1. Import samples/ACE/k26.ace to  ugenedb (via open file)
    QString fileName = "2400.ugenedb";
    QString ugenedb = sandBoxDir + fileName;
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, ugenedb));
    GTUtilsProject::openFile(testDir + "_common_data/ace/ace_test_1.ace");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: assembly view for Contig_1 opened with refrence sequence added to it
    bool ref = GTUtilsAssemblyBrowser::hasReference("1 [2400.ugenedb]");
    CHECK_SET_ERR(ref, "no reference")
}

GUI_TEST_CLASS_DEFINITION(test_2401) {
    // 1. Open the file "_common_data/ace/ace_test_1.ace".
    // 2. Set the ugenedb path for import: "_common_data/scenarios/sandbox/2401.ugenedb".
    // 3. Click OK
    QString sandbox = testDir + "_common_data/scenarios/sandbox/";
    QString fileName = "2401.ugenedb";
    QString ugenedb = sandbox + fileName;
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, ugenedb));
    GTUtilsProject::openFile(testDir + "_common_data/ace/ace_test_1.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the file is imported without errors, the assembly is opened.
    // 4. Close the project.
    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    // 5. Open the file "_common_data/ace/ace_test_11_(error).ace".
    // 6. Set the same ugenedb path for import: "_common_data/scenarios/sandbox/2401.ugenedb".
    // 7. Click OK.
    // 8. Click Append.
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, ugenedb));
    GTUtilsProject::openFile(testDir + "_common_data/ace/ace_test_11_(error).ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the file is not imported but "2401.ugenedb" still exists.
    QString s = GUrl(ugenedb).getURLString();
    CHECK_SET_ERR(QFile::exists(s), "ugenedb file does not exist");

    // 9. Open the file "2401.ugenedb".
    GTFileDialog::openFile(sandbox, fileName);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected: assembly is opened without errors.
    GTUtilsProjectTreeView::findIndex(fileName);
}

GUI_TEST_CLASS_DEFINITION(test_2402) {
    QDir outputDir(testDir + "_common_data/scenarios/sandbox");
    const QString outputFilePath = outputDir.absolutePath() + "/test_2402.gb";

    // 1.Open Workflow Designer
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2.Add 'Read Sequence' element
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    // 3. Add 'Assembly Sequences witn CAP3' element
    GTUtilsWorkflowDesigner::addAlgorithm("Assembly Sequences with CAP3");
    WorkflowProcessItem* readWorker = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    WorkflowProcessItem* assemblyWorker = GTUtilsWorkflowDesigner::getWorker("Assembly Sequences with CAP3");
    // 4. Connect elements
    GTUtilsWorkflowDesigner::connect(readWorker, assemblyWorker);

    // Expected state: all slots are connected
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Assembly Sequences with CAP3"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Well done!"));
    GTWidget::click(GTAction::button("Validate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_2403) {
    // 1. Open "human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Resize the main UGENE window that not all buttons will be visible on the sequence toolbar.
    QMainWindow* mw = AppContext::getMainWindow()->getQMainWindow();
    GTWidget::showNormal(mw);
    GTWidget::resizeWidget(mw, QSize(300, mw->size().height()));

    // 3. Click on the "Show full toolbar" button.
    auto toolbarWidget = GTWidget::findWidget("mwtoolbar_activemdi");
    auto expandWidget = GTWidget::findWidget("qt_toolbar_ext_button", toolbarWidget);

    GTWidget::click(expandWidget);

    // Expected: all toolbar actions appears.
    GTWidget::findWidget("toggleViewButton", toolbarWidget);

    GTWidget::click(expandWidget);
}

GUI_TEST_CLASS_DEFINITION(test_2404) {
    /*  1. Open human_T1.fa
    2. Open Search in sequence OP tab
    3. Input "AAAAA" pattern to the Search for: field
    4. Expand all available parameters
    Expected: scrollbar appears
    Current: layout breaks
*/
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionsPanel::runFindPatternWithHotKey("AAAAA");
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Other settings"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Save annotation(s) to"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Annotation parameters"));
    auto sa = GTWidget::findScrollArea("OP_SCROLL_AREA");
    QScrollBar* scroll = sa->verticalScrollBar();
    CHECK_SET_ERR(scroll != nullptr, "Scroll bar is NULL");
    CHECK_SET_ERR(scroll->isVisible(), "Scroll bar is invisible!");
}

GUI_TEST_CLASS_DEFINITION(test_2406) {
    //    1. Create the {Read Sequence -> Write Sequence} workflow.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    const QString sequenceReaderName = "Read Sequence";
    const QString sequenceWriterName = "Write Sequence";

    GTUtilsWorkflowDesigner::addAlgorithm(sequenceReaderName, true);
    GTUtilsWorkflowDesigner::addAlgorithm(sequenceWriterName, true);

    WorkflowProcessItem* sequenceReader = GTUtilsWorkflowDesigner::getWorker(sequenceReaderName);
    WorkflowProcessItem* sequenceWriter = GTUtilsWorkflowDesigner::getWorker(sequenceWriterName);

    CHECK_SET_ERR(nullptr != sequenceReader, "Sequence reader element is NULL");
    CHECK_SET_ERR(nullptr != sequenceWriter, "Sequence writer element is NULL");

    GTUtilsWorkflowDesigner::connect(sequenceReader, sequenceWriter);

    //    2. Click on the output file field and input "TEST" file name and press Enter.
    //    Expected: TEST file name appears in the output file name field
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter(sequenceWriterName));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", "TEST", GTUtilsWorkflowDesigner::textValue);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    const QString expectedPreValue = "TEST";
    const QString resultPreValue = GTUtilsWorkflowDesigner::getParameter("Output file", true);
    CHECK_SET_ERR(expectedPreValue == resultPreValue,
                  QString("Unexpected value: expected '%1' get '%2'").arg(expectedPreValue).arg(resultPreValue));

    //    3. Change the file format to the genbank
    //    Expected: TEST.gb file name appears in the output file name field
    GTUtilsWorkflowDesigner::setParameter("Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);

    const QString expectedPostValue = "TEST.gb";
    const QString resultPostValue = GTUtilsWorkflowDesigner::getParameter("Output file", true);
    CHECK_SET_ERR(expectedPostValue == resultPostValue,
                  QString("Unexpected value: expected '%1' get '%2'").arg(expectedPostValue).arg(resultPostValue));
}

GUI_TEST_CLASS_DEFINITION(test_2407) {
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "10000_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("10000_sequences.aln"));

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__UNLOAD_SELECTED}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("10000_sequences.aln"));
    GTMouseDriver::doubleClick();
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2410) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(166740, 166755));

    auto sequenceWidget = GTWidget::findWidget("ADV_single_sequence_widget_0");

    GTWidget::click(sequenceWidget);
    GTKeyboardUtils::selectAll();

    auto graphAction = GTWidget::findWidget("GraphMenuAction", sequenceWidget, false);
    Runnable* chooser = new PopupChooser({"GC Content (%)"});
    GTUtilsDialog::waitForDialog(chooser);

    GTWidget::click(graphAction);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTAction::button("action_zoom_in_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    auto renderArea = GTWidget::findWidget("GSequenceGraphViewRenderArea", sequenceWidget);
    const QPoint mouseInitialPos(4 * renderArea->width() / 7, renderArea->height() / 2);
    GTWidget::click(renderArea, Qt::LeftButton, mouseInitialPos);

    const QPoint mouseInitialAbsPos = GTMouseDriver::getMousePosition();
    const int rightMouseLimit = mouseInitialAbsPos.x() * 1.3;

    for (int x = mouseInitialAbsPos.x(); x < rightMouseLimit; x += 5) {
        const QPoint currentPos(x, mouseInitialAbsPos.y());
        GTMouseDriver::moveTo(currentPos);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2415) {
    // 1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Right click on the object sequence name in the project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1 (UCSC April 2002 chr7:115977709-117855134)"));

    // 3. Click the menu {Edit -> Rename}.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Rename"}));
    GTMouseDriver::click(Qt::RightButton);

    // 4. Enter the new name: "name".
    GTKeyboardDriver::keySequence("name");

    // 5. Press Enter.
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    // Expected state: the sequence is renamed.
    GTUtilsProjectTreeView::findIndex("name");
}

GUI_TEST_CLASS_DEFINITION(test_2424) {
    //     1. Open WD. Add Read Sequence, Write sequence and Filter quality (custm element with script section) workers. Make sure Fitler Quality worker's script text is empty.
    QString workflowOutputDirPath(testDir + "_common_data/scenarios/sandbox");
    QDir workflowOutputDir(workflowOutputDirPath);
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Quality Filter Example");

    WorkflowProcessItem* seqReader = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    WorkflowProcessItem* seqWriter = GTUtilsWorkflowDesigner::getWorker("Write Sequence");
    WorkflowProcessItem* qualFilter = GTUtilsWorkflowDesigner::getWorker("Quality Filter Example");

    GTUtilsWorkflowDesigner::connect(seqReader, qualFilter);
    GTUtilsWorkflowDesigner::connect(qualFilter, seqWriter);
    //     2. Connect workers into schema, set input data and output data (for example eas.fastq from samples)
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    const QString outputFilePath = workflowOutputDir.absolutePath() + "/test.gb";

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);
    //     3. Launch the schema.
    //     Expected state: schema didnt start, error "Quality filter example"
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Run workflow"));
    GTUtilsWorkflowDesigner::checkErrorList("Quality Filter Example: Empty script text");
}

GUI_TEST_CLASS_DEFINITION(test_2430) {
    //    1. Check the debug mode checkbox in the applications settings
    //    2. Add an element to the scene
    //    3. Open breakpoints manager
    //    4. Try to add the breakpoint to the element by shortcut (Ctrl+B)
    //    Current: nothing happens
    //    Expected: the breakpoint appears

    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::toggleBreakpointManager();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick('b', Qt::ControlModifier);

    QStringList breakpoints = GTUtilsWorkflowDesigner::getBreakpointList();
    CHECK_SET_ERR(breakpoints.size() == 1, "Wrong count of breakpoints!");
}

GUI_TEST_CLASS_DEFINITION(test_2431) {
    // 1. Check the debug mode checkbox in the applications settings
    // 2. Add an element to the scene
    // 3. Try to delete the element by delete button
    // Current: nothing happens
    // Expected: the element will be deleted

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::toggleDebugMode();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    WorkflowProcessItem* readSeq = GTUtilsWorkflowDesigner::getWorker("Read Sequence", {false});
    CHECK_SET_ERR(readSeq == nullptr, "Item didn't deleted");
}

GUI_TEST_CLASS_DEFINITION(test_2432) {
    //    1. Check the debug mode checkbox in the applications settings
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::toggleBreakpointManager();

    //    2. Add an element to the scene
    GTUtilsWorkflowDesigner::addAlgorithm("Read File URL(s)");

    //    3. Add the breakpoint to the element
    GTUtilsWorkflowDesigner::setBreakpoint("Read File URL(s)");

    //    4. Delete the element
    //    Expected: there are no breakpoints in the breakpoints manager
    GTUtilsWorkflowDesigner::removeItem("Read File URL(s)");
    const QStringList breakpoints = GTUtilsWorkflowDesigner::getBreakpointList();
    CHECK_SET_ERR(breakpoints.isEmpty(), "There are breakpoints in the workflow");
}

GUI_TEST_CLASS_DEFINITION(test_2449) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Create a phylogenetic tree for the alignment.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_2449.nwk", 0, 0, true));
    GTMenu::clickMainMenuItem({"Actions", "Tree", "Build Tree"});
    GTUtilsTaskTreeView::waitTaskFinished();

    // There is a font size spinbox. You can set zero value to it: in this case font has its standard size (on Mac), but this value is incorrect.
    auto sizeSpinBox = GTWidget::findSpinBox("fontSizeSpinBox");

    GTWidget::setFocus(sizeSpinBox);
    int prev = 0;
    while (0 < sizeSpinBox->value()) {
        prev = sizeSpinBox->value();
        GTKeyboardDriver::keyClick(Qt::Key_Down);
        if (prev <= sizeSpinBox->value()) {
            break;
        }
    }

    CHECK_SET_ERR(0 < sizeSpinBox->value(), "Invalid size spin box bound");
}

GUI_TEST_CLASS_DEFINITION(test_2451) {
    //    1. Open Workflow designer
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open sample {Alignment->Align sequences with MUSCLE}
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    //    Expected state: There is "Show wizard" tool button

    //    3. Press "Show wizard" button

    class customWizard : public CustomScenario {
    public:
        void run() {
            //    4. Select input MSA "samples\CLUSTALW\COI.aln"
            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << dataDir + "samples/CLUSTALW/COI.aln"));
            //    5. Press "Next" button
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            //    6. Press "Run" button
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Align Sequences with MUSCLE Wizard", new customWizard()));
    GTWidget::click(GTAction::button("Show wizard"));
    //    Expected state: Align sequences with MUSCLE Wizard appeared

    //    Expected state: Scheme successfully performed
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2459) {
    //    1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Set any reference sequence.
    GTUtilsDialog::add(new PopupChooserByText({"Set this sequence as reference"}));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton, QPoint(10, 10));

    //    3. Open context menu, open the "Highlighting" submenu, set the "Agreements" type.
    GTUtilsDialog::add(new PopupChooserByText({"Appearance", "Highlighting", "Agreements"}));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);

    //    4. Open context menu again, open the "Export" submenu, choose the "Export highlighted" menu item.
    //    Expected state: the "Export highlighted to file" dialog appears.

    //    5. Set any valid filename, other settings should be default. Click "Export".
    //    Expected state: exporting successfully completes, UGENE doesn't crash.
    //    Current state: UGENE crashes.
    GTUtilsDialog::add(new PopupChooserByText({"Export", "Export highlighted"}));
    GTUtilsDialog::add(new ExportHighlightedDialogFiller(sandBoxDir + "test_2459.txt"));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2460) {
    // 1. Open "COI.aln".

    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Remove all sequences except the first one.
    QStringList list = GTUtilsMSAEditorSequenceArea::getNameList();
    GTUtilsMsaEditor::removeRows(1, list.size() - 1);

    // 3. Align the result one-line-msa by kalign with default values.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTUtilsDialog::add(new KalignDialogFiller());
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Kalign task finishes with error. Redo button is disabled.
    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");

    QAbstractButton* redo = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(redo != nullptr, "There is no REDO button");
    CHECK_SET_ERR(redo->isEnabled() == false, "REDO button is enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2470) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/2470", "blast_result.gb");
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

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        };

    private:
        const QString dbPath;
        const QString outputPath;
    };

    GTUtilsDialog::waitForDialog(
        new OkClicker(
            testDir + "_common_data/scenarios/_regression/2470/nice_base.nhr",
            testDir + "_common_data/scenarios/sandbox/2470_fetched.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"fetchMenu", "fetchSequenceById"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::getItemCenter("Conocephalus_sp.");
    GTUtilsProjectTreeView::getItemCenter("Montana_montana");
    GTUtilsProjectTreeView::getItemCenter("Gampsocleis_sedakovii_EF540828");
}

GUI_TEST_CLASS_DEFINITION(test_2475) {
    // 1. Open WD.
    // 2. Open Single-sample (NGS samples).
    // 3. Set proper input data.
    // 4. Validate scheme.
    // Expected state: validation passed.

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new DefaultWizardFiller("Tuxedo Wizard"));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read RNA-seq Short Reads"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bowtie2/reads_1.fq");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bowtie2/reads_2.fq");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Map RNA-Seq Reads with TopHat"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Bowtie index folder", testDir + "_common_data/bowtie2/index", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Bowtie index basename", "human_T1_cutted", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Validate workflow"));
}

GUI_TEST_CLASS_DEFINITION(test_2482) {
    // 1. Open "COI.nwk".
    GTFileDialog::openFile(dataDir + "samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Change the tree layout to unrooted.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Unrooted"}));
    GTWidget::click(GTWidget::findWidget("Layout"));

    // 3. Select any node in the tree that is not a leaf.
    QList<TvNodeItem*> items = GTUtilsPhyTree::getNodes();
    CHECK_SET_ERR(items.size() >= 4, "Incorrect tree size");

    QPoint nodeCoords = GTUtilsPhyTree::getGlobalCenterCoord(items.at(3));
    GTMouseDriver::moveTo(nodeCoords);

    GTMouseDriver::click();

    // 4. Call context menu on the Tree Viewer.
    // Expected state: menu items "Swap Sublings" and "Reroot tree" are disabled.
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Reroot tree"}, PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Swap Siblings"}, PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2487) {
    // 1. Open "COI.nwk".
    // 2. Select root-node.
    // 3. Call context menu.
    // 'Reroot tree' and 'Collapse ' options must be disabled

    GTFileDialog::openFile(dataDir + "samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<TvNodeItem*> items = GTUtilsPhyTree::getNodes();
    CHECK_SET_ERR(!items.empty(), "Tree is empty");

    QPoint rootCoords = GTUtilsPhyTree::getGlobalCenterCoord(items.first());
    GTMouseDriver::moveTo(rootCoords);
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Reroot tree"}, PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Collapse"}, PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2496) {
    // 1. Open file testDir/_common_data/scenarios/assembly/example-alignment.bam
    // Expected state: import bam dialog appeared

    GTUtilsDialog::waitForDialog(
        new ImportBAMFileFiller(testDir + "_common_data/scenarios/sandbox/example-alignment.bam.ugenedb"));
    GTFileDialog::openFile(testDir + "_common_data/scenarios/assembly/", "example-alignment.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2498) {
    // 1. Open the /test/_common_data/fasta/empty.fa empty msa file.
    // 2. Open context menu on the sequence area. Go to the {Export -> amino translation}
    //
    GTFileDialog::openFile(testDir + "_common_data/fasta/", "empty.fa");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));

    // Default alphabet for an empty alignment is nucleic -> Amino is enabled.
    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_EXPORT, "exportNucleicMsaToAminoAction"}, PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2506) {
    // 1. Open file _common_data/vcf_consensus/vcf_cons_out_damaged_1.vcf
    // Expected state: error message appears.
    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/vcf_consensus/", "vcf_cons_out_damaged_1.vcf");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2506_1) {
    // 1. Open file _common_data/vcf_consensus/vcf_cons_out_damaged_2.vcf
    // Expected state: error message appears.
    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/vcf_consensus/", "vcf_cons_out_damaged_2.vcf");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2513) {
    // Check Swap sibling & re-root for inner & leaf nodes in circular tree.
    GTFileDialog::openFile(dataDir + "/samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Switch to the circular layout on the tree view.
    GTWidget::click(GTWidget::findWidget("OP_TREES_WIDGET"));
    auto layoutCombo = GTWidget::findComboBox("layoutCombo");
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<TvNodeItem*> nodes = GTUtilsPhyTree::getNodes();
    CHECK_SET_ERR(!nodes.isEmpty(), "Nodes list is empty");

    TvNodeItem* innerNode = GTUtilsPhyTree::getNodeByBranchText("0.016", "0.017");
    TvNodeItem* tipNode = innerNode->getLeftBranchItem()->getNodeItem();

    GTUtilsPhyTree::clickNode(tipNode);
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes().isEmpty(), "A clicked node wasn't selected");
    GTUtilsDialog::waitForDialog(new PopupChecker({"Swap Siblings"}, PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitForDialog(new PopupChecker({"Reroot tree"}, PopupChecker::IsDisabled));  // We do not support re-root for leaves.
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsPhyTree::clickNode(innerNode);
    CHECK_SET_ERR(!GTUtilsPhyTree::getSelectedNodes().isEmpty(), "A clicked node wasn't selected");
    GTUtilsDialog::waitForDialog(new PopupChecker({"Swap Siblings"}, PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitForDialog(new PopupChecker({"Reroot tree"}, PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2519) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Go to position 20000.
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(20000);

    // 3. Call context menu, select menu item {Edit sequence -> Remove subsequence...}.
    // Expected state: a "Remove subsequence" dialog appears.
    // 4. Set the region (1..190950), other settings should be default, click the "Remove" button.
    // Expected state: UGENE doesn't crash.
    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REMOVE_SUBSEQUENCE}));
    GTUtilsDialog::add(new RemovePartFromSequenceDialogFiller("1..190950"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // 5. Call this dialog again, remove region (1..8999).
    // Expected state: UGENE doesn't crash.
    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REMOVE_SUBSEQUENCE}));
    GTUtilsDialog::add(new RemovePartFromSequenceDialogFiller("1..8999"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_2538) {
    //    1. Open file "_common_data/scenarios/tree_view/COI.nwk"
    GTFileDialog::openFile(dataDir + "/samples/Newick/", "COI.nwk");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on node just near root. Click "Collapse"
    GTMouseDriver::moveTo(GTUtilsPhyTree::getGlobalCenterCoord(GTUtilsPhyTree::getNodes().at(1)));
    GTMouseDriver::click();

    auto treeView = GTWidget::findWidget("treeView");

    const QImage initImg = GTWidget::getImage(treeView);

    GTMouseDriver::doubleClick();

    //    3. Call context menu on node just near root. Click "Expand"
    GTMouseDriver::doubleClick();

    //    Expected state: tree has the same view as at the beginning
    const QImage finalImg = GTWidget::getImage(treeView);

    // images have several pixels differ. so sizes are compared
    CHECK_SET_ERR(initImg.size() == finalImg.size(), "different images");
}

GUI_TEST_CLASS_DEFINITION(test_2540) {
    //    1. Forbid to write to the dir with the source bam(sam) file.
    QDir().mkpath(sandBoxDir + "test_2540");
    GTFile::copy(testDir + "_common_data/bam/chrM.sorted.bam", sandBoxDir + "test_2540/chrM.sorted.bam");
    GTFile::setReadOnly(sandBoxDir + "test_2540");
    //    2. Try to open this file with UGENE.
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(sandBoxDir + "test_2540/chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: an import dialog appeared.
    //    3. Set the destination location with enough permissions.
    //    4. Click "Import" button.
    //    Expected state: file is successfully imported.
}

GUI_TEST_CLASS_DEFINITION(test_2542) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Lock the document
    GTUtilsDialog::waitForDialog(new PopupChooser({"action_document_lock"}));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::click(Qt::RightButton);

    // 3. Do pairwise alignment with any sequences.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(1, 1));
    GTWidget::click(GTWidget::findWidget("OP_PAIRALIGN"));
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Output settings"));

    auto check = GTWidget::findCheckBox("inNewWindowCheckBox");
    GTCheckBox::setChecked(check, false);

    // State:
    // Align button not active if file locked for writing (user locking or format didn't support writing) and selected aligning in current file option
    auto button = GTWidget::findWidget("alignButton");

    CHECK_SET_ERR(!button->isEnabled(), "Align button is enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2544) {
    //    1. Open "data/samples/FASTA/human_T1.fa"
    GTFile::copy(dataDir + "samples/FASTA/human_T1.fa", sandBoxDir + "test_2544.fa");
    GTFileDialog::openFile(sandBoxDir + "test_2544.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    2. Use context menu { Edit sequence -> Remove subsequence... }
    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EDIT, "action_edit_remove_sub_sequences"}));
    GTUtilsDialog::add(new RemovePartFromSequenceDialogFiller("10..20"));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());

    //    Expected state: "Remove subsequence" dialog has appeared
    //    3. Set string "10..20" to the "Region to remove" field, press the "Remove" button
    //    Expected state: the dialog has disappeared, subsequence has been removed

    //    4. Change permissions to the file to read-only
    GTFile::setReadOnly(sandBoxDir + "test_2544.fa");
    //    5. Use context menu on the document item in project view { Save selected documents }

    //    Expected state: message box has appeared
    //    6. Press "Save" button
    //    Expected state: "Save as" dialog has appeared
    //    7. Choose the same file, press "Save"
    //    Expected state: message box has appeared
    //    8. Press "Save"

    class InnerMessageBoxFiller : public MessageBoxDialogFiller {
    public:
        InnerMessageBoxFiller()
            : MessageBoxDialogFiller(QMessageBox::Yes) {
        }
        void run() {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Cancel, "", "permissionBox"));
            MessageBoxDialogFiller::run();
        }
    };
    class CustomSaver : public GTFileDialogUtils {
    public:
        CustomSaver()
            : GTFileDialogUtils(sandBoxDir, "test_2544.fa", GTFileDialogUtils::Save) {
        }
        void commonScenario() {
            fileDialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new InnerMessageBoxFiller());
            setName();
            clickButton(button);
        }
    };
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__SAVE_DOCUMENT}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Save, "", "permissionBox"));
    GTUtilsDialog::add(new CustomSaver());
    GTUtilsProjectTreeView::click("test_2544.fa", Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_2545) {
    // 1. Remove the "spidey" external tool, if it exists.
    GTUtilsExternalTools::removeTool("Spidey");

    // 2. Open "human_t1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Call context menu, select the {Align -> Align sequence to mRna} menu item.
    // Expected state: UGENE offers to select the "spidey" external tool.
    // 4. Refuse the offering.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ALIGN", "Align sequence to mRNA"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Expected state: align task is cancelled.
    CHECK_SET_ERR(0 == GTUtilsTaskTreeView::getTopLevelTasksCount(), "The task is not canceled");
}

GUI_TEST_CLASS_DEFINITION(test_2549) {
    // this needed to ensure that log view has text
    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/gtf/invalid", "AB375112_annotations.gtf"));
    GTUtilsDialog::add(new DocumentFormatSelectorDialogFiller("GTF"));
    GTMenu::clickMainMenuItem({"File", "Open as..."});

    GTKeyboardDriver::keyClick('3', Qt::AltModifier);
    GTWidget::click(GTWidget::findWidget("dock_log_view"));
    GTKeyboardDriver::keySequence("this sequence needed to ensure that log view has text");

    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    const QString clipboardContent = GTClipboard::text();
    CHECK_SET_ERR(!clipboardContent.isEmpty(), "Clipboard is empty");
}

GUI_TEST_CLASS_DEFINITION(test_2565) {
    //    1. Open "samples/Genbank/murine.gb".
    //    2. Press Ctrl+F.
    //    3. Insert the pattern "GCTAGCTTAAGTAACGCCACTTTT".
    //    4. Click "Search".
    //    Expected: the pattern is not found. Notification with this information appears.
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionsPanel::runFindPatternWithHotKey("GCTAGCTTAAGTAACGCCACTTTT");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: -/0"), "Results string does not match");
}

GUI_TEST_CLASS_DEFINITION(test_2566) {
    // 1. Open "samples/Genbank/murine.gb".
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Enter the pattern: GCTAGCTTAAGTAACGCCAC
    QWidget* patternInputLine = QApplication::focusWidget();
    CHECK_SET_ERR(patternInputLine != nullptr && patternInputLine->objectName() == "textPattern", "Focus is not on FindPattern widget");

    GTKeyboardDriver::keySequence("GCTAGCTTAAGTAACGCCAC");

    // 4. Choose the algorithm: Substitute.
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");

    // 5. Enter the "Should match" value: 97%
    GTUtilsOptionPanelSequenceView::setMatchPercentage(97);

    // 6. Click "Search".
    //  There is no "Search buttons since UGENE-3483 was done, the task is already launched
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: 97% is entered.
    int matchPercentage = GTUtilsOptionPanelSequenceView::getMatchPercentage();
    CHECK_SET_ERR(matchPercentage == 97, "Entered and actual values don't match");

    // Expected state: the task finished successfully.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2567) {
    // 1. Open "samples/Genbank/murine.gb".
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Enter the pattern: GCTAGCTTAAGTAACGCCAC
    QWidget* patternInputLine = QApplication::focusWidget();
    CHECK_SET_ERR(nullptr != patternInputLine && patternInputLine->objectName() == "textPattern", "Focus is not on FindPattern widget");

    GTKeyboardDriver::keySequence("GCTAGCTTAAGTAACGCCAC");

    // 4. Choose the algorithm: Substitute.
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");

    // 5. Enter the "Should match" value: 30%
    GTUtilsOptionPanelSequenceView::setMatchPercentage(30);

    // 6. Click "Search".
    //  There is no "Search buttons since UGENE-3483 was done, the task is already launched
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: the task finished successfully.
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2568) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add the "Call Variants" sample
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Run the wizard

    class customFileDialog : public CustomScenario {
    public:
        void run() {
            QWidget* d = GTWidget::getActiveModalWidget();
            auto dialog = qobject_cast<QFileDialog*>(d);
            CHECK_SET_ERR(dialog, "activeModalWidget is not file dialog");

            QString name = dialog->directory().dirName();
            CHECK_SET_ERR(name == "Assembly", "unexpectyed dir name: " + name);
            GTWidget::click(GTWidget::findButtonByText("Cancel", dialog));
        }
    };

    class customWizard : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    4. Click to browse a reference file and choose a reference
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Assembly/chrM.fa"));
            auto browse = GTWidget::findWidget("browseButton", GTWidget::findWidget("Reference sequence file labeledWidget", dialog));
            GTWidget::click(browse);
            //    Expected: the file is chosen. (The file's folder is DIR)

            //    5. Click to browse a BAM/SAM file

            //    Expected: the open file dialog is opened with the folder DIR
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(new customFileDialog()));
            GTWidget::click(GTWidget::findWidget("addFileButton"));

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Call Variants Wizard", new customWizard()));
    GTWidget::click(GTAction::button("Show wizard"));
}

GUI_TEST_CLASS_DEFINITION(test_2569) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add the Call Variants sample.
    GTUtilsWorkflowDesigner::addSample("Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/chrM.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cmdline/call-variations/chrM.fa");
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    4. Click "External Tools" on the appeared Dashboard.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);
    //    5. Expand "SAMtools run"
    auto runNode = GTUtilsDashboard::getExternalToolNodeByText("SAMtools run");
    GTUtilsDashboard::clickNodeTitle(runNode);
    //    6. Check that "Command" node exists.
    GTUtilsDashboard::getExternalToolNodeByText(runNode, "Command");
}

GUI_TEST_CLASS_DEFINITION(test_2577) {
    // 1. Open {data/samples/CLUSTALW/COI.aln}.
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Call context menu somewhere on the alignment, select {Consenssus mode...} menu item.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Consensus mode"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Expected state: options panel "General" tab opens.
    GTWidget::findWidget("OP_MSA_GENERAL");
}

GUI_TEST_CLASS_DEFINITION(test_2578) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open options panel 'Highlighting' tab.
    GTWidget::click(GTWidget::findWidget("OP_MSA_HIGHLIGHTING"));

    //    3. Select 'agreements' highlighting scheme.
    auto combo = GTWidget::findComboBox("highlightingScheme");
    GTComboBox::selectItemByText(combo, "Agreements");

    auto exportButton = GTWidget::findWidget("exportHighlightning");
    CHECK_SET_ERR(!exportButton->isEnabled(), "exportButton is enabled unexpectedly");

    //    4. Select any reference sequence.
    //    Expected state: the "Export" button is active.
    GTWidget::click(GTWidget::findWidget("sequenceLineEdit"));
    GTKeyboardDriver::keySequence("Montana_montana");
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    CHECK_SET_ERR(exportButton->isEnabled(), "exportButton is disabled unexpectedly");

    //    5. Select 'General' tab, then select 'Highlighting' tab again.
    //    Expected state: the "Export" button is active.
    GTWidget::click(GTWidget::findWidget("OP_MSA_GENERAL"));
    GTWidget::click(GTWidget::findWidget("OP_MSA_HIGHLIGHTING"));

    exportButton = GTWidget::findWidget("exportHighlightning");
    CHECK_SET_ERR(nullptr != exportButton, "exportButton not found");
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
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();

            QString path = AppSettingsDialogFiller::getExternalToolPath("MAFFT");
            AppSettingsDialogFiller::clearToolPath("MAFFT");
            AppSettingsDialogFiller::setExternalToolPath("MAFFT", path);
            GTKeyboardDriver::keyClick(Qt::Key_Tab);
            GTUtilsTaskTreeView::waitTaskFinished();

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new MafftInactivation()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}
GUI_TEST_CLASS_DEFINITION(test_2581) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with MUSCLE }
    // Expected state: the "Align with MUSCLE" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash
    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Default));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}));

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2581_1) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with ClustalW }
    // Expected state: the "Align with ClustalW" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash

    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new ClustalWDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with ClustalW"}, GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2581_2) {
    //    1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped_same_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Use context menu { Align -> Align with ClustalO }
    //    Expected state: the "Align with Clustal Omega" dialog has appeared

    //    3. Press the "Align" button
    //    Expected state: after a few seconds alignment has finished, UGENE does not crash
    GTUtilsDialog::add(new PopupChooserByText({"Align", "Align with ClustalO..."}));
    GTUtilsDialog::add(new ClustalOSupportRunDialogFiller());
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2581_3) {
    //    1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "ma2_gapped_same_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Use context menu { Align -> Align with MAFFT }
    //    Expected state: the "Align with MAFFT" dialog has appeared

    //    3. Press the "Align" button
    //    Expected state: after a few seconds alignment has finished, UGENE does not crash
    GTUtilsDialog::add(new PopupChooserByText({"Align", "Align with MAFFT..."}));
    MAFFTSupportRunDialogFiller::Parameters parameters;
    GTUtilsDialog::add(new MAFFTSupportRunDialogFiller(&parameters));
    GTWidget::click(GTUtilsMSAEditorSequenceArea::getSequenceArea(), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2581_4) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with T-Coffee }
    // Expected state: the "Align with T-Coffee" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash

    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new TCoffeeDailogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with T-Coffee"}, GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2581_5) {
    // 1. Open file "_common_data/scenarios/msa/ma2_gapped_same_names.aln"
    // 2. Use context menu { Align -> Align with Kalign }
    // Expected state: the "Align with Kalign" dialog has appeared
    // 3. Press the "Align" button
    // Expected state: after a few seconds alignment has finished, UGENE does not crash

    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped_same_names.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}, GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2583) {
    //    1. Open file data/samples/EMBL/AL000263.emb
    GTFileDialog::openFile(dataDir + "samples/EMBL/AL000263.emb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open file test/_common_data/regression/2583/My_Document_2.gb
    GTFileDialog::openFile(testDir + "_common_data/regression/2583/MyDocument_2.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    3. Drag "AB000263 standart annotations" AL000263.emb sequence view
    QModelIndex idx = GTUtilsProjectTreeView::findIndex("AB000263 standard annotations");
    QWidget* sequence = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(sequence != nullptr, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(idx, sequence);
    //    "Edit objct relations" dialog appeared. Click "OK"
    //    4. Select all "blast result" tree items in annotation tree view
    QList<QTreeWidgetItem*> blastResultItems = GTUtilsAnnotationsTreeView::findItems("blast result");
    GTUtilsAnnotationsTreeView::selectItems(blastResultItems);
    //    5. Use context menu: "Export"->"Export BLAST result to alignment"

    GTUtilsDialog::waitForDialog(new ExportBlastResultDialogFiller(sandBoxDir + "test_2583/test_2583.aln", true));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_EXPORT", "export_BLAST_result_to_alignment"}));
    GTMouseDriver::click(Qt::RightButton);
    //    "Export BLAST result to alignment" dialog appeared. Set some output file.
    //    check "add reference to alignment" checkBox
    //    Click "Export"
    //    Expected state: aligned parts are different with same parts in ref sequence.//Kirill can give more comments
    GTUtilsProjectTreeView::openView();
    GTUtilsProjectTreeView::toggleView();
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(30, 1), QPoint(41, 1), "TGCGGCTGCTCT");
}

GUI_TEST_CLASS_DEFINITION(test_2605) {
    GTLogTracer lt;
    // 1. Open file _common_data/fasta/multy_fa.fa as multiple alignment
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Export subalignment from this msa to any MSA format
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(testDir + "_common_data/scenarios/sandbox/2605.aln", {"SEQUENCE_1"}, 6, 237));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    // Expected state: export is successful, no any messages in log like "There is no sequence objects in given file, unable to convert it in multiple alignment"
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2612) {
    // 1. Open sequence "samples/fasta/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Search for 20 first nucleotides (TTGTCAGATTCACCAAAGTT) using Find Pattern.
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTGTCAGATTCACCAAAGTT");
    GTWidget::click(GTWidget::findWidget("getAnnotationsPushButton"));
    // Expected state: the annotation with pattern created and shown in sequence view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
    // 3. Delete annotation from annotation editor.
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    // Expected state: there is no annotation in sequence view.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("misc_feature", nullptr, {false}) == nullptr, "Annotations document not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_2619) {
    // 1. Open file samples/genbank/sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open context menu for any qualifier on annotation table view.
    // Expected state: submenu "Copy" didn't contain items "Edit qualifier" and "Add 'evidence' column"
    GTUtilsDialog::waitForDialog(new PopupChecker({ADV_MENU_COPY, "edit_qualifier_action"}, PopupChecker::NotExists));
    GTUtilsAnnotationsTreeView::callContextMenuOnQualifier("5'UTR", "evidence");

    GTUtilsDialog::waitForDialog(new PopupChecker({ADV_MENU_COPY, "toggle_column"}, PopupChecker::NotExists));
    GTUtilsAnnotationsTreeView::callContextMenuOnQualifier("5'UTR", "evidence");
}

GUI_TEST_CLASS_DEFINITION(test_2622) {
    // 1. Open "_common_data/fasta/multy_fa.fa".
    // 2. Choose "Merge sequences ..." with 100 bases.
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge, 100));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Press Ctrl + F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 4. Choose "Regular expression" algorithm.
    GTUtilsOptionPanelSequenceView::setAlgorithm("Regular expression");

    // 5. Write "X+" in the pattern string.
    auto textPattern = GTWidget::findWidget("textPattern");
    GTWidget::click(textPattern);
    GTKeyboardDriver::keyClick('X');
    GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);

    // UGENE does not hang.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2622_1) {
    // 1. Open "_common_data/fasta/multy_fa.fa".
    // 2. Choose "Merge sequences ..." with 100 bases.
    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge, 100));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Press Ctrl + F.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 4. Choose "Regular expression" algorithm.
    GTUtilsOptionPanelSequenceView::setAlgorithm("Regular expression");

    // 5. Write "X+" in the pattern string.
    auto textPattern = GTWidget::findWidget("textPattern");
    GTWidget::click(textPattern);
    GTKeyboardDriver::keyClick('X');
    GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);

    // 6. Check "Results no longer than" and set the value 1.
    auto boxUseMaxResultLen = GTWidget::findCheckBox("boxUseMaxResultLen");
    GTCheckBox::setChecked(boxUseMaxResultLen, true);

    auto boxMaxResultLen = GTWidget::findSpinBox("boxMaxResultLen");
    GTSpinBox::setValue(boxMaxResultLen, 1, GTGlobals::UseKeyBoard);

    // UGENE does not hang and all results are 1 bp length (100 results).
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());

    auto resultLabel = GTWidget::findLabel("resultLabel");
    CHECK_SET_ERR(resultLabel->text() == "Results: 1/100", "Unexpected find algorithm results");
}

GUI_TEST_CLASS_DEFINITION(test_2632) {
    //    1. Opened the full tuxedo wizard
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Go to the second page
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            //    3. Set bowtie index and a known transcript file.
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bowtie/index/e_coli.1.ebwt"));
            GTWidget::click(GTWidget::findButtonByText("Select\nbowtie index file", dialog));
            GTUtilsWizard::setParameter("Known transcript file", QVariant(QDir().absoluteFilePath(testDir + "_common_data/gtf/valid.gtf")));

            //    4. Revern to first page, then click next again
            //    UGENE crashes
            GTUtilsWizard::clickButton(GTUtilsWizard::Back);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
}

GUI_TEST_CLASS_DEFINITION(test_2638) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Select "tuxedo" sample
    //    3. Set proper input data.
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_1000.fastq"), map));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");

    GTUtilsWorkflowDesigner::click("Assemble Transcripts with Cufflinks");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Launch the pipeline.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Open "input" tab on the dashboard
    GTUtilsDashboard::openTab(GTUtilsDashboard::Input);
    QWidget* indexButton = GTWidget::findButtonByText("index", GTUtilsDashboard::getDashboard());
    QString type = indexButton->property("file-url").toString().split("\n")[0];
    CHECK_SET_ERR(type == "file", "Unexpected action type, expected file, got: " + type);
}

GUI_TEST_CLASS_DEFINITION(test_2640) {
    //    0. Set CPU optimisation in settings dialog
    class UpdateCPUCountScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::Resources);
            auto cpuBox = GTWidget::findSpinBox("cpuBox", dialog);
            GTSpinBox::setValue(cpuBox, 94, GTGlobals::UseKeyBoard);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new UpdateCPUCountScenario));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Select "tuxedo" sample
    //    3. Set proper input data
    GTLogTracer lt;
    QMap<QString, QVariant> map;
    map.insert("Bowtie index folder", QDir().absoluteFilePath(testDir + "_common_data/bowtie/index"));
    map.insert("Bowtie index basename", "e_coli");
    map.insert("Bowtie version", "Bowtie1");
    GTUtilsDialog::waitForDialog(new WizardFiller("Tuxedo Wizard", QList<QStringList>() << (QStringList() << testDir + "_common_data/e_coli/e_coli_1000.fastq"), map));
    GTUtilsDialog::waitForDialog(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Single-sample", "Single-end"}));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    GTUtilsWorkflowDesigner::click("Assemble Transcripts with Cufflinks");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Launch pipeline
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state: tophat launched with argument -p
    CHECK_SET_ERR(lt.hasMessage("tophat2/tophat -p 94 --output-dir"), "Expected message not found");
}

GUI_TEST_CLASS_DEFINITION(test_2651) {
    // 1. File->Search NCBI GenBank...
    // 2. In the search field paste
    // AB797204.1 AB797210.1 AB797201.1
    // 3. Click Search
    // 4. Select three results and download them
    // 5. Close the dialog
    GTLogTracer lt;

    QList<int> resultNumbersToSelect = {0, 1, 2};
    QVariant variantNumbers = QVariant::fromValue<QList<int>>(resultNumbersToSelect);
    QVariant searchField = QVariant::fromValue<QPair<int, QString>>(QPair<int, QString>(0, "AB797204.1 AB797210.1 AB797201.1"));

    QList<DownloadRemoteFileDialogFiller::Action> remoteDialogActions = {DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::ClickOk, {})};
    QVariant remoteDialogActionsVariant = QVariant::fromValue<QList<DownloadRemoteFileDialogFiller::Action>>(remoteDialogActions);

    QList<NcbiSearchDialogFiller::Action> actions;
    actions << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::SetTerm, searchField)
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::ClickSearch, {})
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::WaitTasksFinish, {})
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::SelectResultsByNumbers, variantNumbers)
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::ClickDownload, remoteDialogActionsVariant)
            << NcbiSearchDialogFiller::Action(NcbiSearchDialogFiller::ClickClose, {});
    GTUtilsDialog::waitForDialog(new NcbiSearchDialogFiller(actions));

    GTMenu::clickMainMenuItem({"File", "Search NCBI GenBank..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    // 6. With Ctrl pressed, select all three annotation objects in the project view
    GTUtilsProjectTreeView::openView();
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    GTUtilsProjectTreeView::click("AB797210 features");
    GTUtilsProjectTreeView::click("AB797204 features");
    GTUtilsProjectTreeView::click("AB797201 features");

    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // 7. delete this objects through context menu
    GTUtilsDialog::waitForDialog(new PopupChooser({"action_project__remove_selected_action"}));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : the objects are deleted, the popup is shown
    QModelIndex firstIndex = GTUtilsProjectTreeView::findIndex("AB797210 features", {false});
    CHECK_SET_ERR(!firstIndex.isValid(), "The \"AB797210 features\" item has not been deleted");

    QModelIndex secondIndex = GTUtilsProjectTreeView::findIndex("AB797204 features", {false});
    CHECK_SET_ERR(!secondIndex.isValid(), "The \"AB797204 features\" item has not been deleted");

    QModelIndex thirdIndex = GTUtilsProjectTreeView::findIndex("AB797201 features", {false});
    CHECK_SET_ERR(!thirdIndex.isValid(), "The \"AB797201 features\" item has not been deleted");

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2656) {
    //    0. Create a file with an empty sequence. A FASTA file with the first line ">seq1" and the empty second line
    //    1. Open any sequence
    //    2. On the toolbar, press "Build dotplot"
    //    3. Press "Load sequence" in the dialog. Load the empty sequence
    //    4. Select the empty sequence as the second sequence in the combobox

    //    Bug: UGENE crashes
    //    Expected: an error message is shown
    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class DotplotLoadSequenceFiller : public Filler {
    public:
        DotplotLoadSequenceFiller(const QString& _seqPath, const QString& _seqName)
            : Filler("DotPlotDialog"), seqPath(_seqPath), seqName(_seqName) {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(seqPath, seqName));
            GTWidget::click(GTWidget::findPushButton("loadSequenceButton", dialog));

            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button != nullptr, "Cancel button is NULL");
            GTWidget::click(button);
        }

        QString seqPath;
        QString seqName;
    };

    GTUtilsDialog::waitForDialog(new DotplotLoadSequenceFiller(testDir + "_common_data/fasta", "empty_2.fa"));
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));
    GTUtilsDialog::checkNoActiveWaiters();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
}

GUI_TEST_CLASS_DEFINITION(test_2662) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open Call Variants sample.
    GTUtilsWorkflowDesigner::addSample("Call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //    3. Set valid input data.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Assembly (BAM/SAM)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bam/chrM.sorted.bam");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cmdline/call-variations/chrM.fa");
    //    4. Start the scheme.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    5. Open External Tools tab.
    GTUtilsDashboard::openTab(GTUtilsDashboard::ExternalTools);
    //    Expected state: vcfTools executible file is /usr/bin/perl path/to/vcfutils.pl
    //    Actual: vcfTools executible file is /usr/bin/perl

    auto node = GTUtilsDashboard::getExternalToolNodeByText("vcfutils run");
    GTWidget::click(node);

    if (isOsWindows()) {
        GTUtilsDashboard::getExternalToolNodeByText("samtools\\vcfutils.pl", false);
    } else {
        GTUtilsDashboard::getExternalToolNodeByText("samtools/vcfutils.pl", false);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2667) {
    //    1. Open {/data/samples/genbank/murine.gb}.
    //    Expected state: a document was added, it contains two object: an annotation and a sequence
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select the annotation object in the project view.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::click();

    //    3. Press "delete" key.
    //    Expected state: the annotation object is removed from the document.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    GTUtilsProjectTreeView::findIndex("NC_001363 features", options);
}

GUI_TEST_CLASS_DEFINITION(test_2683) {
    //    1. Open Tuxedo sample scheme in WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class custom : public CustomScenario {
    public:
        void run() {
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QWidget* dialog = GTWidget::getActiveModalWidget();
            QString title = GTUtilsWizard::getPageTitle();
            CHECK_SET_ERR(title == "Cuffdiff Samples", "unexpected title: " + title);

            GTWidget::findWidget("Sample1", dialog);
            GTWidget::findWidget("Sample2", dialog);

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
    //    Expected state: in appeared wizard there is a page "Cuffdiff Samples"
    //    where user can divide input datasets into samples for Cuffdiff tool.
}

GUI_TEST_CLASS_DEFINITION(test_2690) {
    //    1. Open "human_t1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Create an annotation: Group name - "1", location - "1..1".
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "1", "ann1", "1..1"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    //    3. Create an annotation: Group name - "2", location - "5..5, 6..7".
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "2", "ann2", "5..5, 6..7"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    //    4. Open the "Annotation highlighting" OP widget.
    GTWidget::click(GTWidget::findWidget("OP_ANNOT_HIGHLIGHT"));
    //    5. Select the first annotation.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("ann1"));
    GTMouseDriver::click();
    //    6. Click the "next annotation" button.
    auto nextAnnotationButton = GTWidget::findWidget("nextAnnotationButton");
    GTWidget::click(nextAnnotationButton);
    //    Expected state: the second annotation is selected.
    QString str = GTUtilsAnnotationsTreeView::getSelectedItem();
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
    GTFileDialog::openFile(dataDir + "/samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    ADVSingleSequenceWidget* wgt = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(wgt != nullptr, "No sequence widget");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(wgt), "No CV opened");

    class ImageQualityChecker : public Filler {
    public:
        ImageQualityChecker()
            : Filler("ImageExportForm") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto formatsBox = GTWidget::findComboBox("formatsBox", dialog);
            auto spin = GTWidget::findSpinBox("qualitySpinBox", dialog);

            GTComboBox::selectItemByText(formatsBox, "SVG");
            CHECK_SET_ERR(!spin->isVisible(), "Quality spin box is visible!");
            GTComboBox::selectItemByText(formatsBox, "PDF");
            CHECK_SET_ERR(!spin->isVisible(), "Quality spin box is visible!");
            GTComboBox::selectItemByText(formatsBox, "PS");
            CHECK_SET_ERR(!spin->isVisible(), "Quality spin box is visible!");

            GTComboBox::selectItemByText(formatsBox, "JPG");
            CHECK_SET_ERR(spin->isVisible(), "Quality spin box not visible!");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ImageQualityChecker());
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "Save circular view as image"}, GTGlobals::UseMouse));

    auto circularView = GTWidget::findWidget("CV_ADV_single_sequence_widget_0");
    GTWidget::click(circularView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2709) {
    //    1. Open tuxedo sample "no-new-transcripts"

    class test_2709_canceler : public CustomScenario {
    public:
        void run() {
            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"No-new-transcripts", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new test_2709_canceler()));
    GTUtilsWorkflowDesigner::addSample("tuxedo");
    //    Expected state: "No novel junctions" tophat parameter set to true by default
    GTUtilsWorkflowDesigner::click("Map RNA-Seq Reads with TopHat");
    QString result = GTUtilsWorkflowDesigner::getParameter("No novel junctions");
    CHECK_SET_ERR(result == "True", "No novel junctions parameter is " + result);
}

GUI_TEST_CLASS_DEFINITION(test_2713) {
    //    1. Open file {data/samples/Genbank/murine.gb}
    GTFile::copy(dataDir + "samples/Genbank/murine.gb", sandBoxDir + "test_2713.gb");
    GTFileDialog::openFile(sandBoxDir, "test_2713.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open file {data/samples/FASTA/human_T1.fa}
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Drag and drop annotation object "NC_001363 features" from project view to sequence view
    //    Expected state: the "Edit Object Relations" dialog has appeared
    //    4. Press "OK"
    //    Expected state: annotations has appeared on the sequence view
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    QPoint point = GTUtilsProjectTreeView::getItemCenter("NC_001363 features");
    point.setX(point.x() + 1);
    point.setY(point.y() + 1);
    GTMouseDriver::moveTo(point);
    GTMouseDriver::click();

    QPoint endPoint = GTWidget::getWidgetCenter(GTUtilsAnnotationsTreeView::getTreeWidget());
    GTMouseDriver::dragAndDrop(point, endPoint);

    GTUtilsDialog::checkNoActiveWaiters();

    //    5. Open file {data/samples/Genbank/murine.gb} with text editor, then make some modification and save file
    //    Expected state: dialog about detected file modification has appeared in UGENE window
    //    6. Press "Yes"
    //    Expected state: "human_T1" view has disappeared from the "Bookmarks" list, "murine.gb" has been reloaded.
    const GUIDialogWaiter::WaitSettings waitSettings("");
    GTUtilsDialog::waitForDialog(
        new MessageBoxDialogFiller(
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

    GTGlobals::sleep(1000);  // Wait at least 1 second: UGENE does not detect file changes within 1 second interval.
    opened = file.open(QIODevice::WriteOnly);
    CHECK_SET_ERR(opened, "Can't open the file: " + sandBoxDir + "test_2713.gb");
    file.write(fileData);
    file.close();

    GTUtilsDialog::checkNoActiveWaiters();
    //    7. Open "human_T1" sequence view
    //    Expected state: annotations from "murine.gb" present on the sequence view
    GTUtilsProjectTreeView::doubleClickItem("human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::findFirstAnnotation();
}

GUI_TEST_CLASS_DEFINITION(test_2726) {
    // 1. Open "COI.aln".
    // 2. Select the second symbol at the first line.
    // 3. Press backspace twice.
    // Expected state: undo and redo buttons are disabled.
    GTFileDialog::openFile(dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(1, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // Expected state : MSA view has opened, the "Undo" button is disabled.
    QAbstractButton* undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");
    // Expected state : MSA view has opened, the "Redo" button is disabled.
    QAbstractButton* redoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!redoButton->isEnabled(), "'Redo' button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2729) {
    //    1. Open {_common_data/fasta/AMINO.fa}
    //    Expected state: there is no a "Graphs" button on the sequence toolbar for amino, it is invisible and disabled.
    GTFileDialog::openFile(testDir + "_common_data/fasta/", "AMINO.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    QAbstractButton* graphsButton = GTAction::button("GraphMenuAction", GTUtilsSequenceView::getSeqWidgetByNumber());
    CHECK_SET_ERR(nullptr != graphsButton, "Graphs button is NULL");
    CHECK_SET_ERR(!graphsButton->isEnabled(), "Graphs button is unexpectedly enabled");

    //    2. Click the "Graphs" button.
    //    Expected state: menu is shown.
    //    GTUtilsDialog::waitForDialog(new PopupChooser(QStringList()));
    //    GTWidget::click(graphsButton);
}

GUI_TEST_CLASS_DEFINITION(test_2730) {
    /*  1. Open "_common_data/fasta/abcd.fa" as separate sequences in the sequence viewer.
    2. Click the "Automatic Annotation Highlighting" button on the first sequence's toolbar.
    3. Click the "Plasmid features" button.
    Expected: UGENE does not crash.
*/
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/fasta/abcd.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto parent = GTWidget::findWidget("ADV_single_sequence_widget_0");

    auto menuAction = GTWidget::findWidget("AutoAnnotationUpdateAction", parent);
    GTUtilsDialog::waitForDialog(new PopupChooser({"Plasmid features"}));
    GTWidget::click(menuAction);
    // Close file - UGENE does not crash.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("abcd.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_2737) {
    // 1. Open any sequence without annotations (e.g. "_common_data/fasta/AMINO.fa")
    GTFileDialog::openFile(testDir + "_common_data/fasta/", "AMINO.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Add few annotations with different names.
    GTUtilsAnnotationsTreeView::createAnnotation("group", "name1", "1..10");
    GTUtilsAnnotationsTreeView::createAnnotation("group", "name2", "11..20", false);
    GTUtilsAnnotationsTreeView::createAnnotation("group", "name3", "21..30", false);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open Annotation Highlighting tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    // 4. Delete all annotations one by one.
    GTUtilsAnnotationsTreeView::deleteItem("name1");
    GTUtilsAnnotationsTreeView::deleteItem("name2");
    GTUtilsAnnotationsTreeView::deleteItem("name3");

    // Expected state: there is no annotations is annotation tree.
    auto annotationsTree = GTWidget::findWidget("OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(!annotationsTree->isVisible(), "Annotations tree is shown");
}

GUI_TEST_CLASS_DEFINITION(test_2737_1) {
    GTLogTracer lt;
    // 1. Open "murine.gb";
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 1.1. Open Annotation Highlighting tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    // 2. Delete all annotations in random order;
    QTreeWidgetItem* annotation = nullptr;
    while (nullptr != (annotation = GTUtilsAnnotationsTreeView::findFirstAnnotation({false}))) {
        uiLog.trace("annotation text is: " + annotation->text(0));
        GTUtilsAnnotationsTreeView::deleteItem(annotation);
    }

    // Expected state: no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2754) {
    // 1. Open "murine.gb";
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QAbstractButton* next = GTWidget::findButtonByText("Next", dialog);
            CHECK_SET_ERR(!next->isEnabled(), "Next button is unexpectidly enabled");

            QAbstractButton* selectAll = GTWidget::findButtonByText("Select all", dialog);
            CHECK_SET_ERR(!selectAll->isEnabled(), "Select all button is unexpectidly enabled");

            GTWidget::click(GTWidget::findButtonByText("Close", dialog));
        }
    };

    GTUtilsDialog::waitForDialog(new FindQualifierFiller(new custom()));

    GTUtilsDialog::waitForDialog(new PopupChooser({"find_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("CDS"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_2761_1) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    QDir().mkpath(sandBoxDir + "test_2761_1");
    GTFile::setReadOnly(sandBoxDir + "test_2761_1");
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Select some area in the MSA.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 2), QPoint(5, 5));
    //    3. Context menu of the area -> Export -> Save subalignment.

    class customFiller : public ExtractSelectedAsMSADialogFiller {
    public:
        customFiller()
            : ExtractSelectedAsMSADialogFiller(testDir + "_common_data/scenarios/sandbox/test_2761_1/2761.aln", {"Bicolorana_bicolor_EF540830", "Roeseliana_roeseli"}) {
        }
        void run() override {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "No write permission"));
            ExtractSelectedAsMSADialogFiller::run();
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new customFiller());
    GTMouseDriver::click(Qt::RightButton);
    //    4. Set the destination path to the dir without write permissions.
    //    5. Click "Extract".
    //    Expected: the message about write permissions to the dir appears. The extraction task is not run.
}

GUI_TEST_CLASS_DEFINITION(test_2761_2) {
    //    1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Select some area in the MSA.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 2), QPoint(5, 5));
    //    3. Context menu of the area -> Export -> Save subalignment.

    class customFiller : public ExtractSelectedAsMSADialogFiller {
    public:
        customFiller()
            : ExtractSelectedAsMSADialogFiller(testDir + "_common_data/scenarios/sandbox/test_2761_2/2761.aln", {"Bicolorana_bicolor_EF540830", "Roeseliana_roeseli"}) {
        }
        void run() override {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Export folder does not exist"));
            ExtractSelectedAsMSADialogFiller::run();
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new customFiller());
    GTMouseDriver::click(Qt::RightButton);
    //    4. Set the destination path to the dir that does not exist.
    //    5. Click "Extract".
    //    Expected: the message about write permissions to the dir appears. The extraction task is not run.
}

GUI_TEST_CLASS_DEFINITION(test_2762) {
    /*  1. Open something, e.g. "human_T1.fa".
    2. Close the project.
        Expected state: a dialog will appear that offer you to save the project.
    3. Press escape key.
        Expected state: the dialog will be closed as canceled.
*/
    class EscClicker : public Filler {
    public:
        EscClicker()
            : Filler("SaveProjectDialog") {
        }
        void run() override {
            if (isOsMac()) {
                GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
            } else {
                GTKeyboardDriver::keyClick(Qt::Key_Escape);
            }
        }
    };

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new EscClicker());
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTUtilsProject::checkProject();
}

GUI_TEST_CLASS_DEFINITION(test_2770) {
    // 1. File -> New document from text.
    // 2. Data: TTTTTTTTTTTTTTTTTTTTTTTAAATTTTTTTTTTTTTTTTTTTTTTT
    // Location: set the valid output file.
    // 3. Create.
    // 4. Ctrl + F.
    // 5. Pattern: TTTTTTTTTTTTTTTTTTTTTTTAATTTTTTTTTTTTTTTTTTTTTTT
    // Algorithm: InsDel
    // Should match: 30%
    // 6. Search.
    // Expected: two annotations are found.
    Runnable* filler = new CreateDocumentFiller(
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
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("result");

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern("TTTTTTTTTTTTTTTTTTTTTTTAATTTTTTTTTTTTTTTTTTTTTTT", true);

    GTUtilsOptionPanelSequenceView::setAlgorithm("InsDel");

    GTUtilsOptionPanelSequenceView::setMatchPercentage(30);

    GTUtilsOptionPanelSequenceView::clickGetAnnotation();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::findItem("misc_feature  (0, 2)");
}

GUI_TEST_CLASS_DEFINITION(test_2773) {
    // 1. Open file test/_common_data/cmdline/custom-script-worker-functions/translateTest/translateTest.uwl
    // 2. Set parameters:
    // input: _common_data/cmdline/DNA.fa
    // offset: sss
    // out: some/valid/path
    // 3. run the scheme.
    // Expected state: UGENE doesn't crash, error message appears.

    GTFile::copy(testDir + "_common_data/cmdline/_proto/translateTest.usa", dataDir + "/workflow_samples/users/translateTest.usa");

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/cmdline/custom-script-worker-functions/translateTest/translateTest.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cmdline/DNA.fa");

    GTUtilsWorkflowDesigner::click("translateTest");
    GTUtilsWorkflowDesigner::setParameter("offset", "sss", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click("Write sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", "_common_data/cmdline/result_test_offset", GTUtilsWorkflowDesigner::textValue);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTWidget::click(GTAction::button("Run workflow"));

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}
GUI_TEST_CLASS_DEFINITION(test_2778) {
    // 1. Use main menu : tools->align to reference->align short reads
    // 2. Set input parameters
    // input sequence : _common_data / genome_aligner / chrY.fa
    // short reads : _common_data / genome_aligner / shortreads15Mb.fasta
    // mismatches allowed : checked
    // 3. Press start

    // Expected state : the task should be finished without errors.

    GTLogTracer lt;

    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(testDir + "_common_data/genome_aligner/",
                                                               "chrY.fa",
                                                               testDir + "_common_data/genome_aligner/",
                                                               "shortreads1Mb.fasta",
                                                               true);
    parameters.samOutput = false;
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(&parameters));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2784) {
    GTLogTracer lt;

    // 1. Open the file "data/samples/CLUSTALW/COI.aln"
    GTFileDialog::openFile(dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : MSA view has opened, the "Undo" button is disabled.
    QAbstractButton* undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");

    // 2. Choose in the context menu: Align->Align with MUSCLE….
    // Expected state : The "Align with MUSCLE" dialog has appeared
    // 3. Check the "Translation to amino when aligning" checkbox and press "Align"
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString initialRegionContent = GTClipboard::text();

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Default, true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));

    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(10, 5));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : Alignment task has started.After some time it finishes without errors
    // and alignment gets changed somehow.The "Undo" button becomes active
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectedly disabled");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString alignedRegionContent = GTClipboard::text();
    CHECK_SET_ERR(alignedRegionContent != initialRegionContent, "Alignment content has not been changed");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 4. Click on the "Undo" button
    GTWidget::click(undoButton);

    // Expected state : Alignment has been restored to its initial state.The "Undo" button gets disabled,
    // the "Redo" has been enabled
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString undoneRegionContent = GTClipboard::text();
    CHECK_SET_ERR(undoneRegionContent == initialRegionContent, "Undo hasn't reverted changes");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    CHECK_SET_ERR(!undoButton->isEnabled(), "'Undo' button is unexpectedly enabled");

    QAbstractButton* redoButton = GTAction::button("msa_action_redo");
    CHECK_SET_ERR(redoButton->isEnabled(), "'Redo' button is unexpectedly disabled");

    // 5. Click on the "Redo" button
    GTWidget::click(redoButton);

    // Expected state : Alignment has been changed.The "Redo" button gets disabled,
    // the "Undo" has been enabled
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(41, 0), QPoint(43, 17));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString redoneRegionContent = GTClipboard::text();
    CHECK_SET_ERR(redoneRegionContent == alignedRegionContent, "Redo hasn't changed the alignment");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    CHECK_SET_ERR(!redoButton->isEnabled(), "'Redo' button is unexpectedly enabled");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectedly disabled");
}

GUI_TEST_CLASS_DEFINITION(test_2796) {
    // 1. Open file "_common_data/fasta/fa2.fa"
    GTFileDialog::openFile(testDir + "_common_data/fasta", "fa2.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Use main menu { Actions -> Analyse -> Find pattern[Smith-Waterman]... }
    // Expected state: the "Smith-Waterman Search" dialog has appeared
    // 3. Press "Cancel" in the dialog
    // Expected state: dialog has closed
    SmithWatermanDialogFiller* filler = new SmithWatermanDialogFiller();
    filler->button = SmithWatermanDialogFiller::Cancel;
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_2801) {
    // 1. Open {_common_data/clustal/100_sequences.aln}.
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Start MAFFT with default values.
    GTUtilsDialog::waitForDialog(new MAFFTSupportRunDialogFiller(new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with MAFFT"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // 3. Cancel "align" task.
    GTUtilsTaskTreeView::openView();
    GTUtilsTaskTreeView::checkTaskIsPresent("Run MAFFT alignment task");
    GTUtilsTaskTreeView::cancelTask("Run MAFFT alignment task");
    // Expected state: the task is cancelled, there is no MAFFT processes with its subprocesses (check for the "disttbfast" process)
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2801_1) {
    // 1. Open {_common_data/clustal/100_sequences.aln}.
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Start MAFFT with default values.
    GTUtilsDialog::waitForDialog(new MAFFTSupportRunDialogFiller(new MAFFTSupportRunDialogFiller::Parameters()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with MAFFT"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTGlobals::sleep(10000);  // Give a task some time to run.

    // 3. Cancel the "align" task.
    GTUtilsTaskTreeView::openView();
    GTUtilsTaskTreeView::checkTaskIsPresent("Run MAFFT alignment task");
    GTUtilsTaskTreeView::cancelTask("Run MAFFT alignment task");
    // Expected state: the task is cancelled, there is no MAFFT processes with its subprocesses (check for the "disttbfast" process)
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2808) {
    //    1. Open WD.
    //    2. Add "Sequence Marker" element to the scene, select it.
    //    Expected state: there are buttons on the "parameters" widget: "add", "edit" and "remove". The "add" button is enabled, other buttons are disabled.
    //    3. Add a new marker group (click the "add" button and fill the dialog).
    //    Expected state: a new group was added, there is no selection in the marker group list, the "add" button is enabled, other buttons are disabled.
    //    4. Select the added group.
    //    Expected state: the group is selected, all buttons are enabled.
    //    5. Click the "remove" button.
    //    Expected state: the group is removed (the list is empty), the "add" button is enabled, other buttons are disabled.

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Sequence Marker");
    GTUtilsWorkflowDesigner::click("Sequence Marker");

    auto addButton = GTWidget::findToolButton("addButton");

    auto editButton = GTWidget::findToolButton("editButton");

    auto removeButton = GTWidget::findToolButton("removeButton");

    CHECK_SET_ERR(addButton->isEnabled(), "AddButton is disabled!");
    CHECK_SET_ERR(!editButton->isEnabled(), "EditButton is enabled!");
    CHECK_SET_ERR(!removeButton->isEnabled(), "AddButton is enabled!");

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

    CHECK_SET_ERR(addButton->isEnabled(), "AddButton is disabled!");
    CHECK_SET_ERR(!editButton->isEnabled(), "EditButton is enabled!");
    CHECK_SET_ERR(!removeButton->isEnabled(), "AddButton is enabled!");

    auto groupTable = GTWidget::findTableView("markerTable");
    GTWidget::click(groupTable);

    QPoint p = GTTableView::getCellPosition(groupTable, 0, 0);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();

    CHECK_SET_ERR(addButton->isEnabled(), "AddButton is disabled!");
    CHECK_SET_ERR(editButton->isEnabled(), "EditButton is disabled!");
    CHECK_SET_ERR(removeButton->isEnabled(), "AddButton is disabled!");

    GTWidget::click(removeButton);

    CHECK_SET_ERR(groupTable->model() != nullptr, "Abstract table model is NULL");
    CHECK_SET_ERR(groupTable->model()->rowCount() == 0, "Marker table is not empty!");
}

GUI_TEST_CLASS_DEFINITION(test_2809) {
    //    1. Open WD.
    //    2. Add a "Sequence Marker" element to the scene, select it.
    //    3. Add several items to marker group list on the parameters widget.
    //    Expected state: if all items are visible, there is no vertical scroll bar.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Sequence Marker");
    GTUtilsWorkflowDesigner::click("Sequence Marker");

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
    GTWidget::click(groupTable);

    QScrollBar* scroll = groupTable->verticalScrollBar();
    CHECK_SET_ERR(scroll != nullptr, "Scroll bar is NULL");
    CHECK_SET_ERR(!scroll->isVisible(), "Scroll bar is visible!");
}

GUI_TEST_CLASS_DEFINITION(test_2811) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::toggleDebugMode();
    GTUtilsWorkflowDesigner::toggleBreakpointManager();

    //    2. Open any workflow, create a breakpoint for any element.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTUtilsWorkflowDesigner::setBreakpoint("Align with MUSCLE");

    //    3. Open another workflow.
    //    Expected state: breakpoints list is cleared.
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    QStringList breakpointList = GTUtilsWorkflowDesigner::getBreakpointList();
    CHECK_SET_ERR(breakpointList.isEmpty(), "There are unexpected breakpoints");
}

GUI_TEST_CLASS_DEFINITION(test_2829) {
    // 1) Open files "data/samples/Genbank/murine.gb" and "data/samples/Genbank/sars.gb" in separated views
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2) Click on toolbar 'Build dotplot' button
    // 3) In opened dialog click 'OK' button
    GTUtilsDialog::waitForDialog(new DotPlotFiller());
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

    // 4) Choose some annotation by left mouse button on the upper sequence view
    // Expected state: horizontal or vertical selection is shown on DotPlot
    GTTreeWidget::click(GTUtilsAnnotationsTreeView::findItem("gene"));

    // 5) In second sequence view click Remove sequence on the toolbar
    // Expected state: DotPlot closed and UGENE didn't crash
    GTUtilsMdi::activateWindow("NC_001363 [murine.gb]");

    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363", GTUtilsMdi::activeWindow());
    GTWidget::click(GTWidget::findWidget("remove_sequence", toolbar));
}

GUI_TEST_CLASS_DEFINITION(test_2853) {
    GTUtilsDialog::waitForDialog(new NCBISearchDialogSimpleFiller("rat"));

    GTMenu::clickMainMenuItem({"File", "Search NCBI GenBank..."}, GTGlobals::UseKey);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2863) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Add "Read File URL(s)".
    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::addElement("Read File URL(s)");
    //    3. Add "File Format Conversion".
    WorkflowProcessItem* conversion = GTUtilsWorkflowDesigner::addElement("File Format Conversion");
    //    4. Connect the elements.
    GTUtilsWorkflowDesigner::connect(fileList, conversion);
    //    Expected: the converter's input slot "Source URL" is binded with the "Source URL" slot of the Read File URL(s).
    GTUtilsWorkflowDesigner::click(conversion);
    QTableWidget* table = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    QString s1 = table->item(0, 0)->text();
    QString s2 = table->item(0, 1)->text();
    CHECK_SET_ERR(s1 == "Source URL", "unexpected first value: " + s1);
    CHECK_SET_ERR(s2 == "Source URL (by Read File URL(s))", "unexpected second value: " + s2)
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
    GTLogTracer lt;

    AlignShortReadsFiller::Parameters parameters(testDir + "_common_data/e_coli/",
                                                 "NC_008253.gff",
                                                 testDir + "_common_data/e_coli/",
                                                 "e_coli_1000.fastq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);

    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new MessageBoxDialogFiller("Yes"));
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "test_2866.ugenedb"));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2884) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Place Cuffdiff element on the scene
    GTUtilsWorkflowDesigner::addAlgorithm("Test for Diff. Expression with Cuffdiff");

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Test for Diff. Expression with Cuffdiff"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter("Multi read correct") == "False", "'Mate inner distance', Parameter value doesn't amtch");
}

GUI_TEST_CLASS_DEFINITION(test_2887) {
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Place Tophat element on the scene
    GTUtilsWorkflowDesigner::addAlgorithm("Map RNA-Seq Reads with TopHat");

    // 3. check "Mate inner distance" parameter is 50
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Map RNA-Seq Reads with TopHat"));
    GTMouseDriver::click();
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getParameter("Mate inner distance") == "50", "'Mate inner distance', Parameter value doesn't amtch");
}

GUI_TEST_CLASS_DEFINITION(test_2894) {
    // Open {_common_data/clustal/100_sequences.aln}.
    GTFileDialog::openFile(testDir + "_common_data/clustal", "100_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsProjectTreeView::toggleView();  // close project tree view to get more space.

    // Use context menu {Tree->Build Tree}.
    // Expected state: "Build phylogenetic tree" dialog has been appeared.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_2894_COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Run "Phylip Neighbor Joining" with default parameters.
    // Expected state: tree view has been appeared.
    GTWidget::findWidget("treeView");
    GTUtilsOptionPanelMsa::toggleTab(GTUtilsOptionPanelMsa::TreeOptions);  // close project tree view to get more space.

    // Press refresh tree button on the tree's toolbar.
    // Expected state: "Calculating Phylogenetic Tree" task has been started.
    GTWidget::click(GTAction::button("Refresh tree"));
    int taskCount = GTUtilsTaskTreeView::countTasks("Calculating Phylogenetic Tree");
    CHECK_SET_ERR(taskCount == 1, QString("1. Wrong tasks number. Expected 1, actual: ").arg(taskCount));

    // Press refresh button again.
    // Expected state: a new refresh task is not started, the old one is in process.
    GTWidget::click(GTAction::button("Refresh tree"));

    taskCount = GTUtilsTaskTreeView::countTasks("Calculating Phylogenetic Tree");
    CHECK_SET_ERR(taskCount == 1, QString("2. Wrong tasks number. Expected 1, actual: ").arg(taskCount));
    // Close the tree view while the task is running.
    // Expected state: UGENE doesn't crash, view is closed, task is canceled.
    GTUtilsProjectTreeView::click("test_2894_COI.nwk");

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));  // Save the nwk file? Select 'No'.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_2895) {
    // 1. Open "_common_data/fasta/amino_multy.fa" as multiple alignment.
    // 2. Open "_common_data/fasta/amino_multy_ext.fa" as separate sequences.
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/amino_multy_ext.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/amino_multy.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 3. Try to add to the amino_multy.fa document, any sequence from the amino_multy_ext.fa document.
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("amino_multy_ext.fa", "chr1_gl000191_random Amino translation 0 direct"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "Sequence from current project..."});
    // Expected: an object will be added.
}

GUI_TEST_CLASS_DEFINITION(test_2897) {
    //    1. Open {data/samples/CLUSTALW/COI.aln}.
    GTFileDialog::openFile(dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Open options panel 'Highlighting' tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::Highlighting);

    auto combo = GTWidget::findComboBox("highlightingScheme");
    int oldItemsNumber = combo->count();

    //    3. Create a new custom nucleotide color scheme.
    QString colorSchemeName = GTUtils::genUniqueString(name);
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTUtilsDialog::add(new NewColorSchemeCreator(colorSchemeName, NewColorSchemeCreator::nucl));
    MSAEditorSequenceArea* msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea();
    GTMenu::showContextMenu(msaSeqArea);

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", colorSchemeName}));
    GTMenu::showContextMenu(msaSeqArea);

    combo = GTWidget::findComboBox("highlightingScheme");
    int newItemsNumber = combo->count();

    CHECK_SET_ERR(newItemsNumber == oldItemsNumber, "exportButton is disabled unexpectedly");
}

GUI_TEST_CLASS_DEFINITION(test_2899) {
    //    1. Start WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Open "RNA-seq analysis with Tuxedo tools" sample.
    //    3. Click the "Setup" button (default values: Full, Single-end).

    class custom : public CustomScenario {
    public:
        void run() {
            //    4. Go to the "Cuffmerge settings" page and click the "Defaults" button.
            QWidget* dialog = GTWidget::getActiveModalWidget();
            for (int i = 0; i < 4; i++) {
                GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            }
            CHECK_SET_ERR(GTUtilsWizard::getPageTitle() == "Cuffmerge settings", "unexpected page");
            GTUtilsWizard::clickButton(GTUtilsWizard::Defaults);

            //    5. Return to the "Tophat settings" page and set any file as Bowtie index (it might be valid Bowtie index or not).
            for (int i = 0; i < 2; i++) {
                GTUtilsWizard::clickButton(GTUtilsWizard::Back);
            }

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/bowtie2/index/human_T1_cutted.2.bt2"));
            GTWidget::click(GTWidget::findButtonByText("Select\nbowtie index file", dialog));
            //    Expected state: index is set if it is valid (file has valid extension), no crash
            bool basename = GTUtilsWizard::getParameter("Bowtie index basename").toString() == "human_T1_cutted";
            bool dir = GTUtilsWizard::getParameter("Bowtie index folder").toString().contains("_common_data/bowtie2/index");
            CHECK_SET_ERR(basename, "unexpected basename");
            CHECK_SET_ERR(dir, "unexpected dir");
            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };
    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Tuxedo Workflow", {"Full", "Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Tuxedo Wizard", new custom()));
    GTUtilsWorkflowDesigner::addSample("RNA-seq analysis with Tuxedo tools");
}

GUI_TEST_CLASS_DEFINITION(test_2900) {
    //    1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    //    2. Sequence view context menu -> Analyze -> Restriction sites.
    //    3. Press "OK".
    //    Expected: the 8 regions of auto-annotations are created.
    const QStringList defaultEnzymes = {"BamHI", "BglII", "ClaI", "DraI", "EcoRI", "EcoRV", "HindIII", "PstI", "SalI", "SmaI", "XmaI"};
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(defaultEnzymes));
    GTMenu::showContextMenu(GTUtilsSequenceView::getSeqWidgetByNumber());

    GTUtilsTaskTreeView::waitTaskFinished();
    const int firstAnnotationsCount = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup("enzyme  (8, 0)").size();

    //    4. Repeast 2-3.
    //    Expected: there are still 8 regions of auto-annotations. Old regions are removed, new ones are added.
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(defaultEnzymes));
    GTMenu::showContextMenu(GTUtilsSequenceView::getSeqWidgetByNumber());
    GTUtilsTaskTreeView::waitTaskFinished();
    const int secondAnnotationsCount = GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup("enzyme  (8, 0)").size();

    CHECK_SET_ERR(firstAnnotationsCount == secondAnnotationsCount, QString("Annotations count differs: %1 annotations in the first time, %2 annotations in the second time").arg(firstAnnotationsCount).arg(secondAnnotationsCount));
}

GUI_TEST_CLASS_DEFINITION(test_2903) {
    //    1. Open the attached file
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/regression/2903", "unknown_virus.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() override {
            GTWidget::getActiveModalWidget();
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    };

    //    2. Click on the Analyze->Query NCBI BLAST database context menu
    //    3. Click on the Search button
    //    Expected state: the task starts with no errors
    //    Current state: the following error appears: 'RemoteBLASTTask' task failed: Database couldn't prepare the response
    GTUtilsDialog::waitForDialog(new RemoteBLASTDialogFiller(new Scenario));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ANALYSE", "Query NCBI BLAST database"}));
    GTMenu::showContextMenu(GTWidget::findWidget("ren"
                                                 "der_area_virus_X"));
    QString blastTaskName = "RemoteBLASTTask";
    GTUtilsTaskTreeView::checkTaskIsPresent(blastTaskName);
    GTGlobals::sleep(10000);  // Give a task some time to run.

    // Cancel the task. If not cancelled the run may last too long to trigger timeout in nightly tests.
    GTUtilsTaskTreeView::cancelTask(blastTaskName, false);

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_2907) {
    // 1. Open file _common_data/genbank/pBR322.gb
    GTFileDialog::openFile(testDir + "_common_data/genbank/", "pBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. In annotations tree view go to element Auto - annotations->enzyme->EcoRI(0, 1)->EcoRI
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    auto qt_toolbar_ext_button = GTWidget::findWidget("qt_toolbar_ext_button", GTWidget::findWidget("mwtoolbar_activemdi"), {false});
    if (qt_toolbar_ext_button != nullptr && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(qt_toolbar_ext_button);
    }
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("EcoRI"));

    // 3. Stop cursor on EcoRI qualifier.Wait for tooltip
    // Expected state : tooltip will appear
    GTUtilsToolTip::checkExistingToolTip("db_xref");
}

GUI_TEST_CLASS_DEFINITION(test_2910) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+A.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 10000..15000
    // 4. Click the "Go" button.

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("10000..15000"));
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);

    // Expected state: the region is selected, there is a label "[5001 bp]" on the pan view.
    QVector<U2Region> reg = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(reg.size() == 1, QString("unexpected number of selected regions: ").arg(reg.size()));
    CHECK_SET_ERR(reg.first().length == 5001, QString("unexpected selection length: ").arg(reg.first().length));
    CHECK_SET_ERR(reg.first().startPos == 9999, QString("unexpected selection start pos: ").arg(reg.first().startPos));
}
GUI_TEST_CLASS_DEFINITION(test_2910_1) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+A.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 2000..5000,100000..110000
    // 4. Click the "Go" button.

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("2000..5000,100000..110000"));
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);

    // Expected state: the region is selected, there is a label "[3001 bp]" and "[10001 bp]" on the pan view.
    QVector<U2Region> reg = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(reg.size() == 2, QString("unexpected number of selected regions: ").arg(reg.size()));
    CHECK_SET_ERR(reg.first().length == 3001, QString("unexpected selection length: ").arg(reg.first().length));
    CHECK_SET_ERR(reg.first().startPos == 1999, QString("unexpected selection start pos: ").arg(reg.first().startPos));

    CHECK_SET_ERR(reg.at(1).length == 10001, QString("unexpected selection length: ").arg(reg.at(1).length));
    CHECK_SET_ERR(reg.at(1).startPos == 99999, QString("unexpected selection start pos: ").arg(reg.at(1).startPos));
}

GUI_TEST_CLASS_DEFINITION(test_2910_2) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+A.
    // Expected state: a "Region Selection" dialog appeared.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 0..5000
    // Expected state: the region is invalid, a "Go" button is disabled.
    class CancelClicker : public Filler {
    public:
        CancelClicker()
            : Filler("RangeSelectionDialog") {
        }
        virtual void run() {
            QWidget* w = GTWidget::getActiveModalWidget();
            QRadioButton* multipleButton = w->findChild<QRadioButton*>("miltipleButton");
            CHECK_SET_ERR(multipleButton != nullptr, "RadioButton \"miltipleButton\" not found");
            GTRadioButton::click(multipleButton);

            QLineEdit* regionEdit = w->findChild<QLineEdit*>("multipleRegionEdit");
            CHECK_SET_ERR(regionEdit != nullptr, "QLineEdit \"multipleRegionEdit\" not foud");
            GTLineEdit::setText(regionEdit, "0..5000");

            QDialogButtonBox* buttonBox = w->findChild<QDialogButtonBox*>("buttonBox");
            CHECK_SET_ERR(nullptr != buttonBox, "button box is null");
            QPushButton* button = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(nullptr != button, "cancel button is null");
            QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!okButton->isEnabled(), "OK button is unexpectedly enabled");
            GTWidget::click(button);
        }
    };
    GTUtilsDialog::waitForDialog(new CancelClicker());
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);
}
GUI_TEST_CLASS_DEFINITION(test_2910_3) {
    // 1. Open {data/samples/FASTA/human_T1.fa}.
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+A.
    // Expected state: a "Region Selection" dialog appeared.
    // 3. Select a "Multiple Range Selection" mode, enter the region: 1..199951
    // Expected state: the region is invalid, a "Go" button is disabled.
    class CancelClicker : public Filler {
    public:
        CancelClicker()
            : Filler("RangeSelectionDialog") {
        }
        virtual void run() {
            QWidget* w = GTWidget::getActiveModalWidget();
            QRadioButton* multipleButton = w->findChild<QRadioButton*>("miltipleButton");
            CHECK_SET_ERR(multipleButton != nullptr, "RadioButton \"miltipleButton\" not found");
            GTRadioButton::click(multipleButton);

            QLineEdit* regionEdit = w->findChild<QLineEdit*>("multipleRegionEdit");
            CHECK_SET_ERR(regionEdit != nullptr, "QLineEdit \"multipleRegionEdit\" not foud");
            GTLineEdit::setText(regionEdit, "1..199951");

            QDialogButtonBox* buttonBox = w->findChild<QDialogButtonBox*>("buttonBox");
            CHECK_SET_ERR(nullptr != buttonBox, "button box is null");
            QPushButton* button = buttonBox->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(nullptr != button, "cancel button is null");
            QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(!okButton->isEnabled(), "OK button is unexpectedly enabled");
            GTWidget::click(button);
        }
    };
    GTUtilsDialog::waitForDialog(new CancelClicker());
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_2923) {
    // 1. Open "samples/FASTA/human_T1.fa".
    // Expected: the sequence view is opened.
    // 2. Press Ctrl+W.
    // Expected: the sequence view is closed.

    // Close active view
    GTUtilsMdi::closeActiveWindow();

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMdi::activeWindow();

    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    QWidget* mdi = GTUtilsMdi::activeWindow({false});
    CHECK_SET_ERR(mdi == nullptr, "Sequence view is not closed");
}

GUI_TEST_CLASS_DEFINITION(test_2924) {
    // 1. Open "_common_data/scenarios/_regression/2924/human_T1_cutted.fa".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/2924/", "human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "_common_data/scenarios/_regression/2924/MyDocument_3.gb".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/2924/", "MyDocument_3.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Drag'n'drop the annotation object from the project to the sequence view.
    // Expected: the dialog appears.
    // 4. Click "OK".
    // Expected : the annotations are shown on the sequence view.
    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(GTUtilsProjectTreeView::findIndex("Annotations"), GTUtilsAnnotationsTreeView::getTreeWidget());

    // 5. Click the "Shown circular view" button on the sequence toolbar.
    // Expected : the circular view is shown.
    ADVSingleSequenceWidget* seqWidget = GTUtilsSequenceView::getSeqWidgetByNumber();
    GTUtilsCv::cvBtn::click(seqWidget);

    // 6. Click the "Zoom in" button several times until it becomes disabled.
    // Expected : UGENE does not crash.
    QWidget* zoomInButton = GTAction::button("tbZoomIn_human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1_cutted.fa]");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(zoomInButton);
    }
}

GUI_TEST_CLASS_DEFINITION(test_2927) {
    //    1. Open file "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open file "data/samples/PDB/1CRN.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB/1CRN.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Remove first document with delete key.
    GTUtilsProjectTreeView::click("1CF7.PDB");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Press "Open" toolBar button.
    //    Expected state: UGENE doesn't crash.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/PDB/1CF7.PDB"));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_MAIN, "Open");
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

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class SiteconCustomFiller : public Filler {
    public:
        SiteconCustomFiller()
            : Filler("SiteconSearchDialog") {
        }
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto modelFileEdit = GTWidget::findLineEdit("modelFileEdit");
            CHECK_SET_ERR(modelFileEdit->text().isEmpty(), "Model is set!");

            auto errComboBox = GTWidget::findComboBox("errLevelBox");
            CHECK_SET_ERR(errComboBox->currentText().isEmpty(), "Threshold is set!");

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new SiteconCustomFiller());
    GTWidget::click(GTWidget::findWidget("SITECON_widget"));
}

GUI_TEST_CLASS_DEFINITION(test_2930) {
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_2930"));
    GTUtilsProject::openFile(dataDir + "samples/ACE/K26.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_2931) {
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_2931"));
    GTUtilsProject::openFile(dataDir + "samples/ACE/K26.ace");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ConvertAssemblyToSAMDialogFiller(sandBoxDir, "test_2931"));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Convert UGENE assembly database to SAM..."});

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2945) {
    //    1.    Open file with annotations, e.g. murine.db
    //    2.    Open Circular View(CV)
    //    3.    Move the splitter between CV and Sequence View(SV) up to hide CV (CV action button is still active)
    //    4.    Move the splitter between Sequence View and Annotation Tree View down.
    //    expected state: CV appeared. all parts of CV are visiable

    // New state of UGENE: seq.view is resizable now, so CV will not be visible by dragging splitter between SV and AE

    GTFileDialog::openFile(dataDir + "/samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTAction::button("CircularViewAction"));
    QWidget* zoomIn = GTAction::button("tbZoomIn_NC_001363 [murine.gb]");
    CHECK_SET_ERR(zoomIn != nullptr, "zoomIn action on CV not found");

    auto splitterHandler = GTWidget::findWidget("qt_splithandle_annotated_DNA_scrollarea");
    GTWidget::click(splitterHandler);

    auto mainToolBar = GTWidget::findWidget("mwtoolbar_activemdi");
    QPoint bottomLeftToolBar = mainToolBar->geometry().bottomLeft();
    bottomLeftToolBar = mainToolBar->mapToGlobal(bottomLeftToolBar);

    GTMouseDriver::press();
    GTMouseDriver::moveTo(bottomLeftToolBar);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    QPoint handlePosition = splitterHandler->pos();

    QAbstractButton* cvButton = GTAction::button("CircularViewAction");
    CHECK_SET_ERR(cvButton->isChecked(), "CV button is not checked!");

    auto splitter = GTWidget::findSplitter("annotated_DNA_splitter");
    int idx = splitter->indexOf(GTWidget::findWidget("annotations_tree_view"));
    QSplitterHandle* handle = splitter->handle(idx);
    CHECK_SET_ERR(handle != nullptr, "SplitterHadle not found");

    GTWidget::click(handle);

    QPoint p = GTMouseDriver::getMousePosition();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p + QPoint(0, 50));
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    CHECK_SET_ERR(handlePosition == splitterHandler->pos(), QString("Handler was moved: expected: %1, actual: %2").arg(splitter->pos().y()).arg(handlePosition.y()));
}

GUI_TEST_CLASS_DEFINITION(test_2951) {
    GTLogTracer lt;
    // 1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Click the "Scripting mode" tool button -> Show scripting options.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Show scripting options"}));
    GTWidget::click(GTAction::button(GTAction::findActionByText("Scripting mode")));

    // 3. Create the workflow: "Read sequence" -> "Write sequence".
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    // 4. Set the input sequence: _common_data/fasta/abcd.fa.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/abcd.fa");

    // 5. Set the correct output sequence.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Sequence"));
    GTMouseDriver::click();
    QString outFile = GUrl(sandBoxDir + "test_2951.gb").getURLString();
    GTUtilsWorkflowDesigner::setParameter("Output file", outFile, GTUtilsWorkflowDesigner::textValue);

    // 6. Edit a script for "Sequence count limit":
    //     printToLog("test");
    //     1
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsDialog::waitForDialog(new ScriptEditorDialogFiller("", "printToLog(\"test message\");\n1"));
    GTUtilsWorkflowDesigner::setParameterScripting("Sequence count limit", "user script");

    // Expected: the result file contains only the first sequence from the input file; there is a message "test" in the script details log.
    GTWidget::click(GTAction::button("Run workflow"));
    GTUtilsTaskTreeView::waitTaskFinished();
    bool printed = lt.hasMessage("test message");
    CHECK_SET_ERR(printed, "No message in the log");

    GTUtilsProject::openFile(outFile);
    QModelIndex objIdx = GTUtilsProjectTreeView::findIndex("d");
    QTreeView* tree = GTUtilsProjectTreeView::getTreeView();
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

    GTLogTracer lt;
    ADVSingleSequenceWidget* seqWidget =
        GTUtilsProject::openFileExpectSequence(
            testDir + "_common_data/scenarios/_regression/2924",
            "human_T1_cutted.fa",
            "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTUtilsCv::cvBtn::click(seqWidget);

    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(seqWidget), "Unexpected state of CV widget!");

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"}, GTGlobals::UseMouse);

    seqWidget = GTUtilsProject::openFileExpectSequence(
        testDir + "_common_data/scenarios/_regression/2924",
        "human_T1_cutted.fa",
        "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTUtilsCv::cvBtn::click(seqWidget);
    CHECK_SET_ERR(GTUtilsCv::cvBtn::isChecked(seqWidget), "Unexpected state of CV button!");
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(seqWidget), "Unexpected state of CV widget!");

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2962_2) {
    //    Open "_common_data/fasta/DNA.fa".
    //    Remove the second sequence object from the document.
    //    Click the "Toggle circular views" button on the main toolbar.
    //    Expected state: the circular view is toggled it is possible.
    //    Current state: UGENE crashes.

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QPoint p = GTUtilsProjectTreeView::getItemCenter("GXL_141618");
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();

    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsCv::commonCvBtn::click();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_2971) {
    /*  1. Open "COI.aln".
    2. Context menu -> Add -> Sequence from file...
    3. Choose a file with a large sequence (~50mb), e.g human chr21.
    Expected: UGENE does not crash.
*/
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTAction::button("Show overview"));  // needed to swith off rendering overview
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/2971", "hg18_21.fa"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    // Close file - UGENE does not crash.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_2972) {
    //    1. Open "samples/FASTA/human_T1.fa".
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Click the menu Tools -> HMMER tools -> HMM3 -> Search with HMM3 phmmer.
    GTUtilsDialog::waitForDialog(new UHMM3PhmmerDialogFiller(dataDir + "samples/Newick/COI.nwk"));
    GTMenu::clickMainMenuItem({"Tools", "HMMER tools", "Search with phmmer..."});

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");
    QString error = lt.getJoinedErrorString();
    QString expectedError = isOsWindows()
                                ? "Task {Search with phmmer} finished with error: Subtask {PHMMER search tool} is failed"
                                : "is empty or misformatted";

    CHECK_SET_ERR(error.contains(expectedError), "actual error is " + error);
    //    3. Choose the query sequence file: any non-sequence format file (e.g. *.mp3).
    //    4. Click search.
    //    Expected state: the search task fails, UGENE does not crash.
}

GUI_TEST_CLASS_DEFINITION(test_2975) {
    //    1. Open "_common_data/fasta/all_alphabet.fa" as multiple alignment.
    GTUtilsProject::openMultiSequenceFileAsMalignment(testDir + "_common_data/fasta", "all_and_raw_alphabets.fa");

    //    2. Open the "Pairwise alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::PairwiseAlignment);

    //    3. Set the first sequence as the first sequence and the second sequence as the second sequence, select the "Smith-Waterman" algorithm.
    //    Expected state: align button is blocked
    GTUtilsOptionPanelMsa::addFirstSeqToPA("Dna");
    GTUtilsOptionPanelMsa::addSecondSeqToPA("Raw");
    GTUtilsOptionPanelMsa::setPairwiseAlignmentAlgorithm("Smith-Waterman");

    QPushButton* alignButton = GTUtilsOptionPanelMsa::getAlignButton();
    CHECK_SET_ERR(nullptr != alignButton, "Align button is NULL");
    CHECK_SET_ERR(!alignButton->isEnabled(), "Align button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_2981) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "/samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Click a "Build Tree" button on the main toolbar.
    GTUtilsDialog::add(new BuildTreeDialogFiller(sandBoxDir + "test_3276_COI.wnk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: a "Build Phyligenetic Tree" dialog appears.

    //    3. Set any acceptable path and build a tree with default parameters.
    //    Expected state: the tree is shown in the MSA Editor.

    //    4. Click a "Layout" button on the tree view toolbar, select a "Circular" menu item.
    auto layoutCombo = GTWidget::findComboBox("layoutCombo");
    GTComboBox::selectItemByText(layoutCombo, "Circular");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the tree becomes circular.
    auto treeView = GTWidget::findGraphicsView("treeView");
    int initW = treeView->rect().width();

    //    5. Hide/show a project view.
    GTKeyboardDriver::keyClick('1', Qt::AltModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick('1', Qt::AltModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the tree size is not changed.
    int finalW = treeView->rect().width();
    CHECK_SET_ERR(finalW == initW, QString("Tree widget width does not match: initial: %1, final: %2").arg(initW).arg(finalW));
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
    GTLogTracer lt;

    GTFileDialog::openFile(testDir + "_common_data/fasta", "RAW2.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QDir().mkpath(sandBoxDir + "test_3305");
    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(true, "<auto>", "misc_feature", "complement(1..5)", sandBoxDir + "test_2987/test_2987.gb"));
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "create_annotation_action"));

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "ep_exportAnnotations2CSV"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "test_2987/test_2987", ExportAnnotationsFiller::csv, false));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("test_2987.gb"));
    GTMouseDriver::click(Qt::RightButton);

    QFile csvFile(sandBoxDir + "test_2987/test_2987");
    CHECK_SET_ERR(!(csvFile.exists() && csvFile.size() == 0), "An empty file exists");
}

GUI_TEST_CLASS_DEFINITION(test_2991) {
    /*  1. Open file _common_data/alphabets/extended_amino_1000.fa
    Expected state: Alphabet of opened sequence must be [amino ext]
*/
    GTFileDialog::openFile(testDir + "_common_data/alphabets/", "extended_amino_1000.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto w = GTWidget::findWidget("ADV_single_sequence_widget_0");
    auto label = GTWidget::findLabel("nameLabel", w);
    CHECK_SET_ERR(label->text().contains("[amino ext]"), QString("Unexpected label of sequence name: %1, must contain %2").arg(label->text()).arg("[amino ext]"));
}

GUI_TEST_CLASS_DEFINITION(test_2998) {
    // 1. Open human_T1.fa
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Find any pattern
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTTTTAAAAA");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: the task is finished without errors.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
