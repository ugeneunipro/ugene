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
#include <primitives/GTMenu.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QDir>
#include <QGraphicsItem>
#include <QMainWindow>

#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

#include "GTTestsMSAMultiline.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "system/GTClipboard.h"

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline {
using namespace HI;

static void enumerateMenu(QMenu* menu, QList<QString>* textItems) {
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

GUI_TEST_CLASS_DEFINITION(general_test_0001) {
    // UGENE-7042

    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 3. Find seq area #1
    auto w = GTUtilsMSAEditorSequenceArea::getSequenceArea(1);

    // 4. Switch to singleline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(false);

    // 5. Find seq area #0, but not #1
    w = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    w = GTUtilsMSAEditorSequenceArea::getSequenceArea(1, false);
    CHECK_SET_ERR(w == nullptr, QString("Unexpectedly found sequence area #1"));
}

GUI_TEST_CLASS_DEFINITION(zoom_to_selection_test_0001) {
    // UGENE-7605

    const QString seqName = "Phaneroptera_falcata";

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* resetZoom = GTAction::button("Reset Zoom");
    GTWidget::click(resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // Select seq.
    GTUtilsMsaEditor::selectRowsByName({seqName});

    resetZoom = GTAction::button("Reset Zoom");
    QAbstractButton* zoom_to_sel = GTAction::button("Zoom To Selection");
    GTWidget::click(zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(resetZoom);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::setMultilineMode(false);

    // Must not crash
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0001) {
    // UGENE-7522

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt* lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor()->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    GScrollBar* vscroll = GTWidget::findExactWidget<GScrollBar*>("multiline_vertical_sequence_scroll");
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
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0002) {
    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    GScrollBar* vscroll = GTWidget::findExactWidget<GScrollBar*>("multiline_vertical_sequence_scroll");
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
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(600);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt* lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor()->getUI()->getUI(i);
        if (w == nullptr) {
            break;
        }
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(lastWgtIndex);
    CHECK_SET_ERR(lastBaseIdx == 603, QString("Not at the end: %1").arg(lastBaseIdx));
}

GUI_TEST_CLASS_DEFINITION(menu_test_0001) {
    // UGENE-7524

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt* lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor()->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    // Select region
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(1, 2));

    // Show context menu
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
    auto menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    CHECK_SET_ERR(menu != nullptr, QString("No conrext menu"));

    // Check menu length
    QList<QString> allItems;
    enumerateMenu(menu, &allItems);
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(2, 2));
    QSet<QString> allSet = allItems.toSet();
    CHECK_SET_ERR(allSet.size() == allItems.size(), "Context menu contains repeated items");
}

GUI_TEST_CLASS_DEFINITION(goto_test_0001) {
    // UGENE-7571

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check goto
    GTUtilsMsaEditor::gotoWithKeyboardShortcut(200);

    GTUtilsMsaEditor::gotoWithKeyboardShortcut(550);
}

GUI_TEST_CLASS_DEFINITION(overview_test_0001) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    auto overview = GTWidget::findWidget("msa_overview_area_graph");

    QImage initialImage = GTWidget::getImage(overview);

    //  Use context menu on overview: {Calculation method->Clustal}
    GTUtilsDialog::waitForDialog(new PopupChooser({"Calculation method", "Clustal"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(overview);

    //  Check that image was changed.
    QImage clustalModeImage = GTWidget::getImage(overview);
    CHECK_SET_ERR(initialImage != clustalModeImage, "overview was not changed (clustal)");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to 'histogram' mode.
    GTUtilsDialog::waitForDialog(new PopupChooser({"Display settings", "Graph type", "Histogram"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(overview);
    GTUtilsTaskTreeView::waitTaskFinished();
    QImage histogramModeImage = GTWidget::getImage(overview);
    CHECK_SET_ERR(histogramModeImage != clustalModeImage, "overview was not changed (histogram)");
}

GUI_TEST_CLASS_DEFINITION(overview_test_0002) {
    //    Open data/samples/CLUSTALW/HIV-1.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::toggleView();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    auto overview = GTWidget::findWidget("msa_overview_area_graph");
    //    Show simple overview
    GTUtilsDialog::waitForDialog(new PopupChooser({"Show simple overview"}));
    GTMenu::showContextMenu(overview);

    auto simple = GTWidget::findWidget("msa_overview_area_simple");
    QColor initColor = GTWidget::getColor(simple, simple->geometry().center());
    QString initColorS = initColor.name();
    //    Press on overview with mouse left button

    GTWidget::click(overview);
    QColor finalColor = GTWidget::getColor(simple, simple->geometry().center());
    QString finalColorS = finalColor.name();
    CHECK_SET_ERR(initColorS != finalColorS, "color was not changed(1)");
    //    Expected state: visible range moved
    //    Drag visible range with mouse
    QColor initColor1 = GTWidget::getColor(simple, simple->geometry().topLeft() + QPoint(5, 5));
    QString initColorS1 = initColor1.name();
    GTMouseDriver::press();
    GTMouseDriver::moveTo(QPoint(10, GTMouseDriver::getMousePosition().y()));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    //    Expected state: visible range dragged
    QColor finalColor1 = GTWidget::getColor(simple, simple->geometry().topLeft() + QPoint(5, 5));
    QString finalColorS1 = finalColor1.name();
    CHECK_SET_ERR(initColorS1 != finalColorS1, "color was not changed(2)")
}

GUI_TEST_CLASS_DEFINITION(overview_test_0003) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    2. Go to MSA Overview context menu (right click on msa overview).
    GTUtilsDialog::waitForDialog(new ColorDialogFiller(255, 0, 0));
    GTUtilsDialog::waitForDialog(new PopupChooser({"Display settings", "Set color"}));
    GTMenu::showContextMenu(GTWidget::findWidget("msa_overview_area_graph"));

    //    3. Go to {Display settings -> Color}.
    //    Expected state: color dialog appears.
    //    4. Chenge current color.
    //    Expected state: graph color had changed.
    auto graph = GTWidget::findWidget("msa_overview_area_graph");
    const QColor c = GTWidget::getColor(graph, QPoint(5, graph->rect().height() - 5));
    CHECK_SET_ERR(c.name() == "#eda2a2", "simple overview has wrong color. Expected: #eda2a2, Found: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(overview_test_0004) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Show simple overview"}));
    GTMenu::showContextMenu(GTWidget::findWidget("msa_overview_area"));

    auto overviewSimple = GTWidget::findWidget("msa_overview_area_simple");
    auto overviewGraph = GTWidget::findWidget("msa_overview_area_graph");

    // saving overviews' images
    QImage imgSimple1 = GTWidget::getImage(overviewSimple);

    //    2. Select one symbol.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(5, 5));
    GTMouseDriver::click();

    //    3. Press Delete button and release it after a while.
    GTKeyboardDriver::keyPress(Qt::Key_Space);

    //    Expected state: while button is pressed graph overview is blocked. Overview updating starts on button release.
    //    Simple overview updates simultaneously.
    // checking simple overview image changed
    QImage imgSimple2 = GTWidget::getImage(overviewSimple);

    CHECK_SET_ERR(imgSimple1 != imgSimple2, "simple overview not updated");

    QColor c = GTWidget::getColor(overviewGraph, overviewGraph->rect().topLeft() + QPoint(5, 5));

    GTKeyboardDriver::keyRelease(Qt::Key_Space);

    CHECK_SET_ERR(c.name() == "#a0a0a4", "simple overview has wrong color. Expected: #a0a0a4, Found: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(keys_test_0001) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa", "Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished();  // wait for overview rendering to finish.
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    GTUtilsMSAEditorSequenceArea::click(QPoint(5, 5));
    //    Check keys: arrows
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 4), QPoint(5, 4)));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 4), QPoint(4, 4)));

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 5), QPoint(4, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 5), QPoint(5, 5)));

    //    shift + arrows
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Up);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 4), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Left);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 4), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Down);
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(4, 5), QPoint(5, 5)));

    GTKeyboardDriver::keyClick(Qt::Key_Right);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTThread::waitForMainThread();
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(5, 5), QPoint(5, 5)));
    //    end
    auto vbar = GTWidget::findScrollBar("multiline_vertical_sequence_scroll");
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
}

