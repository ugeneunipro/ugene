/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QHeaderView>
#include <QTreeWidgetItem>

#include <U2View/ADVConstants.h>
#include <U2View/AnnotationsTreeView.h>

#include "GTTestsAnnotationsQualifiers.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "utils/GTUtilsApp.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "system/GTFile.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include "primitives/GTMenu.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_annotations_qualifiers {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem(os, "qual");
    GTGlobals::sleep(1000);

}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    Runnable *filler1 = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem(os, "qual");
    GTGlobals::sleep(1000);

    GTUtilsAnnotationsTreeView::findItem(os, "qual1");
    GTGlobals::sleep(1000);
    }

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    Runnable *filler1 = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTGlobals::sleep(1000);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem(os, "qual");
    GTGlobals::sleep(1000);

    GTUtilsAnnotationsTreeView::findItem(os, "qual1");
    GTGlobals::sleep(1000);
    }


GUI_TEST_CLASS_DEFINITION(test_0002) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::click();

    Runnable *filler = new EditQualifierFiller(os, "qu");
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != NULL, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::click();

    Runnable *filler = new EditQualifierFiller(os, "qu");
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != NULL, "Item qu not found in tree widget");
    }

GUI_TEST_CLASS_DEFINITION(test_0002_2) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::click();

    Runnable *filler = new EditQualifierFiller(os, "qu");
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::click();

    Runnable *filler1 = new EditQualifierFiller(os, "qu1");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != NULL, "Item qu not found in tree widget");

    }

GUI_TEST_CLASS_DEFINITION(test_0003) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qu", "va");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != NULL, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qu", "va");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::doubleClick();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != NULL, "Item qu not found in tree widget");
    }

