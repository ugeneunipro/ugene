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

#include "GTTestsSequenceView.h"

#include "GTGlobals.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAnnotationsHighlightingTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

#include "api/GTSequenceReadingModeDialog.h"

#include "system/GTClipboard.h"
#include "system/GTFile.h"

#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateRulerDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportImageDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/GraphLabelsSelectDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/GraphSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectorFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SetSequenceOriginDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/primer3/Primer3DialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/biostruct3d_view/StructuralAlignmentDialogFiller.h"

#include "utils/GTUtilsToolTip.h"
#include "utils/GTThread.h"

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <base_dialogs/MessageBoxFiller.h>

#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTWidget.h>
#include <primitives/GTAction.h>
#include <primitives/GTMenu.h>
#include <primitives/PopupChooser.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTTreeWidget.h>

#include <U2Core/AppContext.h>

#include <U2View/ADVConstants.h>
#include <U2View/DetView.h>
#include <U2View/GSequenceLineView.h>
#include <U2View/GSequenceGraphView.h>
#include <U2View/Overview.h>

#include <QApplication>
#include <QGroupBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>


namespace U2 {

namespace GUITest_common_scenarios_sequence_view {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0002) {
//    disable circular view for protein sequences (0002400)

//    1. Open file samples/PDB/1CF7.pdb
//    Expected state: 'Show circular view' button disabled for 1CF7 chain 1 sequence and 1CF7 chain 2 sequence, but enabled for others

    GTFileDialog::openFile(os, dataDir+"samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(3000);

    QWidget *w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    QWidget *w3 = GTWidget::findWidget(os, "ADV_single_sequence_widget_3");

    GTGlobals::FindOptions f;
    f.failIfNotFound = false;

    QWidget *button0 = GTWidget::findWidget(os,"CircularViewAction",w0,f);
    QWidget *button1 = GTWidget::findWidget(os,"CircularViewAction",w1,f);
    QWidget *button2 = GTWidget::findWidget(os,"CircularViewAction",w2);
    QWidget *button3 = GTWidget::findWidget(os,"CircularViewAction",w3);
    GTGlobals::sleep(3000);

    CHECK_SET_ERR(button0 == NULL, "b0 is not NULL");
    CHECK_SET_ERR(button1 == NULL, "b1 is not NULL");
    CHECK_SET_ERR(button2 != NULL, "b2 is NULL");
    CHECK_SET_ERR(button3 != NULL, "b3 is NULL");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {

    GTFileDialog::openFile(os, dataDir+"samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(3000);

    QWidget *w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    QWidget *w3 = GTWidget::findWidget(os, "ADV_single_sequence_widget_3");

    QAbstractButton* b0 = GTAction::button(os,"complement_action", w0);
    QAbstractButton* b1 = GTAction::button(os,"complement_action", w1);
    QAbstractButton* b2 = GTAction::button(os,"complement_action", w2);
    QAbstractButton* b3 = GTAction::button(os,"complement_action", w3);
    GTGlobals::sleep(3000);

    CHECK_SET_ERR(b0 == NULL, "b0 is not NULL");
    CHECK_SET_ERR(b1 == NULL, "b1 is not NULL");
    CHECK_SET_ERR(b2 != NULL, "b2 is NULL");
    CHECK_SET_ERR(b3 != NULL, "b3 is NULL");

    }
GUI_TEST_CLASS_DEFINITION(test_0002_2) {

    GTFileDialog::openFile(os, dataDir+"samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(3000);

    QWidget *w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    QWidget *w3 = GTWidget::findWidget(os, "ADV_single_sequence_widget_3");

    QWidget *b0 = GTWidget::findWidget(os, "translationsMenuToolbarButton", w0, GTGlobals::FindOptions(false));
    QWidget *b1 = GTWidget::findWidget(os, "translationsMenuToolbarButton", w1, GTGlobals::FindOptions(false));
    QWidget *b2 = GTWidget::findWidget(os, "translationsMenuToolbarButton", w2);
    QWidget *b3 = GTWidget::findWidget(os, "translationsMenuToolbarButton", w3);
    GTGlobals::sleep(3000);

    CHECK_SET_ERR(b0 == NULL, "b0 is not NULL");
    CHECK_SET_ERR(b1 == NULL, "b1 is not NULL");
    CHECK_SET_ERR(b2 != NULL, "b2 is NULL");
    CHECK_SET_ERR(b3 != NULL, "b3 is NULL");

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
    GTGlobals::sleep();

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    Runnable *chooser = new PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, toggleViewButton);

    GTGlobals::sleep();
    QAbstractButton* complement = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement -> isEnabled() == false, "button is not disabled");

    Runnable *chooser1 = new PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser1);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "do_not_translate_radiobutton", PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    GTGlobals::sleep();
    QAbstractButton* complement1 = GTAction::button(os, "complement_action");

    GTGlobals::sleep();
    CHECK_SET_ERR(complement1 -> isEnabled() == true, "button is not enabled");
    }
GUI_TEST_CLASS_DEFINITION(test_0003_1) {

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QWidget* ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "mwtoolbar_activemdi"));
    if(ext_button != NULL && ext_button->isVisible()){
        GTWidget::click(os, ext_button);
    }

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    Runnable *chooser = new PopupChooser(os, QStringList() << "toggleAllSequenceViews");
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, toggleViewButton);

    GTGlobals::sleep();
    QAbstractButton* complement = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement -> isEnabled() == false, "button is not disabled");

    Runnable *chooser1 = new PopupChooser(os, QStringList() << "toggleAllSequenceViews");
    GTUtilsDialog::waitForDialog(os, chooser1);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "do_not_translate_radiobutton", PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    GTGlobals::sleep();
    QAbstractButton* complement1 = GTAction::button(os, "complement_action");

    GTGlobals::sleep();
    CHECK_SET_ERR(complement1 -> isEnabled() == true, "button is not enabled");
    }
GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    Runnable *chooser = new PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, toggleViewButton);

    GTGlobals::sleep();
    QAbstractButton* complement = GTAction::button(os, "complement_action");

    CHECK_SET_ERR(complement -> isEnabled() == false, "button is not disabled");

    Runnable *chooser1 = new PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser1);
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "do_not_translate_radiobutton", PopupChecker::IsEnabled));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    GTGlobals::sleep();
    CHECK_SET_ERR(complement -> isEnabled() == true, "button is not disabled");

    Runnable *chooser2 = new PopupChooser(os, QStringList() << "toggleDetailsView");
    GTUtilsDialog::waitForDialog(os, chooser2);
    GTWidget::click(os, toggleViewButton);

    GTGlobals::sleep();
    QAbstractButton* complement1 = GTAction::button(os, "complement_action");

    GTGlobals::sleep();
    CHECK_SET_ERR(complement1 -> isEnabled() == false, "button is not disabled");


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
    GTGlobals::sleep(1000);

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 3);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_selected_sequence_region"));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) { //CHANGES: keyboard used instead mouse

    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "fa1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 3);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_selected_sequence_region"));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));
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
        AllActionsPopupChooser(HI::GUITestOpStatus &_os) : PopupChooser(_os, QStringList()) {}
        void run() {
            GTGlobals::sleep(100);
            QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());

            int i = 0;
            QList<QAction*> act = activePopupMenu->actions();
            foreach (QAction* a, act) {
                QString s = a->objectName();
                if (i == 1) {
                    GTMenu::clickMenuItem(os, activePopupMenu, s, GTGlobals::UseMouse);
                }
                GTGlobals::sleep(100);
                i++;
            }
        }
    };

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFiles(os, testDir + "_common_data/fasta/multy_fa.fa");
    GTUtilsDocument::checkDocument(os, "multy_fa.fa");

    QWidget* toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    CHECK_SET_ERR(toggleAutoAnnotationsButton != NULL, "toggleAutoAnnotationsButton is NULL");
    CHECK_SET_ERR(toggleAutoAnnotationsButton->isEnabled() == false, "toggleAutoAnnotationsButton is enabled, expected disabled");


    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsDocument::checkDocument(os, "human_T1.fa");

    toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
    CHECK_SET_ERR(toggleAutoAnnotationsButton != NULL, "toggleAutoAnnotationsButton is NULL");
    CHECK_SET_ERR(toggleAutoAnnotationsButton->isEnabled() == true, "toggleAutoAnnotationsButton is disabled, expected enabled");

    GTUtilsDialog::waitForDialog(os, new AllActionsPopupChooser(os));
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep(1000);
}
GUI_TEST_CLASS_DEFINITION(test_0006) {

//1. Enable Auto-annotations
//     GTGlobals::sleep();
//     GTGlobals::sleep();
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
//  !!! dirty fastfix of test, very temporary
    QToolBar* tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
//

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    //2. Open any PBD file from samples
    //Expected state: there is no auto-annotations for opened sequences

    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem(os, "enzymes", GTGlobals::FindOptions(false));
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orfs", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(enz == NULL, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == NULL, "orfs unexpectedly present");
    GTGlobals::sleep();

    }

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    //1. Enable Auto-annotations
    GTGlobals::sleep();
    GTGlobals::sleep();
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
//  !!! dirty fastfix of test, very temporary
    QToolBar* tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
