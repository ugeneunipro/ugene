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

#include "MultipleChromatogramAlignmentRow.h"

#include <QList>

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNAChromatogram.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "MultipleChromatogramAlignment.h"

namespace U2 {

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow()
    : MultipleAlignmentRow(new MultipleChromatogramAlignmentRowData) {
}

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow(const MultipleAlignmentRow& maRow)
    : MultipleAlignmentRow(maRow) {
    SAFE_POINT(maRowData.dynamicCast<MultipleChromatogramAlignmentRowData>() != nullptr, "Can't cast MultipleAlignmentRow to MultipleChromatogramAlignmentRow", );
}

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow(MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRow(new MultipleChromatogramAlignmentRowData(mcaData)) {
}

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow(MultipleChromatogramAlignmentRowData* mcaRowData)
    : MultipleAlignmentRow(mcaRowData) {
}

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow(const U2MsaRow& rowInDb,
                                                                   const DNAChromatogram& chromatogram,
                                                                   const DNASequence& sequence,
                                                                   const QVector<U2MsaGap>& gaps,
                                                                   MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRow(new MultipleChromatogramAlignmentRowData(rowInDb, chromatogram, sequence, gaps, mcaData)) {
}

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow(const U2MsaRow& rowInDb,
                                                                   const QString& rowName,
                                                                   const DNAChromatogram& chromatogram,
                                                                   const QByteArray& rawData,
                                                                   MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRow(new MultipleChromatogramAlignmentRowData(rowInDb, rowName, chromatogram, rawData, mcaData)) {
}

MultipleChromatogramAlignmentRow::MultipleChromatogramAlignmentRow(const MultipleChromatogramAlignmentRow& row, MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRow(new MultipleChromatogramAlignmentRowData(row, mcaData)) {
}

MultipleChromatogramAlignmentRowData* MultipleChromatogramAlignmentRow::data() const {
    return getMcaRowData().data();
}

MultipleChromatogramAlignmentRowData& MultipleChromatogramAlignmentRow::operator*() {
    return *getMcaRowData();
}

const MultipleChromatogramAlignmentRowData& MultipleChromatogramAlignmentRow::operator*() const {
    return *getMcaRowData();
}

MultipleChromatogramAlignmentRowData* MultipleChromatogramAlignmentRow::operator->() {
    return getMcaRowData().data();
}

const MultipleChromatogramAlignmentRowData* MultipleChromatogramAlignmentRow::operator->() const {
    return getMcaRowData().data();
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentRow::clone() const {
    return getMcaRowData()->getExplicitCopy();
}

QSharedPointer<MultipleChromatogramAlignmentRowData> MultipleChromatogramAlignmentRow::getMcaRowData() const {
    return maRowData.dynamicCast<MultipleChromatogramAlignmentRowData>();
}

MultipleChromatogramAlignmentRowData::MultipleChromatogramAlignmentRowData(MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MCA),
      alignment(mcaData) {
    removeTrailingGaps();
}

MultipleChromatogramAlignmentRowData::MultipleChromatogramAlignmentRowData(const U2MsaRow& rowInDb,
                                                                           const DNAChromatogram& _chromatogram,
                                                                           const DNASequence& sequence,
                                                                           const QVector<U2MsaGap>& gaps,
                                                                           MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MCA, sequence, gaps),
      alignment(mcaData) {
    initialRowInDb = rowInDb;
    chromatogram = _chromatogram;
    SAFE_POINT(alignment != nullptr, "Parent MultipleChromatogramAlignmentData are NULL", );
    removeTrailingGaps();
}

MultipleChromatogramAlignmentRowData::MultipleChromatogramAlignmentRowData(const U2MsaRow& rowInDb,
                                                                           const QString& rowName,
                                                                           const DNAChromatogram& _chromatogram,
                                                                           const QByteArray& rawData,
                                                                           MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MCA),
      alignment(mcaData) {
    chromatogram = _chromatogram;
    initialRowInDb = rowInDb;
    QByteArray sequenceData;
    QVector<U2MsaGap> gapModel;
    MaDbiUtils::splitBytesToCharsAndGaps(rawData, sequenceData, gapModel);
    sequence = DNASequence(rowName, sequenceData);
    setGapModel(gapModel);
}

MultipleChromatogramAlignmentRowData::MultipleChromatogramAlignmentRowData(const MultipleChromatogramAlignmentRow& row, MultipleChromatogramAlignmentData* mcaData)
    : MultipleAlignmentRowData(MultipleAlignmentDataType::MCA, row->sequence, row->gaps),
      alignment(mcaData) {
    additionalInfo = row->additionalInfo;
    initialRowInDb = row->initialRowInDb;
    chromatogram = row->chromatogram;
    SAFE_POINT(alignment != nullptr, "Parent MultipleChromatogramAlignmentData are NULL", );
}

bool MultipleChromatogramAlignmentRowData::isDefault() const {
    static const MultipleChromatogramAlignmentRowData defaultRow;
    return isEqual(defaultRow);
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentRowData::mid(int pos, int count, U2OpStatus& os) const {
    MultipleChromatogramAlignmentRow row = getExplicitCopy();
    row->crop(os, pos, count);
    return row;
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentRowData::getExplicitCopy() const {
    return MultipleChromatogramAlignmentRow(new MultipleChromatogramAlignmentRowData(*this));
}

void MultipleChromatogramAlignmentRowData::setParentAlignment(const MultipleChromatogramAlignment& msa) {
    setParentAlignment(msa.data());
}

void MultipleChromatogramAlignmentRowData::setParentAlignment(MultipleChromatogramAlignmentData* mcaData) {
    alignment = mcaData;
}

MultipleAlignmentData* MultipleChromatogramAlignmentRowData::getMultipleAlignmentData() const {
    return alignment;
}

}  // namespace U2
