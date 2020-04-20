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

#include <U2Core/GHints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MultipleSequenceAlignmentObject.h"

namespace U2 {

MultipleSequenceAlignmentObject::MultipleSequenceAlignmentObject(const QString &name,
    const U2EntityRef &msaRef,
    const QVariantMap &hintsMap,
    const MultipleSequenceAlignment &alnData)
    : MultipleAlignmentObject(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, name, msaRef, hintsMap, alnData) {

}

const MultipleSequenceAlignment MultipleSequenceAlignmentObject::getMsa() const {
    return getMultipleAlignment().dynamicCast<MultipleSequenceAlignment>();
}

const MultipleSequenceAlignment MultipleSequenceAlignmentObject::getMsaCopy() const {
    return getMsa()->getExplicitCopy();
}

MultipleSequenceAlignmentObject* MultipleSequenceAlignmentObject::clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints) const {
    DbiOperationsBlock opBlock(dstDbiRef, os);
    Q_UNUSED(opBlock);
    CHECK_OP(os, NULL);

    QScopedPointer<GHintsDefaultImpl> gHints(new GHintsDefaultImpl(getGHintsMap()));
    gHints->setAll(hints);
    const QString dstFolder = gHints->get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    MultipleSequenceAlignment msa = getMsa()->getExplicitCopy();
    MultipleSequenceAlignmentObject *clonedObj = MultipleSequenceAlignmentImporter::createAlignment(dstDbiRef, dstFolder, msa, os);
    CHECK_OP(os, NULL);

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

void MultipleSequenceAlignmentObject::updateGapModel(U2OpStatus &os, const U2MsaMapGapModel &rowsGapModel) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleSequenceAlignment msa = getMultipleAlignment();

    const QList<qint64> rowIds = msa->getRowsIds();
    QList<qint64> modifiedRowIds;
    foreach(qint64 rowId, rowsGapModel.keys()) {
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

void MultipleSequenceAlignmentObject::updateGapModel(const QList<MultipleSequenceAlignmentRow> &sourceRows) {
    const QList<MultipleSequenceAlignmentRow> oldRows = getMsa()->getMsaRows();

    SAFE_POINT(oldRows.count() == sourceRows.count(), "Different rows count", );

    U2MsaMapGapModel newGapModel;
    QList<MultipleSequenceAlignmentRow>::ConstIterator oldRowsIterator = oldRows.begin();
    QList<MultipleSequenceAlignmentRow>::ConstIterator sourceRowsIterator = sourceRows.begin();
    for (; oldRowsIterator != oldRows.end(); oldRowsIterator++, sourceRowsIterator++) {
        newGapModel[(*oldRowsIterator)->getRowId()] = (*sourceRowsIterator)->getGapModel();
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

void MultipleSequenceAlignmentObject::crop(const U2Region &window, const QSet<QString> &rowNames) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    const MultipleSequenceAlignment &ma = getMultipleAlignment();

    QList<qint64> rowIds;
    for (int i = 0; i < ma->getNumRows(); ++i) {
        QString rowName = ma->getRow(i)->getName();
        if (rowNames.isEmpty() || rowNames.contains(rowName)) {
            qint64 rowId = ma->getRow(i)->getRowId();
            rowIds.append(rowId);
        }
    }

    U2OpStatus2Log os;
    MsaDbiUtils::crop(entityRef, rowIds, window.startPos, window.length, os);
    SAFE_POINT_OP(os, );

    updateCachedMultipleAlignment();
}

void MultipleSequenceAlignmentObject::crop(const U2Region &window, const QList<qint64> &rowIds) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    U2OpStatus2Log os;
    MsaDbiUtils::crop(entityRef, rowIds, window.startPos, window.length, os);
    SAFE_POINT_OP(os, );

    updateCachedMultipleAlignment();
}

void MultipleSequenceAlignmentObject::crop(const U2Region &window) {
    crop(window, QSet<QString>());
}

void MultipleSequenceAlignmentObject::updateRow(U2OpStatus &os, int rowIdx, const QString &name, const QByteArray &seqBytes, const U2MsaRowGapModel &gapModel) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );

    const MultipleSequenceAlignment msa = getMultipleAlignment();
    SAFE_POINT(rowIdx >= 0 && rowIdx < msa->getNumRows(), "Invalid row index", );
    qint64 rowId = msa->getRow(rowIdx)->getRowId();

    MsaDbiUtils::updateRowContent(entityRef, rowId, seqBytes, gapModel, os);
    CHECK_OP(os, );

    MaDbiUtils::renameRow(entityRef, rowId, name, os);
    CHECK_OP(os, );
}

