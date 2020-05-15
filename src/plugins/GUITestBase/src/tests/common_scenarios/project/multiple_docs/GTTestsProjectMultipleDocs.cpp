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

#include "GTTestsProjectMultipleDocs.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>

#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTGlobals.h"
#include "GTUtilsDocument.h"
#include "GTUtilsProject.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "utils/GTUtilsApp.h"

namespace U2 {

namespace GUITest_common_scenarios_project_multiple_docs {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state:
    // 	1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Use menu {File->Save Project As}
    // Expected state: "Save project as" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    // 	{Project name:} proj2
    // 	{Project Folder:} _common_data/scenarios/sandbox
    // 	{Project file} proj2
    // 4. Click Save button
    GTUtilsDialog::waitForDialog(os, new SaveProjectAsDialogFiller(os, "proj2", testDir + "_common_data/scenarios/sandbox/proj2"));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Save project as...");
    GTGlobals::sleep();

    // 5. Use menu {File->Open}. Open file samples/PDB/1CF7.PDB
    GTFileDialog::openFile(os, dataDir + "samples/PDB/", "1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state:
    // 	1) Project view with documents "1CF7.PDB", "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1CF7.PDB");
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 6. Close project
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");
    GTGlobals::sleep();

    // 7. Open project from the location used in item 3
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state:
    // Project has 2 documents: 1CF7.PDB and 1.gb
    GTUtilsDocument::checkDocument(os, "1.gb");
    GTUtilsDocument::checkDocument(os, "1CF7.PDB");
}

}    // namespace GUITest_common_scenarios_project_multiple_docs

}    // namespace U2
