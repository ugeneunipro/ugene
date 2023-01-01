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

#include "Dashboard.h"

#include <QApplication>
#include <QButtonGroup>
#include <QClipboard>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Designer/DashboardInfoRegistry.h>

#include <U2Gui/MainWindow.h>

#include <U2Lang/WorkflowUtils.h>

#include "DashboardTabPage.h"
#include "DomUtils.h"
#include "ExternalToolsDashboardWidget.h"
#include "NotificationsDashboardWidget.h"
#include "OutputFilesDashboardWidget.h"
#include "ParametersDashboardWidget.h"
#include "StatisticsDashboardWidget.h"
#include "StatusDashboardWidget.h"

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

#define OVERVIEW_TAB_INDEX 0
#define INPUT_TAB_INDEX 1
#define EXTERNAL_TOOLS_TAB_INDEX 2

#define EXTERNAL_TOOLS_WIDGET_STATE_KEY "external_tools_widget"
#define PARAMETERS_WIDGET_STATE_KEY "parameters_widget"
#define NOTIFICATIONS_WIDGET_STATE_KEY "notifications_widget"
#define STATISTICS_WIDGET_STATE_KEY "statistics_widget"
#define STATUS_WIDGET_STATE_KEY "status_widget"
#define OUTPUT_FILES_WIDGET_STATE_KEY "output_files_widget"

/************************************************************************/
/* Dashboard */
/************************************************************************/
Dashboard::Dashboard(const WorkflowMonitor* monitor, const QString& name, QWidget* parent)
    : QWidget(parent), name(name), opened(true),
      monitor(monitor), workflowInProgress(true),
      mainLayout(nullptr), stackedWidget(nullptr), overviewTabPage(nullptr), notificationsWidget(nullptr),
      statisticsWidget(nullptr), statusWidget(nullptr), outputFilesWidget(nullptr), inputTabPage(nullptr),
      parametersWidget(nullptr), externalToolsTabPage(nullptr), externalToolsWidget(nullptr) {
    setObjectName("Dashboard");
    setContextMenuPolicy(Qt::NoContextMenu);
    initLayout();
    connect(monitor, SIGNAL(si_dirSet(const QString&)), SLOT(sl_setDirectory(const QString&)));
    connect(monitor, SIGNAL(si_taskStateChanged(Monitor::TaskState)), SLOT(sl_workflowStateChanged(Monitor::TaskState)));
    connect(monitor, SIGNAL(si_logChanged(Monitor::LogEntry)), SLOT(sl_onLogChanged(Monitor::LogEntry)));
    connect(getMonitor(), SIGNAL(si_runStateChanged(bool)), SLOT(sl_runStateChanged(bool)));
}

Dashboard::Dashboard(const QString& dirPath, QWidget* parent)
    : QWidget(parent),
      dir(dirPath), opened(true), monitor(nullptr), workflowInProgress(false),
      mainLayout(nullptr), stackedWidget(nullptr), overviewTabPage(nullptr), notificationsWidget(nullptr),
      statisticsWidget(nullptr), statusWidget(nullptr), outputFilesWidget(nullptr), inputTabPage(nullptr),
      parametersWidget(nullptr), externalToolsTabPage(nullptr), externalToolsWidget(nullptr) {
    setObjectName("Dashboard");
    setContextMenuPolicy(Qt::NoContextMenu);
    loadSettings();
    saveSettings();
}

