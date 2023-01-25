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

#include "./StatisticsDashboardWidget.h"

#include <U2Core/ProjectModel.h>

#include <U2Lang/WorkflowUtils.h>

#include "./DomUtils.h"

namespace U2 {

#define STATISTICS_WIDGET_ID QString("statisticsWidget123")

static QList<StatisticsRow> dom2StatisticRows(const QDomElement& dom) {
    QList<StatisticsRow> statisticsRows;
    QDomElement tbody = DomUtils::findElementById(dom, STATISTICS_WIDGET_ID);
    for (auto tr = tbody.firstChildElement("tr"); !tr.isNull(); tr = tr.nextSiblingElement("tr")) {
        QString id = tr.attribute("id");
        if (id.isEmpty() || id == "undefined") {
            continue;
        }
        QDomElement td1 = tr.firstChildElement("td");
        QDomElement td2 = td1.nextSiblingElement("td");
        QDomElement td3 = td2.nextSiblingElement("td");
        statisticsRows << StatisticsRow(id, td1.text(), td2.text(), td3.text());
    }
    return statisticsRows;
}

StatisticsDashboardWidget::StatisticsDashboardWidget(const QDomElement& dom, const WorkflowMonitor* monitor)
    : monitor(monitor) {
    setFixedWidth(550);
    tableGridLayout = new QGridLayout();
    tableGridLayout->setSpacing(0);
    setLayout(tableGridLayout);

    if (monitor != nullptr) {
        connect(monitor,
                SIGNAL(si_workerInfoChanged(const QString&, const Monitor::WorkerInfo&)),
                SLOT(sl_workerInfoChanged(const QString&, const Monitor::WorkerInfo&)));
        connect(monitor, SIGNAL(si_updateProducers()), SLOT(sl_updateProducers()));
    }

    addTableHeadersRow(tableGridLayout, QStringList() << tr("Element") << tr("Elapsed time") << tr("Output messages"));
    statisticsRows = dom2StatisticRows(dom);
    for (auto row : qAsConst(statisticsRows)) {
        addTableRow(tableGridLayout, row.id, QStringList() << row.name << row.time << row.count);
    }
}

bool StatisticsDashboardWidget::isValidDom(const QDomElement& dom) {
    return !DomUtils::findElementById(dom, STATISTICS_WIDGET_ID).isNull();
}

QString StatisticsDashboardWidget::toHtml() const {
    QString html = "<div id=\"statisticsWidget\">\n<table>\n";
    html += "<thead><tr><th>" + tr("Element") + "</th><th>" + tr("Elapsed time") + "</th><th>" + tr("Output messages") + "</th></tr></thead>\n";
    html += "<tbody id=\"" + STATISTICS_WIDGET_ID + "\">\n";
    for (auto row : qAsConst(statisticsRows)) {
        html += "<tr class=\"filled-row\" id=\"" + row.id.toHtmlEscaped() + "\">";
        html += "<td>" + row.name.toHtmlEscaped() + "</td>";
        html += "<td>" + row.time.toHtmlEscaped() + "</td>";
        html += "<td>" + row.count.toHtmlEscaped() + "</td>";
        html += "</tr>\n";
    }
    html += "</tbody>\n";
    html += "</table>\n</div>\n";
    return html;
}

static QString formatTimeString(qint64 microseconds) {
    int milliseconds = microseconds / 1000;
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    return QString("%1:%2:%3.%4")
        .arg(QString::number(hours), 2, '0')
        .arg(QString::number(minutes % 60), 2, '0')
        .arg(QString::number(seconds % 60), 2, '0')
        .arg(QString::number(milliseconds % 1000), 3, '0');
}

void StatisticsDashboardWidget::sl_workerInfoChanged(const QString& actorId, const Monitor::WorkerInfo& info) {
    QString name = monitor->actorName(actorId);
    QString time = formatTimeString(info.timeMks);
    QString count = QString::number(monitor->getDataProduced(actorId));
    bool isAdded = addOrUpdateTableRow(tableGridLayout, actorId, QStringList() << name << time << count);
    if (isAdded) {
        statisticsRows << StatisticsRow(actorId, name, time, count);
    }
}

void StatisticsDashboardWidget::sl_updateProducers() {
    const QMap<QString, Monitor::WorkerInfo>& workerInfoMap = monitor->getWorkersInfo();
    const QList<QString> actorIdList = workerInfoMap.keys();
    for (const QString& actorId : qAsConst(actorIdList)) {
        const Monitor::WorkerInfo& info = workerInfoMap[actorId];
        sl_workerInfoChanged(actorId, info);
    }
}

StatisticsRow::StatisticsRow(const QString& id, const QString& name, const QString& time, const QString& count)
    : id(id), name(name), time(time), count(count) {
}

}  // namespace U2
