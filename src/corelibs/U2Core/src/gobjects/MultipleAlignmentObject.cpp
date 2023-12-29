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

#include "MultipleAlignmentObject.h"

#include <U2Core/DbiConnection.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MsaExportUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

MaSavedState::MaSavedState()
    : lastState(nullptr) {
}

MaSavedState::~MaSavedState() {
    delete lastState;
}

bool MaSavedState::hasState() const {
    return lastState != nullptr;
}

MultipleAlignment MaSavedState::takeState() {
    const MultipleAlignment state = *lastState;
    delete lastState;
    lastState = nullptr;
    return state;
}

void MaSavedState::setState(const MultipleAlignment& ma) {
    delete lastState;
    lastState = new MultipleAlignment(ma->getCopy());
}

MultipleAlignmentObject::MultipleAlignmentObject(const QString& gobjectType,
                                                 const QString& name,
                                                 const U2EntityRef& maRef,
                                                 const QVariantMap& hintsMap,
                                                 const MultipleAlignment& alignment)
    : GObject(gobjectType, name, hintsMap),
      cachedMa(alignment->getCopy()) {
    entityRef = maRef;
    dataLoaded = false;

    if (!cachedMa->isEmpty()) {
        dataLoaded = true;
    }
}

MultipleAlignmentObject::~MultipleAlignmentObject() {
    emit si_invalidateAlignmentObject();
}

void MultipleAlignmentObject::setTrackMod(U2OpStatus& os, U2TrackModType trackMod) {
    // Prepare the connection
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );

    U2ObjectDbi* objectDbi = con.dbi->getObjectDbi();
    SAFE_POINT(objectDbi != nullptr, "NULL Object Dbi", );

    // Set the new status
    objectDbi->setTrackModType(entityRef.entityId, trackMod, os);
}

void MultipleAlignmentObject::replaceCharacter(int startPos, int rowIndex, char newChar) {
    replaceCharacters({startPos, 1}, rowIndex, newChar);
}

void MultipleAlignmentObject::replaceCharacters(const U2Region& columnRange, int rowIndex, char newChar) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleAlignment& ma = getAlignment();
    SAFE_POINT(U2Region(0, ma->getLength()).contains(columnRange), "Invalid parameters", );

    const MultipleAlignmentRow& row = ma->getRow(rowIndex);
    qint64 rowId = row->getRowId();
    U2OpStatus2Log os;

    if (newChar != U2Msa::GAP_CHAR) {
        MsaDbiUtils::replaceCharactersInRow(entityRef, rowId, columnRange, newChar, os);
        CHECK_OP(os, )
    } else {
        MsaDbiUtils::removeRegion(entityRef, {rowId}, columnRange.startPos, columnRange.length, os);
        CHECK_OP(os, )

        MsaDbiUtils::insertGaps(entityRef, {rowId}, columnRange.startPos, columnRange.length, os, true);
        CHECK_OP(os, )
    }

    MaModificationInfo mi;
    mi.rowContentChanged = true;
    mi.rowListChanged = false;
    mi.alignmentLengthChanged = false;
    mi.modifiedRowIds << rowId;

    if (newChar != ' ' && !ma->getAlphabet()->contains(newChar)) {
        const DNAAlphabet* alp = U2AlphabetUtils::findBestAlphabet(QByteArray(1, newChar));
        const DNAAlphabet* newAlphabet = U2AlphabetUtils::deriveCommonAlphabet(alp, ma->getAlphabet());
        SAFE_POINT(newAlphabet != nullptr, "Common alphabet is NULL", );

        if (newAlphabet->getId() != ma->getAlphabet()->getId()) {
            MaDbiUtils::updateMaAlphabet(entityRef, newAlphabet->getId(), os);
            mi.alphabetChanged = true;
            SAFE_POINT_OP(os, );
        }
    }
    updateCachedMultipleAlignment(mi);
}

const MultipleAlignment& MultipleAlignmentObject::getAlignment() const {
    ensureDataLoaded();
    return cachedMa;
}

void MultipleAlignmentObject::setMultipleAlignment(const MultipleAlignment& newMa, MaModificationInfo mi, const QVariantMap& hints) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    U2OpStatus2Log os;
    updateDatabase(os, newMa);
    SAFE_POINT_OP(os, );

    mi.hints = hints;
    updateCachedMultipleAlignment(mi);
}

MultipleAlignment MultipleAlignmentObject::getMultipleAlignmentCopy() const {
    return getAlignment()->getCopy();
}

void MultipleAlignmentObject::setGObjectName(const QString& newName) {
    ensureDataLoaded();
    CHECK(cachedMa->getName() != newName, );

    if (!isStateLocked()) {
        U2OpStatus2Log os;
        MaDbiUtils::renameMa(entityRef, newName, os);
        CHECK_OP(os, );

        updateCachedMultipleAlignment();
    } else {
        GObject::setGObjectName(newName);
        cachedMa->setName(newName);
    }
}

const DNAAlphabet* MultipleAlignmentObject::getAlphabet() const {
    return getAlignment()->getAlphabet();
}

qint64 MultipleAlignmentObject::getLength() const {
    return getAlignment()->getLength();
}

qint64 MultipleAlignmentObject::getRowCount() const {
    return getAlignment()->getRowCount();
}

