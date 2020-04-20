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

#ifndef _U2_DASHBOARD_INFO_REGISTRY_H_
#define _U2_DASHBOARD_INFO_REGISTRY_H_

#include "DashboardInfo.h"
#include "ScanDashboardsDirTask.h"

namespace U2 {

class U2DESIGNER_EXPORT DashboardInfoRegistry : public QObject {
    Q_OBJECT
public:
    bool registerEntry(const DashboardInfo &dashboardInfo);
    bool unregisterEntry(const QString &id);

    DashboardInfo getById(const QString &dashboardId) const;
    QStringList getAllIds() const;
    QList<DashboardInfo> getAllEntries() const;

    bool isEmpty() const;

    void scanDashboardsDir();
    void removeDashboards(const QStringList &ids);

    void updateDashboardInfo(const DashboardInfo &newDashboardInfo);
    void updateDashboardInfos(const QList<DashboardInfo> &newDashboardInfos);

    void reserveName(const QString &dashboardId, const QString &name);
    void releaseReservedName(const QString &dashboardId);
    QSet<QString> getReservedNames() const;

private slots:
    void sl_scanTaskFinished();

signals:
    void si_scanningStarted();
    void si_scanningFinished();

    void si_dashboardsListChanged(const QStringList &added, const QStringList &removed);
    void si_dashboardsChanged(const QStringList &ids);

private:
    bool registerEntrySilently(const DashboardInfo &dashboardInfo);
    bool unregisterEntrySilently(const QString &id);
    bool updateInfo(const DashboardInfo &newDashboardInfo);

    QPointer<ScanDashboardsDirTask> scanTask;

    QMap<QString, DashboardInfo> registry;
    QMap<QString, QString> reservedNames;   // dashboards for running workflows are not registered, but they can reserve their name for the correct name rolling
};

}   // namespace U2

#endif // _U2_DASHBOARD_INFO_REGISTRY_H_
