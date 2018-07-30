/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QFileInfo>

#include <U2Core/CopyFileTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/WorkflowMonitor.h>

#include "trimmomatic/steps/IlluminaClipStep.h"

#include "TrimmomaticWorker.h"
#include "TrimmomaticWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

const QString TrimmomaticWorker::TRIMMOMATIC_DIR = "trimmomatic";
const QString TrimmomaticWorker::SE_OUTPUT_FILE_NAME_SUFFIX = "_trim";
const QString TrimmomaticWorker::PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX = "P";
const QString TrimmomaticWorker::PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX = "U";
const QString TrimmomaticWorker::LOG_FILE_NAME_ENDING = "_trimlog.txt";

TrimmomaticWorker::TrimmomaticWorker(Actor *actor)
    : BaseWorker(actor),
      input(NULL),
      output(NULL),
      pairedReadsInput(false),
      generateLog(false),
      prepared(false)
{

}

void TrimmomaticWorker::init() {
    input = ports.value(TrimmomaticWorkerFactory::INPUT_PORT_ID);
    output = ports.value(TrimmomaticWorkerFactory::OUTPUT_PORT_ID);

    SAFE_POINT(NULL != input, QString("Port with ID '%1' is NULL!").arg(TrimmomaticWorkerFactory::INPUT_PORT_ID), );
    SAFE_POINT(NULL != output, QString("Port with ID '%1' is NULL!").arg(TrimmomaticWorkerFactory::OUTPUT_PORT_ID), );

    pairedReadsInput = (getValue<QString>(TrimmomaticWorkerFactory::INPUT_DATA_ATTR_ID) == TrimmomaticTaskSettings::PAIRED_END);
    generateLog = getValue<bool>(TrimmomaticWorkerFactory::GENERATE_LOG_ATTR_ID);
}

QPair<QString, QString> TrimmomaticWorker::getAbsoluteAndCopiedPathFromStep(const QString& trimmingStep) const {
    int indexOfFirstQuote = trimmingStep.indexOf("'");
    int indexOfSecondQuote = trimmingStep.indexOf("'", indexOfFirstQuote + 1);
    QString absoluteFilePath = trimmingStep.mid(indexOfFirstQuote + 1, (indexOfSecondQuote - 1) - indexOfFirstQuote);

    QFileInfo fi(absoluteFilePath);
    return QPair<QString, QString>(absoluteFilePath, QString(context->workingDir() + "/" + fi.fileName()));
}

Task *TrimmomaticWorker::tick() {
    if (!prepared) {
        QList<Task*> tasks;
        QStringList trimmingSteps = getValue<QStringList>(TrimmomaticWorkerFactory::TRIMMING_STEPS_ATTR_ID);
        QSet<QString> takenNames;
        foreach(const QString& trimmingStep, trimmingSteps) {
            if (!trimmingStep.startsWith(IlluminaClipStepFactory::ID)) {
                continue;
            }
            QPair<QString, QString> paths = getAbsoluteAndCopiedPathFromStep(trimmingStep);
            QFile destFile(paths.second);
            paths.second = GUrlUtils::rollFileName(paths.second, "_", takenNames);
            takenNames.insert(paths.second);
            tasks.append(new CopyFileTask(paths.first, paths.second));
            QFileInfo copy(paths.second);
            copiedAdapters.append(copy.fileName());
        }
        if (!tasks.isEmpty()) {
            Task *copyFiles = new MultiTask(tr("Copy adapters to working folder"), tasks);
            connect(new TaskSignalMapper(copyFiles), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskPrepareFinished(Task *)));
            return copyFiles;
        }
        prepared = true;
    }
    if (isDone()) {
        return NULL;
    }
    if (input->hasMessage()) {
        U2OpStatus2Log os;
        TrimmomaticTaskSettings settings = getSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }
        settings.workingDirectory = context->workingDir();
        TrimmomaticTask *task = new TrimmomaticTask(settings);
        task->addListeners(createLogListeners());
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    }

    if (input->isEnded()) {
        setDone();
        output->setEnded();
    }

    return NULL;
}

void TrimmomaticWorker::cleanup() {

}

