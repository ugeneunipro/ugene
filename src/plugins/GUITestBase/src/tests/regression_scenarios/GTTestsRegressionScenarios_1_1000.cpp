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

#include "GTTestsRegressionScenarios_1_1000.h"
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

#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotationsTreeView.h>
#include <U2View/DetView.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>

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
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTClipboard.h"
#include "system/GTFile.h"
#include "utils/GTThread.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_regression_scenarios {

GUI_TEST_CLASS_DEFINITION(test_0057_1) {
    // Crash on a number of multisequence files opening in the merge mode
    // 1. Open samples/Genbank/sars.gb.
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click the right mouse button on NC_001363 annotations tree view and select menu item "Find qualifier"
    // Expected state : Opened "Find Qualifier" dialog.
    // 3. Enter to Name field 'db_xref' and click "Next" button
    FindQualifierFiller::FindQualifierFillerSettings settings("db_xref", QString(), false, true, 1, false, false, false);
    GTUtilsDialog::waitForDialog(new FindQualifierFiller(settings));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Find qualifier..."}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("NC_004718 features [sars.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : Found first qualifier with name db_xref and value GI:30124074
    QList<QTreeWidgetItem*> selectedItems = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    CHECK_SET_ERR(2 == selectedItems.size(), "Unexpected number of selected items");
    const QString qualifierName = selectedItems[1]->data(0, Qt::DisplayRole).toString();
    CHECK_SET_ERR("db_xref" == qualifierName, "Unexpected qualifier name");
    const QString qualifierType = selectedItems[1]->data(1, Qt::DisplayRole).toString();
    CHECK_SET_ERR(qualifierType.isEmpty(), "Qualifier unexpectedly has a type");
    const QString qualifierValue = selectedItems[1]->data(2, Qt::DisplayRole).toString();
    CHECK_SET_ERR("GI:30124074" == qualifierValue, "Unexpected qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0057_2) {
    // Crash on a number of multisequence files opening in the merge mode
    // 1. Open samples/Genbank/sars.gb.
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click on right mouse button on NC_001363 annotations tree view and select menu item "Find qualifier"
    // Expected state : Opened "Find Qualifier" dialog.
    // 3. Enter to Name field 'db_xref' and click "Select All" button
    FindQualifierFiller::FindQualifierFillerSettings settings("db_xref", QString(), false, true);
    GTUtilsDialog::waitForDialog(new FindQualifierFiller(settings));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Find qualifier..."}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("NC_004718 features [sars.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : Selected 58 qualifiers with name db_xref
    QList<QTreeWidgetItem*> selectedItems = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    int qualifiersCount = 0;
    const QString expectedQualifierName = "db_xref";

    foreach (QTreeWidgetItem* item, selectedItems) {
        auto qualifierItem = dynamic_cast<AVQualifierItem*>(item);
        if (nullptr != qualifierItem) {
            qualifiersCount++;
            const QString qualifierName = item->data(0, Qt::DisplayRole).toString();
            CHECK_SET_ERR(expectedQualifierName == qualifierName, QString("Unexpected qualifier name: expect '%1', got '%2'").arg(expectedQualifierName).arg(qualifierName));
        }
    }

    CHECK_SET_ERR(58 == qualifiersCount, "Unexpected number of selected items");
}

GUI_TEST_CLASS_DEFINITION(test_0057_3) {
    // Crash on a number of multisequence files opening in the merge mode
    // 1. Open samples / Genbank / murine.gb.
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click on right mouse button on NC_001363 annotations tree view and select menu item "Find qualifier"
    // Expected state : Opened "Find Qualifier" dialog.
    // 3. Enter to Value field 'pol polyprotein fragment' and click "Next" button
    FindQualifierFiller::FindQualifierFillerSettings settings(QString(), "pol polyprotein fragment", false, true, 1, false, false);
    GTUtilsDialog::waitForDialog(new FindQualifierFiller(settings));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Find qualifier..."}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("NC_001363 features [murine.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state : Founded first qualifier with name 'product' and value 'pol polyprotein fragment'
    QList<QTreeWidgetItem*> selectedItems = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    CHECK_SET_ERR(2 == selectedItems.size(), "Unexpected number of selected items");
    const QString qualifierName = selectedItems[1]->data(0, Qt::DisplayRole).toString();
    CHECK_SET_ERR("product" == qualifierName, "Unexpected qualifier name");
    const QString qualifierType = selectedItems[1]->data(1, Qt::DisplayRole).toString();
    CHECK_SET_ERR(qualifierType.isEmpty(), "Qualifier unexpectedly has a type");
    const QString qualifierValue = selectedItems[1]->data(2, Qt::DisplayRole).toString();
    CHECK_SET_ERR("pol polyprotein fragment" == qualifierValue, "Unexpected qualifier value");
}

GUI_TEST_CLASS_DEFINITION(test_0057_4) {
    //    Crash on a number of multi-sequence files opening in the merge mode
    //    1. Open samples/Genbank/murine.gb.
    //    2. Click on right mouse button on NC_001363 annotations tree view and select menu item "Find qualifier"
    //    Expected state: Opened "Find Qualifier" dialog.
    //    3. Enter to Name and Value fields 'protein' and 'NP_5', then click "Select All" button
    //    Expected state: Founded 2 qualifiers:
    //    protein_id - NP_597742.2
    //    protein_id - NP_597744.1

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* featuresItem = GTUtilsAnnotationsTreeView::findItem("NC_001363 features [murine.gb]");
    FindQualifierFiller::FindQualifierFillerSettings settings("protein", "NP_5", false, false);
    settings.selectAll = true;

    GTUtilsDialog::waitForDialog(new FindQualifierFiller(settings));
    GTUtilsDialog::waitForDialog(new PopupChooser({"find_qualifier_action"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(featuresItem);
    GTThread::waitForMainThread();

    QStringList qualValues;
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    for (QTreeWidgetItem* i : qAsConst(items)) {
        auto item = dynamic_cast<AVItem*>(i);
        CHECK_SET_ERR(item != nullptr, "AvItem is NULL");
        if (item->type == AVItemType_Qualifier) {
            auto avQualifierItem = (AVQualifierItem*)item;
            qualValues << avQualifierItem->qValue;
        }
    }

    CHECK_SET_ERR(qualValues.size() == 2, "Incorrect qualifiers count");
    CHECK_SET_ERR(qualValues.contains("NP_597742.2"), "NP_597742.2 qualifier was not selected");
    CHECK_SET_ERR(qualValues.contains("NP_597744.1"), "NP_597744.1 qualifier was not selected");
}

GUI_TEST_CLASS_DEFINITION(test_0057_5) {
    //    Crash on a number of multi-sequence files opening in the merge mode
    //    1. Open samples/Genbank/murine.gb.
    //    2. Click on right mouse button on NC_001363 annotations tree view and select menu item "Find qualifier"
    //    Expected state: Opened "Find Qualifier" dialog.
    //    3. Enter to Name and Value fields 'protein' and 'NP_5'. Also set checkbox to 'Exact match', then click "Next" button
    //    Expected state: Showed message box with information about reaults are not found.

    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* featuresItem = GTUtilsAnnotationsTreeView::findItem("NC_001363 features [murine.gb]");

    FindQualifierFiller::FindQualifierFillerSettings settings("protein",
                                                              "NP_5",
                                                              true,
                                                              true,
                                                              1,
                                                              false,
                                                              true,
                                                              false);
    GTUtilsDialog::waitForDialog(new FindQualifierFiller(settings));
    GTUtilsDialog::waitForDialog(new PopupChooser({"find_qualifier_action"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(featuresItem);
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    foreach (QTreeWidgetItem* i, items) {
        auto item = dynamic_cast<AVItem*>(i);
        CHECK_SET_ERR(item != nullptr, "AvItem is NULL");
        CHECK_SET_ERR(item->type == AVItemType_Group, "There are items selected");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0057_6) {
    //    Crash on a number of multisequence files opening in the merge mode
    //    1. Open samples/Genbank/murine.gb.
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click on right mouse button on NC_001363 annotations tree view and select menu item "Find qualifier"
    //    Expected state: Opened "Find Qualifier" dialog.
    //    3. Enter to Name field 'source', Value field should be empty. Click "Next" button
    //    Expected state: a result is found.
    //    4. Click "Next" button again.
    //    Expected state: Showed message box with information about the end of tree is reached.

    QTreeWidgetItem* featuresItem = GTUtilsAnnotationsTreeView::findItem("NC_001363 features [murine.gb]");

    FindQualifierFiller::FindQualifierFillerSettings settings("organism",
                                                              "",
                                                              true,
                                                              true,
                                                              2,
                                                              true,
                                                              false,
                                                              true);
    GTUtilsDialog::waitForDialog(new FindQualifierFiller(settings));
    GTUtilsDialog::waitForDialog(new PopupChooser({"find_qualifier_action"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(featuresItem);
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    bool qualifierIsSelected = false;
    foreach (QTreeWidgetItem* item, items) {
        qualifierIsSelected |= (nullptr != dynamic_cast<AVQualifierItem*>(item));
    }
    CHECK_SET_ERR(qualifierIsSelected, "No qualifiers are selected");
}

GUI_TEST_CLASS_DEFINITION(test_0073_1) {
    /* 1) Open "Find substrings in sequences" WD sample
     * 2) Click on "Find substrings" element
     *   Expected state: "Qualifier name for pattern name" element parameter presents
     */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Find substrings in sequences");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Find Substrings");

    QString patternName = GTUtilsWorkflowDesigner::getParameter("Qualifier name for pattern name");  // pattern_name

    CHECK_SET_ERR(patternName == QString("pattern_name"), "Parameter 'Qualifier name for pattern name' has wrong default value");
}
GUI_TEST_CLASS_DEFINITION(test_0073_2) {
    /* 1) Open "Find patterns" WD sample
     * 2) Click on "Find Pattern" element
     *   Expected state: "Qualifier name for pattern name" element parameter presents
     */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Find patterns");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsWorkflowDesigner::click("Find Pattern");

    QString patternName = GTUtilsWorkflowDesigner::getParameter("Qualifier name for pattern name");  // pattern_name

    CHECK_SET_ERR(patternName == QString("pattern_name"), "Parameter 'Qualifier name for pattern name' has wrong default value");
}

GUI_TEST_CLASS_DEFINITION(test_0339) {
    //    UGENE-339, MacOs only

    // Open samples/FASTA/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Press Ctrl+Shift+F hotkey replaced with Cmd+Shift+F on Mac
    // Expected: Smith-Waterman dialog appears
    // Close the dialog
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("SmithWatermanDialogBase", QDialogButtonBox::Cancel));
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('f', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Press Ctrl+N hotkey replaced with Cmd+N on Mac
    // Expected: New annotation dialog appears
    // Close the dialog
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("CreateAnnotationDialog", QDialogButtonBox::Cancel));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Press Ctrl+F hotkey replaced with Cmd+F on Mac
    // Expected: Search pattern panel appears
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    bool isTabOpened = GTUtilsOptionPanelSequenceView::isTabOpened(GTUtilsOptionPanelSequenceView::Search);
    CHECK_SET_ERR(isTabOpened, "'Search in sequence' tab is not opened");
}

GUI_TEST_CLASS_DEFINITION(test_0394) {
    //    1. Add to project sars.gb, murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open sars.gb in sequence view.
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    3. Drag and drop murine.gb sequence NC_001363 to opened in previous step sequence view.
    GTUtilsProjectTreeView::dragAndDrop(GTUtilsProjectTreeView::findIndex("NC_001363"), GTUtilsMdi::activeWindow()->findChild<ADVSingleSequenceWidget*>());
    //    Expected state: Two sequnces showed in sequence view and NC_001363 is active.
    int num = GTUtilsSequenceView::getSeqWidgetsNumber();
    CHECK_SET_ERR(num == 2, QString("Unexpected sequence number: %1").arg(num));
    //    Order of seaqences is:
    //        1) NC_004718 [dna]
    QString first = GTUtilsSequenceView::getSeqName(0);
    CHECK_SET_ERR(first == "NC_004718", "Unexpected first sequence: " + first);
    //        2) NC_001363 [dna]
    QString second = GTUtilsSequenceView::getSeqName(1);
    CHECK_SET_ERR(second == "NC_001363", "Unexpected second sequence: " + second);
    // TODO: good drag and drop needed
    //     4. Drag sequence NC_001363 to up and drop.
    //         1) NC_001363 [dna]
    //         2) NC_004718 [dna]
}

GUI_TEST_CLASS_DEFINITION(test_0407) {
    // 1. Open _common_data/scenarios/_regression/407/trail.fas
    // Expected state: a message box appears
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/407/trail.fas");
}

GUI_TEST_CLASS_DEFINITION(test_0490) {
    // 1. Select "Tools > Multiple alignment > [any item]"
    // 2. In the dialog appeared select _common_data/scenarios/_regression/490/fasta-example.fa
    // 3. Click "Align"
    // Expected stat:  UGENE not crashes

    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/490/fasta-example.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new KalignDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0567) {
    //    1. Open samples/human_T1.fa
    //    2. Press button "build dotplot" on toolbar.
    //    Expected: "Build dotplot" dialog is opened.
    //    3. Press button "Load sequence".
    //    4. Open File _common_data/scenarios/dp_view/dpm1.fa
    //    5. Press button "Load sequence"
    //    6. Open File _common_data/scenarios/dp_view/dpm2.fa
    //    Expected: UGENE does not crash.

    class Test_0567 : public Filler {
    public:
        Test_0567()
            : Filler("DotPlotDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/dp_view/dpm1.fa"));
            GTWidget::click(GTWidget::findPushButton("loadSequenceButton", dialog));

            GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/dp_view/dpm2.fa"));
            GTWidget::click(GTWidget::findPushButton("loadSequenceButton", dialog));

            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button != nullptr, "cancel button is NULL");
            GTWidget::click(button);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new Test_0567());
    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));

    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0574) {
    // 1. Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select in menu Actions->Cloning->Create fragment...
    // 3. Set any region, don`t include overhangs, click OK
    GTUtilsDialog::waitForDialog(new CreateFragmentDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"Cloning", "Create Fragment"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Select newly created fragment
    GTUtilsAnnotationsTreeView::selectItemsByName({"Fragment (1-5833)"});

    // 5. Select in menu Actions->Cloning->Construct molecule...
    class Scenario : public CustomScenario {
    public:
        void run() {
            // 6. In dialog select your fragment in "Available fragments", click Add
            GTWidget::click(GTWidget::findWidget("takeAllButton"));

            // 7. Select your fragment in "New molecule contents", click Edit
            auto tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget("molConstructWidget"));
            GTTreeWidget::click(GTTreeWidget::findItem(tree, "Blunt"));

            // 8. For the left end: select Overhang, check Custom Overhang, select 5'-3', write "AA" (without the quotes) to the 5'-3' edit
            // 9. For the right end: select Overhang, check Custom Overhang, select 3'-5', write "CC" (without the quotes) to the 3'-5' edit
            EditFragmentDialogFiller::Parameters p;
            p.lSticky = true;
            p.lCustom = true;
            p.lDirect = true;
            p.lDirectText = "AA";
            p.rSticky = true;
            p.rCustom = true;
            p.rDirect = false;
            p.rComplText = "CC";
            GTUtilsDialog::waitForDialog(new EditFragmentDialogFiller(p));
            GTWidget::click(GTWidget::findWidget("editFragmentButton"));

            // 10. Click OK, open edit dialog for this fragment again.
            // Expected state: 3'-5' edit for the right end contains "CC" (bug: it contains "GG")
            p.checkRComplText = true;
            GTUtilsDialog::waitForDialog(new EditFragmentDialogFiller(p));
            GTTreeWidget::click(GTTreeWidget::findItem(tree, "AA (Fwd)"));
            GTWidget::click(GTWidget::findWidget("editFragmentButton"));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Construct molecule..."});
}

GUI_TEST_CLASS_DEFINITION(test_0587) {
    class CheckBowtie2Filler : public Filler {
    public:
        CheckBowtie2Filler()
            : Filler("BuildIndexFromRefDialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto methodNamesBox = GTWidget::findComboBox("methodNamesBox", dialog);
            for (int i = 0; i < methodNamesBox->count(); i++) {
                if (methodNamesBox->itemText(i) == "UGENE Genome Aligner") {
                    GTComboBox::selectItemByIndex(methodNamesBox, i);
                }
            }

            GTFileDialogUtils* ob = new GTFileDialogUtils(testDir + "_common_data/genbank/", "NC_014267.1_cut.gb");
            GTUtilsDialog::waitForDialog(ob);
            GTWidget::click(GTWidget::findWidget("addRefButton", dialog));
            GTUtilsTaskTreeView::waitTaskFinished();

            ob = new GTFileDialogUtils(sandBoxDir, "587_NC_014267.1_cut", GTFileDialogUtils::Save);
            GTUtilsDialog::waitForDialog(ob);
            GTWidget::click(GTWidget::findWidget("setIndexFileNameButton", dialog));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new CheckBowtie2Filler());
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Build index for reads mapping..."});
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0597) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open graph.
    auto sequenceWidget = GTWidget::findWidget("ADV_single_sequence_widget_0");
    GTUtilsDialog::waitForDialog(new PopupChooser({"Karlin Signature Difference"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction", sequenceWidget));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Save cutoffs as annotations and check the expected result.
    class SaveGraphCutoffsDialogFiller : public Filler {
    public:
        SaveGraphCutoffsDialogFiller()
            : Filler("SaveGraphCutoffsDialog") {
        }
        void commonScenario() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto newTablePath = GTWidget::findLineEdit("leNewTablePath", dialog);
            GTLineEdit::setText(newTablePath, sandBoxDir + "test_0597");

            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* okButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != nullptr, "OK button is NULL");
            GTWidget::click(okButton);
        }
    };

    GTUtilsDialog::waitForDialog(new SaveGraphCutoffsDialogFiller());
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "save_cutoffs_as_annotation"}));
    GTMenu::showContextMenu(GTWidget::findWidget("GSequenceGraphViewRenderArea"));
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::findItem("graph_cutoffs  (0, 4)");
}

GUI_TEST_CLASS_DEFINITION(test_0598) {
    // 1. Open file '_common_data/fasta/Mycobacterium.fna'
    GTFileDialog::openFile(testDir + "_common_data/fasta", "Mycobacterium.fna");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto sequenceWidget = GTWidget::findWidget("ADV_single_sequence_widget_0");

    // 2. Show DNA Flexibility graph
    // Expected state: 'Calculate graph points' task is started
    auto graphAction = GTWidget::findWidget("GraphMenuAction", sequenceWidget);
    GTUtilsDialog::waitForDialog(new PopupChooser({"DNA Flexibility"}));
    GTWidget::click(graphAction);
    GTUtilsTask::waitTaskStart("Calculate graph points", 30000);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Zoom graph
    // Expected state: cached data is used and 'Calculate graph points' task is not started
    GTWidget::click(GTAction::button("action_zoom_in_gi|119866057|ref|NC_008705.1| Mycobacterium sp. KMS, complete genome"));
    CHECK_SET_ERR(GTUtilsTaskTreeView::getTopLevelTasksCount() == 0, "'Calculate graph points' task is started, but cached data should be used");

    // 4. Use context menu {Graph -> Graph settings...}
    // Expected state: 'Graph Settings' dialog is appeared
    // 5. Set parameters: window = 1000, step = 4
    // 6. Press 'Ok'
    // Expected state: 'Calculate graph points' task is started.
    GTUtilsDialog::waitForDialog(new GraphSettingsDialogFiller(200, 100));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    QWidget* graphView = GTUtilsSequenceView::getGraphView();
    GTWidget::click(graphView, Qt::RightButton);
    GTUtilsTaskTreeView::checkTaskIsPresent("Calculate graph points");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0605) {
    //    Crash on a number of multisequence files opening in the merge mode

    //    1. Open _common_data/scenarios/dp_view/EF591300.gb.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/dp_view/EF591300.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open _common_data/scenarios/dp_view/dpm1.fa.
    //    3. Choose the "Merge sequences into single sequence to show in sequence viewer" option with any gap size in the "Sequence reading option" dialog.
    GTUtilsProject::openMultiSequenceFileAsMergedSequence(testDir + "_common_data/scenarios/dp_view/dpm1.fa");

    //    4. Open _common_data/scenarios/dp_view/dpm2.fa.
    //    5. Choose the same option.
    GTUtilsProject::openMultiSequenceFileAsMergedSequence(testDir + "_common_data/scenarios/dp_view/dpm2.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0610) {
    GTLogTracer lt;

    //    1. Open any MSA, i.e. samples/ClustalW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Click on any letter (it will be selected with dash rectangle)
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(1, 1));

    //    3. Click on white area below sequences (there will be no selection)
    // this step is deprecated: it will cause selecting a symbol in the last row.
    GTUtilsMSAEditorSequenceArea::cancelSelection();

    //    4.  Modify MSA: align with any algorithm
    //    Expected state: UGENE not crashed
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "align_with_kalign"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new KalignDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0627) {
    //    1. Open _common_data/fasta/fa1.fa.
    //    Expected state: the file opens in the sequence viewer.
    //    2. Click the {Primer3} button on the toolbar.
    //    Expected state: Primer designer dialog appeares.
    //    3. Check all fields except the list below for tooltips presence.
    //    List of exceptions:
    //        Region defenition on all tags;
    //        "Reset Form" and "Pick Primers" buttons on all tags;
    //        Main tag (tag has not tooltip):
    //            Pick left primer checkbox
    //            left primer lineedit below checkbox
    //            Pick hybridization probe checkbox
    //            olygo lineedit below checkbox
    //            Pick right primer checkbox
    //            right primer lineedit below checkbox
    //        General settings tag:
    //            Liberal base checkbox
    //            Show debug info checkbox
    //            Do not treat ambiguity codes in libraries as consensus checkbox
    //            Lowercase masking checkbox
    //        Internal Oligo tag: nothing
    //        Penalty Weights: nothing
    //        Span Intron/Exon: nothing
    //        Sequence quality: nothing
    //        Resul Settings: all fields and buttons
    //    Expected state: tooltips are presented.

    class ToolTipsChecker : public Filler {
    public:
        ToolTipsChecker()
            : Filler("Primer3Dialog") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            QList<QObject*> children = findAllChildren(dialog);
            QList<QString> objsWithoutTooltips;
            for (QObject* obj : qAsConst(children)) {
                bool lineEditSpinBoxOrComboBox = qobject_cast<QLineEdit*>(obj) != nullptr ||
                                                 qobject_cast<QSpinBox*>(obj) != nullptr ||
                                                 qobject_cast<QComboBox*>(obj) != nullptr;
                bool widgetWithoutToolTip = qobject_cast<QWidget*>(obj) != nullptr &&
                                            qobject_cast<QWidget*>(obj)->toolTip().isEmpty();
                if (lineEditSpinBoxOrComboBox && widgetWithoutToolTip && obj->objectName() != "qt_spinbox_lineedit") {
                    objsWithoutTooltips << obj->objectName();
                }
            }

            QList<QString> exceptions;
            exceptions << "start_edit_line"
                       << "end_edit_line"
                       << "region_type_combo"
                       << "edit_SEQUENCE_PRIMER"
                       << "edit_SEQUENCE_PRIMER_REVCOMP"
                       << "edit_SEQUENCE_INTERNAL_OLIGO"
                       << "cbExistingTable"
                       << "cbAnnotationType"
                       << "leNewTablePath"
                       << "leDescription"
                       << "leftOverlapSizeSpinBox"
                       << "rightOverlapSizeSpinBox"
                       << "leGroupName"
                       << "leAnnotationName";

            for (const QString& name : objsWithoutTooltips) {
                CHECK_SET_ERR(exceptions.contains(name), QString("The following field has no tool tip: %1").arg(name));
            }

            GTWidget::close(dialog);
        }

    private:
        QList<QObject*> findAllChildren(QObject* obj) {
            QList<QObject*> children;
            foreach (QObject* o, obj->children()) {
                children << o;
                children << findAllChildren(o);
            }
            return children;
        }
    };

    GTFileDialog::openFile(testDir + "_common_data/fasta/fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* primer3 = GTAction::button("primer3_action");
    CHECK_SET_ERR(primer3 != nullptr, "primer3_action not found");

    GTUtilsDialog::waitForDialog(new ToolTipsChecker());
    GTWidget::click(primer3);
}

GUI_TEST_CLASS_DEFINITION(test_0652) {
    // 1) Open /data/samples/fasta/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2) Open /data/samples/gff/5prime_utr_intron_A20.gff
    GTFileDialog::openFile(dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3) Drag and drop first annotation from gff file to sequence view of human_T1
    // 4) On question "Found annotations that are out of sequence range, continue?" answer "Yes"
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject("Ca20Chr1 features");

    // 5) In annotations tree view open added annotation and put the mouse cursor over this annotation.
    GTUtilsAnnotationsTreeView::findItem("5_prime_UTR_intron");
}

GUI_TEST_CLASS_DEFINITION(test_0659) {
    //    Write annotations worker is broken
    //    1. Open WD. Create simple scheme "read sequence"->"Write annotations"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Set "GenBank" as output document format for "Write annotations" worker.
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Annotations", true);
    GTUtilsWorkflowDesigner::connect(read, write);

    //    3. Set up valid output input files for scheme, and run it (for example set input file samples/GENBANK/sars.gb)
    GTUtilsWorkflowDesigner::click(read);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsWorkflowDesigner::click(write);
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_659", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: annotations has been written in single file
    QDir d(QDir(sandBoxDir).absolutePath());
    QStringList list = d.entryList({"test_659*"});
    CHECK_SET_ERR(list.count() == 1, QString("unexpected files number: %1").arg(list.count()));
    CHECK_SET_ERR(list.first() == "test_659", "unexpected file name: " + list.first());

    GTWidget::click(GTAction::button("toggleDashboard"));
    // check csv format
    GTUtilsWorkflowDesigner::click(write);
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_659_1", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Document format", "CSV", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: annotations has been written in single file
    list = d.entryList({"test_659*"});
    CHECK_SET_ERR(list.count() == 2, QString("unexpected files number: %1").arg(list.count()));
    CHECK_SET_ERR(list.contains("test_659_1"), "unexpected file name csv: " + list[1]);

    GTWidget::click(GTAction::button("toggleDashboard"));
    // check gff format
    GTUtilsWorkflowDesigner::click(write);
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_659_2", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("Document format", "GFF", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: annotations has been written in single file
    list = d.entryList({"test_659*"});
    CHECK_SET_ERR(list.count() == 3, QString("unexpected files number: %1").arg(list.count()));
    CHECK_SET_ERR(list.contains("test_659_2"), "unexpected file name csv: " + list[2]);
}

GUI_TEST_CLASS_DEFINITION(test_0666) {
    /* Crash on removing some annotations from Primer3 result
     * 1. Open samples\FASTA\human_T1.fa
     * 2. Search about 50 primers
     * 3. Try delete 10 or more results annotations
     *   Expected state: UGENE not crashes
     */
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/regression/666/", "regression_0666.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    QModelIndex projectTreeItem = GTUtilsProjectTreeView::findIndex("Annotations");

    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(projectTreeItem, GTUtilsSequenceView::getPanOrDetView());
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsAnnotationsTreeView::selectItemsByName(
        {
            "pair 1  (0, 2)",
            "pair 10  (0, 2)",
            "pair 11  (0, 2)",
            "pair 12  (0, 2)",
            "pair 13  (0, 2)",
            "pair 14  (0, 2)",
            "pair 15  (0, 2)",
            "pair 16  (0, 2)",
            "pair 17  (0, 2)",
            "pair 18  (0, 2)",
            "pair 19  (0, 2)",
        });
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_0677) {
    // 1. Open the file bamExample.bam.
    // 2. Check the box 'Import unmapped reads' and import the file.
    // Expected state: UGENE not crashed

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_0677/test_0677.ugenedb", "", "", true));
    GTFileDialog::openFile(testDir + "_common_data/bam/", "1.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}
GUI_TEST_CLASS_DEFINITION(test_0678) {
    // 1. Open samples/PDB/1CF7.pdb
    // 2. Navigate in annotation tree, unfolding following items: {1CF7 chain 1 annotation <97> chain_info (0, 1) <97> chain_info}
    // Expected state: UGENE not crashes

    GTFileDialog::openFile(dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::selectItemsByName({"chain_info"});
}

GUI_TEST_CLASS_DEFINITION(test_0680) {
    // 1. Open /data/sample/PDB/1CRN.pdb
    // Expected state: Sequence is opened
    // 2. Do context menu "Analyze - Predict Secondary Structure"
    // Expected state: Predict Secondary Structure dialog is appeared
    // 3. Set "Range Start" 1, "Range End": 2, set any prediction algorithm
    // 4. Press "Start prediction" button
    // UGENE not crashes
    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new PredictSecondaryStructureDialogFiller(1, 2, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ANALYSE, "Predict secondary structure"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0681) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/681", "seq.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 9));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, ADV_COPY_TRANSLATION_ACTION}, GTGlobals::UseKey));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QString text = GTClipboard::text();
    CHECK_SET_ERR(text == "TRC", "Sequcence part translated to <" + text + ">, expected TRC");
}

GUI_TEST_CLASS_DEFINITION(test_0684) {
    //    1. Open samples/Genbank/sars
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select region 2000-9000
    GTUtilsSequenceView::selectSequenceRegion(2000, 9000);

    //    3. Activate "Clonning->Create fragment"

    class CreateFragmentScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    The opened dialog should have the selected region by default.
            GTComboBox::checkCurrentValue(GTWidget::findComboBox("region_type_combo", dialog), "Selected region");
            GTLineEdit::checkText(GTWidget::findLineEdit("start_edit_line", dialog), "2000");
            GTLineEdit::checkText(GTWidget::findLineEdit("end_edit_line", dialog), "9000");

            //    4. Set left overhang "AATT", forward
            GTGroupBox::setChecked("leftEndBox", dialog);
            GTRadioButton::click(GTWidget::findRadioButton("lDirectButton", dialog));
            GTLineEdit::setText(GTWidget::findLineEdit("lCustomOverhangEdit", dialog), "AATT");

            //       Set right overhang "AATT", reverse-complement
            GTGroupBox::setChecked("rightEndBox", dialog);
            GTRadioButton::click(GTWidget::findRadioButton("rComplButton", dialog));
            GTLineEdit::setText(GTWidget::findLineEdit("rCustomOverhangEdit", dialog), "AATT");

            //       Click OK
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateFragmentDialogFiller(new CreateFragmentScenario));
    GTMenu::clickMainMenuItem({"Actions", "Cloning", "Create fragment..."});

    //    5. Activate "Cloning->Construct molecule"

    class ConstructMoleculeScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //       Add created fragment.
            GTListWidget::click(GTWidget::findListWidget("fragmentListWidget", dialog), "NC_004718 (sars.gb) Fragment (2000-9000) [core length - 7001 bp]");
            GTWidget::click(GTWidget::findWidget("takeButton", dialog));

            //       Set "Make circular" option selected
            GTCheckBox::setChecked(GTWidget::findCheckBox("makeCircularBox", dialog));

            //       The overhangs should be highlighted in green
            auto item = GTTreeWidget::findItem(GTWidget::findTreeWidget("molConstructWidget", dialog), "NC_004718 (sars.gb) Fragment (2000-9000) [core length - 7001 bp]", nullptr, 1);

            const QColor color1 = item->textColor(0);
            const QColor color2 = item->textColor(2);
            const QColor expectedColor = Qt::green;

            CHECK_SET_ERR(expectedColor == color1, QString("An unexpected item text color in column 0: expect '%1', got '%2'").arg(expectedColor.name()).arg(color1.name()));
            CHECK_SET_ERR(expectedColor == color2, QString("An unexpected item text color in column 2: expect '%1', got '%2'").arg(expectedColor.name()).arg(color2.name()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(new ConstructMoleculeScenario));
    GTMenu::clickMainMenuItem({"Actions", "Cloning", "Construct molecule..."});
}

GUI_TEST_CLASS_DEFINITION(test_0685) {
    // 1. Do menu tools->Blast Search (ext. tools must be configured)
    // 2. Set next parameters:
    // {Select input file} _common_data\scenarios\external_tools\blast\SequenceLength_00003000.txt
    // {Select search} blastp
    // Expected state: UGENE not crashes
    BlastLocalSearchDialogFiller::Parameters blastParams;
    blastParams.runBlast = true;
    blastParams.programNameText = "blastp";
    blastParams.withInputFile = true;
    blastParams.inputPath = testDir + "_common_data/scenarios/external_tools/blast/SequenceLength_00003000.txt";
    blastParams.dbPath = testDir + "_common_data/cmdline/external-tool-support/blastplus/human_T1/human_T1.nhr";
    GTUtilsDialog::waitForDialog(new BlastLocalSearchDialogFiller(blastParams));
    GTMenu::clickMainMenuItem({"Tools", "BLAST", "BLAST search..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0688) {
    //    1. Open any sequence
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. do context menu {Analyzq->Search TBFS with matrices}

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    3. In opened dialog, select any matrix and press Search button
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "position_weight_matrix/JASPAR/fungi/MA0265.1.pfm"));
            GTWidget::click(GTWidget::findWidget("pbSelectModelFile", dialog));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished();

            //    Expected state: selected matrix still shown
            const QString expectedPath = QFileInfo(dataDir + "position_weight_matrix/JASPAR/fungi/MA0265.1.pfm").canonicalFilePath();
            GTLineEdit::checkText(GTWidget::findLineEdit("modelFileEdit", dialog), expectedPath);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new PwmSearchDialogFiller(new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find TFBS with matrices");
}

GUI_TEST_CLASS_DEFINITION(test_0700) {
    /* Selecting "Cancel" in the "Import BAM file" dialog causes an error (UGENE-700)
     * 1. Open a _common_data/scenarios/assembly/example-alignment.bam
     * 2. Click "Cancel".
     *   Expected state: UGENE not crashed
     */
    class CancelScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(new CancelScenario()));
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/assembly/example-alignment.bam");
}

GUI_TEST_CLASS_DEFINITION(test_0702) {
    // 1. open _common_data / fasta / DNA.fa in merge mode.
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select first contig(1..743) region.
    GTUtilsAnnotationsTreeView::selectItemsByName({"contig"});

    // 3. Do context menu{ Export->export sequence of selected annotations).
    // 4. Fill next fields in appeared dialog, and execute it :
    //{export to file} -D : / test / _common_data / fasta / DNA_annotation.fastq
    //{ file format to use } -FASTQ
    //{ Save document to the project } -checked
    //{ Save as separare sequences } -checked.
    // Expected state : UGENE not crashed
    GTUtilsDialog::waitForDialog(new ExportSequenceOfSelectedAnnotationsFiller(sandBoxDir + "1.fa", ExportSequenceOfSelectedAnnotationsFiller::Fastq, ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export", "Export sequence of selected annotations..."}));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0703) {
    GTFile::copy(dataDir + "samples/Assembly/chrM.fa", sandBoxDir + "1.fa");

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "1.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(sandBoxDir + "1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::activateWindow("chrM [1.ugenedb]");

    GTUtilsProjectTreeView::dragAndDrop(GTUtilsProjectTreeView::findIndex("chrM", GTUtilsProjectTreeView::findIndex("1.fa")), GTUtilsMdi::activeWindow());

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTUtilsDocument::removeDocument("1.fa");
    GTUtilsDocument::removeDocument("1.ugenedb");

    GTFile::removeDir(sandBoxDir + "1.fa");

    // 1) Opened a BAM file that had a reference sequence associated with it, but there shouldn't be such sequence anymore.
    GTFileDialog::openFile(sandBoxDir + "1.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : UGENE not crashes
}

GUI_TEST_CLASS_DEFINITION(test_0733) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 1. Drop "Write sequence" element on the scheme
    // Expected state: workflow designer opens, one empty write sequence element is presented.
    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::addElement("Write Sequence");

    // 2. Select this element.
    GTUtilsWorkflowDesigner::click(writer);

    // Expected state: Property editor appears, {parameters->Document format} is "fasta"
    QString format = GTUtilsWorkflowDesigner::getParameter("Document format");
    CHECK_SET_ERR("FASTA" == format, "Wrong format");

    // 3. Change {Parameters->Output file} to "result.gb".
    // Expected state: {Parameters->Output file} is "%some_path%/result.gb", {Parameters->Document format} is "fasta".
    GTUtilsWorkflowDesigner::setParameter("Output file", "result.gb", GTUtilsWorkflowDesigner::textValue);

    // 4. Change {Parameters->Document Format} to "GenBank" and press Enter.
    GTUtilsWorkflowDesigner::setParameter("Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);

    // Expected state: {Parameters->Output file} changes to "%some_path%/result.gb".
    QString url = GTUtilsWorkflowDesigner::getParameter("utput file");
    CHECK_SET_ERR("result.gb" == url, "Wrong url");
}

GUI_TEST_CLASS_DEFINITION(test_0734) {
    // 1. Open "_common_data/fasta/test.TXT".
    GTUtilsProject::openFile(testDir + "_common_data/fasta/test.TXT");

    // 2. Open "_common_data/clustal/test_alignment.aln".
    GTUtilsProject::openFile(testDir + "_common_data/clustal/test_alignment.aln");

    // Expected state: two documents are opened in the project view; MSA Editor are shown with test_alignment.
    GTUtilsProjectTreeView::findIndex("test.TXT");  // checks are inside
    GTUtilsProjectTreeView::findIndex("test_alignment.aln");  // checks are inside
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 3. Drag'n'drop "Sequence4" object of "test.TXT" document from the project tree to the MSA Editor.
    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_LOAD, "Sequence from current project"}));
    GTUtilsDialog::add(new ProjectTreeItemSelectorDialogFiller("test.TXT", "Sequence4"));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: UGENE does not crash; a new "Sequence4" row appears in the alignment.
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 4, QString("Sequence count mismatch. Expected: %1. Actual: %2").arg(4).arg(names.size()));
    CHECK_SET_ERR(names.last() == "Sequence4",
                  QString("Inserted sequence name mismatch. Expected: %1. Actual: %2").arg("Sequence4").arg(names.last()));
}
GUI_TEST_CLASS_DEFINITION(test_0746) {
    // 1. Open file _common_data\scenarios\_regression\764\short.fa
    // 2. Make 'Detailed view' visible (if not)
    // Expected state: 'Show amino translation' and 'Complement strand' buttons are enabled (not grey)
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/764/short.fa");
    auto toggleViewButton = GTWidget::findWidget("toggleViewButton");
    Runnable* chooser = new PopupChooser({"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(chooser);
    GTWidget::click(toggleViewButton);

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));

    QAbstractButton* complement = GTAction::button("complement_action");
    CHECK_SET_ERR(complement->isEnabled() == true, "button is not enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0750) {
    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2. Enable the scripting mode (select "Actions > Scripting mode > Show scripting options").
    GTUtilsDialog::waitForDialog(new PopupChooser({"Show scripting options"}));
    GTWidget::click(GTAction::button(GTAction::findActionByText("Scripting mode")));
    //    3. Drag the "Read from remote database" element to the Scene.
    GTUtilsWorkflowDesigner::addElement("Read Sequence from Remote Database");
    //    4. Press the "..." button of the "Resource IDs" value cell.

    class Custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findButtonByText("Cancel", dialog));
        }
    };
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("", QDialogButtonBox::Ok, new Custom()));
    auto table = GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTTableView::getCellPoint(table, 2, 1));
    GTMouseDriver::click();

    GTWidget::click(GTWidget::findButtonByText("...", table));
    //    5. Close the appeared dialog.
    CHECK_SET_ERR(!table->selectionModel()->hasSelection(), "Selection unexpectidly presents");
    //    Expected state: script cell didn't selected
}

GUI_TEST_CLASS_DEFINITION(test_0762) {
    // 1. Open human_T1.fa from examples
    //
    // 2. Try search tandems with default settings and with new Annotations Table.
    // Expected state: UGENE not crashes
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new FindTandemsDialogFiller(sandBoxDir + "test_0762.gb"));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find tandem repeats");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0768) {
    //    1. Open WD. Press toolbar button "Create script object".
    //    Expected state: Create element with script dialog appears.

    //    2. Fill the next field in dialog:
    //        {Name} 123

    //    3. Click OK button.

    //    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    auto w = GTWidget::findTreeWidget("WorkflowPaletteElements");

    QTreeWidgetItem* foundItem = nullptr;
    QList<QTreeWidgetItem*> outerList = w->findItems("", Qt::MatchContains);
    for (int i = 0; i < outerList.count(); i++) {
        QList<QTreeWidgetItem*> innerList;

        for (int j = 0; j < outerList.value(i)->childCount(); j++) {
            innerList.append(outerList.value(i)->child(j));
        }

        foreach (QTreeWidgetItem* item, innerList) {
            QString s = item->data(0, Qt::UserRole).value<QAction*>()->text();
            if (s == "test_0768") {
                foundItem = item;
            }
        }
    }
    if (foundItem != nullptr) {
        GTUtilsWorkflowDesigner::setCurrentTab(GTUtilsWorkflowDesigner::algorithms);

        GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove"}));
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "", "Remove element"));
        GTUtilsWorkflowDesigner::clickOnPalette("test_0768", Qt::RightButton);

        GTUtilsMdi::click(GTGlobals::Close);

        GTUtilsWorkflowDesigner::openWorkflowDesigner();
    }

    GTUtilsDialog::waitForDialog(new CreateElementWithScriptDialogFiller("test_0768"));
    GTMenu::clickMainMenuItem({"Actions", "Create element with script..."});

    //    4. Select created worker. Press toolbar button "Edit script text".
    //    Expected state: Script editor dialog appears.

    //    5. Paste "xyz" at the script text area. Click "Check syntax" button
    //    Expected state: messagebox "Syntax is OK!" appears.

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("test_0768"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new ScriptEditorDialogSyntaxChecker("xyz", "Syntax is OK!"));
    GTMenu::clickMainMenuItem({"Actions", "Edit script of the element..."});
}

