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

#include "MsaImportUtils.h"

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleAlignmentInfo.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/MultipleAlignmentRowInfo.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

MultipleAlignmentObject* MsaImportUtils::createMsaObject(const U2DbiRef& dbiRef,
                                                         MultipleAlignment& al,
                                                         U2OpStatus& os,
                                                         const QString& folder) {
    DbiConnection con(dbiRef, true, os);
    CHECK(!os.isCanceled(), nullptr);
    SAFE_POINT_OP(os, nullptr);
    SAFE_POINT_EXT(con.dbi != nullptr, os.setError(L10N::nullPointerError("Destination database")), nullptr);

    TmpDbiObjects tmpObjects(dbiRef, os);  // remove the MSA object if opStatus is incorrect

    // MSA object and info
    U2DataId msaId = createEmptyMsaObject(con, folder, al->getName(), al->getAlphabet(), os);
    CHECK_OP(os, nullptr);

    tmpObjects.objects << msaId;

    importMsaInfo(con, msaId, al->getInfo(), os);
    CHECK_OP(os, nullptr);

    // MSA rows
    QList<U2Sequence> sequences = importMsaSequences(con, folder, al, os);
    QList<QVector<U2MsaGap>> gapsPerRow = al->getGapModel();
    CHECK_OP(os, nullptr);

    QList<U2MsaRow> rows = importMsaRows(con, al, msaId, sequences, gapsPerRow, os);
    CHECK_OP(os, nullptr);
    SAFE_POINT_EXT(rows.size() == al->getRowCount(), os.setError(QString("Unexpected error on MSA rows import")), nullptr);

    if (!rows.isEmpty()) {
        // if the imported alignment is not empty -> set it length to the max(len, orig-len).
        // this way we preserve trailing gaps removed by the importRows call.
        qint64 srcAlignmentLen = al->getLength();
        qint64 newAlignmentLen = con.dbi->getMsaDbi()->getMsaLength(msaId, os);
        if (srcAlignmentLen > newAlignmentLen) {
            con.dbi->getMsaDbi()->updateMsaLength(msaId, srcAlignmentLen, os);
            CHECK_OP(os, nullptr);
        }
    }

    for (int i = 0, n = al->getRowCount(); i < n; ++i) {
        al->getRow(i)->setRowDbInfo(rows.at(i));
    }

    return new MultipleAlignmentObject(al->getName(), U2EntityRef(dbiRef, msaId), QVariantMap(), al);
}

MultipleAlignmentObject* MsaImportUtils::createMcaObject(const U2DbiRef& dbiRef,
                                                         MultipleAlignment& mca,
                                                         U2OpStatus& os,
                                                         const QString& folder) {
    DbiConnection connection(dbiRef, true, os);
    CHECK(!os.isCanceled(), nullptr);
    SAFE_POINT_OP(os, nullptr);
    SAFE_POINT_EXT(connection.dbi != nullptr, os.setError(L10N::nullPointerError("Destination database")), nullptr);

    TmpDbiObjects objs(dbiRef, os);

    // MCA object and info
    U2Msa dbMca = importMcaObject(os, connection, folder, mca);
    objs.objects << dbMca.id;
    CHECK_OP(os, nullptr);

    importMcaInfo(os, connection, dbMca.id, mca);
    CHECK_OP(os, nullptr);

    // MCA rows
    QList<MsaDbRowSnapshot> mcaRowsDatabaseData = importRowChildObjects(os, connection, folder, mca);
    CHECK_OP(os, nullptr);

    QList<U2MsaRow> rows = importMcaRows(os, connection, dbMca, mcaRowsDatabaseData);
    CHECK_OP(os, nullptr);
    SAFE_POINT_EXT(rows.size() == mca->getRowCount(), os.setError(QString("Unexpected error on MCA rows import")), nullptr);

    for (int i = 0, n = mca->getRowCount(); i < n; ++i) {
        mca->getRow(i)->setRowDbInfo(rows.at(i));
    }

    return new MultipleAlignmentObject(mca->getName(), U2EntityRef(dbiRef, dbMca.id), QVariantMap(), mca, GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT);
}

