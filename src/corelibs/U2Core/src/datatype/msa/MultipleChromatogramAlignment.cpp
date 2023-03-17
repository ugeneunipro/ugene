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

#include <typeinfo>

#include <QSet>

#include <U2Core/McaRowInnerData.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MaStateCheck.h"
#include "MultipleChromatogramAlignment.h"

namespace U2 {

MultipleChromatogramAlignment::MultipleChromatogramAlignment()
    : MultipleAlignment(new MultipleChromatogramAlignmentData()) {
}

MultipleChromatogramAlignment::MultipleChromatogramAlignment(const MultipleAlignment& ma)
    : MultipleAlignment(ma) {
    SAFE_POINT(maData.dynamicCast<MultipleChromatogramAlignmentData>() != nullptr, "Can't cast MultipleAlignment to MultipleChromatogramAlignment", );
}

MultipleChromatogramAlignment::MultipleChromatogramAlignment(MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignment(mcaData) {
}

MultipleChromatogramAlignment::MultipleChromatogramAlignment(const QString& name, const DNAAlphabet* alphabet, const QList<MultipleChromatogramAlignmentRow>& rows)
    : MultipleAlignment(new MultipleChromatogramAlignmentData(name, alphabet, rows)) {
}

MultipleChromatogramAlignmentData* MultipleChromatogramAlignment::data() const {
    return getMcaData().data();
}

MultipleChromatogramAlignmentData& MultipleChromatogramAlignment::operator*() {
    return *getMcaData();
}

const MultipleChromatogramAlignmentData& MultipleChromatogramAlignment::operator*() const {
    return *getMcaData();
}

MultipleChromatogramAlignmentData* MultipleChromatogramAlignment::operator->() {
    return getMcaData().data();
}

const MultipleChromatogramAlignmentData* MultipleChromatogramAlignment::operator->() const {
    return getMcaData().data();
}

MultipleChromatogramAlignment MultipleChromatogramAlignment::clone() const {
    return getMcaData()->getCopy();
}

QSharedPointer<MultipleChromatogramAlignmentData> MultipleChromatogramAlignment::getMcaData() const {
    return maData.dynamicCast<MultipleChromatogramAlignmentData>();
}

namespace {

QList<MultipleAlignmentRow> convertToMaRows(const QList<MultipleChromatogramAlignmentRow>& mcaRows) {
    QList<MultipleAlignmentRow> maRows;
    foreach (const MultipleChromatogramAlignmentRow& mcaRow, mcaRows) {
        maRows << mcaRow;
    }
    return maRows;
}

}  // namespace

MultipleChromatogramAlignmentData::MultipleChromatogramAlignmentData(const QString& name, const DNAAlphabet* alphabet, const QList<MultipleChromatogramAlignmentRow>& rows)
    : MultipleAlignmentData(MultipleAlignmentDataType::MCA, name, alphabet, convertToMaRows(rows)) {
}

MultipleChromatogramAlignmentData::MultipleChromatogramAlignmentData(const MultipleChromatogramAlignmentData& mcaData)
    : MultipleAlignmentData(MultipleAlignmentDataType::MCA) {
    copy(mcaData);
}

bool MultipleChromatogramAlignmentData::trim(bool removeLeadingGaps) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    bool result = false;

    if (removeLeadingGaps) {
        // Verify if there are leading columns of gaps
        // by checking the first gap in each row
        int leadingGapColumnsNum = 0;
        for (const auto& row : qAsConst(rows)) {
            if (!row->getGaps().isEmpty()) {
                const U2MsaGap& firstGap = row->getGaps().first();
                if (firstGap.startPos > 0) {
                    leadingGapColumnsNum = 0;
                    break;
                } else {
                    if (leadingGapColumnsNum == 0) {
                        leadingGapColumnsNum = firstGap.length;
                    } else {
                        leadingGapColumnsNum = qMin(leadingGapColumnsNum, firstGap.length);
                    }
                }
            } else {
                leadingGapColumnsNum = 0;
                break;
            }
        }

        // If there are leading gap columns, remove them
        U2OpStatus2Log os;
        if (leadingGapColumnsNum > 0) {
            for (int i = 0; i < getRowCount(); ++i) {
                getMcaRow(i)->removeChars(0, leadingGapColumnsNum, os);
                CHECK_OP(os, true);
                result = true;
            }
        }
    }

    // Verify right side of the alignment (trailing gaps and rows' lengths)
    qint64 newLength = 0;
    foreach (const MultipleChromatogramAlignmentRow& row, rows) {
        if (newLength == 0) {
            newLength = row->getRowLengthWithoutTrailing();
        } else {
            newLength = qMax(row->getRowLengthWithoutTrailing(), newLength);
        }
    }

    if (newLength != length) {
        length = newLength;
        result = true;
    }

    return result;
}

bool MultipleChromatogramAlignmentData::simplify() {
    MaStateCheck check(this);
    Q_UNUSED(check);

    int newLen = 0;
    bool changed = false;
    for (int i = 0, n = getRowCount(); i < n; i++) {
        changed |= getMcaRow(i)->simplify();
        newLen = qMax(newLen, getMcaRow(i)->getCoreEnd());
    }

    if (!changed) {
        assert(length == newLen);
        return false;
    }
    length = newLen;
    return true;
}

bool MultipleChromatogramAlignmentData::hasEmptyGapModel() const {
    foreach (const MultipleChromatogramAlignmentRow& row, rows) {
        if (!row->getGaps().isEmpty()) {
            return false;
        }
    }
    return true;
}

bool MultipleChromatogramAlignmentData::hasEqualLength() const {
    const int defaultSequenceLength = -1;
    int sequenceLength = defaultSequenceLength;
    for (int i = 0, n = rows.size(); i < n; ++i) {
        if (defaultSequenceLength != sequenceLength && sequenceLength != getMcaRow(i)->getUngappedLength()) {
            return false;
        } else {
            sequenceLength = getMcaRow(i)->getUngappedLength();
        }
    }
    return true;
}

MultipleChromatogramAlignment MultipleChromatogramAlignmentData::mid(int start, int len) const {
    SAFE_POINT(start >= 0 && start + len <= length,
               QString("Incorrect parameters were passed to MultipleChromatogramAlignmentData::mid: "
                       "start '%1', len '%2', the alignment length is '%3'")
                   .arg(start)
                   .arg(len)
                   .arg(length),
               MultipleChromatogramAlignment());

    MultipleChromatogramAlignment res(getName(), alphabet);
    MaStateCheck check(res.data());
    Q_UNUSED(check);

    U2OpStatus2Log os;
    foreach (const MultipleChromatogramAlignmentRow& row, rows) {
        MultipleChromatogramAlignmentRow mRow = row->mid(start, len, os);
        mRow->setParentAlignment(res);
        res->rows << mRow;
    }
    res->length = len;
    return res;
}

MultipleChromatogramAlignmentData& MultipleChromatogramAlignmentData::operator+=(const MultipleChromatogramAlignmentData& mcaData) {
    // TODO: it is used in MUSCLE alignment and it should be something like this. But this emthod is incorrect for the MCA
    MaStateCheck check(this);
    Q_UNUSED(check);

    SAFE_POINT(mcaData.alphabet == alphabet, "Different alphabets in MultipleChromatogramAlignmentData::operator+=", *this);

    int nSeq = getRowCount();
    SAFE_POINT(mcaData.getRowCount() == nSeq, "Different number of rows in MultipleChromatogramAlignmentData::operator+=", *this);

    U2OpStatus2Log os;
    for (int i = 0; i < nSeq; i++) {
        getMcaRow(i)->append(mcaData.getMcaRow(i), length, os);
    }

    length += mcaData.length;
    return *this;
}

bool MultipleChromatogramAlignmentData::operator==(const MultipleChromatogramAlignmentData& other) const {
    return isEqual(other);
}

bool MultipleChromatogramAlignmentData::operator!=(const MultipleChromatogramAlignmentData& other) const {
    return !isEqual(other);
}

bool MultipleChromatogramAlignmentData::crop(const U2Region& region, const QSet<QString>& rowNames, U2OpStatus& os) {
    if (!(region.startPos >= 0 && region.length > 0 && region.length < length && region.startPos < length)) {
        os.setError(QString("Incorrect region was passed to MultipleChromatogramAlignmentData::crop, "
                            "startPos '%1', length '%2'")
                        .arg(region.startPos)
                        .arg(region.length));
        return false;
    }

    int cropLen = region.length;
    if (region.endPos() > length) {
        cropLen -= (region.endPos() - length);
    }

    MaStateCheck check(this);
    Q_UNUSED(check);

    QList<MultipleChromatogramAlignmentRow> newList;
    for (int i = 0; i < rows.size(); i++) {
        MultipleChromatogramAlignmentRow row = getMcaRow(i).clone();
        const QString rowName = row->getName();
        if (rowNames.contains(rowName)) {
            row->crop(os, region.startPos, cropLen);
            CHECK_OP(os, false);
            newList << row;
        }
    }
    setRows(newList);

    length = cropLen;
    return true;
}

bool MultipleChromatogramAlignmentData::crop(const U2Region& region, U2OpStatus& os) {
    return crop(region, getRowNames().toSet(), os);
}

bool MultipleChromatogramAlignmentData::crop(int start, int count, U2OpStatus& os) {
    return crop(U2Region(start, count), os);
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::createRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes) {
    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MultipleChromatogramAlignmentRowData::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(name, newSequenceBytes);

    U2MsaRow row;
    return MultipleChromatogramAlignmentRow(row, chromatogram, newSequence, newGapsModel, this);
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::createRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    QString errorDescr = "Failed to create a multiple alignment row";
    if (sequence.constSequence().indexOf(U2Msa::GAP_CHAR) != -1) {
        coreLog.trace("Attempted to create an alignment row from a sequence with gaps");
        os.setError(errorDescr);
        return MultipleChromatogramAlignmentRow();
    }

    int length = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos > length || !gap.isValid()) {
            coreLog.trace("Incorrect gap model was passed to MultipleChromatogramAlignmentData::createRow");
            os.setError(errorDescr);
            return MultipleChromatogramAlignmentRow();
        }
        length += gap.length;
    }

    return MultipleChromatogramAlignmentRow(rowInDb, chromatogram, sequence, gaps, this);
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::createRow(const MultipleChromatogramAlignmentRow& row) {
    return MultipleChromatogramAlignmentRow(row, this);
}

