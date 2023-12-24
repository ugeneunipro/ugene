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

#include "MultipleAlignment.h"

#include <QSet>

#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MaStateCheck.h"
#include "MultipleAlignmentInfo.h"

namespace U2 {

MultipleAlignment::MultipleAlignment(const MultipleAlignmentDataType& type, const QString& name, const DNAAlphabet* alphabet)
    : maData(new MultipleAlignmentData(type)) {
    if (!name.isEmpty()) {
        maData->setName(name);
    }
    if (alphabet != nullptr) {
        maData->setAlphabet(alphabet);
    }
}

MultipleAlignment::MultipleAlignment(MultipleAlignmentData* _maData)
    : maData(_maData) {
}

MultipleAlignmentData* MultipleAlignment::data() const {
    return maData.data();
}

MultipleAlignmentData& MultipleAlignment::operator*() {
    return *maData;
}

const MultipleAlignmentData& MultipleAlignment::operator*() const {
    return *maData;
}

MultipleAlignmentData* MultipleAlignment::operator->() {
    return maData.data();
}

const MultipleAlignmentData* MultipleAlignment::operator->() const {
    return maData.data();
}

MultipleAlignmentData::MultipleAlignmentData(const MultipleAlignmentDataType& _type,
                                             const QString& name,
                                             const DNAAlphabet* _alphabet,
                                             const QVector<MultipleAlignmentRow>& _rows)
    : type(_type), alphabet(_alphabet), rows(_rows) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    SAFE_POINT(alphabet == nullptr || !name.isEmpty(), "Incorrect parameters in MultipleAlignmentData ctor", );  // TODO: check the condition, it is strange

    setName(name);
    for (auto& row : qAsConst(rows)) {
        length = qMax(length, (qint64)row->getRowLengthWithoutTrailing());  // TODO: implement or replace the method for row length
    }
}

MultipleAlignmentData::MultipleAlignmentData(const MultipleAlignmentData& data)
    : type(data.type) {
    copyFrom(data);
}

void MultipleAlignmentData::copyFrom(const MultipleAlignmentData& other) {
    SAFE_POINT(type == other.type, "Incompatible types", );
    clear();
    alphabet = other.alphabet;
    length = other.length;
    info = other.info;

    for (int i = 0; i < other.rows.size(); i++) {
        MultipleAlignmentRow row = createRow(other.rows[i]);
        addRowPrivate(row, other.length, i);
    }
}

void MultipleAlignmentData::clear() {
    MaStateCheck check(this);
    Q_UNUSED(check);
    rows.clear();
    length = 0;
}

bool MultipleAlignmentData::trim(bool removeLeadingGaps) {
    MaStateCheck check(this);

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
                getRow(i)->removeChars(0, leadingGapColumnsNum, os);
                CHECK_OP(os, true);
                result = true;
            }
        }
    }

    // Verify right side of the alignment (trailing gaps and rows' lengths)
    qint64 newLength = 0;
    foreach (const MultipleAlignmentRow& row, rows) {
        if (newLength == 0) {
            newLength = row->getRowLengthWithoutTrailing();
        } else {
            newLength = qMax((qint64)row->getRowLengthWithoutTrailing(), newLength);
        }
    }

    if (newLength != length) {
        length = newLength;
        result = true;
    }

    return result;
}

bool MultipleAlignmentData::simplify() {
    MaStateCheck check(this);
    Q_UNUSED(check);

    int newLen = 0;
    bool changed = false;
    for (int i = 0, n = getRowCount(); i < n; i++) {
        changed |= getRow(i)->simplify();
        newLen = qMax(newLen, getRow(i)->getCoreEnd());
    }

    if (!changed) {
        assert(length == newLen);
        return false;
    }
    length = newLen;
    return true;
}

QString MultipleAlignmentData::getName() const {
    return MultipleAlignmentInfo::getName(info);
}

void MultipleAlignmentData::setName(const QString& newName) {
    MultipleAlignmentInfo::setName(info, newName);
}

const DNAAlphabet* MultipleAlignmentData::getAlphabet() const {
    return alphabet;
}

void MultipleAlignmentData::setAlphabet(const DNAAlphabet* newAlphabet) {
    SAFE_POINT(newAlphabet != nullptr, "Internal error: attempted to set NULL alphabet for the alignment", );
    alphabet = newAlphabet;
}

