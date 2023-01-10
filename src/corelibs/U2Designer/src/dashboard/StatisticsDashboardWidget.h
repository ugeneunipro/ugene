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

#ifndef _U2_STATISTICS_DASHBOARD_WIDGET_H_
#define _U2_STATISTICS_DASHBOARD_WIDGET_H_

#include <QDomElement>
#include <QGridLayout>
#include <QWidget>

#include <U2Core/global.h>

#include <U2Lang/WorkflowMonitor.h>

#include "DashboardWidget.h"

namespace U2 {

class U2DESIGNER_EXPORT StatisticsRow {
public:
    StatisticsRow(const QString& id, const QString& name, const QString& time, const QString& count);

    QString id;
    QString name;
    QString time;
    QString count;
};

class U2DESIGNER_EXPORT StatisticsDashboardWidget : public QWidget, public DashboardWidgetUtils {
    Q_OBJECT
public:
    StatisticsDashboardWidget(const QDomElement& dom, const WorkflowMonitor* monitor = nullptr);

    static bool isValidDom(const QDomElement& dom);

    QString toHtml() const;

private slots:
    void sl_workerInfoChanged(const QString& actorId, const Monitor::WorkerInfo& info);
    void sl_updateProducers();

private:
    const WorkflowMonitor* monitor;
    QGridLayout* tableGridLayout;
    QList<StatisticsRow> statisticsRows;
};

}  // namespace U2

#endif
