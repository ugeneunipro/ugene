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

#include "U2SequenceUtils.h"

#include <QApplication>
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GObject.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

namespace U2 {

const QString U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH = "update-length";
const QString U2SequenceDbiHints::EMPTY_SEQUENCE = "empty-sequence";

DNAAlphabetType U2SequenceUtils::alphabetType(const U2EntityRef& ref, U2OpStatus& os) {
    DNAAlphabetType res = DNAAlphabet_RAW;
    DbiConnection con(ref.dbiRef, os);

    U2Sequence seq = con.dbi->getSequenceDbi()->getSequenceObject(ref.entityId, os);
    CHECK_OP(os, res);

    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(seq.alphabet.id);
    CHECK_EXT(al != nullptr, os.setError(tr("Alphabet is not found!")), res);

    return al->getType();
}

qint64 U2SequenceUtils::length(const U2EntityRef& ref, U2OpStatus& os) {
    DbiConnection con(ref.dbiRef, os);

    U2Sequence seq = con.dbi->getSequenceDbi()->getSequenceObject(ref.entityId, os);
    CHECK_OP(os, -1);

    return seq.length;
}

U2Sequence U2SequenceUtils::copySequence(const DNASequence& srcSeq, const U2DbiRef& dstDbi, const QString& dstFolder, U2OpStatus& os) {
    U2Sequence res;
    // TODO: ClustalW format does not assign sequence alphabets!
    res.alphabet = srcSeq.alphabet == nullptr ? nullptr : srcSeq.alphabet->getId();
    res.circular = srcSeq.circular;
    res.length = srcSeq.length();
    res.visualName = srcSeq.getName();

    TmpDbiObjects tmpObjects(dstDbi, os);

    if (os.isCoR()) {
        return res;
    }

    DbiConnection dstCon(dstDbi, os);
    CHECK_OP(os, res);
    dstCon.dbi->getSequenceDbi()->createSequenceObject(res, dstFolder, os);
    CHECK_OP(os, res);

    tmpObjects.objects.append(res.id);

    dstCon.dbi->getSequenceDbi()->updateSequenceData(res.id, U2Region(0, 0), srcSeq.seq, QVariantMap(), os);
    CHECK_OP(os, res);

    return res;
}

U2Sequence U2SequenceUtils::copySequence(const U2EntityRef& srcSeq, const U2DbiRef& dstDbi, const QString& dstFolder, U2OpStatus& os) {
    U2Sequence res;
    DbiConnection srcCon(srcSeq.dbiRef, os);
    CHECK_OP(os, res);

    U2SequenceDbi* srcSeqDbi = srcCon.dbi->getSequenceDbi();
    SAFE_POINT_EXT(srcSeqDbi != nullptr, os.setError("Invalid sequence DBI"), res);
    U2Sequence seq = srcSeqDbi->getSequenceObject(srcSeq.entityId, os);
    CHECK_OP(os, res);

    res = seq;
    U2TrackModType modType = res.trackModType;
    res.trackModType = NoTrack;

    res.id.clear();
    res.length = 0;

    TmpDbiObjects tmpObjects(dstDbi, os);

    DbiConnection dstCon(dstDbi, os);
    CHECK_OP(os, res);
    U2SequenceDbi* dstSeqDbi = dstCon.dbi->getSequenceDbi();
    SAFE_POINT_EXT(dstSeqDbi != nullptr, os.setError("Invalid sequence DBI"), res);
    dstSeqDbi->createSequenceObject(res, dstFolder, os);
    CHECK_OP(os, res);

    tmpObjects.objects.append(res.id);

    const qint64 MAX_CHUNK_LENGTH = 4194304;  // 4 MiB chunk
    for (qint64 pos = 0; pos < seq.length; pos += MAX_CHUNK_LENGTH) {
        const qint64 currentChunkSize = qMin(MAX_CHUNK_LENGTH, seq.length - pos);
        const U2Region chunkRegion(pos, currentChunkSize);
        const QByteArray chunkContent = srcSeqDbi->getSequenceData(srcSeq.entityId, chunkRegion, os);
        CHECK_OP(os, res);
        dstSeqDbi->updateSequenceData(res.id, chunkRegion, chunkContent, QVariantMap(), os);
        CHECK_OP(os, res);
        res.length += currentChunkSize;
    }

    U2DbiObjectRank rank = srcCon.dbi->getObjectDbi()->getObjectRank(seq.id, os);
    CHECK_OP(os, res);

    dstCon.dbi->getObjectDbi()->setObjectRank(res.id, rank, os);
    CHECK_OP(os, res);

    res.trackModType = modType;
    return res;
}

void U2SequenceUtils::updateSequenceName(const U2EntityRef& entityRef, const QString& newName, U2OpStatus& os) {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );

