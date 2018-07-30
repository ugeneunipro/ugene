/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "GTTestsSequenceSelection.h"

#include "GTUtilsAnnotationsHighlightingTreeView.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

#include <base_dialogs/GTFileDialog.h>

#include "system/GTClipboard.h"

#include <primitives/GTAction.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>

#include <utils/GTKeyboardUtils.h>

#include <U2View/Overview.h>

#include "QTreeWidget"

namespace U2 {

namespace GUITest_common_scenarios_sequence_selection {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(double_click_test_0001) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state :
    //The corresponding annotations are shown in the Zoom View, Details View and Annotations Editor.
    //There is no selection in the Overview, Zoom View, Details View, Annotations Editor.
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.isEmpty(), "Unexpected selection");
    QString annTreeItem = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(annTreeItem.isEmpty(), QString("Incorrect selected item %1").arg(annTreeItem));

    //2. Double - click on the first CDS annotation in the Annotations Editor(the annotation location is 1042..2658).
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);

    //Expected state :
    //In the Overview : there is blue selection.
    //In the Zoom View : the annotation has wide border, the blue selection is shown.
    //In the Details View : the annotation has wide border, there is dashed selection.
    //In the Annotations Editor : the annotation is selected in the tree view.
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Expected section size: 1, current : %1").arg(selection.size()));
    CHECK_SET_ERR(selection.first() == U2Region(1041, 1617), QString("Expected selection: startPos = 1041, length = 1617, current: startPos = %1, length = %2").arg(selection.first().startPos).arg(selection.first().length));
    annTreeItem = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(annTreeItem == "CDS", QString("Incorrect selected item name, expected: CDS, current: %1").arg(annTreeItem));
}

GUI_TEST_CLASS_DEFINITION(double_click_test_0002) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Double - click on the first CDS annotation in the Annotations Editor(the annotation location is 1042..2658).
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, true);

    //3. Open "Copy/Paste" menu item in the Details View context menu.
    //Expected state :The following items are enabled :
    //    "Copy sequence"
    //    "Copy reverse-complement sequence"
    //    "Copy translation"
    //    "Copy reverse-complement translation"
    //    "Copy annotation sequence"
    //    "Copy annotation sequence translation"
    QStringList menuPath;
    menuPath << "Copy/Paste";
    QStringList itemsNames;
    itemsNames << "Copy sequence" << "Copy reverse-complement sequence" << "Copy translation" << "Copy reverse-complement translation" << "Copy annotation sequence" << "Copy annotation sequence translation";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, itemsNames, PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    Ctrl + C(Cmd + C on Mac OS X) keyboard shortcut is shown nearby the "Copy sequence" item.
    QKeySequence check_ks = QKeySequence(QKeySequence(Qt::CTRL | Qt::Key_C));
    QAction* copy = GTAction::findActionByText(os, "Copy sequence");
    QKeySequence ks = copy->shortcut();
    CHECK_SET_ERR(ks == check_ks, "Unexpected shortcut");

    //4. Open "Copy/Paste" menu item in the "Actions" main menu.
    //Expected state : Items, specified in the step 1 expected state, are enabled.
    //Ctrl + C(Cmd + C on Mac OS X) keyboard shortcut is shown nearby the "Copy sequence" item.
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions" << menuPath, itemsNames, PopupChecker::CheckOption(PopupChecker::IsEnabled));

    //5.Look at the SV toolbar.
    //Expected state : the same buttons as on the step 1 are enabled.

    //6. Click "Copy sequence".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, menuPath << "Copy sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    menuPath.removeOne("Copy sequence");

    //Expected state : a sequence that starts from "ATGGGCCAGACTGTT" is stored in the clipboard.
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGGCCAGACTGTT"), QString("'Copy sequence clipboard check', expected: ATGGGCCAGACTGTT, current: %1").arg(text.left(10)));

    //7. Click "Copy annotation sequence translation".
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << menuPath << "Copy annotation sequence translation");

    //Expected state : a sequence that starts from "MGQTVTTPLSL" is stored in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("MGQTVTTPLSL"), QString("'Copy annotation sequence translation' clipboard check, expected: MGQTVTTPLSL, current: %1").arg(text.left(10)));

    //8. Press Ctrl + C(or Cmd + C on Mac OS X).
    GTKeyboardUtils::copy(os);

    //Expected state : a sequence that starts from "ATGGGCCAGACTGTT" is stored in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGGCCAGACTGTT"), QString("'Copy sequence clipboard check', expected: ATGGGCCAGACTGTT, current: %1").arg(text.left(10)));
}

