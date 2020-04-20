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

#include "TaskWatchdog.h"

namespace U2 {

TaskWatchdog::TaskWatchdog(QObject *resource, Task *task)
: resource(resource), task(task), cancelWithError(false)
{
    connect(resource, SIGNAL(destroyed()), SLOT(sl_onResourceDestroyed()));
}

void TaskWatchdog::setCancelError(const QString &errorMessage) {
    cancelWithError = true;
    this->errorMessage = errorMessage;
}

void TaskWatchdog::sl_onResourceDestroyed() {
    task->cancel();
    if (cancelWithError && !task->hasError()) {
        task->setError(errorMessage);
    }
}

void TaskWatchdog::trackResourceExistence(QObject *resource, Task *task) {
    TaskWatchdog *tracker = new TaskWatchdog(resource, task);
    tracker->setParent(task);
}

void TaskWatchdog::trackResourceExistence(QObject *resource, Task *task, const QString &errorMessage) {
    TaskWatchdog *tracker = new TaskWatchdog(resource, task);
    tracker->setParent(task);
    tracker->setCancelError(errorMessage);
}

} // U2
