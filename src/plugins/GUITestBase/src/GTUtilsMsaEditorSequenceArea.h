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

#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorSequenceArea.h>
#include "U2View/MsaEditorSimilarityColumn.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"

namespace U2 {

class GTUtilsMSAEditorSequenceArea {
public:
    // The "index" is line index while multiline mode
    static MSAEditorSequenceArea* getSequenceArea(GUITestOpStatus& os, int index = 0, bool checkError = true);
    static MSAEditorConsensusArea* getConsensusArea(GUITestOpStatus& os, int index = 0, bool checkError = true);
    static MsaEditorSimilarityColumn *getSimilarityColumn(GUITestOpStatus &os, int index = 0);
    static QScrollBar *getHorizontalNamesScroll(GUITestOpStatus &os, int index = 0);
    static void callContextMenu(GUITestOpStatus& os, const QPoint& innerCoords = QPoint());  // zero-based position

    static void checkSelectedRect(GUITestOpStatus& os, int multilineIndex, const QRect& expectedRect);
    static void checkSelectedRect(GUITestOpStatus& os, const QRect& expectedRect);

    static void checkConsensus(GUITestOpStatus& os, QString cons, int index = 0);
    // may be used for selecting visible columns only
    static void selectColumnInConsensus(GUITestOpStatus& os, int columnNumber, int index = 0);

    // MSAEditorNameList

    /** Returns all names in MSA object as stored in the object. */
    static QStringList getNameList(GUITestOpStatus& os);

    /**
     * Returns ordered list of row names rendered on the screen: both from the visible on the screen are and from the overflow (available with the scroll).
     * Excludes invisible rows inside of collapsed groups.
     * */
    static QStringList getCurrentRowNames(GUITestOpStatus& os);

    static bool hasSequencesWithNames(GUITestOpStatus& os, const QStringList& names);

    /**
     * Returns list of visible row names.
     * Uses original MSA row names if 'asShownInNameList' is false (default) or the final rendered
     * row text if 'asShownInNameList' is true.
     */
    static QStringList getVisibleNames(GUITestOpStatus& os, bool asShownInNameList = false);

    // "index" is a line index in multiline mode
    static QString getSimilarityValue(GUITestOpStatus& os, int row, int index = 0);
    static void clickCollapseTriangle(GUITestOpStatus& os, QString seqName);
    static bool isCollapsed(GUITestOpStatus& os, QString seqName);
    static bool collapsingMode(GUITestOpStatus& os);

    static int getFirstVisibleBaseIndex(GUITestOpStatus& os, int multilineIndex = 0, bool countClipped = true);
    static int getLastVisibleBaseIndex(GUITestOpStatus& os, int multilineIndex = 0);

    /** Returns index of the first visible view row index in the MSA sequence area. */
    static int getFirstVisibleRowIndex(GUITestOpStatus& os, bool countClipped = false);

    /** Returns index of the last visible view row index in the MSA sequence area. */
    static int getLastVisibleRowIndex(GUITestOpStatus& os, bool countClipped = false);

    static int getLength(GUITestOpStatus& os);
    static int getNumVisibleBases(GUITestOpStatus& os);

    static QRect getSelectedRect(GUITestOpStatus& os);
    static void dragAndDropSelection(GUITestOpStatus& os, const QPoint& fromMaPosition, const QPoint& toMaPosition);

    static void moveTo(GUITestOpStatus& os, const QPoint& p);

    // selects area in MSA coordinates, if some p coordinate less than 0, it becomes max valid coordinate
    // zero-based position
    static void selectArea(GUITestOpStatus& os, QPoint p1 = QPoint(0, 0), QPoint p2 = QPoint(-1, -1), GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void selectArea(GUITestOpStatus& os, int multilineIndex, QPoint p1 = QPoint(0, 0), QPoint p2 = QPoint(-1, -1), GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void cancelSelection(GUITestOpStatus& os);
    /** Returns on-screen (global) bounding rectangle for the base position. */
    static QRect getPositionRect(GUITestOpStatus& os, const QPoint& position, int index = 0);
    static QPoint convertCoordinates(GUITestOpStatus& os, const QPoint p, int index = 0);
    static void click(GUITestOpStatus& os, const QPoint& screenMaPoint = QPoint(0, 0));

    /** Calls context menu Copy/Paste->Copy. */
    static void copySelectionByContextMenu(GUITestOpStatus& os);

    // scrolls to the position (in the MSA zero-based coordinates)
    static void scrollToPosition(GUITestOpStatus& os, const QPoint& position);
    static void scrollToBottom(GUITestOpStatus& os);
    static void moveMouseToPosition(GUITestOpStatus& os, const QPoint& globalMaPosition);
    static void clickToPosition(GUITestOpStatus& os, const QPoint& globalMaPosition);

    static void selectSequence(GUITestOpStatus& os, const QString& seqName);
    static void selectSequence(GUITestOpStatus& os, const int row);
    static bool isSequenceSelected(GUITestOpStatus& os, const QString& seqName);
    static void removeSequence(GUITestOpStatus& os, const QString& sequenceName);
    static int getSelectedSequencesNum(GUITestOpStatus& os);
    static bool isSequenceVisible(GUITestOpStatus& os, const QString& seqName);
    static QString getSequenceData(GUITestOpStatus& os, const QString& sequenceName);
    static QString getSequenceData(GUITestOpStatus& os, int rowNumber);

    static bool offsetsVisible(GUITestOpStatus& os);

    static bool hasAminoAlphabet(GUITestOpStatus& os);
    static bool isSequenceHighlighted(GUITestOpStatus& os, const QString& seqName);
    static QString getColor(GUITestOpStatus& os, QPoint p);
    static bool hasPixelWithColor(GUITestOpStatus& os, const QPoint& p, const QColor& color);
    static void checkMsaCellColors(GUITestOpStatus& os, const QPoint& pos, const QString& fgColor, const QString& bgColor);
    static void checkMsaCellColor(GUITestOpStatus& os, const QPoint& pos, const QString& color);
    static bool checkColor(GUITestOpStatus& os, const QPoint& p, const QString& expectedColor);
    static int getRowHeight(GUITestOpStatus& os, int rowNumber);

    static void renameSequence(GUITestOpStatus& os, const QString& seqToRename, const QString& newName, bool useCopyPaste = false);
    static void replaceSymbol(GUITestOpStatus& os, const QPoint& maPoint, char newSymbol);

    static void createColorScheme(GUITestOpStatus& os, const QString& schemeName, const NewColorSchemeCreator::alphabet al);
    static void deleteColorScheme(GUITestOpStatus& os, const QString& schemeName);

    static void checkSelection(GUITestOpStatus& os, const QPoint& start, const QPoint& end, const QString& expected);

    static bool isAlignmentLocked(GUITestOpStatus& os);

    /*
     *expandedBorder: 0 - top, 1 - right, 2 - bottom, 3 - left, 4 - right top, 5 - right bottom, 6 - left bottom, 7 - left top
     */
    static void expandSelectedRegion(GUITestOpStatus& os, const int expandedBorder, const int symbolsToExpand);

    static void zoomIn(GUITestOpStatus& os);
    static void zoomOut(GUITestOpStatus& os);
    static void zoomToMax(GUITestOpStatus& os);
    static void zoomToMin(GUITestOpStatus& os);

    static const QString highlightingColor;
};

}  // namespace U2
