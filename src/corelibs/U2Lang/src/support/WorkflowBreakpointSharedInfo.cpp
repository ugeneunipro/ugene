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

#include "WorkflowBreakpointSharedInfo.h"

const char *BREAK_ALWAYS = "break always";
const char *BREAK_WHEN_HIT_COUNT_EQUAL = "break when the hit count is equal to ";
const char *BREAK_WHEN_HIT_COUNT_MULTIPLE = "break when the hit count is multiple of ";
const char *BREAK_WHEN_HIT_COUNT_GREATER_OR_EQUAL = "break when the hit count is greater than or equal to ";

namespace U2 {

BreakpointHitCounterDump::BreakpointHitCounterDump(BreakpointHitCountCondition _condition, quint32 _hitCount,
    quint32 _hitCounterParameter) : typeOfCondition(_condition), hitCount(_hitCount),
    hitCounterParameter(_hitCounterParameter)
{

}

U2LANG_EXPORT const QMap<BreakpointHitCountCondition, QString> &getNamesOfHitCounters() {
    static QMap<BreakpointHitCountCondition, QString> namesOfHitCounters;
    if(namesOfHitCounters.isEmpty()) {
        namesOfHitCounters[ALWAYS] = QObject::tr(BREAK_ALWAYS);
        namesOfHitCounters[HIT_COUNT_EQUAL] = QObject::tr(BREAK_WHEN_HIT_COUNT_EQUAL);
        namesOfHitCounters[HIT_COUNT_MULTIPLE] = QObject::tr(BREAK_WHEN_HIT_COUNT_MULTIPLE);
        namesOfHitCounters[HIT_COUNT_GREATER_OR_EQUAL]
            = QObject::tr(BREAK_WHEN_HIT_COUNT_GREATER_OR_EQUAL);
    }
    return namesOfHitCounters;
}

BreakpointConditionDump::BreakpointConditionDump(BreakpointConditionParameter _conditionParameter,
    const QString &_condition, bool _isEnabled) : conditionParameter(_conditionParameter),
    condition(_condition), isEnabled(_isEnabled)
{

}

} // namespace U2
