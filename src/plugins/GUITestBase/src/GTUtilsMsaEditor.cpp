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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include <U2View/BaseWidthController.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MSAEditorOverviewArea.h>
#include <U2View/MaGraphOverview.h>
#include <U2View/MaSimpleOverview.h>
#include <U2View/RowHeightController.h>

#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProjectTreeView.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMsaEditor"

#define GT_METHOD_NAME "getGraphOverviewTopLeftPixelColor"
QColor GTUtilsMsaEditor::getGraphOverviewPixelColor(GUITestOpStatus &os, const QPoint &point) {
    return GTWidget::getColor(os, getGraphOverview(os), point);
}

QColor GTUtilsMsaEditor::getSimpleOverviewPixelColor(GUITestOpStatus &os, const QPoint &point) {
    return GTWidget::getColor(os, getSimpleOverview(os), point);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditor"
MSAEditor * GTUtilsMsaEditor::getEditor(GUITestOpStatus &os) {
    MsaEditorWgt *editorUi = getEditorUi(os);
    CHECK_OP(os, NULL);
    return editorUi->getEditor();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditorUi"
MsaEditorWgt * GTUtilsMsaEditor::getEditorUi(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);
    return activeWindow->findChild<MsaEditorWgt *>();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGraphOverview"
MaGraphOverview * GTUtilsMsaEditor::getGraphOverview(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);

    MaGraphOverview *result = GTWidget::findExactWidget<MaGraphOverview *>(os, MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME + QString("_graph"), activeWindow);
    GT_CHECK_RESULT(NULL != result, "MaGraphOverview is not found", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSimpleOverview"
MaSimpleOverview * GTUtilsMsaEditor::getSimpleOverview(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);

    MaSimpleOverview *result = GTWidget::findExactWidget<MaSimpleOverview *>(os, MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME + QString("_simple"), activeWindow);
    GT_CHECK_RESULT(NULL != result, "MaSimpleOverview is not found", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeView"
MSAEditorTreeViewerUI * GTUtilsMsaEditor::getTreeView(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);
    return GTWidget::findExactWidget<MSAEditorTreeViewerUI *>(os, "treeView", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameListArea"
MaEditorNameList * GTUtilsMsaEditor::getNameListArea(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);

    MaEditorNameList *result = GTWidget::findExactWidget<MaEditorNameList *>(os, "msa_editor_name_list", activeWindow);
    GT_CHECK_RESULT(NULL != result, "MaGraphOverview is not found", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusArea"
MSAEditorConsensusArea * GTUtilsMsaEditor::getConsensusArea(GUITestOpStatus &os) {
    QWidget *activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_OP(os, NULL);
    return GTWidget::findExactWidget<MSAEditorConsensusArea *>(os, "consArea", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRect"
MSAEditorSequenceArea * GTUtilsMsaEditor::getSequenceArea(GUITestOpStatus &os) {
    return GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRect"
QRect GTUtilsMsaEditor::getSequenceNameRect(GUITestOpStatus &os, const QString &sequenceName) {
    MaEditorNameList *nameList = getNameListArea(os);
    GT_CHECK_RESULT(NULL != nameList, "MSAEditorNameList not found", QRect());

    const QStringList names = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);
    const int rowNumber = names.indexOf(sequenceName);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Sequence '%1' not found").arg(sequenceName), QRect());
    return getSequenceNameRect(os,  rowNumber);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRect"
QRect GTUtilsMsaEditor::getSequenceNameRect(GUITestOpStatus &os, int rowNumber) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(0 <= rowNumber, QString("Sequence '%1' not found").arg(rowNumber), QRect());

    MaEditorNameList *nameList = getNameListArea(os);
    GT_CHECK_RESULT(NULL != nameList, "MSAEditorNameList not found", QRect());

    const U2Region rowScreenRange = getEditorUi(os)->getRowHeightController()->getScreenYRegionByViewRowIndex(rowNumber);
    return QRect(nameList->mapToGlobal(QPoint(0, rowScreenRange.startPos)), nameList->mapToGlobal(QPoint(nameList->width(), rowScreenRange.endPos())));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnHeaderRect"
QRect GTUtilsMsaEditor::getColumnHeaderRect(GUITestOpStatus &os, int column) {
    MSAEditorConsensusArea *consensusArea = getConsensusArea(os);
    GT_CHECK_RESULT(NULL != consensusArea, "Consensus area is NULL", QRect());
    MSAEditorSequenceArea *sequenceArea = getSequenceArea(os);
    GT_CHECK_RESULT(NULL != sequenceArea, "Sequence area is NULL", QRect());
    MSAEditor *editor = getEditor(os);
    GT_CHECK_RESULT(NULL != editor, "MSA Editor is NULL", QRect());

    BaseWidthController *baseWidthController = editor->getUI()->getBaseWidthController();
    return QRect(consensusArea->mapToGlobal(QPoint(baseWidthController->getBaseScreenOffset(column),
                                                   consensusArea->geometry().top())),
                 QSize(baseWidthController->getBaseWidth(),
                       consensusArea->height()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "replaceSequence"
void GTUtilsMsaEditor::replaceSequence(GUITestOpStatus &os, const QString &sequenceToReplace, int targetPosition) {
    clickSequenceName(os, sequenceToReplace);

    targetPosition = qMax(0, qMin(getSequencesCount(os) - 1, targetPosition));
    const QString targetSequenceName = GTUtilsMSAEditorSequenceArea::getNameList(os)[targetPosition];

    const QPoint dragFrom = getSequenceNameRect(os, sequenceToReplace).center();
    const QPoint dragTo = getSequenceNameRect(os, targetSequenceName).center();

    GTMouseDriver::moveTo(dragFrom);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(dragTo);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "replaceSequence"
void GTUtilsMsaEditor::replaceSequence(GUITestOpStatus &os, int rowNumber, int targetPosition) {
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GT_CHECK(0 <= rowNumber && rowNumber <= names.size(), "Row number is out of boundaries");
    replaceSequence(os, names[rowNumber], targetPosition);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeColumn"
void GTUtilsMsaEditor::removeColumn(GUITestOpStatus &os, int column) {
    clickColumn(os, column);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeRows"
void GTUtilsMsaEditor::removeRows(GUITestOpStatus &os, int firstRowNumber, int lastRowNumber) {
    selectRows(os, firstRowNumber, lastRowNumber);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToSequence"
void GTUtilsMsaEditor::moveToSequence(GUITestOpStatus &os, int rowNumber) {
    const QRect sequenceNameRect = getSequenceNameRect(os, rowNumber);
    GTMouseDriver::moveTo(sequenceNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToSequenceName"
void GTUtilsMsaEditor::moveToSequenceName(GUITestOpStatus &os, const QString &sequenceName) {
    const QRect sequenceNameRect = getSequenceNameRect(os, sequenceName);
    GTMouseDriver::moveTo(sequenceNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSequence"
void GTUtilsMsaEditor::clickSequence(GUITestOpStatus &os, int rowNumber, Qt::MouseButton mouseButton) {
    moveToSequence(os, rowNumber);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSequenceName"
void GTUtilsMsaEditor::clickSequenceName(GUITestOpStatus &os, const QString &sequenceName, Qt::MouseButton mouseButton) {
    moveToSequenceName(os, sequenceName);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToColumn"
void GTUtilsMsaEditor::moveToColumn(GUITestOpStatus &os, int column) {
    GTUtilsMSAEditorSequenceArea::scrollToPosition(os, QPoint(column, 1));
    const QRect columnHeaderRect = getColumnHeaderRect(os, column);
    GTMouseDriver::moveTo(columnHeaderRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickColumn"
void GTUtilsMsaEditor::clickColumn(GUITestOpStatus &os, int column, Qt::MouseButton mouseButton) {
    moveToColumn(os, column);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRows"
void GTUtilsMsaEditor::selectRows(GUITestOpStatus &os, int firstRowNumber, int lastRowNumber, GTGlobals::UseMethod method) {
    switch (method) {
    case GTGlobals::UseKey:
        clickSequence(os, firstRowNumber);
        GTKeyboardDriver::keyPress(Qt::Key_Shift);
        clickSequence(os, lastRowNumber);
        GTKeyboardDriver::keyRelease(Qt::Key_Shift);
        break;
    case GTGlobals::UseMouse:
        GTMouseDriver::dragAndDrop(getSequenceNameRect(os, firstRowNumber).center(),
                                   getSequenceNameRect(os, lastRowNumber).center());
        break;
    case GTGlobals::UseKeyBoard:
        GT_CHECK(false, "Not implemented");
    default:
        GT_CHECK(false, "An unknown method");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectColumns"
void GTUtilsMsaEditor::selectColumns(GUITestOpStatus &os, int firstColumnNumber, int lastColumnNumber, GTGlobals::UseMethod method) {
    switch (method) {
    case GTGlobals::UseKey:
        clickColumn(os, firstColumnNumber);
        GTKeyboardDriver::keyPress(Qt::Key_Shift);
        clickColumn(os, lastColumnNumber);
        GTKeyboardDriver::keyRelease(Qt::Key_Shift);
        break;
    case GTGlobals::UseMouse:
        GTMouseDriver::dragAndDrop(getColumnHeaderRect(os, firstColumnNumber).center(),
                                   getColumnHeaderRect(os, lastColumnNumber).center());
        break;
    case GTGlobals::UseKeyBoard:
        GT_CHECK(false, "Not implemented");
    default:
        GT_CHECK(false, "An unknown method");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clearSelection"
void GTUtilsMsaEditor::clearSelection(GUITestOpStatus &os) {
    Q_UNUSED(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceSequenceName"
QString GTUtilsMsaEditor::getReferenceSequenceName(GUITestOpStatus &os) {
    return GTUtilsOptionPanelMsa::getReference(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCollapsingMode"
void GTUtilsMsaEditor::toggleCollapsingMode(GUITestOpStatus &os) {
    Q_UNUSED(os);
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Switch on/off collapsing"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSequenceCollapsed"
bool GTUtilsMsaEditor::isSequenceCollapsed(GUITestOpStatus &os, const QString &seqName){
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GT_CHECK_RESULT(names.contains(seqName), "sequence " + seqName + " not found in name list", false);
    QStringList visiablenames = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);

    return !visiablenames.contains(seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCollapsingMode"
void GTUtilsMsaEditor::toggleCollapsingGroup(GUITestOpStatus &os, const QString &groupName) {
    Q_UNUSED(os);

    const QRect sequenceNameRect = getSequenceNameRect(os, groupName);
    QPoint magicExpandButtonOffset;
#ifdef Q_OS_WIN
    magicExpandButtonOffset = QPoint(15, 10);
#else
    magicExpandButtonOffset = QPoint(15, 5);
#endif
    GTMouseDriver::moveTo(sequenceNameRect.topLeft() + magicExpandButtonOffset);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequencesCount"
int GTUtilsMsaEditor::getSequencesCount(GUITestOpStatus &os) {
    QWidget *statusWidget = GTWidget::findWidget(os, "msa_editor_status_bar");
    return GTMSAEditorStatusWidget::getSequencesCount(os, statusWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWholeData"
QStringList GTUtilsMsaEditor::getWholeData(GUITestOpStatus &os) {
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GT_CHECK_RESULT(!names.isEmpty(), "The name list is empty", QStringList());

    clickSequenceName(os, names.first());
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    clickSequenceName(os, names.last());
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    GTKeyboardUtils::copy(os);
    GTGlobals::sleep(500);

    return GTClipboard::text(os).split('\n');
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMsaEditor::undo(GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "redo"
void GTUtilsMsaEditor::redo(GUITestOpStatus &os) {
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUndoEnabled"
bool GTUtilsMsaEditor::isUndoEnabled(GUITestOpStatus &os) {
    return GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isRedoEnabled"
bool GTUtilsMsaEditor::isRedoEnabled(GUITestOpStatus &os) {
    return GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "buildPhylogeneticTree"
void GTUtilsMsaEditor::buildPhylogeneticTree(GUITestOpStatus &os, const QString &pathToSave) {
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, pathToSave, 0, 0, true));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDropSequenceFromProject"
void GTUtilsMsaEditor::dragAndDropSequenceFromProject(GUITestOpStatus &os, const QStringList &pathToSequence) {
    GTUtilsProjectTreeView::dragAndDrop(os, pathToSequence, getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReference"
void GTUtilsMsaEditor::setReference(GUITestOpStatus &os, const QString &sequenceName) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Set this sequence as reference", GTGlobals::UseMouse));
    clickSequenceName(os, sequenceName, Qt::RightButton);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
