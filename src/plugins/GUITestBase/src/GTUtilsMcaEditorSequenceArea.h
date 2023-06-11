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

#include <U2View/McaEditorSequenceArea.h>

namespace U2 {
using namespace HI;

class GTUtilsMcaEditorSequenceArea {
public:
    static McaEditorSequenceArea* getSequenceArea();
    static QStringList getVisibleNames();
    static int getRowHeight(int rowNumber);
    static void clickToPosition(const QPoint& globalMaPosition);
    static void scrollToPosition(const QPoint& position);
    static void scrollToBase(int position);
    static void clickCollapseTriangle(QString seqName, bool showChromatogram);
    static bool isChromatogramShown(QString seqName);
    static QStringList getNameList();
    static void callContextMenu(const QPoint& innerCoords = QPoint());
    static void moveTo(const QPoint& p);
    static QPoint convertCoordinates(const QPoint p);
    static QString getReferenceReg(int num, int length);
    static QString getSelectedReferenceReg();
    static void moveTheBorderBetweenAlignmentAndRead(int shift);
    static void dragAndDrop(const QPoint p);
    static U2Region getSelectedRowsNum();
    static QStringList getSelectedRowsNames();
    static QRect getSelectedRect();

    /**
     * Returns sequence area local rectangle for the base position at the given row.
     * The returned rectangle includes both character area & chromatogram column area below the character.
     */
    static QRect getPositionRect(int rowIndex, int referenceBaseIndex);

    static void clickToReferencePositionCenter(qint64 position, const QPoint& clickPointAdjustment = QPoint(0, 0));
    static void moveCursorToReferencePositionCenter(qint64 position, const QPoint& movePointAdjustment = QPoint(0, 0));
    /**
     *0 - ViewMode
     *1 - ReplaceCharMode
     *2 - InsertCharMode
     *Return value of this function is not enum "MaMode" to avoid encapsulation violation
     */
    static short getCharacterModificationMode();
    /**
     *Valid just if one character in sequence area selected
     */
    static char getSelectedReadChar();
    static char getReadCharByPos(const QPoint p);
    static qint64 getRowLength(const int numRow);
    static qint64 getReferenceLength();
    static qint64 getReferenceLengthWithGaps();
    static U2Region getReferenceSelection();
    static QString getSelectedConsensusReg();
    static QString getConsensusStringByRegion(const U2Region reg);
};

}  // namespace U2