    U2SequenceDbi* sequenceDbi = con.dbi->getSequenceDbi();
    U2Sequence sequenceObject = sequenceDbi->getSequenceObject(entityRef.entityId, os);
    CHECK_OP(os, );
    if (sequenceObject.visualName != newName) {
        sequenceObject.visualName = newName;
        sequenceDbi->updateSequenceObject(sequenceObject, os);
    }
}

static QList<QByteArray> _extractRegions(const U2EntityRef& seqRef, const QVector<U2Region>& regions, const DNATranslation* complTT, U2OpStatus& os) {
    QList<QByteArray> res;

    DbiConnection con(seqRef.dbiRef, os);
    CHECK_OP(os, res);
    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    U2Sequence seq = seqDbi->getSequenceObject(seqRef.entityId, os);
    CHECK_OP(os, res);

    QVector<U2Region> safeLocation = regions;
    U2Region::bound(0, seq.length, safeLocation);

    for (int i = 0, n = safeLocation.size(); i < n; i++) {
        const U2Region& oReg = safeLocation.at(i);
        if (complTT == nullptr) {
            QByteArray part = seqDbi->getSequenceData(seq.id, U2Region(oReg.startPos, oReg.length), os);
            CHECK_OP(os, QList<QByteArray>());
            res.append(part);
        } else {
            QByteArray arr = seqDbi->getSequenceData(seq.id, U2Region(oReg.startPos, oReg.length), os);
            CHECK_OP(os, QList<QByteArray>());
            TextUtils::reverse(arr.data(), arr.length());
            complTT->translate(arr.data(), arr.length());
            res.prepend(arr);
        }
    }
    return res;
}

QList<QByteArray> U2SequenceUtils::extractRegions(const U2EntityRef& seqRef, const QVector<U2Region>& origLocation, const DNATranslation* complTT, const DNATranslation* aminoTT, bool join, U2OpStatus& os) {
    QList<QByteArray> res = _extractRegions(seqRef, origLocation, complTT, os);
    CHECK_OP(os, res)

    DbiConnection con(seqRef.dbiRef, os);
    CHECK_OP(os, res);

    U2SequenceDbi* seqDbi = con.dbi->getSequenceDbi();
    U2Sequence seq = seqDbi->getSequenceObject(seqRef.entityId, os);
    CHECK_OP(os, res);

    if (seq.circular && res.size() > 1) {
        const U2Region& firstL = origLocation.first();
        const U2Region& lastL = origLocation.last();
        if (firstL.startPos == 0 && lastL.endPos() == seq.length) {
            QByteArray lastS = res.last();
            QByteArray firstS = res.first();
            res.removeLast();
            res[0] = lastS.append(firstS);
        }
    }
    if (aminoTT != nullptr) {
        res = U1SequenceUtils::translateRegions(res, aminoTT, join);
    }

    if (join && res.size() > 1) {
        QByteArray joined = U1SequenceUtils::joinRegions(res);
        res.clear();
        res.append(joined);
    }

    return res;
}

U2EntityRef U2SequenceUtils::import(U2OpStatus& os, const U2DbiRef& dbiRef, const DNASequence& seq, const U2AlphabetId& alphabetId) {
    return import(os, dbiRef, U2ObjectDbi::ROOT_FOLDER, seq, alphabetId);
}

