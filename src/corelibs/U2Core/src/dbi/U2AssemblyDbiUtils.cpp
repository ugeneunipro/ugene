/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

qint64 U2AssemblyDbiUtils::calculateLeftShiftForReadsInRegion(U2AssemblyDbi* srcAssemblyDbi, U2DataId srcObjId, const U2Region& desiredRegion, U2OpStatus& os) {
    QScopedPointer<U2DbiIterator<U2AssemblyRead>> iter(srcAssemblyDbi->getReads(srcObjId, desiredRegion, os, true, true));
    CHECK_OP(os, 0);
    CHECK(iter->hasNext(), 0);

    // calculate shift for received reads
    qint64 shiftValue = std::numeric_limits<qint64>::max();
    while (iter->hasNext()) {
        U2AssemblyRead read = iter->next();
        CHECK_OP(os, 0);
        shiftValue = qMin(read->leftmostPos, shiftValue);
    }
    return shiftValue;
}

}