QVariantMap MultipleAlignmentData::getInfo() const {
    return info;
}

void MultipleAlignmentData::setInfo(const QVariantMap& newInfo) {
    info = newInfo;
}

bool MultipleAlignmentData::isEmpty() const {
    return getLength() == 0 || rows.isEmpty();
}

int MultipleAlignmentData::getLength() const {
    return (int)length;
}

void MultipleAlignmentData::setLength(int newLength) {
    SAFE_POINT(newLength >= 0, QString("Internal error: attempted to set length '%1' for an alignment").arg(newLength), );

    MaStateCheck check(this);
    Q_UNUSED(check);

    if (newLength >= length) {
        length = newLength;
        return;
    }

    U2OpStatus2Log os;
    for (int i = 0, n = getRowCount(); i < n; i++) {
        rows[i]->crop(os, 0, newLength);
        CHECK_OP(os, );
    }
    length = newLength;
}

int MultipleAlignmentData::getRowCount() const {
    return rows.size();
}

QList<QVector<U2MsaGap>> MultipleAlignmentData::getGapModel() const {
    QList<QVector<U2MsaGap>> gapModel;
    for (const MultipleAlignmentRow& row : qAsConst(rows)) {
        gapModel << row->getGaps();
    }
    return gapModel;
}

static bool isGreaterByName(const MultipleAlignmentRow& row1, const MultipleAlignmentRow& row2) {
    return QString::compare(row1->getName(), row2->getName(), Qt::CaseInsensitive) > 0;
}

static bool isLessByName(const MultipleAlignmentRow& row1, const MultipleAlignmentRow& row2) {
    return QString::compare(row1->getName(), row2->getName(), Qt::CaseInsensitive) < 0;
}

static bool isGreaterByLength(const MultipleAlignmentRow& row1, const MultipleAlignmentRow& row2) {
    return row1->getUngappedLength() > row2->getUngappedLength();
}

static bool isLessByLength(const MultipleAlignmentRow& row1, const MultipleAlignmentRow& row2) {
    return row1->getUngappedLength() < row2->getUngappedLength();
}

static bool isGreaterByLeadingGap(const MultipleAlignmentRow& row1, const MultipleAlignmentRow& row2) {
    return row1->getCoreStart() > row2->getCoreStart();
}

static bool isLessByLeadingGap(const MultipleAlignmentRow& row1, const MultipleAlignmentRow& row2) {
    return row1->getCoreStart() < row2->getCoreStart();
}

void MultipleAlignmentData::sortRows(const MultipleAlignment::SortType& sortType, MultipleAlignment::Order order, const U2Region& range) {
    U2Region allRowsRange = U2Region(0, rows.size());
    SAFE_POINT(range.intersect(allRowsRange) == range, "Sort range is out of bounds", )
    MaStateCheck check(this);
    Q_UNUSED(check);
    bool isAscending = order == MultipleAlignment::Ascending;
    U2Region sortingRange = range.isEmpty() ? allRowsRange : range;
    const auto& rangeStartIterator = rows.begin() + sortingRange.startPos;
    const auto& rangeEndIterator = rows.begin() + sortingRange.endPos();
    switch (sortType) {
        case MultipleAlignment::SortByName:
            std::stable_sort(rangeStartIterator, rangeEndIterator, isAscending ? isLessByName : isGreaterByName);
            break;
        case MultipleAlignment::SortByLength:
            std::stable_sort(rangeStartIterator, rangeEndIterator, isAscending ? isLessByLength : isGreaterByLength);
            break;
        case MultipleAlignment::SortByLeadingGap:
            std::stable_sort(rangeStartIterator, rangeEndIterator, isAscending ? isLessByLeadingGap : isGreaterByLeadingGap);
            break;
        default:
            FAIL("Unsupported sort type: " + QString::number(sortType), );
    }
}

MultipleAlignmentRow MultipleAlignmentData::getRow(int rowIndex) {
    static MultipleAlignmentRow emptyRow;
    int rowCount = rows.count();
    SAFE_POINT(rowIndex >= 0 && rowIndex < rowCount, "Internal error: unexpected row index was passed to MAlignment::getRow", emptyRow);
    return rows[rowIndex];
}

