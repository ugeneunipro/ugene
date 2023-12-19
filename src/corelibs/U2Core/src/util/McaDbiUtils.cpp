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

#include "McaDbiUtils.h"

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogram.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignment.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

void McaDbiUtils::updateMca(U2OpStatus& os, const U2EntityRef& mcaRef, const MultipleChromatogramAlignment& mca) {
    // Move to the MCAImporter
    // TODO: check, if a transaction or an operation block should be started
    DbiConnection connection(mcaRef.dbiRef, os);
    CHECK_OP(os, );

    U2AttributeDbi* attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(attributeDbi != nullptr, os.setError("NULL Attribute Dbi"), );

    U2MsaDbi* msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(msaDbi != nullptr, os.setError("NULL Msa Dbi"), );

    U2SequenceDbi* sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(sequenceDbi != nullptr, os.setError("NULL Sequence Dbi"), );

    const DNAAlphabet* alphabet = mca->getAlphabet();
    SAFE_POINT_EXT(alphabet != nullptr, os.setError("The alignment alphabet is NULL"), );

    //// UPDATE MCA
    U2Msa dbMca(U2Type::Mca);
    dbMca.id = mcaRef.entityId;
    dbMca.visualName = mca->getName();
    dbMca.alphabet.id = alphabet->getId();
    dbMca.length = mca->getLength();

    msaDbi->updateMsaName(mcaRef.entityId, mca->getName(), os);
    CHECK_OP(os, );

    msaDbi->updateMsaAlphabet(mcaRef.entityId, alphabet->getId(), os);
    CHECK_OP(os, );

    msaDbi->updateMsaLength(mcaRef.entityId, mca->getLength(), os);
    CHECK_OP(os, );

    //// UPDATE ROWS AND SEQUENCES
    // Get rows that are currently stored in the database
    QList<U2MsaRow> currentRows = MsaDbiUtils::getMsaRows(os, mcaRef);
    CHECK_OP(os, );

    QList<qint64> currentRowIds;
    QList<qint64> newRowsIds = mca->getRowsIds();
    QList<qint64> eliminatedRows;
    // TODO: get the mca folder and create child objects there
    const QString dbFolder = U2ObjectDbi::ROOT_FOLDER;

    foreach (const U2MsaRow& currentRow, currentRows) {
        currentRowIds << currentRow.rowId;

        // Update data for rows with the same row and child objects IDs
        if (newRowsIds.contains(currentRow.rowId)) {
            // Update sequence and row info
            U2MsaRow newRow = mca->getMcaRowByRowId(currentRow.rowId, os)->getRowDbInfo();
            CHECK_OP(os, );

            if (newRow.chromatogramId != currentRow.chromatogramId || newRow.sequenceId != currentRow.sequenceId) {
                // Kill the row from the current alignment, it is incorrect. New row with this ID will be created later.
                // TODO: replace with specific utils
                MsaDbiUtils::removeRow(mcaRef, currentRow.rowId, os);
                CHECK_OP(os, );

                currentRowIds.removeOne(currentRow.rowId);
                continue;
            }

            const McaRowMemoryData rowMemoryData = mca->getMcaRowByRowId(newRow.rowId, os)->getRowMemoryData();
            CHECK_OP(os, );

            msaDbi->updateRowName(mcaRef.entityId, newRow.rowId, rowMemoryData.sequence.getName(), os);
            CHECK_OP(os, );

            msaDbi->updateRowContent(mcaRef.entityId, newRow.rowId, rowMemoryData.sequence.seq, rowMemoryData.gapModel, os);
            CHECK_OP(os, );

            ChromatogramUtils::updateChromatogramData(os, mcaRef.entityId, U2EntityRef(mcaRef.dbiRef, newRow.chromatogramId), rowMemoryData.chromatogram);
            CHECK_OP(os, );
        } else {
            // Remove rows that are no more present in the alignment
            eliminatedRows.append(currentRow.rowId);
        }
    }

    msaDbi->removeRows(mcaRef.entityId, eliminatedRows, os);
    CHECK_OP(os, );

    // Add rows that are stored in memory, but are not present in the database,
    // remember the rows order
    QList<qint64> rowsOrder;
    for (int i = 0, n = mca->getRowCount(); i < n; ++i) {
        const MultipleChromatogramAlignmentRow mcaRow = mca->getMcaRow(i);
        U2MsaRow dbRow = mcaRow->getRowDbInfo();

        if (!dbRow.hasValidChildObjectIds() || !currentRowIds.contains(dbRow.rowId)) {
            // Import the child objects
            const U2EntityRef chromatogramRef = ChromatogramUtils::import(os, connection.dbi->getDbiRef(), dbFolder, mcaRow->getChromatogram());
            CHECK_OP(os, );

            const U2EntityRef sequenceRef = U2SequenceUtils::import(os, connection.dbi->getDbiRef(), dbFolder, mcaRow->getSequence(), dbMca.alphabet.id);
            CHECK_OP(os, );

            // Create the row
            dbRow.rowId = U2MsaRow::INVALID_ROW_ID;  // set the row ID automatically
            dbRow.chromatogramId = chromatogramRef.entityId;
            dbRow.sequenceId = sequenceRef.entityId;
            dbRow.gstart = 0;
            dbRow.gend = mcaRow->getRowLength();
            dbRow.gaps = mcaRow->getGaps();

            MsaDbiUtils::addRow(mcaRef, -1, dbRow, os);
            CHECK_OP(os, );
        }
        rowsOrder << dbRow.rowId;
    }

    //// UPDATE ROWS POSITIONS
    msaDbi->setNewRowsOrder(mcaRef.entityId, rowsOrder, os);

    //// UPDATE ALIGNMENT ATTRIBUTES
    QVariantMap info = mca->getInfo();

    foreach (const QString& key, info.keys()) {
        QString value = info.value(key).toString();
        U2StringAttribute attribute(mcaRef.entityId, key, value);

        attributeDbi->createStringAttribute(attribute, os);
        CHECK_OP(os, );
    }
}