GUI_TEST_CLASS_DEFINITION(test_0774) {
    //    1. Create new scheme in Workflow Designer: "Read sequence" > "Write sequence".
    //    2. Input two files in the "Input files" parameter of the "Read sequence" element.
    //    3. Select "Merge" in the "Mode" parameter of the "Read sequence" element.
    //    4. Set location of an output data file in the "Output file" parameter of the "Write sequence".
    //    5. Run the schema.
    //    Expected result: The scheme finished successfully.
    //    6. Open the result file in Sequence View.
    //    Expected result: The result file contains two sequences. Each sequence is merged sequences from
    //    each input file respectively.

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    CHECK_SET_ERR(read != nullptr, "Read Sequence element not found");
    //    GTUtilsWorkflowDesigner::setDatasetInputFolder(dataDir + "samples/Genbank");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsWorkflowDesigner::setParameter("Mode", "Merge", GTUtilsWorkflowDesigner::comboValue);

    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence");
    CHECK_SET_ERR(write != nullptr, "Write Sequence element not found");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_0774", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(read, write);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(sandBoxDir + "/test_0774");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsSequenceView::getSeqWidgetsNumber() == 2, "Incorrect count of sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0775) {
    //    1. Open human_T1.fa sequence
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("1..2,5..10,15..20"));
    // GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(10, 20));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    //    2. Open "Region selection" dialog {Ctrl+a} fill it with next data:
    //        {Multiple range selection} checked
    //        {Multi region:} 1..2,5..10,15..20

    //    3. Press 'Go' button
    //    Expected state: this regions are selected on the view
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy sequence"}));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);

    QString text = GTClipboard::text();
    CHECK_SET_ERR(text == "TTCAGATTAAAGTT", "unexpected selection: " + text);
}

