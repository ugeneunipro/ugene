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

#include "MultipleAlignment.h"

#include <QSet>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MaStateCheck.h"
#include "MultipleAlignmentInfo.h"

namespace U2 {

MultipleAlignment::MultipleAlignment(MultipleAlignmentData *maData)
    : maData(maData) {
}

MultipleAlignment::~MultipleAlignment() {
}

MultipleAlignmentData *MultipleAlignment::data() const {
    return maData.data();
}

MultipleAlignmentData &MultipleAlignment::operator*() {
    return *maData;
}

const MultipleAlignmentData &MultipleAlignment::operator*() const {
    return *maData;
}

MultipleAlignmentData *MultipleAlignment::operator->() {
    return maData.data();
}

const MultipleAlignmentData *MultipleAlignment::operator->() const {
    return maData.data();
}

MultipleAlignmentData::MultipleAlignmentData(const QString &name, const DNAAlphabet *alphabet, const QList<MultipleAlignmentRow> &rows)
    : alphabet(alphabet),
      rows(rows),
      length(0) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    SAFE_POINT(NULL == alphabet || !name.isEmpty(), "Incorrect parameters in MultipleAlignmentData ctor", );    // TODO: check the condition, it is strange

    setName(name);
    for (int i = 0, n = rows.size(); i < n; i++) {
        length = qMax(length, rows[i]->getRowLengthWithoutTrailing());    // TODO: implement or replace the method for row length
    }
}

MultipleAlignmentData::~MultipleAlignmentData() {
}

void MultipleAlignmentData::clear() {
    MaStateCheck check(this);
    Q_UNUSED(check);
    rows.clear();
    length = 0;
}

QString MultipleAlignmentData::getName() const {
    return MultipleAlignmentInfo::getName(info);
}

void MultipleAlignmentData::setName(const QString &newName) {
    MultipleAlignmentInfo::setName(info, newName);
}

const DNAAlphabet *MultipleAlignmentData::getAlphabet() const {
    return alphabet;
}

void MultipleAlignmentData::setAlphabet(const DNAAlphabet *newAlphabet) {
    SAFE_POINT(newAlphabet != nullptr, "Internal error: attempted to set NULL alphabet for the alignment", );
    alphabet = newAlphabet;
}

QVariantMap MultipleAlignmentData::getInfo() const {
    return info;
}

void MultipleAlignmentData::setInfo(const QVariantMap &newInfo) {
    info = newInfo;
}

bool MultipleAlignmentData::isEmpty() const {
    return getLength() == 0 || rows.isEmpty();
}

int MultipleAlignmentData::getLength() const {
    return length;
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
    for (int i = 0, n = getNumRows(); i < n; i++) {
        rows[i]->crop(os, 0, newLength);
        CHECK_OP(os, );
    }
    length = newLength;
}

int MultipleAlignmentData::getNumRows() const {
    return rows.size();
}

U2MsaListGapModel MultipleAlignmentData::getGapModel() const {
    U2MsaListGapModel gapModel;
    const int alignmentLength = getLength();
    for (const MultipleAlignmentRow &row : rows) {
        gapModel << row->getGapModel();
        const int rowPureLength = row->getRowLengthWithoutTrailing();
        if (rowPureLength < alignmentLength) {
            gapModel.last() << U2MsaGap(rowPureLength, alignmentLength - rowPureLength);
        }
    }
    return gapModel;
}

static bool isGreaterByName(const MultipleAlignmentRow &row1, const MultipleAlignmentRow &row2) {
    return QString::compare(row1->getName(), row2->getName(), Qt::CaseInsensitive) > 0;
}

static bool isLessByName(const MultipleAlignmentRow &row1, const MultipleAlignmentRow &row2) {
    return QString::compare(row1->getName(), row2->getName(), Qt::CaseInsensitive) < 0;
}

void MultipleAlignmentData::sortRowsByName(MultipleAlignment::Order order) {
    MaStateCheck check(this);
    Q_UNUSED(check);
    bool isAscending = order == MultipleAlignment::Ascending;
    std::stable_sort(rows.begin(), rows.end(), isAscending ? isLessByName : isGreaterByName);
}

