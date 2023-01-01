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

#include <primitives/GTWidget.h>

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>

#include "GTUtilsTask.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsTask"

#define GT_METHOD_NAME "getTaskByName"
Task* GTUtilsTask::getTaskByName(HI::GUITestOpStatus& os, const QString& taskName, const GTGlobals::FindOptions& options) {
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    GT_CHECK_RESULT(scheduler != nullptr, "task scheduler is NULL", {});
    QList<Task*> allTasks = scheduler->getTopLevelTasks();
    Task* result = nullptr;
    for (int i = 0; i < allTasks.size(); i++) {
        Task* task = allTasks[i];
        QString name = task->getTaskName();
        if (name == taskName) {
            CHECK_SET_ERR_RESULT(result == nullptr, "Found multiple tasks with the same name: " + taskName + ", example: " + name, nullptr);
            result = task;
        }
        allTasks << task->getPureSubtasks();
    }
    GT_CHECK_RESULT(result != nullptr || !options.failIfNotFound, "No top-level with name " + taskName, nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoTask"
void GTUtilsTask::checkNoTask(HI::GUITestOpStatus& os, const QString& taskName) {
    Task* task = getTaskByName(os, taskName, {false});
    GT_CHECK(task == nullptr, "task " + taskName + " unexpectedly found");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "waitTaskStart"
void GTUtilsTask::waitTaskStart(HI::GUITestOpStatus& os, const QString& taskName, int timeout) {
    Task* task = nullptr;
    for (int time = 0; time < timeout && task == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        task = getTaskByName(os, taskName, {false});
    }
    GT_CHECK(task != nullptr, "waitTaskStart: task '" + taskName + "' is not found");
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
