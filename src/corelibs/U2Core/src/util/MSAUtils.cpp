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

#include "MSAUtils.h"

#include <QListIterator>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

bool MSAUtils::equalsIgnoreGaps(const MultipleAlignmentRow& row, int startPos, const QByteArray& pat, int& alternateLen) {
    int sLen = row->getCoreEnd();
    int pLen = pat.size();
    int i = startPos;
    int gapsCounter = 0;
    for (int j = 0; i < sLen && j < pLen; i++, j++) {
        char c1 = row->charAt(i);
        char c2 = pat[j];
        while (c1 == U2Msa::GAP_CHAR && ++i < sLen) {
            gapsCounter++;
            c1 = row->charAt(i);
        }
        if (c1 != c2) {
            alternateLen = i - startPos;
            return false;
        }
    }
    alternateLen = i - startPos;
    if (alternateLen - gapsCounter < pLen) {
        return false;
    }
    return true;
}

int MSAUtils::getPatternSimilarityIgnoreGaps(const MultipleSequenceAlignmentRow& row, int startPos, const QByteArray& pat, int& alternateLen) {
    int sLen = row->getCoreEnd();
    int pLen = pat.size();
    int i = startPos;
    int similarity = 0;
    for (int j = 0; i < sLen && j < pLen; i++, j++) {
        char c1 = row->charAt(i);
        char c2 = pat[j];
        while (c1 == U2Msa::GAP_CHAR && ++i < sLen) {
            c1 = row->charAt(i);
        }
        if (c1 == c2) {
            similarity++;
        }
    }
    alternateLen = i - startPos;
    return similarity;
}

MultipleSequenceAlignment MSAUtils::seq2ma(const QList<DNASequence>& list, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw) {
    MultipleSequenceAlignment ma(MA_OBJECT_NAME);
    const DNAAlphabet* alphabet = deriveCommonAlphabet(list, recheckAlphabetFromDataIfRaw);
    ma->setAlphabet(alphabet);
    foreach (const DNASequence& seq, list) {
        // TODO: handle memory overflow
        ma->addRow(seq.getName(), seq.seq);
    }
    CHECK_OP(os, MultipleSequenceAlignment());
    return ma;
}

namespace {

MultipleSequenceAlignmentObject* prepareSequenceHeadersList(const QList<GObject*>& list, bool useGenbankHeader, QList<U2SequenceObject*>& dnaList, QList<QString>& nameList) {
    foreach (GObject* obj, list) {
        auto dnaObj = qobject_cast<U2SequenceObject*>(obj);
        if (dnaObj == nullptr) {
            if (auto maObj = qobject_cast<MultipleSequenceAlignmentObject*>(obj)) {
                return maObj;
            }
            continue;
        }

        QString rowName = dnaObj->getSequenceName();
        if (useGenbankHeader) {
            QString sourceName = dnaObj->getStringAttribute(DNAInfo::SOURCE);
            if (!sourceName.isEmpty()) {
                rowName = sourceName;
            }
        }

        dnaList << dnaObj;
        nameList << rowName;
    }
    return nullptr;
}

void appendSequenceToAlignmentRow(MultipleSequenceAlignment& ma, int rowIndex, int afterPos, const U2SequenceObject& seq, U2OpStatus& os) {
    U2Region seqRegion(0, seq.getSequenceLength());
    const qint64 blockReadFromBD = 4194305;  // 4 MB + 1

    qint64 sequenceLength = seq.getSequenceLength();
    for (qint64 startPosition = seqRegion.startPos; startPosition < seqRegion.length; startPosition += blockReadFromBD) {
        U2Region readRegion(startPosition, qMin(blockReadFromBD, sequenceLength - startPosition));
        QByteArray readedData = seq.getSequenceData(readRegion);
        ma->appendChars(rowIndex, afterPos, readedData.constData(), readedData.size());
        afterPos += readRegion.length;
        CHECK_OP(os, );
    }
}

}  // unnamed namespace

