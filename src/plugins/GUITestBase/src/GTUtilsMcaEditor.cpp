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

QWidget* GTUtilsMcaEditor::getActiveMcaEditorWindow() {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(McaEditorFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}

void GTUtilsMcaEditor::checkMcaEditorWindowIsActive() {
    getActiveMcaEditorWindow();
}

McaEditor* GTUtilsMcaEditor::getEditor() {
    McaEditorWgt* editorUi = getEditorUi();
    McaEditor* editor = editorUi->getEditor();
    GT_CHECK_RESULT(editor != nullptr, "MCA Editor is NULL", nullptr);
    return editor;
}

McaEditorWgt* GTUtilsMcaEditor::getEditorUi() {
    checkMcaEditorWindowIsActive();
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

QLabel* GTUtilsMcaEditor::getReferenceLabel() {
    auto referenceLabelContainerWidget = GTWidget::findWidget("reference label container widget", getEditorUi());
    return GTWidget::findLabel("", referenceLabelContainerWidget);
}

McaEditorNameList* GTUtilsMcaEditor::getNameListArea() {
    return GTWidget::findExactWidget<McaEditorNameList*>("mca_editor_name_list", getEditorUi());
}

McaEditorSequenceArea* GTUtilsMcaEditor::getSequenceArea() {
    return GTWidget::findExactWidget<McaEditorSequenceArea*>("mca_editor_sequence_area", getEditorUi());
}

McaEditorConsensusArea* GTUtilsMcaEditor::getConsensusArea() {
    QWidget* activeWindow = getActiveMcaEditorWindow();
    return GTWidget::findExactWidget<McaEditorConsensusArea*>("consArea", activeWindow);
}

McaEditorReferenceArea* GTUtilsMcaEditor::getReferenceArea() {
    QWidget* activeWindow = getActiveMcaEditorWindow();
    return GTWidget::findExactWidget<McaEditorReferenceArea*>("mca_editor_reference_area", activeWindow);
}

QScrollBar* GTUtilsMcaEditor::getHorizontalScrollBar() {
    return GTWidget::findScrollBar("horizontal_sequence_scroll", getEditorUi());
}

QScrollBar* GTUtilsMcaEditor::getVerticalScrollBar() {
    return GTWidget::findScrollBar("vertical_sequence_scroll", getEditorUi());
}

MultipleAlignmentRowData* GTUtilsMcaEditor::getMcaRow(int rowNum) {
    McaEditor* mcaEditor = GTUtilsMcaEditor::getEditor();
    MultipleChromatogramAlignmentObject* maObj = mcaEditor->getMaObject();
    GT_CHECK_RESULT(maObj != nullptr, "MultipleChromatogramAlignmentObject not found", nullptr);

    MultipleAlignmentRow row = maObj->getRow(rowNum);
    return row.data();
}

QAction* GTUtilsMcaEditor::getOffsetAction() {
    McaEditorWgt* editorWgt = GTUtilsMcaEditor::getEditorUi();
    GT_CHECK_RESULT(editorWgt != nullptr, "McaEditorWgt not found", nullptr);

    MSAEditorOffsetsViewController* offsetController = editorWgt->getOffsetsViewController();
    GT_CHECK_RESULT(offsetController != nullptr, "MSAEditorOffsetsViewController is NULL", nullptr);
    return offsetController->toggleColumnsViewAction;
}


QString GTUtilsMcaEditor::getReferenceLabelText() {
    QLabel* referenceLabel = getReferenceLabel();
    GT_CHECK_RESULT(nullptr != referenceLabel, "Reference label is NULL", "");
    if (referenceLabel->textFormat() != Qt::PlainText) {
        QTextDocument textDocument;
        textDocument.setHtml(referenceLabel->text());
        return textDocument.toPlainText();
    } else {
        return referenceLabel->text();
    }
}

int GTUtilsMcaEditor::getReadsCount() {
    auto statusBar = GTWidget::findWidget("mca_editor_status_bar", getEditorUi());
    auto readsCountLabel = GTWidget::findLabel("Line", statusBar);

    QRegExp readsCounRegExp("Ln \\d+|\\- / (\\d+)");
    readsCounRegExp.indexIn(readsCountLabel->text());
    const QString totalReadsCountString = readsCounRegExp.cap(1);

    bool isNumber = false;
    const int totalReadsCount = totalReadsCountString.toInt(&isNumber);
    GT_CHECK_RESULT(isNumber, QString("Can't convert the reads count string to number: %1").arg(totalReadsCountString), -1);

    return totalReadsCount;
}

const QStringList GTUtilsMcaEditor::getReadsNames() {
    return getEditor()->getMaObject()->getMultipleAlignment()->getRowNames();
}

const QStringList GTUtilsMcaEditor::getDirectReadsNames() {
    QStringList directReadsNames;
    MultipleChromatogramAlignmentObject* mcaObject = getEditor()->getMaObject();
    const int rowsCount = mcaObject->getRowCount();
    for (int i = 0; i < rowsCount; i++) {
        if (!mcaObject->getMcaRow(i)->isReversed()) {
            directReadsNames << mcaObject->getMcaRow(i)->getName();
        }
    }
    return directReadsNames;
}

const QStringList GTUtilsMcaEditor::getReverseComplementReadsNames() {
    QStringList reverseComplementedReadsNames;
    MultipleChromatogramAlignmentObject* mcaObject = getEditor()->getMaObject();
    const int rowsCount = mcaObject->getRowCount();
    for (int i = 0; i < rowsCount; i++) {
        if (mcaObject->getMcaRow(i)->isReversed()) {
            reverseComplementedReadsNames << mcaObject->getMcaRow(i)->getName();
        }
    }
    return reverseComplementedReadsNames;
}

QRect GTUtilsMcaEditor::getReadNameRect(const QString& readName) {
    McaEditorNameList* nameList = getNameListArea();
    GT_CHECK_RESULT(nullptr != nameList, "McaEditorNameList not found", QRect());

    const QStringList names = GTUtilsMcaEditorSequenceArea::getVisibleNames();
    const int rowNumber = names.indexOf(readName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(readName), QRect());
    return getReadNameRect(rowNumber);
}

QRect GTUtilsMcaEditor::getReadNameRect(int rowNumber) {
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(rowNumber), QRect());

    McaEditorNameList* nameList = getNameListArea();
    GT_CHECK_RESULT(nullptr != nameList, "McaEditorNameList not found", QRect());

    const U2Region rowScreenRange = getEditorUi()->getRowHeightController()->getScreenYRegionByViewRowIndex(rowNumber);
    return QRect(nameList->mapToGlobal(QPoint(0, rowScreenRange.startPos)), nameList->mapToGlobal(QPoint(nameList->width(), rowScreenRange.endPos())));
}

void GTUtilsMcaEditor::scrollToRead(const QString& readName) {
    scrollToRead(readName2readNumber(readName));
}

void GTUtilsMcaEditor::scrollToRead(int readNumber) {
    McaEditorWgt* mcaEditorWgt = getEditorUi();
    const U2Region rowRange = mcaEditorWgt->getRowHeightController()->getGlobalYRegionByViewRowIndex(readNumber);
    CHECK(!U2Region(mcaEditorWgt->getScrollController()->getScreenPosition().y(), getNameListArea()->height()).contains(rowRange), );
    GTScrollBar::moveSliderWithMouseToValue(getVerticalScrollBar(), rowRange.center() - mcaEditorWgt->getSequenceArea()->height() / 2);
}

void GTUtilsMcaEditor::moveToReadName(const QString& readName) {
    moveToReadName(readName2readNumber(readName));
}

void GTUtilsMcaEditor::moveToReadName(int readNumber) {
    scrollToRead(readNumber);
    const QRect readNameRect = getReadNameRect(readNumber);
    GTMouseDriver::moveTo(readNameRect.center());
}

void GTUtilsMcaEditor::clickReadName(const QString& readName, Qt::MouseButton mouseButton, const Qt::KeyboardModifiers& modifiers) {
    clickReadName(readName2readNumber(readName), mouseButton, modifiers);
}

void GTUtilsMcaEditor::clickReadName(int readNumber, Qt::MouseButton mouseButton, const Qt::KeyboardModifiers& modifiers) {
    moveToReadName(readNumber);

    QList<Qt::Key> modifierKeys = GTKeyboardDriver::modifiersToKeys(modifiers);
    for (auto key : qAsConst(modifierKeys)) {
        GTKeyboardDriver::keyPress(key);
    }
    GTMouseDriver::click(mouseButton);
    for (auto key : qAsConst(modifierKeys)) {
        GTKeyboardDriver::keyRelease(key);
    }
}

void GTUtilsMcaEditor::doubleClickReadName(int readIndex) {
    moveToReadName(readIndex);
    GTMouseDriver::doubleClick();
}

void GTUtilsMcaEditor::removeRead(const QString& readName) {
    clickReadName(readName);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(500);
}

void GTUtilsMcaEditor::undo() {
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_undo"));
}

void GTUtilsMcaEditor::redo() {
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_redo"));
}

void GTUtilsMcaEditor::zoomIn() {
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Zoom In"));
}

void GTUtilsMcaEditor::zoomOut() {
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Zoom Out"));
}

void GTUtilsMcaEditor::resetZoom() {
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Reset Zoom"));
}

bool GTUtilsMcaEditor::isUndoEnabled() {
    return GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_undo")->isEnabled();
}

bool GTUtilsMcaEditor::isRedoEnabled() {
    return GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_redo")->isEnabled();
}

void GTUtilsMcaEditor::toggleShowChromatogramsMode() {
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Show chromatograms"));
}

int GTUtilsMcaEditor::readName2readNumber(const QString& readName) {
    const QStringList names = GTUtilsMcaEditorSequenceArea::getVisibleNames();
    const int rowNumber = names.indexOf(readName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Read '%1' not found").arg(readName), -1);
    return rowNumber;
}

void GTUtilsMcaEditor::selectReadsByName(const QStringList& rowNames) {
    for (const QString& rowName : qAsConst(rowNames)) {
        clickReadName(rowName, Qt::LeftButton, Qt::ControlModifier);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
