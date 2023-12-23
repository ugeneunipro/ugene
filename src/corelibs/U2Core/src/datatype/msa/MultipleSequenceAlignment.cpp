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
}  // namespace U2
