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

#include <U2Core/DNASequence.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include "MsaRowUtils.h"

namespace U2 {

int MsaRowUtils::getRowLength(const QByteArray &seq, const U2MsaRowGapModel &gaps) {
    return seq.length() + getGapsLength(gaps);
}

int MsaRowUtils::getGapsLength(const U2MsaRowGapModel &gaps) {
    int length = 0;
    foreach (const U2MsaGap &elt, gaps) {
        length += elt.gap;
    }
    return length;
}

char MsaRowUtils::charAt(const QByteArray &seq, const U2MsaRowGapModel &gaps, int pos) {
    if (pos < 0 || pos >= getRowLength(seq, gaps)) {
        return U2Msa::GAP_CHAR;
    }

    int gapsLength = 0;
    foreach (const U2MsaGap &gap, gaps) {
        // Current gap is somewhere further in the row
        if (gap.offset > pos) {
            break;
        }
        // Inside the gap
        else if ((pos >= gap.offset) && (pos < gap.offset + gap.gap)) {
            return U2Msa::GAP_CHAR;
        }
        // Go further in the row, calculating the current gaps length
        else {
            gapsLength += gap.gap;
        }
    }

    if (pos >= gapsLength + seq.length()) {
        return U2Msa::GAP_CHAR;
    }

    int index = pos - gapsLength;
    bool indexIsInBounds = (index < seq.length()) && (index >= 0);

    SAFE_POINT(indexIsInBounds,
        QString("Internal error detected in MultipleSequenceAlignmentRow::charAt,"
        " row length is '%1', gapsLength is '%2'!").arg(getRowLength(seq, gaps)).arg(index), U2Msa::GAP_CHAR);
    return seq[index];
}

qint64 MsaRowUtils::getRowLengthWithoutTrailing(const QByteArray &seq, const U2MsaRowGapModel &gaps) {
    int rowLength = getRowLength(seq, gaps);
    int rowLengthWithoutTrailingGap = rowLength;
    if (!gaps.isEmpty()) {
        if (U2Msa::GAP_CHAR == charAt(seq, gaps, rowLength - 1)) {
            U2MsaGap lastGap = gaps.last();
            rowLengthWithoutTrailingGap -= lastGap.gap;
        }
    }
    return rowLengthWithoutTrailingGap;
}

qint64 MsaRowUtils::getRowLengthWithoutTrailing(qint64 dataLength, const U2MsaRowGapModel &gaps) {
    qint64 gappedDataLength = dataLength;
    foreach (const U2MsaGap &gap, gaps) {
        if (gap.offset > gappedDataLength) {
            break;
        }
        gappedDataLength += gap.gap;
    }
    return gappedDataLength;
}

qint64 MsaRowUtils::getUngappedPosition(const U2MsaRowGapModel &gaps, qint64 dataLength, qint64 position, bool allowGapInPos) {
    if (isGap(dataLength, gaps, position) && !allowGapInPos) {
        return -1;
    }

    int gapsLength = 0;
    foreach (const U2MsaGap &gap, gaps) {
        if (gap.offset < position) {
            if (allowGapInPos) {
                gapsLength += (gap.offset + gap.gap < position) ? gap.gap : gap.gap - (gap.offset + gap.gap - position);
            } else {
                gapsLength += gap.gap;
            }
        } else {
            break;
        }
    }

    return position - gapsLength;
}

U2Region MsaRowUtils::getGappedRegion(const U2MsaRowGapModel& gaps, const U2Region& ungappedRegion) {
    U2Region result(ungappedRegion);
    foreach(const U2MsaGap & gap, gaps) {
        if (gap.offset <= result.startPos) { //leading gaps
            result.startPos += gap.gap;
        } else if (gap.offset > result.startPos && gap.offset < result.endPos()) { //inner gaps
            result.length += gap.gap;
        } else { //trailing
            break;
        }
    }
    return result;
}

U2Region MsaRowUtils::getUngappedRegion(const U2MsaRowGapModel& gaps, const U2Region& selection) {
    int shiftStartPos = 0;
    int decreaseLength = 0;
    foreach(const U2MsaGap & gap, gaps) {
        if (gap.endPos() < selection.startPos) {
            shiftStartPos += gap.gap;
        } else if (gap.offset < selection.startPos && gap.offset + gap.gap >= selection.startPos) {
            shiftStartPos = selection.startPos - gap.offset;
            decreaseLength += gap.offset + gap.gap - selection.startPos;
        } else if (gap.offset < selection.endPos() && gap.offset >= selection.startPos) {
            if (gap.endPos() >= selection.endPos()) {
                decreaseLength += selection.endPos() - gap.offset;
            } else {
                decreaseLength += gap.gap;
            }
        } else if (gap.offset <= selection.startPos && gap.offset + gap.gap >= selection.endPos()) {
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

int MsaRowUtils::getCoreStart(const U2MsaRowGapModel &gaps) {
    if (!gaps.isEmpty() && gaps.first().offset == 0) {
        return gaps.first().gap;
    }
    return 0;
}

void MsaRowUtils::insertGaps(U2OpStatus &os, U2MsaRowGapModel &gaps, int rowLengthWithoutTrailing, int position, int count) {
    SAFE_POINT_EXT(0 <= count, os.setError(QString("Internal error: incorrect parameters were passed to MsaRowUtils::insertGaps, "
                                                   "pos '%1', count '%2'").arg(position).arg(count)), );
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
                if (position >= gaps[i].offset) {
                    if (position <= gaps[i].offset + gaps[i].gap) {
                        gaps[i].gap += count;
                    }
                } else {
                    gaps[i].offset += count;
                }
            }
        } else {
            // Insert between chars
            bool found = false;

            int indexGreaterGaps = 0;
            for (int i = 0; i < gaps.count(); ++i) {
                if (position > gaps[i].offset + gaps[i].gap) {
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
                    gaps[i].offset += count;
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

void MsaRowUtils::removeGaps(U2OpStatus &os, U2MsaRowGapModel &gaps, int rowLengthWithoutTrailing, int position, int count) {
    SAFE_POINT_EXT(0 <= position && 0 <= count, os.setError(QString("Internal error: incorrect parameters were passed to MsaRowUtils::removeGaps, "
                                                                    "pos '%1', count '%2'").arg(position).arg(count)), );
    CHECK(position <= rowLengthWithoutTrailing, );

    QList<U2MsaGap> newGapModel;
    int endRegionPos = position + count; // non-inclusive
    foreach (U2MsaGap gap, gaps) {
        qint64 gapEnd = gap.offset + gap.gap;
        if (gapEnd < position) {
            newGapModel << gap;
        } else if (gapEnd <= endRegionPos) {
            if (gap.offset < position) {
                gap.gap = position - gap.offset;
                newGapModel << gap;
            }
            // Otherwise just remove the gap (do not write to the new gap model)
        } else {
            if (gap.offset < position) {
                gap.gap -= count;
                SAFE_POINT(gap.gap >= 0, "Non-positive gap length", );
                newGapModel << gap;
            } else if (gap.offset < endRegionPos) {
                gap.gap = gapEnd - endRegionPos;
                gap.offset = position;
                SAFE_POINT(gap.gap > 0, "Non-positive gap length", );
                SAFE_POINT(gap.offset >= 0, "Negative gap offset", );
                newGapModel << gap;
            } else {
                // Shift the gap
                gap.offset -= count;
                SAFE_POINT(gap.offset >= 0, "Negative gap offset", );
                newGapModel << gap;
            }
        }
    }

    gaps = newGapModel;
}

void MsaRowUtils::addOffsetToGapModel(U2MsaRowGapModel &gapModel, int offset) {
    if (0 == offset) {
        return;
    }

    if (!gapModel.isEmpty()) {
        U2MsaGap &firstGap = gapModel[0];
        if (0 == firstGap.offset) {
            firstGap.gap += offset;
        } else {
            SAFE_POINT(offset >= 0, "Negative gap offset", );
            U2MsaGap beginningGap(0, offset);
            gapModel.insert(0, beginningGap);
        }

        // Shift other gaps
        if (gapModel.count() > 1) {
            for (int i = 1; i < gapModel.count(); ++i) {
                qint64 newOffset = gapModel[i].offset + offset;
                SAFE_POINT(newOffset >= 0, "Negative gap offset", );
                gapModel[i].offset = newOffset;
            }
        }
    } else {
        SAFE_POINT(offset >= 0, "Negative gap offset", );
        U2MsaGap gap(0, offset);
        gapModel.append(gap);
    }
}

void MsaRowUtils::shiftGapModel(U2MsaRowGapModel &gapModel, int shiftSize) {
    CHECK(!gapModel.isEmpty(), );
    CHECK(shiftSize != 0, );
    CHECK(-shiftSize <= gapModel.first().offset, );
    for (int i = 0; i < gapModel.size(); i++) {
        gapModel[i].offset += shiftSize;
    }
}

bool MsaRowUtils::isGap(int dataLength, const U2MsaRowGapModel &gapModel, int position) {
    int gapsLength = 0;
    foreach (const U2MsaGap &gap, gapModel) {
        if (gap.offset <= position && position < gap.offset + gap.gap) {
            return true;
        }
        if (position < gap.offset) {
            return false;
        }
        gapsLength += gap.gap;
    }

    if (dataLength + gapsLength <= position) {
        return true;
    }

    return false;
}

void MsaRowUtils::chopGapModel(U2MsaRowGapModel &gapModel, qint64 maxLength) {
    chopGapModel(gapModel, U2Region(0, maxLength));
}

void MsaRowUtils::chopGapModel(U2MsaRowGapModel &gapModel, const U2Region &boundRegion) {
    // Remove gaps after the region
    while (!gapModel.isEmpty() && gapModel.last().offset >= boundRegion.endPos()) {
        gapModel.removeLast();
    }

    if (!gapModel.isEmpty() && gapModel.last().endPos() > boundRegion.endPos()) {
        gapModel.last().gap = boundRegion.endPos() - gapModel.last().offset;
    }

    // Remove gaps before the region
    qint64 removedGapsLength = 0;
    while (!gapModel.isEmpty() && gapModel.first().endPos() < boundRegion.startPos) {
        removedGapsLength += gapModel.first().gap;
        gapModel.removeFirst();
    }

    if (!gapModel.isEmpty() && gapModel.first().offset < boundRegion.startPos) {
        removedGapsLength += boundRegion.startPos - gapModel.first().offset;
        gapModel.first().gap -= boundRegion.startPos - gapModel.first().offset;
        gapModel.first().offset  = boundRegion.startPos;
    }

    shiftGapModel(gapModel, -removedGapsLength);
}

QByteArray MsaRowUtils::joinCharsAndGaps(const DNASequence &sequence, const U2MsaRowGapModel &gapModel, int rowLength, bool keepLeadingGaps, bool keepTrailingGaps) {
    QByteArray bytes = sequence.constSequence();
    int beginningOffset = 0;

    if (gapModel.isEmpty()) {
        return bytes;
    }

    for (int i = 0; i < gapModel.size(); ++i) {
        QByteArray gapsBytes;
        if (!keepLeadingGaps && (0 == gapModel[i].offset)) {
            beginningOffset = gapModel[i].gap;
            continue;
        }

        gapsBytes.fill(U2Msa::GAP_CHAR, gapModel[i].gap);
        bytes.insert(gapModel[i].offset - beginningOffset, gapsBytes);
    }

    if (keepTrailingGaps && (bytes.size() < rowLength)) {
        QByteArray gapsBytes;
        gapsBytes.fill(U2Msa::GAP_CHAR, rowLength - bytes.size());
        bytes.append(gapsBytes);
    }

    return bytes;
}

namespace {

U2MsaGap getNextGap(QListIterator<U2MsaGap> &mainGapModelIterator, QListIterator<U2MsaGap> &additionalGapModelIterator, qint64 &gapsFromMainModelLength) {
    SAFE_POINT(mainGapModelIterator.hasNext() || additionalGapModelIterator.hasNext(), "Out of gap models boundaries", U2MsaGap());

    if (!mainGapModelIterator.hasNext()) {
        U2MsaGap gap = additionalGapModelIterator.next();
        gap.offset += gapsFromMainModelLength;
        return gap;
    }

    if (!additionalGapModelIterator.hasNext()) {
        const U2MsaGap mainGap = mainGapModelIterator.next();
        gapsFromMainModelLength += mainGap.gap;
        return mainGap;
    }

    const U2MsaGap mainGap = mainGapModelIterator.peekNext();
    const U2MsaGap additionalGap = additionalGapModelIterator.peekNext();
    const U2MsaGap intersection = mainGap.intersect(additionalGap);

    if (intersection.isValid()) {
        const U2MsaGap unitedGap = U2MsaGap(qMin(mainGap.offset, additionalGap.offset + gapsFromMainModelLength), mainGap.gap + additionalGap.gap);
        gapsFromMainModelLength += mainGap.gap;
        mainGapModelIterator.next();
        additionalGapModelIterator.next();
        return unitedGap;
    }

    if (mainGap.offset <= additionalGap.offset + gapsFromMainModelLength) {
        gapsFromMainModelLength += mainGap.gap;
        mainGapModelIterator.next();
        return mainGap;
    } else {
        U2MsaGap shiftedAdditionalGap = additionalGapModelIterator.next();
        shiftedAdditionalGap.offset += gapsFromMainModelLength;
        return shiftedAdditionalGap;
    }
}

}

U2MsaRowGapModel MsaRowUtils::insertGapModel(const U2MsaRowGapModel &mainGapModel, const U2MsaRowGapModel &additionalGapModel) {
    U2MsaRowGapModel mergedGapModel;
    QListIterator<U2MsaGap> mainGapModelIterator(mainGapModel);
    QListIterator<U2MsaGap> additionalGapModelIterator(additionalGapModel);
    qint64 gapsFromMainModelLength = 0;
    while (mainGapModelIterator.hasNext() || additionalGapModelIterator.hasNext()) {
        mergedGapModel << getNextGap(mainGapModelIterator, additionalGapModelIterator, gapsFromMainModelLength);
    }
    mergeConsecutiveGaps(mergedGapModel);
    return mergedGapModel;
}

void MsaRowUtils::mergeConsecutiveGaps(U2MsaRowGapModel &gapModel) {
    CHECK(!gapModel.isEmpty(), );
    QList<U2MsaGap> newGapModel;

    newGapModel << gapModel[0];
    int indexInNewGapModel = 0;
    for (int i = 1; i < gapModel.count(); ++i) {
        const qint64 previousGapEnd = newGapModel[indexInNewGapModel].offset + newGapModel[indexInNewGapModel].gap - 1;
        const qint64 currectGapStart = gapModel[i].offset;
        SAFE_POINT(currectGapStart > previousGapEnd, "Incorrect gap model during merging consecutive gaps", );
        if (currectGapStart == previousGapEnd + 1) {
            // Merge gaps
            const qint64 newGapLength = newGapModel[indexInNewGapModel].gap + gapModel[i].gap;
            SAFE_POINT(newGapLength > 0, "Non-positive gap length", )
            newGapModel[indexInNewGapModel].gap = newGapLength;
        } else {
            // Add the gap to the list
            newGapModel << gapModel[i];
            indexInNewGapModel++;
        }
    }
    gapModel = newGapModel;
}

namespace {

bool hasIntersection(const U2MsaGap &gap1, const U2MsaGap &gap2) {
    return gap1.offset < gap2.endPos() && gap2.offset < gap1.endPos();
}

// Moves the iterator that points to the less gap
// returns true, if step was successfully done
// returns false, if the end is already reached
bool stepForward(QMutableListIterator<U2MsaGap> &firstIterator, QMutableListIterator<U2MsaGap> &secondIterator) {
    CHECK(firstIterator.hasNext(), false);
    CHECK(secondIterator.hasNext(), false);
    const U2MsaGap &firstGap = firstIterator.peekNext();
    const U2MsaGap &secondGap = secondIterator.peekNext();
    if (firstGap.offset <= secondGap.offset) {
        firstIterator.next();
    } else {
        secondIterator.next();
    }
    return true;
}

// forward iterators to the state, when the next values have an intersection
// returns true if there an intersection was found, otherwise return false
bool forwardToIntersection(QMutableListIterator<U2MsaGap> &firstIterator, QMutableListIterator<U2MsaGap> &secondIterator) {
    bool endReached = false;
    while (!hasIntersection(firstIterator.peekNext(), secondIterator.peekNext())) {
        endReached = !stepForward(firstIterator, secondIterator);
        if (endReached) {
            break;
        }
    }
    return !endReached;
}

QPair<U2MsaGap, U2MsaGap> subGap(const U2MsaGap &subFrom, const U2MsaGap &subWhat) {
    QPair<U2MsaGap, U2MsaGap> result;
    if (subFrom.offset < subWhat.offset) {
        result.first = U2MsaGap(subFrom.offset, subWhat.offset - subFrom.offset);
    }
    if (subFrom.endPos() > subWhat.endPos()) {
        result.second = U2MsaGap(subWhat.endPos(), subFrom.endPos() - subWhat.endPos());
    }
    return result;
}

void removeCommonPart(QMutableListIterator<U2MsaGap> &iterator, const U2MsaGap &commonPart){
    const QPair<U2MsaGap, U2MsaGap> gapDifference = subGap(iterator.peekNext(), commonPart);
    if (gapDifference.second.isValid()) {
        iterator.peekNext() = gapDifference.second;
    }
    if (gapDifference.first.isValid()) {
        iterator.insert(gapDifference.first);
    }
    if (!gapDifference.first.isValid() && !gapDifference.second.isValid()) {
        iterator.next();
        iterator.remove();
    }
}

// extracts a common part from the next values, a difference between values is written back to the models
U2MsaGap extractCommonPart(QMutableListIterator<U2MsaGap> &firstIterator, QMutableListIterator<U2MsaGap> &secondIterator) {
    SAFE_POINT(firstIterator.hasNext() && secondIterator.hasNext(), "Out of gap model boundaries", U2MsaGap());
    U2MsaGap &firstGap = firstIterator.peekNext();
    U2MsaGap &secondGap = secondIterator.peekNext();

    const U2MsaGap commonPart = firstGap.intersect(secondGap);
    SAFE_POINT(commonPart.isValid(), "Gaps don't have an intersection", U2MsaGap());
    removeCommonPart(firstIterator, commonPart);
    removeCommonPart(secondIterator, commonPart);

    return commonPart;
}

}

void MsaRowUtils::getGapModelsDifference(const U2MsaRowGapModel &firstGapModel, const U2MsaRowGapModel &secondGapModel, U2MsaRowGapModel &commonPart, U2MsaRowGapModel &firstDifference, U2MsaRowGapModel &secondDifference) {
    commonPart.clear();
    firstDifference = firstGapModel;
    QMutableListIterator<U2MsaGap> firstIterator(firstDifference);
    secondDifference = secondGapModel;
    QMutableListIterator<U2MsaGap> secondIterator(secondDifference);

    while (firstIterator.hasNext() && secondIterator.hasNext()) {
        const bool intersectionFound = forwardToIntersection(firstIterator, secondIterator);
        if (!intersectionFound) {
            break;
        }
        commonPart << extractCommonPart(firstIterator, secondIterator);
    }
    mergeConsecutiveGaps(commonPart);
}

namespace {

void insertGap(U2MsaRowGapModel &gapModel, const U2MsaGap &gap) {
    for (int i = 0; i < gapModel.size(); i++) {
        if (gapModel[i].endPos() < gap.offset) {
            // search the proper location
            continue;
        } else if (gapModel[i].offset > gap.endPos()) {
            // no intersection, just insert
            gapModel.insert(i, gap);
        } else {
            // there is an intersection
            gapModel[i].offset = qMin(gapModel[i].offset, gap.offset);
            gapModel[i].setEndPos(qMax(gapModel[i].endPos(), gap.endPos()));
            int gapsToRemove = 0;
            for (int j = i + 1; j < gapModel.size(); j++) {
                if (gapModel[j].endPos() <= gapModel[i].endPos()) {
                    // this gap is fully covered by a new gap, just remove
                    gapsToRemove++;
                } else if (gapModel[j].offset <= gapModel[i].endPos()) {
                    // this gap is partially covered by a new gap, enlarge the new gap and remove
                    gapModel[i].setEndPos(qMax(gapModel[i].endPos(), gapModel[j].endPos()));
                    gapsToRemove++;
                } else {
                    break;
                }
            }

            gapModel.erase(gapModel.begin() + i + 1, gapModel.begin() + i + gapsToRemove + 1);
        }
    }
}

void subtitudeGap(QMutableListIterator<U2MsaGap> &minuendIterator, QMutableListIterator<U2MsaGap> &subtrahendIterator) {
    const QPair<U2MsaGap, U2MsaGap> substitutionResult = subGap(minuendIterator.next(), subtrahendIterator.peekNext());
    minuendIterator.remove();

    if (substitutionResult.second.isValid()) {
        minuendIterator.insert(substitutionResult.second);
        minuendIterator.previous();
    }

    if (substitutionResult.first.isValid()) {
        minuendIterator.insert(substitutionResult.first);
        minuendIterator.previous();
    }
}

}

U2MsaRowGapModel MsaRowUtils::mergeGapModels(const U2MsaListGapModel &gapModels) {
    U2MsaRowGapModel mergedGapModel;
    foreach (const U2MsaRowGapModel &gapModel, gapModels) {
        foreach (const U2MsaGap &gap, gapModel) {
            insertGap(mergedGapModel, gap);
        }
    }
    return mergedGapModel;
}

U2MsaRowGapModel MsaRowUtils::subtitudeGapModel(const U2MsaRowGapModel &minuendGapModel, const U2MsaRowGapModel &subtrahendGapModel) {
    U2MsaRowGapModel result = minuendGapModel;
    U2MsaRowGapModel subtrahendGapModelCopy = subtrahendGapModel;
    QMutableListIterator<U2MsaGap> minuendIterator(result);
    QMutableListIterator<U2MsaGap> subtrahendIterator(subtrahendGapModelCopy);

    while (minuendIterator.hasNext() && subtrahendIterator.hasNext()) {
        const bool intersectionFound = forwardToIntersection(minuendIterator, subtrahendIterator);
        if (!intersectionFound) {
            break;
        }
        subtitudeGap(minuendIterator, subtrahendIterator);
    }

    return minuendGapModel;
}

U2MsaRowGapModel MsaRowUtils::reverseGapModel(const U2MsaRowGapModel &gapModel, qint64 rowLengthWithoutTrailing) {
    U2MsaRowGapModel reversedGapModel = gapModel;

    foreach (const U2MsaGap &gap, gapModel) {
        if (rowLengthWithoutTrailing - gap.endPos() < 0) {
            Q_ASSERT(false);     // original model has gaps out of range or trailing gaps
            continue;
        }
        reversedGapModel.prepend(U2MsaGap(rowLengthWithoutTrailing - gap.offset, gap.gap));
    }

    if (hasLeadingGaps(gapModel)) {
        reversedGapModel.removeLast();
        reversedGapModel.prepend(gapModel.first());
    }

    return reversedGapModel;
}

bool MsaRowUtils::hasLeadingGaps(const U2MsaRowGapModel &gapModel) {
    return !gapModel.isEmpty() && gapModel.first().offset == 0;
}

void MsaRowUtils::removeTrailingGapsFromModel(qint64 length, U2MsaRowGapModel &gapModel) {
    for (int i = 0; i < gapModel.size(); i++) {
        const U2MsaGap &gap = gapModel.at(i);
        if (gap.offset >= length) {
            while (gapModel.size() > i) {
                gapModel.removeLast();
            }
        } else {
            length += gap.gap;
        }
    }
}

}   // namespace U2
