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

#ifndef _U2_GT_UTILS_MSA_EDITOR_H_
#define _U2_GT_UTILS_MSA_EDITOR_H_

#include <GTGlobals.h>
#include <primitives/PopupChooser.h>

#include <QColor>
#include <QListWidget>
#include <QRect>
#include <QToolButton>

namespace U2 {

class MSAEditor;
class MSAEditorConsensusArea;
class MaEditorNameList;
class MSAEditorSequenceArea;
class MSAEditorTreeViewerUI;
class MsaEditorWgt;

// If you can't find an appropriate method check the GTUtilsMsaEditorSequenceArea class
class GTUtilsMsaEditor {
public:
    /** Returns active MSA editor window or fails. */
    static QWidget* getActiveMsaEditorWindow(HI::GUITestOpStatus& os);

    /** Checks that the active MDI window is MSA editor window or fails. */
    static void checkMsaEditorWindowIsActive(HI::GUITestOpStatus& os);

    /** Checks that there are no MSA editor window opened (active or not active). */
    static void checkNoMsaEditorWindowIsOpened(HI::GUITestOpStatus& os);

    /** Returns "Show overview" toolbar button. */
    static QToolButton* getShowOverviewButton(HI::GUITestOpStatus& os);

    static QColor getGraphOverviewPixelColor(HI::GUITestOpStatus& os, const QPoint& point);
    static QColor getSimpleOverviewPixelColor(HI::GUITestOpStatus& os, const QPoint& point);

    static MSAEditor* getEditor(HI::GUITestOpStatus& os);
    static MsaEditorWgt* getEditorUi(HI::GUITestOpStatus& os);

    /** Returns overview area widget: a container of graph & simple overviews. */
    static QWidget* getOverviewArea(HI::GUITestOpStatus& os);

    static QWidget* getGraphOverview(HI::GUITestOpStatus& os);
    static QWidget* getSimpleOverview(HI::GUITestOpStatus& os);

    static MSAEditorTreeViewerUI* getTreeView(HI::GUITestOpStatus& os);

    /** Checks that there is no Tree view opened in the active MSA editor. */
    static void checkNoTreeView(HI::GUITestOpStatus& os);

    static MaEditorNameList* getNameListArea(HI::GUITestOpStatus& os);
    static MSAEditorConsensusArea* getConsensusArea(HI::GUITestOpStatus& os);
    static MSAEditorSequenceArea* getSequenceArea(HI::GUITestOpStatus& os);

    static QRect getSequenceNameRect(HI::GUITestOpStatus& os, const QString& sequenceName);
    static QRect getSequenceNameRect(HI::GUITestOpStatus& os, int viewRowIndex);
    static QRect getColumnHeaderRect(HI::GUITestOpStatus& os, int column);

    static void replaceSequence(HI::GUITestOpStatus& os, const QString& sequenceToReplace, int targetPosition);
    static void replaceSequence(HI::GUITestOpStatus& os, int rowNumber, int targetPosition);
    static void removeColumn(HI::GUITestOpStatus& os, int column);
    static void removeRows(HI::GUITestOpStatus& os, int firstRowNumber, int lastRowNumber);

    static void moveToSequence(HI::GUITestOpStatus& os, int rowNumber);
    static void moveToSequenceName(HI::GUITestOpStatus& os, const QString& sequenceName);
    static void clickSequence(HI::GUITestOpStatus& os, int rowNumber, Qt::MouseButton mouseButton = Qt::LeftButton);

    /** Clicks sequence with a mouse button and a keyboard key pressed. */
    static void clickSequenceName(HI::GUITestOpStatus& os,
                                  const QString& sequenceName,
                                  const Qt::MouseButton& mouseButton = Qt::LeftButton,
                                  const Qt::KeyboardModifiers& modifiers = Qt::NoModifier);

    static void moveToColumn(HI::GUITestOpStatus& os, int column);
    static void clickColumn(HI::GUITestOpStatus& os, int column, Qt::MouseButton mouseButton = Qt::LeftButton);

    static void selectRows(HI::GUITestOpStatus& os, int firstRowIndex, int lastRowIndex, HI::GTGlobals::UseMethod method = HI::GTGlobals::UseKey);

    /** Select rows in the name list by name using Ctrl + Mouse click. Fails if any of the rows is not found. */
    static void selectRowsByName(HI::GUITestOpStatus& os, const QStringList& rowNames);

    static void selectColumns(HI::GUITestOpStatus& os, int firstColumnNumber, int lastColumnNumber, HI::GTGlobals::UseMethod method = HI::GTGlobals::UseKey);

