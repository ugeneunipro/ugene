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

#include "MultipleSequenceAlignmentObject.h"

#include <U2Core/GHints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

MultipleSequenceAlignmentObject::MultipleSequenceAlignmentObject(const QString& name,
                                                                 const U2EntityRef& msaRef,
                                                                 const QVariantMap& hintsMap,
                                                                 const MultipleAlignment& alnData)
    : MultipleAlignmentObject(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, name, msaRef, hintsMap, alnData) {
}

MultipleSequenceAlignmentObject* MultipleSequenceAlignmentObject::clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints) const {
    DbiOperationsBlock opBlock(dstDbiRef, os);
    CHECK_OP(os, nullptr);

    QScopedPointer<GHintsDefaultImpl> gHints(new GHintsDefaultImpl(getGHintsMap()));
    gHints->setAll(hints);
    const QString dstFolder = gHints->get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    MultipleAlignment msa = getAlignment()->getCopy();
    MultipleSequenceAlignmentObject* clonedObj = MultipleSequenceAlignmentImporter::createAlignment(dstDbiRef, dstFolder, msa, os);
    CHECK_OP(os, nullptr);

    clonedObj->setGHints(gHints.take());
    clonedObj->setIndexInfo(getIndexInfo());
    return clonedObj;
}


void MultipleSequenceAlignmentObject::deleteColumnsWithGaps(U2OpStatus& os, int requiredGapsCount) {
    const QList<U2Region> regionsToDelete = MSAUtils::getColumnsWithGaps(getGapModel(), getRows(), getLength(), requiredGapsCount);
    CHECK(!regionsToDelete.isEmpty(), );
    CHECK(regionsToDelete.first().length != getLength(), );

    for (int n = regionsToDelete.size(), i = n - 1; i >= 0; i--) {
        removeRegion(regionsToDelete[i].startPos, 0, regionsToDelete[i].length, getRowCount(), true, false);
        os.setProgress(100 * (n - i) / n);
    }
    updateCachedMultipleAlignment();
}

}  // namespace U2
