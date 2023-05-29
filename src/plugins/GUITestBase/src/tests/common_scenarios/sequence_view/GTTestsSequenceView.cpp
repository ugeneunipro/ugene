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
#include "runnables/ugene/corelibs/U2Gui/GraphSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/GTComboBoxWithCheckBoxes.h"
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

    GTFileDialog::openFile(os, dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    auto w3 = GTWidget::findWidget(os, "ADV_single_sequence_widget_3");

    auto button0 = GTWidget::findWidget(os, "CircularViewAction", w0, false);
    CHECK_SET_ERR(button0 == nullptr, "b0 is not NULL");

    auto button1 = GTWidget::findWidget(os, "CircularViewAction", w1, false);
    CHECK_SET_ERR(button1 == nullptr, "b1 is not NULL");

    GTWidget::findWidget(os, "CircularViewAction", w2);
    GTWidget::findWidget(os, "CircularViewAction", w3);
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTFileDialog::openFile(os, dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    auto w3 = GTWidget::findWidget(os, "ADV_single_sequence_widget_3");

    QAbstractButton* b0 = GTAction::button(os, "complement_action", w0, false);
    CHECK_SET_ERR(b0 == nullptr, "b0 is not NULL");

    QAbstractButton* b1 = GTAction::button(os, "complement_action", w1, false);
    CHECK_SET_ERR(b1 == nullptr, "b1 is not NULL");

    GTAction::button(os, "complement_action", w2);
    GTAction::button(os, "complement_action", w3);
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    GTFileDialog::openFile(os, dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    auto w3 = GTWidget::findWidget(os, "ADV_single_sequence_widget_3");

    auto b0 = GTWidget::findWidget(os, "translationsMenuToolbarButton", w0, false);
    CHECK_SET_ERR(b0 == nullptr, "b0 is not NULL");

    auto b1 = GTWidget::findWidget(os, "translationsMenuToolbarButton", w1, false);
    CHECK_SET_ERR(b1 == nullptr, "b1 is not NULL");

    GTWidget::findWidget(os, "translationsMenuToolbarButton", w2);
    GTWidget::findWidget(os, "translationsMenuToolbarButton", w3);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Unnecessary buttons are enabled in zoom view (0001251)

    //    1. Open file samples/FASTA/human_T1.fa

    //    2. Hide details view with button {Toggle view->Hide details view}, if it already hided skip this step.
    //    Expected state: buttons 'Show amino translations' and 'Show amino strand' are disabled

    //    3. Show details view with button {Toggle view->Show details view}
    //    Expected state: buttons 'Show amino translations' and 'Show amino strand' becomes enabled

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    Runnable* chooser = new PopupChooser(os, {"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, toggleViewButton);

    QAbstractButton* complement = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement->isEnabled() == false, "button is not disabled");

    Runnable* chooser1 = new PopupChooser(os, {"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(os, chooser1);
    GTWidget::click(os, toggleViewButton);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    QAbstractButton* complement1 = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement1->isEnabled() == true, "button is not enabled");
}
GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "mwtoolbar_activemdi"));
    if (ext_button != nullptr && ext_button->isVisible()) {
        GTWidget::click(os, ext_button);
    }

    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    Runnable* chooser = new PopupChooser(os, {"toggleAllSequenceViews"});
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, toggleViewButton);

    QAbstractButton* complement = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement->isEnabled() == false, "button is not disabled");

    Runnable* chooser1 = new PopupChooser(os, {"toggleAllSequenceViews"});
    GTUtilsDialog::waitForDialog(os, chooser1);
    GTWidget::click(os, toggleViewButton);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    QAbstractButton* complement1 = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement1->isEnabled() == true, "button is not enabled");
}
GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    Runnable* chooser = new PopupChooser(os, {"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, toggleViewButton);

    QAbstractButton* complement = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement->isEnabled() == false, "button is not disabled");

    Runnable* chooser1 = new PopupChooser(os, {"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(os, chooser1);
    GTWidget::click(os, toggleViewButton);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {"do_not_translate_radiobutton"}, PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    CHECK_SET_ERR(complement->isEnabled() == true, "button is not disabled");

    Runnable* chooser2 = new PopupChooser(os, {"toggleDetailsView"});
    GTUtilsDialog::waitForDialog(os, chooser2);
    GTWidget::click(os, toggleViewButton);

    QAbstractButton* complement1 = GTAction::button(os, "complement_action");
    CHECK_SET_ERR(complement1->isEnabled() == false, "button is not disabled");
}
GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    UGENE crashes on subsequence export (0002423)

    //    1. Open sequence

    //    2. Select some region

    //    3. Call context menu "Export -> Export selected sequence region"

    //    4. Press Export button in dialog.
    //    Expected state: UGENE not crashed

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 3);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {  // CHANGES: keyboard used instead mouse

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 3);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));
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
        AllActionsPopupChooser(HI::GUITestOpStatus& _os)
            : PopupChooser(_os, QStringList()) {
        }
        void run() override {
            auto activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());

            int i = 0;
            QList<QAction*> act = activePopupMenu->actions();
            for (QAction* a : qAsConst(act)) {
                QString s = a->objectName();
                if (i == 1) {
                    GTMenu::clickMenuItem(os, activePopupMenu, s, GTGlobals::UseMouse);
                }
                i++;
            }
        }
    };

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsDocument::checkDocument(os, "multy_fa.fa");

    auto toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    CHECK_SET_ERR(toggleAutoAnnotationsButton->isEnabled() == false, "toggleAutoAnnotationsButton is enabled, expected disabled");

    GTUtilsProject::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsDocument::checkDocument(os, "human_T1.fa");

    toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    CHECK_SET_ERR(toggleAutoAnnotationsButton->isEnabled() == true, "toggleAutoAnnotationsButton is disabled, expected enabled");

    GTUtilsDialog::waitForDialog(os, new AllActionsPopupChooser(os));
    GTWidget::click(os, toggleAutoAnnotationsButton);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // 1. Enable Auto-annotations
    //      GTGlobals::sleep();
    //      GTGlobals::sleep();
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    //  !!! dirty fastfix of test, very temporary
    auto tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
    //

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    // 2. Open any PBD file from samples
    // Expected state: there is no auto-annotations for opened sequences

    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem(os, "enzymes", nullptr, {false});
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orfs", nullptr, {false});
    CHECK_SET_ERR(enz == nullptr, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == nullptr, "orfs unexpectedly present");
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    // 1. Enable Auto-annotations
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    //  !!! dirty fastfix of test, very temporary
    auto tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
    //

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    // 1. Disable Auto-annotations
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    // 3. Enable Auto-annotations
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    // 2. Open any PBD file from samples
    // Expected state: there is no auto-annotations for opened sequences
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: there is no auto-annotations for opened sequences
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem(os, "enzymes", nullptr, {false});
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orfs", nullptr, {false});
    CHECK_SET_ERR(enz == nullptr, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == nullptr, "orfs unexpectedly present");
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    // 2. Open any PBD file from samples
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 1. Enable Auto-annotations
    auto toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    //  !!! dirty fastfix of test, very temporary
    auto tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
    //

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);

    // Expected state: there is no auto-annotations for opened sequences
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem(os, "enzymes", nullptr, {false});
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orfs", nullptr, {false});
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
    GTUtilsDialog::waitForDialog(os, new GTSequenceReadingModeDialogUtils(os));
    GTFileDialog::openFileList(os, testDir + "_common_data/fasta/", {"DNA.fa", "DNA_1_seq.fa"});

    int length = GTUtilsSequenceView::getLengthOfSequence(os);
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

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "linear_circular_results.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(os, new Primer3DialogFiller(os));
    GTWidget::click(os, GTUtilsSequenceView::getPanOrDetView(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {8, 27}, {105, 124} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {8, 27}, {135, 154} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {4, 24}, {105, 124} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {8, 27}, {137, 156} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {8, 27}, {113, 132} });
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    //     1. Open '_common_data/primer3/circular_primers.gb
    //     2. Select 1..16,151..351 (region with junction point)
    //     3. Launch Primer3 with default settings (it will use the selected region)
    //     Expected state: the sequence is circular - a few primers cover junction point

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "circular_primers.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsSequenceView::selectSeveralRegionsByDialog(os, "1..16,151..351");

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(os, new Primer3DialogFiller(os));
    GTWidget::click(os, GTUtilsSequenceView::getPanOrDetView(os), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {221, 241}, {335, 351}, {1, 3} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {221, 241}, {336, 351}, {1, 5} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {220, 241}, {335, 351}, {1, 3} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {221, 241}, {333, 351} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {221, 241}, {333, 351}, {1, 1} });
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    //    1. Open 'human_T1.fa'
    //    2. Open CV
    //    3. Select big region covering junction point
    //    4. Launch Primer3 search (set results count to 50)
    //    Expected state: primers are located on both selected regions(start and end).

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsProjectTreeView::markSequenceAsCircular(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, "150000..199950,1..50000"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, { "Select", "Sequence region" }));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    Primer3DialogFiller::Primer3Settings settings;
    settings.resultsCount = 50;
    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));    
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Primer3");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {22172, 22191}, {22369, 22388} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 10  (0, 2)", { {185965, 185984}, {186089, 186108} });
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    //    1. Open '_common_data/primer3/DNA.gb'
    //    2. Select region that cover junction point
    //    3. Launch Primer3 search with default settings
    //    Expected state: primers(left and right) are located on either side of junction point

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction"));

    if (!GTUtilsSequenceView::getPanOrDetView(os)->isVisible()) {
        auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
        GTUtilsDialog::add(os, new PopupChooser(os, {"toggleZoomView"}));
        GTWidget::click(os, toggleViewButton);
    }

    GTUtilsDialog::add(os, new SelectSequenceRegionDialogFiller(os, "560..743,1..180"));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTThread::waitForMainThread();

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "primer3_action"}));
    GTUtilsDialog::add(os, new Primer3DialogFiller(os));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {95, 114}, {685, 704} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {142, 161}, {685, 704} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {143, 162}, {685, 704} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {143, 162}, {607, 626} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {47, 66}, {685, 704} });
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    //    1. Open 'common_data/primer3/DNA.gb'
    //    2. Open Primer3 dialog
    //    3. Set region to circular: 560..(743,1)..180
    //    4. Accept the dialog
    //    Expected state: no warning apeared, primers are located on selected region
    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "primer3_action"}));
    Primer3DialogFiller::Primer3Settings settings;
    settings.start = 560;
    settings.end = 180;
    GTUtilsDialog::add(os, new Primer3DialogFiller(os, settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 1  (0, 2)", { {95, 114}, {685, 704} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 2  (0, 2)", { {142, 161}, {685, 704} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 3  (0, 2)", { {143, 162}, {685, 704} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 4  (0, 2)", { {143, 162}, {607, 626} });
    GTUtilsAnnotationsTreeView::checkAnnotationRegions(os, "pair 5  (0, 2)", { {47, 66}, {685, 704} });
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    //    Default export
    //    1. Open sars.gb
    //    2. Export sequence as image

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_image_0027"));
    GTWidget::click(os, GTAction::button(os, "export_image"));
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    //    Check region selector appearance
    //    Region selector is hidden if CurrentView export is selected

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class RegionChecker : public Filler {
    public:
        RegionChecker(HI::GUITestOpStatus& os)
            : Filler(os, "ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTWidget::clickWindowTitle(os, dialog);

            auto radioButton = GTWidget::findRadioButton(os, "currentViewButton", dialog);
            GTRadioButton::click(os, radioButton);

            auto rangeSelector = GTWidget::findWidget(os, "range_selector", dialog);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is visible");

            radioButton = GTWidget::findRadioButton(os, "zoomButton", dialog);
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(rangeSelector->isVisible(), "range_selector is hidden");

            radioButton = GTWidget::findRadioButton(os, "detailsButton", dialog);
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(rangeSelector->isVisible(), "range_selector is hidden");

            radioButton = GTWidget::findRadioButton(os, "currentViewButton", dialog);
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is hidden");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new RegionChecker(os));
    GTWidget::click(os, GTAction::button(os, "export_image"));
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

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);

    QAction* zoom = GTAction::findActionByText(os, "Zoom In");
    CHECK_SET_ERR(zoom != nullptr, "Cannot find Zoom In action");
    for (int i = 0; i < 8; i++) {
        GTWidget::click(os, GTAction::button(os, zoom));
    }

    class LimitsChecker : public Filler {
    public:
        LimitsChecker(HI::GUITestOpStatus& os)
            : Filler(os, "ImageExportForm") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTWidget::clickWindowTitle(os, dialog);

            auto rangeSelector = GTWidget::findWidget(os, "range_selector", dialog);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is visible");

            auto box = GTWidget::findDialogButtonBox(os, "buttonBox", dialog);
            QPushButton* okbutton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okbutton != nullptr, "ok button is NULL");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            auto radioButton = GTWidget::findRadioButton(os, "zoomButton", dialog);
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            auto end = GTWidget::findLineEdit(os, "end_edit_line", dialog);
            GTLineEdit::setText(os, end, "100");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            radioButton = GTWidget::findRadioButton(os, "detailsButton", dialog);
            GTRadioButton::click(os, radioButton);
            GTLineEdit::setText(os, end, "10000");
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            GTLineEdit::setText(os, end, "100");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button != nullptr, "cancel button is NULL");
            GTWidget::click(os, button);
        }
    };

    GTUtilsDialog::waitForDialog(os, new LimitsChecker(os));
    GTWidget::click(os, GTAction::button(os, "export_image"));
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

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, {"YkrI"}));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Analyze", "Find restriction sites..."}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class SvgLimitsChecker : public Filler {
    public:
        SvgLimitsChecker(HI::GUITestOpStatus& os)
            : Filler(os, "ImageExportForm") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            // set SVG format
            auto formatsBox = GTWidget::findComboBox(os, "formatsBox", dialog);
            GTComboBox::selectItemByText(os, formatsBox, "SVG");

            // export is not available
            auto buttonBox = GTWidget::findDialogButtonBox(os, "buttonBox", dialog);

            QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okButton != nullptr, "ok button is NULL");
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is expectedly enabled");

            // select Details -- export is disabled
            auto radioButton = GTWidget::findRadioButton(os, "detailsButton", dialog);
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is unexpectedly enabled");

            auto end = GTWidget::findLineEdit(os, "end_edit_line", dialog);
            GTLineEdit::setText(os, end, "2300");
            CHECK_SET_ERR(okButton->isEnabled(), "Export button is unexpectedly disabled");

            // set Zoom view export
            radioButton = GTWidget::findRadioButton(os, "zoomButton", dialog);
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(okButton->isEnabled(), "Export button is unexpectedly disabled");
            GTLineEdit::setText(os, end, "199000");
            CHECK_SET_ERR(!okButton->isEnabled(), "Export button is unexpectedly enabled");

            // select a fewer region -- export is enabled
            GTLineEdit::setText(os, end, "10000");
            CHECK_SET_ERR(okButton->isEnabled(), "Export button is unexpectedly disabled");

            auto fileEdit = GTWidget::findLineEdit(os, "fileNameEdit", dialog);
            GTLineEdit::setText(os, fileEdit, sandBoxDir + "seq_view_test_0030.svg");

            GTWidget::click(os, okButton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SvgLimitsChecker(os), 180000);
    GTWidget::click(os, GTAction::button(os, "export_image"));
    GTUtilsDialog::checkNoActiveWaiters(os, 180000);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    qint64 fileSize = GTFile::getSize(os, sandBoxDir + "seq_view_test_0030.svg");
    CHECK_SET_ERR(fileSize > 15 * 1024 * 1024, QString("SVG file is too small: %1").arg(fileSize));
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    Select some redion
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "Copy reverse complement sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext == "AACTTTGGTGA", "Unexpected reverse complement: " + clipboardtext)
}

