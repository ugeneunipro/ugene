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

#include "GTTestsAnnotationsQualifiers.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <utils/GTThread.h>

#include <QHeaderView>
#include <QTreeWidgetItem>

#include <U2View/ADVConstants.h>
#include <U2View/AnnotationsTreeView.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"
#include "system/GTFile.h"

namespace U2 {

namespace GUITest_common_scenarios_annotations_qualifiers {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");
    GTThread::waitForMainThread();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual1", "val1"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");

    GTUtilsAnnotationsTreeView::findItem("qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual1", "val1"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");

    GTUtilsAnnotationsTreeView::findItem("qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::expandItem("C");  // Expand annotation to make qualifiers visible.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual1"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    GTUtilsAnnotationsTreeView::findItem("qu");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::expandItem("B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    GTUtilsAnnotationsTreeView::findItem("qu");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsAnnotationsTreeView::expandItem("B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsAnnotationsTreeView::expandItem("C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual1"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu1"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsAnnotationsTreeView::findItem("qu");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::expandItem("C");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu", "va"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual1"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::findItem("qu");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::expandItem("B");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu", "va"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::findItem("qu");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::expandItem("B");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu", "va"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::expandItem("C");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qu1", "va1"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("qual1"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::findItem("qu");
    GTUtilsAnnotationsTreeView::findItem("qu1");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTUtilsProjectTreeView::doubleClickItem("NC_001363 sequence");

    auto annotationCItem = GTUtilsAnnotationsTreeView::expandItem("C");
    auto qualifierCItem = GTUtilsAnnotationsTreeView::findItem("qual1", annotationCItem);
    GTTreeWidget::click(qualifierCItem);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    qualifierCItem = GTUtilsAnnotationsTreeView::findItem("qual1", annotationCItem, {false}, false);
    CHECK_SET_ERR(qualifierCItem == nullptr, "There is annotation qual1, expected state there is no annotation qual1");

    auto annotationBItem = GTUtilsAnnotationsTreeView::expandItem("B");
    auto qualifierBItem = GTUtilsAnnotationsTreeView::findItem("qual", annotationBItem);
    GTTreeWidget::click(qualifierBItem);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    qualifierBItem = GTUtilsAnnotationsTreeView::findItem("qual", annotationBItem, {false}, false);
    CHECK_SET_ERR(qualifierBItem == nullptr, "There is annotation qual1, expected state there is no annotation qual");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual1", "val1"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual1", "val1"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Open "_common_data/scenarios/annotations_qualifiers/test_6_murine.gb".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/annotations_qualifiers/test_6_murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click the "db_xref" qualifier value in any "CDS" annotation.
    GTUtilsAnnotationsTreeView::expandItem("CDS");
    GTTreeWidget::click(GTUtilsAnnotationsTreeView::findItem("db_xref"), AnnotationsTreeView::COLUMN_VALUE);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: a P03334 is loaded and opened.
    QWidget* activeWindow = GTUtilsMdi::activeWindow();
    QString expectedTitle = "GAG_MSVMO [P03334.txt]";
    CHECK_SET_ERR(expectedTitle == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'").arg(expectedTitle).arg(activeWindow->windowTitle()));

    // Open "test_6_murine.gb" view and click the same qualifier value again.
    GTUtilsProjectTreeView::doubleClickItem("test_6_murine.gb");

    activeWindow = GTUtilsMdi::activeWindow();
    QString expectedTitle1 = "NC_001363 [test_6_murine.gb]";
    CHECK_SET_ERR(expectedTitle1 == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'").arg(expectedTitle1).arg(activeWindow->windowTitle()));

    GTTreeWidget::click(GTUtilsAnnotationsTreeView::findItem("db_xref"), AnnotationsTreeView::COLUMN_VALUE);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: nothing happens, the original view is still active.
    activeWindow = GTUtilsMdi::activeWindow();
    CHECK_SET_ERR(nullptr != activeWindow, "Active window is NULL");
    CHECK_SET_ERR(expectedTitle1 == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'").arg(expectedTitle1).arg(activeWindow->windowTitle()));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("new_qualifier", "qwe"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("new_qualifier");
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("new_qualifier", "qwe"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("new_qualifier");
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    GTFile::copy(testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("qual", "val"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("qual");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("new_qualifier", "qwe"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("new_qualifier");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("new_qualifier_1", "qwe_1"));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem("new_qualifier_1");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFile::copy(testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "1anot_1seq.gen");
    GTFileDialog::openFile(sandBoxDir + "1anot_1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    QString longQualifierValueNoSpaces =
        QString("Most qualifier values will be a descriptive text phrase which must be enclosed ") +
        QString("in double quotation marks. When the text occupies more than one line, a single ") +
        QString("set of quotation marks is required at the beginning and at the end of the ") +
        QString("text");

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("long", longQualifierValueNoSpaces));

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("CDS"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsAnnotationsTreeView::findItem("long");

    GTUtilsDocument::saveDocument("1anot_1seq.gen");
    GTUtilsDocument::unloadDocument("1anot_1seq.gen");
    GTUtilsDocument::loadDocument("1anot_1seq.gen");

    GTUtilsAnnotationsTreeView::expandItem("CDS");
    QTreeWidgetItem* qualifierTreeItem = GTUtilsAnnotationsTreeView::findItem("long");
    CHECK_SET_ERR(qualifierTreeItem->text(AnnotationsTreeView::COLUMN_VALUE) == longQualifierValueNoSpaces, "Different qualifier value!");

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_REMOVE, "Selected annotations and qualifiers"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("long"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFile::copy(testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "1anot_1seq.gen");
    GTFileDialog::openFile(sandBoxDir + "1anot_1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    QString longQualifierValueNoSpaces = "Mostqualifiervalueswillbeadescriptivetextphrasewhichmustbeenclosedindoublequotationmarks.";

    GTUtilsDialog::waitForDialog(new EditQualifierFiller("noSpaces", longQualifierValueNoSpaces));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_ADD, "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("CDS"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsAnnotationsTreeView::findItem("noSpaces");

    GTUtilsDocument::saveDocument("1anot_1seq.gen");
    GTUtilsDocument::unloadDocument("1anot_1seq.gen");
    GTUtilsDocument::loadDocument("1anot_1seq.gen");

    GTUtilsAnnotationsTreeView::expandItem("CDS");
    QTreeWidgetItem* qualifierTreeItem = GTUtilsAnnotationsTreeView::findItem("noSpaces");
    CHECK_SET_ERR(qualifierTreeItem->text(AnnotationsTreeView::COLUMN_VALUE) == longQualifierValueNoSpaces, "Different qualifier value!");

    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_REMOVE, "Selected annotations and qualifiers"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("noSpaces"));
    GTMouseDriver::click(Qt::RightButton);
}

}  // namespace GUITest_common_scenarios_annotations_qualifiers
}  // namespace U2
