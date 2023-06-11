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

#include "U2View/MsaEditorSimilarityColumn.h"
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"

namespace U2 {

class GTUtilsMSAEditorSequenceArea {
public:
    // The "index" is line index while multiline mode
    static MSAEditorSequenceArea* getSequenceArea(int index = 0, bool checkError = true);
    static MSAEditorConsensusArea* getConsensusArea(int index = 0, bool checkError = true);
    static MsaEditorSimilarityColumn* getSimilarityColumn(int index = 0);
    static QScrollBar* getHorizontalNamesScroll(int index = 0);
    static void callContextMenu(const QPoint& innerCoords = QPoint());  // zero-based position

    static void checkSelectedRect(int multilineIndex, const QRect& expectedRect);
    static void checkSelectedRect(const QRect& expectedRect);

    static void checkConsensus(QString cons, int index = 0);
    // may be used for selecting visible columns only
    static void selectColumnInConsensus(int columnNumber, int index = 0);

    // MSAEditorNameList

    /** Returns all names in MSA object as stored in the object. */
    static QStringList getNameList();

    /**
     * Returns ordered list of row names rendered on the screen: both from the visible on the screen are and from the overflow (available with the scroll).
     * Excludes invisible rows inside of collapsed groups.
     * */
    static QStringList getCurrentRowNames();

    static bool hasSequencesWithNames(const QStringList& names);

    /**
     * Returns list of visible row names.
     * Uses original MSA row names if 'asShownInNameList' is false (default) or the final rendered
     * row text if 'asShownInNameList' is true.
     */
    static QStringList getVisibleNames(bool asShownInNameList = false);

    // "index" is a line index in multiline mode
    static QString getSimilarityValue(int row, int index = 0);
    static void clickCollapseTriangle(QString seqName);
    static bool isCollapsed(QString seqName);
    static bool collapsingMode();

    static int getFirstVisibleBaseIndex(int multilineIndex = 0, bool countClipped = true);
    static int getLastVisibleBaseIndex(int multilineIndex = 0);

    /** Returns index of the first visible view row index in the MSA sequence area. */
    static int getFirstVisibleRowIndex(bool countClipped = false);

    /** Returns index of the last visible view row index in the MSA sequence area. */
    static int getLastVisibleRowIndex(bool countClipped = false);

    static int getLength();
    static int getNumVisibleBases();

    static QRect getSelectedRect();
    static void dragAndDropSelection(const QPoint& fromMaPosition, const QPoint& toMaPosition);

    static void moveTo(const QPoint& p);

    // selects area in MSA coordinates, if some p coordinate less than 0, it becomes max valid coordinate
    // zero-based position
    static void selectArea(QPoint p1 = QPoint(0, 0), QPoint p2 = QPoint(-1, -1), GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void selectArea(int multilineIndex, QPoint p1 = QPoint(0, 0), QPoint p2 = QPoint(-1, -1), GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void cancelSelection();
    /** Returns on-screen (global) bounding rectangle for the base position. */
    static QRect getPositionRect(const QPoint& position, int index = 0);
    static QPoint convertCoordinates(const QPoint p, int index = 0);
    static void click(const QPoint& screenMaPoint = QPoint(0, 0));

    /** Calls context menu Copy/Paste->Copy. */
    static void copySelectionByContextMenu();

    // scrolls to the position (in the MSA zero-based coordinates)
    static void scrollToPosition(const QPoint& position);
    static void scrollToBottom();
    static void moveMouseToPosition(const QPoint& globalMaPosition);
    static void clickToPosition(const QPoint& globalMaPosition);

    static void selectSequence(const QString& seqName);
    static void selectSequence(const int row);
    static bool isSequenceSelected(const QString& seqName);
    static void removeSequence(const QString& sequenceName);
    static int getSelectedSequencesNum();
    static bool isSequenceVisible(const QString& seqName);
    static QString getSequenceData(const QString& sequenceName);
    static QString getSequenceData(int rowNumber);

    static bool offsetsVisible();

    static bool hasAminoAlphabet();
    static bool isSequenceHighlighted(const QString& seqName);
    static QString getColor(QPoint p);
    static bool hasPixelWithColor(const QPoint& p, const QColor& color);
    static void checkMsaCellColors(const QPoint& pos, const QString& fgColor, const QString& bgColor);
    static void checkMsaCellColor(const QPoint& pos, const QString& color);
    static bool checkColor(const QPoint& p, const QString& expectedColor);
    static int getRowHeight(int rowNumber);

    static void renameSequence(const QString& seqToRename, const QString& newName, bool useCopyPaste = false);
    static void replaceSymbol(const QPoint& maPoint, char newSymbol);

    static void createColorScheme(const QString& schemeName, const NewColorSchemeCreator::alphabet al);
    static void deleteColorScheme(const QString& schemeName);

    static void checkSelection(const QPoint& start, const QPoint& end, const QString& expected);

    static bool isAlignmentLocked();

    /*
     *expandedBorder: 0 - top, 1 - right, 2 - bottom, 3 - left, 4 - right top, 5 - right bottom, 6 - left bottom, 7 - left top
     */
    static void expandSelectedRegion(const int expandedBorder, const int symbolsToExpand);

    static void zoomIn();
    static void zoomOut();
    static void zoomToMax();
    static void zoomToMin();

    static const QString highlightingColor;
};

}  // namespace U2
