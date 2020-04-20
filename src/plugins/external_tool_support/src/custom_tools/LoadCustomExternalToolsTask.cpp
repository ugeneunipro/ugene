/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ImportCustomToolsTask.h"
#include "LoadCustomExternalToolsTask.h"
#include "RegisterCustomToolTask.h"

namespace U2 {

LoadCustomExternalToolsTask::LoadCustomExternalToolsTask()
    : Task(tr("Load custom external tools"), TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel)
{

}

const QList<CustomExternalTool *> &LoadCustomExternalToolsTask::getTools() const {
    return tools;
}

void LoadCustomExternalToolsTask::prepare() {
    QList<Task *> registerTask;

    const QString storagePath = AppContext::getAppSettings()->getUserAppsSettings()->getCustomToolsConfigsDirPath();

    QDir dir(storagePath);
    CHECK(dir.exists(), );

    dir.setNameFilters(QStringList() << "*.xml");
    QFileInfoList fileList = dir.entryInfoList();

    foreach (const QFileInfo &fileInfo, fileList) {
        addSubTask(new RegisterCustomToolTask(fileInfo.filePath()));
    }
}

QList<Task *> LoadCustomExternalToolsTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    RegisterCustomToolTask *registerTask = qobject_cast<RegisterCustomToolTask *>(subTask);
    SAFE_POINT_EXT(nullptr != registerTask, setError("Unexpected task, can't cast it to RegisterCustomToolTask *"), result);
    CustomExternalTool *tool = registerTask->getTool();
    CHECK(nullptr != tool, result);
    tools << tool;
    return result;
}

}   // namespace U2
