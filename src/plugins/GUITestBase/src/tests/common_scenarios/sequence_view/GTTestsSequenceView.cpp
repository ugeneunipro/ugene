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

#include "GTTestsSequenceView.h"
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLabel.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>

#include <QApplication>
#include <QGroupBox>
#include <QTreeWidgetItem>

#include <U2Core/AppContext.h>

#include <U2View/ADVConstants.h>
#include <U2View/DetView.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/Overview.h>

#include "GTGlobals.h"
#include "GTUtilsAnnotationsHighlightingTreeView.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateRulerDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/GTComboBoxWithCheckBoxes.h"
#include "runnables/ugene/corelibs/U2Gui/GraphSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SetSequenceOriginDialogFiller.h"
#include "runnables/ugene/plugins/biostruct3d_view/StructuralAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTClipboard.h"
#include "system/GTFile.h"
#include "utils/GTThread.h"
#include "utils/GTUtilsToolTip.h"

namespace U2 {

namespace GUITest_common_scenarios_sequence_view {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    disable circular view for protein sequences (0002400)

    //    1. Open file samples/PDB/1CF7.pdb
    //    Expected state: 'Show circular view' button disabled for 1CF7 chain 1 sequence and 1CF7 chain 2 sequence, but enabled for others

    GTFileDialog::openFile(dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto w0 = GTWidget::findWidget("ADV_single_sequence_widget_0");
    auto w1 = GTWidget::findWidget("ADV_single_sequence_widget_1");
    auto w2 = GTWidget::findWidget("ADV_single_sequence_widget_2");
    auto w3 = GTWidget::findWidget("ADV_single_sequence_widget_3");

    auto button0 = GTWidget::findWidget("CircularViewAction", w0, false);
    CHECK_SET_ERR(button0 == nullptr, "b0 is not NULL");

    auto button1 = GTWidget::findWidget("CircularViewAction", w1, false);
    CHECK_SET_ERR(button1 == nullptr, "b1 is not NULL");

    GTWidget::findWidget("CircularViewAction", w2);
    GTWidget::findWidget("CircularViewAction", w3);
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTFileDialog::openFile(dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto w0 = GTWidget::findWidget("ADV_single_sequence_widget_0");
    auto w1 = GTWidget::findWidget("ADV_single_sequence_widget_1");
    auto w2 = GTWidget::findWidget("ADV_single_sequence_widget_2");
    auto w3 = GTWidget::findWidget("ADV_single_sequence_widget_3");

    QAbstractButton* b0 = GTAction::button("complement_action", w0, false);
    CHECK_SET_ERR(b0 == nullptr, "b0 is not NULL");

    QAbstractButton* b1 = GTAction::button("complement_action", w1, false);
    CHECK_SET_ERR(b1 == nullptr, "b1 is not NULL");

    GTAction::button("complement_action", w2);
    GTAction::button("complement_action", w3);
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    GTFileDialog::openFile(dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto w0 = GTWidget::findWidget("ADV_single_sequence_widget_0");
    auto w1 = GTWidget::findWidget("ADV_single_sequence_widget_1");
    auto w2 = GTWidget::findWidget("ADV_single_sequence_widget_2");
    auto w3 = GTWidget::findWidget("ADV_single_sequence_widget_3");

    auto b0 = GTWidget::findWidget("translationsMenuToolbarButton", w0, false);
    CHECK_SET_ERR(b0 == nullptr, "b0 is not NULL");

    auto b1 = GTWidget::findWidget("translationsMenuToolbarButton", w1, false);
    CHECK_SET_ERR(b1 == nullptr, "b1 is not NULL");

    GTWidget::findWidget("translationsMenuToolbarButton", w2);
    GTWidget::findWidget("translationsMenuToolbarButton", w3);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Unnecessary buttons are enabled in zoom view (0001251)

    //    1. Open file samples/FASTA/human_T1.fa

    //    2. Hide details view with button {Toggle view->Hide details view}, if it already hided skip this step.
    //    Expected state: buttons 'Show amino translations' and 'Show amino strand' are disabled

    //    3. Show details view with button {Toggle view->Show details view}
    //    Expected state: buttons 'Show amino translations' and 'Show amino strand' becomes enabled

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto toggleViewButton = GTWidget::findWidget("toggleViewButton");
    Runnable* chooser = new PopupChooser({"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(chooser);
    GTWidget::click(toggleViewButton);

    QAbstractButton* complement = GTAction::button("complement_action");

    CHECK_SET_ERR(complement->isEnabled() == false, "button is not disabled");

    Runnable* chooser1 = new PopupChooser({"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(chooser1);
    GTWidget::click(toggleViewButton);

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));

    QAbstractButton* complement1 = GTAction::button("complement_action");

    CHECK_SET_ERR(complement1->isEnabled() == true, "button is not enabled");
}
GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto ext_button = GTWidget::findWidget("qt_toolbar_ext_button", GTWidget::findWidget("mwtoolbar_activemdi"));
    if (ext_button != nullptr && ext_button->isVisible()) {
        GTWidget::click(ext_button);
    }

    auto toggleViewButton = GTWidget::findWidget("toggleViewButton");
    Runnable* chooser = new PopupChooser({"toggleAllSequenceViews"});
    GTUtilsDialog::waitForDialog(chooser);
    GTWidget::click(toggleViewButton);

    QAbstractButton* complement = GTAction::button("complement_action");

    CHECK_SET_ERR(complement->isEnabled() == false, "button is not disabled");

    Runnable* chooser1 = new PopupChooser({"toggleAllSequenceViews"});
    GTUtilsDialog::waitForDialog(chooser1);
    GTWidget::click(toggleViewButton);

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));

    QAbstractButton* complement1 = GTAction::button("complement_action");

    CHECK_SET_ERR(complement1->isEnabled() == true, "button is not enabled");
}
GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto toggleViewButton = GTWidget::findWidget("toggleViewButton");
    Runnable* chooser = new PopupChooser({"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(chooser);
    GTWidget::click(toggleViewButton);

    QAbstractButton* complement = GTAction::button("complement_action");

    CHECK_SET_ERR(complement->isEnabled() == false, "button is not disabled");

    Runnable* chooser1 = new PopupChooser({"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(chooser1);
    GTWidget::click(toggleViewButton);

    GTUtilsDialog::waitForDialog(new PopupChecker({"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));

    CHECK_SET_ERR(complement->isEnabled() == true, "button is not disabled");

    Runnable* chooser2 = new PopupChooser({"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(chooser2);
    GTWidget::click(toggleViewButton);

    QAbstractButton* complement1 = GTAction::button("complement_action");
    CHECK_SET_ERR(complement1->isEnabled() == false, "button is not disabled");
}
GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    UGENE crashes on subsequence export (0002423)

    //    1. Open sequence

    //    2. Select some region

    //    3. Call context menu "Export -> Export selected sequence region"

    //    4. Press Export button in dialog.
    //    Expected state: UGENE not crashed

    GTFileDialog::openFile(testDir + "_common_data/fasta/", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1, 3);

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {  // CHANGES: keyboard used instead mouse

    GTFileDialog::openFile(testDir + "_common_data/fasta/", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1, 3);

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //    ADVSeqWidget auto annotations (0002588)

    //    1. Open file with amino alphabet (_common_data/fasta/multy_fa.fa)
    //    Expected state: Auto-annotations disabled

    //    3. Open file with (samples/FASTA/human_T1.fa)

    //    4. Activate AA
    //    Expected state: UGENE not Crashes

    class AllActionsPopupChooser : public PopupChooser {
    public:
        AllActionsPopupChooser()
            : PopupChooser(QStringList()) {
        }
        void run() override {
            auto activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());

            int i = 0;
            QList<QAction*> act = activePopupMenu->actions();
            for (QAction* a : qAsConst(act)) {
                QString s = a->objectName();
                if (i == 1) {
                    GTMenu::clickMenuItem(activePopupMenu, s, GTGlobals::UseMouse);
                }
                i++;
            }
        }
    };

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsProject::openFile(testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsDocument::checkDocument("multy_fa.fa");

    auto toggleAutoAnnotationsButton = GTWidget::findWidget("toggleAutoAnnotationsButton");
    CHECK_SET_ERR(toggleAutoAnnotationsButton->isEnabled() == false, "toggleAutoAnnotationsButton is enabled, expected disabled");

    GTUtilsProject::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsDocument::checkDocument("human_T1.fa");

    toggleAutoAnnotationsButton = GTWidget::findWidget("toggleAutoAnnotationsButton");
    CHECK_SET_ERR(toggleAutoAnnotationsButton->isEnabled() == true, "toggleAutoAnnotationsButton is disabled, expected enabled");

    GTUtilsDialog::waitForDialog(new AllActionsPopupChooser());
    GTWidget::click(toggleAutoAnnotationsButton);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // 1. Enable Auto-annotations
    //      GTGlobals::sleep();
    //      GTGlobals::sleep();
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto toggleAutoAnnotationsButton = GTWidget::findWidget("toggleAutoAnnotationsButton");
    //  !!! dirty fastfix of test, very temporary
    auto tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
    //

    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    // 2. Open any PBD file from samples
    // Expected state: there is no auto-annotations for opened sequences

    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem("enzymes", nullptr, {false});
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem("orfs", nullptr, {false});
    CHECK_SET_ERR(enz == nullptr, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == nullptr, "orfs unexpectedly present");
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    // 1. Enable Auto-annotations
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto toggleAutoAnnotationsButton = GTWidget::findWidget("toggleAutoAnnotationsButton");
    //  !!! dirty fastfix of test, very temporary
    auto tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
    //

    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    // 1. Disable Auto-annotations
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    // 3. Enable Auto-annotations
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    // 2. Open any PBD file from samples
    // Expected state: there is no auto-annotations for opened sequences
    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there is no auto-annotations for opened sequences
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem("enzymes", nullptr, {false});
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem("orfs", nullptr, {false});
    CHECK_SET_ERR(enz == nullptr, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == nullptr, "orfs unexpectedly present");
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    // 2. Open any PBD file from samples
    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 1. Enable Auto-annotations
    auto toggleAutoAnnotationsButton = GTWidget::findWidget("toggleAutoAnnotationsButton");
    //  !!! dirty fastfix of test, very temporary
    auto tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
    //

    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(extensionButton);
    }
    GTWidget::click(toggleAutoAnnotationsButton);

    // Expected state: there is no auto-annotations for opened sequences
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem("enzymes", nullptr, {false});
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem("orfs", nullptr, {false});
    CHECK_SET_ERR(enz == nullptr, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == nullptr, "orfs unexpectedly present");
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    //    1) Click "Open file" button.

    //    2) Choose two sequence files: "_common_data/fasta/DNA.fa" and "_common_data/fasta/DNA_1_seq.fa". And click "Open" button.
    //    Expected state: the dialog appears.

    //    3) Choose "Merge sequence mode" and click "OK" button.
    //    Expected state: one sequence view with a merged sequence is opened. Its name is "merged_document.gb"; its length is 2249.

    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTFileDialog::openFileList(testDir + "_common_data/fasta/", {"DNA.fa", "DNA_1_seq.fa"});

    int length = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(2249 == length, QString("Sequence length mismatch. Expected: %1. Actual: %2").arg(2249).arg(length));

    QString mergedFileName = testDir + "_common_data/fasta/merged_document.gb";
    QFile::remove(mergedFileName);
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    //     1. Open '_common_data/primer3/linear_circular_results.fa'
    //     2. Launch Primer3 with default settings
    //     Expected state: even the sequence is linear, Primer3 finds primers on junction
    //
    //     The line above has the incorrect statement,
    //     which appeared because of a major bug, which was in primer3 results implementation.
    //     This bug has been fixed during the primer3 update,
    //     results could be located on junction point only if sequence is circular
    //     and the selected region contains jusnction point (see test_0022).

    GTFileDialog::openFile(testDir + "/_common_data/primer3", "linear_circular_results.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(new Primer3DialogFiller());
    GTWidget::click(GTUtilsSequenceView::getPanOrDetView(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{8, 27}, {105, 124}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{8, 27}, {135, 154}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{4, 24}, {105, 124}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{8, 27}, {137, 156}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{8, 27}, {113, 132}});
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    //     1. Open '_common_data/primer3/circular_primers.gb
    //     2. Select 1..16,151..351 (region with junction point)
    //     3. Launch Primer3 with default settings (it will use the selected region)
    //     Expected state: the sequence is circular - a few primers cover junction point

    GTFileDialog::openFile(testDir + "/_common_data/primer3", "circular_primers.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsSequenceView::selectSeveralRegionsByDialog("1..16,151..351");

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(new Primer3DialogFiller());
    GTWidget::click(GTUtilsSequenceView::getPanOrDetView(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{221, 241}, {335, 351}, {1, 3}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{221, 241}, {336, 351}, {1, 5}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{220, 241}, {335, 351}, {1, 3}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{221, 241}, {333, 351}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{221, 241}, {333, 351}, {1, 1}});
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    //    1. Open 'human_T1.fa'
    //    2. Open CV
    //    3. Select big region covering junction point
    //    4. Launch Primer3 search (set results count to 50)
    //    Expected state: primers are located on both selected regions(start and end).

    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsProjectTreeView::markSequenceAsCircular("human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("150000..199950,1..50000"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence region"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    Primer3DialogFiller::Primer3Settings settings;
    settings.resultsCount = 50;
    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{22172, 22191}, {22369, 22388}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 10  (0, 2)", {{185965, 185984}, {186089, 186108}});
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    //    1. Open '_common_data/primer3/DNA.gb'
    //    2. Select region that cover junction point
    //    3. Launch Primer3 search with default settings
    //    Expected state: primers(left and right) are located on either side of junction point

    GTFileDialog::openFile(testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTWidget::click(GTWidget::findWidget("CircularViewAction"));

    if (!GTUtilsSequenceView::getPanOrDetView()->isVisible()) {
        auto toggleViewButton = GTWidget::findWidget("toggleViewButton");
        GTUtilsDialog::add(new PopupChooser({"toggleZoomView"}));
        GTWidget::click(toggleViewButton);
    }

    GTUtilsDialog::add(new SelectSequenceRegionDialogFiller("560..743,1..180"));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTThread::waitForMainThread();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(new Primer3DialogFiller());
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{95, 114}, {685, 704}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{142, 161}, {685, 704}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{143, 162}, {685, 704}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{143, 162}, {607, 626}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{47, 66}, {685, 704}});
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    //    1. Open 'common_data/primer3/DNA.gb'
    //    2. Open Primer3 dialog
    //    3. Set region to circular: 560..(743,1)..180
    //    4. Accept the dialog
    //    Expected state: no warning apeared, primers are located on selected region
    GTFileDialog::openFile(testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "primer3_action"}));
    Primer3DialogFiller::Primer3Settings settings;
    settings.start = 560;
    settings.end = 180;
    GTUtilsDialog::add(new Primer3DialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 1  (0, 2)", {{95, 114}, {685, 704}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 2  (0, 2)", {{142, 161}, {685, 704}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 3  (0, 2)", {{143, 162}, {685, 704}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 4  (0, 2)", {{143, 162}, {607, 626}});
    GTUtilsAnnotationsTreeView::checkAnnotationRegions("pair 5  (0, 2)", {{47, 66}, {685, 704}});
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    //    Default export
    //    1. Open sars.gb
    //    2. Export sequence as image

    GTFileDialog::openFile(dataDir + "/samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ExportSequenceImage(sandBoxDir + "seq_image_0027"));
    GTWidget::click(GTAction::button("export_image"));
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    //    Check region selector appearance
    //    Region selector is hidden if CurrentView export is selected

    GTFileDialog::openFile(dataDir + "/samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    class RegionChecker : public Filler {
    public:
        RegionChecker()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::clickWindowTitle(dialog);

            auto radioButton = GTWidget::findRadioButton("currentViewButton", dialog);
            GTRadioButton::click(radioButton);

            auto rangeSelector = GTWidget::findWidget("range_selector", dialog);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is visible");

            radioButton = GTWidget::findRadioButton("zoomButton", dialog);
            GTRadioButton::click(radioButton);
            CHECK_SET_ERR(rangeSelector->isVisible(), "range_selector is hidden");

            radioButton = GTWidget::findRadioButton("detailsButton", dialog);
            GTRadioButton::click(radioButton);
            CHECK_SET_ERR(rangeSelector->isVisible(), "range_selector is hidden");

            radioButton = GTWidget::findRadioButton("currentViewButton", dialog);
            GTRadioButton::click(radioButton);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is hidden");
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new RegionChecker());
    GTWidget::click(GTAction::button("export_image"));
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
    //    Check boundaries of export in raster formats
    //    1. Open sars.gb
    //    2. Zoom in a few times
    //    3. Open Export Image dialog
    //    4. Select Details view export
    //    Expected state: warning message appeared, export is blocked
    //    5. Select a fewer region
    //    Expected state: warning is hidden, export is available
    //    6. Select Zoomed Image export
    //    Expected state: warning message appeared, export is blocked
    //    7. Select a fewer region
    //    Expected state: warning is hidden, export is available

    GTFileDialog::openFile(dataDir + "/samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(wrapButton);

    QAction* zoom = GTAction::findActionByText("Zoom In");
    CHECK_SET_ERR(zoom != nullptr, "Cannot find Zoom In action");
    for (int i = 0; i < 8; i++) {
        GTWidget::click(GTAction::button(zoom));
    }

    class LimitsChecker : public Filler {
    public:
        LimitsChecker()
            : Filler("ImageExportForm") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::clickWindowTitle(dialog);

            auto rangeSelector = GTWidget::findWidget("range_selector", dialog);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is visible");

            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* okbutton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okbutton != nullptr, "ok button is NULL");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            auto radioButton = GTWidget::findRadioButton("zoomButton", dialog);
            GTRadioButton::click(radioButton);
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            auto end = GTWidget::findLineEdit("end_edit_line", dialog);
            GTLineEdit::setText(end, "100");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            radioButton = GTWidget::findRadioButton("detailsButton", dialog);
            GTRadioButton::click(radioButton);
            GTLineEdit::setText(end, "10000");
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            GTLineEdit::setText(end, "100");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button != nullptr, "cancel button is NULL");
            GTWidget::click(button);
        }
    };

    GTUtilsDialog::waitForDialog(new LimitsChecker());
    GTWidget::click(GTAction::button("export_image"));
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    //    Check boundaries of export fro SVG
    //    1. Open human_T1.fa
    //    2. Find restriction sites YkrI
    //    3. Open Export Image dialog
    //    4. Select SVG format
    //    Expected state: export is not available
    //    5. Select Details view export
    //    Expected state: warning message appeared, export is blocked
    //    6. Select a fewer region
    //    Expected state: warning is hidden, export is available
    //    6. Select Zoomed Image export and the whole sequence to export
    //    Expected state: warning message appeared, export is blocked
    //    5. Select a fewer region and export that
    //    Expected state: warning is hidden, export is available

    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"YkrI"}));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Analyze", "Find restriction sites..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsTaskTreeView::waitTaskFinished();

    class SvgLimitsChecker : public Filler {
    public:
        SvgLimitsChecker()
            : Filler("ImageExportForm") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            // set SVG format
            auto formatsBox = GTWidget::findComboBox("formatsBox", dialog);
            GTComboBox::selectItemByText(formatsBox, "SVG");

            // export is not available
            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);

            QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != nullptr, "ok button is NULL");
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is expectedly enabled");

            // select Details -- export is disabled
            auto radioButton = GTWidget::findRadioButton("detailsButton", dialog);
            GTRadioButton::click(radioButton);
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is unexpectedly enabled");

            auto end = GTWidget::findLineEdit("end_edit_line", dialog);
            GTLineEdit::setText(end, "2300");
            CHECK_SET_ERR(okButton->isEnabled(), "Export button is unexpectedly disabled");

            // set Zoom view export
            radioButton = GTWidget::findRadioButton("zoomButton", dialog);
            GTRadioButton::click(radioButton);
            CHECK_SET_ERR(okButton->isEnabled(), "Export button is unexpectedly disabled");
            GTLineEdit::setText(end, "199000");
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is unexpectedly enabled");

            // select a fewer region -- export is enabled
            GTLineEdit::setText(end, "10000");
            CHECK_SET_ERR(okButton->isEnabled(), "Export button is unexpectedly disabled");

            auto fileEdit = GTWidget::findLineEdit("fileNameEdit", dialog);
            GTLineEdit::setText(fileEdit, sandBoxDir + "seq_view_test_0030.svg");

            GTWidget::click(okButton);
        }
    };

    GTUtilsDialog::waitForDialog(new SvgLimitsChecker(), 180000);
    GTWidget::click(GTAction::button("export_image"));
    GTUtilsDialog::checkNoActiveWaiters(180000);
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 fileSize = GTFile::getSize(sandBoxDir + "seq_view_test_0030.svg");
    CHECK_SET_ERR(fileSize > 15 * 1024 * 1024, QString("SVG file is too small: %1").arg(fileSize));
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    Select some redion
    GTUtilsSequenceView::selectSequenceRegion(10, 20);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy reverse complement sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString clipboardtext = GTClipboard::text();
    CHECK_SET_ERR(clipboardtext == "AACTTTGGTGA", "Unexpected reverse complement: " + clipboardtext)
}

GUI_TEST_CLASS_DEFINITION(test_0031_1) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    Select some redion
    GTUtilsSequenceView::selectSequenceRegion(10, 20);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "Copy reverse complement sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString clipboardtext = GTClipboard::text();
    CHECK_SET_ERR(clipboardtext == "AACTTTGGTGA", "Unexpected reverse complement: " + clipboardtext)
}

GUI_TEST_CLASS_DEFINITION(test_0031_2) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    Select annotation
    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 2, 0, true);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString clipboardtext = GTClipboard::text();
    CHECK_SET_ERR(clipboardtext.startsWith("AATGAAAGAC"), "Unexpected reverse complement start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("GTCTTTCATT"), "Unexpected reverse complement end: " + clipboardtext.right(10));

    //    Check joined annotations
    GTUtilsAnnotationsTreeView::clickItem("CDS", 2, true);

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    clipboardtext = GTClipboard::text();
    CHECK_SET_ERR(clipboardtext.startsWith("ATGGTAGCAG"), "Unexpected reverse complement for joined annotation start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("TCTAGACTGA"), "Unexpected reverse complement for joined annotation end: " + clipboardtext.right(10));
}

GUI_TEST_CLASS_DEFINITION(test_0031_3) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    Select annotation
    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 2, 0, true);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString clipboardtext = GTClipboard::text();
    CHECK_SET_ERR(clipboardtext.startsWith("AATGAAAGAC"), "Unexpected reverse complement start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("GTCTTTCATT"), "Unexpected reverse complement end: " + clipboardtext.right(10));

    //    Check joined annotations
    GTUtilsAnnotationsTreeView::clickItem("CDS", 2, true);

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    clipboardtext = GTClipboard::text();
    CHECK_SET_ERR(clipboardtext.startsWith("ATGGTAGCAG"), "Unexpected reverse complement for joined annotation start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("TCTAGACTGA"), "Unexpected reverse complement for joined annotation end: " + clipboardtext.right(10));
}

GUI_TEST_CLASS_DEFINITION(test_0032) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //  Click "Amino translations" button on mdi toolbar
    auto translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    class UncheckComplement : public CustomScenario {
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            GTMenu::clickMenuItemByText(activePopupMenu, {"Set up frames manually"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame -1"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame -2"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame -3"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(new PopupChecker(new UncheckComplement()));
    GTWidget::click(translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters();

    //  Check "Show direct only"
    class DirectPopupChecker : public CustomScenario {
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            QAction* direct1 = GTMenu::getMenuItem(activePopupMenu, "Frame +1", true);
            QAction* direct2 = GTMenu::getMenuItem(activePopupMenu, "Frame +2", true);
            QAction* direct3 = GTMenu::getMenuItem(activePopupMenu, "Frame +3", true);
            QAction* compl1 = GTMenu::getMenuItem(activePopupMenu, "Frame -1", true);
            QAction* compl2 = GTMenu::getMenuItem(activePopupMenu, "Frame -2", true);
            QAction* compl3 = GTMenu::getMenuItem(activePopupMenu, "Frame -3", true);

            CHECK_SET_ERR(direct1->isChecked(), "check1: direct1 is not checked");
            CHECK_SET_ERR(direct2->isChecked(), "check1: direct2 is not checked");
            CHECK_SET_ERR(direct3->isChecked(), "check1: direct3 is not checked");
            CHECK_SET_ERR(!compl1->isChecked(), "check1: compl1 is checked");
            CHECK_SET_ERR(!compl2->isChecked(), "check1: compl2 is checked");
            CHECK_SET_ERR(!compl3->isChecked(), "check1: compl3 is checked");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(new PopupChecker(new DirectPopupChecker()));
    GTWidget::click(translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters();

    //    Check "Show complement only"
    class UncheckDirectCheckComplement : public CustomScenario {
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame +1"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame +2"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame +3"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame -1"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame -2"});
            GTMenu::clickMenuItemByText(activePopupMenu, {"Frame -3"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(new PopupChecker(new UncheckDirectCheckComplement()));
    GTWidget::click(translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters();

    class ComplPopupChecker : public CustomScenario {
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            QAction* direct1 = GTMenu::getMenuItem(activePopupMenu, "Frame +1", true);
            QAction* direct2 = GTMenu::getMenuItem(activePopupMenu, "Frame +2", true);
            QAction* direct3 = GTMenu::getMenuItem(activePopupMenu, "Frame +3", true);
            QAction* compl1 = GTMenu::getMenuItem(activePopupMenu, "Frame -1", true);
            QAction* compl2 = GTMenu::getMenuItem(activePopupMenu, "Frame -2", true);
            QAction* compl3 = GTMenu::getMenuItem(activePopupMenu, "Frame -3", true);

            CHECK_SET_ERR(!direct1->isChecked(), "check2: direct1 is checked");
            CHECK_SET_ERR(!direct2->isChecked(), "check2: direct2 is checked");
            CHECK_SET_ERR(!direct3->isChecked(), "check2: direct3 is checked");
            CHECK_SET_ERR(compl1->isChecked(), "check2: compl1 is not checked");
            CHECK_SET_ERR(compl2->isChecked(), "check2: compl2 is not checked");
            CHECK_SET_ERR(compl3->isChecked(), "check2: compl3 is not checked");
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(new PopupChecker(new ComplPopupChecker()));
    GTWidget::click(translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters();

    //    Check "Show all".
    class ShowAllFramesScenario : public CustomScenario {
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            GTMenu::clickMenuItemByText(activePopupMenu, {"Show all frames"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(new PopupChecker(new ShowAllFramesScenario()));
    GTWidget::click(translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters();

    // Check results of Show all frames.
    class AllPopupChecker : public CustomScenario {
        void run() override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu();
            QAction* direct1 = GTMenu::getMenuItem(activePopupMenu, "Frame +1", true);
            QAction* direct2 = GTMenu::getMenuItem(activePopupMenu, "Frame +2", true);
            QAction* direct3 = GTMenu::getMenuItem(activePopupMenu, "Frame +3", true);
            QAction* compl1 = GTMenu::getMenuItem(activePopupMenu, "Frame -1", true);
            QAction* compl2 = GTMenu::getMenuItem(activePopupMenu, "Frame -2", true);
            QAction* compl3 = GTMenu::getMenuItem(activePopupMenu, "Frame -3", true);

            CHECK_SET_ERR(direct1->isChecked(), "check3: direct1 is checked");
            CHECK_SET_ERR(direct2->isChecked(), "check3: direct2 is checked");
            CHECK_SET_ERR(direct3->isChecked(), "check3: direct3 is checked");
            CHECK_SET_ERR(compl1->isChecked(), "check3: compl1 is not checked");
            CHECK_SET_ERR(compl2->isChecked(), "check3: compl2 is not checked");
            CHECK_SET_ERR(compl3->isChecked(), "check3: compl3 is not checked");
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(new PopupChecker(new AllPopupChecker()));
    GTWidget::click(translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters();
}
#undef GET_ACTIONS

GUI_TEST_CLASS_DEFINITION(test_0034) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto panView = GTWidget::findWidget("pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QImage init = GTWidget::getImage(panView);
    //    Create custom ruler
    GTUtilsDialog::add(new PopupChooser({"Rulers", "Create new ruler"}));
    GTUtilsDialog::add(new CreateRulerDialogFiller("name", 1000));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    QImage second = GTWidget::getImage(panView);
    CHECK_SET_ERR(init != second, "ruler not created");
    //    Hide ruler
    GTUtilsDialog::add(new PopupChooser({"Rulers", "Show Custom Rulers"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    //    Set focus on tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("human_T1.fa"));
    GTMouseDriver::click();
    second = GTWidget::getImage(panView);
    CHECK_SET_ERR(init == second, "ruler not hidden");
    //    Remove ruler
    GTUtilsDialog::add(new PopupChooserByText({"Rulers...", "Remove 'name'"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsDialog::add(new PopupChecker({"Rulers", "Show Custom Rulers"}, PopupChecker::IsDisabled));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    GTUtilsDialog::add(new PopupChecker({"Rulers", "Remove 'name'"}, PopupChecker::NotExists));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Double click on pan view
    auto panView = GTWidget::findWidget("pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(panView, Qt::LeftButton, QPoint(panView->rect().right() - 50, panView->rect().center().y()));
    GTMouseDriver::doubleClick();
    //    Expected: Sequence scrolled to clicked position
    auto det = GTWidget::findWidget("det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QScrollBar* scrollBar = det->findChild<QScrollBar*>();
    CHECK_SET_ERR(scrollBar->value() > 150000, QString("Unexpected value: %1").arg(scrollBar->value()));
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Select 2 annotations.
    GTUtilsAnnotationsTreeView::createAnnotation("new_group", "ann1", "10..20");
    GTUtilsAnnotationsTreeView::createAnnotation("new_group", "ann2", "40..50", false);
    GTUtilsAnnotationsTreeView::selectItemsByName({"ann1", "ann2"});
    //    Check "Sequence between selected annotations"  and
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence between selected annotations"}));
    GTMouseDriver::click(Qt::RightButton);
    QVector<U2Region> select = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(select.size() == 1, QString("Wrong number of selections: %1").arg(select.size()));
    U2Region s = select.first();
    CHECK_SET_ERR(s.startPos == 20, QString("Unexpected start pos: %1").arg(s.startPos));
    CHECK_SET_ERR(s.length == 19, QString("Unexpected selection length: %1").arg(s.length));
    //    "Sequence around selected annotations" actions
    GTUtilsDialog::waitForDialog(new PopupChooser({"Select", "Sequence around selected annotations"}));
    GTMouseDriver::click(Qt::RightButton);
    select = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(select.size() == 1, QString("Wrong number of selections: %1").arg(select.size()));
    s = select.first();
    CHECK_SET_ERR(s.startPos == 9, QString("Unexpected start pos: %1").arg(s.startPos));
    CHECK_SET_ERR(s.length == 41, QString("Unexpected selection length: %1").arg(s.length));
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Select any area
    GTUtilsSequenceView::selectSequenceRegion(10000, 11000);
    //    Press zoom to selection button
    GTUtilsDialog::waitForDialog(new ZoomToRangeDialogFiller());
    GTWidget::click(GTAction::button("zoom_to_range_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    auto pan = GTWidget::findExactWidget<PanView*>("pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    U2Region r = pan->getVisibleRange();
    CHECK_SET_ERR(r.startPos == 9999, QString("Unexpected start: %1").arg(r.startPos));
    CHECK_SET_ERR(r.length == 1001, QString("Unexpected length: %1").arg(r.length));
}

GUI_TEST_CLASS_DEFINITION(test_0037_1) {
    // Check defails "zoom to selection" dialog values
    //     Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Press zoom to selection button
    GTUtilsDialog::waitForDialog(new ZoomToRangeDialogFiller());
    GTWidget::click(GTAction::button("zoom_to_range_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    auto pan = GTWidget::findExactWidget<PanView*>("pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    U2Region r = pan->getVisibleRange();
    CHECK_SET_ERR(r.startPos == 0, QString("Unexpected start: %1").arg(r.startPos));
    CHECK_SET_ERR(r.length == 199950, QString("Unexpected length: %1").arg(r.length));
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto exportButton = GTWidget::findWidget("export_image");
    //    Export image
    //    GTUtilsDialog::waitForDialog(new ExportSequenceImage(sandBoxDir + "seq_view_test_0037_1.png"));
    //    GTWidget::click(exportButton);
    //    bool exists = GTFile::check(sandBoxDir + "seq_view_test_0037_1.png");
    //    CHECK_SET_ERR(exists, "Image not exported");
    //    GTGlobals::sleep(1000);

    ExportSequenceImage::Settings s = ExportSequenceImage::Settings(ExportSequenceImage::ZoomedView, U2Region(1, 1000));
    GTUtilsDialog::waitForDialog(new ExportSequenceImage(sandBoxDir + "seq_view_test_0037_1_1.png", s));
    GTWidget::click(exportButton);
    bool exists = GTFile::check(sandBoxDir + "seq_view_test_0037_1_1.png");
    CHECK_SET_ERR(exists, "Zoomed view not exported");

    //    s.type = ExportSequenceImage::DetailsView;
    //    GTUtilsDialog::waitForDialog(new ExportSequenceImage(sandBoxDir + "seq_view_test_0037_1_2.png", s));
    //    GTWidget::click(exportButton);
    //    exists = GTFile::check(sandBoxDir + "seq_view_test_0037_1_2.png");
    //    CHECK_SET_ERR(exists, "Details view not exported");
}

GUI_TEST_CLASS_DEFINITION(test_0040) {
    // scrollbar on seq view
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(wrapButton);
    GTUtilsProjectTreeView::toggleView(GTGlobals::UseKey);

    auto det = GTWidget::findExactWidget<DetView*>("det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QScrollBar* scroll = GTScrollBar::getScrollBar("singleline_scrollbar");

    GTWidget::click(scroll);
    U2Region r = det->getVisibleRange();
    CHECK_SET_ERR(r.startPos > 89, QString("Unexpected start pos: %1").arg(r.startPos));
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    // test key events
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(wrapButton);

    auto det = GTWidget::findExactWidget<DetView*>("det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(det);
    qint64 initLength = det->getVisibleRange().length;

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    int start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 1, QString("1 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 2, QString("2 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 1, QString("3 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Up);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 0, QString("4 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == initLength, QString("5 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 0, QString("6 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_End);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 199950 - initLength, QString("7 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Home);
    start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start == 0, QString("8 Unexpected sequence start: %1").arg(start));
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select an annotation.
    GTUtilsAnnotationsTreeView::clickItem("misc_feature", 1, true);

    // Expected: the annotation is selected.
    QString selected = GTUtilsAnnotationsTreeView::getSelectedItem();
    CHECK_SET_ERR(selected == "misc_feature", "Unexpected selected annotation: " + selected);

    // Click on the annotation in seq view with the right button.
    GTUtilsDialog::waitForDialog(new PopupChecker({ADV_MENU_EDIT, "edit_annotation_tree_item"}, PopupChecker::IsEnabled));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsDialog::waitForDialog(new PopupChecker({ADV_MENU_REMOVE, "Selected annotations and qualifiers"}, PopupChecker::IsEnabled));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    move mouse to annotation on det view
    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 2);
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(20, 0));
    QString tooltip = GTUtilsToolTip::getToolTip();
    QString expected = "<table><tr><td bgcolor=#ffff99 bordercolor=black width=15></td><td><big>misc_feature</big></td></tr><tr><td></td><td><b>Location"
                       "</b> = 2..590</td></tr><tr><td/><td><nobr><b>note</b> = 5' terminal repeat</nobr><br><nobr><b>Sequence</b> = AATGAAAGACCCCACCCGTAGGTGGCAAGCTAGCTTAAGT"
                       " ...</nobr><br><nobr><b>Translation</b> = NERPHP*VAS*LK ...</nobr></td></tr></table>";
    CHECK_SET_ERR(tooltip == expected, "Unexpected tooltip: " + tooltip)
}

GUI_TEST_CLASS_DEFINITION(test_0044) {
    // Overview wheel event
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto over = qobject_cast<Overview*>(GTWidget::findWidget("OverviewRenderArea")->parentWidget()->parentWidget());
    GTWidget::click(over);
    for (int i = 0; i < 10; i++) {
        GTMouseDriver::scroll(1);
    }

    U2Region r = GTUtilsSequenceView::getPanViewByNumber()->getVisibleRange();
    CHECK_SET_ERR(r.startPos > 90000, QString("Unexpected visible range: %1").arg(r.startPos));
}

GUI_TEST_CLASS_DEFINITION(test_0044_1) {
    // selection on overview
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    auto over = qobject_cast<Overview*>(GTWidget::findWidget("OverviewRenderArea")->parentWidget()->parentWidget());
    GTWidget::click(over);

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(100, 0));
    GTMouseDriver::release();
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTThread::waitForMainThread();

    QVector<U2Region> selectionVector = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(selectionVector.size() == 1, QString("unexpected number of selected regions: %1").arg(selectionVector.size()));
    CHECK_SET_ERR(selectionVector.first().length > 1000, QString("unexpected selection length: &1").arg(selectionVector.first().length))
}

GUI_TEST_CLASS_DEFINITION(test_0044_2) {
    // move slider
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    Overview* over = GTUtilsSequenceView::getOverviewByNumber();
    GTWidget::click(over);
    GTMouseDriver::doubleClick();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(0, over->geometry().height() / 3));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(over->geometry().width() / 3, 0));
    GTMouseDriver::release();

    int start = GTUtilsSequenceView::getVisibleStart();
    CHECK_SET_ERR(start > 150000, QString("Unexpected selection start: %1").arg(start));

    //    GTWidget::click(GTUtilsMdi::activeWindow());
    //    OverviewRenderArea* renderArea = GTWidget::findExactWidget<OverviewRenderArea*>("OverviewRenderArea", over);
    //    int detX = renderArea->posToCoord(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView()->getVisibleRange().startPos) - 6;
    //    QRect r = QRect(detX, 8, 12, 10);
    //    GTMouseDriver::moveTo(renderArea->mapToGlobal(r.center()));
    //    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0045) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* pan = GTUtilsSequenceView::getPanViewByNumber();
    QImage init = GTWidget::getImage(pan);
    // show restriction sites
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    auto qt_toolbar_ext_button = GTWidget::findWidget("qt_toolbar_ext_button", GTWidget::findWidget("mwtoolbar_activemdi"), {false});
    if (qt_toolbar_ext_button != nullptr && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(qt_toolbar_ext_button);
    }
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished();
    // show orfs
    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    if (qt_toolbar_ext_button != nullptr && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(qt_toolbar_ext_button);
    }
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QScrollBar*> list = pan->findChildren<QScrollBar*>();
    QScrollBar* vertical = nullptr;
    for (QScrollBar* b : qAsConst(list)) {
        if (b->orientation() == Qt::Vertical) {
            vertical = b;
        }
    }
    GTWidget::click(vertical);
    GTMouseDriver::scroll(5);
    QImage final = GTWidget::getImage(pan);
    CHECK_SET_ERR(init != final, "pan view was not changed")
}

GUI_TEST_CLASS_DEFINITION(test_0046) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    for (int i = 0; i < 5; i++) {
        GTWidget::click(GTAction::button("action_zoom_in_NC_001363"));
    }
    qint64 vis = GTUtilsSequenceView::getPanViewByNumber()->getVisibleRange().startPos;
    CHECK_SET_ERR(vis > 2300, QString("1 wrong visiable range start: %1").arg(vis));

    for (int i = 0; i < 5; i++) {
        GTWidget::click(GTAction::button("action_zoom_out_NC_001363"));
    }
    vis = GTUtilsSequenceView::getPanViewByNumber()->getVisibleRange().startPos;
    CHECK_SET_ERR(vis == 0, QString("2 wrong visiable range start: %1").arg(vis));

    for (int i = 0; i < 5; i++) {
        GTWidget::click(GTAction::button("action_zoom_in_NC_001363"));
    }
    vis = GTUtilsSequenceView::getPanViewByNumber()->getVisibleRange().startPos;
    CHECK_SET_ERR(vis > 2300, QString("3 wrong visiable range start: %1").arg(vis));

    GTWidget::click(GTAction::button("action_zoom_to_sequence_NC_001363"));
    vis = GTUtilsSequenceView::getPanViewByNumber()->getVisibleRange().startPos;
    CHECK_SET_ERR(vis == 0, QString("4 wrong visiable range start: %1").arg(vis));
}

GUI_TEST_CLASS_DEFINITION(test_0047) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    //    Open CV
    GTUtilsCv::cvBtn::click(GTUtilsSequenceView::getSeqWidgetByNumber());
    //    Use context menu on CV
    GTUtilsDialog::waitForDialog(new SetSequenceOriginDialogFiller(1000));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EDIT, "Set new sequence origin"}));
    GTWidget::click(GTWidget::findWidget("CV_ADV_single_sequence_widget_0"), Qt::RightButton);
    //    check "Set new sequence origin" action
    GTUtilsSequenceView::clickAnnotationDet("CDS", 43);
}

GUI_TEST_CLASS_DEFINITION(test_0048) {
    //    Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    //    Use context menu on annotation in tree view
    GTUtilsDialog::add(new PopupChooserByText({"Add", "Objects with annotations..."}));
    GTUtilsDialog::add(new ProjectTreeItemSelectorDialogFiller("murine.gb", "NC_001363 features"));
    GTUtilsDialog::add(new CreateObjectRelationDialogFiller());
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    //    Check {add-> Objects with annotations} action
    GTUtilsAnnotationsTreeView::findItem("NC_001363 features [murine.gb]");
}

GUI_TEST_CLASS_DEFINITION(test_0050) {
    class WrongNameChecker : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Illegal annotation name"));
            auto nameEdit = GTWidget::findLineEdit("leAnnotationName", dialog);
            GTLineEdit::setText(nameEdit, "//");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    class WrongDistanceChecker : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto gbFormatLocation = GTWidget::findRadioButton("rbGenbankFormat", dialog);
            GTRadioButton::click(gbFormatLocation);

            auto locationEdit = GTWidget::findLineEdit("leLocation", dialog);
            GTLineEdit::clear(locationEdit);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Invalid location"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(locationEdit, "1..");

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Invalid location"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(locationEdit, "1..0");
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Invalid location"));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::selectItems({GTUtilsAnnotationsTreeView::findItem("CDS")});

    GTUtilsDialog::waitForDialog(new EditAnnotationFiller(new WrongNameChecker));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    GTUtilsDialog::waitForDialog(new EditAnnotationFiller(new WrongDistanceChecker));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

GUI_TEST_CLASS_DEFINITION(test_0050_1) {
    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new PopupChooserByText({"Genes", "promoter"}));
            GTWidget::click(GTWidget::findWidget("showNameGroupsButton", dialog));
            auto nameEdit = GTWidget::findLineEdit("nameEdit", dialog);
            CHECK_SET_ERR(nameEdit->text() == "promoter", "unexpected name: " + nameEdit->text());

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAnnotationsTreeView::selectItems({GTUtilsAnnotationsTreeView::findItem("CDS")});

    GTUtilsDialog::waitForDialog(new EditAnnotationFiller(new custom));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

GUI_TEST_CLASS_DEFINITION(test_0051) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("OP_ANNOT_HIGHLIGHT"));

    QColor init = GTUtilsAnnotHighlightingTreeView::getItemColor("CDS");
    CHECK_SET_ERR(init.name() == "#9bffff", "unexpected CDS annotations color: " + init.name());

    GTUtilsDialog::waitForDialog(new ColorDialogFiller(255, 0, 0));
    GTUtilsAnnotHighlightingTreeView::click("CDS");
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(90, 0));
    GTMouseDriver::click();

    QColor final = GTUtilsAnnotHighlightingTreeView::getItemColor("CDS");
    CHECK_SET_ERR(final.name() == "#ff0000", "CDS annotations color changed wrong: " + init.name());
}

GUI_TEST_CLASS_DEFINITION(test_0052) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    DetView* det = GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView();
    GTWidget::click(det);
    QImage image1 = GTWidget::getImage(det);

    GTWidget::click(GTAction::button("complement_action"));
    GTWidget::click(det);
    QImage image2 = GTWidget::getImage(det);

    GTWidget::click(GTAction::button("complement_action"));
    GTWidget::click(det);
    QImage image3 = GTWidget::getImage(det);

    CHECK_SET_ERR(image1 != image2, "Image was not changed");
    CHECK_SET_ERR(image1 == image3, "Image was not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0052_1) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QWidget* sequenceArea = GTUtilsSequenceView::getDetViewByNumber(0)->getDetViewRenderArea();
    GTWidget::click(sequenceArea);
    QImage image1 = GTWidget::getImage(sequenceArea);

    GTUtilsDialog::waitForDialog(new PopupChooser({"show_all_frames_radiobutton"}));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));
    GTWidget::click(sequenceArea);
    QImage image2 = GTWidget::getImage(sequenceArea);

    GTUtilsDialog::waitForDialog(new PopupChooser({"do_not_translate_radiobutton"}));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));
    GTWidget::click(sequenceArea);
    QImage image3 = GTWidget::getImage(sequenceArea);

    CHECK_SET_ERR(image1 != image2, "Image was not changed");
    CHECK_SET_ERR(image1 == image3, "Image was not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0053) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Open any graph
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}, GTGlobals::UseMouse));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    GTUtilsDialog::checkNoActiveWaiters();

    //    Add label with shift+left mouse
    QWidget* graphView = GTUtilsSequenceView::getGraphView();
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTWidget::click(graphView, Qt::LeftButton, QPoint(50, 50));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    QList<QVariant> list = GTUtilsSequenceView::getLabelPositions(GTUtilsSequenceView::getGraphView());
    CHECK_SET_ERR(list.size() == 1, QString("unexpected number of labels: %1").arg(list.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0054) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Open any graph
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    //    Add label with shift+left mouse
    GSequenceGraphView* graphView = GTUtilsSequenceView::getGraphView();
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTWidget::click(graphView, Qt::LeftButton, QPoint(50, 50));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    //    Use context menu {graph->Graph settings}
    GTUtilsDialog::waitForDialog(new GraphSettingsDialogFiller(-1, -1, 0, 0, 255, 0, 0));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    GTWidget::click(graphView, Qt::RightButton);
    QColor c = GTUtilsSequenceView::getGraphColor(graphView);
    QString s = c.name();
    CHECK_SET_ERR(s == "#ff0000", "unexpected color: " + s);
    //    In settings dialog change label color
}

GUI_TEST_CLASS_DEFINITION(test_0055) {
    //    Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Open any graph
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    QWidget* graphView = GTUtilsSequenceView::getGraphView();
    GTWidget::click(graphView);
    // GTWidget::getAllWidgetsInfo(GTUtilsSequenceView::getGraphView());
    QWidget* pointer = nullptr;
    const QList<QWidget*> childList = GTUtilsSequenceView::getGraphView()->findChildren<QWidget*>();
    for (QWidget* w : qAsConst(childList)) {
        if (w->geometry().width() == 10) {
            pointer = w;
        }
    }
    CHECK_SET_ERR(pointer != nullptr, "pointer not found");

    //    Use context menu {graph->Graph settings}
    GTUtilsDialog::waitForDialog(new GraphSettingsDialogFiller(-1, -1, 10, 15));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    GTWidget::click(graphView, Qt::RightButton);
    GTWidget::click(GTUtilsSequenceView::getGraphView());
    QPoint p1 = pointer->mapToGlobal(pointer->geometry().center());

    GTUtilsDialog::waitForDialog(new GraphSettingsDialogFiller(-1, -1, 100, 150));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    GTWidget::click(graphView, Qt::RightButton);
    GTWidget::click(GTUtilsSequenceView::getGraphView());
    QPoint p2 = pointer->mapToGlobal(pointer->geometry().center());

    GTUtilsDialog::waitForDialog(new GraphSettingsDialogFiller(-1, -1, 10, 150));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    GTWidget::click(graphView, Qt::RightButton);
    GTWidget::click(GTUtilsSequenceView::getGraphView());
    QPoint p3 = pointer->mapToGlobal(pointer->geometry().center());

    int y1 = p1.y();
    int y2 = p2.y();
    int y3 = p3.y();
    CHECK_SET_ERR(y2 > y3 && y1 < y3, QString("unexpected pointer coordinates: %1 %2 %3").arg(y1).arg(y2).arg(y3));

    //    In settings dialog set proper cutoff values
}

GUI_TEST_CLASS_DEFINITION(test_0056) {
    // wrong cutoff values
    //     Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Open any graph
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    QWidget* graphView = GTUtilsSequenceView::getGraphView();

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Invalid cutoff range"));

            auto minmaxGroup = GTWidget::findGroupBox("minmaxGroup", dialog);
            GTGroupBox::setChecked(minmaxGroup);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new GraphSettingsDialogFiller(new custom));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "visual_properties_action"}));
    GTWidget::click(graphView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0057) {
    // Check that graph shows expected min/max label count.

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open GC Content (%).
    GTUtilsDialog::waitForDialog(new PopupChooser({"GC Content (%)"}));
    GTWidget::click(GTWidget::findWidget("GraphMenuAction"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Zoom in, so we have a readable picture for the label tests.
    QAction* zoomInAction = GTAction::findActionByText("Zoom In");
    for (int i = 0; i < 5; i++) {
        GTWidget::click(GTAction::button(zoomInAction));
    }

    // Use context menu {graph->Select all extremum points}.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Graph", "show_labels_for_min_max_points"}));
    GSequenceGraphView* graphView = GTUtilsSequenceView::getGraphView();
    GTWidget::click(graphView, Qt::RightButton);

    // Check labels count match the expected value.
    int labelCount = GTUtilsSequenceView::getGraphLabels(graphView).size();
    CHECK_SET_ERR(labelCount == 8, QString("Unexpected labels number: %1").arg(labelCount));
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    // complex test on local toolbars buttons

    GTFileDialog::openFile(dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAction* zoomIn = GTAction::findAction("action_zoom_in_A1#berezikov");

    GTWidget::click(GTAction::button(zoomIn));
    GTWidget::click(GTAction::button(zoomIn));
    GTWidget::click(GTAction::button(zoomIn));

    auto chromView = GTWidget::findWidget("chromatogram_view_A1#berezikov");

    QImage image = GTWidget::getImage(chromView);

    QAction* bars = GTAction::findActionByText("Show quality bars");
    GTWidget::click(GTAction::button(bars));

    CHECK_SET_ERR(image != GTWidget::getImage(chromView), "Nothing changed on Chromatogram View after Bars adding");

    image = GTWidget::getImage(chromView);

    QAction* traces = GTAction::findActionByText("Show/hide trace");

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"A"}));
    GTWidget::click(GTAction::button(traces));

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"C"}));
    GTWidget::click(GTAction::button(traces));

    CHECK_SET_ERR(image != GTWidget::getImage(chromView), "Nothing changed on Chromatogram View after Traces hiding");
}

GUI_TEST_CLASS_DEFINITION(test_0059) {
    //"Invert annotation selection" action test
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Invert annotation selection"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(GTUtilsAnnotationsTreeView::findItem("CDS"));
    QList<QTreeWidgetItem*> selected = GTUtilsAnnotationsTreeView::getAllSelectedItems();
    CHECK_SET_ERR(selected.size() == 7, QString("Unexpected number of selected items: %1").arg(selected.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0059_1) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new PopupChooser({"ORFs"}));
    GTWidget::click(GTWidget::findWidget("toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("CreateAnnotationDialog"));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Make auto-annotations persistent"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(GTUtilsAnnotationsTreeView::findItem("orf  (0, 27)"));

    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem("orf  (0, 27)");
    QString s = orf->parent()->text(0);
    CHECK_SET_ERR(s == "NC_001363 features [murine.gb] *", "unexpected parent: " + s)
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto nameLabel = GTWidget::findWidget("nameLabel");
    GTMouseDriver::moveTo(GTWidget::getWidgetCenter(nameLabel) + QPoint(2 * nameLabel->geometry().width() / 3, 0));
    GTMouseDriver::doubleClick();

    auto overViewSe2 = GTWidget::findWidget("overview_NC_001363", nullptr, false);
    auto DetailsViewSe2 = GTWidget::findWidget("det_view_NC_001363", nullptr, false);
    auto zoomViewSe2 = GTWidget::findWidget("pan_view_NC_001363", nullptr, false);
    auto toolBarSe2 = GTWidget::findWidget("tool_bar_NC_001363", nullptr, false);
    CHECK_SET_ERR(overViewSe2->isVisible() == false &&
                      DetailsViewSe2->isVisible() == false &&
                      zoomViewSe2->isVisible() == false,
                  "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0061_1) {
    // 1. Open "data/Samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Check that first sequence name is '1CF7 chain A sequence'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7 chain A sequence"));
    // 3. Check that first annotation table name is '1CF7 chain A annotation'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7 chain A annotation"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("1CF7 chain A annotation [1CF7.PDB]"));

    // 4. Check that there is 'molecule_name' qualifier with value 'PROTEIN (TRANSCRIPTION FACTOR E2F-4)'  in 'chain_info' annotation for chain A
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection().isEmpty(), "Selection is not empty");
    GTUtilsAnnotationsTreeView::selectItemsByName({"chain_info"});
    QString moleculeName = GTUtilsAnnotationsTreeView::getQualifierValue("molecule_name", GTUtilsAnnotationsTreeView::findItem("chain_info"));
    CHECK_SET_ERR(moleculeName == "PROTEIN (TRANSCRIPTION FACTOR E2F-4)", QString("Incorrect molecule name is detected for chain A: %1").arg(moleculeName));
    QTreeWidgetItem* annotationsChainB = GTUtilsAnnotationsTreeView::findItem("1CF7 chain B annotation [1CF7.PDB]");

    // 5. Check that there is 'molecule_name' qualifier with value 'PROTEIN (TRANSCRIPTION FACTOR DP-2)'  in 'chain_info' annotation for chain B
    QTreeWidgetItem* chainInfoGroup = GTUtilsAnnotationsTreeView::findItem("chain_info  (0, 1)", annotationsChainB);
    GTUtilsAnnotationsTreeView::selectItems({chainInfoGroup});
    QTreeWidgetItem* chainInfo = GTUtilsAnnotationsTreeView::findItem("chain_info", chainInfoGroup);
    GTUtilsAnnotationsTreeView::selectItems({chainInfo});
    QString moleculeNameChainB = GTUtilsAnnotationsTreeView::getQualifierValue("molecule_name", chainInfo);

    CHECK_SET_ERR(moleculeNameChainB == "PROTEIN (TRANSCRIPTION FACTOR DP-2)", QString("Incorrect molecule name is detected for chain B: %1").arg(moleculeNameChainB));
}

GUI_TEST_CLASS_DEFINITION(test_0061_2) {
    // 1. Open "_common_data/pdb/1CRN_without_compnd_tag.PDB"
    GTFileDialog::openFile(testDir + "_common_data/pdb", "1CRN_without_compnd_tag.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Check that first annotation table name is '1CRN chain A sequence'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CRN chain A annotation"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("1CRN chain A annotation [1CRN_without_compnd_tag.PDB]"));

    // 3. Check that there is 'chain_id' qualifier with value 'A'  in 'chain_info' annotation for chain A
    GTUtilsAnnotationsTreeView::selectItemsByName({"chain_info"});
    QString chainId = GTUtilsAnnotationsTreeView::getQualifierValue("chain_id", "chain_info");
    CHECK_SET_ERR("A" == chainId, QString("Incorrect 'chain_info' qualifier value: %1").arg(chainId));

    // 4. Check that there is not 'molecule_name' qualifier
    QTreeWidgetItem* moleculeName = GTUtilsAnnotationsTreeView::findItem("molecule_name", nullptr, {false});
    CHECK_SET_ERR(moleculeName == nullptr, QString("There is 'moleculeName' qualifier"));
}

GUI_TEST_CLASS_DEFINITION(test_0061_3) {
    // 1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Call context menu on the 3dview, select {Structural Alignment -> Align With...} menu item.
    // 3. Check that chain combobox contains identifiers: A, B, C, D.
    // 4. Accept the dialog.
    QStringList chainIndexes;
    chainIndexes << "A"
                 << "B"
                 << "C"
                 << "D";
    GTUtilsDialog::waitForDialog(new StructuralAlignmentDialogFiller(chainIndexes));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Structural Alignment", "align_with"}));
    auto widget3d = GTWidget::findWidget("1-1CF7");
    GTWidget::click(widget3d, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. "Wrap sequence" button is checked by default
    //    Expected state: vertical scrollbar is present, horizontal scrollbar is hidden, sequence is split into lines
    //    3. Uncheck "Wrap sequence" button on the local toolbar
    //    Expected state: the view is not resized, horizontal scrollbar appears, vertical scrollbar is hidden, sequence is in one line

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(seqWgt != nullptr, "No sequence widget found");
    QSize seqWgtSize = seqWgt->size();

    QScrollBar* scrollBar = GTScrollBar::getScrollBar("multiline_scrollbar");
    CHECK_SET_ERR(scrollBar != nullptr, "Cannot find multiline_scrollbar");
    CHECK_SET_ERR(scrollBar->isVisible(), "multiline_scrollbar is hidden");

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(wrapButton);
    CHECK_SET_ERR(seqWgtSize == seqWgt->size(), "Multi-line mode resized the view");
    CHECK_SET_ERR(scrollBar->isHidden(), "multiline_scrollbar is visible");

    QScrollBar* hScrollBar = GTScrollBar::getScrollBar("singleline_scrollbar");
    CHECK_SET_ERR(hScrollBar != nullptr, "Cannot find singleline_scrollbar");
    CHECK_SET_ERR(hScrollBar->isVisible(), "singleline_scrollbar is not visible");
}

GUI_TEST_CLASS_DEFINITION(test_0063) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. Scroll to the middle of the sequence (say visible range starts from X position)
    //    3. Uncheck "Wrap sequence"
    //    Expected state: visible range starts from X position
    //    4. Scroll a little (say visible range starts from Y position now in a single line mode)
    //    5. Check "Wrap sequence" back
    //    Expected state: sequence is displayed in lines, and first line contains Y pos

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    qint64 pos = 789;
    GTUtilsSequenceView::goToPosition(pos);
    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange();
    CHECK_SET_ERR(visibleRange.contains(pos), "Visible range does not contain 789 position");
    pos = visibleRange.startPos;

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(wrapButton);

    visibleRange = GTUtilsSequenceView::getVisibleRange();
    CHECK_SET_ERR(visibleRange.startPos == pos, "Visible range does not contain requeried position");
    GTUtilsSequenceView::goToPosition(pos + 20);
    visibleRange = GTUtilsSequenceView::getVisibleRange();
    pos = visibleRange.startPos;

    GTWidget::click(wrapButton);
    visibleRange = GTUtilsSequenceView::getVisibleRange();
    CHECK_SET_ERR(visibleRange.contains(pos), "Start position of visible range was changed");
}

GUI_TEST_CLASS_DEFINITION(test_0064) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. Scroll with the mouse wheel to the end of the sequence and back to the beginning

    GTFileDialog::openFile(testDir + "_common_data/fasta", "seq4.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    QScrollBar* scrollBar = GTScrollBar::getScrollBar("multiline_scrollbar");
    CHECK_SET_ERR(scrollBar != nullptr, "Cannot find multiline_scrollbar");
    GTScrollBar::moveSliderWithMouseWheelDown(scrollBar, scrollBar->maximum());

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange();
    CHECK_SET_ERR(visibleRange.contains(GTUtilsSequenceView::getSeqWidgetByNumber()->getSequenceLength() - 1), "The end position of the sequence is not visible. Failed to scroll to the end_1");

    GTScrollBar::moveSliderWithMouseWheelUp(scrollBar, scrollBar->maximum());

    visibleRange = GTUtilsSequenceView::getVisibleRange();
    CHECK_SET_ERR(visibleRange.contains(1), "The end position of the sequence is not visible. Failed to scroll to the end_2");
}

GUI_TEST_CLASS_DEFINITION(test_0065) {
    //    1. Open any sequence (e.g. murine.gb), "Wrap sequence"
    //    2. Set focus on the DetView
    //    3. Press down-arrow button a few times
    //    Expected state: the view is scrolled down a little
    //    4. Press up-arrow button a few times
    //    Expected state: the view is scrolled up a little
    //    5. Press page-down button
    //    Expected state: the sequence was scrolled to the next line
    //    6. Press page-up button
    //    Expected state: the sequence was scrolled to the previous line

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    GTWidget::setFocus(GTUtilsSequenceView::getSeqWidgetByNumber()->getDetView());

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange();
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    qint64 currentStartPos = GTUtilsSequenceView::getVisibleRange().startPos;
    CHECK_SET_ERR(visibleRange.startPos == currentStartPos, QString("Visible range was changed after Key_Down: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));

    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    currentStartPos = GTUtilsSequenceView::getVisibleRange().startPos;
    CHECK_SET_ERR(visibleRange.startPos == currentStartPos, QString("Visible range was changed after Key_Up: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));

    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    currentStartPos = GTUtilsSequenceView::getVisibleRange().startPos;
    CHECK_SET_ERR(visibleRange.startPos != currentStartPos, QString("Visible range was not changed after Key_PageDown: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));

    visibleRange = GTUtilsSequenceView::getVisibleRange();
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    currentStartPos = GTUtilsSequenceView::getVisibleRange().startPos;
    CHECK_SET_ERR(visibleRange.startPos != currentStartPos, QString("Visible range was not changed after Key_PageUp: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));
}

GUI_TEST_CLASS_DEFINITION(test_0066) {
    //    1. Open any sequence (e.g. sars.gb)
    //    2. Click "Wrap sequence"
    //    3. Enlarge the view at the bottom
    //    Expected state: more lines become visible at the bottom of the view, visible range starts from the same position
    //    4. Reduce the height of the view by the bottom splitter
    //    Expected state: lines at the bottom become hidden, visible range starts from the same position
    //    5.Enlarge the view at the top
    //    Expected state: more lines become visible at the bottom of the view, visible range starts from the same position
    //    6. Reduce the height of the view by the top splitter
    //    Expected state: lines at the bottom become hidden, visible range starts from the same position
    //    7. Scroll to the end of the sequence
    //    8. Enlarge at the top
    //    Expected state: more lines become visible at the top, the end of the sequence is still visible
    //    9. Enlarge at the bottom
    //    Expected state: more lines become visible at the top, the end of the sequence is still visible

    GTFileDialog::openFile(dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::goToPosition(1000);

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange();
    auto splitter = GTWidget::findSplitter("annotated_DNA_splitter");
    QWidget* bottomSplitterHandle = splitter->handle(splitter->count() - 1);  // GTWidget::findWidget("qt_splithandle_", GTWidget::findWidget("annotated_DNA_splitter"));
    CHECK_SET_ERR(bottomSplitterHandle != nullptr, "Cannot find bottom splitter handle");
    GTWidget::click(bottomSplitterHandle);
    QPoint p1 = GTMouseDriver::getMousePosition();
    QPoint p2 = QPoint(p1.x(), p1.y() + 100);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(), "Start position of visible range was changed on enlarge at the bottom");

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(), "Start position of visible range was changed on reduce at the bottom");

    auto topSplitterHandle = GTWidget::findWidget("qt_splithandle_det_view_NC_004718");
    GTWidget::click(topSplitterHandle);
    p1 = GTMouseDriver::getMousePosition();
    p2 = QPoint(p1.x(), p1.y() - 100);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(), "Start position of visible range was changed on enlarge at the top");

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(), "Start position of visible range was changed on reduce at the top");
}

GUI_TEST_CLASS_DEFINITION(test_0067) {
    //    1. Open very short sequence <50 (e.g. "_common_data/fasta/fa1.fa")
    //    2. Ensure that DetView is visible (click "Show detail view" button)
    //    3. Randomly click on an empty space after the end of the sequence
    //    4. Click "Wrap sequence"
    //    Expected state: the view looks the same, only the scroll bar was changed to vertical
    //    5. Randomly click on an empty space after the end of the sequence
    //    Export "Sequence details" to file
    //    6. Expected state: exported image have the whole sequence in it and has the minimum size to fit the sequence (no extra white space)

    GTFileDialog::openFile(testDir + "_common_data/fasta", "fa1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    auto showDetViewButton = GTWidget::findWidget("show_hide_details_view");
    GTWidget::click(showDetViewButton);

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(seqWgt != nullptr, "No sequence widget found");

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    QScrollBar* scrollBar = GTScrollBar::getScrollBar("singleline_scrollbar");
    CHECK_SET_ERR(scrollBar->isHidden(), "Horizontal scroll bar is visible");

    GTWidget::click(seqWgt);
    GTMouseDriver::doubleClick();
    GTWidget::click(wrapButton);
    GTWidget::click(seqWgt);
    GTMouseDriver::doubleClick();

    ExportSequenceImage::Settings s(ExportSequenceImage::DetailsView, U2Region(1, 3));
    GTUtilsDialog::waitForDialog(new ExportSequenceImage(sandBoxDir + "seq_image_0067", s));
    GTWidget::click(GTAction::button("export_image"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QPixmap p(sandBoxDir + "seq_image_0027");
    CHECK_SET_ERR(p.size() != QSize() && p.size() != seqWgt->getDetView()->getDetViewRenderArea()->size(), "Exported image size is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_0068) {
    //    1. Open any sequence (e.g. murine.gb)
    //    Optionally: enlarge the widget for a better view
    //    2. "Wrap seqeence" is on be default
    //    3. Uncheck "Show amino translations" button
    //    Expected state: the view is updated, the lines fill all available space
    //    4. Uncheck "Show complement strand"
    //    Expected state: the view is updated, the lines fill all available space
    //    5. Check "Show amino translations" button
    //    Expected state: the view is updated, the lines fill all available space
    //    6. Check "Show complement strand"
    //    Expected state: the view is updated, the lines fill all available space

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::getActiveSequenceViewWindow();
    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange();

    GTUtilsDialog::waitForDialog(new PopupChooser({"show_all_frames_radiobutton"}));
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(visibleRange != GTUtilsSequenceView::getVisibleRange(), "Visible range was not changed on translation show/hide");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_0069) {
    //   Open a sequence (e.g. murine.gb)
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that multi-line mode is enabled
    GTUtilsSequenceView::getActiveSequenceViewWindow();
    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection().isEmpty(), "Selection is not empty");

    DetView* seqWgt = GTUtilsSequenceView::getDetViewByNumber();
    CHECK_SET_ERR(seqWgt != nullptr, "Cannot find DetView widget");

    //   Move the mouse to the other line and release the button (make a selection)
    GTWidget::click(seqWgt);
    QPoint p1 = GTMouseDriver::getMousePosition();
    QPoint p2(p1.x() + 300, p1.y() + 200);

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    CHECK_SET_ERR(!GTUtilsSequenceView::getSelection().isEmpty(), "Nothing is selected");
}

GUI_TEST_CLASS_DEFINITION(test_0070) {
    //    1. Open any sequence with annotations (e.g. murine.gb)
    //    2. Click on annotation
    //    Expected state: annotation is selected
    //    3. Click "Wrap sequence"
    //    4. Enlarge the view to see a few lines of the sequence
    //    5. Click on a few annotations located on a different lines of the sequence
    //    Expected state: clicked annotation is selected

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(wrapButton);

    GTUtilsSequenceView::clickAnnotationDet("misc_feature", 2, 0, true);
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(!selection.isEmpty(), "Nothing is selected");
    GTWidget::click(wrapButton);

    GTUtilsSequenceView::clickAnnotationDet("CDS", 1042, 0, true);
    CHECK_SET_ERR(!GTUtilsSequenceView::getSelection().isEmpty(), "Selection is empty");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection() != selection, "Selection was not changed");
}

GUI_TEST_CLASS_DEFINITION(test_0071) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. Click "Export image"
    //    3. Fill the dialog (select a region from the middle of the sequence) and export the dialog
    //    Expected state: the result file contains the lines of the sequence started from the specified position, no extra empty space

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber();
    CHECK_SET_ERR(seqWgt != nullptr, "Cannot find sequence widget");

    QAbstractButton* wrapButton = GTAction::button("wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    ExportSequenceImage::Settings s(ExportSequenceImage::DetailsView, U2Region(1, 2000));
    GTUtilsDialog::waitForDialog(new ExportSequenceImage(sandBoxDir + "seq_image_0071", s));
    GTWidget::click(GTAction::button("export_image"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QPixmap p(sandBoxDir + "seq_image_0071");
    CHECK_SET_ERR(p.size() != QSize() && p.size() != seqWgt->getDetView()->getDetViewRenderArea()->size(), "Exported image size is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_0075) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click the translations toolbar button.
    GTWidget::click(GTWidget::findWidget("translationsMenuToolbarButton"));

    // Expected: the menu appears.
    auto menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    CHECK_SET_ERR(nullptr != menu, "No menu");

    // 3. Click "Show/hide translations".
    GTMenu::clickMenuItem(menu, "do_not_translate_radiobutton");

    // Expected: the menu doesn't disappear.
    CHECK_SET_ERR(nullptr != QApplication::activePopupWidget(), "Menu disappeared 1");

    // 4. Click "Show all".
    GTMenu::clickMenuItemByText(menu, {"Show all frames"});

    // Expected: the menu doesn't disappear.
    CHECK_SET_ERR(nullptr != QApplication::activePopupWidget(), "Menu disappeared 2");

    // 5. Click somewhere else.
    GTWidget::click(GTUtilsMdi::activeWindow());

    // Expected: the menu disappears.
    CHECK_SET_ERR(QApplication::activePopupWidget() == nullptr, "Menu is shown");
}

GUI_TEST_CLASS_DEFINITION(test_0076) {
    // UGENE-3267: Specifying a region when searching for restriction sites
    // 1. Open /_common_data/genbank/pBR322.gb
    // 2. Search the defauult set of the restriction site: "EcoRI"
    // Expected state: the EcoRI restriction site is found on the zero-end position
    // 4. Remove the circular mark
    // 5. Search for the restriction site again
    // Expected state: restriciton sites were recalculated and the is no annotation on zero position

    GTFileDialog::openFile(testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller({"EcoRI"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString("EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("SYNPBR322"));
    GTUtilsDialog::add(new PopupChooserByText({"Mark as circular"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller({"EcoRI"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("EcoRI", nullptr, {false}) == nullptr, "EcoRI is unexpectedly found");
}

GUI_TEST_CLASS_DEFINITION(test_0077) {
    // UGENE-3267: Specifying a region when searching for restriction sites
    // 1. Open /_common_data/genbank/pBR322.gb
    // 2. Open Find enzymes dialog
    // 3. Set search region: 4200..10 and find restriction sites
    // Expected state: there is only one result
    // 4. Open Find enzymes dialog
    // 6. Set search region: 3900..900 and exclude region 4300..10, find restriction sites
    // Expected state: no annotations on zero position

    GTFileDialog::openFile(testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    const QStringList defaultEnzymes = {"ClaI", "DraI", "EcoRI"};
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    FindEnzymesDialogFillerSettings settings(defaultEnzymes);
    settings.searchRegionStart = 4200;
    settings.searchRegionEnd = 10;
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("ClaI", nullptr, {false}) == nullptr, "ClaI is unexpectedly found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("DraI", nullptr, {false}) == nullptr, "DraI is unexpectedly found");
    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString("EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    settings.searchRegionStart = 3900;
    settings.searchRegionEnd = 300;
    settings.excludeRegionStart = 4300;
    settings.excludeRegionEnd = 10;
    GTUtilsDialog::add(new FindEnzymesDialogFiller(settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("EcoRI", nullptr, {false}) == nullptr, "EcoRI is unexpectedly found");
    GTUtilsAnnotationsTreeView::findItem("ClaI");
    GTUtilsAnnotationsTreeView::findItem("DraI");
}

GUI_TEST_CLASS_DEFINITION(test_0078) {
    // UGENE-3267: Specifying a region when searching for restriction sites
    // 1. Open /samples/Genbank/murine.gb
    // 2. Open Find enzymes dialog
    // 3. Set start search position to 5000, end position to 1000
    // Expected state: the line edits are red
    // 4. Click Ok
    // Expected state: message box appears
    // 5. Set start search position back to 1
    // 6. Check the exclude checkbox
    // 7. Click Ok
    // Expected state: message box appears

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class RegionSelectorChecker : public Filler {
    public:
        RegionSelectorChecker()
            : Filler("FindEnzymesDialog") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto regionSelector = GTWidget::findWidget("region_selector_with_excluded");

            auto start = GTWidget::findLineEdit("startLineEdit", regionSelector);
            GTLineEdit::setText(start, "5000");

            auto end = GTWidget::findLineEdit("endLineEdit", regionSelector);
            GTLineEdit::setText(end, "1000");

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(start, "1");

            auto exclude = GTWidget::findCheckBox("excludeCheckBox");
            GTCheckBox::setChecked(exclude);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new RegionSelectorChecker());
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_0079_1) {
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTComboBoxWithCheckBoxes::selectItemByText("cbSuppliers", dialog, {"Not defined"}, GTGlobals::UseMouse);

            auto text = GTLabel::getText("statusLabel", dialog);
            CHECK_SET_ERR(text.contains("Total number of enzymes: 15510, selected 0"), QString("Unexpected text: %1").arg(text));

            auto chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts("cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 1, QString("Current checked size: %1").arg(chekedValues.size()));
            CHECK_SET_ERR(chekedValues.first() == "Not defined", QString("Current checked value: %1").arg(chekedValues.first()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList {}, new custom()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find restriction sites");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0079_2) {
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTWidget::click(GTWidget::findWidget("pbSelectNone"));
            auto chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts("cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 0, QString("Current checked size after pbSelectNone: %1").arg(chekedValues.size()));

            GTWidget::click(GTWidget::findWidget("pbSelectAll"));
            chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts("cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 16, QString("Current checked size after pbSelectAll: %1").arg(chekedValues.size()));

            GTComboBoxWithCheckBoxes::selectItemByText("cbSuppliers", dialog, {"Not defined", "Thermo Fisher Scientific"});
            chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts("cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 2, QString("Current checked size after pbSelectAll: %1").arg(chekedValues.size()));

            GTWidget::click(GTWidget::findWidget("pbInvertSelection"));
            chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts("cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 14, QString("Current checked size after pbSelectAll: %1").arg(chekedValues.size()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList {}, new custom()));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Find restriction sites");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0080) {
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    class custom : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/enzymes/all_possible_tooltips.bairoch"));
            GTWidget::click(GTWidget::findWidget("enzymesFileButton", dialog));

            auto tree = GTWidget::findTreeWidget("tree", dialog);
            for (int i = 0; i < 22; i++) {
                auto id = QString::number(i);
                if (id.size() == 1) {
                    id = "0" + id;
                }
                auto name = "A" + id;
                auto item = GTTreeWidget::findItem(tree, name);
                auto tooltip = item->data(0, Qt::ToolTipRole).toString();
                auto toltipFromFile = GTFile::readAll(testDir + "_common_data/enzymes/tooltips/" + name + ".html");
                CHECK_SET_ERR(tooltip == toltipFromFile, QString("Incorrect tooltip").arg(name));
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller(QStringList {}, new custom()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Analyze", "Find restriction sites..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

}  // namespace GUITest_common_scenarios_sequence_view

}  // namespace U2