//

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    //1. Disable Auto-annotations
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    //3. Enable Auto-annotations
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    //2. Open any PBD file from samples
    //Expected state: there is no auto-annotations for opened sequences
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //Expected state: there is no auto-annotations for opened sequences
    GTGlobals::sleep();
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem(os, "enzymes", GTGlobals::FindOptions(false));
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orfs", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(enz == NULL, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == NULL, "orfs unexpectedly present");
    GTGlobals::sleep();

    }

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    //2. Open any PBD file from samples
    GTGlobals::sleep();
    GTGlobals::sleep();
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //1. Enable Auto-annotations
    QWidget* toggleAutoAnnotationsButton = GTWidget::findWidget(os, "toggleAutoAnnotationsButton");
//  !!! dirty fastfix of test, very temporary
    QToolBar* tb = qobject_cast<QToolBar*>(toggleAutoAnnotationsButton->parent());
    QToolButton* extensionButton = tb->findChild<QToolButton*>("qt_toolbar_ext_button");
//

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    if (extensionButton->isVisible()) {
        GTWidget::click(os, extensionButton);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, toggleAutoAnnotationsButton);
    GTGlobals::sleep();

    //Expected state: there is no auto-annotations for opened sequences
    GTGlobals::sleep();
    QTreeWidgetItem* enz = GTUtilsAnnotationsTreeView::findItem(os, "enzymes", GTGlobals::FindOptions(false));
    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orfs", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(enz == NULL, "enzymes unexpectedly present");
    CHECK_SET_ERR(orf == NULL, "orfs unexpectedly present");
    GTGlobals::sleep();

    }

GUI_TEST_CLASS_DEFINITION(test_0018) {
//    1) Click "Open file" button.

//    2) Choose two sequence files: "_common_data/fasta/DNA.fa" and "_common_data/fasta/DNA_1_seq.fa". And click "Open" button.
//    Expected state: the dialog appears.

//    3) Choose "Merge sequence mode" and click "OK" button.
//    Expected state: one sequence view with a merged sequence is opened. Its name is "merged_document.gb"; its length is 2249.

    QList<QUrl> files;
    files << testDir + "_common_data/fasta/DNA.fa";
    files << testDir + "_common_data/fasta/DNA_1_seq.fa";
    GTSequenceReadingModeDialog::mode = GTSequenceReadingModeDialog::Merge;
    GTUtilsProject::openFiles(os, files);

    int length = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(2249 == length, QString("Sequence length mismatch. Expected: %1. Actual: %2").arg(2249).arg(length));

    QString mergedFileName = testDir + "_common_data/fasta/merged_document.gb";
    QFile::remove(mergedFileName);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
//     1. Open '_common_data/primer3/linear_circular_results.fa'
//     2. Launch Primer3 with default settings
//     Expected state: even the sequence is linear, Primer3 finds primers on junction

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "linear_circular_results.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os));

    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);

    GTGlobals::sleep();
    QList<U2Region> pair2 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 2  (0, 2)");
    CHECK_SET_ERR( pair2.contains(U2Region(3, 21)), "No 4..24 region");
    CHECK_SET_ERR( pair2.contains(U2Region(153, 12)), "No 154..165 region");
    CHECK_SET_ERR( pair2.contains(U2Region(0, 8)), "No 1..8 region");

    QList<U2Region> pair5 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 5  (0, 2)");
    CHECK_SET_ERR( pair5.contains(U2Region(6, 21)), "No 7..27 region");
    CHECK_SET_ERR( pair5.contains(U2Region(155, 10)), "No 156..165 region");
    CHECK_SET_ERR( pair5.contains(U2Region(0, 10)), "No 1..10 region");
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
//     1. Open '_common_data/primer3/circular_primers.gb
//     2. Launch Primer3 with default settings
//     Expected state: the sequence is circular - a few primers cover junction point

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "circular_primers.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os));

    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);

    GTGlobals::sleep();
    QList<U2Region> pair1 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 1  (0, 2)");
    CHECK_SET_ERR( pair1.contains(U2Region(139, 20)), "No 140..159 region");
    CHECK_SET_ERR( pair1.contains(U2Region(331, 20)), "No 332..351 region");

    QList<U2Region> pair5 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 5  (0, 2)");
    CHECK_SET_ERR( pair5.contains(U2Region(123, 20)), "No 124..143 region");
    CHECK_SET_ERR( pair5.contains(U2Region(350, 1)), "No 351..351 region");
    CHECK_SET_ERR( pair5.contains(U2Region(0, 19)), "No 1..19 region");
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
//     1. Open '_common_data/primer3/DNA.gb'
//     2. Launch Primer3 with default settings
//     Expected state: results cover junction point
//     3. Remove circular mark
//     4. Launch Primer3 with different annotation name
//     Expected state: results are linear

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os));

    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);

    GTGlobals::sleep();
    QList<U2Region> pair1 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 1  (0, 2)");
    CHECK_SET_ERR( pair1.contains(U2Region(160, 20)), "No 161..180 region");
    CHECK_SET_ERR( pair1.contains(U2Region(684, 20)), "No 685..704 region");

    QList<U2Region> pair4 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 4  (0, 2)");
    CHECK_SET_ERR( pair4.contains(U2Region(66, 20)), "No 67..86 region");
    CHECK_SET_ERR( pair4.contains(U2Region(606, 20)), "No 607..626 region");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Primers_DNA"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Mark as circular"));
    GTMouseDriver::click(Qt::RightButton);

    GTWidget::click(os, GTWidget::findWidget(os, "render_area_Primers_DNA"));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    Primer3DialogFiller::Primer3Settings settings;
    settings.resultsCount = 5;
    settings.primersName = "linear";
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));

    GTMouseDriver::click();
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(5000);
    pair1 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 1  (0, 2)", "linear  (5, 0)");
    CHECK_SET_ERR( pair1.contains(U2Region(423, 20)), "No 424..443 region");
    CHECK_SET_ERR( pair1.contains(U2Region(582, 20)), "No 583..602 region");

    QList<U2Region> pair5 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 5  (0, 2)", "linear  (5, 0)");
    CHECK_SET_ERR( pair5.contains(U2Region(422, 20)), "No 423..442 region");
    CHECK_SET_ERR( pair5.contains(U2Region(607, 20)), "No 608..627 region");

    GTUtilsDialog::waitForDialog(os, new MessageBoxNoToAllOrNo(os));
    GTUtilsDocument::removeDocument(os, "DNA.gb");
