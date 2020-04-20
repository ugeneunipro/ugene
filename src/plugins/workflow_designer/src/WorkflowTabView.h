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

#ifndef _U2_WORKFLOWTABVIEW_H_
#define _U2_WORKFLOWTABVIEW_H_

#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>

#include <U2Designer/Dashboard.h>
#include <U2Designer/DashboardInfo.h>

#include <U2Lang/WorkflowMonitor.h>

class QGraphicsView;

namespace U2 {
using namespace Workflow;

class Dashboard;
class WorkflowView;

class WorkflowTabView : public QTabWidget {
    Q_OBJECT
public:
    WorkflowTabView(WorkflowView *parent);

    void addDashboard(WorkflowMonitor *monitor, const QString &name = QString());
    bool hasDashboards() const;

    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void si_countChanged();
    void si_hideLoadBtnHint(); // should be common for all dashboards

private slots:
    void sl_closeTab();
    void sl_dashboardsListChanged(const QStringList &added, const QStringList &removed);
    void sl_dashboardsChanged(const QStringList &dashboardIds);
    void sl_renameTab();
    void sl_showDashboard(int idx);
    void sl_workflowStateChanged(bool isRunning);

private:
    int appendDashboard(Dashboard *db);
    void removeDashboard(Dashboard *db);
    QString generateName(const QString &baseName = "") const;
    QSet<QString> allNames() const;
    QStringList allIds() const;
    QMap<QString, Dashboard *> getDashboards(const QStringList &dashboardIds) const;

    WorkflowView *parent;
};

class RegistryConnectionBlocker {
public:
    RegistryConnectionBlocker(WorkflowTabView *tabView);
    ~RegistryConnectionBlocker();

    static void connectRegistry(WorkflowTabView *tabView);
    static void disconnectRegistry(WorkflowTabView *tabView);

private:
    WorkflowTabView *tabView;
    static int count;
};

} // U2

#endif // _U2_WORKFLOWTABVIEW_H_
