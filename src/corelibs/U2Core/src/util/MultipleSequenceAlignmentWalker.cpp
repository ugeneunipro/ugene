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

#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "MultipleSequenceAlignmentWalker.h"

namespace U2 {

class RowWalker {
public:
    RowWalker(const MultipleSequenceAlignmentRow &row, char gapChar)
    : row(row), gaps(row->getGapModel()), seqPos(0), gapChar(gapChar)
    {

    }

    QByteArray nextData(int startPos, int length, U2OpStatus &os) {
        QByteArray result = row->getSequence().constSequence().mid(seqPos, length);

        if (gaps.isEmpty()) { // add trailing gaps if it is possible
            seqPos += result.length();
            return result + gapsBytes(length - result.length());
        }

        int gapsInserted = 0;
        while (hasGapsInRegion(startPos, length, os)) {
            U2MsaGap gap = gaps.takeFirst();
            U2MsaGap inRegion;
            U2MsaGap outRegion;
            splitGap(startPos, length, gap, inRegion, outRegion, os);
            CHECK_OP(os, "");

            result.insert(gap.offset - startPos, gapsBytes(inRegion.gap));
            gapsInserted += inRegion.gap;

            if (outRegion.gap > 0) {
                gaps.prepend(outRegion);
                break;
            }
        }
        CHECK_OP(os, "");

        if (result.length() < length) { // add trailing gaps if it is possible
            gapsInserted += length - result.length();
            result += gapsBytes(length - result.length());
        }

        seqPos += length - gapsInserted;
        return result.left(length);
    }

private:
    bool hasGapsInRegion(int startPos, int length, U2OpStatus &os) const {
        CHECK(!gaps.isEmpty(), false);
        const U2MsaGap &gap = gaps.first();

        SAFE_POINT_EXT(gap.offset >= startPos, os.setError(L10N::badArgument(MultipleSequenceAlignmentObject::tr("Unexpected gap start"))), false);

        if (gap.offset >= startPos + length) {
            return false;
        }
        return true;
    }

    static void splitGap(int startPos, int length, const U2MsaGap &gap, U2MsaGap &inRegion, U2MsaGap &outRegion, U2OpStatus &os) {
        SAFE_POINT_EXT(gap.offset >= startPos, os.setError(L10N::badArgument(MultipleSequenceAlignmentObject::tr("Unexpected gap start (too small)"))), );
        SAFE_POINT_EXT(gap.offset < startPos + length, os.setError(L10N::badArgument(MultipleSequenceAlignmentObject::tr("Unexpected gap start (too big)"))), );

        int endPos = startPos + length - 1;
        if (gap.offset + gap.gap <= endPos) {
            inRegion = gap;
        } else {
            int inRegionLength = endPos - gap.offset + 1;
            int outRegionLength = gap.gap - inRegionLength;
            inRegion = U2MsaGap(gap.offset, inRegionLength);
            outRegion = U2MsaGap(endPos + 1, outRegionLength);
        }

        SAFE_POINT_EXT((startPos + length >= inRegion.offset + inRegion.gap)
                    && (inRegion.gap + outRegion.gap == gap.gap),
                    os.setError(L10N::internalError() + MultipleSequenceAlignmentObject::tr(" Incorrect gap splitting")), );
    }

    QByteArray gapsBytes(int length) const {
        return QByteArray(length, gapChar);
    }

private:
    const MultipleSequenceAlignmentRow row;
    QList<U2MsaGap> gaps;
    int seqPos;
    const char gapChar;
};

/************************************************************************/
/* MultipleSequenceAlignmentWalker */
/************************************************************************/
MultipleSequenceAlignmentWalker::MultipleSequenceAlignmentWalker(const MultipleSequenceAlignment &msa, char gapChar)
: msa(msa), currentOffset(0)
{
    for (int i=0; i<msa->getNumRows(); i++) {
        rowWalkerList << new RowWalker(msa->getMsaRow(i), gapChar);
    }
}

MultipleSequenceAlignmentWalker::~MultipleSequenceAlignmentWalker() {
    qDeleteAll(rowWalkerList);
}

bool MultipleSequenceAlignmentWalker::isEnded() const {
    return currentOffset >= msa->getLength();
}

QList<QByteArray> MultipleSequenceAlignmentWalker::nextData(int length, U2OpStatus &os) {
    QList<QByteArray> result;
    SAFE_POINT_EXT(!isEnded(), os.setError(L10N::internalError() + MultipleSequenceAlignmentObject::tr(" Alignment walker is ended")), result);
    SAFE_POINT_EXT(msa->getNumRows() == rowWalkerList.size(), os.setError(L10N::internalError() + MultipleSequenceAlignmentObject::tr(" Alignment changed")), result);

    int charsRemain = msa->getLength() - currentOffset;
    int chunkSize = (charsRemain < length) ? charsRemain : length;

    for (int i=0; i<msa->getNumRows(); i++) {
        QByteArray rowBytes = rowWalkerList[i]->nextData(currentOffset, chunkSize, os);
        CHECK_OP(os, QList<QByteArray>());
        result << rowBytes;
    }
    currentOffset += chunkSize;
    return result;
}

} // U2