//    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
//    1. Open 'human_T1.fa'
//    2. Open CV
//    3. Select big region covering junction point
//    4. Launch Primer3 search (set results count to 50)
//    Expected state: primers are located on both selected regions(start and end).

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    ADVSingleSequenceWidget* wgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(wgt != NULL, "ADVSequenceWidget is NULL");
    GTUtilsCv::cvBtn::click(os, wgt);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, "150000..199950,1..50000"));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTGlobals::sleep(200);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    Primer3DialogFiller::Primer3Settings settings;
    settings.resultsCount = 50;
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));
    GTWidget::click(os, wgt->getDetView(), Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<U2Region> pair32 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 32  (0, 2)");
    CHECK_SET_ERR( pair32.contains(U2Region(36311, 20)), "No 36312..36331 region");
    CHECK_SET_ERR( pair32.contains(U2Region(36552, 20)), "No 36553..36572 region");

    QList<U2Region> pair50 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 50  (0, 2)");
    CHECK_SET_ERR( pair50.contains(U2Region(2350, 20)), "No 2351..2370 region");
    CHECK_SET_ERR( pair50.contains(U2Region(2575, 20)), "No 2576..2595 region");

    QList<U2Region> pair14 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 14  (0, 2)");
    CHECK_SET_ERR( pair14.contains(U2Region(194911, 20)), "No 194912..194931 region");
    CHECK_SET_ERR( pair14.contains(U2Region(195159, 20)), "No 195160..195179 region");

    QList<U2Region> pair4 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 4  (0, 2)");
    CHECK_SET_ERR( pair4.contains(U2Region(163701, 20)), "No 163702..163721 region");
    CHECK_SET_ERR( pair4.contains(U2Region(163856, 20)), "No 163857..163876 region");
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
//    1. Open '_common_data/primer3/DNA.gb'
//    2. Select region that cover junction point
//    3. Launch Primer3 search with default settings
//    Expected state: primers(left and right) are located on either side of junction point

    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    ADVSingleSequenceWidget* wgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(wgt != NULL, "ADVSequenceWidget is NULL");

    GTWidget::click(os, GTWidget::findWidget(os, "CircularViewAction"));

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleZoomView"));
    GTWidget::click(os, toggleViewButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, "560..743,1..180"));
    GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os));
    GTWidget::click(os, wgt, Qt::RightButton, QPoint(10,10));

    GTGlobals::sleep();

    QList<U2Region> pair1 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 1  (0, 2)");
    CHECK_SET_ERR( pair1.contains(U2Region(160, 20)), "No 161..180 region");
    CHECK_SET_ERR( pair1.contains(U2Region(684, 20)), "No 685..704 region");

    QList<U2Region> pair4 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 4  (0, 2)");
    CHECK_SET_ERR( pair4.contains(U2Region(66, 20)), "No 67..86 region");
    CHECK_SET_ERR( pair4.contains(U2Region(606, 20)), "No 607..626 region");
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
//    1. Open 'common_data/primer3/DNA.gb'
//    2. Open Primer3 dialog
//    3. Set region to circular: 560..(743,1)..180
//    4. Accept the dialog
//    Expected state: no warning apeared, primers are located on selected region
    GTFileDialog::openFile(os, testDir + "/_common_data/primer3", "DNA.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    ADVSingleSequenceWidget* wgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(wgt != NULL, "ADVSequenceWidget is NULL");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "primer3_action"));
    Primer3DialogFiller::Primer3Settings settings;
    settings.start = 560;
    settings.end = 180;
    GTUtilsDialog::waitForDialog(os, new Primer3DialogFiller(os, settings));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_Primers_DNA"), Qt::RightButton);

    GTGlobals::sleep();

    QList<U2Region> pair1 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 1  (0, 2)");
    CHECK_SET_ERR( pair1.contains(U2Region(160, 20)), "No 161..180 region");
    CHECK_SET_ERR( pair1.contains(U2Region(684, 20)), "No 685..704 region");

    QList<U2Region> pair4 = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(os, "pair 4  (0, 2)");
    CHECK_SET_ERR( pair4.contains(U2Region(66, 20)), "No 67..86 region");
    CHECK_SET_ERR( pair4.contains(U2Region(606, 20)), "No 607..626 region");
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
            : Filler(os, "ImageExportForm") {}
        virtual void run() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            GTWidget::clickWindowTitle(os, dialog);

            QRadioButton* radioButton = dialog->findChild<QRadioButton*>("currentViewButton");
            GTRadioButton::click(os, radioButton);

            QWidget* rangeSelector = dialog->findChild<QWidget*>("range_selector");
            CHECK_SET_ERR(rangeSelector != NULL, "range_selector not found");
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is visible");

            radioButton = dialog->findChild<QRadioButton*>("zoomButton");
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(rangeSelector->isVisible(), "range_selector is hidden");

            radioButton = dialog->findChild<QRadioButton*>("detailsButton");
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(rangeSelector->isVisible(), "range_selector is hidden");

            radioButton = dialog->findChild<QRadioButton*>("currentViewButton");
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is hidden");

            QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");
            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button !=NULL, "ok button is NULL");
            GTWidget::click(os, button);
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
    CHECK_SET_ERR(zoom != NULL, "Cannot find Zoom In action");
    for (int i = 0; i < 8; i++) {
        GTWidget::click(os, GTAction::button(os, zoom));
    }

    class LimitsChecker : public Filler {
    public:
        LimitsChecker(HI::GUITestOpStatus& os)
            : Filler(os, "ImageExportForm") {}
        virtual void run() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
            GTWidget::clickWindowTitle(os, dialog);

            QWidget* rangeSelector = dialog->findChild<QWidget*>("range_selector");
            CHECK_SET_ERR(rangeSelector != NULL, "range_selector not found");
            CHECK_SET_ERR(!rangeSelector->isVisible(), "range_selector is visible");

            QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");
            QPushButton* okbutton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okbutton !=NULL, "ok button is NULL");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            QRadioButton* radioButton = dialog->findChild<QRadioButton*>("zoomButton");
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            QLineEdit* end = dialog->findChild<QLineEdit*>("end_edit_line");
            GTLineEdit::setText(os, end, "100");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            radioButton = dialog->findChild<QRadioButton*>("detailsButton");
            GTRadioButton::click(os, radioButton);
            GTLineEdit::setText(os, end, "10000");
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            GTLineEdit::setText(os, end, "100");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button !=NULL, "cancel button is NULL");
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
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "YkrI"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Analyze" << "Find restriction sites..."));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    class SvgLimitsChecker : public Filler {
    public:
        SvgLimitsChecker(HI::GUITestOpStatus& os)
            : Filler(os, "ImageExportForm") {}
        virtual void run() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            // set SVG format
            QComboBox* formatsBox = dialog->findChild<QComboBox*>("formatsBox");
            CHECK_SET_ERR(formatsBox != NULL, "formatBox is NULL");
            GTComboBox::setIndexWithText(os, formatsBox, "svg");

            // export is not available
            QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");
            QPushButton* okbutton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(okbutton != NULL, "ok button is NULL");
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is expectedly enabled");

            // select Details -- export is disabled
            QRadioButton* radioButton = dialog->findChild<QRadioButton*>("detailsButton");
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            QLineEdit* end = dialog->findChild<QLineEdit*>("end_edit_line");
            GTLineEdit::setText(os, end, "2300");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            // set Zoom view export
            radioButton = dialog->findChild<QRadioButton*>("zoomButton");
            GTRadioButton::click(os, radioButton);
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");
            GTLineEdit::setText(os, end, "199000");
            CHECK_SET_ERR(!okbutton->isEnabled(), "Export button is unexpectedly enabled");

            // select a fewer region -- export is enabled
            GTLineEdit::setText(os, end, "10000");
            CHECK_SET_ERR(okbutton->isEnabled(), "Export button is unexpectedly disabled");

            QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
            GTLineEdit::setText(os, fileEdit, sandBoxDir + "/seq_view_test_0030.svg");

            GTWidget::click(os, okbutton);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SvgLimitsChecker(os));
    GTWidget::click(os, GTAction::button(os, "export_image"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    qint64 fileSize = GTFile::getSize(os, sandBoxDir + "/seq_view_test_0030.svg");
    CHECK_SET_ERR(fileSize > 15 * 1024 * 1024, QString("SVG file is too small: %1").arg(fileSize));
    //remove document
    GTWidget::click(os, GTUtilsProjectTreeView::getTreeView(os));
    GTKeyboardDriver::keyClick( 'a', Qt::ControlModifier);
    GTGlobals::sleep(100);

    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTThread::waitForMainThread();
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_0031){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Select some redion
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);
//    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY
                                                      << "Copy reverse complement sequence"));

    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext == "AACTTTGGTGA", "Unexpected reverse complement: " + clipboardtext)
}

GUI_TEST_CLASS_DEFINITION(test_0031_1){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Select some redion
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);
//    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY
                                                      << "Copy reverse complement sequence"));

    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext == "AACTTTGGTGA", "Unexpected reverse complement: " + clipboardtext)
}

GUI_TEST_CLASS_DEFINITION(test_0031_2){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Select annotation
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
//    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY
                                                      << "action_copy_annotation_sequence"));

    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("AATGAAAGAC"), "Unexpected reverse complement start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("GTCTTTCATT"), "Unexpected reverse complement end: " + clipboardtext.right(10));

//    Check joined annotations
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 2, true);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY
                                                      << "action_copy_annotation_sequence"));

    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("ATGGTAGCAG"), "Unexpected reverse complement for joined annotation start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("TCTAGACTGA"), "Unexpected reverse complement for joined annotation end: " + clipboardtext.right(10));
}

GUI_TEST_CLASS_DEFINITION(test_0031_3){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Select annotation
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
//    Use context menu {Copy->Copy reverse complement sequence}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY
                                                      << "action_copy_annotation_sequence"));

    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    QString clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("AATGAAAGAC"), "Unexpected reverse complement start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("GTCTTTCATT"), "Unexpected reverse complement end: " + clipboardtext.right(10));

//    Check joined annotations
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 2, true);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_COPY
                                                      << "action_copy_annotation_sequence"));

    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

    clipboardtext = GTClipboard::text(os);
    CHECK_SET_ERR(clipboardtext.startsWith("ATGGTAGCAG"), "Unexpected reverse complement for joined annotation start: " + clipboardtext.left(10));
    CHECK_SET_ERR(clipboardtext.endsWith("TCTAGACTGA"), "Unexpected reverse complement for joined annotation end: " + clipboardtext.right(10));
}