MultipleSequenceAlignment MSAUtils::seq2ma(const QList<GObject*>& list, U2OpStatus& os, bool useGenbankHeader, bool recheckAlphabetFromDataIfRaw) {
    QList<U2SequenceObject*> dnaList;
    QStringList nameList;

    MultipleSequenceAlignmentObject* obj = prepareSequenceHeadersList(list, useGenbankHeader, dnaList, nameList);
    if (obj != nullptr) {
        return obj->getMsaCopy();
    }

    MultipleSequenceAlignment ma(MA_OBJECT_NAME);
    ma->setAlphabet(deriveCommonAlphabet(dnaList, recheckAlphabetFromDataIfRaw, os));

    int i = 0;
    SAFE_POINT(dnaList.size() == nameList.size(), "DNA list size differs from name list size", MultipleSequenceAlignment());
    QListIterator<U2SequenceObject*> listIterator(dnaList);
    QListIterator<QString> nameIterator(nameList);
    while (listIterator.hasNext()) {
        const U2SequenceObject& seq = *(listIterator.next());
        const QString& objName = nameIterator.next();

        CHECK_OP(os, MultipleSequenceAlignment());

        ma->addRow(objName, QByteArray(""));

        SAFE_POINT(i < ma->getRowCount(), "Row count differ from expected after adding row", MultipleSequenceAlignment());
        appendSequenceToAlignmentRow(ma, i, 0, seq, os);
        CHECK_OP(os, MultipleSequenceAlignment());
        i++;
    }

    return ma;
}

static const DNAAlphabet* selectBestAlphabetForAlignment(const QList<const DNAAlphabet*>& availableAlphabets) {
    const DNAAlphabet* bestMatch = nullptr;
    foreach (const DNAAlphabet* alphabet, availableAlphabets) {
        if (bestMatch == nullptr || bestMatch->isRaw()) {  // prefer any other alphabet over RAW.
            bestMatch = alphabet;
            continue;
        }
        if (bestMatch->isDNA() && alphabet->isAmino()) {  // prefer Amino over DNA.
            bestMatch = alphabet;
            continue;
        }
        if (bestMatch->isExtended() && !alphabet->isExtended()) {  // narrow down the set of characters.
            bestMatch = alphabet;
        }
    }
    return bestMatch;
}

const DNAAlphabet* MSAUtils::deriveCommonAlphabet(const QList<DNASequence>& sequenceList, bool recheckAlphabetFromDataIfRaw) {
    // first perform fast check using sequence alphabets only.
    QList<const DNAAlphabet*> alphabetList;
    foreach (const DNASequence& sequence, sequenceList) {
        alphabetList.append(sequence.alphabet);
    }
    const DNAAlphabet* resultAlphabet = deriveCommonAlphabet(alphabetList);
    if (!resultAlphabet->isRaw() || !recheckAlphabetFromDataIfRaw) {
        return resultAlphabet;
    }
    // now perform slow check with raw data access.
    QSet<const DNAAlphabet*> availableAlphabets = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets().toSet();
    foreach (const DNASequence& sequence, sequenceList) {
        QList<const DNAAlphabet*> sequenceAlphabets = U2AlphabetUtils::findAllAlphabets(sequence.constData());
        availableAlphabets.intersect(sequenceAlphabets.toSet());
    }
    return selectBestAlphabetForAlignment(availableAlphabets.toList());
}

const DNAAlphabet* MSAUtils::deriveCommonAlphabet(const QList<U2SequenceObject*>& sequenceList, bool recheckAlphabetFromDataIfRaw, U2OpStatus& os) {
    // first perform fast check using sequence alphabets only.
    QList<const DNAAlphabet*> alphabetList;
    foreach (const U2SequenceObject* sequenceObject, sequenceList) {
        alphabetList.append(sequenceObject->getAlphabet());
    }
    const DNAAlphabet* resultAlphabet = deriveCommonAlphabet(alphabetList);
    if (!resultAlphabet->isRaw() || !recheckAlphabetFromDataIfRaw) {
        return resultAlphabet;
    }
    // now perform slow check with raw data access.
    QSet<const DNAAlphabet*> availableAlphabets = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets().toSet();
    foreach (const U2SequenceObject* sequence, sequenceList) {
        QList<const DNAAlphabet*> sequenceAlphabets = U2AlphabetUtils::findAllAlphabets(sequence->getWholeSequence(os).constData());
        availableAlphabets.intersect(sequenceAlphabets.toSet());
    }
    return selectBestAlphabetForAlignment(availableAlphabets.toList());
}

