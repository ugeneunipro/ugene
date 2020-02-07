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

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/ConvertFileTask.h>

#include "PrepareInputFastaFilesTask.h"
#include "FormatDBSupport.h"
#include "FormatDBSupportTask.h"

namespace U2 {

void FormatDBSupportTaskSettings::reset() {
    inputFilesPath = QList<QString>();
    outputPath = "";
    databaseTitle = "";
    isInputAmino = true;
    tempDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(FormatDBSupport::FORMATDB_TMP_DIR);
}

FormatDBSupportTask::FormatDBSupportTask(const QString& id, const FormatDBSupportTaskSettings& _settings) :
        Task(tr("Run NCBI FormatDB task"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
        prepareTask(NULL),
        formatDBTask(NULL),
        toolId(id),
        settings(_settings)
{
    GCOUNTER(cvar, tvar, "FormatDBSupportTask");
}

void FormatDBSupportTask::prepare(){
    const QString tempDir = prepareTempDir();
    CHECK_OP(stateInfo, );

    prepareTask = new PrepareInputFastaFilesTask(settings.inputFilesPath, tempDir);
    addSubTask(prepareTask);
}

QList<Task *> FormatDBSupportTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK(subTask != NULL, result);
    CHECK(!subTask->isCanceled() && !subTask->hasError(), result);

    if (prepareTask == subTask) {
        inputFastaFiles << prepareTask->getFastaFiles();
        fastaTmpFiles << prepareTask->getTempFiles();
        createFormatDbTask();
        CHECK_OP(stateInfo, result);
        result << formatDBTask;
    }

    return result;
}

Task::ReportResult FormatDBSupportTask::report() {
    // remove tmp files
    if (!fastaTmpFiles.isEmpty()) {
        QDir dir(QFileInfo(fastaTmpFiles.first()).absoluteDir());
        if (!dir.removeRecursively()) {
            stateInfo.addWarning(tr("Can not remove folder for temporary files."));
            emit si_stateChanged();
        }
    }
    return ReportResult_Finished;
}

QString FormatDBSupportTask::generateReport() const {
    QString res;
    if (isCanceled()) {
        res += QString(tr("Blast database creation has been cancelled")) + "<br>";
        if (QFile::exists(externalToolLog)) {
            res += prepareLink(externalToolLog);
        }
        return res;
    }
    if (!hasError()) {
        res += QString(tr("Blast database has been successfully created") + "<br><br>");
        res += QString(tr("Source sequences: "));
        foreach(const QString &filePath, settings.inputFilesPath){
            res += prepareLink(filePath);
            if(filePath.size() > 1){
                res += "<br>    ";
            }
        }
        res += "<br>";
        res += QString(tr("Database file path: %1")).arg(QDir::toNativeSeparators(settings.outputPath)) + "<br>";
        QString type = settings.isInputAmino ? "protein" : "nucleotide";
        res += QString(tr("Type: %1")).arg(type) + "<br>";
        if (QFile::exists(externalToolLog)) {
            res += QString(tr("Formatdb log file path: "));
            res += prepareLink(externalToolLog);
        }
    }else{
        res += QString(tr("Blast database creation has been failed")) + "<br><br>";
        if (QFile::exists(externalToolLog)) {
            res += QString(tr("Formatdb log file path: "));
            res += prepareLink(externalToolLog);
        }
    }
    return res;
}

namespace {

QString getTempDirName(qint64 taskId) {
    return "FormatDB_" + QString::number(taskId) + "_" +
            QDate::currentDate().toString("dd.MM.yyyy") + "_" +
            QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
            QString::number(QCoreApplication::applicationPid()) + "/";
}

}

QString FormatDBSupportTask::prepareTempDir() {
    const QString tmpDirName = getTempDirName(getTaskId());
    const QString tmpDir = GUrlUtils::prepareDirLocation(settings.tempDirPath + "/" + tmpDirName, stateInfo);
    CHECK_OP(stateInfo, "");
    CHECK_EXT(!tmpDir.isEmpty(), setError(tr("Cannot create temp folder")), "");
    return tmpDir;
}

QString FormatDBSupportTask::prepareLink( const QString &path ) const {
    QString preparedPath = path;
    if(preparedPath.startsWith("'") || preparedPath.startsWith("\"")) {
        preparedPath.remove(0,1);
    }
    if (preparedPath.endsWith("'") || preparedPath.endsWith("\"")) {
        preparedPath.chop(1);
    }
    return "<a href=\"file:///" + QDir::toNativeSeparators(preparedPath) + "\">" +
        QDir::toNativeSeparators(preparedPath) + "</a><br>";
}

void FormatDBSupportTask::createFormatDbTask() {
    SAFE_POINT_EXT(formatDBTask == NULL, setError(tr("Trying to initialize Format DB task second time")), );

    QStringList arguments;
    assert(toolId == FormatDBSupport::ET_MAKEBLASTDB_ID);
    for (int i = 0; i < inputFastaFiles.length(); i++){
        inputFastaFiles[i] = "\"" + inputFastaFiles[i] + "\"";
    }
    arguments << "-in" << inputFastaFiles.join(" ");
    arguments <<"-logfile"<< settings.outputPath + "MakeBLASTDB.log";
    externalToolLog = settings.outputPath + "MakeBLASTDB.log";
    if(settings.outputPath.contains(" ")){
        stateInfo.setError(tr("Output database path contain space characters."));
        return;
    }
    arguments <<"-out"<< settings.outputPath;
    arguments <<"-dbtype"<< (settings.isInputAmino ? "prot" : "nucl");

    formatDBTask = new ExternalToolRunTask(toolId, arguments, new ExternalToolLogParser());
    formatDBTask->setSubtaskProgressWeight(95);
}

}   // namespace U2
