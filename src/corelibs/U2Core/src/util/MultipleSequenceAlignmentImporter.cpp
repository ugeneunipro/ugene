/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/L10n.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleAlignmentInfo.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include "MultipleSequenceAlignmentImporter.h"

namespace U2 {

MultipleSequenceAlignmentObject * MultipleSequenceAlignmentImporter::createAlignment(const U2DbiRef& dbiRef, MultipleSequenceAlignment& al, U2OpStatus& os) {
    return createAlignment(dbiRef, U2ObjectDbi::ROOT_FOLDER, al, os);
}

MultipleSequenceAlignmentObject * MultipleSequenceAlignmentImporter::createAlignment(const U2DbiRef &dbiRef, const QString &folder, MultipleSequenceAlignment &al,
    U2OpStatus &os, const QList<U2Sequence> &alignedSeqs)
{
    if (!alignedSeqs.isEmpty() && alignedSeqs.size() != al->getNumRows()) {
        os.setError(QObject::tr("Unexpected number of sequences in a multiple alignment"));
        return NULL;
    }
    DbiConnection con(dbiRef, true, os);
    CHECK(!os.isCanceled(), NULL);
    SAFE_POINT_OP(os, NULL);
    SAFE_POINT_EXT(NULL != con.dbi, os.setError(L10N::nullPointerError("Destination database")), NULL);

    TmpDbiObjects objs(dbiRef, os); // remove the MSA object if opStatus is incorrect

    // MSA object and info
    U2DataId msaId = createEmptyMsaObject(con, folder, al->getName(), al->getAlphabet(), os);
    CHECK_OP(os, NULL);

    objs.objects << msaId;

    importMsaInfo(con, msaId, al->getInfo(), os);
    CHECK_OP(os, NULL);

    // MSA rows
    QList<U2Sequence> sequences;
    U2MsaListGapModel gapModel;

    if (alignedSeqs.isEmpty()) {
        sequences = importSequences(con, folder, al, os);
        gapModel = al->getGapModel();
        CHECK_OP(os, NULL);
    } else {
        setChildRankForSequences(con, alignedSeqs, os);
        CHECK_OP(os, NULL);

        sequences = alignedSeqs;
        splitToCharsAndGaps(con, sequences, gapModel, os);
        CHECK_OP(os, NULL);
    }

    QList<U2MsaRow> rows = importRows(con, al, msaId, sequences, gapModel, os);
    CHECK_OP(os, NULL);
    SAFE_POINT_EXT(rows.size() == al->getNumRows(), os.setError(QObject::tr("Unexpected error on MSA rows import")), NULL);

    if (!rows.isEmpty()) {
        // if the imported alignment is not empty -> set it length to the max(len, orig-len).
        // this way we preserve trailing gaps removed by the importRows call.
        qint64 srcAlignmentLen = al->getLength();
        qint64 newAlignmentLen = con.dbi->getMsaDbi()->getMsaLength(msaId, os);
        if (srcAlignmentLen > newAlignmentLen) {
            con.dbi->getMsaDbi()->updateMsaLength(msaId, srcAlignmentLen, os);
            CHECK_OP(os, NULL);
        }
    }

    for (int i = 0, n = al->getNumRows(); i < n; ++i) {
        al->getMsaRow(i)->setRowDbInfo(rows.at(i));
    }

    return new MultipleSequenceAlignmentObject(al->getName(), U2EntityRef(dbiRef, msaId), QVariantMap(), al);
}

void MultipleSequenceAlignmentImporter::setChildRankForSequences(const DbiConnection &con, const QList<U2Sequence> &sequences, U2OpStatus &os) {
    SAFE_POINT(NULL != con.dbi, L10N::nullPointerError("database connection"), );
    U2ObjectDbi *objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(NULL != objDbi, L10N::nullPointerError("object storage"), );

    foreach (const U2Sequence &seq, sequences) {
        objDbi->setObjectRank(seq.id, U2DbiObjectRank_Child, os);
        CHECK_OP(os, );
    }
}

U2DataId MultipleSequenceAlignmentImporter::createEmptyMsaObject(const DbiConnection& con, const QString& folder, const QString& name, const DNAAlphabet* alphabet, U2OpStatus& os) {
    SAFE_POINT(alphabet != NULL, "The alignment alphabet is NULL during importing!", U2DataId());

    QString visualName = name;
    if (visualName.isEmpty()) {
        QDate date = QDate::currentDate();
        QString generatedName = "MSA" + date.toString();
        coreLog.trace(QString("A multiple alignment name was empty! Generated a new name %1").arg(generatedName));
        visualName = generatedName;
    }

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "NULL MSA Dbi during importing an alignment!", U2DataId());

    U2DataId id = msaDbi->createMsaObject(folder, visualName, alphabet->getId(), 0, os);
    CHECK_OP(os, U2DataId());

    return id;
}

