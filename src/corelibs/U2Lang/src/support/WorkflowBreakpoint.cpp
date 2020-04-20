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

#include "WorkflowBreakpoint.h"
#include "BaseBreakpointHitCounter.h"

namespace U2 {

WorkflowBreakpoint::WorkflowBreakpoint(const ActorId &actor, Workflow::WorkflowContext *context)
    : enabled(true),
      actorId(actor),
      hitCounter(BaseBreakpointHitCounter::createInstance()),
      conditionChecker(QString(), context)
{

}

WorkflowBreakpoint::~WorkflowBreakpoint() {
    delete hitCounter;
}

void WorkflowBreakpoint::setEnabled(bool enable) {
    enabled = enable;
}

bool WorkflowBreakpoint::isEnabled() const {
    return enabled;
}

ActorId WorkflowBreakpoint::getActorId() const {
    return actorId;
}

void WorkflowBreakpoint::setLabels(const QList<BreakpointLabel> &newLabels) {
    labels = newLabels;
}

QList<BreakpointLabel> WorkflowBreakpoint::getLabels() const {
    return labels;
}

void WorkflowBreakpoint::setHitCounter(BreakpointHitCountCondition typeOfCounter, quint32
    parameter)
{
    if(typeOfCounter != hitCounter->getCondition()) {
        delete hitCounter;
        hitCounter = BaseBreakpointHitCounter::createInstance(typeOfCounter,
            QVariant::fromValue<quint32>(parameter));
    } else if(parameter != hitCounter->getHitCountParameter()) {
        hitCounter->setHitCountParameter(parameter);
    }
}

BreakpointConditionDump WorkflowBreakpoint::getConditionDump() const {
    BreakpointConditionDump result(conditionChecker.getConditionParameter(),
        conditionChecker.getConditionText(), conditionChecker.isEnabled());
    return result;
}

void WorkflowBreakpoint::setConditionEnabled(bool enabled) {
    conditionChecker.setEnabled(enabled);
}

void WorkflowBreakpoint::setConditionParameter(BreakpointConditionParameter newParameter) {
    conditionChecker.setConditionParameter(newParameter);
}

void WorkflowBreakpoint::setConditionText(const QString &text) {
    conditionChecker.setConditionText(text);
}

BreakpointHitCounterDump WorkflowBreakpoint::getHitCounterDump() const {
    return BreakpointHitCounterDump(hitCounter->getCondition(), hitCounter->getHitCount(),
        hitCounter->getHitCountParameter());
}

quint32 WorkflowBreakpoint::getHitCount() const {
    return hitCounter->getHitCount();
}

bool WorkflowBreakpoint::hit(const AttributeScript *conditionContext) {
    Q_ASSERT(NULL != hitCounter);
    return (conditionChecker.evaluateCondition(conditionContext) && hitCounter->hit());
}

void WorkflowBreakpoint::resetHitCounter() {
    Q_ASSERT(NULL != hitCounter);
    hitCounter->reset();
}

void WorkflowBreakpoint::setContext(Workflow::WorkflowContext *context) {
    conditionChecker.setContext(context);
}

} // namespace U2