GUI_TEST_CLASS_DEFINITION(test_0776) {
    /* 1. Open WD.
     * 2. Create a scheme with the "Search for TFBS with weight matrix" element.
     * 3. Run the scheme with following parameters: input sequence - human_t1, input weight matrix - position_weight_matrix/UniPROBE/Cell08/Alx3_3418.2.pwm.
     *   Expected state: error report "Bad sequence supplied to Weight Matrix Search" doesn't appear
     */
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    WorkflowProcessItem* readWM = GTUtilsWorkflowDesigner::addElement("Read Weight Matrix");
    GTUtilsWorkflowDesigner::setParameter("Input file(s)", QDir(dataDir).absolutePath() + "/position_weight_matrix/UniPROBE/Cell08/Alx3_3418.2.pwm", GTUtilsWorkflowDesigner::textValue);

    WorkflowProcessItem* search = GTUtilsWorkflowDesigner::addElement("Search for TFBS with Weight Matrix");
    // Search for TFBS with Weight Matrix

    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(read, search);
    GTUtilsWorkflowDesigner::connect(readWM, search);
    GTUtilsWorkflowDesigner::connect(search, write);

    GTUtilsWorkflowDesigner::click(write);
    GTUtilsWorkflowDesigner::setParameter("Document format", "GenBank", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_0776.gb", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0778) {
    //    1. Open file _common_data\scenarios\_regression\778\mixed.fa
    //    2. Open menu {Settings->Preferences}
    //    Expected state: Application settings dialog opened
    //    3. Select File format section, set {Create annotations for case switchings:} "Upper case annotation" and press OK button
    //    Expected state: sequence view reopened, sequence marked with two "upper_case" annotations with coordinates 1..4, 8..10
    //    4. Open menu {Settings->Preferences}
    //    Expected state: Application settings dialog opened
    //    5. Select File format section, set {Create annotations for sequence case switch regions:} "Lower case annotation" and press OK button
    //    Expected state: sequence view reopened, sequence marked with two "upper_case" annotations with coordinates 5..7, 11..13

    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/778/mixed.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class CaseAnnotations : public CustomScenario {
    public:
        CaseAnnotations(const QString& _name)
            : name(_name) {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::FileFormat);

            auto caseCombo = GTWidget::findComboBox("caseCombo");
            GTComboBox::selectItemByText(caseCombo, name);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

    private:
        QString name;
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CaseAnnotations("Use upper case annotations")));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    CHECK_SET_ERR(regions.size() == 2, "Annotated regions number is incorrect");
    CHECK_SET_ERR(regions.contains(U2Region(0, 4)), "No annotation 1..4");
    CHECK_SET_ERR(regions.contains(U2Region(7, 3)), "No annotation 8..10");

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CaseAnnotations("Use lower case annotations")));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    regions = GTUtilsAnnotationsTreeView::getAnnotatedRegions();
    CHECK_SET_ERR(regions.size() == 2, "Annotated regions number is incorrect");
    CHECK_SET_ERR(regions.contains(U2Region(4, 3)), "No annotation 1..4");
    CHECK_SET_ERR(regions.contains(U2Region(10, 3)), "No annotation 8..10");
}

