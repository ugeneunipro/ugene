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

#include "MsaRowUtils.h"

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

int MsaRowUtils::getRowLength(const QByteArray& seq, const QVector<U2MsaGap>& gaps) {
    return seq.length() + getGapsLength(gaps);
}

int MsaRowUtils::getGapsLength(const QVector<U2MsaGap>& gaps) {
    int length = 0;
    foreach (const U2MsaGap& elt, gaps) {
        length += elt.length;
    }
    return length;
}

char MsaRowUtils::charAt(const QByteArray& seq, const QVector<U2MsaGap>& gaps, int pos) {
    if (pos < 0 || pos >= getRowLength(seq, gaps)) {
        return U2Msa::GAP_CHAR;
    }

    int gapsLength = 0;
    foreach (const U2MsaGap& gap, gaps) {
        // Current gap is somewhere further in the row
        if (gap.startPos > pos) {
            break;
        }
        // Inside the gap
        else if ((pos >= gap.startPos) && (pos < gap.startPos + gap.length)) {
            return U2Msa::GAP_CHAR;
        }
        // Go further in the row, calculating the current gaps length
        else {
            gapsLength += gap.length;
        }
    }

    if (pos >= gapsLength + seq.length()) {
        return U2Msa::GAP_CHAR;
    }

    int index = pos - gapsLength;
    bool indexIsInBounds = (index < seq.length()) && (index >= 0);

    SAFE_POINT(indexIsInBounds,
               QString("Internal error detected in MultipleSequenceAlignmentRow::charAt,"
                       " row length is '%1', gapsLength is '%2'!")
                   .arg(getRowLength(seq, gaps))
                   .arg(index),
               U2Msa::GAP_CHAR);
    return seq[index];
}

qint64 MsaRowUtils::getRowLengthWithoutTrailing(const QByteArray& seq, const QVector<U2MsaGap>& gaps) {
    int rowLength = getRowLength(seq, gaps);
    int rowLengthWithoutTrailingGap = rowLength;
    if (!gaps.isEmpty()) {
        if (U2Msa::GAP_CHAR == charAt(seq, gaps, rowLength - 1)) {
            U2MsaGap lastGap = gaps.last();
            rowLengthWithoutTrailingGap -= lastGap.length;
        }
    }
    return rowLengthWithoutTrailingGap;
}

qint64 MsaRowUtils::getRowLengthWithoutTrailing(qint64 dataLength, const QVector<U2MsaGap>& gaps) {
    qint64 gappedDataLength = dataLength;
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos > gappedDataLength) {
            break;
        }
        gappedDataLength += gap.length;
    }
    return gappedDataLength;
}

qint64 MsaRowUtils::getUngappedPosition(const QVector<U2MsaGap>& gaps, qint64 dataLength, qint64 position, bool allowGapInPos) {
    if (isGap(dataLength, gaps, position) && !allowGapInPos) {
        return -1;
    }

    int gapsLength = 0;
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos < position) {
            if (allowGapInPos) {
                gapsLength += (gap.startPos + gap.length < position) ? gap.length : gap.length - (gap.startPos + gap.length - position);
            } else {
                gapsLength += gap.length;
            }
        } else {
            break;
        }
    }

    return position - gapsLength;
}

U2Region MsaRowUtils::getGappedRegion(const QVector<U2MsaGap>& gaps, const U2Region& ungappedRegion) {
    U2Region result(ungappedRegion);
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos <= result.startPos) {  // leading gaps
            result.startPos += gap.length;
        } else if (gap.startPos > result.startPos && gap.startPos < result.endPos()) {  // inner gaps
            result.length += gap.length;
        } else {  // trailing
            break;
        }
    }
    return result;
}

