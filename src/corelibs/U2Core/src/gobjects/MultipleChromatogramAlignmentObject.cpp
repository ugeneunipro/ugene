/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "MultipleChromatogramAlignmentObject.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DbiConnection.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/McaDbiUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignmentExporter.h>
#include <U2Core/MultipleChromatogramAlignmentImporter.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

namespace U2 {

const QString MultipleChromatogramAlignmentObject::MCAOBJECT_REFERENCE = "MCAOBJECT_REFERENCE";

MultipleChromatogramAlignmentObject::MultipleChromatogramAlignmentObject(const QString& name,
                                                                         const U2EntityRef& mcaRef,
                                                                         const QVariantMap& hintsMap,
                                                                         const MultipleChromatogramAlignment& mca)
    : MultipleAlignmentObject(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT, name, mcaRef, hintsMap, mca), referenceObj(nullptr) {
}

MultipleChromatogramAlignmentObject::~MultipleChromatogramAlignmentObject() {
    delete referenceObj;
}

GObject* MultipleChromatogramAlignmentObject::clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints) const {
    DbiOperationsBlock opBlock(dstDbiRef, os);
    CHECK_OP(os, nullptr);

    QScopedPointer<GHintsDefaultImpl> gHints(new GHintsDefaultImpl(getGHintsMap()));
    gHints->setAll(hints);
    const QString dstFolder = gHints->get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    MultipleChromatogramAlignment mca = getMcaCopy();
    MultipleChromatogramAlignmentObject* clonedObject = MultipleChromatogramAlignmentImporter::createAlignment(os, dstDbiRef, dstFolder, mca);
    CHECK_OP(os, nullptr);

    QScopedPointer<MultipleChromatogramAlignmentObject> p(clonedObject);

    DbiConnection srcCon(getEntityRef().dbiRef, os);
    CHECK_OP(os, nullptr);

    DbiConnection dstCon(dstDbiRef, os);
    CHECK_OP(os, nullptr);

    U2Sequence referenceCopy = U2SequenceUtils::copySequence(getReferenceObj()->getEntityRef(), dstDbiRef, dstFolder, os);
    CHECK_OP(os, nullptr);

    U2ByteArrayAttribute attribute;
    U2Object obj;
    obj.dbiId = dstDbiRef.dbiId;
    obj.id = clonedObject->getEntityRef().entityId;
    obj.version = clonedObject->getModificationVersion();
    U2AttributeUtils::init(attribute, obj, MultipleChromatogramAlignmentObject::MCAOBJECT_REFERENCE);
    attribute.value = referenceCopy.id;
    dstCon.dbi->getAttributeDbi()->createByteArrayAttribute(attribute, os);
    CHECK_OP(os, nullptr);

    clonedObject->setGHints(gHints.take());
    clonedObject->setIndexInfo(getIndexInfo());
    return p.take();
}

U2SequenceObject* MultipleChromatogramAlignmentObject::getReferenceObj() const {
    if (referenceObj == nullptr) {
        U2OpStatus2Log status;
        DbiConnection con(getEntityRef().dbiRef, status);
        CHECK_OP(status, nullptr);

        U2ByteArrayAttribute attribute = U2AttributeUtils::findByteArrayAttribute(con.dbi->getAttributeDbi(), getEntityRef().entityId, MCAOBJECT_REFERENCE, status);
        CHECK_OP(status, nullptr);

        GObject* obj = GObjectUtils::createObject(con.dbi->getDbiRef(), attribute.value, "reference object");

        referenceObj = qobject_cast<U2SequenceObject*>(obj);
        connect(this, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), referenceObj, SLOT(sl_resetDataCaches()));
        connect(this, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), referenceObj, SIGNAL(si_sequenceChanged()));
    }

    return referenceObj;
}

char MultipleChromatogramAlignmentObject::charAt(int seqNum, qint64 position) const {
    SAFE_POINT(seqNum >= 0 && seqNum < getRowCount(), QString("Invalid sequence num: %1").arg(seqNum), U2Msa::GAP_CHAR);
    SAFE_POINT(position >= 0 && position < getLength(), QString("Invalid position: %1").arg(position), U2Msa::GAP_CHAR);
    return getMcaRow(seqNum)->charAt(position);
}