GUI_TEST_CLASS_DEFINITION(test_0779) {
    // 1.Create a simple scheme with two elements:
    // "Read sequence" element and "Write annotations" element.
    // 2. Connect the elements.
    // 3. Switch the "File format" property of the "Write annotations" element from "GenBank" (defualt) to "csv".
    // 4. Click on the scheme area and there will be a crash.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Annotations", true);
    GTUtilsWorkflowDesigner::setParameter("Document format", "CSV", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Write Annotations"));
}

GUI_TEST_CLASS_DEFINITION(test_0782) {
    //    1. Open file data/samples/FASTA/human_T1.fa in sequence view.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Build graph - {Graphs -> GC content (%)}.
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    //    3. Press right mouse button in the graph area, choose {Graph -> Graph settings...}.
    auto graphView = GTWidget::findWidget("GSequenceGraphViewRenderArea");
    GTWidget::click(graphView);
    QImage init = GTWidget::getImage(graphView);
    // init.save("/home/vmalin/init", "BMP");
    class custom : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findButtonByText("Cancel", dialog));
        }
    };
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("GraphSettingsDialog", QDialogButtonBox::Cancel, new custom));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    GTWidget::click(graphView, Qt::RightButton);
    //    4. In "Graph Settings" dialog change graph's color, then press "Cancel".
    GTWidget::click(graphView);
    //    Expected result: Graph's color didn't change.
    QImage final = GTWidget::getImage(graphView);
    // final.save("/home/vmalin/final", "BMP");

    CHECK_SET_ERR(final == init, "graph view changed");
    //    5. Repeat the third step, then check "Cutoff for minimum and maximum values".

    //    6. Try to apply dialog with different values, in fields "Minimum" and "Maximum".

    //    Expected result: Cutoff graph is drawn correctly.
}

GUI_TEST_CLASS_DEFINITION(test_0786) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::setParameter("Accession filter", "NC_001363", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::click("Write Sequence");
    QFile outputFile(sandBoxDir + "786_out.fa");
    QString outputFilePath = QFileInfo(outputFile).absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter("Output file", outputFilePath, GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!outputFile.exists(outputFilePath), "File should not exist");
}

GUI_TEST_CLASS_DEFINITION(test_0792) {
    //    1) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    2) Put "Read Sequence" worker on the scheme
    GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    //    Expected state: Dataset view opened

    //    3) Click "Add folder", select data/samples/Genbank
    const QString inputDir = QFileInfo(dataDir + "samples/Genbank").absoluteFilePath();
    QDir().mkpath(inputDir);
    GTUtilsWorkflowDesigner::setDatasetInputFolder(inputDir);

    //    4) Click on appeared item in the file list
    auto datasetWidget = GTWidget::findWidget("DatasetWidget");
    auto items = GTWidget::findListWidget("itemsArea", datasetWidget);
    GTListWidget::click(items, "Genbank", Qt::RightButton);
    //    Expected state:
    //        the following widgets appears:
    GTWidget::findLineEdit("includeMaskEdit");
    GTWidget::findLineEdit("excludeMaskEdit");
    GTWidget::findCheckBox("recursiveBox");
    //            Include mask, Exclude mask lineedits;
    //            Recursive checkbox
    GTWidget::click(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_0798) {
    //    1. File -> Open As.
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge));
    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("GenBank"));
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/798", "1.gb"));
    GTMenu::clickMainMenuItem({"File", "Open as..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Choose the file "_common_data/scenarios/_regression/798/1.gb" as Genbank in merging mode with default parametes.
    QList<QTreeWidgetItem*> list = GTUtilsAnnotationsTreeView::findItems("source");
    QString s1 = list.first()->text(2);
    QString s2 = list.last()->text(2);
    CHECK_SET_ERR(s1 == "1..117046", "unexpected coordinates1: " + s1);
    CHECK_SET_ERR(s2 == "117057..143727", "unexpected coordinates2: " + s2);
    //    Expected state: there is 2 annotations named 'source' with coordinates 1..117046 and 117057..143727
}

GUI_TEST_CLASS_DEFINITION(test_0801) {
    // 1. Open human_T1.fa sequence
    // 2. Open find pattern tab on options panel {Ctrl+f}. Fill fields with next data:
    // {Search for} AAAGCTTTA
    // {Region} Custom region 2 5
    // Expected state: UGENE does not crash}
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionsPanel::runFindPatternWithHotKey("AAAGCTTTA");
    GTUtilsOptionPanelSequenceView::setRegionType("Custom region");
    GTLineEdit::setText(GTWidget::findLineEdit("editStart"), "2");
    GTLineEdit::setText(GTWidget::findLineEdit("editEnd"), "5");
}

GUI_TEST_CLASS_DEFINITION(test_0807) {
    QString somenameEtcFile = sandBoxDir + "807.etc";
    QFile::copy(testDir + "_common_data/scenarios/workflow designer/somename.etc", somenameEtcFile);

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::removeCmdlineWorkerFromPalette("somename");

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(somenameEtcFile));
    GTWidget::click(GTAction::button("AddElementWithCommandLineTool"));

    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.tool = "bedtools";
    settings.command = "testtest $in";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));

    GTUtilsDialog::waitForDialog(new PopupChooser({"editConfiguration"}));
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("somename"));
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    class ResetAndApplyScenario : public CustomScenario {
    public:
        void run() {
            auto nextButton = GTWidget::findWidget("__qt__passive_wizardbutton1");
            GTWidget::click(nextButton);
            GTWidget::click(GTWidget::findWidget("pbDeleteInput"));
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Reset));
            GTWidget::click(GTWidget::findButtonByText("Finish"));

            // Same wizard is shown now. The first page is opened.
            nextButton = GTWidget::findWidget("__qt__passive_wizardbutton1");
            GTWidget::click(nextButton);
            GTWidget::click(GTWidget::findWidget("pbDeleteInput"));
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Apply));
            GTWidget::click(GTWidget::findButtonByText("Finish"));
        }
    };
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(new ResetAndApplyScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooser({"editConfiguration"}));
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("somename"));
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    class ApplyScenario : public CustomScenario {
    public:
        void run() {
            auto nextButton = GTWidget::findWidget("__qt__passive_wizardbutton1");
            GTWidget::click(nextButton);
            GTWidget::click(GTWidget::findWidget("pbDeleteInput"));
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTWidget::click(nextButton);
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Apply"));
            GTWidget::click(GTWidget::findButtonByText("Finish"));
        }
    };

    // Expected state: element disappeared from the scene.
    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getWorkers().isEmpty(), "The worker is not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_0808) {
    // 1. Create a scheme with the "Read sequence" element and the "Write sequence" element, human_t1.da as an input sequence
    // 2. Set the "Split sequence" option of the "Write sequence" element to 2
    // 3. Run the scheme

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), GTUtilsWorkflowDesigner::getWorker("Write Sequence"));

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::addInputFile("Read Sequence", dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Split sequence", "2", GTUtilsWorkflowDesigner::spinValue);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0812) {
    // 1. Create a "seq.txt" file in <some_path> location.
    // 2. Click "Create element with external tool".
    // 3. Input a name.
    // 4. Specify a slot.
    // 5. There is no need to add a parameter.
    // 6. Input the following execution string: "copy <some_path>\seq.txt <some_path>\seq2.txt" (change <some_path> to your location).
    // or the following execution string: "copy "<some_path>\seq.txt" "<some_path>\seq3.txt"" (with quotes for paths).
    // 7. Execute the schema (that contains this element only).
    // => The schema executes successfully.
    // 4. Verify whether the file has been copied.

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "Element_0812";

    QList<CreateElementWithCommandLineToolFiller::InOutData> input;
    CreateElementWithCommandLineToolFiller::InOutDataType inOutDataType;
    inOutDataType.first = CreateElementWithCommandLineToolFiller::Sequence;
    inOutDataType.second = "FASTA";
    input << CreateElementWithCommandLineToolFiller::InOutData("in1",
                                                               inOutDataType);
    settings.input = input;
    settings.command = "copy _common_data/scenarios/_regression/812/seq.txt _common_data/scenarios/_regression/812/seq2.txt";

    // GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Continue"));

    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    QAbstractButton* createElement = GTAction::button("createElementWithCommandLineTool");
    GTWidget::click(createElement);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Ok"));
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
}
GUI_TEST_CLASS_DEFINITION(test_0814) {
    //    1. Open UGENE preferences in main menu.
    //    Expected state: "Application settings" dialog appeared.
    //    2. Go to the {Logging} part.
    //    Expected state: logging settings were shown.
    //    3. Check the {Save output to file} checkbox, click OK button.
    //    Expected state: warning message appeared.
    //    4. Close warning message.
    //    Expected state: "Application settings" dialog closed.
    //    5. Repeat steps 1-3.
    //    Expected state: logging settings were shown, {Save output to file} checkbox is unchecked.
    //    6. Check the {Save output to file} checkbox, enter some file (manually or by select file dialog) and click OK button.
    //    Expected state: "Application settings dialog" closed without any messages.
    //    7. Find your file on on the disk.
    //    Expected state: log file exists.

    class LogFile_1 : public CustomScenario {
    public:
        LogFile_1(QString name = QString())
            : name(name) {
        }
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::Logging);

            auto fileOut = GTWidget::findCheckBox("fileOutCB");
            CHECK_SET_ERR(!fileOut->isChecked(), "CheckBox is checked!");
            GTCheckBox::setChecked(fileOut);

            auto fileName = GTWidget::findLineEdit("outFileEdit");
            GTLineEdit::setText(fileName, name);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

    private:
        QString name;
    };

    GTLogTracer lt;

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new LogFile_1()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new LogFile_1(QDir(sandBoxDir).absolutePath() + "test_0814_log")));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    CHECK_SET_ERR(GTFile::check(QDir(sandBoxDir).absolutePath() + "test_0814_log") == true, "Log file not found");
}

