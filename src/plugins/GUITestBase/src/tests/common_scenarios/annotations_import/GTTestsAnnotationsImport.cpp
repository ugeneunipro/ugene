/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include "GTTestsAnnotationsImport.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/dna_export/ImportAnnotationsToCsvFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_annotations_import {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 2, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 0, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns2.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "[sep123]", 0, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns2.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "[sep123]", 0, "#", false, true, "AUTO", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns2.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, false, true, "[sep123]", 0, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns3.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 0, "$#_[[sA", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns3.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, false, true, ",", 0, "$#_[[sA", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns3.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 0, "$#_[[sA", false, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(true, 6)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);

    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(true, 3)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::LengthParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::LengthParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(true, 3)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::LengthParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns2.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "[sep123]", 0, "#", true, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns2.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "[sep123]", 0, "#", false, true, "MISC", roleParameters);

    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns2.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, "[sep123]", 0, "#", true, true, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0008_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", false, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0008_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::NameParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::QualifierParameter("Qual")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns1.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 0, "#", false, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("a1");
}
GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::IgnoreParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::StrandMarkParameter(true, "yes")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns5.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", true, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("MISC");
}
GUI_TEST_CLASS_DEFINITION(test_0009_1) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::IgnoreParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::StrandMarkParameter(true, "yes")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns5.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", true, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("MISC");
}
GUI_TEST_CLASS_DEFINITION(test_0009_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_import/", "se1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    ImportAnnotationsToCsvFiller::RoleParameters roleParameters;
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(0, new ImportAnnotationsToCsvFiller::IgnoreParameter()));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(1, new ImportAnnotationsToCsvFiller::StartParameter(false)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(2, new ImportAnnotationsToCsvFiller::EndParameter(true)));
    roleParameters.append(ImportAnnotationsToCsvFiller::RoleColumnParameter(3, new ImportAnnotationsToCsvFiller::StrandMarkParameter(true, "yes")));

    Runnable* filler = new ImportAnnotationsToCsvFiller(testDir + "_common_data/scenarios/annotations_import/anns5.csv", testDir + "_common_data/scenarios/sandbox/result.gb", ImportAnnotationsToCsvFiller::Genbank, true, true, ",", 1, "#", true, false, "MISC", roleParameters);
    GTUtilsDialog::waitForDialog(filler);
    GTUtilsDialog::waitForDialog(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, "import_annotations_from_CSV_file"}));
    GTUtilsProjectTreeView::click("se1.fa", Qt::RightButton);
    GTUtilsProjectTreeView::findIndex("result.gb");
    GTUtilsAnnotationsTreeView::findItem("MISC");
}
}  // namespace GUITest_common_scenarios_annotations_import
}  // namespace U2