static bool isGreaterByLength(const MultipleAlignmentRow &row1, const MultipleAlignmentRow &row2) {
    return row1->getUngappedLength() > row2->getUngappedLength();
}

static bool isLessByLength(const MultipleAlignmentRow &row1, const MultipleAlignmentRow &row2) {
    return row1->getUngappedLength() < row2->getUngappedLength();
}

void MultipleAlignmentData::sortRowsByLength(MultipleAlignment::Order order) {
    MaStateCheck check(this);
    Q_UNUSED(check);
    bool isAscending = order == MultipleAlignment::Ascending;
    std::stable_sort(rows.begin(), rows.end(), isAscending ? isLessByLength : isGreaterByLength);
}

MultipleAlignmentRow MultipleAlignmentData::getRow(int rowIndex) {
    int rowsCount = rows.count();
    SAFE_POINT(0 != rowsCount, "No rows", getEmptyRow());
    SAFE_POINT(rowIndex >= 0 && (rowIndex < rowsCount), "Internal error: unexpected row index was passed to MAlignmnet::getRow", getEmptyRow());
    return rows[rowIndex];
}

const MultipleAlignmentRow &MultipleAlignmentData::getRow(int rowIndex) const {
    static MultipleAlignmentRow emptyRow = getEmptyRow();
    int rowsCount = rows.count();
    SAFE_POINT(0 != rowsCount, "No rows", emptyRow);
    SAFE_POINT(rowIndex >= 0 && (rowIndex < rowsCount), "Internal error: unexpected row index was passed to MAlignment::getRow", emptyRow);
    return rows[rowIndex];
}

const MultipleAlignmentRow &MultipleAlignmentData::getRow(const QString &name) const {
    static MultipleAlignmentRow emptyRow = getEmptyRow();
    for (int i = 0; i < rows.count(); i++) {
        if (rows[i]->getName() == name) {
            return rows[i];
        }
    }
    SAFE_POINT(false, "Internal error: row name passed to MAlignmnet::getRow function not exists", emptyRow);
}

const QList<MultipleAlignmentRow> &MultipleAlignmentData::getRows() const {
    return rows;
}

QList<qint64> MultipleAlignmentData::getRowsIds() const {
    QList<qint64> rowIds;
    foreach (const MultipleAlignmentRow &row, rows) {
        rowIds.append(row->getRowId());
    }
    return rowIds;
}

QList<qint64> MultipleAlignmentData::getRowIdsByRowIndexes(const QList<int> &rowIndexes) const {
    QList<qint64> rowIds;
    foreach (int rowIndex, rowIndexes) {
        bool isValidRowIndex = rowIndex >= 0 && rowIndex < rows.size();
        rowIds.append(isValidRowIndex ? rows[rowIndex]->getRowId() : -1);
    }
    return rowIds;
}

MultipleAlignmentRow MultipleAlignmentData::getRowByRowId(qint64 rowId, U2OpStatus &os) const {
    static MultipleAlignmentRow emptyRow = getEmptyRow();
    foreach (const MultipleAlignmentRow &row, rows) {
        if (row->getRowId() == rowId) {
            return row;
        }
    }
    os.setError("Failed to find a row in an alignment");
    return emptyRow;
}

char MultipleAlignmentData::charAt(int rowNumber, qint64 position) const {
    return getRow(rowNumber)->charAt(position);
}

bool MultipleAlignmentData::isGap(int rowNumber, qint64 pos) const {
    return getRow(rowNumber)->isGap(pos);
}

QStringList MultipleAlignmentData::getRowNames() const {
    QStringList rowNames;
    foreach (const MultipleAlignmentRow &row, rows) {
        rowNames.append(row->getName());
    }
    return rowNames;
}

int MultipleAlignmentData::getRowIndexByRowId(qint64 rowId, U2OpStatus &os) const {
    for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex) {
        if (rows.at(rowIndex)->getRowId() == rowId) {
            return rowIndex;
        }
    }
    os.setError("Invalid row id");
    return U2MsaRow::INVALID_ROW_ID;
}