const QVector<MultipleAlignmentRow>& MultipleAlignmentObject::getRows() const {
    return getAlignment()->getRows();
}

const MultipleAlignmentRow& MultipleAlignmentObject::getRow(int row) const {
    return getAlignment()->getRow(row);
}

int MultipleAlignmentObject::getRowPosById(qint64 rowId) const {
    return getAlignment()->getRowsIds().indexOf(rowId);
}

QList<QVector<U2MsaGap>> MultipleAlignmentObject::getGapModel() const {
    return getAlignment()->getGapModel();
}

void MultipleAlignmentObject::removeRow(int rowIdx) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleAlignment& ma = getAlignment();
    SAFE_POINT(rowIdx >= 0 && rowIdx < ma->getRowCount(), "Invalid row index", );
    qint64 rowId = ma->getRow(rowIdx)->getRowId();

    U2OpStatus2Log os;
    removeRowPrivate(os, entityRef, rowId);
    SAFE_POINT_OP(os, );

    MaModificationInfo mi;
    mi.rowContentChanged = false;
    mi.alignmentLengthChanged = false;

    QList<qint64> removedRowIds;
    removedRowIds << rowId;

    updateCachedMultipleAlignment(mi, removedRowIds);
}

void MultipleAlignmentObject::removeRowsById(const QList<qint64>& rowIds) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    CHECK(!rowIds.isEmpty(), );

    QList<qint64> removedRowIds;
    for (qint64 rowId : qAsConst(rowIds)) {
        U2OpStatus2Log os;
        removeRowPrivate(os, entityRef, rowId);
        if (!os.hasError()) {
            removedRowIds << rowId;
        }
    }

    MaModificationInfo mi;
    mi.rowContentChanged = false;
    mi.alignmentLengthChanged = false;
    updateCachedMultipleAlignment(mi, removedRowIds);

    SAFE_POINT(removedRowIds.size() == rowIds.size(), "Failed to remove some rows", );
}

void MultipleAlignmentObject::removeRows(const QList<int>& rowIndexes) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    CHECK(!rowIndexes.isEmpty(), );

    const MultipleAlignment& ma = getAlignment();
    QList<qint64> rowIdsToRemove;
    foreach (int rowIdx, rowIndexes) {
        SAFE_POINT(rowIdx >= 0 && rowIdx < ma->getRowCount(), "Invalid row index", );
        qint64 rowId = ma->getRow(rowIdx)->getRowId();
        rowIdsToRemove << rowId;
    }

    removeRowsById(rowIdsToRemove);
}

void MultipleAlignmentObject::renameRow(int rowIdx, const QString& newName) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleAlignment& ma = getAlignment();
    SAFE_POINT(rowIdx >= 0 && rowIdx < ma->getRowCount(), "Invalid row index", );
    qint64 rowId = ma->getRow(rowIdx)->getRowId();

    U2OpStatus2Log os;
    MaDbiUtils::renameRow(entityRef, rowId, newName, os);
    SAFE_POINT_OP(os, );

    MaModificationInfo mi;
    mi.alignmentLengthChanged = false;
    updateCachedMultipleAlignment(mi);
}

bool MultipleAlignmentObject::isRegionEmpty(int startPos, int startRow, int numChars, int numRows) const {
    const MultipleAlignment& ma = getAlignment();
    bool isEmpty = true;
    for (int row = startRow; row < startRow + numRows && isEmpty; ++row) {
        for (int pos = startPos; pos < startPos + numChars && isEmpty; ++pos) {
            isEmpty = ma->isGap(row, pos);
        }
    }
    return isEmpty;
}

bool MultipleAlignmentObject::isRegionEmpty(const QList<int>& rowIndexes, int x, int width) const {
    const MultipleAlignment& ma = getAlignment();
    bool isEmpty = true;
    for (int i = 0; i < rowIndexes.size() && isEmpty; i++) {
        int rowIndex = rowIndexes[i];
        for (int pos = x; pos < x + width && isEmpty; pos++) {
            isEmpty = ma->isGap(rowIndex, pos);
        }
    }
    return isEmpty;
}

void MultipleAlignmentObject::moveRowsBlock(int firstRow, int numRows, int shift) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    QList<qint64> rowIds = getAlignment()->getRowsIds();
    QList<qint64> rowsToMove;

    for (int i = 0; i < numRows; ++i) {
        rowsToMove << rowIds[firstRow + i];
    }

    U2OpStatusImpl os;
    MaDbiUtils::moveRows(entityRef, rowsToMove, shift, os);
    CHECK_OP(os, );

    updateCachedMultipleAlignment();
}

QList<qint64> MultipleAlignmentObject::getRowIds() const {
    return getAlignment()->getRowsIds();
}

QList<qint64> MultipleAlignmentObject::getRowIdsByRowIndexes(const QList<int>& rowIndexes) const {
    QList<qint64> allRowIds = getRowIds();
    QList<qint64> resultRowIds;
    int rowCount = getRowCount();
    for (int rowIndex : qAsConst(rowIndexes)) {
        SAFE_POINT(rowIndex >= 0 && rowIndex < rowCount, "Invalid row index: " + QString::number(rowIndex), {});
        resultRowIds << allRowIds[rowIndex];
    }
    return resultRowIds;
}

