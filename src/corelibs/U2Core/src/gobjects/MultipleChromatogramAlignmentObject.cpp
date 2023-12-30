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

#include "MultipleChromatogramAlignmentObject.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DbiConnection.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MsaExportUtils.h>
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
                                                                         const MultipleAlignment& mca)
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

    MultipleAlignment mca = getCopy();
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

void MultipleChromatogramAlignmentObject::deleteColumnsWithGaps(U2OpStatus& os) {
    QList<U2Region> regionsToDelete = getColumnsWithGaps();
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
