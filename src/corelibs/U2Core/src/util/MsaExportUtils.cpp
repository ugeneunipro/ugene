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

#include "MsaExportUtils.h"

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/MultipleAlignmentRowInfo.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceDbi.h>

static const char* ROWS_SEQS_COUNT_MISMATCH_ERROR = "Different number of rows and sequences!";

namespace U2 {

MultipleAlignment MsaExportUtils::loadAlignment(const U2DbiRef& dbiRef, const U2DataId& msaId, U2OpStatus& os) {
    DbiConnection connection;
    connection.open(dbiRef, false, os);
    CHECK_OP(os, {});

    U2MsaDbi* msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_NN(msaDbi, {});

    U2Msa msa = msaDbi->getMsaObject(msaId, os);
    CHECK_OP(os, {});

    MultipleAlignment ma;
    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(msa.alphabet);
    ma->setAlphabet(alphabet);
    ma->setName(msa.visualName);
    ma->setLength((int)msa.length);

    QVariantMap alInfo = loadAlignmentInfo(msaId, os, connection);
    CHECK_OP(os, {});
    ma->setInfo(alInfo);

    // Rows and their sequences
    QList<U2MsaRow> rows = readRows(msaId, os, connection);
    CHECK_OP(os, {});

    QList<MsaRowSnapshot> rowSnapshots = loadRows(rows, os, connection);
    CHECK_OP(os, {});

    SAFE_POINT(rows.count() == rowSnapshots.count(), ROWS_SEQS_COUNT_MISMATCH_ERROR, {});

    for (int i = 0; i < rows.count(); ++i) {
        const MsaRowSnapshot& rowSnapshot = rowSnapshots[i];
        if (rowSnapshot.chromatogram.isEmpty()) {  // Use original MSA code.
            ma->addRow(rows[i], rowSnapshot.sequence, os);
        } else {  // Use original MCA code.
            ma->addRow(rows[i], rowSnapshot.chromatogram, rowSnapshot.sequence, os);
            ma->getRow(i)->setAdditionalInfo(rowSnapshot.additionalInfo);
        }
        CHECK_OP(os, {});
    }
    return ma;
}

QList<MsaRowSnapshot> MsaExportUtils::loadRows(const U2DbiRef& dbiRef,
                                               const U2DataId& msaId,
                                               const QList<qint64>& rowIds,
                                               U2OpStatus& os) {
    DbiConnection connection;
    connection.open(dbiRef, false, os);
    CHECK_OP(os, {});

    QList<U2MsaRow> rows = readRows(msaId, rowIds, os, connection);
    CHECK_OP(os, {});

    return loadRows(rows, os, connection);
}

QList<U2MsaRow> MsaExportUtils::readRows(const U2DataId& msaId, U2OpStatus& os, const DbiConnection& connection) {
    U2MsaDbi* msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_NN(msaDbi, {});

    return msaDbi->getRows(msaId, os);
}

QList<U2MsaRow> MsaExportUtils::readRows(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os, const DbiConnection& connection) {
    U2MsaDbi* msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_NN(msaDbi, {});
    QList<U2MsaRow> result;
    foreach (qint64 rowId, rowIds) {
        result.append(msaDbi->getRow(msaId, rowId, os));
        SAFE_POINT_OP(os, QList<U2MsaRow>());
    }
    return result;
}

QList<MsaRowSnapshot> MsaExportUtils::loadRows(const QList<U2MsaRow>& rows, U2OpStatus& os, const DbiConnection& connection) {
    U2SequenceDbi* sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_NN(sequenceDbi, {});

    U2AttributeDbi* attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(attributeDbi != nullptr, os.setError("Attribute Dbi is NULL during exporting an alignment info"), {});

    QList<MsaRowSnapshot> rowSnapshots;
    rowSnapshots.reserve(rows.count());
    for (const auto& row : qAsConst(rows)) {
        MsaRowSnapshot rowSnapshot;
        rowSnapshot.rowId = row.rowId;

        U2Region regionInSequence(row.gstart, row.gend - row.gstart);

        QByteArray seqData = sequenceDbi->getSequenceData(row.sequenceId, regionInSequence, os);
        CHECK_OP(os, {});

        U2Sequence seqObj = sequenceDbi->getSequenceObject(row.sequenceId, os);
        CHECK_OP(os, {});

        rowSnapshot.sequence = DNASequence(seqObj.visualName, seqData);
        rowSnapshot.gaps = row.gaps;
        rowSnapshot.rowLength = row.length;

        if (!row.chromatogramId.isEmpty()) {
            U2EntityRef chromatogramRef(connection.dbi->getDbiRef(), row.chromatogramId);
            rowSnapshot.chromatogram = ChromatogramUtils::exportChromatogram(os, chromatogramRef);
            CHECK_OP(os, {});

            QList<U2DataId> reversedAttributeIds = attributeDbi->getObjectAttributes(row.chromatogramId, MultipleAlignmentRowInfo::REVERSED, os);
            CHECK_OP(os, {});

            if (!reversedAttributeIds.isEmpty()) {
                bool isReversed = attributeDbi->getIntegerAttribute(reversedAttributeIds.last(), os).value == 1;
                MultipleAlignmentRowInfo::setReversed(rowSnapshot.additionalInfo, isReversed);
            }

            QList<U2DataId> complementedAttributeIds = attributeDbi->getObjectAttributes(row.chromatogramId, MultipleAlignmentRowInfo::COMPLEMENTED, os);
            CHECK_OP(os, {});

            if (!reversedAttributeIds.isEmpty()) {
                bool isComplemented = attributeDbi->getIntegerAttribute(complementedAttributeIds.last(), os).value == 1;
                MultipleAlignmentRowInfo::setComplemented(rowSnapshot.additionalInfo, isComplemented);
            }
        }
    }

    return rowSnapshots;
}

QVariantMap MsaExportUtils::loadAlignmentInfo(const U2DataId& msaId, U2OpStatus& os, const DbiConnection& connection) {
    U2AttributeDbi* attrDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_NN(attrDbi, {});

    // Get all MSA attributes
    QVariantMap alInfo;
    QList<U2DataId> attributeIds = attrDbi->getObjectAttributes(msaId, "", os);
    CHECK_OP(os, QVariantMap());

    for (const U2DataId& attributeId : qAsConst(attributeIds)) {
        U2StringAttribute attr = attrDbi->getStringAttribute(attributeId, os);
        CHECK_OP(os, {});
        alInfo.insert(attr.name, attr.value);
    }

    return alInfo;
}

}  // namespace U2