GUI_TEST_CLASS_DEFINITION(double_click_test_0003) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Double - click on the CDS annotation with location(3875..4999) in the Zoom View.
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 3875, 0, true);
    GTGlobals::sleep();

    //    Expected state :
    //    The Details View has been scrolled to the annotation location.The annotation has wide border.There is dashed selection of the region.
    //    The "CDS" group in the Annotations Editor has been opened.The annotation is selected in the tree view.
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "CDS");
    CHECK_SET_ERR(items.size() == 4, QString("Incorect size of CDS items in the tree, expcted: 4, current: %1").arg(items.size()));
    CHECK_SET_ERR(items[2]->isSelected(), "Item is not selected");
    QTreeWidgetItem* par = items[2]->parent();
    while (par != NULL) {
        CHECK_SET_ERR(par->isExpanded(), "Item is not expanded");
        par = par->parent();
    }
    CHECK_SET_ERR(!items[2]->isExpanded(), "Item is expanded");

    //3. Click on the "Zoom In" button in the Zoom View left toolbar.
    QAction* zoom = GTAction::findActionByText(os, "Zoom In");
    CHECK_SET_ERR(zoom != NULL, "Cannot find Zoom In action");
    GTWidget::click(os, GTAction::button(os, zoom));

    //Expected state : the Zoom View has been zoomed to the region of the annotation location.
    int start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start > 3000, "Location moved incorrect, first check");

    //4. Double - click on the "misc_feature" annotation with location(2..590) in the Annotations Editor.
    GTUtilsAnnotationsTreeView::clickItem(os, "misc_feature", 1, true);

    //    Expected state:
    //    The Details View has been scrolled to the "misc_feature" location.The annotation has wide border.There is dashed selection of the region.
    //    The Zoom View has been scrolled to the "misc_feature" location.The annotation has wide border.There is blue selection for the region.
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start < 1000, "Location moved incorrect, second check");

    //5. In the Details View scroll to coordinate 5050. Double - click on the annotation located there.
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    GTWidget::click(os, GTAction::button(os, wrapMode));
    GTUtilsSequenceView::clickAnnotationDet(os, "CDS", 5048, 0, true);
    GTGlobals::sleep();

    //    Expected state :
    //    The Zoom View has been scrolled to the fourth CDS annotation.The annotation has wide border.There is blue selection for the region.
    //    The fourth CDS annotation is selected in the Annotations Editor.
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start > 4500, "Location moved incorrect, third check");
}

GUI_TEST_CLASS_DEFINITION(double_click_test_0004) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Switch on the editing mode.
    QAction* editMode = GTAction::findActionByText(os, "Edit sequence");
    CHECK_SET_ERR(editMode != NULL, "Cannot find Edit mode action");
    GTWidget::click(os, GTAction::button(os, editMode));

    //3. Double-click in the Details View on the direct strand between coordinates 6 and 7.
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    //4. Press 'G' on the keyboard.
    GTKeyboardDriver::keyClick('g');
    GTGlobals::sleep();

    //Expected state:
    //The "misc_feature" annotations with locations (2..590) and (13..15) have been removed.
    //The sequence has been modified. It starts from characters "AGTGGGGGCT".
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "misc_feature");
    CHECK_SET_ERR(items.size() == 1, "Annotation was not removed");
}

