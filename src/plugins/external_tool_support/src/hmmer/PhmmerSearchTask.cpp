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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "HmmerParseSearchResultsTask.h"
#include "HmmerSupport.h"
#include "PhmmerSearchTask.h"
#include "utils/ExportTasks.h"

#include "HmmerBuildTask.h"

namespace U2 {

const QString PhmmerSearchTask::INPUT_SEQUENCE_FILENAME = "input_sequence.fa";
const QString PhmmerSearchTask::PER_DOMAIN_HITS_FILENAME = "per_domain_hits.txt";

PhmmerSearchTask::PhmmerSearchTask(const PhmmerSearchSettings &settings)
    : ExternalToolSupportTask(tr("Search with phmmer"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsEnabled | TaskFlag_ReportingIsSupported),
      settings(settings),
      saveSequenceTask(NULL),
      phmmerTask(NULL),
      parseTask(NULL),
      removeWorkingDir(false)
{
    GCOUNTER(cvar, tvar, "HMMER Search");
    SAFE_POINT_EXT(settings.validate(), setError("Settings are invalid"), );
}

QList<SharedAnnotationData> PhmmerSearchTask::getAnnotations() const {
    CHECK(NULL != parseTask, QList<SharedAnnotationData>());
    return parseTask->getAnnotations();
}

void PhmmerSearchTask::prepare() {
    prepareWorkingDir();

    if (settings.targetSequenceUrl.isEmpty()) {
        SAFE_POINT_EXT(NULL != settings.targetSequence, setError(L10N::nullPointerError("sequence object")), );
        prepareSequenceSaveTask();
        addSubTask(saveSequenceTask);
    } else {
        preparePhmmerTask();
        addSubTask(phmmerTask);
    }
}

QList<Task *> PhmmerSearchTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK_OP(stateInfo, result);

    if (subTask == saveSequenceTask) {
        preparePhmmerTask();
        result << phmmerTask;
    } else if (subTask == phmmerTask) {
        parseTask = new HmmerParseSearchResultsTask(settings.workingDir + "/" + PER_DOMAIN_HITS_FILENAME, settings.pattern);
        parseTask->setSubtaskProgressWeight(5);
        result << parseTask;
    } else if (subTask == parseTask) {
        removeTempDir();
        if (settings.annotationTable != NULL) {
            Task *createAnnotationsTask = new CreateAnnotationsTask(settings.annotationTable, parseTask->getAnnotations(), settings.pattern.groupName);
            createAnnotationsTask->setSubtaskProgressWeight(5);
            result << createAnnotationsTask;
        }
    }

    return result;
}

QString PhmmerSearchTask::generateReport() const {
    QString res;
    res += "<table>";
    res += "<tr><td><b>" + tr("Query sequence: ") + "</b></td><td>" + QFileInfo(settings.querySequenceUrl).absoluteFilePath() + "</td></tr>";

    if (hasError() || isCanceled()) {
        res += "<tr><td><b>" + tr("Task was not finished") + "</b></td><td></td></tr>";
        res += "</table>";
        return res;
    }

    if (NULL != settings.annotationTable && NULL != settings.annotationTable->getDocument()) {
        res += "<tr><td><b>" + tr("Result annotation table: ") + "</b></td><td>" + settings.annotationTable->getDocument()->getName() + "</td></tr>";
    }
    res += "<tr><td><b>" + tr("Result annotation group: ") + "</b></td><td>" + settings.pattern.groupName + "</td></tr>";
    res += "<tr><td><b>" + tr("Result annotation name: ") + "</b></td><td>" + settings.pattern.annotationName + "</td></tr>";

    res += "<tr><td><b>" + tr("Results count: ") + "</b></td><td>" + QString::number(getAnnotations().size()) + "</td></tr>";
    res += "</table>";
    return res;
}

namespace {

const QString PHMMER_TEMP_DIR = "phmmer";

QString getTaskTempDirName(const QString &prefix, Task *task) {
    return prefix + QString::number(task->getTaskId()) + "_" +
            QDate::currentDate().toString("dd.MM.yyyy") + "_" +
            QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
            QString::number(QCoreApplication::applicationPid());
}

}

void PhmmerSearchTask::prepareWorkingDir() {
    if (settings.workingDir.isEmpty()) {
        QString tempDirName = getTaskTempDirName("phmmer_search_", this);
        settings.workingDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(PHMMER_TEMP_DIR) + "/" + tempDirName;
        removeWorkingDir = true;
    }

    QDir tempDir(settings.workingDir);
    if (tempDir.exists()) {
        ExternalToolSupportUtils::removeTmpDir(settings.workingDir, stateInfo);
        CHECK_OP(stateInfo, );
    }

    if (!tempDir.mkpath(settings.workingDir)) {
        setError(tr("Cannot create a folder for temporary files."));
        return;
    }
}

void PhmmerSearchTask::removeTempDir() const {
    CHECK(removeWorkingDir, );
    U2OpStatusImpl os;
    ExternalToolSupportUtils::removeTmpDir(settings.workingDir, os);
}

QStringList PhmmerSearchTask::getArguments() const {
    QStringList arguments;

    if (PhmmerSearchSettings::OPTION_NOT_SET != settings.t) {
        arguments << "-T" << QString::number(settings.t);
    } else {
        arguments << "-E" << QString::number(settings.e);
    }

    if (PhmmerSearchSettings::OPTION_NOT_SET != settings.z) {
        arguments << "-Z" << QString::number(settings.z);
    }

    if (PhmmerSearchSettings::OPTION_NOT_SET != settings.domT) {
        arguments << "--domT" << QString::number(settings.domT);
    } else if (PhmmerSearchSettings::OPTION_NOT_SET != settings.domE) {
        arguments << "--domE" << QString::number(settings.domE);
    }

    if (PhmmerSearchSettings::OPTION_NOT_SET != settings.domZ) {
        arguments << "--domZ" << QString::number(settings.domZ);
    }

    if (settings.doMax) {
        arguments << "--max";
    } else {
        arguments << "--F1" << QString::number(settings.f1);
        arguments << "--F2" << QString::number(settings.f2);
        arguments << "--F3" << QString::number(settings.f3);
    }

    if (settings.noBiasFilter) {
        arguments << "--nobias";
    }

    if (settings.noNull2) {
        arguments << "--nonull2";
    }

    arguments << "--EmL" << QString::number(settings.eml);
    arguments << "--EmN" << QString::number(settings.emn);
    arguments << "--EvL" << QString::number(settings.evl);
    arguments << "--EvN" << QString::number(settings.evn);
    arguments << "--EfL" << QString::number(settings.efl);
    arguments << "--EfN" << QString::number(settings.efn);
    arguments << "--Eft" << QString::number(settings.eft);

    arguments << "--popen" << QString::number(settings.popen);
    arguments << "--pextend" << QString::number(settings.pextend);

    arguments << "--seed" << QString::number(settings.seed);
    arguments << "--cpu" << QString::number(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());

    arguments << "--noali";
    arguments << "--domtblout" << settings.workingDir + "/" + PER_DOMAIN_HITS_FILENAME;

    arguments << settings.querySequenceUrl;
    arguments << settings.targetSequenceUrl;

    return arguments;
}

void PhmmerSearchTask::prepareSequenceSaveTask() {
    settings.targetSequenceUrl = settings.workingDir + "/" + INPUT_SEQUENCE_FILENAME;
    saveSequenceTask = new SaveSequenceTask(settings.targetSequence, settings.targetSequenceUrl, BaseDocumentFormats::FASTA);
    saveSequenceTask->setSubtaskProgressWeight(5);
}

void PhmmerSearchTask::preparePhmmerTask() {
    phmmerTask = new ExternalToolRunTask(HmmerSupport::PHMMER_TOOL_ID, getArguments(), new ExternalToolLogParser());
    setListenerForTask(phmmerTask);
    phmmerTask->setSubtaskProgressWeight(85);
}

}   // namespace U2