void MultipleChromatogramAlignmentData::setRows(const QList<MultipleChromatogramAlignmentRow>& mcaRows) {
    rows = convertToMaRows(mcaRows);
}

void MultipleChromatogramAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes) {
    MultipleChromatogramAlignmentRow newRow = createRow(name, chromatogram, bytes);
    addRowPrivate(newRow, bytes.size(), -1);
}

void MultipleChromatogramAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes, int rowIndex) {
    MultipleChromatogramAlignmentRow newRow = createRow(name, chromatogram, bytes);
    addRowPrivate(newRow, bytes.size(), rowIndex);
}

void MultipleChromatogramAlignmentData::addRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, U2OpStatus& os) {
    MultipleChromatogramAlignmentRow newRow = createRow(rowInDb, chromatogram, sequence, rowInDb.gaps, os);
    CHECK_OP(os, );
    addRowPrivate(newRow, rowInDb.length, -1);
}

void MultipleChromatogramAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    U2MsaRow row;
    MultipleChromatogramAlignmentRow newRow = createRow(row, chromatogram, sequence, gaps, os);
    CHECK_OP(os, );

    int len = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        len += gap.length;
    }

    newRow->setName(name);
    addRowPrivate(newRow, len, -1);
}

void MultipleChromatogramAlignmentData::addRow(const U2MsaRow& rowInDb, const McaRowMemoryData& mcaRowMemoryData, U2OpStatus& os) {
    addRow(rowInDb, mcaRowMemoryData.chromatogram, mcaRowMemoryData.sequence, os);
}