const DNAAlphabet* MSAUtils::deriveCommonAlphabet(const QList<const DNAAlphabet*>& alphabetList) {
    const DNAAlphabet* result = nullptr;
    foreach (const DNAAlphabet* alphabet, alphabetList) {
        result = result == nullptr ? alphabet : U2AlphabetUtils::deriveCommonAlphabet(result, alphabet);
    }
    return result == nullptr ? AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::RAW()) : result;
}

QList<DNASequence> MSAUtils::convertMsaToSequenceList(const MultipleSequenceAlignment& msa,
                                                      U2OpStatus& os,
                                                      bool trimGaps,
                                                      const QSet<qint64>& rowIdFilter,
                                                      const U2Region& columnRegion) {
    QBitArray gapCharMap = TextUtils::createBitMap(U2Msa::GAP_CHAR);
    int msaLength = msa->getLength();
    CHECK_EXT(U2Region(0, msaLength).contains(columnRegion), os.setError(tr("Invalid column region")), {});

    const DNAAlphabet* msaAlphabet = msa->getAlphabet();
    QList<DNASequence> sequenceList;
    const QList<MultipleSequenceAlignmentRow>& rows = msa->getMsaRows();
    for (const MultipleSequenceAlignmentRow& row : qAsConst(rows)) {
        if (!rowIdFilter.isEmpty() && !rowIdFilter.contains(row->getRowId())) {
            continue;
        }
        DNASequence sequence(row->getName(), row->toByteArray(os, msaLength), msaAlphabet);
        CHECK_OP(os, {});
        if (!columnRegion.isEmpty()) {
            sequence.seq = sequence.seq.mid(columnRegion.startPos, columnRegion.length);
        }
        if (trimGaps) {
            int newLen = TextUtils::remove(sequence.seq.data(), sequence.seq.length(), gapCharMap);
            sequence.seq.resize(newLen);
        }
        sequenceList << sequence;
    }
    return sequenceList;
}

bool MSAUtils::checkPackedModelSymmetry(const MultipleSequenceAlignment& ali, U2OpStatus& ti) {
    if (ali->getLength() == 0) {
        ti.setError(tr("Alignment is empty!"));
        return false;
    }
    int coreLen = ali->getLength();
    if (coreLen == 0) {
        ti.setError(tr("Alignment is empty!"));
        return false;
    }
    for (int i = 0, n = ali->getRowCount(); i < n; i++) {
        int rowCoreLength = ali->getMsaRow(i)->getCoreLength();
        if (rowCoreLength > coreLen) {
            ti.setError(tr("Sequences in alignment have different sizes!"));
            return false;
        }
    }
    return true;
}

int MSAUtils::getRowIndexByName(const MultipleSequenceAlignment& ma, const QString& name) {
    int idx = 0;

    foreach (const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        if (row->getName() == name) {
            return idx;
        }
        ++idx;
    }

    return -1;
}

namespace {

bool listContainsSeqObject(const QList<GObject*>& objs, int& firstSeqObjPos) {
    int objectNumber = 0;
    foreach (GObject* o, objs) {
        if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
            firstSeqObjPos = objectNumber;
            return true;
        }
        objectNumber++;
    }
    return false;
}

QList<U2Sequence> getDbSequences(const QList<GObject*>& objects) {
    Document* parentDoc = nullptr;
    QList<U2Sequence> sequencesInDb;
    foreach (GObject* o, objects) {
        if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
            if ((parentDoc = o->getDocument()) != nullptr) {
                parentDoc->removeObject(o, DocumentObjectRemovalMode_Release);
            }
            QScopedPointer<U2SequenceObject> seqObj(qobject_cast<U2SequenceObject*>(o));
            SAFE_POINT(!seqObj.isNull(), "Unexpected object type", QList<U2Sequence>());
            sequencesInDb.append(U2SequenceUtils::getSequenceDbInfo(seqObj.data()));
        }
    }
    return sequencesInDb;
}

}  // namespace