U2DataId MsaImportUtils::createEmptyMsaObject(const DbiConnection& con, const QString& folder, const QString& name, const DNAAlphabet* alphabet, U2OpStatus& os) {
    SAFE_POINT(alphabet != nullptr, "The alignment alphabet is NULL during importing!", U2DataId());

    QString visualName = name;
    if (visualName.isEmpty()) {
        QDate date = QDate::currentDate();
        QString generatedName = "MSA" + date.toString();
        coreLog.trace(QString("A multiple alignment name was empty! Generated a new name %1").arg(generatedName));
        visualName = generatedName;
    }

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, "NULL MSA Dbi during importing an alignment!", U2DataId());

    U2DataId id = msaDbi->createMsaObject(folder, visualName, alphabet->getId(), 0, os);
    CHECK_OP(os, U2DataId());

    return id;
}

void MsaImportUtils::importMsaInfo(const DbiConnection& con, const U2DataId& msaId, const QVariantMap& alInfo, U2OpStatus& os) {
    U2AttributeDbi* attrDbi = con.dbi->getAttributeDbi();
    SAFE_POINT(attrDbi != nullptr, "NULL Attribute Dbi during importing an alignment!", );

    foreach (QString key, alInfo.keys()) {
        if (key != MultipleAlignmentInfo::NAME) {  // name is stored in the object
            QString val = alInfo.value(key).value<QString>();
            U2StringAttribute attr(msaId, key, val);

            attrDbi->createStringAttribute(attr, os);
            CHECK_OP(os, );
        }
    }
}

QList<U2Sequence> MsaImportUtils::importMsaSequences(const DbiConnection& con, const QString& folder, const MultipleAlignment& al, U2OpStatus& os) {
    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    SAFE_POINT(seqDbi != nullptr, "NULL Sequence Dbi during importing an alignment!", QList<U2Sequence>());

    QList<U2Sequence> sequences;
    for (int i = 0; i < al->getRowCount(); ++i) {
        DNASequence dnaSeq = al->getRow(i)->getSequence();

        U2Sequence sequence = U2Sequence();
        sequence.visualName = dnaSeq.getName();
        sequence.circular = dnaSeq.circular;
        sequence.length = dnaSeq.length();

        const DNAAlphabet* alphabet = dnaSeq.alphabet;
        if (alphabet == nullptr) {
            alphabet = U2AlphabetUtils::findBestAlphabet(dnaSeq.constData(), dnaSeq.length());
        }
        SAFE_POINT(alphabet != nullptr, "Failed to get alphabet for a sequence!", QList<U2Sequence>());
        sequence.alphabet.id = alphabet->getId();

        seqDbi->createSequenceObject(sequence, folder, os, U2DbiObjectRank_Child);
        CHECK_OP(os, QList<U2Sequence>());

        QVariantMap hints;
        const QByteArray& seqData = dnaSeq.constSequence();
        seqDbi->updateSequenceData(sequence.id, U2_REGION_MAX, seqData, hints, os);
        CHECK_OP(os, QList<U2Sequence>());

        sequences.append(sequence);
    }
    return sequences;
}

QList<U2MsaRow> MsaImportUtils::importMsaRows(const DbiConnection& con,
                                              MultipleAlignment& al,
                                              const U2DataId& msaId,
                                              const QList<U2Sequence>& sequences,
                                              const QList<QVector<U2MsaGap>>& msaGapModel,
                                              U2OpStatus& os) {
    QList<U2MsaRow> rows;
    SAFE_POINT_EXT(sequences.size() == msaGapModel.size(), os.setError("Gap model doesn't fit sequences count"), rows);

    for (int rowIdx = 0, seqIdx = 0; rowIdx < al->getRowCount(); ++rowIdx, ++seqIdx) {
        U2Sequence seq = sequences[seqIdx];
        MultipleAlignmentRow alignmentRow = al->getRow(rowIdx);
        const QVector<U2MsaGap>& gapModel = msaGapModel[rowIdx];
        if (!gapModel.isEmpty() && (gapModel.last().startPos + gapModel.last().length) == MsaRowUtils::getRowLength(alignmentRow->getSequence().seq, gapModel)) {
            // remove trailing gap if it exists
            QVector<U2MsaGap> newGapModel = gapModel;
            newGapModel.removeLast();
            alignmentRow->setGapModel(newGapModel);
        }
        U2MsaRow row;
        row.sequenceId = seq.id;
        row.gstart = 0;
        row.gend = seq.length;
        row.gaps = alignmentRow->getGaps();
        row.length = alignmentRow->getRowLengthWithoutTrailing();

        rows.append(row);
    }

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(msaDbi != nullptr, "NULL MSA Dbi during importing an alignment!", QList<U2MsaRow>());

    msaDbi->addRows(msaId, rows, -1, os);
    CHECK_OP(os, QList<U2MsaRow>());
    return rows;
}

