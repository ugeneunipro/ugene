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
#include <U2Core/DataPathRegistry.h>
#include <U2Core/IOAdapterUtils.h>

#include <U2Formats/TabulatedFormatReader.h>

#include "PrepareWevoteTaxonomyDataTask.h"
#include "WevoteSupport.h"
#include "WevoteTask.h"

namespace U2 {

WevoteTaskSettings::WevoteTaskSettings()
    : penalty(2),
      numberOfAgreedTools(0),
      scoreThreshold(0),
      numberOfThreads(1)
{

}

const QString WevoteTask::SUFFIX = "_WEVOTE_Details.txt";

WevoteTask::WevoteTask(const WevoteTaskSettings &_settings, WorkflowProcess &_workflowProcess)
    : ExternalToolSupportTask(tr("Improve classification with WEVOTE"), TaskFlags_FOSE_COSC),
      settings(_settings),
      workflowProcess(_workflowProcess),
      prepareTaxonomyTask(NULL),
      wevotePrefix(settings.workingDir + "/" + QFileInfo(settings.outputFileUrl).completeBaseName())
{
    checkSettings();
    CHECK_OP(stateInfo, );
}

const QString &WevoteTask::getClassificationUrl() const {
    return settings.outputFileUrl;
}

const LocalWorkflow::TaxonomyClassificationResult &WevoteTask::getClassification() const {
    return classification;
}

void WevoteTask::prepare() {
    prepareTaxonomyTask = new PrepareWevoteTaxonomyDataTask(workflowProcess);
    addSubTask(prepareTaxonomyTask);
}

QList<Task *> WevoteTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> newSubTasks;
    CHECK_OP(stateInfo, newSubTasks);

    if (subTask == prepareTaxonomyTask) {
        const QStringList arguments = getArguments();
        CHECK_OP(stateInfo, newSubTasks);

        ExternalToolRunTask *wevoteTask = new ExternalToolRunTask(WevoteSupport::TOOL_ID, arguments, new ExternalToolLogParser());
        setListenerForTask(wevoteTask);
        newSubTasks << wevoteTask;
    }

    return newSubTasks;
}

void WevoteTask::run() {
    CHECK_EXT(QFileInfo(wevotePrefix + SUFFIX).exists(), setError(tr("Wevote output file not found")), );

    moveFile();
    CHECK_OP(stateInfo, );

    parseClassification();
    CHECK_OP(stateInfo, );
}

void WevoteTask::checkSettings() {
    SAFE_POINT_EXT(settings.penalty > 0, setError("Penalty is less or equal zero"), );
    SAFE_POINT_EXT(settings.numberOfAgreedTools >= 0, setError(tr("Number of agreed tools is less than zero")), );
    SAFE_POINT_EXT(settings.scoreThreshold >= 0, setError(tr("Score threshold is less than zero")), );
    SAFE_POINT_EXT(settings.numberOfThreads > 0, setError(tr("Number of threads is less or equal zero")), );
    SAFE_POINT_EXT(!settings.inputFileUrl.isEmpty(), setError(tr("Input file URL is empty")), );
    SAFE_POINT_EXT(!settings.outputFileUrl.isEmpty(), setError(tr("Output file URL is empty")), );
}

QStringList WevoteTask::getArguments() {
    QStringList arguments;
    arguments << "-v";
    arguments << "-i" << settings.inputFileUrl;
    arguments << "-p" << wevotePrefix;

    SAFE_POINT_EXT(NULL != prepareTaxonomyTask, setError("prepareTaxonomyTask is NULL"), arguments);
    const QString wevoteTaxonomyDir = prepareTaxonomyTask->getWevoteTaxonomyDir();
    CHECK_EXT(!wevoteTaxonomyDir.isEmpty(), setError(tr("Can't find prepared taxonomy for WEVOTE")), arguments);
    arguments << "-d" << prepareTaxonomyTask->getWevoteTaxonomyDir();

    arguments << "-s" << QString::number(settings.scoreThreshold);
    arguments << "-n" << QString::number(settings.numberOfThreads);
    arguments << "-k" << QString::number(settings.penalty);
    arguments << "-a" << QString::number(settings.numberOfAgreedTools);

    return arguments;
}

void WevoteTask::moveFile() {
    CHECK(wevotePrefix + SUFFIX != settings.outputFileUrl, );
    const bool success = QFile::rename(wevotePrefix + SUFFIX, settings.outputFileUrl);
    CHECK_EXT(success, setError(tr("Can't overwrite the file \"%1\"").arg(settings.outputFileUrl)), );
}

void WevoteTask::parseClassification() {
    QScopedPointer<IOAdapter> ioAdapter(IOAdapterUtils::open(settings.outputFileUrl, stateInfo));
    CHECK_OP(stateInfo, );

    TabulatedFormatReader reader(stateInfo, ioAdapter.data());
    while (reader.hasNextLine()) {
        const QStringList columns = reader.getNextLine();
        CHECK_EXT(columns.size() >= 2, setError(tr("Too few columns in the result file.")), );
        bool ok = false;
        classification.insert(columns.first(), columns.last().toUInt(&ok));
        CHECK_EXT(ok, setError(tr("Can't parse the taxID: \"%1\"").arg(columns.last())), );
    }
}

}   // namespace U2
