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

#include "MultipleAlignmentRow.h"

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleAlignmentRowInfo.h>
#include <U2Core/U2SafePoints.h>

#include "MultipleAlignment.h"

namespace U2 {

MultipleAlignmentRow::MultipleAlignmentRow(MultipleAlignmentData* _maRowData)
    : maRowData(new MultipleAlignmentRowData(_maRowData)) {
}

MultipleAlignmentRow::MultipleAlignmentRow(MultipleAlignmentRowData* _maRowData)
    : maRowData(_maRowData) {
}

MultipleAlignmentRow::MultipleAlignmentRow(const U2MsaRow& rowInDb,
                                           const DNAChromatogram& chromatogram,
                                           const DNASequence& sequence,
                                           const QVector<U2MsaGap>& gaps,
                                           MultipleAlignmentData* maData)
    : maRowData(new MultipleAlignmentRowData(rowInDb, chromatogram, sequence, gaps, maData)) {
}

MultipleAlignmentRow::MultipleAlignmentRow(const U2MsaRow& rowInDb,
                                           const DNASequence& sequence,
                                           const QVector<U2MsaGap>& gaps,
                                           MultipleAlignmentData* maData)
    : maRowData(new MultipleAlignmentRowData(rowInDb, {}, sequence, gaps, maData)) {
}

MultipleAlignmentRow::MultipleAlignmentRow(const U2MsaRow& rowInDb,
                                           const QString& rowName,
                                           const DNAChromatogram& chromatogram,
                                           const QByteArray& rawData,
                                           MultipleAlignmentData* maData)
    : maRowData(new MultipleAlignmentRowData(rowInDb, rowName, chromatogram, rawData, maData)) {
}

MultipleAlignmentRow::MultipleAlignmentRow(const MultipleAlignmentRow& row, MultipleAlignmentData* mcaData)
    : maRowData(new MultipleAlignmentRowData(row, mcaData)) {
}

MultipleAlignmentRow MultipleAlignmentRow::clone() const {
    return maRowData->getExplicitCopy();
}

MultipleAlignmentRowData* MultipleAlignmentRow::data() const {
    return maRowData.data();
}

MultipleAlignmentRowData& MultipleAlignmentRow::operator*() {
    return *maRowData;
}

const MultipleAlignmentRowData& MultipleAlignmentRow::operator*() const {
    return *maRowData;
}

MultipleAlignmentRowData* MultipleAlignmentRow::operator->() {
    return maRowData.data();
}

const MultipleAlignmentRowData* MultipleAlignmentRow::operator->() const {
    return maRowData.data();
}

MultipleAlignmentRowData::MultipleAlignmentRowData(MultipleAlignmentData* _alignment)
    : alignment(_alignment) {
}

MultipleAlignmentRowData::MultipleAlignmentRowData(const U2MsaRow& _rowInDb,
                                                   const DNAChromatogram& _chromatogram,
                                                   const DNASequence& _sequence,
                                                   const QVector<U2MsaGap>& _gaps,
                                                   MultipleAlignmentData* _alignment)
    : sequence(_sequence),
      gaps(_gaps),
      chromatogram(_chromatogram),
      initialRowInDb(_rowInDb),
      alignment(_alignment) {
    SAFE_POINT(alignment != nullptr, "Parent MultipleChromatogramAlignmentData are NULL", );
    removeTrailingGaps();
}

MultipleAlignmentRowData::MultipleAlignmentRowData(const U2MsaRow& _rowInDb,
                                                   const QString& _rowName,
                                                   const DNAChromatogram& _chromatogram,
                                                   const QByteArray& rawData,
                                                   MultipleAlignmentData* _alignment)
    : chromatogram(_chromatogram),
      initialRowInDb(_rowInDb),
      alignment(_alignment) {
    QByteArray sequenceData;
    QVector<U2MsaGap> gapModel;
    MaDbiUtils::splitBytesToCharsAndGaps(rawData, sequenceData, gapModel);
    sequence = DNASequence(_rowName, sequenceData);
    setGapModel(gapModel);
}

MultipleAlignmentRowData::MultipleAlignmentRowData(const MultipleAlignmentRow& _row, MultipleAlignmentData* _alignment)
    : sequence(_row->sequence),
      gaps(_row->gaps),
      chromatogram(_row->chromatogram),
      initialRowInDb(_row->initialRowInDb),
      additionalInfo(_row->additionalInfo),
      alignment(_alignment) {
    SAFE_POINT_NN(alignment, );
}

MultipleAlignmentRowData::MultipleAlignmentRowData(const DNASequence& sequence, const QVector<U2MsaGap>& gaps)
    : sequence(sequence), gaps(gaps) {
}

int MultipleAlignmentRowData::getUngappedPosition(int pos) const {
    return (int)MsaRowUtils::getUngappedPosition(gaps, sequence.length(), pos);
}

DNASequence MultipleAlignmentRowData::getUngappedSequence() const {
    return sequence;
}

U2Region MultipleAlignmentRowData::getGapped(const U2Region& region) {
    return MsaRowUtils::getGappedRegion(gaps, region);
}

bool MultipleAlignmentRowData::operator==(const MultipleAlignmentRowData& other) const {
    return isEqual(other);
}

bool MultipleAlignmentRowData::operator!=(const MultipleAlignmentRowData& other) const {
    return !isEqual(other);
}

bool MultipleAlignmentRowData::isTrailingOrLeadingGap(qint64 position) const {
    CHECK(isGap(position), false);
    return position < getCoreStart() || position > getCoreEnd() - 1;
}

U2Region MultipleAlignmentRowData::getCoreRegion() const {
    return {getCoreStart(), getCoreLength()};
}

U2Region MultipleAlignmentRowData::getUngappedRegion(const U2Region& gappedRegion) const {
    if (gappedRegion == U2Region(0, 0)) {
        return gappedRegion;
    }
    U2Region noTrailingGapsRegion(gappedRegion);

    if (noTrailingGapsRegion.endPos() > getRowLengthWithoutTrailing()) {
        noTrailingGapsRegion.length = getRowLengthWithoutTrailing() - noTrailingGapsRegion.startPos;
    }
    return MsaRowUtils::getUngappedRegion(gaps, noTrailingGapsRegion);
}

/* Compares sequences of 2 rows ignoring gaps. */
bool MultipleAlignmentRowData::isEqualIgnoreGaps(const MultipleAlignmentRowData* row1, const MultipleAlignmentRowData* row2) {
    SAFE_POINT_NN(row1, false);
    SAFE_POINT_NN(row2, false);
    if (row1 == row2) {
        return true;
    }
    if (row1->getUngappedLength() != row2->getUngappedLength()) {
        return false;
    }
    return row1->getUngappedSequence().seq == row2->getUngappedSequence().seq;
}

bool MultipleAlignmentRowData::isEqualCore(const MultipleAlignmentRowData& other) const {
    CHECK(sequence.seq == other.sequence.seq, false);
    CHECK(sequence.length() > 0, true);

    if (!chromatogram->isEmpty() || !other.chromatogram->isEmpty()) {
        CHECK(ChromatogramUtils::checkAllFieldsEqual(chromatogram, other.chromatogram), false);
    }

    QVector<U2MsaGap> thisGaps = gaps;
    if (!thisGaps.isEmpty() && charAt(0) == U2Msa::GAP_CHAR) {
        thisGaps.removeFirst();
    }

    QVector<U2MsaGap> otherGaps = other.getGaps();
    if (!otherGaps.isEmpty() && other.charAt(0) == U2Msa::GAP_CHAR) {
        otherGaps.removeFirst();
    }

    return thisGaps == otherGaps;
}

QByteArray MultipleAlignmentRowData::getSequenceWithGaps(bool keepLeadingGaps, bool keepTrailingGaps) const {
    QByteArray bytes = sequence.constSequence();
    int beginningOffset = 0;

    if (gaps.isEmpty()) {
        return bytes;
    }

    for (const auto& gap : qAsConst(gaps)) {
        QByteArray gapsBytes;
        if (!keepLeadingGaps && (0 == gap.startPos)) {
            beginningOffset = gap.length;
            continue;
        }

        gapsBytes.fill(U2Msa::GAP_CHAR, gap.length);
        bytes.insert(gap.startPos - beginningOffset, gapsBytes);
    }
    SAFE_POINT(alignment != nullptr, "Parent MAlignment is NULL", QByteArray());
    if (keepTrailingGaps && bytes.size() < alignment->getLength()) {
        QByteArray gapsBytes;
        gapsBytes.fill(U2Msa::GAP_CHAR, alignment->getLength() - bytes.size());
        bytes.append(gapsBytes);
    }

    return bytes;
}

const DNAChromatogram& MultipleAlignmentRowData::getChromatogram() const {
    return chromatogram;
}

QString MultipleAlignmentRowData::getName() const {
    return sequence.getName();
}

void MultipleAlignmentRowData::setName(const QString& name) {
    sequence.setName(name);
}

qint64 MultipleAlignmentRowData::getRowId() const {
    return initialRowInDb.rowId;
}

void MultipleAlignmentRowData::setRowId(qint64 rowId) {
    initialRowInDb.rowId = rowId;
}

void MultipleAlignmentRowData::setSequenceId(const U2DataId& sequenceId) {
    initialRowInDb.sequenceId = sequenceId;
}

U2MsaRow MultipleAlignmentRowData::getRowDbInfo() const {
    U2MsaRow row;
    row.rowId = initialRowInDb.rowId;
    row.sequenceId = initialRowInDb.sequenceId;
    row.chromatogramId = initialRowInDb.chromatogramId;
    row.gstart = 0;
    row.gend = sequence.length();
    row.gaps = gaps;
    row.length = getRowLengthWithoutTrailing();
    return row;
}

void MultipleAlignmentRowData::setRowDbInfo(const U2MsaRow& dbRow) {
    invalidateGappedCache();
    initialRowInDb = dbRow;
}

void MultipleAlignmentRowData::invalidateGappedCache() const {
    gappedCacheOffset = 0;
    gappedSequenceCache.clear();
}

const DNASequence& MultipleAlignmentRowData::getSequence() const {
    return sequence;
}

void MultipleAlignmentRowData::setGapModel(const QVector<U2MsaGap>& newGapModel) {
    invalidateGappedCache();
    gaps = newGapModel;
    removeTrailingGaps();
}

void MultipleAlignmentRowData::removeTrailingGaps() {
    CHECK(!gaps.isEmpty(), )
    MsaRowUtils::removeTrailingGapsFromModel(sequence.length(), gaps);
}

DNAChromatogram MultipleAlignmentRowData::getGappedChromatogram() const {
    return ChromatogramUtils::getGappedChromatogram(chromatogram, gaps);
}

qint64 MultipleAlignmentRowData::getGappedPosition(int pos) const {
    return MsaRowUtils::getGappedRegion(gaps, U2Region(pos, 1)).startPos;
}

QByteArray MultipleAlignmentRowData::toByteArray(U2OpStatus& os, int length) const {
    if (length < getCoreEnd()) {
        os.setError("Failed to get row data");
        return {};
    }

    if (gaps.isEmpty() && sequence.length() == length) {
        return sequence.constSequence();
    }

    QByteArray bytes = getSequenceWithGaps(true, true);

    // Append additional gaps, if necessary
    if (length > bytes.count()) {
        QByteArray gapsBytes;
        gapsBytes.fill(U2Msa::GAP_CHAR, length - bytes.count());
        bytes.append(gapsBytes);
    }
    if (length < bytes.count()) {
        // cut extra trailing gaps
        bytes = bytes.left(length);
    }

    return bytes;
}

int MultipleAlignmentRowData::getRowLength() const {
    SAFE_POINT_NN(alignment, 0);
    return alignment->getLength();
}

int MultipleAlignmentRowData::getRowLengthWithoutTrailing() const {
    return MsaRowUtils::getRowLength(sequence.seq, gaps);
}

int MultipleAlignmentRowData::getCoreStart() const {
    return MsaRowUtils::getCoreStart(gaps);
}

int MultipleAlignmentRowData::getCoreEnd() const {
    return getRowLengthWithoutTrailing();
}

qint64 MultipleAlignmentRowData::getCoreLength() const {
    int coreStart = getCoreStart();
    int coreEnd = getCoreEnd();
    int length = coreEnd - coreStart;
    SAFE_POINT(length >= 0, QString("Internal error in MultipleAlignmentRowData: coreEnd is %1, coreStart is %2!").arg(coreEnd).arg(coreStart), length);
    return length;
}

QByteArray MultipleAlignmentRowData::getCore() const {
    return getSequenceWithGaps(false, false);
}

QByteArray MultipleAlignmentRowData::getData() const {
    return getSequenceWithGaps(true, true);
}

bool MultipleAlignmentRowData::simplify() {
    CHECK(!gaps.isEmpty(), false)
    invalidateGappedCache();
    gaps.clear();
    return true;
}

void MultipleAlignmentRowData::append(const MultipleAlignmentRow& anotherRow, int lengthBefore, U2OpStatus& os) {
    append(*anotherRow, lengthBefore, os);
}

void MultipleAlignmentRowData::append(const MultipleAlignmentRowData& anotherRow, int lengthBefore, U2OpStatus& os) {
    int rowLength = getRowLengthWithoutTrailing();

    if (lengthBefore < rowLength) {
        coreLog.trace(QString("Internal error: incorrect length '%1' were passed to MultipleAlignmentRowData::append,"
                              "coreEnd is '%2'")
                          .arg(lengthBefore)
                          .arg(getCoreEnd()));
        os.setError("Failed to append one row to another");
        return;
    }

    invalidateGappedCache();

    // Gap between rows
    if (lengthBefore > rowLength) {
        gaps.append(U2MsaGap(getRowLengthWithoutTrailing(), lengthBefore - getRowLengthWithoutTrailing()));
    }

    // Merge gaps
    QVector<U2MsaGap> anotherRowGaps = anotherRow.getGaps();
    for (int i = 0; i < anotherRowGaps.count(); ++i) {
        anotherRowGaps[i].startPos += lengthBefore;
    }
    gaps.append(anotherRowGaps);
    mergeConsecutiveGaps();

    // Merge sequences
    DNASequenceUtils::append(sequence, anotherRow.sequence);

    // Merge chromatograms
    ChromatogramUtils::append(chromatogram, anotherRow.chromatogram);
}

void MultipleAlignmentRowData::mergeConsecutiveGaps() {
    MsaRowUtils::mergeConsecutiveGaps(gaps);
}

void MultipleAlignmentRowData::setRowContent(const DNAChromatogram& newChromatogram, const DNASequence& newSequence, const QVector<U2MsaGap>& newGapModel, U2OpStatus& os) {
    SAFE_POINT_EXT(!newSequence.constSequence().contains(U2Msa::GAP_CHAR), os.setError("The sequence must be without gaps"), );
    chromatogram = newChromatogram;
    sequence = newSequence;
    setGapModel(newGapModel);

    if (sequence.length() > chromatogram->seqLength) {
        ushort baseCall = chromatogram->baseCalls.isEmpty() ? 0 : chromatogram->baseCalls.last();
        chromatogram->baseCalls.insert(chromatogram->seqLength, sequence.length() - chromatogram->seqLength, baseCall);
    }
}

void MultipleAlignmentRowData::setRowContent(const DNASequence& newSequence, const QVector<U2MsaGap>& newGapModel, U2OpStatus& os) {
    SAFE_POINT_EXT(!newSequence.constSequence().contains(U2Msa::GAP_CHAR), os.setError("The sequence must be without gaps"), );
    invalidateGappedCache();
    sequence = newSequence;
    chromatogram = DNAChromatogram();  // Clear chromatogram.
    setGapModel(newGapModel);
}

void MultipleAlignmentRowData::setRowContent(const QByteArray& bytes, int offset, U2OpStatus&) {
    invalidateGappedCache();

    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MaDbiUtils::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(getName(), newSequenceBytes);

    addOffsetToGapModel(newGapsModel, offset);

    sequence = newSequence;
    chromatogram = DNAChromatogram();  // Clear chromatogram.
    gaps = newGapsModel;
    removeTrailingGaps();
}

void MultipleAlignmentRowData::addOffsetToGapModel(QVector<U2MsaGap>& gapModel, int offset) {
    CHECK(offset != 0, );

    if (!gapModel.isEmpty()) {
        U2MsaGap& firstGap = gapModel[0];
        if (firstGap.startPos == 0) {
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
                gapModel[i].startPos = (int)newOffset;
            }
        }
    } else {
        SAFE_POINT(offset >= 0, "Negative gap offset", );
        U2MsaGap gap(0, offset);
        gapModel.append(gap);
    }
}