U2Msa MsaImportUtils::importMcaObject(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const MultipleAlignment& mca) {
    U2Msa dbMca(U2Type::Mca);
    const DNAAlphabet* alphabet = mca->getAlphabet();
    SAFE_POINT_EXT(alphabet != nullptr, os.setError("The alignment alphabet is NULL during importing"), U2Msa(U2Type::Mca));

    dbMca.alphabet.id = alphabet->getId();
    dbMca.length = mca->getLength();
    dbMca.visualName = mca->getName();
    if (dbMca.visualName.isEmpty()) {
        QDate date = QDate::currentDate();
        QString generatedName = "MCA" + date.toString();
        coreLog.trace(QString("A multiple alignment name was empty. Generated a new name %1").arg(generatedName));
        dbMca.visualName = generatedName;
    }

    U2MsaDbi* msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(msaDbi != nullptr, os.setError("NULL MSA Dbi during importing an alignment"), U2Msa(U2Type::Mca));

    dbMca.id = msaDbi->createMcaObject(folder, dbMca.visualName, dbMca.alphabet, dbMca.length, os);
    CHECK_OP(os, U2Msa(U2Type::Mca));

    return dbMca;
}

void MsaImportUtils::importMcaInfo(U2OpStatus& os, const DbiConnection& connection, const U2DataId& mcaId, const MultipleAlignment& mca) {
    const QVariantMap info = mca->getInfo();

    U2AttributeDbi* attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(attributeDbi != nullptr, os.setError("NULL Attribute Dbi during importing an alignment"), );

    foreach (const QString key, info.keys()) {
        if (key != MultipleAlignmentInfo::NAME) {  // name is stored in the object
            const QString value = info.value(key).toString();
            U2StringAttribute attribute(mcaId, key, value);
            attributeDbi->createStringAttribute(attribute, os);
            CHECK_OP(os, );
        }
    }
}

QList<MsaDbRowSnapshot> MsaImportUtils::importRowChildObjects(U2OpStatus& os,
                                                              const DbiConnection& connection,
                                                              const QString& folder,
                                                              const MultipleAlignment& mca) {
    QList<MsaDbRowSnapshot> mcaRowsDatabaseData;
    UdrDbi* udrDbi = connection.dbi->getUdrDbi();
    SAFE_POINT_EXT(udrDbi != nullptr, os.setError("NULL UDR Dbi during importing an alignment"), mcaRowsDatabaseData);
    U2SequenceDbi* sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(sequenceDbi != nullptr, os.setError("NULL Sequence Dbi during importing an alignment"), mcaRowsDatabaseData);

    const DNAAlphabet* alphabet = mca->getAlphabet();
    SAFE_POINT_EXT(alphabet != nullptr, os.setError("MCA alphabet is NULL"), mcaRowsDatabaseData);
    const U2AlphabetId alphabetId = alphabet->getId();

    foreach (const MultipleAlignmentRow& row, mca->getRows()) {
        MsaDbRowSnapshot mcaRowDatabaseData;

        mcaRowDatabaseData.chromatogram = importChromatogram(os, connection, folder, row->getChromatogram());
        CHECK_OP(os, mcaRowsDatabaseData);

        mcaRowDatabaseData.sequence = importReferenceSequence(os, connection, folder, row->getSequence(), alphabetId);
        CHECK_OP(os, mcaRowsDatabaseData);

        createRelation(os, connection, mcaRowDatabaseData.sequence, mcaRowDatabaseData.chromatogram.id);

        mcaRowDatabaseData.additionalInfo = row->getAdditionalInfo();
        importRowAdditionalInfo(os, connection, mcaRowDatabaseData.chromatogram, mcaRowDatabaseData.additionalInfo);
        CHECK_OP(os, mcaRowsDatabaseData);

        mcaRowDatabaseData.gapModel = row->getGaps();
        mcaRowDatabaseData.rowLength = row->getRowLengthWithoutTrailing();

        mcaRowsDatabaseData << mcaRowDatabaseData;
    }

    return mcaRowsDatabaseData;
}