GUI_TEST_CLASS_DEFINITION(mixed_test_0001) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click on the first CDS annotation with location(1042..2658).
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042);

    //3. Hold Shift, click on the second CDS annotation with location(2970, 3873) in the Annotations Editor.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    //GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 2970);
    //GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep();

    //    Expected state :
    //    In the Overview : there is NO blue selection.
    //    In the Zoom View : both annotations have wide border, the blue selection is NOT shown.
    //    In the Details View : both annotations have wide border, there is NO dashed selection.
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.isEmpty(), "Some incorrect selection");

    //    In the Annotations Editor : both annotations are selected in the tree view.
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "CDS");
    CHECK_SET_ERR(items.size() == 4, QString("Incorect size of CDS items in the tree, expcted: 4, current: %1").arg(items.size()));
    CHECK_SET_ERR(items[0]->isSelected(), "First item in the annotation tree view is not selected");
    CHECK_SET_ERR(items[1]->isSelected(), "Second item in the annotation tree view is not selected");

    //4. Hold Ctrl, double - click on the fourth CDS annotation with location(5048..5203).
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 5048, 0, true);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //    Expected state :
    //    In the Overview : there is blue selection for region(5048..5203).
    //    In the Zoom View : all three annotations have wide border, the blue selection is shown for region(5048..5203) only.
    //    In the Details View : all three annotations have wide border, the dashed selection is shown for region(5048..5203).
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Incorrect selection size, expected: 1, current: %1").arg(selection.size()));

    //    In the Annotations Editor : the corresponding three annotations are selected in the tree view.
    CHECK_SET_ERR(items[0]->isSelected(), "First item in the annotation tree view is not selected");
    CHECK_SET_ERR(items[1]->isSelected(), "Second item in the annotation tree view is not selected");
    CHECK_SET_ERR(items[3]->isSelected(), "Fourth item in the annotation tree view is not selected");

    //5. Select a region using the mouse.
    const QPoint pos = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(QPoint(pos.x(), pos.y() - 20));
    GTMouseDriver::click();
    GTGlobals::sleep();

    //    Expected state : nothing is selected in the Overview, Zoom View, Details View, Annotations Editor.
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.isEmpty(), "Some incorrect selection");
    CHECK_SET_ERR(!items[0]->isSelected(), "First item - unexpected selection");
    CHECK_SET_ERR(!items[1]->isSelected(), "Second item - unexpected selection");
    CHECK_SET_ERR(!items[2]->isSelected(), "Third item - unexpected selection");
    CHECK_SET_ERR(!items[3]->isSelected(), "Fourth item - unexpected selection");
}

