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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTToolbar.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include <U2View/BaseWidthController.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorOverviewArea.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaEditorSelection.h>
#include <U2View/MaGraphOverview.h>
#include <U2View/MaSimpleOverview.h>
#include <U2View/RowHeightController.h>

#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTMSAEditorStatusWidget.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMsaEditor"

#define GT_METHOD_NAME "getActiveMsaEditorWindow"
QWidget* GTUtilsMsaEditor::getActiveMsaEditorWindow(GUITestOpStatus& os) {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(os, MsaEditorFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMsaEditorWindowIsActive"
void GTUtilsMsaEditor::checkMsaEditorWindowIsActive(GUITestOpStatus& os) {
    getActiveMsaEditorWindow(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoMsaEditorWindowIsOpened"
void GTUtilsMsaEditor::checkNoMsaEditorWindowIsOpened(GUITestOpStatus& os) {
    GTUtilsMdi::checkNoObjectViewWindowIsOpened(os, MsaEditorFactory::ID);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGraphOverviewTopLeftPixelColor"
QColor GTUtilsMsaEditor::getGraphOverviewPixelColor(GUITestOpStatus& os, const QPoint& point) {
    return GTWidget::getColor(os, getGraphOverview(os), point);
}

QColor GTUtilsMsaEditor::getSimpleOverviewPixelColor(GUITestOpStatus& os, const QPoint& point) {
    return GTWidget::getColor(os, getSimpleOverview(os), point);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditor"
MSAEditor* GTUtilsMsaEditor::getEditor(GUITestOpStatus& os) {
    MsaEditorWgt* editorUi = getEditorUi(os);
    return editorUi->getEditor();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditorUi"
MsaEditorWgt* GTUtilsMsaEditor::getEditorUi(GUITestOpStatus& os) {
    checkMsaEditorWindowIsActive(os);
    MsaEditorWgt* msaEditorWgt = nullptr;
    // For some reason MsaEditorWgt is not within normal widgets hierarchy (wrong parent?), so can't use GTWidget::findWidget here.
    for (int time = 0; time < GT_OP_WAIT_MILLIS && msaEditorWgt == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        MainWindow* mainWindow = AppContext::getMainWindow();
        QWidget* activeWindow = mainWindow == nullptr ? nullptr : mainWindow->getMDIManager()->getActiveWindow();
        if (activeWindow == nullptr) {
            continue;
        }
        msaEditorWgt = activeWindow->findChild<MsaEditorWgt*>();
    }
    GT_CHECK_RESULT(msaEditorWgt != nullptr, "MSA Editor widget is not found", nullptr);
    return msaEditorWgt;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOverviewArea"
QWidget* GTUtilsMsaEditor::getOverviewArea(GUITestOpStatus& os) {
    QWidget* activeWindow = getActiveMsaEditorWindow(os);
    return GTWidget::findExactWidget<MaEditorOverviewArea*>(os, MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME, activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getShowOverviewButton"
QToolButton* GTUtilsMsaEditor::getShowOverviewButton(HI::GUITestOpStatus& os) {
    auto showOverviewButton = qobject_cast<QToolButton*>(GTAction::button(os, "Show overview"));
    CHECK_SET_ERR_RESULT(showOverviewButton != nullptr, "Overview button is not found", nullptr);
    return showOverviewButton;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGraphOverview"
QWidget* GTUtilsMsaEditor::getGraphOverview(GUITestOpStatus& os) {
    QWidget* overviewArea = getOverviewArea(os);
    return GTWidget::findExactWidget<MaGraphOverview*>(os, MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME + "_graph", overviewArea);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSimpleOverview"
QWidget* GTUtilsMsaEditor::getSimpleOverview(GUITestOpStatus& os) {
    QWidget* overviewArea = getOverviewArea(os);
    return GTWidget::findExactWidget<MaSimpleOverview*>(os, MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME + "_simple", overviewArea);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeView"
MSAEditorTreeViewerUI* GTUtilsMsaEditor::getTreeView(GUITestOpStatus& os) {
    QWidget* activeWindow = getActiveMsaEditorWindow(os);
    return GTWidget::findExactWidget<MSAEditorTreeViewerUI*>(os, "treeView", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameListArea"
MaEditorNameList* GTUtilsMsaEditor::getNameListArea(GUITestOpStatus& os) {
    QWidget* activeWindow = getActiveMsaEditorWindow(os);
    MaEditorNameList* result = GTWidget::findExactWidget<MaEditorNameList*>(os, "msa_editor_name_list", activeWindow);
    GT_CHECK_RESULT(result != nullptr, "MaGraphOverview is not found", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusArea"
MSAEditorConsensusArea* GTUtilsMsaEditor::getConsensusArea(GUITestOpStatus& os) {
    QWidget* activeWindow = getActiveMsaEditorWindow(os);
    return GTWidget::findExactWidget<MSAEditorConsensusArea*>(os, "consArea", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRect"
MSAEditorSequenceArea* GTUtilsMsaEditor::getSequenceArea(GUITestOpStatus& os) {
    return GTUtilsMSAEditorSequenceArea::getSequenceArea(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRectByName"
QRect GTUtilsMsaEditor::getSequenceNameRect(GUITestOpStatus& os, const QString& sequenceName) {
    QStringList rowNames = GTUtilsMSAEditorSequenceArea::getCurrentRowNames(os);
    int viewRowIndex = rowNames.indexOf(sequenceName);
    GT_CHECK_RESULT(viewRowIndex >= 0, QString("Sequence '%1' not found").arg(sequenceName), {});
    return getSequenceNameRect(os, viewRowIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRectByIndex"
QRect GTUtilsMsaEditor::getSequenceNameRect(GUITestOpStatus& os, int viewRowIndex) {
    GT_CHECK_RESULT(viewRowIndex >= 0, QString("Sequence '%1' not found").arg(viewRowIndex), QRect());

    U2Region nameListYRegion = getEditorUi(os)->getRowHeightController()->getScreenYRegionByViewRowIndex(viewRowIndex);
    MaEditorNameList* nameList = getNameListArea(os);
    QPoint topLeftPoint = nameList->mapToGlobal(QPoint(0, (int)nameListYRegion.startPos));
    QPoint bottomRightPoint = nameList->mapToGlobal(QPoint(nameList->width(), (int)nameListYRegion.endPos()));
    return QRect(topLeftPoint, bottomRightPoint);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnHeaderRect"
QRect GTUtilsMsaEditor::getColumnHeaderRect(GUITestOpStatus& os, int column) {
    MSAEditorConsensusArea* consensusArea = getConsensusArea(os);
    GT_CHECK_RESULT(nullptr != consensusArea, "Consensus area is NULL", QRect());
    MSAEditorSequenceArea* sequenceArea = getSequenceArea(os);
    GT_CHECK_RESULT(nullptr != sequenceArea, "Sequence area is NULL", QRect());
    MSAEditor* editor = getEditor(os);
    GT_CHECK_RESULT(nullptr != editor, "MSA Editor is NULL", QRect());

    BaseWidthController *baseWidthController = editor->getUI()->getUI()->getBaseWidthController();
    return QRect(consensusArea->mapToGlobal(QPoint(baseWidthController->getBaseScreenOffset(column),
                                                   consensusArea->geometry().top())),
                 QSize(baseWidthController->getBaseWidth(),
                       consensusArea->height()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "replaceSequence"
void GTUtilsMsaEditor::replaceSequence(GUITestOpStatus& os, const QString& sequenceToReplace, int targetPosition) {
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
void GTUtilsMsaEditor::replaceSequence(GUITestOpStatus& os, int rowNumber, int targetPosition) {
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GT_CHECK(0 <= rowNumber && rowNumber <= names.size(), "Row number is out of boundaries");
    replaceSequence(os, names[rowNumber], targetPosition);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeColumn"
void GTUtilsMsaEditor::removeColumn(GUITestOpStatus& os, int column) {
    clickColumn(os, column);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeRows"
void GTUtilsMsaEditor::removeRows(GUITestOpStatus& os, int firstRowNumber, int lastRowNumber) {
    selectRows(os, firstRowNumber, lastRowNumber);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToSequence"
void GTUtilsMsaEditor::moveToSequence(GUITestOpStatus& os, int rowNumber) {
    const QRect sequenceNameRect = getSequenceNameRect(os, rowNumber);
    GTMouseDriver::moveTo(sequenceNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToSequenceName"
void GTUtilsMsaEditor::moveToSequenceName(GUITestOpStatus& os, const QString& sequenceName) {
    QRect sequenceNameRect = getSequenceNameRect(os, sequenceName);
    GTMouseDriver::moveTo(sequenceNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSequence"
void GTUtilsMsaEditor::clickSequence(GUITestOpStatus& os, int rowNumber, Qt::MouseButton mouseButton) {
    moveToSequence(os, rowNumber);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSequenceName"
void GTUtilsMsaEditor::clickSequenceName(GUITestOpStatus& os, const QString& sequenceName, const Qt::MouseButton& mouseButton, const Qt::KeyboardModifiers& modifiers) {
    moveToSequenceName(os, sequenceName);

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

#define GT_METHOD_NAME "moveToColumn"
void GTUtilsMsaEditor::moveToColumn(GUITestOpStatus& os, int column) {
    GTUtilsMSAEditorSequenceArea::scrollToPosition(os, QPoint(column, 1));
    const QRect columnHeaderRect = getColumnHeaderRect(os, column);
    GTMouseDriver::moveTo(columnHeaderRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickColumn"
void GTUtilsMsaEditor::clickColumn(GUITestOpStatus& os, int column, Qt::MouseButton mouseButton) {
    moveToColumn(os, column);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRows"
void GTUtilsMsaEditor::selectRows(GUITestOpStatus& os, int firstRowNumber, int lastRowNumber, GTGlobals::UseMethod method) {
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
            GT_FAIL("Not implemented", );
        default:
            GT_FAIL("An unknown method", );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRowsByName"
void GTUtilsMsaEditor::selectRowsByName(HI::GUITestOpStatus& os, const QStringList& rowNames) {
    for (const QString& rowName : qAsConst(rowNames)) {
        clickSequenceName(os, rowName, Qt::LeftButton, Qt::ControlModifier);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectColumns"
void GTUtilsMsaEditor::selectColumns(GUITestOpStatus& os, int firstColumnNumber, int lastColumnNumber, GTGlobals::UseMethod method) {
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
            GT_FAIL("Not implemented", );
        default:
            GT_FAIL("An unknown method", );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clearSelection"
void GTUtilsMsaEditor::clearSelection(GUITestOpStatus& os) {
    if (!getEditor(os)->getSelection().isEmpty()) {
        GTKeyboardDriver::keyClick(Qt::Key_Escape);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNameList"
void GTUtilsMsaEditor::checkNameList(GUITestOpStatus& os, const QStringList& nameList) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    QStringList nameListInEditor = editor->getMaObject()->getMultipleAlignment()->getRowNames();
    CHECK_SET_ERR(nameListInEditor == nameList, "Name list does not match");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelection"
void GTUtilsMsaEditor::checkSelection(HI::GUITestOpStatus& os, const QList<QRect>& expectedRects) {
    MSAEditor* msaEditor = GTUtilsMsaEditor::getEditor(os);
    QList<QRect> selectedRects = msaEditor->getSelection().getRectList();
    CHECK_SET_ERR(selectedRects.size() == expectedRects.size(), QString("Expected selection size: %1, actual: %2").arg(expectedRects.size()).arg(selectedRects.size()));
    for (int i = 0; i < selectedRects.size(); i++) {
        QRect expectedRect = expectedRects[i];
        QRect selectedRect = selectedRects[i];
        CHECK_SET_ERR(selectedRect == expectedRect,
                      QString("Selection rect is not equal to the expected one, idx: %1, rect: (x:%2, y:%3, w: %4, h: %5), expected: (x: %6, y: %7, w: %8, h: %9)")
                          .arg(i)
                          .arg(selectedRect.x())
                          .arg(selectedRect.y())
                          .arg(selectedRect.width())
                          .arg(selectedRect.height())
                          .arg(expectedRect.x())
                          .arg(expectedRect.y())
                          .arg(expectedRect.width())
                          .arg(expectedRect.height()));
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelectionByNames"
void GTUtilsMsaEditor::checkSelectionByNames(GUITestOpStatus& os, const QStringList& selectedNames) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor(os);
    QStringList rowNames = editor->getMaObject()->getMultipleAlignment()->getRowNames();
    QList<int> selectedRowIndexes = editor->getSelection().getSelectedRowIndexes();
    QStringList selectedNamesFromEditor;
    for (int i = 0; i < selectedRowIndexes.size(); i++) {
        selectedNamesFromEditor << rowNames[selectedRowIndexes[i]];
    }
    GT_CHECK(selectedNames == selectedNamesFromEditor,
             QString("Unexpected selection! Expected: %1, got: %2").arg(selectedNames.join(",")).arg(selectedNamesFromEditor.join(",")));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReferenceSequenceName"
QString GTUtilsMsaEditor::getReferenceSequenceName(GUITestOpStatus& os) {
    return GTUtilsOptionPanelMsa::getReference(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCollapsingMode"
void GTUtilsMsaEditor::toggleCollapsingMode(GUITestOpStatus& os) {
    Q_UNUSED(os);
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Switch on/off collapsing"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSequenceCollapsed"
bool GTUtilsMsaEditor::isSequenceCollapsed(GUITestOpStatus& os, const QString& seqName) {
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GT_CHECK_RESULT(names.contains(seqName), "sequence " + seqName + " not found in name list", false);
    QStringList visiablenames = GTUtilsMSAEditorSequenceArea::getVisibleNames(os);

    return !visiablenames.contains(seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCollapsingMode"
void GTUtilsMsaEditor::toggleCollapsingGroup(GUITestOpStatus& os, const QString& groupName) {
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
int GTUtilsMsaEditor::getSequencesCount(GUITestOpStatus& os) {
    QWidget* statusWidget = GTWidget::findWidget(os, "msa_editor_status_bar");
    return GTMSAEditorStatusWidget::getSequencesCount(os, statusWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWholeData"
QStringList GTUtilsMsaEditor::getWholeData(GUITestOpStatus& os) {
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList(os);
    GT_CHECK_RESULT(!names.isEmpty(), "The name list is empty", QStringList());

    clickSequenceName(os, names.first());
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    clickSequenceName(os, names.last());
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    GTKeyboardUtils::copy();
    GTGlobals::sleep(500);

    return GTClipboard::text(os).split('\n');
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMsaEditor::undo(GUITestOpStatus& os) {
    getActiveMsaEditorWindow(os);
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "redo"
void GTUtilsMsaEditor::redo(GUITestOpStatus& os) {
    getActiveMsaEditorWindow(os);
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsMsaEditor::zoomIn(GUITestOpStatus& os) {
    QToolBar* toolbar = GTToolbar::getToolbar(os, "mwtoolbar_activemdi");
    QWidget* zoomInButton = GTToolbar::getWidgetForActionObjectName(os, toolbar, "Zoom In");
    GTWidget::click(os, zoomInButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomOut"
void GTUtilsMsaEditor::zoomOut(GUITestOpStatus& os) {
    QToolBar* toolbar = GTToolbar::getToolbar(os, "mwtoolbar_activemdi");
    QWidget* zoomOutButton = GTToolbar::getWidgetForActionObjectName(os, toolbar, "Zoom Out");
    GTWidget::click(os, zoomOutButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUndoEnabled"
bool GTUtilsMsaEditor::isUndoEnabled(GUITestOpStatus& os) {
    getActiveMsaEditorWindow(os);
    return GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_undo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isRedoEnabled"
bool GTUtilsMsaEditor::isRedoEnabled(GUITestOpStatus& os) {
    getActiveMsaEditorWindow(os);
    return GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "msa_action_redo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "buildPhylogeneticTree"
void GTUtilsMsaEditor::buildPhylogeneticTree(GUITestOpStatus& os, const QString& pathToSave) {
    GTUtilsDialog::waitForDialog(os, new BuildTreeDialogFiller(os, pathToSave, 0, 0, true));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Build Tree");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeActiveTreeTab"
void GTUtilsMsaEditor::closeActiveTreeTab(GUITestOpStatus& os) {
    QWidget* treeTabWidget = GTWidget::findWidget(os, "msa_editor_tree_tab_area", getActiveMsaEditorWindow(os));

    QTabBar* tabBar = treeTabWidget->findChild<QTabBar*>();
    GT_CHECK(tabBar != nullptr, "Tree tab widget must have a tab bar!");

    int tabIndex = tabBar->currentIndex();
    GT_CHECK(tabIndex >= 0, "TabBar must have a current tab!");

    QWidget* closeTabButton = tabBar->tabButton(tabIndex, QTabBar::RightSide);
    if (closeTabButton == nullptr) {
        closeTabButton = tabBar->tabButton(tabIndex, QTabBar::RightSide);
        GT_CHECK(closeTabButton != nullptr, "TabBar must have close button!");
    }
    GTWidget::click(os, closeTabButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDropSequenceFromProject"
void GTUtilsMsaEditor::dragAndDropSequenceFromProject(GUITestOpStatus& os, const QStringList& pathToSequence) {
    GTUtilsProjectTreeView::dragAndDrop(os, pathToSequence, getEditorUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activateAlignSequencesToAlignmentMenu"
void GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(HI::GUITestOpStatus& os, const QString& partOfMenuItemText) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {partOfMenuItemText}, GTGlobals::UseMouse, Qt::MatchContains));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAlignSequencesToAlignmentMenu"
void GTUtilsMsaEditor::checkAlignSequencesToAlignmentMenu(HI::GUITestOpStatus& os, const QString& partOfMenuItemText, const PopupChecker::CheckOption& checkOption) {
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, {partOfMenuItemText}, checkOption, GTGlobals::UseMouse, Qt::MatchContains));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReference"
void GTUtilsMsaEditor::setReference(GUITestOpStatus& os, const QString& sequenceName) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Set this sequence as reference", GTGlobals::UseMouse));
    clickSequenceName(os, sequenceName, Qt::RightButton);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openExcludeList"
void GTUtilsMsaEditor::openExcludeList(HI::GUITestOpStatus& os, bool waitUntilLoaded) {
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "exclude_list_toggle_action");
    if (!toggleExcludeListButton->isChecked()) {
        GTWidget::click(os, toggleExcludeListButton);
        if (waitUntilLoaded) {
            GTUtilsTaskTreeView::waitTaskFinished(os);
        }
    }
    GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow);  // Test that Exclude List is present.
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeExcludeList"
void GTUtilsMsaEditor::closeExcludeList(HI::GUITestOpStatus& os, bool waitUntilFinished) {
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "exclude_list_toggle_action");
    if (toggleExcludeListButton->isChecked()) {
        GTWidget::click(os, toggleExcludeListButton);
        if (waitUntilFinished) {
            GTUtilsTaskTreeView::waitTaskFinished(os);
        }
    }
    // Test that Exclude List is not present.
    CHECK_SET_ERR(GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow, {false}) == nullptr, "Exclude List widget is present");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveRowsToExcludeList"
void GTUtilsMsaEditor::moveRowsToExcludeList(HI::GUITestOpStatus& os, const QStringList& rowNames) {
    GTUtilsMsaEditor::clearSelection(os);
    GTUtilsMsaEditor::selectRowsByName(os, rowNames);
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto button = GTWidget::findToolButton(os, "exclude_list_move_from_msa_button", msaEditorWindow);
    CHECK_SET_ERR(button->isEnabled(), "Button is not enabled: " + button->objectName());
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveRowFromExcludeList"
void GTUtilsMsaEditor::moveRowFromExcludeList(HI::GUITestOpStatus& os, const QString& rowName) {
    auto listWidget = getExcludeListWidget(os);
    GTListWidget::click(os, listWidget, rowName);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto button = GTWidget::findToolButton(os, "exclude_list_move_to_msa_button", msaEditorWindow);
    CHECK_SET_ERR(button->isEnabled(), "Button is not enabled: " + button->objectName());
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkExcludeList"
void GTUtilsMsaEditor::checkExcludeList(GUITestOpStatus& os, const QStringList& nameList) {
    auto listWidget = getExcludeListWidget(os);
    QStringList actualNameList = GTListWidget::getItems(os, listWidget);
    CHECK_SET_ERR(actualNameList == nameList, "Name list does not match, expected: " + nameList.join(";") + ", got: " + actualNameList.join(";"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRowsByNameInExcludeList"
void GTUtilsMsaEditor::selectRowsByNameInExcludeList(HI::GUITestOpStatus& os, const QStringList& rowNames) {
    auto listWidget = getExcludeListWidget(os);
    GTListWidget::selectItemsByText(os, listWidget, rowNames);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkExcludeListSelection"
void GTUtilsMsaEditor::checkExcludeListSelection(HI::GUITestOpStatus& os, const QStringList& rowNames) {
    auto listWidget = getExcludeListWidget(os);
    GTListWidget::checkSelection(os, listWidget, rowNames);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExcludeListWidget"
QListWidget* GTUtilsMsaEditor::getExcludeListWidget(HI::GUITestOpStatus& os) {
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow(os);
    auto excludeList = GTWidget::findWidget(os, "msa_exclude_list", msaEditorWindow);
    return GTWidget::findListWidget(os, "exclude_list_name_list_widget", excludeList);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