MultipleSequenceAlignmentObject* MSAUtils::seqObjs2msaObj(const QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os, bool shallowCopy, bool recheckAlphabetFromDataIfRaw) {
    CHECK(!objects.isEmpty(), nullptr);

    int firstSeqObjPos = -1;
    CHECK(listContainsSeqObject(objects, firstSeqObjPos), nullptr);
    SAFE_POINT_EXT(-1 != firstSeqObjPos, os.setError("Sequence object not found"), nullptr);

    const U2DbiRef dbiRef = objects.at(firstSeqObjPos)->getEntityRef().dbiRef;  // make a copy instead of referencing since objects will be deleted

    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, nullptr);

    const bool useGenbankHeader = hints.value(ObjectConvertion_UseGenbankHeader, false).toBool();
    MultipleSequenceAlignment ma = seq2ma(objects, os, useGenbankHeader, recheckAlphabetFromDataIfRaw);
    CHECK_OP(os, nullptr);
    CHECK(!ma->isEmpty(), nullptr);

    const QList<U2Sequence> sequencesInDB = shallowCopy ? getDbSequences(objects) : QList<U2Sequence>();

    const QString dstFolder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    return MultipleSequenceAlignmentImporter::createAlignment(dbiRef, dstFolder, ma, os, sequencesInDB);
}

MultipleSequenceAlignmentObject* MSAUtils::seqDocs2msaObj(QList<Document*> docs, const QVariantMap& hints, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw) {
    CHECK(!docs.isEmpty(), nullptr);
    QList<GObject*> objects;
    foreach (Document* doc, docs) {
        objects << doc->getObjects();
    }
    return seqObjs2msaObj(objects, hints, os, recheckAlphabetFromDataIfRaw);
}

void MSAUtils::assignOriginalDataIds(const MultipleSequenceAlignment& origMsa,
                                     MultipleSequenceAlignment& newMsa,
                                     QList<int>& removedRowIndexes,
                                     QList<int>& addedRowIndexes) {
    QList<MultipleSequenceAlignmentRow> origMsaRows = origMsa->getMsaRows();
    QSet<qint64> remappedRowIds;
    for (int newRowIndex = 0; newRowIndex < newMsa->getRowCount(); newRowIndex++) {
        MultipleSequenceAlignmentRow newMsaRow = newMsa->getMsaRow(newRowIndex);
        QString newRowNameForCompare = newMsaRow->getName().replace(" ", "_");
        bool isNewRowRemapped = false;
        for (int origRowIndex = 0; origRowIndex < origMsaRows.size(); origRowIndex++) {
            const MultipleSequenceAlignmentRow& origMsaRow = origMsaRows[origRowIndex];
            QString origRowNameForCompare = origMsaRow->getName().replace(" ", "_");
            if (newRowNameForCompare == origRowNameForCompare && origMsaRow->getSequence().seq == newMsaRow->getSequence().seq) {
                isNewRowRemapped = true;
                qint64 rowId = origMsaRow->getRowDbInfo().rowId;
                newMsa->setRowId(newRowIndex, rowId);
                remappedRowIds.insert(rowId);

                U2DataId sequenceId = origMsaRow->getRowDbInfo().sequenceId;
                newMsa->setSequenceId(newRowIndex, sequenceId);
                break;
            }
        }
        if (!isNewRowRemapped) {
            addedRowIndexes << newRowIndex;
        }
    }
    for (int origRowIndex = 0, origRowCount = origMsaRows.size(); origRowIndex < origRowCount; origRowIndex++) {
        qint64 origRowId = origMsaRows[origRowIndex]->getRowId();
        if (!remappedRowIds.contains(origRowId)) {
            removedRowIndexes << origRowIndex;
        }
    }
}

void MSAUtils::assignOriginalDataIds(const MultipleSequenceAlignment& origMsa,
                                     MultipleSequenceAlignment& newMsa,
                                     U2OpStatus& os) {
    QList<int> removed;
    QList<int> added;
    assignOriginalDataIds(origMsa, newMsa, removed, added);
    if (!added.isEmpty() || !removed.isEmpty()) {
        os.setError(tr("Failed to map result MSA rows into original MSA rows. Removed: %1, added: %2").arg(removed.size()).arg(added.size()));
    }
}

