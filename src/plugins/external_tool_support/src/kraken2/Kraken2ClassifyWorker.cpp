/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Kraken2ClassifyWorker.h"

#include <QFileInfo>

#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowMonitor.h>

#include "Kraken2ClassifyTask.h"
#include "Kraken2ClassifyWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

const QString Kraken2ClassifyWorker::KRAKEN_DIR = "Kraken2";

Kraken2ClassifyWorker::Kraken2ClassifyWorker(Actor *actor) : BaseWorker(actor, false) {
};

void Kraken2ClassifyWorker::init() {
    input = ports.value(Kraken2ClassifyWorkerFactory::IN_PORT_DESCR_SINGLE);
    SAFE_POINT(input != nullptr, QString("Port with id '%1' is NULL").arg(Kraken2ClassifyWorkerFactory::IN_PORT_DESCR_SINGLE), );
    isPairedReadsInput = (getValue<QString>(Kraken2ClassifyWorkerFactory::INPUT_DATA_ATTR_ID) == Kraken2ClassifyTaskSettings::PAIRED_END);
    if (isPairedReadsInput) {
        pairedInput = ports.value(Kraken2ClassifyWorkerFactory::IN_PORT_DESCR_PAIRED);
        SAFE_POINT(pairedInput != nullptr, QString("Port with id '%1' is NULL").arg(Kraken2ClassifyWorkerFactory::IN_PORT_DESCR_PAIRED), );
    }
}

Task *Kraken2ClassifyWorker::tick() {
    if (isReadyToRun()) {
        U2OpStatus2Log os;
        Kraken2ClassifyTaskSettings settings = getSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        Kraken2ClassifyTask *task = new Kraken2ClassifyTask(settings);
        task->addListeners(createLogListeners());
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    }

    if (dataFinished()) {
        setDone();
    }
    return nullptr;
}

void Kraken2ClassifyWorker::cleanup() {
}

bool Kraken2ClassifyWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    int hasMsg1 = input->hasMessage();
    bool ended1 = input->isEnded();
    if (!isPairedReadsInput) {
        return hasMsg1 || ended1;
    }

    int hasMsg2 = pairedInput->hasMessage();
    bool ended2 = pairedInput->isEnded();

    if (hasMsg1 && hasMsg2) {
        return true;
    } else if (hasMsg1) {
        return ended2;
    } else if (hasMsg2) {
        return ended1;
    }

    return ended1 && ended2;
}

void Kraken2ClassifyWorker::sl_taskFinished(Task *task) {
    Kraken2ClassifyTask *krakenTask = qobject_cast<Kraken2ClassifyTask *>(task);
    QString outputUrl = krakenTask->getClassificationURL();
    if (!krakenTask->isFinished() || krakenTask->hasError() || krakenTask->isCanceled() || !QFileInfo::exists(outputUrl)) {
        return;
    }
    monitor()->addOutputFile(outputUrl, getActorId());
}

bool Kraken2ClassifyWorker::isReadyToRun() const {
    return input->hasMessage() && (!isPairedReadsInput || pairedInput->hasMessage());
}

bool Kraken2ClassifyWorker::dataFinished() const {
    if(isPairedReadsInput) {
        return input->isEnded() && pairedInput->isEnded();
    }
    return input->isEnded();
}

Kraken2ClassifyTaskSettings Kraken2ClassifyWorker::getSettings(U2OpStatus &os) {
    Kraken2ClassifyTaskSettings settings;
    settings.databaseUrl = getValue<QString>(Kraken2ClassifyWorkerFactory::DATABASE_ATTR_ID);
    settings.quickOperation = getValue<bool>(Kraken2ClassifyWorkerFactory::QUICK_OPERATION_ATTR_ID);
    settings.numberOfThreads = getValue<int>(Kraken2ClassifyWorkerFactory::THREADS_NUMBER_ATTR_ID);

    const Message message = getMessageAndSetupScriptValues(input);
    settings.readsUrl = message.getData().toMap()[Kraken2ClassifyWorkerFactory::INPUT_SLOT].toString();

    if (isPairedReadsInput) {
        settings.pairedReads = true;
        settings.pairedReadsUrl = getMessageAndSetupScriptValues(pairedInput).getData().toMap()[Kraken2ClassifyWorkerFactory::PAIRED_INPUT_SLOT].toString();
        if (settings.readsUrl.isEmpty() || settings.pairedReadsUrl.isEmpty()) {
            os.setError(tr("Quantity of files with reads in \"URL 1\" and \"URL 2\" should be equal."));
            return settings;
        }
    }

    QString tmpDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    tmpDir = GUrlUtils::createDirectory(tmpDir + KRAKEN_DIR, "_", os);

    settings.classificationUrl = getValue<QString>(Kraken2ClassifyWorkerFactory::OUTPUT_URL_ATTR_ID);
    if (settings.classificationUrl.isEmpty()) {
        const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
        QString fileUrl = metadata.getFileUrl();
        settings.classificationUrl = tmpDir + "/Kraken2_result.txt";
    }
    settings.classificationUrl = GUrlUtils::rollFileName(settings.classificationUrl, "_");

    return settings;
}

}  // namespace LocalWorkflow
}  // namespace U2