#define GET_ACTIONS QMenu *activePopupMenu = qobject_cast<QMenu *>(QApplication::activePopupWidget()); \
CHECK_SET_ERR(NULL != activePopupMenu, "Active popup menu is NULL"); \
QAction* direct1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +1", true); \
QAction* direct2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +2", true); \
QAction* direct3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame +3", true); \
QAction* compl1 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -1", true); \
QAction* compl2 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -2", true); \
QAction* compl3 = GTMenu::getMenuItem(os, activePopupMenu, "Frame -3", true);

GUI_TEST_CLASS_DEFINITION(test_0032){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//  Click "Amino translations" button on mdi toolbar
    QWidget *translationsMenuToolbarButton = GTWidget::findWidget(os, "translationsMenuToolbarButton");

    class UncheckComplement : public CustomScenario {
        void run (HI::GUITestOpStatus &os) {
            QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            CHECK_SET_ERR(NULL != activePopupMenu, "Active popup menu is NULL");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Set up frames manually");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame -1");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame -2");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame -3");
            GTGlobals::sleep();
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }

    };

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new UncheckComplement));
    GTWidget::click(os, translationsMenuToolbarButton);

//  Check "Show direct only"
    class DirectPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            GET_ACTIONS

            CHECK_SET_ERR(direct1->isChecked(), "check1: direct1 is not checked");
            CHECK_SET_ERR(direct2->isChecked(), "check1: direct2 is not checked");
            CHECK_SET_ERR(direct3->isChecked(), "check1: direct3 is not checked");
            CHECK_SET_ERR(!compl1->isChecked(), "check1: compl1 is checked");
            CHECK_SET_ERR(!compl2->isChecked(), "check1: compl2 is checked");
            CHECK_SET_ERR(!compl3->isChecked(), "check1: compl3 is checked");


            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTGlobals::sleep(200);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new DirectPopupChecker));
    GTWidget::click(os, translationsMenuToolbarButton);

    //    Check "Show complementary only"

    class UncheckDirectCheckComplement : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            CHECK_SET_ERR(NULL != activePopupMenu, "Active popup menu is NULL");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame +1");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame +2");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame +3");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame -1");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame -2");
            GTMenu::clickMenuItemByText(os, activePopupMenu, QStringList() << "Frame -3");
            GTGlobals::sleep();
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new UncheckDirectCheckComplement));
    GTWidget::click(os, translationsMenuToolbarButton);

    class ComplPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            GET_ACTIONS

            CHECK_SET_ERR(!direct1->isChecked(), "check2: direct1 is checked");
            CHECK_SET_ERR(!direct2->isChecked(), "check2: direct2 is checked");
            CHECK_SET_ERR(!direct3->isChecked(), "check2: direct3 is checked");
            CHECK_SET_ERR(compl1->isChecked(), "check2: compl1 is not checked");
            CHECK_SET_ERR(compl2->isChecked(), "check2: compl2 is not checked");
            CHECK_SET_ERR(compl3->isChecked(), "check2: compl3 is not checked");


            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTGlobals::sleep(200);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new ComplPopupChecker));
    GTWidget::click(os, translationsMenuToolbarButton);

    //    Check "Show all"
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"show_all_frames_radiobutton"));
    GTWidget::click(os, translationsMenuToolbarButton);

    class AllPopupChecker : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            GET_ACTIONS

            CHECK_SET_ERR(direct1->isChecked(), "check3: direct1 is checked");
            CHECK_SET_ERR(direct2->isChecked(), "check3: direct2 is checked");
            CHECK_SET_ERR(direct3->isChecked(), "check3: direct3 is checked");
            CHECK_SET_ERR(compl1->isChecked(), "check3: compl1 is not checked");
            CHECK_SET_ERR(compl2->isChecked(), "check3: compl2 is not checked");
            CHECK_SET_ERR(compl3->isChecked(), "check3: compl3 is not checked");


            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTGlobals::sleep(200);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, new AllPopupChecker));
    GTWidget::click(os, translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GET_ACTIONS

GUI_TEST_CLASS_DEFINITION(test_0034){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* panView = GTWidget::findWidget(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QImage init = GTWidget::getImage(os, panView);
//    Create custom ruler
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Rulers" << "Create new ruler"));
    GTUtilsDialog::waitForDialog(os, new CreateRulerDialogFiller(os, "name", 1000));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTGlobals::sleep(500);
    QImage second = GTWidget::getImage(os, panView);
    CHECK_SET_ERR(init != second, "ruler not created");
//    Hide ruler
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Rulers" << "Show Custom Rulers"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTGlobals::sleep(500);
//    Set focus on tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "human_T1.fa"));
    GTMouseDriver::click();
    second = GTWidget::getImage(os, panView);
    CHECK_SET_ERR(init == second, "ruler not hidden");
//    Remove ruler
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Rulers..." << "Remove 'name'"));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Rulers" << "Show Custom Rulers", PopupChecker::IsDisabled));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Rulers" << "Remove 'name'", PopupChecker::NotExists));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os), Qt::RightButton);
    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_0035){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Double click on pan view
    QWidget* panView = GTWidget::findWidget(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, panView, Qt::LeftButton, QPoint(panView->rect().right() - 50, panView->rect().center().y()));
    GTGlobals::sleep(500);
    GTMouseDriver::doubleClick();
//    Expected: Sequence scrolled to clicked position
    QWidget* det = GTWidget::findWidget(os, "det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QScrollBar* scrollBar = det->findChild<QScrollBar*>();
    CHECK_SET_ERR(scrollBar->value() > 150000, QString("Unexpected value: %1").arg(scrollBar->value()));
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0036){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Select 2 annotations.
    GTUtilsAnnotationsTreeView::createAnnotation(os, "new_group", "ann1", "10..20");
    GTUtilsAnnotationsTreeView::createAnnotation(os, "new_group", "ann2", "40..50",false);
    GTUtilsAnnotationsTreeView::selectItems(os, QStringList()<< "ann1" << "ann2");
//    Check "Sequence between selected annotations"  and
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Select"
                                                      << "Sequence between selected annotations"));
    GTMouseDriver::click(Qt::RightButton);
    QVector<U2Region> select = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(select.size() ==1, QString("Wrong number of selections: %1").arg(select.size()));
    U2Region s = select.first();
    CHECK_SET_ERR(s.startPos == 20, QString("Unexpected start pos: %1").arg(s.startPos));
    CHECK_SET_ERR(s.length == 19, QString("Unexpected selection length: %1").arg(s.length));
//    "Sequence around selected annotations" actions
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Select"
                                                      << "Sequence around selected annotations"));
    GTMouseDriver::click(Qt::RightButton);
    select = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(select.size() ==1, QString("Wrong number of selections: %1").arg(select.size()));
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
    PanView* pan = GTWidget::findExactWidget<PanView*>(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    U2Region r = pan->getVisibleRange();
    CHECK_SET_ERR(r.startPos == 9999, QString("Unexpected start: %1").arg(r.startPos));
    CHECK_SET_ERR(r.length == 1001, QString("Unexpected length: %1").arg(r.length));
}

