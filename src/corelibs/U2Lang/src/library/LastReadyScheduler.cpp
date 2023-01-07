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

#include "LastReadyScheduler.h"

#include <U2Core/TaskSignalMapper.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ElapsedTimeUpdater.h>
#include <U2Lang/WorkflowDebugStatus.h>
#include <U2Lang/WorkflowMonitor.h>

namespace U2 {

namespace LocalWorkflow {

LastReadyScheduler::LastReadyScheduler(Schema* sh)
    : Scheduler(sh), lastWorker(nullptr), canLastTaskBeCanceled(false), requestedActorForNextTick(), timeUpdater(nullptr) {
}

LastReadyScheduler::~LastReadyScheduler() {
    delete timeUpdater;
}

void LastReadyScheduler::init() {
    foreach (Actor* a, schema->getProcesses()) {
        BaseWorker* w = a->castPeer<BaseWorker>();
        QList<IntegralBus*> portBuses = w->getPorts().values();
        for (IntegralBus* bus : qAsConst(portBuses)) {
            bus->setWorkflowContext(context);
        }
        w->setContext(context);
        w->init();
    }

    topologicSortedGraph = schema->getActorBindingsGraph().getTopologicalSortedGraph(schema->getProcesses());
}

bool LastReadyScheduler::isReady() const {
    foreach (Actor* a, schema->getProcesses()) {
        if (a->castPeer<BaseWorker>()->isReady()) {
            return true;
        }
    }
    return false;
}

inline ActorId LastReadyScheduler::actorId() const {
    CHECK(nullptr != lastWorker, "");
    return lastWorker->getActor()->getId();
}

inline bool LastReadyScheduler::hasValidFinishedTask() const {
    return (nullptr != lastWorker) && (nullptr != lastTask) && (lastTask->isFinished());
}

inline qint64 LastReadyScheduler::lastTaskTimeSec() const {
    qint64 startMks = lastTask->getTimeInfo().startTime;
    qint64 endMks = lastTask->getTimeInfo().finishTime;
    return endMks - startMks;
}

inline void LastReadyScheduler::measuredTick() {
    CHECK(lastWorker != nullptr, );
    lastWorker->deleteBackupMessagesFromPreviousTick();

    lastTask = lastWorker->tick(canLastTaskBeCanceled);

    delete timeUpdater;
    timeUpdater = nullptr;

    if (lastTask != nullptr) {
        timeUpdater = new ElapsedTimeUpdater(actorId(), context->getMonitor(), lastTask);
        timeUpdater->start(1000);

        context->getMonitor()->registerTask(lastTask, actorId());
    }
}

Task* LastReadyScheduler::tick() {
    for (int vertexLabel = 0; vertexLabel < topologicSortedGraph.size(); vertexLabel++) {
        foreach (Actor* a, topologicSortedGraph.value(vertexLabel)) {
            if (a->castPeer<BaseWorker>()->isReady()) {
                if (requestedActorForNextTick.isEmpty() || a->getId() == requestedActorForNextTick) {
                    lastWorker = a->castPeer<BaseWorker>();
                    measuredTick();
                    debugInfo->checkActorForBreakpoint(a);
                    if (!requestedActorForNextTick.isEmpty()) {
                        requestedActorForNextTick = ActorId();
                    }
                    return lastTask;
                }
            }
        }
    }
    assert(0);
    return nullptr;
}

Task* LastReadyScheduler::replayLastWorkerTick() {
    lastWorker->saveCurrentChannelsStateAndRestorePrevious();
    Task* result = lastWorker->tick();
    lastWorker->restoreActualChannelsState();
    return result;
}

bool LastReadyScheduler::isDone() const {
    foreach (Actor* a, schema->getProcesses()) {
        if (!a->castPeer<BaseWorker>()->isDone()) {
            return false;
        }
    }
    return true;
}

void LastReadyScheduler::cleanup() {
    foreach (Actor* a, schema->getProcesses()) {
        a->castPeer<BaseWorker>()->cleanup();
    }
}

WorkerState LastReadyScheduler::getWorkerState(const Actor* a) {
    BaseWorker* w = a->castPeer<BaseWorker>();
    if (lastWorker == w) {
        Task* t = lastTask;
        if (w->isDone() && t && t->isFinished()) {
            return WorkerDone;
        }
        return WorkerRunning;
    }
    if (w->isDone()) {
        return WorkerDone;
    } else if (w->isReady()) {
        return WorkerReady;
    }
    return WorkerWaiting;
}

WorkerState LastReadyScheduler::getWorkerState(const ActorId& id) {
    Actor* actor1 = schema->actorById(id);
    if (actor1 != nullptr) {
        return getWorkerState(actor1);
    }
    QList<Actor*> actors = schema->actorsByOwnerId(id);
    assert(actors.size() > 0);

    bool someWaiting = false;
    bool someDone = false;
    bool someReady = false;
    for (Actor* actor2 : qAsConst(actors)) {
        WorkerState state = getWorkerState(actor2);
        switch (state) {
            case WorkerRunning:
                return WorkerRunning;
            case WorkerWaiting:
                someWaiting = true;
                break;
            case WorkerDone:
                someDone = true;
                break;
            case WorkerReady:
                someReady = true;
                break;
            default:
                break;
        }
    }
    if (someWaiting) {
        return WorkerWaiting;
    }
    if (someReady) {
        return WorkerReady;
    } else {
        assert(someDone);
        Q_UNUSED(someDone);
        return WorkerDone;
    }
}

bool LastReadyScheduler::cancelCurrentTaskIfAllowed() {
    if (nullptr == lastTask) {
        return false;
    }
    if (!lastTask->isFinished() && canLastTaskBeCanceled) {
        lastTask->cancel();
        return true;
    }
    return false;
}

void LastReadyScheduler::makeOneTick(const ActorId& actor) {
    requestedActorForNextTick = actor;
}

}  // namespace LocalWorkflow

}  // namespace U2
