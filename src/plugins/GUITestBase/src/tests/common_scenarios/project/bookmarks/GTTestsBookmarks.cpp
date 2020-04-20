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

#include "GTTestsBookmarks.h"
#include "GTGlobals.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include "primitives/GTMenu.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "GTUtilsProject.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "utils/GTUtilsApp.h"
#include "utils/GTUtilsToolTip.h"
#include <base_dialogs/MessageBoxFiller.h>
#include "GTUtilsMdi.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsSequenceView.h"
#include <primitives/GTTreeWidget.h>
#include "GTUtilsMdi.h"
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/MaEditorFactory.h>
#include <base_dialogs/GTFileDialog.h>
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

namespace U2{

namespace GUITest_common_scenarios_project_bookmarks {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0002) {

//  1. Open a GenBank file with circular view (tested on NC_014267.gb from NCBI DB)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/" , "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

//  2. Create a bookmark
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_ADD_BOOKMARK, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 [s] NC_014267 sequence"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

//  3. Close project
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Close project");
    GTGlobals::sleep();
}
GUI_TEST_CLASS_DEFINITION(test_0002_1) {

    // 1. Open a GenBank file with circular view (tested on NC_014267.gb from NCBI DB)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/" , "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    // 2. Create a bookmark. Change: Create 2 bookmarks.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_ADD_BOOKMARK, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 [s] NC_014267 sequence"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_ADD_BOOKMARK, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 [s] NC_014267 sequence"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    // 3. Close project
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Close project");
    GTGlobals::sleep();
}
GUI_TEST_CLASS_DEFINITION(test_0002_2) {

    // 1. Open a GenBank file with circular view (tested on NC_014267.gb from NCBI DB)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/" , "NC_014267.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    // 2. Create a bookmark
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_ADD_BOOKMARK, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 [s] NC_014267 sequence"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    //2'. Change: Remove the bookmark
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_REMOVE_BOOKMARK, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsBookmarksTreeView::getItemCenter(os, "NC_014267 [s] NC_014267 sequence"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    // 3. Close project
    GTUtilsDialog::waitForDialog(os,new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Close project");
    GTGlobals::sleep();
}
} // namespace
} // namespace U2


