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

#include "GTTestsAnnotations.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include <QTreeWidget>

#include <U2View/ADVConstants.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsProject.h"
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
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363 sequence");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by three different ways
    GTUtilsAnnotationsTreeView::findItem("ann1");
    GTUtilsAnnotationsTreeView::findItem("ann2");
    GTUtilsAnnotationsTreeView::findItem("ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363 sequence");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by three different ways
    GTUtilsAnnotationsTreeView::findItem("ann1");
    GTUtilsAnnotationsTreeView::findItem("ann2");
    GTUtilsAnnotationsTreeView::findItem("ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0001_2) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363 sequence");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by three different ways
    GTUtilsAnnotationsTreeView::findItem("ann1");
    GTUtilsAnnotationsTreeView::findItem("ann2");
    GTUtilsAnnotationsTreeView::findItem("ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();
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
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // Expected state: annotation with 2 segments has been created
    GTUtilsAnnotationsTreeView::findItem("D");
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();
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
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // Expected state: annotation with 2 segments has been created
    GTUtilsAnnotationsTreeView::findItem("D");
}

GUI_TEST_CLASS_DEFINITION(test_0002_2) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();
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
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // Expected state: annotation with 2 segments has been created
    GTUtilsAnnotationsTreeView::findItem("D");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    // Expected state: there is three new annotations on sequence created by three different ways
    GTUtilsAnnotationsTreeView::findItem("ann1");
    GTUtilsAnnotationsTreeView::findItem("ann2");
    GTUtilsAnnotationsTreeView::findItem("ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(new PopupChooser({"toggle_HL_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0004_1) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(new PopupChooser({"toggle_HL_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0004_2) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(new PopupChooser({"toggle_HL_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363 sequence");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Expected state: there is three new annotations on sequence created by three different ways
    GTUtilsAnnotationsTreeView::findItem("ann1");
    GTUtilsAnnotationsTreeView::findItem("ann2");
    GTUtilsAnnotationsTreeView::findItem("ann3");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Creating joined annotation
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");
    //
    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();
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
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // Expected state: annotation with 2 segments has been created
    GTUtilsAnnotationsTreeView::findItem("D");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Annotation editor: update annotations incorrect behavior (0001585)
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // 3. Create 2 annotations:
    //     1) a1 in group a1
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a1_group", "a1", "10..16"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    //     2) a1 in group a2
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "a2_group", "a1", "18..20"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Toggle highlight for a1.
    GTUtilsDialog::waitForDialog(new PopupChooser({"toggle_HL_action"}));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("a1"));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: both annotations (a1) and groups (a1, a2) looks muted (grayed out)
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Creating joined annotation
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument("1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "DDD", "D", "join(10..16,18..20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // Expected state: annotation with 2 segments has been created
    GTUtilsAnnotationsTreeView::findItem("D");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // Creating annotations by different ways
    //
    // Steps:
    //
    // 1. Use menu {File->Open}. Open project _common_data/scenarios/project/proj2.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    // Expected state:
    //     1) Project view with document "1.gb" has been opened
    GTUtilsDocument::checkDocument("1.gb");

    // 2. Open view for "1.gb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("NC_001363 features"));
    GTMouseDriver::doubleClick();

    // Click "Hide zoom view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363 sequence");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    // 5. Press right mouse button on sequence area, use context menu item {Add->New Annotation} to create annotation
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann3", "complement(1.. 20)"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    // 3. Create annotation using menu {Actions->Add->New Annotation}
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Create annotation using keyboard shortcut Ctrl+N
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann2", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Expected state: there is three new annotations on sequence created by three different ways
    GTUtilsAnnotationsTreeView::findItem("ann1");
    GTUtilsAnnotationsTreeView::findItem("ann2");
    GTUtilsAnnotationsTreeView::findItem("ann3");
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

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(testDir, "_common_data/fasta/DNA.fa");
    GTUtilsDialog::checkNoActiveWaiters();

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141619"), "No GXL_141619 object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141618"), "No GXL_141618 object!");

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "ann_1", "200..300", sandBoxDir + "ann_test_0010_1_19.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("det_view_GXL_141619"), Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "ann_2", "100..200", sandBoxDir + "ann_test_0010_1_18.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("det_view_GXL_141618"), Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsAnnotationsTreeView::selectItemsByName({"ann_1", "ann_2"});

    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0010_1.bed", ExportAnnotationsFiller::bed));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDocument::removeDocument("DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("ann_test_0010_1_18.gb");
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("ann_test_0010_1_19.gb");
    GTUtilsDialog::checkNoActiveWaiters();

    GTFileDialog::openFile(sandBoxDir, "ann_export_test_0010_1.bed");

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141619 features"), "No GXL_141619 features object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141618 features"), "No GXL_141618 features object!");
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

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(testDir, "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141619"), "No GXL_141619 object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141618"), "No GXL_141618 object!");

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "ann_1", "200..300", sandBoxDir + "ann_test_0010_2_19.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("det_view_GXL_141619"), Qt::RightButton);

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "ann_2", "100..200", sandBoxDir + "ann_test_0010_2_18.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTWidget::click(GTWidget::findWidget("det_view_GXL_141618"), Qt::RightButton);

    GTUtilsAnnotationsTreeView::selectItemsByName({"ann_1", "ann_2"});

    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0010_2.gff", ExportAnnotationsFiller::gff));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDocument::removeDocument("DNA.fa");
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("ann_test_0010_2_18.gb");
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("ann_test_0010_2_19.gb");

    GTFileDialog::openFile(sandBoxDir, "ann_export_test_0010_2.gff");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141619 features"), "No GXL_141619 features object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141618 features"), "No GXL_141618 features object!");
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

    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTFileDialog::openFileWithDialog(testDir, "_common_data/fasta/DNA.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141619"), "No GXL_141619 object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141618"), "No GXL_141618 object!");

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(true, "<auto>", "ann_1", "200..300", sandBoxDir + "ann_test_0010_3_19.gb"));
    GTWidget::click(GTWidget::findWidget("det_view_GXL_141619"), Qt::RightButton);

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(true, "<auto>", "ann_2", "100..200", sandBoxDir + "ann_test_0010_3_18.gb"));
    GTWidget::click(GTWidget::findWidget("det_view_GXL_141618"), Qt::RightButton);

    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "YT483", "ann_1");
    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "001T", "ann_1");
    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "YT496", "ann_2");
    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "0012", "ann_2");

    GTUtilsAnnotationsTreeView::selectItemsByName({"ann_1", "ann_2"});

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0010_3.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());

    GTUtilsDocument::removeDocument("DNA.fa");
    GTUtilsDialog::add(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("ann_test_0010_3_18.gb");
    GTUtilsDialog::add(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("ann_test_0010_3_19.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("GTF"));
    GTFileDialog::openFile(sandBoxDir, "ann_export_test_0010_3.gtf");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141619 features"), "No GXL_141619 features object!");
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("GXL_141618 features"), "No GXL_141618 features object!");
}

GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // gene_id and transcript_id attributes are required for GTF format!

    //    1. Open "human_T1.fa"
    //    2. Create an annotation
    //    3. Try to export it to GTF format
    //    Expected state: info message - "GTF format is not strict - some annotations do not have "gene_id" and/or "transcript_id" qualifiers"

    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(true, "exon", "annotation", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "TR321", "annotation");
    GTUtilsAnnotationsTreeView::selectItemsByName({"annotation"});

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasMessage("GTF format is not strict - some annotations do not have 'gene_id' and/or 'transcript_id' qualifiers"), "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {
    // gene_id and transcript_id attributes are required for GTF format!

    //    1. Open "human_T1.fa"
    //    2. Create an annotation
    //    3. Try to export it to GTF format
    //    Expected state: info message - "GTF format is not strict - some annotations do not have "gene_id" and/or "transcript_id" qualifiers"

    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(true, "<auto>", "ann", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "XCV", "ann");
    GTUtilsAnnotationsTreeView::selectItemsByName({"ann"});

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasMessage("GTF format is not strict - some annotations do not have 'gene_id' and/or 'transcript_id' qualifiers"), "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_0011_3) {
    // gene_id and transcript_id attributes are required for GTF format!

    //    1. Open "human_T1.fa"
    //    2. Create an annotation
    //    4. Add 'gene_id' and 'transcript_id' qualifiers
    //    3. Export it to GTF format
    //    Expected state: no errors in the log

    GTLogTracer lt;

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Hide zoom view
    auto toolbar = GTWidget::findWidget("views_tool_bar_human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ADD", "create_annotation_action"}));
    GTUtilsDialog::add(new CreateAnnotationWidgetFiller(true, "<auto>", "ann", "200..300", sandBoxDir + "ann_test_0011_1.gb"));
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"), Qt::RightButton);

    GTWidget::click(GTUtilsAnnotationsTreeView::getTreeWidget());
    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "XCV", "ann");
    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "TR321", "ann");

    GTUtilsAnnotationsTreeView::selectItemsByName({"ann"});

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0011_1.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTMouseDriver::click(Qt::RightButton);
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
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

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 features"), "Object not found");

    GTFileDialog::openFile(testDir + "_common_data/gff/", "scaffold_90.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTUtilsProjectTreeView::checkItem("scaffold_90 features");

    QModelIndex idx = GTUtilsProjectTreeView::findIndex("scaffold_90 features");
    QWidget* sequence = GTUtilsSequenceView::getPanOrDetView();
    CHECK_SET_ERR(sequence != nullptr, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(idx, sequence);

    QStringList annList;
    annList << "5'UTR"
            << "exon";

    GTUtilsAnnotationsTreeView::selectItemsByName(annList);

    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0012_1.bed", ExportAnnotationsFiller::bed));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDocument::removeDocument("sars.gb");
    GTUtilsDocument::removeDocument("scaffold_90.gff");

    GTFileDialog::openFile(sandBoxDir, "ann_export_test_0012_1.bed");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 features"), "Object not found");
    GTUtilsProjectTreeView::checkNoItem("scaffold_90 features");
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

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 features"), "Object not found");

    GTFileDialog::openFile(testDir + "_common_data/gff/", "scaffold_90.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTUtilsProjectTreeView::checkItem("scaffold_90 features");

    QModelIndex idx = GTUtilsProjectTreeView::findIndex("scaffold_90 features");
    QWidget* sequence = GTUtilsSequenceView::getPanOrDetView();
    CHECK_SET_ERR(sequence != nullptr, "Sequence widget not found");

    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(idx, sequence);

    QStringList annList;
    annList << "5'UTR"
            << "exon";

    GTUtilsAnnotationsTreeView::selectItemsByName(annList);

    GTUtilsDialog::waitForDialog(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0012_2.gff", ExportAnnotationsFiller::gff));
    GTUtilsDialog::waitForDialog(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDocument::removeDocument("sars.gb");
    GTUtilsDocument::removeDocument("scaffold_90.gff");

    GTFileDialog::openFile(sandBoxDir, "ann_export_test_0012_2.gff");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 features"), "Object not found");

    GTUtilsProjectTreeView::checkNoItem("scaffold_90 features");
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

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 features"), "Object not found");

    GTFileDialog::openFile(testDir + "_common_data/gff/", "scaffold_90.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTUtilsProjectTreeView::checkItem("scaffold_90 features");

    QModelIndex documentProjectTreeIndex = GTUtilsProjectTreeView::findIndex("scaffold_90 features");
    QWidget* sequenceView = GTUtilsSequenceView::getPanOrDetView();

    GTUtilsDialog::add(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(documentProjectTreeIndex, sequenceView);
    GTUtilsTaskTreeView::waitTaskFinished();

    QList<QTreeWidgetItem*> exons = GTUtilsAnnotationsTreeView::findItems("exon");
    QList<QTreeWidgetItem*> utrs = GTUtilsAnnotationsTreeView::findItems("5'UTR");
    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "XCV", exons[0]);
    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "TR321", exons[0]);
    GTUtilsAnnotationsTreeView::createQualifier("gene_id", "XCV", utrs[0]);
    GTUtilsAnnotationsTreeView::createQualifier("transcript_id", "TR321", utrs[0]);

    GTUtilsAnnotationsTreeView::selectItemsByName({"5'UTR", "exon"});

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, "action_export_annotations"}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(sandBoxDir + "ann_export_test_0012_3.gtf", ExportAnnotationsFiller::gtf, false, false, false));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDialog::add(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("sars.gb");
    GTUtilsDialog::add(new MessageBoxDialogFiller("No"));
    GTUtilsDocument::removeDocument("scaffold_90.gff");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new DocumentFormatSelectorDialogFiller("GTF"));
    GTUtilsProject::openFile(sandBoxDir + "ann_export_test_0012_3.gtf");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("NC_004718 features"), "Object not found");
    GTUtilsProjectTreeView::checkNoItem("scaffold_90 features");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/proj2.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open view for "1.gb"
    GTUtilsProjectTreeView::doubleClickItem("NC_001363 features");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 3. Create annotation using menu {Actions->Add->New Annotation}, with description field filled
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(false, "<auto>", "ann1", "complement(1.. 20)", "", "description"));
    GTMenu::clickMainMenuItem({"Actions", "Add", "New annotation..."});

    // 4. Check what created annotation has corresponding qualifier 'note'
    auto groupItem = GTUtilsAnnotationsTreeView::findItem("ann1  (0, 1)");
    auto annotationItem = GTUtilsAnnotationsTreeView::findItem("ann1", groupItem);
    QString qualifierValue = GTUtilsAnnotationsTreeView::getQualifierValue("note", annotationItem);
    CHECK_SET_ERR(qualifierValue == "description", "Unexpected qualified value: " + qualifierValue);
}

}  // namespace GUITest_common_scenarios_annotations
}  // namespace U2
