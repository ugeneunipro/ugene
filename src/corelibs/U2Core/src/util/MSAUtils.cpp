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

#include "MSAUtils.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>

#include <QListIterator>
#include <U2Core/AppContext.h>

namespace U2 {

bool MSAUtils::equalsIgnoreGaps(const MultipleAlignmentRow& row, int startPos, const QByteArray& pat, int &alternateLen) {
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

int MSAUtils::getPatternSimilarityIgnoreGaps(const MultipleSequenceAlignmentRow& row, int startPos, const QByteArray& pat, int &alternateLen) {
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
    foreach(const DNASequence& seq, list) {
        //TODO: handle memory overflow
        ma->addRow(seq.getName(), seq.seq);
    }
    CHECK_OP(os, MultipleSequenceAlignment());
    return ma;
}

namespace {

MultipleSequenceAlignmentObject * prepareSequenceHeadersList(const QList<GObject *> &list, bool useGenbankHeader, QList<U2SequenceObject *> &dnaList,
    QList<QString> &nameList) {
    foreach(GObject *obj, list) {
        U2SequenceObject *dnaObj = qobject_cast<U2SequenceObject *>(obj);
        if (dnaObj == NULL) {
            if (MultipleSequenceAlignmentObject *maObj = qobject_cast<MultipleSequenceAlignmentObject *>(obj)) {
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
    return NULL;
}


void appendSequenceToAlignmentRow(MultipleSequenceAlignment &ma, int rowIndex, int afterPos, const U2SequenceObject &seq, U2OpStatus &os) {
    U2Region seqRegion(0, seq.getSequenceLength());
    const qint64 blockReadFromBD = 4194305; // 4 MB + 1

    qint64 sequenceLength = seq.getSequenceLength();
    for (qint64 startPosition = seqRegion.startPos; startPosition < seqRegion.length; startPosition += blockReadFromBD) {
        U2Region readRegion(startPosition, qMin(blockReadFromBD, sequenceLength - startPosition));
        QByteArray readedData = seq.getSequenceData(readRegion);
        ma->appendChars(rowIndex, afterPos, readedData.constData(), readedData.size());
        afterPos += readRegion.length;
        CHECK_OP(os, );
    }
}

} // unnamed namespace

MultipleSequenceAlignment MSAUtils::seq2ma(const QList<GObject *> &list, U2OpStatus &os, bool useGenbankHeader, bool recheckAlphabetFromDataIfRaw) {
    QList<U2SequenceObject *> dnaList;
    QStringList nameList;

    MultipleSequenceAlignmentObject *obj = prepareSequenceHeadersList(list, useGenbankHeader, dnaList, nameList);
    if (NULL != obj) {
        return obj->getMsaCopy();
    }

    MultipleSequenceAlignment ma(MA_OBJECT_NAME);
    ma->setAlphabet(deriveCommonAlphabet(dnaList, recheckAlphabetFromDataIfRaw, os));

    int i = 0;
    SAFE_POINT(dnaList.size() == nameList.size(), "DNA list size differs from name list size", MultipleSequenceAlignment());
    QListIterator<U2SequenceObject *> listIterator(dnaList);
    QListIterator<QString> nameIterator(nameList);
    while (listIterator.hasNext()) {
        const U2SequenceObject &seq = *(listIterator.next());
        const QString &objName = nameIterator.next();

        CHECK_OP(os, MultipleSequenceAlignment());

        ma->addRow(objName, QByteArray(""));

        SAFE_POINT(i < ma->getNumRows(), "Row count differ from expected after adding row", MultipleSequenceAlignment());
        appendSequenceToAlignmentRow(ma, i, 0, seq, os);
        CHECK_OP(os, MultipleSequenceAlignment());
        i++;
    }

    return ma;
}

static const DNAAlphabet* selectBestAlphabetForAlignment(const QList<const DNAAlphabet*>& availableAlphabets) {
    const DNAAlphabet* bestMatch = NULL;
    foreach (const DNAAlphabet* alphabet, availableAlphabets) {
        if (bestMatch == NULL || bestMatch->isRaw()) { // prefer any other alphabet over RAW.
            bestMatch = alphabet;
            continue;
        }
        if (bestMatch->isDNA() && alphabet->isAmino()) { // prefer Amino over DNA.
            bestMatch = alphabet;
            continue;
        }
        if (bestMatch->isExtended() && !alphabet->isExtended()) { // narrow down the set of characters.
            bestMatch = alphabet;
        }
    }
    return bestMatch;
}

const DNAAlphabet* MSAUtils::deriveCommonAlphabet(const QList<DNASequence>& sequenceList, bool recheckAlphabetFromDataIfRaw) {
    // first perform fast check using sequence alphabets only.
    QList<const DNAAlphabet*> alphabetList;
    foreach(const DNASequence& sequence, sequenceList) {
        alphabetList.append(sequence.alphabet);
    }
    const DNAAlphabet* resultAlphabet = deriveCommonAlphabet(alphabetList);
    if (!resultAlphabet->isRaw() || !recheckAlphabetFromDataIfRaw) {
        return resultAlphabet;
    }
    // now perform slow check with raw data access.
    QSet<const DNAAlphabet*> availableAlphabets = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets().toSet();
    foreach(const DNASequence& sequence, sequenceList) {
        QList<const DNAAlphabet*> sequenceAlphabets = U2AlphabetUtils::findAllAlphabets(sequence.constData());
        availableAlphabets.intersect(sequenceAlphabets.toSet());
    }
    return selectBestAlphabetForAlignment(availableAlphabets.toList());
}

const DNAAlphabet* MSAUtils::deriveCommonAlphabet(const QList<U2SequenceObject*>& sequenceList, bool recheckAlphabetFromDataIfRaw, U2OpStatus& os) {
    // first perform fast check using sequence alphabets only.
    QList<const DNAAlphabet*> alphabetList;
    foreach(const U2SequenceObject* sequenceObject, sequenceList) {
        alphabetList.append(sequenceObject->getAlphabet());
    }
    const DNAAlphabet* resultAlphabet = deriveCommonAlphabet(alphabetList);
    if (!resultAlphabet->isRaw() || !recheckAlphabetFromDataIfRaw) {
        return resultAlphabet;
    }
    // now perform slow check with raw data access.
    QSet<const DNAAlphabet*> availableAlphabets = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets().toSet();
    foreach(const U2SequenceObject* sequence, sequenceList) {
        QList<const DNAAlphabet*> sequenceAlphabets = U2AlphabetUtils::findAllAlphabets(sequence->getWholeSequence(os).constData());
        availableAlphabets.intersect(sequenceAlphabets.toSet());
    }
    return selectBestAlphabetForAlignment(availableAlphabets.toList());
}

const DNAAlphabet* MSAUtils::deriveCommonAlphabet(const QList<const DNAAlphabet*>& alphabetList) {
    const DNAAlphabet* result = NULL;
    foreach(const DNAAlphabet* alphabet, alphabetList) {
        result = result == NULL ? alphabet : U2AlphabetUtils::deriveCommonAlphabet(result, alphabet);
    }
    return result == NULL ? AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::RAW()) : result;
}

QList<DNASequence> MSAUtils::ma2seq(const MultipleSequenceAlignment& ma, bool trimGaps) {
    QList<DNASequence> lst;
    QBitArray gapCharMap = TextUtils::createBitMap(U2Msa::GAP_CHAR);
    int len = ma->getLength();
    const DNAAlphabet* al = ma->getAlphabet();
    U2OpStatus2Log os;
    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        DNASequence s(row->getName(), row->toByteArray(os, len), al);
        if (trimGaps) {
            int newLen = TextUtils::remove(s.seq.data(), s.length(), gapCharMap);
            s.seq.resize(newLen);
        }
        lst << s;
    }
    return lst;
}

QList<DNASequence> MSAUtils::ma2seq(const MultipleSequenceAlignment& ma, bool trimGaps, const QSet<qint64>& rowIds) {
    QBitArray gapCharMap = TextUtils::createBitMap(U2Msa::GAP_CHAR);
    int len = ma->getLength();
    const DNAAlphabet* al = ma->getAlphabet();
    U2OpStatus2Log os;
    QList<DNASequence> result;
    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        if (rowIds.contains(row->getRowId())) {
            DNASequence s(row->getName(), row->toByteArray(os, len), al);
            if (trimGaps) {
                int newLen = TextUtils::remove(s.seq.data(), s.length(), gapCharMap);
                s.seq.resize(newLen);
            }
            result << s;
        }
    }
    return result;
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
    for (int i = 0, n = ali->getNumRows(); i < n; i++) {
        int rowCoreLength = ali->getMsaRow(i)->getCoreLength();
        if (rowCoreLength > coreLen) {
            ti.setError(tr("Sequences in alignment have different sizes!"));
            return false;
        }
    }
    return true;
}

int MSAUtils::getRowIndexByName(const MultipleSequenceAlignment &ma, const QString &name) {
    int idx = 0;

    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        if (row->getName() == name) {
            return idx;
        }
        ++idx;
    }

    return -1;
}

namespace {

bool listContainsSeqObject(const QList<GObject *> &objs, int &firstSeqObjPos) {
    int objectNumber = 0;
    foreach(GObject *o, objs) {
        if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
            firstSeqObjPos = objectNumber;
            return true;
        }
        objectNumber++;
    }
    return false;
}

QList<U2Sequence> getDbSequences(const QList<GObject *> &objects) {
    Document *parentDoc = NULL;
    QList<U2Sequence> sequencesInDb;
    foreach(GObject *o, objects) {
        if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
            if (NULL != (parentDoc = o->getDocument())) {
                parentDoc->removeObject(o, DocumentObjectRemovalMode_Release);
            }
            QScopedPointer<U2SequenceObject> seqObj(qobject_cast<U2SequenceObject *>(o));
            SAFE_POINT(!seqObj.isNull(), "Unexpected object type", QList<U2Sequence>());
            sequencesInDb.append(U2SequenceUtils::getSequenceDbInfo(seqObj.data()));
        }
    }
    return sequencesInDb;
}

}