const MultipleAlignmentRow& MultipleAlignmentData::getRow(int rowIndex) const {
    static MultipleAlignmentRow emptyRow;
    int rowCount = rows.count();
    SAFE_POINT(rowIndex >= 0 && rowIndex < rowCount, "Internal error: unexpected row index was passed to MAlignment::getRow", emptyRow);
    return rows[rowIndex];
}

const MultipleAlignmentRow& MultipleAlignmentData::getRow(const QString& name) const {
    static MultipleAlignmentRow emptyRow;
    for (int i = 0; i < rows.count(); i++) {
        if (rows[i]->getName() == name) {
            return rows[i];
        }
    }
    FAIL("Internal error: row name passed to MultipleAlignmentData::getRow function not exists", emptyRow);
}

const QVector<MultipleAlignmentRow>& MultipleAlignmentData::getRows() const {
    return rows;
}

QList<qint64> MultipleAlignmentData::getRowsIds() const {
    QList<qint64> rowIds;
    foreach (const MultipleAlignmentRow& row, rows) {
        rowIds.append(row->getRowId());
    }
    return rowIds;
}

QList<qint64> MultipleAlignmentData::getRowIdsByRowIndexes(const QList<int>& rowIndexes) const {
    QList<qint64> rowIds;
    foreach (int rowIndex, rowIndexes) {
        bool isValidRowIndex = rowIndex >= 0 && rowIndex < rows.size();
        rowIds.append(isValidRowIndex ? rows[rowIndex]->getRowId() : -1);
    }
    return rowIds;
}

MultipleAlignmentRow MultipleAlignmentData::getRowByRowId(qint64 rowId, U2OpStatus& os) const {
    static MultipleAlignmentRow emptyRow;
    foreach (const MultipleAlignmentRow& row, rows) {
        if (row->getRowId() == rowId) {
            return row;
        }
    }
    os.setError("Failed to find a row in an alignment");
    return emptyRow;
}

char MultipleAlignmentData::charAt(int rowIndex, qint64 position) const {
    int rowCount = rows.count();
    return rowIndex >= 0 && rowIndex < rowCount
               ? rows[rowIndex]->charAt(position)  // Hotspot: 20% Faster this way than getRow().
               : getRow(rowIndex)->charAt(position);
}

bool MultipleAlignmentData::isGap(int rowNumber, qint64 pos) const {
    return getRow(rowNumber)->isGap(pos);
}

bool MultipleAlignmentData::isLeadingOrTrailingGap(int rowNumber, qint64 pos) const {
    return getRow(rowNumber)->isLeadingOrTrailingGap(pos);
}

void MultipleAlignmentData::insertGaps(int row, int pos, int count, U2OpStatus& os) {
    if (pos > length) {
        length = pos + count;
        return;
    }
    if (row >= getRowCount() || row < 0 || pos > length || pos < 0 || count < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed "
                              "to MultipleAlignmentData::insertGaps: row index '%1', pos '%2', count '%3'")
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
    getRow(row)->insertGaps(pos, count, os);

    qint64 rowLength = rows[row]->getRowLengthWithoutTrailing();
    length = qMax(length, rowLength);
}

QStringList MultipleAlignmentData::getRowNames() const {
    QStringList rowNames;
    foreach (const MultipleAlignmentRow& row, rows) {
        rowNames.append(row->getName());
    }
    return rowNames;
}

int MultipleAlignmentData::getRowIndexByRowId(qint64 rowId, U2OpStatus& os) const {
    for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex) {
        if (rows.at(rowIndex)->getRowId() == rowId) {
            return rowIndex;
        }
    }
    os.setError("Invalid row id");
    return -1;
}

void MultipleAlignmentData::renameRow(int rowIndex, const QString& name) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getRowCount(),
               QString("Incorrect row index '%1' was passed to MultipleAlignmentData::renameRow: "
                       "the number of rows is '%2'")
                   .arg(rowIndex)
                   .arg(getRowCount()), );
    SAFE_POINT(!name.isEmpty(),
               "Incorrect parameter 'name' was passed to MultipleAlignmentData::renameRow: "
               "Can't set the name of a row to an empty string", );
    rows[rowIndex]->setName(name);
}

void MultipleAlignmentData::setRowId(int rowIndex, qint64 rowId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getRowCount(), "Invalid row index", );
    rows[rowIndex]->setRowId(rowId);
}