U2EntityRef U2SequenceUtils::import(U2OpStatus& os, const U2DbiRef& dbiRef, const QString& folder, const DNASequence& seq, const U2AlphabetId& alphabetId) {
    U2EntityRef res;
    U2SequenceImporter i;

    i.startSequence(os, dbiRef, folder, seq.getName(), seq.circular, alphabetId);
    CHECK_OP(os, res);

    i.addBlock(seq.constData(), seq.length(), os);
    CHECK_OP(os, res);

    U2Sequence u2seq = i.finalizeSequenceAndValidate(os);
    CHECK_OP(os, res);

    res.dbiRef = dbiRef;
    res.entityId = u2seq.id;

    setSequenceInfo(os, res, seq.info);
    CHECK_OP(os, res);

    setQuality(res, seq.quality);

    return res;
}

void U2SequenceUtils::setQuality(const U2EntityRef& entityRef, const DNAQuality& q) {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QList<U2DataId> idQualList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::FASTQ_QUAL_CODES, os);
    CHECK_OP(os, );
    if (!idQualList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeAttributes(idQualList, os);
        CHECK_OP(os, );
    }
    QList<U2DataId> idQualTypeList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::FASTQ_QUAL_TYPE, os);
    CHECK_OP(os, );
    if (!idQualTypeList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeAttributes(idQualTypeList, os);
        CHECK_OP(os, );
    }

    U2ByteArrayAttribute qualityCodes(entityRef.entityId, DNAInfo::FASTQ_QUAL_CODES, q.qualCodes);
    U2IntegerAttribute qualityType(entityRef.entityId, DNAInfo::FASTQ_QUAL_TYPE, q.type);
    con.dbi->getAttributeDbi()->createByteArrayAttribute(qualityCodes, os);
    CHECK_OP(os, );
    con.dbi->getAttributeDbi()->createIntegerAttribute(qualityType, os);
    CHECK_OP(os, );
}

void U2SequenceUtils::setSequenceInfo(U2OpStatus& os, const U2EntityRef& entityRef, const QVariantMap& info) {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QList<U2DataId> chainIdList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::CHAIN_ID, os);
    CHECK_OP(os, );
    if (!chainIdList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(chainIdList.first(), os);
        CHECK_OP(os, );
    }
    QList<U2DataId> commentList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::COMMENT, os);
    CHECK_OP(os, );
    if (!commentList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(commentList.first(), os);
        CHECK_OP(os, );
    }
    QList<U2DataId> definitionList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::DEFINITION, os);
    CHECK_OP(os, );
    if (!definitionList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(definitionList.first(), os);
        CHECK_OP(os, );
    }
    U2StringAttribute chainID(entityRef.entityId, DNAInfo::CHAIN_ID, info.value(DNAInfo::CHAIN_ID).toString());
    U2StringAttribute comment(entityRef.entityId, DNAInfo::COMMENT, info.value(DNAInfo::COMMENT).toString());
    U2StringAttribute definition(entityRef.entityId, DNAInfo::DEFINITION, info.value(DNAInfo::DEFINITION).toString());
    con.dbi->getAttributeDbi()->createStringAttribute(chainID, os);
    CHECK_OP(os, );
    con.dbi->getAttributeDbi()->createStringAttribute(comment, os);
    CHECK_OP(os, );
    con.dbi->getAttributeDbi()->createStringAttribute(definition, os);
    CHECK_OP(os, );
}