GUI_TEST_CLASS_DEFINITION(test_0037_1) {
//Check defails "zoom to selection" dialog values
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Press zoom to selection button
    GTUtilsDialog::waitForDialog(os, new ZoomToRangeDialogFiller(os));
    GTWidget::click(os, GTAction::button(os, "zoom_to_range_human_T1 (UCSC April 2002 chr7:115977709-117855134)"));
    PanView* pan = GTWidget::findExactWidget<PanView*>(os, "pan_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    U2Region r = pan->getVisibleRange();
    CHECK_SET_ERR(r.startPos == 0, QString("Unexpected start: %1").arg(r.startPos));
    CHECK_SET_ERR(r.length == 199950, QString("Unexpected length: %1").arg(r.length));
}

GUI_TEST_CLASS_DEFINITION(test_0038){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* exportButton = GTWidget::findWidget(os, "export_image");
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
    GTGlobals::sleep(1000);

//    s.type = ExportSequenceImage::DetailsView;
//    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_view_test_0037_1_2.png", s));
//    GTWidget::click(os, exportButton);
//    exists = GTFile::check(os, sandBoxDir + "seq_view_test_0037_1_2.png");
//    CHECK_SET_ERR(exists, "Details view not exported");
    GTGlobals::sleep(1000);
    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_0040){
//scrollbar on seq view
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);
    GTUtilsProjectTreeView::toggleView(os, GTGlobals::UseKey);

    DetView* det = GTWidget::findExactWidget<DetView*>(os, "det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    QScrollBar* scroll = GTScrollBar::getScrollBar(os, "singleline_scrollbar");

    GTWidget::click(os, scroll);
    U2Region r = det->getVisibleRange();
    CHECK_SET_ERR(r.startPos > 89, QString("Unexpected start pos: %1").arg(r.startPos));
}

GUI_TEST_CLASS_DEFINITION(test_0041){
    //test key events
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);

    DetView* det = GTWidget::findExactWidget<DetView*>(os, "det_view_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, det);
    GTGlobals::sleep(500);
    int initLength = det->getVisibleRange().length;

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTGlobals::sleep(500);
    int start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 1, QString("1 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 2, QString("2 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 1, QString("3 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 0, QString("4 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == initLength, QString("5 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 0, QString("6 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_End);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 199950 - initLength, QString("7 Unexpected sequence start: %1").arg(start));

    GTKeyboardDriver::keyClick(Qt::Key_Home);
    GTGlobals::sleep(500);
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start == 0, QString("8 Unexpected sequence start: %1").arg(start));
}

GUI_TEST_CLASS_DEFINITION(test_0042){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Select annotation
    GTUtilsAnnotationsTreeView::clickItem(os, "misc_feature", 1, true);
//    Expected: annotation selected
    QString selected = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(selected == "misc_feature", "Unexpected selected anntoation: " + selected);
//    Click on annotation on seq view with right button
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item", PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_REMOVE
                                                      << "Selected annotations and qualifiers", PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
//    Check context menu action
}

GUI_TEST_CLASS_DEFINITION(test_0043){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    move mouse to annotation on det view
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2);
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(20, 0));
    GTGlobals::sleep();
    QString tooltip = GTUtilsToolTip::getToolTip();
    QString expected = "<table><tr><td bgcolor=#ffff99 bordercolor=black width=15></td><td><big>misc_feature</big></td></tr><tr><td></td><td><b>Location"
            "</b> = 2..590</td></tr><tr><td/><td><nobr><b>note</b> = 5' terminal repeat</nobr><br><nobr><b>Sequence</b> = AATGAAAGACCCCACCCGTAGGTGGCAAGCTAGCTTAAGT"
            " ...</nobr><br><nobr><b>Translation</b> = NERPHP*VAS*LK ...</nobr></td></tr></table>";
    CHECK_SET_ERR(tooltip == expected, "Unexpected toolip: " + tooltip)

}

GUI_TEST_CLASS_DEFINITION(test_0044){
    //Overview weel event
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Overview* over = qobject_cast<Overview*>(GTWidget::findWidget(os, "OverviewRenderArea")->parentWidget()->parentWidget());
    GTWidget::click(os, over);
    GTGlobals::sleep(1000);
    for(int i = 0; i<10; i++){
        GTMouseDriver::scroll(1);
        GTGlobals::sleep(200);
    }
    GTGlobals::sleep(1000);

    U2Region r = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange();
    CHECK_SET_ERR(r.startPos > 99000, QString("Unexpected visible range: %1").arg(r.startPos));
}

GUI_TEST_CLASS_DEFINITION(test_0044_1){
    //selection on overview
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Overview* over = qobject_cast<Overview*>(GTWidget::findWidget(os, "OverviewRenderArea")->parentWidget()->parentWidget());
    GTWidget::click(os, over);

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(100, 0));
    GTMouseDriver::release();
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTThread::waitForMainThread();

    QVector<U2Region> selectionVector = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selectionVector.size() == 1, QString("unexpected number of selected regions: %1").arg(selectionVector.size()));
    CHECK_SET_ERR(selectionVector.first().length >1000, QString("unexpected selection length: &1").arg(selectionVector.first().length))
}

GUI_TEST_CLASS_DEFINITION(test_0044_2){
    //move slider
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    Overview* over = GTUtilsSequenceView::getOverviewByNumber(os);
    GTWidget::click(os, over);
    GTMouseDriver::doubleClick();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(0, over->geometry().height()/3));
    GTGlobals::sleep();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(over->geometry().width()/3, 0));
    GTMouseDriver::release();
    GTGlobals::sleep();

    int start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start>150000, QString("Unexpected selection start: %1").arg(start));

//    GTWidget::click(os, GTUtilsMdi::activeWindow(os));
//    OverviewRenderArea* renderArea = GTWidget::findExactWidget<OverviewRenderArea*>(os, "OverviewRenderArea", over);
//    int detX = renderArea->posToCoord(GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView()->getVisibleRange().startPos) - 6;
//    QRect r = QRect(detX, 8, 12, 10);
//    GTMouseDriver::moveTo(os, renderArea->mapToGlobal(r.center()));
//    GTGlobals::sleep();

}

GUI_TEST_CLASS_DEFINITION(test_0045){
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* pan = GTUtilsSequenceView::getPanViewByNumber(os);
    QImage init = GTWidget::getImage(os, pan);
//show restriction sites
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Restriction Sites"));
    QWidget* qt_toolbar_ext_button = GTWidget::findWidget(os, "qt_toolbar_ext_button",
                                                          GTWidget::findWidget(os, "mwtoolbar_activemdi"), GTGlobals::FindOptions(false));
    if(qt_toolbar_ext_button != NULL && qt_toolbar_ext_button->isVisible()){
        GTWidget::click(os, qt_toolbar_ext_button);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);
//show orfs
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    if(qt_toolbar_ext_button != NULL && qt_toolbar_ext_button->isVisible()){
        GTWidget::click(os, qt_toolbar_ext_button);
        GTGlobals::sleep(1000);
    }
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<QScrollBar*> list = pan->findChildren<QScrollBar*>();
    QScrollBar* vertical = NULL;
    foreach (QScrollBar* b, list) {
        if(b->orientation() == Qt::Vertical){
            vertical = b;
        }
    }
    GTWidget::click(os, vertical);
    GTMouseDriver::scroll(5);
    GTGlobals::sleep(1000);
    QImage final = GTWidget::getImage(os, pan);
    CHECK_SET_ERR(init != final, "pan view was not changed")
}

GUI_TEST_CLASS_DEFINITION(test_0046){
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    for(int i=0; i<5; i++){
        GTWidget::click(os, GTAction::button(os, "action_zoom_in_NC_001363"));
    }
    int vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis>2300, QString("1 wrong visiable range start: %1").arg(vis));

    for(int i=0; i<5; i++){
        GTWidget::click(os, GTAction::button(os, "action_zoom_out_NC_001363"));
    }
    vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis == 0, QString("2 wrong visiable range start: %1").arg(vis));

    for(int i=0; i<5; i++){
        GTWidget::click(os, GTAction::button(os, "action_zoom_in_NC_001363"));
    }
    vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis>2300, QString("3 wrong visiable range start: %1").arg(vis));

    GTWidget::click(os, GTAction::button(os, "action_zoom_to_sequence_NC_001363"));
    vis = GTUtilsSequenceView::getPanViewByNumber(os)->getVisibleRange().startPos;
    CHECK_SET_ERR(vis == 0, QString("4 wrong visiable range start: %1").arg(vis));
}

GUI_TEST_CLASS_DEFINITION(test_0047){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Open CV
    GTUtilsCv::cvBtn::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
//    Use context menu on CV
    GTUtilsDialog::waitForDialog(os, new SetSequenceOriginDialogFiller(os, 1000));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EDIT
                                                      << "Set new sequence origin"));
    GTWidget::click(os, GTWidget::findWidget(os, "CV_ADV_single_sequence_widget_0"), Qt::RightButton);
//    check "Set new sequence origin" action
    GTGlobals::sleep();
    GTUtilsSequenceView::clickAnnotationDet(os, "CDS", 43);
}

GUI_TEST_CLASS_DEFINITION(test_0048){
//    Open murine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Use context menu on annotation in tree view
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, "murine.gb", "NC_001363 features"));
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList()<<"Add"<<"Objects with annotations..."));
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView(), Qt::RightButton);
//    Check {add-> Objects with annotations} action
    GTUtilsAnnotationsTreeView::findItem(os, "NC_001363 features [murine.gb]");
}

