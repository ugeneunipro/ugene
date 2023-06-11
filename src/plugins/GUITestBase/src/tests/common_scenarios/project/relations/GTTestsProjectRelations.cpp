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

#include "GTTestsProjectRelations.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>
#include <system/GTFile.h>

#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTGlobals.h"
#include "GTUtilsDocument.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"

namespace U2 {

namespace GUITest_common_scenarios_project_relations {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/project/proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTUtilsDocument::checkDocument("1.gb", AnnotatedDNAViewFactory::ID);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    QString filePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString fileName = "proj4.uprj";
    QString firstAnn = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnn = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFile::copy(firstAnn, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnn, sandBoxDir + "/" + secondAnnFileName);
    GTFileDialog::openFile(sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("1.gb");
    GTUtilsDocument::checkDocument("2.gb");

    QModelIndex parent = GTUtilsProjectTreeView::findIndex("1.gb");
    QModelIndex child = GTUtilsProjectTreeView::findIndex("NC_001363 features", parent);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(child));
    GTMouseDriver::doubleClick();
    GTUtilsDocument::checkDocument("1.gb", AnnotatedDNAViewFactory::ID);

    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished();
}

}  // namespace GUITest_common_scenarios_project_relations

}  // namespace U2
