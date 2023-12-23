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
#include <U2Core/MsaDbiUtils.h>
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

MultipleChromatogramAlignment::MultipleChromatogramAlignment(const QString& name, const DNAAlphabet* alphabet, const QList<MultipleAlignmentRow>& rows)
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

MultipleChromatogramAlignmentData::MultipleChromatogramAlignmentData(const QString& name, const DNAAlphabet* alphabet, const QList<MultipleAlignmentRow>& rows)
    : MultipleAlignmentData(MultipleAlignmentDataType::MCA, name, alphabet, rows.toVector()) {
}

MultipleChromatogramAlignmentData::MultipleChromatogramAlignmentData(const MultipleChromatogramAlignmentData& mcaData)
    : MultipleAlignmentData(MultipleAlignmentDataType::MCA) {
    copy(mcaData);
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
    foreach (const MultipleAlignmentRow& row, rows) {
        MultipleAlignmentRow mRow = row->mid(start, len, os);
        mRow->setParentAlignment(res.data());
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
        getRow(i)->append(mcaData.getRow(i), (int)length, os);
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

MultipleAlignmentRow MultipleChromatogramAlignmentData::createRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes) {
    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MaDbiUtils::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(name, newSequenceBytes);

    U2MsaRow row;
    return {row, chromatogram, newSequence, newGapsModel, this};
}

MultipleAlignmentRow MultipleChromatogramAlignmentData::createRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    QString errorDescr = "Failed to create a multiple alignment row";
    if (sequence.constSequence().indexOf(U2Msa::GAP_CHAR) != -1) {
        coreLog.trace("Attempted to create an alignment row from a sequence with gaps");
        os.setError(errorDescr);
        return {MultipleAlignmentDataType::MCA};
    }

    int length = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos > length || !gap.isValid()) {
            coreLog.trace("Incorrect gap model was passed to MultipleChromatogramAlignmentData::createRow");
            os.setError(errorDescr);
            return {MultipleAlignmentDataType::MCA};
        }
        length += gap.length;
    }

    return {rowInDb, chromatogram, sequence, gaps, this};
}

MultipleAlignmentRow MultipleChromatogramAlignmentData::createRow(const MultipleAlignmentRow& row) {
    return {row, this};
}

void MultipleChromatogramAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes) {
    MultipleAlignmentRow newRow = createRow(name, chromatogram, bytes);
    addRowPrivate(newRow, bytes.size(), -1);
}

void MultipleChromatogramAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes, int rowIndex) {
    MultipleAlignmentRow newRow = createRow(name, chromatogram, bytes);
    addRowPrivate(newRow, bytes.size(), rowIndex);
}

void MultipleChromatogramAlignmentData::addRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, U2OpStatus& os) {
    MultipleAlignmentRow newRow = createRow(rowInDb, chromatogram, sequence, rowInDb.gaps, os);
    CHECK_OP(os, );
    addRowPrivate(newRow, rowInDb.length, -1);
}

void MultipleChromatogramAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    U2MsaRow row;
    MultipleAlignmentRow newRow = createRow(row, chromatogram, sequence, gaps, os);
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

void MultipleChromatogramAlignmentData::appendChars(int row, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleChromatogramAlignmentData::appendChars").arg(row), );

    MultipleAlignmentRow appendedRow = createRow("", DNAChromatogram(), QByteArray(str, len));

    qint64 rowLength = getRow(row)->getRowLength();

    U2OpStatus2Log os;
    getRow(row)->append(appendedRow, (int)rowLength, os);
    CHECK_OP(os, );

    length = qMax(length, rowLength + len);
}

void MultipleChromatogramAlignmentData::appendChars(int row, qint64 afterPos, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleChromatogramAlignmentData::appendChars").arg(row), );

    MultipleAlignmentRow appendedRow = createRow("", DNAChromatogram(), QByteArray(str, len));

    U2OpStatus2Log os;
    getRow(row)->append(appendedRow, (int)afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + len);
}


void MultipleChromatogramAlignmentData::replaceChars(int row, char origChar, char resultChar) {
    SAFE_POINT(row >= 0 && row < getRowCount(), QString("Incorrect row index '%1' in MultipleChromatogramAlignmentData::replaceChars").arg(row), );

    if (origChar == resultChar) {
        return;
    }

    U2OpStatus2Log os;
    getRow(row)->replaceChars(origChar, resultChar, os);
}

void MultipleChromatogramAlignmentData::setSequenceId(int rowIndex, const U2DataId& sequenceId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getRowCount(), "Invalid row index", );
    getRow(rowIndex)->setSequenceId(sequenceId);
}

MultipleAlignment MultipleChromatogramAlignmentData::getCopy() const {
    return getExplicitCopy();
}

MultipleChromatogramAlignment MultipleChromatogramAlignmentData::getExplicitCopy() const {
    return {new MultipleChromatogramAlignmentData(*this)};
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
        MultipleAlignmentRow row = createRow(other.rows[i]);
        addRowPrivate(row, other.length, i);
    }
}

MultipleAlignmentRow MultipleChromatogramAlignmentData::getEmptyRow() const {
    return {MultipleAlignmentDataType::MCA};
}

}  // namespace U2