void MultipleAlignmentData::removeRow(int rowIndex, U2OpStatus& os) {
    if (rowIndex < 0 || rowIndex >= getRowCount()) {
        coreLog.trace(QString("Internal error: incorrect parameters was passed to MultipleAlignmentData::removeRow, "
                              "rowIndex '%1', the number of rows is '%2'")
                          .arg(rowIndex)
                          .arg(getRowCount()));
        os.setError("Failed to remove a row");
        return;
    }

    MaStateCheck check(this);
    Q_UNUSED(check);

    rows.removeAt(rowIndex);

    if (rows.isEmpty()) {
        length = 0;
    }
}

void MultipleAlignmentData::removeChars(int rowNumber, int pos, int n, U2OpStatus& os) {
    if (rowNumber >= getRowCount() || rowNumber < 0 || pos > length || pos < 0 || n < 0) {
        coreLog.trace(QString("Internal error: incorrect parameters were passed "
                              "to MultipleAlignmentData::removeChars: row index '%1', pos '%2', count '%3'")
                          .arg(rowNumber)
                          .arg(pos)
                          .arg(n));
        os.setError("Failed to remove chars from an alignment");
        return;
    }

    MaStateCheck check(this);
    Q_UNUSED(check);

    getRow(rowNumber)->removeChars(pos, n, os);
}

void MultipleAlignmentData::moveRowsBlock(int startRow, int numRows, int delta) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    // Assumption: numRows is rather big, delta is small (1~2)
    // It's more optimal to move abs(delta) of rows then the block itself

    int i = 0;
    int k = qAbs(delta);

    SAFE_POINT((delta > 0 && startRow + numRows + delta - 1 < rows.length()) || (delta < 0 && startRow + delta >= 0),
               QString("Incorrect parameters in MultipleAlignmentData::moveRowsBlock: "
                       "startRow: '%1', numRows: '%2', delta: '%3'")
                   .arg(startRow)
                   .arg(numRows)
                   .arg(delta), );

    QVector<MultipleAlignmentRow> toMove;
    int fromRow = delta > 0 ? startRow + numRows : startRow + delta;

    while (i < k) {
        MultipleAlignmentRow row = rows.takeAt(fromRow);
        toMove.append(row);
        i++;
    }

    int toRow = delta > 0 ? startRow : startRow + numRows - k;

    while (toMove.count() > 0) {
        MultipleAlignmentRow row = toMove.takeLast();
        rows.insert(toRow, row);
    }
}

bool MultipleAlignmentData::isEqual(const MultipleAlignmentData& other) const {
    CHECK(this != &other, true);
    CHECK(type == other.type, false);
    CHECK(alphabet == other.alphabet, false);
    CHECK(length == other.length, false);
    CHECK(rows.size() == other.rows.size(), false);
    for (int i = 0; i < rows.size(); i++) {
        CHECK(rows[i]->isEqual(*other.rows[i]), false);
    }
    return true;
}

bool MultipleAlignmentData::operator==(const MultipleAlignmentData& other) const {
    return isEqual(other);
}

bool MultipleAlignmentData::operator!=(const MultipleAlignmentData& other) const {
    return !isEqual(other);
}

void MultipleAlignmentData::check() const {
#ifdef DEBUG
    assert(getRowCount() != 0 || length == 0);
    for (int i = 0, n = getRowCount(); i < n; i++) {
        assert(rows[i].getCoreEnd() <= length);
    }
#endif
}

bool MultipleAlignmentData::sortRowsByList(const QStringList& rowsOrder) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    const QStringList rowNames = getRowNames();
    foreach (const QString& rowName, rowNames) {
        CHECK(rowsOrder.contains(rowName), false);
    }

    QVector<MultipleAlignmentRow> sortedRows;
    foreach (const QString& rowName, rowsOrder) {
        int rowIndex = rowNames.indexOf(rowName);
        if (rowIndex >= 0) {
            sortedRows.append(rows[rowIndex]);
        }
    }

    rows = sortedRows;
    return true;
}

void MultipleAlignmentData::addRowPrivate(const MultipleAlignmentRow& row, qint64 rowLenWithTrailingGaps, int rowIndex) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    length = qMax(rowLenWithTrailingGaps, length);
    int idx = rowIndex == -1 ? getRowCount() : qBound(0, rowIndex, getRowCount());
    rows.insert(idx, row);
}