U2Region MsaRowUtils::getUngappedRegion(const QVector<U2MsaGap>& gaps, const U2Region& selection) {
    int shiftStartPos = 0;
    int decreaseLength = 0;
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.endPos() < selection.startPos) {
            shiftStartPos += gap.length;
        } else if (gap.startPos < selection.startPos && gap.startPos + gap.length >= selection.startPos) {
            shiftStartPos = selection.startPos - gap.startPos;
            decreaseLength += gap.startPos + gap.length - selection.startPos;
        } else if (gap.startPos < selection.endPos() && gap.startPos >= selection.startPos) {
            if (gap.endPos() >= selection.endPos()) {
                decreaseLength += selection.endPos() - gap.startPos;
            } else {
                decreaseLength += gap.length;
            }
        } else if (gap.startPos <= selection.startPos && gap.startPos + gap.length >= selection.endPos()) {
            return U2Region(0, 0);
        } else {
            break;
        }
    }
    U2Region result(selection.startPos - shiftStartPos, selection.length - decreaseLength);
    SAFE_POINT(result.startPos >= 0, "Error with calculation ungapped region", U2Region(0, 0));
    SAFE_POINT(result.length > 0, "Error with calculation ungapped region", U2Region(0, 0));
    return result;
}

int MsaRowUtils::getCoreStart(const QVector<U2MsaGap>& gaps) {
    if (!gaps.isEmpty() && gaps.first().startPos == 0) {
        return gaps.first().length;
    }
    return 0;
}

void MsaRowUtils::insertGaps(U2OpStatus& os, QVector<U2MsaGap>& gaps, int rowLengthWithoutTrailing, int position, int count) {
    SAFE_POINT_EXT(0 <= count, os.setError(QString("Internal error: incorrect parameters were passed to MsaRowUtils::insertGaps, "
                                                   "pos '%1', count '%2'")
                                               .arg(position)
                                               .arg(count)), );
    CHECK(0 <= position && position < rowLengthWithoutTrailing, );

    if (0 == position) {
        addOffsetToGapModel(gaps, count);
    } else {
        const int dataLength = rowLengthWithoutTrailing - getGapsLength(gaps);
        if (isGap(dataLength, gaps, position) || isGap(dataLength, gaps, position - 1)) {
            // A gap is near
            // Find the gaps and append 'count' gaps to it
            // Shift all gaps that further in the row
            for (int i = 0; i < gaps.count(); ++i) {
                if (position >= gaps[i].startPos) {
                    if (position <= gaps[i].startPos + gaps[i].length) {
                        gaps[i].length += count;
                    }
                } else {
                    gaps[i].startPos += count;
                }
            }
        } else {
            // Insert between chars
            bool found = false;

            int indexGreaterGaps;
            for (int i = 0; i < gaps.count(); ++i) {
                if (position > gaps[i].startPos + gaps[i].length) {
                    continue;
                } else {
                    found = true;
                    U2MsaGap newGap(position, count);
                    gaps.insert(i, newGap);
                    indexGreaterGaps = i;
                    break;
                }
            }

            // If found somewhere between existent gaps
            if (found) {
                // Shift further gaps
                for (int i = indexGreaterGaps + 1; i < gaps.count(); ++i) {
                    gaps[i].startPos += count;
                }
            } else {
                // This is the last gap
                U2MsaGap newGap(position, count);
                gaps.append(newGap);
                return;
            }
        }
    }
}