void MultipleAlignmentObject::updateRowsOrder(U2OpStatus& os, const QList<qint64>& rowIds) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    if (rowIds == getRowIds()) {
        return;
    }
    MaDbiUtils::updateRowsOrder(entityRef, rowIds, os);
    CHECK_OP(os, );

    MaModificationInfo mi;
    mi.alignmentLengthChanged = false;
    updateCachedMultipleAlignment(mi);
}

void MultipleAlignmentObject::changeLength(U2OpStatus& os, qint64 newLength) {
    qint64 length = getLength();
    CHECK(length != newLength, );

    MaDbiUtils::updateMaLength(getEntityRef(), newLength, os);
    CHECK_OP(os, );

    bool rowContentChangeStatus = false;
    if (newLength < length) {
        qint64 numRows = getRowCount();
        for (int i = 0; i < numRows; i++) {
            MultipleAlignmentRow row = getRow(i);
            qint64 rowLengthWithoutTrailing = row->getRowLengthWithoutTrailing();
            if (rowLengthWithoutTrailing > newLength) {
                row->crop(os, 0, newLength);
                CHECK_OP(os, );
                rowContentChangeStatus = true;
            }
        }
    }

    MaModificationInfo modificationInfo;
    modificationInfo.rowContentChanged = rowContentChangeStatus;
    modificationInfo.rowListChanged = false;
    updateCachedMultipleAlignment(modificationInfo);
}

void MultipleAlignmentObject::updateCachedMultipleAlignment(const MaModificationInfo& mi, const QList<qint64>& removedRowIds) {
    ensureDataLoaded();
    emit si_startMaUpdating();

    MultipleAlignment oldCachedMa = cachedMa->getCopy();

    // Undo-Redo Framework does not provide detailed change info.
    // In this case or if we do not know which rows are changed we reload the whole object.
    bool isUnknownChange = mi.type == MaModificationType_Undo || mi.type == MaModificationType_Redo || (mi.modifiedRowIds.isEmpty() && removedRowIds.isEmpty());
    U2OpStatus2Log os;
    if (isUnknownChange) {
        loadAlignment(os);  // Reload 'cachedMa'.
        SAFE_POINT_OP(os, );
    } else {
        if (mi.alignmentLengthChanged) {
            qint64 msaLength = MaDbiUtils::getMaLength(entityRef, os);
            SAFE_POINT_OP(os, );
            if (msaLength != cachedMa->getLength()) {
                cachedMa->setLength(msaLength);
            }
        }

        if (mi.alphabetChanged) {
            U2AlphabetId alphabet = MaDbiUtils::getMaAlphabet(entityRef, os);
            SAFE_POINT_OP(os, );
            if (alphabet.id != cachedMa->getAlphabet()->getId() && !alphabet.id.isEmpty()) {
                const DNAAlphabet* newAlphabet = U2AlphabetUtils::getById(alphabet);
                cachedMa->setAlphabet(newAlphabet);
            }
        }

        if (!removedRowIds.isEmpty()) {
            foreach (qint64 rowId, removedRowIds) {
                const int rowIndex = cachedMa->getRowIndexByRowId(rowId, os);
                SAFE_POINT_OP(os, );
                cachedMa->removeRow(rowIndex, os);
                SAFE_POINT_OP(os, );
            }
        }
        if (!mi.modifiedRowIds.isEmpty()) {
            updateCachedRows(os, mi.modifiedRowIds);
        }
    }

    setModified(true);
    if (!mi.middleState) {
        emit si_alignmentChanged(oldCachedMa, mi);

        if (cachedMa->isEmpty() && !oldCachedMa->isEmpty()) {
            emit si_alignmentBecomesEmpty(true);
        } else if (!cachedMa->isEmpty() && oldCachedMa->isEmpty()) {
            emit si_alignmentBecomesEmpty(false);
        }

        QString newName = cachedMa->getName();
        if (oldCachedMa->getName() != newName) {
            setGObjectNameNotDbi(newName);
        }
    }
    if (!removedRowIds.isEmpty()) {
        emit si_rowsRemoved(removedRowIds);
    }
    if (cachedMa->getAlphabet()->getId() != oldCachedMa->getAlphabet()->getId()) {
        emit si_alphabetChanged(mi, oldCachedMa->getAlphabet());
    }
}

void MultipleAlignmentObject::sortRowsByList(const QStringList& order) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    MultipleAlignment ma = getAlignment()->getCopy();
    ma->sortRowsByList(order);
    CHECK(ma->getRowsIds() != cachedMa->getRowsIds(), );

    U2OpStatusImpl os;
    MaDbiUtils::updateRowsOrder(entityRef, ma->getRowsIds(), os);
    SAFE_POINT_OP(os, );

    MaModificationInfo mi;
    mi.alignmentLengthChanged = false;
    mi.rowContentChanged = false;
    mi.rowListChanged = false;
    updateCachedMultipleAlignment(mi);
}

void MultipleAlignmentObject::insertCharacter(int rowIndex, int pos, char newChar) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    insertGap(U2Region(0, getRowCount()), pos, 1);
    replaceCharacter(pos, rowIndex, newChar);
}