MultipleSequenceAlignmentObject * MSAUtils::seqObjs2msaObj(const QList<GObject *> &objects, const QVariantMap &hints, U2OpStatus &os, bool shallowCopy, bool recheckAlphabetFromDataIfRaw) {
    CHECK(!objects.isEmpty(), NULL);

    int firstSeqObjPos = -1;
    CHECK(listContainsSeqObject(objects, firstSeqObjPos), NULL);
    SAFE_POINT_EXT(-1 != firstSeqObjPos, os.setError("Sequence object not found"), NULL);

    const U2DbiRef dbiRef = objects.at(firstSeqObjPos)->getEntityRef().dbiRef; // make a copy instead of referencing since objects will be deleted

    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    const bool useGenbankHeader = hints.value(ObjectConvertion_UseGenbankHeader, false).toBool();
    MultipleSequenceAlignment ma = seq2ma(objects, os, useGenbankHeader, recheckAlphabetFromDataIfRaw);
    CHECK_OP(os, NULL);
    CHECK(!ma->isEmpty(), NULL);

    const QList<U2Sequence> sequencesInDB = shallowCopy ? getDbSequences(objects) : QList<U2Sequence>();

    const QString dstFolder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    return MultipleSequenceAlignmentImporter::createAlignment(dbiRef, dstFolder, ma, os, sequencesInDB);
}