GUI_TEST_CLASS_DEFINITION(test_0050){
    class WrongNameChecker : public CustomScenario {
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Illegal annotation name"));
            QLineEdit* nameEdit = GTWidget::findExactWidget<QLineEdit*>(os, "leAnnotationName", dialog);
            GTLineEdit::setText(os, nameEdit, "//");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    class WrongDistanceChecker : public CustomScenario {
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QRadioButton* gbFormatLocation = dialog->findChild<QRadioButton*>("rbGenbankFormat");
            CHECK_SET_ERR(gbFormatLocation != NULL, "radio button rbGenbankFormat not found");
            GTRadioButton::click(os, gbFormatLocation);

            QLineEdit* locationEdit = GTWidget::findExactWidget<QLineEdit*>(os, "leLocation", dialog);
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
    GTUtilsAnnotationsTreeView::selectItems(os, QList<QTreeWidgetItem*>() << GTUtilsAnnotationsTreeView::findItem(os, "CDS"));

    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, new WrongNameChecker));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, new WrongDistanceChecker));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_0050_1){
    class custom : public CustomScenario {
    public:
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Genes" << "promoter"));
            GTWidget::click(os, GTWidget::findWidget(os, "showNameGroupsButton", dialog));
            QLineEdit* nameEdit = GTWidget::findExactWidget<QLineEdit*>(os, "nameEdit", dialog);
            CHECK_SET_ERR(nameEdit->text() == "promoter", "unexpected name: " + nameEdit->text());

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsAnnotationsTreeView::selectItems(os, QList<QTreeWidgetItem*>() << GTUtilsAnnotationsTreeView::findItem(os, "CDS"));

    GTUtilsDialog::waitForDialog(os, new EditAnnotationFiller(os, new custom));
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep(1000);

}

GUI_TEST_CLASS_DEFINITION(test_0051){

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT"));

    QColor init = GTUtilsAnnotHighlightingTreeView::getItemColor(os, "CDS");
    CHECK_SET_ERR(init.name() == "#9bffff", "unexpected CDS annotations color: " + init.name());

    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, 255, 0, 0));
    GTUtilsAnnotHighlightingTreeView::click(os, "CDS");
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(90, 0));
    GTMouseDriver::click();
    GTGlobals::sleep(1000);

    QColor final = GTUtilsAnnotHighlightingTreeView::getItemColor(os, "CDS");
    CHECK_SET_ERR(final.name() == "#ff0000", "CDS annotations color changed wrong: " + init.name());

}

GUI_TEST_CLASS_DEFINITION(test_0052){
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    DetView* det = GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView();
    GTWidget::click(os, det);
    GTGlobals::sleep(1000);
    QImage image1 = GTWidget::getImage(os, det);

    GTWidget::click(os, GTAction::button(os, "complement_action"));
    GTGlobals::sleep(1000);
    GTWidget::click(os, det);
    GTGlobals::sleep(1000);
    QImage image2 = GTWidget::getImage(os, det);

    GTWidget::click(os, GTAction::button(os, "complement_action"));
    GTGlobals::sleep(1000);
    GTWidget::click(os, det);
    GTGlobals::sleep(1000);
    QImage image3 = GTWidget::getImage(os, det);

    CHECK_SET_ERR(image1 != image2, "Image was not changed");
    CHECK_SET_ERR(image1 == image3, "Image was not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0052_1){
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    DetView* det = GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView();
    GTWidget::click(os, det);
    GTGlobals::sleep(1000);
    QImage image1 = GTWidget::getImage(os, det);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "show_all_frames_radiobutton"));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
    GTGlobals::sleep(2000);
    GTWidget::click(os, det);
    GTGlobals::sleep(1000);
    QImage image2 = GTWidget::getImage(os, det);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "do_not_translate_radiobutton"));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
    GTGlobals::sleep(2000);
    GTWidget::click(os, det);
    GTGlobals::sleep(1000);
    QImage image3 = GTWidget::getImage(os, det);

    CHECK_SET_ERR(image1 != image2, "Image was not changed");
    CHECK_SET_ERR(image1 == image3, "Image was not restored");
}

GUI_TEST_CLASS_DEFINITION(test_0053){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)", GTGlobals::UseMouse));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Add label with shift+left mouse
    QWidget* graphView = GTUtilsSequenceView::getGraphView(os);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTWidget::click(os, graphView, Qt::LeftButton, QPoint(50, 50));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    QList<QVariant> list = GTUtilsSequenceView::getLabelPositions(os, GTUtilsSequenceView::getGraphView(os));
    CHECK_SET_ERR(list.size() == 1, QString("unexpected number of labels: %1").arg(list.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0054){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"GC Content (%)"));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTGlobals::sleep(500);
//    Add label with shift+left mouse
    GSequenceGraphView* graphView = GTUtilsSequenceView::getGraphView(os);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTWidget::click(os, graphView, Qt::LeftButton, QPoint(50, 50));
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep(1000);
//    Use context menu {graph->Graph settings}
    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1, -1, 0, 0, 255, 0, 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Graph"
                                                      << "visual_properties_action"));
    GTWidget::click(os, graphView, Qt::RightButton);
    QColor c = GTUtilsSequenceView::getGraphColor(os, graphView);
    QString s = c.name();
    CHECK_SET_ERR(s == "#ff0000", "unexpected color: " + s);
    GTGlobals::sleep(500);
//    In settings dialog change label color
}

GUI_TEST_CLASS_DEFINITION(test_0055){
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"GC Content (%)"));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTGlobals::sleep(500);
    QWidget* graphView = GTUtilsSequenceView::getGraphView(os);
    GTWidget::click(os, graphView);
    //GTWidget::getAllWidgetsInfo(os, GTUtilsSequenceView::getGraphView(os));
    QWidget* pointer = NULL;
    foreach (QWidget* w, GTUtilsSequenceView::getGraphView(os)->findChildren<QWidget*>()) {
        if(w->geometry().width() == 10){
            pointer = w;
        }
    }
    CHECK_SET_ERR(pointer != NULL, "pointer not found");

//    Use context menu {graph->Graph settings}
    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1,-1, 10, 15));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Graph"
                                                      << "visual_properties_action"));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTWidget::click(os, GTUtilsSequenceView::getGraphView(os));
    QPoint p1 = pointer->mapToGlobal(pointer->geometry().center());

    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1,-1, 100, 150));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Graph"
                                                      << "visual_properties_action"));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTWidget::click(os, GTUtilsSequenceView::getGraphView(os));
    QPoint p2 = pointer->mapToGlobal(pointer->geometry().center());

    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, -1,-1, 10, 150));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Graph"
                                                      << "visual_properties_action"));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTWidget::click(os, GTUtilsSequenceView::getGraphView(os));
    QPoint p3 = pointer->mapToGlobal(pointer->geometry().center());

    int y1 = p1.y();
    int y2 = p2.y();
    int y3 = p3.y();
    CHECK_SET_ERR(y2>y3 && y1<y3, QString("unexpected pointer coordinates: %1 %2 %3").arg(y1).arg(y2).arg(y3));
    GTGlobals::sleep();

//    In settings dialog set proper cutoff values
}


GUI_TEST_CLASS_DEFINITION(test_0056){
    //wrong cutoff values
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"GC Content (%)"));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTGlobals::sleep(500);
    QWidget* graphView = GTUtilsSequenceView::getGraphView(os);


    class custom : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "Invalid cutoff range"));
            QGroupBox* minmaxGroup = GTWidget::findExactWidget<QGroupBox*>(os, "minmaxGroup", dialog);
            minmaxGroup->setChecked(true);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTGlobals::sleep(500);
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new GraphSettingsDialogFiller(os, new custom));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Graph"
                                                      << "visual_properties_action"));
    GTWidget::click(os, graphView, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0057){
    //Uses processor core!!!
//    Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Open any graph
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"GC Content (%)"));
    GTWidget::click(os, GTWidget::findWidget(os, "GraphMenuAction"));
    GTGlobals::sleep(500);
    GSequenceGraphView* graphView = GTUtilsSequenceView::getGraphView(os);

//    Use context menu {graph->Select all extremum points}
    GTUtilsDialog::waitForDialog(os, new GraphLabelsSelectDialogFiller(os, 5000));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Graph"
                                                      << "select_all_extremum_points"));
    GTWidget::click(os, graphView, Qt::RightButton);
    GTGlobals::sleep();
//    In dialog select any value
    int labelsNum = GTUtilsSequenceView::getGraphLabels(os, graphView).size();
    CHECK_SET_ERR(labelsNum == 81, QString("unexpected labels number: %1").arg(labelsNum))
}

GUI_TEST_CLASS_DEFINITION(test_0058){
    // complex test on local toolbars buttons

    GTFileDialog::openFile(os, dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAction* zoomIn = GTAction::findAction(os, "action_zoom_in_A1#berezikov");
    CHECK_SET_ERR(zoomIn != NULL, "Cannot find action_zoom_in_A1#berezikov");

    GTWidget::click(os, GTAction::button(os, zoomIn));
    GTWidget::click(os, GTAction::button(os, zoomIn));
    GTWidget::click(os, GTAction::button(os, zoomIn));

    QWidget* chromView = GTWidget::findWidget(os, "chromatogram_view_A1#berezikov");
    CHECK_SET_ERR(chromView != NULL, "Cannot find chromatogram_view_A1#berezikov");

    QImage image = GTWidget::getImage(os, chromView);

    QAction* bars = GTAction::findActionByText(os, "Show quality bars");
    CHECK_SET_ERR(bars, "Cannot find 'Show quality bars' action");
    GTWidget::click(os, GTAction::button(os, bars));

    CHECK_SET_ERR(image != GTWidget::getImage(os, chromView), "Nothing changed on Chromatogram View after Bars adding");

    image = GTWidget::getImage(os, chromView);

    QAction* traces = GTAction::findActionByText(os, "Show/hide trace");
    CHECK_SET_ERR(traces != NULL, "Cannot find 'Show/hide trace' action");

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "A"));
    GTWidget::click(os, GTAction::button(os, traces));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "C"));
    GTWidget::click(os, GTAction::button(os, traces));

    CHECK_SET_ERR(image != GTWidget::getImage(os, chromView), "Nothing changed on Chromatogram View after Traces hiding");
}