GUI_TEST_CLASS_DEFINITION(test_0821) {
    // 1. Open files samples/genbank/sars.gb and samples/genbank/murine.gb in merge mode
    // Expected state: annotations in both files has right coordinates

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(dataDir + "samples/Genbank/", {"sars.gb", "murine.gb"}));
    GTMenu::clickMainMenuItem({"File", "Open..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    // TODO: QList<U2Region> cds = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("contig (0, 2)");
    // TODO: CHECK_SET_ERR( cds.contains(U2Region(0, 5833)), "No 1..5833 region");
}

GUI_TEST_CLASS_DEFINITION(test_0828) {
    //     1. Open WD.
    //     2. Add to scheme "Sequence marker" element.
    //     3. In property editor groupbox "Parameters" press "Add" button.
    //     Expected state: "Create marker group" dialog appears, its combobox "Marker group type" contains field "Sequence name markers".
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addElement("Sequence Marker");
    GTUtilsWorkflowDesigner::click("Sequence Marker");

    class OkClicker : public Filler {
    public:
        OkClicker()
            : Filler("EditMarkerGroupDialog") {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget();
            auto combo = GTWidget::findComboBox("typeBox", w);
            GTComboBox::selectItemByText(combo, "Sequence name markers");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new OkClicker());
    GTWidget::click(GTWidget::findToolButton("addButton"));
}

GUI_TEST_CLASS_DEFINITION(test_0830) {
    // 1) Tools->DNA assembly->Config assembly with CAPS3
    // 2) Base ->
    //     Input Files:
    //         _common_data/scenarios/CAP3/region2.fa
    //         _common_data/scenarios/CAP3/region4.fa
    //     Run
    QString outUrl = sandBoxDir + "830.ace";
    QFile(outUrl).remove();
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, outUrl));
    GTUtilsDialog::waitForDialog(new CAP3SupportDialogFiller({testDir + "_common_data/scenarios/CAP3/region2.fa", testDir + "_common_data/scenarios/CAP3/region4.fa"}, outUrl));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Reads de novo assembly (with CAP3)..."});

    // 3) wait for task error, ensure that no output files are in the project
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkNoItem("830.ugenedb");
}

GUI_TEST_CLASS_DEFINITION(test_0834) {
    GTFileDialog::openFile(testDir + "_common_data/genbank/NC_014267.1_cut.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("gene", "gene  (0, 1)") == "join(1..74213,77094..140426)",
                  "Annotation \"gene\" has incorrect location");
}

GUI_TEST_CLASS_DEFINITION(test_0835) {
    // 1. Open, for example, "murine.gb" and "sars.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the sequence objects in one Sequence View.
    //  = > Both sequences are shown in one Sequence View.
    QModelIndex idx = GTUtilsProjectTreeView::findIndex("NC_001363");
    QWidget* seqView = GTUtilsMdi::activeWindow();
    CHECK_SET_ERR(seqView != nullptr, "Sequence widget not found");

    GTUtilsProjectTreeView::dragAndDrop(idx, seqView);

    // 3. Enable Circular View for one of the sequences(for example, "murine.gb").
    //  = > The Circular View with the Restriction Site Map is shown.
    GTWidget::click(GTWidget::findWidget("CircularViewAction", GTWidget::findWidget("views_tool_bar_NC_004718")));

    auto restrictionMapTreeWidget = GTWidget::findWidget("restrictionMapTreeWidget");
    CHECK_SET_ERR(nullptr != restrictionMapTreeWidget && restrictionMapTreeWidget->isVisible(),
                  "Restriction map widget isn't visible unexpectedly");

    // 4. Delete the sequence with the Circular View from the Project View.
    //  = > The sequence has been deleted, the Circular View is not shown.
    //!= > The Restriction Site Map is NOT still shown.
    GTUtilsProjectTreeView::click("sars.gb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTThread::waitForMainThread();

    restrictionMapTreeWidget = GTWidget::findWidget("restrictionMapTreeWidget", nullptr, {false});
    CHECK_SET_ERR(restrictionMapTreeWidget == nullptr, "Restriction map widget is visible unexpectedly");
}

GUI_TEST_CLASS_DEFINITION(test_0839) {
    // 1. Use menu {Tools->Weight matrix->Build Weight Matrix}.
    // Expected state: "Build weight or frequency matrix" dialog appeared.
    //
    // 2. Click {...} button for "Input item".
    // Expected state: "Select file with alignment" dialog appeared.
    //
    // 3. Open any non msa file (e.g. a tree file  - *.nwk format).
    // Expected state:
    // 1). UGENE does not crash.
    // 2). Messagebox  about unsupported format appeared.
    QList<PwmBuildDialogFiller::Action> actions;
    actions << PwmBuildDialogFiller::Action(PwmBuildDialogFiller::ExpectInvalidFile, "");
    actions << PwmBuildDialogFiller::Action(PwmBuildDialogFiller::SelectInput, dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    actions << PwmBuildDialogFiller::Action(PwmBuildDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(new PwmBuildDialogFiller(actions));

    GTMenu::clickMainMenuItem({"Tools", "Search for TFBS", "Build weight matrix..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0840) {
    //"Digest Into Fragments" displays number of cuts incorrectly
    // When enzyme is presented both in auto-annotation and in permanent annotation, the number of cuts shows +1.

    class DigestCircularSequenceScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            auto availableEnzymeWidget = GTWidget::findListWidget("availableEnzymeWidget", dialog);
            availableEnzymeWidget->findItems("EcoRV : 1 cut(s)", Qt::MatchExactly);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    class CreateAnnotationScenario : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            GTLineEdit::setText("leAnnotationName", "EcoRV", dialog);
            GTLineEdit::setText("leGroupName", "enzyme", dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 1. Open "_common_data/enzymes/bsai_reverse_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/enzymes/bsai_reverse_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Find restriction sites" on toolbar.
    // 3. Click OK button in dialog appeared.
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller({"EcoRV"}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getSeqWidgetByNumber());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: One new auto-annotation appears ("enzyme/EcoRV")
    // 4. Select this annotation.
    QList<QTreeWidgetItem*> items1 = GTUtilsAnnotationsTreeView::findItems("EcoRV");
    CHECK_SET_ERR(items1.size() == 1, "1. Unexpected annotation count: " + QString::number(items1.size()));
    GTTreeWidget::doubleClick(items1[0]);  // Select annotation & sequence under annotation.

    // 5. Select "New annotation" in context menu.
    // 6. Fill fields with: "Group name" - "enzyme", "Annotation name" - "EcoRV".
    // 7. Click Create button.
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new CreateAnnotationScenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "New annotation");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Non-auto annotation appears ("enzyme/EcoRV") for the same region.
    QList<QTreeWidgetItem*> items2 = GTUtilsAnnotationsTreeView::findItems("EcoRV");
    CHECK_SET_ERR(items2.size() == 2, "2. Unexpected annotation count: " + QString::number(items2.size()));
    CHECK_SET_ERR(items2[0]->parent() != items2[1]->parent(), "Annotations are siblings unexpectedly");

    // 8. Select "Actions"->"Cloning"->"Digest into Fragments..." in main menu.
    // Expected state: there is only one enzyme in the "Available enzymes" field with 1 cut.
    // Bug state: there is one enzyme but with 2 cuts.
    GTUtilsDialog::add(new PopupChooserByText({"Cloning", "Digest into fragments..."}));
    GTUtilsDialog::add(new DigestSequenceDialogFiller(new DigestCircularSequenceScenario));
    GTMenu::showContextMenu(GTUtilsSequenceView::getSeqWidgetByNumber());
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0842) {
    //    1) Create some custom cmdline worker with some name ("test", for example).
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::setCurrentTab(GTUtilsWorkflowDesigner::algorithms);

    QTreeWidgetItem* treeItem = GTUtilsWorkflowDesigner::findTreeItem("test", GTUtilsWorkflowDesigner::algorithms, true, false);
    if (treeItem != nullptr) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "", "Remove element"));
        GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove"}));
        GTTreeWidget::click(treeItem);
        GTMouseDriver::click(Qt::RightButton);
    }
    QTreeWidgetItem* treeItem1 = GTUtilsWorkflowDesigner::findTreeItem("test1", GTUtilsWorkflowDesigner::algorithms, true, false);
    if (treeItem1 != nullptr) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "", "Remove element"));
        GTUtilsDialog::waitForDialog(new PopupChooserByText({"Remove"}));
        GTTreeWidget::click(treeItem1);
        GTMouseDriver::click(Qt::RightButton);
    }

    const QStringList groupNames = GTUtilsWorkflowDesigner::getPaletteGroupNames();
    const int customElementsCount = groupNames.contains("Custom Elements with External Tools") ? GTUtilsWorkflowDesigner::getPaletteGroupEntries("Custom Elements with External Tools").size() : 0;

    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "test";
    settings.input << CreateElementWithCommandLineToolFiller::InOutData("in", CreateElementWithCommandLineToolFiller::InOutDataType(CreateElementWithCommandLineToolFiller::Sequence, "FASTA"));
    settings.command = "<My tool> $in";
    // GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Continue"));
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    2) Right click at this worker on the palette -> Edit.
    //    3) Set a new name for the worker ("test1", for example).
    //    4) Next -> Next -> Next -> Finish.
    //    5) "Remove this element?" -> Cancel.
    settings.elementName = "test1";
    settings.input.clear();
    // GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "You have changed the structure of the element"));
    // GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Continue", "You don't use listed parameters in template string"));
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));

    GTUtilsWorkflowDesigner::setCurrentTab(GTUtilsWorkflowDesigner::algorithms);

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit"}));
    GTUtilsWorkflowDesigner::clickOnPalette("test", Qt::RightButton);
    GTGlobals::sleep(5000);  // added to ensure that crash is not here or to fix this crash

    //    Expected state: There are no custom workers on the palette now
    const QList<QTreeWidgetItem*> customElements = GTUtilsWorkflowDesigner::getPaletteGroupEntries("Custom Elements with External Tools");
    CHECK_SET_ERR(customElementsCount + 1 == customElements.size(), QString("Unexpected custom elements count: expect %1, got %2").arg(customElementsCount + 1).arg(customElements.size()));

    //    6) Click at the test worker on the palette
    //    Expected state: UGENE not crashes.
    GTUtilsWorkflowDesigner::clickOnPalette("test1");
}

GUI_TEST_CLASS_DEFINITION(test_0844) {
    /* 1. Open "samples/human_t1".
     * 2. In advanced settings of Tandem Finder choose "Suffix array" (unoptimized algorithm)
     * 3. Start finding tandems
     *   Expected state: UGENE no crashes
     */
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTComboBox::selectItemByText(GTWidget::findComboBox("algoComboBox", dialog), "Suffix array");

            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget"), 1);

            GTLineEdit::setText("leNewTablePath", sandBoxDir + "test_0844.gb", dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new FindTandemsDialogFiller(new Scenario()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find tandem repeats");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0846) {
    //    1. Open "samples/human_t1".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Add any annotations;
    GTUtilsAnnotationsTreeView::createAnnotation("", "", "1..100");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Use popup menu {Export->Export annotations}
    //    4. Chose "csv" in combobox "File format"
    //    5. Click checkbox "Save sequence names"
    //    6. Click "OK"
    GTUtilsDialog::add(new PopupChooserByText({"Export", "Export annotations..."}, GTGlobals::UseKey));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "test_0846.csv", ExportAnnotationsFiller::csv, false));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: Annotations with sequence names were saved
    QString data = GTFile::readAll(sandBoxDir + "test_0846.csv");
    QString expectedSubstring = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    CHECK_SET_ERR(data.contains(expectedSubstring), "Sequence name not found in the result file");
}

GUI_TEST_CLASS_DEFINITION(test_0854) {
    /* 1. Open samples/genbank/PBR322.gb.
     * 2. Export the sequence into alignment using context menu in project tab.
     * 3. Align it with MUSCLE.
     *   Expected result: UGENE doesn't crash.
     */
    GTFileDialog::openFile(dataDir + "samples/Genbank/PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("PBR322.gb"));
    GTMouseDriver::click();
    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(new ExportSequenceAsAlignmentFiller(sandBoxDir, "test_0854.aln", ExportSequenceAsAlignmentFiller::Clustalw, true));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new MuscleDialogFiller(MuscleDialogFiller::Default));
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0858) {
    //    1. Open human_t1.fa
    //    2. Open the Statistics bar on the Options Panel
    //    3. Select and copy Characters Occurrence table
    //    4. Paste the table in a Text Editor
    //    Expected state: the table in the editor and in the Options Panel have the same content

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);

    auto label = GTWidget::findLabel("characters_occurrence_label");

    QString expected = "<table cellspacing=5><tr><td><b>A:&nbsp;&nbsp;</td><td>62 842 &nbsp;&nbsp;</td><td>"
                       "31.4%&nbsp;&nbsp;</td></tr><tr><td><b>C:&nbsp;&nbsp;</td><td>40 041 &nbsp;&nbsp;</td><td>20.0%&nbsp;"
                       "&nbsp;</td></tr><tr><td><b>G:&nbsp;&nbsp;</td><td>37 622 &nbsp;&nbsp;</td><td>18.8%&nbsp;&nbsp;</td>"
                       "</tr><tr><td><b>T:&nbsp;&nbsp;</td><td>59 445 &nbsp;&nbsp;</td><td>29.7%&nbsp;&nbsp;</td></tr></table>";
    QString got = label->text();
    CHECK_SET_ERR(got == expected, QString("The clipboard text is incorrect: [%1], expected [%2]").arg(got).arg(expected));
}