QVariantMap U2SequenceUtils::getSequenceInfo(U2OpStatus& os, const U2EntityRef& entityRef, const QString& name) {
    QVariantMap resultingInfo;
    DbiConnection con(entityRef.dbiRef, os);
    QList<U2DataId> chainIdList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::CHAIN_ID, os);
    CHECK_OP(os, resultingInfo);
    QList<U2DataId> commentList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::COMMENT, os);
    CHECK_OP(os, resultingInfo);
    QList<U2DataId> definitionList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::DEFINITION, os);
    CHECK_OP(os, resultingInfo);
    if (!chainIdList.isEmpty() && !commentList.isEmpty() && !definitionList.isEmpty()) {
        resultingInfo.insert(DNAInfo::CHAIN_ID, con.dbi->getAttributeDbi()->getStringAttribute(chainIdList.first(), os).value);
        CHECK_OP(os, QVariantMap());
        resultingInfo.insert(DNAInfo::COMMENT, con.dbi->getAttributeDbi()->getStringAttribute(commentList.first(), os).value);
        CHECK_OP(os, QVariantMap());
        resultingInfo.insert(DNAInfo::DEFINITION, con.dbi->getAttributeDbi()->getStringAttribute(definitionList.first(), os).value);
        CHECK_OP(os, QVariantMap());
    }
    U2StringAttribute attr = U2AttributeUtils::findStringAttribute(con.dbi->getAttributeDbi(), entityRef.entityId, DNAInfo::GENBANK_HEADER, os);
    if (attr.hasValidId()) {
        resultingInfo.insert(DNAInfo::GENBANK_HEADER, attr.value);
        CHECK_OP(os, QVariantMap());
    }

    attr = U2AttributeUtils::findStringAttribute(con.dbi->getAttributeDbi(), entityRef.entityId, DNAInfo::SOURCE, os);
    if (attr.hasValidId()) {
        resultingInfo.insert(DNAInfo::SOURCE, attr.value);
        CHECK_OP(os, QVariantMap());
    }

    attr = U2AttributeUtils::findStringAttribute(con.dbi->getAttributeDbi(), entityRef.entityId, DNAInfo::ACCESSION, os);
    if (attr.hasValidId()) {
        resultingInfo.insert(DNAInfo::ACCESSION, attr.value);
        CHECK_OP(os, QVariantMap());
    }
    attr = U2AttributeUtils::findStringAttribute(con.dbi->getAttributeDbi(), entityRef.entityId, Translation_Table_Id_Attribute, os);
    if (attr.hasValidId()) {
        resultingInfo.insert(Translation_Table_Id_Attribute, attr.value);
        CHECK_OP(os, QVariantMap());
    }

    if (!name.isEmpty()) {
        resultingInfo.insert(DNAInfo::ID, name);
    }
    return resultingInfo;
}

U2Sequence U2SequenceUtils::getSequenceDbInfo(U2SequenceObject* seqObj) {
    U2Sequence seq;

    seq.id = seqObj->getEntityRef().entityId;
    seq.dbiId = seqObj->getEntityRef().dbiRef.dbiId;
    if (seqObj->getAlphabet() != nullptr) {
        seq.alphabet.id = seqObj->getAlphabet()->getId();
    }
    seq.circular = seqObj->isCircular();
    seq.length = seqObj->getSequenceLength();
    seq.visualName = seqObj->getSequenceName();

    return seq;
}

//////////////////////////////////////////////////////////////////////////
// U2SequenceImporter
#define DEFAULT_SEQUENCE_INSERT_BLOCK_SIZE (4 * 1024 * 1024)

/**
 * If GObjectHint_CaseAnns is present in the QVariantMap (it should be stored as int),
 * then the method verifies the value and returns it (if it is correct).
 * Otherwise, returns NO_CASE_ANNS.
 */
static CaseAnnotationsMode getCaseAnnotationsModeHint(const QVariantMap& fs) {
    if (fs.keys().contains(GObjectHint_CaseAnns)) {
        QVariant caseAnnsVariant = fs.value(GObjectHint_CaseAnns);
        SAFE_POINT(caseAnnsVariant.canConvert<int>(), "Can't convert a case annotations hint!", NO_CASE_ANNS);

        bool conversionIsOK = 0;
        int caseAnnsInt = caseAnnsVariant.toInt(&conversionIsOK);
        SAFE_POINT(conversionIsOK, "Can't convert a case annotations hint to int!", NO_CASE_ANNS);

        SAFE_POINT((caseAnnsInt == LOWER_CASE) || (caseAnnsInt == UPPER_CASE) || (caseAnnsInt == NO_CASE_ANNS),
                   "Incorrect value of a case annotation hint!",
                   NO_CASE_ANNS);

        return (CaseAnnotationsMode)caseAnnsInt;
    }

    return NO_CASE_ANNS;
}