GUI_TEST_CLASS_DEFINITION(keys_test_0002) {
    //    Open COI.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/Chikungunya_E1.fasta");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    Press on some sequence in nameList
    GTUtilsMsaEditor::clickSequence(2);
    auto hscroll = GTUtilsMSAEditorSequenceArea::getHorizontalNamesScroll(0);
    auto vscroll = GTWidget::findScrollBar("multiline_vertical_sequence_scroll");

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
}

GUI_TEST_CLASS_DEFINITION(consensus_test_0001) {
    //    Open COI.aln consArea
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    Select some area on consensus with mouse
    GTUtilsMsaEditor::selectColumns(1, 10, GTGlobals::UseMouse);

    //    Check selection on consensus and alignment
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(QPoint(1, 0), QPoint(10, 17)));
}

GUI_TEST_CLASS_DEFINITION(similarity_test_0001) {
    //    Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //    Open "Statistics" OP tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Statistics);
    //    Set some reference sequence
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    //    Click "Show distance column". Check state
    auto showDistancesColumnCheck = GTWidget::findCheckBox("showDistancesColumnCheck");
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    QString val1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    QString val2 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(2);
    CHECK_SET_ERR(val1 == "0%", "1: unexpected valeu1: " + val1);
    CHECK_SET_ERR(val2 == "20%", "1: unexpected valeu2: " + val2);
    //    Click "Show distance column". Check state
    GTCheckBox::setChecked(showDistancesColumnCheck, false);
    auto column = GTUtilsMSAEditorSequenceArea::getSimilarityColumn(0);
    CHECK_SET_ERR(!column->isVisible(), "similarity column unexpectidly found");
    //    Click "Show distance column". Check state
    GTCheckBox::setChecked(showDistancesColumnCheck, true);
    val1 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(0);
    val2 = GTUtilsMSAEditorSequenceArea::getSimilarityValue(2);
    CHECK_SET_ERR(val1 == "0%", "2: unexpected valeu1: " + val1);
    CHECK_SET_ERR(val2 == "20%", "2: unexpected valeu2: " + val2);
}

