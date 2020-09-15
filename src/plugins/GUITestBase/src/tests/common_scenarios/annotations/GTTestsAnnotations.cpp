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

#include "GTTestsAnnotations.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QTreeWidget>

#include <U2View/ADVConstants.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_annotations {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by threee different ways
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "ann1");
    GTUtilsAnnotationsTreeView::findItem(os, "ann2");
    GTUtilsAnnotationsTreeView::findItem(os, "ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by threee different ways
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "ann1");
    GTUtilsAnnotationsTreeView::findItem(os, "ann2");
    GTUtilsAnnotationsTreeView::findItem(os, "ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by threee different ways
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "ann1");
    GTUtilsAnnotationsTreeView::findItem(os, "ann2");
    GTUtilsAnnotationsTreeView::findItem(os, "ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();
    //
    // 3. Do menu {Actions->Add->New Annotation}
    // Expected state: "Create annotation" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Group Name} DDD
    //     {Annotation Name} D
    //     {Location} join(10..16,18..20)
    //
    // 4. Click Create button
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // Expected state: annotation with 2 segments has been created
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "D");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();
    //
    // 3. Do menu {Actions->Add->New Annotation}
    // Expected state: "Create annotation" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Group Name} DDD
    //     {Annotation Name} D
    //     {Location} join(10..16,18..20)
    //
    // 4. Click Create button
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // Expected state: annotation with 2 segments has been created
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "D");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();
    //
    // 3. Do menu {Actions->Add->New Annotation}
    // Expected state: "Create annotation" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Group Name} DDD
    //     {Annotation Name} D
    //     {Location} join(10..16,18..20)
    //
    // 4. Click Create button
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // Expected state: annotation with 2 segments has been created
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "D");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by threee different ways
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "ann1");
    GTUtilsAnnotationsTreeView::findItem(os, "ann2");
    GTUtilsAnnotationsTreeView::findItem(os, "ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggle_HL_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggle_HL_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggle_HL_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    // Expected state: there is three new annotations on sequence created by threee different ways
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "ann1");
    GTUtilsAnnotationsTreeView::findItem(os, "ann2");
    GTUtilsAnnotationsTreeView::findItem(os, "ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();
    //
    // 3. Do menu {Actions->Add->New Annotation}
    // Expected state: "Create annotation" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Group Name} DDD
    //     {Annotation Name} D
    //     {Location} join(10..16,18..20)
    //
    // 4. Click Create button
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // Expected state: annotation with 2 segments has been created
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "D");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "toggle_HL_action"));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();
    //
    // 3. Do menu {Actions->Add->New Annotation}
    // Expected state: "Create annotation" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Group Name} DDD
    //     {Annotation Name} D
    //     {Location} join(10..16,18..20)
    //
    // 4. Click Create button
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // Expected state: annotation with 2 segments has been created
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "D");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // Click "Hide zoom view"
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_NC_001363 sequence");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_NC_001363 sequence");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    // Expected state: there is three new annotations on sequence created by threee different ways
    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::findItem(os, "ann1");
    GTUtilsAnnotationsTreeView::findItem(os, "ann2");
    GTUtilsAnnotationsTreeView::findItem(os, "ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0010_1) {
    //    Export annotations from different annotation table objects
    //    The following scenario should work with BED, GFF and GTF formats.
    //    1. Open '_common_data/fasta/DNA.fa' as separate sequences
    //    2. Select a few annotations from different annotation table objects
    //    3. {Export -> Export annotations...}, select on of the following formats: BED, GFF, GTF
    //      Expected state: 'Export annotations' dialog appeared
    //    4. Open exported file
    //      Expected state: there are two annotation table objects

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(os, testDir, "_common_data/fasta/DNA.fa");
    GTUtilsDialog::waitAllFinished(os);

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141619"), "No GXL_141619 object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141618"), "No GXL_141618 object!");

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann_1", "200..300", sandBoxDir + "ann_test_0010_1_19.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_GXL_141619"), Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann_2", "100..200", sandBoxDir + "ann_test_0010_1_18.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_GXL_141618"), Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "ann_1"
                                                              << "ann_2");

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0010_1.bed", ExportAnnotationsFiller::bed, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    GTUtilsDocument::removeDocument(os, "DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "ann_test_0010_1_18.gb");
    GTUtilsDialog::waitAllFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "ann_test_0010_1_19.gb");
    GTUtilsDialog::waitAllFinished(os);

    GTFileDialog::openFile(os, sandBoxDir, "ann_export_test_0010_1.bed");

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141619 features"), "No GXL_141619 features object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141618 features"), "No GXL_141618 features object!");
}

