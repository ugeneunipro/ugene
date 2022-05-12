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

#include "GTTestsMSAMultiline.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline {
using namespace HI;

namespace {
void enumerateMenu(QMenu *menu, QList<QString> *textItems)
{
    foreach (QAction *action, menu->actions()) {
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
}

GUI_TEST_CLASS_DEFINITION(general_test_0001)
{
    // UGENE-7042

    // 1. Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 3. Find seq area #1
    auto w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 1);
    CHECK_SET_ERR(w != nullptr, QString("Can't find sequence area #1"));

    // 4. Switch to multiline mode
    // Press "Multiline View" button on toolbar
    mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 5. Find seq area #0, but not #1
    w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);
    CHECK_SET_ERR(w != nullptr, QString("Can't find sequence area #0"));
    w = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 1);
    CHECK_SET_ERR(w == nullptr, QString("Unexpectedly found sequence area #1"));
}

GUI_TEST_CLASS_DEFINITION(zoom_to_selection_test_0001)
{
    // UGENE-7605

    const QString seqName = "Phaneroptera_falcata";

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QAbstractButton* reset_zoom = GTAction::button(os, "Reset Zoom");
    GTWidget::click(os, reset_zoom);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Select seq.
    GTUtilsMsaEditor::selectRowsByName(os, {seqName});

    reset_zoom = GTAction::button(os, "Reset Zoom");
    QAbstractButton* zoom_to_sel = GTAction::button(os, "Zoom To Selection");
    GTWidget::click(os, zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, reset_zoom);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, zoom_to_sel);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTWidget::click(os, reset_zoom);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Must not crash
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0001)
{
    // UGENE-7522

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt *lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    GScrollBar *vscroll = qobject_cast<GScrollBar *>(
        GTWidget::findWidget(os, "multiline_vertical_sequence_scroll"));
    CHECK_SET_ERR(vscroll != nullptr, QString("Can't find vertical scroll bar"));
    CHECK_SET_ERR(vscroll->isVisible(), QString("Vertical scroll is not visible"));
    CHECK_SET_ERR(vscroll->isEnabled(), QString("Vertical scroll is disabled"));

    int minVal = vscroll->minimum();
    int maxVal = vscroll->maximum();
    int curVal = vscroll->value();
    CHECK_SET_ERR(minVal == 0 && maxVal > 600 && curVal == 0,
                  QString("Unexpected vertical scroll values min=%1, max=%2, value=%3")
                      .arg(minVal).arg(maxVal).arg(curVal));
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0002)
{
    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GScrollBar *vscroll = qobject_cast<GScrollBar *>(
        GTWidget::findWidget(os, "multiline_vertical_sequence_scroll"));
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
    MaEditorWgt *lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt *w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os, lastWgtIndex);
    CHECK_SET_ERR(lastBaseIdx == 603, QString("Not at the end"));
}

GUI_TEST_CLASS_DEFINITION(vscroll_test_0003)
{
    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GScrollBar *hscroll = qobject_cast<GScrollBar *>(
        GTWidget::findWidget(os, "multiline_horizontal_sequence_scroll"));
    CHECK_SET_ERR(hscroll != nullptr, QString("Can't find horizontal scroll bar"));
    CHECK_SET_ERR(hscroll->isVisible(), QString("Horizontal scroll is not visible"));
    CHECK_SET_ERR(hscroll->isEnabled(), QString("Horizontal scroll is disabled"));

    int minVal = hscroll->minimum();
    int maxVal = hscroll->maximum();
    int curVal = hscroll->value();
    CHECK_SET_ERR(minVal == 0 && maxVal > 600 && curVal == 0,
                  QString("Unexpected vertical scroll values min=%1, max=%2, value=%3")
                      .arg(minVal)
                      .arg(maxVal)
                      .arg(curVal));

    // Scroll to end
    hscroll->setValue(maxVal);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt *lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt *w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    int lastBaseIdx = GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(os, lastWgtIndex);
    CHECK_SET_ERR(lastBaseIdx == 603, QString("Not at the end"));
}

GUI_TEST_CLASS_DEFINITION(menu_test_0001)
{
    // UGENE-7524

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Find seq last area
    int lastWgtIndex = 0;
    MaEditorWgt *lastWgt = nullptr;
    for (int i = 0; i < 30; i++) {
        MaEditorWgt *w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
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
    QMenu *menu = GTMenu::showContextMenu(os, GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0));
    CHECK_SET_ERR(menu != nullptr, QString("No conrext menu"));

    // Check menu length
    QList<QString> allItems;
    enumerateMenu(menu, &allItems);
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(2, 2));
    QSet<QString> allSet = allItems.toSet();
    CHECK_SET_ERR(allSet.size() == allItems.size(), "Context menu contains repeated items");
}

GUI_TEST_CLASS_DEFINITION(goto_test_0001)
{
    // UGENE-7571

    // Open file data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    QAbstractButton *mmode = GTAction::button(os, "Multiline View");
    GTWidget::click(os, mmode);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check goto
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 200));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    GTUtilsDialog::waitForDialog(os, new GoToDialogFiller(os, 550));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(overview_test_0001) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

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
}

GUI_TEST_CLASS_DEFINITION(overview_test_0002) {
    //    Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

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
}

GUI_TEST_CLASS_DEFINITION(overview_test_0003) {
    //    Open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Switch to multiline mode
    // Press "Multiline View" button on toolbar
    GTUtilsMsaEditor::setMultilineMode(os, true);
    GTUtilsTaskTreeView::waitTaskFinished(os);

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
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline
}  // namespace U2
