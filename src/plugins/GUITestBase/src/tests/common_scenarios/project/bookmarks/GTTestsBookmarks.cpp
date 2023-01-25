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

#include "GTTestsBookmarks.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>

#include "GTGlobals.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_project_bookmarks {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0002) {
    //  1. Open a GenBank file with circular view (tested on NC_014267.gb from NCBI DB)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/", "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //  2. Create a bookmark
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_ADD_BOOKMARK}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 sequence [NC_014267.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    //  3. Close project
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
}
GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // 1. Open a GenBank file with circular view (tested on NC_014267.gb from NCBI DB)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/", "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Create a bookmark. Change: Create 2 bookmarks.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_ADD_BOOKMARK}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 sequence [NC_014267.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_ADD_BOOKMARK}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 sequence [NC_014267.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    // 3. Close project
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
}
GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // 1. Open a GenBank file with circular view (tested on NC_014267.gb from NCBI DB)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/", "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Create a bookmark
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_ADD_BOOKMARK}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 sequence [NC_014267.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    // 2'. Change: Remove the bookmark
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {ACTION_REMOVE_BOOKMARK}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 sequence [NC_014267.gb]"));
    GTMouseDriver::click(Qt::RightButton);

    // 3. Close project
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, {"File", "Close project"});
}
}  // namespace GUITest_common_scenarios_project_bookmarks
}  // namespace U2