GUI_TEST_CLASS_DEFINITION(test_0861_1) {
    // 1. Open sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Annotations Highlighting" bar of the Options Panel
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    // Expected state : all annotation groups of sars.gb are in the "Annotation type" window:
    //  3'UTR, 5'UTR, CDS, comment, gene, mat_peptide, misc_feature, source
    auto highlightTree = GTWidget::findTreeWidget("OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(highlightTree->topLevelItemCount() == 8, "Unexpected number of annotations");
    CHECK_SET_ERR(highlightTree->topLevelItem(0)->text(0) == "3'UTR", QString("Unexpected annotation name at row %1").arg(0));
    CHECK_SET_ERR(highlightTree->topLevelItem(1)->text(0) == "5'UTR", QString("Unexpected annotation name at row %1").arg(1));
    CHECK_SET_ERR(highlightTree->topLevelItem(2)->text(0) == "CDS", QString("Unexpected annotation name at row %1").arg(2));
    CHECK_SET_ERR(highlightTree->topLevelItem(3)->text(0) == "comment", QString("Unexpected annotation name at row %1").arg(3));
    CHECK_SET_ERR(highlightTree->topLevelItem(4)->text(0) == "gene", QString("Unexpected annotation name at row %1").arg(4));
    CHECK_SET_ERR(highlightTree->topLevelItem(5)->text(0) == "mat_peptide", QString("Unexpected annotation name at row %1").arg(5));
    CHECK_SET_ERR(highlightTree->topLevelItem(6)->text(0) == "misc_feature", QString("Unexpected annotation name at row %1").arg(6));
    CHECK_SET_ERR(highlightTree->topLevelItem(7)->text(0) == "source", QString("Unexpected annotation name at row %1").arg(7));
}

GUI_TEST_CLASS_DEFINITION(test_0861_2) {
    // 1. Open sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Annotations Highlighting" bar of the Options Panel
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    auto highlightTree = GTWidget::findTreeWidget("OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(highlightTree->topLevelItemCount() == 8, QString("Unexpected number of annotations: %1").arg(highlightTree->topLevelItemCount()));

    // 3. Switch on ORFs auto annotation
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Show ORFs"}));
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected : "ORF" annotation appeared in the "Annotation type" window
    CHECK_SET_ERR(highlightTree->topLevelItemCount() == 9, QString("Unexpected number of annotations: %1").arg(highlightTree->topLevelItemCount()));
    CHECK_SET_ERR(highlightTree->topLevelItem(7)->text(0) == "orf", QString("Unexpected annotation name at row %1").arg(7));
}

GUI_TEST_CLASS_DEFINITION(test_0861_3) {
    // 1. Open sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Annotations Highlighting" bar of the Options Panel
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    auto highlightTree = GTWidget::findTreeWidget("OP_ANNOT_HIGHLIGHT_TREE");
    CHECK_SET_ERR(highlightTree->topLevelItemCount() == 8, "Unexpected number of annotations");

    // 3. Remove 3'UTR annotation group.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("3'UTR  (0, 1)"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : 3'UTR group disappeared from "Annotation type" window
    CHECK_SET_ERR(highlightTree->topLevelItemCount() == 7, "Unexpected number of annotations");
    CHECK_SET_ERR(highlightTree->topLevelItem(0)->text(0) == "5'UTR", QString("Unexpected annotation name at row %1").arg(0));
}

GUI_TEST_CLASS_DEFINITION(test_0861_4) {
    // 1. Open human_t1.fa
    // 2. Open the "Annotations Highlighting" bar of the Options Panel
    // Expected state: the message "The sequence doesn't have any annotations." is shown on the bar

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    auto noAnnotTypesLabel = GTWidget::findLabel("noAnnotTypesLabel");
    CHECK_SET_ERR(noAnnotTypesLabel->isVisible(), "label is not visible");
    CHECK_SET_ERR(noAnnotTypesLabel->text() == "The sequence doesn't have any annotations.",
                  "unexpected text: " + noAnnotTypesLabel->text());
}

GUI_TEST_CLASS_DEFINITION(test_0861_5) {
    //    1. Open sars.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Open the "Annotations Highlighting" bar of the Options Panel
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);
    auto editQualifiers = GTWidget::findLineEdit("editQualifiers");
    GTLineEdit::setText(editQualifiers, "aaaaaaaaaaa aaaaaaaaaaaa");
    QString style = editQualifiers->styleSheet();
    CHECK_SET_ERR(style == "background-color: rgb(255, 152, 142);", "unexpected styleSheet: " + style);
    //    3. Enter something with an incorrect symbol (eg "space")
    //    Expected state: the line editor is highlighted in red background-color: rgb(255, 152, 142);
}

GUI_TEST_CLASS_DEFINITION(test_0866) {
    // 1. Open WD
    // 2. Ad the following elements to the scheme: Read File URL(s), Write Plain Text 1, Write Plain Text 2
    // 3. Connect Read File URL(s) to both writers
    // Expected state: Read File URL(s) connected successfully

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::addElement("Read File URL(s)");
    WorkflowProcessItem* writer1 = GTUtilsWorkflowDesigner::addElement("Write Plain Text");
    GTUtilsWorkflowDesigner::addElement("Write Plain Text");

    GTUtilsWorkflowDesigner::connect(fileList, writer1);
    GTUtilsWorkflowDesigner::connect(fileList, GTUtilsWorkflowDesigner::getWorker("Write Plain Text 1"));
}

GUI_TEST_CLASS_DEFINITION(test_0868) {
    // Open chrM in Assembly Browser.
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.sorted.bam");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    // Zoom in multiple times to enable panning.
    for (int i = 0; i < 12; i++) {
        GTUtilsAssemblyBrowser::zoomIn();
    }

    // Add  a bookmark and activate it to ensure that view is aligned.
    GTUtilsBookmarksTreeView::addBookmark(GTUtilsMdi::activeWindow()->objectName(), "bookmark");
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter("bookmark"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished();  // Navigation starts a new task. Wait for it to finish.

    // Take the screenshot.
    auto readsArea = GTWidget::findWidget("assembly_reads_area");
    GTWidget::click(readsArea);  // Click on the center of render area to set a predefined hover state.
    QImage initialImage = GTWidget::getImage(readsArea);

    // Navigate to any other region.
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    GTUtilsTaskTreeView::waitTaskFinished();  // Navigation starts a new task. Wait for it to finish.

    // Activate the bookmark again.
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter("bookmark"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished();  // Navigation starts a new task. Wait for it to finish.

    // Expected state: the new view matches the initial.
    GTWidget::click(readsArea);  // Click on the center of render area to set a predefined hover state.
    QImage currentImage = GTWidget::getImage(readsArea);
    CHECK_SET_ERR(currentImage == initialImage, "The current view does not match the initial.");
}

GUI_TEST_CLASS_DEFINITION(test_0871) {
    // 1. Open WD
    // 2. Create a scheme with the following elements: Read Sequence, Amino Translation, Write Sequence
    // 3. Put humant_t1.fa as an input sequence
    // 4. Run the scheme
    // Expected state: the sequences that correspond to 3 translations frames of human_t1 are created

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");

    WorkflowProcessItem* amino = GTUtilsWorkflowDesigner::addElement("Amino Acid Translation", true);
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_0871.fa", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::connect(read, amino);
    GTUtilsWorkflowDesigner::connect(amino, write);
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsDashboard::clickOutputFile("test_0871.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::rename("human_T1 (UCSC April 2002 chr7:115977709-117855134) Amino translation 0 direct", "0");
    GTUtilsProjectTreeView::rename("human_T1 (UCSC April 2002 chr7:115977709-117855134) Amino translation 1 direct", "1");
    GTUtilsProjectTreeView::rename("human_T1 (UCSC April 2002 chr7:115977709-117855134) Amino translation 2 direct", "2");
}

GUI_TEST_CLASS_DEFINITION(test_0873) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addSample("Merge sequences and shift corresponding annotations");

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/sars.gb");

    //    3. fill all needed parameters and run schema
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: there are more then 10 result files and they are grouped into sublists

    QWidget* button = GTWidget::findButtonByText("merged.fa", GTUtilsDashboard::getDashboard());
    GTWidget::click(button);
    GTThread::waitForMainThread();

    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(sequenceLength == 35594, "Sequence length is " + QString::number(sequenceLength) + ", expected 35594");
}

GUI_TEST_CLASS_DEFINITION(test_0878) {
    //    1. Open several documents of any kind - sequence view, workflow designer, whatever.
    //    2. Activate any document except the #1 (as numbered in Window menu).
    //    3. Click on the Log button at bottom toolbar.
    //    Expected state: Log view is opened, selected document is shown.
    //    4. Click on the Log button again.
    //    Expected state: Log view is closed, selected document is active (not first).

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* win = GTUtilsMdi::activeWindow();
    CHECK_SET_ERR(win != nullptr, "Active window not found");

    GTWidget::click(GTWidget::findWidget("doc_label__dock_log_view"));
    CHECK_SET_ERR(win == GTUtilsMdi::activeWindow(), "Incorrect active window");
    GTWidget::click(GTWidget::findWidget("doc_label__dock_log_view"));
    CHECK_SET_ERR(win == GTUtilsMdi::activeWindow(), "Incorrect active window");
}

GUI_TEST_CLASS_DEFINITION(test_0882) {
    // 1. Open "data/samples/FASTQ/eas.fastq".
    // 2. Choose the "Align reads to reference sequence" option in appearing "Sequence reading option" dialog.
    // 3. Click "Ok" and specify a reference sequence in the "Align short reads" dialog (Use UGENE genome aligner).
    // 4. Click "Start" button.
    // Expected result: The dialog disappears, a notification about alignment results appears.
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Align));
    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(dataDir + "samples/FASTA/human_T1.fa", QStringList());
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok, "can't be mapped"));
    GTUtilsProject::openFileExpectNoProject(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0886) {
    // 1. Open file "_common_data/fasta/Gene.fa" in UGENE as a separate sequence or as a multiple alignment.
    // Expected result: UGENE doesn't crash, there is a warning report about non-imported sequences.

    // Check 'as a sequence' mode.
    GTLogTracer l1;
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/fasta/Gene.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList errors = l1.errorMessages;
    CHECK_SET_ERR(errors.size() == 1, "Wrong errors count 1 != " + QString::number(errors.size()) + ", errors: " + errors.join(","));

    // Cleanup.
    GTUtilsProjectTreeView::click("Gene.fa");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check 'as an alignment' mode.
    GTLogTracer l2;
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/Gene.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    errors = l2.errorMessages;
    CHECK_SET_ERR(errors.size() == 1, "Wrong errors count 1 != " + QString::number(errors.size()) + ", errors: " + errors.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_0888) {
    //    1. Open WD sample scheme "Convert seq/qual pair to Fastq"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Convert seq/qual pair to FASTQ");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    2. Press "save as" and save it to the valid location
    GTUtilsWorkflowDesigner::saveWorkflowAs(sandBoxDir + "test_0888.uwl", "test_0888");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    3. Press "save schema" 3 times
    for (int i = 0; i < 3; i++) {
        GTUtilsWorkflowDesigner::saveWorkflow();
    }
    GTUtilsTaskTreeView::waitTaskFinished();

    //    4. Open saved schema in text editor
    //    Expected state: only 8 first lines starts with '#'
    QFile file(sandBoxDir + "test_0888.uwl");
    file.open(QFile::ReadOnly);
    const QList<QByteArray> data = file.readAll().split('\n');

    int count = 0;
    foreach (const QByteArray& line, data) {
        if (line.startsWith("#")) {
            count++;
        }
    }

    CHECK_SET_ERR(8 == count, QString("Unexpected count of '#' symbols: expet %1, got %2").arg(8).arg(count));
}

GUI_TEST_CLASS_DEFINITION(test_0889) {
    // 64-bit OS supported only
#ifdef Q_OS_WIN32
    return;
#endif

    //  1) Open RNA.fa
    //  2) Use context menu on sequence {Align->Align sequence to mRNA}
    //  3) Select any item
    //  4) Click "Create"
    //
    //  Expected state: UGENE not crashed
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/RNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class OkClicker : public Filler {
    public:
        OkClicker()
            : Filler("CreateAnnotationDialog") {
        }
        void run() override {
            QWidget* w = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(w, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new OkClicker());
    GTUtilsDialog::waitForDialog(new ProjectTreeItemSelectorDialogFiller("RNA.fa", "Sequence"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ALIGN", "Align sequence to mRNA"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_0896) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::removeCmdlineWorkerFromPalette("SAMtools");

    GTUtilsDialog::add(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/896/_input", "SAMtools.etc"));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Discard));
    GTWidget::click(GTAction::button("AddElementWithCommandLineTool"));
    GTUtilsMdi::click(GTGlobals::Close);

    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/896/_input/url_out_in_exttool.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"editConfiguration"}));
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.tool = "SAMtools";
    settings.command = "%USUPP_SAMTOOLS% view -b -S -o '" + QDir(sandBoxDir).absolutePath() + "/test_0896out.bam' $sam";
    GTUtilsDialog::add(new CreateElementWithCommandLineToolFiller(settings));
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("SAMtools"));
    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    WorkflowProcessItem* samtools = GTUtilsWorkflowDesigner::getWorker("SAMtools", true);

    GTUtilsWorkflowDesigner::click(samtools);
    QTableWidget* table = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    GTUtilsWorkflowDesigner::setTableValue("Plain text", "Source URL (by File List)", GTUtilsWorkflowDesigner::comboValue, table);

    GTUtilsWorkflowDesigner::click("File List");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/bowtie/pattern/e_coli_1000.sam");

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "test_0896"));
    GTFileDialog::openFile(sandBoxDir, "test_0896out.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0898) {
    //    1. Open _common_data/ugenedb/example_alignment.ugenedb
    GTFileDialog::openFile(testDir + "_common_data/ugenedb/example-alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Zoom in until reads appear.

    //    3. Try to find consensus above reads area.
    //    Expected state: you found consensus.
    auto consArea = GTWidget::findWidget("Consensus area");
    CHECK_SET_ERR(consArea->isVisible(), "consensus area is not visiable");

    //    4. Open {Assembly Browser Settings} on options panel.
    GTWidget::click(GTWidget::findWidget("OP_ASS_SETTINGS"));

    //    5. Look for any algorithms in {Consensus algorithm} combobox (SAMtools, for example).
    auto consensusAlgorithmCombo = GTWidget::findComboBox("consensusAlgorithmCombo");

    //    Expected state: there are some algorithms.
    GTComboBox::checkValuesPresence(consensusAlgorithmCombo, {"SAMtools", "Default"});
}

GUI_TEST_CLASS_DEFINITION(test_0899) {
    //    1) Import data\samples\Assembly\chrM.sorted.bam
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state:
            //        1) opened dialog have File formats: {FASTA, FASTQ, GFF, Genbank, Vector NTI sequence}
            auto documentFormatComboBox = GTWidget::findComboBox("documentFormatComboBox", dialog);
            QStringList comboList;
            comboList << "FASTA"
                      << "FASTQ"
                      << "GFF"
                      << "GenBank"
                      << "Vector NTI sequence";
            GTComboBox::checkValuesPresence(documentFormatComboBox, comboList);

            //        2) region: {whole sequence, visible, custom}
            auto region_type_combo = GTWidget::findComboBox("region_type_combo", dialog);
            QStringList regionComboList;
            regionComboList << "Whole sequence"
                            << "Visible"
                            << "Custom region";
            GTComboBox::checkValuesPresence(region_type_combo, regionComboList);

            GTComboBox::selectItemByText(documentFormatComboBox, "GenBank");
            GTComboBox::selectItemByText(region_type_combo, "Whole sequence");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    //    Export consensus
    GTUtilsDialog::waitForDialog(new ExportConsensusDialogFiller(new Scenario()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export consensus..."}));
    GTWidget::click(GTWidget::findWidget("Consensus area"), Qt::RightButton);
    GTUtilsProjectTreeView::checkItem("chrM_consensus.gb");
}

GUI_TEST_CLASS_DEFINITION(test_0908) {
    // 1) Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::removeCmdlineWorkerFromPalette("test_0908");

    // 2) Click "Create element with external tool"
    // 3) input name "test"
    // 4) input data : "in1" and "in2" of FASTA
    // 5) output data : "out1" of FASTA
    // 6) Execution string : "cmd /c copy $in1 $out1 | copy $in2 $out1"
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/908/test_0908.etc"));
    GTWidget::click(GTAction::button("AddElementWithCommandLineTool"));

    // 7) Add input and output readers of FASTA
    GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/AMINO.fa");

    GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/alphabet.fa");

    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::addElement("Write Sequence");

    WorkflowProcessItem* cmdlineWorker = GTUtilsWorkflowDesigner::getWorker("test_0908");

    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence"), cmdlineWorker);
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read Sequence 1"), cmdlineWorker);
    GTUtilsWorkflowDesigner::connect(cmdlineWorker, writer);

    // 8) Run schema
    // Expected state : UGENE not crashed
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0910) {
    //    1. Create a scheme with the "Read sequence" element and the "Write sequence element".
    //    2. Take a file with more than one sequence as an input for the "Read sequence" element.
    //    3. Set the "Accumulate objects" parameters to False
    //    4. Set the "Existing file" parameter to "Rename"
    //    5. Run the scheme
    //    6. The number of output files must be equal to the number of input sequences

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/multy_fa.fa");

    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::addElement("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Accumulate objects", "False", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Existing file", "Rename", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_0910", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::connect(read, write);
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    QDir dir(sandBoxDir);
    CHECK_SET_ERR(dir.entryList(QDir::Files).count() == 2, QString("Incorrect count of sequence files: got %1, expected 2").arg(dir.entryList(QDir::Files).count()));
    foreach (const QString& fileName, dir.entryList(QDir::Files)) {
        CHECK_SET_ERR(fileName.startsWith("test_0910"), "Incorrect result file");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0928) {
    // 1. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
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

    // 2. Click "Find ORFs" button on the sequence view toolbar.
    // Expected state: ORF dialog appears. UGENE does not crash.
    // 3. Click "Ok" button and wait for the end of the task.
    GTUtilsDialog::waitForDialog(new OkClicker());
    GTWidget::click(GTAction::button("Find ORFs"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: 837 orfs are found.
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("orf  (0, 837)");
    CHECK_SET_ERR(item->childCount() == 837, QString("ORFs count mismatch. Expected: %1. Actual: %2").arg(837).arg(item->childCount()));
}

GUI_TEST_CLASS_DEFINITION(test_0930) {
    //    1. Open any *.bam file (e.g. "_common_data/bam/scerevisiae.bam").

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Look at the first column in the table above "Source URL" label.
            //    Expected state: you saw "Assembly name" in the colunms head.

            auto tableWidget = GTWidget::findTableWidget("tableWidget", dialog);
            QString header = tableWidget->horizontalHeaderItem(0)->text();
            CHECK_SET_ERR(header == "Assembly name", "unexpected header: " + header);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(new Scenario()));
    GTFileDialog::openFile(testDir + "_common_data/bam/scerevisiae.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: the "Import BAM File" dialog appeared.
}

GUI_TEST_CLASS_DEFINITION(test_0934) {
    //    1. Open file: _common_data\regression\934\trim_fa.fa
    GTUtilsProject::openMultiSequenceFileAsMalignment(testDir + "_common_data/regression/934/trim_fa.fa");

    //    2. Open "Export sequences into alignment" dialog
    //    3. Select CLUSTALW format. Expected state: warning that na,es will be cut is shown
    //    4. Select MSF format and export
    // Steps 2 - 4 are deprecated

    //    5. Align sequences with CLUSTALW. Excpected state: after alignment names are the same as before
    const QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    GTUtilsDialog::add(new PopupChooserByText({"Align", "Align with ClustalW..."}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new ClustalWDialogFiller());
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsTaskTreeView::waitTaskFinished();

    const QStringList resultNames = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(originalNames == resultNames, QString("Unexpected sequence names: expect '%1', got '%2'").arg(originalNames.join(", ")).arg(resultNames.join(", ")));
}

GUI_TEST_CLASS_DEFINITION(test_0935) {
    //    1. Start the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    //    Expected state: the Eorkflow Designeg opened.

    //    2. Add to the scene three elements: any Data Reader (e.g. Read Alignment), any Data Writer(e.g. Write Alignment), any element with input and output (e.g. Find Repeats).
    GTUtilsWorkflowDesigner::addAlgorithm("Read Alignment");
    GTUtilsWorkflowDesigner::addAlgorithm("Write Alignment");
    GTUtilsWorkflowDesigner::addAlgorithm("CD Search");
    //    Expected state: three elements are presented on the scene.

    //    3. For each element: select element, see to its properties.
    GTUtilsWorkflowDesigner::click("Read Alignment");
    auto out = GTWidget::findGroupBox("outputPortBox");
    CHECK_SET_ERR(out->title() == "Output data", "unexpected out box title: " + out->title());

    GTUtilsWorkflowDesigner::click("Write Alignment", QPoint(-20, -20));
    auto in = GTWidget::findGroupBox("inputPortBox");
    CHECK_SET_ERR(in->title() == "Input data", "unexpected in box title: " + in->title());

    GTUtilsWorkflowDesigner::click("CD Search");
    in = GTWidget::findGroupBox("inputPortBox");
    CHECK_SET_ERR(in->title() == "Input data", "unexpected in box title: " + in->title());
    out = GTWidget::findGroupBox("outputPortBox");
    CHECK_SET_ERR(out->title() == "Output data", "unexpected out box title: " + out->title());
    //    Expected state: if element hasn't the input port, there is no "Input data" section in properties.
    //            if element hasn't the output port, there is no "Outpu data" section in propeties.
}

GUI_TEST_CLASS_DEFINITION(test_0938) {
    //    1. Open any file in assembly view.
    //    2. Browse options panel.
    //    3. Open "Navigation" tab.
    //    Expected state: The tab contains "Enter position in assembly" edit field, "Go" button and "Most Covered Regions".

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "chrM.sorted.bam.ugenedb"));
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.sorted.bam");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("OP_ASS_NAVIGATION"));
    GTWidget::click(GTWidget::findWidget("OP_ASS_INFO"));
    GTWidget::click(GTWidget::findWidget("OP_ASS_SETTINGS"));
    GTWidget::click(GTWidget::findWidget("OP_ASS_NAVIGATION"));

    auto parent = GTWidget::findWidget("OP_OPTIONS_WIDGET");
    CHECK_SET_ERR(GTWidget::findWidget("go_to_pos_line_edit", parent) != nullptr, "go_to_pos_line_edit not found");
    CHECK_SET_ERR(GTWidget::findWidget("goButton", parent) != nullptr, "Go! button not found");
    CHECK_SET_ERR(GTWidget::findWidget("COVERED", parent) != nullptr, "Covered regions widget not found");
}

GUI_TEST_CLASS_DEFINITION(test_0940) {
    // 1. Open samples/CLUSTALW/COI.aln.
    // 2. Select any part of the alignment.
    // 3. Context menu of the selected part -> Export -> Save subalignment.
    // 4. Choose the same output file (samples/CLUSTALW/COI.aln) and press "Extract" button.
    // Expected state: UGENE does not crash.

    GTFile::copy(dataDir + "samples/CLUSTALW/COI.aln", sandBoxDir + "test_0940.aln");

    GTFileDialog::openFile(sandBoxDir, "test_0940.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(sandBoxDir + "test_0940.aln", GTUtilsMSAEditorSequenceArea::getNameList()));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(test_0941) {
    // 1. Open COI.aln
    // 2. Select the first sequence and choose {Edit->Replace slected row with reverse}. Expected state: The sequences is reversed
    // 3. Select the second sequence and choose {Edit->Replace slected row with complement}. Expected state: The sequences is complemented
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    GTUtilsMSAEditorSequenceArea::selectSequence("Isophya_altaica_EF540820");
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_EDIT, "replace_selected_rows_with_reverse-complement"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(sandBoxDir + "test_0941.aln", GTUtilsMSAEditorSequenceArea::getNameList()));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    GTUtilsTaskTreeView::waitTaskFinished();

    QString resultFileContent = GTFile::readAll(sandBoxDir + "test_0941.aln");
    QString expectedFileContent = GTFile::readAll(testDir + "_common_data/scenarios/_regression/941/test_0941.aln");
    CHECK_SET_ERR(resultFileContent == expectedFileContent, "Incorrect result file content");
}

GUI_TEST_CLASS_DEFINITION(test_0947) {
    /*  1. Open "data/samples/ABIF/A01.abi".
     *  3. Open GC Content (%) graph
     *  2. Close chromatogram view
     *    Expected state: GC Content (%) graph view resized
     */
    GTFileDialog::openFile(dataDir + "/samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto sequenceWidget = GTWidget::findWidget("ADV_single_sequence_widget_0");
    GTWidget::click(sequenceWidget);

    auto graphAction = GTWidget::findWidget("GraphMenuAction", sequenceWidget, false);
    Runnable* chooser = new PopupChooser({"GC Content (%)"});
    GTUtilsDialog::waitForDialog(chooser);

    GTWidget::click(graphAction);
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* zoomAction = GTAction::button("action_zoom_in_A1#berezikov");
    CHECK_SET_ERR(nullptr != zoomAction, "zoomAction is not present");

    for (int i = 0; i < 10; i++) {
        GTWidget::click(zoomAction);
        GTGlobals::sleep(100);
    }
}

GUI_TEST_CLASS_DEFINITION(test_0948) {
    // 1. Open "open file" dialog
    // 2. Select both files _common_data\scenarios\_regression\948\s1.fa and _common_data\scenarios\_regression\948\s2.fa
    // 3. Press "Open" and select "merge mode"
    // Expected state: warning messagebox about different alphabets has appeared, sequences not merged, but opened in different views
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(testDir + "_common_data/scenarios/_regression/948/", {"s1.fa", "s2.fa"}));
    GTMenu::clickMainMenuItem({"File", "Open..."});

    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0952) {
    // 1. Start UGENE.
    // 2. Press "Ctrl+O" or "Open" button on the main toolbar.
    // Expected state: "Select files to open..." dialog appears.
    // 3. Explore to the folder "data/samples/Genbank", then choose "CVU55762.gb" and "murine.gb" using Ctrl key
    // and press "Open" button.
    // Expected state: "Multiple sequence reading mode" dialog appears.
    // 4. Set radio button "Merge sequence mode", set "New document name" if you need, then press "OK".

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTFileDialog::openFileList(dataDir + "samples/Genbank/", {"murine.gb", "sars.gb"});

    // Expected state: New merged document appears in project view.
    // 5. Open the sequences from the third step in sequence view.
    // Expected state: all the annotations from both sequences were converted correctly to the merged document i.e. without
    // any shifts relatively to a start of sequence.
    GTUtilsAnnotationsTreeView::getItemCenter("3'UTR");
    bool found = GTUtilsAnnotationsTreeView::findRegion("3'UTR", U2Region(35232, 363));
    CHECK_SET_ERR(found, "Wrong annotations shifting");
}

GUI_TEST_CLASS_DEFINITION(test_0958) {
    //    1. Create *.csv file with the following content
    //    "Name","Start","End","Length","Complementary","Gene","desc","property","prop1","prop2"
    //    "test01","1","400","400","no","tEs01","unknown funtion","blablabla","",""
    //    "test02","60108","71020","10913","yes","","","","kobietghiginua","addsomethinghere"
    //    2. Open data/samples/FASTA/human_T1.fa.
    //    3. In the project tree's context menu choose option "Import" > "Import annotations from CSV file".
    //    4. In appearing "Import annotations from CSV file" dialog specify the *.csv file you have created,
    //    "Result file" in Genbank format. Then in "Results preview" field specify the desirable interpretations
    //    for each column accordingly to the first row in the preview table.
    //    5. Press "Run".
    //    Expected result: annotations have been imported to the sequence with correct locations and qualifiers.

    QFile file(sandBoxDir + "test_0958.csv");
    file.open(QFile::WriteOnly);
    file.write("\"Name\",\"Start\",\"End\",\"Length\",\"Complementary\",\"Gene\",\"desc\",\"property\",\"prop1\",\"prop2\"\n"
               "\"test01\",\"1\",\"400\",\"400\",\"no\",\"tEs01\",\"unknown funtion\",\"blablabla\",\"\",\"\"\n"
               "\"test02\",\"60108\",\"71020\",\"110913\",\"yes\",\"\",\"\",\"\",\"kobietghiginua\",\"addsomethinghere\"\n");
    file.close();

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters r;
    r << ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter())
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::LengthParameter())
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(4, new ImportAnnotationsToCsvFiller::StrandMarkParameter(true, "yes"))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(5, new ImportAnnotationsToCsvFiller::QualifierParameter("Gene"))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(6, new ImportAnnotationsToCsvFiller::QualifierParameter("desc"))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(7, new ImportAnnotationsToCsvFiller::QualifierParameter("property"))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(8, new ImportAnnotationsToCsvFiller::QualifierParameter("prop1"))
      << ImportAnnotationsToCsvFiller::RoleColumnParameter(9, new ImportAnnotationsToCsvFiller::QualifierParameter("prop2"));

    ImportAnnotationsToCsvFiller* filler = new ImportAnnotationsToCsvFiller(sandBoxDir + "test_0958.csv", sandBoxDir + "test_0958.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "misc_feature", r);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({"action_project__export_import_menu_action", "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("human_T1.fa", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::selectItemsByName({"test01"});

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("test01") != nullptr, "Annotation item not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString("test01") == "1..400", "Annotation region was improted incorrectly")
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("Gene", "test01") == "tEs01", "Qualifier Gene was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("desc", "test01") == "unknown funtion", "Qualifier desc was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("property", "test01") == "blablabla", "Qualifier property was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("prop1", "test01") == "", "Qualifier prop1 was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("prop2", "test01") == "", "Qualifier prop2 was improted incorrectly");

    GTUtilsAnnotationsTreeView::selectItemsByName({"test02"});
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("test02") != nullptr, "Annotation item not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString("test02") == "complement(60108..71020)", "Annotation region was improted incorrectly")
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("Gene", "test02") == "",
                  QString("Qualifier Gene was improted incorrectly: got '%1', expected ''").arg(GTUtilsAnnotationsTreeView::getQualifierValue("Gene", "test02")));
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("desc", "test02") == "", "Qualifier desc was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("property", "test02") == "", "Qualifier property was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("prop1", "test02") == "kobietghiginua", "Qualifier prop1 was improted incorrectly");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getQualifierValue("prop2", "test02") == "addsomethinghere", "Qualifier prop2 was improted incorrectly");
}

GUI_TEST_CLASS_DEFINITION(test_0965) {
    // 1. Open a few files supporting bookmarks.
    // Expected state: in "Bookmarks" area corresponding number of root bookmarks are created.
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Create a few sub bookmarks for each view.
    GTUtilsBookmarksTreeView::addBookmark(GTUtilsMdi::activeWindow()->objectName(), "murine");
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter("NC_004718 [sars.gb]"));
    GTMouseDriver::doubleClick();
    GTUtilsBookmarksTreeView::addBookmark(GTUtilsMdi::activeWindow()->objectName(), "sars");

    // 3. Press right mouse button on any bookmark connected with currently invisible view.
    // Expected state: "Add bookmark" action is disabled in appeared context menu.
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter("murine"));
    GTUtilsDialog::waitForDialog(new PopupChecker(QStringList(ACTION_ADD_BOOKMARK), PopupChecker::IsDisabled));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0967_1) {
    // 1. Open any document: Project View shown.
    // 2. Minimize and then restore the main window:  Project View should stay visible.
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    // Check that the project is visible.
    CHECK_SET_ERR(GTUtilsProjectTreeView::isVisible(), "ProjectTreeView is not visible (check #1)");

    QMainWindow* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    GTWidget::showMinimized(mainWindow);
    GTWidget::showMaximized(mainWindow);

    // Check that the project is still visible.
    CHECK_SET_ERR(GTUtilsProjectTreeView::isVisible(), "ProjectTreeView is not visible (check #2)");
}

GUI_TEST_CLASS_DEFINITION(test_0967_2) {
    // 1. Open Log view: Log view shown.
    // 2. Minimize and then restore the main window: log view should be not hidden.

    GTKeyboardDriver::keyClick('3', Qt::AltModifier);
    auto logView = GTWidget::findWidget("dock_log_view");
    CHECK_SET_ERR(logView->isVisible(), "Log View is not visible (check #1)");

    QMainWindow* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    GTWidget::showMinimized(mainWindow);
    GTWidget::showMaximized(mainWindow);

    logView = GTWidget::findWidget("dock_log_view");
    CHECK_SET_ERR(logView->isVisible(), "Log View is not visible (check #2)");
}

GUI_TEST_CLASS_DEFINITION(test_0967_3) {
    // 1. Open Tasks view: expected state: Tasks View shown.
    // 2. Minimize and then restore the main window:  Tasks View should be not hidden.

    GTKeyboardDriver::keyClick('2', Qt::AltModifier);
    auto tasksView = GTWidget::findWidget("taskViewTree");
    CHECK_SET_ERR(tasksView->isVisible(), "taskViewTree is not visible (check #1)");

    QMainWindow* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    GTWidget::showMinimized(mainWindow);
    GTWidget::showMaximized(mainWindow);

    tasksView = GTWidget::findWidget("taskViewTree");
    CHECK_SET_ERR(tasksView->isVisible(), "taskViewTree is not visible (check #2)");
}

GUI_TEST_CLASS_DEFINITION(test_0969) {
    // 1. Open any scheme file and do nothing with the opened scene (do not change).
    // 2. Click on 'Load scheme' or 'New scheme' button.
    // Expected state: WD doesn't ask to save the current scene

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("call variants");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTWidget::click(GTAction::button("New workflow action"));
}

GUI_TEST_CLASS_DEFINITION(test_0970) {
    // 1. Enable Auto Annotations. Open human_T1.fa
    // 2. Open context menu for the Auto annotation in annotation tree view
    // Expected state: "Disable 'annotation' highlighting" item not presents in menu
}

GUI_TEST_CLASS_DEFINITION(test_0981_1) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(
        new InsertSequenceFiller(
            "qweqwea",
            InsertSequenceFiller::Resize,
            1,
            "",
            InsertSequenceFiller::FASTA,
            false,
            false,
            GTGlobals::UseMouse,
            true));
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Insert subsequence..."}, GTGlobals::UseKey);
}

