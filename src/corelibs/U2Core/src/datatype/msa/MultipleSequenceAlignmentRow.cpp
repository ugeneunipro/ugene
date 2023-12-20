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

MultipleSequenceAlignmentRow MultipleSequenceAlignmentRowData::mid(int pos, int count, U2OpStatus& os) const {
    MultipleSequenceAlignmentRow row = getExplicitCopy();
    row->crop(os, pos, count);
    return row;
}

MultipleSequenceAlignmentRow MultipleSequenceAlignmentRowData::getExplicitCopy() const {
    return MultipleSequenceAlignmentRow(new MultipleSequenceAlignmentRowData(*this));
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

bool MultipleSequenceAlignmentRowData::isDefault() const {
    static const MultipleSequenceAlignmentRowData defaultRow;
    return isEqual(defaultRow);
}

}  // namespace U2
