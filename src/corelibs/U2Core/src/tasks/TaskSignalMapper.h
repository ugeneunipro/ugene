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

#ifndef _U2_TASK_MAPPER_H_
#define _U2_TASK_MAPPER_H_

#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class Task;

class U2CORE_EXPORT TaskSignalMapper : public QObject {
    Q_OBJECT
public:
    TaskSignalMapper(Task *t);
    Task *getTask() const;

private slots:
    void sl_taskStateChanged();

signals:
    void si_taskPrepared(Task *);
    void si_taskRunning(Task *);
    void si_taskFinished(Task *);
    void si_taskSucceeded(Task *);
    void si_taskFailed(Task *);

private:
    Task *task;
};

} //namespace U2

#endif