void MultipleAlignmentObject::insertGap(const U2Region& rows, int pos, int nGaps) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    const MultipleAlignment& ma = getAlignment();
    int startSeq = rows.startPos;
    int endSeq = startSeq + rows.length;

    QList<qint64> rowIds;
    for (int i = startSeq; i < endSeq; ++i) {
        qint64 rowId = ma->getRow(i)->getRowId();
        rowIds.append(rowId);
    }
    insertGapByRowIdList(rowIds, pos, nGaps);
}

void MultipleAlignmentObject::insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps) {
    const MultipleAlignment& ma = getAlignment();
    QList<qint64> rowIds;
    for (int i = 0; i < rowIndexes.size(); i++) {
        int rowIndex = rowIndexes[i];
        qint64 rowId = ma->getRow(rowIndex)->getRowId();
        rowIds.append(rowId);
    }
    insertGapByRowIdList(rowIds, pos, nGaps);
}

void MultipleAlignmentObject::insertGapByRowIdList(const QList<qint64>& rowIds, int pos, int nGaps) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    U2OpStatus2Log os;
    bool collapseTrailingGaps = type == GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT;
    MsaDbiUtils::insertGaps(entityRef, rowIds, pos, nGaps, os, collapseTrailingGaps);
    SAFE_POINT_OP(os, );

    MaModificationInfo mi;
    mi.rowListChanged = false;
    mi.modifiedRowIds = rowIds;
    updateCachedMultipleAlignment(mi);
}

namespace {

template<typename T>
inline QList<T> mergeLists(const QList<T>& first, const QList<T>& second) {
    QList<T> result = first;
    foreach (const T& item, second) {
        if (!result.contains(item)) {
            result.append(item);
        }
    }
    return result;
}

QList<qint64> getRowsAffectedByDeletion(const MultipleAlignment& ma, const QList<qint64>& removedRowIds) {
    QList<qint64> rowIdsAffectedByDeletion;
    U2OpStatus2Log os;
    const QList<qint64> maRows = ma->getRowsIds();
    int previousRemovedRowIndex = -1;
    foreach (qint64 removedRowId, removedRowIds) {
        if (-1 != previousRemovedRowIndex) {
            const int currentRemovedRowIndex = ma->getRowIndexByRowId(removedRowId, os);
            SAFE_POINT_OP(os, QList<qint64>());
            SAFE_POINT(currentRemovedRowIndex > previousRemovedRowIndex, "Rows order violation", QList<qint64>());
            const int countOfUnchangedRowsBetween = currentRemovedRowIndex - previousRemovedRowIndex - 1;
            if (0 < countOfUnchangedRowsBetween) {
                for (int middleRowIndex = previousRemovedRowIndex + 1; middleRowIndex < currentRemovedRowIndex; ++middleRowIndex) {
                    rowIdsAffectedByDeletion += maRows[middleRowIndex];
                }
            }
        }
        previousRemovedRowIndex = ma->getRowIndexByRowId(removedRowId, os);
        SAFE_POINT_OP(os, QList<qint64>());
    }
    const int lastDeletedRowIndex = ma->getRowIndexByRowId(removedRowIds.last(), os);
    SAFE_POINT_OP(os, QList<qint64>());
    if (lastDeletedRowIndex < maRows.size() - 1) {  // if the last removed row was not in the bottom of the msa
        rowIdsAffectedByDeletion += maRows.mid(lastDeletedRowIndex + 1);
    }
    return rowIdsAffectedByDeletion;
}

}  // namespace

void MultipleAlignmentObject::removeRegion(const QList<int>& rowIndexes, int x, int width, bool removeEmptyRows) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleAlignment& ma = getAlignment();
    QList<qint64> modifiedRowIds = convertMaRowIndexesToMaRowIds(rowIndexes);
    U2OpStatus2Log os;
    removeRegionPrivate(os, entityRef, modifiedRowIds, x, width);
    SAFE_POINT_OP(os, );

    QList<qint64> removedRowIds;
    if (removeEmptyRows) {
        removedRowIds = MsaDbiUtils::removeEmptyRows(entityRef, modifiedRowIds, os);
        SAFE_POINT_OP(os, );
        if (!removedRowIds.isEmpty()) {
            // suppose that if at least one row in msa was removed then all the rows below it were changed
            QList<qint64> rowIdsAffectedByDeletion = getRowsAffectedByDeletion(ma, removedRowIds);
            foreach (qint64 removedRowId, removedRowIds) {  // removed rows ain't need to be updated.
                modifiedRowIds.removeAll(removedRowId);
            }
            modifiedRowIds = mergeLists(modifiedRowIds, rowIdsAffectedByDeletion);
        }
    }

    MaModificationInfo mi;
    mi.modifiedRowIds = modifiedRowIds;
    updateCachedMultipleAlignment(mi, removedRowIds);

    if (!removedRowIds.isEmpty()) {
        emit si_rowsRemoved(removedRowIds);
    }
}

