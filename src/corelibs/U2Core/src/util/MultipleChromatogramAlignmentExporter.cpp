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

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/McaDbiUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include "MultipleChromatogramAlignmentExporter.h"
#include "datatype/msa/MultipleAlignmentRowInfo.h"

namespace U2 {

MultipleChromatogramAlignment MultipleChromatogramAlignmentExporter::getAlignment(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId) const {
    SAFE_POINT_EXT(!connection.isOpen(), os.setError("Connection is already opened"), MultipleChromatogramAlignment());
    connection.open(dbiRef, false, os);
    CHECK_OP(os, MultipleChromatogramAlignment());

    // Rows and their child objects
    QList<U2McaRow> rows = exportRows(os, dbiRef, mcaId);
    CHECK_OP(os, MultipleChromatogramAlignment());

    QList<McaRowMemoryData> mcaRowsMemoryData = exportDataOfRows(os, rows);
    CHECK_OP(os, MultipleChromatogramAlignment());
    SAFE_POINT_EXT(rows.count() == mcaRowsMemoryData.count(), os.setError("Different number of rows and sequences"), MultipleChromatogramAlignment());

    MultipleChromatogramAlignment mca;
    for (int i = 0; i < rows.count(); ++i) {
        mca->addRow(rows[i], mcaRowsMemoryData[i], os);
        mca->getMcaRow(i)->setAdditionalInfo(mcaRowsMemoryData[i].additionalInfo);
    }

    // Info
    QVariantMap info = exportAlignmentInfo(os, mcaId);
    CHECK_OP(os, MultipleChromatogramAlignment());

    mca->setInfo(info);

    // Alphabet, name and length
    U2Msa dbMca = exportAlignmentObject(os, mcaId);
    CHECK_OP(os, MultipleChromatogramAlignment());

    const DNAAlphabet *alphabet = U2AlphabetUtils::getById(dbMca.alphabet);
    SAFE_POINT_EXT(NULL != alphabet, os.setError(QString("Alphabet with ID '%1' not found").arg(dbMca.alphabet.id)), MultipleChromatogramAlignment());
    mca->setAlphabet(alphabet);
    mca->setName(dbMca.visualName);
    mca->setLength(dbMca.length);

    return mca;
}

QMap<qint64, McaRowMemoryData> MultipleChromatogramAlignmentExporter::getMcaRowMemoryData(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId, const QList<qint64> rowIds) const {
    QMap<qint64, McaRowMemoryData> result;
    SAFE_POINT_EXT(!connection.isOpen(), os.setError("Connection is already opened"), result);
    connection.open(dbiRef, false, os);
    CHECK_OP(os, result);

    QList<U2McaRow> rows = exportRows(os, dbiRef, mcaId, rowIds);
    CHECK_OP(os, result);

    QList<McaRowMemoryData> rowsData = exportDataOfRows(os, rows);
    CHECK_OP(os, result);
    SAFE_POINT_EXT(rows.count() == rowsData.count(), os.setError("Different number of rows and sequences"), result);

    for (int i = 0; i < rows.size(); i++) {
        result.insert(rows[i].rowId, rowsData[i]);
    }

    return result;
}

QList<U2McaRow> MultipleChromatogramAlignmentExporter::exportRows(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId) const {
    return McaDbiUtils::getMcaRows(os, U2EntityRef(dbiRef, mcaId));
}

QList<U2McaRow> MultipleChromatogramAlignmentExporter::exportRows(U2OpStatus &os, const U2DbiRef &dbiRef, const U2DataId &mcaId, const QList<qint64> rowIds) const {
    QList<U2McaRow> result;
    foreach(qint64 rowId, rowIds) {
        result << McaDbiUtils::getMcaRow(os, U2EntityRef(dbiRef, mcaId), rowId);
        CHECK_OP(os, QList<U2McaRow>());
    }
    return result;
}

QList<McaRowMemoryData> MultipleChromatogramAlignmentExporter::exportDataOfRows(U2OpStatus &os, const QList<U2McaRow> &rows) const {
    QList<McaRowMemoryData> mcaRowsMemoryData;
    mcaRowsMemoryData.reserve(rows.count());

    foreach(const U2McaRow &row, rows) {
        McaRowMemoryData mcaRowMemoryData;
        mcaRowMemoryData.chromatogram = ChromatogramUtils::exportChromatogram(os, U2EntityRef(connection.dbi->getDbiRef(), row.chromatogramId));
        CHECK_OP(os, QList<McaRowMemoryData>());

        mcaRowMemoryData.sequence = exportSequence(os, row.sequenceId);
        CHECK_OP(os, QList<McaRowMemoryData>());

        mcaRowMemoryData.additionalInfo = exportRowAdditionalInfo(os, row.chromatogramId);

        mcaRowMemoryData.gapModel = row.gaps;
        mcaRowMemoryData.rowLength = row.length;

        mcaRowsMemoryData << mcaRowMemoryData;
    }

    return mcaRowsMemoryData;
}

DNASequence MultipleChromatogramAlignmentExporter::exportSequence(U2OpStatus &os, const U2DataId &sequenceId) const {
    U2SequenceDbi *sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(NULL != sequenceDbi, os.setError("NULL Sequence Dbi during exporting rows sequences"), DNASequence());

    U2Sequence dbSequence = sequenceDbi->getSequenceObject(sequenceId, os);
    CHECK_OP(os, DNASequence());

    QScopedPointer<U2SequenceObject> sequenceObject(new U2SequenceObject(dbSequence.visualName, U2EntityRef(connection.dbi->getDbiRef(), dbSequence.id)));
    return sequenceObject->getSequence(U2_REGION_MAX, os);
}

QVariantMap MultipleChromatogramAlignmentExporter::exportRowAdditionalInfo(U2OpStatus &os, const U2DataId &chromatogramId) const {
    U2AttributeDbi *attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(NULL != attributeDbi, os.setError("NULL Attribute Dbi during exporting an alignment info"), QVariantMap());

    QVariantMap additionalInfo;
    QList<U2DataId> reversedAttributeIds = attributeDbi->getObjectAttributes(chromatogramId, MultipleAlignmentRowInfo::REVERSED, os);
    CHECK_OP(os, QVariantMap());

    if (!reversedAttributeIds.isEmpty()) {
        MultipleAlignmentRowInfo::setReversed(additionalInfo, attributeDbi->getIntegerAttribute(reversedAttributeIds.last(), os).value == 1);
    }

    QList<U2DataId> complementedAttributeIds = attributeDbi->getObjectAttributes(chromatogramId, MultipleAlignmentRowInfo::COMPLEMENTED, os);
    CHECK_OP(os, QVariantMap());

    if (!reversedAttributeIds.isEmpty()) {
        MultipleAlignmentRowInfo::setComplemented(additionalInfo, attributeDbi->getIntegerAttribute(complementedAttributeIds.last(), os).value == 1);
    }

    return additionalInfo;
}

QVariantMap MultipleChromatogramAlignmentExporter::exportAlignmentInfo(U2OpStatus &os, const U2DataId &mcaId) const {
    U2AttributeDbi *attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(NULL != attributeDbi, os.setError("NULL Attribute Dbi during exporting an alignment info"), QVariantMap());
    U2Dbi* dbi = attributeDbi->getRootDbi();
    SAFE_POINT_EXT(NULL != dbi, os.setError("NULL root Dbi during exporting an alignment info"), QVariantMap());

    QVariantMap info;
    QList<U2DataId> attributeIds = attributeDbi->getObjectAttributes(mcaId, "", os);
    CHECK_OP(os, QVariantMap());

    foreach(const U2DataId &attributeId, attributeIds) {
        if (dbi->getEntityTypeById(attributeId) != U2Type::AttributeString) {
            continue;
        }
        const U2StringAttribute attr = attributeDbi->getStringAttribute(attributeId, os);
        CHECK_OP(os, QVariantMap());
        info.insert(attr.name, attr.value);
    }

    return info;
}

U2Mca MultipleChromatogramAlignmentExporter::exportAlignmentObject(U2OpStatus &os, const U2DataId &mcaId) const {
    U2MsaDbi *msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(NULL != msaDbi, os.setError("NULL MSA Dbi during exporting an alignment object"), U2Msa());
    U2Msa dbMsa = msaDbi->getMsaObject(mcaId, os);
    return U2Mca(dbMsa);
}

}   // namespace U2