const QString U2SequenceImporter::EMPTY_SEQUENCE_ERROR = QApplication::translate("U2SequenceImporter",
                                                                                 "Sequence was not imported. Probably, this is because the sequence is empty.");

U2SequenceImporter::U2SequenceImporter(const QVariantMap& fs, bool lazyMode, bool singleThread)
    : lazyMode(lazyMode), singleThread(singleThread), sequenceCreated(false) {
    insertBlockSize = DEFAULT_SEQUENCE_INSERT_BLOCK_SIZE;
    currentLength = 0;
    isUnfinishedRegion = false;
    caseAnnsMode = getCaseAnnotationsModeHint(fs);
    sequenceCreated = false;
    committedLength = 0;
}

U2SequenceImporter::U2SequenceImporter(qint64 _insertBlockSize, const QVariantMap& fs, bool lazyMode, bool singleThread)
    : insertBlockSize(_insertBlockSize), lazyMode(lazyMode), singleThread(singleThread) {
    insertBlockSize = qMin((qint64)10, insertBlockSize);
    currentLength = 0;
    isUnfinishedRegion = false;
    caseAnnsMode = getCaseAnnotationsModeHint(fs);
    sequenceCreated = false;
    committedLength = 0;
}

U2SequenceImporter::~U2SequenceImporter() {
    if (con.isOpen() && sequenceCreated) {
        coreLog.trace(QString("Removing sequence from unfinished import: %1").arg(sequence.visualName));
        U2OpStatus2Log os;
        con.dbi->getObjectDbi()->removeObject(sequence.id, os);
    }
}

void U2SequenceImporter::enableAminoTranslation(const DNATranslation* newAminoTT) {
    SAFE_POINT(!sequenceCreated, "enableAminoTranslation can be set only during initialization", );
    this->aminoTT = newAminoTT;
}

void U2SequenceImporter::enableReverseComplement(const DNATranslation* newComplTT) {
    SAFE_POINT(!sequenceCreated, "enableReverseComplement can be set only during initialization", );
    this->complTT = newComplTT;
}

void U2SequenceImporter::startSequence(U2OpStatus& os,
                                       const U2DbiRef& dbiRef,
                                       const QString& dstFolder,
                                       const QString& visualName,
                                       bool circular,
                                       const U2AlphabetId& alphabetId) {
    SAFE_POINT(!con.isOpen(), "Connection is already opened!", );
    con.open(dbiRef, true, os);
    CHECK_OP(os, );

    folder = dstFolder;

    sequence = U2Sequence();
    sequence.visualName = visualName;
    sequence.circular = circular;
    sequence.alphabet.id = alphabetId.id;

    currentLength = 0;
    isUnfinishedRegion = false;
    annList.clear();

    alphabetCharacterHit.reset();

    if (!lazyMode) {
        con.dbi->getSequenceDbi()->createSequenceObject(sequence, folder, os);
        CHECK_OP(os, );
        sequenceCreated = true;
    }
}

void U2SequenceImporter::addBlock(const char* data, qint64 len, U2OpStatus& os) {
    CHECK(len > 0, );
    for (int i = 0; i < len; i++) {
        alphabetCharacterHit.set((unsigned char)data[i], true);
    }
    QByteArray bytes;
    for (size_t i = 0; i < alphabetCharacterHit.size(); i++) {
        if (alphabetCharacterHit[i]) {
            bytes.append(i);
        }
    }
    const DNAAlphabet* resAl = U2AlphabetUtils::findBestAlphabet(bytes);
    CHECK_EXT(resAl != nullptr, os.setError("Failed to match sequence alphabet!"), );
    if (resAl != U2AlphabetUtils::getById(sequence.alphabet)) {
        sequence.alphabet.id = resAl->getId();
        if (sequenceCreated) {
            con.dbi->getSequenceDbi()->updateSequenceObject(sequence, os);
            CHECK_OP(os, );
        }
    }

    _addBlock2Buffer(data, len, os);

    if (caseAnnsMode != NO_CASE_ANNS) {
        annList << U1AnnotationUtils::getCaseAnnotations(data, len, currentLength, isUnfinishedRegion, unfinishedRegion, LOWER_CASE == caseAnnsMode);
    }
    currentLength += len;
}