void MultipleSequenceAlignmentObject::replaceCharacter(int startPos, int rowIndex, char newChar) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    const MultipleSequenceAlignment msa = getMultipleAlignment();
    SAFE_POINT(rowIndex >= 0 && startPos + 1 <= msa->getLength(), "Invalid parameters", );
    qint64 modifiedRowId = msa->getRow(rowIndex)->getRowId();
    qint64 rowLength = msa->getRow(rowIndex)->getCoreLength();

    U2OpStatus2Log os;
    bool wasRowRemoved = false;
    if (newChar != U2Msa::GAP_CHAR) {
        MsaDbiUtils::replaceCharacterInRow(entityRef, modifiedRowId, startPos, newChar, os);
    } else {
        if (rowLength == 1) {
            MsaDbiUtils::removeRow(entityRef, modifiedRowId, os);
            wasRowRemoved = true;
        } else {
            MsaDbiUtils::removeRegion(entityRef, QList<qint64>() << modifiedRowId, startPos, 1, os);
            MsaDbiUtils::insertGaps(entityRef, QList<qint64>() << modifiedRowId, startPos, 1, os, false);
        }
    }
    SAFE_POINT_OP(os, );

    MaModificationInfo mi;
    if (wasRowRemoved) {
        mi.rowListChanged = true;
    } else {
        mi.rowContentChanged = true;
        mi.rowListChanged = false;
        mi.alignmentLengthChanged = false;
        mi.modifiedRowIds << modifiedRowId;
    }

    if (newChar != ' ' && !msa->getAlphabet()->contains(newChar)) {
        const DNAAlphabet *alp = U2AlphabetUtils::findBestAlphabet(QByteArray(1, newChar));
        const DNAAlphabet *newAlphabet = U2AlphabetUtils::deriveCommonAlphabet(alp, msa->getAlphabet());
        SAFE_POINT(NULL != newAlphabet, "Common alphabet is NULL", );

        if (newAlphabet->getId() != msa->getAlphabet()->getId()) {
            MaDbiUtils::updateMaAlphabet(entityRef, newAlphabet->getId(), os);
            mi.alphabetChanged = true;
            SAFE_POINT_OP(os, );
        }
    }

    if (wasRowRemoved) {
        updateCachedMultipleAlignment(mi, QList<qint64>() << modifiedRowId);
    } else {
        updateCachedMultipleAlignment(mi);
    }
}

void MultipleSequenceAlignmentObject::deleteColumnsWithGaps(U2OpStatus &os, int requiredGapsCount) {
    const QList<U2Region> regionsToDelete = MSAUtils::getColumnsWithGaps(getGapModel(), getLength(), requiredGapsCount);
    CHECK(!regionsToDelete.isEmpty(), );
    CHECK(regionsToDelete.first().length != getLength(), );

    for (int n = regionsToDelete.size(), i = n - 1; i >= 0; i--) {
        removeRegion(regionsToDelete[i].startPos, 0, regionsToDelete[i].length, getNumRows(), true, false);
        os.setProgress(100 * (n - i) / n);
    }
    updateCachedMultipleAlignment();
}

void MultipleSequenceAlignmentObject::loadAlignment(U2OpStatus &os) {
    MultipleSequenceAlignmentExporter msaExporter;
    cachedMa = msaExporter.getAlignment(entityRef.dbiRef, entityRef.entityId, os);
}

void MultipleSequenceAlignmentObject::updateCachedRows(U2OpStatus &os, const QList<qint64> &rowIds) {
    MultipleSequenceAlignment cachedMsa = cachedMa.dynamicCast<MultipleSequenceAlignment>();

    MultipleSequenceAlignmentExporter msaExporter;
    QList<MsaRowReplacementData> rowsAndSeqs = msaExporter.getAlignmentRows(entityRef.dbiRef, entityRef.entityId, rowIds, os);
    SAFE_POINT_OP(os, );
    foreach(const MsaRowReplacementData &data, rowsAndSeqs) {
        const int rowIndex = cachedMsa->getRowIndexByRowId(data.row.rowId, os);
        SAFE_POINT_OP(os, );
        cachedMsa->setRowContent(rowIndex, data.sequence.seq);
        cachedMsa->setRowGapModel(rowIndex, data.row.gaps);
        cachedMsa->renameRow(rowIndex, data.sequence.getName());
    }
}

void MultipleSequenceAlignmentObject::updateDatabase(U2OpStatus &os, const MultipleAlignment &ma) {
    const MultipleSequenceAlignment msa = ma.dynamicCast<MultipleSequenceAlignment>();
    MsaDbiUtils::updateMsa(entityRef, msa, os);
}

void MultipleSequenceAlignmentObject::removeRowPrivate(U2OpStatus &os, const U2EntityRef &msaRef, qint64 rowId) {
    MsaDbiUtils::removeRow(msaRef, rowId, os);
}

void MultipleSequenceAlignmentObject::removeRegionPrivate(U2OpStatus &os, const U2EntityRef &maRef, const QList<qint64> &rows,
    int startPos, int nBases) {
    MsaDbiUtils::removeRegion(maRef, rows, startPos, nBases, os);
}

}   // namespace U2
