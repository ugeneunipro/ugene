/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
#include <U2View/MaEditorSelection.h>
#include <U2View/MsaEditor.h>
#include <U2View/MsaEditorConsensusArea.h>
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

int GTUtilsMSAEditorSequenceArea::getBaseWidth() {
    return GTUtilsMsaEditor::getEditor()->getLineWidget(0)->getBaseWidthController()->getBaseWidth();
}

MsaEditorSequenceArea* GTUtilsMSAEditorSequenceArea::getSequenceArea(int index, bool checkError) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    if (checkError) {
        CHECK_SET_ERR_RESULT(activeWindow != nullptr,
                             QString("Can't find sequence area #%1").arg(index),
                             nullptr);
    }
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<MsaEditorSequenceArea*>("msa_editor_sequence_area", activeWindow);
}

MsaEditorConsensusArea* GTUtilsMSAEditorSequenceArea::getConsensusArea(int index, bool checkError) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    if (checkError) {
        CHECK_SET_ERR_RESULT(activeWindow != nullptr,
                             QString("Can't find consensus area #%1").arg(index),
                             nullptr);
    }
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<MsaEditorConsensusArea*>("consArea", activeWindow);
}

QScrollBar* GTUtilsMSAEditorSequenceArea::getHorizontalNamesScroll(int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<QScrollBar*>("horizontal_names_scroll", activeWindow);
}

MsaEditorSimilarityColumn* GTUtilsMSAEditorSequenceArea::getSimilarityColumn(int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    if (activeWindow == nullptr) {
        return nullptr;
    }
    return GTWidget::findExactWidget<MsaEditorSimilarityColumn*>("msa_editor_similarity_column", activeWindow);
}

void GTUtilsMSAEditorSequenceArea::callContextMenu(const QPoint& innerCoords) {
    if (innerCoords.isNull()) {
        GTWidget::click(getSequenceArea(), Qt::RightButton);
    } else {
        moveTo(innerCoords);
        GTMouseDriver::click(Qt::RightButton);
    }
}

void GTUtilsMSAEditorSequenceArea::moveTo(const QPoint& p) {
    QPoint convP = convertCoordinates(p);

    GTMouseDriver::moveTo(convP);
}

QRect GTUtilsMSAEditorSequenceArea::getPositionRect(
    const QPoint& position,
    int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #index widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    GT_CHECK_RESULT(activeWindow != nullptr, QString("Can't find MaEditorWgt %1").arg(index), QRect());
    auto msaEditArea = GTWidget::findExactWidget<MsaEditorSequenceArea*>("msa_editor_sequence_area", activeWindow);

    auto msaEditorWidget = qobject_cast<MsaEditorWgt*>(msaEditArea->getEditor()->getLineWidget(index));
    U2Region regX = msaEditorWidget->getBaseWidthController()->getBaseGlobalRange(position.x());
    U2Region regY = msaEditorWidget->getRowHeightController()->getGlobalYRegionByViewRowIndex(position.y());

    QPoint topLeftGlobalPoint = msaEditArea->mapToGlobal(QPoint(regX.startPos, regY.startPos));
    QPoint bottomRightGlobalPoint = msaEditArea->mapToGlobal(QPoint(regX.endPos(), regY.endPos()));

    return {topLeftGlobalPoint, bottomRightGlobalPoint};
}

QPoint GTUtilsMSAEditorSequenceArea::convertCoordinates(const QPoint& p, int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    GT_CHECK_RESULT(activeWindow != nullptr, QString("Can't find MaEditorWgt %1").arg(index), QPoint());
    auto msaEditArea = GTWidget::findExactWidget<MsaEditorSequenceArea*>("msa_editor_sequence_area", activeWindow);

    auto ui = qobject_cast<MsaEditorWgt*>(msaEditArea->getEditor()->getLineWidget(index));
    int posX = ui->getBaseWidthController()->getBaseScreenCenter(p.x());
    int posY = (int)ui->getRowHeightController()->getScreenYRegionByViewRowIndex(p.y()).center();
    return msaEditArea->mapToGlobal({posX, posY});
}

