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

#include "./OutputFilesDashboardWidget.h"

#include <U2Lang/WorkflowUtils.h>

#include "./DomUtils.h"

namespace U2 {

#define OUTPUT_FILES_WIDGET_ID QString("outputWidget123")

static void addUrlToResult(const QDomElement& element, QStringList& result) {
    QString url = DashboardWidgetUtils::parseOpenUrlValueFromOnClick(element.attribute("onclick"));
    if (url.isEmpty()) {
        url = element.text();
    }
    result << url;
}

static QStringList readFilesList(const QDomElement& td) {
    QStringList result;
    QDomElement button = td.firstChildElement("div").firstChildElement("div").firstChildElement("button");
    if (button.hasAttribute("onclick")) {
        addUrlToResult(button, result);
        return result;
    }
    QDomElement li = td.firstChildElement("div").firstChildElement("ul").firstChildElement("li");
    for (; !li.isNull(); li = li.nextSiblingElement("li")) {
        addUrlToResult(li.firstChildElement("a"), result);
    }
    return result;
}

static QList<WorkerOutputInfo> dom2WorkerOutputInfo(const QDomElement& dom) {
    QList<WorkerOutputInfo> result;
    QDomElement tBody = DomUtils::findElementById(dom, OUTPUT_FILES_WIDGET_ID);
    for (QDomElement tr = tBody.firstChildElement("tr"); !tr.isNull(); tr = tr.nextSiblingElement("tr")) {
        QDomElement td1 = tr.firstChildElement("td");
        QDomElement td2 = td1.nextSiblingElement("td");
        result << WorkerOutputInfo(tr.attribute("id"), td2.text(), readFilesList(td1));
    }
    return result;
}

/** When number of files will be > MIN_FILE_COUNT_TO_USE_SINGLE_BUTTON a single DashboardFileButton will be used to show all of them. */
#define MIN_FILE_COUNT_TO_USE_SINGLE_BUTTON 11

OutputFilesDashboardWidget::OutputFilesDashboardWidget(const QString& dashboardDir, const QDomElement& dom, const WorkflowMonitor* monitor)
    : dashboardDir(dashboardDir), monitor(monitor) {
    setFixedWidth(550);
    setObjectName("OutputFilesDashboardWidget");
    tableGridLayout = new QGridLayout();
    tableGridLayout->setSpacing(0);
    setLayout(tableGridLayout);

    addTableHeadersRow(tableGridLayout, QStringList() << tr("File") << tr("Producer"));
    workerOutputList = dom2WorkerOutputInfo(dom);

    addTrailingEmptyRows(false);

    for (int workerIndex = 0; workerIndex < workerOutputList.size(); workerIndex++) {
        updateWorkerRow(workerIndex);
    }
    if (monitor != nullptr) {
        connect(monitor, SIGNAL(si_newOutputFile(const Monitor::FileInfo&)), SLOT(sl_newOutputFile(const Monitor::FileInfo&)));
    }
}

void OutputFilesDashboardWidget::addTrailingEmptyRows(bool callTableUpdate) {
    while (workerOutputList.size() < 3) {
        workerOutputList << WorkerOutputInfo("", "", QStringList());
        if (callTableUpdate) {
            updateWorkerRow(workerOutputList.size() - 1);
        }
    }
}

void OutputFilesDashboardWidget::sl_newOutputFile(const Monitor::FileInfo& info) {
    for (int i = 0; i < workerOutputList.size(); i++) {
        WorkerOutputInfo& worker = workerOutputList[i];
        if (worker.id == info.actor) {
            worker.files << info.url;
            updateWorkerRow(i);
            return;
        }
    }
    CHECK(monitor != nullptr, );
    // Remove trailing empty rows.
    while (workerOutputList.size() > 0 && workerOutputList.last().id.isEmpty()) {
        workerOutputList.removeLast();
    }

    // Add the new file row.
    workerOutputList << WorkerOutputInfo(info.actor, monitor->actorName(info.actor), QStringList() << info.url);
    updateWorkerRow(workerOutputList.size() - 1);

    // Add trailing empty rows back.
    addTrailingEmptyRows(true);
}

void OutputFilesDashboardWidget::updateWorkerRow(int workerIndex) {
    auto worker = workerOutputList[workerIndex];
    bool isLastRow = workerIndex == workerOutputList.size() - 1;
    // Left cell: a button with URLs.
    if (worker.files.isEmpty()) {
        addTableCell(tableGridLayout, worker.name, "", workerIndex + 1, 0, isLastRow, false);
    } else if (worker.files.length() == 1 || worker.files.length() >= MIN_FILE_COUNT_TO_USE_SINGLE_BUTTON) {
        auto button = new DashboardFileButton(worker.files, dashboardDir, monitor);
        addTableCell(tableGridLayout, worker.name, button, workerIndex + 1, 0, isLastRow, false);
    } else {
        auto cellWidget = new QWidget();
        auto cellWidgetLayout = new QVBoxLayout();
        cellWidgetLayout->setContentsMargins(0, 0, 0, 0);
        cellWidget->setLayout(cellWidgetLayout);
        for (auto url : qAsConst(worker.files)) {
            cellWidgetLayout->addWidget(new DashboardFileButton(QStringList() << url, dashboardDir, monitor));
        }
        addTableCell(tableGridLayout, worker.name, cellWidget, workerIndex + 1, 0, isLastRow, false);
    }

    // Right cell: a worker name.
    addTableCell(tableGridLayout, worker.name, worker.name, workerIndex + 1, 1, isLastRow, true);
}

bool OutputFilesDashboardWidget::isValidDom(const QDomElement& dom) {
    return !DomUtils::findElementById(dom, OUTPUT_FILES_WIDGET_ID).isNull();
}

QString OutputFilesDashboardWidget::toHtml() const {
    QString html = "<div id=\"outputWidget\">\n<table>\n";
    html += "<thead><tr><th>" + tr("File") + "</th><th>" + tr("Producer") + "</th></tr></thead>\n";
    html += "<tbody id=\"" + OUTPUT_FILES_WIDGET_ID + "\">\n";
    for (auto workerInfo : qAsConst(workerOutputList)) {
        if (workerInfo.id.isEmpty()) {
            continue;
        }
        html += "<tr id=\"output-widget:" + workerInfo.id.replace('"', "_") + "\">\n";
        html += "<td><div><ul>\n";
        for (auto url : qAsConst(workerInfo.files)) {
            html += "<li><a>" + url.toHtmlEscaped() + "</a></li>\n";
        }
        html += "</ul></div></td>\n";
        html += "<td>" + workerInfo.name.toHtmlEscaped() + "</td>\n";
        html += "</tr>\n";
    }
    html += "</tbody>\n</table>\n</div>";
    return html;
}

WorkerOutputInfo::WorkerOutputInfo(const QString& id, const QString& name, const QStringList& files)
    : id(id), name(name), files(files) {
}

}  // namespace U2