GUI_TEST_CLASS_DEFINITION(mixed_test_0002) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click on the first CDS annotation with location(1042..2658).
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042);
    GTGlobals::sleep();

    //3. Hold Ctrl, click on the second CDS annotation with location(2970..3873).
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 2970);
    GTGlobals::sleep();

    //4. Hold Ctrl, double - click on the third CDS annotation with location(3875..4999).
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 3875, 0, true);
    GTGlobals::sleep();

    //5. Hold Ctrl, double - click on the fourth CDS annotation with location(5048..5203).
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 5048, 0, true);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //6. Click "Copy sequence".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : there is joined sequence from the third and fourth annotations in the clipboard.
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGCGCATT"), QString("Unexpected start of the clipboard text, expected: ATGGCGCATT, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("CGAGCCATAG"), QString("Unexpected end of the clipboard text, expected: CGAGCCATAG, current: %1").arg(text.right(10)));

    //7. Click "Copy reverse-complement sequence".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy reverse-complement sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : there is joined reverse - complement sequence from the third and fourth annotations in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("CTATGGCTCG"), QString("Unexpected start of the clipboard text, expected: CTATGGCTCG, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("AATGCGCCAT"), QString("Unexpected end of the clipboard text, expected: AATGCGCCAT, current: %1").arg(text.right(10)));

    //8. Click "Copy translation".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy translation"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : there is joined sequence from the third and fourth annotations translations in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("MAHSTPCSQT"), QString("Unexpected start of the clipboard text, expected: MAHSTPCSQT, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("QLKPIEYEP*"), QString("Unexpected end of the clipboard text, expected: QLKPIEYEP*, current: %1").arg(text.right(10)));

    //9. Click "Copy reverse-complement translation".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy reverse-complement translation"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : there is joined sequence from the third and fourth annotations reverse - complement translations in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("LWLVLYRLQL"), QString("Unexpected start of the clipboard text, expected: LWLVLYRLQL, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("SLGAWR*MRH"), QString("Unexpected end of the clipboard text, expected: SLGAWR*MRH, current: %1").arg(text.right(10)));

    //10. Click "Copy annotation sequence".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy annotation sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : there is joined sequence from all four selected annotations in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGGCCAGA"), QString("Unexpected start of the clipboard text, expected: ATGGGCCAGA, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("CGAGCCATAG"), QString("Unexpected end of the clipboard text, expected: CGAGCCATAG, current: %1").arg(text.right(10)));

    //11. Click "Copy annotation sequence translation".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy annotation sequence translation"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : there is joined sequence from all four selected annotations translation in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("MGQTVTTPLS"), QString("Unexpected start of the clipboard text, expected: MGQTVTTPLS, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("QLKPIEYEP*"), QString("Unexpected end of the clipboard text, expected: QLKPIEYEP*, current: %1").arg(text.right(10)));


    //12. Press Ctrl + C(Cmd + C on Mac OS X).
    GTKeyboardUtils::copy(os);

    //    Expected state : there is joined sequence from the third and fourth annotations in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGCGCATT"), QString("Unexpected start of the clipboard text, expected: ATGGCGCATT, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("CGAGCCATAG"), QString("Unexpected end of the clipboard text, expected: CGAGCCATAG*, current: %1").arg(text.right(10)));
}

GUI_TEST_CLASS_DEFINITION(mixed_test_0003) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click on the first CDS annotation with location(1042..2658).
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 1042);
    GTGlobals::sleep();

    //3. Open menu item "Select" in the Details View context menu.
    //    Expected state :
    //    There is item "Sequence between selected annotations", it is disabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Select" << "Sequence between selected annotations", PopupChecker::IsDisabled));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    There is item "Sequence around selected annotations", it is enabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Select" << "Sequence around selected annotations", PopupChecker::IsEnabled));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Click "Sequence around selected annotations".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Select" << "Sequence around selected annotations"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state :
    //In the Overview : there is blue selection for region(1042..2658).
    //    In the Zoom View : there is blue selection for region(1042..2658).
    //    In the Details View : there is dashed selection for region(1042..2658).
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Incorrect size of selected regions, expectd: 1, current: %1").arg(selection.size()));
    U2Region sel = selection.first();
    CHECK_SET_ERR(sel == U2Region(1041, 1617), QString("Unexpected selected region, expected^ start 2658, length 311, current: start %1 length %2").arg(sel.startPos).arg(sel.length));

    //5. Hold Ctrl, click on the second CDS annotation with location(2970..3873).
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 2970);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //6. Open menu item "Select" in the Details View context menu.
    //    Expected state :
    //    There is item "Sequence between selected annotations", it is enabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Select" << "Sequence between selected annotations", PopupChecker::IsEnabled));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    There is item "Sequence around selected annotations", it is enabled.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Select" << "Sequence around selected annotations", PopupChecker::IsEnabled));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //7. Click "Sequence between selected annotations".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Select" << "Sequence between selected annotations"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state :
    //    Region(2659, 2969) is selected.
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Incorrect size of selected regions, expectd: 1, current: %1").arg(selection.size()));
    sel = selection.first();
    CHECK_SET_ERR(sel == U2Region(2658, 311), QString("Unexpected selected region, expected: start 2658, length 311, current: start %1 length %2").arg(sel.startPos).arg(sel.length));

    //    The first and the second CDS annotations are selected(but not their regions).
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "CDS");
    CHECK_SET_ERR(items.size() == 4, QString("Incorect size of CDS items in the tree, expcted: 4, current: %1").arg(items.size()));
    CHECK_SET_ERR(items[0]->isSelected(), "First item in the annotation tree view is not selected");
    CHECK_SET_ERR(items[1]->isSelected(), "Second item in the annotation tree view is not selected");

    //8. Click "Select > Sequence around selected annotations".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Select" << "Sequence around selected annotations"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state :
    //    Region(1042, 3873) is selected.
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.size() == 1, QString("Incorrect size of selected regions, expectd: 1, current: %1").arg(selection.size()));
    sel = selection.first();
    CHECK_SET_ERR(sel == U2Region(1041, 2372), QString("Unexpected selected region, expected: start 1041, length 2372, current: start %1 length %2").arg(sel.startPos).arg(sel.length));

    //    The first and the second CDS annotations are selected.
    items = GTUtilsAnnotationsTreeView::findItems(os, "CDS");
    CHECK_SET_ERR(items.size() == 4, QString("Incorect size of CDS items in the tree, expcted: 4, current: %1").arg(items.size()));
    CHECK_SET_ERR(items[0]->isSelected(), "First item in the annotation tree view is not selected");
    CHECK_SET_ERR(items[1]->isSelected(), "Second item in the annotation tree view is not selected");
}

