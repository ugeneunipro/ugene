/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
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

#ifndef _U2_SCAN_DASHBOARDS_DIR_TASK_H_
#define _U2_SCAN_DASHBOARDS_DIR_TASK_H_

#include <U2Core/Task.h>

#include "DashboardInfo.h"

namespace U2 {

class U2DESIGNER_EXPORT ScanDashboardsDirTask : public Task {
    Q_OBJECT
public:
    ScanDashboardsDirTask();

    const QList<DashboardInfo>& getResult() const;

private:
    void run() override;

    static bool isDashboardDir(const QString& dirPath);
    static DashboardInfo readDashboardInfo(const QString& dirPath);

    QList<DashboardInfo> dashboardInfos;
};

}  // namespace U2

#endif  // _U2_SCAN_DASHBOARDS_DIR_TASK_H_
