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

#include <primitives/GTWidget.h>
#include "GTUtilsTaskTreeView.h"

#include "GTUtilsTask.h"
#include <U2Core/AppContext.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsTask"

#define GT_METHOD_NAME "getTopLevelTasks"
QList<Task*> GTUtilsTask::getTopLevelTasks(HI::GUITestOpStatus &os){
    Q_UNUSED(os);
    TaskScheduler* scheduller = AppContext::getTaskScheduler();
    GT_CHECK_RESULT(scheduller != NULL, "task scheduler is NULL", QList<Task*>());
    return scheduller->getTopLevelTasks();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSubTaskByName"
Task* GTUtilsTask::getSubTaskByName(HI::GUITestOpStatus &os, QString taskName, GTGlobals::FindOptions options){
    Task* result;
    QList<Task*> tasks = getTopLevelTasks(os);
    foreach (Task* t, tasks) {
        result = getSubTaskByName(os, t, taskName, false);
    }
    if(options.failIfNotFound == true){
        GT_CHECK_RESULT(result != NULL, "no subtask with name " + taskName, NULL);
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSubTaskByName"
Task* GTUtilsTask::getSubTaskByName(HI::GUITestOpStatus &os, Task *parent, QString taskName, GTGlobals::FindOptions options){
    Task* result;
    foreach (const QPointer<Task> &t, parent->getSubtasks()) {
        if(t->getTaskName() == taskName){
            return t.data();
        }else{
            result = getSubTaskByName(os, t.data(), taskName, false);
        }
    }

    if(options.failIfNotFound == true){
        GT_CHECK_RESULT(result != NULL, "no subtask with name " + taskName, NULL);
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTaskByName"
Task* GTUtilsTask::getTaskByName(HI::GUITestOpStatus &os, QString taskName, GTGlobals::FindOptions options){
    QList<Task*> tasks = getTopLevelTasks(os);
    foreach (Task* t, tasks) {
        QString name = t->getTaskName();
        if(name == taskName){
            return t;
        }
    }
    if(options.failIfNotFound == true){
        GT_CHECK_RESULT(false, "task " + taskName + " not found", NULL);
    }
    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTask"
void GTUtilsTask::checkTask(HI::GUITestOpStatus &os, QString taskName){
    getTaskByName(os, taskName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoTask"
void GTUtilsTask::checkNoTask(HI::GUITestOpStatus &os, QString taskName){
    Task* t = getTaskByName(os, taskName, GTGlobals::FindOptions(false));
    GT_CHECK(t == NULL, "tast " + taskName + " unexpectidly found");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "cancelTask"
void GTUtilsTask::cancelTask(HI::GUITestOpStatus &os, QString taskName){
    Task* t = getTaskByName(os, taskName);
    t->cancel();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "cancelSubTask"
void GTUtilsTask::cancelSubTask(HI::GUITestOpStatus &os, QString taskName){
    Task* t = getSubTaskByName(os, taskName);
    t->cancel();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "waitTaskStart"
void GTUtilsTask::waitTaskStart(HI::GUITestOpStatus &os, QString taskName, int timeOut){
    int i = 0;
    while(getTaskByName(os, taskName, GTGlobals::FindOptions(false)) == NULL){
       GTGlobals::sleep(100);
       i++;
       if(i > (timeOut/100)){
           GT_CHECK(false, "task " + taskName + " not launched");
           break;
       }
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}