GUI_TEST_CLASS_DEFINITION(test_0031_1) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    Select some redion
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "Copy reverse complement sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext == "AACTTTGGTGA", "Unexpected reverse complement: " + clipboardtext)
}

GUI_TEST_CLASS_DEFINITION(test_0031_2) {
    //    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    Select annotation
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("AATGAAAGAC"), "Unexpected reverse complement start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("GTCTTTCATT"), "Unexpected reverse complement end: " + clipboardtext.right(10));

    //    Check joined annotations
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 2, true);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("ATGGTAGCAG"), "Unexpected reverse complement for joined annotation start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("TCTAGACTGA"), "Unexpected reverse complement for joined annotation end: " + clipboardtext.right(10));
}

GUI_TEST_CLASS_DEFINITION(test_0031_3) {
    //    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    Select annotation
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
    //    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("AATGAAAGAC"), "Unexpected reverse complement start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("GTCTTTCATT"), "Unexpected reverse complement end: " + clipboardtext.right(10));

    //    Check joined annotations
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 2, true);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_COPY, "action_copy_annotation_sequence"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("ATGGTAGCAG"), "Unexpected reverse complement for joined annotation start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("TCTAGACTGA"), "Unexpected reverse complement for joined annotation end: " + clipboardtext.right(10));
}

