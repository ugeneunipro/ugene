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

#include <QGraphicsView>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QTabBar>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/SignalBlocker.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DashboardInfoRegistry.h>
#include <U2Designer/ScanDashboardsDirTask.h>

#include "WorkflowTabView.h"
#include "WorkflowViewController.h"

namespace U2 {

class CloseButton : public QPushButton {
public:
    CloseButton(QWidget *content)
        : QPushButton(QIcon(":workflow_designer/images/delete.png"), ""), _content(content)
    {
        setToolTip(WorkflowTabView::tr("Close dashboard"));
        setFlat(true);
        setFixedSize(16, 16);
    }

    QWidget * content() const {
        return _content;
    }

private:
    QWidget *_content;
};

WorkflowTabView::WorkflowTabView(WorkflowView *_parent)
    : QTabWidget(_parent),
      parent(_parent)
{
    setUsesScrollButtons(true);
    setTabPosition(QTabWidget::North);
    tabBar()->setShape(QTabBar::TriangularNorth);
    tabBar()->setMovable(true);
    { // it is needed for QTBUG-21808 and UGENE-2486
        QList<QToolButton*> scrollButtons = tabBar()->findChildren<QToolButton*>();
        foreach (QToolButton *b, scrollButtons) {
            b->setAutoFillBackground(true);
        }
    }

    setDocumentMode(true);
    connect(this, SIGNAL(currentChanged(int)), SLOT(sl_showDashboard(int)));

    tabBar()->installEventFilter(this);

    setObjectName("WorkflowTabView");
    sl_dashboardsListChanged(AppContext::getDashboardInfoRegistry()->getAllIds(), QStringList());
    RegistryConnectionBlocker::connectRegistry(this);
}

void WorkflowTabView::sl_showDashboard(int idx) {
    Dashboard *db = dynamic_cast<Dashboard*>(widget(idx));
    CHECK(NULL != db, );
    db->onShow();
}

void WorkflowTabView::sl_workflowStateChanged(bool isRunning) {
    QWidget *db = dynamic_cast<QWidget*>(sender());
    SAFE_POINT(NULL != db, "NULL dashboard", );
    int idx = indexOf(db);
    CHECK(-1 != idx, );
    CloseButton* closeButton = dynamic_cast<CloseButton*>(tabBar()->tabButton(idx, QTabBar::RightSide));
    SAFE_POINT(NULL != db, "NULL close button", );
    closeButton->setEnabled(!isRunning);
}

int WorkflowTabView::appendDashboard(Dashboard *db) {
    RegistryConnectionBlocker registryConnectionBlocker(this);
    Q_UNUSED(registryConnectionBlocker);

    if (db->getName().isEmpty()) {
        db->setName(generateName());
    }

    int idx = addTab(db, db->getName());

    CloseButton *closeButton = new CloseButton(db);
    tabBar()->setTabButton(idx, QTabBar::RightSide, closeButton);
    if (db->isWorkflowInProgress()) {
        closeButton->setEnabled(false);
        connect(db, SIGNAL(si_workflowStateChanged(bool)), SLOT(sl_workflowStateChanged(bool)));
    }
    connect(closeButton, SIGNAL(clicked()), SLOT(sl_closeTab()));
    connect(db, SIGNAL(si_loadSchema(const QString &)), parent, SLOT(sl_loadScene(const QString &)));
    connect(db, SIGNAL(si_hideLoadBtnHint()), this, SIGNAL(si_hideLoadBtnHint()));
    connect(this, SIGNAL(si_hideLoadBtnHint()), db, SLOT(sl_hideLoadBtnHint()));

    emit si_countChanged();
    return idx;
}

void WorkflowTabView::removeDashboard(Dashboard *dashboard) {
    CHECK(!dashboard->isWorkflowInProgress(), );
    removeTab(indexOf(dashboard));
    delete dashboard;
    emit si_countChanged();
}

void WorkflowTabView::addDashboard(WorkflowMonitor *monitor, const QString &baseName) {
    RegistryConnectionBlocker registryConnectionBlocker(this);
    Q_UNUSED(registryConnectionBlocker);

    QString name = generateName(baseName);
    int idx = appendDashboard(new Dashboard(monitor, name, this));
    setCurrentIndex(idx);
}

bool WorkflowTabView::hasDashboards() const {
    return count() > 0;
}

void WorkflowTabView::sl_closeTab() {
    RegistryConnectionBlocker registryConnectionBlocker(this);
    Q_UNUSED(registryConnectionBlocker);

    CloseButton *button = dynamic_cast<CloseButton*>(sender());
    SAFE_POINT(NULL != button, "NULL close button", );
    int idx = indexOf(button->content());
    Dashboard *db = dynamic_cast<Dashboard*>(widget(idx));
    db->setClosed();
    removeTab(idx);
    delete db;
    emit si_countChanged();
}

void WorkflowTabView::sl_renameTab() {
    RegistryConnectionBlocker registryConnectionBlocker(this);
    Q_UNUSED(registryConnectionBlocker);

    QAction *rename = dynamic_cast<QAction*>(sender());
    CHECK(NULL != rename, );
    int idx = rename->data().toInt();
    Dashboard *db = dynamic_cast<Dashboard*>(widget(idx));
    CHECK(NULL != db, );

    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename Dashboard"),
        tr("New dashboard name:"), QLineEdit::Normal,
        db->getName(), &ok);
    if (ok && !newName.isEmpty()) {
        db->setName(newName);
        setTabText(idx, newName);
    }
}