U2MsaRow MSAUtils::copyRowFromSequence(U2SequenceObject* seqObj, const U2DbiRef& dstDbi, U2OpStatus& os) {
    U2MsaRow row;
    CHECK_EXT(seqObj != nullptr, os.setError("NULL sequence object"), row);

    DNASequence dnaSeq = seqObj->getWholeSequence(os);
    CHECK_OP(os, row);

    return copyRowFromSequence(dnaSeq, dstDbi, os);
}

U2MsaRow MSAUtils::copyRowFromSequence(DNASequence dnaSeq, const U2DbiRef& dstDbi, U2OpStatus& os) {
    U2MsaRow row;
    row.rowId = -1;  // set the ID automatically

    QByteArray oldSeqData = dnaSeq.seq;
    int tailGapsIndex = oldSeqData.length() - 1;
    for (; tailGapsIndex >= 0; tailGapsIndex--) {
        if (U2Msa::GAP_CHAR != oldSeqData[tailGapsIndex]) {
            tailGapsIndex++;
            break;
        }
    }

    if (tailGapsIndex < oldSeqData.length()) {
        oldSeqData.chop(oldSeqData.length() - tailGapsIndex);
    }

    dnaSeq.seq.clear();
    MaDbiUtils::splitBytesToCharsAndGaps(oldSeqData, dnaSeq.seq, row.gaps);
    U2Sequence seq = U2SequenceUtils::copySequence(dnaSeq, dstDbi, U2ObjectDbi::ROOT_FOLDER, os);
    CHECK_OP(os, row);

    row.sequenceId = seq.id;
    row.gstart = 0;
    row.gend = seq.length;
    row.length = MsaRowUtils::getRowLengthWithoutTrailing(dnaSeq.seq, row.gaps);
    MsaRowUtils::chopGapModel(row.gaps, row.length);
    return row;
}

void MSAUtils::copyRowFromSequence(MultipleSequenceAlignmentObject* msaObj, U2SequenceObject* seqObj, U2OpStatus& os) {
    CHECK_EXT(msaObj != nullptr, os.setError("NULL msa object"), );

    U2MsaRow row = copyRowFromSequence(seqObj, msaObj->getEntityRef().dbiRef, os);
    CHECK_OP(os, );

    U2EntityRef entityRef = msaObj->getEntityRef();
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    CHECK_EXT(con.dbi != nullptr, os.setError("NULL root dbi"), );

    con.dbi->getMsaDbi()->addRow(entityRef.entityId, -1, row, os);
}

MultipleSequenceAlignment MSAUtils::createCopyWithIndexedRowNames(const MultipleSequenceAlignment& ma, const QString& prefix) {
    MultipleSequenceAlignment res = ma->getExplicitCopy();
    int rowNumber = res->getRowCount();
    for (int i = 0; i < rowNumber; i++) {
        res->renameRow(i, prefix + QString::number(i));
    }
    return res;
}

bool MSAUtils::restoreOriginalRowNamesFromIndexedNames(MultipleSequenceAlignment& ma, const QStringList& names, const QString& prefix) {
    int rowCount = ma->getRowCount();
    CHECK(rowCount == names.size() || !prefix.isEmpty(), false);

    QStringList resultNames;
    QStringList indexedNames = ma->getRowNames();
    for (QString indexedName : qAsConst(indexedNames)) {
        if (!prefix.isEmpty()) {
            if (indexedName.startsWith(prefix)) {
                indexedName = indexedName.mid(prefix.length());
            } else {
                // Do not remap the name. Use it as it is.
                resultNames << indexedName;
                continue;
            }
        }
        bool ok = false;
        int idx = indexedName.toInt(&ok);
        CHECK(ok && idx >= 0 && idx < rowCount, false);
        resultNames << names[idx];
    }

    for (int i = 0; i < resultNames.size(); i++) {
        ma->renameRow(i, resultNames[i]);
    }
    return true;
}

