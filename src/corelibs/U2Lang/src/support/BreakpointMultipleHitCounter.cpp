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

#include <U2Core/U2SafePoints.h>

#include "BreakpointMultipleHitCounter.h"

namespace U2 {

BreakpointMultipleHitCounter::BreakpointMultipleHitCounter(
    BreakpointHitCountCondition initCondition, quint32 initMultiple)
    : BaseBreakpointHitCounter(initCondition), multiple(initMultiple)
{
    SAFE_POINT( 0 < multiple, "Invalid value for hit counter!", );
}

bool BreakpointMultipleHitCounter::hit() {
    return (0 == ++hitCount % multiple) ? true : false;
}

quint32 BreakpointMultipleHitCounter::getHitCountParameter() const {
    return multiple;
}

void BreakpointMultipleHitCounter::setHitCountParameter(quint32 newParameter) {
    multiple = newParameter;
}

} // namespace U2