void MultipleAlignmentData::renameRow(int rowIndex, const QString &name) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getNumRows(),
               QString("Incorrect row index '%1' was passed to MultipleAlignmentData::renameRow: "
                       "the number of rows is '%2'")
                   .arg(rowIndex)
                   .arg(getNumRows()), );
    SAFE_POINT(!name.isEmpty(),
               "Incorrect parameter 'name' was passed to MultipleAlignmentData::renameRow: "
               "Can't set the name of a row to an empty string", );
    rows[rowIndex]->setName(name);
}

void MultipleAlignmentData::setRowId(int rowIndex, qint64 rowId) {
    SAFE_POINT(rowIndex >= 0 && rowIndex < getNumRows(), "Invalid row index", );
    rows[rowIndex]->setRowId(rowId);
}

void MultipleAlignmentData::removeRow(int rowIndex, U2OpStatus &os) {
    if (rowIndex < 0 || rowIndex >= getNumRows()) {
        coreLog.trace(QString("Internal error: incorrect parameters was passed to MultipleAlignmentData::removeRow, "
                              "rowIndex '%1', the number of rows is '%2'")
                          .arg(rowIndex)
                          .arg(getNumRows()));
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

void MultipleAlignmentData::removeChars(int rowNumber, int pos, int n, U2OpStatus &os) {
    if (rowNumber >= getNumRows() || rowNumber < 0 || pos > length || pos < 0 || n < 0) {
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

    QList<MultipleAlignmentRow> toMove;
    int fromRow = delta > 0 ? startRow + numRows : startRow + delta;

    while (i < k) {
        const MultipleAlignmentRow row = rows.takeAt(fromRow);
        toMove.append(row);
        i++;
    }

    int toRow = delta > 0 ? startRow : startRow + numRows - k;

    while (toMove.count() > 0) {
        int n = toMove.count();
        const MultipleAlignmentRow row = toMove.takeAt(n - 1);
        rows.insert(toRow, row);
    }
}

bool MultipleAlignmentData::operator==(const MultipleAlignmentData &other) const {
    const bool lengthsAreEqual = (length == other.length);
    const bool alphabetsAreEqual = (alphabet == other.alphabet);
    bool rowsAreEqual = (rows.size() == other.rows.size());
    for (int i = 0; i < rows.size() && rowsAreEqual; i++) {
        rowsAreEqual &= (*rows[i] == *other.rows[i]);
    }
    return lengthsAreEqual && alphabetsAreEqual && rowsAreEqual;
}

bool MultipleAlignmentData::operator!=(const MultipleAlignmentData &other) const {
    return !operator==(other);
}

void MultipleAlignmentData::check() const {
#ifdef DEBUG
    assert(getNumRows() != 0 || length == 0);
    for (int i = 0, n = getNumRows(); i < n; i++) {
        assert(rows[i].getCoreEnd() <= length);
    }
#endif
}

bool MultipleAlignmentData::sortRowsByList(const QStringList &rowsOrder) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    const QStringList rowNames = getRowNames();
    foreach (const QString &rowName, rowNames) {
        CHECK(rowsOrder.contains(rowName), false);
    }

    QList<MultipleAlignmentRow> sortedRows;
    foreach (const QString &rowName, rowsOrder) {
        int rowIndex = rowNames.indexOf(rowName);
        if (rowIndex >= 0) {
            sortedRows.append(rows[rowIndex]);
        }
    }

    rows = sortedRows;
    return true;
}

void MultipleAlignmentData::addRowPrivate(const MultipleAlignmentRow &row, qint64 rowLenWithTrailingGaps, int rowIndex) {
    MaStateCheck check(this);
    Q_UNUSED(check);

    length = qMax(rowLenWithTrailingGaps, length);
    int idx = rowIndex == -1 ? getNumRows() : qBound(0, rowIndex, getNumRows());
    rows.insert(idx, row);
}

}    // namespace U2
