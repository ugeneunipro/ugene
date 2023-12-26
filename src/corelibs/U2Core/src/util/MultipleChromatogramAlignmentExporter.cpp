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

#include "MultipleChromatogramAlignmentExporter.h"

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include "datatype/msa/MultipleAlignmentRowInfo.h"

namespace U2 {

MultipleAlignment MultipleChromatogramAlignmentExporter::getAlignment(U2OpStatus& os, const U2DbiRef& dbiRef, const U2DataId& mcaId) const {
    MultipleAlignment mca;

    SAFE_POINT_EXT(!connection.isOpen(), os.setError("Connection is already opened"), mca);
    connection.open(dbiRef, false, os);
    CHECK_OP(os, mca);

    // Rows and their child objects
    QList<U2MsaRow> rows = exportRows(os, dbiRef, mcaId);
    CHECK_OP(os, mca);

    QList<McaRowMemoryData> mcaRowsMemoryData = exportDataOfRows(os, rows);
    CHECK_OP(os, mca);
    SAFE_POINT_EXT(rows.count() == mcaRowsMemoryData.count(), os.setError("Different number of rows and sequences"), mca);

    for (int i = 0; i < rows.count(); ++i) {
        const McaRowMemoryData& rowData = mcaRowsMemoryData[i];
        mca->addRow(rows[i], rowData.chromatogram, rowData.sequence, os);
        mca->getRow(i)->setAdditionalInfo(rowData.additionalInfo);
    }

    // Info
    QVariantMap info = exportAlignmentInfo(os, mcaId);
    CHECK_OP(os, mca);

    mca->setInfo(info);

    // Alphabet, name and length
    U2Msa dbMca = exportAlignmentObject(os, mcaId);
    CHECK_OP(os, mca);

    const DNAAlphabet* alphabet = U2AlphabetUtils::getById(dbMca.alphabet);
    SAFE_POINT_EXT(alphabet != nullptr, os.setError(QString("Alphabet with ID '%1' not found").arg(dbMca.alphabet.id)), mca);
    mca->setAlphabet(alphabet);
    mca->setName(dbMca.visualName);
    mca->setLength(dbMca.length);

    return mca;
}

QMap<qint64, McaRowMemoryData> MultipleChromatogramAlignmentExporter::getMcaRowMemoryData(U2OpStatus& os,
                                                                                          const U2DbiRef& dbiRef,
                                                                                          const U2DataId& mcaId,
                                                                                          const QList<qint64>& rowIds) const {
    QMap<qint64, McaRowMemoryData> result;
    SAFE_POINT_EXT(!connection.isOpen(), os.setError("Connection is already opened"), result);
    connection.open(dbiRef, false, os);
    CHECK_OP(os, result);

    QList<U2MsaRow> rows = exportRows(os, dbiRef, mcaId, rowIds);
    CHECK_OP(os, result);

    QList<McaRowMemoryData> rowsData = exportDataOfRows(os, rows);
    CHECK_OP(os, result);
    SAFE_POINT_EXT(rows.count() == rowsData.count(), os.setError("Different number of rows and sequences"), result);

    for (int i = 0; i < rows.size(); i++) {
        result.insert(rows[i].rowId, rowsData[i]);
    }

    return result;
}

QList<U2MsaRow> MultipleChromatogramAlignmentExporter::exportRows(U2OpStatus& os, const U2DbiRef& dbiRef, const U2DataId& mcaId) const {
    U2EntityRef ref(dbiRef, mcaId);
    return MsaDbiUtils::getMsaRows(os, ref);
}

QList<U2MsaRow> MultipleChromatogramAlignmentExporter::exportRows(U2OpStatus& os, const U2DbiRef& dbiRef, const U2DataId& mcaId, const QList<qint64>& rowIds) const {
    QList<U2MsaRow> result;
    U2EntityRef entityRef(dbiRef, mcaId);
    foreach (qint64 rowId, rowIds) {
        result << MsaDbiUtils::getMsaRow(os, entityRef, rowId);
        CHECK_OP(os, QList<U2MsaRow>());
    }
    return result;
}

QList<McaRowMemoryData> MultipleChromatogramAlignmentExporter::exportDataOfRows(U2OpStatus& os, const QList<U2MsaRow>& rows) const {
    QList<McaRowMemoryData> mcaRowsMemoryData;
    mcaRowsMemoryData.reserve(rows.count());

    for (const U2MsaRow& row: qAsConst(rows)) {
        McaRowMemoryData mcaRowMemoryData;
        U2EntityRef chromatogramRef(connection.dbi->getDbiRef(), row.chromatogramId);
        mcaRowMemoryData.chromatogram = ChromatogramUtils::exportChromatogram(os, chromatogramRef);
        CHECK_OP(os, {});

        mcaRowMemoryData.sequence = exportSequence(os, row.sequenceId);
        CHECK_OP(os, {});

        mcaRowMemoryData.additionalInfo = exportRowAdditionalInfo(os, row.chromatogramId);

        mcaRowMemoryData.gapModel = row.gaps;
        mcaRowMemoryData.rowLength = row.length;

        mcaRowsMemoryData << mcaRowMemoryData;
    }

    return mcaRowsMemoryData;
}

DNASequence MultipleChromatogramAlignmentExporter::exportSequence(U2OpStatus& os, const U2DataId& sequenceId) const {
    U2SequenceDbi* sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(sequenceDbi != nullptr, os.setError("NULL Sequence Dbi during exporting rows sequences"), DNASequence());

    U2Sequence dbSequence = sequenceDbi->getSequenceObject(sequenceId, os);
    CHECK_OP(os, DNASequence());

    QScopedPointer<U2SequenceObject> sequenceObject(new U2SequenceObject(dbSequence.visualName, U2EntityRef(connection.dbi->getDbiRef(), dbSequence.id)));
    return sequenceObject->getSequence(U2_REGION_MAX, os);
}

QVariantMap MultipleChromatogramAlignmentExporter::exportRowAdditionalInfo(U2OpStatus& os, const U2DataId& chromatogramId) const {
    U2AttributeDbi* attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(attributeDbi != nullptr, os.setError("NULL Attribute Dbi during exporting an alignment info"), QVariantMap());

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

QVariantMap MultipleChromatogramAlignmentExporter::exportAlignmentInfo(U2OpStatus& os, const U2DataId& mcaId) const {
    U2AttributeDbi* attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(attributeDbi != nullptr, os.setError("NULL Attribute Dbi during exporting an alignment info"), QVariantMap());
    U2Dbi* dbi = attributeDbi->getRootDbi();
    SAFE_POINT_EXT(dbi != nullptr, os.setError("NULL root Dbi during exporting an alignment info"), QVariantMap());

    QVariantMap info;
    QList<U2DataId> attributeIds = attributeDbi->getObjectAttributes(mcaId, "", os);
    CHECK_OP(os, QVariantMap());

    foreach (const U2DataId& attributeId, attributeIds) {
        if (dbi->getEntityTypeById(attributeId) != U2Type::AttributeString) {
            continue;
        }
        const U2StringAttribute attr = attributeDbi->getStringAttribute(attributeId, os);
        CHECK_OP(os, QVariantMap());
        info.insert(attr.name, attr.value);
    }

    return info;
}

U2Msa MultipleChromatogramAlignmentExporter::exportAlignmentObject(U2OpStatus& os, const U2DataId& mcaId) const {
    U2MsaDbi* msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(msaDbi != nullptr, os.setError("NULL MSA Dbi during exporting an alignment object"), U2Msa(U2Type::Mca));
    return msaDbi->getMsaObject(mcaId, os);
}

}  // namespace U2
