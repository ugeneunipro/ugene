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

#include <drivers/GTMouseDriver.h>
#include <primitives/GTScrollBar.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/BaseWidthController.h>
#include <U2View/DrawHelper.h>
#include <U2View/McaEditor.h>
#include <U2View/McaEditorConsensusArea.h>
#include <U2View/McaEditorNameList.h>
#include <U2View/McaEditorReferenceArea.h>
#include <U2View/McaEditorSequenceArea.h>
#include <U2View/RowHeightController.h>
#include <U2View/SequenceObjectContext.h>

#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMcaEditorSequenceArea"

#define GT_METHOD_NAME "getSequenceArea"
McaEditorSequenceArea *GTUtilsMcaEditorSequenceArea::getSequenceArea(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMcaEditor::getActiveMcaEditorWindow(os);
    McaEditorSequenceArea *result = qobject_cast<McaEditorSequenceArea *>(GTWidget::findWidget(os, "mca_editor_sequence_area", activeWindow));
    GT_CHECK_RESULT(NULL != result, "MsaEditorSequenceArea is not found", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getVisibleNames"
QStringList GTUtilsMcaEditorSequenceArea::getVisibleNames(GUITestOpStatus &os) {
    McaEditor *editor = GTUtilsMcaEditor::getEditor(os);
    McaEditorNameList *nameListArea = GTUtilsMcaEditor::getNameListArea(os);
    CHECK_SET_ERR_RESULT(NULL != nameListArea, "Mca Editor name list area is NULL", QStringList());

    const QList<int> visibleRowsIndexes = editor->getUI()->getDrawHelper()->getVisibleMaRowIndexes(
        nameListArea->height());

    QStringList visibleRowNames;
    foreach (const int rowIndex, visibleRowsIndexes) {
        visibleRowNames << editor->getMaObject()->getRow(rowIndex)->getName();
    }
    return visibleRowNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowHeight"
int GTUtilsMcaEditorSequenceArea::getRowHeight(GUITestOpStatus &os, int rowNumber) {
    McaEditorWgt *ui = GTUtilsMcaEditor::getEditorUi(os);
    return ui->getRowHeightController()->getRowHeightByViewRowIndex(rowNumber);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickToPosition"
void GTUtilsMcaEditorSequenceArea::clickToPosition(GUITestOpStatus &os, const QPoint &globalMaPosition) {
    McaEditorSequenceArea *mcaSeqArea = GTWidget::findExactWidget<McaEditorSequenceArea *>(os, "mca_editor_sequence_area", GTUtilsMcaEditor::getActiveMcaEditorWindow(os));
    GT_CHECK(mcaSeqArea->isInRange(globalMaPosition),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(globalMaPosition.x())
                 .arg(globalMaPosition.y())
                 .arg(mcaSeqArea->getEditor()->getAlignmentLen())
                 .arg(mcaSeqArea->getViewRowCount()));

    scrollToPosition(os, globalMaPosition);
    GTGlobals::sleep();

    BaseWidthController *widthController = mcaSeqArea->getEditor()->getUI()->getBaseWidthController();
    RowHeightController *heightController = mcaSeqArea->getEditor()->getUI()->getRowHeightController();
    QPoint positionCenter(widthController->getBaseScreenCenter(globalMaPosition.x()),
                          heightController->getScreenYRegionByViewRowIndex(globalMaPosition.y()).center());
    GT_CHECK(mcaSeqArea->rect().contains(positionCenter, false), "Position is not visible");

    GTMouseDriver::moveTo(mcaSeqArea->mapToGlobal(positionCenter));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToPosition"
void GTUtilsMcaEditorSequenceArea::scrollToPosition(GUITestOpStatus &os, const QPoint &position) {
    McaEditorSequenceArea *mcaSeqArea = GTWidget::findExactWidget<McaEditorSequenceArea *>(os, "mca_editor_sequence_area", GTUtilsMcaEditor::getActiveMcaEditorWindow(os));
    GT_CHECK(mcaSeqArea->isInRange(position),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(position.x())
                 .arg(position.y())
                 .arg(mcaSeqArea->getEditor()->getAlignmentLen())
                 .arg(mcaSeqArea->getViewRowCount()));

    CHECK(!mcaSeqArea->isVisible(position, false), );

    if (!mcaSeqArea->isRowVisible(position.y(), false)) {
        GTUtilsMcaEditor::scrollToRead(os, position.y());
    }
    GTThread::waitForMainThread();

    if (!mcaSeqArea->isPositionVisible(position.x(), false)) {
        scrollToBase(os, position.x());
    }
    GTThread::waitForMainThread();

    CHECK_SET_ERR(mcaSeqArea->isVisible(position, false),
                  QString("The position is still invisible after scrolling: (%1, %2)").arg(position.x()).arg(position.y()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToBase"
void GTUtilsMcaEditorSequenceArea::scrollToBase(GUITestOpStatus &os, int position) {
    BaseWidthController *widthController = GTUtilsMcaEditor::getEditorUi(os)->getBaseWidthController();
    int scrollBarValue = widthController->getBaseGlobalRange(position).center() -
                         GTUtilsMcaEditor::getEditorUi(os)->getSequenceArea()->width() / 2;
    GTScrollBar::moveSliderWithMouseToValue(os, GTUtilsMcaEditor::getHorizontalScrollBar(os), scrollBarValue);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCollapseTriangle"
void GTUtilsMcaEditorSequenceArea::clickCollapseTriangle(GUITestOpStatus &os, QString rowName, bool showChromatogram) {
    McaEditorSequenceArea *mcaEditArea = qobject_cast<McaEditorSequenceArea *>(GTWidget::findWidget(os, "mca_editor_sequence_area"));
    GT_CHECK(mcaEditArea != nullptr, "McaEditorSequenceArea not found");

    int viewRowIndex = getVisibleNames(os).indexOf(rowName);
    GT_CHECK(viewRowIndex != -1, "sequence not found in nameList");
    QWidget *nameList = GTWidget::findWidget(os, "mca_editor_name_list");
    RowHeightController *rowHeightController = mcaEditArea->getEditor()->getUI()->getRowHeightController();
    int yPos = rowHeightController->getScreenYRegionByViewRowIndex(viewRowIndex).startPos + rowHeightController->getRowHeightByViewRowIndex(viewRowIndex) / 2;
    if (showChromatogram) {
        yPos -= 65;
    }
    QPoint localCoord = QPoint(15, yPos);
    QPoint globalCoord = nameList->mapToGlobal(localCoord);
    GTMouseDriver::moveTo(globalCoord);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isChromatogramShown"
bool GTUtilsMcaEditorSequenceArea::isChromatogramShown(GUITestOpStatus &os, QString rowName) {
    GTThread::waitForMainThread();
    McaEditorSequenceArea *mcaEditArea = qobject_cast<McaEditorSequenceArea *>(GTWidget::findWidget(os, "mca_editor_sequence_area"));
    GT_CHECK_RESULT(mcaEditArea != NULL, "McaEditorSequenceArea not found", false);
    int rowNum = GTUtilsMcaEditor::getReadsNames(os).indexOf(rowName);
    GT_CHECK_RESULT(rowNum != -1, "sequence not found in nameList", false);
    int rowHeight = mcaEditArea->getEditor()->getUI()->getRowHeightController()->getRowHeightByViewRowIndex(rowNum);
    bool isChromatogramShown = rowHeight > 100;
    return isChromatogramShown;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameList"
QStringList GTUtilsMcaEditorSequenceArea::getNameList(GUITestOpStatus &os) {
    QMainWindow *mw = AppContext::getMainWindow()->getQMainWindow();
    McaEditor *editor = mw->findChild<McaEditor *>();
    CHECK_SET_ERR_RESULT(editor != NULL, "MsaEditor not found", QStringList());

    QStringList result = editor->getMaObject()->getMultipleAlignment()->getRowNames();

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenu"
void GTUtilsMcaEditorSequenceArea::callContextMenu(GUITestOpStatus &os, const QPoint &innerCoords) {
    if (innerCoords.isNull()) {
        GTWidget::click(os, getSequenceArea(os), Qt::RightButton);
    } else {
        moveTo(os, innerCoords);
        GTMouseDriver::click(Qt::RightButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveTo"
void GTUtilsMcaEditorSequenceArea::moveTo(GUITestOpStatus &os, const QPoint &p) {
    QPoint convP = convertCoordinates(os, p);

    GTMouseDriver::moveTo(convP);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "convertCoordinates"
QPoint GTUtilsMcaEditorSequenceArea::convertCoordinates(GUITestOpStatus &os, const QPoint p) {
    QWidget *activeWindow = GTUtilsMcaEditor::getActiveMcaEditorWindow(os);
    McaEditorSequenceArea *mcaEditArea = qobject_cast<McaEditorSequenceArea *>(GTWidget::findWidget(os, "mca_editor_sequence_area", activeWindow));

    const int posX = static_cast<int>(mcaEditArea->getEditor()->getUI()->getBaseWidthController()->getBaseGlobalRange(p.x()).center());
    const int posY = static_cast<int>(mcaEditArea->getEditor()->getUI()->getRowHeightController()->getGlobalYRegionByViewRowIndex(p.y()).center());
    return mcaEditArea->mapToGlobal(QPoint(posX, posY));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceReg"
QString GTUtilsMcaEditorSequenceArea::getReferenceReg(GUITestOpStatus &os, int num, int length) {
    McaEditor *editor = GTUtilsMcaEditor::getEditor(os);
    MultipleChromatogramAlignmentObject *obj = editor->getMaObject();
    GT_CHECK_RESULT(obj != NULL, "MultipleChromatogramAlignmentObject not found", QString());

    U2OpStatus2Log status;
    QByteArray seq = obj->getReferenceObj()->getSequenceData(U2Region(num, length), status);
    CHECK_OP(status, QString());

    return seq;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedReferenceReg"
QString GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(GUITestOpStatus &os) {
    McaEditor *editor = GTUtilsMcaEditor::getEditor(os);
    MultipleChromatogramAlignmentObject *obj = editor->getMaObject();
    GT_CHECK_RESULT(obj != NULL, "MultipleChromatogramAlignmentObject not found", QString());

    U2Region sel = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    int num = sel.startPos;
    int length = sel.length;

    U2OpStatus2Log status;
    QByteArray seq = obj->getReferenceObj()->getSequenceData(U2Region(num, length), status);

    return seq;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveTheBorderBetweenAlignmentAndRead"
void GTUtilsMcaEditorSequenceArea::moveTheBorderBetweenAlignmentAndRead(HI::GUITestOpStatus &os, int shift) {
    QStringList visible = getVisibleNames(os);
    GT_CHECK_RESULT(visible.size() != 0, "No visible reads", );
    QString firstVisible = visible.first();

    const QRect sequenceNameRect = GTUtilsMcaEditor::getReadNameRect(os, firstVisible);
    GTMouseDriver::moveTo(QPoint(sequenceNameRect.right() + 2, sequenceNameRect.center().y()));
    GTMouseDriver::press(Qt::LeftButton);
    GTGlobals::sleep(1000);
    GTMouseDriver::moveTo(QPoint(sequenceNameRect.right() + 2 + shift, sequenceNameRect.center().y()));
    GTMouseDriver::release(Qt::LeftButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDrop"
void GTUtilsMcaEditorSequenceArea::dragAndDrop(HI::GUITestOpStatus & /*os*/, const QPoint p) {
    GTMouseDriver::click();
    GTGlobals::sleep(1000);
    GTMouseDriver::press(Qt::LeftButton);
    GTGlobals::sleep(1000);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::release(Qt::LeftButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedRowsNum"
U2Region GTUtilsMcaEditorSequenceArea::getSelectedRowsNum(GUITestOpStatus &os) {
    McaEditorNameList *mcaNameList = GTUtilsMcaEditor::getNameListArea(os);
    CHECK_SET_ERR_RESULT(mcaNameList != NULL, "McaEditorNameList not found", U2Region());

    U2Region selection = mcaNameList->getSelection();
    return selection;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedRowsNames"
QStringList GTUtilsMcaEditorSequenceArea::getSelectedRowsNames(GUITestOpStatus &os) {
    U2Region sel = getSelectedRowsNum(os);
    QStringList names = getNameList(os);

    QStringList res;
    for (int i = sel.startPos; i < sel.endPos(); i++) {
        res << names[i];
    }

    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedRect"
QRect GTUtilsMcaEditorSequenceArea::getSelectedRect(GUITestOpStatus &os) {
    McaEditorSequenceArea *mcaEditArea = qobject_cast<McaEditorSequenceArea *>(GTWidget::findWidget(os, "mca_editor_sequence_area"));
    GT_CHECK_RESULT(mcaEditArea != NULL, "McaEditorSequenceArea not found", QRect());

    return mcaEditArea->getSelection().toRect();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickToReferencePositionCenter"
void GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(GUITestOpStatus &os, qint64 position, const QPoint &clickPointAdjustment) {
    moveCursorToReferencePositionCenter(os, position, clickPointAdjustment);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveCursorToReferencePositionCenter"
void GTUtilsMcaEditorSequenceArea::moveCursorToReferencePositionCenter(GUITestOpStatus &os, qint64 position, const QPoint &movePointAdjustment) {
    QPoint selectedPoint(position, 2);
    McaEditorSequenceArea *mcaSeqArea = GTWidget::findExactWidget<McaEditorSequenceArea *>(os, "mca_editor_sequence_area", GTUtilsMcaEditor::getActiveMcaEditorWindow(os));
    GT_CHECK(mcaSeqArea->isInRange(selectedPoint),
             QString("Position is out of range: [%1, %2], range: [%3, %4]")
                 .arg(selectedPoint.x())
                 .arg(selectedPoint.y())
                 .arg(mcaSeqArea->getEditor()->getAlignmentLen())
                 .arg(mcaSeqArea->getViewRowCount()));

    scrollToPosition(os, selectedPoint);

    McaEditorWgt *mcaWidget = mcaSeqArea->getEditor()->getUI();
    int centerX = mcaWidget->getBaseWidthController()->getBaseScreenCenter(selectedPoint.x());
    int centerY = mcaWidget->getRowHeightController()->getSingleRowHeight() / 2;
    QPoint cursorPosition(centerX + movePointAdjustment.x(), centerY + movePointAdjustment.y());
    GT_CHECK(mcaSeqArea->rect().contains(cursorPosition, false), "Position is not visible");

    QWidget *refSeqView = GTWidget::findWidget(os, "mca_editor_reference_area");
    GTMouseDriver::moveTo(refSeqView->mapToGlobal(cursorPosition));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCharacterModificationMode"
short GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(GUITestOpStatus &os) {
    McaEditorSequenceArea *mcaSeqArea = GTUtilsMcaEditorSequenceArea::getSequenceArea(os);
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor sequence area is not found", 3);

    short mod = mcaSeqArea->getModInfo();
    return mod;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedChar"
char GTUtilsMcaEditorSequenceArea::getSelectedReadChar(GUITestOpStatus &os) {
    QRect selection = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    GT_CHECK_RESULT(selection.width() > 0 && selection.height() > 0, "There is no selection", U2Mca::INVALID_CHAR);
    GT_CHECK_RESULT(selection.width() <= 1 && selection.height() <= 1, "The selection is too big", U2Mca::INVALID_CHAR);
    int rowNum = selection.y();
    qint64 pos = selection.x();

    McaEditorSequenceArea *mcaSeqArea = GTUtilsMcaEditorSequenceArea::getSequenceArea(os);
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor sequence area is not found", U2Mca::INVALID_CHAR);

    McaEditor *mcaEditor = mcaSeqArea->getEditor();
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor is not found", U2Mca::INVALID_CHAR);

    MultipleChromatogramAlignmentObject *mcaObj = mcaEditor->getMaObject();
    GT_CHECK_RESULT(mcaObj != NULL, "MCA Object is not found", U2Mca::INVALID_CHAR);

    const MultipleChromatogramAlignmentRow mcaRow = mcaObj->getRow(rowNum);

    char selectedChar = mcaRow->charAt(pos);
    return selectedChar;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadCharByPos"
char GTUtilsMcaEditorSequenceArea::getReadCharByPos(GUITestOpStatus &os, const QPoint p) {
    int rowNum = p.y();
    qint64 pos = p.x();

    McaEditorSequenceArea *mcaSeqArea = GTUtilsMcaEditorSequenceArea::getSequenceArea(os);
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor sequence area is not found", U2Mca::INVALID_CHAR);

    McaEditor *mcaEditor = mcaSeqArea->getEditor();
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor is not found", U2Mca::INVALID_CHAR);

    MultipleChromatogramAlignmentObject *mcaObj = mcaEditor->getMaObject();
    GT_CHECK_RESULT(mcaObj != NULL, "MCA Object is not found", U2Mca::INVALID_CHAR);

    const MultipleChromatogramAlignmentRow mcaRow = mcaObj->getRow(rowNum);

    char selectedChar = mcaRow->charAt(pos);
    return selectedChar;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRowLength"
qint64 GTUtilsMcaEditorSequenceArea::getRowLength(GUITestOpStatus &os, const int numRow) {
    McaEditorSequenceArea *mcaSeqArea = GTUtilsMcaEditorSequenceArea::getSequenceArea(os);
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor sequence area is not found", 0);

    McaEditor *mcaEditor = mcaSeqArea->getEditor();
    GT_CHECK_RESULT(mcaSeqArea != NULL, "MCA Editor is not found", 0);

    MultipleChromatogramAlignmentObject *mcaObj = mcaEditor->getMaObject();
    GT_CHECK_RESULT(mcaObj != NULL, "MCA Object is not found", 0);

    const MultipleChromatogramAlignmentRow mcaRow = mcaObj->getRow(numRow);

    qint64 rowLength = mcaRow->getCoreLength();
    return rowLength;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceLength"
qint64 GTUtilsMcaEditorSequenceArea::getReferenceLength(GUITestOpStatus &os) {
    McaEditor *editor = GTUtilsMcaEditor::getEditor(os);
    MultipleChromatogramAlignmentObject *obj = editor->getMaObject();
    GT_CHECK_RESULT(obj != NULL, "MultipleChromatogramAlignmentObject not found", 0);

    U2OpStatus2Log status;
    qint64 refLength = obj->getReferenceObj()->getSequenceLength();

    return refLength;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceLengthWithGaps"
qint64 GTUtilsMcaEditorSequenceArea::getReferenceLengthWithGaps(GUITestOpStatus &os) {
    McaEditor *editor = GTUtilsMcaEditor::getEditor(os);
    MultipleChromatogramAlignmentObject *obj = editor->getMaObject();
    GT_CHECK_RESULT(obj != NULL, "MultipleChromatogramAlignmentObject not found", 0);

    int length = obj->getReferenceLengthWithGaps();

    return length;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceSelection"
U2Region GTUtilsMcaEditorSequenceArea::getReferenceSelection(GUITestOpStatus &os) {
    McaEditorReferenceArea *mcaEditArea = qobject_cast<McaEditorReferenceArea *>(GTWidget::findWidget(os, "mca_editor_reference_area"));
    GT_CHECK_RESULT(mcaEditArea != NULL, "McaEditorReferenceArea not found", U2Region());

    SequenceObjectContext *seqContext = mcaEditArea->getSequenceContext();
    GT_CHECK_RESULT(seqContext != NULL, "SequenceObjectContext not found", U2Region());

    DNASequenceSelection *dnaSel = seqContext->getSequenceSelection();
    GT_CHECK_RESULT(dnaSel != NULL, "DNASequenceSelection not found", U2Region());

    QVector<U2Region> region = dnaSel->getSelectedRegions();

    CHECK(region.size() != 0, U2Region());

    GT_CHECK_RESULT(region.size() == 1, "Incorrect selected region", U2Region());

    return region.first();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedConsensusReg"
QString GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(GUITestOpStatus &os) {
    McaEditorConsensusArea *consArea = GTUtilsMcaEditor::getConsensusArea(os);
    GT_CHECK_RESULT(consArea != NULL, "Consensus area not found", QString());

    QSharedPointer<MSAEditorConsensusCache> consCache = consArea->getConsensusCache();

    U2Region sel = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    int start = sel.startPos;
    int length = sel.length;

    QString res;
    for (int i = 0; i < length; i++) {
        int pos = start + i;
        char ch = consCache->getConsensusChar(pos);
        res.append(ch);
    }
    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusStringByPos"
QString GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(GUITestOpStatus &os, const U2Region reg) {
    McaEditorConsensusArea *consArea = GTUtilsMcaEditor::getConsensusArea(os);
    GT_CHECK_RESULT(consArea != NULL, "Consensus area not found", QString());

    QSharedPointer<MSAEditorConsensusCache> consCache = consArea->getConsensusCache();

    int start = reg.startPos;
    int length = reg.length;

    QString res;
    for (int i = 0; i < length; i++) {
        int pos = start + i;
        char ch = consCache->getConsensusChar(pos);
        res.append(ch);
    }
    return res;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}    // namespace U2
