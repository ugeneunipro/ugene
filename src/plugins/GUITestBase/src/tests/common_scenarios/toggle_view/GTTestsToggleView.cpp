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
#include <primitives/PopupChooser.h>

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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed
    auto sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2", nullptr, false);
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2", nullptr, false);
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2", nullptr, false);
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2", nullptr, false);
    CHECK_SET_ERR(overViewSe2 == nullptr && detailsViewSe2 == nullptr && zoomViewSe2 == nullptr && toolBarSe2 == nullptr, "there are widgets of ADV_single_sequence_widget");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Remove sequence.
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se1 sequence has been closed
    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, false);
    CHECK_SET_ERR(sequenceWidget1 == nullptr, "sequenceWidget is present");

    auto overViewSe1 = GTWidget::findWidget(os, "overview_se1", nullptr, false);
    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1", nullptr, false);
    auto zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1", nullptr, false);
    auto toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1", nullptr, false);
    CHECK_SET_ERR(overViewSe1 == nullptr && detailsViewSe1 == nullptr && zoomViewSe1 == nullptr && toolBarSe1 == nullptr, "there are widgets of ADV_single_sequence_widget");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTUtilsMdi::click(os, GTGlobals::Close);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, false);
    CHECK_SET_ERR(sequenceWidget1 == nullptr, "sequenceWidget is present");

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    QWidget* activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");
}

GUI_TEST_CLASS_DEFINITION(test_0001_3) {
    // Close all MDI windows.
    GTUtilsMdi::click(os, GTGlobals::Close);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");
    toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");
}