void MultipleAlignmentObject::removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows, bool track) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    QList<qint64> modifiedRowIds;
    const MultipleAlignment& ma = getAlignment();
    const QVector<MultipleAlignmentRow>& maRows = ma->getRows();
    SAFE_POINT(nRows > 0 && startRow >= 0 && startRow + nRows <= maRows.size() && startPos + nBases <= ma->getLength(), "Invalid parameters", );
    QVector<MultipleAlignmentRow>::ConstIterator it = maRows.begin() + startRow;
    QVector<MultipleAlignmentRow>::ConstIterator end = it + nRows;
    for (; it != end; it++) {
        modifiedRowIds << (*it)->getRowId();
    }

    U2OpStatus2Log os;
    removeRegionPrivate(os, entityRef, modifiedRowIds, startPos, nBases);
    SAFE_POINT_OP(os, );

    QList<qint64> removedRows;
    if (removeEmptyRows) {
        removedRows = MsaDbiUtils::removeEmptyRows(entityRef, modifiedRowIds, os);
        SAFE_POINT_OP(os, );
        if (!removedRows.isEmpty()) {  // suppose that if at least one row in msa was removed then
            // all the rows below it were changed
            const QList<qint64> rowIdsAffectedByDeletion = getRowsAffectedByDeletion(ma, removedRows);
            foreach (qint64 removedRowId, removedRows) {  // removed rows ain't need to be update
                modifiedRowIds.removeAll(removedRowId);
            }
            modifiedRowIds = mergeLists(modifiedRowIds, rowIdsAffectedByDeletion);
        }
    }

    if (track || !removedRows.isEmpty()) {
        MaModificationInfo mi;
        mi.modifiedRowIds = modifiedRowIds;
        updateCachedMultipleAlignment(mi, removedRows);
    }

    if (!removedRows.isEmpty()) {
        emit si_rowsRemoved(removedRows);
    }
}

int MultipleAlignmentObject::deleteGap(U2OpStatus& os, const U2Region& rows, int pos, int maxGaps) {
    QList<int> rowIndexes;
    for (int i = (int)rows.startPos; i < (int)rows.endPos(); i++) {
        rowIndexes << i;
    }
    return deleteGapByRowIndexList(os, rowIndexes, pos, maxGaps);
}

static QList<int> toUniqueRowIndexes(const QList<int>& rowIndexes, int numRows) {
    QSet<int> uniqueRowIndexes;
    for (int i = 0; i < rowIndexes.size(); i++) {
        int rowIndex = rowIndexes[i];
        if (rowIndex >= 0 && rowIndex < numRows) {
            uniqueRowIndexes << rowIndex;
        }
    }
    return uniqueRowIndexes.toList();
}

int MultipleAlignmentObject::deleteGapByRowIndexList(U2OpStatus& os, const QList<int>& rowIndexes, int pos, int maxGaps) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", 0);

    int removingGapColumnCount = getMaxWidthOfGapRegion(os, rowIndexes, pos, maxGaps);
    SAFE_POINT_OP(os, 0);

    if (removingGapColumnCount == 0) {
        return 0;
    } else if (removingGapColumnCount < maxGaps) {
        pos += maxGaps - removingGapColumnCount;
    }
    QList<qint64> modifiedRowIds;
    MultipleAlignment msa = getMultipleAlignmentCopy();
    // iterate through given rows to update each of them in DB
    QList<int> uniqueRowIndexes = toUniqueRowIndexes(rowIndexes, getRowCount());
    for (int i = 0; i < rowIndexes.size(); i++) {
        int rowIndex = uniqueRowIndexes[i];
        msa->removeChars(rowIndex, pos, removingGapColumnCount, os);
        CHECK_OP(os, 0);

        MultipleAlignmentRow row = msa->getRow(rowIndex);
        MaDbiUtils::updateRowGapModel(entityRef, row->getRowId(), row->getGaps(), os);
        CHECK_OP(os, 0);
        modifiedRowIds << row->getRowId();
    }

    if (uniqueRowIndexes.size() == getRowCount()) {
        // delete columns
        MaDbiUtils::updateMaLength(entityRef, getLength() - removingGapColumnCount, os);
        CHECK_OP(os, 0);
    }

    MaModificationInfo mi;
    mi.rowListChanged = false;
    mi.modifiedRowIds = modifiedRowIds;
    updateCachedMultipleAlignment(mi);
    return removingGapColumnCount;
}

MultipleAlignment MultipleAlignmentObject::getCopy() const {
    return getAlignment()->getCopy();
}

int MultipleAlignmentObject::shiftRegion(int startPos, int startRow, int nBases, int nRows, int shift) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", 0);
    SAFE_POINT(!isRegionEmpty(startPos, startRow, nBases, nRows), "Region is empty", 0);
    SAFE_POINT(0 <= startPos && 0 <= startRow && 0 < nBases && 0 < nRows, "Invalid parameters of selected region encountered", 0);
    U2OpStatusImpl os;

    int n = 0;
    if (shift > 0) {
        // if last symbol selected - do not add gaps at the end
        if (startPos + nBases != getLength()) {
            // if some trailing gaps are selected --> save them!
            if (startPos + nBases + shift > getLength()) {
                bool increaseAlignmentLen = true;
                for (int i = startRow; i < startRow + nRows; i++) {
                    int rowLen = getRow(i)->getRowLengthWithoutTrailing();
                    if (rowLen >= startPos + nBases + shift) {
                        increaseAlignmentLen = false;
                        break;
                    }
                }
                if (increaseAlignmentLen) {
                    MaDbiUtils::updateMaLength(entityRef, startPos + nBases + shift, os);
                    SAFE_POINT_OP(os, 0);
                    updateCachedMultipleAlignment();
                }
            }
        }

        insertGap(U2Region(startRow, nRows), startPos, shift);
        n = shift;
    } else if (0 < startPos) {
        if (0 > startPos + shift) {
            shift = -startPos;
        }
        n = -deleteGap(os, U2Region(startRow, nRows), startPos + shift, -shift);
        SAFE_POINT_OP(os, 0);
    }
    return n;
}