GUI_TEST_CLASS_DEFINITION(test_0032) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  Click "Amino translations" button on mdi toolbar
    auto translationsMenuToolbarButton = GTWidget::findWidget(os, "translationsMenuToolbarButton");

    class UncheckComplement : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu(os);
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Set up frames manually"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame -1"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame -2"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame -3"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new UncheckComplement()));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters(os);

    //  Check "Show direct only"
    class DirectPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu(os);
            QAction* direct1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +1", true);
            QAction* direct2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +2", true);
            QAction* direct3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +3", true);
            QAction* compl1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -1", true);
            QAction* compl2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -2", true);
            QAction* compl3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -3", true);

            CHECK_SET_ERR(direct1->isChecked(), "check1: direct1 is not checked");
            CHECK_SET_ERR(direct2->isChecked(), "check1: direct2 is not checked");
            CHECK_SET_ERR(direct3->isChecked(), "check1: direct3 is not checked");
            CHECK_SET_ERR(!compl1->isChecked(), "check1: compl1 is checked");
            CHECK_SET_ERR(!compl2->isChecked(), "check1: compl2 is checked");
            CHECK_SET_ERR(!compl3->isChecked(), "check1: compl3 is checked");

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new DirectPopupChecker()));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Check "Show complement only"
    class UncheckDirectCheckComplement : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu(os);
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame +1"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame +2"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame +3"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame -1"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame -2"});
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Frame -3"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new UncheckDirectCheckComplement()));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters(os);

    class ComplPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu(os);
            QAction* direct1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +1", true);
            QAction* direct2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +2", true);
            QAction* direct3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +3", true);
            QAction* compl1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -1", true);
            QAction* compl2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -2", true);
            QAction* compl3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -3", true);

            CHECK_SET_ERR(!direct1->isChecked(), "check2: direct1 is checked");
            CHECK_SET_ERR(!direct2->isChecked(), "check2: direct2 is checked");
            CHECK_SET_ERR(!direct3->isChecked(), "check2: direct3 is checked");
            CHECK_SET_ERR(compl1->isChecked(), "check2: compl1 is not checked");
            CHECK_SET_ERR(compl2->isChecked(), "check2: compl2 is not checked");
            CHECK_SET_ERR(compl3->isChecked(), "check2: compl3 is not checked");
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new ComplPopupChecker()));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Check "Show all".
    class ShowAllFramesScenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu(os);
            GTMenu::clickMenuItemByText(os, activePopupMenu, {"Show all frames"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new ShowAllFramesScenario()));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters(os);

    // Check results of Show all frames.
    class AllPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            QMenu* activePopupMenu = GTWidget::getActivePopupMenu(os);
            QAction* direct1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +1", true);
            QAction* direct2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +2", true);
            QAction* direct3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +3", true);
            QAction* compl1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -1", true);
            QAction* compl2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -2", true);
            QAction* compl3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -3", true);

            CHECK_SET_ERR(direct1->isChecked(), "check3: direct1 is checked");
            CHECK_SET_ERR(direct2->isChecked(), "check3: direct2 is checked");
            CHECK_SET_ERR(direct3->isChecked(), "check3: direct3 is checked");
            CHECK_SET_ERR(compl1->isChecked(), "check3: compl1 is not checked");
            CHECK_SET_ERR(compl2->isChecked(), "check3: compl2 is not checked");
            CHECK_SET_ERR(compl3->isChecked(), "check3: compl3 is not checked");
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new AllPopupChecker()));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTUtilsDialog::checkNoActiveWaiters(os);
}
#undef GET_ACTIONS

