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

#ifndef _U2_MSAROWUTILS_H_
#define _U2_MSAROWUTILS_H_

#include <U2Core/U2Msa.h>
#include <U2Core/global.h>

namespace U2 {

class DNASequence;
class U2OpStatus;
class U2Region;

class U2CORE_EXPORT MsaRowUtils {
public:
    static int getRowLength(const QByteArray &seq, const U2MsaRowGapModel &gaps);
    static int getGapsLength(const U2MsaRowGapModel &gaps);
    static char charAt(const QByteArray &seq, const U2MsaRowGapModel &gaps, int pos);
    static qint64 getRowLengthWithoutTrailing(const QByteArray &seq, const U2MsaRowGapModel &gaps);
    static qint64 getRowLengthWithoutTrailing(qint64 dataLength, const U2MsaRowGapModel &gaps);
    /**
     * The method maps `pos` in MSA coordinates to a character position in 'seq', i.e. gaps aren't taken into account.
     * If false == 'allowGapInPos' and the gap symbol is located in 'pos' then the method returns -1.
     * Otherwise if true == 'allowGapInPos' and the gap symbol is located in 'pos' then the method returns
     * the position of a non-gap character left-most to the 'pos'.
     */
    static qint64 getUngappedPosition(const U2MsaRowGapModel &gaps, qint64 dataLength, qint64 position, bool allowGapInPos = false);
    //Only inner gaps, no leading and trailing
    static U2Region getGappedRegion(const U2MsaRowGapModel &gaps, const U2Region &ungapped);
    static U2Region getUngappedRegion(const U2MsaRowGapModel &gaps, const U2Region &selection);
    static int getCoreStart(const U2MsaRowGapModel &gaps);

    static void insertGaps(U2OpStatus &os, U2MsaRowGapModel &gaps, int rowLengthWithoutTrailing, int position, int count);
    static void removeGaps(U2OpStatus &os, U2MsaRowGapModel &gaps, int rowLengthWithoutTrailing, int position, int count);

    /**
     * Add "offset" of gaps to the beginning of the row
     * Warning: it is not verified that the row sequence is not empty.
     */
    static void addOffsetToGapModel(U2MsaRowGapModel &gapModel, int offset);
    static void shiftGapModel(U2MsaRowGapModel &gapModel, int shiftSize);
    static bool isGap(int dataLength, const U2MsaRowGapModel &gapModel, int position);
    static bool isLeadingOrTrailingGap(int dataLength, const U2MsaRowGapModel &gapModel, int position);
    static void chopGapModel(U2MsaRowGapModel &gapModel, qint64 maxLength);
    static void chopGapModel(U2MsaRowGapModel &gapModel, const U2Region &boundRegion);    // gaps will be shifted
    static QByteArray joinCharsAndGaps(const DNASequence &sequence, const U2MsaRowGapModel &gapModel, int rowLength, bool keepLeadingGaps, bool keepTrailingGaps);
    static U2MsaRowGapModel insertGapModel(const U2MsaRowGapModel &firstGapModel, const U2MsaRowGapModel &secondGapModel);
    static void mergeConsecutiveGaps(U2MsaRowGapModel &gapModel);
    static void getGapModelsDifference(const U2MsaRowGapModel &firstGapModel,
                                       const U2MsaRowGapModel &secondGapModel,
                                       U2MsaRowGapModel &commonPart,
                                       U2MsaRowGapModel &firstDifference,
                                       U2MsaRowGapModel &secondDifference);
    static U2MsaRowGapModel mergeGapModels(const U2MsaListGapModel &gapModels);
    static U2MsaRowGapModel subtitudeGapModel(const U2MsaRowGapModel &minuendGapModel, const U2MsaRowGapModel &subtrahendGapModel);
    static U2MsaRowGapModel reverseGapModel(const U2MsaRowGapModel &gapModel, qint64 rowLengthWithoutTrailing);    // this method reverses only core gaps. Leading and trailing gaps are not involved to calculations
    static bool hasLeadingGaps(const U2MsaRowGapModel &gapModel);
    static void removeTrailingGapsFromModel(qint64 length, U2MsaRowGapModel &gapModel);
};

}    // namespace U2

#endif    // _U2_MSAROWUTILS_H_
