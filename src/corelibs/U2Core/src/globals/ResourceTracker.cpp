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

#include "ResourceTracker.h"

#include "Task.h"

namespace U2 {

ResourceTracker::~ResourceTracker() {
    assert(resMap.isEmpty());
}


void ResourceTracker::registerResourceUser(const QString& resourceName, Task* t) {
    TList list = resMap.value(resourceName);
    assert(!list.contains(t));
    list.append(t);
    resMap[resourceName] = list;
    coreLog.details(tr("resource '%1' is used by '%2'").arg(resourceName).arg(t->getTaskName()));
    emit si_resourceUserRegistered(resourceName, t);
}



void ResourceTracker::unregisterResourceUser(const QString& resourceName, Task* t) {
    assert(resMap.contains(resourceName));
    TList list = resMap.value(resourceName);
    assert(list.contains(t));
    list.removeOne(t);
    if (list.isEmpty()) {
        resMap.remove(resourceName);
    } else {
        resMap[resourceName] = list;
    }
    coreLog.details(tr("resource '%1' is released by '%2'").arg(resourceName).arg(t->getTaskName()));
    emit si_resourceUserUnregistered(resourceName, t);
}


QList<Task*> ResourceTracker::getResourceUsers(const QString& resourceName) {
    QList<Task*> res = resMap.value(resourceName);
    return res;
}




} //namespace
