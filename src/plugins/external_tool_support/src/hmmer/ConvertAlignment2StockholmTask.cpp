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

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ConvertAlignment2StockholmTask.h"
#include "utils/ExportTasks.h"

namespace U2 {

ConvertAlignment2Stockholm::ConvertAlignment2Stockholm(const QString &msaUrl, const QString &workingDir)
    : Task(tr("Convert alignment to Stockholm format"), TaskFlags_NR_FOSE_COSC),
      loadTask(NULL),
      saveTask(NULL),
      msaUrl(msaUrl),
      workingDir(workingDir)
{
    SAFE_POINT_EXT(!msaUrl.isEmpty(), setError("Msa URL is empty"), );
}

const QString & ConvertAlignment2Stockholm::getResultUrl() const {
    return resultUrl;
}

void ConvertAlignment2Stockholm::prepare() {
    QVariantMap hints;
    hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    loadTask = LoadDocumentTask::getDefaultLoadDocTask(msaUrl);
    addSubTask(loadTask);
}

QList<Task *> ConvertAlignment2Stockholm::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK_OP(stateInfo, result);

    if (subTask == loadTask) {
        prepareResultUrl();
        CHECK_OP(stateInfo, result);

        prepareSaveTask();
        CHECK_OP(stateInfo, result);
        result << saveTask;
    }

    return result;
}

namespace {

const QString TEMP_DIR = "convert";

QString getTaskTempDirName(const QString &prefix, Task *task) {
    return prefix + QString::number(task->getTaskId()) + "_" +
            QDate::currentDate().toString("dd.MM.yyyy") + "_" +
            QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
            QString::number(QCoreApplication::applicationPid());
}

}

void ConvertAlignment2Stockholm::prepareResultUrl() {
    if (workingDir.isEmpty()) {
        QString tempDirName = getTaskTempDirName("convert_", this);
        workingDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(TEMP_DIR) + "/" + tempDirName;
    }
    resultUrl = workingDir + "/" + QFileInfo(msaUrl).baseName() + ".sto";

    QDir tempDir(workingDir);
    if (tempDir.exists()){
        ExternalToolSupportUtils::removeTmpDir(workingDir, stateInfo);
        CHECK_OP(stateInfo, );
    }
    if (!tempDir.mkpath(workingDir)){
        setError(tr("Cannot create a folder for temporary files."));
        return;
    }
}

void ConvertAlignment2Stockholm::prepareSaveTask() {
    Document *document = loadTask->takeDocument();
    QList<GObject *> objects = document->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    CHECK_EXT(!objects.isEmpty(), setError(tr("File doesn't contain any multiple alignments.")), );

    if (1 < objects.size()) {
        stateInfo.addWarning(tr("File contains several multiple alignments. Only the first one is saved to the result file."));
    }

    MultipleSequenceAlignmentObject *maObject = qobject_cast<MultipleSequenceAlignmentObject *>(objects.first());
    saveTask = new SaveAlignmentTask(maObject->getMultipleAlignment(), resultUrl, BaseDocumentFormats::STOCKHOLM);
    saveTask->setSubtaskProgressWeight(50);
}

}   // namespace U2
