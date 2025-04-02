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

QList<U2AssemblyRead> U2AssmblyDbiUtils::getShiftedReadsToLeft(U2AssemblyDbi* srcAssemblyDbi, U2DataId srcObjId, const U2Region& desiredRegion, U2OpStatus& os) {
    QList<U2AssemblyRead> shiftedReadsList;
    QScopedPointer<U2DbiIterator<U2AssemblyRead>> iterPtr(srcAssemblyDbi->getReads(srcObjId, desiredRegion, os, true, true));
    CHECK_OP(os, shiftedReadsList);
    QScopedPointer<U2DbiIterator<U2AssemblyRead>> iterForShiftCalculationPtr(srcAssemblyDbi->getReads(srcObjId, desiredRegion, os, true, true));
    CHECK_OP(os, shiftedReadsList);

    qint64 shiftValue = std::numeric_limits<qint64>::max();
    qint64 regionLength = 0;
    while (iterForShiftCalculationPtr->hasNext()) {
        U2AssemblyRead read = iterForShiftCalculationPtr->next();
        CHECK_OP(os, shiftedReadsList);
        shiftValue = qMin(read->leftmostPos, shiftValue);
        regionLength = qMax(read->leftmostPos + read->effectiveLen, regionLength);
    }

    regionLength -= shiftValue;
    
    while (iterPtr->hasNext()) {
        U2AssemblyRead read(new U2AssemblyReadData(*iterPtr->next().data()));
        U2AssemblyReadData* rd = read.data();
        CHECK_OP(os, shiftedReadsList);
        read->leftmostPos -= shiftValue;
        shiftedReadsList.append(read);
    }
    return shiftedReadsList;
}

}
