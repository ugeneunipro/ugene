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

}  // namespace U2