GUI_TEST_CLASS_DEFINITION(one_click_test_0001) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state :
    //    The corresponding annotations are shown in the Zoom View, Details View and Annotations Editor.
    //    There is no selection in the Overview, Zoom View, Details View, Annotations Editor.
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.isEmpty(), "Unexpected selection");
    QString annTreeItem = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(annTreeItem.isEmpty(), QString("Incorrect selected item %1").arg(annTreeItem));

    //2. Click on the first CDS annotation in the Annotations Editor(the annotation location is 1042..2658).
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, false);

    //    Expected state :
    //    In the Overview : there is NO blue selection.
    //    In the Zoom View : the annotation has wide border, but there is NO blue selection.
    //    In the Details View : the annotation has wide border, but there is NO dashed selection.
    //    In the Annotations Editor : the annotation is selected in the tree view.
    selection = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(selection.isEmpty(), QString("Expected section size: 0, current : %1").arg(selection.size()));

    annTreeItem = GTUtilsAnnotationsTreeView::getSelectedItem(os);
    CHECK_SET_ERR(annTreeItem == "CDS", QString("Incorrect selected item name, expected: CDS, current: %1").arg(annTreeItem));
}

GUI_TEST_CLASS_DEFINITION(one_click_test_0002) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click on the first CDS annotation in the Annotations Editor(the annotation location is 1042..2658).
    GTUtilsAnnotationsTreeView::clickItem(os, "CDS", 1, false);

    //3. Open "Copy/Paste" menu item in the Details View context menu.
    //    Expected state :
    //    The following items are disabled :
    //    "Copy sequence"
    //    "Copy reverse-complement sequence"
    //    "Copy translation"
    //    "Copy reverse-complement translation"
    //    The following items are enabled :
    //    "Copy annotation sequence"
    //    "Copy annotation sequence translation"
    QStringList enabledItemsNames = QStringList() << "Copy sequence" << "Copy reverse-complement sequence" << "Copy translation" << "Copy reverse-complement translation";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Copy/Paste", enabledItemsNames, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList disabledItemsNames = QStringList() << "Copy annotation sequence" << "Copy annotation sequence translation";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Copy/Paste", disabledItemsNames, PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Ctrl + C(Cmd + C on Mac OS X) keyboard shortcut is shown nearby the "Copy annotation sequence" item.
    QKeySequence check_ks = QKeySequence(QKeySequence(Qt::CTRL | Qt::Key_C));
    QAction* copy = GTAction::findActionByText(os, "Copy annotation sequence");
    QKeySequence ks = copy->shortcut();
    CHECK_SET_ERR(ks == check_ks, "Unexpected shortcut");

    //4. Open "Copy/Paste" menu item in the "Actions" main menu.
    //    Expected state :
    //    The same items as on step 1 are disabled / enabled.
    //    Ctrl + C(Cmd + C on Mac OS X) keyboard shortcut is shown nearby the "Copy annotation sequence" item.
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions" << "Copy/Paste", enabledItemsNames, PopupChecker::CheckOption(PopupChecker::IsDisabled));
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions" << "Copy/Paste", disabledItemsNames, PopupChecker::CheckOption(PopupChecker::IsEnabled));

    //5. Look at the SV toolbar.
    //    Expected state : the same buttons as on step 1 are disabled / enabled.

    //6. Click "Copy annotation sequence".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy annotation sequence"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : a sequence that starts from "ATGGGCCAGACTGTT" is stored in the clipboard.
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGGCCAGA"), QString("Unexpected start of the clipboard text, expected: ATGGGCCAGA, current: %1").arg(text.left(10)));

    //7. Click "Copy annotation sequence translation".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy annotation sequence translation"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : a sequence that starts from "MGQTVTTPLSL" is stored in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("MGQTVTTPLS"), QString("Unexpected start of the clipboard text, expected: MGQTVTTPLS, current: %1").arg(text.left(10)));

    //8. Press Ctrl + C(or Cmd + C on Mac OS X).
    GTKeyboardUtils::copy(os);

    //    Expected state : a sequence that starts from "ATGGGCCAGACTGTT" is stored in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("ATGGGCCAGA"), QString("Unexpected start of the clipboard text, expected: ATGGGCCAGA, current: %1").arg(text.left(10)));
}

