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

#ifndef _U2_DASHBOARD_H_
#define _U2_DASHBOARD_H_

#include <QDomElement>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Lang/WorkflowMonitor.h>

#include "DashboardTabPage.h"

namespace U2 {
using namespace Workflow;

class ExternalToolsDashboardWidget;
class ParametersDashboardWidget;
class NotificationsDashboardWidget;
class StatisticsDashboardWidget;
class StatusDashboardWidget;
class OutputFilesDashboardWidget;

class U2DESIGNER_EXPORT Dashboard : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(Dashboard)
public:
    Dashboard(const WorkflowMonitor* monitor, const QString& name, QWidget* parent);
    Dashboard(const QString& dirPath, QWidget* parent);

    void onShow();

    const QPointer<const WorkflowMonitor>& getMonitor() const;

    bool isOpened() const;
    void setClosed();

    const QString& directory() const;
    const QString& getDashboardId() const;

    const QString& getName() const;
    void setName(const QString& value);

    bool isWorkflowInProgress();

    static const QString REPORT_SUB_DIR;
    static const QString DB_FILE_NAME;
    static const QString SETTINGS_FILE_NAME;
    static const QString OPENED_SETTING;
    static const QString NAME_SETTING;

    static const QString STATE_RUNNING;
    static const QString STATE_RUNNING_WITH_PROBLEMS;
    static const QString STATE_FINISHED_WITH_PROBLEMS;
    static const QString STATE_FAILED;
    static const QString STATE_SUCCESS;
    static const QString STATE_CANCELED;

signals:
    void si_loadSchema(const QString& url);
    void si_hideLoadBtnHint();
    void si_workflowStateChanged(bool isRunning);
    void si_serializeContent(const QString& content);

public slots:
    void sl_loadSchema();

private slots:
    void sl_runStateChanged(bool paused);
    void sl_onLogChanged(Monitor::LogEntry logEntry);
    void sl_setDirectory(const QString& dir);
    void sl_workflowStateChanged(Monitor::TaskState state);

    /** Toggles tab button by id. */
    void sl_onTabButtonToggled(int id, bool checked);

private:
    void saveReportFile();

    /** Initializes layout with all widgets initialized with the given initial states. */
    void initLayout(const QMap<QString, QDomElement>& initialWidgetStates = QMap<QString, QDomElement>());
    void saveSettings();
    void loadSettings();
    static QMap<QString, QDomElement> readInitialWidgetStates(const QString& htmlUrl);

    void registerDashboard() const;
    void updateDashboard() const;
    void reserveName() const;
    void initExternalToolsTabWidget();

    QString name;
    QString dir;
    bool opened;
    const QPointer<const WorkflowMonitor> monitor;
    bool workflowInProgress;

    QVBoxLayout* mainLayout;

    QToolButton* overviewTabButton;
    QToolButton* inputTabButton;
    QToolButton* externalToolsTabButton;

    QStackedWidget* stackedWidget;

    DashboardTabPage* overviewTabPage;
    NotificationsDashboardWidget* notificationsWidget;
    StatisticsDashboardWidget* statisticsWidget;
    StatusDashboardWidget* statusWidget;
    OutputFilesDashboardWidget* outputFilesWidget;

    DashboardTabPage* inputTabPage;
    ParametersDashboardWidget* parametersWidget;

    DashboardTabPage* externalToolsTabPage;
    ExternalToolsDashboardWidget* externalToolsWidget;
    QDomElement externalToolsWidgetState;
};

}  // namespace U2

#endif  // _U2_DASHBOARD_H_