GUI_TEST_CLASS_DEFINITION(test_0034) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto panView = GTWidget::findWidget(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QImage init = GTWidget::getImage(os, panView);
    //    Create custom ruler
    GTUtilsDialog::add(os, new PopupChooser(os, {"Rulers", "Create new ruler"}));
    GTUtilsDialog::add(os, new CreateRulerDialogFiller(os, "name", 1000));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
    QImage second = GTWidget::getImage(os, panView);
    CHECK_SET_ERR(init != second, "ruler not created");
    //    Hide ruler
    GTUtilsDialog::add(os, new PopupChooser(os, {"Rulers", "Show Custom Rulers"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
    //    Set focus on tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa"));
    GTMouseDriver::click();
    second = GTWidget::getImage(os, panView);
    CHECK_SET_ERR(init == second, "ruler not hidden");
    //    Remove ruler
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Rulers...", "Remove 'name'"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsDialog::add(os, new PopupChecker(os, {"Rulers", "Show Custom Rulers"}, PopupChecker::IsDisabled));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
    GTUtilsDialog::add(os, new PopupChecker(os, {"Rulers", "Remove 'name'"}, PopupChecker::NotExists));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Double click on pan view
    auto panView = GTWidget::findWidget(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, panView, Qt::LeftButton, QPoint(panView->rect().right() - 50, panView->rect().center().y()));
    GTMouseDriver::doubleClick();
    //    Expected: Sequence scrolled to clicked position
    auto det = GTWidget::findWidget(os, "det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QScrollBar* scrollBar = det->findChild<QScrollBar*>();
    CHECK_SET_ERR(scrollBar->value() > 150000, QString("Unexpected value: %1").arg(scrollBar->value()));
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    //    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Select 2 annotations.
    GTUtilsAnnotationsTreeView::createAnnotation(os, "new_group", "ann1", "10..20");
    GTUtilsAnnotationsTreeView::createAnnotation(os, "new_group", "ann2", "40..50", false);
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"ann1", "ann2"});
    //    Check "Sequence between selected annotations"  and
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence between selected annotations"}));
    GTMouseDriver::click(Qt::RightButton);
    QVector<U2Region> select = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(select.size() == 1, QString("Wrong number of selections: %1").arg(select.size()));
    U2Region s = select.first();
    CHECK_SET_ERR(s.startPos == 20, QString("Unexpected start pos: %1").arg(s.startPos));
    CHECK_SET_ERR(s.length == 19, QString("Unexpected selection length: %1").arg(s.length));
    //    "Sequence around selected annotations" actions
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Select", "Sequence around selected annotations"}));
    GTMouseDriver::click(Qt::RightButton);
    select = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(select.size() == 1, QString("Wrong number of selections: %1").arg(select.size()));
    s = select.first();
    CHECK_SET_ERR(s.startPos == 9, QString("Unexpected start pos: %1").arg(s.startPos));
    CHECK_SET_ERR(s.length == 41, QString("Unexpected selection length: %1").arg(s.length));
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Select any area
    GTUtilsSequenceView::selectSequenceRegion(os, 10000, 11000);
    //    Press zoom to selection button
    GTUtilsDialog::waitForDialog(os, new ZoomToRangeDialogFiller(os));
    GTWidget::click(os, GTAction::button(os, "zoom_to_range_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    auto pan = GTWidget::findExactWidget<PanView*>(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    U2Region r = pan->getVisibleRange();
    CHECK_SET_ERR(r.startPos == 9999, QString("Unexpected start: %1").arg(r.startPos));
    CHECK_SET_ERR(r.length == 1001, QString("Unexpected length: %1").arg(r.length));
}

GUI_TEST_CLASS_DEFINITION(test_0037_1) {
    // Check defails "zoom to selection" dialog values
    //     Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Press zoom to selection button
    GTUtilsDialog::waitForDialog(os, new ZoomToRangeDialogFiller(os));
    GTWidget::click(os, GTAction::button(os, "zoom_to_range_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    auto pan = GTWidget::findExactWidget<PanView*>(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    U2Region r = pan->getVisibleRange();
    CHECK_SET_ERR(r.startPos == 0, QString("Unexpected start: %1").arg(r.startPos));
    CHECK_SET_ERR(r.length == 199950, QString("Unexpected length: %1").arg(r.length));
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto exportButton = GTWidget::findWidget(os, "export_image");
    //    Export image
    //    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_view_test_0037_1.png"));
    //    GTWidget::click(os, exportButton);
    //    bool exists = GTFile::check(os, sandBoxDir + "seq_view_test_0037_1.png");
    //    CHECK_SET_ERR(exists, "Image not exported");
    //    GTGlobals::sleep(1000);

    ExportSequenceImage::Settings s = ExportSequenceImage::Settings(ExportSequenceImage::ZoomedView, U2Region(1, 1000));
    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_view_test_0037_1_1.png", s));
    GTWidget::click(os, exportButton);
    bool exists = GTFile::check(os, sandBoxDir + "seq_view_test_0037_1_1.png");
    CHECK_SET_ERR(exists, "Zoomed view not exported");

    //    s.type = ExportSequenceImage::DetailsView;
    //    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_view_test_0037_1_2.png", s));
    //    GTWidget::click(os, exportButton);
    //    exists = GTFile::check(os, sandBoxDir + "seq_view_test_0037_1_2.png");
    //    CHECK_SET_ERR(exists, "Details view not exported");
}

GUI_TEST_CLASS_DEFINITION(test_0040) {
    // scrollbar on seq view
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);
    GTUtilsProjectTreeView::toggleView(os, GTGlobals::UseKey);

    auto det = GTWidget::findExactWidget<DetView*>(os, "det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QScrollBar* scroll = GTScrollBar::getScrollBar(os, "singleline_scrollbar");

    GTWidget::click(os, scroll);
    U2Region r = det->getVisibleRange();
    CHECK_SET_ERR(r.startPos > 89, QString("Unexpected start pos: %1").arg(r.startPos));
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    // test key events
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);

    auto det = GTWidget::findExactWidget<DetView*>(os, "det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, det);
    qint64 initLength = det->getVisibleRange().length;

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    int start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 1, QString("1 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 2, QString("2 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 1, QString("3 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Up);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 0, QString("4 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == initLength, QString("5 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 0, QString("6 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_End);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 199950 - initLength, QString("7 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Home);
    start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start == 0, QString("8 Unexpected sequence start: %1").arg(start));
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select an annotation.
    GTUtilsAnnotationsTreeView::clickItem(os, "misc_feature", 1, true);

    // Expected: the annotation is selected.
    QString selected = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(selected == "misc_feature", "Unexpected selected annotation: " + selected);

    // Click on the annotation in seq view with the right button.
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {ADV_MENU_EDIT, "edit_annotation_tree_item"}, PopupChecker::IsEnabled));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {ADV_MENU_REMOVE, "Selected annotations and qualifiers"}, PopupChecker::IsEnabled));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
    //    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    move mouse to annotation on det view
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2);
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(20, 0));
    QString tooltip = GTUtilsToolTip::getToolTip();
    QString expected = "<table><tr><td bgcolor=#ffff99 bordercolor=black width=15></td><td><big>misc_feature</big></td></tr><tr><td></td><td><b>Location"
                       "</b> = 2..590</td></tr><tr><td/><td><nobr><b>note</b> = 5' terminal repeat</nobr><br><nobr><b>Sequence</b> = AATGAAAGACCCCACCCGTAGGTGGCAAGCTAGCTTAAGT"
                       " ...</nobr><br><nobr><b>Translation</b> = NERPHP*VAS*LK ...</nobr></td></tr></table>";
    CHECK_SET_ERR(tooltip == expected, "Unexpected tooltip: " + tooltip)
}

GUI_TEST_CLASS_DEFINITION(test_0044) {
    // Overview wheel event
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto over = qobject_cast<Overview*>(GTWidget::findWidget(os, "OverviewRenderArea")->parentWidget()->parentWidget());
    GTWidget::click(os, over);
    for (int i = 0; i < 10; i++) {
        GTMouseDriver::scroll(1);
    }

    U2Region r = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange();
    CHECK_SET_ERR(r.startPos > 90000, QString("Unexpected visible range: %1").arg(r.startPos));
}

GUI_TEST_CLASS_DEFINITION(test_0044_1) {
    // selection on overview
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto over = qobject_cast<Overview*>(GTWidget::findWidget(os, "OverviewRenderArea")->parentWidget()->parentWidget());
    GTWidget::click(os, over);

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(100, 0));
    GTMouseDriver::release();
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTThread::waitForMainThread();

    QVector<U2Region> selectionVector = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selectionVector.size() == 1, QString("unexpected number of selected regions: %1").arg(selectionVector.size()));
    CHECK_SET_ERR(selectionVector.first().length > 1000, QString("unexpected selection length: &1").arg(selectionVector.first().length))
}

