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

#include <QList>

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNAChromatogram.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleAlignmentRowInfo.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "MultipleChromatogramAlignment.h"
#include "MultipleChromatogramAlignmentRow.h"

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
      alignment(mcaData),
      additionalInfo(row->additionalInfo) {
    initialRowInDb = row->initialRowInDb;
    chromatogram = row->chromatogram;
    SAFE_POINT(alignment != nullptr, "Parent MultipleChromatogramAlignmentData are NULL", );
}

void MultipleChromatogramAlignmentRowData::insertGaps(int position, int count, U2OpStatus& os) {
    MsaRowUtils::insertGaps(os, gaps, getRowLengthWithoutTrailing(), position, count);
}

void MultipleChromatogramAlignmentRowData::removeChars(int pos, int count, U2OpStatus& os) {
    if (pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MultipleChromatogramAlignmentRowData::removeChars, "
                              "pos '%1', count '%2'")
                          .arg(pos)
                          .arg(count));
        os.setError("Can't remove chars from a row");
        return;
    }

    if (pos >= getRowLengthWithoutTrailing()) {
        return;
    }

    if (pos < getRowLengthWithoutTrailing()) {
        int startPosInSeq = -1;
        int endPosInSeq = -1;
        getStartAndEndSequencePositions(pos, count, startPosInSeq, endPosInSeq);

        // Remove inside a gap
        if ((startPosInSeq < endPosInSeq) && (-1 != startPosInSeq) && (-1 != endPosInSeq)) {
            DNASequenceUtils::removeChars(sequence, startPosInSeq, endPosInSeq, os);
            CHECK_OP(os, );
            chromatogram.baseCalls.remove(startPosInSeq, endPosInSeq - startPosInSeq);
        }
    }

    // Remove gaps from the gaps model
    removeGapsFromGapModel(os, pos, count);

    removeTrailingGaps();
    mergeConsecutiveGaps();
}

char MultipleChromatogramAlignmentRowData::charAt(qint64 position) const {
    return MsaRowUtils::charAt(sequence.seq, gaps, position);
}

bool MultipleChromatogramAlignmentRowData::isGap(qint64 position) const {
    return MsaRowUtils::isGap(sequence.length(), gaps, position);
}

bool MultipleChromatogramAlignmentRowData::isLeadingOrTrailingGap(qint64 position) const {
    return MsaRowUtils::isLeadingOrTrailingGap(sequence.length(), gaps, position);
}

int MultipleChromatogramAlignmentRowData::getUngappedPosition(int pos) const {
    return MsaRowUtils::getUngappedPosition(gaps, sequence.length(), pos);
}

qint64 MultipleChromatogramAlignmentRowData::getBaseCount(qint64 before) const {
    const int rowLength = MsaRowUtils::getRowLength(sequence.seq, gaps);
    const int trimmedRowPos = before < rowLength ? before : rowLength;
    return MsaRowUtils::getUngappedPosition(gaps, sequence.length(), trimmedRowPos, true);
}

const QMap<DNAChromatogram::Trace, QVector<ushort> DNAChromatogram::*> PEAKS =
    {{DNAChromatogram::Trace::Trace_A, &DNAChromatogram::A},
     {DNAChromatogram::Trace::Trace_C, &DNAChromatogram::C},
     {DNAChromatogram::Trace::Trace_G, &DNAChromatogram::G},
     {DNAChromatogram::Trace::Trace_T, &DNAChromatogram::T}};

QPair<DNAChromatogram::ChromatogramTraceAndValue, DNAChromatogram::ChromatogramTraceAndValue>
    MultipleChromatogramAlignmentRowData::getTwoHighestPeaks(qint64 position, bool& hasTwoPeaks) const {
    hasTwoPeaks = true;
    int previousBaseCall = chromatogram.baseCalls[position != 0 ? position - 1 : position];
    int baseCall = chromatogram.baseCalls[position];
    int nextBaseCall = chromatogram.baseCalls[position != (chromatogram.baseCalls.size() - 1) ? position + 1 : position];
    QList<DNAChromatogram::ChromatogramTraceAndValue> peaks;

    auto peaksKeys = PEAKS.keys();
    for (auto peak : qAsConst(peaksKeys)) {
        const QVector<ushort>& chromatogramBaseCallVector = chromatogram.*PEAKS.value(peak);
        auto peakValue = chromatogramBaseCallVector[baseCall];
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
        return {{DNAChromatogram::Trace::Trace_A, 0}, {DNAChromatogram::Trace::Trace_C, 0}};
    }

    std::sort(peaks.begin(),
              peaks.end(),
              [](const auto& first, const auto& second) {
                  return first.value > second.value;
              });
    return {peaks[0], peaks[1]};
}

bool MultipleChromatogramAlignmentRowData::isEqual(const MultipleAlignmentRowData& other) const {
    CHECK(other.type == MultipleAlignmentDataType::MCA, false);
    auto mcaRow = dynamic_cast<const MultipleChromatogramAlignmentRowData*>(&other);
    SAFE_POINT(mcaRow != nullptr, "Not an MCA row!", false);
    return isEqual(*mcaRow);
}

bool MultipleChromatogramAlignmentRowData::isEqual(const MultipleChromatogramAlignmentRowData& other) const {
    CHECK(this != &other, true);
    CHECK(getName() == other.getName(), false);
    return isEqualCore(other);
}