MultipleSequenceAlignmentObject* MSAUtils::seqDocs2msaObj(QList<Document*> docs, const QVariantMap& hints, U2OpStatus& os, bool recheckAlphabetFromDataIfRaw) {
    CHECK(!docs.isEmpty(), NULL);
    QList<GObject*> objects;
    foreach(Document* doc, docs) {
        objects << doc->getObjects();
    }
    return seqObjs2msaObj(objects, hints, os, recheckAlphabetFromDataIfRaw);
}

QList<qint64> MSAUtils::compareRowsAfterAlignment(const MultipleSequenceAlignment& origMsa, MultipleSequenceAlignment& newMsa, U2OpStatus& os) {
    QList<qint64> rowsOrder;
    const QList<MultipleSequenceAlignmentRow> origMsaRows = origMsa->getMsaRows();
    for (int i = 0, n = newMsa->getNumRows(); i < n; ++i) {
        const MultipleSequenceAlignmentRow newMsaRow = newMsa->getMsaRow(i);
        QString rowName = newMsaRow->getName().replace(" ", "_");

        bool rowFound = false;
        foreach(const MultipleSequenceAlignmentRow &origMsaRow, origMsaRows) {
            if (origMsaRow->getName().replace(" ", "_") == rowName && origMsaRow->getSequence().seq == newMsaRow->getSequence().seq) {
                rowFound = true;
                qint64 rowId = origMsaRow->getRowDbInfo().rowId;
                newMsa->setRowId(i, rowId);
                rowsOrder.append(rowId);

                U2DataId sequenceId = origMsaRow->getRowDbInfo().sequenceId;
                newMsa->setSequenceId(i, sequenceId);

                break;
            }
        }

        if (!rowFound) {
            os.setError(tr("Can't find a row in an alignment!"));
            return QList<qint64>();
        }
    }
    return rowsOrder;
}

