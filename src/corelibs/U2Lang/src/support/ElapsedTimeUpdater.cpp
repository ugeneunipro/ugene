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

#include <U2Core/Timer.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/WorkflowMonitor.h>

#include "ElapsedTimeUpdater.h"

namespace U2 {
namespace LocalWorkflow {

ElapsedTimeUpdater::ElapsedTimeUpdater(const ActorId& runningActorId, WorkflowMonitor* monitor, Task* executedTask)
    : runningActorId(runningActorId), monitor(monitor), executedTask(executedTask), elapsedTime(0)
{
    connect(this, SIGNAL(timeout()), SLOT(sl_updateTime()));
    connect(new TaskSignalMapper(executedTask), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
}

void ElapsedTimeUpdater::sl_taskFinished(Task*) {
    stop();
    qint64 newElapsedTime = GTimer::currentTimeMicros() - executedTask->getTimeInfo().startTime;
    monitor->addTick(newElapsedTime - elapsedTime, runningActorId);
    executedTask = NULL;
}

ElapsedTimeUpdater::~ElapsedTimeUpdater() {
    CHECK(NULL != executedTask,);
    qint64 newElapsedTime = executedTask->getTimeInfo().finishTime - executedTask->getTimeInfo().startTime;
    if(newElapsedTime > elapsedTime) {
        monitor->addTick(newElapsedTime - elapsedTime, runningActorId);
    }
}

void ElapsedTimeUpdater::sl_updateTime() {
    qint64 newElapsedTime = GTimer::currentTimeMicros() - executedTask->getTimeInfo().startTime;
    monitor->addTick(newElapsedTime - elapsedTime, runningActorId);
    elapsedTime = newElapsedTime;
}

} // LocalWorkflow
} // U2