GUI_TEST_CLASS_DEFINITION(test_0044_2) {
    // move slider
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Overview* over = GTUtilsSequenceView::getOverviewByNumber(os);
    GTWidget::click(os, over);
    GTMouseDriver::doubleClick();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(0, over->geometry().height() / 3));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(over->geometry().width() / 3, 0));
    GTMouseDriver::release();

    int start = GTUtilsSequenceView::getVisibleStart(os);
    CHECK_SET_ERR(start > 150000, QString("Unexpected selection start: %1").arg(start));

    //    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
    //    OverviewRenderArea* renderArea = GTWidget::findExactWidget<OverviewRenderArea*>(os, "OverviewRenderArea", over);
    //    int detX = renderArea->posToCoord(GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView()->getVisibleRange().startPos) - 6;
    //    QRect r = QRect(detX, 8, 12, 10);
    //    GTMouseDriver::moveTo(os, renderArea->mapToGlobal(r.center()));
    //    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0045) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* pan = GTUtilsSequenceView::getPanViewByNumber(os);
    QImage init = GTWidget::getImage(os, pan);
    // show restriction sites
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Restriction Sites"}));
    auto qt_toolbar_ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "mwtoolbar_activemdi"), {false});
    if (qt_toolbar_ext_button != nullptr && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(os, qt_toolbar_ext_button);
    }
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);
    // show orfs
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    if (qt_toolbar_ext_button != nullptr && qt_toolbar_ext_button->isVisible()) {
        GTWidget::click(os, qt_toolbar_ext_button);
    }
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<QScrollBar*> list = pan->findChildren<QScrollBar*>();
    QScrollBar* vertical = nullptr;
    for (QScrollBar* b : qAsConst(list)) {
        if (b->orientation() == Qt::Vertical) {
            vertical = b;
        }
    }
    GTWidget::click(os, vertical);
    GTMouseDriver::scroll(5);
    QImage final = GTWidget::getImage(os, pan);
    CHECK_SET_ERR(init != final, "pan view was not changed")
}

GUI_TEST_CLASS_DEFINITION(test_0046) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    for (int i = 0; i < 5; i++) {
        GTWidget::click(os, GTAction::button(os, "action_zoom_in_NC_001363"));
    }
    qint64 vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis > 2300, QString("1 wrong visiable range start: %1").arg(vis));

    for (int i = 0; i < 5; i++) {
        GTWidget::click(os, GTAction::button(os, "action_zoom_out_NC_001363"));
    }
    vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis == 0, QString("2 wrong visiable range start: %1").arg(vis));

    for (int i = 0; i < 5; i++) {
        GTWidget::click(os, GTAction::button(os, "action_zoom_in_NC_001363"));
    }
    vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis > 2300, QString("3 wrong visiable range start: %1").arg(vis));

    GTWidget::click(os, GTAction::button(os, "action_zoom_to_sequence_NC_001363"));
    vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis == 0, QString("4 wrong visiable range start: %1").arg(vis));
}

GUI_TEST_CLASS_DEFINITION(test_0047) {
    //    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    //    Open CV
    GTUtilsCv::cvBtn::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    //    Use context menu on CV
    GTUtilsDialog::waitForDialog(os, new SetSequenceOriginDialogFiller(os, 1000));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ADV_MENU_EDIT, "Set new sequence origin"}));
    GTWidget::click(os, GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0"), Qt::RightButton);
    //    check "Set new sequence origin" action
    GTUtilsSequenceView::clickAnnotationDet(os, "CDS", 43);
}

GUI_TEST_CLASS_DEFINITION(test_0048) {
    //    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //    Use context menu on annotation in tree view
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Add", "Objects with annotations..."}));
    GTUtilsDialog::add(os, new ProjectTreeItemSelectorDialogFiller(os, "murine.gb", "NC_001363 features"));
    GTUtilsDialog::add(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
    //    Check {add-> Objects with annotations} action
    GTUtilsAnnotationsTreeView::findItem(os, "NC_001363 features [murine.gb]");
}

GUI_TEST_CLASS_DEFINITION(test_0050) {
    class WrongNameChecker : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Illegal annotation name"));
            auto nameEdit = GTWidget::findLineEdit(os, "leAnnotationName", dialog);
            GTLineEdit::setText(os, nameEdit, "//");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    class WrongDistanceChecker : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto gbFormatLocation = GTWidget::findRadioButton(os, "rbGenbankFormat", dialog);
            GTRadioButton::click(os, gbFormatLocation);

            auto locationEdit = GTWidget::findLineEdit(os, "leLocation", dialog);
            GTLineEdit::clear(os, locationEdit);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location"));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(os, locationEdit, "1..");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location"));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(os, locationEdit, "1..0");
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid location"));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItems(os, {GTUtilsAnnotationsTreeView::findItem(os, "CDS")});

    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, new WrongNameChecker));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, new WrongDistanceChecker));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

GUI_TEST_CLASS_DEFINITION(test_0050_1) {
    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Genes", "promoter"}));
            GTWidget::click(os, GTWidget::findWidget(os, "showNameGroupsButton", dialog));
            auto nameEdit = GTWidget::findLineEdit(os, "nameEdit", dialog);
            CHECK_SET_ERR(nameEdit->text() == "promoter", "unexpected name: " + nameEdit->text());

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItems(os, {GTUtilsAnnotationsTreeView::findItem(os, "CDS")});

    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, new custom));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

GUI_TEST_CLASS_DEFINITION(test_0051) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT"));

    QColor init = GTUtilsAnnotHighlightingTreeView::getItemColor(os, "CDS");
    CHECK_SET_ERR(init.name() == "#9bffff", "unexpected CDS annotations color: " + init.name());

    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, 255, 0, 0));
    GTUtilsAnnotHighlightingTreeView::click(os, "CDS");
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(90, 0));
    GTMouseDriver::click();

    QColor final = GTUtilsAnnotHighlightingTreeView::getItemColor(os, "CDS");
    CHECK_SET_ERR(final.name() == "#ff0000", "CDS annotations color changed wrong: " + init.name());
}