GUI_TEST_CLASS_DEFINITION(test_0001_4) {
    // Close all MDI windows.
    GTUtilsMdi::click(os, GTGlobals::Close);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto toolbar = GTWidget::findToolBar(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1", nullptr, false);
    CHECK_SET_ERR(sequenceWidget2 == nullptr, "sequenceWidget is present");

    toolbar = GTWidget::findToolBar(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_sequence", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, false);
    CHECK_SET_ERR(sequenceWidget1 == nullptr, "sequenceWidget is present");

    auto activeMDIWindow = GTUtilsMdi::activeWindow(os, false);
    CHECK_SET_ERR(activeMDIWindow == nullptr, "there is active MDI window");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "se2"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findToolBar(os, "views_tool_bar_se2");
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
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

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide all. CHANGES: using 'Toggle views' instead 'Toggle view'
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleAllSequenceViews"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for se2 sequence has been closed, but toolbar still present.
    GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible() && !detailsViewSe2->isVisible() && !zoomViewSe2->isVisible(), "there are widgets not hidden widgets of ADV_single_sequence_widget");
    CHECK_SET_ERR(toolBarSe2->isVisible(), "toolbar is hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show all. CHANGES: using 'Toggle views' instead 'Toggle view'
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleAllSequenceViews"}));
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
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
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show details.
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show details. CHANGES: using 'Toggle views' instead 'Toggle view', clicking twice to avoid misunderstanding
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // checking hiding all det.views
    detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    CHECK_SET_ERR(!detailsViewSe2->isVisible() && !detailsViewSe1->isVisible(), "details view isn't hidden");

    // clicking 2nd time
    // toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
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

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide details. CHANGES: hiding all views for all sequences, instead hiding only details
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleAllSequenceViews"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: detailed view for se2 sequence has been closed.
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(!detailsViewSe2->isVisible(), "details view isn't hidden");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show details.
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
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
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible")

    // 3. Click on toolbar button Show overview for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Toggle view for sequence se2. Click menu item Hide overview. CHANGES: using 'Toggle views' instead 'Toggle view'
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible");

    // 3. Click on toolbar button Show overview for sequence se2.
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide overview for sequence se2.
    auto toolbar = GTWidget::findToolBar(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible");

    // 3. Click on toolbar button Toggle view for sequence se2. Click menu item Show overview. CHANGES: using 'Toggle views' instead 'Toggle view'
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_3) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide overview for sequence se1.
    auto toolbar = GTWidget::findToolBar(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se1");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible");

    // 3. Click on toolbar button Show overview for sequence se1.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0004_4) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Click on toolbar button Hide all for sequence se2.
    auto toolbar = GTWidget::findToolBar(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_all_views", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    CHECK_SET_ERR(!overViewSe2->isVisible(), "overview is visible")

    // 3. Click on toolbar button Show overview for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: overview for se2 sequence has been appeared
    CHECK_SET_ERR(overViewSe2->isVisible(), "overview is hidden")
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa, open file as separate sequences
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");

    // 2. Click on toolbar button Toggle views. Click menu item Hide all overviews.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: panoramic views for both sequences has been closed, all zoom buttons at sequence views becomes disabled
    CHECK_SET_ERR(!overViewSe1->isVisible() && !overViewSe2->isVisible(),
                  "panoramic views for both sequences has not been closed");

    // 3. Click on toolbar button Toggle views. Click menu item Show all overviews.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // QWidget *_overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    // QWidget *_overViewSe2 = GTWidget::findWidget(os, "overview_se2");

    // Expected state: panoramic view for both sequences has been appeared
    CHECK_SET_ERR(overViewSe1->isVisible() && overViewSe2->isVisible(),
                  "panoramic view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto toolBarSe1 = GTWidget::findWidget(os, "tool_bar_se1");
    auto toolBarSe2 = GTWidget::findWidget(os, "tool_bar_se2");
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");

    // 2. Click on toolbar button Toggle views. Click menu item Hide all details.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: views for both sequences has been closed, but toolbars still present.
    CHECK_SET_ERR(!detailsViewSe1->isVisible() && !detailsViewSe2->isVisible(), "panoramic view for both sequences has not been shown");
    CHECK_SET_ERR(toolBarSe1->isVisible() && toolBarSe2->isVisible(), "panoramic view for both sequences has not been shown");

    // 3. Click on toolbar button Toggle views. Click menu item Show all details.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: view for both sequences has been appeared
    CHECK_SET_ERR(detailsViewSe1->isVisible() && detailsViewSe2->isVisible(),
                  "panoramic view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");
    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    auto zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    auto toolBarSe1 = GTWidget::findToolBar(os, "tool_bar_se1");
    auto toolBarSe2 = GTWidget::findToolBar(os, "tool_bar_se2");
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");

    // 2. Click on toolbar button Toggle views. Click menu item Hide all sequences.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleAllSequenceViews"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: all views for both sequences has been closed, but toolbars still present.
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(), "panoramic views for both sequences has not been closed");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");
    CHECK_SET_ERR(zoomViewSe1->isHidden() && zoomViewSe2->isHidden(), "zoom views for both sequences has not been closed");
    CHECK_SET_ERR(!toolBarSe1->isHidden() && !toolBarSe2->isHidden(), "toolbars view for both sequences has not been shown");

    // 3. Click on toolbar button Toggle views. Click menu item Show all sequences.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleAllSequenceViews"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: all views for both sequences has been appeared
    CHECK_SET_ERR(!overViewSe1->isHidden() && !overViewSe2->isHidden(), "panoramic view for both sequences has not been shown");
    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe2->isHidden(), "details view for both sequences has not been shown");
    CHECK_SET_ERR(!zoomViewSe1->isHidden() && !zoomViewSe2->isHidden(), "zoom view for both sequences has not been shown");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar* toolBarSe2 = mainWindow->findChild<QToolBar*>("tool_bar_se2");
    CHECK_SET_ERR(toolBarSe2 != nullptr, "Tool bar not found");

    QAbstractButton* zoomButton = GTAction::button(os, "zoom_to_range_se2");
    QAbstractButton* zoomInButton = GTAction::button(os, "action_zoom_in_se2");
    QAbstractButton* zoomOutButton = GTAction::button(os, "action_zoom_out_se2");

    // 2. Click on toolbar button Zoom to range for sequence se2. Select region 20..50, then click OK.
    GTUtilsDialog::waitForDialog(os, new ZoomToRangeDialogFiller(os, 20, 50));
    if (zoomButton->isVisible()) {
        GTWidget::click(os, zoomButton);
    } else {
        // the button is hidden, the action is in toolbar extension menu
        auto extMenuButton = GTWidget::findWidget(os, "qt_toolbar_ext_button", GTWidget::findWidget(os, "pan_view_se2"));
        GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Zoom to range..."}));
        GTWidget::click(os, extMenuButton);
    }
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: sequence overview show 20..50 sequence region, all zoom buttons at this toolbar becomes enabled
    //  TODO: Expected state: sequence overview show 20..50 sequence region
    CHECK_SET_ERR(zoomInButton->isEnabled(), "Zoom In button is not enabled");
    CHECK_SET_ERR(zoomOutButton->isEnabled(), "Zoom Out button is not enabled");

    // 3. Click on toolbar button Hide zoom view for sequence se2.
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: panoramic view for se2 sequence has been closed, all zoom buttons at this toolbar becomes disabled
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramic view for se2 sequence has been not closed");
    CHECK_SET_ERR(!zoomInButton->isEnabled(), "Zoom In button is enabled");
    CHECK_SET_ERR(!zoomOutButton->isEnabled(), "Zoom Out button is enabled");

    // 4. Click on toolbar button Show zoom view for sequence se2.
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: panoramic view for se2 sequence has been appeared, all zoom buttons at this toolbar becomes enabled
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramic view for se2 sequence has been not appeared");
    CHECK_SET_ERR(zoomInButton->isEnabled(), "Zoom In button is not enabled");
    CHECK_SET_ERR(zoomOutButton->isEnabled(), "Zoom Out button is not enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    auto overViewSe1 = GTWidget::findWidget(os, "overview_se1");
    auto overViewSe2 = GTWidget::findWidget(os, "overview_se2");

    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(overViewSe2->isHidden(), "panoramic view for se2 sequence has been not closed");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(), "panoramic views for both sequences has been not closed");

    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_overview", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!overViewSe2->isHidden(), "panoramic view for se2 sequence has been not shown");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleOverview"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(overViewSe1->isHidden() && overViewSe2->isHidden(), "panoramic view for both sequences has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar* toolBarSe1 = mainWindow->findChild<QToolBar*>("tool_bar_se1");
    CHECK_SET_ERR(toolBarSe1 != nullptr, "Tool bar not found");

    GTAction::button(os, "zoom_to_range_se1");
    GTAction::button(os, "action_zoom_out_se1");
    QAbstractButton* zoomInButton = GTAction::button(os, "action_zoom_in_se1");

    // 2. Press 'Zoom in' button for seq1.
    GTWidget::click(os, zoomInButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    CHECK_SET_ERR(!zoomViewSe1->isHidden(), "panoramic view for se1 sequence has been not appeared");

    // 3. Close zoom view for seq1
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Zoom area (grey rectangle) has disappeared from overview for seq1
    CHECK_SET_ERR(zoomViewSe1->isHidden(), "panoramic view for se1 sequence has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar* toolBarSe1 = mainWindow->findChild<QToolBar*>("tool_bar_se1");
    QToolBar* toolBarSe2 = mainWindow->findChild<QToolBar*>("tool_bar_se2");
    CHECK_SET_ERR(toolBarSe1 != nullptr, "Tool bar not found");
    CHECK_SET_ERR(toolBarSe2 != nullptr, "Tool bar not found");

    GTAction::button(os, "action_zoom_in_se1");
    GTAction::button(os, "action_zoom_in_se2");
    QAbstractButton* zoomInButton1 = GTAction::button(os, "action_zoom_in_se1");
    GTAction::button(os, "action_zoom_out_se1");
    QAbstractButton* zoomInButton2 = GTAction::button(os, "action_zoom_in_se2");
    GTAction::button(os, "action_zoom_out_se2");

    // 2. Press 'Zoom in' button for both. CHANGES: for both instead of seq1
    GTWidget::click(os, zoomInButton1);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    CHECK_SET_ERR(!zoomViewSe1->isHidden(), "panoramic view for se1 sequence has been not appeared");

    GTWidget::click(os, zoomInButton2);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramic view for se1 sequence has been not appeared");

    // 3. Close zoom view for both. CHANGES: for both instead of seq1
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Zoom area (grey rectangle) has disappeared from overview for both. CHANGES: for both instead of seq1
    CHECK_SET_ERR(zoomViewSe1->isHidden(), "panoramic view for se1 sequence has been not closed");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramic view for se2 sequence has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    QWidget* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QToolBar* toolBarSe1 = mainWindow->findChild<QToolBar*>("tool_bar_se1");
    QToolBar* toolBarSe2 = mainWindow->findChild<QToolBar*>("tool_bar_se2");
    CHECK_SET_ERR(toolBarSe1 != nullptr, "Tool bar not found");
    CHECK_SET_ERR(toolBarSe2 != nullptr, "Tool bar not found");

    GTAction::button(os, "zoom_to_range_se1");
    GTAction::button(os, "zoom_to_range_se2");
    QAbstractButton* zoomInButton1 = GTAction::button(os, "action_zoom_in_se1");
    GTAction::button(os, "action_zoom_out_se1");
    QAbstractButton* zoomInButton2 = GTAction::button(os, "action_zoom_in_se2");
    GTAction::button(os, "action_zoom_out_se2");

    // 2. Press 'Zoom in' button for both. CHANGES: for both instead of seq1
    GTWidget::click(os, zoomInButton1);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto zoomViewSe1 = GTWidget::findWidget(os, "pan_view_se1");
    CHECK_SET_ERR(!zoomViewSe1->isHidden(), "panoramic view for se1 sequence has been not appeared");

    GTWidget::click(os, zoomInButton2);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    auto zoomViewSe2 = GTWidget::findWidget(os, "pan_view_se2");
    CHECK_SET_ERR(!zoomViewSe2->isHidden(), "panoramic view for se1 sequence has been not appeared");

    // 3. Close zoom views by global Toggle View Button. CHANGES: for both instead of seq1 + global Toggle View Button instead of the seq1 and buttons

    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleZoomView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: Zoom area (grey rectangle) has disappeared from overview for both.
    CHECK_SET_ERR(zoomViewSe1->isHidden(), "panoramic view for se1 sequence has been not closed");
    CHECK_SET_ERR(zoomViewSe2->isHidden(), "panoramic view for se2 sequence has been not closed");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator(Yellow triangle) on overview has disappeared from overview
}

GUI_TEST_CLASS_DEFINITION(test_0012_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view CHANGES: for 1 and 2 instead of global button
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator(Yellow triangle) on overview has disappeared from overview
}

GUI_TEST_CLASS_DEFINITION(test_0012_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe1->isHidden(),
                  "details views for both sequences has not been show");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe1->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator(Yellow triangle) on overview has disappeared from overview
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappeared from zoom view
}
GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view CHANGES: for 1 and 2 instead of global button
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappeared from zoom view
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Close detailed view
    auto toggleViewButton = GTWidget::findWidget(os, "toggleViewButton");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto detailsViewSe1 = GTWidget::findWidget(os, "det_view_se1");
    auto detailsViewSe2 = GTWidget::findWidget(os, "det_view_se2");
    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe2->isHidden(),
                  "details views for both sequences has not been closed");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!detailsViewSe1->isHidden() && !detailsViewSe1->isHidden(), "details views for both sequences has not been show");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"toggleDetailsView"}));
    GTWidget::click(os, toggleViewButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(detailsViewSe1->isHidden() && detailsViewSe1->isHidden(), "details views for both sequences has not been closed");

    // TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappeared from zoom view
}
GUI_TEST_CLASS_DEFINITION(test_0014) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}
    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, circularViewSe1);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Close detailed view
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    //  TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappeared from graph view
}
GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}. Changes: for both sequences
    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, circularViewSe1);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto circularViewSe2 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget2);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, circularViewSe2);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Close detailed view. Changes: for both sequences
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    //  TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappeared from graph view for both sequences
}
GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    // 1. Use menu {File->Open}. Open file _common_data/scenarios/project/multiple.fa
    Runnable* dialog = new SequenceReadingModeSelectorDialogFiller(os);
    GTUtilsDialog::waitForDialog(os, dialog);
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}. Changes: for 1 sequences
    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, circularViewSe1);

    //  3. Close detailed view. Changes: for 1 sequences
    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_se1");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  2. Open graph view {Graphs->CG% content}. Changes: for 2 sequences
    auto sequenceWidget2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    auto circularViewSe2 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget2);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, circularViewSe2);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  3. Close detailed view. Changes: for 2 sequences
    toolbar = GTWidget::findWidget(os, "views_tool_bar_se2");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_details_view", toolbar));
    //  TODO: Expected state: position indicator (Grey dotted line rectangle)  has disappeared from graph view for both sequences
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // 1. Use menu {File->Open}. Open file _common_data/genome_aligner/chrY.fa
    GTUtilsProject::openFile(os, testDir + "_common_data/genome_aligner/chrY.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    //  2. Open graph view {Graphs->CG% content}
    auto sequenceWidget1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    auto circularViewSe1 = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget1);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
    GTWidget::click(os, circularViewSe1);
    GTUtilsTask::waitTaskStart(os, "Calculate graph points", 10000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"GC Content (%)"}));
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

    auto toolbar = GTWidget::findWidget(os, "views_tool_bar_SYNPBR322");
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