GUI_TEST_CLASS_DEFINITION(test_0981_2) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller(1, 2));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);
    Runnable* filler1 = new ReplaceSubsequenceDialogFiller(
        "qweqwea",
        false,
        true);
    GTUtilsDialog::waitForDialog(filler1);
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, ACTION_EDIT_REPLACE_SUBSEQUENCE}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0986) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    SmithWatermanDialogFiller* filler = new SmithWatermanDialogFiller();
    filler->button = SmithWatermanDialogFiller::Cancel;
    GTUtilsDialog::waitForDialog(filler);

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0986_1) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    SmithWatermanDialogFiller* filler = new SmithWatermanDialogFiller();
    filler->button = SmithWatermanDialogFiller::Cancel;
    GTUtilsDialog::waitForDialog(filler);

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0986_2) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTRegionSelector::RegionSelectorSettings regionSelectorSettings(1, 2);
    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller("ATCG", regionSelectorSettings));

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0986_3) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTRegionSelector::RegionSelectorSettings regionSelectorSettings(1, 2);
    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller("ATCGAT", regionSelectorSettings));

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_0986_4) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTRegionSelector::RegionSelectorSettings regionSelectorSettings(1, 2);
    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller("ATCGAT", regionSelectorSettings));

    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0994) {
    /* 1. _common_data/scenarios/regression/994/musMusc.gb
     * 2. Expand contigs_snp group.
     * 3. Call a tooltip of he first annotation in the group.
     *   Expected state: UGENE not crashes
     */
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/994/", "musMusc.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("106-c1_38ftp"));
}

