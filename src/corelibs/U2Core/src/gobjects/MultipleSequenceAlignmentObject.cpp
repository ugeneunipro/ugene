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

#include "MultipleSequenceAlignmentObject.h"

#include <U2Core/GHints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

MultipleSequenceAlignmentObject::MultipleSequenceAlignmentObject(const QString& name,
                                                                 const U2EntityRef& msaRef,
                                                                 const QVariantMap& hintsMap,
                                                                 const MultipleSequenceAlignment& alnData)
    : MultipleAlignmentObject(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, name, msaRef, hintsMap, alnData) {
}

const MultipleSequenceAlignment& MultipleSequenceAlignmentObject::getMsa() const {
    return getMultipleAlignment().dynamicCast<const MultipleSequenceAlignment&>();
}

const MultipleSequenceAlignment MultipleSequenceAlignmentObject::getMsaCopy() const {
    return getMsa()->getExplicitCopy();
}

MultipleSequenceAlignmentObject* MultipleSequenceAlignmentObject::clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints) const {
    DbiOperationsBlock opBlock(dstDbiRef, os);
    CHECK_OP(os, nullptr);

    QScopedPointer<GHintsDefaultImpl> gHints(new GHintsDefaultImpl(getGHintsMap()));
    gHints->setAll(hints);
    const QString dstFolder = gHints->get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    MultipleSequenceAlignment msa = getMsa()->getExplicitCopy();
    MultipleSequenceAlignmentObject* clonedObj = MultipleSequenceAlignmentImporter::createAlignment(dstDbiRef, dstFolder, msa, os);
    CHECK_OP(os, nullptr);

    clonedObj->setGHints(gHints.take());
    clonedObj->setIndexInfo(getIndexInfo());
    return clonedObj;
}

char MultipleSequenceAlignmentObject::charAt(int seqNum, qint64 position) const {
    return getMultipleAlignment()->charAt(seqNum, position);
}

const MultipleSequenceAlignmentRow MultipleSequenceAlignmentObject::getMsaRow(int row) const {
    return getRow(row).dynamicCast<MultipleSequenceAlignmentRow>();
}

void MultipleSequenceAlignmentObject::updateGapModel(U2OpStatus& os, const QMap<qint64, QVector<U2MsaGap>>& rowsGapModel) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleSequenceAlignment msa = getMultipleAlignment();

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

void MultipleSequenceAlignmentObject::updateGapModel(const QList<MultipleSequenceAlignmentRow>& sourceRows) {
    const QList<MultipleSequenceAlignmentRow> oldRows = getMsa()->getMsaRows();

    SAFE_POINT(oldRows.count() == sourceRows.count(), "Different rows count", );

    QMap<qint64, QVector<U2MsaGap>> newGapModel;
    QList<MultipleSequenceAlignmentRow>::ConstIterator oldRowsIterator = oldRows.begin();
    QList<MultipleSequenceAlignmentRow>::ConstIterator sourceRowsIterator = sourceRows.begin();
    for (; oldRowsIterator != oldRows.end(); oldRowsIterator++, sourceRowsIterator++) {
        newGapModel[(*oldRowsIterator)->getRowId()] = (*sourceRowsIterator)->getGaps();
    }

    U2OpStatus2Log os;
    updateGapModel(os, newGapModel);
}

void MultipleSequenceAlignmentObject::insertGap(const U2Region& rows, int pos, int nGaps) {
    MultipleAlignmentObject::insertGap(rows, pos, nGaps, false);
}

void MultipleSequenceAlignmentObject::insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps) {
    MultipleAlignmentObject::insertGapByRowIndexList(rowIndexes, pos, nGaps, false);
}

void MultipleSequenceAlignmentObject::crop(const QList<qint64>& rowIds, const U2Region& columnRange) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    U2OpStatus2Log os;
    MsaDbiUtils::crop(entityRef, rowIds, columnRange, os);
    SAFE_POINT_OP(os, );

    updateCachedMultipleAlignment();
}

void MultipleSequenceAlignmentObject::crop(const U2Region& columnRange) {
    crop(getRowIds(), columnRange);
}