    /** Checks that MSA editor selection is equal to the given rect. Fails if not. */
    static void checkSelection(HI::GUITestOpStatus& os, const QList<QRect>& expectedRects);

    /** Checks that the current selection names & order are equal to 'selectedNames' list. */
    static void checkSelectionByNames(HI::GUITestOpStatus& os, const QStringList& selectedNames);

    static void clearSelection(HI::GUITestOpStatus& os);

    /** Checks current sequence (row) name list (with order). */
    static void checkNameList(HI::GUITestOpStatus& os, const QStringList& nameList);

    static QString getReferenceSequenceName(HI::GUITestOpStatus& os);
    static void setReference(HI::GUITestOpStatus& os, const QString& sequenceName);

    static void toggleCollapsingMode(HI::GUITestOpStatus& os);
    static void toggleCollapsingGroup(HI::GUITestOpStatus& os, const QString& groupName);
    static bool isSequenceCollapsed(HI::GUITestOpStatus& os, const QString& seqName);

    static int getSequencesCount(HI::GUITestOpStatus& os);
    static QStringList getWholeData(HI::GUITestOpStatus& os);

    static void undo(HI::GUITestOpStatus& os);
    static void redo(HI::GUITestOpStatus& os);

    /** Clicks zoom-in button if the button is enabled. Does nothing if the button is disabled. */
    static void zoomIn(HI::GUITestOpStatus& os);

    /** Clicks zoom-out button if the button is enabled. Does nothing if the button is disabled. */
    static void zoomOut(HI::GUITestOpStatus& os);

    /** Clicks zoom-to-selection button if the button is enabled. Fails if the button is disabled. */
    static void zoomToSelection(HI::GUITestOpStatus& os);

    /** Clicks reset zoom button if the button is enabled. Fails if the button is disabled. */
    static void resetZoom(HI::GUITestOpStatus& os);

    static bool isUndoEnabled(HI::GUITestOpStatus& os);
    static bool isRedoEnabled(HI::GUITestOpStatus& os);

    static void buildPhylogeneticTree(HI::GUITestOpStatus& os, const QString& pathToSave);

    /** Clicks 'Build Tree' button. A dialog filler must be already be set by the caller. */
    static void clickBuildTreeButton(HI::GUITestOpStatus& os);

    /** Closes the active tree tab. Fails if there is no phy-tab found. */
    static void closeActiveTreeTab(HI::GUITestOpStatus& os);

    static void dragAndDropSequenceFromProject(HI::GUITestOpStatus& os, const QStringList& pathToSequence);

    /** Clicks "Align sequence(s) to alignment" button and selects a menu item with the given text. */
    static void activateAlignSequencesToAlignmentMenu(HI::GUITestOpStatus& os, const QString& partOfMenuItemText);

    /** Clicks "Align sequence(s) to alignment" button has the given state. */
    static void checkAlignSequencesToAlignmentMenu(HI::GUITestOpStatus& os, const QString& partOfMenuItemText, const HI::PopupChecker::CheckOption& checkOption);

    /**
     * Clicks "Exclude list" button on the toolbar (if needed) and checks that "Exclude List" widget is present.
     * 'waitUntilLoaded' calls to 'waitTaskFinished' and is needed to wait until the content of exclude list is loaded.
     */
    static void openExcludeList(HI::GUITestOpStatus& os, bool waitUntilLoaded = true);

    /**
     * Ensures exclude list is closed.
     * 'waitUntilSaved' calls to 'waitTaskFinished' and is needed to wait until the content of exclude list is saved.
     */
    static void closeExcludeList(HI::GUITestOpStatus& os, bool waitUntilSaved = true);

    /** Moves rows with the given names from MSA to Exclude List. */
    static void moveRowsToExcludeList(HI::GUITestOpStatus& os, const QStringList& rowNames);

    /** Moves row with the given name from Exclude List to MSA. */
    static void moveRowFromExcludeList(HI::GUITestOpStatus& os, const QString& rowName);

    /** Check current exclude list content. The Exclude List must be opened. */
    static void checkExcludeList(HI::GUITestOpStatus& os, const QStringList& rowNames);

    /** Selects rows by name in exclude list. */
    static void selectRowsByNameInExcludeList(HI::GUITestOpStatus& os, const QStringList& rowNames);

    /** Checks that the given set of rows is currently selected in Exclude List. */
    static void checkExcludeListSelection(HI::GUITestOpStatus& os, const QStringList& rowNames);

    /** Returns active Exclude List widget instance. */
    static QListWidget* getExcludeListWidget(HI::GUITestOpStatus& os);
};

}  // namespace U2

#endif  // _U2_GT_UTILS_MSA_EDITOR_H_