void MultipleAlignmentData::removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows) {
    SAFE_POINT(startPos >= 0 && startPos + nBases <= length && nBases > 0,
               QString("Incorrect parameters were passed to MultipleAlignmentData::removeRegion: startPos '%1', "
                       "nBases '%2', the length is '%3'")
                   .arg(startPos)
                   .arg(nBases)
                   .arg(length), );
    SAFE_POINT(startRow >= 0 && startRow + nRows <= getRowCount() && (nRows > 0 || (nRows == 0 && getRowCount() == 0)),
               QString("Incorrect parameters were passed to MultipleAlignmentData::removeRegion: startRow '%1', "
                       "nRows '%2', the number of rows is '%3'")
                   .arg(startRow)
                   .arg(nRows)
                   .arg(getRowCount()), );

    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    for (int i = startRow + nRows; --i >= startRow;) {
        getRow(i)->removeChars(startPos, nBases, os);
        SAFE_POINT_OP(os, );

        if (removeEmptyRows && (getRow(i)->getSequence().length() == 0)) {
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

bool MultipleAlignmentData::crop(const QList<qint64>& rowIds, const U2Region& columnRange, U2OpStatus& os) {
    if (!(columnRange.startPos >= 0 && columnRange.length > 0 && columnRange.length < length && columnRange.startPos < length)) {
        os.setError(QString("Incorrect region was passed to MultipleSequenceData::crop, "
                            "startPos '%1', length '%2'")
                        .arg(columnRange.startPos)
                        .arg(columnRange.length));
        return false;
    }

    qint64 safeLength = columnRange.length;
    if (columnRange.endPos() > length) {
        safeLength -= columnRange.endPos() - length;
    }

    MaStateCheck check(this);
    Q_UNUSED(check);

    QSet<qint64> rowIdSet = rowIds.toSet();
    QVector<MultipleAlignmentRow> newRowList;
    for (int i = 0; i < rows.size(); i++) {
        MultipleAlignmentRow row = getRow(i).clone();
        qint64 rowId = row->getRowId();
        if (rowIdSet.contains(rowId)) {
            row->crop(os, (int)columnRange.startPos, (int)safeLength);
            CHECK_OP(os, false);
            newRowList << row;
        }
    }
    rows = newRowList;

    length = safeLength;
    return true;
}

bool MultipleAlignmentData::crop(const U2Region& region, U2OpStatus& os) {
    return crop(getRowsIds(), region, os);
}

bool MultipleAlignmentData::sortRowsBySimilarity(QVector<U2Region>& united) {
    QVector<MultipleAlignmentRow> sortedRows = getRowsSortedBySimilarity(united);
    if (getRows() == sortedRows) {
        return false;
    }
    rows = sortedRows;
    return true;
}

QVector<MultipleAlignmentRow> MultipleAlignmentData::getRowsSortedBySimilarity(QVector<U2Region>& united) const {
    QVector<MultipleAlignmentRow> oldRows = getRows();
    QVector<MultipleAlignmentRow> sortedRows;
    while (!oldRows.isEmpty()) {
        MultipleAlignmentRow row = oldRows.takeFirst();
        sortedRows << row;
        int start = sortedRows.size() - 1;
        int len = 1;
        QMutableVectorIterator<MultipleAlignmentRow> iter(oldRows);
        while (iter.hasNext()) {
            const MultipleAlignmentRow& next = iter.next();
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
    return sortedRows;
}

void MultipleAlignmentData::setRowContent(int rowNumber, const QByteArray& sequence, int offset) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getRowCount(),
               QString("Incorrect row index '%1' was passed to MultipleAlignmentData::setRowContent: "
                       "the number of rows is '%2'")
                   .arg(rowNumber)
                   .arg(getRowCount()), );
    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    getRow(rowNumber)->setRowContent(sequence, offset, os);
    SAFE_POINT_OP(os, );

    length = qMax(length, (qint64)sequence.size() + offset);
}

void MultipleAlignmentData::setRowContent(int rowNumber, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getRowCount(),
               QString("Incorrect row index '%1' was passed to MultipleAlignmentData::setRowContent: "
                       "the number of rows is '%2'")
                   .arg(rowNumber)
                   .arg(getRowCount()), );
    MaStateCheck check(this);
    Q_UNUSED(check);

    U2OpStatus2Log os;
    getRow(rowNumber)->setRowContent(chromatogram, sequence, gapModel, os);
    SAFE_POINT_OP(os, );

    length = qMax(length, (qint64)MsaRowUtils::getRowLength(sequence.seq, gapModel));
}

void MultipleAlignmentData::toUpperCase() {
    for (int i = 0, n = getRowCount(); i < n; i++) {
        getRow(i)->toUpperCase();
    }
}

void MultipleAlignmentData::setSequenceId(int rowIndex, const U2DataId& sequenceId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getRowCount(), "Invalid row index", );
    getRow(rowIndex)->setSequenceId(sequenceId);
}

void MultipleAlignmentData::setRowGapModel(int rowNumber, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(rowNumber >= 0 && rowNumber < getRowCount(), "Invalid row index", );
    length = qMax(length, (qint64)MsaRowUtils::getGapsLength(gapModel) + getRow(rowNumber)->getSequence().length());
    getRow(rowNumber)->setGapModel(gapModel);
}

void MultipleAlignmentData::addRow(const QString& name, const QByteArray& bytes) {
    MultipleAlignmentRow newRow = createRow(name, bytes);
    addRowPrivate(newRow, bytes.size(), -1);
}

void MultipleAlignmentData::addRow(const QString& name, const QByteArray& bytes, int rowIndex) {
    MultipleAlignmentRow newRow = createRow(name, bytes);
    addRowPrivate(newRow, bytes.size(), rowIndex);
}

void MultipleAlignmentData::addRow(const U2MsaRow& rowInDb, const DNASequence& sequence, U2OpStatus& os) {
    MultipleAlignmentRow newRow = createRow(rowInDb, sequence, rowInDb.gaps, os);
    CHECK_OP(os, );
    addRowPrivate(newRow, rowInDb.length, -1);
}

void MultipleAlignmentData::addRow(const QString& name, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    SAFE_POINT(type == MultipleAlignmentDataType::MSA, "Can't use a method with no chromatogram for MSA", );
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

void MultipleAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes) {
    MultipleAlignmentRow newRow = createRow(name, chromatogram, bytes);
    addRowPrivate(newRow, bytes.size(), -1);
}

void MultipleAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes, int rowIndex) {
    MultipleAlignmentRow newRow = createRow(name, chromatogram, bytes);
    addRowPrivate(newRow, bytes.size(), rowIndex);
}