void MultipleSequenceAlignmentImporter::importMsaInfo(const DbiConnection& con, const U2DataId& msaId, const QVariantMap& alInfo, U2OpStatus& os) {
    U2AttributeDbi* attrDbi = con.dbi->getAttributeDbi();
    SAFE_POINT(NULL != attrDbi, "NULL Attribute Dbi during importing an alignment!",);

    foreach (QString key, alInfo.keys()) {
        if (key != MultipleAlignmentInfo::NAME) { // name is stored in the object
            QString val =  alInfo.value(key).value<QString>();
            U2StringAttribute attr(msaId, key, val);

            attrDbi->createStringAttribute(attr, os);
            CHECK_OP(os, );
        }
    }
}

QList<U2Sequence> MultipleSequenceAlignmentImporter::importSequences(const DbiConnection& con, const QString& folder, const MultipleSequenceAlignment& al, U2OpStatus& os) {
    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    SAFE_POINT(NULL != seqDbi, "NULL Sequence Dbi during importing an alignment!", QList<U2Sequence>());

    QList<U2Sequence> sequences;
    for (int i = 0; i < al->getNumRows(); ++i) {
        DNASequence dnaSeq = al->getMsaRow(i)->getSequence();

        U2Sequence sequence = U2Sequence();
        sequence.visualName = dnaSeq.getName();
        sequence.circular = dnaSeq.circular;
        sequence.length = dnaSeq.length();

        const DNAAlphabet* alphabet = dnaSeq.alphabet;
        if (NULL == alphabet) {
            alphabet = U2AlphabetUtils::findBestAlphabet(dnaSeq.constData(), dnaSeq.length());
        }
        SAFE_POINT(NULL != alphabet, "Failed to get alphabet for a sequence!", QList<U2Sequence>());
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

void MultipleSequenceAlignmentImporter::splitToCharsAndGaps(const DbiConnection &con, QList<U2Sequence> &sequences, U2MsaListGapModel &gapModel, U2OpStatus &os) {
    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    SAFE_POINT(NULL != seqDbi, "NULL Sequence Dbi during importing an alignment!", );

    gapModel.clear();
    for (int i = 0; i < sequences.size(); i++) {
        const QByteArray sequenceData = seqDbi->getSequenceData(sequences[i].id, U2_REGION_MAX, os);
        CHECK_OP(os, );

        QByteArray pureSequenceData;
        U2MsaRowGapModel sequenceGapModel;
        MaDbiUtils::splitBytesToCharsAndGaps(sequenceData, pureSequenceData, sequenceGapModel);
        gapModel << sequenceGapModel;

        if (sequenceGapModel.isEmpty()) {
            continue;
        }

        QVariantMap hints;
        hints[U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH] = true;
        hints[U2SequenceDbiHints::EMPTY_SEQUENCE] = false;

        seqDbi->updateSequenceData(sequences[i].id, U2_REGION_MAX, pureSequenceData, hints, os);
        CHECK_OP(os, );

        sequences[i].length = pureSequenceData.size();
    }
}

QList<U2MsaRow> MultipleSequenceAlignmentImporter::importRows(const DbiConnection& con, MultipleSequenceAlignment& al, const U2DataId& msaId, const QList<U2Sequence> &sequences, const U2MsaListGapModel &msaGapModel, U2OpStatus& os) {
    QList<U2MsaRow> rows;
    SAFE_POINT_EXT(sequences.size() == msaGapModel.size(), os.setError("Gap model doesn't fit sequences count"), rows);

    for (int rowIdx = 0, seqIdx = 0; rowIdx < al->getNumRows(); ++rowIdx, ++seqIdx) {
        U2Sequence seq = sequences[seqIdx];
        if (seq.length > 0) {
            MultipleSequenceAlignmentRow alignmentRow = al->getMsaRow(rowIdx);
            const U2MsaRowGapModel gapModel = msaGapModel[rowIdx];
            if (!gapModel.isEmpty() && (gapModel.last().offset + gapModel.last().gap) == MsaRowUtils::getRowLength(alignmentRow->getSequence().seq, gapModel)) {
                // remove trailing gap if it exists
                U2MsaRowGapModel newGapModel = gapModel;
                newGapModel.removeLast();
                alignmentRow->setGapModel(newGapModel);
            }

            U2MsaRow row;
            row.sequenceId = seq.id;
            row.gstart = 0;
            row.gend = seq.length;
            row.gaps = alignmentRow->getGapModel();
            row.length = alignmentRow->getRowLengthWithoutTrailing();

            rows.append(row);
        } else {
            al->removeRow(rowIdx, os);
            --rowIdx;
        }
    }

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "NULL MSA Dbi during importing an alignment!", QList<U2MsaRow>());

    msaDbi->addRows(msaId, rows, os);
    CHECK_OP(os, QList<U2MsaRow>());
    return rows;
}

} // namespace
