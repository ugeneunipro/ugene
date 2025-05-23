/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QDomElement>
#include <QGridLayout>
#include <QToolButton>
#include <QWidget>

#include <U2Core/global.h>

#include <U2Lang/WorkflowMonitor.h>

#include "DashboardWidget.h"

namespace U2 {

class U2DESIGNER_EXPORT WorkerOutputInfo {
public:
    WorkerOutputInfo(const QString& id, const QString& name, const QStringList& files);
    QString id;
    QString name;
    QStringList files;
};

class U2DESIGNER_EXPORT OutputFilesDashboardWidget : public QWidget, public DashboardWidgetUtils {
    Q_OBJECT
public:
    OutputFilesDashboardWidget(const QString& dashboardDir, const QDomElement& dom, const WorkflowMonitor* monitor = nullptr);

    static bool isValidDom(const QDomElement& dom);

    QString toHtml() const;

private slots:
    void sl_newOutputFile(const Monitor::FileInfo& info);

private:
    void updateWorkerRow(int workerIndex);

    /** Ensures the table has at least 3 rows: add empty rows to the end. */
    void addTrailingEmptyRows(bool callTableUpdate);

    QString dashboardDir;
    const WorkflowMonitor* monitor;
    QGridLayout* tableGridLayout;
    QList<WorkerOutputInfo> workerOutputList;
};

}  // namespace U2
