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
#include <U2Core/AppSettings.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowUtils.h>

#include "Metaphlan2Support.h"
#include "Metaphlan2Worker.h"
#include "Metaphlan2WorkerFactory.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationUtils.h"

namespace U2 {
namespace LocalWorkflow {

const QString Metaphlan2Worker::METAPHLAN2_ROOT_DIR = "MetaPhlAn2";
const QString Metaphlan2Worker::BOWTIE2OUT_DIR = "bowtie2out";
const QString Metaphlan2Worker::BOWTIE2OUT_SUFFIX = "bowtie2out";
const QString Metaphlan2Worker::PROFILE_DIR = "profiles";
const QString Metaphlan2Worker::PROFILE_SUFFIX = "profile";

Metaphlan2Worker::Metaphlan2Worker(Actor* actor) :
                  BaseWorker(actor),
                  input(nullptr) {}

void Metaphlan2Worker::init() {
    input = ports.value(Metaphlan2WorkerFactory::INPUT_PORT_ID);
    SAFE_POINT(nullptr != input, QString("Port with id '%1' is nullptr")
                              .arg(Metaphlan2WorkerFactory::INPUT_PORT_ID), );
}

Task* Metaphlan2Worker::tick() {
    if (isReadyToRun()) {
        U2OpStatus2Log os;
        Metaphlan2TaskSettings settings = getSettings(os);
        CHECK(!os.hasError(), new FailTask(os.getError()));

        Metaphlan2ClassifyTask* task = new Metaphlan2ClassifyTask(settings);
        task->addListeners(createLogListeners());
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return task;
    }

    if (dataFinished()) {
        setDone();
    }

    return nullptr;
}

void Metaphlan2Worker::cleanup() {

}

void Metaphlan2Worker::sl_taskFinished(Task* task) {
    Metaphlan2ClassifyTask* metaphlan2Task = qobject_cast<Metaphlan2ClassifyTask*>(task);
    if (!metaphlan2Task->isFinished() || metaphlan2Task->hasError() || metaphlan2Task->isCanceled()) {
        return;
    }

    addOutputToDashboard(metaphlan2Task->getBowtie2OutputUrl(), "Bowtie2");
    addOutputToDashboard(metaphlan2Task->getOutputUrl(), "MetaPhlAn2");
}

bool Metaphlan2Worker::isReadyToRun() const {
    return input->hasMessage();
}

bool Metaphlan2Worker::dataFinished() const {
    return input->isEnded();
}

Metaphlan2TaskSettings Metaphlan2Worker::getSettings(U2OpStatus &os) {
    Metaphlan2TaskSettings settings;
    settings.isPairedEnd = getValue<QString>(Metaphlan2WorkerFactory::SEQUENCING_READS) == Metaphlan2WorkerFactory::PAIRED_END;

    const Message message = getMessageAndSetupScriptValues(input);
    settings.readsUrl = message.getData().toMap()[Metaphlan2WorkerFactory::INPUT_SLOT].toString();
    if (settings.isPairedEnd) {
        settings.pairedReadsUrl = message.getData().toMap()[Metaphlan2WorkerFactory::PAIRED_INPUT_SLOT].toString();
    }

    settings.databaseUrl = getValue<QString>(Metaphlan2WorkerFactory::DB_URL);
    settings.numberOfThreads = getValue<int>(Metaphlan2WorkerFactory::NUM_THREADS);
    settings.analysisType = getValue<QString>(Metaphlan2WorkerFactory::ANALYSIS_TYPE);
    settings.taxLevel = getValue<QString>(Metaphlan2WorkerFactory::TAX_LEVEL);
    settings.normalizeByMetagenomeSize = getValue<QString>(Metaphlan2WorkerFactory::NORMALIZE) == Metaphlan2WorkerFactory::NOT_SKIP_NORMILIZE_BY_SIZE;
    settings.presenceThreshold = getValue<int>(Metaphlan2WorkerFactory::PRESENCE_THRESHOLD);

    const QString defaultOutputDirectory = getDefaultOutputDir();

    settings.bowtie2OutputFile = getValue<QString>(Metaphlan2WorkerFactory::BOWTIE2_OUTPUT_URL);
    if (settings.bowtie2OutputFile.isEmpty()) {
        settings.bowtie2OutputFile = createOutputToolDirectory(defaultOutputDirectory, message, settings.isPairedEnd, Bowtie2);
    } else {
        const bool dirCreated = QDir().mkpath(QFileInfo(settings.bowtie2OutputFile).absoluteDir().path());
        CHECK_EXT(dirCreated, os.setError(tr("Can't create a folder for the output file: %1").arg(settings.bowtie2OutputFile)), settings);
    }
    settings.bowtie2OutputFile = GUrlUtils::rollFileName(settings.bowtie2OutputFile, "_");

    settings.outputFile = getValue<QString>(Metaphlan2WorkerFactory::OUTPUT_URL);
    if (settings.outputFile.isEmpty()) {
        settings.outputFile = createOutputToolDirectory(defaultOutputDirectory, message, settings.isPairedEnd, MetaPhlAn2);
    } else {
        const bool dirCreated = QDir().mkpath(QFileInfo(settings.outputFile).absoluteDir().path());
        CHECK_EXT(dirCreated, os.setError(tr("Can't create a folder for the output file: %1").arg(settings.outputFile)), settings);
    }
    settings.outputFile = GUrlUtils::rollFileName(settings.outputFile, "_");

    QString bowtie2AlignerPath = WorkflowUtils::getExternalToolPath(Metaphlan2Support::ET_BOWTIE_2_ALIGNER_ID);
    CHECK_EXT(!bowtie2AlignerPath.isEmpty(), os.setError("Bowtie2 aligner isn't found"), settings);

    settings.bowtie2ExternalToolPath = QFileInfo(bowtie2AlignerPath).dir().path();
    QString pythonPath = WorkflowUtils::getExternalToolPath(Metaphlan2Support::ET_PYTHON_BIO_ID);
    CHECK_EXT(!pythonPath.isEmpty(), os.setError("Python isn't found"), settings);

    settings.pythonExternalToolPath = QFileInfo(pythonPath).dir().path();
    settings.tmpDir = AppContext::getAppSettings()->getUserAppsSettings()->createCurrentProcessTemporarySubDir(os, METAPHLAN2_ROOT_DIR);
    CHECK_OP(os, settings);

    return settings;
}

QString Metaphlan2Worker::getDefaultOutputDir() const {
    QString defaultOutputDirectory = FileAndDirectoryUtils::getWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    defaultOutputDirectory += METAPHLAN2_ROOT_DIR;
    return GUrlUtils::rollFileName(defaultOutputDirectory, "_");
}

QString Metaphlan2Worker::createOutputToolDirectory(const QString& outputDirectory, const Message& message, const bool isPairedEnd, const Output out) const {
    QStringList suffix;
    QString folder;
    switch (out) {
    case Bowtie2:
        suffix << BOWTIE2OUT_SUFFIX;
        folder = BOWTIE2OUT_DIR;
        break;
    case MetaPhlAn2:
        suffix << PROFILE_SUFFIX;
        folder = PROFILE_DIR;
        break;
    }
    QString outputToolDirectory = QString("%1/%2").arg(outputDirectory).arg(folder);
    createDirectory(outputToolDirectory);

    const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
    QString result = QString("%1/%2").arg(outputToolDirectory).arg(NgsReadsClassificationUtils::getBaseFileNameWithSuffixes(metadata.getFileUrl(), suffix, "txt", isPairedEnd));

    return result;
}

void Metaphlan2Worker::createDirectory(QString& dir) const {
    dir = GUrlUtils::rollFileName(dir, "_");
    QDir outDir(dir);
    outDir.mkpath(dir);
}

void Metaphlan2Worker::addOutputToDashboard(const QString& outputUrl, const QString& outputName) const {
    if (QFileInfo::exists(outputUrl)) {
        context->getMonitor()->addOutputFile(outputUrl, getActor()->getId());
    } else {
        coreLog.error(tr("%1 output file doesn't exist").arg(outputName));
    }
}

}
}
