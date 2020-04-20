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

#include <U2Core/U2SafePoints.h>

#include "NoFailTaskWrapper.h"

namespace U2 {

NoFailTaskWrapper::NoFailTaskWrapper(Task *task)
: Task("Wrapper", TaskFlags(TaskFlag_CancelOnSubtaskCancel) | TaskFlag_NoRun),
  subTask(task)
{
    SAFE_POINT_EXT(NULL != subTask, setError("NULL task"), );
    setTaskName(tr("Wrapper task for: \"%1\"").arg(subTask->getTaskName()));
}

void NoFailTaskWrapper::prepare() {
    CHECK(NULL != subTask, );
    addSubTask(subTask);
}

Task * NoFailTaskWrapper::originalTask() const {
    return subTask;
}

bool NoFailTaskWrapper::hasWarning() const {
    SAFE_POINT(subTask != NULL, tr("SubTask is NULL"), false);
    return subTask->hasWarning();
}

QStringList NoFailTaskWrapper::getWarnings() const {
    SAFE_POINT(subTask != NULL, tr("SubTask is NULL"), QStringList());
    return subTask->getWarnings();
}

} // U2
