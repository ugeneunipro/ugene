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

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Designer/DashboardInfoRegistry.h>

#include <U2Gui/MainWindow.h>

#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "Dashboard.h"
#include "DashboardPageController.h"

namespace U2 {

const QString Dashboard::REPORT_SUB_DIR = "report/";
const QString Dashboard::DB_FILE_NAME = "dashboard.html";
const QString Dashboard::SETTINGS_FILE_NAME = "settings.ini";
const QString Dashboard::OPENED_SETTING = "opened";
const QString Dashboard::NAME_SETTING = "name";

const QString Dashboard::STATE_RUNNING = "RUNNING";
const QString Dashboard::STATE_RUNNING_WITH_PROBLEMS = "RUNNING_WITH_PROBLEMS";
const QString Dashboard::STATE_FINISHED_WITH_PROBLEMS = "FINISHED_WITH_PROBLEMS";
const QString Dashboard::STATE_FAILED = "FAILED";
const QString Dashboard::STATE_SUCCESS = "SUCCESS";
const QString Dashboard::STATE_CANCELED = "CANCELED";

/************************************************************************/
/* Dashboard */
/************************************************************************/
Dashboard::Dashboard(const WorkflowMonitor *monitor, const QString &name, QWidget *parent)
    : U2WebView(parent),
      loadingStarted(false),
      loadUrl("qrc:///U2Designer/html/Dashboard.html"),
      name(name),
      opened(true),
      monitor(monitor),
      workflowInProgress(true),
      dashboardPageController(new DashboardPageController(this))
{
    connect(monitor, SIGNAL(si_report()), SLOT(sl_serialize()));
    connect(monitor, SIGNAL(si_dirSet(const QString &)), SLOT(sl_setDirectory(const QString &)));
    connect(monitor, SIGNAL(si_taskStateChanged(Monitor::TaskState)), SLOT(sl_workflowStateChanged(Monitor::TaskState)));

    connect(dashboardPageController, SIGNAL(si_pageReady()), SLOT(sl_serialize()));
    connect(dashboardPageController, SIGNAL(si_pageReady()), SLOT(sl_pageReady()));

    setContextMenuPolicy(Qt::NoContextMenu);
    loadDocument();
    setObjectName("Dashboard");
}

Dashboard::Dashboard(const QString &dirPath, QWidget *parent)
    : U2WebView(parent),
      loadingStarted(false),
      loadUrl(QUrl::fromLocalFile(dirPath + REPORT_SUB_DIR + DB_FILE_NAME).toString()),
      dir(dirPath),
      opened(true),
      monitor(NULL),
      workflowInProgress(false),
      dashboardPageController(new DashboardPageController(this))
{
    setContextMenuPolicy(Qt::NoContextMenu);
    loadSettings();
    saveSettings();

    connect(dashboardPageController, SIGNAL(si_pageReady()), SLOT(sl_pageReady()));
    setObjectName("Dashboard");
}

void Dashboard::onShow() {
    CHECK(!loadingStarted, );
    loadDocument();
}

const QPointer<const WorkflowMonitor> &Dashboard::getMonitor() const {
    return monitor;
}

void Dashboard::setClosed() {
    opened = false;
    saveSettings();
    updateDashboard();
}

const QString &Dashboard::directory() const {
    return dir;
}

const QString &Dashboard::getDashboardId() const {
    return dir;
}

const QString &Dashboard::getName() const {
    return name;
}

void Dashboard::setName(const QString &value) {
    name = value;
    saveSettings();
    updateDashboard();
}

QString Dashboard::getPageFilePath() const {
    return dir + REPORT_SUB_DIR + DB_FILE_NAME;
}

void Dashboard::loadSchema() {
    const QString url = dir + REPORT_SUB_DIR + WorkflowMonitor::WORKFLOW_FILE_NAME;
    emit si_loadSchema(url);
}

void Dashboard::initiateHideLoadButtonHint() {
    WorkflowSettings::setShowLoadButtonHint(false);
    emit si_hideLoadBtnHint();
}

bool Dashboard::isWorkflowInProgress() {
    return workflowInProgress;
}

void Dashboard::sl_hideLoadBtnHint() {
    dashboardPageController->runJavaScript("hideLoadBtnHint()");
}

void Dashboard::sl_runStateChanged(bool paused) {
    QString script = paused ? "pauseTimer()" : "startTimer()";
    dashboardPageController->runJavaScript(script);
}

void Dashboard::sl_pageReady() {
    if (NULL != getMonitor()) {
        connect(getMonitor(), SIGNAL(si_runStateChanged(bool)), SLOT(sl_runStateChanged(bool)));
    }

    if (!WorkflowSettings::isShowLoadButtonHint()) {
        dashboardPageController->runJavaScript("hideLoadBtnHint()");
    }
}

void Dashboard::sl_serialize() {
    CHECK(dashboardPageController->isPageReady(), );
    QCoreApplication::processEvents();
    QString reportDir = dir + REPORT_SUB_DIR;
    QDir d(reportDir);
    if (!d.exists(reportDir)) {
        bool created = d.mkpath(reportDir);
        CHECK_EXT(created, ioLog.error(tr("Can not create a folder: ") + reportDir), );
    }
    dashboardPageController->savePage(getPageFilePath());
    saveSettings();
}

void Dashboard::sl_setDirectory(const QString &value) {
    dir = value;
    saveSettings();
    reserveName();
}

void Dashboard::sl_workflowStateChanged(Monitor::TaskState state) {
    workflowInProgress = (state == Monitor::RUNNING) || (state == Monitor::RUNNING_WITH_PROBLEMS);
    if (!workflowInProgress) {
        emit si_workflowStateChanged(workflowInProgress);
        registerDashboard();
        AppContext::getDashboardInfoRegistry()->releaseReservedName(getDashboardId());
    }
}

void Dashboard::loadDocument() {
    loadingStarted = true;
    dashboardPageController->loadPage(loadUrl);
}

void Dashboard::saveSettings() {
    QSettings settings(dir + REPORT_SUB_DIR + SETTINGS_FILE_NAME, QSettings::IniFormat);
    settings.setValue(OPENED_SETTING, opened);
    settings.setValue(NAME_SETTING, name);
    settings.sync();
}

void Dashboard::loadSettings() {
    QSettings settings(dir + REPORT_SUB_DIR + SETTINGS_FILE_NAME, QSettings::IniFormat);
    opened = true;
    name = settings.value(NAME_SETTING).toString();
}

void Dashboard::registerDashboard() const {
    DashboardInfo dashboardInfo(directory());
    dashboardInfo.name = name;
    const bool registered = AppContext::getDashboardInfoRegistry()->registerEntry(dashboardInfo);
    Q_ASSERT(registered);
    Q_UNUSED(registered);
}

void Dashboard::updateDashboard() const {
    DashboardInfo info(dir, opened);
    info.name = name;
    AppContext::getDashboardInfoRegistry()->updateDashboardInfo(info);
}

void Dashboard::reserveName() const {
    AppContext::getDashboardInfoRegistry()->reserveName(getDashboardId(), name);
}

}   // namespace U2
