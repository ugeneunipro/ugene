/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "./StatusDashboardWidget.h"

#include <QDateTime>
#include <QHBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/HoverQLabel.h>

#include <U2Lang/Dataset.h>
#include <U2Lang/WorkflowUtils.h>

#include "./Dashboard.h"
#include "./DomUtils.h"

namespace U2 {

static int parseProgress(const QDomElement& dom) {
    QDomElement progressBar = DomUtils::findElementById(dom, "progressBar").firstChildElement("div");
    if (progressBar.isNull()) {
        return 0;
    }
    QString widthToken = "width: ";
    QString style = progressBar.attribute("style");
    if (!style.startsWith(widthToken)) {
        return 100;
    }
    QString progressString = style.mid(widthToken.length(), style.indexOf("%") - widthToken.length()).trimmed();
    return progressString.toInt();
}

static Monitor::TaskState parseStateFromClass(const QString& cssClass) {
    if (cssClass.contains("alert-error")) {
        return Monitor::TaskState::FAILED;
    } else if (cssClass.contains("alert-success")) {
        return Monitor::TaskState::SUCCESS;
    }
    return Monitor::CANCELLED;
}

static const QString STATUS_LABEL_COMMON_STYLE = "padding: 8px 35px 8px 14px; margin-bottom: 10px; border-radius: 4px;";

static const QString STATUS_LABEL_DEFAULT_BACKGROUND_COLOR_LIGHT = "#f5f5f5";
static const QString STATUS_LABEL_DEFAULT_BORDER_COLOR_LIGHT = "#e3e3e3";
static const QString STATUS_LABEL_DEFAULT_BACKGROUND_COLOR_DARK = "#3A3A3A";
static const QString STATUS_LABEL_DEFAULT_BORDER_COLOR_DARK = "#4C4C4C";
static const QString STATUS_LABEL_DEFAULT_STYLE = "background-color: %1; border: 1px solid %2;";

static const QString STATUS_LABEL_INFO_TEXT_COLOR_LIGHT = "#3a87ad";
static const QString STATUS_LABEL_INFO_BACKGROUND_COLOR_LIGHT = "#d9edf7";
static const QString STATUS_LABEL_INFO_BORDER_COLOR_LIGHT = "#bce8f1";
static const QString STATUS_LABEL_INFO_TEXT_COLOR_DARK = "#70D1FF";
static const QString STATUS_LABEL_INFO_BACKGROUND_COLOR_DARK = "#305970";
static const QString STATUS_LABEL_INFO_BORDER_COLOR_DARK = "#2E616B";

static const QString STATUS_LABEL_ERROR_TEXT_COLOR_LIGHT = "#b94a48";
static const QString STATUS_LABEL_ERROR_BACKGROUND_COLOR_LIGHT = "#f2dede";
static const QString STATUS_LABEL_ERROR_BORDER_COLOR_LIGHT = "#eed3d7";
static const QString STATUS_LABEL_ERROR_TEXT_COLOR_DARK = "#FF8E8C";
static const QString STATUS_LABEL_ERROR_BACKGROUND_COLOR_DARK = "#583030";
static const QString STATUS_LABEL_ERROR_BORDER_COLOR_DARK = "#6B222D";

static const QString STATUS_LABEL_SUCCESS_TEXT_COLOR_LIGHT = "#468847";
static const QString STATUS_LABEL_SUCCESS_BACKGROUND_COLOR_LIGHT = "#dff0d8";
static const QString STATUS_LABEL_SUCCESS_BORDER_COLOR_LIGHT = "#d6e9c6";
static const QString STATUS_LABEL_SUCCESS_TEXT_COLOR_DARK = "#84FF84";
static const QString STATUS_LABEL_SUCCESS_BACKGROUND_COLOR_DARK = "#20440D";
static const QString STATUS_LABEL_SUCCESS_BORDER_COLOR_DARK = "#233F09";

static const QString STATUS_LABEL_NOT_DEFAULT_STYLE = "color: %1; background-color: %2; border: 1px solid %3;";

StatusDashboardWidget::StatusDashboardWidget(const QDomElement& dom, const WorkflowMonitor* monitor)
    : monitor(monitor), timer(nullptr), timerStartMillis(0) {
    setFixedWidth(550);
    setObjectName("StatusDashboardWidget");
    timeText = DomUtils::findElementById(dom, "timer").text();
    int progress = parseProgress(dom);
    QDomElement statusBar = DomUtils::findElementById(dom, "status-bar");
    state = parseStateFromClass(statusBar.attribute("class"));
    statusMessage = DomUtils::findElementById(statusBar, "status-message").firstChildElement("span").text();

    auto layout = new QVBoxLayout();
    setLayout(layout);
    timeLabel = new QLabel(tr("Time %1").arg(timeText));
    // There is no default state, but CANCELLED is considered as default for the next function
    auto defaultState = Monitor::TaskState::CANCELLED;
    timeLabel->setStyleSheet(state2LabelStyle(defaultState));
    timeLabel->setObjectName("timeLabel");
    layout->addWidget(timeLabel);

    auto progressBarRowLayout = new QHBoxLayout();
    progressBarRowLayout->setContentsMargins(0, 0, 0, 10);
    progressBar = new QProgressBar();
    progressBar->setFixedWidth(250);
    progressBar->setStyleSheet("QProgressBar {"
                               "  text-align: center;"
                               "  color: white;"
                               "  border-radius: 4px;"
                               "  background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #149bdf, stop: 1 #0480be);"
                               "}"
                               "QProgressBar::chunk:horizontal {"
                               "  border-radius: 4px;"
                               "}");
    progressBar->setMaximum(100);
    progressBar->setValue(progress);
    progressBarRowLayout->addStretch();
    progressBarRowLayout->addWidget(progressBar);
    progressBarRowLayout->addStretch();
    layout->addLayout(progressBarRowLayout);

    statusMessageLabel = new QLabel("<center>" + statusMessage + "</center>");
    statusMessageLabel->setStyleSheet(state2LabelStyle(state));
    statusMessageLabel->setObjectName("statusMessageLabel");
    layout->addWidget(statusMessageLabel);

    if (monitor != nullptr) {
        connect(monitor, SIGNAL(si_progressChanged(int)), SLOT(sl_progressChanged(int)));
        connect(monitor, SIGNAL(si_taskStateChanged(Monitor::TaskState)), SLOT(sl_taskStateChanged(Monitor::TaskState)));
        updateTimeLabel();
        sl_taskStateChanged(monitor->getTaskState());
    }
    connect(&timer, SIGNAL(timeout()), SLOT(sl_timerEvent()));
    connect(AppContext::getMainWindow(), &MainWindow::si_colorThemeSwitched, this, &StatusDashboardWidget::sl_colorThemeSwitched);
}

void StatusDashboardWidget::startTimer() {
    timer.stop();
    timer.start(500);
    updateTimeLabel();
}

void StatusDashboardWidget::stopTimer() {
    timer.stop();
    updateTimeLabel();
}

void StatusDashboardWidget::sl_timerEvent() {
    updateTimeLabel();
}

void StatusDashboardWidget::sl_colorThemeSwitched() {
    // There is no default state, but CANCELLED is considered as default for the next function
    auto defaultState = Monitor::TaskState::CANCELLED;
    timeLabel->setStyleSheet(state2LabelStyle(defaultState));
    statusMessageLabel->setStyleSheet(state2LabelStyle(state));
}

void StatusDashboardWidget::updateTimeLabel() {
    qint64 currentTimeMillis = QDateTime::currentMSecsSinceEpoch();
    if (timerStartMillis == 0) {
        timerStartMillis = currentTimeMillis;
    }
    int milliseconds = QDateTime::currentMSecsSinceEpoch() - timerStartMillis;
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    timeText = QString("%1:%2:%3")
                   .arg(QString::number(hours), 2, '0')
                   .arg(QString::number(minutes % 60), 2, '0')
                   .arg(QString::number(seconds % 60), 2, '0');
    timeLabel->setText(tr("Time %1").arg(timeText));
}

void StatusDashboardWidget::sl_progressChanged(int progress) {
    progressBar->setValue(progress);
}

QString StatusDashboardWidget::state2StatusMessage(const Monitor::TaskState& state) {
    switch (state) {
        case Monitor::RUNNING:
            return tr("The workflow task is in progress…");
        case Monitor::RUNNING_WITH_PROBLEMS:
            return tr("The workflow task is in progress. There are problems…");
        case Monitor::FINISHED_WITH_PROBLEMS:
            return tr("The workflow task has been finished with warnings!");
        case Monitor::FAILED:
            return tr("The workflow task has been finished with errors!");
        case Monitor::SUCCESS:
            return tr("The workflow task has been finished successfully!");
        default:
            return tr("The workflow task has been canceled!");
    }
}

QString StatusDashboardWidget::state2LabelStyle(const Monitor::TaskState& state) {
    QString result = STATUS_LABEL_COMMON_STYLE;
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    switch (state) {
        case Monitor::RUNNING: {
            QString textColor = isDark ? STATUS_LABEL_INFO_TEXT_COLOR_DARK : STATUS_LABEL_INFO_TEXT_COLOR_LIGHT;
            QString backgroundColor = isDark ? STATUS_LABEL_INFO_BACKGROUND_COLOR_DARK : STATUS_LABEL_INFO_BACKGROUND_COLOR_LIGHT;
            QString borderColor = isDark ? STATUS_LABEL_INFO_BORDER_COLOR_DARK : STATUS_LABEL_INFO_BORDER_COLOR_LIGHT;
            result += STATUS_LABEL_NOT_DEFAULT_STYLE.arg(textColor).arg(backgroundColor).arg(borderColor);
            break;
        }
        case Monitor::FAILED: {
            QString textColor = isDark ? STATUS_LABEL_ERROR_TEXT_COLOR_DARK : STATUS_LABEL_ERROR_TEXT_COLOR_LIGHT;
            QString backgroundColor = isDark ? STATUS_LABEL_ERROR_BACKGROUND_COLOR_DARK : STATUS_LABEL_ERROR_BACKGROUND_COLOR_LIGHT;
            QString borderColor = isDark ? STATUS_LABEL_ERROR_BORDER_COLOR_DARK : STATUS_LABEL_ERROR_BORDER_COLOR_LIGHT;
            result += STATUS_LABEL_NOT_DEFAULT_STYLE.arg(textColor).arg(backgroundColor).arg(borderColor);
            break;
        }
        case Monitor::SUCCESS: {
            QString textColor = isDark ? STATUS_LABEL_SUCCESS_TEXT_COLOR_DARK : STATUS_LABEL_SUCCESS_TEXT_COLOR_LIGHT;
            QString backgroundColor = isDark ? STATUS_LABEL_SUCCESS_BACKGROUND_COLOR_DARK : STATUS_LABEL_SUCCESS_BACKGROUND_COLOR_LIGHT;
            QString borderColor = isDark ? STATUS_LABEL_SUCCESS_BORDER_COLOR_DARK : STATUS_LABEL_SUCCESS_BORDER_COLOR_LIGHT;
            result += STATUS_LABEL_NOT_DEFAULT_STYLE.arg(textColor).arg(backgroundColor).arg(borderColor);
            break;
        }
        default:
            QString backgroundColor = isDark ? STATUS_LABEL_DEFAULT_BACKGROUND_COLOR_DARK : STATUS_LABEL_DEFAULT_BACKGROUND_COLOR_LIGHT;
            QString borderColor = isDark ? STATUS_LABEL_DEFAULT_BORDER_COLOR_DARK : STATUS_LABEL_DEFAULT_BORDER_COLOR_LIGHT;
            result += STATUS_LABEL_DEFAULT_STYLE.arg(backgroundColor).arg(borderColor);
            break;
    }
    return result;
}

void StatusDashboardWidget::sl_taskStateChanged(Monitor::TaskState newState) {
    state = newState;
    statusMessage = state2StatusMessage(state);
    statusMessageLabel->setText("<center>" + statusMessage + "</center>");
    statusMessageLabel->setStyleSheet(state2LabelStyle(state));
}

bool StatusDashboardWidget::isValidDom(const QDomElement& dom) {
    return !DomUtils::findElementById(dom, "progressBar").isNull() &&
           !DomUtils::findElementById(dom, "status-message").isNull() &&
           !DomUtils::findElementById(dom, "timer").isNull();
}

static QString getClassByState(const Monitor::TaskState& state) {
    switch (state) {
        case Monitor::FAILED:
            return "alert-error";
        case Monitor::SUCCESS:
            return "alert-success";
        default:
            return "";
    }
}

QString StatusDashboardWidget::toHtml() const {
    QString html = "<div id=\"statusWidget\">";
    // Time
    html += "<div id=\"timer\">" + timeText + "</div>";

    // Progress
    html += "<div id=\"progressBar\"><div style=\"width: " + QString::number(progressBar->value()) + "%;\"></div></div>";

    // Status message
    html += "<div id=\"status-bar\" class=\"" + getClassByState(state) + "\">";
    html += " <div id=\"status-message\"><span>" + statusMessage + "</span></div>";
    html += "</div>";
    html += "</div>";
    return html;
}

}  // namespace U2