void U2SequenceImporter::addSequenceBlock(const U2EntityRef& sequenceRef, const U2Region& r, U2OpStatus& os) {
    _addBuffer2Db(os);
    CHECK_OP(os, );
    DbiConnection con(sequenceRef.dbiRef, os);
    CHECK_OP(os, );

    // TODO: optimize -> create utility that uses small to copy sequence!
    QByteArray arr = con.dbi->getSequenceDbi()->getSequenceData(sequenceRef.entityId, r, os);
    CHECK_OP(os, );
    addBlock(arr.constData(), arr.size(), os);
}

void U2SequenceImporter::addDefaultSymbolsBlock(int n, U2OpStatus& os) {
    SAFE_POINT(n >= 0, QString("Invalid number of symbols: %1").arg(n), );
    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(sequence.alphabet.id);
    if (al == nullptr) {
        os.setError(QObject::tr("Unable to detect sequence alphabet. Probably, this is because some of merged sequences are empty."));
        return;
    }
    char defaultChar = U2AlphabetUtils::getDefaultSymbol(sequence.alphabet);
    QByteArray a(n, defaultChar);
    _addBlock2Buffer(a.data(), a.size(), os);
    currentLength += n;
}

void U2SequenceImporter::_addBlock2Buffer(const char* data, qint64 dataLength, U2OpStatus& os) {
    CHECK(dataLength > 0, );
    const char* newBlock = data;
    int newBlockLength = (int)dataLength;

    QScopedPointer<QByteArray> complBlockPointer;  // container of the reverse-complementary block sequence.
    if (complTT != nullptr) {
        complBlockPointer.reset(new QByteArray(newBlockLength, Qt::Uninitialized));
        char* revComplBlock = complBlockPointer->data();
        TextUtils::reverse(newBlock, revComplBlock, newBlockLength);
        complTT->translate(revComplBlock, newBlockLength);
        newBlock = revComplBlock;
    }

    QScopedPointer<QByteArray> aminoBlockPointer;  // container of the translated block sequence.
    if (aminoTT != nullptr) {
        if (newBlockLength + aminoTranslationBuffer.size() < 3) {
            aminoTranslationBuffer.append(newBlock, newBlockLength);
            return;
        }
        const char* const dnaBlock = newBlock;
        aminoBlockPointer.reset(new QByteArray(newBlockLength / 3 + 1, Qt::Uninitialized));
        char* aminoBlock = aminoBlockPointer->data();
        int remainingDnaBlockLength = newBlockLength;
        int dnaBlockOffset = 0;
        bool hasCharFromAminoBuffer = false;
        // Finish the pending block first.
        if (!aminoTranslationBuffer.isEmpty()) {
            SAFE_POINT(aminoTranslationBuffer.length() <= 2, "Invalid size of aminoTranslationBuffer", );
            hasCharFromAminoBuffer = true;
            dnaBlockOffset = 3 - aminoTranslationBuffer.size();
            aminoTranslationBuffer.append(dnaBlock, dnaBlockOffset);
            aminoTT->translate(aminoTranslationBuffer.constData(), 3, aminoBlock, 1);
            aminoBlockPointer->append(dnaBlock, 1);
            remainingDnaBlockLength -= dnaBlockOffset;
            aminoTranslationBuffer.clear();
        }
        int aminoBlockLength = remainingDnaBlockLength / 3;
        aminoTT->translate(dnaBlock + dnaBlockOffset, remainingDnaBlockLength, aminoBlock + (hasCharFromAminoBuffer ? 1 : 0), aminoBlockLength);
        remainingDnaBlockLength = remainingDnaBlockLength % 3;

        newBlock = aminoBlock;
        newBlockLength = (hasCharFromAminoBuffer ? 1 : 0) + aminoBlockLength;

        if (remainingDnaBlockLength != 0) {
            aminoTranslationBuffer.append(dnaBlock + dataLength - remainingDnaBlockLength, remainingDnaBlockLength);
        }
    }

    if (newBlockLength + sequenceBuffer.length() < insertBlockSize) {
        sequenceBuffer.append(newBlock, newBlockLength);
        return;
    }
    _addBlock2Db(sequenceBuffer.data(), sequenceBuffer.length(), os);
    CHECK_OP(os, );
    sequenceBuffer.clear();
    _addBlock2Db(newBlock, newBlockLength, os);
}

