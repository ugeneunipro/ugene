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
#include <U2View/BaseWidthController.h>
#include <U2View/RowHeightController.h>

namespace U2 {

namespace GUITest_common_scenarios_MSA_editor_multiline {
using namespace HI;

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
        MaEditorWgt* w = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(i);
        if (w == nullptr)
            break;
        lastWgt = w;
        lastWgtIndex = i;
    }
    CHECK_SET_ERR(lastWgt != nullptr, QString("Can't find any sequence area"));
    CHECK_SET_ERR(lastWgtIndex > 1, QString("Not in multiline mode"));

    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(0, 0), QPoint(1, 2));
}

}  // namespace GUITest_common_scenarios_MSA_editor_multiline
}  // namespace U2
