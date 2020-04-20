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
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/L10n.h>
#include <U2Core/McaDbiUtils.h>
#include <U2Core/MultipleAlignmentInfo.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/MultipleChromatogramAlignmentRow.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include "MultipleChromatogramAlignmentImporter.h"
#include "datatype/msa/MultipleAlignmentRowInfo.h"

namespace U2 {

MultipleChromatogramAlignmentObject * MultipleChromatogramAlignmentImporter::createAlignment(U2OpStatus &os,
    const U2DbiRef &dbiRef,
    const QString &folder,
    MultipleChromatogramAlignment &mca) {
    DbiConnection connection(dbiRef, true, os);
    CHECK(!os.isCanceled(), NULL);
    SAFE_POINT_OP(os, NULL);
    SAFE_POINT_EXT(NULL != connection.dbi, os.setError(L10N::nullPointerError("Destination database")), NULL);

    TmpDbiObjects objs(dbiRef, os);

    // MCA object and info
    U2Mca dbMca = importMcaObject(os, connection, folder, mca);
    objs.objects << dbMca.id;
    CHECK_OP(os, NULL);

    importMcaInfo(os, connection, dbMca.id, mca);
    CHECK_OP(os, NULL);

    // MCA rows
    QList<McaRowDatabaseData> mcaRowsDatabaseData = importRowChildObjects(os, connection, folder, mca);
    CHECK_OP(os, NULL);

    QList<U2McaRow> rows = importRows(os, connection, dbMca, mcaRowsDatabaseData);
    CHECK_OP(os, NULL);
    SAFE_POINT_EXT(rows.size() == mca->getNumRows(), os.setError(QObject::tr("Unexpected error on MCA rows import")), NULL);

    for (int i = 0, n = mca->getNumRows(); i < n; ++i) {
        mca->getMcaRow(i)->setRowDbInfo(rows.at(i));
    }

    return new MultipleChromatogramAlignmentObject(mca->getName(), U2EntityRef(dbiRef, dbMca.id), QVariantMap(), mca);
}

U2Mca MultipleChromatogramAlignmentImporter::importMcaObject(U2OpStatus &os, const DbiConnection &connection, const QString &folder, const MultipleChromatogramAlignment &mca) {
    U2Mca dbMca;
    const DNAAlphabet *alphabet = mca->getAlphabet();
    SAFE_POINT_EXT(NULL != alphabet, os.setError("The alignment alphabet is NULL during importing"), U2Mca());

    dbMca.alphabet.id = alphabet->getId();
    dbMca.length = mca->getLength();
    dbMca.visualName = mca->getName();
    if (dbMca.visualName.isEmpty()) {
        QDate date = QDate::currentDate();
        QString generatedName = "MCA" + date.toString();
        coreLog.trace(QString("A multiple alignment name was empty. Generated a new name %1").arg(generatedName));
        dbMca.visualName = generatedName;
    }

    U2MsaDbi *msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(NULL != msaDbi, os.setError("NULL MSA Dbi during importing an alignment"), U2Mca());

    dbMca.id = msaDbi->createMcaObject(folder, dbMca.visualName, dbMca.alphabet, dbMca.length, os);
    CHECK_OP(os, U2Mca());

    return dbMca;
}

void MultipleChromatogramAlignmentImporter::importMcaInfo(U2OpStatus &os, const DbiConnection &connection, const U2DataId &mcaId, const MultipleChromatogramAlignment &mca) {
    const QVariantMap info = mca->getInfo();

    U2AttributeDbi *attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(NULL != attributeDbi, os.setError("NULL Attribute Dbi during importing an alignment"), );

    foreach(const QString key, info.keys()) {
        if (key != MultipleAlignmentInfo::NAME) { // name is stored in the object
            const QString value = info.value(key).toString();
            U2StringAttribute attribute(mcaId, key, value);
            attributeDbi->createStringAttribute(attribute, os);
            CHECK_OP(os, );
        }
    }
}

QList<McaRowDatabaseData> MultipleChromatogramAlignmentImporter::importRowChildObjects(U2OpStatus &os,
    const DbiConnection &connection,
    const QString &folder,
    const MultipleChromatogramAlignment &mca) {
    QList<McaRowDatabaseData> mcaRowsDatabaseData;
    UdrDbi *udrDbi = connection.dbi->getUdrDbi();
    SAFE_POINT_EXT(NULL != udrDbi, os.setError("NULL UDR Dbi during importing an alignment"), mcaRowsDatabaseData);
    U2SequenceDbi *sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(NULL != sequenceDbi, os.setError("NULL Sequence Dbi during importing an alignment"), mcaRowsDatabaseData);

    const DNAAlphabet *alphabet = mca->getAlphabet();
    SAFE_POINT_EXT(NULL != alphabet, os.setError("MCA alphabet is NULL"), mcaRowsDatabaseData);
    const U2AlphabetId alphabetId = alphabet->getId();

    foreach(const MultipleChromatogramAlignmentRow &row, mca->getMcaRows()) {
        McaRowDatabaseData mcaRowDatabaseData;

        mcaRowDatabaseData.chromatogram = importChromatogram(os, connection, folder, row->getChromatogram());
        CHECK_OP(os, mcaRowsDatabaseData);

        mcaRowDatabaseData.sequence = importSequence(os, connection, folder, row->getSequence(), alphabetId);
        CHECK_OP(os, mcaRowsDatabaseData);

        createRelation(os, connection, mcaRowDatabaseData.sequence, mcaRowDatabaseData.chromatogram.id);

        mcaRowDatabaseData.additionalInfo = row->getAdditionalInfo();
        importRowAdditionalInfo(os, connection, mcaRowDatabaseData.chromatogram, mcaRowDatabaseData.additionalInfo);
        CHECK_OP(os, mcaRowsDatabaseData);

        mcaRowDatabaseData.gapModel = row->getGapModel();
        mcaRowDatabaseData.rowLength = row->getRowLengthWithoutTrailing();

        mcaRowsDatabaseData << mcaRowDatabaseData;
    }

    return mcaRowsDatabaseData;
}

QList<U2McaRow> MultipleChromatogramAlignmentImporter::importRows(U2OpStatus &os,
    const DbiConnection &connection,
    U2Mca &dbMca,
    const QList<McaRowDatabaseData> &mcaRowsDatabaseData) {
    QList<U2McaRow> rows;

    foreach(const McaRowDatabaseData &mcaRowDatabaseData, mcaRowsDatabaseData) {
        U2McaRow row;
        row.chromatogramId = mcaRowDatabaseData.chromatogram.id;
        row.sequenceId = mcaRowDatabaseData.sequence.id;
        row.gaps = mcaRowDatabaseData.gapModel;
        row.gstart = 0;
        row.gend = mcaRowDatabaseData.sequence.length;
        row.length = mcaRowDatabaseData.rowLength;

        rows << row;
    }

    McaDbiUtils::addRows(os, U2EntityRef(connection.dbi->getDbiRef(), dbMca.id), rows);
    CHECK_OP(os, QList<U2McaRow>());
    return rows;
}

U2Chromatogram MultipleChromatogramAlignmentImporter::importChromatogram(U2OpStatus &os,
    const DbiConnection &connection,
    const QString &folder,
    const DNAChromatogram &chromatogram) {
    const U2EntityRef chromatogramRef = ChromatogramUtils::import(os, connection.dbi->getDbiRef(), folder, chromatogram);
    CHECK_OP(os, U2Chromatogram());
    connection.dbi->getObjectDbi()->setObjectRank(chromatogramRef.entityId, U2DbiObjectRank_Child, os);
    CHECK_OP(os, U2Chromatogram());
    return ChromatogramUtils::getChromatogramDbInfo(os, chromatogramRef);
}

U2Sequence MultipleChromatogramAlignmentImporter::importSequence(U2OpStatus &os,
    const DbiConnection &connection,
    const QString &folder,
    const DNASequence &sequence,
    const U2AlphabetId &alphabetId) {
    const U2EntityRef sequenceRef = U2SequenceUtils::import(os, connection.dbi->getDbiRef(), folder, sequence, alphabetId);
    CHECK_OP(os, U2Sequence());
    connection.dbi->getObjectDbi()->setObjectRank(sequenceRef.entityId, U2DbiObjectRank_Child, os);
    CHECK_OP(os, U2Sequence());
    return connection.dbi->getSequenceDbi()->getSequenceObject(sequenceRef.entityId, os);
}

void MultipleChromatogramAlignmentImporter::importRowAdditionalInfo(U2OpStatus &os, const DbiConnection &connection, const U2Chromatogram &chromatogram, const QVariantMap &additionalInfo) {
    U2IntegerAttribute reversedAttribute;
    reversedAttribute.objectId = chromatogram.id;
    reversedAttribute.name = MultipleAlignmentRowInfo::REVERSED;
    reversedAttribute.version = chromatogram.version;
    reversedAttribute.value = MultipleAlignmentRowInfo::getReversed(additionalInfo) ? 1 : 0;

    connection.dbi->getAttributeDbi()->createIntegerAttribute(reversedAttribute, os);
    CHECK_OP(os, );

    U2IntegerAttribute complementedAttribute;
    complementedAttribute.objectId = chromatogram.id;
    complementedAttribute.name = MultipleAlignmentRowInfo::COMPLEMENTED;
    complementedAttribute.version = chromatogram.version;
    complementedAttribute.value = MultipleAlignmentRowInfo::getComplemented(additionalInfo) ? 1 : 0;

    connection.dbi->getAttributeDbi()->createIntegerAttribute(complementedAttribute, os);
    CHECK_OP(os, );
}

void MultipleChromatogramAlignmentImporter::createRelation(U2OpStatus &os, const DbiConnection &connection, const U2Sequence &sequence, const U2DataId &chromatogramId) {
    U2ObjectRelation dbRelation;
    dbRelation.id = chromatogramId;
    dbRelation.referencedName = sequence.visualName;
    dbRelation.referencedObject = sequence.id;
    dbRelation.referencedType = GObjectTypes::SEQUENCE;
    dbRelation.relationRole = ObjectRole_Sequence;

    connection.dbi->getObjectRelationsDbi()->createObjectRelation(dbRelation, os);
    CHECK_OP(os, );
}

}   // namespace U2