const MultipleChromatogramAlignment MultipleChromatogramAlignmentObject::getMca() const {
    return getMultipleAlignment().dynamicCast<MultipleChromatogramAlignment>();
}

const MultipleChromatogramAlignment MultipleChromatogramAlignmentObject::getMcaCopy() const {
    return getMca()->getCopy();
}

const MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentObject::getMcaRow(int row) const {
    return getRow(row).dynamicCast<MultipleChromatogramAlignmentRow>();
}

void MultipleChromatogramAlignmentObject::insertGap(const U2Region& rows, int pos, int nGaps) {
    MultipleAlignmentObject::insertGap(rows, pos, nGaps, true);
}

void MultipleChromatogramAlignmentObject::insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps) {
    MultipleAlignmentObject::insertGapByRowIndexList(rowIndexes, pos, nGaps, true);
}

QList<U2Region> MultipleChromatogramAlignmentObject::getColumnsWithGaps() const {
    QList<QVector<U2MsaGap>> gapModel = getGapModel();
    gapModel.append(getReferenceGapModel());
    return MSAUtils::getColumnsWithGaps(gapModel, getRows(), getLength(), gapModel.size());
}

QVector<U2MsaGap> MultipleChromatogramAlignmentObject::getReferenceGapModel() const {
    QByteArray unusedSequence;
    QVector<U2MsaGap> referenceGapModel;
    MaDbiUtils::splitBytesToCharsAndGaps(getReferenceObj()->getSequenceData(U2_REGION_MAX), unusedSequence, referenceGapModel);
    return referenceGapModel;
}

void MultipleChromatogramAlignmentObject::insertCharacter(int rowIndex, int pos, char newChar) {
    SAFE_POINT(!isStateLocked(), "Alignment state is locked", );
    insertGap(U2Region(0, getRowCount()), pos, 1);
    replaceCharacter(pos, rowIndex, newChar);
}

void MultipleChromatogramAlignmentObject::deleteColumnsWithGaps(U2OpStatus& os) {
    const QList<U2Region> regionsToDelete = getColumnsWithGaps();
    CHECK(!regionsToDelete.isEmpty(), );
    CHECK(regionsToDelete.first().length != getLength(), );

    for (int n = regionsToDelete.size(), i = n - 1; i >= 0; i--) {
        removeRegion(regionsToDelete[i].startPos, 0, regionsToDelete[i].length, getRowCount(), true, false);
        getReferenceObj()->replaceRegion(getEntityRef().entityId, regionsToDelete[i], DNASequence(), os);
        os.setProgress(100 * (n - i) / n);
    }

    const int length = getLength();
    int coloumnsRemoved = 0;
    for (int i = 0; i < regionsToDelete.size(); i++) {
        coloumnsRemoved += regionsToDelete[i].length;
    }
    const int newLength = length - coloumnsRemoved;
    changeLength(os, newLength);
    CHECK_OP(os, );

    updateCachedMultipleAlignment();
}

void MultipleChromatogramAlignmentObject::trimRow(const int rowIndex, int currentPos, U2OpStatus& os, TrimEdge edge) {
    U2EntityRef entityRef = getEntityRef();
    MultipleAlignmentRow row = getRow(rowIndex);
    int rowId = row->getRowId();
    int pos = 0;
    int count = 0;
    switch (edge) {
        case Left:
            pos = row->getCoreStart();
            count = currentPos - pos;
            break;
        case Right:
            pos = currentPos + 1;
            int lengthWithoutTrailing = row->getRowLengthWithoutTrailing();
            count = lengthWithoutTrailing - currentPos;
            break;
    }
    McaDbiUtils::removeRegion(entityRef, rowId, pos, count, os);
    U2Region region(rowIndex, 1);
    if (edge == Left) {
        insertGap(region, 0, count);
    }

    MaModificationInfo modificationInfo;
    modificationInfo.rowContentChanged = true;
    modificationInfo.rowListChanged = false;
    updateCachedMultipleAlignment(modificationInfo);
}

