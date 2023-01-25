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

#include "MultipleSequenceAlignmentWalker.h"

#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

class RowWalker {
public:
    RowWalker(const MultipleSequenceAlignmentRow& row, char gapChar)
        : row(row), gaps(row->getGaps()), seqPos(0), gapChar(gapChar) {
    }

    QByteArray nextData(int startPos, int length, U2OpStatus& os) {
        QByteArray result = row->getSequence().constSequence().mid(seqPos, length);

        if (gaps.isEmpty()) {  // add trailing gaps if it is possible
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

            result.insert(gap.startPos - startPos, gapsBytes(inRegion.length));
            gapsInserted += inRegion.length;

            if (outRegion.length > 0) {
                gaps.prepend(outRegion);
                break;
            }
        }
        CHECK_OP(os, "");

        if (result.length() < length) {  // add trailing gaps if it is possible
            gapsInserted += length - result.length();
            result += gapsBytes(length - result.length());
        }

        seqPos += length - gapsInserted;
        return result.left(length);
    }

private:
    bool hasGapsInRegion(int startPos, int length, U2OpStatus& os) const {
        CHECK(!gaps.isEmpty(), false);
        const U2MsaGap& gap = gaps.first();

        SAFE_POINT_EXT(gap.startPos >= startPos, os.setError(QString("Unexpected gap start")), false);

        if (gap.startPos >= startPos + length) {
            return false;
        }
        return true;
    }

    static void splitGap(int startPos, int length, const U2MsaGap& gap, U2MsaGap& inRegion, U2MsaGap& outRegion, U2OpStatus& os) {
        SAFE_POINT_EXT(gap.startPos >= startPos, os.setError(QString("Unexpected gap start (too small)")), );
        SAFE_POINT_EXT(gap.startPos < startPos + length, os.setError(QString("Unexpected gap start (too big)")), );

        int endPos = startPos + length - 1;
        if (gap.startPos + gap.length <= endPos) {
            inRegion = gap;
        } else {
            int inRegionLength = endPos - gap.startPos + 1;
            int outRegionLength = gap.length - inRegionLength;
            inRegion = U2MsaGap(gap.startPos, inRegionLength);
            outRegion = U2MsaGap(endPos + 1, outRegionLength);
        }

        SAFE_POINT_EXT((startPos + length >= inRegion.startPos + inRegion.length) && (inRegion.length + outRegion.length == gap.length),
                       os.setError(L10N::internalError() + MultipleSequenceAlignmentObject::tr(" Incorrect gap splitting")), );
    }

    QByteArray gapsBytes(int length) const {
        return QByteArray(length, gapChar);
    }

private:
    const MultipleSequenceAlignmentRow row;
    QVector<U2MsaGap> gaps;
    int seqPos;
    const char gapChar;
};

/************************************************************************/
/* MultipleSequenceAlignmentWalker */
/************************************************************************/
MultipleSequenceAlignmentWalker::MultipleSequenceAlignmentWalker(const MultipleSequenceAlignment& msa, char gapChar)
    : msa(msa), currentOffset(0) {
    for (int i = 0; i < msa->getRowCount(); i++) {
        rowWalkerList << new RowWalker(msa->getMsaRow(i), gapChar);
    }
}

MultipleSequenceAlignmentWalker::~MultipleSequenceAlignmentWalker() {
    qDeleteAll(rowWalkerList);
}

bool MultipleSequenceAlignmentWalker::isEnded() const {
    return currentOffset >= msa->getLength();
}

QList<QByteArray> MultipleSequenceAlignmentWalker::nextData(int length, U2OpStatus& os) {
    QList<QByteArray> result;
    SAFE_POINT_EXT(!isEnded(), os.setError(L10N::internalError() + QString(" Alignment walker is ended")), result);
    SAFE_POINT_EXT(msa->getRowCount() == rowWalkerList.size(), os.setError(L10N::internalError() + QString(" Alignment changed")), result);

    int charsRemain = msa->getLength() - currentOffset;
    int chunkSize = (charsRemain < length) ? charsRemain : length;

    for (int i = 0; i < msa->getRowCount(); i++) {
        QByteArray rowBytes = rowWalkerList[i]->nextData(currentOffset, chunkSize, os);
        CHECK_OP(os, QList<QByteArray>());
        result << rowBytes;
    }
    currentOffset += chunkSize;
    return result;
}

}  // namespace U2