GUI_TEST_CLASS_DEFINITION(test_0059){
    //"Invert annotation selection" action test
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList()<<"Invert annotation selection"));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, GTUtilsAnnotationsTreeView::findItem(os, "CDS"));
    QList<QTreeWidgetItem*> selected = GTUtilsAnnotationsTreeView::getAllSelectedItems(os);
    CHECK_SET_ERR(selected.size() == 7, QString("Unexpected number of selected items: %1").arg(selected.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0059_1){
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ORFs"));
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "CreateAnnotationDialog"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList()<<"Make auto-annotations persistent"));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 27)"));
    GTGlobals::sleep();

    QTreeWidgetItem* orf = GTUtilsAnnotationsTreeView::findItem(os, "orf  (0, 27)");
    QString s = orf->parent()->text(0);
    CHECK_SET_ERR(s == "NC_001363 features [murine.gb] *", "unexpected parent: " + s)
}

GUI_TEST_CLASS_DEFINITION(test_0060){
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget* nameLabel = GTWidget::findWidget(os, "nameLabel");
    GTMouseDriver::moveTo(GTWidget::getWidgetCenter(os, nameLabel) + QPoint(2*nameLabel->geometry().width()/3, 0));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep(2000);

    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_NC_001363", NULL, false);
    QWidget *DetailsViewSe2 = GTWidget::findWidget(os, "det_view_NC_001363", NULL, false);
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_NC_001363", NULL, false);
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_NC_001363", NULL, false);
    CHECK_SET_ERR(overViewSe2->isVisible() == false &&
        DetailsViewSe2->isVisible() == false &&
        zoomViewSe2->isVisible() == false
        , "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible() == true, "toolbar is hidden");

}

GUI_TEST_CLASS_DEFINITION(test_0061_1) {
    //1. Open "data/Samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Check that first sequence name is '1CF7 chain A sequence'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7 chain A sequence"));
    //3. Check that first annotation table name is '1CF7 chain A annotation'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7 chain A annotation"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "1CF7 chain A annotation [1CF7.PDB]"));
    //4. Check that there is 'molecule_name' qualifier with value 'PROTEIN (TRANSCRIPTION FACTOR E2F-4)'  in 'chain_info' annotation for chain A
    QString moleculeName = GTUtilsAnnotationsTreeView::getQualifierValue(os, "molecule_name", GTUtilsAnnotationsTreeView::findItem(os, "chain_info"));
    CHECK_SET_ERR(moleculeName == "PROTEIN (TRANSCRIPTION FACTOR E2F-4)", QString("Incorrect molecule name is detected for chain A: %1").arg(moleculeName));
    QTreeWidgetItem* annotationsChainB = GTUtilsAnnotationsTreeView::findItem(os, "1CF7 chain B annotation [1CF7.PDB]");
    //5. Check that there is 'molecule_name' qualifier with value 'PROTEIN (TRANSCRIPTION FACTOR DP-2)'  in 'chain_info' annotation for chain B
    CHECK_SET_ERR(NULL != annotationsChainB, QString("Can't find '1CF7 chain B annotation [1CF7.PDB]' annotations document"));
    QTreeWidgetItem* chainInfoGroup = GTUtilsAnnotationsTreeView::findItem(os, "chain_info  (0, 1)", annotationsChainB);
    CHECK_SET_ERR(NULL != chainInfoGroup, QString("Can't find 'chain_info' annotation group for chain B"));
    QTreeWidgetItem* chainInfo = GTUtilsAnnotationsTreeView::findItem(os, "chain_info", chainInfoGroup);
    CHECK_SET_ERR(NULL != chainInfo, QString("Can't find 'chain_info' annotation for chain B"));
    QString moleculeNameChainB = GTUtilsAnnotationsTreeView::getQualifierValue(os, "molecule_name", chainInfo);

    CHECK_SET_ERR(moleculeNameChainB == "PROTEIN (TRANSCRIPTION FACTOR DP-2)", QString("Incorrect molecule name is detected for chain B: %1").arg(moleculeNameChainB));
}

GUI_TEST_CLASS_DEFINITION(test_0061_2) {
    //1. Open "_common_data/pdb/1CRN_without_compnd_tag.PDB"
    GTFileDialog::openFile(os, testDir + "_common_data/pdb", "1CRN_without_compnd_tag.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Check that first annotation table name is '1CRN chain A sequence'
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CRN chain A annotation"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "1CRN chain A annotation [1CRN_without_compnd_tag.PDB]"));

    //3. Check that there is 'chain_id' qualifier with value 'A'  in 'chain_info' annotation for chain A
    QString chainId = GTUtilsAnnotationsTreeView::getQualifierValue(os, "chain_id" ,"chain_info");
    CHECK_SET_ERR("A" == chainId, QString("Incorrect 'chain_info' qualifier value: %1").arg(chainId));
    //4. Check that there is not 'molecule_name' qualifier
    QTreeWidgetItem *moleculeName = GTUtilsAnnotationsTreeView::findItem(os, "molecule_name", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == moleculeName, QString("There is 'moleculeName' qualifier"));
}

GUI_TEST_CLASS_DEFINITION(test_0061_3) {
    //1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Call context menu on the 3dview, select {Structural Alignment -> Align With...} menu item.
    //3. Check that chain combobox contains identifiers: A, B, C, D.
    //4. Accept the dialog.
    QStringList chainIndexes;
    chainIndexes << "A" << "B" << "C" << "D";
    GTUtilsDialog::waitForDialog(os, new StructuralAlignmentDialogFiller(os, chainIndexes));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Structural Alignment" << "align_with"));
    QWidget *widget3d = GTWidget::findWidget(os, "1-1CF7");
    CHECK_SET_ERR(NULL != widget3d, "3D widget was not found");
    GTWidget::click(os, widget3d, Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
//    1. Open any sequence (e.g. murine.gb)
//    2. "Wrap sequence" button is checked by default
//    Expected state: vertical scrollbar is present, horizontal scrollbar is hidden, sequence is split into lines
//    3. Uncheck "Wrap sequence" button on the local toolbar
//    Expected state: the view is not resized, horizontal scrollbar appears, vertical scrollbar is hidden, sequence is in one line

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != NULL, "No sequence widget found");
    QSize seqWgtSize = seqWgt->size();

    QScrollBar* scrollBar = GTScrollBar::getScrollBar(os, "multiline_scrollbar");
    CHECK_SET_ERR(scrollBar != NULL, "Cannot find multiline_scrollbar");
    CHECK_SET_ERR(scrollBar->isVisible(), "multiline_scrollbar is hidden");

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);
    CHECK_SET_ERR(seqWgtSize == seqWgt->size(), "Multi-line mode resized the view");
    CHECK_SET_ERR(scrollBar->isHidden(), "multiline_scrollbar is visible");

    QScrollBar* hScrollBar = GTScrollBar::getScrollBar(os, "singleline_scrollbar");
    CHECK_SET_ERR(hScrollBar != NULL, "Cannot find singleline_scrollbar");
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

    int pos = 789;
    GTUtilsSequenceView::goToPosition(os, pos);
    GTGlobals::sleep();
    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(pos), "Visible range does not contain 789 position");
    pos = visibleRange.startPos;

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    GTWidget::click(os, wrapButton);
    GTGlobals::sleep();

    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.startPos == pos, "Visible range does not contain requeried position");
    GTUtilsSequenceView::goToPosition(os, pos + 20);
    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    pos = visibleRange.startPos;

    GTWidget::click(os, wrapButton);
    GTGlobals::sleep();
    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(pos), "Start position of visible range was changed");
}

