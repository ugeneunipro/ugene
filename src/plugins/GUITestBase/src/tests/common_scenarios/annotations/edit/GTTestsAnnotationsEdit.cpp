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

#include "GTTestsAnnotationsEdit.h"
#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTKeyboardUtils.h"
#include <primitives/GTWidget.h>
#include <base_dialogs/GTFileDialog.h>
#include "primitives/GTMenu.h"
#include <primitives/GTTreeWidget.h>
#include "utils/GTUtilsApp.h"
#include "GTUtilsDocument.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsSequenceView.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditGroupAnnotationsDialogFiller.h"
#include "GTUtilsTaskTreeView.h"

#include <U2View/ADVConstants.h>

namespace U2 {

namespace GUITest_common_scenarios_annotations_edit {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    //     2) UGENE window titled with text "UGENE"
    QString expectedTitle;
#ifdef Q_OS_MAC
    expectedTitle = "proj2 UGENE";
#else
    expectedTitle = "-* UGENE";
#endif
    GTUtilsApp::checkUGENETitle(os, expectedTitle);

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to BB.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "B_group  (0, 2)");

    Runnable *filler = new EditGroupAnnotationsFiller(os, "BB");
    GTUtilsDialog::waitForDialog(os, filler);

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "BB  (0, 2)") != NULL, "Item BB not found in tree widget");


}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation B in annotation tree. Click F2. Change name to BB.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "B_group  (0, 2)");

    Runnable *filler = new EditGroupAnnotationsFiller(os, "BB");
    GTUtilsDialog::waitForDialog(os, filler);

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "BB  (0, 2)") != NULL, "Item BB not found in tree widget");

    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "BB  (0, 2)");

    Runnable *filler1 = new EditGroupAnnotationsFiller(os, "B_group");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "B_group  (0, 2)") != NULL, "Item B_group not found in tree widget");

}


GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to BB.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "B_group  (0, 2)");

    Runnable *filler = new EditGroupAnnotationsFiller(os, "BB");
    GTUtilsDialog::waitForDialog(os, filler);

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "BB  (0, 2)") != NULL, "Item BB not found in tree widget");


    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "C_group  (0, 1)");

    Runnable *filler1 = new EditGroupAnnotationsFiller(os, "CC");
    GTUtilsDialog::waitForDialog(os, filler1);

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "CC  (0, 1)") != NULL, "Item BB not found in tree widget");

}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "CC", "80 ..90");
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "CC") != NULL, "Item CC not found in tree widget");

}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "CC", "80 ..90");
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "CC") != NULL, "Item CC not found in tree widget");

    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "CC");
    Runnable *filler1 = new EditAnnotationFiller(os, "C", "80 ..90");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "C") != NULL, "Item C not found in tree widget");
}


GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "CC", "80 ..90");
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "CC") != NULL, "Item CC not found in tree widget");

    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "B");
    Runnable *filler1 = new EditAnnotationFiller(os, "BB", "30 ..120");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "BB") != NULL, "Item BB not found in tree widget");
}


GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "C", "20 ..40");
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    bool found = GTUtilsAnnotationsTreeView::findRegion(os, "C", U2Region(20, 40));
    CHECK_SET_ERR(found == true, "There is no {20 ..40} region in annotation");

}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "C", "20 ..40");
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    bool found = GTUtilsAnnotationsTreeView::findRegion(os, "C", U2Region(20, 40));
    CHECK_SET_ERR(found == true, "There is no {20 ..40} region in annotation");
    // 4. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler1 = new EditAnnotationFiller(os, "C", "10 ..90");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    bool found1 = GTUtilsAnnotationsTreeView::findRegion(os, "C", U2Region(10, 90));
    CHECK_SET_ERR(found1 == true, "There is no {10 ..90} region in annotation");
}