void Dashboard::initLayout(const QMap<QString, QDomElement>& initialWidgetStates) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    auto tabButtonsRow = new QWidget(this);
    mainLayout->addWidget(tabButtonsRow);

    tabButtonsRow->setObjectName("tabButtonsRow");
    tabButtonsRow->setStyleSheet(
        "#tabButtonsRow {background: url(':U2Designer/images/background-menu.png') repeat scroll 0 0 transparent;}");

    auto tabButtonsLayout = new QHBoxLayout(tabButtonsRow);
    tabButtonsLayout->setMargin(5);
    tabButtonsLayout->addSpacing(20);

    QString tabButtonStyleSheet = "QToolButton {"
                                  "  color: white;"
                                  "  border-radius: 6px;"
                                  "  padding: 4px;"
                                  "}\n"
                                  "QToolButton:checked {"
                                  "  color: white;"
                                  "  background: url(':U2Designer/images/background-menu-button.png') repeat scroll 0 0 transparent;"
                                  "}"
                                  "QToolButton:hover:!checked {"
                                  "  color: #005580;"
                                  "  background: white;"
                                  "}\n";
    setObjectName("dashboardWidget");

    overviewTabButton = new QToolButton(tabButtonsRow);
    overviewTabButton->setText(tr("Overview"));
    overviewTabButton->setObjectName("overviewTabButton");
    overviewTabButton->setStyleSheet(tabButtonStyleSheet);
    overviewTabButton->setCursor(Qt::PointingHandCursor);
    overviewTabButton->setCheckable(true);
    overviewTabButton->setChecked(true);
    tabButtonsLayout->addWidget(overviewTabButton);

    inputTabButton = new QToolButton(tabButtonsRow);
    inputTabButton->setText(tr("Input"));
    inputTabButton->setObjectName("inputTabButton");
    inputTabButton->setStyleSheet(tabButtonStyleSheet);
    inputTabButton->setCursor(Qt::PointingHandCursor);
    inputTabButton->setCheckable(true);
    tabButtonsLayout->addWidget(inputTabButton);

    externalToolsTabButton = new QToolButton(tabButtonsRow);
    externalToolsTabButton->setText(tr("External Tools"));
    externalToolsTabButton->setObjectName("externalToolsTabButton");
    externalToolsTabButton->setStyleSheet(tabButtonStyleSheet);
    externalToolsTabButton->setCursor(Qt::PointingHandCursor);
    externalToolsTabButton->setCheckable(true);
    tabButtonsLayout->addWidget(externalToolsTabButton);

    auto tabButtonGroup = new QButtonGroup(tabButtonsRow);
    tabButtonGroup->setExclusive(true);
    tabButtonGroup->addButton(overviewTabButton, OVERVIEW_TAB_INDEX);
    tabButtonGroup->addButton(inputTabButton, INPUT_TAB_INDEX);
    tabButtonGroup->addButton(externalToolsTabButton, EXTERNAL_TOOLS_TAB_INDEX);
    connect(tabButtonGroup, SIGNAL(buttonToggled(int, bool)), SLOT(sl_onTabButtonToggled(int, bool)));

    tabButtonsLayout->addStretch(INT_MAX);  // Push the last button to the end.

    auto loadSchemaButton = new QToolButton(tabButtonsRow);
    loadSchemaButton->setIcon(QIcon(":U2Designer/images/load_schema.png"));
    loadSchemaButton->setObjectName("loadSchemaButton");
    loadSchemaButton->setToolTip(tr("Open workflow schema"));
    //    loadSchemaButton->setText(tr("Open schema"));
    //    loadSchemaButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    loadSchemaButton->setStyleSheet("padding: 4px 6px;");
    loadSchemaButton->setCursor(Qt::PointingHandCursor);
    connect(loadSchemaButton, SIGNAL(clicked()), SLOT(sl_loadSchema()));

    tabButtonsLayout->addWidget(loadSchemaButton);
    tabButtonsLayout->addSpacing(20);

    stackedWidget = new QStackedWidget();
    mainLayout->addWidget(stackedWidget, INT_MAX);

    // Overview tab.
    overviewTabPage = new DashboardTabPage("overview_tab_page");
    stackedWidget->addWidget(overviewTabPage);

    QDomElement outputFilesWidgetState = initialWidgetStates.value(OUTPUT_FILES_WIDGET_STATE_KEY);
    outputFilesWidget = new OutputFilesDashboardWidget(dir, outputFilesWidgetState, monitor);
    overviewTabPage->addDashboardWidget(tr("Output files"), outputFilesWidget);

    QDomElement statusWidgetState = initialWidgetStates.value(STATUS_WIDGET_STATE_KEY);
    statusWidget = new StatusDashboardWidget(statusWidgetState, monitor);
    overviewTabPage->addDashboardWidget(tr("Workflow task"), statusWidget);

    QDomElement notificationsWidgetState = initialWidgetStates.value(NOTIFICATIONS_WIDGET_STATE_KEY);
    notificationsWidget = new NotificationsDashboardWidget(notificationsWidgetState, monitor);
    auto notificationsDashboardWidget = overviewTabPage->addDashboardWidget(tr("Notifications"), notificationsWidget);
    notificationsWidget->setDashboardWidget(notificationsDashboardWidget);

    QDomElement statisticsWidgetState = initialWidgetStates.value(STATISTICS_WIDGET_STATE_KEY);
    statisticsWidget = new StatisticsDashboardWidget(statisticsWidgetState, monitor);
    overviewTabPage->addDashboardWidget(tr("Common statistics"), statisticsWidget);

    // Input tab.
    inputTabPage = new DashboardTabPage("input_tab_page");
    stackedWidget->addWidget(inputTabPage);

    QDomElement parametersWidgetState = initialWidgetStates.value(PARAMETERS_WIDGET_STATE_KEY);
    parametersWidget = new ParametersDashboardWidget(dir, parametersWidgetState, monitor);
    inputTabPage->addDashboardWidget(tr("Parameters"), parametersWidget);

    // External tools tab.
    externalToolsTabPage = new DashboardTabPage("external_tools_tab_page");
    stackedWidget->addWidget(externalToolsTabPage);

    externalToolsWidgetState = initialWidgetStates.value(EXTERNAL_TOOLS_WIDGET_STATE_KEY);
    externalToolsTabButton->setVisible(!externalToolsWidgetState.isNull());

    stackedWidget->setCurrentIndex(0);
}

