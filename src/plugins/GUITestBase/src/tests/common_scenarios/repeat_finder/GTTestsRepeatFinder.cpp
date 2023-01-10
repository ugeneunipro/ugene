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

#include "GTTestsRepeatFinder.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>

#include <QTreeWidgetItem>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/FindRepeatsDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_repeat_finder {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    // The Test Runs Repeat Finder then checks if the qualifier "repeat homology" exists in resulting annotations
    //
    // Steps:
    // 1. Use menu {File->Open}. Open file _common_data/fasta/seq4.fa.
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/", "seq4.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "seq4.fa");

    // 2. Run Find Repeats dialog
    Runnable* swDialog = new FindRepeatsDialogFiller(os, testDir + "_common_data/scenarios/sandbox/");
    GTUtilsDialog::waitForDialog(os, swDialog);

    GTMenu::clickMainMenuItem(os, {"Actions", "Analyze", "Find repeats..."}, GTGlobals::UseMouse);

    // 3. Close sequence view, then reopen it
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTMouseDriver::click();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "seq4.fa"));
    GTMouseDriver::doubleClick();

    // 4. Check that annotation has the qualifier "repeat homology"
    QTreeWidgetItem* repeatsGroupItem = GTUtilsAnnotationsTreeView::findItem(os, "repeat_unit  (0, 325)");
    GTTreeWidget::expand(os, repeatsGroupItem);
    GTTreeWidget::expand(os, repeatsGroupItem->child(0));

    GTUtilsAnnotationsTreeView::findItem(os, "repeat_identity");
}

}  // namespace GUITest_common_scenarios_repeat_finder

}  // namespace U2
