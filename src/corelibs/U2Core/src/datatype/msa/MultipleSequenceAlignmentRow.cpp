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

#include "MultipleSequenceAlignmentRow.h"

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow()
    : MultipleAlignmentRow(new MultipleSequenceAlignmentRowData()) {
}

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow(const MultipleAlignmentRow& maRow)
    : MultipleAlignmentRow(maRow) {
    SAFE_POINT(maRowData.dynamicCast<MultipleSequenceAlignmentRowData>() != nullptr, "Can't cast MultipleAlignmentRow to MultipleSequenceAlignmentRow", );
}

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow(MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRow(new MultipleSequenceAlignmentRowData(msaData)) {
}

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow(MultipleSequenceAlignmentRowData* msaRowData)
    : MultipleAlignmentRow(msaRowData) {
}

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRow(new MultipleSequenceAlignmentRowData(rowInDb, sequence, gaps, msaData)) {
}

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow(const U2MsaRow& rowInDb, const QString& rowName, const QByteArray& rawData, MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRow(new MultipleSequenceAlignmentRowData(rowInDb, rowName, rawData, msaData)) {
}

MultipleSequenceAlignmentRow::MultipleSequenceAlignmentRow(const MultipleSequenceAlignmentRow& row, MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRow(new MultipleSequenceAlignmentRowData(row, msaData)) {
}

MultipleSequenceAlignmentRowData* MultipleSequenceAlignmentRow::data() const {
    return getMsaRowData().data();
}

MultipleSequenceAlignmentRowData& MultipleSequenceAlignmentRow::operator*() {
    return *getMsaRowData();
}

const MultipleSequenceAlignmentRowData& MultipleSequenceAlignmentRow::operator*() const {
    return *getMsaRowData();
}

MultipleSequenceAlignmentRowData* MultipleSequenceAlignmentRow::operator->() {
    return getMsaRowData().data();
}

const MultipleSequenceAlignmentRowData* MultipleSequenceAlignmentRow::operator->() const {
    return getMsaRowData().data();
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentRow::clone() const {
    return getMsaRowData()->getExplicitCopy();
}

QSharedPointer<MultipleSequenceAlignmentRowData> MultipleSequenceAlignmentRow::getMsaRowData() const {
    return maRowData.dynamicCast<MultipleSequenceAlignmentRowData>();
}

MultipleSequenceAlignmentRowData::MultipleSequenceAlignmentRowData(MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MSA),
      alignment(msaData) {
    removeTrailingGaps();
}

MultipleSequenceAlignmentRowData::MultipleSequenceAlignmentRowData(const U2MsaRow& rowInDb,
                                                                   const DNASequence& sequence,
                                                                   const QVector<U2MsaGap>& gaps,
                                                                   MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MSA, sequence, gaps),
      alignment(msaData) {
    SAFE_POINT(alignment != nullptr, "Parent MultipleSequenceAlignmentData are NULL", );
    initialRowInDb = rowInDb;
    removeTrailingGaps();
}

MultipleSequenceAlignmentRowData::MultipleSequenceAlignmentRowData(const U2MsaRow& rowInDb, const QString& rowName, const QByteArray& rawData, MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MSA),
      alignment(msaData) {
    initialRowInDb = rowInDb;
    QByteArray sequenceData;
    QVector<U2MsaGap> gapModel;
    MaDbiUtils::splitBytesToCharsAndGaps(rawData, sequenceData, gapModel);
    sequence = DNASequence(rowName, sequenceData);
    setGapModel(gapModel);
}

MultipleSequenceAlignmentRowData::MultipleSequenceAlignmentRowData(const MultipleSequenceAlignmentRow& row, MultipleSequenceAlignmentData* msaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MSA, row->sequence, row->gaps),
      alignment(msaData) {
    initialRowInDb = row->initialRowInDb;
    SAFE_POINT(alignment != nullptr, "Parent MultipleSequenceAlignmentData are NULL", );
}

void MultipleSequenceAlignmentRowData::insertGaps(int pos, int count, U2OpStatus& os) {
    invalidateGappedCache();
    MsaRowUtils::insertGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
}

void MultipleSequenceAlignmentRowData::removeChars(int pos, int count, U2OpStatus& os) {
    if (pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MultipleSequenceAlignmentRowData::removeChars, "
                              "pos '%1', count '%2'")
                          .arg(pos)
                          .arg(count));
        os.setError("Can't remove chars from a row");
        return;
    }
    if (pos >= getRowLengthWithoutTrailing()) {
        return;
    }

    invalidateGappedCache();

    if (pos < getRowLengthWithoutTrailing()) {
        int startPosInSeq = -1;
        int endPosInSeq = -1;
        getStartAndEndSequencePositions(pos, count, startPosInSeq, endPosInSeq);

        // Remove inside a gap
        if ((startPosInSeq < endPosInSeq) && (-1 != startPosInSeq) && (-1 != endPosInSeq)) {
            DNASequenceUtils::removeChars(sequence, startPosInSeq, endPosInSeq, os);
            CHECK_OP(os, );
        }
    }

    // Remove gaps from the gaps model
    removeGapsFromGapModel(os, pos, count);

    removeTrailingGaps();
    mergeConsecutiveGaps();
}

char MultipleSequenceAlignmentRowData::charAt(qint64 position) const {
    return getCharFromCache((int)position);
}

bool MultipleSequenceAlignmentRowData::isGap(qint64 pos) const {
    return MsaRowUtils::isGap(sequence.length(), gaps, pos);
}

bool MultipleSequenceAlignmentRowData::isLeadingOrTrailingGap(qint64 pos) const {
    return MsaRowUtils::isLeadingOrTrailingGap(sequence.length(), gaps, pos);
}

qint64 MultipleSequenceAlignmentRowData::getBaseCount(qint64 before) const {
    const int rowLength = MsaRowUtils::getRowLength(sequence.seq, gaps);
    const int trimmedRowPos = before < rowLength ? before : rowLength;
    return MsaRowUtils::getUngappedPosition(gaps, sequence.length(), trimmedRowPos, true);
}

bool MultipleSequenceAlignmentRowData::isDefault() const {
    static const MultipleSequenceAlignmentRowData defaultRow;
    return isEqual(defaultRow);
}

bool MultipleSequenceAlignmentRowData::isEqual(const MultipleAlignmentRowData& other) const {
    CHECK(other.type == MultipleAlignmentDataType::MSA, false);
    auto msaRow = dynamic_cast<const MultipleSequenceAlignmentRowData*>(&other);
    SAFE_POINT(msaRow != nullptr, "Not an MSA row!", false);
    return isEqual(*msaRow);
}

bool MultipleSequenceAlignmentRowData::isEqual(const MultipleSequenceAlignmentRowData& other) const {
    CHECK(this != &other, true);
    CHECK(getName() == other.getName(), false);
    CHECK(gaps == other.getGaps(), false);

    CHECK(sequence.alphabet == other.sequence.alphabet, false);
    CHECK(sequence.seq == sequence.seq, false);
    return true;
}

void MultipleSequenceAlignmentRowData::crop(U2OpStatus& os, qint64 startPosition, qint64 count) {
    if (startPosition < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MultipleSequenceAlignmentRowData::crop, "
                              "startPos '%1', length '%2', row length '%3'")
                          .arg(startPosition)
                          .arg(count)
                          .arg(getRowLength()));
        os.setError("Can't crop a row!");
        return;
    }
    invalidateGappedCache();

    int initialRowLength = getRowLength();
    int initialSeqLength = getUngappedLength();

    if (startPosition >= getRowLengthWithoutTrailing()) {
        // Clear the row content
        DNASequenceUtils::makeEmpty(sequence);
    } else {
        int startPosInSeq = -1;
        int endPosInSeq = -1;
        getStartAndEndSequencePositions(startPosition, count, startPosInSeq, endPosInSeq);

        // Remove inside a gap
        if ((startPosInSeq <= endPosInSeq) && (-1 != startPosInSeq) && (-1 != endPosInSeq)) {
            if (endPosInSeq < initialSeqLength) {
                DNASequenceUtils::removeChars(sequence, endPosInSeq, getUngappedLength(), os);
                CHECK_OP(os, );
            }

            if (startPosInSeq > 0) {
                DNASequenceUtils::removeChars(sequence, 0, startPosInSeq, os);
                CHECK_OP(os, );
            }
        }
    }

    if (startPosition + count < initialRowLength) {
        removeGapsFromGapModel(os, startPosition + count, initialRowLength - startPosition - count);
    }

    if (startPosition > 0) {
        removeGapsFromGapModel(os, 0, startPosition);
    }
    removeTrailingGaps();
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentRowData::mid(int pos, int count, U2OpStatus& os) const {
    MultipleSequenceAlignmentRow row = getExplicitCopy();
    row->crop(os, pos, count);
    return row;
}

void MultipleSequenceAlignmentRowData::toUpperCase() {
    DNASequenceUtils::toUpperCase(sequence);
}

void MultipleSequenceAlignmentRowData::replaceChars(char origChar, char resultChar, U2OpStatus& os) {
    if (U2Msa::GAP_CHAR == origChar) {
        coreLog.trace("The original char can't be a gap in MultipleSequenceAlignmentRowData::replaceChars");
        os.setError("Failed to replace chars in an alignment row");
        return;
    }

    invalidateGappedCache();

    if (resultChar == U2Msa::GAP_CHAR) {
        // Get indexes of all 'origChar' characters in the row sequence
        QList<int> gapsIndexes;
        for (int i = 0; i < getRowLength(); i++) {
            if (origChar == charAt(i)) {
                gapsIndexes.append(i);
            }
        }

        if (gapsIndexes.isEmpty()) {
            return;  // There is nothing to replace
        }

        // Remove all 'origChar' characters from the row sequence
        sequence.seq.replace(origChar, "");

        // Re-calculate the gaps model
        QVector<U2MsaGap> newGapsModel = gaps;
        for (int i = 0; i < gapsIndexes.size(); ++i) {
            int index = gapsIndexes[i];
            U2MsaGap gap(index, 1);
            newGapsModel.append(gap);
        }
        std::sort(newGapsModel.begin(), newGapsModel.end(), U2MsaGap::lessThan);

        // Replace the gaps model with the new one
        gaps = newGapsModel;
        mergeConsecutiveGaps();
    } else {
        // Just replace all occurrences of 'origChar' by 'resultChar'
        sequence.seq.replace(origChar, resultChar);
    }
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentRowData::getExplicitCopy() const {
    return MultipleSequenceAlignmentRow(new MultipleSequenceAlignmentRowData(*this));
}

void MultipleSequenceAlignmentRowData::getStartAndEndSequencePositions(int pos, int count, int& startPosInSeq, int& endPosInSeq) {
    int rowLengthWithoutTrailingGap = getRowLengthWithoutTrailing();
    SAFE_POINT(pos < rowLengthWithoutTrailingGap,
               QString("Incorrect position '%1' in MultipleSequenceAlignmentRowData::getStartAndEndSequencePosition, "
                       "row length without trailing gaps is '%2'")
                   .arg(pos)
                   .arg(rowLengthWithoutTrailingGap), );

    // Remove chars from the sequence
    // Calculate start position in the sequence
    if (U2Msa::GAP_CHAR == charAt(pos)) {
        int i = 1;
        while (U2Msa::GAP_CHAR == charAt(pos + i)) {
            if (getRowLength() == pos + i) {
                break;
            }
            i++;
        }
        startPosInSeq = getUngappedPosition(pos + i);
    } else {
        startPosInSeq = getUngappedPosition(pos);
    }

    // Calculate end position in the sequence
    int endRegionPos = pos + count;  // non-inclusive

    if (endRegionPos > rowLengthWithoutTrailingGap) {
        endRegionPos = rowLengthWithoutTrailingGap;
    }

    if (endRegionPos == rowLengthWithoutTrailingGap) {
        endPosInSeq = getUngappedLength();
    } else {
        if (U2Msa::GAP_CHAR == charAt(endRegionPos)) {
            int i = 1;
            while (U2Msa::GAP_CHAR == charAt(endRegionPos + i)) {
                if (getRowLength() == endRegionPos + i) {
                    break;
                }
                i++;
            }
            endPosInSeq = getUngappedPosition(endRegionPos + i);
        } else {
            endPosInSeq = getUngappedPosition(endRegionPos);
        }
    }
}

void MultipleSequenceAlignmentRowData::removeGapsFromGapModel(U2OpStatus& os, int pos, int count) {
    invalidateGappedCache();
    MsaRowUtils::removeGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
}

void MultipleSequenceAlignmentRowData::setParentAlignment(const MultipleSequenceAlignment& msa) {
    setParentAlignment(msa.data());
}

void MultipleSequenceAlignmentRowData::setParentAlignment(MultipleSequenceAlignmentData* msaData) {
    invalidateGappedCache();
    alignment = msaData;
}

MultipleAlignmentData* MultipleSequenceAlignmentRowData::getMultipleAlignmentData() const {
    return alignment;
}

int MultipleSequenceAlignmentRowData::getGapsLength() const {
    return MsaRowUtils::getGapsLength(gaps);
}

/**
 * Size of the gapped cache: 200 symbols.
 * Optimized for chars per screen and makes getChar() ~200x faster thanMsaRowUtils::charAt for sequential access.
 */
static constexpr int GAPPED_CACHE_SIZE = 200;

char MultipleSequenceAlignmentRowData::getCharFromCache(int gappedPosition) const {
    if (gappedPosition >= gappedCacheOffset && gappedPosition < gappedCacheOffset + gappedSequenceCache.size()) {
        return gappedSequenceCache[gappedPosition - gappedCacheOffset];
    }
    invalidateGappedCache();
    int newGappedCacheSize = GAPPED_CACHE_SIZE;
    int newGappedCacheOffset = qMax(0, gappedPosition - (newGappedCacheSize / 10));  // Cache one both sides. Prefer forward iteration.

    // Optimize cache size for sequences < GAPPED_CACHE_SIZE.
    int rowLength = getRowLength();
    if (newGappedCacheOffset + newGappedCacheSize > rowLength) {
        newGappedCacheOffset = rowLength - newGappedCacheSize;
        if (newGappedCacheOffset < 0) {
            newGappedCacheOffset = 0;
            newGappedCacheSize = rowLength;
        }
    }
    gappedSequenceCache = MsaRowUtils::getGappedSubsequence({newGappedCacheOffset, newGappedCacheSize}, sequence.constSequence(), gaps);
    CHECK(!gappedSequenceCache.isEmpty(), MsaRowUtils::charAt(sequence.seq, gaps, gappedPosition));
    SAFE_POINT(gappedSequenceCache.size() == newGappedCacheSize, "Invalid gapped cache size!", '?');

    gappedCacheOffset = newGappedCacheOffset;
    int indexInCache = gappedPosition - gappedCacheOffset;
    SAFE_POINT(indexInCache >= 0 && indexInCache < gappedSequenceCache.length(), "Invalid gapped cache index", '?');
    return gappedSequenceCache[gappedPosition - gappedCacheOffset];
}

}  // namespace U2