void U2SequenceImporter::_addBlock2Db(const char* data, qint64 len, U2OpStatus& os) {
    SAFE_POINT(len >= 0, "Illegal block length!", );
    if (len == 0) {
        return;
    }
    QByteArray arr(data, len);
    TextUtils::translate(TextUtils::UPPER_CASE_MAP, arr.data(), arr.length());

    bool updateLength = true;
    bool emptySequence = false;
    bool justCreated = false;
    if (!sequenceCreated) {
        emptySequence = true;
        if (singleThread) {
            SAFE_POINT(0 == committedLength, "Sequence object is not created, but sequence data already exists", );
            sequence.length = len;
            updateLength = false;
        }
        con.dbi->getSequenceDbi()->createSequenceObject(sequence, folder, os);
        CHECK_OP(os, );
        sequenceCreated = true;
        justCreated = true;
    }

    QVariantMap hints;
    hints[U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH] = updateLength;
    hints[U2SequenceDbiHints::EMPTY_SEQUENCE] = emptySequence;
    U2Region reg(sequence.length, 0);
    if (justCreated) {
        reg.startPos = 0;
        reg.length = 0;
    }
    con.dbi->getSequenceDbi()->updateSequenceData(sequence.id, reg, arr, hints, os);
    CHECK_OP(os, );
    if (committedLength == sequence.length) {
        sequence.length += len;
    } else {  // because of lazyMode and delayed sequence creation
        sequence.length = committedLength + len;
    }
    committedLength += len;
}

void U2SequenceImporter::_addBuffer2Db(U2OpStatus& os) {
    CHECK(!sequenceBuffer.isEmpty(), );
    _addBlock2Db(sequenceBuffer.data(), sequenceBuffer.length(), os);
    sequenceBuffer.clear();
}

U2Sequence U2SequenceImporter::finalizeSequence(U2OpStatus& os) {
    _addBuffer2Db(os);
    LOG_OP(os);
    // If sequence is empty, addBlock is never called and alphabet is not set. So set it here to some default value
    if (!sequence.alphabet.isValid() && sequence.version != 0) {
        sequence.alphabet.id = BaseDNAAlphabetIds::RAW();
        con.dbi->getSequenceDbi()->updateSequenceObject(sequence, os);
        LOG_OP(os);
    }
    con.close(os);
    if (caseAnnsMode != NO_CASE_ANNS) {
        annList << U1AnnotationUtils::finalizeUnfinishedRegion(isUnfinishedRegion, unfinishedRegion, LOWER_CASE == caseAnnsMode);

        if (1 == annList.size()) {
            const QVector<U2Region>& regs = annList.first()->getRegions();
            if (1 == regs.size()) {
                U2Region reg = regs.first();
                if (0 == reg.startPos && sequence.length == reg.length) {
                    annList.clear();
                }
            }
        }
    }
    sequenceCreated = false;
    committedLength = 0;
    return sequence;
}

U2Sequence U2SequenceImporter::finalizeSequenceAndValidate(U2OpStatus& os) {
    U2Sequence result = finalizeSequence(os);
    CHECK_OP(os, result);
    if (!result.hasValidId()) {
        os.setError(EMPTY_SEQUENCE_ERROR);
    }
    return result;
}

void U2SequenceImporter::setCaseAnnotationsMode(CaseAnnotationsMode mode) {
    caseAnnsMode = mode;
}

