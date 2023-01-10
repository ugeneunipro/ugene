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

#ifndef _U2_NOTIFICATIONS_DASHBOARD_WIDGET_H_
#define _U2_NOTIFICATIONS_DASHBOARD_WIDGET_H_

#include <QDomElement>
#include <QGridLayout>
#include <QWidget>

#include <U2Core/global.h>

#include <U2Lang/WorkflowMonitor.h>

#include "DashboardWidget.h"

namespace U2 {

class U2DESIGNER_EXPORT NotificationsDashboardInfo {
public:
    NotificationsDashboardInfo(const QString& actorId, const QString& actorName, const QString& type, const QString& message, int count);

    QString actorId;
    QString actorName;
    QString type;
    QString message;
    int count;
};

class U2DESIGNER_EXPORT NotificationsDashboardWidget : public QWidget, public DashboardWidgetUtils {
    Q_OBJECT
public:
    NotificationsDashboardWidget(const QDomElement& dom, const WorkflowMonitor* monitor = nullptr);

    static bool isValidDom(const QDomElement& dom);

    QString toHtml() const;

    void setDashboardWidget(QWidget* dashboardWidget);

    void updateVisibility();

private slots:
    void sl_newNotification(const WorkflowNotification& wdNotification, int count);

private:
    void updateNotificationRow(int workerIndex);

    const QPointer<const WorkflowMonitor> monitor;
    QWidget* dashboardWidget;
    QGridLayout* tableGridLayout;
    QList<NotificationsDashboardInfo> notificationList;
};

}  // namespace U2

#endif