void MultipleChromatogramAlignmentData::insertGaps(int row, int pos, int count, U2OpStatus& os) {
    if (pos > length) {
        length = pos + count;
        return;
    }
    if (row >= getRowCount() || row < 0 || pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed "
                              "to MultipleChromatogramAlignmentData::insertGaps: row index '%1', pos '%2', count '%3'")
                          .arg(row)
                          .arg(pos)
                          .arg(count));
        os.setError("Failed to insert gaps into an alignment");
        return;
    }

    if (pos == length) {
        // add trailing gaps --> just increase alignment len
        length += count;
        return;
    }

    MaStateCheck check(this);
    Q_UNUSED(check);

    if (pos >= rows[row]->getRowLengthWithoutTrailing()) {
        length += count;
        return;
    }
    getMcaRow(row)->insertGaps(pos, count, os);

    qint64 rowLength = rows[row]->getRowLengthWithoutTrailing();
    length = qMax(length, rowLength);
}

void MultipleChromatogramAlignmentData::appendChars(int row, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleChromatogramAlignmentData::appendChars").arg(row), );

    MultipleChromatogramAlignmentRow appendedRow = createRow("", DNAChromatogram(), QByteArray(str, len));

    qint64 rowLength = getMcaRow(row)->getRowLength();

    U2OpStatus2Log os;
    getMcaRow(row)->append(appendedRow, rowLength, os);
    CHECK_OP(os, );

    length = qMax(length, rowLength + len);
}