void MultipleAlignmentData::addRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, U2OpStatus& os) {
    MultipleAlignmentRow newRow = createRow(rowInDb, chromatogram, sequence, rowInDb.gaps, os);
    CHECK_OP(os, );
    addRowPrivate(newRow, rowInDb.length, -1);
}

void MultipleAlignmentData::addRow(const QString& name, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    SAFE_POINT(type == MultipleAlignmentDataType::MCA, "Only MCA can have chromatogram in a row", );
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

MultipleAlignmentRow MultipleAlignmentData::createRow(const QString& name, const QByteArray& bytes) {
    SAFE_POINT(type == MultipleAlignmentDataType::MSA, "Can't use a method with no chromatogram for MSA", {});
    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MaDbiUtils::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(name, newSequenceBytes);

    U2MsaRow row;
    return {row, newSequence, newGapsModel, this};
}

MultipleAlignmentRow MultipleAlignmentData::createRow(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    SAFE_POINT(type == MultipleAlignmentDataType::MSA, "Can't use a method with no chromatogram for MSA", {});
    QString errorText = "Failed to create a multiple alignment row";
    if (sequence.constSequence().indexOf(U2Msa::GAP_CHAR) != -1) {
        coreLog.trace("Attempted to create an alignment row from a sequence with gaps");
        os.setError(errorText);
        return {};
    }

    int sequenceLength = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos > sequenceLength || !gap.isValid()) {
            coreLog.trace("Incorrect gap model was passed to MultipleAlignmentData::createRow");
            os.setError(errorText);
            return {};
        }
        sequenceLength += gap.length;
    }

    return {rowInDb, sequence, gaps, this};
}

MultipleAlignmentRow MultipleAlignmentData::createRow(const MultipleAlignmentRow& row) {
    return {row, this};
}