void MultipleChromatogramAlignmentObject::updateAlternativeMutations(bool showAlternativeMutations, int threshold, U2OpStatus& os) {
    for (int i = 0; i < getRowCount(); i++) {
        const MultipleChromatogramAlignmentRow& mcaRow = static_cast<const MultipleChromatogramAlignmentRow&>(getRow(i));
        qint64 ungappedLength = mcaRow->getUngappedLength();

        QHash<qint64, char> newCharList;
        for (int j = 0; j < ungappedLength; j++) {
            bool ok = false;
            auto res = mcaRow->getTwoHighestPeaks(j, ok);
            if (!ok) {
                continue;
            }

            double minimumThresholdValue = (double)res.second.value / res.first.value * 100;
            char newChar;
            if (minimumThresholdValue < threshold || !showAlternativeMutations) {
                newChar = DNAChromatogram::TRACE_CHARACTER[res.first.trace];
            } else {
                newChar = DNAChromatogram::TRACE_CHARACTER[res.second.trace];
            }

            auto gappedPos = mcaRow->getGappedPosition(j);
            char currentChar = mcaRow->charAt(gappedPos);
            if (currentChar == newChar) {
                continue;
            }

            newCharList.insert(gappedPos, newChar);
        }

        const MultipleAlignment& ma = getMultipleAlignment();
        qint64 modifiedRowId = ma->getRow(i)->getRowId();
        McaDbiUtils::replaceCharactersInRow(getEntityRef(), modifiedRowId, newCharList, os);
        SAFE_POINT_OP(os, );
    }
    updateCachedMultipleAlignment();
}

void MultipleChromatogramAlignmentObject::loadAlignment(U2OpStatus& os) {
    MultipleChromatogramAlignmentExporter mcaExporter;
    cachedMa = mcaExporter.getAlignment(os, entityRef.dbiRef, entityRef.entityId);
}

void MultipleChromatogramAlignmentObject::updateCachedRows(U2OpStatus& os, const QList<qint64>& rowIds) {
    MultipleChromatogramAlignment cachedMca = cachedMa.dynamicCast<MultipleChromatogramAlignment>();

    MultipleChromatogramAlignmentExporter mcaExporter;
    QMap<qint64, McaRowMemoryData> mcaRowsMemoryData = mcaExporter.getMcaRowMemoryData(os, entityRef.dbiRef, entityRef.entityId, rowIds);
    SAFE_POINT_OP(os, );
    foreach (const qint64 rowId, mcaRowsMemoryData.keys()) {
        const int rowIndex = cachedMca->getRowIndexByRowId(rowId, os);
        SAFE_POINT_OP(os, );
        cachedMca->setRowContent(rowIndex, mcaRowsMemoryData[rowId]);
        SAFE_POINT_OP(os, );
        cachedMca->renameRow(rowIndex, mcaRowsMemoryData[rowId].sequence.getName());
    }
}

void MultipleChromatogramAlignmentObject::updateDatabase(U2OpStatus& os, const MultipleAlignment& ma) {
    const MultipleChromatogramAlignment mca = ma.dynamicCast<MultipleChromatogramAlignment>();
    McaDbiUtils::updateMca(os, entityRef, mca);
}

void MultipleChromatogramAlignmentObject::removeRowPrivate(U2OpStatus& os, const U2EntityRef& mcaRef, qint64 rowId) {
    McaDbiUtils::removeRow(mcaRef, rowId, os);
}

void MultipleChromatogramAlignmentObject::removeRegionPrivate(U2OpStatus& os, const U2EntityRef& maRef, const QList<qint64>& rows, int startPos, int nBases) {
    McaDbiUtils::removeCharacters(maRef, rows, startPos, nBases, os);
}

int MultipleChromatogramAlignmentObject::getReferenceLengthWithGaps() const {
    int lengthWithoutGaps = getLength();

    QVector<U2MsaGap> refGapModel = getReferenceGapModel();
    int gapLength = 0;
    foreach (const U2MsaGap gap, refGapModel) {
        gapLength += gap.length;
    }

    return lengthWithoutGaps + gapLength;
}

}  // namespace U2