GUI_TEST_CLASS_DEFINITION(test_0999_1) {
    //    1. Do menu {File->New document from text...}
    //    Expected state: Create Document dialog has appear

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Fill next fields in this dialog
            //        {Paste sequence here} - AAAAAD
            //        {Custom settings} - checked
            //        {Replace unknown symbols with} - z
            GTPlainTextEdit::setText(GTWidget::findPlainTextEdit("sequenceEdit", dialog), "AAAAAD");
            GTGroupBox::setChecked("groupBox", dialog);
            GTRadioButton::click(GTWidget::findRadioButton("replaceRB", dialog));
            GTLineEdit::setText(GTWidget::findLineEdit("symbolToReplaceEdit", dialog), "z");

            //    3. Press Create button
            //    Expected state: error message appears "Replace symbol belongs to selected alphabet"
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Replace symbol is not belongs to selected alphabet"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(new Scenario));
    GTMenu::clickMainMenuItem({"File", "New document from text..."});
}

GUI_TEST_CLASS_DEFINITION(test_0999_2) {
    //    1. Do menu {File->New document from text...}
    //    Expected state: Create Document dialog has appear

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Fill next fields in this dialog
            //        {Paste sequence here} - AAAZZZZZZAAA
            //        {Custom settings} - checked
            //        {Replace unknown symbols with} - T
            //        {Document Location} - %any valid filepath%
            GTPlainTextEdit::setText(GTWidget::findPlainTextEdit("sequenceEdit", dialog), "AAAZZZZZZAAA");
            GTGroupBox::setChecked(GTWidget::findGroupBox("groupBox", dialog));
            GTRadioButton::click(GTWidget::findRadioButton("replaceRB", dialog));
            GTLineEdit::setText(GTWidget::findLineEdit("symbolToReplaceEdit", dialog), "T");
            GTLineEdit::setText(GTWidget::findLineEdit("filepathEdit", dialog), sandBoxDir + "test_0999_2.fa");

            //    3. Press Create button
            //    Expected state: sequence view with sequence "AAATTTTTTAAA" has opened
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateDocumentFiller(new Scenario));
    GTMenu::clickMainMenuItem({"File", "New document from text..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMdi::findWindow("test_0999_2 [test_0999_2.fa]");
}

GUI_TEST_CLASS_DEFINITION(test_1000) {
    //    1. Open "data/samples/3INS.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB/3INS.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. In the context menu of the first sequence in sequence view choose {Analize -> Predict secondary structure...}.
    //    Expected state: "Secondary structure prediction" dialog is opened.

    class Scenario : public CustomScenario {
    public:
        Scenario(const QString& algorithm)
            : CustomScenario(),
              algorithm(algorithm) {
        }

        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmComboBox", dialog), algorithm);

            //    3. Fill fields "Range start" and "Range end" with values "1" and "2" respectively.

            GTLineEdit::setText(GTWidget::findLineEdit("start_edit_line", dialog), "1");
            GTLineEdit::setText(GTWidget::findLineEdit("end_edit_line", dialog), "2");

            //    4. Press "Start prediction".
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }

    private:
        const QString algorithm;
    };

    class DodgeLicenceDialogScenario : public CustomScenario {
    public:
        DodgeLicenceDialogScenario(const QString& algorithm)
            : CustomScenario(),
              algorithm(algorithm) {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new LicenseAgreementDialogFiller());
            GTComboBox::selectItemByText(GTWidget::findComboBox("algorithmComboBox", dialog), algorithm);

            //    4. Press "Start prediction".
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }

    private:
        const QString algorithm;
    };

    //    Expected state: Error notification appears.
    GTLogTracer lt1;
    GTUtilsDialog::add(new PopupChooserByText({"Analyze", "Predict secondary structure..."}));
    GTUtilsDialog::add(new PredictSecondaryStructureDialogFiller(new Scenario("GORIV")));
    // GTUtilsNotifications::waitForNotification(true, "'Secondary structure predict' task failed: The size of sequence is less then minimal allowed size (5 residues).");
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt1.hasError("Task {Secondary structure predict} finished with error: The size of sequence is less then minimal allowed size (5 residues)"), "Expected error not found");

    //    5. Repeat steps 2, 3, then choose another algorithm in dialog.
    //    6. Press "Start prediction".
    //    Expected state: Error notification appears.
    GTLogTracer lt2;
    GTUtilsDialog::add(new PopupChooserByText({"Analyze", "Predict secondary structure..."}));
    GTUtilsDialog::add(new PredictSecondaryStructureDialogFiller(new DodgeLicenceDialogScenario("PsiPred")));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooserByText({"Analyze", "Predict secondary structure..."}));
    GTUtilsDialog::add(new PredictSecondaryStructureDialogFiller(new Scenario("PsiPred")));
    // GTUtilsNotifications::waitForNotification(true, "'Secondary structure predict' task failed: The size of sequence is less then minimal allowed size (5 residues).");
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt2.hasError("Task {Secondary structure predict} finished with error: The size of sequence is less then minimal allowed size (5 residues)"), "Expected error not found");
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
