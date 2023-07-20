/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "WorkflowDebugStatus.h"

#include <QCoreApplication>

#include <U2Core/U2SafePoints.h>

#include "WorkflowBreakpointSharedInfo.h"
#include "WorkflowDebugMessageParser.h"

namespace U2 {

QList<BreakpointLabel> WorkflowDebugStatus::existingBreakpointLabels = QList<BreakpointLabel>();

WorkflowDebugStatus::WorkflowDebugStatus(QObject* parent)
    : QObject(parent), paused(false), isStepIsolated(false), context(nullptr), parser(nullptr) {
}

WorkflowDebugStatus::~WorkflowDebugStatus() {
    qDeleteAll(breakpoints);
    delete parser;
}

WorkflowContext* WorkflowDebugStatus::getContext() const {
    return context;
}

void WorkflowDebugStatus::setContext(WorkflowContext* initContext) {
    SAFE_POINT(initContext != nullptr, "Invalid workflow context!", );
    context = initContext;
    if (Q_LIKELY(parser != nullptr)) {
        parser->setContext(initContext);
    }
}

void WorkflowDebugStatus::setMessageParser(WorkflowDebugMessageParser* initParser) {
    SAFE_POINT(initParser != nullptr, "Invalid workflow context!", );
    parser = initParser;
}

WorkflowDebugMessageParser* WorkflowDebugStatus::getMessageParser() const {
    return parser;
}

void WorkflowDebugStatus::setPause(bool pause) {
    if (pause != paused) {
        paused = pause;
        emit si_pauseStateChanged(paused);
    }
}

bool WorkflowDebugStatus::isPaused() const {
    return paused;
}

void WorkflowDebugStatus::makeIsolatedStep() {
    isStepIsolated = true;
    setPause(false);
}

bool WorkflowDebugStatus::isCurrentStepIsolated() const {
    return isStepIsolated;
}

void WorkflowDebugStatus::addBreakpointToActor(const ActorId& actor) {
    if (!hasBreakpoint(actor)) {
        breakpoints.append(new WorkflowBreakpoint(actor, context));
        emit si_breakpointAdded(actor);
    }
}

void WorkflowDebugStatus::removeBreakpointFromActor(const ActorId& actor) {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    removeBreakpoint(breakpoint);
}

void WorkflowDebugStatus::checkActorForBreakpoint(const Actor* actor) {
    if (isBreakpointActivated(actor)) {
        setPause(true);
        emit si_breakpointIsReached(actor->getId());
    }
    if (isStepIsolated) {
        isStepIsolated = false;
        setPause(true);
    }
}

void WorkflowDebugStatus::sl_pauseTriggerActivated() {
    setPause(true);
}

void WorkflowDebugStatus::sl_resumeTriggerActivated() {
    setPause(false);
}

void WorkflowDebugStatus::sl_isolatedStepTriggerActivated() {
    makeIsolatedStep();
}

void WorkflowDebugStatus::sl_executionFinished() {
    setPause(false);
    isStepIsolated = false;

    foreach (WorkflowBreakpoint* breakpoint, breakpoints) {
        breakpoint->resetHitCounter();
        breakpoint->setContext(nullptr);
    }
}

void WorkflowDebugStatus::respondToInvestigator(const WorkflowInvestigationData& investigationInfo,
                                                const Workflow::Link* bus) {
    emit si_busInvestigationRespond(investigationInfo, bus);
}

void WorkflowDebugStatus::respondMessagesCount(const Link* bus, int countOfMessages) {
    emit si_busCountOfMessagesResponse(bus, countOfMessages);
}

void WorkflowDebugStatus::requestForSingleStep(const ActorId& actor) {
    emit si_singleStepIsRequested(actor);
    makeIsolatedStep();
}

WorkflowBreakpoint* WorkflowDebugStatus::getBreakpointForActor(const ActorId& actor) const {
    foreach (WorkflowBreakpoint* breakpoint, breakpoints) {
        if (actor == breakpoint->getActorId()) {
            return breakpoint;
        }
    }
    return nullptr;
}

bool WorkflowDebugStatus::isBreakpointActivated(const Actor* actor) const {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor->getId());
    if (breakpoint == nullptr || !breakpoint->isEnabled()) {
        return false;
    }
    breakpoint->setContext(context);
    return breakpoint->hit(actor->getCondition());
}

