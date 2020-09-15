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

#include "TrimmomaticWorker.h"

#include <QFileInfo>

#include <U2Core/CopyFileTask.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/WorkflowMonitor.h>

#include "TrimmomaticWorkerFactory.h"
#include "trimmomatic/steps/IlluminaClipStep.h"

namespace U2 {
namespace LocalWorkflow {

const QString TrimmomaticWorker::TRIMMOMATIC_DIR = "Trimmomatic";
const QString TrimmomaticWorker::SE_OUTPUT_FILE_NAME_SUFFIX = "_trim";
const QString TrimmomaticWorker::PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX = "P";
const QString TrimmomaticWorker::PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX = "U";
const QString TrimmomaticWorker::LOG_FILE_NAME_ENDING = "_trimlog.txt";

TrimmomaticWorker::TrimmomaticWorker(Actor *actor)
    : BaseDatasetWorker(actor,
                        TrimmomaticWorkerFactory::INPUT_PORT_ID,
                        TrimmomaticWorkerFactory::OUTPUT_PORT_ID),
      pairedReadsInput(false),
      generateLog(false),
      numberOfThreads(0) {
}

void TrimmomaticWorker::init() {
    BaseDatasetWorker::init();
    pairedReadsInput = (getValue<QString>(TrimmomaticWorkerFactory::INPUT_DATA_ATTR_ID) == TrimmomaticTaskSettings::PAIRED_END);
    generateLog = getValue<bool>(TrimmomaticWorkerFactory::GENERATE_LOG_ATTR_ID);
    trimmingSteps = getValue<QStringList>(TrimmomaticWorkerFactory::TRIMMING_STEPS_ATTR_ID);
    numberOfThreads = getValue<int>(TrimmomaticWorkerFactory::THREADS_NUMBER_ATTR_ID);
}

QPair<QString, QString> TrimmomaticWorker::getAbsoluteAndCopiedPathFromStep(const QString &trimmingStep) const {
    int indexOfFirstQuote = trimmingStep.indexOf("'");
    int indexOfSecondQuote = trimmingStep.indexOf("'", indexOfFirstQuote + 1);
    QString absoluteFilePath = trimmingStep.mid(indexOfFirstQuote + 1, (indexOfSecondQuote - 1) - indexOfFirstQuote);

    QFileInfo fi(absoluteFilePath);
    return QPair<QString, QString>(absoluteFilePath, QString(context->workingDir() + "/" + fi.fileName()));
}

void TrimmomaticWorker::changeAdapters() {
    for (int i = 0, adaptersCounter = 0; i < trimmingSteps.size(); i++) {
        QString &step = trimmingSteps[i];
        if (step.startsWith(IlluminaClipStepFactory::ID)) {
            int indexOfFirstQuote = step.indexOf("'");
            int indexOfSecondQuote = step.indexOf("'", indexOfFirstQuote + 1);
            QString firstPart = step.left(indexOfFirstQuote);
            QString secondPart = step.right(step.size() - (indexOfSecondQuote + 1));
            step = firstPart + QFileInfo(copiedAdapters[adaptersCounter++]).fileName() + secondPart;
        }
    }
}

void TrimmomaticWorker::processMetadata(QList<Task *> tasks) const {
    metaFileUrl.clear();
    CHECK(1 == tasks.size(), );

    TrimmomaticTask *trimTask = qobject_cast<TrimmomaticTask *>(tasks.first());
    metaFileUrl = trimTask->getInputUrl1();
}

void TrimmomaticWorker::cleanup() {
    foreach (const QString &name, copiedAdapters) {
        QFile adapter(name);
        adapter.remove();
    }
}

Task *TrimmomaticWorker::createPrepareTask(U2OpStatus &os) const {
    Q_UNUSED(os);

    QList<Task *> tasks;
    QSet<QString> takenNames;
    for (const QString &trimmingStep : trimmingSteps) {
        if (!trimmingStep.startsWith(IlluminaClipStepFactory::ID)) {
            continue;
        }
        QPair<QString, QString> paths = getAbsoluteAndCopiedPathFromStep(trimmingStep);
        paths.second = GUrlUtils::rollFileName(paths.second, "_", takenNames);
        takenNames.insert(paths.second);
        tasks.append(new CopyFileTask(paths.first, paths.second));
        copiedAdapters.append(paths.second);
    }

    Task *copyFiles = nullptr;
    if (!tasks.isEmpty()) {
        copyFiles = new MultiTask(tr("Copy adapters to working folder"), tasks);
    }

    return copyFiles;
}

void TrimmomaticWorker::onPrepared(Task *task, U2OpStatus &os) {
    MultiTask *prepareTask = qobject_cast<MultiTask *>(task);
    CHECK_EXT(nullptr != prepareTask, os.setError(L10N::internalError("Unexpected prepare task")), );

    changeAdapters();
}

Task *TrimmomaticWorker::createTask(const QList<Message> &messages) const {
    U2OpStatus2Log os;
    const QString workingDirectory = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    const QString trimmomaticWorkingDir = GUrlUtils::createDirectory(workingDirectory + TRIMMOMATIC_DIR, "_", os);
    CHECK_OP(os, nullptr);

    QList<Task *> trimmomaticTasks;
    foreach (const Message &message, messages) {
        const TrimmomaticTaskSettings settings = getSettings(message, trimmomaticWorkingDir);
        TrimmomaticTask *task = new TrimmomaticTask(settings);
        task->addListeners(createLogListeners());
        trimmomaticTasks << task;
    }
    excludedUrls.clear();
    processMetadata(trimmomaticTasks);

    Task *processTrimmomatic = nullptr;
    if (!trimmomaticTasks.isEmpty()) {
        processTrimmomatic = new MultiTask(tr("Process \"Trimmomatic\" with one dataset"), trimmomaticTasks);
    }

    return processTrimmomatic;
}

QVariantMap TrimmomaticWorker::getResult(Task *task, U2OpStatus &os) const {
    MultiTask *multiTask = qobject_cast<MultiTask *>(task);
    CHECK_EXT(nullptr != multiTask, os.setError(L10N::internalError("Unexpected task")), QVariantMap());

    QVariantMap result;
    foreach (Task *task, multiTask->getTasks()) {
        TrimmomaticTask *trimTask = qobject_cast<TrimmomaticTask *>(task);
        CHECK_CONTINUE(trimTask != nullptr);

        if (!pairedReadsInput) {
            const QString seOutputUrl = trimTask->getSeOutputUrl();

            result[TrimmomaticWorkerFactory::OUT_SLOT] = seOutputUrl;

            context->getMonitor()->addOutputFile(seOutputUrl, getActor()->getId());
        } else {
            const QString pairedOutputUrl1 = trimTask->getPairedOutputUrl1();
            const QString pairedOutputUrl2 = trimTask->getPairedOutputUrl2();
            const QString unpairedOutputUrl1 = trimTask->getUnpairedOutputUrl1();
            const QString unpairedOutputUrl2 = trimTask->getUnpairedOutputUrl2();

            result[TrimmomaticWorkerFactory::OUT_SLOT] = pairedOutputUrl1;
            result[TrimmomaticWorkerFactory::PAIRED_OUT_SLOT] = pairedOutputUrl2;

            context->getMonitor()->addOutputFile(pairedOutputUrl1, getActor()->getId());
            context->getMonitor()->addOutputFile(pairedOutputUrl2, getActor()->getId());
            context->getMonitor()->addOutputFile(unpairedOutputUrl1, getActor()->getId());
            context->getMonitor()->addOutputFile(unpairedOutputUrl2, getActor()->getId());
        }

        if (generateLog) {
            const QString logUrl = trimTask->getLogUrl();
            context->getMonitor()->addOutputFile(logUrl, getActor()->getId());
        }
    }

    return result;
}

MessageMetadata TrimmomaticWorker::generateMetadata(const QString &datasetName) const {
    CHECK(!metaFileUrl.isEmpty(), BaseDatasetWorker::generateMetadata(datasetName));

    return MessageMetadata(metaFileUrl, datasetName);
}

QString TrimmomaticWorker::setAutoUrl(const QString &paramId, const QString &inputFileUrl, const QString &workingDir, const QString &fileNameSuffix) const {
    QString value = getValue<QString>(paramId);
    if (value.isEmpty()) {
        QString outputFileName = GUrlUtils::insertSuffix(QUrl(inputFileUrl).fileName(), fileNameSuffix);
        value = workingDir + "/" + outputFileName;
    }
    value = GUrlUtils::rollFileName(value, "_", excludedUrls);
    excludedUrls.insert(value);
    return value;
}

TrimmomaticTaskSettings TrimmomaticWorker::getSettings(const Message &message, const QString &dirForResults) const {
    TrimmomaticTaskSettings settings;
    settings.pairedReadsInput = pairedReadsInput;
    settings.generateLog = generateLog;
    settings.trimmingSteps = trimmingSteps;
    settings.numberOfThreads = numberOfThreads;
    settings.workingDirectory = context->workingDir();

    settings.inputUrl1 = message.getData().toMap()[TrimmomaticWorkerFactory::INPUT_SLOT].toString();

    if (!settings.pairedReadsInput) {
        settings.seOutputUrl = setAutoUrl(TrimmomaticWorkerFactory::OUTPUT_URL_ATTR_ID, settings.inputUrl1, dirForResults, SE_OUTPUT_FILE_NAME_SUFFIX);
    } else {
        settings.inputUrl2 = message.getData().toMap()[TrimmomaticWorkerFactory::PAIRED_INPUT_SLOT].toString();

        settings.pairedOutputUrl1 = setAutoUrl(TrimmomaticWorkerFactory::PAIRED_URL_1_ATTR_ID, settings.inputUrl1, dirForResults, PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX);
        settings.pairedOutputUrl2 = setAutoUrl(TrimmomaticWorkerFactory::PAIRED_URL_2_ATTR_ID, settings.inputUrl2, dirForResults, PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX);
        settings.unpairedOutputUrl1 = setAutoUrl(TrimmomaticWorkerFactory::UNPAIRED_URL_1_ATTR_ID, settings.inputUrl1, dirForResults, PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX);
        settings.unpairedOutputUrl2 = setAutoUrl(TrimmomaticWorkerFactory::UNPAIRED_URL_2_ATTR_ID, settings.inputUrl2, dirForResults, PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX);
    }

    if (settings.generateLog) {
        settings.logUrl = getValue<QString>(TrimmomaticWorkerFactory::LOG_URL_ATTR_ID);
        if (settings.logUrl.isEmpty()) {
            QString baseName = GUrlUtils::getPairedFastqFilesBaseName(settings.inputUrl1, settings.pairedReadsInput);
            settings.logUrl = dirForResults + "/" + baseName + LOG_FILE_NAME_ENDING;
        }
        settings.logUrl = GUrlUtils::rollFileName(settings.logUrl, "_");
    }

    return settings;
}

}    // namespace LocalWorkflow
}    // namespace U2