GUI_TEST_CLASS_DEFINITION(one_click_test_0003) {
    //1. Open "murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : the following buttons on the toolbar are disabled :
    //    "Copy sequence"
    //    "Copy reverse-complement sequence"
    //    "Copy translation"
    //    "Copy reverse-complement translation"
    //    "Copy annotation sequence"
    //    "Copy annotation sequence translation"
    QStringList enabledItemsNamesFirst = QStringList() << "Copy sequence" << "Copy reverse-complement sequence" << "Copy translation" << "Copy reverse-complement translation" << "Copy annotation sequence" << "Copy annotation sequence translation";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Copy/Paste", enabledItemsNamesFirst, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select a sequence region using the mouse.
    GTUtilsSequenceView::selectSequenceRegion(os, 100, 200);

    QStringList enabledItemsNamesSecond = QStringList() << "Copy sequence" << "Copy reverse-complement sequence" << "Copy translation" << "Copy reverse-complement translation";
    QStringList disableItemNames = QStringList()  << "Copy annotation sequence" << "Copy annotation sequence translation";

    //    Expected state :
    //    The following buttons are enabled :
    //    "Copy sequence"
    //    "Copy reverse-complement sequence"
    //    "Copy translation"
    //    "Copy reverse-complement translation"
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Copy/Paste", enabledItemsNamesSecond, PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    The following buttons are disabled :
    //    "Copy annotation sequence"
    //    "Copy annotation sequence translation"
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Copy/Paste", disableItemNames, PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Click "Copy translation".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Copy/Paste" << "Copy translation"));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state : the selected region translation is stored in the clipboard.
    QString text = GTClipboard::text(os);
    CHECK_SET_ERR(text == "RSGTKKQLNTKQDICGKRFLPRLRAKNR*DS*V", QString("Unexpected text in the clipboard, expected: RSGTKKQLNTKQDICGKRFLPRLRAKNR*DS*V, current: %1").arg(text));

    //4. Press Ctrl + C(or Cmd + C on Mac OS X) on the keyboard.
    GTKeyboardUtils::copy(os);

    //    Expected state : the selected region is stored in the clipboard.
    text = GTClipboard::text(os);
    CHECK_SET_ERR(text.startsWith("AGGTCAGGAA"), QString("Unexpected start of the clipboard text, expected: AGGTCAGGAA, current: %1").arg(text.left(10)));
    CHECK_SET_ERR(text.endsWith("GCTGAGTGAT"), QString("Unexpected end of the clipboard text, expected: GCTGAGTGAT, current: %1").arg(text.right(10)));
}

