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

#ifndef _U2_MSA_ROW_UTILS_H_
#define _U2_MSA_ROW_UTILS_H_

#include <U2Core/U2Msa.h>
#include <U2Core/global.h>

namespace U2 {

class DNASequence;
class U2OpStatus;
class U2Region;

class U2CORE_EXPORT MsaRowUtils {
public:
    static int getRowLength(const QByteArray &seq, const QList<U2MsaGap> &gaps);
    static int getGapsLength(const QList<U2MsaGap> &gaps);
    static char charAt(const QByteArray &seq, const QList<U2MsaGap> &gaps, int pos);
    static qint64 getRowLengthWithoutTrailing(const QByteArray &seq, const QList<U2MsaGap> &gaps);
    static qint64 getRowLengthWithoutTrailing(qint64 dataLength, const QList<U2MsaGap> &gaps);
    /**
     * The method maps `pos` in MSA coordinates to a character position in 'seq', i.e. gaps aren't taken into account.
     * If false == 'allowGapInPos' and the gap symbol is located in 'pos' then the method returns -1.
     * Otherwise if true == 'allowGapInPos' and the gap symbol is located in 'pos' then the method returns
     * the position of a non-gap character left-most to the 'pos'.
     */
    static qint64 getUngappedPosition(const QList<U2MsaGap> &gaps, qint64 dataLength, qint64 position, bool allowGapInPos = false);
    // Only inner gaps, no leading and trailing
    static U2Region getGappedRegion(const QList<U2MsaGap> &gaps, const U2Region &ungapped);
    static U2Region getUngappedRegion(const QList<U2MsaGap> &gaps, const U2Region &selection);
    static int getCoreStart(const QList<U2MsaGap> &gaps);

    static void insertGaps(U2OpStatus &os, QList<U2MsaGap> &gaps, int rowLengthWithoutTrailing, int position, int count);
    static void removeGaps(U2OpStatus &os, QList<U2MsaGap> &gaps, int rowLengthWithoutTrailing, int position, int count);

    /**
     * Add "offset" of gaps to the beginning of the row
     * Warning: it is not verified that the row sequence is not empty.
     */
    static void addOffsetToGapModel(QList<U2MsaGap> &gapModel, int offset);
    static void shiftGapModel(QList<U2MsaGap> &gapModel, int shiftSize);
    static bool isGap(int dataLength, const QList<U2MsaGap> &gapModel, int position);
    static bool isLeadingOrTrailingGap(int dataLength, const QList<U2MsaGap> &gapModel, int position);
    static void chopGapModel(QList<U2MsaGap> &gapModel, qint64 maxLength);
    static void chopGapModel(QList<U2MsaGap> &gapModel, const U2Region &boundRegion);  // gaps will be shifted
    static void mergeConsecutiveGaps(QList<U2MsaGap> &gapModel);
    static QList<U2MsaGap> reverseGapModel(const QList<U2MsaGap> &gapModel, qint64 rowLengthWithoutTrailing);  // this method reverses only core gaps. Leading and trailing gaps are not involved to calculations
    static bool hasLeadingGaps(const QList<U2MsaGap> &gapModel);
    static void removeTrailingGapsFromModel(qint64 length, QList<U2MsaGap> &gapModel);
};

}  // namespace U2

#endif  // _U2_MSA_ROW_UTILS_H_