GUI_TEST_CLASS_DEFINITION(test_0052) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    DetView* det = GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView();
    GTWidget::click(os, det);
    QImage image1 = GTWidget::getImage(os, det);

    GTWidget::click(os, GTAction::button(os, "complement_action"));
    GTWidget::click(os, det);
    QImage image2 = GTWidget::getImage(os, det);

    GTWidget::click(os, GTAction::button(os, "complement_action"));
    GTWidget::click(os, det);
    QImage image3 = GTWidget::getImage(os, det);

    CHECK_SET_ERR(image1 != image2, "Image was not changed");
    CHECK_SET_ERR(image1 == image3, "Image was not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0052_1) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget* sequenceArea = GTUtilsSequenceView::getDetViewByNumber(os, 0)->getDetViewRenderArea();
    GTWidget::click(os, sequenceArea);
    QImage image1 = GTWidget::getImage(os, sequenceArea);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"show_all_frames_radiobutton"}));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
    GTWidget::click(os, sequenceArea);
    QImage image2 = GTWidget::getImage(os, sequenceArea);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"do_not_translate_radiobutton"}));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
    GTWidget::click(os, sequenceArea);
    QImage image3 = GTWidget::getImage(os, sequenceArea);

    CHECK_SET_ERR(image1 != image2, "Image was not changed");
    CHECK_SET_ERR(image1 == image3, "Image was not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0053) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}, GTGlobals::UseMouse));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTUtilsDialog::checkNoActiveWaiters(os);

    //    Add label with shift+left mouse
    QWidget* graphView = GTUtilsSequenceView::getGraphView(os);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTWidget::click(os, graphView, Qt::LeftButton, QPoint(50, 50));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    QList<QVariant> list = GTUtilsSequenceView::getLabelPositions(os, GTUtilsSequenceView::getGraphView(os));
    CHECK_SET_ERR(list.size() == 1, QString("unexpected number of labels: %1").arg(list.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0054) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    //    Add label with shift+left mouse
    GSequenceGraphView* graphView = GTUtilsSequenceView::getGraphView(os);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTWidget::click(os, graphView, Qt::LeftButton, QPoint(50, 50));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    //    Use context menu {graph->Graph settings}
    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1, -1, 0, 0, 255, 0, 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Graph", "visual_properties_action"}));
    GTWidget::click(os, graphView, Qt::RightButton);
    QColor c = GTUtilsSequenceView::getGraphColor(os, graphView);
    QString s = c.name();
    CHECK_SET_ERR(s == "#ff0000", "unexpected color: " + s);
    //    In settings dialog change label color
}

GUI_TEST_CLASS_DEFINITION(test_0055) {
    //    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    QWidget* graphView = GTUtilsSequenceView::getGraphView(os);
    GTWidget::click(os, graphView);
    // GTWidget::getAllWidgetsInfo(os, GTUtilsSequenceView::getGraphView(os));
    QWidget* pointer = nullptr;
    const QList<QWidget*> childList = GTUtilsSequenceView::getGraphView(os)->findChildren<QWidget*>();
    for (QWidget* w : qAsConst(childList)) {
        if (w->geometry().width() == 10) {
            pointer = w;
        }
    }
    CHECK_SET_ERR(pointer != nullptr, "pointer not found");

    //    Use context menu {graph->Graph settings}
    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1, -1, 10, 15));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Graph", "visual_properties_action"}));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTWidget::click(os, GTUtilsSequenceView::getGraphView(os));
    QPoint p1 = pointer->mapToGlobal(pointer->geometry().center());

    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1, -1, 100, 150));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Graph", "visual_properties_action"}));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTWidget::click(os, GTUtilsSequenceView::getGraphView(os));
    QPoint p2 = pointer->mapToGlobal(pointer->geometry().center());

    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1, -1, 10, 150));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Graph", "visual_properties_action"}));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTWidget::click(os, GTUtilsSequenceView::getGraphView(os));
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
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    QWidget* graphView = GTUtilsSequenceView::getGraphView(os);

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid cutoff range"));

            auto minmaxGroup = GTWidget::findGroupBox(os, "minmaxGroup", dialog);
            GTGroupBox::setChecked(os, minmaxGroup);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, new custom));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Graph", "visual_properties_action"}));
    GTWidget::click(os, graphView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0057) {
    // Check that graph shows expected min/max label count.

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Open GC Content (%).
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Zoom in, so we have a readable picture for the label tests.
    QAction* zoomInAction = GTAction::findActionByText(os, "Zoom In");
    for (int i = 0; i < 5; i++) {
        GTWidget::click(os, GTAction::button(os, zoomInAction));
    }

    // Use context menu {graph->Select all extremum points}.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Graph", "show_labels_for_min_max_points"}));
    GSequenceGraphView* graphView = GTUtilsSequenceView::getGraphView(os);
    GTWidget::click(os, graphView, Qt::RightButton);

    // Check labels count match the expected value.
    int labelCount = GTUtilsSequenceView::getGraphLabels(os, graphView).size();
    CHECK_SET_ERR(labelCount == 8, QString("Unexpected labels number: %1").arg(labelCount));
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    // complex test on local toolbars buttons

    GTFileDialog::openFile(os, dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAction* zoomIn = GTAction::findAction(os, "action_zoom_in_A1#berezikov");

    GTWidget::click(os, GTAction::button(os, zoomIn));
    GTWidget::click(os, GTAction::button(os, zoomIn));
    GTWidget::click(os, GTAction::button(os, zoomIn));

    auto chromView = GTWidget::findWidget(os, "chromatogram_view_A1#berezikov");

    QImage image = GTWidget::getImage(os, chromView);

    QAction* bars = GTAction::findActionByText(os, "Show quality bars");
    GTWidget::click(os, GTAction::button(os, bars));

    CHECK_SET_ERR(image != GTWidget::getImage(os, chromView), "Nothing changed on Chromatogram View after Bars adding");

    image = GTWidget::getImage(os, chromView);

    QAction* traces = GTAction::findActionByText(os, "Show/hide trace");

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"A"}));
    GTWidget::click(os, GTAction::button(os, traces));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"C"}));
    GTWidget::click(os, GTAction::button(os, traces));

    CHECK_SET_ERR(image != GTWidget::getImage(os, chromView), "Nothing changed on Chromatogram View after Traces hiding");
}

GUI_TEST_CLASS_DEFINITION(test_0059) {
    //"Invert annotation selection" action test
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Invert annotation selection"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, GTUtilsAnnotationsTreeView::findItem(os, "CDS"));
    QList<QTreeWidgetItem*> selected = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(selected.size() == 7, QString("Unexpected number of selected items: %1").arg(selected.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0059_1) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ORFs"}));
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "CreateAnnotationDialog"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Make auto-annotations persistent"}));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 27)"));

    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 27)");
    QString s = orf->parent()->text(0);
    CHECK_SET_ERR(s == "NC_001363 features [murine.gb] *", "unexpected parent: " + s)
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto nameLabel = GTWidget::findWidget(os, "nameLabel");
    GTMouseDriver::moveTo(GTWidget::getWidgetCenter(nameLabel) + QPoint(2 * nameLabel->geometry().width() / 3, 0));
    GTMouseDriver::doubleClick();

    auto overViewSe2 = GTWidget::findWidget(os, "overview_NC_001363", nullptr, false);
    auto DetailsViewSe2 = GTWidget::findWidget(os, "det_view_NC_001363", nullptr, false);
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_NC_001363", nullptr, false);
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_NC_001363", nullptr, false);
    CHECK_SET_ERR(overViewSe2->isVisible() == false &&
                      DetailsViewSe2->isVisible() == false &&
                      zoomViewSe2->isVisible() == false,
                  "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0061_1) {
    // 1. Open "data/Samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Check that first sequence name is '1CF7 chain A sequence'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7 chain A sequence"));
    // 3. Check that first annotation table name is '1CF7 chain A annotation'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7 chain A annotation"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "1CF7 chain A annotation [1CF7.PDB]"));

    // 4. Check that there is 'molecule_name' qualifier with value 'PROTEIN (TRANSCRIPTION FACTOR E2F-4)'  in 'chain_info' annotation for chain A
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection(os).isEmpty(), "Selection is not empty");
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"chain_info"});
    QString moleculeName = GTUtilsAnnotationsTreeView::getQualifierValue(os, "molecule_name", GTUtilsAnnotationsTreeView::findItem(os, "chain_info"));
    CHECK_SET_ERR(moleculeName == "PROTEIN (TRANSCRIPTION FACTOR E2F-4)", QString("Incorrect molecule name is detected for chain A: %1").arg(moleculeName));
    QTreeWidgetItem* annotationsChainB = GTUtilsAnnotationsTreeView::findItem(os, "1CF7 chain B annotation [1CF7.PDB]");

    // 5. Check that there is 'molecule_name' qualifier with value 'PROTEIN (TRANSCRIPTION FACTOR DP-2)'  in 'chain_info' annotation for chain B
    QTreeWidgetItem* chainInfoGroup = GTUtilsAnnotationsTreeView::findItem(os, "chain_info  (0, 1)", annotationsChainB);
    GTUtilsAnnotationsTreeView::selectItems(os, {chainInfoGroup});
    QTreeWidgetItem* chainInfo = GTUtilsAnnotationsTreeView::findItem(os, "chain_info", chainInfoGroup);
    GTUtilsAnnotationsTreeView::selectItems(os, {chainInfo});
    QString moleculeNameChainB = GTUtilsAnnotationsTreeView::getQualifierValue(os, "molecule_name", chainInfo);

    CHECK_SET_ERR(moleculeNameChainB == "PROTEIN (TRANSCRIPTION FACTOR DP-2)", QString("Incorrect molecule name is detected for chain B: %1").arg(moleculeNameChainB));
}

