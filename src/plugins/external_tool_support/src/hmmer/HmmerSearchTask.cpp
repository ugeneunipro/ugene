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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "HmmerParseSearchResultsTask.h"
#include "HmmerSearchTask.h"
#include "HmmerSupport.h"
#include "utils/ExportTasks.h"

namespace U2 {

const QString HmmerSearchTask::INPUT_SEQUENCE_FILENAME = "input_sequence.fa";
const QString HmmerSearchTask::PER_DOMAIN_HITS_FILENAME = "per_domain_hits.txt";

HmmerSearchTask::HmmerSearchTask(const HmmerSearchSettings &settings)
    : ExternalToolSupportTask(tr("HMMER search"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsEnabled | TaskFlag_ReportingIsSupported),
      settings(settings),
      saveSequenceTask(NULL),
      hmmerTask(NULL),
      parseTask(NULL),
      removeWorkingDir(false),
      hmm2Mode(false)
{
    GCOUNTER(cvar, tvar, "HMMER Search");
    SAFE_POINT_EXT(settings.validate(), setError("Settings are invalid"), );
}

QList<SharedAnnotationData> HmmerSearchTask::getAnnotations() const {
    CHECK(NULL != parseTask, QList<SharedAnnotationData>());
    return parseTask->getAnnotations();
}

void HmmerSearchTask::prepare() {
    prepareWorkingDir();

    if (settings.sequenceUrl.isEmpty()) {
        SAFE_POINT_EXT(NULL != settings.sequence, setError(L10N::nullPointerError("sequence object")), );
        prepareSequenceSaveTask();
        addSubTask(saveSequenceTask);
    } else {
        prepareHmmerTask();
        addSubTask(hmmerTask);
    }
}

QList<Task *> HmmerSearchTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK_OP(stateInfo, result);

    if (subTask == saveSequenceTask) {
        prepareHmmerTask();
        result << hmmerTask;
    } else if (subTask == hmmerTask) {
        prepareParseTask();
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

QString HmmerSearchTask::generateReport() const {
    QString res;
    res += "<table>";
    res += "<tr><td><b>" + tr("HMM profile used: ") + "</b></td><td>" + QFileInfo(settings.hmmProfileUrl).absoluteFilePath()
           + ( hmm2Mode
            ? " <br>Warning: it is not recommended to use HMMER2 models with HMMER3. Details: https://cryptogenomicon.org/2009/03/25/using-hmmer2-models-with-hmmer3-dont-do-that/"
            : "")
            + "</td></tr>";

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

const QString HMMER_TEMP_DIR = "hmmer";

QString getTaskTempDirName(const QString &prefix, Task *task) {
    return prefix + QString::number(task->getTaskId()) + "_" +
            QDate::currentDate().toString("dd.MM.yyyy") + "_" +
            QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
            QString::number(QCoreApplication::applicationPid());
}

}

void HmmerSearchTask::prepareWorkingDir() {
    if (settings.workingDir.isEmpty()) {
        QString tempDirName = getTaskTempDirName("hmmer_search_", this);
        settings.workingDir = GUrlUtils::rollFileName(AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(HMMER_TEMP_DIR) + "/" + tempDirName, "_");
        removeWorkingDir = true;
    }

    QDir tempDir(settings.workingDir);
    if (!tempDir.mkpath(settings.workingDir)) {
        setError(tr("Cannot create a folder for temporary files."));
        return;
    }
}

void HmmerSearchTask::removeTempDir() const {
    CHECK(removeWorkingDir, );
    U2OpStatusImpl os;
    ExternalToolSupportUtils::removeTmpDir(settings.workingDir, os);
}

QStringList HmmerSearchTask::getArguments() const {
    QStringList arguments;

    arguments << "-E" << QString::number(settings.e);
    if (HmmerSearchSettings::OPTION_NOT_SET != settings.t) {
        arguments << "-T" << QString::number(settings.t);
    }

    if (HmmerSearchSettings::OPTION_NOT_SET != settings.z) {
        arguments << "-Z" << QString::number(settings.z);
    }

    if (HmmerSearchSettings::OPTION_NOT_SET != settings.domE) {
        arguments << "--domE" << QString::number(settings.domE);
    }

    if (HmmerSearchSettings::OPTION_NOT_SET != settings.domT) {
        arguments << "--domT" << QString::number(settings.domT);
    }

    if (HmmerSearchSettings::OPTION_NOT_SET != settings.domZ) {
        arguments << "--domZ" << QString::number(settings.domZ);
    }

    switch (settings.useBitCutoffs) {
    case HmmerSearchSettings::None:
        break;
    case HmmerSearchSettings::p7H_GA:
        arguments << "--cut_ga";
        break;
    case HmmerSearchSettings::p7H_TC:
        arguments << "--cut_nc";
        break;
    case HmmerSearchSettings::p7H_NC:
        arguments << "--cut_tc";
        break;
    default:
        FAIL(tr("Unknown option controlling model-specific thresholding"), arguments);
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

    arguments << "--seed" << QString::number(settings.seed);
    arguments << "--cpu" << QString::number(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());

    if (settings.noali) {
        arguments << "--noali";
    }

    arguments << "--domtblout" << settings.workingDir + "/" + PER_DOMAIN_HITS_FILENAME;
    arguments << settings.hmmProfileUrl;
    arguments << settings.sequenceUrl;

    return arguments;
}

void HmmerSearchTask::prepareSequenceSaveTask() {
    settings.sequenceUrl = settings.workingDir + "/" + INPUT_SEQUENCE_FILENAME;
    saveSequenceTask = new SaveSequenceTask(settings.sequence, settings.sequenceUrl, BaseDocumentFormats::FASTA);
    saveSequenceTask->setSubtaskProgressWeight(5);
}

static bool isHmm2Profile(const QString& url) {
    QByteArray header = IOAdapterUtils::readFileHeader(GUrl(url), 6);
    return header.startsWith("HMMER2");
}

void HmmerSearchTask::prepareHmmerTask() {
    hmmerTask = new ExternalToolRunTask(HmmerSupport::SEARCH_TOOL_ID, getArguments(), new Hmmer3LogParser());
    setListenerForTask(hmmerTask);
    hmmerTask->setSubtaskProgressWeight(85);
    hmm2Mode = isHmm2Profile(settings.hmmProfileUrl);
    if (hmm2Mode) {
        stateInfo.addWarning(tr("Warning: it is not recommended to use HMMER2 models with HMMER3"));
    }
}

void HmmerSearchTask::prepareParseTask() {
    parseTask = new HmmerParseSearchResultsTask(settings.workingDir + "/" + PER_DOMAIN_HITS_FILENAME, settings.pattern);
    parseTask->setSubtaskProgressWeight(5);
}

}   // namespace U2