void Dashboard::initExternalToolsTabWidget() {
    CHECK(externalToolsWidget == nullptr && mainLayout != nullptr, );
    externalToolsWidget = new ExternalToolsDashboardWidget(externalToolsWidgetState, monitor);
    externalToolsTabPage->addDashboardWidget(tr("External Tools"), externalToolsWidget);
    externalToolsTabButton->setVisible(true);
}

void Dashboard::sl_onTabButtonToggled(int id, bool checked) {
    if (!checked || mainLayout == nullptr) {
        return;
    }
    switch (id) {
        case OVERVIEW_TAB_INDEX:
            stackedWidget->setCurrentIndex(0);
            break;
        case INPUT_TAB_INDEX:
            stackedWidget->setCurrentIndex(1);
            break;
        case EXTERNAL_TOOLS_TAB_INDEX:
            initExternalToolsTabWidget();
            stackedWidget->setCurrentIndex(2);
            break;
    }
}

void Dashboard::onShow() {
    if (mainLayout == nullptr) {
        initLayout(readInitialWidgetStates(dir + REPORT_SUB_DIR + DB_FILE_NAME));
    }
}

const QPointer<const WorkflowMonitor>& Dashboard::getMonitor() const {
    return monitor;
}

bool Dashboard::isOpened() const {
    return opened;
}

void Dashboard::setClosed() {
    opened = false;
    saveSettings();
    updateDashboard();
}

const QString& Dashboard::directory() const {
    return dir;
}

const QString& Dashboard::getDashboardId() const {
    return dir;
}

const QString& Dashboard::getName() const {
    return name;
}

void Dashboard::setName(const QString& value) {
    name = value;
    saveSettings();
    updateDashboard();
}

void Dashboard::sl_loadSchema() {
    QString url = dir + REPORT_SUB_DIR + WorkflowMonitor::WORKFLOW_FILE_NAME;
    emit si_loadSchema(url);
}

bool Dashboard::isWorkflowInProgress() {
    return workflowInProgress;
}

void Dashboard::sl_runStateChanged(bool paused) {
    CHECK(mainLayout != nullptr, );
    if (paused) {
        statusWidget->stopTimer();
    } else {
        statusWidget->startTimer();
    }
}

void Dashboard::sl_onLogChanged(Monitor::LogEntry logEntry) {
    CHECK(mainLayout != nullptr, );
    initExternalToolsTabWidget();
    externalToolsWidget->addLogEntry(logEntry);
}

