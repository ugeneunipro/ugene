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

#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include "MaStateCheck.h"
#include "MultipleSequenceAlignment.h"

namespace U2 {

MultipleSequenceAlignment::MultipleSequenceAlignment(const QString& name, const DNAAlphabet* alphabet, const QList<MultipleAlignmentRow>& rows)
    : MultipleAlignment(new MultipleSequenceAlignmentData(name, alphabet, rows)) {
}

MultipleSequenceAlignment::MultipleSequenceAlignment(const MultipleAlignment& ma)
    : MultipleAlignment(ma) {
    SAFE_POINT(maData.dynamicCast<MultipleSequenceAlignmentData>() != nullptr, "Can't cast MultipleAlignment to MultipleSequenceAlignment", );
}

MultipleSequenceAlignment::MultipleSequenceAlignment(MultipleSequenceAlignmentData* msaData)
    : MultipleAlignment(msaData) {
}

MultipleSequenceAlignmentData* MultipleSequenceAlignment::data() const {
    return getMsaData().data();
}

MultipleSequenceAlignmentData& MultipleSequenceAlignment::operator*() {
    return *getMsaData();
}

const MultipleSequenceAlignmentData& MultipleSequenceAlignment::operator*() const {
    return *getMsaData();
}

MultipleSequenceAlignmentData* MultipleSequenceAlignment::operator->() {
    return getMsaData().data();
}

const MultipleSequenceAlignmentData* MultipleSequenceAlignment::operator->() const {
    return getMsaData().data();
}

MultipleSequenceAlignment MultipleSequenceAlignment::clone() const {
    return getMsaData()->getExplicitCopy();
}

QSharedPointer<MultipleSequenceAlignmentData> MultipleSequenceAlignment::getMsaData() const {
    return maData.dynamicCast<MultipleSequenceAlignmentData>();
}

static QVector<MultipleAlignmentRow> convertToMaRows(const QList<MultipleAlignmentRow>& msaRows) {
    QVector<MultipleAlignmentRow> maRows;
    foreach (const MultipleAlignmentRow& msaRow, msaRows) {
        maRows << msaRow;
    }
    return maRows;
}

MultipleSequenceAlignmentData::MultipleSequenceAlignmentData(const QString& name,
                                                             const DNAAlphabet* alphabet,
                                                             const QList<MultipleAlignmentRow>& rows)
    : MultipleAlignmentData(MultipleAlignmentDataType::MSA, name, alphabet, convertToMaRows(rows)) {
}

MultipleSequenceAlignmentData::MultipleSequenceAlignmentData(const MultipleSequenceAlignmentData& msaData)
    : MultipleAlignmentData(MultipleAlignmentDataType::MSA) {
    copy(msaData);
}

MultipleSequenceAlignmentData& MultipleSequenceAlignmentData::operator=(const MultipleSequenceAlignment& msa) {
    return *this = *msa;
}

MultipleSequenceAlignmentData& MultipleSequenceAlignmentData::operator=(const MultipleSequenceAlignmentData& msaData) {
    copy(msaData);
    return *this;
}

bool MultipleSequenceAlignmentData::hasEmptyGapModel() const {
    foreach (const MultipleAlignmentRow& row, rows) {
        if (!row->getGaps().isEmpty()) {
            return false;
        }
    }
    return true;
}

bool MultipleSequenceAlignmentData::hasEqualLength() const {
    const int defaultSequenceLength = -1;
    int sequenceLength = defaultSequenceLength;
    for (int i = 0, n = rows.size(); i < n; ++i) {
        if (defaultSequenceLength != sequenceLength && sequenceLength != getRow(i)->getUngappedLength()) {
            return false;
        } else {
            sequenceLength = getRow(i)->getUngappedLength();
        }
    }
    return true;
}

MultipleSequenceAlignment MultipleSequenceAlignmentData::mid(int start, int len) const {
    SAFE_POINT(start >= 0 && start + len <= length,
               QString("Incorrect parameters were passed to MultipleSequenceAlignmentData::mid: "
                       "start '%1', len '%2', the alignment length is '%3'")
                   .arg(start)
                   .arg(len)
                   .arg(length),
               MultipleSequenceAlignment());

    MultipleSequenceAlignment res(getName(), alphabet);
    MaStateCheck check(res.data());
    Q_UNUSED(check);

    U2OpStatus2Log os;
    foreach (const MultipleAlignmentRow& row, rows) {
        MultipleAlignmentRow mRow = row->mid(start, len, os);
        mRow->setParentAlignment(res.data());
        res->rows << mRow;
    }
    res->length = len;
    return res;
}

MultipleSequenceAlignmentData& MultipleSequenceAlignmentData::operator+=(const MultipleSequenceAlignmentData& msaData) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    SAFE_POINT(msaData.alphabet == alphabet, "Different alphabets in MultipleSequenceAlignmentData::operator+=", *this);

    int nSeq = getRowCount();
    SAFE_POINT(msaData.getRowCount() == nSeq, "Different number of rows in MultipleSequenceAlignmentData::operator+=", *this);

    U2OpStatus2Log os;
    for (int i = 0; i < nSeq; i++) {
        getRow(i)->append(msaData.getRow(i), length, os);
    }

    length += msaData.length;
    return *this;
}

bool MultipleSequenceAlignmentData::operator==(const MultipleSequenceAlignmentData& other) const {
    return isEqual(other);
}

bool MultipleSequenceAlignmentData::operator!=(const MultipleSequenceAlignmentData& other) const {
    return !isEqual(other);
}

