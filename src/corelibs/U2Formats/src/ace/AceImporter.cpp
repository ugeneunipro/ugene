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
#include <QTemporaryFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/CloneObjectTask.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Timer.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/ImportWidget.h>

#include <U2Formats/AceFormat.h>

#include "AceImporter.h"
#include "CloneAssemblyWithReferenceToDbiTask.h"
#include "ConvertAceToSqliteTask.h"

namespace U2 {

///////////////////////////////////
//// AceImporterTask
///////////////////////////////////

AceImporterTask::AceImporterTask(const GUrl& url, const QVariantMap& settings) :
    DocumentProviderTask(tr("ACE file import: %1").arg(url.fileName()), TaskFlags_NR_FOSE_COSC),
    convertTask(NULL),
    loadDocTask(NULL),
    isSqliteDbTransit(false),
    settings(settings),
    srcUrl(url)
{
    documentDescription = srcUrl.fileName();
}

void AceImporterTask::prepare() {
    startTime = GTimer::currentTimeMicros();

    hintedDbiRef = settings.value(DocumentFormat::DBI_REF_HINT).value<U2DbiRef>();
    SAFE_POINT_EXT(hintedDbiRef.isValid(), setError(tr("Dbi ref is invalid")), );

    isSqliteDbTransit = SQLITE_DBI_ID != hintedDbiRef.dbiFactoryId;
    if (!isSqliteDbTransit) {
        localDbiRef = hintedDbiRef;
    } else {
        const QString tmpDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("assembly_conversion") + QDir::separator();
        QDir().mkpath(tmpDir);

        const QString pattern = tmpDir + "XXXXXX.ugenedb";
        QTemporaryFile *tempLocalDb = new QTemporaryFile(pattern, this);

        tempLocalDb->open();
        const QString filePath = tempLocalDb->fileName();
        tempLocalDb->close();

        SAFE_POINT_EXT(QFile::exists(filePath), setError(tr("Can't create a temporary database")), );

        localDbiRef = U2DbiRef(SQLITE_DBI_ID, filePath);
    }

    convertTask = new ConvertAceToSqliteTask(srcUrl, localDbiRef);
    addSubTask(convertTask);
}

QList<Task*> AceImporterTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    if (isSqliteDbTransit && convertTask == subTask) {
        initCloneObjectTasks();
        res << cloneTasks;
    }

    else if (isSqliteDbTransit && cloneTasks.contains(subTask)) {
        cloneTasks.removeOne(subTask);
        if (cloneTasks.isEmpty()) {
            initLoadDocumentTask();
            CHECK(NULL != loadDocTask, res);
            res << loadDocTask;
        }
    }

    else if (!isSqliteDbTransit && convertTask == subTask) {
        initLoadDocumentTask();
        CHECK(NULL != loadDocTask, res);
        res << loadDocTask;
    }

    if (loadDocTask == subTask) {
        resultDocument = loadDocTask->takeDocument();
    }

    return res;
}

Task::ReportResult AceImporterTask::report() {
    qint64 totalTime = GTimer::currentTimeMicros() - startTime;
    taskLog.info(QString("AceImporter task total time is %1 sec").arg((double)totalTime / 1000000));
    return ReportResult_Finished;
}

void AceImporterTask::initCloneObjectTasks() {
    const QMap<U2Sequence, U2Assembly> importedObjects = convertTask->getImportedObjects();
    foreach (const U2Sequence &reference, importedObjects.keys()) {
        cloneTasks << new CloneAssemblyWithReferenceToDbiTask(importedObjects[reference], reference, localDbiRef, hintedDbiRef, settings);
    }
}

void AceImporterTask::initLoadDocumentTask() {
    if (settings.value(AceImporter::LOAD_RESULT_DOCUMENT, true).toBool()) {
        loadDocTask = LoadDocumentTask::getDefaultLoadDocTask(convertTask->getDestinationUrl());
        if (loadDocTask == NULL) {
            setError(tr("Failed to get load task for : %1").arg(convertTask->getDestinationUrl().getURLString()));
        }
    }
}


///////////////////////////////////
//// AceImporter
///////////////////////////////////

const QString AceImporter::ID = "ace-importer";
const QString AceImporter::SRC_URL = "source_url";

AceImporter::AceImporter() :
    DocumentImporter(ID, tr("ACE file importer")) {
    ACEFormat aceFormat(NULL);
    extensions << aceFormat.getSupportedDocumentFileExtensions();
    formatIds << aceFormat.getFormatId();
    importerDescription = tr("ACE files importer is used to convert conventional ACE files into UGENE database format." \
                             "Having ACE file converted into UGENE DB format you get an fast and efficient interface " \
                             "to your data with an option to change the content");
    supportedObjectTypes << GObjectTypes::ASSEMBLY;
}

FormatCheckResult AceImporter::checkRawData(const QByteArray& rawData, const GUrl& url) {
    ACEFormat aceFormat(NULL);
    return aceFormat.checkRawData(rawData, url);
}

DocumentProviderTask* AceImporter::createImportTask(const FormatDetectionResult& res, bool, const QVariantMap &hints) {
    QVariantMap settings;
    settings.insert(SRC_URL, res.url.getURLString());

    AceImporterTask* task = NULL;
    if (hints.contains(DocumentFormat::DBI_REF_HINT)) {
        QVariant hint = hints.value(DocumentFormat::DBI_REF_HINT);
        settings.insert(DocumentFormat::DBI_REF_HINT, hint);
    }
    task = new AceImporterTask(res.url, settings);

    return task;
}

}   // namespace U2
