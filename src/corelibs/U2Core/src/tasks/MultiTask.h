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

#ifndef _U2_MULTI_TASK_H_
#define _U2_MULTI_TASK_H_

#include <U2Core/Task.h>

namespace U2 {

class StateLock;

//runs all given tasks as subtasks
class U2CORE_EXPORT MultiTask : public Task {
    Q_OBJECT
public:
    MultiTask(const QString &name, const QList<Task *> &taskz, bool withLock = false, TaskFlags f = TaskFlags_NR_FOSCOE);

    QList<Task *> getTasks() const;

    ReportResult report();
    QString generateReport() const;

private:
    StateLock *l;
    QList<Task *> tasks;
};

//waits until each given task is finished and runs the next task after that
class U2CORE_EXPORT SequentialMultiTask : public Task{
    Q_OBJECT
public:
    SequentialMultiTask(const QString &name, const QList<Task *> &taskz, TaskFlags f = TaskFlags_NR_FOSCOE);

    void prepare();
    virtual QList<Task *> onSubTaskFinished(Task *subTask);
    QList<Task *> getTasks() const;

private:
    QList<Task *> tasks;
};

} //namespace

#endif