void U2::McaDbiUtils::replaceCharactersInRow(const U2EntityRef& mcaRef, qint64 rowId, QHash<qint64, char> newCharList, U2OpStatus& os) {
    QScopedPointer<DbiConnection> con(MaDbiUtils::getCheckedConnection(mcaRef.dbiRef, os));
    CHECK_OP(os, );

    U2MsaDbi* msaDbi = con->dbi->getMsaDbi();
    U2SequenceDbi* sequenceDbi = con->dbi->getSequenceDbi();

    MaDbiUtils::validateRowIds(msaDbi, mcaRef.entityId, QList<qint64>() << rowId, os);
    CHECK_OP(os, );

    U2MsaRow row = MsaDbiUtils::getMsaRow(os, mcaRef, rowId);
    CHECK_OP(os, );

    qint64 msaLength = msaDbi->getMsaLength(mcaRef.entityId, os);
    CHECK_OP(os, );

    U2Region seqReg(0, row.length);
    QByteArray seq = sequenceDbi->getSequenceData(row.sequenceId, seqReg, os);
    CHECK_OP(os, );

    for (const qint64 pos : newCharList.keys()) {
        SAFE_POINT(pos >= 0 && pos < msaLength, "Incorrect position!", );

        char newChar = newCharList.value(pos);
        qint64 posInSeq = -1;
        qint64 endPosInSeq = -1;
        MaDbiUtils::getStartAndEndSequencePositions(seq, row.gaps, pos, 1, posInSeq, endPosInSeq);
        SAFE_POINT(posInSeq >= 0, "incorrect posInSeq value", );
        SAFE_POINT(endPosInSeq >= 0, "incorrect endPosInSeq value", );

        if (posInSeq >= 0 && endPosInSeq > posInSeq) {  // not gap
            DNASequenceUtils::replaceChars(seq, (int)posInSeq, QByteArray(1, newChar), os);
            CHECK_OP(os, );
        } else {
            DNAChromatogram chromatogram = ChromatogramUtils::exportChromatogram(os, U2EntityRef(mcaRef.dbiRef, row.chromatogramId));
            CHECK_OP(os, );

            ChromatogramUtils::insertBase(chromatogram, (int)posInSeq, row.gaps, (int)pos);
            ChromatogramUtils::updateChromatogramData(os, mcaRef.entityId, U2EntityRef(mcaRef.dbiRef, row.chromatogramId), chromatogram);
            CHECK_OP(os, );

            DNASequenceUtils::insertChars(seq, (int)posInSeq, QByteArray(1, newChar), os);
            CHECK_OP(os, );

            MaDbiUtils::calculateGapModelAfterReplaceChar(row.gaps, pos);
        }
    }

    msaDbi->updateRowContent(mcaRef.entityId, rowId, seq, row.gaps, os);
}

}  // namespace U2