void MultipleAlignmentObject::saveState() {
    const MultipleAlignment& ma = getAlignment();
    emit si_completeStateChanged(false);
    savedState.setState(ma);
}

void MultipleAlignmentObject::releaseState() {
    if (!isStateLocked()) {
        emit si_completeStateChanged(true);

        CHECK(savedState.hasState(), );
        MultipleAlignment maBefore = savedState.takeState();
        CHECK(*maBefore != *getAlignment(), );
        setModified(true);

        MaModificationInfo mi;
        emit si_alignmentChanged(maBefore, mi);

        if (cachedMa->isEmpty() && !maBefore->isEmpty()) {
            emit si_alignmentBecomesEmpty(true);
        } else if (!cachedMa->isEmpty() && maBefore->isEmpty()) {
            emit si_alignmentBecomesEmpty(false);
        }
    }
}

bool MultipleAlignmentObject::hasNonTrailingGap() const {
    const QList<QVector<U2MsaGap>>& listGapModel = getGapModel();
    if (std::any_of(listGapModel.constBegin(), listGapModel.constEnd(), [](const QVector<U2MsaGap>& data) { return !data.isEmpty(); })) {
        return true;
    }
    return false;
}

void MultipleAlignmentObject::loadDataCore(U2OpStatus& os) {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    loadAlignment(os);
}

int MultipleAlignmentObject::getMaxWidthOfGapRegion(U2OpStatus& os, const QList<int>& rowIndexes, int pos, int maxGaps) {
    const MultipleAlignment& ma = getAlignment();
    SAFE_POINT_EXT(pos >= 0 && maxGaps >= 0 && pos < ma->getLength(), os.setError("Illegal parameters of the gap region"), 0);

    // check if there is nothing to remove
    int maxRemovedGaps = qBound(0, maxGaps, ma->getLength() - pos);
    if (maxRemovedGaps == 0) {
        return 0;
    }
    QList<int> uniqueRowIndexes = toUniqueRowIndexes(rowIndexes, getRowCount());
    int removingGapColumnCount = maxRemovedGaps;
    bool isRegionInRowTrailingGaps = true;
    // iterate through given rows to determine the width of the continuous gap region
    for (int i = 0; i < uniqueRowIndexes.size(); i++) {
        int rowIndex = uniqueRowIndexes[i];
        int gapCountInCurrentRow = 0;
        // iterate through current row bases to determine gap count
        while (gapCountInCurrentRow < maxRemovedGaps) {
            if (!ma->isGap(rowIndex, pos + maxGaps - gapCountInCurrentRow - 1)) {
                break;
            }
            gapCountInCurrentRow++;
        }

        // determine if the given area intersects a row in the area of trailing gaps
        if (gapCountInCurrentRow != 0 && isRegionInRowTrailingGaps) {
            int trailingPosition = pos + maxRemovedGaps - gapCountInCurrentRow;
            if (trailingPosition != ma->getLength()) {
                while (ma->getLength() > trailingPosition && isRegionInRowTrailingGaps) {
                    isRegionInRowTrailingGaps = isRegionInRowTrailingGaps && ma->isGap(rowIndex, trailingPosition);
                    trailingPosition++;
                }
            }
        } else if (isRegionInRowTrailingGaps) {
            isRegionInRowTrailingGaps = false;
        }

        if (gapCountInCurrentRow == 0) {
            // don't do anything if there is a row without gaps
            return 0;
        }
        removingGapColumnCount = qMin(removingGapColumnCount, gapCountInCurrentRow);
    }

    if (isRegionInRowTrailingGaps) {
        if (uniqueRowIndexes.size() == getRowCount()) {
            return qMin((int)getLength() - pos, maxGaps);
        } else {
            return 0;
        }
    }

    return removingGapColumnCount;
}

QList<qint64> MultipleAlignmentObject::convertMaRowIndexesToMaRowIds(const QList<int>& maRowIndexes, bool excludeErrors) {
    QList<qint64> ids;
    const QVector<MultipleAlignmentRow>& rows = getAlignment()->getRows();
    for (int i = 0; i < maRowIndexes.length(); i++) {
        int index = maRowIndexes[i];
        bool isValid = index >= 0 && index <= rows.size() - 1;
        if (isValid) {
            ids << rows[index]->getRowId();
        } else if (!excludeErrors) {
            ids << -1;
        }
    }
    return ids;
}

QList<int> MultipleAlignmentObject::convertMaRowIdsToMaRowIndexes(const QList<qint64>& maRowIds, bool excludeErrors) {
    QList<int> indexes;
    const QVector<MultipleAlignmentRow>& rows = getAlignment()->getRows();
    for (int i = 0; i < maRowIds.length(); i++) {
        int rowId = maRowIds[i];
        int index = -1;
        for (int j = 0; j < rows.size(); j++) {
            const MultipleAlignmentRow& row = rows[j];
            if (row->getRowId() == rowId) {
                index = j;
                break;
            }
        }
        bool isValid = index >= 0;
        if (isValid) {
            indexes << index;
        } else if (!excludeErrors) {
            indexes << -1;
        }
    }
    return indexes;
}