GUI_TEST_CLASS_DEFINITION(image_export_test_0001) {
    // fail to export big alignment
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "3000_sequences.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    class CustomFiller_0048 : public Filler {
    public:
        CustomFiller_0048()
            : Filler("ImageExportForm") {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto exportType = GTWidget::findComboBox("comboBox", dialog);
            CHECK_SET_ERR(exportType->currentText() == "Whole alignment", "Wrong combo box text!");

            auto hintLabel = GTWidget::findLabel("hintLabel", dialog);
            CHECK_SET_ERR(hintLabel->isVisible(), "Warning message is hidden!");

            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(exportButton != nullptr, "Cannot find Export button");
            CHECK_SET_ERR(!exportButton->isEnabled(), "Export button is enabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "export_msa_as_image_action"}));
    GTUtilsDialog::add(new CustomFiller_0048());

    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));
}

GUI_TEST_CLASS_DEFINITION(drag_n_drop_test_0001) {
    // Shifting region in the Alignment Editor (UGENE-2127)
    //
    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    int oldLength = GTUtilsMSAEditorSequenceArea::getLength();
    // 2. Select the first column.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);
    // Expected state: column became selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 1, 18));

    // 3. Drag the selection with mouse to 5 bases to the right.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 0));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(5, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    // Expected state: alignment moved to 5 bases to the right.

    // 4. Drag the selection with mouse to one base to the left.
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(4, 0));
    GTMouseDriver::release();
    GTThread::waitForMainThread();
    // Expected state: alignment  moved to one bases to the left.

    // Check results
    int newLength = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(4 == newLength - oldLength, QString("Wrong length of changed alignment"));
}