U2MsaRow MSAUtils::copyRowFromSequence(U2SequenceObject *seqObj, const U2DbiRef &dstDbi, U2OpStatus &os) {
    U2MsaRow row;
    CHECK_EXT(NULL != seqObj, os.setError("NULL sequence object"), row);

    DNASequence dnaSeq = seqObj->getWholeSequence(os);
    CHECK_OP(os, row);

    return copyRowFromSequence(dnaSeq, dstDbi, os);
}

U2MsaRow MSAUtils::copyRowFromSequence(DNASequence dnaSeq, const U2DbiRef &dstDbi, U2OpStatus &os) {
    U2MsaRow row;
    row.rowId = -1; // set the ID automatically

    QByteArray oldSeqData = dnaSeq.seq;
    int tailGapsIndex = 0;
    for (tailGapsIndex = oldSeqData.length() - 1; tailGapsIndex >= 0; tailGapsIndex--) {
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


void MSAUtils::copyRowFromSequence(MultipleSequenceAlignmentObject *msaObj, U2SequenceObject *seqObj, U2OpStatus &os) {
    CHECK_EXT(NULL != msaObj, os.setError("NULL msa object"), );

    U2MsaRow row = copyRowFromSequence(seqObj, msaObj->getEntityRef().dbiRef, os);
    CHECK_OP(os, );

    U2EntityRef entityRef = msaObj->getEntityRef();
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    CHECK_EXT(NULL != con.dbi, os.setError("NULL root dbi"), );

    con.dbi->getMsaDbi()->addRow(entityRef.entityId, -1, row, os);
}

MultipleSequenceAlignment MSAUtils::setUniqueRowNames(const MultipleSequenceAlignment &ma) {
    MultipleSequenceAlignment res = ma->getExplicitCopy();
    int rowNumber = res->getNumRows();
    for (int i = 0; i < rowNumber; i++) {
        res->renameRow(i, QString::number(i));
    }
    return res;
}

bool MSAUtils::restoreRowNames(MultipleSequenceAlignment &ma, const QStringList &names) {
    int rowNumber = ma->getNumRows();
    CHECK(rowNumber == names.size(), false);

    QStringList oldNames = ma->getRowNames();
    for (int i = 0; i < rowNumber; i++) {
        int idx = oldNames[i].toInt();
        CHECK(0 <= idx && idx <= rowNumber, false);
        ma->renameRow(i, names[idx]);
    }
    return true;
}

QList<U2Region> MSAUtils::getColumnsWithGaps(const U2MsaListGapModel &maGapModel, int length, int requiredGapsCount) {
    const int rowsCount = maGapModel.size();
    if (-1 == requiredGapsCount) {
        requiredGapsCount = rowsCount;
    }

    QList<U2Region> regionsToDelete;
    for (int columnNumber = 0; columnNumber < length; columnNumber++) {
        int gapCount = 0;
        for (int j = 0; j < rowsCount; j++) {
            if (MsaRowUtils::isGap(length, maGapModel[j], columnNumber)) {
                gapCount++;
            }
        }

        if (gapCount >= requiredGapsCount) {
            if (!regionsToDelete.isEmpty() && regionsToDelete.last().endPos() == static_cast<qint64>(columnNumber)) {
                regionsToDelete.last().length++;
            } else {
                regionsToDelete << U2Region(columnNumber, 1);
            }
        }
    }

    return regionsToDelete;
}

void MSAUtils::removeColumnsWithGaps(MultipleSequenceAlignment &msa, int requiredGapsCount) {
    GTIMER(c, t, "MSAUtils::removeColumnsWithGaps");
    const QList<U2Region> regionsToDelete = getColumnsWithGaps(msa->getGapModel(), msa->getLength(), requiredGapsCount);
    for (int i = regionsToDelete.size() - 1; i >= 0; i--) {
        msa->removeRegion(regionsToDelete[i].startPos, 0, regionsToDelete[i].length, msa->getNumRows(), true);
    }
}

}   // namespace U2
