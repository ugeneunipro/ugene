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
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");
    GTThread::waitForMainThread();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem(os, "qual");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    Runnable *filler1 = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem(os, "qual");

    GTUtilsAnnotationsTreeView::findItem(os, "qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    Runnable *filler1 = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsAnnotationsTreeView::findItem(os, "qual");

    GTUtilsAnnotationsTreeView::findItem(os, "qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::click();

    Runnable *filler = new EditQualifierFiller(os, "qu");
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::click();

    Runnable *filler = new EditQualifierFiller(os, "qu");
    GTUtilsDialog::waitForDialog(os, filler);
    GTKeyboardDriver::keyClick(Qt::Key_F2);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(os, new EditQualifierFiller(os, "qu"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsDialog::checkNoActiveWaiters(os);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(os, new EditQualifierFiller(os, "qu1"));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTUtilsDialog::checkNoActiveWaiters(os);

    GTUtilsAnnotationsTreeView::findItem(os, "qu");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qu", "va");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::doubleClick();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qu", "va");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::doubleClick();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qu", "va");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTMouseDriver::doubleClick();

    Runnable *filler1 = new EditQualifierFiller(os, "qu1", "va1");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTMouseDriver::doubleClick();

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu") != nullptr, "Item qu not found in tree widget");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qu1") != nullptr, "Item qu1 not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 sequence"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTKeyboardDriver::keyPress(Qt::Key_Delete);

    QTreeWidgetItem *qual1 = GTUtilsAnnotationsTreeView::findItem(os, "qual1", {false});
    CHECK_SET_ERR(qual1 == nullptr, "There is annotation qual1, expected state there is no annotation qual1");
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 sequence"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTKeyboardDriver::keyPress(Qt::Key_Delete);

    QTreeWidgetItem *qual = GTUtilsAnnotationsTreeView::findItem(os, "qual", {false});
    CHECK_SET_ERR(qual == nullptr, "There is annotation qual1, expected state there is no annotation qual");
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsAnnotationsTreeView::getItemCenter(os, "C");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual1"));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    QTreeWidgetItem *qual1 = GTUtilsAnnotationsTreeView::findItem(os, "qual1", {false});
    CHECK_SET_ERR(qual1 == nullptr, "There is annotation qual1, expected state there is no annotation qual1");
    GTUtilsAnnotationsTreeView::getItemCenter(os, "B");
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "qual"));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    QTreeWidgetItem *qual = GTUtilsAnnotationsTreeView::findItem(os, "qual", {false});
    CHECK_SET_ERR(qual == nullptr, "There is annotation qual1, expected state there is no annotation qual");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != nullptr, "Item qu not found in tree widget");
}
GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual1") != nullptr, "Item qu not found in tree widget");
}
GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != nullptr, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "qual1", "val1");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual1") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Open "_common_data/scenarios/annotations_qualifiers/test_6_murine.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/annotations_qualifiers/test_6_murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click the "db_xref" qualifier value in any "CDS" annotation.
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));

    GTTreeWidget::click(os, GTUtilsAnnotationsTreeView::findItem(os, "db_xref"), AnnotationsTreeView::COLUMN_VALUE);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: a P03334 is loaded and opened.
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(nullptr != activeWindow, "Active window is NULL");
    QString expectedTitle = "GAG_MSVMO [P03334.txt]";
    CHECK_SET_ERR(expectedTitle == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'").arg(expectedTitle).arg(activeWindow->windowTitle()));

    // Open "test_6_murine.gb" view and click the same qualifier value again.
    GTUtilsProjectTreeView::doubleClickItem(os, "test_6_murine.gb");

    activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(nullptr != activeWindow, "Active window is NULL");
    QString expectedTitle1 = "NC_001363 [test_6_murine.gb]";
    CHECK_SET_ERR(expectedTitle1 == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'").arg(expectedTitle1).arg(activeWindow->windowTitle()));

    GTTreeWidget::click(os, GTUtilsAnnotationsTreeView::findItem(os, "db_xref"), AnnotationsTreeView::COLUMN_VALUE);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: nothing happens, the original view is still active.
    activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(nullptr != activeWindow, "Active window is NULL");
    CHECK_SET_ERR(expectedTitle1 == activeWindow->windowTitle(), QString("An unexpected window is active: expect '%1', got '%2'").arg(expectedTitle1).arg(activeWindow->windowTitle()));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != nullptr, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "new_qualifier", "qwe");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != nullptr, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "new_qualifier", "qwe");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    GTFile::copy(os, testDir + "_common_data/scenarios/project/proj2.uprj", sandBoxDir + "proj2.uprj");
    GTFile::copy(os, testDir + "_common_data/scenarios/project/1.gb", sandBoxDir + "1.gb");

    GTFileDialog::openFile(os, sandBoxDir, "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    Runnable *filler = new EditQualifierFiller(os, "qual", "val");
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "qual") != nullptr, "Item qu not found in tree widget");

    Runnable *filler1 = new EditQualifierFiller(os, "new_qualifier", "qwe");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier") != nullptr, "Item qu not found in tree widget");

    Runnable *filler2 = new EditQualifierFiller(os, "new_qualifier_1", "qwe_1");
    GTUtilsDialog::waitForDialog(os, filler2);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "new_qualifier_1") != nullptr, "Item qu not found in tree widget");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "1anot_1seq.gen");
    GTFileDialog::openFile(os, sandBoxDir + "1anot_1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    QString longQualifierValueNoSpaces =
        QString("Most qualifier values will be a descriptive text phrase which must be enclosed ") +
        QString("in double quotation marks. When the text occupies more than one line, a single ") +
        QString("set of quotation marks is required at the beginning and at the end of the ") +
        QString("text");

    Runnable *filler = new EditQualifierFiller(os, "long", longQualifierValueNoSpaces);
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::checkNoActiveWaiters(os);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "long") != nullptr, "Item long not found in tree widget1");

    GTUtilsDocument::saveDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::unloadDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::loadDocument(os, "1anot_1seq.gen");

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click();

    QTreeWidgetItem *qualifierTreeItem = GTUtilsAnnotationsTreeView::findItem(os, "long");
    CHECK_SET_ERR(qualifierTreeItem->text(AnnotationsTreeView::COLUMN_VALUE) == longQualifierValueNoSpaces, "Different qualifier value!");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_REMOVE << "Selected annotations and qualifiers"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "long"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFile::copy(os, testDir + "_common_data/genbank/1anot_1seq.gen", sandBoxDir + "1anot_1seq.gen");
    GTFileDialog::openFile(os, sandBoxDir + "1anot_1seq.gen");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();

    QString longQualifierValueNoSpaces = "Mostqualifiervalueswillbeadescriptivetextphrasewhichmustbeenclosedindoublequotationmarks.";

    Runnable *filler = new EditQualifierFiller(os, "noSpaces", longQualifierValueNoSpaces);
    GTUtilsDialog::waitForDialog(os, filler);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_ADD << "add_qualifier_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::checkNoActiveWaiters(os);
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "noSpaces") != nullptr, "Item long not found in tree widget1");

    GTUtilsDocument::saveDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::unloadDocument(os, "1anot_1seq.gen");
    GTUtilsDocument::loadDocument(os, "1anot_1seq.gen");

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "CDS"));
    GTMouseDriver::click();

    QTreeWidgetItem *qualifierTreeItem = GTUtilsAnnotationsTreeView::findItem(os, "noSpaces");
    CHECK_SET_ERR(qualifierTreeItem->text(AnnotationsTreeView::COLUMN_VALUE) == longQualifierValueNoSpaces, "Different qualifier value!");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_REMOVE << "Selected annotations and qualifiers"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "noSpaces"));
    GTMouseDriver::click(Qt::RightButton);
}

}  // namespace GUITest_common_scenarios_annotations_qualifiers
}  // namespace U2