GUI_TEST_CLASS_DEFINITION(test_0010_2) {
    //    GFF

    //    Export annotations from different annotation table objects
    //    The following scenario should work with BED, GFF and GTF formats.
    //    1. Open '_common_data/fasta/DNA.fa' as separate sequences
    //    2. Select a few annotations from different annotation table objects
    //    3. {Export -> Export annotations...}, select on of the following formats: BED, GFF, GTF
    //    Expected state: 'Export annotations' dialog appeared
    //    4. Open exported file
    //    Expected state: there are two annotation table objects

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(os, testDir, "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141619"), "No GXL_141619 object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141618"), "No GXL_141618 object!");

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann_1", "200..300", sandBoxDir + "ann_test_0010_2_19.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_GXL_141619"), Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann_2", "100..200", sandBoxDir + "ann_test_0010_2_18.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_GXL_141618"), Qt::RightButton);

    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "ann_1"
                                                              << "ann_2");

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0010_2.gff", ExportAnnotationsFiller::gff, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDocument::removeDocument(os, "DNA.fa");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "ann_test_0010_2_18.gb");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "ann_test_0010_2_19.gb");
    GTGlobals::sleep();

    GTFileDialog::openFile(os, sandBoxDir, "ann_export_test_0010_2.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141619 features"), "No GXL_141619 features object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141618 features"), "No GXL_141618 features object!");
}

