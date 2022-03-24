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

#include "GTTestsMSAEditorOverview.h"
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QMainWindow>

#include <U2Core/AppContext.h>

#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTAction.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"

namespace U2 {

namespace GUITest_common_scenarios_msa_editor_overview {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Open "_common_data/fasta/empty.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta/empty.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    // Expected state: msa is empty, overview is pure white.
    QWidget* simpleOverview = GTWidget::findWidget(os, "msa_overview_area_simple");

    QColor c = GTWidget::getColor(os, simpleOverview, simpleOverview->rect().center());
    CHECK_SET_ERR(c.name() == "#ededed", "simple overview has wrong color. Expected: #ededed, Found: " + c.name());

    QWidget* graphOverview = GTWidget::findWidget(os, "msa_overview_area_graph");
    c = GTWidget::getColor(os, graphOverview, graphOverview->rect().center());
    CHECK_SET_ERR(c.name() == "#ededed", "graph overview has wrong color. Expected: #ededed, Found: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "10000_sequences.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    // Expected state: simple overview is hidden, graph overview is visible.
    auto simpleOverview = GTWidget::findWidget(os, "msa_overview_area_simple");
    CHECK_SET_ERR(!simpleOverview->isVisible(), "simple overview is visible");

    auto graphOverview = GTWidget::findWidget(os, "msa_overview_area_graph");
    CHECK_SET_ERR(graphOverview->isVisible(), "graph overview is visible");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    1. Open "_common_data/clustal/protein.fasta.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "protein.fasta.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Click on "Overview" button on tool bar.
    QAbstractButton* button = GTAction::button(os, "Show overview");
    GTWidget::click(os, button);
    //    Expected state: overview is hidden.
    QWidget* overview = GTWidget::findWidget(os, "msa_overview_area");
    CHECK_SET_ERR(overview != nullptr, "overview is NULL");
    CHECK_SET_ERR(!overview->isVisible(), "overview is visiable");
    //    3. Click on "Overview" button again.
    GTWidget::click(os, button);
    //    Expected state: overview is shown.
    CHECK_SET_ERR(overview->isVisible(), "overview is not visiable");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // + show/hide simple overview
    //    1. Open "_common_data/clustal/protein.fasta.aln"
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "protein.fasta.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Go to MSA Overview context menu (right click on msa overview).
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    //    3. Click "Show simple overview..."
    //    Expected state: simple overview is hidden.
    QWidget* overview = GTWidget::findWidget(os, "msa_overview_area_simple");
    CHECK_SET_ERR(overview->isVisible(), "overview is not visiable");
    //    4. Repeat steps 2-3.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area_graph"));

    //    Expected state: simple overview is shown.
    CHECK_SET_ERR(!overview->isVisible(), "overview is visiable");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Align "samples/CLUSTALW/HIV-1.aln"
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/CLUSTALW/HIV-1.aln"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    //    3. Expected state: simple overview is enabled.
    QWidget* simple = GTWidget::findWidget(os, "msa_overview_area_simple");
    CHECK_SET_ERR(simple->isVisible(), "simple overveiw is not visiable");

    //    4. Resize Ugene window to make overview area smaller.
    QMainWindow* window = AppContext::getMainWindow()->getQMainWindow();
    if (window->isMaximized()) {
        GTWidget::showNormal(os, window);
    }
    GTWidget::resizeWidget(os, window, QSize(550, 550));

    //    Expected state: at some moment simple overview is not displayed -
    //    there is a gray area with "MSA is too big for current window size. Simple overview is unavailable." text.

    // text can not be checked, check color
    QColor c = GTWidget::getColor(os, simple, simple->rect().topLeft() + QPoint(5, 5));
    CHECK_SET_ERR(c.name() == "#a0a0a4", "simple overview has wrong color. Expected: #a0a0a4, Found: " + c.name());

    const int tasksCount = GTUtilsTaskTreeView::getTopLevelTasksCount(os);
    CHECK_SET_ERR(0 == tasksCount, "An unexpected task is running. Ensure that the overview is not calculating");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));
    //    2. Resize main window.
    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    QImage imgSimple1 = GTWidget::getImage(os, overviewSimple);

    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");
    QImage imgGraph1 = GTWidget::getImage(os, overviewGraph);