char MultipleAlignmentObject::charAt(int seqNum, qint64 position) const {
    return getAlignment()->charAt(seqNum, position);
}

void MultipleAlignmentObject::crop(const QList<qint64>& rowIds, const U2Region& columnRange) {
    SAFE_POINT(type == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, "This method is only tested for MSA", );
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    U2OpStatus2Log os;
    MsaDbiUtils::crop(entityRef, rowIds, columnRange, os);
    SAFE_POINT_OP(os, );

    updateCachedMultipleAlignment();
}

void MultipleAlignmentObject::crop(const U2Region& columnRange) {
    crop(getRowIds(), columnRange);
}

void MultipleAlignmentObject::trimRow(const int rowIndex, int currentPos, U2OpStatus& os, TrimEdge edge) {
    U2EntityRef entityRef = getEntityRef();
    MultipleAlignmentRow row = getRow(rowIndex);
    int rowId = row->getRowId();
    int pos = 0;
    int count = 0;
    switch (edge) {
        case Left:
            pos = row->getCoreStart();
            count = currentPos - pos;
            break;
        case Right:
            pos = currentPos + 1;
            int lengthWithoutTrailing = row->getRowLengthWithoutTrailing();
            count = lengthWithoutTrailing - currentPos;
            break;
    }
    MsaDbiUtils::removeRegion(entityRef, {rowId}, pos, count, os);
    U2Region region(rowIndex, 1);
    if (edge == Left) {
        insertGap(region, 0, count);
    }

    MaModificationInfo modificationInfo;
    modificationInfo.rowContentChanged = true;
    modificationInfo.rowListChanged = false;
    updateCachedMultipleAlignment(modificationInfo);
}

void MultipleAlignmentObject::updateAlternativeMutations(bool showAlternativeMutations, int threshold, U2OpStatus& os) {
    for (int i = 0; i < getRowCount(); i++) {
        const MultipleAlignmentRow& mcaRow = getRow(i);
        qint64 ungappedLength = mcaRow->getUngappedLength();

        QHash<qint64, char> newCharMap;
        for (int j = 0; j < ungappedLength; j++) {
            bool ok = false;
            auto res = mcaRow->getTwoHighestPeaks(j, ok);
            if (!ok) {
                continue;
            }

            double minimumThresholdValue = (double)res.second.value / res.first.value * 100;
            DNAChromatogram::Trace trace = (minimumThresholdValue < threshold || !showAlternativeMutations ? res.first : res.second).trace;
            char newChar = DNAChromatogram::BASE_BY_TRACE.value((int)trace);

            auto gappedPos = mcaRow->getGappedPosition(j);
            char currentChar = mcaRow->charAt(gappedPos);
            if (currentChar == newChar) {
                continue;
            }

            newCharMap.insert(gappedPos, newChar);
        }

        const MultipleAlignment& ma = getAlignment();
        qint64 modifiedRowId = ma->getRow(i)->getRowId();
        MsaDbiUtils::replaceCharactersInRow(getEntityRef(), modifiedRowId, newCharMap, os);
        SAFE_POINT_OP(os, );
    }
    updateCachedMultipleAlignment();
}

void MultipleAlignmentObject::loadAlignment(U2OpStatus& os) {
    cachedMa = MsaExportUtils::loadAlignment(entityRef.dbiRef, entityRef.entityId, os);
}

void MultipleAlignmentObject::updateCachedRows(U2OpStatus& os, const QList<qint64>& rowIds) {
    QList<MsaRowSnapshot> snapshots = MsaExportUtils::loadRows(entityRef.dbiRef, entityRef.entityId, rowIds, os);
    CHECK_OP(os, );
    SAFE_POINT(snapshots.length() == rowIds.length(), "Row count does not match", );
    for (int i = 0; i < rowIds.length(); i++) {
        qint64 rowId = rowIds[i];
        const MsaRowSnapshot& row = snapshots[i];
        int rowIndex = cachedMa->getRowIndexByRowId(rowId, os);
        SAFE_POINT_OP(os, );
        if (type == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
            cachedMa->setRowContent(rowIndex, row.sequence.seq);
            cachedMa->setRowGapModel(rowIndex, row.gaps);
        } else {
            cachedMa->setRowContent(rowIndex, row.chromatogram, row.sequence, row.gaps);
        }
        cachedMa->renameRow(rowIndex, row.sequence.getName());
    }
}

void MultipleAlignmentObject::updateDatabase(U2OpStatus& os, const MultipleAlignment& ma) {
    MsaDbiUtils::updateMsa(entityRef, ma, os);
}

void MultipleAlignmentObject::removeRowPrivate(U2OpStatus& os, const U2EntityRef& msaRef, qint64 rowId) {
    MsaDbiUtils::removeRow(msaRef, rowId, os);
}