void MultipleChromatogramAlignmentData::appendChars(int row, qint64 afterPos, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleChromatogramAlignmentData::appendChars").arg(row), );

    MultipleChromatogramAlignmentRow appendedRow = createRow("", DNAChromatogram(), QByteArray(str, len));

    U2OpStatus2Log os;
    getMcaRow(row)->append(appendedRow, afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + len);
}

void MultipleChromatogramAlignmentData::removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows) {
    SAFE_POINT(startPos >= 0 && startPos + nBases <= length && nBases > 0,
               QString("Incorrect parameters were passed to MultipleChromatogramAlignmentData::removeRegion: startPos '%1', "
                       "nBases '%2', the length is '%3'")
                   .arg(startPos)
                   .arg(nBases)
                   .arg(length), );
    SAFE_POINT(startRow >= 0 && startRow + nRows <= getRowCount() && nRows > 0,
               QString("Incorrect parameters were passed to MultipleChromatogramAlignmentData::removeRegion: startRow '%1', "
                       "nRows '%2', the number of rows is '%3'")
                   .arg(startRow)
                   .arg(nRows)
                   .arg(getRowCount()), );

    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    for (int i = startRow + nRows; --i >= startRow;) {
        getMcaRow(i)->removeChars(startPos, nBases, os);
        SAFE_POINT_OP(os, );

        if (removeEmptyRows && (0 == getMcaRow(i)->getSequence().length())) {
            rows.removeAt(i);
        }
    }

    if (nRows == rows.size()) {
        // full columns were removed
        length -= nBases;
        if (length == 0) {
            rows.clear();
        }
    }
}

int MultipleChromatogramAlignmentData::getRowCount() const {
    return rows.size();
}

void MultipleChromatogramAlignmentData::renameRow(int row, const QString& name) {
    SAFE_POINT(row >= 0 && row < getRowCount(),
               QString("Incorrect row index '%1' was passed to MultipleChromatogramAlignmentData::renameRow: "
                       "the number of rows is '%2'")
                   .arg(row)
                   .arg(getRowCount()), );
    SAFE_POINT(!name.isEmpty(),
               "Incorrect parameter 'name' was passed to MultipleChromatogramAlignmentData::renameRow: "
               "Can't set the name of a row to an empty string", );
    rows[row]->setName(name);
}

void MultipleChromatogramAlignmentData::replaceChars(int row, char origChar, char resultChar) {
    SAFE_POINT(row >= 0 && row < getRowCount(), QString("Incorrect row index '%1' in MultipleChromatogramAlignmentData::replaceChars").arg(row), );

    if (origChar == resultChar) {
        return;
    }

    U2OpStatus2Log os;
    getMcaRow(row)->replaceChars(origChar, resultChar, os);
}

void MultipleChromatogramAlignmentData::setRowContent(int rowNumber, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getRowCount(),
               QString("Incorrect row index '%1' was passed to MultipleChromatogramAlignmentData::setRowContent: "
                       "the number of rows is '%2'")
                   .arg(rowNumber)
                   .arg(getRowCount()), );
    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    getMcaRow(rowNumber)->setRowContent(chromatogram, sequence, gapModel, os);
    SAFE_POINT_OP(os, );

    length = qMax(length, (qint64)MsaRowUtils::getRowLength(sequence.seq, gapModel));
}

void MultipleChromatogramAlignmentData::setRowContent(int rowNumber, const McaRowMemoryData& mcaRowMemoryData) {
    setRowContent(rowNumber, mcaRowMemoryData.chromatogram, mcaRowMemoryData.sequence, mcaRowMemoryData.gapModel);
}