void MsaRowUtils::removeGaps(U2OpStatus& os, QVector<U2MsaGap>& gaps, int rowLengthWithoutTrailing, int position, int count) {
    SAFE_POINT_EXT(0 <= position && 0 <= count, os.setError(QString("Internal error: incorrect parameters were passed to MsaRowUtils::removeGaps, "
                                                                    "pos '%1', count '%2'")
                                                                .arg(position)
                                                                .arg(count)), );
    CHECK(position <= rowLengthWithoutTrailing, );

    QVector<U2MsaGap> newGapModel;
    int endRegionPos = position + count;  // non-inclusive
    foreach (U2MsaGap gap, gaps) {
        qint64 gapEnd = gap.startPos + gap.length;
        if (gapEnd < position) {
            newGapModel << gap;
        } else if (gapEnd <= endRegionPos) {
            if (gap.startPos < position) {
                gap.length = position - gap.startPos;
                newGapModel << gap;
            }
            // Otherwise just remove the gap (do not write to the new gap model)
        } else {
            if (gap.startPos < position) {
                gap.length -= count;
                SAFE_POINT(gap.length >= 0, "Non-positive gap length", );
                newGapModel << gap;
            } else if (gap.startPos < endRegionPos) {
                gap.length = gapEnd - endRegionPos;
                gap.startPos = position;
                SAFE_POINT(gap.length > 0, "Non-positive gap length", );
                SAFE_POINT(gap.startPos >= 0, "Negative gap offset", );
                newGapModel << gap;
            } else {
                // Shift the gap
                gap.startPos -= count;
                SAFE_POINT(gap.startPos >= 0, "Negative gap offset", );
                newGapModel << gap;
            }
        }
    }

    gaps = newGapModel;
}

void MsaRowUtils::addOffsetToGapModel(QVector<U2MsaGap>& gapModel, int offset) {
    if (0 == offset) {
        return;
    }

    if (!gapModel.isEmpty()) {
        U2MsaGap& firstGap = gapModel[0];
        if (0 == firstGap.startPos) {
            firstGap.length += offset;
        } else {
            SAFE_POINT(offset >= 0, "Negative gap offset", );
            U2MsaGap beginningGap(0, offset);
            gapModel.insert(0, beginningGap);
        }

        // Shift other gaps
        if (gapModel.count() > 1) {
            for (int i = 1; i < gapModel.count(); ++i) {
                qint64 newOffset = gapModel[i].startPos + offset;
                SAFE_POINT(newOffset >= 0, "Negative gap offset", );
                gapModel[i].startPos = newOffset;
            }
        }
    } else {
        SAFE_POINT(offset >= 0, "Negative gap offset", );
        U2MsaGap gap(0, offset);
        gapModel.append(gap);
    }
}

void MsaRowUtils::shiftGapModel(QVector<U2MsaGap>& gapModel, int shiftSize) {
    CHECK(!gapModel.isEmpty(), );
    CHECK(shiftSize != 0, );
    CHECK(-shiftSize <= gapModel.first().startPos, );
    for (int i = 0; i < gapModel.size(); i++) {
        gapModel[i].startPos += shiftSize;
    }
}

bool MsaRowUtils::isGap(int dataLength, const QVector<U2MsaGap>& gapModel, int position) {
    int gapsLength = 0;
    foreach (const U2MsaGap& gap, gapModel) {
        if (gap.startPos <= position && position < gap.startPos + gap.length) {
            return true;
        }
        if (position < gap.startPos) {
            return false;
        }
        gapsLength += gap.length;
    }

    if (dataLength + gapsLength > position) {
        return false;
    }

    return true;
}

bool MsaRowUtils::isLeadingOrTrailingGap(int dataLength, const QVector<U2MsaGap>& gapModel, int position) {
    if (gapModel.isEmpty()) {
        return false;
    }
    if (gapModel[0].startPos == 0 && position < gapModel[0].endPos()) {
        return true;  // leading gap.
    }
    int totalGapsLen = 0;
    for (const U2MsaGap& gap : qAsConst(gapModel)) {
        totalGapsLen += gap.length;
        if (position < gap.startPos) {
            return false;  // somewhere in the middle.
        }
    }
    return position >= dataLength + totalGapsLen;  // trailing gap.
}

void MsaRowUtils::chopGapModel(QVector<U2MsaGap>& gapModel, qint64 maxLength) {
    chopGapModel(gapModel, U2Region(0, maxLength));
}