GUI_TEST_CLASS_DEFINITION(test_0061_2) {
    // 1. Open "_common_data/pdb/1CRN_without_compnd_tag.PDB"
    GTFileDialog::openFile(os, testDir + "_common_data/pdb", "1CRN_without_compnd_tag.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Check that first annotation table name is '1CRN chain A sequence'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CRN chain A annotation"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "1CRN chain A annotation [1CRN_without_compnd_tag.PDB]"));

    // 3. Check that there is 'chain_id' qualifier with value 'A'  in 'chain_info' annotation for chain A
    GTUtilsAnnotationsTreeView::selectItemsByName(os, {"chain_info"});
    QString chainId = GTUtilsAnnotationsTreeView::getQualifierValue(os, "chain_id", "chain_info");
    CHECK_SET_ERR("A" == chainId, QString("Incorrect 'chain_info' qualifier value: %1").arg(chainId));

    // 4. Check that there is not 'molecule_name' qualifier
    QTreeWidgetItem* moleculeName = GTUtilsAnnotationsTreeView::findItem(os, "molecule_name", nullptr, {false});
    CHECK_SET_ERR(moleculeName == nullptr, QString("There is 'moleculeName' qualifier"));
}

GUI_TEST_CLASS_DEFINITION(test_0061_3) {
    // 1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Call context menu on the 3dview, select {Structural Alignment -> Align With...} menu item.
    // 3. Check that chain combobox contains identifiers: A, B, C, D.
    // 4. Accept the dialog.
    QStringList chainIndexes;
    chainIndexes << "A"
                 << "B"
                 << "C"
                 << "D";
    GTUtilsDialog::waitForDialog(os, new StructuralAlignmentDialogFiller(os, chainIndexes));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Structural Alignment", "align_with"}));
    auto widget3d = GTWidget::findWidget(os, "1-1CF7");
    GTWidget::click(os, widget3d, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. "Wrap sequence" button is checked by default
    //    Expected state: vertical scrollbar is present, horizontal scrollbar is hidden, sequence is split into lines
    //    3. Uncheck "Wrap sequence" button on the local toolbar
    //    Expected state: the view is not resized, horizontal scrollbar appears, vertical scrollbar is hidden, sequence is in one line

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != nullptr, "No sequence widget found");
    QSize seqWgtSize = seqWgt->size();

    QScrollBar* scrollBar = GTScrollBar::getScrollBar(os, "multiline_scrollbar");
    CHECK_SET_ERR(scrollBar != nullptr, "Cannot find multiline_scrollbar");
    CHECK_SET_ERR(scrollBar->isVisible(), "multiline_scrollbar is hidden");

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);
    CHECK_SET_ERR(seqWgtSize == seqWgt->size(), "Multi-line mode resized the view");
    CHECK_SET_ERR(scrollBar->isHidden(), "multiline_scrollbar is visible");

    QScrollBar* hScrollBar = GTScrollBar::getScrollBar(os, "singleline_scrollbar");
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

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    qint64 pos = 789;
    GTUtilsSequenceView::goToPosition(os, pos);
    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(pos), "Visible range does not contain 789 position");
    pos = visibleRange.startPos;

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);

    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.startPos == pos, "Visible range does not contain requeried position");
    GTUtilsSequenceView::goToPosition(os, pos + 20);
    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    pos = visibleRange.startPos;

    GTWidget::click(os, wrapButton);
    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(pos), "Start position of visible range was changed");
}

GUI_TEST_CLASS_DEFINITION(test_0064) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. Scroll with the mouse wheel to the end of the sequence and back to the beginning

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "seq4.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    QScrollBar* scrollBar = GTScrollBar::getScrollBar(os, "multiline_scrollbar");
    CHECK_SET_ERR(scrollBar != nullptr, "Cannot find multiline_scrollbar");
    GTScrollBar::moveSliderWithMouseWheelDown(os, scrollBar, scrollBar->maximum());

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(GTUtilsSequenceView::getSeqWidgetByNumber(os)->getSequenceLength() - 1), "The end position of the sequence is not visible. Failed to scroll to the end_1");

    GTScrollBar::moveSliderWithMouseWheelUp(os, scrollBar, scrollBar->maximum());

    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
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

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    GTWidget::setFocus(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView());

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    qint64 currentStartPos = GTUtilsSequenceView::getVisibleRange(os).startPos;
    CHECK_SET_ERR(visibleRange.startPos == currentStartPos, QString("Visible range was changed after Key_Down: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));

    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    currentStartPos = GTUtilsSequenceView::getVisibleRange(os).startPos;
    CHECK_SET_ERR(visibleRange.startPos == currentStartPos, QString("Visible range was changed after Key_Up: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));

    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    currentStartPos = GTUtilsSequenceView::getVisibleRange(os).startPos;
    CHECK_SET_ERR(visibleRange.startPos != currentStartPos, QString("Visible range was not changed after Key_PageDown: %1 => %2").arg(visibleRange.startPos).arg(currentStartPos));

    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    currentStartPos = GTUtilsSequenceView::getVisibleRange(os).startPos;
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

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::goToPosition(os, 1000);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    auto splitter = GTWidget::findSplitter(os, "annotated_DNA_splitter");
    QWidget* bottomSplitterHandle = splitter->handle(splitter->count() - 1);  // GTWidget::findWidget(os, "qt_splithandle_", GTWidget::findWidget(os, "annotated_DNA_splitter"));
    CHECK_SET_ERR(bottomSplitterHandle != nullptr, "Cannot find bottom splitter handle");
    GTWidget::click(os, bottomSplitterHandle);
    QPoint p1 = GTMouseDriver::getMousePosition();
    QPoint p2 = QPoint(p1.x(), p1.y() + 100);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(os), "Start position of visible range was changed on enlarge at the bottom");

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(os), "Start position of visible range was changed on reduce at the bottom");

    auto topSplitterHandle = GTWidget::findWidget(os, "qt_splithandle_det_view_NC_004718");
    GTWidget::click(os, topSplitterHandle);
    p1 = GTMouseDriver::getMousePosition();
    p2 = QPoint(p1.x(), p1.y() - 100);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(os), "Start position of visible range was changed on enlarge at the top");

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleStart(os), "Start position of visible range was changed on reduce at the top");
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

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "fa1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto showDetViewButton = GTWidget::findWidget(os, "show_hide_details_view");
    GTWidget::click(os, showDetViewButton);

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != nullptr, "No sequence widget found");

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    QScrollBar* scrollBar = GTScrollBar::getScrollBar(os, "singleline_scrollbar");
    CHECK_SET_ERR(scrollBar->isHidden(), "Horizontal scroll bar is visible");

    GTWidget::click(os, seqWgt);
    GTMouseDriver::doubleClick();
    GTWidget::click(os, wrapButton);
    GTWidget::click(os, seqWgt);
    GTMouseDriver::doubleClick();

    ExportSequenceImage::Settings s(ExportSequenceImage::DetailsView, U2Region(1, 3));
    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_image_0067", s));
    GTWidget::click(os, GTAction::button(os, "export_image"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

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

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"show_all_frames_radiobutton"}));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
    GTUtilsDialog::checkNoActiveWaiters(os);
    CHECK_SET_ERR(visibleRange != GTUtilsSequenceView::getVisibleRange(os), "Visible range was not changed on translation show/hide");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_0069) {
    //   Open a sequence (e.g. murine.gb)
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that multi-line mode is enabled
    GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection(os).isEmpty(), "Selection is not empty");

    DetView* seqWgt = GTUtilsSequenceView::getDetViewByNumber(os);
    CHECK_SET_ERR(seqWgt != nullptr, "Cannot find DetView widget");

    //   Move the mouse to the other line and release the button (make a selection)
    GTWidget::click(os, seqWgt);
    QPoint p1 = GTMouseDriver::getMousePosition();
    QPoint p2(p1.x() + 300, p1.y() + 200);

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    GTThread::waitForMainThread();

    CHECK_SET_ERR(!GTUtilsSequenceView::getSelection(os).isEmpty(), "Nothing is selected");
}

