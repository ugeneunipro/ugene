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

#include <QLabel>
#include <QTextDocument>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>

#include <U2Core/U2SafePoints.h>

#include <U2View/McaEditorConsensusArea.h>
#include <U2View/McaEditor.h>
#include <U2View/McaEditorNameList.h>
#include <U2View/McaEditorReferenceArea.h>
#include <U2View/McaEditorSequenceArea.h>
#include <U2View/McaEditorWgt.h>
#include <U2View/MSAEditorOffsetsView.h>
#include <U2View/RowHeightController.h>
#include <U2View/ScrollController.h>

#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMdi.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMcaEditor"

#define GT_METHOD_NAME "getEditor"
McaEditor *GTUtilsMcaEditor::getEditor(GUITestOpStatus &os) {
    McaEditorWgt *editorUi = getEditorUi(os);
    CHECK_OP(os, NULL);
    McaEditor *editor = editorUi->getEditor();
    GT_CHECK_RESULT(NULL != editor, "MCA Editor is NULL", NULL);
    return editor;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditorUi"
McaEditorWgt *GTUtilsMcaEditor::getEditorUi(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);
    McaEditorWgt *mcaEditorWgt = activeWindow->findChild<McaEditorWgt *>();
    GT_CHECK_RESULT(NULL != mcaEditorWgt, "MCA Editor widget is NULL", NULL);
    return mcaEditorWgt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceLabel"
QLabel *GTUtilsMcaEditor::getReferenceLabel(GUITestOpStatus &os) {
    QWidget *referenceLabelContainerWidget = GTWidget::findExactWidget<QWidget *>(os, "reference label container widget", getEditorUi(os));
    GT_CHECK_RESULT(NULL != referenceLabelContainerWidget, "Reference label not found", NULL);
    return GTWidget::findExactWidget<QLabel *>(os, "", referenceLabelContainerWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameListArea"
McaEditorNameList *GTUtilsMcaEditor::getNameListArea(GUITestOpStatus &os) {
    return GTWidget::findExactWidget<McaEditorNameList *>(os, "mca_editor_name_list", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceArea"
McaEditorSequenceArea *GTUtilsMcaEditor::getSequenceArea(GUITestOpStatus &os) {
    return GTWidget::findExactWidget<McaEditorSequenceArea *>(os, "mca_editor_sequence_area", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusArea"
McaEditorConsensusArea* GTUtilsMcaEditor::getConsensusArea(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);
    return GTWidget::findExactWidget<McaEditorConsensusArea*>(os, "consArea", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceArea"
McaEditorReferenceArea *GTUtilsMcaEditor::getReferenceArea(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);
    return GTWidget::findExactWidget<McaEditorReferenceArea*>(os, "mca_editor_reference_area", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHorizontalScrollBar"
QScrollBar *GTUtilsMcaEditor::getHorizontalScrollBar(GUITestOpStatus &os) {
    return GTWidget::findExactWidget<QScrollBar *>(os, "horizontal_sequence_scroll", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getVerticalScrollBar"
QScrollBar *GTUtilsMcaEditor::getVerticalScrollBar(GUITestOpStatus &os) {
    return GTWidget::findExactWidget<QScrollBar *>(os, "vertical_sequence_scroll", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMcaRow"
MultipleAlignmentRowData* GTUtilsMcaEditor::getMcaRow(GUITestOpStatus &os, int rowNum) {
    McaEditor* mcaEditor = GTUtilsMcaEditor::getEditor(os);
    GT_CHECK_RESULT(NULL != mcaEditor, "McaEditor not found", NULL);

    MultipleChromatogramAlignmentObject* maObj = mcaEditor->getMaObject();
    GT_CHECK_RESULT(NULL != maObj, "MultipleChromatogramAlignmentObject not found", NULL);

    MultipleAlignmentRow row = maObj->getRow(rowNum);

    return row.data();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOffsetAction"
QAction* GTUtilsMcaEditor::getOffsetAction(GUITestOpStatus &os) {
    McaEditorWgt* editorWgt = GTUtilsMcaEditor::getEditorUi(os);
    GT_CHECK_RESULT(editorWgt != NULL, "McaEditorWgt not found", NULL);

    MSAEditorOffsetsViewController* offsetController = editorWgt->getOffsetsViewController();
    GT_CHECK_RESULT(offsetController != NULL, "MSAEditorOffsetsViewController is NULL", NULL);
    return offsetController->getToggleColumnsViewAction();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceLabelText"
QString GTUtilsMcaEditor::getReferenceLabelText(GUITestOpStatus &os) {
    QLabel *referenceLabel = getReferenceLabel(os);
    GT_CHECK_RESULT(NULL != referenceLabel, "Reference label is NULL", "");
    if (referenceLabel->textFormat() != Qt::PlainText) {
        QTextDocument textDocument;
        textDocument.setHtml(referenceLabel->text());
        return textDocument.toPlainText();
    } else {
        return referenceLabel->text();
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadsCount"
int GTUtilsMcaEditor::getReadsCount(GUITestOpStatus &os) {
    QWidget *statusBar = GTWidget::findWidget(os, "mca_editor_status_bar", getEditorUi(os));
    QLabel *readsCountLabel = GTWidget::findExactWidget<QLabel *>(os, "Line", statusBar);

    QRegExp readsCounRegExp("Ln \\d+|\\- / (\\d+)");
    readsCounRegExp.indexIn(readsCountLabel->text());
    const QString totalReadsCountString = readsCounRegExp.cap(1);

    bool isNumber = false;
    const int totalReadsCount =  totalReadsCountString.toInt(&isNumber);
    GT_CHECK_RESULT(isNumber, QString("Can't convert the reads count string to number: %1").arg(totalReadsCountString), -1);

    return totalReadsCount;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadsNames"
const QStringList GTUtilsMcaEditor::getReadsNames(GUITestOpStatus &os) {
    return getEditor(os)->getMaObject()->getMultipleAlignment()->getRowNames();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDirectReadsNames"
const QStringList GTUtilsMcaEditor::getDirectReadsNames(GUITestOpStatus &os) {
    QStringList directReadsNames;
    MultipleChromatogramAlignmentObject *mcaObject = getEditor(os)->getMaObject();
    const int rowsCount = mcaObject->getNumRows();
    for (int i = 0; i < rowsCount; i++) {
        if (!mcaObject->getMcaRow(i)->isReversed()) {
            directReadsNames << mcaObject->getMcaRow(i)->getName();
        }
    }
    return directReadsNames;

}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReverseComplementReadsNames"
const QStringList GTUtilsMcaEditor::getReverseComplementReadsNames(GUITestOpStatus &os) {
    QStringList reverseComplementedReadsNames;
    MultipleChromatogramAlignmentObject *mcaObject = getEditor(os)->getMaObject();
    const int rowsCount = mcaObject->getNumRows();
    for (int i = 0; i < rowsCount; i++) {
        if (mcaObject->getMcaRow(i)->isReversed()) {
            reverseComplementedReadsNames << mcaObject->getMcaRow(i)->getName();
        }
    }
    return reverseComplementedReadsNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadNameRect"
QRect GTUtilsMcaEditor::getReadNameRect(GUITestOpStatus &os, const QString &readName) {
    McaEditorNameList *nameList = getNameListArea(os);
    GT_CHECK_RESULT(NULL != nameList, "McaEditorNameList not found", QRect());

    const QStringList names = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    const int rowNumber = names.indexOf(readName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(readName), QRect());
    return getReadNameRect(os, rowNumber);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadNameRect"
QRect GTUtilsMcaEditor::getReadNameRect(GUITestOpStatus &os, int rowNumber) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(rowNumber), QRect());

    McaEditorNameList *nameList = getNameListArea(os);
    GT_CHECK_RESULT(NULL != nameList, "McaEditorNameList not found", QRect());

    const U2Region rowScreenRange = getEditorUi(os)->getRowHeightController()->getScreenYRegionByViewRowIndex(rowNumber);
    return QRect(nameList->mapToGlobal(QPoint(0, rowScreenRange.startPos)), nameList->mapToGlobal(QPoint(nameList->width(), rowScreenRange.endPos())));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToRead"
void GTUtilsMcaEditor::scrollToRead(GUITestOpStatus &os, const QString &readName) {
    scrollToRead(os, readName2readNumber(os, readName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToRead"
void GTUtilsMcaEditor::scrollToRead(GUITestOpStatus &os, int readNumber) {
    McaEditorWgt *mcaEditorWgt = getEditorUi(os);
    const U2Region rowRange = mcaEditorWgt->getRowHeightController()->getGlobalYRegionByViewRowIndex(readNumber);
    CHECK(!U2Region(mcaEditorWgt->getScrollController()->getScreenPosition().y(), getNameListArea(os)->height()).contains(rowRange), );
    GTScrollBar::moveSliderWithMouseToValue(os, getVerticalScrollBar(os), rowRange.center() - mcaEditorWgt->getSequenceArea()->height() / 2);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToReadName"
void GTUtilsMcaEditor::moveToReadName(GUITestOpStatus &os, const QString &readName) {
    moveToReadName(os, readName2readNumber(os, readName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToReadName"
void GTUtilsMcaEditor::moveToReadName(GUITestOpStatus &os, int readNumber) {
    scrollToRead(os, readNumber);
    const QRect readNameRect = getReadNameRect(os, readNumber);
    GTMouseDriver::moveTo(readNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickReadName"
void GTUtilsMcaEditor::clickReadName(GUITestOpStatus &os, const QString &readName, Qt::MouseButton mouseButton) {
    clickReadName(os, readName2readNumber(os, readName), mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickReadName"
void GTUtilsMcaEditor::clickReadName(GUITestOpStatus &os, int readNumber, Qt::MouseButton mouseButton) {
    moveToReadName(os, readNumber);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMcaEditor::removeRead(GUITestOpStatus &os, const QString &readName) {
    clickReadName(os, readName);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMcaEditor::undo(GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "redo"
void GTUtilsMcaEditor::redo(GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsMcaEditor::zoomIn(HI::GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Zoom In"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomOut"
void GTUtilsMcaEditor::zoomOut(HI::GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Zoom Out"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "resetZoom"
void GTUtilsMcaEditor::resetZoom(HI::GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Reset Zoom"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUndoEnabled"
bool GTUtilsMcaEditor::isUndoEnabled(GUITestOpStatus &os) {
    return GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isRedoEnabled"
bool GTUtilsMcaEditor::isRedoEnabled(GUITestOpStatus &os) {
    return GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleShowChromatogramsMode"
void GTUtilsMcaEditor::toggleShowChromatogramsMode(GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Show chromatograms"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "readName2readNumber"
int GTUtilsMcaEditor::readName2readNumber(GUITestOpStatus &os, const QString &readName) {
    const QStringList names = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    const int rowNumber = names.indexOf(readName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(readName), -1);
    return rowNumber;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