MultipleAlignmentRow MultipleAlignmentData::createRow(const QString& name, const DNAChromatogram& chromatogram, const QByteArray& bytes) {
    SAFE_POINT(type == MultipleAlignmentDataType::MCA, "Only MCA can have a chromatogram", {});
    QByteArray newSequenceBytes;
    QVector<U2MsaGap> newGapsModel;

    MaDbiUtils::splitBytesToCharsAndGaps(bytes, newSequenceBytes, newGapsModel);
    DNASequence newSequence(name, newSequenceBytes);

    U2MsaRow row;
    return {row, chromatogram, newSequence, newGapsModel, this};
}

MultipleAlignmentRow MultipleAlignmentData::createRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os) {
    SAFE_POINT(type == MultipleAlignmentDataType::MCA, "Only MCA can have a chromatogram", {});
    QString errorText = "Failed to create a multiple alignment row";
    if (sequence.constSequence().indexOf(U2Msa::GAP_CHAR) != -1) {
        coreLog.trace("Attempted to create an alignment row from a sequence with gaps");
        os.setError(errorText);
        return {};
    }

    int sequenceLength = sequence.length();
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.startPos > sequenceLength || !gap.isValid()) {
            coreLog.trace("Incorrect gap model was passed to MultipleAlignmentData::createRow");
            os.setError(errorText);
            return {};
        }
        sequenceLength += gap.length;
    }

    return {rowInDb, chromatogram, sequence, gaps, this};
}

bool MultipleAlignmentData::hasEmptyGapModel() const {
    foreach (const MultipleAlignmentRow& row, rows) {
        if (!row->getGaps().isEmpty()) {
            return false;
        }
    }
    return true;
}

bool MultipleAlignmentData::hasEqualLength() const {
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

void MultipleAlignmentData::appendChars(int row, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendChars").arg(row), );

    MultipleAlignmentRow appendedRow = createRow("", QByteArray(str, len));

    qint64 rowLength = getRow(row)->getRowLength();

    U2OpStatus2Log os;
    getRow(row)->append(appendedRow, (int)rowLength, os);
    CHECK_OP(os, );

    length = qMax(length, rowLength + len);
}

void MultipleAlignmentData::appendChars(int row, qint64 afterPos, const char* str, int len) {
    SAFE_POINT(0 <= row && row < getRowCount(),
               QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendChars").arg(row), );

    MultipleAlignmentRow appendedRow = createRow("", QByteArray(str, len));

    U2OpStatus2Log os;
    getRow(row)->append(appendedRow, (int)afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + len);
}

void MultipleAlignmentData::appendRow(int rowNumber, qint64 afterPos, const MultipleAlignmentRow& row, U2OpStatus& os) {
    SAFE_POINT(0 <= rowNumber && rowNumber < getRowCount(),
               QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::appendRow").arg(rowNumber), );

    getRow(rowNumber)->append(row, (int)afterPos, os);
    CHECK_OP(os, );

    length = qMax(length, afterPos + row->getRowLength());
}

void MultipleAlignmentData::replaceChars(int row, char origChar, char resultChar) {
    SAFE_POINT(row >= 0 && row < getRowCount(), QString("Incorrect row index '%1' in MultipleSequenceAlignmentData::replaceChars").arg(row), );

    if (origChar == resultChar) {
        return;
    }

    U2OpStatus2Log os;
    getRow(row)->replaceChars(origChar, resultChar, os);
}

MultipleAlignment MultipleAlignmentData::getCopy() const {
    return {new MultipleAlignmentData(*this)};
}

MultipleAlignment MultipleAlignmentData::mid(int start, int len) const {
    SAFE_POINT(start >= 0 && start + len <= length,
               QString("Incorrect parameters were passed to MultipleAlignmentData::mid: "
                       "start '%1', len '%2', the alignment length is '%3'")
                   .arg(start)
                   .arg(len)
                   .arg(length),
               {type});

    MultipleAlignment res(type, getName(), alphabet);
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

MultipleAlignmentData& MultipleAlignmentData::operator+=(const MultipleAlignmentData& msaData) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    SAFE_POINT(msaData.alphabet == alphabet, "Different alphabets in MultipleAlignmentData::operator+=", *this);

    int nSeq = getRowCount();
    SAFE_POINT(msaData.getRowCount() == nSeq, "Different number of rows in MultipleAlignmentData::operator+=", *this);

    U2OpStatus2Log os;
    for (int i = 0; i < nSeq; i++) {
        getRow(i)->append(msaData.getRow(i), (int)length, os);
    }

    length += msaData.length;
    return *this;
}

}  // namespace U2