GUI_TEST_CLASS_DEFINITION(test_0064) {
//    1. Open any sequence (e.g. murine.gb)
//    2. Scroll with the mouse wheel to the end of the sequence and back to the beginning

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "seq4.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    QScrollBar* scrollBar = GTScrollBar::getScrollBar(os, "multiline_scrollbar");
    CHECK_SET_ERR(scrollBar != NULL, "Cannot find multiline_scrollbar");
    GTGlobals::sleep(500);
    GTScrollBar::moveSliderWithMouseWheelDown(os, scrollBar, scrollBar->maximum());
    GTGlobals::sleep();

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(GTUtilsSequenceView::getSeqWidgetByNumber(os)->getSequenceLength() - 1), "The end position of the sequence is not visible. Failed to scroll to the end_1" );

    GTScrollBar::moveSliderWithMouseWheelUp(os, scrollBar, scrollBar->maximum());
    GTGlobals::sleep(500);

    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    CHECK_SET_ERR(visibleRange.contains(1), "The end position of the sequence is not visible. Failed to scroll to the end_2" );
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

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    GTWidget::setFocus(os, GTUtilsSequenceView::getSeqWidgetByNumber(os)->getDetView());

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleRange(os).startPos, "Visible range was changed");

    for (int i = 0; i < 5; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Up);
    }
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisibleRange(os).startPos, "Visible range was changed");

    visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    GTGlobals::sleep();
    CHECK_SET_ERR(visibleRange.startPos != GTUtilsSequenceView::getVisibleRange(os).startPos, "Visible range was not changed on page down");
    visibleRange = GTUtilsSequenceView::getVisibleRange(os);

    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    GTGlobals::sleep();
    CHECK_SET_ERR(visibleRange.startPos != GTUtilsSequenceView::getVisibleRange(os).startPos, "Visible range was not changed on page up");
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
    GTUtilsSequenceView::goToPosition(os, 1000);
    GTGlobals::sleep();

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);
    QSplitter* splitter = qobject_cast<QSplitter*>(GTWidget::findWidget(os, "annotated_DNA_splitter"));
    CHECK_SET_ERR(splitter != NULL, "Cannot find annotated_DNA_splitter");
    QWidget* bottomSplitterHandle = splitter->handle( splitter->count() - 1);//GTWidget::findWidget(os, "qt_splithandle_", GTWidget::findWidget(os, "annotated_DNA_splitter"));
    CHECK_SET_ERR(bottomSplitterHandle != NULL, "Cannot find bottom splitter handle");
    GTWidget::click(os, bottomSplitterHandle);
    QPoint p1 = GTMouseDriver::getMousePosition();
    QPoint p2 = QPoint(p1.x(), p1.y() + 100);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    GTGlobals::sleep();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisiableStart(os), "Start position of visible range was changed on enlarge at the bottom");

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisiableStart(os), "Start position of visible range was changed on reduce at the bottom");

    QWidget* topSplitterHandle = GTWidget::findWidget(os, "qt_splithandle_det_view_NC_004718");
    CHECK_SET_ERR(topSplitterHandle != NULL, "Cannot find qt_splithandle_det_view_NC_004718");
    GTWidget::click(os, topSplitterHandle);
    p1 = GTMouseDriver::getMousePosition();
    p2 = QPoint(p1.x(), p1.y() - 100);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(p2);
    GTMouseDriver::release();
    GTGlobals::sleep();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisiableStart(os), "Start position of visible range was changed on enlarge at the top");

    GTMouseDriver::press();
    GTMouseDriver::moveTo(p1);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    CHECK_SET_ERR(visibleRange.startPos == GTUtilsSequenceView::getVisiableStart(os), "Start position of visible range was changed on reduce at the top");
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
    QWidget* showDetViewButton = GTWidget::findWidget(os, "show_hide_details_view");
    CHECK_SET_ERR(showDetViewButton != NULL, "Cannot find show_hide_details_view button");
    GTWidget::click(os, showDetViewButton);

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != NULL, "No sequence widget found");

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
    CHECK_SET_ERR(p.size() != QSize() && p.size() !=  seqWgt->getDetView()->getDetViewRenderArea()->size(), "Exported image size is incorrect");
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

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");
    U2Region visibleRange = GTUtilsSequenceView::getVisibleRange(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "show_all_frames_radiobutton"));
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));
    CHECK_SET_ERR(visibleRange != GTUtilsSequenceView::getVisibleRange(os), "Visible range was not changed on translation show/hide");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(1000);
}

GUI_TEST_CLASS_DEFINITION(test_0069) {
//    1. Open any sequence (e.g. murine.gb)
//    2. Resize the view to see a few lines
//    3. Press the mouse button on one line (start the selection)
//    4. Move the mouse to the other line and release the button (finish the selection)
//    Expected state: the sequence between press position and release position is selected (the line is selected)

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != NULL, "Cannot find sequence widget");
    CHECK_SET_ERR(GTUtilsSequenceView::getSelection(os).isEmpty(), "Selection is not empty");

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
    ADVSingleSequenceWidget* seqWgt = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqWgt != NULL, "Cannot find sequence widget");

    QAbstractButton* wrapButton = GTAction::button(os, "wrap_sequence_action");
    CHECK_SET_ERR(wrapButton->isChecked(), "Multi-line mode is unexpectedly inactive");

    ExportSequenceImage::Settings s(ExportSequenceImage::DetailsView, U2Region(1, 2000));
    GTUtilsDialog::waitForDialog(os, new ExportSequenceImage(os, sandBoxDir + "seq_image_0071", s));
    GTWidget::click(os, GTAction::button(os, "export_image"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QPixmap p(sandBoxDir + "seq_image_0071");
    CHECK_SET_ERR(p.size() != QSize() && p.size() !=  seqWgt->getDetView()->getDetViewRenderArea()->size(), "Exported image size is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_0075) {
    //1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");

    //2. Click the translations toolbar button.
    GTWidget::click(os, GTWidget::findWidget(os, "translationsMenuToolbarButton"));

    //Expected: the menu appears.
    QMenu *menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    CHECK_SET_ERR(NULL != menu, "No menu");

    //3. Click "Show/hide translations".
    GTMenu::clickMenuItem(os, menu, "do_not_translate_radiobutton");

    //Expected: the menu doesn't disappear.
    CHECK_SET_ERR(NULL != QApplication::activePopupWidget(), "Menu disappeared 1");

    //4. Click "Show all".
    GTMenu::clickMenuItemByText(os, menu, QStringList() << "Show all frames");

    //Expected: the menu doesn't disappear.
    CHECK_SET_ERR(NULL != QApplication::activePopupWidget(), "Menu disappeared 2");

    //5. Click somewhere else.
    GTWidget::click(os, GTUtilsMdi::activeWindow(os));

    //Expected: the menu disappears.
    CHECK_SET_ERR(NULL == QApplication::activePopupWidget(), "Menu is shown");
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
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "EcoRI"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "SYNPBR322"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Mark as circular"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "EcoRI"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "EcoRI", GTGlobals::FindOptions(false)) == NULL, "EcoRI is unexpectedly found");
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
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList defaultEnzymes = QStringList() << "ClaI" << "DraI" << "EcoRI";
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, defaultEnzymes, 4200, 10));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "ClaI", GTGlobals::FindOptions(false)) == NULL, "ClaI is unexpectedly found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "DraI", GTGlobals::FindOptions(false)) == NULL, "DraI is unexpectedly found");
    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "EcoRI");
    CHECK_SET_ERR(region == "join(4359..4361,1..3)", QString("EcoRI region is incorrect: %1").arg(region));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, defaultEnzymes, 3900, 300, 4300, 10));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "EcoRI", GTGlobals::FindOptions(false)) == NULL, "EcoRI is unexpectedly found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "ClaI") != NULL, "ClaI is unexpectedly not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "DraI") != NULL, "DraI is unexpectedly not found");
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
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class RegionSelectorChecker : public Filler {
    public:
        RegionSelectorChecker(HI::GUITestOpStatus &os)
            : Filler(os, "FindEnzymesDialog") {}

        void run() {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            QWidget* regionSelector = GTWidget::findWidget(os, "region_selector_with_excluded");
            CHECK_SET_ERR(regionSelector != NULL, "region_selector_with_excluded not found");

            QLineEdit* start = GTWidget::findExactWidget<QLineEdit*>(os, "startLineEdit", regionSelector);
            CHECK_SET_ERR(start != NULL, "startLineEdit of 'Search In' region not found");
            GTLineEdit::setText(os, start, "5000");

            QLineEdit* end = GTWidget::findExactWidget<QLineEdit*>(os, "endLineEdit", regionSelector);
            CHECK_SET_ERR(end != NULL, "endLineEdit of 'Search In' region not found");
            GTLineEdit::setText(os, end, "1000");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(os, start, "1");

            QCheckBox* exclude = GTWidget::findExactWidget<QCheckBox*>(os, "excludeCheckBox");
            GTCheckBox::setChecked(os, exclude);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new RegionSelectorChecker(os));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
}

} // namespace GUITest_common_scenarios_sequence_view

} // namespace U2
