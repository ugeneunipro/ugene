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

#include "./StatusDashboardWidget.h"

#include <QDateTime>
#include <QHBoxLayout>

#include <U2Core/ProjectModel.h>

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

#define STATUS_LABEL_COMMON_STYLE QString("padding: 8px 35px 8px 14px; margin-bottom: 10px; border-radius: 4px;")
#define STATUS_LABEL_DEFAULT_STYLE "background-color: #f5f5f5; border: 1px solid #e3e3e3;"
#define STATUS_LABEL_INFO_STYLE "color: #3a87ad; background-color: #d9edf7; border: 1px solid #bce8f1;"
#define STATUS_LABEL_ERROR_STYLE "color: #b94a48; background-color: #f2dede; border: 1px solid #eed3d7;"
#define STATUS_LABEL_SUCCESS_STYLE "color: #468847; background-color: #dff0d8; border: 1px solid #d6e9c6;"

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
    timeLabel->setStyleSheet(STATUS_LABEL_COMMON_STYLE + STATUS_LABEL_DEFAULT_STYLE);
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
    switch (state) {
        case Monitor::RUNNING:
            return STATUS_LABEL_COMMON_STYLE + STATUS_LABEL_INFO_STYLE;
        case Monitor::FAILED:
            return STATUS_LABEL_COMMON_STYLE + STATUS_LABEL_ERROR_STYLE;
        case Monitor::SUCCESS:
            return STATUS_LABEL_COMMON_STYLE + STATUS_LABEL_SUCCESS_STYLE;
        default:
            return STATUS_LABEL_COMMON_STYLE + STATUS_LABEL_DEFAULT_STYLE;
    }
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
