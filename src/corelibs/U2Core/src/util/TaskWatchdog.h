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

#ifndef _U2_TASK_WATCHDOG_H_
#define _U2_TASK_WATCHDOG_H_

#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class Task;

/**
 * Utility class for canceling of a task on different events with GObject, Documents
 * and other entities that can be used in a task. For example, it can be used to cancel a task
 * on removing of a document from a project.
 * This abstraction is needed to move this logic out of Task for making the code simpler.
 */
class U2CORE_EXPORT TaskWatchdog : public QObject {
    Q_OBJECT
public:
    TaskWatchdog(QObject *resource, Task *task);

    void setCancelError(const QString &errorMessage);

    // Cancels the task if the resource is destroyed.
    static void trackResourceExistence(QObject *resource, Task *task);

    // Cancels the task and set the error to the task if the resource is destroyed.
    static void trackResourceExistence(QObject *resource, Task *task, const QString &errorMessage);

private slots:
    void sl_onResourceDestroyed();

private:
    const QObject *resource;
    Task *task;
    bool cancelWithError;
    QString errorMessage;
};

} // U2

#endif // _U2_TASK_WATCHDOG_H_
