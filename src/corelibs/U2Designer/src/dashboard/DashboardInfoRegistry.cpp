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

#include <QSet>

#include <U2Core/AppContext.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include "DashboardInfoRegistry.h"
#include "RemoveDashboardsTask.h"

namespace U2 {

bool DashboardInfoRegistry::registerEntry(const DashboardInfo &dashboardInfo) {
    if (registerEntrySilently(dashboardInfo)) {
        emit si_dashboardsListChanged(QStringList(dashboardInfo.getId()), QStringList());
        return true;
    } else {
        return false;
    }
}

bool DashboardInfoRegistry::unregisterEntry(const QString &id) {
    if (unregisterEntrySilently(id)) {
        emit si_dashboardsListChanged(QStringList(), QStringList(id));
        return true;
    } else {
        return false;
    }
}

DashboardInfo DashboardInfoRegistry::getById(const QString &dashboardId) const {
    return registry.value(dashboardId, DashboardInfo());
}

QStringList DashboardInfoRegistry::getAllIds() const {
    return registry.keys();
}

QList<DashboardInfo> DashboardInfoRegistry::getAllEntries() const {
    return registry.values();
}

bool DashboardInfoRegistry::isEmpty() const {
    return registry.isEmpty();
}

void DashboardInfoRegistry::scanDashboardsDir() {
    if (nullptr != scanTask && !scanTask->isFinished()) {
        scanTask->cancel();
    }
    scanTask = new ScanDashboardsDirTask();
    connect(new TaskSignalMapper(scanTask.data()), SIGNAL(si_taskSucceeded(Task *)), SLOT(sl_scanTaskFinished()));
    AppContext::getTaskScheduler()->registerTopLevelTask(scanTask);
    emit si_scanningStarted();
}

void DashboardInfoRegistry::removeDashboards(const QStringList &ids) {
    QList<DashboardInfo> dashboardInfos;
    foreach (const QString &id, ids) {
        if (registry.contains(id)) {
            dashboardInfos << registry[id];
            unregisterEntrySilently(id);
        }
    }
    RemoveDashboardsTask *removeTask = new RemoveDashboardsTask(dashboardInfos);
    AppContext::getTaskScheduler()->registerTopLevelTask(removeTask);

    emit si_dashboardsListChanged(QStringList(), ids);
}

void DashboardInfoRegistry::updateDashboardInfo(const DashboardInfo &newDashboardInfo) {
    if (updateInfo(newDashboardInfo)) {
        emit si_dashboardsChanged(QStringList(newDashboardInfo.getId()));
    }
}

void DashboardInfoRegistry::updateDashboardInfos(const QList<DashboardInfo> &newDashboardInfos) {
    QStringList updated;
    foreach (const DashboardInfo &newDashboardInfo, newDashboardInfos) {
        if (updateInfo(newDashboardInfo)) {
            updated << newDashboardInfo.getId();
        }
    }
    emit si_dashboardsChanged(updated);
}

void DashboardInfoRegistry::reserveName(const QString &dashboardId, const QString &name) {
    reservedNames.insert(dashboardId, name);
}

void DashboardInfoRegistry::releaseReservedName(const QString &dashboardId) {
    reservedNames.remove(dashboardId);
}

QSet<QString> DashboardInfoRegistry::getReservedNames() const {
    return reservedNames.values().toSet();
}

void DashboardInfoRegistry::sl_scanTaskFinished() {
    QStringList added;
    QStringList removed;
    const QList<DashboardInfo> foundInfos = scanTask->getResult();
    QList<DashboardInfo> registryValues = registry.values();

    foreach (const DashboardInfo &registryValue, registryValues) {
        if (!foundInfos.contains(registryValue)) {
            removed << registryValue.getId();
            unregisterEntrySilently(registryValue.getId());
        }
    }

    registryValues = registry.values();
    foreach (const DashboardInfo &foundInfo, foundInfos) {
        if (!registryValues.contains(foundInfo)) {
            if (registerEntrySilently(foundInfo)) {
                added << foundInfo.getId();
            } else {
                coreLog.error(tr("Can't register a dashboard info: '%1'").arg(foundInfo.getId()));
            }
        }
    }

    emit si_dashboardsListChanged(added, removed);
    emit si_scanningFinished();
}

bool DashboardInfoRegistry::registerEntrySilently(const DashboardInfo &dashboardInfo) {
    CHECK(!registry.contains(dashboardInfo.getId()), false);
    registry.insert(dashboardInfo.getId(), dashboardInfo);
    return true;
}

bool DashboardInfoRegistry::unregisterEntrySilently(const QString &id) {
    CHECK(registry.contains(id), false);
    registry.remove(id);
    return true;
}

bool DashboardInfoRegistry::updateInfo(const DashboardInfo &newDashboardInfo) {
    // DashboardInfo can be absent in the registry in case of workflow output directory changing.
    // If the workflow is running during the changing, the dashboard won't be removed, but dashboardInfo will be unregistered.
    CHECK(registry.contains(newDashboardInfo.getId()), false);
    registry[newDashboardInfo.getId()] = newDashboardInfo;
    return true;
}

}   // namespace U2