GUI_TEST_CLASS_DEFINITION(test_0070) {
    //    1. Open any sequence with annotations (e.g. murine.gb)
    //    2. Click on annotation
    //    Expected state: annotation is selected
    //    3. Click "Wrap sequence"
    //    4. Enlarge the view to see a few lines of the sequence
    //    5. Click on a few annotations located on a different lines of the sequence
    //    Expected state: clicked annotation is selected

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);

    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(!selection.isEmpty(), "Nothing is selected");
    GTWidget::click(os, wrapButton);

    GTUtilsSequenceView::clickAnnotationDet(os, "CDS", 1042, 0, true);
    CHECK_SET_ERR(!GTUtilsSequenceView::getSelection(os).isEmpty(), "Selection is empty");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection(os) != selection, "Selection was not changed");
}

GUI_TEST_CLASS_DEFINITION(test_0071) {
    //    1. Open any sequence (e.g. murine.gb)
    //    2. Click "Export image"
    //    3. Fill the dialog (select a region from the middle of the sequence) and export the dialog
    //    Expected state: the result file contains the lines of the sequence started from the specified position, no extra empty space

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != nullptr, "Cannot find sequence widget");

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    ExportSequenceImage::Settings s(ExportSequenceImage::DetailsView, U2Region(1, 2000));
    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_image_0071", s));
    GTWidget::click(os, GTAction::button(os, "export_image"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QPixmap p(sandBoxDir + "seq_image_0071");
    CHECK_SET_ERR(p.size() != QSize() && p.size() != seqWgt->getDetView()->getDetViewRenderArea()->size(), "Exported image size is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_0075) {
    // 1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Click the translations toolbar button.
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    // Expected: the menu appears.
    auto menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    CHECK_SET_ERR(nullptr != menu, "No menu");

    // 3. Click "Show/hide translations".
    GTMenu::clickMenuItem(os, menu, "do_not_translate_radiobutton");

    // Expected: the menu doesn't disappear.
    CHECK_SET_ERR(nullptr != QApplication::activePopupWidget(), "Menu disappeared 1");

    // 4. Click "Show all".
    GTMenu::clickMenuItemByText(os, menu, {"Show all frames"});

    // Expected: the menu doesn't disappear.
    CHECK_SET_ERR(nullptr != QApplication::activePopupWidget(), "Menu disappeared 2");

    // 5. Click somewhere else.
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));

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

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(os, new FindEnzymesDialogFiller(os, {"EcoRI"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "SYNPBR322"));
    GTUtilsDialog::add(os, new PopupChooserByText(os, {"Mark as circular"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(os, new FindEnzymesDialogFiller(os, {"EcoRI"}));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "EcoRI", nullptr, {false}) == nullptr, "EcoRI is unexpectedly found");
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

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    const QStringList defaultEnzymes = {"ClaI", "DraI", "EcoRI"};
    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "Find restriction sites"}));
    FindEnzymesDialogFillerSettings settings(defaultEnzymes);
    settings.searchRegionStart = 4200;
    settings.searchRegionEnd = 10;
    GTUtilsDialog::add(os, new FindEnzymesDialogFiller(os, settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "ClaI", nullptr, {false}) == nullptr, "ClaI is unexpectedly found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "DraI", nullptr, {false}) == nullptr, "DraI is unexpectedly found");
    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "Find restriction sites"}));
    settings.searchRegionStart = 3900;
    settings.searchRegionEnd = 300;
    settings.excludeRegionStart = 4300;
    settings.excludeRegionEnd = 10;
    GTUtilsDialog::add(os, new FindEnzymesDialogFiller(os, settings));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "EcoRI", nullptr, {false}) == nullptr, "EcoRI is unexpectedly found");
    GTUtilsAnnotationsTreeView::findItem(os, "ClaI");
    GTUtilsAnnotationsTreeView::findItem(os, "DraI");
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

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    class RegionSelectorChecker : public Filler {
    public:
        RegionSelectorChecker(HI::GUITestOpStatus& os)
            : Filler(os, "FindEnzymesDialog") {
        }

        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto regionSelector = GTWidget::findWidget(os, "region_selector_with_excluded");

            auto start = GTWidget::findLineEdit(os, "startLineEdit", regionSelector);
            GTLineEdit::setText(os, start, "5000");

            auto end = GTWidget::findLineEdit(os, "endLineEdit", regionSelector);
            GTLineEdit::setText(os, end, "1000");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(os, start, "1");

            auto exclude = GTWidget::findCheckBox(os, "excludeCheckBox");
            GTCheckBox::setChecked(os, exclude);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(os, new RegionSelectorChecker(os));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea(os);
}

GUI_TEST_CLASS_DEFINITION(test_0079_1) {
    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            GTComboBoxWithCheckBoxes::selectItemByText(os, "cbSuppliers", dialog, { "Not defined" }, GTGlobals::UseMouse);

            auto text = GTLabel::getText(os, "statusLabel", dialog);
            CHECK_SET_ERR(text.contains("Total number of enzymes: 15510, selected 0"), QString("Unexpected text: %1").arg(text));

            auto chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts(os, "cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 1, QString("Current checked size: %1").arg(chekedValues.size()));
            CHECK_SET_ERR(chekedValues.first() == "Not defined", QString("Current checked value: %1").arg(chekedValues.first()));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList{}, new custom()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find restriction sites");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0079_2) {
    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            GTWidget::click(os, GTWidget::findWidget(os, "pbSelectNone"));
            auto chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts(os, "cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 0, QString("Current checked size after pbSelectNone: %1").arg(chekedValues.size()));


            GTWidget::click(os, GTWidget::findWidget(os, "pbSelectAll"));
            chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts(os, "cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 16, QString("Current checked size after pbSelectAll: %1").arg(chekedValues.size()));


            GTComboBoxWithCheckBoxes::selectItemByText(os, "cbSuppliers", dialog, { "Not defined", "Thermo Fisher Scientific" });
            chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts(os, "cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 2, QString("Current checked size after pbSelectAll: %1").arg(chekedValues.size()));

            GTWidget::click(os, GTWidget::findWidget(os, "pbInvertSelection"));
            chekedValues = GTComboBoxWithCheckBoxes::getCheckedItemsTexts(os, "cbSuppliers", dialog);
            CHECK_SET_ERR(chekedValues.size() == 14, QString("Current checked size after pbSelectAll: %1").arg(chekedValues.size()));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList{}, new custom()));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Find restriction sites");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

}  // namespace GUITest_common_scenarios_sequence_view

}  // namespace U2
