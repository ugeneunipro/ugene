/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
 * https://ugene.net
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
#include "MfoldSettings.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {
namespace Mfold {
void toGenbankLocation(U2Location& location, qint64 seqLen) {
    SAFE_POINT(!location->regions.isEmpty(), "Wrong location preconditions: expected 1 region in location, got 0", );

    U2Region region = location->regions[0];
    auto regionHasJunctionPoint = region.length > seqLen - region.startPos;
    if (!regionHasJunctionPoint) {
        return;
    }
    auto firstPartLen = seqLen - region.startPos;
    location->regions = {{region.startPos, firstPartLen}, {0, region.length - firstPartLen}};
    location->op = U2LocationOperator::U2LocationOperator_Join;
}
}  // namespace Mfold
}  // namespace U2
