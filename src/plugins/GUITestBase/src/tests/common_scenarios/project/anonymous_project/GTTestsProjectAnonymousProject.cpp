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

#include "GTTestsProjectAnonymousProject.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTMouseDriver.h>

#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTGlobals.h"
#include "GTUtilsDocument.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/ExportProjectDialogFiller.h"
#include "utils/GTUtilsToolTip.h"

namespace U2 {

namespace GUITest_common_scenarios_project_anonymous_project {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 1. Use menu {File->Open}. Open file data/samples/PDB/1CF7.pdb
    GTFileDialog::openFile(dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //     1) Project view with document "1CF7.PDB" is opened
    GTUtilsDocument::checkDocument("1CF7.PDB");

    // 2. Use menu {File->Export Project}
    // Expected state: "Export Project" dialog has appeared
    // 3. Fill the next field in dialog:
    //     {Destination folder} _common_data/scenarios/sandbox
    //     {Project file name} proj2.uprj
    // 4. Click OK button
    // 5. Click NO in opened messagebox
    GTUtilsDialog::waitForDialog(new ExportProjectDialogFiller(testDir + "_common_data/scenarios/sandbox/proj2.uprj"));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::clickMainMenuItem({"File", "Export project..."});

    // 6. Use menu {File->Close project}
    // 7. Click NO in opened messagebox
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    // 8. Use menu {File->Open}. Open project _common_data/scenarios/sandbox/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //     1) project view with document "1CF7.PDB" has been opened,
    GTUtilsDocument::checkDocument("1CF7.PDB");

    //     3) File path at tooltip for "1CF7.PDB" must be "_common_data/scenarios/sandbox/1CF7.PDB"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7.PDB"));
    GTUtilsToolTip::checkExistingToolTip("_common_data/scenarios/sandbox/1CF7.PDB");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Use menu {File->Open}. Open file samples/PDB/1CF7.pdb
    GTFileDialog::openFile(dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //     1) Project view with document "1CF7.PDB" is opened
    GTUtilsDocument::checkDocument("1CF7.PDB");

    // 2. Use menu {File->Save Project As}
    // Expected state: "Save project as" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Project name:} proj2
    //     {Project Folder:} _common_data/scenarios/sandbox
    //     {Project file} proj2
    // 4. Click Save button
    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("proj2", testDir + "_common_data/scenarios/sandbox/proj2"));
    GTMenu::clickMainMenuItem({"File", "Save project as..."});

    // 5. Use menu {File->Close project}
    GTMenu::clickMainMenuItem({"File", "Close project"});

    // Expected state: project and sequence view closed
    GTUtilsProject::checkProject(GTUtilsProject::NotExists);

    // 6. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //     1) project view with document "1CF7.PDB" has been opened,
    GTUtilsDocument::checkDocument("1CF7.PDB");
    //     3) File path at tooltip for "1CF7.PDB" must be "samples/PDB/1CF7.PDB"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7.PDB"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));
    GTUtilsToolTip::checkExistingToolTip("samples/PDB/1CF7.PDB");
}

}  // namespace GUITest_common_scenarios_project_anonymous_project

}  // namespace U2