bool WorkflowDebugStatus::hasBreakpoint(const ActorId& actor) const {
    return (getBreakpointForActor(actor) != nullptr);
}

void WorkflowDebugStatus::setBreakpointEnabled(const ActorId& actor, bool enabled) {
    foreach (WorkflowBreakpoint* breakpoint, breakpoints) {
        if (actor == breakpoint->getActorId()) {
            breakpoint->setEnabled(enabled);
            if (enabled) {
                emit si_breakpointEnabled(actor);
            } else {
                emit si_breakpointDisabled(actor);
            }
        }
    }
}

void WorkflowDebugStatus::setBreakpointLabels(const ActorId& actor, QStringList actualLabels) {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    breakpoint->setLabels(actualLabels);
}

QStringList WorkflowDebugStatus::getBreakpointLabels(const ActorId& actor) const {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    return QStringList(breakpoint->getLabels());
}

void WorkflowDebugStatus::addNewAvailableBreakpointLabels(const QStringList& newLabels) const {
    foreach (QString label, newLabels) {
        existingBreakpointLabels.append(label);
    }
}

QStringList WorkflowDebugStatus::getAvailableBreakpointLabels() const {
    return QStringList(existingBreakpointLabels);
}

const BreakpointHitCounterDump WorkflowDebugStatus::getHitCounterDumpForActor(const ActorId& actor)
    const {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    return breakpoint->getHitCounterDump();
}

void WorkflowDebugStatus::resetHitCounterForActor(const ActorId& actor) const {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    breakpoint->resetHitCounter();
}

void WorkflowDebugStatus::setHitCounterForActor(const ActorId& actor, BreakpointHitCountCondition typeOfCounter, quint32 parameter) {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    breakpoint->setHitCounter(typeOfCounter, parameter);
}

void WorkflowDebugStatus::removeBreakpoint(WorkflowBreakpoint* breakpoint) {
    ActorId owner = breakpoint->getActorId();
    int removedCount = breakpoints.removeAll(breakpoint);
    Q_ASSERT(removedCount == 1);
    Q_UNUSED(removedCount);
    delete breakpoint;
    emit si_breakpointRemoved(owner);
}

quint32 WorkflowDebugStatus::getHitCountForActor(const ActorId& actor) const {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    return breakpoint->getHitCount();
}

BreakpointConditionDump WorkflowDebugStatus::getConditionDumpForActor(const ActorId& actor) const {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    return breakpoint->getConditionDump();
}

void WorkflowDebugStatus::setConditionParameterForActor(const ActorId& actor,
                                                        BreakpointConditionParameter newParameter) {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    breakpoint->setConditionParameter(newParameter);
}

void WorkflowDebugStatus::setConditionEnabledForActor(const ActorId& actor, bool enabled) {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    breakpoint->setConditionEnabled(enabled);
}

void WorkflowDebugStatus::setConditionTextForActor(const ActorId& actor,
                                                   const QString& newCondition) {
    WorkflowBreakpoint* breakpoint = getBreakpointForActor(actor);
    Q_ASSERT(breakpoint != nullptr);
    breakpoint->setConditionText(newCondition);
}

void WorkflowDebugStatus::convertMessagesToDocuments(const Workflow::Link* bus,
                                                     const QString& messageType,
                                                     int messageNumber,
                                                     const QString& schemeName) {
    emit si_convertMessages2Documents(bus, messageType, messageNumber, schemeName);
}

QList<ActorId> WorkflowDebugStatus::getActorsWithBreakpoints() const {
    QList<ActorId> result;
    foreach (WorkflowBreakpoint* breakpoint, breakpoints) {
        result << breakpoint->getActorId();
    }
    return result;
}

}  // namespace U2
