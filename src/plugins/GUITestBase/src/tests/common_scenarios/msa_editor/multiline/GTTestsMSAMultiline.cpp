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

#include <api/GTUtils.h>
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QDir>
#include <QGraphicsItem>
#include <QMainWindow>

#include <U2Core/AppContext.h>

#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

#include "GTTestsMSAMultiline.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "system/GTClipboard.h"

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline {
using namespace HI;

namespace {
void enumerateMenu(QMenu* menu, QList<QString>* textItems) {
    foreach (QAction* action, menu->actions()) {
        if (action->isSeparator()) {
            qDebug("this action is a separator");
        } else if (action->menu()) {
            qDebug("action: %s", qUtf8Printable(action->text()));
            textItems->append(action->text());
            qDebug(">>> this action is associated with a submenu, iterating it recursively...");
            enumerateMenu(action->menu(), textItems);
            qDebug("<<< finished iterating the submenu");
        } else {
            qDebug("action: %s", qUtf8Printable(action->text()));
            textItems->append(action->text());
        }
    }
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(general_test_0001) {
    // UGENE-7042

    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 3. Find seq area #1
    auto w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 1);

    // 4. Switch to singleline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, false);

    // 5. Find seq area #0, but not #1
    w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);
    w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 1, false);
    CHECK_SET_ERR(w == nullptr, QString("Unexpectedly found sequence area #1"));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(zoom_to_selection_test_0001) {
    // UGENE-7605

    const QString seqName = "Phaneroptera_falcata";

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* resetZoom = GTAction::button(os, "Reset Zoom");
    GTWidget::click(os, resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // Select seq.
    GTUtilsMsaEditor::selectRowsByName(os, {seqName});

    resetZoom = GTAction::button(os, "Reset Zoom");
    QAbstractButton* zoom_to_sel = GTAction::button(os, "Zoom To Selection");
    GTWidget::click(os, zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMsaEditor::setMultilineMode(os, false);

    // Must not crash
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0001) {
    // UGENE-7522

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt* lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    GScrollBar* vscroll = GTWidget::findExactWidget<GScrollBar*>(os, "multiline_vertical_sequence_scroll");
    CHECK_SET_ERR(vscroll != nullptr, QString("Can't find vertical scroll bar"));
    CHECK_SET_ERR(vscroll->isVisible(), QString("Vertical scroll is not visible"));
    CHECK_SET_ERR(vscroll->isEnabled(), QString("Vertical scroll is disabled"));

    int minVal = vscroll->minimum();
    int maxVal = vscroll->maximum();
    int curVal = vscroll->value();
    CHECK_SET_ERR(minVal == 0 && maxVal > 600 && curVal == 0,
                  QString("Unexpected vertical scroll values min=%1, max=%2, value=%3")
                      .arg(minVal)
                      .arg(maxVal)
                      .arg(curVal));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0002) {
    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    GScrollBar* vscroll = GTWidget::findExactWidget<GScrollBar*>(os, "multiline_vertical_sequence_scroll");
    CHECK_SET_ERR(vscroll != nullptr, QString("Can't find vertical scroll bar"));
    CHECK_SET_ERR(vscroll->isVisible(), QString("Vertical scroll is not visible"));
    CHECK_SET_ERR(vscroll->isEnabled(), QString("Vertical scroll is disabled"));

    int minVal = vscroll->minimum();
    int maxVal = vscroll->maximum();
    int curVal = vscroll->value();
    CHECK_SET_ERR(minVal == 0 && maxVal > 600 && curVal == 0,
                  QString("Unexpected vertical scroll values min=%1, max=%2, value=%3")
                      .arg(minVal)
                      .arg(maxVal)
                      .arg(curVal));

    // Vscroll to end
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 600));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt* lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os, lastWgtIndex);
    CHECK_SET_ERR(lastBaseIdx == 603, QString("Not at the end"));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(menu_test_0001) {
    // UGENE-7524

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt* lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    // Select region
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(1, 2));

    // Show context menu
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0));
    auto menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    CHECK_SET_ERR(menu != nullptr, QString("No conrext menu"));

    // Check menu length
    QList<QString> allItems;
    enumerateMenu(menu, &allItems);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(2, 2));
    QSet<QString> allSet = allItems.toSet();
    CHECK_SET_ERR(allSet.size() == allItems.size(), "Context menu contains repeated items");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(goto_test_0001) {
    // UGENE-7571

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check goto
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 200));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 550));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(overview_test_0001) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    auto overview = GTWidget::findWidget(os, "msa_overview_area_graph");

    QImage initialImage = GTWidget::getImage(os, overview);

    //  Use context menu on overview: {Calculation method->Clustal}
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Calculation method", "Clustal"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, overview);

    //  Check that image was changed.
    QImage clustalModeImage = GTWidget::getImage(os, overview);
    CHECK_SET_ERR(initialImage != clustalModeImage, "overview was not changed (clustal)");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to 'histogram' mode.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Display settings", "Graph type", "Histogram"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, overview);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage histogramModeImage = GTWidget::getImage(os, overview);
    CHECK_SET_ERR(histogramModeImage != clustalModeImage, "overview was not changed (histogram)");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(overview_test_0002) {
    //    Open data/samples/CLUSTALW/HIV-1.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::toggleView(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    auto overview = GTWidget::findWidget(os, "msa_overview_area_graph");
    //    Show simple overview
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, overview);

    auto simple = GTWidget::findWidget(os, "msa_overview_area_simple");
    QColor initColor = GTWidget::getColor(os, simple, simple->geometry().center());
    QString initColorS = initColor.name();
    //    Press on overview with mouse left button

    GTWidget::click(os, overview);
    QColor finalColor = GTWidget::getColor(os, simple, simple->geometry().center());
    QString finalColorS = finalColor.name();
    CHECK_SET_ERR(initColorS != finalColorS, "color was not changed(1)");
    //    Expected state: visible range moved
    //    Drag visible range with mouse
    QColor initColor1 = GTWidget::getColor(os, simple, simple->geometry().topLeft() + QPoint(5, 5));
    QString initColorS1 = initColor1.name();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(10, GTMouseDriver::getMousePosition().y()));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    //    Expected state: visible range dragged
    QColor finalColor1 = GTWidget::getColor(os, simple, simple->geometry().topLeft() + QPoint(5, 5));
    QString finalColorS1 = finalColor1.name();
    CHECK_SET_ERR(initColorS1 != finalColorS1, "color was not changed(2)")

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(overview_test_0003) {
    //    Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    2. Go to MSA Overview context menu (right click on msa overview).
    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, 255, 0, 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Display settings", "Set color"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area_graph"));

    //    3. Go to {Display settings -> Color}.
    //    Expected state: color dialog appears.
    //    4. Chenge current color.
    //    Expected state: graph color had changed.
    auto graph = GTWidget::findWidget(os, "msa_overview_area_graph");
    const QColor c = GTWidget::getColor(os, graph, QPoint(5, graph->rect().height() - 5));
    CHECK_SET_ERR(c.name() == "#eda2a2", "simple overview has wrong color. Expected: #eda2a2, Found: " + c.name());

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(overview_test_0004) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    auto overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    auto overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");

    // saving overviews' images
    QImage imgSimple1 = GTWidget::getImage(os, overviewSimple);

    //    2. Select one symbol.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(5, 5));
    GTMouseDriver::click();

    //    3. Press Delete button and release it after a while.
    GTKeyboardDriver::keyPress(Qt::Key_Space);

    //    Expected state: while button is pressed graph overview is blocked. Overview updating starts on button release.
    //    Simple overview updates simultaneously.
    // checking simple overview image changed
    QImage imgSimple2 = GTWidget::getImage(os, overviewSimple);

    CHECK_SET_ERR(imgSimple1 != imgSimple2, "simple overview not updated");

    QColor c = GTWidget::getColor(os, overviewGraph, overviewGraph->rect().topLeft() + QPoint(5, 5));

    GTKeyboardDriver::keyRelease(Qt::Key_Space);

    CHECK_SET_ERR(c.name() == "#a0a0a4", "simple overview has wrong color. Expected: #a0a0a4, Found: " + c.name());

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(keys_test_0001) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa", "Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);  // wait for overview rendering to finish.
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    GTUtilsMSAEditorSequenceArea::click(os, QPoint(5, 5));
    //    Check keys: arrows
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(5, 4), QPoint(5, 4)));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(4, 4), QPoint(4, 4)));

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(4, 5), QPoint(4, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(5, 5), QPoint(5, 5)));

    //    shift + arrows
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(5, 4), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(4, 4), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(4, 5), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTThread::waitForMainThread();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(5, 5), QPoint(5, 5)));
    //    end
    auto vbar = GTWidget::findScrollBar(os, "multiline_vertical_sequence_scroll");
    GTKeyboardDriver::keyClick(Qt::Key_End, Qt::ControlModifier);
    CHECK_SET_ERR(vbar->value() > 20, QString("end key scrollbar value: %1").arg(vbar->value()))
    //    home
    GTKeyboardDriver::keyClick(Qt::Key_Home, Qt::ControlModifier);
    CHECK_SET_ERR(vbar->value() == 0, QString("home key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //    page down
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    CHECK_SET_ERR(vbar->value() > 20, QString("page down key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //    page up
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    CHECK_SET_ERR(vbar->value() == 0, QString("page down key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  end+shift
    GTKeyboardDriver::keyClick(Qt::Key_End, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() == vbar->maximum(), QString("shift + end key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  home+shift
    GTKeyboardDriver::keyClick(Qt::Key_Home, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() == 0, QString("shift + home key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  page down+shift
    GTKeyboardDriver::keyClick(Qt::Key_PageDown, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() > 20, QString("shift + page down key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  page up + shift
    GTKeyboardDriver::keyClick(Qt::Key_PageUp, Qt::ShiftModifier);
    CHECK_SET_ERR(vbar->value() == 0, QString("shift + page down key works wrong. Scrollbar value: %1").arg(vbar->value()))
    //  wheel event
    for (int i = 0; i < 3; i++) {
        GTMouseDriver::scroll(-1);
        GTThread::waitForMainThread();
    }

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(keys_test_0002) {
    //    Open COI.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    Press on some sequence in nameList
    GTUtilsMsaEditor::clickSequence(os, 2);
    auto hscroll = GTUtilsMSAEditorSequenceArea::getHorizontalNamesScroll(os, 0);
    auto vscroll = GTWidget::findScrollBar(os, "multiline_vertical_sequence_scroll");

    //    Check keys:
    //    right,
    for (int i = 0; i < 3; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right);
    }
    CHECK_SET_ERR(hscroll->value() == 3, QString("right key works wrong. Scrollbar has value: %1").arg(hscroll->value()));

    //    left
    for (int i = 0; i < 2; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Left);
    }
    CHECK_SET_ERR(hscroll->value() == 1, QString("left key works wrong. Scrollbar has value: %1").arg(hscroll->value()));

    //    page down
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    CHECK_SET_ERR(vscroll->value() > 20, QString("page down key works wrong: %1").arg(vscroll->value()));

    //    page up
    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    CHECK_SET_ERR(vscroll->value() == 0, QString("page up key works wrong: %1").arg(vscroll->value()));

    //    end
    GTKeyboardDriver::keyClick(Qt::Key_End);
    CHECK_SET_ERR(vscroll->value() > 1650, QString("end key works wrong: %1").arg(vscroll->value()));

    //    home
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    CHECK_SET_ERR(vscroll->value() == 0, QString("end key works wrong: %1").arg(vscroll->value()));

    //    mouse wheel
    for (int i = 0; i < 3; i++) {
        GTMouseDriver::scroll(-1);
    }
    const int scrolledValue = vscroll->value();
    CHECK_SET_ERR(scrolledValue > 0, QString("scroll down works wrong. Scrollbar has value: %1").arg(vscroll->value()));

    for (int i = 0; i < 2; i++) {
        GTMouseDriver::scroll(1);
    }
    CHECK_SET_ERR(0 < vscroll->value() && vscroll->value() < scrolledValue, QString("scroll up works wrong. Scrollbar has value: %1").arg(vscroll->value()));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(consensus_test_0001) {
    //    Open COI.aln consArea
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    Select some area on consensus with mouse
    GTUtilsMsaEditor::selectColumns(os, 1, 10, GTGlobals::UseMouse);

    //    Check selection on consensus and alignment
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(QPoint(1, 0), QPoint(10, 17)));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(similarity_test_0001) {
    //    Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //    Open "Statistics" OP tab
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Statistics);
    //    Set some reference sequence
    GTUtilsOptionPanelMsa::addReference(os, "Phaneroptera_falcata");

    //    Click "Show distance column". Check state
    auto showDistancesColumnCheck = GTWidget::findCheckBox(os, "showDistancesColumnCheck");
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    QString val1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    QString val2 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 2);
    CHECK_SET_ERR(val1 == "0%", "1: unexpected valeu1: " + val1);
    CHECK_SET_ERR(val2 == "20%", "1: unexpected valeu2: " + val2);
    //    Click "Show distance column". Check state
    GTCheckBox::setChecked(os, showDistancesColumnCheck, false);
    auto column = GTUtilsMSAEditorSequenceArea::getSimilarityColumn(os, 0);
    CHECK_SET_ERR(!column->isVisible(), "similarity column unexpectidly found");
    //    Click "Show distance column". Check state
    GTCheckBox::setChecked(os, showDistancesColumnCheck, true);
    val1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 0);
    val2 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(os, 2);
    CHECK_SET_ERR(val1 == "0%", "2: unexpected valeu1: " + val1);
    CHECK_SET_ERR(val2 == "20%", "2: unexpected valeu2: " + val2);

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(image_export_test_0001) {
    // fail to export big alignment
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    class CustomFiller_0048 : public Filler {
    public:
        CustomFiller_0048(HI::GUITestOpStatus& os)
            : Filler(os, "ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto exportType = GTWidget::findComboBox(os, "comboBox", dialog);
            CHECK_SET_ERR(exportType->currentText() == "Whole alignment", "Wrong combo box text!");

            auto hintLabel = GTWidget::findLabel(os, "hintLabel", dialog);
            CHECK_SET_ERR(hintLabel->isVisible(), "Warning message is hidden!");

            auto buttonBox = GTWidget::findDialogButtonBox(os, "buttonBox", dialog);
            QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(exportButton != nullptr, "Cannot find Export button");
            CHECK_SET_ERR(!exportButton->isEnabled(), "Export button is enabled");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(os, new PopupChooser(os, {MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(os, new CustomFiller_0048(os));

    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(drag_n_drop_test_0001) {
    // Shifting region in the Alignment Editor (UGENE-2127)
    //
    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    int oldLength = GTUtilsMSAEditorSequenceArea::getLength(os);
    // 2. Select the first column.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(os, 0);
    // Expected state: column became selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(os, QRect(0, 0, 1, 18));

    // 3. Drag the selection with mouse to 5 bases to the right.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(0, 0));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(5, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    // Expected state: alignment moved to 5 bases to the right.

    // 4. Drag the selection with mouse to one base to the left.
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(4, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    // Expected state: alignment  moved to one bases to the left.

    // Check results
    int newLength = GTUtilsMSAEditorSequenceArea::getLength(os);
    CHECK_SET_ERR(4 == newLength - oldLength, QString("Wrong length of changed alignment"));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(bookmark_test_0001) {
    //     1. Open document samples\CLUSTALW\COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    //     2. Create bookmark. Rename "New bookmark" to "start bookmark"
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]", "start bookmark");

    int startRO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    int startLO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    QWidget* mdiWindow = GTUtilsMdi::activeWindow(os);

    //     3. Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 300));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_NAVIGATION << "action_go_to_position"));
    GTMenu::showContextMenu(os, mdiWindow);

    //     4. Create bookmark. Rename "New bookmark" to "middle bookmark"
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]", "middle bookmark");

    const int midLO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);

    //     5. Scroll msa to the end.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 550));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_NAVIGATION << "action_go_to_position"));
    GTMenu::showContextMenu(os, mdiWindow);

    //     6. Create bookmark. Rename "New bookmark" to "end bookmark"
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]", "end bookmark");

    const int endLO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);

    //     Expected state: clicking on each bookmark will recall corresponding MSA position
    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "start bookmark");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    int RO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    int LO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(startRO == RO && startLO == LO, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "middle bookmark");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    RO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    LO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(midLO == LO, QString("middle bookmark offsets aren't equal to the expected: midLO=%1 LO=%2").arg(midLO).arg(LO));
    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "end bookmark");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    RO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    LO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    // CHECK_SET_ERR(endRO == RO && endLO == LO, "end bookmark offsets aren't equal to the expected");
    CHECK_SET_ERR(endLO == LO, QString("end bookmark offsets aren't equal to the expected: endLO=%3 LO=%4").arg(endLO).arg(LO));
    //     7. Delete Start bookmark
    GTUtilsBookmarksTreeView::deleteBookmark(os, "start bookmark");

    //     Expected state: start bookmark isn't present
    QTreeWidgetItem* startBookmark = GTUtilsBookmarksTreeView::findItem(os, "start bookmark", {false});
    CHECK_SET_ERR(startBookmark == nullptr, "Start bookmark is not deleted");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(bookmark_test_0002) {  // CHANGES: default names used
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // Create a bookmark. Do not rename the new bookmark.
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]");

    int startBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    int startBookmarkLastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);

    // Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 300));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Create a bookmark. Do not rename the new bookmark.
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]");

    int middleBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);

    // Scroll msa to the end.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 550));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Create bookmark. Do not rename the new bookmark..
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]");
    int endBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);

    // Expected state: click on each bookmark sets corresponding MSA position.
    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "New bookmark");
    int firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    int lastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    CHECK_SET_ERR(firstBase == startBookmarkFirstBase, QString("Start bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(startBookmarkFirstBase));
    CHECK_SET_ERR(lastBase == startBookmarkLastBase, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "New bookmark 2");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstBase == middleBookmarkFirstBase, QString("Middle bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(middleBookmarkFirstBase));

    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "New bookmark 3");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstBase == endBookmarkFirstBase, QString("End bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(endBookmarkFirstBase));

    // Delete Start bookmark & check it was deleted.
    GTUtilsBookmarksTreeView::deleteBookmark(os, "New bookmark");
    QTreeWidgetItem* startBookmark = GTUtilsBookmarksTreeView::findItem(os, "New bookmark", {false});
    CHECK_SET_ERR(startBookmark == nullptr, "Start bookmark wasn't deleted");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(bookmark_test_0003) {
    // CHANGES: mid and end coordinates changed.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // Create a bookmark. Rename "New bookmark" to "start bookmark".
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]", "start bookmark");
    int startBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    int startBookmarkLastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);

    //  Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 200));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Create a bookmark. Rename "New bookmark" to "middle bookmark"
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]", "middle bookmark");
    int middleBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);

    // Scroll msa to the end.
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 510));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Create bookmark. Rename "New bookmark" to "end bookmark".
    GTUtilsBookmarksTreeView::addBookmark(os, "COI [COI.aln]", "end bookmark");
    int endBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);

    // Expected state: click on each bookmark sets corresponding MSA position.
    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "start bookmark");
    int firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    int lastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os);
    CHECK_SET_ERR(firstBase == startBookmarkFirstBase, QString("Start bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(startBookmarkFirstBase));
    CHECK_SET_ERR(lastBase == startBookmarkLastBase, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "middle bookmark");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstBase == middleBookmarkFirstBase, QString("Middle bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(middleBookmarkFirstBase));

    GTUtilsBookmarksTreeView::doubleClickBookmark(os, "end bookmark");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(os);
    CHECK_SET_ERR(firstBase == endBookmarkFirstBase, QString("End bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(endBookmarkFirstBase));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(exclude_list_test_0001) {
    // Check initial state of MSA editor & Exclude list with no selection.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));

    auto toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(os, toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open Exclude List.
    GTWidget::click(os, toggleExcludeListButton);

    // Check Exclude List state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText(os, "Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton(os, "exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "COI.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton(os, "exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveFromMsaButton = GTWidget::findToolButton(os, "exclude_list_move_from_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget(os, "exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit(os, "exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(os, toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(exclude_list_test_0002) {
    // Check initial state of MSA editor & Exclude list with no selection.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os));

    auto toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(os, toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open Exclude List.
    GTWidget::click(os, toggleExcludeListButton);

    // Check Exclude List state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText(os, "Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton(os, "exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "ty3.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton(os, "exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveFromMsaButton = GTWidget::findToolButton(os, "exclude_list_move_from_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget(os, "exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit(os, "exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(os, toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, false);

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(tree_test_0001) {
    // Check that after a tree is built and sync mode is enabled the MaEditorRowOrder::Free mode is enabled:
    //  - 'toggle_sequence_row_order_action' is unchecked.
    //  - 'refresh_sequence_row_order_action' is disabled.
    //  - sequences are ordered by the tree.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    QStringList originalSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);

    GTUtilsProjectTreeView::toggleView(os);  // Close project view to make all actions on toolbar available.

    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, sandBoxDir + "msa_editor_tree_test_0002", 0, 0, true));
    GTWidget::click(os, GTAction::button(os, "Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList byTreeSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    QAbstractButton* syncModeButton = GTAction::button(os, "sync_msa_action");
    QAbstractButton* toggleSequenceOrderButton = GTAction::button(os, "toggle_sequence_row_order_action");
    QAbstractButton* refreshSequenceOrderButton = GTAction::button(os, "refresh_sequence_row_order_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/1");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/1");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/1");
    CHECK_SET_ERR(originalSequenceNames1 != byTreeSequenceNames1, "MSA must be re-ordered by tree");

    // Switch to the 'Sequence' mode.
    GTWidget::click(os, toggleSequenceOrderButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/1");
    CHECK_SET_ERR(toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be checked/1");
    CHECK_SET_ERR(refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be enabled/1");

    // Switch to the 'Original' mode.
    GTWidget::click(os, toggleSequenceOrderButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList originalSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/2");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/2");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/2");
    CHECK_SET_ERR(originalSequenceNames1 == originalSequenceNames2, "original sequence name list is not restored");

    // Switch back to the order-by-tree (MaEditorRowOrder::Free) mode.
    GTWidget::click(os, syncModeButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QStringList byTreeSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/2");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/3");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/3");
    CHECK_SET_ERR(byTreeSequenceNames1 == byTreeSequenceNames2, "by-tree sequence name list is not restored");

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0001) {
    // 1. Open an alignment in the Alignment Editor.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 2. Select one character.
    // Expected result : the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(9, 9), QPoint(9, 9));

    // 3. Open the main menu in the sequence area.
    // Expected result : the menu contains an item "Actions > Edit > Replace character".The item is enabled.A hotkey Shift + R is shown nearby.
    // 4. Select the item.
    // Expected result : the character is selected in the replacement mode.
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace selected character"}, GTGlobals::UseMouse);

    // 5. Press a key on the keyboard with another character of the same alphabet(e.g C key).
    // Expected result : the original character of the alignment was replaced with the new one(e.g 'A' was replaced with 'C').Selection is in normal mode.
    GTKeyboardDriver::keyClick('g');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text(os);
    CHECK_SET_ERR(selectionContent == "G", QString("Incorrect selection content: expected - %1, received - %2").arg("G").arg(selectionContent));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0002) {
    // 1. Open an alignment.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 2. Select a gap after the last sequence character.
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(603, 9));

    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    // 3. Replace the gap
    // Expected result : the gap is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);
    GTKeyboardDriver::keyClick('c');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text(os);
    CHECK_SET_ERR(selectionContent == "C", QString("Incorrect selection content: expected - %1, received - %2").arg("C").arg(selectionContent));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0003) {
    // 1. Open an alignment.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 2. Select a gap before the first sequence character.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(os, 0);
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 9), QPoint(0, 9));

    // 3. Replace the gap
    // Expected result : the gap is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick('c');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text(os);
    CHECK_SET_ERR(selectionContent == "C", QString("Incorrect selection content: expected - %1, received - %2").arg("C").arg(selectionContent));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0004) {
    // 1. Open an alignment.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 2. Select a last character in sequence.
    GTUtilsMSAEditorSequenceArea::clickToPosition(os, QPoint(603, 9));

    // 3. Replace the character
    // Expected result : the character is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick('c');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text(os);
    CHECK_SET_ERR(selectionContent == "C", QString("Incorrect selection content: expected - %1, received - %2").arg("C").arg(selectionContent));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0005) {
    // 1. Open an alignment.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    // 2. Select a character in sequence.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 9), QPoint(0, 9));

    // 3. Replace the character to gap by space key
    // Expected result : the character is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString selectionContent = GTClipboard::text(os);
    CHECK_SET_ERR(selectionContent == "-", QString("Incorrect selection content: expected - %1, received - %2").arg("-").arg(selectionContent));

    // 3. Select a character in sequence.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 10), QPoint(0, 10));

    // 4. Replace the character to gap by '-' key
    // Expected result : the character is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick('-');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    selectionContent = GTClipboard::text(os);
    CHECK_SET_ERR(selectionContent == "-", QString("Incorrect selection content: expected - %1, received - %2").arg("-").arg(selectionContent));

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

GUI_TEST_CLASS_DEFINITION(edit_test_0001) {
    // 1. Open document "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);

    static constexpr QPoint TOP_LEFT = QPoint(0, 0);
    static constexpr QPoint BOTTOM_RIGHT = QPoint(10, 10);

    //2. Select area from (0, 0) to (10, 10)
    GTUtilsMSAEditorSequenceArea::selectArea(os, TOP_LEFT, BOTTOM_RIGHT);

    // 3. Click "Replace with gaps" with popup menu
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"MSAE_MENU_EDIT", "replace_with_gaps"}));
    GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0));

    // Expected: selection hasn't been changed
    auto check = [&]() {
        auto selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);
        auto tl = selectedRect.topLeft();
        CHECK_SET_ERR(tl == TOP_LEFT, QString("Expected top-left selection: 0, 0; current: %1, %2").arg(tl.x()).arg(tl.y()));

        auto br = selectedRect.bottomRight();
        CHECK_SET_ERR(br == BOTTOM_RIGHT, QString("Expected bottom-right selection: 0, 0; current: %1, %2").arg(br.x()).arg(br.y()));

        static const QStringList GAPPED_DATA = {
            "-----------TAAGACTTC",
            "-----------TAAGCTTAC",
            "-----------TTAGTTTAT",
            "-----------TCAGTCTAT",
            "-----------TCAGTTTAT",
            "-----------TTAGTCTAC",
            "-----------TCAGATTAT",
            "-----------TTAGATTGC",
            "-----------TTAGATTAT",
            "-----------TAAGTCTAT",
            "-----------TTAGCTTAT"};

        static constexpr QPoint BOTTOM_RIGHT_CHECK = QPoint(20, 10);

        GTUtilsMSAEditorSequenceArea::selectArea(os, TOP_LEFT, BOTTOM_RIGHT_CHECK);
        GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
        auto result = GTClipboard::text(os).split("\n");
        for (int i = 0; i < 11; i++) {
            CHECK_SET_ERR(result[i].startsWith(GAPPED_DATA[i]), QString("Expected sequence beginning: %1; current: %2").arg(GAPPED_DATA[i]).arg(result[i].left(20)));
        }
    };
    check();

    // 4. Click undo
    GTWidget::click(os, GTAction::button(os, "msa_action_undo"));

    // 5. Select area from (0, 0) to (10, 10)
    GTUtilsMSAEditorSequenceArea::selectArea(os, TOP_LEFT, BOTTOM_RIGHT);

    // 6. Click "Replace with gaps" with shortcut
    GTKeyboardDriver::keyClick(Qt::Key_Space, Qt::ShiftModifier);

    // Expected: selection hasn't been changed
    check();

    GTUtilsMsaEditor::setMultilineMode(os, false);
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline
}  // namespace U2
