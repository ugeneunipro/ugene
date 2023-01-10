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

#ifndef _U2_PARAMETERS_DASHBOARD_WIDGET_H_
#define _U2_PARAMETERS_DASHBOARD_WIDGET_H_

#include <QAbstractButton>
#include <QDomElement>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenu>

#include <U2Core/global.h>

#include <U2Lang/WorkflowMonitor.h>

#include "./DashboardWidget.h"

namespace U2 {

class HoverQLabel;

class U2DESIGNER_EXPORT WorkerParameterInfo {
public:
    WorkerParameterInfo(const QString& name, const QString& value, bool isUrl, bool isDir, bool isDataset);
    QString name;
    QString value;
    bool isUrl;
    bool isDir;
    bool isDataset;
};

class U2DESIGNER_EXPORT WorkerParametersInfo {
public:
    WorkerParametersInfo(const QString& workerName, const QList<WorkerParameterInfo>& parameters);
    QString workerName;
    QList<WorkerParameterInfo> parameters;
};

class U2DESIGNER_EXPORT ParametersDashboardWidget : public QWidget, DashboardWidgetUtils {
    Q_OBJECT
public:
    ParametersDashboardWidget(const QString& dashboardDir, const QDomElement& dom, const WorkflowMonitor* monitor = nullptr);

    const QList<WorkerParametersInfo>& getWorkers() const {
        return workers;
    }

    static bool isValidDom(const QDomElement& dom);

    QString toHtml() const;

    void showWorkerParameters(int workerIndex);

public slots:
    void sl_workerLabelClicked();

private:
    QString dashboardDir;
    const QPointer<const WorkflowMonitor> monitor;

    QHBoxLayout* layout;
    QGridLayout* parametersGridLayout;
    QList<WorkerParametersInfo> workers;
    QList<HoverQLabel*> workerNameLabels;
};
}  // namespace U2

#endif