void GTUtilsMSAEditorSequenceArea::selectArea(
    int multilineIndex,
    QPoint p1,
    QPoint p2,
    GTGlobals::UseMethod method) {
    auto sequenceArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(multilineIndex);
    CHECK_SET_ERR_RESULT(sequenceArea != nullptr,
                         QString("Can't find sequence area #%1").arg(multilineIndex), );

    p1.rx() = (p1.x() == -1 ? sequenceArea->getNumVisibleBases() - 1 : p1.x());
    p1.ry() = (p1.y() == -1 ? sequenceArea->getViewRowCount() - 1 : p1.y());

    p2.rx() = (p2.x() == -1 ? sequenceArea->getNumVisibleBases() - 1 : p2.x());
    p2.ry() = (p2.y() == -1 ? sequenceArea->getViewRowCount() - 1 : p2.y());

    switch (method) {
        case GTGlobals::UseKey:
            clickToPosition(p1);  // Make initial selection.
            moveMouseToPosition(p2);  // Scroll to the end.
            GTKeyboardDriver::keyPress(Qt::Key_Shift);  // Make the final selection.
            GTMouseDriver::click();
            GTKeyboardDriver::keyRelease(Qt::Key_Shift);
            break;
        case GTGlobals::UseMouse:
            GTMouseDriver::dragAndDrop(convertCoordinates(p1), convertCoordinates(p2));
            break;
        case GTGlobals::UseKeyBoard:
            GT_FAIL("Not implemented", );
        default:
            GT_FAIL("An unknown method", );
    }
}

void GTUtilsMSAEditorSequenceArea::selectArea(QPoint p1, QPoint p2, GTGlobals::UseMethod method) {
    return selectArea(0, p1, p2, method);
}