MultipleAlignmentRow MultipleSequenceAlignmentData::createRow(const QString& name, const QByteArray& bytes) {
    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MaDbiUtils::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(name, newSequenceBytes);

    U2MsaRow row;
    return {row, newSequence, newGapsModel, this};
}

MultipleAlignmentRow MultipleSequenceAlignmentData::createRow(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    QString errorDescr = "Failed to create a multiple alignment row";
    if (-1 != sequence.constSequence().indexOf(U2Msa::GAP_CHAR)) {
        coreLog.trace("Attempted to create an alignment row from a sequence with gaps");
        os.setError(errorDescr);
        return {MultipleAlignmentDataType::MSA};
    }

    int length = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos > length || !gap.isValid()) {
            coreLog.trace("Incorrect gap model was passed to MultipleSequenceAlignmentData::createRow");
            os.setError(errorDescr);
            return {MultipleAlignmentDataType::MSA};
        }
        length += gap.length;
    }

    return {rowInDb, sequence, gaps, this};
}

MultipleAlignmentRow MultipleSequenceAlignmentData::createRow(const MultipleAlignmentRow& row) {
    return {row, this};
}

void MultipleSequenceAlignmentData::addRow(const QString& name, const QByteArray& bytes) {
    MultipleAlignmentRow newRow = createRow(name, bytes);
    addRowPrivate(newRow, bytes.size(), -1);
}

void MultipleSequenceAlignmentData::addRow(const QString& name, const QByteArray& bytes, int rowIndex) {
    MultipleAlignmentRow newRow = createRow(name, bytes);
    addRowPrivate(newRow, bytes.size(), rowIndex);
}

void MultipleSequenceAlignmentData::addRow(const U2MsaRow& rowInDb, const DNASequence& sequence, U2OpStatus& os) {
    MultipleAlignmentRow newRow = createRow(rowInDb, sequence, rowInDb.gaps, os);
    CHECK_OP(os, );
    addRowPrivate(newRow, rowInDb.length, -1);
}

void MultipleSequenceAlignmentData::addRow(const QString& name, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    U2MsaRow row;
    MultipleAlignmentRow newRow = createRow(row, sequence, gaps, os);
    CHECK_OP(os, );

    int len = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        len += gap.length;
    }

    newRow->setName(name);
    addRowPrivate(newRow, len, -1);
}

void MultipleSequenceAlignmentData::appendChars(int row, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendChars").arg(row), );

    MultipleAlignmentRow appendedRow = createRow("", QByteArray(str, len));

    qint64 rowLength = getRow(row)->getRowLength();

    U2OpStatus2Log os;
    getRow(row)->append(appendedRow, (int)rowLength, os);
    CHECK_OP(os, );

    length = qMax(length, rowLength + len);
}

void MultipleSequenceAlignmentData::appendChars(int row, qint64 afterPos, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendChars").arg(row), );

    MultipleAlignmentRow appendedRow = createRow("", QByteArray(str, len));

    U2OpStatus2Log os;
    getRow(row)->append(appendedRow, (int)afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + len);
}

void MultipleSequenceAlignmentData::appendRow(int rowNumber, qint64 afterPos, const MultipleAlignmentRow& row, U2OpStatus& os) {
    SAFE_POINT(0 <= rowNumber && rowNumber < getRowCount(),
               QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendRow").arg(rowNumber), );

    getRow(rowNumber)->append(row, (int)afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + row->getRowLength());
}

void MultipleSequenceAlignmentData::replaceChars(int row, char origChar, char resultChar) {
    SAFE_POINT(row >= 0 && row < getRowCount(), QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::replaceChars").arg(row), );

    if (origChar == resultChar) {
        return;
    }

    U2OpStatus2Log os;
    getRow(row)->replaceChars(origChar, resultChar, os);
}

void MultipleSequenceAlignmentData::setRowGapModel(int rowNumber, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getRowCount(), "Invalid row index", );
    length = qMax(length, (qint64)MsaRowUtils::getGapsLength(gapModel) + getRow(rowNumber)->sequence.length());
    getRow(rowNumber)->setGapModel(gapModel);
}

void MultipleSequenceAlignmentData::setSequenceId(int rowIndex, const U2DataId& sequenceId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getRowCount(), "Invalid row index", );
    getRow(rowIndex)->setSequenceId(sequenceId);
}

MultipleAlignment MultipleSequenceAlignmentData::getCopy() const {
    return getExplicitCopy();
}

MultipleSequenceAlignment MultipleSequenceAlignmentData::getExplicitCopy() const {
    return {new MultipleSequenceAlignmentData(*this)};
}

void MultipleSequenceAlignmentData::copy(const MultipleAlignmentData& other) {
    try {
        copy(dynamic_cast<const MultipleSequenceAlignmentData&>(other));
    } catch (std::bad_cast&) {
        FAIL("Can't cast MultipleAlignmentData to MultipleSequenceAlignmentData", );
    }
}

void MultipleSequenceAlignmentData::copy(const MultipleSequenceAlignmentData& other) {
    clear();

    alphabet = other.alphabet;
    length = other.length;
    info = other.info;

    for (int i = 0; i < other.rows.size(); i++) {
        MultipleAlignmentRow row = createRow(other.rows[i]);
        addRowPrivate(row, other.length, i);
    }
}

MultipleAlignmentRow MultipleSequenceAlignmentData::getEmptyRow() const {
    return {MultipleAlignmentDataType::MSA};
}

}  // namespace U2