void MsaRowUtils::chopGapModel(QVector<U2MsaGap>& gapModel, const U2Region& boundRegion) {
    // Remove gaps after the region
    while (!gapModel.isEmpty() && gapModel.last().startPos >= boundRegion.endPos()) {
        gapModel.removeLast();
    }

    if (!gapModel.isEmpty() && gapModel.last().endPos() > boundRegion.endPos()) {
        gapModel.last().length = boundRegion.endPos() - gapModel.last().startPos;
    }

    // Remove gaps before the region
    qint64 removedGapsLength = 0;
    while (!gapModel.isEmpty() && gapModel.first().endPos() < boundRegion.startPos) {
        removedGapsLength += gapModel.first().length;
        gapModel.removeFirst();
    }

    if (!gapModel.isEmpty() && gapModel.first().startPos < boundRegion.startPos) {
        removedGapsLength += boundRegion.startPos - gapModel.first().startPos;
        gapModel.first().length -= boundRegion.startPos - gapModel.first().startPos;
        gapModel.first().startPos = boundRegion.startPos;
    }

    shiftGapModel(gapModel, -removedGapsLength);
}

void MsaRowUtils::mergeConsecutiveGaps(QVector<U2MsaGap>& gapModel) {
    CHECK(!gapModel.isEmpty(), );
    QVector<U2MsaGap> newGapModel;

    newGapModel << gapModel[0];
    int indexInNewGapModel = 0;
    for (int i = 1; i < gapModel.count(); ++i) {
        const qint64 previousGapEnd = newGapModel[indexInNewGapModel].startPos + newGapModel[indexInNewGapModel].length - 1;
        const qint64 currectGapStart = gapModel[i].startPos;
        SAFE_POINT(currectGapStart > previousGapEnd, "Incorrect gap model during merging consecutive gaps", );
        if (currectGapStart == previousGapEnd + 1) {
            // Merge gaps
            const qint64 newGapLength = newGapModel[indexInNewGapModel].length + gapModel[i].length;
            SAFE_POINT(newGapLength > 0, "Non-positive gap length", )
            newGapModel[indexInNewGapModel].length = newGapLength;
        } else {
            // Add the gap to the list
            newGapModel << gapModel[i];
            indexInNewGapModel++;
        }
    }
    gapModel = newGapModel;
}

QVector<U2MsaGap> MsaRowUtils::reverseGapModel(const QVector<U2MsaGap>& gapModel, qint64 rowLengthWithoutTrailing) {
    QVector<U2MsaGap> reversedGapModel = gapModel;

    foreach (const U2MsaGap& gap, gapModel) {
        if (rowLengthWithoutTrailing - gap.endPos() < 0) {
            Q_ASSERT(false);  // original model has gaps out of range or trailing gaps
            continue;
        }
        reversedGapModel.prepend(U2MsaGap(rowLengthWithoutTrailing - gap.startPos, gap.length));
    }

    if (hasLeadingGaps(gapModel)) {
        reversedGapModel.removeLast();
        reversedGapModel.prepend(gapModel.first());
    }

    return reversedGapModel;
}

bool MsaRowUtils::hasLeadingGaps(const QVector<U2MsaGap>& gapModel) {
    return !gapModel.isEmpty() && gapModel.first().startPos == 0;
}

void MsaRowUtils::removeTrailingGapsFromModel(qint64 length, QVector<U2MsaGap>& gapModel) {
    for (int i = 0; i < gapModel.size(); i++) {
        const U2MsaGap& gap = gapModel.at(i);
        if (gap.startPos >= length) {
            while (gapModel.size() > i) {
                gapModel.removeLast();
            }
        } else {
            length += gap.length;
        }
    }
}

