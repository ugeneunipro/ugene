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

#include <api/GTMSAEditorStatusWidget.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <QStyle>
#include <QStyleOptionSlider>

#include <U2Core/U2SafePoints.h>

#include <U2View/BaseWidthController.h>
#include <U2View/DrawHelper.h>
#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MaEditorSelection.h>
#include <U2View/MsaEditorSimilarityColumn.h>
#include <U2View/MultilineScrollController.h>
#include <U2View/RowHeightController.h>
#include <U2View/ScrollController.h>

#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "runnables/ugene/corelibs/U2Gui/util/RenameSequenceFiller.h"

namespace U2 {
using namespace HI;
const QString GTUtilsMSAEditorSequenceArea::highlightingColor = "#9999cc";

#define GT_CLASS_NAME "GTUtilsMSAEditorSequenceArea"

#define GT_METHOD_NAME "getSequenceArea"
MSAEditorSequenceArea* GTUtilsMSAEditorSequenceArea::getSequenceArea(GUITestOpStatus& os, int index, bool checkError) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    if (checkError) {
        CHECK_SET_ERR_RESULT(activeWindow != nullptr,
                             QString("Can't find sequence area #%1").arg(index),
                             nullptr);
    }
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<MSAEditorSequenceArea*>(os, "msa_editor_sequence_area", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusArea"
MSAEditorConsensusArea* GTUtilsMSAEditorSequenceArea::getConsensusArea(GUITestOpStatus& os, int index, bool checkError) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    if (checkError) {
        CHECK_SET_ERR_RESULT(activeWindow != nullptr,
                             QString("Can't find consensus area #%1").arg(index),
                             nullptr);
    }
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<MSAEditorConsensusArea*>(os, "consArea", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getHorizontalNamesScroll"
QScrollBar* GTUtilsMSAEditorSequenceArea::getHorizontalNamesScroll(GUITestOpStatus& os, int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<QScrollBar*>(os, "horizontal_names_scroll", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSimilarityColumn"
MsaEditorSimilarityColumn* GTUtilsMSAEditorSequenceArea::getSimilarityColumn(GUITestOpStatus& os, int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<MsaEditorSimilarityColumn*>(os, "msa_editor_similarity_column", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenu"
void GTUtilsMSAEditorSequenceArea::callContextMenu(GUITestOpStatus& os, const QPoint& innerCoords) {
    if (innerCoords.isNull()) {
        GTWidget::click(os, getSequenceArea(os), Qt::RightButton);
    } else {
        moveTo(os, innerCoords);
        GTMouseDriver::click(Qt::RightButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveTo"
void GTUtilsMSAEditorSequenceArea::moveTo(GUITestOpStatus& os, const QPoint& p) {
    QPoint convP = convertCoordinates(os, p);

    GTMouseDriver::moveTo(convP);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPositionRect"
QRect GTUtilsMSAEditorSequenceArea::getPositionRect(GUITestOpStatus& os,
                                                    const QPoint& position,
                                                    int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    GT_CHECK_RESULT(activeWindow != nullptr, QString("Can't find MaEditorWgt %1").arg(index), QRect());
    auto msaEditArea = GTWidget::findExactWidget<MSAEditorSequenceArea*>(os, "msa_editor_sequence_area", activeWindow);

    auto msaEditorWidget = qobject_cast<MsaEditorWgt*>(msaEditArea->getEditor()->getUI()->getUI(index));
    U2Region regX = msaEditorWidget->getBaseWidthController()->getBaseGlobalRange(position.x());
    U2Region regY = msaEditorWidget->getRowHeightController()->getGlobalYRegionByViewRowIndex(position.y());

    QPoint topLeftGlobalPoint = msaEditArea->mapToGlobal(QPoint(regX.startPos, regY.startPos));
    QPoint bottomRightGlobalPoint = msaEditArea->mapToGlobal(QPoint(regX.endPos(), regY.endPos()));

    return QRect(topLeftGlobalPoint, bottomRightGlobalPoint);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "convertCoordinates"
QPoint GTUtilsMSAEditorSequenceArea::convertCoordinates(GUITestOpStatus& os, const QPoint p, int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    GT_CHECK_RESULT(activeWindow != nullptr, QString("Can't find MaEditorWgt %1").arg(index), QPoint());
    auto msaEditArea = GTWidget::findExactWidget<MSAEditorSequenceArea*>(os, "msa_editor_sequence_area", activeWindow);

    auto ui = qobject_cast<MsaEditorWgt*>(msaEditArea->getEditor()->getUI()->getUI(index));
    int posX = ui->getBaseWidthController()->getBaseScreenCenter(p.x());
    int posY = (int)ui->getRowHeightController()->getScreenYRegionByViewRowIndex(p.y()).center();
    return msaEditArea->mapToGlobal({posX, posY});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectArea"
void GTUtilsMSAEditorSequenceArea::selectArea(
    GUITestOpStatus& os,
    int multilineIndex,
    QPoint p1,
    QPoint p2,
    GTGlobals::UseMethod method) {
    auto sequenceArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, multilineIndex);
    CHECK_SET_ERR_RESULT(sequenceArea != nullptr,
                         QString("Can't find sequence area #%1").arg(multilineIndex), );

    p1.rx() = (p1.x() == -1 ? sequenceArea->getNumVisibleBases() - 1 : p1.x());
    p1.ry() = (p1.y() == -1 ? sequenceArea->getViewRowCount() - 1 : p1.y());

    p2.rx() = (p2.x() == -1 ? sequenceArea->getNumVisibleBases() - 1 : p2.x());
    p2.ry() = (p2.y() == -1 ? sequenceArea->getViewRowCount() - 1 : p2.y());

    switch (method) {
        case GTGlobals::UseKey:
            clickToPosition(os, p1);  // Make initial selection.
            moveMouseToPosition(os, p2);  // Scroll to the end.
            GTKeyboardDriver::keyPress(Qt::Key_Shift);  // Make the final selection.
            GTMouseDriver::click();
            GTKeyboardDriver::keyRelease(Qt::Key_Shift);
            break;
        case GTGlobals::UseMouse:
            GTMouseDriver::dragAndDrop(convertCoordinates(os, p1), convertCoordinates(os, p2));
            break;
        case GTGlobals::UseKeyBoard:
            GT_FAIL("Not implemented", );
        default:
            GT_FAIL("An unknown method", );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectArea"
void GTUtilsMSAEditorSequenceArea::selectArea(GUITestOpStatus& os, QPoint p1, QPoint p2, GTGlobals::UseMethod method) {
    return selectArea(os, 0, p1, p2, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "cancelSelection"
void GTUtilsMSAEditorSequenceArea::cancelSelection(GUITestOpStatus& /*os*/) {
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTUtilsMSAEditorSequenceArea::click(GUITestOpStatus& os, const QPoint& screenMaPoint) {
    GTMouseDriver::moveTo(convertCoordinates(os, screenMaPoint));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copySelectionByContextMenu"
void GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu(GUITestOpStatus& os) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Copy/Paste", "Copy"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToPosition"
void GTUtilsMSAEditorSequenceArea::scrollToPosition(GUITestOpStatus& os, const QPoint& position) {
    auto msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);
    CHECK_SET_ERR_RESULT(msaSeqArea != nullptr,
                         QString("Can't find sequence area #%1").arg(0), );
    GT_CHECK(msaSeqArea->isInRange(position),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(position.x())
                 .arg(position.y())
                 .arg(msaSeqArea->getEditor()->getAlignmentLen())
                 .arg(msaSeqArea->getViewRowCount()));

    // Scroll down with a vertical scroll bar first.
    MaEditorWgt* msaWidget = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(0);
    auto verticalScrollBar = GTWidget::findScrollBar(os, "vertical_sequence_scroll", msaWidget);

    QStyleOptionSlider vScrollBarOptions;
    vScrollBarOptions.initFrom(verticalScrollBar);

    for (int clickCount = 0; !msaSeqArea->isRowVisible(position.y(), false); clickCount++) {
        CHECK_SET_ERR_RESULT(clickCount <= 20, "Too many clicks on verticalScrollBar to get to the desired position. Use another method in GUI tests", );
        // Make 1 click to the slider (1-base shift), so scroll bar will have a focus.
        // After that use PageUp/PageDown to scroll faster (1-page shift).
        if (clickCount == 0) {
            QRect sliderSpaceRect = verticalScrollBar->style()->subControlRect(QStyle::CC_ScrollBar, &vScrollBarOptions, QStyle::SC_ScrollBarGroove, verticalScrollBar);
            QPoint bottomEdge(sliderSpaceRect.width() / 2, sliderSpaceRect.y() + sliderSpaceRect.height());

            GTMouseDriver::moveTo(verticalScrollBar->mapToGlobal(bottomEdge) - QPoint(0, 1));
            GTMouseDriver::click();
        } else {
            GTKeyboardDriver::keyClick(Qt::Key_PageDown);
        }
    }

    // Scroll right with a horizontal scroll bar next.
    auto horizontalScrollBar = GTWidget::findScrollBar(os, "horizontal_sequence_scroll", msaWidget);

    QStyleOptionSlider hScrollBarOptions;
    hScrollBarOptions.initFrom(horizontalScrollBar);

    for (int clickCount = 0; !msaSeqArea->isPositionVisible(position.x(), false); clickCount++) {
        CHECK_SET_ERR_RESULT(clickCount <= 20, "Too many clicks on horizontalScrollBar to get to the desired position. Use another method in GUI tests", );
        // Make 1 click to the slider (1-base shift), so scroll bar will have a focus.
        // After that use PageUp/PageDown to scroll faster (1-page shift).
        if (clickCount == 0) {
            QRect sliderSpaceRect = horizontalScrollBar->style()->subControlRect(QStyle::CC_ScrollBar, &hScrollBarOptions, QStyle::SC_ScrollBarGroove, horizontalScrollBar);
            QPoint leftEdge(sliderSpaceRect.x(), sliderSpaceRect.height() / 2);
            QPoint rightEdge(sliderSpaceRect.x() + sliderSpaceRect.width(), sliderSpaceRect.height() / 2);

            int firstBase = msaSeqArea->getFirstVisibleBase();
            int lastBase = msaSeqArea->getLastVisibleBase(false);
            QPoint p;
            if (position.x() >= lastBase) {
                p = horizontalScrollBar->mapToGlobal(rightEdge) + QPoint(3, 0);
            } else if (position.x() <= firstBase) {
                p = horizontalScrollBar->mapToGlobal(leftEdge) - QPoint(3, 0);
            } else {
                p = horizontalScrollBar->mapToGlobal(rightEdge) - QPoint(1, 0);
            }
            GTMouseDriver::moveTo(p);
            GTMouseDriver::click();
        } else {
            GTKeyboardDriver::keyClick(Qt::Key_PageDown);
        }
    }

    SAFE_POINT(msaSeqArea->isVisible(position, false), "The position is still invisible after scrolling", );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToBottom"
void GTUtilsMSAEditorSequenceArea::scrollToBottom(GUITestOpStatus& os) {
    // scroll down
    auto vBar = GTWidget::findScrollBar(os, "vertical_sequence_scroll", GTUtilsMsaEditor::getActiveMsaEditorWindow(os));
#ifdef Q_OS_DARWIN
    vBar->setValue(vBar->maximum());
    return;
#endif

    QStyleOptionSlider vScrollBarOptions;
    vScrollBarOptions.initFrom(vBar);

    while (vBar->value() != vBar->maximum()) {
        const QRect sliderSpaceRect = vBar->style()->subControlRect(QStyle::CC_ScrollBar, &vScrollBarOptions, QStyle::SC_ScrollBarGroove, vBar);
        const QPoint bottomEdge(sliderSpaceRect.width() / 2 + 10, sliderSpaceRect.y() + sliderSpaceRect.height());

        GTMouseDriver::moveTo(vBar->mapToGlobal(bottomEdge) - QPoint(0, 1));
        GTMouseDriver::click();
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveMouseToPosition"
void GTUtilsMSAEditorSequenceArea::moveMouseToPosition(GUITestOpStatus& os, const QPoint& globalMaPosition) {
    auto msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);
    CHECK_SET_ERR_RESULT(msaSeqArea != nullptr,
                         QString("Can't find sequence area #%1").arg(0), );
    GT_CHECK(msaSeqArea->isInRange(globalMaPosition),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(globalMaPosition.x())
                 .arg(globalMaPosition.y())
                 .arg(msaSeqArea->getEditor()->getAlignmentLen())
                 .arg(msaSeqArea->getViewRowCount()));

    auto ui = GTUtilsMsaEditor::getEditor(os)->getMaEditorMultilineWgt();
    QPoint positionCenter;
    uint muiCount = msaSeqArea->getEditor()->getUI()->getChildrenCount();
    uint multilineIndex = 0;
    if (GTUtilsMsaEditor::getMultilineMode(os)) {
        ui->getScrollController()->scrollToPoint(globalMaPosition);
    } else {
        scrollToPosition(os, globalMaPosition);
    }
    do {
        msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, multilineIndex);
        MaEditorWgt* mui = ui->getUI(multilineIndex);
        positionCenter = QPoint(mui->getBaseWidthController()->getBaseScreenCenter(
                                    globalMaPosition.x()),
                                mui->getRowHeightController()
                                    ->getScreenYRegionByViewRowIndex(globalMaPosition.y())
                                    .center());
        multilineIndex++;
    } while (multilineIndex < muiCount && !msaSeqArea->rect().contains(positionCenter, false));
    GT_CHECK(msaSeqArea->rect().contains(positionCenter, false), "Position is not visible");
    GTMouseDriver::moveTo(msaSeqArea->mapToGlobal(positionCenter));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickToPosition"
void GTUtilsMSAEditorSequenceArea::clickToPosition(GUITestOpStatus& os, const QPoint& globalMaPosition) {
    GTUtilsMSAEditorSequenceArea::moveMouseToPosition(os, globalMaPosition);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelectedRect"
void GTUtilsMSAEditorSequenceArea::checkSelectedRect(GUITestOpStatus& os,
                                                     int multilineIndex,
                                                     const QRect& expectedRect) {
    auto msaEditArea = getSequenceArea(os, multilineIndex);

    QRect msaEditRegion = msaEditArea->getEditor()->getSelection().toRect();
    CHECK_SET_ERR(expectedRect == msaEditRegion, QString("Unexpected selection region. Expected: [(%1,%2) (%3,%4)]. Actual: [(%5,%6) (%7,%8)]").arg(expectedRect.topLeft().x()).arg(expectedRect.topLeft().y()).arg(expectedRect.bottomRight().x()).arg(expectedRect.bottomRight().y()).arg(msaEditRegion.topLeft().x()).arg(msaEditRegion.topLeft().y()).arg(msaEditRegion.bottomRight().x()).arg(msaEditRegion.bottomRight().y()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelectedRect"
void GTUtilsMSAEditorSequenceArea::checkSelectedRect(GUITestOpStatus& os, const QRect& expectedRect) {
    checkSelectedRect(os, 0, expectedRect);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameList"
QStringList GTUtilsMSAEditorSequenceArea::getNameList(GUITestOpStatus& os) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    QStringList result = editor->getMaObject()->getMultipleAlignment()->getRowNames();
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCurrentRowNames"
QStringList GTUtilsMSAEditorSequenceArea::getCurrentRowNames(GUITestOpStatus& os) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    int viewRowCount = collapseModel->getViewRowCount();
    QStringList rowNameList;
    for (int viewRowIndex = 0; viewRowIndex < viewRowCount; viewRowIndex++) {
        int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
        MultipleAlignmentRow maRow = editor->getMaObject()->getRow(maRowIndex);
        rowNameList << maRow->getName();
    }
    return rowNameList;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasSequencesWithNames"
bool GTUtilsMSAEditorSequenceArea::hasSequencesWithNames(GUITestOpStatus& os, const QStringList& names) {
    QStringList nameList = getNameList(os);
    QStringList absentNames;
    foreach (const QString& name, names) {
        CHECK_CONTINUE(!nameList.contains(name));

        absentNames << name;
    }
    CHECK_SET_ERR_RESULT(absentNames.isEmpty(),
                         QString("Sequences with the following names are't presented in the alignment: \"%1\".")
                             .arg(absentNames.join("\", \"")),
                         false);

    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getVisibleNames"
QStringList GTUtilsMSAEditorSequenceArea::getVisibleNames(GUITestOpStatus& os, bool asShownInNameList) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    MaEditorNameList* nameListArea = GTUtilsMsaEditor::getNameListArea(os);
    CHECK_SET_ERR_RESULT(nameListArea != nullptr, "MSA Editor name list area is NULL", QStringList());

    QList<int> visibleRowsIndexes = editor->getUI()->getUI(0)->getDrawHelper()->getVisibleMaRowIndexes(nameListArea->height());
    MultipleSequenceAlignmentObject* msaObject = editor->getMaObject();

    QStringList visibleRowNames;
    for (int rowIndex : qAsConst(visibleRowsIndexes)) {
        QString name = asShownInNameList ? nameListArea->getTextForRow(rowIndex) : msaObject->getRow(rowIndex)->getName();
        visibleRowNames << name;
    }
    return visibleRowNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeSequence"
void GTUtilsMSAEditorSequenceArea::removeSequence(GUITestOpStatus& os, const QString& sequenceName) {
    selectSequence(os, sequenceName);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSimilarityValue"
QString GTUtilsMSAEditorSequenceArea::getSimilarityValue(GUITestOpStatus& os, int row, int index) {
    // bad solution, but better then nothing
    auto simCol = GTUtilsMSAEditorSequenceArea::getSimilarityColumn(os, index);

    return simCol->getTextForRow(row);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCollapseTriangle"
void GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(GUITestOpStatus& os, QString seqName) {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);
    auto msaEditWgt = msaEditArea->getEditor()->getUI()->getUI(0);

    int rowNum = getVisibleNames(os).indexOf(seqName);
    GT_CHECK(rowNum != -1, "sequence not found in nameList");
    auto nameList = GTWidget::findExactWidget<QWidget*>(os, "msa_editor_name_list", msaEditWgt);
    QPoint localCoord = QPoint(15, msaEditWgt->getRowHeightController()->getScreenYRegionByViewRowIndex(rowNum).startPos + 7);
    QPoint globalCoord = nameList->mapToGlobal(localCoord);
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isCollapsed"
bool GTUtilsMSAEditorSequenceArea::isCollapsed(GUITestOpStatus& os, QString seqName) {
    QStringList names = getNameList(os);
    QStringList visiable = getVisibleNames(os);
    GT_CHECK_RESULT(names.contains(seqName), "sequence " + seqName + " not found", false);
    return !visiable.contains(seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "collapsingMode"
bool GTUtilsMSAEditorSequenceArea::collapsingMode(GUITestOpStatus& os) {
    QAbstractButton* toggleSequenceOrderButton = GTAction::button(os, "toggle_sequence_row_order_action");
    bool nameListsAreEqual = getVisibleNames(os) == getNameList(os);
    if (nameListsAreEqual && !toggleSequenceOrderButton->isChecked()) {
        return false;
    } else if (!nameListsAreEqual && toggleSequenceOrderButton->isChecked()) {
        return true;
    }
    GT_CHECK_RESULT(false, "something wrong with collapsing mode", false);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFirstVisibleBaseIndex"
int GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(GUITestOpStatus& os, int multilineIndex) {
    auto msaEditArea = getSequenceArea(os, multilineIndex);

    ScrollController* scrollController = msaEditArea->getEditor()->getUI()->getUI(multilineIndex)->getScrollController();
    int clippedIdx = scrollController->getFirstVisibleBase(true);
    int notClippedIdx = scrollController->getFirstVisibleBase(false);
    return clippedIdx + (clippedIdx == notClippedIdx ? 0 : 1);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLastVisibleBaseIndex"
int GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(GUITestOpStatus& os, int multilineIndex) {
    auto msaEditArea = getSequenceArea(os, multilineIndex);

    ScrollController* scrollController = msaEditArea->getEditor()->getUI()->getUI(multilineIndex)->getScrollController();
    int clippedIdx = scrollController->getLastVisibleBase(msaEditArea->width(), true);
    int notClippedIdx = scrollController->getLastVisibleBase(msaEditArea->width(), false);
    return clippedIdx + (clippedIdx == notClippedIdx ? 0 : 1);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFirstVisibleRowIndex"
int GTUtilsMSAEditorSequenceArea::getFirstVisibleRowIndex(GUITestOpStatus& os, bool countClipped) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    return editor->getUI()->getScrollController()->getFirstVisibleViewRowIndex(countClipped);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLastVisibleRowIndex"
int GTUtilsMSAEditorSequenceArea::getLastVisibleRowIndex(GUITestOpStatus& os, bool countClipped) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    int widgetHeight = editor->getMaEditorWgt()->getSequenceArea()->height();
    return editor->getUI()->getScrollController()->getLastVisibleViewRowIndex(widgetHeight, countClipped);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
int GTUtilsMSAEditorSequenceArea::getLength(GUITestOpStatus& os) {
    auto statusWidget = GTWidget::findWidget(os, "msa_editor_status_bar");
    return GTMSAEditorStatusWidget::length(os, statusWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNumVisibleBases"
int GTUtilsMSAEditorSequenceArea::getNumVisibleBases(GUITestOpStatus& os) {
    auto msaEditArea = GTWidget::findExactWidget<MSAEditorSequenceArea*>(os, "msa_editor_sequence_area", GTUtilsMsaEditor::getActiveMsaEditorWindow(os));
    return msaEditArea->getEditor()->getUI()->getUI(0)->getDrawHelper()->getVisibleBasesCount(msaEditArea->width());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedRect"
QRect GTUtilsMSAEditorSequenceArea::getSelectedRect(GUITestOpStatus& os) {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);

    return msaEditArea->getEditor()->getSelection().toRect();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDropSelection"
void GTUtilsMSAEditorSequenceArea::dragAndDropSelection(GUITestOpStatus& os, const QPoint& fromMaPosition, const QPoint& toMaPosition) {
    const MaEditorSelection& selection = getSequenceArea(os)->getEditor()->getSelection();
    GT_CHECK(selection.contains(fromMaPosition), QString("Position (%1, %2) is out of selection").arg(fromMaPosition.x()).arg(fromMaPosition.y()));

    scrollToPosition(os, fromMaPosition);

    GTMouseDriver::dragAndDrop(convertCoordinates(os, fromMaPosition), convertCoordinates(os, toMaPosition));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "offsetsVisible"
bool GTUtilsMSAEditorSequenceArea::offsetsVisible(GUITestOpStatus& os) {
    auto leftOffsetWidget = GTWidget::findWidget(os, "msa_editor_offsets_view_widget_left");
    auto rightOffsetWidget = GTWidget::findWidget(os, "msa_editor_offsets_view_widget_right");

    GT_CHECK_RESULT(leftOffsetWidget->isVisible() == rightOffsetWidget->isVisible(), "offset widget visibility states are not the same", false);

    return leftOffsetWidget->isVisible();
}
#undef GT_METHOD_NAME
#define GT_METHOD_NAME "checkConsensus"
void GTUtilsMSAEditorSequenceArea::checkConsensus(GUITestOpStatus& os, QString cons, int index) {
    auto consArea = getConsensusArea(os, index);

    QSharedPointer<MSAEditorConsensusCache> cache = consArea->getConsensusCache();
    CHECK_SET_ERR(QString(cache->getConsensusLine(true)) == cons,
                  "Wrong consensus. Currens consensus is  " + cache->getConsensusLine(true));
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSequence"
void GTUtilsMSAEditorSequenceArea::selectSequence(GUITestOpStatus& os, const QString& seqName) {
    QStringList names = getVisibleNames(os);
    int row = 0;
    while (names[row] != seqName) {
        row++;
    }
    click(os, QPoint(-5, row));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSequence"
void GTUtilsMSAEditorSequenceArea::selectSequence(GUITestOpStatus& os, const int row) {
    GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);

    click(os, QPoint(-5, row));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSequenceSelected"
bool GTUtilsMSAEditorSequenceArea::isSequenceSelected(GUITestOpStatus& os, const QString& seqName) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);

    // Seq names are drawn on widget, so this hack is needed
    QStringList selectedRowNames;
    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    for (int maIndex : qAsConst(selectedMaRowIndexes)) {
        QString selectedSequenceName = editor->getMaObject()->getRow(maIndex)->getName();
        if (selectedSequenceName == seqName) {
            return true;
        }
    }
    return false;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedSequencesNum"
int GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum(GUITestOpStatus& os) {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);

    return msaEditArea->getEditor()->getSelection().getCountOfSelectedRows();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSequenceVisible"
bool GTUtilsMSAEditorSequenceArea::isSequenceVisible(GUITestOpStatus& os, const QString& seqName) {
    QStringList visiableRowNames = getVisibleNames(os);
    return visiableRowNames.contains(seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceData"
QString GTUtilsMSAEditorSequenceArea::getSequenceData(GUITestOpStatus& os, const QString& sequenceName) {
    MSAEditorSequenceArea* sequenceArea = getSequenceArea(os);
    GT_CHECK_RESULT(sequenceArea != nullptr, "Sequence area is NULL", "");

    const QStringList names = getNameList(os);
    const int rowNumber = names.indexOf(sequenceName);
    GT_CHECK_RESULT(rowNumber >= 0, QString("Sequence '%1' not found").arg(sequenceName), "");

    GTUtilsMsaEditor::clickSequenceName(os, sequenceName);
    GTKeyboardUtils::copy();
    return GTClipboard::text(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceData"
QString GTUtilsMSAEditorSequenceArea::getSequenceData(GUITestOpStatus& os, int rowNumber) {
    MSAEditorSequenceArea* sequenceArea = getSequenceArea(os);
    GT_CHECK_RESULT(sequenceArea != nullptr, "Sequence area is NULL", "");

    const QStringList names = getNameList(os);
    GT_CHECK_RESULT(rowNumber >= 0 && rowNumber <= names.size(), QString("Row with number %1 is out of boundaries").arg(rowNumber), "");

    GTUtilsMsaEditor::clickSequenceName(os, names[rowNumber]);
    GTKeyboardUtils::copy();
    return GTClipboard::text(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectColumnInConsensus"
void GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(GUITestOpStatus& os, int columnNumber, int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(index);
    GT_CHECK(activeWindow != nullptr, QString("Can't find MaEditorWgt %1").arg(index));
    auto msaEditArea = qobject_cast<MSAEditorSequenceArea*>(
        GTWidget::findWidget(os, "msa_editor_sequence_area", activeWindow));
    GT_CHECK_RESULT(msaEditArea != nullptr, "MsaEditorSequenceArea not found", );

    const QWidget* msaOffsetLeft = GTWidget::findWidget(os, "msa_editor_offsets_view_widget_left", activeWindow);
    GT_CHECK_RESULT(msaOffsetLeft != nullptr, "MsaOffset Left not found", );

    QPoint shift = msaOffsetLeft->mapToGlobal(QPoint(0, 0));
    if (msaOffsetLeft->isVisible()) {
        shift = msaOffsetLeft->mapToGlobal(QPoint(msaOffsetLeft->rect().right(), 0));
    }

    const int posX = msaEditArea->getEditor()->getUI()->getUI(index)->getBaseWidthController()->getBaseScreenCenter(columnNumber) + shift.x();

    auto consArea = GTUtilsMSAEditorSequenceArea::getConsensusArea(os, index);

    const int posY = consArea->mapToGlobal(consArea->rect().center()).y();
    GTMouseDriver::moveTo(QPoint(posX, posY));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasAminoAlphabet"
bool GTUtilsMSAEditorSequenceArea::hasAminoAlphabet(GUITestOpStatus& os) {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os, 0);

    return msaEditArea->hasAminoAlphabet();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSequenceHighlighted"
bool GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(GUITestOpStatus& os, const QString& seqName) {
    QStringList names = getVisibleNames(os);
    GT_CHECK_RESULT(names.contains(seqName), QString("sequence with name %1 not found").arg(seqName), false);

    int row = 0;
    while (names[row] != seqName) {
        row++;
    }
    QPoint center = convertCoordinates(os, QPoint(-5, row));

    // There are more than one name list in multiline mode, so at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(0);
    QWidget* nameList = GTWidget::findExactWidget<QWidget*>(os, "msa_editor_name_list", activeWindow);

    GT_CHECK_RESULT(nameList != nullptr, "name list is NULL", false);

    int initCoord = center.y() - getRowHeight(os, row) / 2;
    int finalCoord = center.y() + getRowHeight(os, row) / 2;

    for (int i = initCoord; i < finalCoord; i++) {
        QPoint local = nameList->mapFromGlobal(QPoint(center.x(), i));
        QColor c = GTWidget::getColor(os, nameList, local);
        QString name = c.name();
        if (name == highlightingColor) {
            return true;
        }
    }

    return false;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColor"
QString GTUtilsMSAEditorSequenceArea::getColor(GUITestOpStatus& os, QPoint p) {
    auto msaEditArea = GTWidget::findExactWidget<MSAEditorSequenceArea*>(os, "msa_editor_sequence_area", GTUtilsMsaEditor::getActiveMsaEditorWindow(os));

    QPoint global = convertCoordinates(os, p);
    global.setY(global.y() + (getRowHeight(os, p.y()) / 2 - 2));
    QPoint local = msaEditArea->mapFromGlobal(global);
    QColor c = GTWidget::getColor(os, msaEditArea, local);
    QString name = c.name();
    return name;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMsaCellColors"
void GTUtilsMSAEditorSequenceArea::checkMsaCellColors(GUITestOpStatus& os, const QPoint& pos, const QString& fgColor, const QString& bgColor) {
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::hasPixelWithColor(os, pos, fgColor), "Wrong FG color: " + fgColor + ", position: " + QString::number(pos.x()) + "," + QString::number(pos.y()));

    QString actualBgColor = GTUtilsMSAEditorSequenceArea::getColor(os, pos);
    CHECK_SET_ERR(actualBgColor == bgColor, QString("Wrong BG color! Expected: %1, got: %2").arg(bgColor).arg(actualBgColor));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMsaCellColor"
void GTUtilsMSAEditorSequenceArea::checkMsaCellColor(GUITestOpStatus& os, const QPoint& pos, const QString& color) {
    QString actualColor = GTUtilsMSAEditorSequenceArea::getColor(os, pos);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::hasPixelWithColor(os, pos, color), "Wrong color: " + color + "! Actual: " + actualColor);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasPixelWithColor"
bool GTUtilsMSAEditorSequenceArea::hasPixelWithColor(GUITestOpStatus& os, const QPoint& p, const QColor& color) {
    auto sequenceArea = GTWidget::findExactWidget<MSAEditorSequenceArea*>(os, "msa_editor_sequence_area", GTUtilsMsaEditor::getActiveMsaEditorWindow(os));
    QImage img = GTWidget::getImage(os, sequenceArea);
    QRect rect = getPositionRect(os, p);
    for (int i = rect.left(); i < rect.right(); i++) {
        for (int j = rect.top(); j < rect.bottom(); j++) {
            QPoint local = sequenceArea->mapFromGlobal(QPoint(i, j));
            QColor pixelColor = img.pixel(local);
            if (pixelColor == color) {
                return true;
            }
        }
    }
    return false;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkColor"
bool GTUtilsMSAEditorSequenceArea::checkColor(GUITestOpStatus& os, const QPoint& p, const QString& expectedColor) {
    QColor c = getColor(os, p);
    bool result = (expectedColor == c.name());
    GT_CHECK_RESULT(result, QString("wrong color. Expected: %1, actual: %2").arg(expectedColor).arg(c.name()), result);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowHeight"
int GTUtilsMSAEditorSequenceArea::getRowHeight(GUITestOpStatus& os, int rowNumber) {
    // There are more than one MaEditorWgt in multiline mode, so at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor(os)->getUI()->getUI(0);
    auto ui = qobject_cast<MsaEditorWgt*>(activeWindow);

    SAFE_POINT(ui != nullptr, "Can't find MSA editor windows", -1);
    return ui->getRowHeightController()->getRowHeightByViewRowIndex(rowNumber);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "renameSequence"
void GTUtilsMSAEditorSequenceArea::renameSequence(GUITestOpStatus& os, const QString& seqToRename, const QString& newName, bool useCopyPaste) {
    GTUtilsMsaEditor::clearSelection(os);
    GTUtilsMsaEditor::selectRowsByName(os, {seqToRename});
    GTUtilsDialog::waitForDialog(os, new RenameSequenceFiller(os, newName, seqToRename, useCopyPaste));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "replaceSymbol"
void GTUtilsMSAEditorSequenceArea::replaceSymbol(GUITestOpStatus& os, const QPoint& maPoint, char newSymbol) {
    clickToPosition(os, maPoint);
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);
    GTKeyboardDriver::keyClick(newSymbol);
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createColorScheme"
void GTUtilsMSAEditorSequenceArea::createColorScheme(GUITestOpStatus& os, const QString& colorSchemeName, const NewColorSchemeCreator::alphabet al) {
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(1, 1));
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, colorSchemeName, al));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "deleteColorScheme"
void GTUtilsMSAEditorSequenceArea::deleteColorScheme(GUITestOpStatus& os, const QString& colorSchemeName) {
    GTUtilsMSAEditorSequenceArea::moveTo(os, QPoint(1, 1));
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, colorSchemeName, NewColorSchemeCreator::nucl, NewColorSchemeCreator::Delete));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelection"
void GTUtilsMSAEditorSequenceArea::checkSelection(GUITestOpStatus& os, const QPoint& start, const QPoint& end, const QString& expected) {
    GTWidget::click(os, GTUtilsMsaEditor::getActiveMsaEditorWindow(os));
    selectArea(os, start, end);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTGlobals::sleep(500);
    QString clipboardText = GTClipboard::text(os);
    GT_CHECK(clipboardText == expected, QString("unexpected selection:\n%1").arg(clipboardText));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isAlignmentLocked"
bool GTUtilsMSAEditorSequenceArea::isAlignmentLocked(GUITestOpStatus& os) {
    MSAEditorSequenceArea* msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
    GT_CHECK_RESULT(msaSeqArea != nullptr, "MsaEditorSequenceArea is not found", false);

    return msaSeqArea->isAlignmentLocked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandSelectedRegion"
void GTUtilsMSAEditorSequenceArea::expandSelectedRegion(GUITestOpStatus& os, const int expandedBorder, const int symbolsToExpand) {
    MsaEditorWgt* ui = GTUtilsMsaEditor::getEditorUi(os);
    CHECK_SET_ERR(ui != nullptr, "MsaEditorWgt not found");

    const int height = ui->getRowHeightController()->getSingleRowHeight();
    const int width = ui->getBaseWidthController()->getBaseWidth();
    const QRect selection = GTUtilsMSAEditorSequenceArea::getSelectedRect(os);

    QPoint startPos;
    switch (expandedBorder) {
        case (0):
            startPos = QPoint(selection.center().x(), selection.top());
            break;
        case (1):
            startPos = QPoint(selection.right(), selection.center().y());
            break;
        case (2):
            startPos = QPoint(selection.center().x(), selection.bottom());
            break;
        case (3):
            startPos = QPoint(selection.left(), selection.center().y());
            break;
        case (4):
            startPos = selection.topRight();
            break;
        case (5):
            startPos = selection.bottomRight();
            break;
        case (6):
            startPos = selection.bottomLeft();
            break;
        case (7):
            startPos = selection.topLeft();
            break;
        default:
            CHECK_SET_ERR(false, QString("Unexpected movable border"));
    }

    startPos = convertCoordinates(os, startPos);

    switch (expandedBorder) {
        case (0):
            startPos = QPoint(startPos.x(), startPos.y() - height / 2);
            break;
        case (1):
            startPos = QPoint(startPos.x() + width / 2, startPos.y());
            break;
        case (2):
            startPos = QPoint(startPos.x(), startPos.y() + height / 2);
            break;
        case (3):
            startPos = QPoint(startPos.x() - width / 2, startPos.y());
            break;
        case (4):
            startPos = QPoint(startPos.x() + width / 2, startPos.y() - height / 2);
            break;
        case (5):
            startPos = QPoint(startPos.x() + width / 2, startPos.y() + height / 2);
            break;
        case (6):
            startPos = QPoint(startPos.x() - width / 2, startPos.y() + height / 2);
            break;
        case (7):
            startPos = QPoint(startPos.x() - width / 2, startPos.y() - height / 2);
            break;
    }

    GTMouseDriver::moveTo(startPos);
    GTGlobals::sleep(500);
    GTMouseDriver::press();

    QPoint endPos;
    switch (expandedBorder) {
        case (0):
        case (2):
            endPos = QPoint(startPos.x(), startPos.y() + symbolsToExpand * height);
            break;
        case (1):
        case (3):
            endPos = QPoint(startPos.x() + symbolsToExpand * width, startPos.y());
            break;
        case (4):
        case (6):
            endPos = QPoint(startPos.x() + symbolsToExpand * width, startPos.y() - symbolsToExpand * height);
            break;
        case (5):
        case (7):
            endPos = QPoint(startPos.x() + symbolsToExpand * width, startPos.y() + symbolsToExpand * height);
            break;
    }

    GTMouseDriver::moveTo(endPos);
    GTMouseDriver::release();
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

void GTUtilsMSAEditorSequenceArea::zoomIn(GUITestOpStatus& os) {
    auto zoomInButton = GTWidget::findButtonByText(os, "Zoom in");

    GTWidget::click(os, zoomInButton);
}

void GTUtilsMSAEditorSequenceArea::zoomOut(GUITestOpStatus& os) {
    auto zoomOutButton = GTWidget::findButtonByText(os, "Zoom out");

    GTWidget::click(os, zoomOutButton);
}

void GTUtilsMSAEditorSequenceArea::zoomToMax(GUITestOpStatus& os) {
    auto zoomInButton = GTWidget::findButtonByText(os, "Zoom in");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(os, zoomInButton);
    }
}

void GTUtilsMSAEditorSequenceArea::zoomToMin(GUITestOpStatus& os) {
    auto zoomOutButton = GTWidget::findButtonByText(os, "Zoom out");

    while (zoomOutButton->isEnabled()) {
        GTWidget::click(os, zoomOutButton);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