void MultipleChromatogramAlignmentData::toUpperCase() {
    for (int i = 0, n = getRowCount(); i < n; i++) {
        getMcaRow(i)->toUpperCase();
    }
}

bool MultipleChromatogramAlignmentData::sortRowsBySimilarity(QVector<U2Region>& united) {
    QList<MultipleChromatogramAlignmentRow> oldRows = getMcaRows();
    QList<MultipleChromatogramAlignmentRow> sortedRows;
    while (!oldRows.isEmpty()) {
        const MultipleChromatogramAlignmentRow row = oldRows.takeFirst();
        sortedRows << row;
        int start = sortedRows.size() - 1;
        int len = 1;
        QMutableListIterator<MultipleChromatogramAlignmentRow> iter(oldRows);
        while (iter.hasNext()) {
            const MultipleChromatogramAlignmentRow& next = iter.next();
            if (next->isEqualCore(*row)) {
                sortedRows << next;
                iter.remove();
                ++len;
            }
        }
        if (len > 1) {
            united.append(U2Region(start, len));
        }
    }
    if (getMcaRows() != sortedRows) {
        setRows(sortedRows);
        return true;
    }
    return false;
}

const QList<MultipleChromatogramAlignmentRow> MultipleChromatogramAlignmentData::getMcaRows() const {
    QList<MultipleChromatogramAlignmentRow> mcaRows;
    foreach (const MultipleAlignmentRow& maRow, rows) {
        mcaRows << maRow.dynamicCast<MultipleChromatogramAlignmentRow>();
    }
    return mcaRows;
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::getMcaRowByRowId(qint64 rowId, U2OpStatus& os) const {
    return getRowByRowId(rowId, os).dynamicCast<MultipleChromatogramAlignmentRow>(os);
}

char MultipleChromatogramAlignmentData::charAt(int rowNumber, int pos) const {
    return getMcaRow(rowNumber)->charAt(pos);
}

bool MultipleChromatogramAlignmentData::isGap(int rowNumber, int pos) const {
    return getMcaRow(rowNumber)->isGap(pos);
}

bool MultipleChromatogramAlignmentData::isTrailingOrLeadingGap(int rowNumber, int pos) const {
    return getMcaRow(rowNumber)->isTrailingOrLeadingGap(pos);
}

void MultipleChromatogramAlignmentData::setRowGapModel(int rowNumber, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getRowCount(), "Invalid row index", );
    length = qMax(length, (qint64)MsaRowUtils::getGapsLength(gapModel) + getMcaRow(rowNumber)->sequence.length());
    getMcaRow(rowNumber)->setGapModel(gapModel);
}

void MultipleChromatogramAlignmentData::setSequenceId(int rowIndex, const U2DataId& sequenceId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getRowCount(), "Invalid row index", );
    getMcaRow(rowIndex)->setSequenceId(sequenceId);
}

const MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::getMcaRow(const QString& name) const {
    return getRow(name).dynamicCast<const MultipleChromatogramAlignmentRow>();
}

MultipleAlignment MultipleChromatogramAlignmentData::getCopy() const {
    return getExplicitCopy();
}

MultipleChromatogramAlignment MultipleChromatogramAlignmentData::getExplicitCopy() const {
    return MultipleChromatogramAlignment(new MultipleChromatogramAlignmentData(*this));
}

void MultipleChromatogramAlignmentData::copy(const MultipleAlignmentData& other) {
    try {
        copy(dynamic_cast<const MultipleChromatogramAlignmentData&>(other));
    } catch (std::bad_cast&) {
        FAIL("Can't cast MultipleAlignmentData to MultipleChromatogramAlignmentData", );
    }
}

void MultipleChromatogramAlignmentData::copy(const MultipleChromatogramAlignmentData& other) {
    clear();

    alphabet = other.alphabet;
    length = other.length;
    info = other.info;

    for (int i = 0; i < other.rows.size(); i++) {
        const MultipleChromatogramAlignmentRow row = createRow(other.rows[i]);
        addRowPrivate(row, other.length, i);
    }
}

MultipleAlignmentRow MultipleChromatogramAlignmentData::getEmptyRow() const {
    return MultipleChromatogramAlignmentRow();
}

}  // namespace U2
