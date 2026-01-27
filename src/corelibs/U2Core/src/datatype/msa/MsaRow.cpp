/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "MsaRow.h"

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MsaRowInfo.h>
#include <U2Core/U2SafePoints.h>

#include "Msa.h"

namespace U2 {

MsaRow::MsaRow(MsaData* _maRowData)
    : maRowData(new MsaRowData(_maRowData)) {
}

MsaRow::MsaRow(MsaRowData* _maRowData)
    : maRowData(_maRowData) {
}

MsaRow::MsaRow(const U2MsaRow& rowInDb,
               const DNASequence& sequence,
               const QVector<U2MsaGap>& gaps,
               const U2DataId& chromatogramId,
               const Chromatogram& chromatogram,
               MsaData* maData)
    : maRowData(new MsaRowData(rowInDb, sequence, gaps, chromatogramId, chromatogram, maData)) {
}

MsaRow::MsaRow(const U2MsaRow& rowInDb,
               const DNASequence& sequence,
               const QVector<U2MsaGap>& gaps,
               MsaData* maData)
    : maRowData(new MsaRowData(rowInDb, sequence, gaps, {}, {}, maData)) {
}

MsaRow::MsaRow(const U2MsaRow& rowInDb,
               const QString& rowName,
               const QByteArray& rawSequenceData,
               MsaData* maData)
    : maRowData(new MsaRowData(rowInDb, rowName, rawSequenceData, maData)) {
}

MsaRow::MsaRow(const MsaRow& row, MsaData* mcaData)
    : maRowData(new MsaRowData(row, mcaData)) {
}

MsaRow MsaRow::clone() const {
    return maRowData->getExplicitCopy();
}

MsaRowData* MsaRow::data() const {
    return maRowData.data();
}

MsaRowData& MsaRow::operator*() {
    return *maRowData;
}

const MsaRowData& MsaRow::operator*() const {
    return *maRowData;
}

MsaRowData* MsaRow::operator->() {
    return maRowData.data();
}

const MsaRowData* MsaRow::operator->() const {
    return maRowData.data();
}

MsaRowData::MsaRowData(MsaData* _alignment)
    : alignment(_alignment) {
}

MsaRowData::MsaRowData(const U2MsaRow& _rowInDb,
                       const DNASequence& _sequence,
                       const QVector<U2MsaGap>& _gaps,
                       const U2DataId& _chromatogramId,
                       const Chromatogram& _chromatogram,
                       MsaData* _alignment)
    : sequence(_sequence),
      gaps(_gaps),
      chromatogramId(_chromatogramId),
      chromatogram(_chromatogram),
      initialRowInDb(_rowInDb),
      alignment(_alignment) {
    SAFE_POINT(alignment != nullptr, "Parent MultipleChromatogramAlignmentData are NULL", );
    removeTrailingGaps();
}

MsaRowData::MsaRowData(const U2MsaRow& _rowInDb,
                       const QString& _rowName,
                       const QByteArray& rawSequenceData,
                       MsaData* _alignment)
    : initialRowInDb(_rowInDb),
      alignment(_alignment) {
    QByteArray sequenceData;
    QVector<U2MsaGap> gapModel;
    MaDbiUtils::splitBytesToCharsAndGaps(rawSequenceData, sequenceData, gapModel);
    sequence = DNASequence(_rowName, sequenceData);
    setGapModel(gapModel);
}

MsaRowData::MsaRowData(const MsaRow& _row, MsaData* _alignment)
    : sequence(_row->sequence),
      gaps(_row->gaps),
      chromatogramId(_row->chromatogramId),
      chromatogram(_row->chromatogram),
      initialRowInDb(_row->initialRowInDb),
      additionalInfo(_row->additionalInfo),
      alignment(_alignment) {
    SAFE_POINT_NN(alignment, );
}

int MsaRowData::getUngappedPosition(int pos) const {
    return (int)MsaRowUtils::getUngappedPosition(gaps, sequence.length(), pos);
}

DNASequence MsaRowData::getUngappedSequence() const {
    return sequence;
}

U2Region MsaRowData::getGapped(const U2Region& region) {
    return MsaRowUtils::getGappedRegion(gaps, region);
}

bool MsaRowData::operator==(const MsaRowData& other) const {
    return isEqual(other);
}

bool MsaRowData::operator!=(const MsaRowData& other) const {
    return !isEqual(other);
}

bool MsaRowData::isTrailingOrLeadingGap(qint64 position) const {
    CHECK(isGap(position), false);
    return position < getCoreStart() || position > getCoreEnd() - 1;
}

U2Region MsaRowData::getCoreRegion() const {
    return {getCoreStart(), getCoreLength()};
}

U2Region MsaRowData::getUngappedRegion(const U2Region& gappedRegion) const {
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
bool MsaRowData::isEqualIgnoreGaps(const MsaRowData* row1, const MsaRowData* row2) {
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

bool MsaRowData::isEqualCore(const MsaRowData& other) const {
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

QByteArray MsaRowData::getSequenceWithGaps(bool keepLeadingGaps, bool keepTrailingGaps) const {
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

const Chromatogram& MsaRowData::getChromatogram() const {
    return chromatogram;
}

QString MsaRowData::getName() const {
    return sequence.getName();
}

void MsaRowData::setName(const QString& name) {
    sequence.setName(name);
}

qint64 MsaRowData::getRowId() const {
    return initialRowInDb.rowId;
}

void MsaRowData::setRowId(qint64 rowId) {
    initialRowInDb.rowId = rowId;
}

void MsaRowData::setSequenceId(const U2DataId& sequenceId) {
    initialRowInDb.sequenceId = sequenceId;
}

U2MsaRow MsaRowData::getRowDbInfo() const {
    U2MsaRow row;
    row.rowId = initialRowInDb.rowId;
    row.sequenceId = initialRowInDb.sequenceId;
    row.gstart = 0;
    row.gend = sequence.length();
    row.gaps = gaps;
    row.length = getRowLengthWithoutTrailing();
    return row;
}

const U2DataId& MsaRowData::getSequenceId() const {
    return initialRowInDb.sequenceId;
}

const U2DataId& MsaRowData::getChromatogramId() const {
    return chromatogramId;
}

void MsaRowData::setRowDbInfo(const U2MsaRow& dbRow) {
    invalidateGappedCache();
    initialRowInDb = dbRow;
}

void MsaRowData::invalidateGappedCache() const {
    gappedCacheOffset = 0;
    gappedSequenceCache.clear();
}

const DNASequence& MsaRowData::getSequence() const {
    return sequence;
}

void MsaRowData::setGapModel(const QVector<U2MsaGap>& newGapModel) {
    invalidateGappedCache();
    gaps = newGapModel;
    removeTrailingGaps();
}

void MsaRowData::removeTrailingGaps() {
    CHECK(!gaps.isEmpty(), )
    MsaRowUtils::removeTrailingGapsFromModel(sequence.length(), gaps);
}

Chromatogram MsaRowData::getGappedChromatogram() const {
    return ChromatogramUtils::getGappedChromatogram(chromatogram, gaps);
}

qint64 MsaRowData::getGappedPosition(int pos) const {
    return MsaRowUtils::getGappedRegion(gaps, U2Region(pos, 1)).startPos;
}

QByteArray MsaRowData::toByteArray(U2OpStatus& os, int length) const {
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

int MsaRowData::getRowLength() const {
    SAFE_POINT_NN(alignment, 0);
    return alignment->getLength();
}

int MsaRowData::getRowLengthWithoutTrailing() const {
    return MsaRowUtils::getRowLength(sequence.seq, gaps);
}

int MsaRowData::getCoreStart() const {
    return MsaRowUtils::getCoreStart(gaps);
}

int MsaRowData::getCoreEnd() const {
    return getRowLengthWithoutTrailing();
}

qint64 MsaRowData::getCoreLength() const {
    int coreStart = getCoreStart();
    int coreEnd = getCoreEnd();
    int length = coreEnd - coreStart;
    SAFE_POINT(length >= 0, QString("Internal error in MsaRowData: coreEnd is %1, coreStart is %2!").arg(coreEnd).arg(coreStart), length);
    return length;
}

QByteArray MsaRowData::getCore() const {
    return getSequenceWithGaps(false, false);
}

QByteArray MsaRowData::getData() const {
    return getSequenceWithGaps(true, true);
}

bool MsaRowData::simplify() {
    CHECK(!gaps.isEmpty(), false)
    invalidateGappedCache();
    gaps.clear();
    return true;
}

void MsaRowData::append(const MsaRow& anotherRow, int lengthBefore, U2OpStatus& os) {
    append(*anotherRow, lengthBefore, os);
}

void MsaRowData::append(const MsaRowData& anotherRow, int lengthBefore, U2OpStatus& os) {
    int rowLength = getRowLengthWithoutTrailing();

    if (lengthBefore < rowLength) {
        coreLog.trace(QString("Internal error: incorrect length '%1' were passed to MsaRowData::append,"
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

void MsaRowData::mergeConsecutiveGaps() {
    MsaRowUtils::mergeConsecutiveGaps(gaps);
}

void MsaRowData::setRowContent(const Chromatogram& newChromatogram, const DNASequence& newSequence, const QVector<U2MsaGap>& newGapModel, U2OpStatus& os) {
    SAFE_POINT_EXT(!newSequence.constSequence().contains(U2Msa::GAP_CHAR), os.setError("The sequence must be without gaps"), );
    chromatogram = newChromatogram;
    sequence = newSequence;
    setGapModel(newGapModel);

    if (sequence.length() > chromatogram->seqLength) {
        ushort baseCall = chromatogram->baseCalls.isEmpty() ? 0 : chromatogram->baseCalls.last();
        chromatogram->baseCalls.insert(chromatogram->seqLength, sequence.length() - chromatogram->seqLength, baseCall);
    }
}

void MsaRowData::setRowContent(const DNASequence& newSequence, const QVector<U2MsaGap>& newGapModel, U2OpStatus& os) {
    SAFE_POINT_EXT(!newSequence.constSequence().contains(U2Msa::GAP_CHAR), os.setError("The sequence must be without gaps"), );
    invalidateGappedCache();
    sequence = newSequence;
    chromatogram = Chromatogram();  // Clear chromatogram.
    setGapModel(newGapModel);
}

void MsaRowData::setRowContent(const QByteArray& bytes, int offset, U2OpStatus&) {
    invalidateGappedCache();

    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MaDbiUtils::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(getName(), newSequenceBytes);

    addOffsetToGapModel(newGapsModel, offset);

    sequence = newSequence;
    chromatogram = Chromatogram();  // Clear chromatogram.
    gaps = newGapsModel;
    removeTrailingGaps();
}

void MsaRowData::addOffsetToGapModel(QVector<U2MsaGap>& gapModel, int offset) {
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

void MsaRowData::insertGaps(int pos, int count, U2OpStatus& os) {
    invalidateGappedCache();
    MsaRowUtils::insertGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
}

void MsaRowData::removeChars(int pos, int count, U2OpStatus& os) {
    if (pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MsaRowData::removeChars, "
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

void MsaRowData::getStartAndEndSequencePositions(int pos, int count, int& startPosInSeq, int& endPosInSeq) const {
    int rowLengthWithoutTrailingGap = getRowLengthWithoutTrailing();
    SAFE_POINT(pos < rowLengthWithoutTrailingGap,
               QString("Incorrect position '%1' in MsaRowData::getStartAndEndSequencePosition, "
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

void MsaRowData::removeGapsFromGapModel(U2OpStatus& os, int pos, int count) {
    invalidateGappedCache();
    MsaRowUtils::removeGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
}

char MsaRowData::charAt(qint64 position) const {
    return getCharFromCache((int)position);
}

bool MsaRowData::isGap(qint64 pos) const {
    return MsaRowUtils::isGap(sequence.length(), gaps, (int)pos);
}

bool MsaRowData::isLeadingOrTrailingGap(qint64 pos) const {
    return MsaRowUtils::isLeadingOrTrailingGap(sequence.length(), gaps, (int)pos);
}

qint64 MsaRowData::getBaseCount(qint64 before) const {
    int rowLength = MsaRowUtils::getRowLength(sequence.seq, gaps);
    int trimmedRowPos = before < rowLength ? (int)before : rowLength;
    return MsaRowUtils::getUngappedPosition(gaps, sequence.length(), trimmedRowPos, true);
}

/**
 * Size of the gapped cache: 200 symbols.
 * Optimized for chars per screen and makes getChar() ~200x faster thanMsaRowUtils::charAt for sequential access.
 */
static constexpr int GAPPED_CACHE_SIZE = 200;

char MsaRowData::getCharFromCache(int gappedPosition) const {
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

bool MsaRowData::isEqual(const MsaRowData& other) const {
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

void MsaRowData::crop(U2OpStatus& os, int startPosition, int count) {
    if (startPosition < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MsaRowData::crop, "
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

void MsaRowData::toUpperCase() {
    DNASequenceUtils::toUpperCase(sequence);
}

void MsaRowData::setAdditionalInfo(const QVariantMap& newAdditionalInfo) {
    additionalInfo = newAdditionalInfo;
}

const QVariantMap& MsaRowData::getAdditionalInfo() const {
    return additionalInfo;
}

void MsaRowData::replaceChars(char origChar, char resultChar, U2OpStatus& os) {
    if (origChar == U2Msa::GAP_CHAR) {
        coreLog.trace("The original char can't be a gap in MsaRowData::replaceChars");
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

const QVector<U2MsaGap>& MsaRowData::getGaps() const {
    return gaps;
}

void MsaRowData::reverse() {
    sequence = DNASequenceUtils::reverse(sequence);
    chromatogram = ChromatogramUtils::reverse(chromatogram);
    gaps = MsaRowUtils::reverseGapModel(gaps, getRowLengthWithoutTrailing());
    MsaRowInfo::setReversed(additionalInfo, !isReversed());
}

void MsaRowData::complement() {
    sequence = DNASequenceUtils::complement(sequence);
    chromatogram = ChromatogramUtils::complement(chromatogram);
    MsaRowInfo::setComplemented(additionalInfo, !isComplemented());
}

void MsaRowData::reverseComplement() {
    reverse();
    complement();
}

bool MsaRowData::isReversed() const {
    return MsaRowInfo::getReversed(additionalInfo);
}

bool MsaRowData::isComplemented() const {
    return MsaRowInfo::getComplemented(additionalInfo);
}

const QMap<ChromatogramData::Trace, QVector<ushort> ChromatogramData::*> PEAKS =
    {{ChromatogramData::Trace::Trace_A, &ChromatogramData::A},
     {ChromatogramData::Trace::Trace_C, &ChromatogramData::C},
     {ChromatogramData::Trace::Trace_G, &ChromatogramData::G},
     {ChromatogramData::Trace::Trace_T, &ChromatogramData::T}};

QPair<ChromatogramData::TraceAndValue, ChromatogramData::TraceAndValue>
    MsaRowData::getTwoHighestPeaks(int position, bool& hasTwoPeaks) const {
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
    QList<ushort> allTraceValues;
    for (auto peak : qAsConst(peaksKeys)) {
        const QVector<ushort>& chromatogramBaseCallVector = chromatogramData.*PEAKS.value(peak);
        ushort peakValue = chromatogramBaseCallVector[baseCall];
        allTraceValues << peakValue;
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

    std::sort(allTraceValues.begin(), allTraceValues.end(), std::greater<>());
    std::sort(peaks.begin(),
              peaks.end(),
              [](const auto& first, const auto& second) {
                  return first.value > second.value;
              });
    if (peaks[0].value != allTraceValues[0] && peaks[1].value != allTraceValues[1]) {
        hasTwoPeaks = false;
        return {{ChromatogramData::Trace::Trace_A, 0}, {ChromatogramData::Trace::Trace_C, 0}};
    }

    return {peaks[0], peaks[1]};
}

int MsaRowData::getUngappedLength() const {
    return sequence.length();
}

bool MsaRowData::isDefault() const {
    static const MsaRowData emptyRow;
    return isEqual(emptyRow);
}

MsaRow MsaRowData::mid(int pos, int count, U2OpStatus& os) const {
    MsaRow row = getExplicitCopy();
    row->crop(os, pos, count);
    return row;
}

MsaRow MsaRowData::getExplicitCopy() const {
    return {new MsaRowData(*this)};
}

void MsaRowData::setParentAlignment(MsaData* alignmentData) {
    alignment = alignmentData;
}

}  // namespace U2