GUI_TEST_CLASS_DEFINITION(test_0010_3) {
    //    GTF

    //    Export annotations from different annotation table objects
    //    The following scenario should work with BED, GFF and GTF formats.
    //    1. Open '_common_data/fasta/DNA.fa' as separate sequences
    //    2. Select a few annotations from different annotation table objects
    //    3. {Export -> Export annotations...}, select on of the following formats: BED, GFF, GTF
    //    Expected state: 'Export annotations' dialog appeared
    //    4. Open exported file
    //    Expected state: there are two annotation table objects

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(os, testDir, "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141619"), "No GXL_141619 object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141618"), "No GXL_141618 object!");

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann_1", "200..300", sandBoxDir + "ann_test_0010_3_19.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_GXL_141619"), Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann_2", "100..200", sandBoxDir + "ann_test_0010_3_18.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "det_view_GXL_141618"), Qt::RightButton);

    QStringList annList;
    annList << "ann_1"
            << "ann_2";
    GTUtilsAnnotationsTreeView::selectItems(os, annList);
    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "YT483", "ann_1");
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "001T", "ann_1");
    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "YT496", "ann_2");
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "0012", "ann_2");

    GTGlobals::sleep();
    GTUtilsAnnotationsTreeView::selectItems(os, annList);

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, sandBoxDir + "ann_export_test_0010_3.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDocument::removeDocument(os, "DNA.fa");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "ann_test_0010_3_18.gb");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "ann_test_0010_3_19.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "GTF"));
    GTFileDialog::openFile(os, sandBoxDir, "ann_export_test_0010_3.gtf");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141619 features"), "No GXL_141619 features object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "GXL_141618 features"), "No GXL_141618 features object!");
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // gene_id and transcript_id attributes are required for GTF format!

    //    1. Open "human_T1.fa"
    //    2. Create an annotation
    //    3. Try to export it to GTF format
    //    Expected state: info message - "GTF format is not strict - some annotations do not have "gene_id" and/or "transcript_id" qualifiers"

    GTLogTracer l("GTF format is not strict - some annotations do not have \"gene_id\" and/or \"transcript_id\" qualifiers");

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_human_T1(UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "exon", "annotation", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);
    GTWidget::click(os, GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "TR321", "annotation");

    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "annotation");

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new DocumentFormatSelectorDialogFiller(os, "GTF"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    CHECK_SET_ERR(l.isExpectedMessageFound, "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    // gene_id and transcript_id attributes are required for GTF format!

    //    1. Open "human_T1.fa"
    //    2. Create an annotation
    //    3. Try to export it to GTF format
    //    Expected state: info message - "GTF format is not strict - some annotations do not have "gene_id" and/or "transcript_id" qualifiers"

    GTLogTracer l("GTF format is not strict - some annotations do not have \"gene_id\" and/or \"transcript_id\" qualifiers");

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_human_T1(UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);
    GTWidget::click(os, GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "XCV", "ann");

    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "ann");

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new DocumentFormatSelectorDialogFiller(os, "GTF"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    CHECK_SET_ERR(l.isExpectedMessageFound, "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_0011_3) {
    // gene_id and transcript_id attributes are required for GTF format!

    //    1. Open "human_T1.fa"
    //    2. Create an annotation
    //    4. Add 'gene_id' and 'transcript_id' quilifiers
    //    3. Export it to GTF format
    //    Expected state: no errors in the log

    GTLogTracer l;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Click "Hide zoom view
    QWidget *toolbar = GTWidget::findWidget(os, "views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    CHECK_SET_ERR(toolbar != nullptr, "Cannot find views_tool_bar_human_T1(UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(os, GTWidget::findWidget(os, "show_hide_zoom_view", toolbar));
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "ann", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD"
                                                                        << "create_annotation_action"));
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"), Qt::RightButton);
    GTWidget::click(os, GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "XCV", "ann");
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "TR321", "ann");

    GTUtilsAnnotationsTreeView::selectItems(os, QStringList() << "ann");

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_0012_1) {
    // BED

    //    1. Open sequence
    //    2. Open annotation file
    //    3. Create a relation between the first annotation object and sequence
    //    4. Open another annotation file
    //    5. Create a relation between the second annotation object and sequence
    //    Expected state: there are 2 ann.objects in annotation tree view
    //    6. Select a few annotation from different annotation table objects
    //    7. Export them  to BED, GTF ot GFF format
    //    8. Open the result file
    //    Expected state: annotation table object name contain sequence name

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 features"), "Object not found");

    GTFileDialog::openFile(os, testDir + "_common_data/gff/", "scaffold_90.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    GTUtilsProjectTreeView::checkItem(os, "scaffold_90 features");

    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os, "scaffold_90 features");
    QWidget *sequence = GTUtilsSequenceView::getPanOrDetView(os);
    CHECK_SET_ERR(sequence != nullptr, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idx, sequence);

    QStringList annList;
    annList << "5'UTR"
            << "exon";

    GTUtilsAnnotationsTreeView::selectItems(os, annList);

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0012_1.bed", ExportAnnotationsFiller::bed, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDocument::removeDocument(os, "sars.gb");
    GTUtilsDocument::removeDocument(os, "scaffold_90.gff");
    GTGlobals::sleep();

    GTFileDialog::openFile(os, sandBoxDir, "ann_export_test_0012_1.bed");
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 features"), "Object not found");
    GTUtilsProjectTreeView::checkNoItem(os, "scaffold_90 features");
}

GUI_TEST_CLASS_DEFINITION(test_0012_2) {
    // GFF

    //    1. Open sequence
    //    2. Open annotation file
    //    3. Create a relation between the first annotation object and sequence
    //    4. Open another annotation file
    //    5. Create a relation between the second annotation object and sequence
    //    Expected state: there are 2 ann.objects in annotation tree view
    //    6. Select a few annotation from different annotation table objects
    //    7. Export them  to BED, GTF ot GFF format
    //    8. Open the result file
    //    Expected state: annotation table object name contain sequence name

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 features"), "Object not found");

    GTFileDialog::openFile(os, testDir + "_common_data/gff/", "scaffold_90.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    GTUtilsProjectTreeView::checkItem(os, "scaffold_90 features");

    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os, "scaffold_90 features");
    QWidget *sequence = GTUtilsSequenceView::getPanOrDetView(os);
    CHECK_SET_ERR(sequence != NULL, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idx, sequence);

    QStringList annList;
    annList << "5'UTR"
            << "exon";

    GTUtilsAnnotationsTreeView::selectItems(os, annList);

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0012_2.gff", ExportAnnotationsFiller::gff, os));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDocument::removeDocument(os, "sars.gb");
    GTUtilsDocument::removeDocument(os, "scaffold_90.gff");
    GTGlobals::sleep();

    GTFileDialog::openFile(os, sandBoxDir, "ann_export_test_0012_2.gff");
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 features"), "Object not found");

    GTUtilsProjectTreeView::checkNoItem(os, "scaffold_90 features");
}

GUI_TEST_CLASS_DEFINITION(test_0012_3) {
    // GTF - gtf annotations MUST have gene_id and transcript_id attributes

    //    1. Open sequence
    //    2. Open annotation file
    //    3. Create a relation between the first annotation object and sequence
    //    4. Open another annotation file
    //    5. Create a relation between the second annotation object and sequence
    //    Expected state: there are 2 ann.objects in annotation tree view
    //    6. Select a few annotation from different annotation table objects
    //    7. Export them  to BED, GTF ot GFF format
    //    8. Open the result file
    //    Expected state: annotation table object name contain sequence name

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 features"), "Object not found");

    GTFileDialog::openFile(os, testDir + "_common_data/gff/", "scaffold_90.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    GTUtilsProjectTreeView::checkItem(os, "scaffold_90 features");

    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os, "scaffold_90 features");
    QWidget *sequence = GTUtilsSequenceView::getPanOrDetView(os);
    CHECK_SET_ERR(sequence != NULL, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, idx, sequence);

    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "XCV", "exon");
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "TR321", "exon");
    GTUtilsAnnotationsTreeView::createQualifier(os, "gene_id", "XCV", "5'UTR");
    GTUtilsAnnotationsTreeView::createQualifier(os, "transcript_id", "TR321", "5'UTR");

    QStringList annList;
    annList << "5'UTR"
            << "exon";

    GTUtilsAnnotationsTreeView::selectItems(os, annList);

    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(os, sandBoxDir + "ann_export_test_0012_3.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ADV_MENU_EXPORT << "action_export_annotations"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "sars.gb");
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "No"));
    GTUtilsDocument::removeDocument(os, "scaffold_90.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "GTF"));
    GTFileDialog::openFile(os, sandBoxDir, "ann_export_test_0012_3.gtf");
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(os, "NC_004718 features"), "Object not found");
    GTUtilsProjectTreeView::checkNoItem(os, "scaffold_90 features");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument(os, "1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 features"));
    GTMouseDriver::doubleClick();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 3. Create annotation using menu {Actions->Add->New Annotation}, with description field filled
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, false, "<auto>", "ann1", "complement(1.. 20)", "", "description"));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Add"
                                                << "New annotation...");
    GTGlobals::sleep();

    //4. Check what created annotation has corresponding qualifier 'note'
    QTreeWidget *treeWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    QTreeWidgetItem *annotationsRoot = GTUtilsAnnotationsTreeView::findItem(os, "ann1  (0, 1)", treeWidget);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, annotationsRoot->child(0)));
    GTMouseDriver::doubleClick();
    GTUtilsAnnotationsTreeView::findItem(os, "note");
}

}    // namespace GUITest_common_scenarios_annotations
}    // namespace U2