QList<U2MsaRow> MsaImportUtils::importMcaRows(U2OpStatus& os,
                                              const DbiConnection& connection,
                                              U2Msa& dbMca,
                                              const QList<MsaDbRowSnapshot>& snapshots) {
    QList<U2MsaRow> rows;

    for (const MsaDbRowSnapshot& snapshot : qAsConst(snapshots)) {
        U2MsaRow row;
        row.sequenceId = snapshot.sequence.id;
        row.gaps = snapshot.gapModel;
        row.gstart = 0;
        row.gend = snapshot.sequence.length;
        row.length = snapshot.rowLength;
        rows << row;
    }

    U2EntityRef mcaRef(connection.dbi->getDbiRef(), dbMca.id);
    for (int i = 0; i < rows.length(); i++) {
        MsaDbiUtils::addRow(mcaRef, -1, rows[i], os);
        CHECK_OP(os, {});
    }
    return rows;
}

U2Chromatogram MsaImportUtils::importChromatogram(U2OpStatus& os,
                                                  const DbiConnection& connection,
                                                  const QString& folder,
                                                  const Chromatogram& chromatogram) {
    const U2EntityRef chromatogramRef = ChromatogramUtils::import(os, connection.dbi->getDbiRef(), folder, chromatogram);
    CHECK_OP(os, U2Chromatogram());
    connection.dbi->getObjectDbi()->setObjectRank(chromatogramRef.entityId, U2DbiObjectRank_Child, os);
    CHECK_OP(os, U2Chromatogram());
    return ChromatogramUtils::getChromatogramDbInfo(os, chromatogramRef);
}

U2Sequence MsaImportUtils::importReferenceSequence(U2OpStatus& os,
                                                   const DbiConnection& connection,
                                                   const QString& folder,
                                                   const DNASequence& sequence,
                                                   const U2AlphabetId& alphabetId) {
    const U2EntityRef sequenceRef = U2SequenceUtils::import(os, connection.dbi->getDbiRef(), folder, sequence, alphabetId);
    CHECK_OP(os, U2Sequence());
    connection.dbi->getObjectDbi()->setObjectRank(sequenceRef.entityId, U2DbiObjectRank_Child, os);
    CHECK_OP(os, U2Sequence());
    return connection.dbi->getSequenceDbi()->getSequenceObject(sequenceRef.entityId, os);
}

void MsaImportUtils::importRowAdditionalInfo(U2OpStatus& os, const DbiConnection& connection, const U2Chromatogram& chromatogram, const QVariantMap& additionalInfo) {
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

void MsaImportUtils::createRelation(U2OpStatus& os, const DbiConnection& connection, const U2Sequence& sequence, const U2DataId& chromatogramId) {
    U2ObjectRelation dbRelation;
    dbRelation.id = chromatogramId;
    dbRelation.referencedName = sequence.visualName;
    dbRelation.referencedObject = sequence.id;
    dbRelation.referencedType = GObjectTypes::SEQUENCE;
    dbRelation.relationRole = ObjectRole_Sequence;

    connection.dbi->getObjectRelationsDbi()->createObjectRelation(dbRelation, os);
    CHECK_OP(os, );
}

}  // namespace U2