GUI_TEST_CLASS_DEFINITION(test_0003_2) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qu", "va");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler1 = new EditQualifierFiller(os, "qu1", "va1");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::doubleClick();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != NULL, "Item qu not found in tree widget");
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu1") != NULL, "Item qu1 not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 sequence"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTKeyboardDriver::keyPress(Qt::Key_Delete);
    GTGlobals::sleep();

    QTreeWidgetItem *qual1 = GTUtilsAnnotationsTreeView::findItem(os, "qual1", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(qual1 == NULL, "There is annotation qual1, expected state there is no annotation qual1");

}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 sequence"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTKeyboardDriver::keyPress(Qt::Key_Delete);
    GTGlobals::sleep();

    GTGlobals::sleep();

    QTreeWidgetItem *qual = GTUtilsAnnotationsTreeView::findItem(os, "qual", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(qual == NULL, "There is annotation qual1, expected state there is no annotation qual");

}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();

    GTGlobals::sleep();
    QTreeWidgetItem *qual1 = GTUtilsAnnotationsTreeView::findItem(os, "qual1", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(qual1 == NULL, "There is annotation qual1, expected state there is no annotation qual1");
    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    GTGlobals::sleep();
    QTreeWidgetItem *qual = GTUtilsAnnotationsTreeView::findItem(os, "qual", GTGlobals::FindOptions(false));
    CHECK_SET_ERR(qual == NULL, "There is annotation qual1, expected state there is no annotation qual");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != NULL, "Item qu not found in tree widget");

}
GUI_TEST_CLASS_DEFINITION(test_0005_1) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual1") != NULL, "Item qu not found in tree widget");

}
GUI_TEST_CLASS_DEFINITION(test_0005_2) {

    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != NULL, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual1") != NULL, "Item qu not found in tree widget");

}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Open "_common_data/scenarios/annotations_qualifiers/test_6_murine.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/annotations_qualifiers/test_6_murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click the "db_xref" qualifier value in any "CDS" annotation.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTGlobals::sleep();

    GTTreeWidget::click(os, GTUtilsAnnotationsTreeView::findItem(os, "db_xref"), AnnotationsTreeView::COLUMN_VALUE);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: a P03334 is loaded and opened.
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(NULL != activeWindow, "Active window is NULL");
    QString expectedTitle = "P03334 [s] GAG_MSVMO";
    CHECK_SET_ERR(expectedTitle == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'")
            .arg(expectedTitle).arg(activeWindow->windowTitle()));

    // Open "test_6_murine.gb" view and click the same qualifier value again.
    GTUtilsProjectTreeView::doubleClickItem(os, "test_6_murine.gb");

    activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(NULL != activeWindow, "Active window is NULL");
    expectedTitle = "test_6_murine [s] NC_001363";
    CHECK_SET_ERR(expectedTitle == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'")
            .arg(expectedTitle).arg(activeWindow->windowTitle()));

    GTTreeWidget::click(os, GTUtilsAnnotationsTreeView::findItem(os, "db_xref"), AnnotationsTreeView::COLUMN_VALUE);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: nothing happens, the original view is still active.
    activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(NULL != activeWindow, "Active window is NULL");
    CHECK_SET_ERR(expectedTitle == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'")
            .arg(expectedTitle).arg(activeWindow->windowTitle()));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != NULL, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "new_qualifier", "qwe");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier") != NULL, "Item qu not found in tree widget");

}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != NULL, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "new_qualifier", "qwe");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier") != NULL, "Item qu not found in tree widget");

}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != NULL, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "new_qualifier", "qwe");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier") != NULL, "Item qu not found in tree widget");

    Runnable *filler2 = new EditQualifierFiller(os, "new_qualifier_1", "qwe_1");
    GTUtilsDialog::waitForDialog(os, filler2);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier_1") != NULL, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "1anot_1seq.gen");
    GTFileDialog::openFile(os, sandBoxDir + "1anot_1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    QString longQualifierValueNoSpaces =
            QString("Most qualifier values will be a descriptive text phrase which must be enclosed ") +
            QString("in double quotation marks. When the text occupies more than one line, a single ") +
            QString("set of quotation marks is required at the beginning and at the end of the ") +
            QString("text");

    Runnable *filler = new EditQualifierFiller(os, "long", longQualifierValueNoSpaces);
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTGlobals::sleep(1000);
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialogClosed();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "long") != NULL, "Item long not found in tree widget1");

    GTUtilsDocument::saveDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::unloadDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::loadDocument(os, "1anot_1seq.gen");

    GTGlobals::sleep();

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click();

    QTreeWidgetItem* qualifierTreeItem = GTUtilsAnnotationsTreeView::findItem(os, "long");
    CHECK_SET_ERR(qualifierTreeItem->text(AnnotationsTreeView::COLUMN_VALUE) == longQualifierValueNoSpaces, "Different qualifier value!");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_REMOVE << "Selected annotations and qualifiers"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "long"));
    GTGlobals::sleep(1000);
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "1anot_1seq.gen");
    GTFileDialog::openFile(os, sandBoxDir + "1anot_1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    QString longQualifierValueNoSpaces = "Mostqualifiervalueswillbeadescriptivetextphrasewhichmustbeenclosedindoublequotationmarks.";

    Runnable *filler = new EditQualifierFiller(os, "noSpaces", longQualifierValueNoSpaces);
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::waitForDialogClosed();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "noSpaces") != NULL, "Item long not found in tree widget1");

    GTUtilsDocument::saveDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::unloadDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::loadDocument(os, "1anot_1seq.gen");

    GTGlobals::sleep();

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click();

    QTreeWidgetItem* qualifierTreeItem = GTUtilsAnnotationsTreeView::findItem(os, "noSpaces");
    CHECK_SET_ERR(qualifierTreeItem->text(AnnotationsTreeView::COLUMN_VALUE) == longQualifierValueNoSpaces, "Different qualifier value!");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_REMOVE << "Selected annotations and qualifiers"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "noSpaces"));
    GTMouseDriver::click(Qt::RightButton);
}

} // namespace GUITest_common_scenarios_annotations_qualifiers
} // namespace U2
