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

#include <U2Core/DocumentModel.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include "ImportDirToDatabaseTask.h"
#include "ImportDocumentToDatabaseTask.h"
#include "ImportFileToDatabaseTask.h"
#include "ImportObjectToDatabaseTask.h"
#include "ImportToDatabaseTask.h"

namespace U2 {

ImportToDatabaseTask::ImportToDatabaseTask(QList<U2::Task *> tasks, int maxParallelSubtasks) :
    MultiTask(tr("Import to the database"), tasks, false, TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled | TaskFlag_PropagateSubtaskDesc)
{
    GCOUNTER(cvar, tvar, "ImportToDatabaseTask");
    setMaxParallelSubtasks(maxParallelSubtasks);
    startTime = GTimer::currentTimeMicros();
}

void ImportToDatabaseTask::run() {
    reportString = createReport();
}

QString ImportToDatabaseTask::generateReport() const {
    return reportString;
}

Task::ReportResult ImportToDatabaseTask::report() {
    qint64 endTime = GTimer::currentTimeMicros();
    ioLog.details(tr("Import complete: %1 seconds").arg((endTime - startTime) / 1000000.0));
    return ReportResult_Finished;
}

void ImportToDatabaseTask::sortSubtasks() const {
    foreach (const QPointer<Task> &subtask, getSubtasks()) {
        ImportDirToDatabaseTask* dirSubtask = qobject_cast<ImportDirToDatabaseTask*>(subtask.data());
        ImportDocumentToDatabaseTask* documentSubtask = qobject_cast<ImportDocumentToDatabaseTask*>(subtask.data());
        ImportFileToDatabaseTask* fileSubtask = qobject_cast<ImportFileToDatabaseTask*>(subtask.data());
        ImportObjectToDatabaseTask* objectSubtask = qobject_cast<ImportObjectToDatabaseTask*>(subtask.data());

        if (NULL != dirSubtask) {
            dirSubtasks << dirSubtask;
        } else if (NULL != documentSubtask) {
            documentSubtasks << documentSubtask;
        } else if (NULL != fileSubtask) {
            fileSubtasks << fileSubtask;
        } else if (NULL != objectSubtask) {
            objectSubtasks << objectSubtask;
        }
    }
}

QString ImportToDatabaseTask::createReport() const {
    QString report;

    if (isCanceled()) {
        report += tr("The import task was cancelled.");
    } else if (hasError()) {
        report += tr("The import task has failed.");
    } else {
        report += tr("The import task has finished.");
    }

    report += "<br><br><br>";
    sortSubtasks();

    const QString importedFilesAndDirs = sayAboutImportedFilesAndDirs();
    const QString importedProjectItems = sayAboutImportedProjectItems();
    const QString skippedFilesAndDirs = sayAboutSkippedFilesAndDirs();
    const QString skippedProjectItems = sayAboutSkippedProjectItems();

    if (!importedFilesAndDirs.isEmpty()) {
        report += "<hr></hr>";
        report += importedFilesAndDirs;
    }

    if (!importedProjectItems.isEmpty()) {
        report += "<hr></hr>";
        report += importedProjectItems;
    }

    if (!skippedFilesAndDirs.isEmpty()) {
        report += "<hr></hr>";
        report += skippedFilesAndDirs;
    }

    if (!skippedProjectItems.isEmpty()) {
        report += "<hr></hr>";
        report += skippedProjectItems;
    }

    return report;
}

QString ImportToDatabaseTask::sayAboutImportedFilesAndDirs() const {
    CHECK(!dirSubtasks.isEmpty() || !fileSubtasks.isEmpty(), "");

    const QString dirsResult = sayAboutImportedDirs();
    const QString filesResult = sayAboutImportedFiles();
    CHECK(!dirsResult.isEmpty() || !filesResult.isEmpty(), "");

    return tr("Successfully imported files:<br><br>") + dirsResult + filesResult;
}

QString ImportToDatabaseTask::sayAboutImportedDirs() const {
    QString result;

    foreach (ImportDirToDatabaseTask* dirSubtask, dirSubtasks) {
        const QStringList importedFiles = dirSubtask->getImportedFiles();
        foreach (const QString& importedFile, importedFiles) {
            result += importedFile + "<br>";
        }
    }

    return result;
}

QString ImportToDatabaseTask::sayAboutImportedFiles() const {
    QString result;

    foreach (ImportFileToDatabaseTask* fileSubtask, fileSubtasks) {
        if (fileSubtask->isCanceled() || fileSubtask->hasError()) {
            continue;
        }

        result += fileSubtask->getFilePath() + "<br>";
    }

    return result;
}

QString ImportToDatabaseTask::sayAboutImportedProjectItems() const {
    CHECK(!documentSubtasks.isEmpty() || !objectSubtasks.isEmpty(), "");

    const QString documentsResult = sayAboutImportedDocuments();
    const QString objectsResult = sayAboutImportedObjects();
    CHECK(!documentsResult.isEmpty() || !objectsResult.isEmpty(), "");

    return tr("Successfully imported objects:<br><br>") + documentsResult + objectsResult;
}

QString ImportToDatabaseTask::sayAboutImportedDocuments() const {
    QString result;

    foreach (ImportDocumentToDatabaseTask* documentSubtask, documentSubtasks) {
        Document* document = documentSubtask->getSourceDocument();
        if (NULL == document) {
            continue;
        }

        const QStringList importedObjectNames = documentSubtask->getImportedObjectNames();
        if (importedObjectNames.isEmpty()) {
            continue;
        }

        result += tr("Document " ) + document->getName() + ":<br>";

        foreach (const QString& importedObjectName, importedObjectNames) {
            result += "    " + importedObjectName + "<br>";
        }

        result += "<br>";
    }

    return result;
}

QString ImportToDatabaseTask::sayAboutImportedObjects() const {
    QString result;

    foreach (ImportObjectToDatabaseTask* objectSubtask, objectSubtasks) {
        if (objectSubtask->isCanceled() || objectSubtask->hasError()) {
            continue;
        }

        GObject* object = objectSubtask->getSourceObject();
        if (NULL != object) {
            result += object->getGObjectName() + "<br>";
        }
    }
    CHECK(!result.isEmpty(), result);

    return tr("Just objects without a document:") + "<br>" + result;
}

QString ImportToDatabaseTask::sayAboutSkippedFilesAndDirs() const {
    CHECK(!dirSubtasks.isEmpty() || !fileSubtasks.isEmpty(), "");

    const QString dirsResult = sayAboutSkippedDirs();
    const QString filesResult = sayAboutSkippedFiles();
    CHECK(!dirsResult.isEmpty() || !filesResult.isEmpty(), "");

    return tr("Not imported files:<br><br>") + dirsResult + filesResult;
}

QString ImportToDatabaseTask::sayAboutSkippedDirs() const {
    QString result;

    foreach (ImportDirToDatabaseTask* dirSubtask, dirSubtasks) {
        const StrStrMap skippedFiles = dirSubtask->getSkippedFiles();
        foreach (const QString& skippedFile, skippedFiles.keys()) {
            result += skippedFile + ": " + skippedFiles[skippedFile] + "<br>";
        }
    }

    return result;
}

QString ImportToDatabaseTask::sayAboutSkippedFiles() const {
    QString result;

    foreach (ImportFileToDatabaseTask* fileSubtask, fileSubtasks) {
        if (fileSubtask->isCanceled()) {
            result += fileSubtask->getFilePath() + ": " + tr("Import was cancelled") + "<br>";
        } else if (fileSubtask->hasError()) {
            result += fileSubtask->getFilePath() + ": " + fileSubtask->getError() + "<br>";
        }
    }

    return result;
}

QString ImportToDatabaseTask::sayAboutSkippedProjectItems() const {
    CHECK(!documentSubtasks.isEmpty() || !objectSubtasks.isEmpty(), "");

    const QString documentsResult = sayAboutSkippedDocuments();
    const QString objectsResult = sayAboutSkippedObjects();
    CHECK(!documentsResult.isEmpty() || !objectsResult.isEmpty(), "");

    return tr("Not imported objects:<br><br>") + documentsResult + objectsResult;
}

QString ImportToDatabaseTask::sayAboutSkippedDocuments() const {
    QString result;

    foreach (ImportDocumentToDatabaseTask* documentSubtask, documentSubtasks) {
        Document* document = documentSubtask->getSourceDocument();
        if (NULL == document) {
            continue;
        }

        const QStringList skippedObjectNames = documentSubtask->getSkippedObjectNames();
        if (skippedObjectNames.isEmpty()) {
            continue;
        }

        result += tr("Document " ) + document->getName() + ":<br>";

        foreach (const QString& skippedObjectName, skippedObjectNames) {
            result += "    " + skippedObjectName + "<br>";
        }

        result += "<br>";
    }

    return result;
}

QString ImportToDatabaseTask::sayAboutSkippedObjects() const {
    QString result;

    foreach (ImportObjectToDatabaseTask* objectSubtask, objectSubtasks) {
        if (!objectSubtask->isCanceled() && !objectSubtask->hasError()) {
            continue;
        }

        GObject* object = objectSubtask->getSourceObject();
        if (NULL != object) {
            result += object->getGObjectName() + "<br>";
        }
    }
    CHECK(!result.isEmpty(), result);

    return tr("Just objects without a document:") + "<br>" + result;
}

}   // namespace U2
