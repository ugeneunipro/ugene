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

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/BAMUtils.h>

#include <U2Gui/OpenViewTask.h>

#include <U2View/AssemblyModel.h>

#include "ExtractAssemblyRegionTask.h"

namespace U2 {

ExtractAssemblyRegionAndOpenViewTask::ExtractAssemblyRegionAndOpenViewTask(const ExtractAssemblyRegionTaskSettings& settings)
    : Task(tr("Extract Assembly Region And Open View Task"), TaskFlags_NR_FOSE_COSC), settings(settings), extractTask(NULL) {}

void ExtractAssemblyRegionAndOpenViewTask::prepare() {
    if (settings.addToProject) {
        setSubtaskProgressWeight(50);
    } else {
        setSubtaskProgressWeight(100);
    }

    extractTask = new ExtractAssemblyRegionTask(settings);
    addSubTask(extractTask);
}

QList<Task*> ExtractAssemblyRegionAndOpenViewTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    CHECK(NULL != subTask, result);

    if (subTask->hasError() || subTask->isCanceled()) {
        return result;
    }

    if (extractTask == subTask && settings.addToProject) {
        Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(settings.fileUrl);
        CHECK(openTask != NULL, result);
        result.append(openTask);
    }
    return result;
}

ExtractAssemblyRegionTask::ExtractAssemblyRegionTask(const ExtractAssemblyRegionTaskSettings& settings)
    : Task(tr("Extract Assembly Region Task"), TaskFlag_None), settings(settings) {
    tpm = Task::Progress_Manual;
}

void ExtractAssemblyRegionTask::run() {
    taskLog.details("Start extracting regions to assembly");

    if (settings.fileFormat == BaseDocumentFormats::BAM || settings.fileFormat == BaseDocumentFormats::SAM) {
        QList<GObject*> objects;
        objects.append(settings.obj);
        BAMUtils::writeObjects(objects, GUrl(settings.fileUrl), settings.fileFormat, stateInfo, settings.regionToExtract);
    } else if (settings.fileFormat == BaseDocumentFormats::UGENEDB) {
        const U2DbiRef dbiRef = U2DbiRef(SQLITE_DBI_ID, settings.fileUrl);
        SAFE_POINT_OP(stateInfo, );

        U2EntityRef dstEntityRef = AssemblyObject::dbi2dbiExtractRegion(settings.obj, dbiRef, stateInfo, settings.regionToExtract, QVariantMap());
        SAFE_POINT_OP(stateInfo, );
    } else {
        setError(tr("Unsupported assembly format"));
    }
    taskLog.details("Finish extracting regions to assembly");
}

}
