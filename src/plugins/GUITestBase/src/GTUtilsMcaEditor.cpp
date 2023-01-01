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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QLabel>
#include <QTextDocument>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/MSAEditorOffsetsView.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/McaEditor.h>
#include <U2View/McaEditorConsensusArea.h>
#include <U2View/McaEditorNameList.h>
#include <U2View/McaEditorReferenceArea.h>
#include <U2View/McaEditorSequenceArea.h>
#include <U2View/RowHeightController.h>
#include <U2View/ScrollController.h>

#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMdi.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMcaEditor"

#define GT_METHOD_NAME "getActiveMcaEditorWindow"
QWidget* GTUtilsMcaEditor::getActiveMcaEditorWindow(GUITestOpStatus& os) {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(os, McaEditorFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMcaEditorWindowIsActive"
void GTUtilsMcaEditor::checkMcaEditorWindowIsActive(GUITestOpStatus& os) {
    getActiveMcaEditorWindow(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditor"
McaEditor* GTUtilsMcaEditor::getEditor(GUITestOpStatus& os) {
    McaEditorWgt* editorUi = getEditorUi(os);
    McaEditor* editor = editorUi->getEditor();
    GT_CHECK_RESULT(editor != nullptr, "MCA Editor is NULL", nullptr);
    return editor;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditorUi"
McaEditorWgt* GTUtilsMcaEditor::getEditorUi(GUITestOpStatus& os) {
    checkMcaEditorWindowIsActive(os);
    McaEditorWgt* mcaEditorWgt = nullptr;
    // For some reason McaEditorWgt is not within normal widgets hierarchy (wrong parent?), so can't use GTWidget::findWidget here.
    for (int time = 0; time < GT_OP_WAIT_MILLIS && mcaEditorWgt == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        MainWindow* mainWindow = AppContext::getMainWindow();
        QWidget* activeWindow = mainWindow == nullptr ? nullptr : mainWindow->getMDIManager()->getActiveWindow();
        if (activeWindow == nullptr) {
            continue;
        }
        mcaEditorWgt = activeWindow->findChild<McaEditorWgt*>();
    }
    GT_CHECK_RESULT(mcaEditorWgt != nullptr, "MCA Editor widget is NULL", nullptr);
    return mcaEditorWgt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceLabel"
QLabel* GTUtilsMcaEditor::getReferenceLabel(GUITestOpStatus& os) {
    auto referenceLabelContainerWidget = GTWidget::findWidget(os, "reference label container widget", getEditorUi(os));
    return GTWidget::findLabel(os, "", referenceLabelContainerWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameListArea"
McaEditorNameList* GTUtilsMcaEditor::getNameListArea(GUITestOpStatus& os) {
    return GTWidget::findExactWidget<McaEditorNameList*>(os, "mca_editor_name_list", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceArea"
McaEditorSequenceArea* GTUtilsMcaEditor::getSequenceArea(GUITestOpStatus& os) {
    return GTWidget::findExactWidget<McaEditorSequenceArea*>(os, "mca_editor_sequence_area", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusArea"
McaEditorConsensusArea* GTUtilsMcaEditor::getConsensusArea(GUITestOpStatus& os) {
    QWidget* activeWindow = getActiveMcaEditorWindow(os);
    return GTWidget::findExactWidget<McaEditorConsensusArea*>(os, "consArea", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceArea"
McaEditorReferenceArea* GTUtilsMcaEditor::getReferenceArea(GUITestOpStatus& os) {
    QWidget* activeWindow = getActiveMcaEditorWindow(os);
    return GTWidget::findExactWidget<McaEditorReferenceArea*>(os, "mca_editor_reference_area", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHorizontalScrollBar"
QScrollBar* GTUtilsMcaEditor::getHorizontalScrollBar(GUITestOpStatus& os) {
    return GTWidget::findScrollBar(os, "horizontal_sequence_scroll", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getVerticalScrollBar"
QScrollBar* GTUtilsMcaEditor::getVerticalScrollBar(GUITestOpStatus& os) {
    return GTWidget::findScrollBar(os, "vertical_sequence_scroll", getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMcaRow"
MultipleAlignmentRowData* GTUtilsMcaEditor::getMcaRow(GUITestOpStatus& os, int rowNum) {
    McaEditor* mcaEditor = GTUtilsMcaEditor::getEditor(os);
    MultipleChromatogramAlignmentObject* maObj = mcaEditor->getMaObject();
    GT_CHECK_RESULT(maObj != nullptr, "MultipleChromatogramAlignmentObject not found", nullptr);

    MultipleAlignmentRow row = maObj->getRow(rowNum);
    return row.data();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOffsetAction"
QAction* GTUtilsMcaEditor::getOffsetAction(GUITestOpStatus& os) {
    McaEditorWgt* editorWgt = GTUtilsMcaEditor::getEditorUi(os);
    GT_CHECK_RESULT(editorWgt != nullptr, "McaEditorWgt not found", nullptr);

    MSAEditorOffsetsViewController* offsetController = editorWgt->getOffsetsViewController();
    GT_CHECK_RESULT(offsetController != nullptr, "MSAEditorOffsetsViewController is NULL", nullptr);
    return offsetController->toggleColumnsViewAction;
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceLabelText"
QString GTUtilsMcaEditor::getReferenceLabelText(GUITestOpStatus& os) {
    QLabel* referenceLabel = getReferenceLabel(os);
    GT_CHECK_RESULT(nullptr != referenceLabel, "Reference label is NULL", "");
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
int GTUtilsMcaEditor::getReadsCount(GUITestOpStatus& os) {
    auto statusBar = GTWidget::findWidget(os, "mca_editor_status_bar", getEditorUi(os));
    auto readsCountLabel = GTWidget::findLabel(os, "Line", statusBar);

    QRegExp readsCounRegExp("Ln \\d+|\\- / (\\d+)");
    readsCounRegExp.indexIn(readsCountLabel->text());
    const QString totalReadsCountString = readsCounRegExp.cap(1);

    bool isNumber = false;
    const int totalReadsCount = totalReadsCountString.toInt(&isNumber);
    GT_CHECK_RESULT(isNumber, QString("Can't convert the reads count string to number: %1").arg(totalReadsCountString), -1);

    return totalReadsCount;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadsNames"
const QStringList GTUtilsMcaEditor::getReadsNames(GUITestOpStatus& os) {
    return getEditor(os)->getMaObject()->getMultipleAlignment()->getRowNames();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDirectReadsNames"
const QStringList GTUtilsMcaEditor::getDirectReadsNames(GUITestOpStatus& os) {
    QStringList directReadsNames;
    MultipleChromatogramAlignmentObject* mcaObject = getEditor(os)->getMaObject();
    const int rowsCount = mcaObject->getRowCount();
    for (int i = 0; i < rowsCount; i++) {
        if (!mcaObject->getMcaRow(i)->isReversed()) {
            directReadsNames << mcaObject->getMcaRow(i)->getName();
        }
    }
    return directReadsNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReverseComplementReadsNames"
const QStringList GTUtilsMcaEditor::getReverseComplementReadsNames(GUITestOpStatus& os) {
    QStringList reverseComplementedReadsNames;
    MultipleChromatogramAlignmentObject* mcaObject = getEditor(os)->getMaObject();
    const int rowsCount = mcaObject->getRowCount();
    for (int i = 0; i < rowsCount; i++) {
        if (mcaObject->getMcaRow(i)->isReversed()) {
            reverseComplementedReadsNames << mcaObject->getMcaRow(i)->getName();
        }
    }
    return reverseComplementedReadsNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadNameRect"
QRect GTUtilsMcaEditor::getReadNameRect(GUITestOpStatus& os, const QString& readName) {
    McaEditorNameList* nameList = getNameListArea(os);
    GT_CHECK_RESULT(nullptr != nameList, "McaEditorNameList not found", QRect());

    const QStringList names = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    const int rowNumber = names.indexOf(readName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(readName), QRect());
    return getReadNameRect(os, rowNumber);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadNameRect"
QRect GTUtilsMcaEditor::getReadNameRect(GUITestOpStatus& os, int rowNumber) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(rowNumber), QRect());

    McaEditorNameList* nameList = getNameListArea(os);
    GT_CHECK_RESULT(nullptr != nameList, "McaEditorNameList not found", QRect());

    const U2Region rowScreenRange = getEditorUi(os)->getRowHeightController()->getScreenYRegionByViewRowIndex(rowNumber);
    return QRect(nameList->mapToGlobal(QPoint(0, rowScreenRange.startPos)), nameList->mapToGlobal(QPoint(nameList->width(), rowScreenRange.endPos())));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToRead"
void GTUtilsMcaEditor::scrollToRead(GUITestOpStatus& os, const QString& readName) {
    scrollToRead(os, readName2readNumber(os, readName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToRead"
void GTUtilsMcaEditor::scrollToRead(GUITestOpStatus& os, int readNumber) {
    McaEditorWgt* mcaEditorWgt = getEditorUi(os);
    const U2Region rowRange = mcaEditorWgt->getRowHeightController()->getGlobalYRegionByViewRowIndex(readNumber);
    CHECK(!U2Region(mcaEditorWgt->getScrollController()->getScreenPosition().y(), getNameListArea(os)->height()).contains(rowRange), );
    GTScrollBar::moveSliderWithMouseToValue(os, getVerticalScrollBar(os), rowRange.center() - mcaEditorWgt->getSequenceArea()->height() / 2);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToReadName"
void GTUtilsMcaEditor::moveToReadName(GUITestOpStatus& os, const QString& readName) {
    moveToReadName(os, readName2readNumber(os, readName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToReadName"
void GTUtilsMcaEditor::moveToReadName(GUITestOpStatus& os, int readNumber) {
    scrollToRead(os, readNumber);
    const QRect readNameRect = getReadNameRect(os, readNumber);
    GTMouseDriver::moveTo(readNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickReadName"
void GTUtilsMcaEditor::clickReadName(GUITestOpStatus& os, const QString& readName, Qt::MouseButton mouseButton, const Qt::KeyboardModifiers& modifiers) {
    clickReadName(os, readName2readNumber(os, readName), mouseButton, modifiers);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickReadName"
void GTUtilsMcaEditor::clickReadName(GUITestOpStatus& os, int readNumber, Qt::MouseButton mouseButton, const Qt::KeyboardModifiers& modifiers) {
    moveToReadName(os, readNumber);

    QList<Qt::Key> modifierKeys = GTKeyboardDriver::modifiersToKeys(modifiers);
    for (auto key : qAsConst(modifierKeys)) {
        GTKeyboardDriver::keyPress(key);
    }
    GTMouseDriver::click(mouseButton);
    for (auto key : qAsConst(modifierKeys)) {
        GTKeyboardDriver::keyRelease(key);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickReadName"
void GTUtilsMcaEditor::doubleClickReadName(GUITestOpStatus& os, int readIndex) {
    moveToReadName(os, readIndex);
    GTMouseDriver::doubleClick();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMcaEditor::removeRead(GUITestOpStatus& os, const QString& readName) {
    clickReadName(os, readName);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMcaEditor::undo(GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "redo"
void GTUtilsMcaEditor::redo(GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsMcaEditor::zoomIn(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Zoom In"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomOut"
void GTUtilsMcaEditor::zoomOut(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Zoom Out"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "resetZoom"
void GTUtilsMcaEditor::resetZoom(HI::GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Reset Zoom"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUndoEnabled"
bool GTUtilsMcaEditor::isUndoEnabled(GUITestOpStatus& os) {
    return GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isRedoEnabled"
bool GTUtilsMcaEditor::isRedoEnabled(GUITestOpStatus& os) {
    return GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleShowChromatogramsMode"
void GTUtilsMcaEditor::toggleShowChromatogramsMode(GUITestOpStatus& os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Show chromatograms"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "readName2readNumber"
int GTUtilsMcaEditor::readName2readNumber(GUITestOpStatus& os, const QString& readName) {
    const QStringList names = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    const int rowNumber = names.indexOf(readName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(readName), -1);
    return rowNumber;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectReadsByName"
void GTUtilsMcaEditor::selectReadsByName(HI::GUITestOpStatus& os, const QStringList& rowNames) {
    for (const QString& rowName : qAsConst(rowNames)) {
        clickReadName(os, rowName, Qt::LeftButton, Qt::ControlModifier);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