void MultipleAlignmentObject::removeRegionPrivate(U2OpStatus& os, const U2EntityRef& maRef, const QList<qint64>& rows, int startPos, int nBases) {
    MsaDbiUtils::removeRegion(maRef, rows, startPos, nBases, os);
}

void MultipleAlignmentObject::updateGapModel(U2OpStatus& os, const QMap<qint64, QVector<U2MsaGap>>& rowsGapModel) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleAlignment msa = getAlignment();

    QList<qint64> rowIds = msa->getRowsIds();
    QList<qint64> modifiedRowIds;
    foreach (qint64 rowId, rowsGapModel.keys()) {
        if (!rowIds.contains(rowId)) {
            os.setError(QString("Can't update gaps of a multiple alignment: cannot find a row with the id %1").arg(rowId));
            return;
        }

        MaDbiUtils::updateRowGapModel(entityRef, rowId, rowsGapModel.value(rowId), os);
        CHECK_OP(os, );
        modifiedRowIds.append(rowId);
    }

    MaModificationInfo mi;
    mi.rowListChanged = false;
    mi.modifiedRowIds = modifiedRowIds;
    updateCachedMultipleAlignment(mi);
}

void MultipleAlignmentObject::updateGapModel(const QList<MultipleAlignmentRow>& sourceRows) {
    QList<MultipleAlignmentRow> oldRows = getAlignment()->getRows().toList();

    SAFE_POINT(oldRows.count() == sourceRows.count(), "Different rows count", );

    QMap<qint64, QVector<U2MsaGap>> newGapModel;
    QList<MultipleAlignmentRow>::ConstIterator oldRowsIterator = oldRows.begin();
    QList<MultipleAlignmentRow>::ConstIterator sourceRowsIterator = sourceRows.begin();
    for (; oldRowsIterator != oldRows.end(); oldRowsIterator++, sourceRowsIterator++) {
        newGapModel[(*oldRowsIterator)->getRowId()] = (*sourceRowsIterator)->getGaps();
    }

    U2OpStatus2Log os;
    updateGapModel(os, newGapModel);
}

void MultipleAlignmentObject::updateRow(U2OpStatus& os, int rowIdx, const QString& name, const QByteArray& seqBytes, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleAlignment msa = getAlignment();
    SAFE_POINT(rowIdx >= 0 && rowIdx < msa->getRowCount(), "Invalid row index", );
    qint64 rowId = msa->getRow(rowIdx)->getRowId();

    MsaDbiUtils::updateRowContent(entityRef, rowId, seqBytes, gapModel, os);
    CHECK_OP(os, );

    MaDbiUtils::renameRow(entityRef, rowId, name, os);
    CHECK_OP(os, );
}

void MultipleAlignmentObject::replaceAllCharacters(char oldChar, char newChar, const DNAAlphabet* newAlphabet) {
    SAFE_POINT(type == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, "The method is tested only with Msa", );
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    SAFE_POINT(oldChar != U2Msa::GAP_CHAR && newChar != U2Msa::GAP_CHAR, "Gap characters replacement is not supported", );
    CHECK(oldChar != newChar, );

    U2OpStatus2Log os;
    QList<qint64> modifiedRowIds = MsaDbiUtils::replaceNonGapCharacter(entityRef, oldChar, newChar, os);
    CHECK_OP(os, );
    if (modifiedRowIds.isEmpty() && newAlphabet == getAlphabet()) {
        return;
    }

    MaModificationInfo mi;
    mi.rowContentChanged = true;
    mi.rowListChanged = false;
    mi.alignmentLengthChanged = false;
    mi.modifiedRowIds = modifiedRowIds;

    if (newAlphabet != nullptr && newAlphabet != getAlphabet()) {
        MaDbiUtils::updateMaAlphabet(entityRef, newAlphabet->getId(), os);
        SAFE_POINT_OP(os, );
        mi.alphabetChanged = true;
    }
    if (!mi.alphabetChanged && mi.modifiedRowIds.isEmpty()) {
        return;  // Nothing changed.
    }
    updateCachedMultipleAlignment(mi);
}

void MultipleAlignmentObject::morphAlphabet(const DNAAlphabet* newAlphabet, const QByteArray& replacementMap) {
    SAFE_POINT(type == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, "The method is tested only with Msa", );
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    SAFE_POINT(newAlphabet != nullptr, "newAlphabet is null!", );
    U2OpStatus2Log os;
    QList<qint64> modifiedRowIds = MsaDbiUtils::keepOnlyAlphabetChars(entityRef, newAlphabet, replacementMap, os);
    CHECK_OP(os, );
    if (modifiedRowIds.isEmpty() && newAlphabet == getAlphabet()) {
        return;
    }

    MaModificationInfo mi;
    mi.rowContentChanged = true;
    mi.rowListChanged = false;
    mi.alignmentLengthChanged = false;
    mi.modifiedRowIds = modifiedRowIds;

    if (newAlphabet != getAlphabet()) {
        MaDbiUtils::updateMaAlphabet(entityRef, newAlphabet->getId(), os);
        SAFE_POINT_OP(os, );
        mi.alphabetChanged = true;
    }
    if (!mi.alphabetChanged && mi.modifiedRowIds.isEmpty()) {
        return;  // Nothing changed.
    }
    updateCachedMultipleAlignment(mi);
}

}  // namespace U2
