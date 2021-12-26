/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/PopupChooser.h>

#include <QApplication>
#include <QClipboard>
#include <QMainWindow>

#include <U2Core/AppContext.h>

#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTTestsToggleView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectorFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_toggle_view {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Remove sequence.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed
    QWidget *sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2", nullptr, false);
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2", nullptr, false);
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2", nullptr, false);
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2", nullptr, false);
    CHECK_SET_ERR(overViewSe2 == nullptr && detailsViewSe2 == nullptr && zoomViewSe2 == nullptr && toolBarSe2 == nullptr, "there are widgets of ADV_single_sequence_widget");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Remove sequence.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se1 sequence has been closed
    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, false);
    CHECK_SET_ERR(sequenceWidget1 == nullptr, "sequenceWidget is present");

    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1", nullptr, false);
    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1", nullptr, false);
    QWidget *zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1", nullptr, false);
    QWidget *toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1", nullptr, false);
    CHECK_SET_ERR(overViewSe1 == nullptr && detailsViewSe1 == nullptr && zoomViewSe1 == nullptr && toolBarSe1 == nullptr, "there are widgets of ADV_single_sequence_widget");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTUtilsMdi::click(os, GTGlobals::Close);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, false);
    CHECK_SET_ERR(sequenceWidget1 == nullptr, "sequenceWidget is present");

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    QWidget *activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");
}

GUI_TEST_CLASS_DEFINITION(test_0001_3) {
    // Close all MDI windows.
    GTUtilsMdi::click(os, GTGlobals::Close);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");
    toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");
}