void MultipleAlignmentRowData::insertGaps(int pos, int count, U2OpStatus& os) {
    invalidateGappedCache();
    MsaRowUtils::insertGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
}

void MultipleAlignmentRowData::removeChars(int pos, int count, U2OpStatus& os) {
    if (pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MultipleAlignmentRowData::removeChars, "
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

void MultipleAlignmentRowData::getStartAndEndSequencePositions(int pos, int count, int& startPosInSeq, int& endPosInSeq) const {
    int rowLengthWithoutTrailingGap = getRowLengthWithoutTrailing();
    SAFE_POINT(pos < rowLengthWithoutTrailingGap,
               QString("Incorrect position '%1' in MultipleAlignmentRowData::getStartAndEndSequencePosition, "
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
    } else if (U2Msa::GAP_CHAR == charAt(endRegionPos)) {
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

void MultipleAlignmentRowData::removeGapsFromGapModel(U2OpStatus& os, int pos, int count) {
    invalidateGappedCache();
    MsaRowUtils::removeGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
}

char MultipleAlignmentRowData::charAt(qint64 position) const {
    return getCharFromCache((int)position);
}

bool MultipleAlignmentRowData::isGap(qint64 pos) const {
    return MsaRowUtils::isGap(sequence.length(), gaps, (int)pos);
}

bool MultipleAlignmentRowData::isLeadingOrTrailingGap(qint64 pos) const {
    return MsaRowUtils::isLeadingOrTrailingGap(sequence.length(), gaps, (int)pos);
}

qint64 MultipleAlignmentRowData::getBaseCount(qint64 before) const {
    int rowLength = MsaRowUtils::getRowLength(sequence.seq, gaps);
    int trimmedRowPos = before < rowLength ? (int)before : rowLength;
    return MsaRowUtils::getUngappedPosition(gaps, sequence.length(), trimmedRowPos, true);
}

/**
 * Size of the gapped cache: 200 symbols.
 * Optimized for chars per screen and makes getChar() ~200x faster thanMsaRowUtils::charAt for sequential access.
 */
static constexpr int GAPPED_CACHE_SIZE = 200;

char MultipleAlignmentRowData::getCharFromCache(int gappedPosition) const {
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

bool MultipleAlignmentRowData::isEqual(const MultipleAlignmentRowData& other) const {
    CHECK(this != &other, true);
    CHECK(getName() == other.getName(), false);
    CHECK(gaps == other.getGaps(), false);
    CHECK(sequence.alphabet == other.sequence.alphabet, false);
    CHECK(sequence.seq == sequence.seq, false);
    if (!chromatogram->isEmpty() || !other.chromatogram->isEmpty()) {
        CHECK(ChromatogramUtils::checkAllFieldsEqual(chromatogram, other.chromatogram), false);
    }
    return true;
}

void MultipleAlignmentRowData::crop(U2OpStatus& os, int startPosition, int count) {
    if (startPosition < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MultipleAlignmentRowData::crop, "
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

    if (!chromatogram->isEmpty()) {
        ChromatogramUtils::crop(chromatogram, startPosition, count);
    }

    if (startPosition + count < initialRowLength) {
        removeGapsFromGapModel(os, startPosition + count, initialRowLength - startPosition - count);
    }

    if (startPosition > 0) {
        removeGapsFromGapModel(os, 0, startPosition);
    }
    removeTrailingGaps();
}

void MultipleAlignmentRowData::toUpperCase() {
    DNASequenceUtils::toUpperCase(sequence);
}

void MultipleAlignmentRowData::setAdditionalInfo(const QVariantMap& newAdditionalInfo) {
    additionalInfo = newAdditionalInfo;
}

const QVariantMap& MultipleAlignmentRowData::getAdditionalInfo() const {
    return additionalInfo;
}

void MultipleAlignmentRowData::replaceChars(char origChar, char resultChar, U2OpStatus& os) {
    if (origChar == U2Msa::GAP_CHAR) {
        coreLog.trace("The original char can't be a gap in MultipleAlignmentRowData::replaceChars");
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
        for (int index : gapsIndexes) {
            U2MsaGap gap(index, 1);
            newGapsModel.append(gap);
        }
        std::sort(newGapsModel.begin(), newGapsModel.end(), U2MsaGap::lessThan);

        // Replace the gaps model with the new one
        gaps = newGapsModel;
        mergeConsecutiveGaps();

        if (!chromatogram->isEmpty()) {
            for (int index : qAsConst(gapsIndexes)) {
                chromatogram->baseCalls.removeAt(index);
            }
            chromatogram->seqLength -= gapsIndexes.size();
        }

    } else {
        // Just replace all occurrences of 'origChar' by 'resultChar'
        sequence.seq.replace(origChar, resultChar);
    }
}

const QVector<U2MsaGap>& MultipleAlignmentRowData::getGaps() const {
    return gaps;
}

void MultipleAlignmentRowData::reverse() {
    sequence = DNASequenceUtils::reverse(sequence);
    chromatogram = ChromatogramUtils::reverse(chromatogram);
    gaps = MsaRowUtils::reverseGapModel(gaps, getRowLengthWithoutTrailing());
    MultipleAlignmentRowInfo::setReversed(additionalInfo, !isReversed());
}

void MultipleAlignmentRowData::complement() {
    sequence = DNASequenceUtils::complement(sequence);
    chromatogram = ChromatogramUtils::complement(chromatogram);
    MultipleAlignmentRowInfo::setComplemented(additionalInfo, !isComplemented());
}

void MultipleAlignmentRowData::reverseComplement() {
    reverse();
    complement();
}

bool MultipleAlignmentRowData::isReversed() const {
    return MultipleAlignmentRowInfo::getReversed(additionalInfo);
}

bool MultipleAlignmentRowData::isComplemented() const {
    return MultipleAlignmentRowInfo::getComplemented(additionalInfo);
}

const QMap<ChromatogramData::Trace, QVector<ushort> ChromatogramData::*> PEAKS =
    {{ChromatogramData::Trace::Trace_A, &ChromatogramData::A},
     {ChromatogramData::Trace::Trace_C, &ChromatogramData::C},
     {ChromatogramData::Trace::Trace_G, &ChromatogramData::G},
     {ChromatogramData::Trace::Trace_T, &ChromatogramData::T}};

QPair<ChromatogramData::TraceAndValue, ChromatogramData::TraceAndValue>
    MultipleAlignmentRowData::getTwoHighestPeaks(int position, bool& hasTwoPeaks) const {
    if (chromatogram->isEmpty()) {
        hasTwoPeaks = false;
        return {
            ChromatogramData::TraceAndValue(ChromatogramData::Trace::Trace_A, 0),
            ChromatogramData::TraceAndValue(ChromatogramData::Trace::Trace_C, 0),
        };
    }
    hasTwoPeaks = true;
    int previousBaseCall = chromatogram->baseCalls[position != 0 ? position - 1 : position];
    int baseCall = chromatogram->baseCalls[position];
    int nextBaseCall = chromatogram->baseCalls[position != (chromatogram->baseCalls.size() - 1) ? position + 1 : position];
    QList<ChromatogramData::TraceAndValue> peaks;

    auto peaksKeys = PEAKS.keys();
    const ChromatogramData& chromatogramData = *chromatogram;
    for (auto peak : qAsConst(peaksKeys)) {
        const QVector<ushort>& chromatogramBaseCallVector = chromatogramData.*PEAKS.value(peak);
        auto peakValue = chromatogramBaseCallVector[baseCall];
        int startOfCharacterBaseCall = baseCall - ((baseCall - previousBaseCall) / 2);
        int startValue = chromatogramBaseCallVector[startOfCharacterBaseCall];
        if (previousBaseCall == baseCall) {
            startValue = chromatogramBaseCallVector[0];
        }
        int endOfCharacterBaseCall = baseCall + ((nextBaseCall - baseCall) / 2);
        int endValue = chromatogramBaseCallVector[endOfCharacterBaseCall];
        if (nextBaseCall == baseCall) {
            endValue = chromatogramBaseCallVector[chromatogramBaseCallVector.size() - 1];
        }

        if (startValue <= peakValue && endValue <= peakValue) {
            peaks.append({peak, peakValue});
        }
    }

    if (peaks.size() < 2) {
        hasTwoPeaks = false;
        return {{ChromatogramData::Trace::Trace_A, 0}, {ChromatogramData::Trace::Trace_C, 0}};
    }

    std::sort(peaks.begin(),
              peaks.end(),
              [](const auto& first, const auto& second) {
                  return first.value > second.value;
              });
    return {peaks[0], peaks[1]};
}

int MultipleAlignmentRowData::getUngappedLength() const {
    return sequence.length();
}

bool MultipleAlignmentRowData::isDefault() const {
    static const MultipleAlignmentRowData emptyRow;
    return isEqual(emptyRow);
}

MultipleAlignmentRow MultipleAlignmentRowData::mid(int pos, int count, U2OpStatus& os) const {
    MultipleAlignmentRow row = getExplicitCopy();
    row->crop(os, pos, count);
    return row;
}

MultipleAlignmentRow MultipleAlignmentRowData::getExplicitCopy() const {
    return {new MultipleAlignmentRowData(*this)};
}

void MultipleAlignmentRowData::setParentAlignment(MultipleAlignmentData* alignmentData) {
    alignment = alignmentData;
}

}  // namespace U2
