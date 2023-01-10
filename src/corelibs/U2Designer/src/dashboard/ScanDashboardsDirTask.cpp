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

#include "ScanDashboardsDirTask.h"

#include <QDir>
#include <QSettings>

#include <U2Core/U2SafePoints.h>

#include <U2Lang/WorkflowSettings.h>

#include "Dashboard.h"

namespace U2 {

ScanDashboardsDirTask::ScanDashboardsDirTask()
    : Task(tr("Scan dashboards folder"), TaskFlag_None) {
    tpm = Progress_Manual;
}

const QList<DashboardInfo>& ScanDashboardsDirTask::getResult() const {
    return dashboardInfos;
}

void ScanDashboardsDirTask::run() {
    QDir outDir(WorkflowSettings::getWorkflowOutputDirectory());
    CHECK(outDir.exists(), );

    QFileInfoList dirs = outDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    int counter = 0;
    foreach (const QFileInfo& info, dirs) {
        CHECK_OP(stateInfo, );
        QString dirPath = info.absoluteFilePath() + "/";
        if (isDashboardDir(dirPath)) {
            dashboardInfos << readDashboardInfo(dirPath);
        }
        stateInfo.setProgress((100 * counter++) / dirs.count());
    }
}

bool ScanDashboardsDirTask::isDashboardDir(const QString& dirPath) {
    QDir dir(dirPath + Dashboard::REPORT_SUB_DIR);
    CHECK(dir.exists(), false);
    CHECK(dir.exists(Dashboard::DB_FILE_NAME), false);
    CHECK(dir.exists(Dashboard::SETTINGS_FILE_NAME), false);
    return true;
}

DashboardInfo ScanDashboardsDirTask::readDashboardInfo(const QString& dirPath) {
    DashboardInfo info(dirPath);
    QSettings settings(dirPath + Dashboard::REPORT_SUB_DIR + Dashboard::SETTINGS_FILE_NAME, QSettings::IniFormat);
    info.opened = settings.value(Dashboard::OPENED_SETTING).toBool();
    info.name = settings.value(Dashboard::NAME_SETTING).toString();
    return info;
}

}  // namespace U2