bool MSAUtils::restoreOriginalRowProperties(MultipleSequenceAlignment& resultMa, const MultipleSequenceAlignment& originalMa, const QString& prefix) {
    int rowCount = resultMa->getRowCount();
    CHECK(rowCount == originalMa->getRowCount() || !prefix.isEmpty(), false);

    for (int resultMaIndex = 0; resultMaIndex < rowCount; resultMaIndex++) {
        MultipleSequenceAlignmentRow resultRow = resultMa->getMsaRow(resultMaIndex);
        QString indexedName = resultRow->getName();
        if (!prefix.isEmpty()) {
            if (!indexedName.startsWith(prefix)) {
                continue;  // Do not remap the name. Use it as it is.
            }
            indexedName = indexedName.mid(prefix.length());
        }
        bool ok = false;
        int originalRowIndex = indexedName.toInt(&ok);
        CHECK(ok && originalRowIndex >= 0 && originalRowIndex < rowCount, false);
        MultipleSequenceAlignmentRow originalRow = originalMa->getMsaRow(originalRowIndex);
        U2MsaRow originalRowInfo = originalRow->getRowDbInfo();
        resultMa->setRowId(resultMaIndex, originalRowInfo.rowId);
        resultMa->setSequenceId(resultMaIndex, originalRowInfo.sequenceId);
        resultMa->renameRow(resultMaIndex, originalRow->getName());
    }
    return true;
}

QList<U2Region> MSAUtils::getColumnsWithGaps(const QList<QVector<U2MsaGap>>& maGapModel, const QList<MultipleAlignmentRow>& rows, int alignmentLength, int requiredGapsCount) {
    const int rowsCount = rows.size();
    if (requiredGapsCount == -1) {
        requiredGapsCount = rowsCount;
    }

    QList<U2Region> regionsToDelete;
    for (int columnNumber = 0; columnNumber < alignmentLength; columnNumber++) {
        int gapCount = 0;
        for (int j = 0; j < maGapModel.size(); j++) {
            int ungappedLength = (j == rows.size()) ? alignmentLength : rows[j]->getUngappedLength();
            if (MsaRowUtils::isGap(ungappedLength, maGapModel[j], columnNumber)) {
                gapCount++;
            }
            if (gapCount == requiredGapsCount) {
                break;
            }
        }

        if (gapCount == requiredGapsCount) {
            if (!regionsToDelete.isEmpty() && regionsToDelete.last().endPos() == static_cast<qint64>(columnNumber)) {
                regionsToDelete.last().length++;
            } else {
                regionsToDelete << U2Region(columnNumber, 1);
            }
        }
    }

    return regionsToDelete;
}

void MSAUtils::removeColumnsWithGaps(MultipleSequenceAlignment& msa, int requiredGapsCount) {
    GTIMER(c, t, "MSAUtils::removeColumnsWithGaps");
    const QList<U2Region> regionsToDelete = getColumnsWithGaps(msa->getGapModel(), msa->getRows(), msa->getLength(), requiredGapsCount);
    for (int i = regionsToDelete.size() - 1; i >= 0; i--) {
        msa->removeRegion(regionsToDelete[i].startPos, 0, regionsToDelete[i].length, msa->getRowCount(), true);
    }
}

QString MSAUtils::rollMsaRowName(const QString& rowName, const QSet<QString>& usedRowNamesSet, const QString& suffixSeparator) {
    QString result = rowName;
    int counter = 1;
    while (usedRowNamesSet.contains(result)) {
        result = rowName + suffixSeparator + QString::number(counter);
        counter++;
    }
    return result;
}

void MSAUtils::addRowsToMsa(U2EntityRef& msaObjectRef, QList<MultipleSequenceAlignmentRow>& rows, U2OpStatus& os) {
    DbiConnection con(msaObjectRef.dbiRef, os);
    SAFE_POINT_OP(os, );

    U2MsaDbi* msaDbi = con.dbi->getMsaDbi();
    SAFE_POINT_OP(os, );

    QList<U2MsaRow> msaRows;
    for (MultipleSequenceAlignmentRow& row : rows) {
        U2MsaRow msaRow = copyRowFromSequence(row->getSequence(), msaObjectRef.dbiRef, os);
        SAFE_POINT_OP(os, );
        msaDbi->addRow(msaObjectRef.entityId, -1, msaRow, os);
        SAFE_POINT_OP(os, );
        msaDbi->updateGapModel(msaObjectRef.entityId, msaRow.rowId, row->getGaps(), os);
        SAFE_POINT_OP(os, );

        row->setRowId(msaRow.rowId);
        row->setSequenceId(msaRow.sequenceId);
    }
}

}  // namespace U2
