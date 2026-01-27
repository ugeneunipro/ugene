/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "U2AssemblyDbiUtils.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

QPair<qint64, qint64> U2AssemblyDbiUtils::calculateLeftShiftAndLength(U2AssemblyDbi* srcAssemblyDbi, U2DataId srcObjId, 
                                                                      const U2Region& desiredRegion, U2OpStatus& os) {
    QScopedPointer<U2DbiIterator<U2AssemblyRead>> iter(srcAssemblyDbi->getReads(srcObjId, desiredRegion, os, true, true));
    const QPair<qint64, qint64> emptyResult(0, 0);
    CHECK_OP(os, emptyResult);
    CHECK(iter->hasNext(), emptyResult);

    qint64 shiftValue = std::numeric_limits<qint64>::max();
    qint64 maxLength = 0;
    while (iter->hasNext()) {
        const U2AssemblyRead read = iter->next();
        CHECK_OP(os, emptyResult);
        shiftValue = qMin(read->leftmostPos, shiftValue);
        maxLength = qMax(maxLength, read->leftmostPos + read->effectiveLen);
    }
    return QPair<qint64, qint64>(shiftValue, maxLength - shiftValue);
}

}
