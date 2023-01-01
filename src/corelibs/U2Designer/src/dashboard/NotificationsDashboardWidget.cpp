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

#include "./NotificationsDashboardWidget.h"

#include <U2Core/U2SafePoints.h>

#include <U2Lang/WorkflowUtils.h>

#include "./DomUtils.h"

namespace U2 {

#define NOTIFICATIONS_WIDGET_ID QString("problemsWidget123")

static QList<NotificationsDashboardInfo> dom2NotificationInfo(const QDomElement& dom) {
    QList<NotificationsDashboardInfo> result;
    QDomElement tbody = DomUtils::findElementById(dom, NOTIFICATIONS_WIDGET_ID);
    for (QDomElement tr = tbody.firstChildElement("tr"); !tr.isNull(); tr = tr.nextSiblingElement("tr")) {
        QDomElement td1 = tr.firstChildElement("td");
        QDomElement td2 = td1.nextSiblingElement("td");
        QDomElement td3 = td2.nextSiblingElement("td");
        QString type = QFileInfo(td1.firstChildElement("img").attribute("src")).baseName();
        QString actorName = td2.text();
        QString message = td3.text().trimmed();
        int count = 1;
        if (message.startsWith("(")) {
            int closingBraceIndex = message.indexOf(")");
            if (closingBraceIndex > 0) {
                count = qMax(message.mid(1, closingBraceIndex - 2).toInt(), 1);
                message = message.right(closingBraceIndex).trimmed();
            }
        }
        result << NotificationsDashboardInfo(tr.attribute("id"), actorName, type, message, count);
    }
    return result;
}

NotificationsDashboardWidget::NotificationsDashboardWidget(const QDomElement& dom, const WorkflowMonitor* monitor)
    : monitor(monitor), dashboardWidget(nullptr) {
    setFixedWidth(550);
    setObjectName("NotificationsDashboardWidget");
    tableGridLayout = new QGridLayout();
    tableGridLayout->setSpacing(0);
    tableGridLayout->setColumnStretch(0, 0);
    tableGridLayout->setColumnStretch(1, 35);
    tableGridLayout->setColumnStretch(2, 65);
    setLayout(tableGridLayout);

    addTableHeadersRow(tableGridLayout, QStringList() << tr("Type") << tr("Element") << tr("Message"));
    notificationList = dom2NotificationInfo(dom);
    for (int notificationIndex = 0; notificationIndex < notificationList.size(); notificationIndex++) {
        updateNotificationRow(notificationIndex);
    }
    if (monitor != nullptr) {
        for (auto notification : qAsConst(monitor->getNotifications())) {
            sl_newNotification(notification, 1);
        }
        connect(monitor, SIGNAL(si_newNotification(WorkflowNotification, int)), SLOT(sl_newNotification(WorkflowNotification, int)));
    }
}

void NotificationsDashboardWidget::setDashboardWidget(QWidget* dashboardWidgetParent) {
    dashboardWidget = dashboardWidgetParent;
    updateVisibility();
}

void NotificationsDashboardWidget::updateVisibility() {
    CHECK(dashboardWidget != nullptr, );
    dashboardWidget->setHidden(notificationList.isEmpty());
}

void NotificationsDashboardWidget::sl_newNotification(const WorkflowNotification& wdNotification, int count) {
    for (int i = 0; i < notificationList.size(); i++) {
        NotificationsDashboardInfo& oldNotification = notificationList[i];
        if (oldNotification.actorId == wdNotification.actorId &&
            oldNotification.type == wdNotification.type &&
            oldNotification.message == wdNotification.message) {
            oldNotification.count = count;
            updateNotificationRow(i);
            return;
        }
    }
    CHECK(monitor != nullptr, );
    QString actorName = monitor->actorName(wdNotification.actorId);
    notificationList << NotificationsDashboardInfo(wdNotification.actorId, actorName, wdNotification.type, wdNotification.message, count);
    updateVisibility();
    updateNotificationRow(notificationList.size() - 1);
}

void NotificationsDashboardWidget::updateNotificationRow(int workerIndex) {
    auto info = notificationList[workerIndex];
    QString messageWithCount = (info.count > 1 ? "(" + QString::number(info.count) + ") " : "") + info.message;
    bool isLastRow = workerIndex == notificationList.size() - 1;
    int rowIndex = workerIndex + 1;
    QString iconHtml = info.type.isEmpty() ? "" : "<center><img class=\"" + info.type + "\" src=\":/U2Lang/images/" + info.type + "_20px.png\"></center>";
    QString rowId = QString::number(workerIndex);
    addTableCell(tableGridLayout, rowId, iconHtml, rowIndex, 0, isLastRow, false);
    addTableCell(tableGridLayout, rowId, info.actorName, rowIndex, 1, isLastRow, false);
    addTableCell(tableGridLayout, rowId, messageWithCount, rowIndex, 2, isLastRow, true);
}

bool NotificationsDashboardWidget::isValidDom(const QDomElement& dom) {
    return !DomUtils::findElementById(dom, NOTIFICATIONS_WIDGET_ID).isNull();
}

QString NotificationsDashboardWidget::toHtml() const {
    QString html = "<div id=\"problemsWidget\">\n<table>\n";
    html += "<thead><tr><th>" + tr("Type") + "</th><th>" + tr("Element") + "</th><th>" + tr("Message") + "</th></tr></thead>\n";
    html += "<tbody id=\"" + NOTIFICATIONS_WIDGET_ID + "\">\n";
    for (auto notification : qAsConst(notificationList)) {
        if (notification.type.isEmpty()) {
            continue;
        }
        html += "<tr id=\"notification-widget:" + notification.actorId.replace('"', "_") + "\">";
        html += "<td><img src=\"" + notification.type + ".png\"></td>";
        html += "<td>" + notification.actorName.toHtmlEscaped() + "</td>";
        html += "<td>" + notification.message.toHtmlEscaped() + "</td>";
        html += "</tr>\n";
    }
    html += "</tbody>\n</table>\n</div>\n";
    return html;
}

NotificationsDashboardInfo::NotificationsDashboardInfo(const QString& actorId, const QString& actorName, const QString& type, const QString& message, int count)
    : actorId(actorId), actorName(actorName), type(type), message(message), count(count) {
}

}  // namespace U2