bool MultipleChromatogramAlignmentRowData::isEqualCore(const MultipleAlignmentRowData& other) const {
    if (other.type == MultipleAlignmentDataType::MCA) {
        auto mcaRow = dynamic_cast<const MultipleChromatogramAlignmentRowData*>(&other);
        SAFE_POINT(mcaRow != nullptr, "Not an MCA row!", false);
        CHECK(ChromatogramUtils::areEqual(chromatogram, mcaRow->chromatogram), false);
    }
    return MultipleAlignmentRowData::isEqualCore(other);
}

bool MultipleChromatogramAlignmentRowData::isDefault() const {
    static const MultipleChromatogramAlignmentRowData defaultRow;
    return isEqual(defaultRow);
}

void MultipleChromatogramAlignmentRowData::crop(U2OpStatus& os, qint64 startPosition, qint64 count) {
    if (startPosition < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed to MultipleChromatogramAlignmentRowData::crop, "
                              "startPos '%1', length '%2', row length '%3'")
                          .arg(startPosition)
                          .arg(count)
                          .arg(getRowLength()));
        os.setError("Can't crop a row!");
        return;
    }

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

    ChromatogramUtils::crop(chromatogram, startPosition, count);

    if (startPosition + count < initialRowLength) {
        removeGapsFromGapModel(os, startPosition + count, initialRowLength - startPosition - count);
    }

    if (startPosition > 0) {
        removeGapsFromGapModel(os, 0, startPosition);
    }
    removeTrailingGaps();
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentRowData::mid(int pos, int count, U2OpStatus& os) const {
    MultipleChromatogramAlignmentRow row = getExplicitCopy();
    row->crop(os, pos, count);
    return row;
}

void MultipleChromatogramAlignmentRowData::toUpperCase() {
    DNASequenceUtils::toUpperCase(sequence);
}

void MultipleChromatogramAlignmentRowData::replaceChars(char origChar, char resultChar, U2OpStatus& os) {
    if (U2Msa::GAP_CHAR == origChar) {
        coreLog.trace("The original char can't be a gap in MultipleChromatogramAlignmentRowData::replaceChars");
        os.setError("Failed to replace chars in an alignment row");
        return;
    }

    if (U2Msa::GAP_CHAR == resultChar) {
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
        for (int i = 0; i < gapsIndexes.size(); ++i) {
            int index = gapsIndexes[i];
            U2MsaGap gap(index, 1);
            newGapsModel.append(gap);
        }
        std::sort(newGapsModel.begin(), newGapsModel.end(), U2MsaGap::lessThan);

        // Replace the gaps model with the new one
        gaps = newGapsModel;
        mergeConsecutiveGaps();

        foreach (int index, gapsIndexes) {
            chromatogram.baseCalls.removeAt(index);
        }
        chromatogram.seqLength -= gapsIndexes.size();
    } else {
        // Just replace all occurrences of 'origChar' by 'resultChar'
        sequence.seq.replace(origChar, resultChar);
    }
}

MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentRowData::getExplicitCopy() const {
    return MultipleChromatogramAlignmentRow(new MultipleChromatogramAlignmentRowData(*this));
}

void MultipleChromatogramAlignmentRowData::setAdditionalInfo(const QVariantMap& newAdditionalInfo) {
    additionalInfo = newAdditionalInfo;
}

QVariantMap MultipleChromatogramAlignmentRowData::getAdditionalInfo() const {
    return additionalInfo;
}

McaRowMemoryData MultipleChromatogramAlignmentRowData::getRowMemoryData() const {
    McaRowMemoryData mcaRowMemoryData;
    mcaRowMemoryData.chromatogram = chromatogram;
    mcaRowMemoryData.gapModel = gaps;
    mcaRowMemoryData.sequence = sequence;
    mcaRowMemoryData.rowLength = getRowLengthWithoutTrailing();
    mcaRowMemoryData.additionalInfo = additionalInfo;
    return mcaRowMemoryData;
}

void MultipleChromatogramAlignmentRowData::reverse() {
    sequence = DNASequenceUtils::reverse(sequence);
    chromatogram = ChromatogramUtils::reverse(chromatogram);
    gaps = MsaRowUtils::reverseGapModel(gaps, getRowLengthWithoutTrailing());
    MultipleAlignmentRowInfo::setReversed(additionalInfo, !isReversed());
}

void MultipleChromatogramAlignmentRowData::complement() {
    sequence = DNASequenceUtils::complement(sequence);
    chromatogram = ChromatogramUtils::complement(chromatogram);
    MultipleAlignmentRowInfo::setComplemented(additionalInfo, !isComplemented());
}

void MultipleChromatogramAlignmentRowData::reverseComplement() {
    reverse();
    complement();
}

bool MultipleChromatogramAlignmentRowData::isReversed() const {
    return MultipleAlignmentRowInfo::getReversed(additionalInfo);
}

bool MultipleChromatogramAlignmentRowData::isComplemented() const {
    return MultipleAlignmentRowInfo::getComplemented(additionalInfo);
}

void MultipleChromatogramAlignmentRowData::getStartAndEndSequencePositions(int pos, int count, int& startPosInSeq, int& endPosInSeq) {
    int rowLengthWithoutTrailingGap = getRowLengthWithoutTrailing();
    SAFE_POINT(pos < rowLengthWithoutTrailingGap,
               QString("Incorrect position '%1' in MultipleChromatogramAlignmentRowData::getStartAndEndSequencePosition, "
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
    } else {
        if (U2Msa::GAP_CHAR == charAt(endRegionPos)) {
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
}

void MultipleChromatogramAlignmentRowData::removeGapsFromGapModel(U2OpStatus& os, int pos, int count) {
    MsaRowUtils::removeGaps(os, gaps, getRowLengthWithoutTrailing(), pos, count);
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