void GTUtilsMSAEditorSequenceArea::cancelSelection() {
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

void GTUtilsMSAEditorSequenceArea::click(const QPoint& screenMaPoint) {
    GTMouseDriver::moveTo(convertCoordinates(screenMaPoint));
    GTMouseDriver::click();
}

void GTUtilsMSAEditorSequenceArea::copySelectionByContextMenu() {
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();
}

void GTUtilsMSAEditorSequenceArea::scrollToPosition(const QPoint& position) {
    if (GTUtilsMsaEditor::getMultilineMode()) {
        class ScrollInMainThread : public CustomScenario {
        public:
            ScrollInMainThread(const QPoint& _p)
                : p(_p) {
            }
            void run() override {
                auto ui = GTUtilsMsaEditor::getEditor()->getMainWidget();
                ui->getScrollController()->scrollToPoint(p);
            }
            QPoint p;
        };
        GTThread::runInMainThread(new ScrollInMainThread(position));
        return;
    }
    auto msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    CHECK_SET_ERR_RESULT(msaSeqArea != nullptr,
                         QString("Can't find sequence area #%1").arg(0), );
    GT_CHECK(msaSeqArea->isInRange(position),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(position.x())
                 .arg(position.y())
                 .arg(msaSeqArea->getEditor()->getAlignmentLen())
                 .arg(msaSeqArea->getViewRowCount()));

    // Scroll down with a vertical scroll bar first.
    MaEditorWgt* msaWidget = GTUtilsMsaEditor::getEditor()->getLineWidget(0);
    auto verticalScrollBar = GTWidget::findScrollBar("vertical_sequence_scroll", msaWidget);

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
    auto horizontalScrollBar = GTWidget::findScrollBar("horizontal_sequence_scroll", msaWidget);

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

void GTUtilsMSAEditorSequenceArea::scrollToBottom() {
    // scroll down
    auto vBar = GTWidget::findScrollBar("vertical_sequence_scroll", GTUtilsMsaEditor::getActiveMsaEditorWindow());
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

void GTUtilsMSAEditorSequenceArea::moveMouseToPosition(const QPoint& globalMaPosition) {
    auto msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    CHECK_SET_ERR_RESULT(msaSeqArea != nullptr,
                         QString("Can't find sequence area #%1").arg(0), );
    GT_CHECK(msaSeqArea->isInRange(globalMaPosition),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(globalMaPosition.x())
                 .arg(globalMaPosition.y())
                 .arg(msaSeqArea->getEditor()->getAlignmentLen())
                 .arg(msaSeqArea->getViewRowCount()));

    auto ui = GTUtilsMsaEditor::getEditor()->getMainWidget();
    QPoint positionCenter;
    uint muiCount = msaSeqArea->getEditor()->getMainWidget()->getLineWidgetCount();
    uint multilineIndex = 0;
    scrollToPosition(globalMaPosition);
    do {
        msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(multilineIndex);
        MaEditorWgt* mui = ui->getLineWidget(multilineIndex);
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

void GTUtilsMSAEditorSequenceArea::clickToPosition(const QPoint& globalMaPosition) {
    GTUtilsMSAEditorSequenceArea::moveMouseToPosition(globalMaPosition);
    GTMouseDriver::click();
}

void GTUtilsMSAEditorSequenceArea::checkSelectedRect(
    int multilineIndex,
    const QRect& expectedRect) {
    auto msaEditArea = getSequenceArea(multilineIndex);

    QRect msaEditRegion = msaEditArea->getEditor()->getSelection().toRect();
    CHECK_SET_ERR(expectedRect == msaEditRegion, QString("Unexpected selection region. Expected: [(%1,%2) (%3,%4)]. Actual: [(%5,%6) (%7,%8)]").arg(expectedRect.topLeft().x()).arg(expectedRect.topLeft().y()).arg(expectedRect.bottomRight().x()).arg(expectedRect.bottomRight().y()).arg(msaEditRegion.topLeft().x()).arg(msaEditRegion.topLeft().y()).arg(msaEditRegion.bottomRight().x()).arg(msaEditRegion.bottomRight().y()));
}

void GTUtilsMSAEditorSequenceArea::checkSelectedRect(const QRect& expectedRect) {
    checkSelectedRect(0, expectedRect);
}

QStringList GTUtilsMSAEditorSequenceArea::getNameList() {
    MsaEditor* editor = GTUtilsMsaEditor::getEditor();
    QStringList result = editor->getMaObject()->getAlignment()->getRowNames();
    return result;
}

QStringList GTUtilsMSAEditorSequenceArea::getCurrentRowNames() {
    MsaEditor* editor = GTUtilsMsaEditor::getEditor();
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    int viewRowCount = collapseModel->getViewRowCount();
    QStringList rowNameList;
    for (int viewRowIndex = 0; viewRowIndex < viewRowCount; viewRowIndex++) {
        int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
        MsaRow maRow = editor->getMaObject()->getRow(maRowIndex);
        rowNameList << maRow->getName();
    }
    return rowNameList;
}

bool GTUtilsMSAEditorSequenceArea::hasSequencesWithNames(const QStringList& names) {
    QStringList nameList = getNameList();
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

QStringList GTUtilsMSAEditorSequenceArea::getVisibleNames(bool asShownInNameList) {
    MsaEditor* editor = GTUtilsMsaEditor::getEditor();
    MaEditorNameList* nameListArea = GTUtilsMsaEditor::getNameListArea();
    CHECK_SET_ERR_RESULT(nameListArea != nullptr, "MSA Editor name list area is NULL", QStringList());

    QList<int> visibleRowsIndexes = editor->getLineWidget(0)->getDrawHelper()->getVisibleMaRowIndexes(nameListArea->height());
    MsaObject* msaObject = editor->getMaObject();

    QStringList visibleRowNames;
    for (int rowIndex : qAsConst(visibleRowsIndexes)) {
        QString name = asShownInNameList ? nameListArea->getTextForRow(rowIndex) : msaObject->getRow(rowIndex)->getName();
        visibleRowNames << name;
    }
    return visibleRowNames;
}

void GTUtilsMSAEditorSequenceArea::removeSequence(const QString& sequenceName) {
    selectSequence(sequenceName);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTThread::waitForMainThread();
}

QString GTUtilsMSAEditorSequenceArea::getSimilarityValue(int row, int index) {
    // bad solution, but better then nothing
    auto simCol = GTUtilsMSAEditorSequenceArea::getSimilarityColumn(index);

    return simCol->getTextForRow(row);
}

void GTUtilsMSAEditorSequenceArea::clickCollapseTriangle(const QString& seqName) {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);
    auto msaEditWgt = msaEditArea->getEditor()->getLineWidget(0);

    int rowNum = getVisibleNames().indexOf(seqName);
    GT_CHECK(rowNum != -1, "sequence not found in nameList");
    auto nameList = GTWidget::findExactWidget<QWidget*>("msa_editor_name_list", msaEditWgt);
    QPoint localCoord = QPoint(15, msaEditWgt->getRowHeightController()->getScreenYRegionByViewRowIndex(rowNum).startPos + 7);
    QPoint globalCoord = nameList->mapToGlobal(localCoord);
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
}

bool GTUtilsMSAEditorSequenceArea::isCollapsed(const QString& seqName) {
    QStringList names = getNameList();
    QStringList visiable = getVisibleNames();
    GT_CHECK_RESULT(names.contains(seqName), "sequence " + seqName + " not found", false);
    return !visiable.contains(seqName);
}

bool GTUtilsMSAEditorSequenceArea::collapsingMode() {
    QAbstractButton* toggleSequenceOrderButton = GTAction::button("toggle_sequence_row_order_action");
    bool nameListsAreEqual = getVisibleNames() == getNameList();
    if (nameListsAreEqual && !toggleSequenceOrderButton->isChecked()) {
        return false;
    } else if (!nameListsAreEqual && toggleSequenceOrderButton->isChecked()) {
        return true;
    }
    GT_CHECK_RESULT(false, "something wrong with collapsing mode", false);
}

int GTUtilsMSAEditorSequenceArea::getFirstVisibleBaseIndex(int multilineIndex, bool countClipped) {
    auto msaEditArea = getSequenceArea(multilineIndex);

    ScrollController* scrollController = msaEditArea->getEditor()->getLineWidget(multilineIndex)->getScrollController();
    return scrollController->getFirstVisibleBase(countClipped);
}

int GTUtilsMSAEditorSequenceArea::getLastVisibleBaseIndex(int multilineIndex) {
    auto msaEditArea = getSequenceArea(multilineIndex);

    ScrollController* scrollController = msaEditArea->getEditor()->getLineWidget(multilineIndex)->getScrollController();
    int clippedIdx = scrollController->getLastVisibleBase(msaEditArea->width(), true);
    int notClippedIdx = scrollController->getLastVisibleBase(msaEditArea->width(), false);
    return clippedIdx + (clippedIdx == notClippedIdx ? 0 : 1);
}

int GTUtilsMSAEditorSequenceArea::getFirstVisibleRowIndex(bool countClipped) {
    MsaEditor* editor = GTUtilsMsaEditor::getEditor();
    return editor->getMainWidget()->getScrollController()->getFirstVisibleViewRowIndex(countClipped);
}

int GTUtilsMSAEditorSequenceArea::getLastVisibleRowIndex(bool countClipped) {
    MsaEditor* editor = GTUtilsMsaEditor::getEditor();
    int widgetHeight = editor->getLineWidget(0)->getSequenceArea()->height();
    return editor->getMainWidget()->getScrollController()->getLastVisibleViewRowIndex(widgetHeight, countClipped);
}

int GTUtilsMSAEditorSequenceArea::getLength() {
    auto statusWidget = GTWidget::findWidget("msa_editor_status_bar");
    return GTMSAEditorStatusWidget::length(statusWidget);
}

int GTUtilsMSAEditorSequenceArea::getNumVisibleBases() {
    auto msaEditArea = GTWidget::findExactWidget<MsaEditorSequenceArea*>("msa_editor_sequence_area", GTUtilsMsaEditor::getActiveMsaEditorWindow());
    return msaEditArea->getEditor()->getLineWidget(0)->getDrawHelper()->getVisibleBasesCount(msaEditArea->width());
}

QRect GTUtilsMSAEditorSequenceArea::getSelectedRect() {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    return msaEditArea->getEditor()->getSelection().toRect();
}

void GTUtilsMSAEditorSequenceArea::dragAndDropSelection(const QPoint& fromMaPosition, const QPoint& toMaPosition) {
    const MaEditorSelection& selection = getSequenceArea()->getEditor()->getSelection();
    GT_CHECK(selection.contains(fromMaPosition), QString("Position (%1, %2) is out of selection").arg(fromMaPosition.x()).arg(fromMaPosition.y()));

    scrollToPosition(fromMaPosition);

    GTMouseDriver::dragAndDrop(convertCoordinates(fromMaPosition), convertCoordinates(toMaPosition));
}

bool GTUtilsMSAEditorSequenceArea::offsetsVisible() {
    auto leftOffsetWidget = GTWidget::findWidget("msa_editor_offsets_view_widget_left");
    auto rightOffsetWidget = GTWidget::findWidget("msa_editor_offsets_view_widget_right");

    GT_CHECK_RESULT(leftOffsetWidget->isVisible() == rightOffsetWidget->isVisible(), "offset widget visibility states are not the same", false);

    return leftOffsetWidget->isVisible();
}
void GTUtilsMSAEditorSequenceArea::checkConsensus(const QString& cons, int index) {
    auto consArea = getConsensusArea(index);

    QSharedPointer<MsaEditorConsensusCache> cache = consArea->getConsensusCache();
    CHECK_SET_ERR(QString(cache->getConsensusLine(true)) == cons,
                  "Wrong consensus. Current consensus is  " + cache->getConsensusLine(true));
}

void GTUtilsMSAEditorSequenceArea::selectSequence(const QString& seqName) {
    QStringList names = getVisibleNames();
    int row = 0;
    while (names[row] != seqName) {
        row++;
    }
    click(QPoint(-5, row));
}

void GTUtilsMSAEditorSequenceArea::selectSequence(const int row) {
    GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    click(QPoint(-5, row));
}

bool GTUtilsMSAEditorSequenceArea::isSequenceSelected(const QString& seqName) {
    MsaEditor* editor = GTUtilsMsaEditor::getEditor();

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

int GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum() {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    return msaEditArea->getEditor()->getSelection().getCountOfSelectedRows();
}

bool GTUtilsMSAEditorSequenceArea::isSequenceVisible(const QString& seqName) {
    QStringList visiableRowNames = getVisibleNames();
    return visiableRowNames.contains(seqName);
}

QString GTUtilsMSAEditorSequenceArea::getSequenceData(const QString& sequenceName) {
    MsaEditorSequenceArea* sequenceArea = getSequenceArea();
    GT_CHECK_RESULT(sequenceArea != nullptr, "Sequence area is NULL", "");

    const QStringList names = getNameList();
    const int rowNumber = names.indexOf(sequenceName);
    GT_CHECK_RESULT(rowNumber >= 0, QString("Sequence '%1' not found").arg(sequenceName), "");

    GTUtilsMsaEditor::clickSequenceName(sequenceName);
    GTKeyboardUtils::copy();
    return GTClipboard::text();
}

QString GTUtilsMSAEditorSequenceArea::getSequenceData(int rowNumber) {
    MsaEditorSequenceArea* sequenceArea = getSequenceArea();
    GT_CHECK_RESULT(sequenceArea != nullptr, "Sequence area is NULL", "");

    const QStringList names = getNameList();
    GT_CHECK_RESULT(rowNumber >= 0 && rowNumber <= names.size(), QString("Row with number %1 is out of boundaries").arg(rowNumber), "");

    GTUtilsMsaEditor::clickSequenceName(names[rowNumber]);
    GTKeyboardUtils::copy();
    return GTClipboard::text();
}

void GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(int columnNumber, int index) {
    // There are more than one msa_editor_sequence_area in multiline mode, so
    // at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(index);
    GT_CHECK(activeWindow != nullptr, QString("Can't find MaEditorWgt %1").arg(index));
    auto msaEditArea = qobject_cast<MsaEditorSequenceArea*>(
        GTWidget::findWidget("msa_editor_sequence_area", activeWindow));
    GT_CHECK_RESULT(msaEditArea != nullptr, "MsaEditorSequenceArea not found", );

    const QWidget* msaOffsetLeft = GTWidget::findWidget("msa_editor_offsets_view_widget_left", activeWindow);
    GT_CHECK_RESULT(msaOffsetLeft != nullptr, "MsaOffset Left not found", );

    QPoint shift = msaOffsetLeft->mapToGlobal(QPoint(0, 0));
    if (msaOffsetLeft->isVisible()) {
        shift = msaOffsetLeft->mapToGlobal(QPoint(msaOffsetLeft->rect().right(), 0));
    }

    const int posX = msaEditArea->getEditor()->getLineWidget(index)->getBaseWidthController()->getBaseScreenCenter(columnNumber) + shift.x();

    auto consArea = GTUtilsMSAEditorSequenceArea::getConsensusArea(index);

    const int posY = consArea->mapToGlobal(consArea->rect().center()).y();
    GTMouseDriver::moveTo(QPoint(posX, posY));
    GTMouseDriver::click();
}

bool GTUtilsMSAEditorSequenceArea::hasAminoAlphabet() {
    auto msaEditArea = GTUtilsMSAEditorSequenceArea::getSequenceArea(0);

    return msaEditArea->hasAminoAlphabet();
}

bool GTUtilsMSAEditorSequenceArea::isSequenceHighlighted(const QString& seqName) {
    QStringList names = getVisibleNames();
    GT_CHECK_RESULT(names.contains(seqName), QString("sequence with name %1 not found").arg(seqName), false);

    int row = 0;
    while (names[row] != seqName) {
        row++;
    }

    QPoint center = GTUtilsMsaEditor::getSequenceNameRect(row).center();

    // There are more than one name list in multiline mode, so at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(0);
    auto nameList = GTWidget::findExactWidget<QWidget*>("msa_editor_name_list", activeWindow);
    GT_CHECK_RESULT(nameList != nullptr, "name list is NULL", false);

    int initCoord = center.y() - getRowHeight(row) / 2;
    int finalCoord = center.y() + getRowHeight(row) / 2;

    for (int i = initCoord; i < finalCoord; i++) {
        QPoint local = nameList->mapFromGlobal(QPoint(center.x(), i));
        QColor c = GTWidget::getColor(nameList, local);
        QString name = c.name();
        if (name == highlightingColor) {
            return true;
        }
    }

    return false;
}

QString GTUtilsMSAEditorSequenceArea::getColor(QPoint p) {
    auto msaEditArea = GTWidget::findExactWidget<MsaEditorSequenceArea*>("msa_editor_sequence_area", GTUtilsMsaEditor::getActiveMsaEditorWindow());

    QPoint global = convertCoordinates(p);
    global.setY(global.y() + (getRowHeight(p.y()) / 2 - 2));
    QPoint local = msaEditArea->mapFromGlobal(global);
    QColor c = GTWidget::getColor(msaEditArea, local);
    QString name = c.name();
    return name;
}

void GTUtilsMSAEditorSequenceArea::checkMsaCellColors(const QPoint& pos, const QString& fgColor, const QString& bgColor) {
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::hasPixelWithColor(pos, fgColor), "Wrong FG color: " + fgColor + ", position: " + QString::number(pos.x()) + "," + QString::number(pos.y()));

    QString actualBgColor = GTUtilsMSAEditorSequenceArea::getColor(pos);
    CHECK_SET_ERR(actualBgColor == bgColor, QString("Wrong BG color! Expected: %1, got: %2").arg(bgColor).arg(actualBgColor));
}

void GTUtilsMSAEditorSequenceArea::checkMsaCellColor(const QPoint& pos, const QString& color) {
    QString actualColor = GTUtilsMSAEditorSequenceArea::getColor(pos);
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::hasPixelWithColor(pos, color), "Wrong color: " + color + "! Actual: " + actualColor);
}

bool GTUtilsMSAEditorSequenceArea::hasPixelWithColor(const QPoint& p, const QColor& color) {
    auto sequenceArea = GTWidget::findExactWidget<MsaEditorSequenceArea*>("msa_editor_sequence_area", GTUtilsMsaEditor::getActiveMsaEditorWindow());
    QImage img = GTWidget::getImage(sequenceArea);
    QRect rect = getPositionRect(p);
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

bool GTUtilsMSAEditorSequenceArea::checkColor(const QPoint& p, const QString& expectedColor) {
    QColor c = getColor(p);
    bool result = (expectedColor == c.name());
    GT_CHECK_RESULT(result, QString("wrong color. Expected: %1, actual: %2").arg(expectedColor).arg(c.name()), result);
    return result;
}

int GTUtilsMSAEditorSequenceArea::getRowHeight(int rowNumber) {
    // There are more than one MaEditorWgt in multiline mode, so at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getLineWidget(0);
    auto ui = qobject_cast<MsaEditorWgt*>(activeWindow);

    SAFE_POINT(ui != nullptr, "Can't find MSA editor windows", -1);
    return ui->getRowHeightController()->getRowHeightByViewRowIndex(rowNumber);
}

void GTUtilsMSAEditorSequenceArea::renameSequence(const QString& seqToRename, const QString& newName, bool useCopyPaste) {
    GTUtilsMsaEditor::clearSelection();
    GTUtilsMsaEditor::selectRowsByName({seqToRename});
    GTUtilsDialog::waitForDialog(new RenameSequenceFiller(newName, seqToRename, useCopyPaste));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

void GTUtilsMSAEditorSequenceArea::replaceSymbol(const QPoint& maPoint, char newSymbol) {
    clickToPosition(maPoint);
    GTKeyboardDriver::keyClick('r', Qt::ShiftModifier);
    GTKeyboardDriver::keyClick(newSymbol);
    GTThread::waitForMainThread();
}

void GTUtilsMSAEditorSequenceArea::createColorScheme(const QString& colorSchemeName, const NewColorSchemeCreator::alphabet& al) {
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(1, 1));
    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator(colorSchemeName, al));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();
}

void GTUtilsMSAEditorSequenceArea::deleteColorScheme(const QString& colorSchemeName) {
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(1, 1));
    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator(colorSchemeName, NewColorSchemeCreator::nucl, NewColorSchemeCreator::Delete));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_APPEARANCE, "Colors", "Custom schemes", "Create new color scheme"}));
    GTMouseDriver::click(Qt::RightButton);
}

