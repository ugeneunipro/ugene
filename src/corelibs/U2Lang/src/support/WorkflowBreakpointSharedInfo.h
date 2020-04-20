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

#ifndef _U2_WORKFLOW_BREAKPOINT_HIT_COUNTER_SHARED_INFO_H_
#define _U2_WORKFLOW_BREAKPOINT_HIT_COUNTER_SHARED_INFO_H_

#include <QMap>
#include <QString>

#include <U2Core/global.h>
#include <U2Lang/Attribute.h>

namespace U2 {

enum BreakpointHitCountCondition {
    ALWAYS, HIT_COUNT_EQUAL, HIT_COUNT_MULTIPLE, HIT_COUNT_GREATER_OR_EQUAL
};

struct U2LANG_EXPORT BreakpointHitCounterDump {
    BreakpointHitCounterDump(BreakpointHitCountCondition _condition, quint32 _hitCount,
        quint32 _hitCounterParameter);

    BreakpointHitCountCondition typeOfCondition;
    quint32 hitCount;
    quint32 hitCounterParameter;
};

U2LANG_EXPORT const QMap<BreakpointHitCountCondition, QString> &getNamesOfHitCounters();

enum BreakpointConditionParameter {
    IS_TRUE, HAS_CHANGED
};

struct U2LANG_EXPORT BreakpointConditionDump {
    BreakpointConditionDump(BreakpointConditionParameter _conditionParameter,
        const QString &_condition, bool _isEnabled);

    BreakpointConditionParameter conditionParameter;
    QString condition;
    bool isEnabled;
};

}

#endif // _U2_WORKFLOW_BREAKPOINT_HIT_COUNTER_SHARED_INFO_H_
