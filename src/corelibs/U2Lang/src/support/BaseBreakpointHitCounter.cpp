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

#include "BaseBreakpointHitCounter.h"
#include "BreakpointEqualHitCounter.h"
#include "BreakpointGreaterOrEqualHitCounter.h"
#include "BreakpointMultipleHitCounter.h"

const quint32 DEFAULT_HIT_COUNT_PARAMETER = 1;
const quint32 HIT_COUNTER_INITIAL_VALUE = 0;

namespace U2 {

BaseBreakpointHitCounter::BaseBreakpointHitCounter(BreakpointHitCountCondition initCondition)
    : condition(initCondition), hitCount(HIT_COUNTER_INITIAL_VALUE)
{

}

BaseBreakpointHitCounter::~BaseBreakpointHitCounter() {

}

BreakpointHitCountCondition BaseBreakpointHitCounter::getCondition() const {
    return condition;
}

bool BaseBreakpointHitCounter::hit() {
    ++hitCount;
    return true;
}

quint32 BaseBreakpointHitCounter::getHitCount() const {
    return hitCount;
}

void BaseBreakpointHitCounter::setHitCountParameter(quint32 newParameter) {
    Q_UNUSED(newParameter);
}

quint32 BaseBreakpointHitCounter::getHitCountParameter() const {
    return DEFAULT_HIT_COUNT_PARAMETER;
}

void BaseBreakpointHitCounter::reset() {
    hitCount = HIT_COUNTER_INITIAL_VALUE;
}

BaseBreakpointHitCounter * BaseBreakpointHitCounter::createInstance(
    BreakpointHitCountCondition kindOfCondition, const QVariant &hitCounterParameter)
{
    BaseBreakpointHitCounter *result = NULL;
    bool conversionResult = true;
    switch(kindOfCondition) {
    case ALWAYS:
        result = new BaseBreakpointHitCounter(kindOfCondition);
        break;
    case HIT_COUNT_EQUAL:
        result = new BreakpointEqualHitCounter(kindOfCondition,
            hitCounterParameter.toUInt(&conversionResult));
        break;
    case HIT_COUNT_MULTIPLE:
        result = new BreakpointMultipleHitCounter(kindOfCondition,
            hitCounterParameter.toUInt(&conversionResult));
        break;
    case HIT_COUNT_GREATER_OR_EQUAL:
        result = new BreakpointGreaterOrEqualHitCounter(kindOfCondition,
            hitCounterParameter.toUInt(&conversionResult));
        break;
    default:
        Q_ASSERT(false);
    }
    Q_ASSERT(NULL != result);
    Q_ASSERT(conversionResult);

    return result;
}

} // namespace U2
