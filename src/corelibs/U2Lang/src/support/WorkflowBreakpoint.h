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

#ifndef _U2_WORKFLOW_BREAKPOINT_H_
#define _U2_WORKFLOW_BREAKPOINT_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/WorkflowContext.h>

#include "WorkflowBreakpointSharedInfo.h"
#include "BreakpointConditionChecker.h"

namespace U2 {

typedef QString BreakpointLabel;

class BaseBreakpointHitCounter;
class AttributeScript;

class WorkflowBreakpoint {
    Q_DISABLE_COPY(WorkflowBreakpoint)
public:
    WorkflowBreakpoint(const ActorId &actor, Workflow::WorkflowContext *context = NULL);
    ~WorkflowBreakpoint();

    //////////////////////////////////////////////////////////////////////////
    ///////////        Common methods       //////////////////////////////////

    ActorId getActorId() const;
    void setContext(Workflow::WorkflowContext *context);

    void setEnabled(bool enable);
    bool isEnabled() const;

    //////////////////////////////////////////////////////////////////////////
    ///////////         Label control       //////////////////////////////////

    void setLabels(const QList<BreakpointLabel> &newLabels);
    QList<BreakpointLabel> getLabels() const;

    //////////////////////////////////////////////////////////////////////////
    ///////////     Hit counter control     //////////////////////////////////

    void setHitCounter(BreakpointHitCountCondition typeOfCounter, quint32 parameter);
    BreakpointHitCounterDump getHitCounterDump() const;
    quint32 getHitCount() const;
    void resetHitCounter();

    //////////////////////////////////////////////////////////////////////////
    ///////////      Condition control      //////////////////////////////////

    BreakpointConditionDump getConditionDump() const;
    void setConditionEnabled(bool enabled);
    void setConditionParameter(BreakpointConditionParameter newParameter);
    void setConditionText(const QString &text);

    // this method should be called before execution of an appropriate worker
    // to keep hit count actual
    bool hit(const AttributeScript *conditionContext);

private:
    bool enabled;
    const ActorId actorId;
    QList<BreakpointLabel> labels;
    BaseBreakpointHitCounter *hitCounter;
    BreakpointConditionChecker conditionChecker;
};

} // namespace U2

#endif // _U2_WORKFLOW_BREAKPOINT_H_
