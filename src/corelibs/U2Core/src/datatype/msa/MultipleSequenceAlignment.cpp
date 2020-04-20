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

#include <typeinfo>

#include <QSet>

#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Region.h>

#include "MaStateCheck.h"
#include "MultipleSequenceAlignment.h"
#include "MultipleAlignmentInfo.h"

namespace U2 {

MultipleSequenceAlignment::MultipleSequenceAlignment(const QString &name, const DNAAlphabet *alphabet, const QList<MultipleSequenceAlignmentRow> &rows)
    : MultipleAlignment(new MultipleSequenceAlignmentData(name, alphabet, rows))
{

}

MultipleSequenceAlignment::MultipleSequenceAlignment(const MultipleAlignment &ma)
    : MultipleAlignment(ma)
{
    SAFE_POINT(NULL != maData.dynamicCast<MultipleSequenceAlignmentData>(), "Can't cast MultipleAlignment to MultipleSequenceAlignment", );
}

MultipleSequenceAlignment::MultipleSequenceAlignment(MultipleSequenceAlignmentData *msaData)
    : MultipleAlignment(msaData)
{

}

MultipleSequenceAlignmentData * MultipleSequenceAlignment::data() const {
    return getMsaData().data();
}

MultipleSequenceAlignmentData & MultipleSequenceAlignment::operator*() {
    return *getMsaData();
}

const MultipleSequenceAlignmentData & MultipleSequenceAlignment::operator*() const {
    return *getMsaData();
}

MultipleSequenceAlignmentData * MultipleSequenceAlignment::operator->() {
    return getMsaData().data();
}

const MultipleSequenceAlignmentData * MultipleSequenceAlignment::operator->() const {
    return getMsaData().data();
}

MultipleSequenceAlignment MultipleSequenceAlignment::clone() const {
    return getMsaData()->getExplicitCopy();
}

QSharedPointer<MultipleSequenceAlignmentData> MultipleSequenceAlignment::getMsaData() const {
    return maData.dynamicCast<MultipleSequenceAlignmentData>();
}

namespace {

QList<MultipleAlignmentRow> convertToMaRows(const QList<MultipleSequenceAlignmentRow> &msaRows) {
    QList<MultipleAlignmentRow> maRows;
    foreach (const MultipleSequenceAlignmentRow &msaRow, msaRows) {
        maRows << msaRow;
    }
    return maRows;
}

}

MultipleSequenceAlignmentData::MultipleSequenceAlignmentData(const QString &name, const DNAAlphabet *alphabet, const QList<MultipleSequenceAlignmentRow> &rows)
    : MultipleAlignmentData(name, alphabet, convertToMaRows(rows))
{

}

MultipleSequenceAlignmentData::MultipleSequenceAlignmentData(const MultipleSequenceAlignmentData &msaData)
    : MultipleAlignmentData()
{
    copy(msaData);
}

MultipleSequenceAlignmentData & MultipleSequenceAlignmentData::operator=(const MultipleSequenceAlignment &msa) {
    return *this = *msa;
}

MultipleSequenceAlignmentData & MultipleSequenceAlignmentData::operator=(const MultipleSequenceAlignmentData &msaData) {
    copy(msaData);
    return *this;
}

bool MultipleSequenceAlignmentData::trim(bool removeLeadingGaps) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    bool result = false;

