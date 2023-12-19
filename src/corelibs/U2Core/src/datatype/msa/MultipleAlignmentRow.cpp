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
#include <U2Core/U2SafePoints.h>

#include "MultipleAlignment.h"

namespace U2 {

MultipleAlignmentRow::MultipleAlignmentRow(MultipleAlignmentRowData* ma)
    : maRowData(ma) {
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

MultipleAlignmentRowData::MultipleAlignmentRowData(const MultipleAlignmentDataType& _type)
    : type(_type) {
}

MultipleAlignmentRowData::MultipleAlignmentRowData(const MultipleAlignmentDataType& _type, const DNASequence& sequence, const QVector<U2MsaGap>& gaps)
    : type(_type), sequence(sequence), gaps(gaps) {
}

int MultipleAlignmentRowData::getUngappedPosition(int pos) const {
    return MsaRowUtils::getUngappedPosition(gaps, sequence.length(), pos);
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

    for (int i = 0; i < gaps.size(); ++i) {
        QByteArray gapsBytes;
        if (!keepLeadingGaps && (0 == gaps[i].startPos)) {
            beginningOffset = gaps[i].length;
            continue;
        }

        gapsBytes.fill(U2Msa::GAP_CHAR, gaps[i].length);
        bytes.insert(gaps[i].startPos - beginningOffset, gapsBytes);
    }
    MultipleAlignmentData* alignment = getMultipleAlignmentData();
    SAFE_POINT(alignment != nullptr, "Parent MAlignment is NULL", QByteArray());
    if (keepTrailingGaps && bytes.size() < alignment->getLength()) {
        QByteArray gapsBytes;
        gapsBytes.fill(U2Msa::GAP_CHAR, alignment->getLength() - bytes.size());
        bytes.append(gapsBytes);
    }

    return bytes;
}

bool MultipleAlignmentRowData::isComplemented() const {
    return false;
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
    MultipleAlignmentData* alignment = getMultipleAlignmentData();
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
    SAFE_POINT(length >= 0, QString("Internal error in MultipleChromatogramAlignmentRowData: coreEnd is %1, coreStart is %2!").arg(coreEnd).arg(coreStart), length);
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

}  // namespace U2