void WorkflowTabView::sl_dashboardsListChanged(const QStringList &added, const QStringList &removed) {
    RegistryConnectionBlocker registryConnectionBlocker(this);
    Q_UNUSED(registryConnectionBlocker);

    int countBeforeAdding = 0;
    {
        SignalBlocker signalBlocker(this);
        Q_UNUSED(signalBlocker);

        for (int i = count() - 1; i >= 0; --i) {
            Dashboard *dashboard = qobject_cast<Dashboard *>(widget(i));
            SAFE_POINT(nullptr != dashboard, "Can't cast QWidget to Dashboard", );
            const QString id = dashboard->getDashboardId();

            if (removed.contains(id)) {
                removeDashboard(dashboard);
            }
        }

        countBeforeAdding = count();

        DashboardInfoRegistry *dashboardInfoRegistry = AppContext::getDashboardInfoRegistry();
        const QStringList existingIds = allIds();
        foreach (const QString &dashboardId, added) {
            if (!existingIds.contains(dashboardId)) {
                const DashboardInfo dashboardInfo = dashboardInfoRegistry->getById(dashboardId);
                if (dashboardInfo.opened) {
                    Dashboard *dashboard = new Dashboard(dashboardInfo.path, this);
                    appendDashboard(dashboard);
                }
            }
        }
    }

    const int countAfterAdding = count();
    if (0 == countBeforeAdding && countAfterAdding > 0) {
        const int newIndex = countAfterAdding - 1;
        if (newIndex > 0) {
            setCurrentIndex(countAfterAdding - 1);
        } else {
            // emit the signal manually, because signals emitting was blocked during the dashboards adding
            emit currentChanged(newIndex);
        }
    }

    emit si_countChanged();
}

void WorkflowTabView::sl_dashboardsChanged(const QStringList &dashboardIds) {
    RegistryConnectionBlocker registryConnectionBlocker(this);
    Q_UNUSED(registryConnectionBlocker);

    QMap<QString, Dashboard *> dashboardsMap = getDashboards(dashboardIds);
    DashboardInfoRegistry *dashboardInfoRegistry = AppContext::getDashboardInfoRegistry();
    foreach (const QString &dashboardId, dashboardsMap.keys()) {
        const DashboardInfo dashboardInfo = dashboardInfoRegistry->getById(dashboardId);
        Dashboard *dashboard = dashboardsMap[dashboardId];
        if (nullptr == dashboard) {
            if (dashboardInfo.opened) {
                // Currently the dashboards that become visible are added to the end
                appendDashboard(new Dashboard(dashboardInfo.path, this));
            }
            continue;
        }

        if (!dashboardInfo.opened) {
            dashboard->setClosed();
            removeDashboard(dashboard);
        } else if (dashboardInfo.name != dashboard->getName()) {
            dashboard->setName(dashboardInfo.name);
        }
    }
}

QSet<QString> WorkflowTabView::allNames() const {
    QSet<QString> result;

    const QList<DashboardInfo> dashboardInfos = AppContext::getDashboardInfoRegistry()->getAllEntries();
    foreach (const DashboardInfo &dashboardInfo, dashboardInfos) {
        result << dashboardInfo.name;
    }

    result += AppContext::getDashboardInfoRegistry()->getReservedNames();

    return result;
}