    QMainWindow* window = AppContext::getMainWindow()->getQMainWindow();
    GTWidget::showNormal(os, window);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QImage imgSimple2 = GTWidget::getImage(os, overviewSimple);
    QImage imgGraph2 = GTWidget::getImage(os, overviewGraph);

    CHECK_SET_ERR(imgSimple1 != imgSimple2, "simple overview not updated");
    CHECK_SET_ERR(imgGraph1 != imgGraph2, "graph overview not updated");
    //    Expected state: msa overview is updating according to current window size.
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));
    //    2. Delete symbols until msa becomes very small.
    //    Expected state: overview updates on each msa change, overview is displayed correctly .
    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");

    // Close Project view for small screens
    GTKeyboardDriver::keyClick('1', Qt::AltModifier);

    for (int i = 0; i < 12; i++) {
        // saving overviews' images
        QImage imgSimple1 = GTWidget::getImage(os, overviewSimple);
        QImage imgGraph1 = GTWidget::getImage(os, overviewGraph);

        GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(40, 17));
        GTKeyboardDriver::keyClick(Qt::Key_Delete);

        // checking images changed
        QImage imgSimple2 = GTWidget::getImage(os, overviewSimple);
        QImage imgGraph2 = GTWidget::getImage(os, overviewGraph);

        CHECK_SET_ERR(imgSimple1 != imgSimple2, "simple overview not updated");
        CHECK_SET_ERR(imgGraph1 != imgGraph2, "graph overview not updated")
    }
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");

    // saving overviews' images
    QImage imgSimple1 = GTWidget::getImage(os, overviewSimple);

    //    2. Select some area in msa view and move it with mouse.
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(10, 10));
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(5, 5));
    GTMouseDriver::press();
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(15, 5));

    //    Expected state: while mouse button is pressed graph overview is blocked. On mouse release overview updating starts.
    //    Simple overview updates simultaneously.
    // checking simple overview image changed
    QImage imgSimple2 = GTWidget::getImage(os, overviewSimple);

    CHECK_SET_ERR(imgSimple1 != imgSimple2, "simple overview not updated");

#ifdef Q_OS_WIN
    const QPoint point(6, 6);
#else
    const QPoint point(overviewGraph->rect().center() - QPoint(0, 20));