GUI_TEST_CLASS_DEFINITION(bookmark_test_0001) {
    //     1. Open document samples\CLUSTALW\COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    //     2. Create bookmark. Rename "New bookmark" to "start bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "start bookmark");

    int startRO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    int startLO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    QWidget* mdiWindow = GTUtilsMdi::activeWindow();

    //     3. Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(300));
    GTUtilsDialog::waitForDialog(new PopupChooser(QStringList() << MSAE_MENU_NAVIGATION << "action_go_to_position"));
    GTMenu::showContextMenu(mdiWindow);

    //     4. Create bookmark. Rename "New bookmark" to "middle bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "middle bookmark");

    const int midLO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    //     5. Scroll msa to the end.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(550));
    GTUtilsDialog::waitForDialog(new PopupChooser(QStringList() << MSAE_MENU_NAVIGATION << "action_go_to_position"));
    GTMenu::showContextMenu(mdiWindow);

    //     6. Create bookmark. Rename "New bookmark" to "end bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "end bookmark");

    const int endLO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    //     Expected state: clicking on each bookmark will recall corresponding MSA position
    GTUtilsBookmarksTreeView::doubleClickBookmark("start bookmark");
    GTUtilsTaskTreeView::waitTaskFinished();

    int RO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    int LO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(startRO == RO && startLO == LO, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark("middle bookmark");
    GTUtilsTaskTreeView::waitTaskFinished();

    RO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    LO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(midLO == LO, QString("middle bookmark offsets aren't equal to the expected: midLO=%1 LO=%2").arg(midLO).arg(LO));
    GTUtilsBookmarksTreeView::doubleClickBookmark("end bookmark");
    GTUtilsTaskTreeView::waitTaskFinished();

    RO = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    LO = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    // CHECK_SET_ERR(endRO == RO && endLO == LO, "end bookmark offsets aren't equal to the expected");
    CHECK_SET_ERR(endLO == LO, QString("end bookmark offsets aren't equal to the expected: endLO=%3 LO=%4").arg(endLO).arg(LO));
    //     7. Delete Start bookmark
    GTUtilsBookmarksTreeView::deleteBookmark("start bookmark");

    //     Expected state: start bookmark isn't present
    QTreeWidgetItem* startBookmark = GTUtilsBookmarksTreeView::findItem("start bookmark", {false});
    CHECK_SET_ERR(startBookmark == nullptr, "Start bookmark is not deleted");
}

GUI_TEST_CLASS_DEFINITION(bookmark_test_0002) {  // CHANGES: default names used
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // Create a bookmark. Do not rename the new bookmark.
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]");

    int startBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    int startBookmarkLastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();

    // Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(300));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Create a bookmark. Do not rename the new bookmark.
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]");

    int middleBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Scroll msa to the end.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(550));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Create bookmark. Do not rename the new bookmark..
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]");
    int endBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Expected state: click on each bookmark sets corresponding MSA position.
    GTUtilsBookmarksTreeView::doubleClickBookmark("New bookmark");
    int firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    int lastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(firstBase == startBookmarkFirstBase, QString("Start bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(startBookmarkFirstBase));
    CHECK_SET_ERR(lastBase == startBookmarkLastBase, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark("New bookmark 2");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBase == middleBookmarkFirstBase, QString("Middle bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(middleBookmarkFirstBase));

    GTUtilsBookmarksTreeView::doubleClickBookmark("New bookmark 3");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBase == endBookmarkFirstBase, QString("End bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(endBookmarkFirstBase));

    // Delete Start bookmark & check it was deleted.
    GTUtilsBookmarksTreeView::deleteBookmark("New bookmark");
    QTreeWidgetItem* startBookmark = GTUtilsBookmarksTreeView::findItem("New bookmark", {false});
    CHECK_SET_ERR(startBookmark == nullptr, "Start bookmark wasn't deleted");
}

GUI_TEST_CLASS_DEFINITION(bookmark_test_0003) {
    // CHANGES: mid and end coordinates changed.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // Create a bookmark. Rename "New bookmark" to "start bookmark".
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "start bookmark");
    int startBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    int startBookmarkLastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();

    //  Scroll msa to the middle.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(200));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Create a bookmark. Rename "New bookmark" to "middle bookmark"
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "middle bookmark");
    int middleBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Scroll msa to the end.
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(510));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "action_go_to_position"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Create bookmark. Rename "New bookmark" to "end bookmark".
    GTUtilsBookmarksTreeView::addBookmark("COI [COI.aln]", "end bookmark");
    int endBookmarkFirstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();

    // Expected state: click on each bookmark sets corresponding MSA position.
    GTUtilsBookmarksTreeView::doubleClickBookmark("start bookmark");
    int firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    int lastBase = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex();
    CHECK_SET_ERR(firstBase == startBookmarkFirstBase, QString("Start bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(startBookmarkFirstBase));
    CHECK_SET_ERR(lastBase == startBookmarkLastBase, "start bookmark offsets aren't equal to the expected");

    GTUtilsBookmarksTreeView::doubleClickBookmark("middle bookmark");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBase == middleBookmarkFirstBase, QString("Middle bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(middleBookmarkFirstBase));

    GTUtilsBookmarksTreeView::doubleClickBookmark("end bookmark");
    firstBase = GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex();
    CHECK_SET_ERR(firstBase == endBookmarkFirstBase, QString("End bookmark first base offset does not match: %1 vs %2").arg(firstBase).arg(endBookmarkFirstBase));
}

GUI_TEST_CLASS_DEFINITION(exclude_list_test_0001) {
    // Check initial state of MSA editor & Exclude list with no selection.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    auto toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open Exclude List.
    GTWidget::click(toggleExcludeListButton);

    // Check Exclude List state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText("Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton("exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "COI.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton("exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton("exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveFromMsaButton = GTWidget::findToolButton("exclude_list_move_from_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget("exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit("exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
}

GUI_TEST_CLASS_DEFINITION(exclude_list_test_0002) {
    // Check initial state of MSA editor & Exclude list with no selection.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
    CHECK_SET_ERR(excludeListWidget == nullptr, "Exclude list must not be opened by default");

    GTUtilsDialog::waitForDialog(new PopupChecker({MSAE_MENU_COPY, "exclude_list_move_from_msa_action"}, PopupChecker::IsDisabled));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea());

    auto toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(toolbar, "exclude_list_toggle_action");
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked by default");

    // Open Exclude List.
    GTWidget::click(toggleExcludeListButton);

    // Check Exclude List state.
    CHECK_SET_ERR(toggleExcludeListButton->isChecked(), "Toggle exclude list button must be checked");
    excludeListWidget = GTWidget::findWidget("msa_exclude_list", msaEditorWindow);

    GTWidget::findLabelByText("Exclude list is empty", excludeListWidget);

    // Check buttons.
    auto selectFileButton = GTWidget::findToolButton("exclude_list_select_file_button", excludeListWidget);
    CHECK_SET_ERR(selectFileButton->text() == "ty3.exclude-list.fasta", "Invalid select file button text: " + selectFileButton->text());
    CHECK_SET_ERR(selectFileButton->isEnabled(), "selectFileButton must be enabled");

    auto saveAsButton = GTWidget::findToolButton("exclude_list_save_as_button", excludeListWidget);
    CHECK_SET_ERR(saveAsButton->isEnabled(), "saveAsButton must be enabled");

    auto moveToMsaButton = GTWidget::findToolButton("exclude_list_move_to_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveToMsaButton->isEnabled(), "moveToMsa button must not be enabled");

    auto moveFromMsaButton = GTWidget::findToolButton("exclude_list_move_from_msa_button", excludeListWidget);
    CHECK_SET_ERR(!moveFromMsaButton->isEnabled(), "moveToExcludeList button must not be enabled");

    auto nameListArea = GTWidget::findListWidget("exclude_list_name_list_widget", excludeListWidget);
    CHECK_SET_ERR(nameListArea->isVisible(), "Name list must be visible");

    auto sequenceViewArea = GTWidget::findPlainTextEdit("exclude_list_sequence_view", excludeListWidget);
    CHECK_SET_ERR(sequenceViewArea->isVisible(), "Sequence area must be visible");

    // Hide exclude list.
    GTWidget::click(toggleExcludeListButton);
    CHECK_SET_ERR(!toggleExcludeListButton->isChecked(), "Toggle exclude list button must not be checked");
    GTWidget::findWidget("msa_exclude_list", msaEditorWindow, false);
}

GUI_TEST_CLASS_DEFINITION(tree_test_0001) {
    // Check that after a tree is built and sync mode is enabled the MaEditorRowOrder::Free mode is enabled:
    //  - 'toggle_sequence_row_order_action' is unchecked.
    //  - 'refresh_sequence_row_order_action' is disabled.
    //  - sequences are ordered by the tree.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    QStringList originalSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    GTUtilsProjectTreeView::toggleView();  // Close project view to make all actions on toolbar available.

    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "msa_editor_tree_test_0002", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList byTreeSequenceNames1 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    QAbstractButton* toggleSequenceOrderButton = GTAction::button("toggle_sequence_row_order_action");
    QAbstractButton* refreshSequenceOrderButton = GTAction::button("refresh_sequence_row_order_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/1");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/1");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/1");
    CHECK_SET_ERR(originalSequenceNames1 != byTreeSequenceNames1, "MSA must be re-ordered by tree");

    // Switch to the 'Sequence' mode.
    GTWidget::click(toggleSequenceOrderButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/1");
    CHECK_SET_ERR(toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be checked/1");
    CHECK_SET_ERR(refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be enabled/1");

    // Switch to the 'Original' mode.
    GTWidget::click(toggleSequenceOrderButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList originalSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(!syncModeButton->isChecked(), "Sync mode must be OFF/2");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/2");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/2");
    CHECK_SET_ERR(originalSequenceNames1 == originalSequenceNames2, "original sequence name list is not restored");

    // Switch back to the order-by-tree (MaEditorRowOrder::Free) mode.
    GTWidget::click(syncModeButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList byTreeSequenceNames2 = GTUtilsMSAEditorSequenceArea::getVisibleNames();
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/2");
    CHECK_SET_ERR(!toggleSequenceOrderButton->isChecked(), "toggleSequenceOrderButton must be unchecked/3");
    CHECK_SET_ERR(!refreshSequenceOrderButton->isEnabled(), "refreshSequenceOrderButton must be disabled/3");
    CHECK_SET_ERR(byTreeSequenceNames1 == byTreeSequenceNames2, "by-tree sequence name list is not restored");
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0001) {
    // 1. Open an alignment in the Alignment Editor.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 2. Select one character.
    // Expected result : the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(9, 9), QPoint(9, 9));

    // 3. Open the main menu in the sequence area.
    // Expected result : the menu contains an item "Actions > Edit > Replace character".The item is enabled.A hotkey Shift + R is shown nearby.
    // 4. Select the item.
    // Expected result : the character is selected in the replacement mode.
    GTMenu::clickMainMenuItem({"Actions", "Edit", "Replace selected character"}, GTGlobals::UseMouse);

    // 5. Press a key on the keyboard with another character of the same alphabet(e.g C key).
    // Expected result : the original character of the alignment was replaced with the new one(e.g 'A' was replaced with 'C').Selection is in normal mode.
    GTKeyboardDriver::keyClick('g');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR(selectionContent == "G", QString("Incorrect selection content: expected - %1, received - %2").arg("G").arg(selectionContent));
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0002) {
    // 1. Open an alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 2. Select a gap after the last sequence character.
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(603, 9));

    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTKeyboardDriver::keyClick(Qt::Key_Up);

    // 3. Replace the gap
    // Expected result : the gap is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);
    GTKeyboardDriver::keyClick('c');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR(selectionContent == "C", QString("Incorrect selection content: expected - %1, received - %2").arg("C").arg(selectionContent));
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0003) {
    // 1. Open an alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 2. Select a gap before the first sequence character.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 9), QPoint(0, 9));

    // 3. Replace the gap
    // Expected result : the gap is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick('c');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR(selectionContent == "C", QString("Incorrect selection content: expected - %1, received - %2").arg("C").arg(selectionContent));
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0004) {
    // 1. Open an alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 2. Select a last character in sequence.
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(603, 9));

    // 3. Replace the character
    // Expected result : the character is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick('c');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    const QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR(selectionContent == "C", QString("Incorrect selection content: expected - %1, received - %2").arg("C").arg(selectionContent));
}

GUI_TEST_CLASS_DEFINITION(replace_character_test_0005) {
    // 1. Open an alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 2. Select a character in sequence.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 9), QPoint(0, 9));

    // 3. Replace the character to gap by space key
    // Expected result : the character is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick(Qt::Key_Space);

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString selectionContent = GTClipboard::text();
    CHECK_SET_ERR(selectionContent == "-", QString("Incorrect selection content: expected - %1, received - %2").arg("-").arg(selectionContent));

    // 3. Select a character in sequence.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 10), QPoint(0, 10));

    // 4. Replace the character to gap by '-' key
    // Expected result : the character is successfully replaced.
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);

    GTKeyboardDriver::keyClick('-');

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    selectionContent = GTClipboard::text();
    CHECK_SET_ERR(selectionContent == "-", QString("Incorrect selection content: expected - %1, received - %2").arg("-").arg(selectionContent));
}

GUI_TEST_CLASS_DEFINITION(edit_test_0001) {
    // 1. Open document "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(true);

    // 2. Select area from (0, 0) to (10, 10)
    GTUtilsMSAEditorSequenceArea::selectArea({0, 0}, {10, 10});

    // 3. Click "Replace with gaps" with popup menu
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "replace_with_gaps"}));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    // Expected: selection hasn't been changed
    auto check = [&]() {
        auto selectedRect = GTUtilsMSAEditorSequenceArea::getSelectedRect();
        auto tl = selectedRect.topLeft();
        CHECK_SET_ERR(tl.x() == 11 && tl.y() == 0, QString("Expected top-left selection: 11, 0; current: %1, %2").arg(tl.x()).arg(tl.y()));

        auto br = selectedRect.bottomRight();
        CHECK_SET_ERR(br.x() == 21 && br.y() == 10, QString("Expected bottom-right selection: 21, 0; current: %1, %2").arg(br.x()).arg(br.y()));

        const QStringList GAPPED_DATA = {
            "-----------ATTCGAGCCGA",
            "-----------ATCCGGGCCGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCAGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGAGCCGA",
            "-----------ATCCGGGCTGA",
            "-----------ATTCGAGCTGA",
            "-----------ATTCGTGCTGA"};

        GTUtilsMSAEditorSequenceArea::selectArea({0, 0}, {22, 10});
        GTKeyboardUtils::copy();
        auto result = GTClipboard::text().split("\n");
        for (int i = 0; i < 11; i++) {
            CHECK_SET_ERR(result[i].startsWith(GAPPED_DATA[i]), QString("Expected sequence beginning: %1; current: %2").arg(GAPPED_DATA[i]).arg(result[i].left(20)));
        }
    };
    check();

    // 4. Click undo
    GTWidget::click(GTAction::button("msa_action_undo"));
    GTWidget::click(GTAction::button("msa_action_undo"));

    // 5. Select area from (0, 0) to (10, 10)
    GTUtilsMSAEditorSequenceArea::selectArea({0, 0}, {10, 10});

    // 6. Click "Replace with gaps" with shortcut
    GTKeyboardDriver::keyClick(Qt::Key_Space, Qt::ShiftModifier);

    // Expected: selection hasn't been changed
    check();
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline
}  // namespace U2