bool U2SequenceImporter::isCaseAnnotationsModeOn() const {
    return caseAnnsMode != NO_CASE_ANNS;
}

QList<SharedAnnotationData>& U2SequenceImporter::getCaseAnnotations() {
    return annList;
}

qint64 U2SequenceImporter::getCurrentLength() const {
    return currentLength;
}

void U2MemorySequenceImporter::addBlock(const char* data, qint64 len, U2OpStatus& os) {
    SAFE_POINT(aminoTT == nullptr, "Import with amino translation is not supported by U2MemorySequenceImporter", );
    SAFE_POINT(complTT == nullptr, "Import with reverse-complementary translation is not supported by U2MemorySequenceImporter", );
    if (qstrlen(data) < len) {
        os.setError("Wrong data length in addBlock");
        return;
    }

    // derive common alphabet
    const DNAAlphabet* blockAl = U2AlphabetUtils::findBestAlphabet(data, len);
    CHECK_EXT(blockAl != nullptr, os.setError("Failed to match sequence alphabet!"), );

    const DNAAlphabet* oldAl = U2AlphabetUtils::getById(sequence.alphabet);
    const DNAAlphabet* resAl = blockAl;
    if (oldAl != nullptr) {
        if (oldAl->getType() == DNAAlphabet_AMINO && resAl->getType() == DNAAlphabet_NUCL) {
            resAl = oldAl;
        } else if (resAl->getType() == DNAAlphabet_AMINO && oldAl->getType() == DNAAlphabet_NUCL) {
            oldAl = resAl;
        } else {
            resAl = U2AlphabetUtils::deriveCommonAlphabet(blockAl, oldAl);
        }
        CHECK_EXT(resAl != nullptr, os.setError(U2SequenceUtils::tr("Failed to derive sequence alphabet!")), );
    }

    if (resAl != U2AlphabetUtils::getById(sequence.alphabet)) {
        sequence.alphabet.id = resAl->getId();
    }

    sequenceData.append(data, len);
}

void U2MemorySequenceImporter::addDefaultSymbolsBlock(int n, U2OpStatus& os) {
    SAFE_POINT_EXT(n >= 0, os.setError(QString("Invalid number of symbols: %1").arg(n)), );
    char defaultChar = U2AlphabetUtils::getDefaultSymbol(sequence.alphabet);
    QByteArray a(n, defaultChar);

    sequenceData.append(a);
}

qint64 U2MemorySequenceImporter::getCurrentLength() const {
    return sequenceData.length();
}

QByteArray U2PseudoCircularization::createSequenceWithCircularOverlaps(const QByteArray& sequence, int maxLinearRegionLength) {
    int linearRegionLength = maxLinearRegionLength < 0 ? sequence.length() : maxLinearRegionLength;
    QByteArray result = sequence;
    result.append(result.left(linearRegionLength));
    return result;
}

QVector<U2Region> U2PseudoCircularization::getOriginalSequenceCoordinates(const U2Region& circularRegion, qint64 originalSequenceLength) {
    SAFE_POINT(circularRegion.endPos() <= originalSequenceLength * 2, "Invalid circular region", {});
    if (circularRegion.endPos() <= originalSequenceLength) {
        return {circularRegion};
    }
    if (circularRegion.startPos > originalSequenceLength) {
        return {{circularRegion.startPos - originalSequenceLength, circularRegion.length}};
    } else {
        return {
            {circularRegion.startPos, originalSequenceLength - circularRegion.startPos},
            {0, circularRegion.endPos() - originalSequenceLength},
        };
    }
}

void U2PseudoCircularization::convertToOriginalSequenceCoordinates(U2Location& location, qint64 originalSequenceLength) {
    QVector<U2Region> regions;
    for (const U2Region& region : qAsConst(location->regions)) {
        QVector<U2Region> originalRegions = getOriginalSequenceCoordinates(region, originalSequenceLength);
        if (originalRegions.length() > 1) {
            location->op = U2LocationOperator_Join;
        }
        regions << originalRegions;
    }
    location->regions = regions;
}

}  // namespace U2
