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

#include "MultipleSequenceAlignmentExporter.h"

#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceDbi.h>

static const char* NULL_MSA_DBI_ERROR = "NULL MSA Dbi during exporting rows of an alignment!";
static const char* OPENED_DBI_CONNECTION_ERROR = "Connection is already opened!";
static const char* ROWS_SEQS_COUNT_MISMATCH_ERROR = "Different number of rows and sequences!";

namespace U2 {

MultipleAlignment MultipleSequenceAlignmentExporter::getAlignment(const U2DbiRef& dbiRef, const U2DataId& msaId, U2OpStatus& os) const {
    static const MultipleAlignment emptyObject;
    SAFE_POINT(!con.isOpen(), OPENED_DBI_CONNECTION_ERROR, emptyObject);
    con.open(dbiRef, false, os);
    CHECK_OP(os, emptyObject);

    // Rows and their sequences
    QList<U2MsaRow> rows = exportRows(msaId, os);
    CHECK_OP(os, emptyObject);

    QList<DNASequence> sequences = exportSequencesOfRows(rows, os);
    CHECK_OP(os, emptyObject);

    SAFE_POINT(rows.count() == sequences.count(), ROWS_SEQS_COUNT_MISMATCH_ERROR, emptyObject);

    MultipleAlignment al;
    for (int i = 0; i < rows.count(); ++i) {
        al->addRow(rows[i], sequences[i], os);
        CHECK_OP(os, emptyObject);
    }

    // Info
    QVariantMap alInfo = exportAlignmentInfo(msaId, os);
    CHECK_OP(os, emptyObject);

    al->setInfo(alInfo);

    // Alphabet, name and length
    U2Msa msa = exportAlignmentObject(msaId, os);
    CHECK_OP(os, emptyObject);

    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(msa.alphabet);
    al->setAlphabet(alphabet);
    al->setName(msa.visualName);
    al->setLength(msa.length);
    return al;
}

QList<MsaRowReplacementData> MultipleSequenceAlignmentExporter::getAlignmentRows(const U2DbiRef& dbiRef,
                                                                                 const U2DataId& msaId,
                                                                                 const QList<qint64> rowIds,
                                                                                 U2OpStatus& os) const {
    SAFE_POINT(!con.isOpen(), OPENED_DBI_CONNECTION_ERROR, QList<MsaRowReplacementData>());
    con.open(dbiRef, false, os);
    CHECK_OP(os, QList<MsaRowReplacementData>());

    QList<U2MsaRow> rows = exportRows(msaId, rowIds, os);
    CHECK_OP(os, QList<MsaRowReplacementData>());

    QList<DNASequence> sequences = exportSequencesOfRows(rows, os);
    CHECK_OP(os, QList<MsaRowReplacementData>());

    QList<MsaRowReplacementData> result;
    SAFE_POINT(rows.count() == sequences.count(), ROWS_SEQS_COUNT_MISMATCH_ERROR, QList<MsaRowReplacementData>());
    for (int i = 0; i < rows.length(); ++i) {
        result << MsaRowReplacementData(sequences.at(i), rows.at(i));
    }
    return result;
}

QList<U2MsaRow> MultipleSequenceAlignmentExporter::exportRows(const U2DataId& msaId, U2OpStatus& os) const {
    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, NULL_MSA_DBI_ERROR, QList<U2MsaRow>());

    return msaDbi->getRows(msaId, os);
}

QList<U2MsaRow> MultipleSequenceAlignmentExporter::exportRows(const U2DataId& msaId, const QList<qint64> rowIds, U2OpStatus& os) const {
    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, NULL_MSA_DBI_ERROR, QList<U2MsaRow>());
    QList<U2MsaRow> result;
    foreach (qint64 rowId, rowIds) {
        result.append(msaDbi->getRow(msaId, rowId, os));
        SAFE_POINT_OP(os, QList<U2MsaRow>());
    }
    return result;
}

QList<DNASequence> MultipleSequenceAlignmentExporter::exportSequencesOfRows(QList<U2MsaRow> rows, U2OpStatus& os) const {
    U2SequenceDbi* sequenceDbi = con.dbi->getSequenceDbi();
    SAFE_POINT(sequenceDbi != nullptr, "NULL Sequence Dbi during exporting rows sequences!", QList<DNASequence>());

    QList<DNASequence> sequences;
    sequences.reserve(rows.count());
    for (int i = 0, n = rows.count(); i < n; ++i) {
        const U2DataId& sequenceId = rows[i].sequenceId;
        qint64 gstart = rows[i].gstart;
        qint64 gend = rows[i].gend;
        U2Region regionInSequence(gstart, gend - gstart);

        QByteArray seqData = sequenceDbi->getSequenceData(sequenceId, regionInSequence, os);
        CHECK_OP(os, QList<DNASequence>());

        U2Sequence seqObj = sequenceDbi->getSequenceObject(sequenceId, os);
        CHECK_OP(os, QList<DNASequence>());

        DNASequence seq(seqObj.visualName, seqData);
        sequences.append(seq);
    }

    return sequences;
}

QVariantMap MultipleSequenceAlignmentExporter::exportAlignmentInfo(const U2DataId& msaId, U2OpStatus& os) const {
    U2AttributeDbi* attrDbi = con.dbi->getAttributeDbi();
    SAFE_POINT(attrDbi != nullptr, "NULL Attribute Dbi during exporting an alignment info!", QVariantMap());

    // Get all MSA attributes
    QVariantMap alInfo;
    QList<U2DataId> attributeIds = attrDbi->getObjectAttributes(msaId, "", os);
    CHECK_OP(os, QVariantMap());

    foreach (U2DataId attributeId, attributeIds) {
        U2StringAttribute attr = attrDbi->getStringAttribute(attributeId, os);
        CHECK_OP(os, QVariantMap());

        QString key = attr.name;
        QVariant val = QVariant(attr.value);

        alInfo.insert(key, val);
    }

    return alInfo;
}

U2Msa MultipleSequenceAlignmentExporter::exportAlignmentObject(const U2DataId& msaId, U2OpStatus& os) const {
    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, "NULL MSA Dbi during exporting an alignment object!", {U2Type::Msa});

    return msaDbi->getMsaObject(msaId, os);
}

}  // namespace U2