void TrimmomaticWorker::sl_taskFinished(Task *task) {
    TrimmomaticTask *trimTask = qobject_cast<TrimmomaticTask *>(task);
    if (!trimTask->isFinished() || trimTask->hasError() || trimTask->isCanceled()) {
        return;
    }

    if (!pairedReadsInput) {
        const QString seOutputUrl = trimTask->getSeOutputUrl();

        QVariantMap data;
        data[TrimmomaticWorkerFactory::OUT_SLOT] = seOutputUrl;
        output->put(Message(output->getBusType(), data));

        context->getMonitor()->addOutputFile(seOutputUrl, getActor()->getId());
    }
    else {
        const QString pairedOutputUrl1 = trimTask->getPairedOutputUrl1();
        const QString pairedOutputUrl2 = trimTask->getPairedOutputUrl2();
        const QString unpairedOutputUrl1 = trimTask->getUnpairedOutputUrl1();
        const QString unpairedOutputUrl2 = trimTask->getUnpairedOutputUrl2();

        QVariantMap data;
        data[TrimmomaticWorkerFactory::OUT_SLOT] = pairedOutputUrl1;
        data[TrimmomaticWorkerFactory::PAIRED_OUT_SLOT] = pairedOutputUrl2;
        output->put(Message(output->getBusType(), data));

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

void TrimmomaticWorker::sl_taskPrepareFinished(Task *task) {
    MultiTask *copyFilesTask = qobject_cast<MultiTask *>(task);
    if (!copyFilesTask->isFinished() || copyFilesTask->hasError() || copyFilesTask->isCanceled()) {
        setDone();
        output->setEnded();
        return;
    }
    prepared = true;
}

QString TrimmomaticWorker::setAutoUrl(const QString &paramId, const QString &inputFileUrl, const QString &workingDir, const QString &fileNameSuffix) {
    QString value = getValue<QString>(paramId);
    if (value.isEmpty()) {
        QString outputFileName = GUrlUtils::insertSuffix(QUrl(inputFileUrl).fileName(), fileNameSuffix);
        value = workingDir + "/" + outputFileName;
    }
    GUrlUtils::rollFileName(value, "_");
    return value;
}

TrimmomaticTaskSettings TrimmomaticWorker::getSettings(U2OpStatus &os) {
    TrimmomaticTaskSettings settings;

    QString workingDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    workingDir = GUrlUtils::createDirectory(workingDir + TRIMMOMATIC_DIR , "_", os);

    const Message message = getMessageAndSetupScriptValues(input);
    settings.inputUrl1 = message.getData().toMap()[TrimmomaticWorkerFactory::INPUT_SLOT].toString();

    if (!pairedReadsInput) {
        settings.seOutputUrl = setAutoUrl(TrimmomaticWorkerFactory::OUTPUT_URL_ATTR_ID, settings.inputUrl1, workingDir, SE_OUTPUT_FILE_NAME_SUFFIX);
    } else {
        settings.inputUrl2 = message.getData().toMap()[TrimmomaticWorkerFactory::PAIRED_INPUT_SLOT].toString();

        settings.readsArePaired = true;

        settings.pairedOutputUrl1 = setAutoUrl(TrimmomaticWorkerFactory::PAIRED_URL_1_ATTR_ID, settings.inputUrl1, workingDir, PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX);
        settings.pairedOutputUrl2 = setAutoUrl(TrimmomaticWorkerFactory::PAIRED_URL_2_ATTR_ID, settings.inputUrl2, workingDir, PE_OUTPUT_PAIRED_FILE_NAME_SUFFIX);
        settings.unpairedOutputUrl1 = setAutoUrl(TrimmomaticWorkerFactory::UNPAIRED_URL_1_ATTR_ID, settings.inputUrl1, workingDir, PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX);
        settings.unpairedOutputUrl2 = setAutoUrl(TrimmomaticWorkerFactory::UNPAIRED_URL_2_ATTR_ID, settings.inputUrl2, workingDir, PE_OUTPUT_UNPAIRED_FILE_NAME_SUFFIX);
    }

    settings.trimmingSteps = getValue<QStringList>(TrimmomaticWorkerFactory::TRIMMING_STEPS_ATTR_ID);
    for (int i = 0, adaptersCounter = 0; i < settings.trimmingSteps.size(); i++) {
        QString &step = settings.trimmingSteps[i];
        if (step.startsWith(IlluminaClipStepFactory::ID)) {
            int indexOfFirstQuote = step.indexOf("'");
            int indexOfSecondQuote = step.indexOf("'", indexOfFirstQuote + 1);
            QString firstPart = step.left(indexOfFirstQuote);
            QString secondPart = step.right(step.size() - (indexOfSecondQuote + 1));
            step = firstPart + copiedAdapters[adaptersCounter++] + secondPart;
        }
    }

    if (generateLog) {
        settings.generateLog = true;
        settings.logUrl = getValue<QString>(TrimmomaticWorkerFactory::LOG_URL_ATTR_ID);
        if (settings.logUrl.isEmpty()) {
            QString baseName = GUrlUtils::getPairedFastqFilesBaseName(settings.inputUrl1, settings.readsArePaired);
            settings.logUrl = workingDir + "/" + baseName + LOG_FILE_NAME_ENDING;
        }
        settings.logUrl = GUrlUtils::rollFileName(settings.logUrl, "_");
    }

    settings.numberOfThreads = getValue<int>(TrimmomaticWorkerFactory::THREADS_NUMBER_ATTR_ID);

    return settings;
}

} // namespace LocalWorkflow
} // namespace U2