    if ( removeLeadingGaps ) {
        // Verify if there are leading columns of gaps
        // by checking the first gap in each row
        qint64 leadingGapColumnsNum = 0;
        foreach (const MultipleSequenceAlignmentRow &row, rows) {
            if (row->getGapModel().count() > 0) {
                const U2MsaGap firstGap = row->getGapModel().first();
                if (firstGap.offset > 0) {
                    leadingGapColumnsNum = 0;
                    break;
                } else {
                    if (leadingGapColumnsNum == 0) {
                        leadingGapColumnsNum = firstGap.gap;
                    } else {
                        leadingGapColumnsNum = qMin(leadingGapColumnsNum, firstGap.gap);
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
            for (int i = 0; i < getNumRows(); ++i) {
                getMsaRow(i)->removeChars(0, leadingGapColumnsNum, os);
                CHECK_OP(os, true);
                result = true;
            }
        }
    }

    // Verify right side of the alignment (trailing gaps and rows' lengths)
    qint64 newLength = 0;
    foreach (const MultipleSequenceAlignmentRow &row, rows) {
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

bool MultipleSequenceAlignmentData::simplify() {
    MaStateCheck check(this);
    Q_UNUSED(check);

    int newLen = 0;
    bool changed = false;
    for (int i = 0, n = getNumRows(); i < n; i++) {
        changed |= getMsaRow(i)->simplify();
        newLen = qMax(newLen, getMsaRow(i)->getCoreEnd());
    }

    if (!changed) {
        assert(length == newLen);
        return false;
    }
    length = newLen;
    return true;
}

bool MultipleSequenceAlignmentData::hasEmptyGapModel() const {
    foreach (const MultipleSequenceAlignmentRow &row, rows) {
        if (!row->getGapModel().isEmpty()) {
            return false;
        }
    }
    return true;
}

bool MultipleSequenceAlignmentData::hasEqualLength() const {
    const int defaultSequenceLength = -1;
    int sequenceLength = defaultSequenceLength;
    for (int i = 0, n = rows.size(); i < n; ++i) {
        if (defaultSequenceLength != sequenceLength && sequenceLength != getMsaRow(i)->getUngappedLength()) {
            return false;
        } else {
            sequenceLength = getMsaRow(i)->getUngappedLength();
        }
    }
    return true;
}

MultipleSequenceAlignment MultipleSequenceAlignmentData::mid(int start, int len) const {
    SAFE_POINT(start >= 0 && start + len <= length,
               QString("Incorrect parameters were passed to MultipleSequenceAlignmentData::mid: "
                       "start '%1', len '%2', the alignment length is '%3'").arg(start).arg(len).arg(length),
               MultipleSequenceAlignment());

    MultipleSequenceAlignment res(getName(), alphabet);
    MaStateCheck check(res.data());
    Q_UNUSED(check);

    U2OpStatus2Log os;
    foreach (const MultipleSequenceAlignmentRow &row, rows) {
        MultipleSequenceAlignmentRow mRow = row->mid(start, len, os);
        mRow->setParentAlignment(res);
        res->rows << mRow;
    }
    res->length = len;
    return res;
}

MultipleSequenceAlignmentData & MultipleSequenceAlignmentData::operator+=(const MultipleSequenceAlignmentData &msaData) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    SAFE_POINT(msaData.alphabet == alphabet, "Different alphabets in MultipleSequenceAlignmentData::operator+=", *this);

    int nSeq = getNumRows();
    SAFE_POINT(msaData.getNumRows() == nSeq, "Different number of rows in MultipleSequenceAlignmentData::operator+=", *this);

    U2OpStatus2Log os;
    for (int i = 0; i < nSeq; i++) {
        getMsaRow(i)->append(msaData.getMsaRow(i), length, os);
    }

    length += msaData.length;
    return *this;
}

bool MultipleSequenceAlignmentData::operator==(const MultipleSequenceAlignmentData &other) const {
    bool lengthsAreEqual = (length == other.length);
    bool alphabetsAreEqual = (alphabet == other.alphabet);
    bool rowsAreEqual = (rows == other.rows);
    return lengthsAreEqual && alphabetsAreEqual && rowsAreEqual;
}

bool MultipleSequenceAlignmentData::operator!=(const MultipleSequenceAlignmentData &other) const {
    return !operator==(other);
}

bool MultipleSequenceAlignmentData::crop(const U2Region &region, const QSet<QString> &rowNames, U2OpStatus &os) {
    if (!(region.startPos >= 0 && region.length > 0 && region.length < length && region.startPos < length)) {
        os.setError(QString("Incorrect region was passed to MultipleSequenceAlignmentData::crop, "
                            "startPos '%1', length '%2'").arg(region.startPos).arg(region.length));
        return false;
    }

    int cropLen = region.length;
    if (region.endPos() > length) {
        cropLen -= (region.endPos() - length);
    }

    MaStateCheck check(this);
    Q_UNUSED(check);

    QList<MultipleSequenceAlignmentRow> newList;
    for (int i = 0 ; i < rows.size(); i++) {
        MultipleSequenceAlignmentRow row = getMsaRow(i).clone();
        const QString rowName = row->getName();
        if (rowNames.contains(rowName)){
            row->crop(os, region.startPos, cropLen);
            CHECK_OP(os, false);
            newList << row;
        }
    }
    setRows(newList);

    length = cropLen;
    return true;
}

bool MultipleSequenceAlignmentData::crop(const U2Region &region, U2OpStatus &os) {
    return crop(region, getRowNames().toSet(), os);
}

bool MultipleSequenceAlignmentData::crop(int start, int count, U2OpStatus &os) {
    return crop(U2Region(start, count), os);
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::createRow(const QString &name, const QByteArray &bytes) {
    QByteArray newSequenceBytes;
    QList<U2MsaGap> newGapsModel;

    MultipleSequenceAlignmentRowData::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(name, newSequenceBytes);

    U2MsaRow row;
    return MultipleSequenceAlignmentRow(row, newSequence, newGapsModel, this);
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::createRow(const U2MsaRow &rowInDb, const DNASequence &sequence, const QList<U2MsaGap> &gaps, U2OpStatus &os) {
    QString errorDescr = "Failed to create a multiple alignment row";
    if (-1 != sequence.constSequence().indexOf(U2Msa::GAP_CHAR)) {
        coreLog.trace("Attempted to create an alignment row from a sequence with gaps");
        os.setError(errorDescr);
        return MultipleSequenceAlignmentRow();
    }

    int length = sequence.length();
    foreach (const U2MsaGap &gap, gaps) {
        if (gap.offset > length || !gap.isValid()) {
            coreLog.trace("Incorrect gap model was passed to MultipleSequenceAlignmentData::createRow");
            os.setError(errorDescr);
            return MultipleSequenceAlignmentRow();
        }
        length += gap.gap;
    }

    return MultipleSequenceAlignmentRow(rowInDb, sequence, gaps, this);
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::createRow(const MultipleSequenceAlignmentRow &row) {
    return MultipleSequenceAlignmentRow(row, this);
}

void MultipleSequenceAlignmentData::setRows(const QList<MultipleSequenceAlignmentRow> &msaRows) {
    rows = convertToMaRows(msaRows);
}

void MultipleSequenceAlignmentData::addRow(const QString &name, const QByteArray &bytes) {
    MultipleSequenceAlignmentRow newRow = createRow(name, bytes);
    addRowPrivate(newRow, bytes.size(), -1);
}

void MultipleSequenceAlignmentData::addRow(const QString &name, const QByteArray &bytes, int rowIndex) {
    MultipleSequenceAlignmentRow newRow = createRow(name, bytes);
    addRowPrivate(newRow, bytes.size(), rowIndex);
}

void MultipleSequenceAlignmentData::addRow(const U2MsaRow &rowInDb, const DNASequence &sequence, U2OpStatus &os) {
    MultipleSequenceAlignmentRow newRow = createRow(rowInDb, sequence, rowInDb.gaps, os);
    CHECK_OP(os, );
    addRowPrivate(newRow, rowInDb.length, -1);
}

void MultipleSequenceAlignmentData::addRow(const QString &name, const DNASequence &sequence, const QList<U2MsaGap> &gaps, U2OpStatus &os) {
    U2MsaRow row;
    MultipleSequenceAlignmentRow newRow = createRow(row, sequence, gaps, os);
    CHECK_OP(os, );

    int len = sequence.length();
    foreach (const U2MsaGap &gap, gaps) {
        len += gap.gap;
    }

    newRow->setName(name);
    addRowPrivate(newRow, len, -1);
}

void MultipleSequenceAlignmentData::insertGaps(int row, int pos, int count, U2OpStatus &os) {
    if (row >= getNumRows() || row < 0 || pos > length || pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed "
            "to MultipleSequenceAlignmentData::insertGaps: row index '%1', pos '%2', count '%3'").arg(row).arg(pos).arg(count));
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
    getMsaRow(row)->insertGaps(pos, count, os);

    qint64 rowLength = rows[row]->getRowLengthWithoutTrailing();
    length = qMax(length, rowLength);
}

void MultipleSequenceAlignmentData::appendChars(int row, const char *str, int len) {
    SAFE_POINT(0 <= row && row < getNumRows(),
        QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendChars").arg(row), );

    MultipleSequenceAlignmentRow appendedRow = createRow("", QByteArray(str, len));

    qint64 rowLength = getMsaRow(row)->getRowLength();

    U2OpStatus2Log os;
    getMsaRow(row)->append(appendedRow, rowLength, os);
    CHECK_OP(os, );

    length = qMax(length, rowLength + len);
}

void MultipleSequenceAlignmentData::appendChars(int row, qint64 afterPos, const char *str, int len) {
    SAFE_POINT(0 <= row && row < getNumRows(),
        QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendChars").arg(row), );

    MultipleSequenceAlignmentRow appendedRow = createRow("", QByteArray(str, len));

    U2OpStatus2Log os;
    getMsaRow(row)->append(appendedRow, afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + len);
}

void MultipleSequenceAlignmentData::appendRow(int rowNumber, const MultipleSequenceAlignmentRow &row, bool ignoreTrailingGaps, U2OpStatus &os) {
    appendRow(rowNumber, ignoreTrailingGaps ? getMsaRow(rowNumber)->getRowLengthWithoutTrailing() : getMsaRow(rowNumber)->getRowLength(), row, os);
}

void MultipleSequenceAlignmentData::appendRow(int rowNumber, qint64 afterPos, const MultipleSequenceAlignmentRow &row, U2OpStatus &os) {
    SAFE_POINT(0 <= rowNumber && rowNumber < getNumRows(),
        QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendRow").arg(rowNumber), );

    getMsaRow(rowNumber)->append(row, afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + row->getRowLength());
}

void MultipleSequenceAlignmentData::removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows) {
    SAFE_POINT(startPos >= 0 && startPos + nBases <= length && nBases > 0,
        QString("Incorrect parameters were passed to MultipleSequenceAlignmentData::removeRegion: startPos '%1', "
        "nBases '%2', the length is '%3'").arg(startPos).arg(nBases).arg(length),);
    SAFE_POINT(startRow >= 0 && startRow + nRows <= getNumRows() && (nRows > 0 || (nRows == 0 && getNumRows() == 0)),
        QString("Incorrect parameters were passed to MultipleSequenceAlignmentData::removeRegion: startRow '%1', "
        "nRows '%2', the number of rows is '%3'").arg(startRow).arg(nRows).arg(getNumRows()),);

    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    for (int i = startRow + nRows; --i >= startRow;) {
        getMsaRow(i)->removeChars(startPos, nBases, os);
        SAFE_POINT_OP(os, );

        if (removeEmptyRows && (0 == getMsaRow(i)->getSequence().length())) {
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

int MultipleSequenceAlignmentData::getNumRows() const {
    return rows.size();
}

void MultipleSequenceAlignmentData::renameRow(int row, const QString &name) {
    SAFE_POINT(row >= 0 && row < getNumRows(),
        QString("Incorrect row index '%1' was passed to MultipleSequenceAlignmentData::renameRow: "
        "the number of rows is '%2'").arg(row).arg(getNumRows()), );
    SAFE_POINT(!name.isEmpty(),
        "Incorrect parameter 'name' was passed to MultipleSequenceAlignmentData::renameRow: "
        "Can't set the name of a row to an empty string", );
    rows[row]->setName(name);
}


void MultipleSequenceAlignmentData::replaceChars(int row, char origChar, char resultChar) {
    SAFE_POINT(row >= 0 && row < getNumRows(), QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::replaceChars").arg(row), );

    if (origChar == resultChar) {
        return;
    }

    U2OpStatus2Log os;
    getMsaRow(row)->replaceChars(origChar, resultChar, os);
}

void MultipleSequenceAlignmentData::setRowContent(int rowNumber, const QByteArray &sequence, int offset) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getNumRows(),
        QString("Incorrect row index '%1' was passed to MultipleSequenceAlignmentData::setRowContent: "
        "the number of rows is '%2'").arg(rowNumber).arg(getNumRows()), );
    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    getMsaRow(rowNumber)->setRowContent(sequence, offset, os);
    SAFE_POINT_OP(os, );

    length = qMax(length, (qint64)sequence.size() + offset);
}

void MultipleSequenceAlignmentData::toUpperCase() {
    for (int i = 0, n = getNumRows(); i < n; i++) {
        getMsaRow(i)->toUpperCase();
    }
}

bool MultipleSequenceAlignmentData::sortRowsBySimilarity(QVector<U2Region> &united) {
    QList<MultipleSequenceAlignmentRow> sortedRows = getRowsSortedBySimilarity(united);
    if (getMsaRows() == sortedRows) {
        return false;
    }
    setRows(sortedRows);
    return true;
}

QList<MultipleSequenceAlignmentRow> MultipleSequenceAlignmentData::getRowsSortedBySimilarity(QVector<U2Region> &united) const {
    QList<MultipleSequenceAlignmentRow> oldRows = getMsaRows();
    QList<MultipleSequenceAlignmentRow> sortedRows;
    while (!oldRows.isEmpty()) {
        const MultipleSequenceAlignmentRow row = oldRows.takeFirst();
        sortedRows << row;
        int start = sortedRows.size() - 1;
        int len = 1;
        QMutableListIterator<MultipleSequenceAlignmentRow> iter(oldRows);
        while (iter.hasNext()) {
            const MultipleSequenceAlignmentRow &next = iter.next();
            if (next->isRowContentEqual(row)) {
                sortedRows << next;
                iter.remove();
                ++len;
            }
        }
        if (len > 1) {
            united.append(U2Region(start, len));
        }
    }
    return sortedRows;
}

const QList<MultipleSequenceAlignmentRow> MultipleSequenceAlignmentData::getMsaRows() const {
    QList<MultipleSequenceAlignmentRow> msaRows;
    foreach (const MultipleAlignmentRow &maRow, rows) {
        msaRows << maRow.dynamicCast<MultipleSequenceAlignmentRow>();
    }
    return msaRows;
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::getMsaRowByRowId(qint64 rowId, U2OpStatus &os) const {
    return getRowByRowId(rowId, os).dynamicCast<MultipleSequenceAlignmentRow>(os);
}

char MultipleSequenceAlignmentData::charAt(int rowNumber, int pos) const {
    return getMsaRow(rowNumber)->charAt(pos);
}

bool MultipleSequenceAlignmentData::isGap(int rowNumber, int pos) const {
    return getMsaRow(rowNumber)->isGap(pos);
}

void MultipleSequenceAlignmentData::setRowGapModel(int rowNumber, const QList<U2MsaGap> &gapModel) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getNumRows(), "Invalid row index", );
    length = qMax(length, (qint64)MsaRowUtils::getGapsLength(gapModel) + getMsaRow(rowNumber)->sequence.length());
    getMsaRow(rowNumber)->setGapModel(gapModel);
}

void MultipleSequenceAlignmentData::setSequenceId(int rowIndex, const U2DataId &sequenceId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getNumRows(), "Invalid row index", );
    getMsaRow(rowIndex)->setSequenceId(sequenceId);
}

const MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::getMsaRow(const QString &name) const {
    return getRow(name).dynamicCast<const MultipleSequenceAlignmentRow>();
}

MultipleAlignment MultipleSequenceAlignmentData::getCopy() const {
    return getExplicitCopy();
}

MultipleSequenceAlignment MultipleSequenceAlignmentData::getExplicitCopy() const {
    return MultipleSequenceAlignment(new MultipleSequenceAlignmentData(*this));
}

void MultipleSequenceAlignmentData::copy(const MultipleAlignmentData &other) {
    try {
        copy(dynamic_cast<const MultipleSequenceAlignmentData &>(other));
    } catch (std::bad_cast) {
        FAIL("Can't cast MultipleAlignmentData to MultipleSequenceAlignmentData", );
    }
}

void MultipleSequenceAlignmentData::copy(const MultipleSequenceAlignmentData &other) {
    clear();

    alphabet = other.alphabet;
    length = other.length;
    info = other.info;

    for (int i = 0; i < other.rows.size(); i++) {
        const MultipleSequenceAlignmentRow row = createRow(other.rows[i]);
        addRowPrivate(row, other.length, i);
    }
}

MultipleAlignmentRow MultipleSequenceAlignmentData::getEmptyRow() const {
    return MultipleSequenceAlignmentRow();
}

} // namespace U2