QStringList WorkflowTabView::allIds() const {
    QStringList result;
    for (int i = 0; i < count(); i++) {
        Dashboard *db = qobject_cast<Dashboard *>(widget(i));
        result << db->getDashboardId();
    }
    return result;
}

QMap<QString, Dashboard *> WorkflowTabView::getDashboards(const QStringList &dashboardIds) const {
    QMap<QString, Dashboard *> result;
    for (int i = 0; i < count(); ++i) {
        Dashboard *dashboard = qobject_cast<Dashboard *>(widget(i));
        SAFE_POINT(nullptr != dashboard, "Can't cast QWidget to Dashboard", result);
        if (dashboardIds.contains(dashboard->getDashboardId())) {
            result.insert(dashboard->getDashboardId(), dashboard);
        }
    }

    if (result.size() != dashboardIds.size()) {
        const QSet<QString> difference = dashboardIds.toSet() - result.keys().toSet();
        foreach (const QString &dashboardId, difference) {
            result.insert(dashboardId, nullptr);
        }
    }

    return result;
}

QString WorkflowTabView::generateName(const QString &name) const {
    QString baseName = name;
    if (baseName.isEmpty()) {
        baseName = tr("Run");
    }

    QString result;
    QSet<QString> all = allNames();
    int num = 1;
    do {
        result = baseName + QString(" %1").arg(num);
        num++;
    } while (all.contains(result));
    return result;
}

bool WorkflowTabView::eventFilter(QObject *watched, QEvent *event) {
    CHECK(watched == tabBar(), false);
    CHECK(QEvent::MouseButtonRelease == event->type(), false);

    QMouseEvent *me = dynamic_cast<QMouseEvent*>(event);
    int idx = tabBar()->tabAt(me->pos());
    CHECK(idx >=0 && idx < count(), false);

    if (Qt::RightButton == me->button()) {
        QMenu m(tabBar());
        QAction *rename = new QAction(tr("Rename"), this);
        rename->setData(idx);
        connect(rename, SIGNAL(triggered()), SLOT(sl_renameTab()));
        m.addAction(rename);
        m.move(tabBar()->mapToGlobal(me->pos()));
        m.exec();
        return true;
    }

    if (me->button() == Qt::MiddleButton) {
        removeTab(idx);
        return true;
    }
    return false;
}

int RegistryConnectionBlocker::count = 0;

RegistryConnectionBlocker::RegistryConnectionBlocker(WorkflowTabView *_tabView)
    : tabView(_tabView)
{
    ++count;
    if (count == 1) {
        disconnectRegistry(tabView);
    }
}

RegistryConnectionBlocker::~RegistryConnectionBlocker() {
    --count;
    if (count == 0) {
        connectRegistry(tabView);
    }
}

void RegistryConnectionBlocker::connectRegistry(WorkflowTabView *tabView) {
    DashboardInfoRegistry *dashboardInfoRegistry = AppContext::getDashboardInfoRegistry();
    QObject::connect(dashboardInfoRegistry,
            SIGNAL(si_dashboardsListChanged(const QStringList &, const QStringList &)),
            tabView,
            SLOT(sl_dashboardsListChanged(const QStringList &, const QStringList &)));
    QObject::connect(dashboardInfoRegistry,
            SIGNAL(si_dashboardsChanged(const QStringList &)),
            tabView,
            SLOT(sl_dashboardsChanged(const QStringList &)));
}

void RegistryConnectionBlocker::disconnectRegistry(WorkflowTabView *tabView) {
    DashboardInfoRegistry *dashboardInfoRegistry = AppContext::getDashboardInfoRegistry();
    QObject::disconnect(dashboardInfoRegistry,
               SIGNAL(si_dashboardsListChanged(const QStringList &, const QStringList &)),
               tabView,
               SLOT(sl_dashboardsListChanged(const QStringList &, const QStringList &)));
    QObject::disconnect(dashboardInfoRegistry,
               SIGNAL(si_dashboardsChanged(const QStringList &)),
               tabView,
               SLOT(sl_dashboardsChanged(const QStringList &)));
}

} // U2