GUI_TEST_CLASS_DEFINITION(test_0003_2) {
    // Rename annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "CC", "20 ..40");
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    bool found = GTUtilsAnnotationsTreeView::findRegion(os, "CC", U2Region(20, 40));
    CHECK_SET_ERR(found == true, "There is no {20 ..40} region in annotation");

    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "B");
    Runnable *filler1 = new EditAnnotationFiller(os, "BB", "20 ..40");
    GTUtilsDialog::waitForDialog(os, filler1);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    GTGlobals::sleep();
    bool found1 = GTUtilsAnnotationsTreeView::findRegion(os, "BB", U2Region(20, 40));
    CHECK_SET_ERR(found1 == true, "There is no {20 ..40} region in annotation");

}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //Rename annotation

    // Steps:
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "C", "20 ..40", true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    Runnable *checker = new EditAnnotationChecker(os, "", "complement(20..40)");
    GTUtilsDialog::waitForDialog(os, checker);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();
}
GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    //Rename annotation

    // Steps:
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    Runnable *filler = new EditAnnotationFiller(os, "CC", "20 ..40", true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    Runnable *checker = new EditAnnotationChecker(os, "CC", "complement(20..40)");
    GTUtilsDialog::waitForDialog(os, checker);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();
}
GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    //Rename annotation

    // Steps:
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation C in annotation tree. Click F2. Change name to CC.

    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "B");
    Runnable *filler = new EditAnnotationFiller(os, "B", "20 ..40", true);
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();

    Runnable *checker = new EditAnnotationChecker(os, "", "complement(20..40)");
    GTUtilsDialog::waitForDialog(os, checker);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_F2);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //Rename annotation
    // Steps:

    // 1. Open data/samples/FASTA/human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Press <Ctrl>+N
    //2.1 CHECK if dialog box titled with "Create annotation" appeared
    //2.2 CHECK if "Create new table" radio button is checked
    //3. Specify the annotation with the next data and the press "Create":
    //Group name: group
    //Annotation name: misc_feature
    //Location: 1..1000
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "group", "misc_feature", "1..1000"));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep();
    //4. CHECK if new gb-format document is loaded into the annotation editor
    //4.1 CHECK if it contains group "group" with annotation "misc_feature" in it
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "group  (0, 1)");
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature");

    //5. Select misc_feature annotation and press <DEL>
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();
    //6. CHECK if misc_feature annotation is removed
    GTUtilsAnnotationsTreeView::findItem(os, "group  (0, 0)");
    GTUtilsAnnotationsTreeView::findItem(os, "misc_feature", GTGlobals::FindOptions(false));
    GTGlobals::sleep();

}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    //Rename annotation
    // Steps:

    // 1. Open data/samples/FASTA/human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Press <Ctrl>+N
    //2.1 CHECK if dialog box titled with "Create annotation" appeared
    //2.2 CHECK if "Create new table" radio button is checked
    //3. Specify the annotation with the next data and the press "Create":
    //Group name: group
    //Annotation name: misc_feature
    //Location: 1..1000
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group", "misc_feature", "1..1000"));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep();

    //4. CHECK if new gb-format document is loaded into the annotation editor
    //4.1 CHECK if it contains group "group" with annotation "misc_feature" in it
    //TODO: 4.2 CHECK if "group" subgroups/annotations counter displays (0,1)
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "group  (0, 1)");
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature");

    //5. Select misc_feature annotation and press <DEL>
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();
    //6. CHECK if misc_feature annotation is removed
    //TODO: 6.1 CHECK if "group" subgroups/annotations counter displays (0,0)
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QTreeWidgetItem *annotationItem = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature", options);
    CHECK_SET_ERR(NULL == annotationItem, "The annotation 'misc_feature' unexpectedly was not removed");
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group_new", "misc_feature_1", "1..500"));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep();

    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "group_new  (0, 1)");
    GTUtilsAnnotationsTreeView::findItem(os, "group  (0, 0)");
    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature_1");

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);

    GTGlobals::sleep();
    annotationItem = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature_1", options);
    CHECK_SET_ERR(NULL == annotationItem, "The annotation 'misc_feature_1' unexpectedly was not removed");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {

    //Rename annotation
    // Steps:

    // 1. Open data/samples/FASTA/human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Press <Ctrl>+N
    //2.1 CHECK if dialog box titled with "Create annotation" appeared
    //2.2 CHECK if "Create new table" radio button is checked
    //3. Specify the annotation with the next data and the press "Create":
    //Group name: group
    //Annotation name: misc_feature
    //Location: 1..1000
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group", "misc_feature", "1..1000"));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "group_new", "misc_feature_1", "1..500"));
    GTKeyboardDriver::keyClick( 'n', Qt::ControlModifier);
    GTGlobals::sleep();

    //4. CHECK if new gb-format document is loaded into the annotation editor
    //4.1 CHECK if it contains group "group" with annotation "misc_feature" in it
    //TODO: 4.2 CHECK if "group" subgroups/annotations counter displays (0,1)
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "group  (0, 1)");
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature");

    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "group_new  (0, 1)");
    QTreeWidgetItem *item1 = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature_1");

    //5. Select misc_feature annotation and press <DEL>
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item1));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();

    //6. CHECK if misc_feature annotation is removed
    //TODO: 6.1 CHECK if "group" subgroups/annotations counter displays (0,0)
    GTUtilsAnnotationsTreeView::findItem(os, "misc_feature", GTGlobals::FindOptions(false));
    GTUtilsAnnotationsTreeView::findItem(os, "misc_feature_1", GTGlobals::FindOptions(false));
    GTGlobals::sleep();

}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //Check rename annotation action at popup menu (UGENE-3449)
    // Rename annotation
    // Steps:
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    
    // Click "Hide zoom view"
    QWidget* toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Check that menu item { Edit -> Annotation } is absent at popup menu of sequence view.
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item",
                                                      PopupChecker::NotExists, GTGlobals::UseMouse));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));

    // 4. Check that menu item { Edit -> Annotation } is absent at popup menu of annotations view.
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item",
                                                      PopupChecker::NotExists, GTGlobals::UseMouse));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "annotations_tree_widget"));

}
GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    //Check rename annotation action at popup menu (UGENE-3449)
    // Rename annotation
    // Steps:
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/1.gb
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    
    // Click "Hide zoom view"
    QWidget* toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1.gb"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Select annotation B in annotation tree.
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "B_group  (0, 2)");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    item = GTUtilsAnnotationsTreeView::findItem(os, "B");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();
    GTGlobals::sleep(100);

    // 4. Check that menu item { Edit -> Annotation } is enabled at popup menu of sequence view.
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item",
                                                      PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTGlobals::sleep();
    // 5. Check that menu item { Edit -> Annotation } is enabled at popup menu of annotations view.
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item",
                                                      PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, "B");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0006_2) {
    //Check rename annotation action at popup menu (UGENE-3449)
    // Rename annotation
    // Steps:
    // 1. Open data/samples/Genbank/mirine.gb
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
   
    // Click "Hide zoom view"
    QWidget* toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 2. Select a group on annotations editor
    QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem(os, "CDS  (0, 4)");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click();

    // 3. Open context menu on sequence area
    // Expected state: { Edit -> Annotation } action is disabled
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item",
                                                      PopupChecker::NotExists, GTGlobals::UseMouse));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));

    // 3. Open context menu on sequence area
    // Expected state: { Edit -> Annotation } action is enabled
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << ADV_MENU_EDIT << "edit_annotation_tree_item",
                                                      PopupChecker::IsEnabled, GTGlobals::UseMouse));
    //GTMenu::showContextMenu(os, GTWidget::findWidget(os, "annotations_tree_widget"));
    GTUtilsAnnotationsTreeView::callContextMenuOnItem(os, "CDS");
}

} // namespace GUITest_common_scenarios_annotations_edit
} // namespace U2