QByteArray MsaRowUtils::getGappedSubsequence(const U2Region& region, const QByteArray& sequence, const QVector<U2MsaGap>& gaps) {
    CHECK(region.length > 0, {});
    // Check if we have no gaps effect at all in the requested region.
    // Return a sequence.mid() in this case: this will prevent any heap allocation for the result sequence.
    bool canUseSubsequence = (gaps.isEmpty() || gaps[0].startPos >= region.endPos()) && (region.startPos >= 0 && region.endPos() <= sequence.length());
    if (canUseSubsequence) {
        return sequence.mid(region.startPos, region.length);
    }
    CHECK(!sequence.isEmpty(), QByteArray(region.length, U2Msa::GAP_CHAR));
    const char* coreSequence = sequence.constData();
    int coreLength = sequence.length();
    QByteArray result;
    result.reserve(region.length);
    int gapIndex = 0;  // Current gap index.
    int corePos = 0;  // Current offset in core sequence.
    int pos = 0;  // Current gapped position in the row.
    int regionEnd = (int)region.endPos();  // Last (exclusive) global gapped position to include into the result.
    if (region.startPos < 0) {
        result.append(-region.startPos, U2Msa::GAP_CHAR);
    }
    // Iterate interleaved gap & core sequence regions.
    while (gapIndex <= gaps.length() && pos < regionEnd && corePos < coreLength) {
        const U2MsaGap* gap = gapIndex == gaps.length() ? nullptr : &gaps[gapIndex];
        bool isValidGapModel = gapIndex == 0 || gap == nullptr || gap->startPos > gaps[gapIndex - 1].endPos();
        SAFE_POINT(isValidGapModel, "Invalid gap model", {});
        if (gap == nullptr || gap->startPos > pos) {
            // Processing core sequence part. At this point no gaps left or the next gap starts after 'pos'.
            int corePartLength = gap == nullptr ? coreLength - corePos : gap->startPos - pos;
            SAFE_POINT(corePos + corePartLength <= coreLength, "Invalid position in core sequence!", {});
            if (region.startPos < pos + corePartLength) {  // Add core sub-sequence to the result.
                int nChars;
                int corePartOffset;
                if (region.startPos <= pos) {  // Add a prefix or whole core sequence region.
                    nChars = qMin(corePartLength, regionEnd - pos);
                    corePartOffset = 0;
                } else if (regionEnd > pos + corePartLength) {  // Add suffix of the core sequence region.
                    nChars = pos + corePartLength - region.startPos;
                    corePartOffset = corePartLength - nChars;
                } else {  // Add middle part of the core sequence region.
                    SAFE_POINT(region.startPos > pos && regionEnd <= pos + corePartLength, "Invalid middle region coordinates", {};)
                    nChars = region.length;
                    corePartOffset = region.startPos - pos;
                }
                SAFE_POINT(nChars > 0 && corePartOffset >= 0 && corePartOffset + nChars <= corePartLength && result.length() + nChars <= region.length,
                           "Invalid sub-region in the core region",
                           {});
                result.append(coreSequence + corePos + corePartOffset, nChars);
            }
            corePos += corePartLength;
            pos += corePartLength;
            SAFE_POINT(corePos <= coreLength, "Core sequence overflow!", {});
        } else {
            // Processing gap region.
            int gapPartLength = (int)gap->length;
            if (region.startPos < pos + gapPartLength) {
                int nGaps;
                if (region.startPos <= pos) {  // Add a prefix or whole gap region.
                    nGaps = qMin(gapPartLength, regionEnd - pos);
                } else if (regionEnd > pos + gapPartLength) {  // Add suffix of the gap region
                    nGaps = pos + gapPartLength - region.startPos;
                } else {  // Add middle part of the gap region.
                    nGaps = region.length;
                }
                SAFE_POINT(nGaps <= gapPartLength && result.length() + nGaps <= region.length, "Invalid gap region", {});
                result.append(nGaps, U2Msa::GAP_CHAR);
            }
            pos += gap->length;
            gapIndex++;
        }
        SAFE_POINT(pos < region.startPos ? result.isEmpty() : result.length() <= pos - region.startPos, "Invalid result region after processing gap.", {});
    }
    SAFE_POINT(result.length() <= region.length, "Invalid result length after processing gaps", {});

    // Add trailing gaps to finalize the region.
    if (result.length() < region.length) {
        int nGaps = region.length - result.length();
        result.append(nGaps, U2Msa::GAP_CHAR);
    }
    SAFE_POINT(result.length() == region.length, "Invalid result length in getGappedSubsequence", {});
    return result;
}

}  // namespace U2
