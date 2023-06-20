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
#include <U2View/MSAEditorTreeViewer.h>
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
#include "runnables/ugene/corelibs/U2Gui/PositionSelectorFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsMsaEditor"

#define GT_METHOD_NAME "getActiveMsaEditorWindow"
QWidget* GTUtilsMsaEditor::getActiveMsaEditorWindow() {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(MsaEditorFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMsaEditorWindowIsActive"
void GTUtilsMsaEditor::checkMsaEditorWindowIsActive() {
    getActiveMsaEditorWindow();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoMsaEditorWindowIsOpened"
void GTUtilsMsaEditor::checkNoMsaEditorWindowIsOpened() {
    GTUtilsMdi::checkNoObjectViewWindowIsOpened(MsaEditorFactory::ID);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGraphOverviewTopLeftPixelColor"
QColor GTUtilsMsaEditor::getGraphOverviewPixelColor(const QPoint& point) {
    return GTWidget::getColor(getGraphOverview(), point);
}

QColor GTUtilsMsaEditor::getSimpleOverviewPixelColor(const QPoint& point) {
    return GTWidget::getColor(getSimpleOverview(), point);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditor"
MSAEditor* GTUtilsMsaEditor::getEditor() {
    MsaEditorWgt* editorUi = getEditorUi();
    return editorUi->getEditor();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getEditorUi"
MsaEditorWgt* GTUtilsMsaEditor::getEditorUi() {
    checkMsaEditorWindowIsActive();
    MsaEditorWgt* msaEditorWgt = nullptr;
    // For some reason MsaEditorWgt is not within normal widgets hierarchy (wrong parent?),
    // so can't use GTWidget::findWidget here.
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
    // Get #0 editor widget
    return qobject_cast<MsaEditorWgt*>(msaEditorWgt->getEditor()->getMaEditorWgt(0));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOverviewArea"
QWidget* GTUtilsMsaEditor::getOverviewArea() {
    QWidget* activeWindow = getActiveMsaEditorWindow();
    return GTWidget::findExactWidget<MaEditorOverviewArea*>(MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME, activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getShowOverviewButton"
QToolButton* GTUtilsMsaEditor::getShowOverviewButton() {
    auto showOverviewButton = qobject_cast<QToolButton*>(GTAction::button("Show overview"));
    CHECK_SET_ERR_RESULT(showOverviewButton != nullptr, "Overview button is not found", nullptr);
    return showOverviewButton;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGraphOverview"
QWidget* GTUtilsMsaEditor::getGraphOverview() {
    QWidget* overviewArea = getOverviewArea();
    return GTWidget::findExactWidget<MaGraphOverview*>(MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME + "_graph", overviewArea);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSimpleOverview"
QWidget* GTUtilsMsaEditor::getSimpleOverview() {
    QWidget* overviewArea = getOverviewArea();
    return GTWidget::findExactWidget<MaSimpleOverview*>(MSAEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME + "_simple", overviewArea);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeView"
QGraphicsView* GTUtilsMsaEditor::getTreeView() {
    QWidget* activeWindow = getActiveMsaEditorWindow();
    return GTWidget::findExactWidget<QGraphicsView*>("treeView", activeWindow);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoTreeView"
void GTUtilsMsaEditor::checkNoTreeView() {
    QWidget* activeWindow = getActiveMsaEditorWindow();
    auto treeView = GTWidget::findExactWidget<MSAEditorTreeViewerUI*>("treeView", activeWindow, {false});
    CHECK_SET_ERR(treeView == nullptr, "checkNoTreeView: found a tree view");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNameListArea"
MaEditorNameList* GTUtilsMsaEditor::getNameListArea() {
    // There are more than one msa_editor_name_list in multiline mode, so at first we get line #0 widget
    MaEditorWgt* activeWindow = GTUtilsMsaEditor::getEditor()->getUI()->getUI(0);
    auto result = GTWidget::findExactWidget<MaEditorNameList*>("msa_editor_name_list", activeWindow);
    GT_CHECK_RESULT(result != nullptr, "MaGraphOverview is not found", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getConsensusArea"
MSAEditorConsensusArea* GTUtilsMsaEditor::getConsensusArea(int index) {
    return GTUtilsMSAEditorSequenceArea::getConsensusArea(index);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceArea"
MSAEditorSequenceArea* GTUtilsMsaEditor::getSequenceArea(int index) {
    return GTUtilsMSAEditorSequenceArea::getSequenceArea(index);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRectByName"
QRect GTUtilsMsaEditor::getSequenceNameRect(const QString& sequenceName) {
    QStringList rowNames = GTUtilsMSAEditorSequenceArea::getCurrentRowNames();
    int viewRowIndex = rowNames.indexOf(sequenceName);
    GT_CHECK_RESULT(viewRowIndex >= 0, QString("Sequence '%1' not found").arg(sequenceName), {});
    return getSequenceNameRect(viewRowIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequenceNameRectByIndex"
QRect GTUtilsMsaEditor::getSequenceNameRect(int viewRowIndex) {
    GT_CHECK_RESULT(viewRowIndex >= 0, QString("Sequence '%1' not found").arg(viewRowIndex), QRect());

    U2Region nameListYRegion = getEditorUi()->getRowHeightController()->getScreenYRegionByViewRowIndex(viewRowIndex);
    MaEditorNameList* nameList = getNameListArea();
    QPoint topLeftPoint = nameList->mapToGlobal(QPoint(0, (int)nameListYRegion.startPos));
    QPoint bottomRightPoint = nameList->mapToGlobal(QPoint(nameList->width(), (int)nameListYRegion.endPos()));
    return QRect(topLeftPoint, bottomRightPoint);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColumnHeaderRect"
QRect GTUtilsMsaEditor::getColumnHeaderRect(int column) {
    MSAEditorConsensusArea* consensusArea = getConsensusArea();
    GT_CHECK_RESULT(nullptr != consensusArea, "Consensus area is NULL", QRect());
    MSAEditorSequenceArea* sequenceArea = getSequenceArea();
    GT_CHECK_RESULT(nullptr != sequenceArea, "Sequence area is NULL", QRect());
    MSAEditor* editor = getEditor();
    GT_CHECK_RESULT(nullptr != editor, "MSA Editor is NULL", QRect());

    BaseWidthController* baseWidthController = editor->getUI()->getUI(0)->getBaseWidthController();
    return QRect(consensusArea->mapToGlobal(QPoint(baseWidthController->getBaseScreenOffset(column),
                                                   consensusArea->geometry().top())),
                 QSize(baseWidthController->getBaseWidth(),
                       consensusArea->height()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "replaceSequence"
void GTUtilsMsaEditor::replaceSequence(const QString& sequenceToReplace, int targetPosition) {
    clickSequenceName(sequenceToReplace);

    targetPosition = qMax(0, qMin(getSequencesCount() - 1, targetPosition));
    const QString targetSequenceName = GTUtilsMSAEditorSequenceArea::getNameList()[targetPosition];

    const QPoint dragFrom = getSequenceNameRect(sequenceToReplace).center();
    const QPoint dragTo = getSequenceNameRect(targetSequenceName).center();

    GTMouseDriver::moveTo(dragFrom);
    GTMouseDriver::press();
    GTMouseDriver::moveTo(dragTo);
    GTMouseDriver::release();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "replaceSequence"
void GTUtilsMsaEditor::replaceSequence(int rowNumber, int targetPosition) {
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    GT_CHECK(0 <= rowNumber && rowNumber <= names.size(), "Row number is out of boundaries");
    replaceSequence(names[rowNumber], targetPosition);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeColumn"
void GTUtilsMsaEditor::removeColumn(int column) {
    clickColumn(column);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeRows"
void GTUtilsMsaEditor::removeRows(int firstRowNumber, int lastRowNumber) {
    selectRows(firstRowNumber, lastRowNumber);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToSequence"
void GTUtilsMsaEditor::moveToSequence(int rowNumber) {
    QRect sequenceNameRect = getSequenceNameRect(rowNumber);
    GTMouseDriver::moveTo(sequenceNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToSequenceName"
void GTUtilsMsaEditor::moveToSequenceName(const QString& sequenceName) {
    QRect sequenceNameRect = getSequenceNameRect(sequenceName);
    GTMouseDriver::moveTo(sequenceNameRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSequence"
void GTUtilsMsaEditor::clickSequence(int rowNumber, Qt::MouseButton mouseButton) {
    moveToSequence(rowNumber);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSequenceName"
void GTUtilsMsaEditor::clickSequenceName(const QString& sequenceName, const Qt::MouseButton& mouseButton, const Qt::KeyboardModifiers& modifiers) {
    moveToSequenceName(sequenceName);

    QList<Qt::Key> modifierKeys = GTKeyboardDriver::modifiersToKeys(modifiers);
    for (auto key : qAsConst(modifierKeys)) {
        GTKeyboardDriver::keyPress(key);
    }
    GTMouseDriver::click(mouseButton);

    std::reverse(modifierKeys.begin(), modifierKeys.end());
    for (auto key : qAsConst(modifierKeys)) {
        GTKeyboardDriver::keyRelease(key);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToColumn"
void GTUtilsMsaEditor::moveToColumn(int column) {
    GTUtilsMSAEditorSequenceArea::scrollToPosition(QPoint(column, 1));
    const QRect columnHeaderRect = getColumnHeaderRect(column);
    GTMouseDriver::moveTo(columnHeaderRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickColumn"
void GTUtilsMsaEditor::clickColumn(int column, Qt::MouseButton mouseButton) {
    moveToColumn(column);
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRows"
void GTUtilsMsaEditor::selectRows(int firstRowIndex, int lastRowIndex, GTGlobals::UseMethod method) {
    switch (method) {
        case GTGlobals::UseKey:
            clickSequence(firstRowIndex);
            GTKeyboardDriver::keyPress(Qt::Key_Shift);
            clickSequence(lastRowIndex);
            GTKeyboardDriver::keyRelease(Qt::Key_Shift);
            break;
        case GTGlobals::UseMouse:
            GTMouseDriver::dragAndDrop(getSequenceNameRect(firstRowIndex).center(),
                                       getSequenceNameRect(lastRowIndex).center());
            break;
        case GTGlobals::UseKeyBoard:
            GT_FAIL("Not implemented", );
        default:
            GT_FAIL("An unknown method", );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRowsByName"
void GTUtilsMsaEditor::selectRowsByName(const QStringList& rowNames) {
    for (const QString& rowName : qAsConst(rowNames)) {
        clickSequenceName(rowName, Qt::LeftButton, Qt::ControlModifier);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectColumns"
void GTUtilsMsaEditor::selectColumns(int firstColumnNumber, int lastColumnNumber, GTGlobals::UseMethod method) {
    switch (method) {
        case GTGlobals::UseKey:
            clickColumn(firstColumnNumber);
            GTKeyboardDriver::keyPress(Qt::Key_Shift);
            clickColumn(lastColumnNumber);
            GTKeyboardDriver::keyRelease(Qt::Key_Shift);
            break;
        case GTGlobals::UseMouse:
            GTMouseDriver::dragAndDrop(getColumnHeaderRect(firstColumnNumber).center(),
                                       getColumnHeaderRect(lastColumnNumber).center());
            break;
        case GTGlobals::UseKeyBoard:
            GT_FAIL("Not implemented", );
        default:
            GT_FAIL("An unknown method", );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clearSelection"
void GTUtilsMsaEditor::clearSelection() {
    if (!getEditor()->getSelection().isEmpty()) {
        GTKeyboardDriver::keyClick(Qt::Key_Escape);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNameList"
void GTUtilsMsaEditor::checkNameList(const QStringList& nameList) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor();
    QStringList nameListInEditor = editor->getMaObject()->getMultipleAlignment()->getRowNames();
    CHECK_SET_ERR(nameListInEditor == nameList, "Name list does not match");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelection"
void GTUtilsMsaEditor::checkSelection(const QList<QRect>& expectedRects) {
    MSAEditor* msaEditor = GTUtilsMsaEditor::getEditor();
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
void GTUtilsMsaEditor::checkSelectionByNames(const QStringList& selectedNames) {
    MSAEditor* editor = GTUtilsMsaEditor::getEditor();
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
QString GTUtilsMsaEditor::getReferenceSequenceName() {
    return GTUtilsOptionPanelMsa::getReference();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCollapsingMode"
void GTUtilsMsaEditor::toggleCollapsingMode() {
    GTWidget::click(GTToolbar::getWidgetForActionTooltip(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "Switch on/off collapsing"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isSequenceCollapsed"
bool GTUtilsMsaEditor::isSequenceCollapsed(const QString& seqName) {
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    GT_CHECK_RESULT(names.contains(seqName), "sequence " + seqName + " not found in name list", false);
    QStringList visiablenames = GTUtilsMSAEditorSequenceArea::getVisibleNames();

    return !visiablenames.contains(seqName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleCollapsingMode"
void GTUtilsMsaEditor::toggleCollapsingGroup(const QString& groupName) {
    const QRect sequenceNameRect = getSequenceNameRect(groupName);
    QPoint magicExpandButtonOffset;
    if (isOsWindows()) {
        magicExpandButtonOffset = QPoint(15, 10);
    } else {
        magicExpandButtonOffset = QPoint(15, 5);
    }
    GTMouseDriver::moveTo(sequenceNameRect.topLeft() + magicExpandButtonOffset);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSequencesCount"
int GTUtilsMsaEditor::getSequencesCount() {
    auto statusWidget = GTWidget::findWidget("msa_editor_status_bar");
    return GTMSAEditorStatusWidget::getSequencesCount(statusWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWholeData"
QStringList GTUtilsMsaEditor::getWholeData() {
    const QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    GT_CHECK_RESULT(!names.isEmpty(), "The name list is empty", QStringList());

    clickSequenceName(names.first());
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    clickSequenceName(names.last());
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    GTKeyboardUtils::copy();
    GTGlobals::sleep(500);

    return GTClipboard::text().split('\n');
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "undo"
void GTUtilsMsaEditor::undo() {
    getActiveMsaEditorWindow();
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_undo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "redo"
void GTUtilsMsaEditor::redo() {
    getActiveMsaEditorWindow();
    GTWidget::click(GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_redo"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsMsaEditor::zoomIn() {
    QToolBar* toolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    QWidget* zoomInButton = GTToolbar::getWidgetForActionObjectName(toolbar, "Zoom In");
    GTWidget::click(zoomInButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomOut"
void GTUtilsMsaEditor::zoomOut() {
    QToolBar* toolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    QWidget* zoomOutButton = GTToolbar::getWidgetForActionObjectName(toolbar, "Zoom Out");
    GTWidget::click(zoomOutButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToSelection"
void GTUtilsMsaEditor::zoomToSelection() {
    QToolBar* toolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    QWidget* zoomToSelectionButton = GTToolbar::getWidgetForActionObjectName(toolbar, "Zoom To Selection");
    GT_CHECK_RESULT(zoomToSelectionButton->isEnabled(), "zoomToSelectionButton is not enabled", );
    GTWidget::click(zoomToSelectionButton);

    // 'zoomToSelection' is a 2 steps action: the second step (the centering) is run with a 200ms delay.
    // See MaEditor::sl_zoomToSelection().
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "resetZoom"
void GTUtilsMsaEditor::resetZoom() {
    QToolBar* toolbar = GTToolbar::getToolbar("mwtoolbar_activemdi");
    QWidget* resetZoomButton = GTToolbar::getWidgetForActionObjectName(toolbar, "Reset Zoom");
    GT_CHECK_RESULT(resetZoomButton->isEnabled(), "resetZoomButton is not enabled", );
    GTWidget::click(resetZoomButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isUndoEnabled"
bool GTUtilsMsaEditor::isUndoEnabled() {
    getActiveMsaEditorWindow();
    return GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_undo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isRedoEnabled"
bool GTUtilsMsaEditor::isRedoEnabled() {
    getActiveMsaEditorWindow();
    return GTToolbar::getWidgetForActionObjectName(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "msa_action_redo")->isEnabled();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "buildPhylogeneticTree"
void GTUtilsMsaEditor::buildPhylogeneticTree(const QString& pathToSave) {
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(pathToSave, 0, 0, true));
    clickBuildTreeButton();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickBuildTreeButton"
void GTUtilsMsaEditor::clickBuildTreeButton() {
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Build Tree");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeActiveTreeTab"
void GTUtilsMsaEditor::closeActiveTreeTab() {
    auto treeTabWidget = GTWidget::findWidget("msa_editor_tree_tab_area", getActiveMsaEditorWindow());

    QTabBar* tabBar = treeTabWidget->findChild<QTabBar*>();
    GT_CHECK(tabBar != nullptr, "Tree tab widget must have a tab bar!");

    int tabIndex = tabBar->currentIndex();
    GT_CHECK(tabIndex >= 0, "TabBar must have a current tab!");

    QWidget* closeTabButton = tabBar->tabButton(tabIndex, QTabBar::RightSide);
    if (closeTabButton == nullptr) {
        closeTabButton = tabBar->tabButton(tabIndex, QTabBar::RightSide);
        GT_CHECK(closeTabButton != nullptr, "TabBar must have close button!");
    }
    GTWidget::click(closeTabButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDropSequenceFromProject"
void GTUtilsMsaEditor::dragAndDropSequenceFromProject(const QStringList& pathToSequence) {
    GTUtilsProjectTreeView::dragAndDrop(pathToSequence, getEditorUi());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "activateAlignSequencesToAlignmentMenu"
void GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu(const QString& partOfMenuItemText) {
    GTUtilsDialog::waitForDialog(new PopupChooserByText({partOfMenuItemText}, GTGlobals::UseMouse, Qt::MatchContains));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAlignSequencesToAlignmentMenu"
void GTUtilsMsaEditor::checkAlignSequencesToAlignmentMenu(const QString& partOfMenuItemText, const PopupChecker::CheckOption& checkOption) {
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({partOfMenuItemText}, checkOption, GTGlobals::UseMouse, Qt::MatchContains));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Align sequence(s) to this alignment");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReference"
void GTUtilsMsaEditor::setReference(const QString& sequenceName) {
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Set this sequence as reference"}, GTGlobals::UseMouse));
    clickSequenceName(sequenceName, Qt::RightButton);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openExcludeList"
void GTUtilsMsaEditor::openExcludeList(bool waitUntilLoaded) {
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "exclude_list_toggle_action");
    if (!toggleExcludeListButton->isChecked()) {
        GTWidget::click(toggleExcludeListButton);
        if (waitUntilLoaded) {
            GTUtilsTaskTreeView::waitTaskFinished();
        }
    }
    GTWidget::findWidget("msa_exclude_list", msaEditorWindow);  // Test that Exclude List is present.
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "closeExcludeList"
void GTUtilsMsaEditor::closeExcludeList(bool waitUntilFinished) {
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto toggleExcludeListButton = GTToolbar::getToolButtonByAction(GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI), "exclude_list_toggle_action");
    if (toggleExcludeListButton->isChecked()) {
        GTWidget::click(toggleExcludeListButton);
        if (waitUntilFinished) {
            GTUtilsTaskTreeView::waitTaskFinished();
        }
    }
    // Test that Exclude List is not present.
    CHECK_SET_ERR(GTWidget::findWidget("msa_exclude_list", msaEditorWindow, {false}) == nullptr, "Exclude List widget is present");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveRowsToExcludeList"
void GTUtilsMsaEditor::moveRowsToExcludeList(const QStringList& rowNames) {
    GTUtilsMsaEditor::clearSelection();
    GTUtilsMsaEditor::selectRowsByName(rowNames);
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto button = GTWidget::findToolButton("exclude_list_move_from_msa_button", msaEditorWindow);
    CHECK_SET_ERR(button->isEnabled(), "Button is not enabled: " + button->objectName());
    GTWidget::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveRowFromExcludeList"
void GTUtilsMsaEditor::moveRowFromExcludeList(const QString& rowName) {
    auto listWidget = getExcludeListWidget();
    GTListWidget::click(listWidget, rowName);

    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto button = GTWidget::findToolButton("exclude_list_move_to_msa_button", msaEditorWindow);
    CHECK_SET_ERR(button->isEnabled(), "Button is not enabled: " + button->objectName());
    GTWidget::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkExcludeList"
void GTUtilsMsaEditor::checkExcludeList(const QStringList& nameList) {
    auto listWidget = getExcludeListWidget();
    QStringList actualNameList = GTListWidget::getItems(listWidget);
    CHECK_SET_ERR(actualNameList == nameList, "Name list does not match, expected: " + nameList.join(";") + ", got: " + actualNameList.join(";"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectRowsByNameInExcludeList"
void GTUtilsMsaEditor::selectRowsByNameInExcludeList(const QStringList& rowNames) {
    auto listWidget = getExcludeListWidget();
    GTListWidget::selectItemsByText(listWidget, rowNames);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkExcludeListSelection"
void GTUtilsMsaEditor::checkExcludeListSelection(const QStringList& rowNames) {
    auto listWidget = getExcludeListWidget();
    GTListWidget::checkSelection(listWidget, rowNames);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExcludeListWidget"
QListWidget* GTUtilsMsaEditor::getExcludeListWidget() {
    auto msaEditorWindow = GTUtilsMsaEditor::getActiveMsaEditorWindow();
    auto excludeList = GTWidget::findWidget("msa_exclude_list", msaEditorWindow);
    return GTWidget::findListWidget("exclude_list_name_list_widget", excludeList);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isMultilineMode"
bool GTUtilsMsaEditor::getMultilineMode() {
    auto toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);

    // Get state of the "Multiline View" button on toolbar
    auto mmode = GTToolbar::getToolButtonByAction(toolbar, "multilineView");
    return mmode->isVisible() && mmode->isEnabled() && mmode->isChecked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setMultilineMode"
void GTUtilsMsaEditor::setMultilineMode(bool isMultilineMode) {
    auto toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);

    // Press "Multiline View" button on toolbar
    auto multilineModeButton = GTToolbar::getToolButtonByAction(toolbar, "multilineView");
    CHECK_SET_ERR_RESULT(multilineModeButton->isVisible(), "\"Multiline View\" button is not visible", );
    CHECK_SET_ERR_RESULT(multilineModeButton->isEnabled(), "\"Multiline View\" button is disabled", );
    CHECK(getMultilineMode() != isMultilineMode, );

    GTWidget::click(multilineModeButton);
    for (int time = 0; time < GT_OP_WAIT_MILLIS && getMultilineMode() != isMultilineMode; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
    }
    CHECK_SET_ERR_RESULT(getMultilineMode() == isMultilineMode, "Multiline mode is not changed", );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "gotoWithKeyboardShortcut"
void GTUtilsMsaEditor::gotoWithKeyboardShortcut(int pos, bool isVisualPosition) {
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsDialog::waitForDialog(new GoToDialogFiller(pos + (isVisualPosition ? 0 : -1)));
    GTKeyboardDriver::keyClick('g', Qt::ControlModifier);
    GTUtilsDialog::checkNoActiveWaiters();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
