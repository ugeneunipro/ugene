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

#include <U2Core/Task.h>

#include "TaskSignalMapper.h"

namespace U2 {

TaskSignalMapper::TaskSignalMapper(Task *t)
    : QObject(t), task(t)
{
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskStateChanged()), Qt::DirectConnection);
}

void TaskSignalMapper::sl_taskStateChanged() {
    if (NULL != task) {
        switch (task->getState()) {
        case Task::State_Prepared:
            emit si_taskPrepared(task);
            break;
        case Task::State_Running:
            emit si_taskRunning(task);
            break;
        case Task::State_Finished:
            emit si_taskFinished(task);
            if (task->hasError() || task->hasSubtasksWithErrors()) {
                emit si_taskFailed(task);
            } else {
                emit si_taskSucceeded(task);
            }
            break;
        case Task::State_New:
            break;
        }
    }
}

Task * TaskSignalMapper::getTask() const {
    return task;
}

} //namespace U2