GUI_TEST_CLASS_DEFINITION(one_click_test_0004) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Click on the CDS annotation with location(3875..4999) in the Zoom View.
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 3875, 0, false);
    GTGlobals::sleep();

    //    Expected state :
    //    The Details View has been scrolled to the annotation location.The annotation has wide border.
    //    The "CDS" group in the Annotations Editor has been opened.The annotation is selected in the tree view.
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "CDS");
    CHECK_SET_ERR(items.size() == 4, QString("Incorect size of CDS items in the tree, expcted: 4, current: %1").arg(items.size()));
    CHECK_SET_ERR(items[2]->isSelected(), "Item is not selected");
    QTreeWidgetItem* par = items[2]->parent();
    while (par != NULL) {
        CHECK_SET_ERR(par->isExpanded(), "Item is not expanded");
        par = par->parent();
    }
    CHECK_SET_ERR(!items[2]->isExpanded(), "Item is expanded");

    //3. Click on the "Zoom In" button in the Zoom View left toolbar.
    QAction* zoom = GTAction::findActionByText(os, "Zoom In");
    CHECK_SET_ERR(zoom != NULL, "Cannot find Zoom In action");
    GTWidget::click(os, GTAction::button(os, zoom));

    //    Expected state : the Zoom View has been zoomed to the region of the annotation location.
    int start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start > 3000, "Location moved incorrect, first check");

    //4. Click on the "misc_feature" annotation with location(2..590) in the Annotations Editor.
    GTUtilsAnnotationsTreeView::clickItem(os, "misc_feature", 1, false);

    //    Expected state :
    //    The Details View has been scrolled to the "misc_feature" location.The annotation has wide border.
    //    The Zoom View has been scrolled to the "misc_feature" location.The annotation has wide border.
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start < 1000, "Location moved incorrect, second check");

    //5. In the Details View scroll to coordinate 5050. Click on the annotation located there.
    QAction* wrapMode = GTAction::findActionByText(os, "Wrap sequence");
    CHECK_SET_ERR(wrapMode != NULL, "Cannot find Wrap sequence action");
    GTWidget::click(os, GTAction::button(os, wrapMode));
    GTUtilsSequenceView::clickAnnotationDet(os, "CDS", 5048, 0, false);
    GTGlobals::sleep();

    //    Expected state :
    //    The Zoom View has been scrolled to the fourth CDS annotation.The annotation has wide border.
    //    The fourth CDS annotation is selected in the Annotations Editor.
    start = GTUtilsSequenceView::getVisiableStart(os);
    CHECK_SET_ERR(start > 4500, "Location moved incorrect, third check");
}

GUI_TEST_CLASS_DEFINITION(one_click_test_0005) {
    //1. Open "murine.gb" in SV.
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Switch on the editing mode.
    QAction* editMode = GTAction::findActionByText(os, "Edit sequence");
    CHECK_SET_ERR(editMode != NULL, "Cannot find Edit mode action");
    GTWidget::click(os, GTAction::button(os, editMode));

    //3. Click in the Details View on the direct strand between coordinates 6 and 7.
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2);
    GTGlobals::sleep();

    //4. Press 'G' on the keyboard.
    GTKeyboardDriver::keyClick('g');
    GTGlobals::sleep();

    //    Expected state : the sequence has been modified.It starts from characters "AAATGAGAAGAC".
    QList<QTreeWidgetItem*> items = GTUtilsAnnotationsTreeView::findItems(os, "misc_feature");
    CHECK_SET_ERR(items.size() == 2, "Annotation was removed");
}

} // namespace GUITest_common_scenarios_sequence_view

} // namespace U2

