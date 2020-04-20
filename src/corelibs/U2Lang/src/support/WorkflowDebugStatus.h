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

#ifndef _U2_WORKFLOW_DEBUG_STATUS_H_
#define _U2_WORKFLOW_DEBUG_STATUS_H_

#include <QObject>

#include <U2Lang/ActorModel.h>
#include <U2Lang/WorkflowContext.h>

#include "WorkflowBreakpoint.h"
#include "WorkflowInvestigationData.h"

namespace U2 {

struct BreakpointHitCounterDump;
class WorkflowBreakpoint;
enum BreakpointHitCountCondition;
class AttributeScript;
enum BreakpointConditionParameter;
class WorkflowDebugMessageParser;

using namespace Workflow;

class U2LANG_EXPORT WorkflowDebugStatus : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowDebugStatus )
public:
    explicit WorkflowDebugStatus(QObject *parent = NULL);
    ~WorkflowDebugStatus();

    void setContext(WorkflowContext *initContext);
    void setMessageParser(WorkflowDebugMessageParser *initParser);
    WorkflowDebugMessageParser *getMessageParser( ) const;

    // the method should be invoked before the running
    // of an actor's task. If it has a breakpoint,
    // then scheme should be paused
    void checkActorForBreakpoint(const Actor *actor);

    bool isPaused() const;
    bool isCurrentStepIsolated() const;
    void requestForSingleStep(const ActorId &actor);

    //////////////////////////////////////////////////////////////////////////
    //////////Common breakpoint manipulations/////////////////////////////////

    // by default adding breakpoint has no extra parameters
    // e.g. hit counter or labels
    void addBreakpointToActor(const ActorId &actor);
    void removeBreakpointFromActor(const ActorId &actor);
    void setBreakpointEnabled(const ActorId &actor, bool enabled);
    QList<ActorId> getActorsWithBreakpoints() const;

    //////////////////////////////////////////////////////////////////////////
    ///////////  Breakpoint labels control  //////////////////////////////////

    void setBreakpointLabels(const ActorId &actor, QStringList actualLabels);
    QStringList getBreakpointLabels(const ActorId &actor) const;
    void addNewAvailableBreakpointLabels(const QStringList &newLabels) const;
    QStringList getAvailableBreakpointLabels() const;

    //////////////////////////////////////////////////////////////////////////
    ///////////Breakpoint hit counter control/////////////////////////////////

    const BreakpointHitCounterDump getHitCounterDumpForActor(const ActorId &actor) const;
    void resetHitCounterForActor(const ActorId &actor) const;
    void setHitCounterForActor(const ActorId &actor, BreakpointHitCountCondition typeOfCounter,
        quint32 parameter);
    quint32 getHitCountForActor(const ActorId &actor) const;

    //////////////////////////////////////////////////////////////////////////
    /////////// Breakpoint condition control /////////////////////////////////

    BreakpointConditionDump getConditionDumpForActor(const ActorId &actor) const;
    void setConditionEnabledForActor(const ActorId &actor, bool enabled);
    void setConditionParameterForActor(const ActorId &actor,
        BreakpointConditionParameter newParameter);
    void setConditionTextForActor(const ActorId &actor, const QString &newCondition);

    //////////////////////////////////////////////////////////////////////////
    ///////////Link investigating control/////////////////////////////////

    void respondToInvestigator(const WorkflowInvestigationData &investigationInfo,
        const Link *bus);
    void respondMessagesCount(const Link *bus, int countOfMessages);
    void respondMessageAttributesCount(const Link *bus, int countOfMessageAttributes);
    void convertMessagesToDocuments(const Workflow::Link *bus, const QString &messageType,
        int messageNumber, const QString &schemeName);

public slots:
    void sl_pauseTriggerActivated();
    void sl_resumeTriggerActivated();
    void sl_isolatedStepTriggerActivated();
    // the slot brings object to its initial default state.
    // it's supposed to be called on scheme's stopping or finishing
    void sl_executionFinished();

signals:
    void si_pauseStateChanged(bool isPaused);
    void si_busInvestigationIsRequested(const Workflow::Link *bus, int messageNumber);
    void si_busCountOfMessagesIsRequested(const Workflow::Link *bus);
    void si_busCountOfMessagesResponse(const Workflow::Link *bus, int countOfMessages);
    void si_busInvestigationRespond(const WorkflowInvestigationData &info, const Workflow::Link *bus);
    void si_convertMessages2Documents(const Workflow::Link *bus, const QString &messageType,
        int messageNumber, const QString &schemeName);
    void si_singleStepIsRequested(const ActorId &actor);
    void si_breakpointAdded(const ActorId &actor);
    void si_breakpointRemoved(const ActorId &actor);
    void si_breakpointEnabled(const ActorId &actor);
    void si_breakpointDisabled(const ActorId &actor);
    void si_breakpointIsReached(const U2::ActorId &actor);

private:
    void setPause(bool pause);
    void makeIsolatedStep();
    bool isBreakpointActivated(const Actor *actor) const;
    bool hasBreakpoint(const ActorId &actor) const;
    // returns NULL if no breakpoint exists for given actor
    WorkflowBreakpoint *getBreakpointForActor(const ActorId &actor) const;
    void removeBreakpoint(WorkflowBreakpoint *breakpoint);

    QList<WorkflowBreakpoint *> breakpoints;
    bool paused;
    bool isStepIsolated;
    WorkflowContext *context;
    WorkflowDebugMessageParser *parser;

    static QList<BreakpointLabel> existingBreakpointLabels;
};

} // namespace U2

#endif // _U2_WORKFLOW_DEBUG_STATUS_H_
