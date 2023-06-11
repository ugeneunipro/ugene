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

#pragma once

#include <GTGlobals.h>
#include <primitives/PopupChooser.h>

#include <QColor>
#include <QGraphicsView>
#include <QListWidget>
#include <QRect>
#include <QToolButton>

namespace U2 {

class MSAEditor;
class MSAEditorConsensusArea;
class MaEditorNameList;
class MSAEditorSequenceArea;
class MsaEditorWgt;

// If you can't find an appropriate method check the GTUtilsMsaEditorSequenceArea class
class GTUtilsMsaEditor {
public:
    /** Returns active MSA editor window or fails. */
    static QWidget* getActiveMsaEditorWindow();

    /** Checks that the active MDI window is MSA editor window or fails. */
    static void checkMsaEditorWindowIsActive();

    /** Checks that there are no MSA editor window opened (active or not active). */
    static void checkNoMsaEditorWindowIsOpened();

    /** Returns "Show overview" toolbar button. */
    static QToolButton* getShowOverviewButton();

    static QColor getGraphOverviewPixelColor(const QPoint& point);
    static QColor getSimpleOverviewPixelColor(const QPoint& point);

    static MSAEditor* getEditor();
    static MsaEditorWgt* getEditorUi();

    /** Returns overview area widget: a container of graph & simple overviews. */
    static QWidget* getOverviewArea();

    static QWidget* getGraphOverview();
    static QWidget* getSimpleOverview();

    static QGraphicsView* getTreeView();

    /** Checks that there is no Tree view opened in the active MSA editor. */
    static void checkNoTreeView();

    static MaEditorNameList* getNameListArea();
    static MSAEditorConsensusArea* getConsensusArea(int index = 0);
    static MSAEditorSequenceArea* getSequenceArea(int index = 0);

    static QRect getSequenceNameRect(const QString& sequenceName);
    static QRect getSequenceNameRect(int viewRowIndex);
    static QRect getColumnHeaderRect(int column);

    static void replaceSequence(const QString& sequenceToReplace, int targetPosition);
    static void replaceSequence(int rowNumber, int targetPosition);
    static void removeColumn(int column);
    static void removeRows(int firstRowNumber, int lastRowNumber);

    static void moveToSequence(int rowNumber);
    static void moveToSequenceName(const QString& sequenceName);
    static void clickSequence(int rowNumber, Qt::MouseButton mouseButton = Qt::LeftButton);

    /** Clicks sequence with a mouse button and a keyboard key pressed. */
    static void clickSequenceName(
        const QString& sequenceName,
        const Qt::MouseButton& mouseButton = Qt::LeftButton,
        const Qt::KeyboardModifiers& modifiers = Qt::NoModifier);

    static void moveToColumn(int column);
    static void clickColumn(int column, Qt::MouseButton mouseButton = Qt::LeftButton);

    static void selectRows(int firstRowIndex, int lastRowIndex, HI::GTGlobals::UseMethod method = HI::GTGlobals::UseKey);

    /** Select rows in the name list by name using Ctrl + Mouse click. Fails if any of the rows is not found. */
    static void selectRowsByName(const QStringList& rowNames);

    static void selectColumns(int firstColumnNumber, int lastColumnNumber, HI::GTGlobals::UseMethod method = HI::GTGlobals::UseKey);

    /** Checks that MSA editor selection is equal to the given rect. Fails if not. */
    static void checkSelection(const QList<QRect>& expectedRects);

    /** Checks that the current selection names & order are equal to 'selectedNames' list. */
    static void checkSelectionByNames(const QStringList& selectedNames);

    static void clearSelection();

    /** Checks current sequence (row) name list (with order). */
    static void checkNameList(const QStringList& nameList);

    static QString getReferenceSequenceName();
    static void setReference(const QString& sequenceName);

    static void toggleCollapsingMode();
    static void toggleCollapsingGroup(const QString& groupName);
    static bool isSequenceCollapsed(const QString& seqName);

    static int getSequencesCount();
    static QStringList getWholeData();

    static void undo();
    static void redo();

    /** Clicks zoom-in button if the button is enabled. Does nothing if the button is disabled. */
    static void zoomIn();

    /** Clicks zoom-out button if the button is enabled. Does nothing if the button is disabled. */
    static void zoomOut();

    /** Clicks zoom-to-selection button if the button is enabled. Fails if the button is disabled. */
    static void zoomToSelection();

    /** Clicks reset zoom button if the button is enabled. Fails if the button is disabled. */
    static void resetZoom();

    static bool isUndoEnabled();
    static bool isRedoEnabled();

    static void buildPhylogeneticTree(const QString& pathToSave);

    /** Clicks 'Build Tree' button. A dialog filler must be already be set by the caller. */
    static void clickBuildTreeButton();

    /** Closes the active tree tab. Fails if there is no phy-tab found. */
    static void closeActiveTreeTab();

    static void dragAndDropSequenceFromProject(const QStringList& pathToSequence);

    /** Clicks "Align sequence(s) to alignment" button and selects a menu item with the given text. */
    static void activateAlignSequencesToAlignmentMenu(const QString& partOfMenuItemText);

    /** Clicks "Align sequence(s) to alignment" button has the given state. */
    static void checkAlignSequencesToAlignmentMenu(const QString& partOfMenuItemText, const HI::PopupChecker::CheckOption& checkOption);

    /**
     * Clicks "Exclude list" button on the toolbar (if needed) and checks that "Exclude List" widget is present.
     * 'waitUntilLoaded' calls to 'waitTaskFinished' and is needed to wait until the content of exclude list is loaded.
     */
    static void openExcludeList(bool waitUntilLoaded = true);

    /**
     * Ensures exclude list is closed.
     * 'waitUntilSaved' calls to 'waitTaskFinished' and is needed to wait until the content of exclude list is saved.
     */
    static void closeExcludeList(bool waitUntilSaved = true);

    /** Moves rows with the given names from MSA to Exclude List. */
    static void moveRowsToExcludeList(const QStringList& rowNames);

    /** Moves row with the given name from Exclude List to MSA. */
    static void moveRowFromExcludeList(const QString& rowName);

    /** Check current exclude list content. The Exclude List must be opened. */
    static void checkExcludeList(const QStringList& rowNames);

    /** Selects rows by name in exclude list. */
    static void selectRowsByNameInExcludeList(const QStringList& rowNames);

    /** Checks that the given set of rows is currently selected in Exclude List. */
    static void checkExcludeListSelection(const QStringList& rowNames);

    /** Returns active Exclude List widget instance. */
    static QListWidget* getExcludeListWidget();

    // Get current multiline mode, return true if multiline mode is active
    static bool getMultilineMode();
    // Press "Multiline View" button on toolbar
    // If mode != getMultilineMode()
    // Then the multiline mode is changed
    // Else nothing is being done
    static void setMultilineMode(bool isMultilineMode);

    /**
     * Clicks Control + G and submits the position. Requires no pending dialog waiters.
     * If 'isVisualPosition' is true, the 'pos' is visual as typed in the GoTo dialog (starts with 1).
     */
    static void gotoWithKeyboardShortcut(int pos, bool isVisualPosition = true);
};

}  // namespace U2