void MultipleSequenceAlignmentObject::updateRow(U2OpStatus& os, int rowIdx, const QString& name, const QByteArray& seqBytes, const QVector<U2MsaGap>& gapModel) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleSequenceAlignment msa = getMultipleAlignment();
    SAFE_POINT(rowIdx >= 0 && rowIdx < msa->getRowCount(), "Invalid row index", );
    qint64 rowId = msa->getRow(rowIdx)->getRowId();

    MsaDbiUtils::updateRowContent(entityRef, rowId, seqBytes, gapModel, os);
    CHECK_OP(os, );

    MaDbiUtils::renameRow(entityRef, rowId, name, os);
    CHECK_OP(os, );
}

void MultipleSequenceAlignmentObject::replaceAllCharacters(char oldChar, char newChar, const DNAAlphabet* newAlphabet) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    SAFE_POINT(oldChar != U2Msa::GAP_CHAR && newChar != U2Msa::GAP_CHAR, "Gap characters replacement is not supported", );
    if (oldChar == newChar) {
        return;
    }

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

void MultipleSequenceAlignmentObject::morphAlphabet(const DNAAlphabet* newAlphabet, const QByteArray& replacementMap) {
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

void MultipleSequenceAlignmentObject::deleteColumnsWithGaps(U2OpStatus& os, int requiredGapsCount) {
    const QList<U2Region> regionsToDelete = MSAUtils::getColumnsWithGaps(getGapModel(), getRows(), getLength(), requiredGapsCount);
    CHECK(!regionsToDelete.isEmpty(), );
    CHECK(regionsToDelete.first().length != getLength(), );

    for (int n = regionsToDelete.size(), i = n - 1; i >= 0; i--) {
        removeRegion(regionsToDelete[i].startPos, 0, regionsToDelete[i].length, getRowCount(), true, false);
        os.setProgress(100 * (n - i) / n);
    }
    updateCachedMultipleAlignment();
}

void MultipleSequenceAlignmentObject::loadAlignment(U2OpStatus& os) {
    MultipleSequenceAlignmentExporter msaExporter;
    cachedMa = msaExporter.getAlignment(entityRef.dbiRef, entityRef.entityId, os);
}

void MultipleSequenceAlignmentObject::updateCachedRows(U2OpStatus& os, const QList<qint64>& rowIds) {
    MultipleSequenceAlignment cachedMsa = cachedMa.dynamicCast<MultipleSequenceAlignment>();

    MultipleSequenceAlignmentExporter msaExporter;
    QList<MsaRowReplacementData> rowsAndSeqs = msaExporter.getAlignmentRows(entityRef.dbiRef, entityRef.entityId, rowIds, os);
    SAFE_POINT_OP(os, );
    foreach (const MsaRowReplacementData& data, rowsAndSeqs) {
        const int rowIndex = cachedMsa->getRowIndexByRowId(data.row.rowId, os);
        SAFE_POINT_OP(os, );
        cachedMsa->setRowContent(rowIndex, data.sequence.seq);
        cachedMsa->setRowGapModel(rowIndex, data.row.gaps);
        cachedMsa->renameRow(rowIndex, data.sequence.getName());
    }
}

void MultipleSequenceAlignmentObject::updateDatabase(U2OpStatus& os, const MultipleAlignment& ma) {
    const MultipleSequenceAlignment msa = ma.dynamicCast<MultipleSequenceAlignment>();
    MsaDbiUtils::updateMsa(entityRef, msa, os);
}

void MultipleSequenceAlignmentObject::removeRowPrivate(U2OpStatus& os, const U2EntityRef& msaRef, qint64 rowId) {
    MsaDbiUtils::removeRow(msaRef, rowId, os);
}

void MultipleSequenceAlignmentObject::removeRegionPrivate(U2OpStatus& os, const U2EntityRef& maRef, const QList<qint64>& rows, int startPos, int nBases) {
    MsaDbiUtils::removeRegion(maRef, rows, startPos, nBases, os);
}

}  // namespace U2
