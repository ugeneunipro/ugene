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

#ifndef _U2_DASHBOARD_H_
#define _U2_DASHBOARD_H_

#include <U2Gui/U2WebView.h>

#include <U2Lang/WorkflowMonitor.h>

namespace U2 {
using namespace Workflow;

class DashboardJsAgent;
class DashboardPageController;

class U2DESIGNER_EXPORT Dashboard : public U2WebView {
    Q_OBJECT
    Q_DISABLE_COPY(Dashboard)
public:
    Dashboard(const WorkflowMonitor *monitor, const QString &name, QWidget *parent);
    Dashboard(const QString &dirPath, QWidget *parent);

    void onShow();

    const QPointer<const WorkflowMonitor> &getMonitor() const;

    void setClosed();
    const QString &directory() const;
    const QString &getDashboardId() const;

    const QString &getName() const;
    void setName(const QString &value);

    QString getPageFilePath() const;

    void loadSchema();

    /** Modifies the application settings and emits signal for all dashboards */
    void initiateHideLoadButtonHint();

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
    void si_loadSchema(const QString &url);
    void si_hideLoadBtnHint();
    void si_workflowStateChanged(bool isRunning);
    void si_serializeContent(const QString& content);

public slots:
    /** Hides the hint on the current dashboard instance */
    void sl_hideLoadBtnHint();

private slots:
    void sl_runStateChanged(bool paused);
    void sl_pageReady();
    void sl_serialize();
    void sl_setDirectory(const QString &dir);
    void sl_workflowStateChanged(Monitor::TaskState state);

private:
    void loadDocument();
    void saveSettings();
    void loadSettings();

    void registerDashboard() const;
    void updateDashboard() const;
    void reserveName() const;

    bool loadingStarted;
    QString loadUrl;
    QString name;
    QString dir;
    bool opened;
    const QPointer<const WorkflowMonitor> monitor;
    bool workflowInProgress;
    DashboardPageController *dashboardPageController;
};

}   // namespace U2

#endif // _U2_DASHBOARD_H_