GUI_TEST_CLASS_DEFINITION(test_0001_4) {
    // Close all MDI windows.
    GTUtilsMdi::click(os, GTGlobals::Close);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, false);
    CHECK_SET_ERR(sequenceWidget1 == nullptr, "sequenceWidget is present");

    QWidget *activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "se2"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide all for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible() && !detailsViewSe2->isVisible() && !zoomViewSe2->isVisible(), "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");

    // 3. Click on toolbar button Show all for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been appeared
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(overViewSe2->isVisible() && detailsViewSe2->isVisible() && zoomViewSe2->isVisible(), "there are widgets hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide all for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible() && !detailsViewSe2->isVisible() && !zoomViewSe2->isVisible(), "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");

    // 3. On toolbar for sequence se2: click the button for show each view.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been appeared
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");

    overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(overViewSe2->isVisible() && detailsViewSe2->isVisible() && zoomViewSe2->isVisible(), "there are widgets hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. On toolbar for sequence se2: click the button for each view.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible() && !detailsViewSe2->isVisible() && !zoomViewSe2->isVisible(),
                  "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");

    // 3. Click on toolbar button Show all for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been appeared
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(overViewSe2->isVisible() && detailsViewSe2->isVisible() && zoomViewSe2->isVisible(), "there are widgets hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0002_3) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Hide zoom view. Click on toolbar button Hide all for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible() && !detailsViewSe2->isVisible() && !zoomViewSe2->isVisible(), "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");

    // 3. Click on toolbar button Show all for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been appeared
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(overViewSe2->isVisible() && detailsViewSe2->isVisible() && zoomViewSe2->isVisible(),
                  "there are widgets hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0002_4) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide all. CHANGES: using 'Toggle views' insetead 'Toggle view'
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleAllSequenceViews"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible() && !detailsViewSe2->isVisible() && !zoomViewSe2->isVisible(), "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show all. CHANGES: using 'Toggle views' insetead 'Toggle view'
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleAllSequenceViews"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been appeared
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(overViewSe2->isVisible() && detailsViewSe2->isVisible() && zoomViewSe2->isVisible(), "there are widgets hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide details for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Show details for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been appeared
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe2->isVisible(), "details view is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide details. CHANGES: using 'Toggle views' instead 'Toggle view'
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show details.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been appeared
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe2->isVisible(), "details view is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide details for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show details. CHANGES: using 'Toggle views' instead 'Toggle view', clicking twice to avoid missunderstanding
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // checking hiding all detviews
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    CHECK_SET_ERR(!detailsViewSe2->isVisible() && !detailsViewSe1->isVisible(), "details view isn't hidden");

    // clicking 2nd time
    // toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Expected state: detailed view for se2 sequence has been appeared
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe2->isVisible(), "details view is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0003_3) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide all for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Show details for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been appeared
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe2->isVisible(), "details view is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0003_4) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide details. CHANGES: hiding all views for all sequences, instead hidding only deatails
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleAllSequenceViews"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show details.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been appeared
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe2->isVisible(), "details view is hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide overview for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible")

    // 3. Click on toolbar button Show overview for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide overview. CHANGES: using 'Toggle views' instead 'Toggle view'
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible");

    // 3. Click on toolbar button Show overview for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide overview for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show overview. CHANGES: using 'Toggle views' instead 'Toggle view'
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_3) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide overview for sequence se1.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se1");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible");

    // 3. Click on toolbar button Show overview for sequence se1.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_4) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide all for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible")

    // 3. Click on toolbar button Show overview for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: over view for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa, open file as separate sequences
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");

    // 2. Click on toolbar button Toggle views. Click menu item Hide all overviews.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: panoramical views for both sequences has been closed, all zoom buttons at sequence views becomes disabled
    CHECK_SET_ERR(!overViewSe1->isVisible() && !overViewSe2->isVisible(),
                  "panoramical views for both sequences has not been closed");

    // 3. Click on toolbar button Toggle views. Click menu item Show all overviews.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // QWidget *_overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    // QWidget *_overViewSe2 = GTWidget::findWidget(os, "overview_se2");

    // Expected state: panoramical view for both sequences has been appeared
    CHECK_SET_ERR(overViewSe1->isVisible() && overViewSe2->isVisible(),
                  "panoramical view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");

    // 2. Click on toolbar button Toggle views. Click menu item Hide all details.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for both sequences has been closed, but toolbars still present.
    CHECK_SET_ERR(!detailsViewSe1->isVisible() && !detailsViewSe2->isVisible(), "panoramical view for both sequences has not been shown");
    CHECK_SET_ERR(toolBarSe1->isVisible() && toolBarSe2->isVisible(), "panoramical view for both sequences has not been shown");

    // 3. Click on toolbar button Toggle views. Click menu item Show all details.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: view for both sequences has been appeared
    CHECK_SET_ERR(detailsViewSe1->isVisible() && detailsViewSe2->isVisible(),
                  "panoramical view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    QWidget *zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    QWidget *toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1");
    QWidget *toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");

    // 2. Click on toolbar button Toggle views. Click menu item Hide all sequences.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleAllSequenceViews"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: all views for both sequences has been closed, but toolbars still present.
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(), "panoramical views for both sequences has not been closed");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");
    CHECK_SET_ERR(zoomViewSe1->isHidden() && zoomViewSe2->isHidden(), "zoom views for both sequences has not been closed");
    CHECK_SET_ERR(!toolBarSe1->isHidden() && !toolBarSe2->isHidden(), "toolbars view for both sequences has not been shown");

    // 3. Click on toolbar button Toggle views. Click menu item Show all sequences.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleAllSequenceViews"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: all views for both sequences has been appeared
    CHECK_SET_ERR(!overViewSe1->isHidden() && !overViewSe2->isHidden(), "panoramical view for both sequences has not been shown");
    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe2->isHidden(), "details view for both sequences has not been shown");
    CHECK_SET_ERR(!zoomViewSe1->isHidden() && !zoomViewSe2->isHidden(), "zoom view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar *toolBarSe2 = mainWindow->findChild<QToolBar *>("tool_bar_se2");
    CHECK_SET_ERR(toolBarSe2 != nullptr, "Tool bar not found");

    QAbstractButton *zoomButton = GTAction::button(os, "zoom_to_range_se2");
    QAbstractButton *zoomInButton = GTAction::button(os, "action_zoom_in_se2");
    QAbstractButton *zoomOutButton = GTAction::button(os, "action_zoom_out_se2");

    // 2. Click on toolbar button Zoom to range for sequence se2. Select region 20..50, then click OK.
    GTUtilsDialog::waitForDialog(os, new ZoomToRangeDialogFiller(os, 20, 50));
    if (zoomButton->isVisible()) {
        GTWidget::click(os, zoomButton);
    } else {
        // the button is hidden, the action is in toolbar extension menu
        QWidget *extMenuButton = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "pan_view_se2"));
        GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Zoom to range..."));
        GTWidget::click(os, extMenuButton);
    }
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: sequence overview show 20..50 sequence region, all zoom buttons at this toolbar becomes enabled
    //  TODO: Expected state: sequence overview showh 20..50 sequence region
    CHECK_SET_ERR(zoomInButton->isEnabled(), "Zoom In button is not enabled");
    CHECK_SET_ERR(zoomOutButton->isEnabled(), "Zoom Out button is not enabled");

    // 3. Click on toolbar button Hide zoom view for sequence se2.
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: panoramical view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramical view for se2 sequence has been not closed");
    CHECK_SET_ERR(!zoomInButton->isEnabled(), "Zoom In button is enabled");
    CHECK_SET_ERR(!zoomOutButton->isEnabled(), "Zoom Out button is enabled");

    // 4. Click on toolbar button Show zoom view for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: panoramical view for se2 sequence has been appeared, all zoom buttons at this toolbar becomes enabled
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramical view for se2 sequence has been not appeared");
    CHECK_SET_ERR(zoomInButton->isEnabled(), "Zoom In button is not enabled");
    CHECK_SET_ERR(zoomOutButton->isEnabled(), "Zoom Out button is not enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    QWidget *overViewSe2 = GTWidget::findWidget(os, "overview_se2");

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(overViewSe2->isHidden(), "panoramical view for se2 sequence has been not closed");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(), "panoramical views for both sequences has been not closed");

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!overViewSe2->isHidden(), "panoramical view for se2 sequence has been not shown");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleOverview"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(), "panoramical view for both sequences has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar *toolBarSe1 = mainWindow->findChild<QToolBar *>("tool_bar_se1");
    CHECK_SET_ERR(toolBarSe1 != nullptr, "Tool bar not found");

    GTAction::button(os, "zoom_to_range_se1");
    GTAction::button(os, "action_zoom_out_se1");
    QAbstractButton *zoomInButton = GTAction::button(os, "action_zoom_in_se1");

    // 2. Press 'Zoom in' button for seq1.
    GTWidget::click(os, zoomInButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    CHECK_SET_ERR(!zoomViewSe1->isHidden(), "panoramical view for se1 sequence has been not appeared");

    // 3. Close zoom view for seq1
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Zoom area (grey rectangle) has disappered from overview for seq1
    CHECK_SET_ERR(zoomViewSe1->isHidden(), "panoramical view for se1 sequence has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar *toolBarSe1 = mainWindow->findChild<QToolBar *>("tool_bar_se1");
    QToolBar *toolBarSe2 = mainWindow->findChild<QToolBar *>("tool_bar_se2");
    CHECK_SET_ERR(toolBarSe1 != nullptr, "Tool bar not found");
    CHECK_SET_ERR(toolBarSe2 != nullptr, "Tool bar not found");

    GTAction::button(os, "action_zoom_in_se1");
    GTAction::button(os, "action_zoom_in_se2");
    QAbstractButton *zoomInButton1 = GTAction::button(os, "action_zoom_in_se1");
    GTAction::button(os, "action_zoom_out_se1");
    QAbstractButton *zoomInButton2 = GTAction::button(os, "action_zoom_in_se2");
    GTAction::button(os, "action_zoom_out_se2");

    // 2. Press 'Zoom in' button for both. CHANGES: for both instead of seq1
    GTWidget::click(os, zoomInButton1);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    CHECK_SET_ERR(!zoomViewSe1->isHidden(), "panoramical view for se1 sequence has been not appeared");

    GTWidget::click(os, zoomInButton2);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramical view for se1 sequence has been not appeared");

    // 3. Close zoom view for both. CHANGES: for both instead of seq1
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Zoom area (grey rectangle) has disappered from overview for both. CHANGES: for both instead of seq1
    CHECK_SET_ERR(zoomViewSe1->isHidden(), "panoramical view for se1 sequence has been not closed");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramical view for se2 sequence has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar *toolBarSe1 = mainWindow->findChild<QToolBar *>("tool_bar_se1");
    QToolBar *toolBarSe2 = mainWindow->findChild<QToolBar *>("tool_bar_se2");
    CHECK_SET_ERR(toolBarSe1 != nullptr, "Tool bar not found");
    CHECK_SET_ERR(toolBarSe2 != nullptr, "Tool bar not found");

    GTAction::button(os, "zoom_to_range_se1");
    GTAction::button(os, "zoom_to_range_se2");
    QAbstractButton *zoomInButton1 = GTAction::button(os, "action_zoom_in_se1");
    GTAction::button(os, "action_zoom_out_se1");
    QAbstractButton *zoomInButton2 = GTAction::button(os, "action_zoom_in_se2");
    GTAction::button(os, "action_zoom_out_se2");

    // 2. Press 'Zoom in' button for both. CHANGES: for both instead of seq1
    GTWidget::click(os, zoomInButton1);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    CHECK_SET_ERR(!zoomViewSe1->isHidden(), "panoramical view for se1 sequence has been not appeared");

    GTWidget::click(os, zoomInButton2);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QWidget *zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramical view for se1 sequence has been not appeared");

    // 3. Close zoom views by global Toogle View Button. CHANGES: for both instead of seq1 + global Toggle View Button instead of the seq1 and buttons

    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleZoomView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Zoom area (grey rectangle) has disappered from overview for both.
    CHECK_SET_ERR(zoomViewSe1->isHidden(), "panoramical view for se1 sequence has been not closed");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramical view for se2 sequence has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator(Yellow triangle) on overview has disappered from overview
}

GUI_TEST_CLASS_DEFINITION(test_0012_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view CHANGES: for 1 and 2 instead of global button
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator(Yellow triangle) on overview has disappered from overview
}

GUI_TEST_CLASS_DEFINITION(test_0012_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe1->isHidden(),
                  "details views for both sequences has not been show");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe1->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator(Yellow triangle) on overview has disappered from overview
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappered from zoom view
}
GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view CHANGES: for 1 and 2 instead of global button
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappered from zoom view
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    QWidget *toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    QWidget *detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(),
                  "details views for both sequences has not been closed");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe1->isHidden(), "details views for both sequences has not been show");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggleDetailsView"));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe1->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappered from zoom view
}
GUI_TEST_CLASS_DEFINITION(test_0014) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}
    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe1);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Close detailed view
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    //  TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappered from graph view
}
GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}. Changes: for both sequences
    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe1);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *circularViewSe2 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget2);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe2);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Close detailed view. Changes: for both sequences
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    //  TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappered from graph view for both sequences
}
GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable *dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}. Changes: for 1 sequences
    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe1);

    //  3. Close detailed view. Changes: for 1 sequences
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  2. Open graph view {Graphs->CG% content}. Changes: for 2 sequences
    QWidget *sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    QWidget *circularViewSe2 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget2);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe2);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Close detailed view. Changes: for 2 sequences
    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    //  TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappered from graph view for both sequences
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // 1. Use menu {File->Open}. Open file _common_data/genome_aligner/chrY.fa
    GTUtilsProject::openFile(os, testDir + "_common_data/genome_aligner/chrY.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}
    QWidget *sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    QWidget *circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe1);
    GTUtilsTask::waitTaskStart(os, "Calculate graph points", 10000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "GC Content (%)"));
    GTWidget::click(os, circularViewSe1);
    CHECK_SET_ERR(GTUtilsTaskTreeView::countTasks(os, "Calculate graph points") == 0, "Calculation task was not cancelled");
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    // 1. Open PBR322.gb
    // 2. Close all views one by one: Overview, Zoom view and Details view
    // 3. Click "Show all" button
    // 4. Click "Remove sequence" button

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/PBR322.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_SYNPBR322");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

}  // namespace GUITest_common_scenarios_toggle_view
}  // namespace U2
