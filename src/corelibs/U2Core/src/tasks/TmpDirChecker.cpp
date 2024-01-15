/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "TmpDirChecker.h"

#include <QDir>
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

TmpDirChecker::TmpDirChecker()
    : Task(tr("Checking access rights to the temporary folder"), TaskFlag_SilentCancelOnShutdown) {
}

void TmpDirChecker::run() {
    commonTempDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath();
    QString ugeneTempDirPath = commonTempDirPath + "/ugene_tmp";

    if (!checkPath(commonTempDirPath)) {
        if (!AppContext::isGUIMode()) {
            QString message = "You do not have permission to write to \"" + commonTempDirPath +
                              "\" folder. Use --tmp-dir=<path_to_file> to set new temporary folder";
            coreLog.error((message));
        } else {
            emit si_checkFailed(commonTempDirPath);
        }
    }

    if (!checkPath(ugeneTempDirPath)) {
        if (!AppContext::isGUIMode()) {
            QString message = "UGENE hasn't permitions to write to its temporary folder \"" + ugeneTempDirPath +
                              "\". Use --tmp-dir=<path_to_file> to set another temporary folder";
            coreLog.error((message));
        } else {
            emit si_checkFailed(ugeneTempDirPath);
        }
    }
}

Task::ReportResult TmpDirChecker::report() {
    if (hasError() && AppContext::isGUIMode()) {
        stateInfo.setError(getError());
    }

    return ReportResult_Finished;
}

QString TmpDirChecker::getNewFilePath(const QString& dirPath, const QString& baseName) {
    QString filePath;
    int idx = 0;
    do {
        filePath = dirPath + "/" + baseName + "_" + QString::number(idx) + ".tmp";
        idx++;
    } while (QFile::exists(filePath));
    return filePath;
}

bool TmpDirChecker::checkPath(QString& path) {
    QDir dir;
    dir.mkpath(path);
    return FileAndDirectoryUtils::isDirectoryWritable(path);
}

}  // namespace U2
