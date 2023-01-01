/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MakeBlastDbTask.h"

#include <QCoreApplication>
#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/ConvertFileTask.h>

#include "BlastSupport.h"
#include "PrepareInputFastaFilesTask.h"

namespace U2 {

void MakeBlastDbSettings::reset() {
    inputFilesPath = QList<QString>();
    outputPath = "";
    databaseTitle = "";
    isInputAmino = true;
    tempDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(BlastSupport::BLAST_TMP_DIR);
}

MakeBlastDbTask::MakeBlastDbTask(const MakeBlastDbSettings& _settings)
    : Task(tr("Run 'MakeBlastDbTask' task"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      settings(_settings) {
    GCOUNTER(cvar, "MakeBlastDbTask");
    externalToolLog = settings.outputPath + "MakeBLASTDB.log";
}

void MakeBlastDbTask::prepare() {
    QString tempDir = prepareTempDir();
    CHECK_OP(stateInfo, );

    prepareTask = new PrepareInputFastaFilesTask(settings.inputFilesPath, tempDir);
    addSubTask(prepareTask);
}

QList<Task*> MakeBlastDbTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(subTask != nullptr, result);
    CHECK(!subTask->isCanceled() && !subTask->hasError(), result);

    if (subTask == prepareTask) {
        inputFastaFiles << prepareTask->getFastaFiles();
        fastaTmpFiles << prepareTask->getTempFiles();
        initMakeBlastDbExternalToolTask();
        CHECK_OP(stateInfo, result);
        result << makeBlastDbExternalToolTask;
    }

    return result;
}

Task::ReportResult MakeBlastDbTask::report() {
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

QString MakeBlastDbTask::generateReport() const {
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
        foreach (const QString& filePath, settings.inputFilesPath) {
            res += prepareLink(filePath);
            if (filePath.size() > 1) {
                res += "<br>    ";
            }
        }
        res += "<br>";
        res += QString(tr("Database file path: %1")).arg(QDir::toNativeSeparators(settings.outputPath)) + "<br>";
        QString type = settings.isInputAmino ? "protein" : "nucleotide";
        res += QString(tr("Type: %1")).arg(type) + "<br>";
        if (QFile::exists(externalToolLog)) {
            res += QString(tr("Log file path: "));
            res += prepareLink(externalToolLog);
        }
    } else {
        res += QString(tr("Blast database creation has been failed")) + "<br><br>";
        if (QFile::exists(externalToolLog)) {
            res += QString(tr("Log file path: "));
            res += prepareLink(externalToolLog);
        }
    }
    return res;
}

namespace {

QString getTempDirName(qint64 taskId) {
    return "makeblastdb_" + QString::number(taskId) + "_" +
           QDate::currentDate().toString("dd.MM.yyyy") + "_" +
           QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
           QString::number(QCoreApplication::applicationPid()) + "/";
}

}  // namespace

QString MakeBlastDbTask::prepareTempDir() {
    const QString tmpDirName = getTempDirName(getTaskId());
    const QString tmpDir = GUrlUtils::prepareDirLocation(settings.tempDirPath + "/" + tmpDirName, stateInfo);
    CHECK_OP(stateInfo, "");
    CHECK_EXT(!tmpDir.isEmpty(), setError(tr("Cannot create temp folder")), "");
    return tmpDir;
}

QString MakeBlastDbTask::prepareLink(const QString& path) const {
    QString preparedPath = path;
    if (preparedPath.startsWith("'") || preparedPath.startsWith("\"")) {
        preparedPath.remove(0, 1);
    }
    if (preparedPath.endsWith("'") || preparedPath.endsWith("\"")) {
        preparedPath.chop(1);
    }
    return "<a href=\"file:///" + QDir::toNativeSeparators(preparedPath) + "\">" +
           QDir::toNativeSeparators(preparedPath) + "</a><br>";
}

void MakeBlastDbTask::initMakeBlastDbExternalToolTask() {
    SAFE_POINT_EXT(makeBlastDbExternalToolTask == nullptr, setError(tr("Trying to initialize Format DB task second time")), );
    if (settings.outputPath.contains(" ")) {
        stateInfo.setError(tr("Output database path contain space characters."));
        return;
    }
    if (inputFastaFiles.isEmpty()) {
        stateInfo.setError(tr("Input file set is empty."));
        return;
    }

    for (int i = 0; i < inputFastaFiles.length(); i++) {
        inputFastaFiles[i] = "\"" + inputFastaFiles[i] + "\"";
    }
    QStringList arguments;
    arguments << "-in" << inputFastaFiles.join(" ");
    arguments << "-logfile" << settings.outputPath + "MakeBLASTDB.log";
    arguments << "-out" << settings.outputPath;
    arguments << "-dbtype" << (settings.isInputAmino ? "prot" : "nucl");

    makeBlastDbExternalToolTask = new ExternalToolRunTask(BlastSupport::ET_MAKEBLASTDB_ID, arguments, new ExternalToolLogParser());
    if (isOsWindows()) {
        // Blast 2.10.1 has some issues with Windows. See https://www.biostars.org/p/413294/#415002
        // TODO: recheck if this fix is still needed with Blast 2.12 or later and recheck the MAP_SIZE value below.
        QMap<QString, QString> env;
        env["BLASTDB_LMDB_MAP_SIZE"] = "1000000";
        makeBlastDbExternalToolTask->setAdditionalEnvVariables(env);
    }
    makeBlastDbExternalToolTask->setSubtaskProgressWeight(95);
}

}  // namespace U2