void GTUtilsMSAEditorSequenceArea::checkSelection(const QPoint& start, const QPoint& end, const QString& expected) {
    GTWidget::click(GTUtilsMsaEditor::getActiveMsaEditorWindow());
    selectArea(start, end);
    GTKeyboardUtils::copy();
    GTGlobals::sleep(500);
    QString clipboardText = GTClipboard::text();
    GT_CHECK(clipboardText == expected, QString("unexpected selection:\n%1").arg(clipboardText));
}

bool GTUtilsMSAEditorSequenceArea::isAlignmentLocked() {
    MsaEditorSequenceArea* msaSeqArea = GTUtilsMSAEditorSequenceArea::getSequenceArea();
    GT_CHECK_RESULT(msaSeqArea != nullptr, "MsaEditorSequenceArea is not found", false);

    return msaSeqArea->isAlignmentLocked();
}

void GTUtilsMSAEditorSequenceArea::expandSelectedRegion(const int expandedBorder, const int symbolsToExpand) {
    MsaEditorWgt* ui = GTUtilsMsaEditor::getEditorUi();
    CHECK_SET_ERR(ui != nullptr, "MsaEditorWgt not found");

    const int height = ui->getRowHeightController()->getSingleRowHeight();
    const int width = ui->getBaseWidthController()->getBaseWidth();
    const QRect selection = GTUtilsMSAEditorSequenceArea::getSelectedRect();

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

    startPos = convertCoordinates(startPos);

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

void GTUtilsMSAEditorSequenceArea::zoomIn() {
    auto zoomInButton = GTWidget::findButtonByText("Zoom in");

    GTWidget::click(zoomInButton);
}

void GTUtilsMSAEditorSequenceArea::zoomOut() {
    auto zoomOutButton = GTWidget::findButtonByText("Zoom out");

    GTWidget::click(zoomOutButton);
}

void GTUtilsMSAEditorSequenceArea::zoomToMax() {
    auto zoomInButton = GTWidget::findButtonByText("Zoom in");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(zoomInButton);
    }
}

void GTUtilsMSAEditorSequenceArea::zoomToMin() {
    auto zoomOutButton = GTWidget::findButtonByText("Zoom out");

    while (zoomOutButton->isEnabled()) {
        GTWidget::click(zoomOutButton);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
