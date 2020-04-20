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

#include <U2Core/TaskSignalMapper.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ElapsedTimeUpdater.h>
#include <U2Lang/WorkflowDebugStatus.h>
#include <U2Lang/WorkflowMonitor.h>

#include "LastReadyScheduler.h"

namespace U2 {

namespace LocalWorkflow {

LastReadyScheduler::LastReadyScheduler(Schema *sh)
    : Scheduler(sh), lastWorker(NULL), canLastTaskBeCanceled(false), requestedActorForNextTick(), timeUpdater(NULL)
{

}

LastReadyScheduler::~LastReadyScheduler() {
    delete timeUpdater;
}

void LastReadyScheduler::init() {
    foreach(Actor* a, schema->getProcesses()) {
        BaseWorker *w = a->castPeer<BaseWorker>();
        foreach (IntegralBus *bus, w->getPorts().values()) {
            bus->setWorkflowContext(context);
        }
        w->setContext(context);
        w->init();
    }

    topologicSortedGraph = schema->getActorBindingsGraph().getTopologicalSortedGraph(schema->getProcesses());
}

bool LastReadyScheduler::isReady() const {
    foreach(Actor* a, schema->getProcesses()) {
        if (a->castPeer<BaseWorker>()->isReady()) {
            return true;
        }
    }
    return false;
}

inline ActorId LastReadyScheduler::actorId() const {
    CHECK(NULL != lastWorker, "");
    return lastWorker->getActor()->getId();
}

inline bool LastReadyScheduler::hasValidFinishedTask() const {
    return (NULL != lastWorker) && (NULL != lastTask) && (lastTask->isFinished());
}

inline qint64 LastReadyScheduler::lastTaskTimeSec() const {
    qint64 startMks = lastTask->getTimeInfo().startTime;
    qint64 endMks = lastTask->getTimeInfo().finishTime;
    return endMks - startMks;
}

inline void LastReadyScheduler::measuredTick() {
    CHECK(NULL != lastWorker, );
    lastWorker->deleteBackupMessagesFromPreviousTick();

    lastTask = lastWorker->tick(canLastTaskBeCanceled);

    delete timeUpdater;
    timeUpdater = NULL;

    if (NULL != lastTask) {
        timeUpdater = new ElapsedTimeUpdater(actorId(), context->getMonitor(), lastTask);
        timeUpdater->start(1000);

        context->getMonitor()->registerTask(lastTask, actorId());
    }
}

Task * LastReadyScheduler::tick() {
    for (int vertexLabel = 0; vertexLabel < topologicSortedGraph.size(); vertexLabel++) {
        foreach (Actor *a, topologicSortedGraph.value(vertexLabel)) {
            if (a->castPeer<BaseWorker>()->isReady()) {
                if(requestedActorForNextTick.isEmpty() || a->getId() == requestedActorForNextTick) {
                    lastWorker = a->castPeer<BaseWorker>();
                    measuredTick();
                    debugInfo->checkActorForBreakpoint(a);
                    if(!requestedActorForNextTick.isEmpty()) {
                        requestedActorForNextTick = ActorId();
                    }
                    return lastTask;
                }

            }
        }
    }
    assert(0);
    return NULL;
}

Task * LastReadyScheduler::replayLastWorkerTick() {
    lastWorker->saveCurrentChannelsStateAndRestorePrevious();
    Task *result = lastWorker->tick();
    lastWorker->restoreActualChannelsState();
    return result;
}

bool LastReadyScheduler::isDone() const {
    foreach(Actor* a, schema->getProcesses()) {
        if (!a->castPeer<BaseWorker>()->isDone()) {
            return false;
        }
    }
    return true;
}

void LastReadyScheduler::cleanup() {
    foreach(Actor* a, schema->getProcesses()) {
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

WorkerState LastReadyScheduler::getWorkerState(const ActorId &id) {
    Actor* a = schema->actorById(id);
    if (NULL == a) {
        QList<Actor*> actors = schema->actorsByOwnerId(id);
        assert(actors.size() > 0);

        bool someWaiting = false;
        bool someDone = false;
        bool someReady = false;
        foreach (Actor *a, actors) {
            WorkerState state = getWorkerState(a);
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
        } else if (someReady) {
            return WorkerReady;
        } else {
            assert(someDone);
            Q_UNUSED(someDone);
            return WorkerDone;
        }
    } else {
        return getWorkerState(a);
    }
}

bool LastReadyScheduler::cancelCurrentTaskIfAllowed() {
    if(NULL == lastTask) {
        return false;
    }
    if(!lastTask->isFinished() && canLastTaskBeCanceled) {
        lastTask->cancel();
        return true;
    }
    return false;
}

void LastReadyScheduler::makeOneTick(const ActorId &actor) {
    requestedActorForNextTick = actor;
}

} // LocalWorkflow

} // U2