void Dashboard::saveReportFile() {
    CHECK(mainLayout != nullptr, );
    QString reportDir = dir + REPORT_SUB_DIR;
    QDir d(reportDir);
    if (!d.exists(reportDir)) {
        bool created = d.mkpath(reportDir);
        CHECK_EXT(created, ioLog.error(tr("Can not create a folder: ") + reportDir), );
    }
    saveSettings();

    QFile file(":U2Designer/html/Dashboard.html");
    if (!file.open(QIODevice::ReadOnly)) {
        coreLog.error(tr("Failed to open Dashboard.html"));
        return;
    };
    QString html = QString::fromUtf8(file.readAll());

    // Output
    html.replace("<div class=\"tab-pane active\" id=\"overview_tab\">",
                 "<div class=\"tab-pane active\" id=\"overview_tab\">\n" + outputFilesWidget->toHtml() + "\n");

    // Notifications
    if (notificationsWidget->isVisible()) {
        html.replace("<div class=\"tab-pane active\" id=\"overview_tab\">",
                     "<div class=\"tab-pane active\" id=\"overview_tab\">\n" + notificationsWidget->toHtml() + "\n");
    }

    // Status
    html.replace("<div class=\"tab-pane active\" id=\"overview_tab\">",
                 "<div class=\"tab-pane active\" id=\"overview_tab\">\n" + statusWidget->toHtml() + "\n");

    // Statistics
    html.replace("<div class=\"tab-pane active\" id=\"overview_tab\">",
                 "<div class=\"tab-pane active\" id=\"overview_tab\">\n" + statisticsWidget->toHtml() + "\n");

    // Input parameters
    html.replace("<div class=\"widget-content\" id=\"parametersWidget\"></div>",
                 "<div class=\"widget-content\" id=\"parametersWidget\">\n" + parametersWidget->toHtml() + "</div>\n");

    // External tools
    if (externalToolsWidget != nullptr) {
        html.replace("<div class=\"widget-content\" id=\"externalToolsWidget\"></div>",
                     "<div class=\"widget-content\" id=\"externalToolsWidget\">\n" + externalToolsWidget->toHtml() + "</div>\n");
    }
    IOAdapterUtils::writeTextFile(dir + REPORT_SUB_DIR + DB_FILE_NAME, html);
}

void Dashboard::sl_setDirectory(const QString& value) {
    dir = value;
    saveSettings();
    reserveName();
}

void Dashboard::sl_workflowStateChanged(Monitor::TaskState state) {
    workflowInProgress = (state == Monitor::RUNNING) || (state == Monitor::RUNNING_WITH_PROBLEMS);
    if (workflowInProgress) {
        return;
    }
    emit si_workflowStateChanged(workflowInProgress);
    saveReportFile();
    registerDashboard();
    AppContext::getDashboardInfoRegistry()->releaseReservedName(getDashboardId());
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
    AppContext::getDashboardInfoRegistry()->registerEntry(dashboardInfo);
}

void Dashboard::updateDashboard() const {
    DashboardInfo info(dir, opened);
    info.name = name;
    AppContext::getDashboardInfoRegistry()->updateDashboardInfo(info);
}

void Dashboard::reserveName() const {
    AppContext::getDashboardInfoRegistry()->reserveName(getDashboardId(), name);
}

static void trimToWrapper(QString& html) {
    int startIdx = html.indexOf("<div id=\"wrapper\">");
    int endIdx = html.indexOf("<div id=\"log_messages\"", startIdx);
    if (startIdx >= 0 && endIdx >= 0) {
        html.remove(endIdx, html.length() - endIdx);
        html.remove(0, startIdx);
    }
}

static void fixImages(QString& html) {
    int startIdx = 0;
    while (true) {
        startIdx = html.indexOf("<img src=", startIdx);
        int endIdx = html.indexOf(">", startIdx);
        if (startIdx < 0 || endIdx < 0) {
            break;
        }
        html.insert(endIdx, '/');
        startIdx = endIdx;
    };
}

static void removeExtraDiv(QString& html) {
    // UGENE 34 and below may have unbalanced divs count.
    int openingDivCount = html.count("<div");
    int closingDivCount = html.count("</div>");
    if (openingDivCount + 1 == closingDivCount) {
        int lastClosingDivIndex = html.lastIndexOf("</div>");
        html.remove(lastClosingDivIndex, 6);
    }
}

