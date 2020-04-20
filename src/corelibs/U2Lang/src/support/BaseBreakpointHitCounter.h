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

#ifndef _U2_BASE_BREAKPOINT_HIT_COUNTER_H_
#define _U2_BASE_BREAKPOINT_HIT_COUNTER_H_

#include "WorkflowBreakpointSharedInfo.h"

const quint32 DEFAULT_HIT_COUNTER_PARAMETER = 1;

namespace U2 {

class BaseBreakpointHitCounter {
public:
    BaseBreakpointHitCounter(BreakpointHitCountCondition initCondition);
    virtual ~BaseBreakpointHitCounter();
    // this method should use to be invoked to keep
    // the hit counter actual. If it returns true, then a breakpoint breaks
    // scheme's execution, else the execution proceeds
    virtual bool hit();
    virtual BreakpointHitCountCondition getCondition() const;
    virtual quint32 getHitCount() const;
    virtual quint32 getHitCountParameter() const;
    virtual void setHitCountParameter(quint32 newParameter);
    virtual void reset();

    static BaseBreakpointHitCounter *createInstance(BreakpointHitCountCondition
        kindOfCondition = ALWAYS, const QVariant &hitCounterParameter
        = QVariant::fromValue<quint32>(DEFAULT_HIT_COUNTER_PARAMETER));

protected:
    BreakpointHitCountCondition condition;
    quint32 hitCount;
};

} // namespace U2

#endif // _U2_BASE_BREAKPOINT_HIT_COUNTER_H_