#endif
    const QColor c = GTWidget::getColor(os, overviewGraph, point);
    CHECK_SET_ERR(c.name() == "#a0a0a4", "simple overview has wrong color. Expected: #a0a0a4, Found: " + c.name());

    GTMouseDriver::release();
    GTThread::waitForMainThread();
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");

    // saving overviews' images
    QImage imageBefore = GTWidget::getImage(os, overviewSimple);

    //    2. Select one symbol.
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(5, 5));
    GTMouseDriver::click();

    //    3. Press Delete button and release it after a while.
    GTKeyboardDriver::keyPress(Qt::Key_Delete);
    GTKeyboardDriver::keyRelease(Qt::Key_Delete);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QImage imageAfter = GTWidget::getImage(os, overviewSimple);
    CHECK_SET_ERR(imageBefore != imageAfter, "simple overview not updated");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");

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
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //    1. Open "_common_data/phylip/seq_protein.ph".
    GTFileDialog::openFile(os, testDir + "_common_data/phylip/seq_protein.ph");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // saving overview image
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");
    const QImage img = GTWidget::getImage(os, overviewGraph);

    //    2. Go to MSA Overview context menu (right click on msa overview).
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Display settings", "Orientation", "Top to bottom"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    //    3. Go to {Display settings... -> Orientation}
    //    4. Change graph orientation.
    //    Expected state: y-axis changes its orientation. No calculation task starts.
    const QImage img1 = GTWidget::getImage(os, overviewGraph);
    CHECK_SET_ERR(img1 != img, "overview not inverted");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    //    1. Open "_common_data/stockholm/5_msa.sto".
    GTFileDialog::openFile(os, testDir + "_common_data/stockholm/5_msa.sto");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Go to MSA Overview context menu (right click on msa overview).
    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, 255, 0, 0));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Display settings", "Set color"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area_graph"));

    //    3. Go to {Display settings -> Color}.
    //    Expected state: color dialog appears.
    //    4. Chenge current color.
    //    Expected state: graph color had changed.
    QWidget* graph = GTWidget::findWidget(os, "msa_overview_area_graph");
    const QColor c = GTWidget::getColor(os, graph, QPoint(5, graph->rect().height() - 5));
    CHECK_SET_ERR(c.name() == "#eda2a2", "simple overview has wrong color. Expected: #eda2a2, Found: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //    1. Open "_common_data/stockholm/5_msa.sto".
    GTFileDialog::openFile(os, testDir + "_common_data/stockholm/5_msa.sto");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Go to MSA Overview context menu (right click on msa overview).
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Display settings", "Graph type", "Line graph"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area_graph"));

    //    3. Go to {Display settings -> Graph type};
    //    4. Change selected type.
    //    Expected state: graph type had changed.
    QWidget* graph = GTWidget::findWidget(os, "msa_overview_area_graph");
    const QColor c = GTWidget::getColor(os, graph, QPoint(5, graph->rect().height() - 5));
    CHECK_SET_ERR(c.name() == "#ededed", "simple overview has wrong color. Expected: #ededed, Found: " + c.name());
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    //    1. Open "_common_data/clustal/COI_na.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal", "COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));
    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    //    2. Go to Highlighting tab on Options panel.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    auto combo = GTWidget::findComboBox(os, "colorScheme");
    GTComboBox::selectItemByText(os, combo, "No colors");

    //    3. Change Color Scheme.
    const QColor c = GTUtilsMsaEditor::getSimpleOverviewPixelColor(os, QPoint(5, overviewSimple->rect().height() - 5));
    CHECK_SET_ERR(c.name() == "#ededed", "simple overview has wrong color. Expected: #ededed, Found: " + c.name());
    //    Expected state: in simple msa overview color scheme was changed.
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/COI na.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Go to MSA overview context menu (right click on MSA Overview).
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Calculation method", "Gaps"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, overviewGraph);

    // Select {Calculation method -> Strict}.
    // Expected state: graph displays the percent of the most frequent nucleotide in column.
    // Current graph corresponds to column over the consensus in sequence area.
    QColor color = GTWidget::getColor(os, overviewGraph, QPoint(5, overviewGraph->rect().height() - 5));
    CHECK_SET_ERR(color.name() == "#ededed", "simple overview has wrong color. Expected: #ededed, Found: " + color.name());

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Calculation method", "Highlighting"}, GTGlobals::UseKey));
    GTMenu::showContextMenu(os, overviewGraph);

    color = GTWidget::getColor(os, overviewGraph, QPoint(5, overviewGraph->rect().height() - 5));
    CHECK_SET_ERR(color.name() == "#d1d1d2", "simple overview has wrong color. Expected: #d1d1d2, Found: " + color.name());
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Go to MSA overview context menu (right click on MSA Overview).
    // Select {Calculation method -> Gaps}.
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Calculation method", "Gaps"}));
    GTMenu::showContextMenu(os, overviewGraph);

    // Expected state: graph overview displays percent of gaps in each column.
    // Save graphView.
    const QImage img = GTWidget::getImage(os, overviewGraph);

    //    4. Go to Highlighting tab on Options panel.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));

    //    5. Select Highlighting to "Gaps"
    auto combo = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, combo, "Gaps");

    //    6. Go to MSA overview context menu (right click on MSA Overview).
    //    7. Select {Calculation method -> Highlighting}.
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Calculation method", "Highlighting"}));
    GTMenu::showContextMenu(os, overviewGraph);
    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));

    //    Expected state: graph didn't change.
    const QImage img1 = GTWidget::getImage(os, overviewGraph);
    CHECK_SET_ERR(img == img1, "overview changed");
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    // 0019 and 0020 scenarios
    //     1. Open "_common_data/CLUSLAL/HIV_1.aln"
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Go to MSA overview context menu (right click on MSA Overview).
    QWidget* overviewGraph = GTWidget::findWidget(os, "msa_overview_area_graph");
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Calculation method", "Highlighting"}));
    GTMenu::showContextMenu(os, overviewGraph);

    //    3. Select {Calculation method -> Highlighting}.
    //    4. Go to Highlighting tab on Options panel.
    // save grahpView
    const QImage img = GTWidget::getImage(os, overviewGraph);

    GTWidget::click(os, GTWidget::findWidget(os, "OP_MSA_HIGHLIGHTING"));
    GTUtilsMSAEditorSequenceArea::selectSequence(os, "sf170");
    GTWidget::click(os, GTWidget::findWidget(os, "addSeq"));

    //    5. Change Highlighting.
    auto combo = GTWidget::findComboBox(os, "highlightingScheme");
    GTComboBox::selectItemByText(os, combo, "Agreements");

    //    Expected state: graph displays percent of highlighted cells in column.
    // save grahpView
    const QImage img1 = GTWidget::getImage(os, overviewGraph);
    CHECK_SET_ERR(img != img1, "overview not changed");

    //    Overview changes after each highlighting change.
    GTComboBox::selectItemByText(os, combo, "Disagreements");
    const QImage img2 = GTWidget::getImage(os, overviewGraph);
    CHECK_SET_ERR(img != img2, "overview not changed");
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    /* 1. Open "_common_data/regression/1393/test_1393.aln".
     * 2. Show simple overview.
     * 3. Select whole alignment.
     * Expected state: whole simple overview is filled with a selection rect.
     * 4. Click "align_new_sequences_to_alignment_action" button on the toolbar and select "data/samples/fastq/eas.fastq".
     * Expected state: sequences are added, two of five sequences are selected both in the sequence area and simple overview.
     * Current state: sequences are added, two of five sequences are selected in the sequence area, but the simple overview is filled with a selection rect like whole alignment is selected.
     */

    GTFileDialog::openFile(os, testDir + "_common_data/regression/1393/test_1393.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    GTUtilsMSAEditorSequenceArea::selectArea(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/FASTQ/eas.fastq"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(os, "MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    const QColor color = GTWidget::getColor(os, overviewSimple, overviewSimple->geometry().topRight() - QPoint(5, -5));
    CHECK_SET_ERR(color.name() == "#ededed", "simple overview has wrong color. Expected: #ededed, found: " + color.name());
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    /* 1. Open "_common_data/regression/1393/test_1393.aln".
     * 2. Select whole alignment.
     * 3. Show simple overview.
     * Expected state: whole simple overview is filled with a selection rect.
     * Current state: selection not showed.
     */

    GTFileDialog::openFile(os, testDir + "_common_data/regression/1393/test_1393.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMSAEditorSequenceArea::selectArea(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    const QColor color = GTWidget::getColor(os, overviewSimple, overviewSimple->geometry().topRight() - QPoint(5, -5));
    CHECK_SET_ERR(color.name() == "#7eaecc", "simple overview has wrong color. Expected: #7eaecc, Found: " + color.name());
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    /* 1. Open "_common_data/regression/1393/test_1393.aln".
     * 2. Open ProjectView if it closed
     * 3. Select whole alignment.
     * 4. Show simple overview.
     * Expected state: whole simple overview is filled with a selection rect.
     * 5. Close ProjectView
     * Expected state: whole simple overview is filled with a selection rect.
     * Current state: selection is not full.
     */

    GTFileDialog::openFile(os, testDir + "_common_data/regression/1393/test_1393.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::openView(os);

    GTUtilsMSAEditorSequenceArea::selectArea(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Show simple overview"}));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "msa_overview_area"));

    GTUtilsProjectTreeView::toggleView(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* overviewSimple = GTWidget::findWidget(os, "msa_overview_area_simple");
    const QColor color = GTWidget::getColor(os, overviewSimple, overviewSimple->geometry().topRight() - QPoint(5, -5));
    CHECK_SET_ERR(color.name() == "#7eaecc", "simple overview has wrong color. Expected: #7eaecc, Found: " + color.name());
}

}  // namespace GUITest_common_scenarios_msa_editor_overview
}  // namespace U2