static void removeHtml(QString& html, const QByteArray& tag) {
    while (true) {
        int startIdx = html.indexOf("<" + tag + ">");
        if (startIdx == -1) {
            break;
        }
        int endIdx = html.indexOf("</" + tag + ">", startIdx);
        if (endIdx == -1) {
            break;
        }
        html.remove(startIdx, endIdx - startIdx + tag.length() + 3);
    }
}

/** In-place fixes old-style UGENE's HTML to be parsable by the QXml. */
static void makeValidDomFromHtml(QString& htmlData) {
    trimToWrapper(htmlData);
    removeHtml(htmlData, "colgroup");
    fixImages(htmlData);
    removeExtraDiv(htmlData);
    htmlData.replace("<br>", "<br/>");
    htmlData.replace("<wbr>", "<wbr/>");
}

static void removeProblemsWidgetDom(QString& htmlData) {
    QString startToken = "<tbody scroll=\"yes\" id=\"problemsWidget123\">";
    QString endToken = "</tbody>";
    int startIndex = htmlData.indexOf(startToken);
    int endIndex = startIndex > 0 ? htmlData.indexOf(endToken, startIndex + startToken.length()) : -1;
    if (startIndex > 0 && endIndex > 0) {
        htmlData.remove(startIndex, endIndex + endToken.length() - startIndex);
    }
}

QMap<QString, QDomElement> Dashboard::readInitialWidgetStates(const QString& htmlUrl) {
    QMap<QString, QDomElement> map;
    QString html = IOAdapterUtils::readTextFile(htmlUrl);
    if (html.isNull()) {
        coreLog.error(tr("Error reading dashboard file: %1").arg(htmlUrl));
        return map;
    }
    makeValidDomFromHtml(html);

    QString error;
    QDomDocument doc = DomUtils::fromString(html, error);
    if (!error.isEmpty()) {
        // There is an known issue with old UGENE version (<35) with illegal ProblemsWidget DOM: inner-ids were not escaped. Remove this widget and try to parse again.
        removeProblemsWidgetDom(html);
        doc = DomUtils::fromString(html, error);
        if (!error.isEmpty()) {
            coreLog.error(tr("Error parsing dashboard file: '%1', file: %2").arg(error).arg(htmlUrl));
            return map;
        }
    }

    QDomElement rootEl = doc.documentElement();

    QDomElement statisticsWidgetEl = DomUtils::findElementById(rootEl, "statisticsWidget");
    map[STATISTICS_WIDGET_STATE_KEY] = StatisticsDashboardWidget::isValidDom(statisticsWidgetEl) ? statisticsWidgetEl : QDomElement();

    QDomElement notificationsWidgetEl = DomUtils::findElementById(rootEl, "problemsWidget");
    map[NOTIFICATIONS_WIDGET_STATE_KEY] = NotificationsDashboardWidget::isValidDom(notificationsWidgetEl) ? notificationsWidgetEl : QDomElement();

    QDomElement statusWidgetEl = DomUtils::findElementById(rootEl, "statusWidget");
    map[STATUS_WIDGET_STATE_KEY] = StatusDashboardWidget::isValidDom(statusWidgetEl) ? statusWidgetEl : QDomElement();

    QDomElement outputFilesWidgetEl = DomUtils::findElementById(rootEl, "outputWidget");
    map[OUTPUT_FILES_WIDGET_STATE_KEY] = OutputFilesDashboardWidget::isValidDom(outputFilesWidgetEl) ? outputFilesWidgetEl : QDomElement();

    QDomElement parametersWidgetEl = DomUtils::findElementById(rootEl, "parametersWidget");
    map[PARAMETERS_WIDGET_STATE_KEY] = ParametersDashboardWidget::isValidDom(parametersWidgetEl) ? parametersWidgetEl : QDomElement();

    QDomElement externalToolsEl = DomUtils::findElementById(rootEl, "externalToolsWidget");
    map[EXTERNAL_TOOLS_WIDGET_STATE_KEY] = ExternalToolsDashboardWidget::isValidDom(externalToolsEl) ? externalToolsEl : QDomElement();

    return map;
}

}  // namespace U2
