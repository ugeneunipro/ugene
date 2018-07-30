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

#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowMonitor.h>

#include "WevoteWorker.h"
#include "WevoteWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

const QString WevoteWorker::WEVOTE_DIR = "wevote";

WevoteWorker::WevoteWorker(Actor *actor)
    : BaseWorker(actor),
      input(NULL),
      output(NULL)
{

}

void WevoteWorker::init() {
    input = ports.value(WevoteWorkerFactory::INPUT_PORT_ID);
    output = ports.value(WevoteWorkerFactory::OUTPUT_PORT_ID);
    SAFE_POINT(NULL != input, QString("Port with id '%1' is NULL").arg(WevoteWorkerFactory::INPUT_PORT_ID), );
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(WevoteWorkerFactory::OUTPUT_PORT_ID), );
}

Task *WevoteWorker::tick() {
    if (isReadyToRun()) {
        U2OpStatus2Log os;
        WevoteTaskSettings settings = getSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        WevoteTask *task = new WevoteTask(settings, context->getWorkflowProcess());
        task->addListeners(createLogListeners());
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    }

    if (dataFinished()) {
        setDone();
        output->setEnded();
    }

    return NULL;
}

void WevoteWorker::cleanup() {

}

void WevoteWorker::sl_taskFinished(Task *task) {
    WevoteTask *wevoteTask = qobject_cast<WevoteTask *>(task);
    if (!wevoteTask->isFinished() || wevoteTask->hasError() || wevoteTask->isCanceled()) {
        return;
    }

    const QString classificationUrl = wevoteTask->getClassificationUrl();
    const TaxonomyClassificationResult classification = wevoteTask->getClassification();

    QVariantMap data;
    data[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT_ID] = QVariant::fromValue<TaxonomyClassificationResult>(classification);
    output->put(Message(output->getBusType(), data));

    context->getMonitor()->addOutputFile(classificationUrl, getActor()->getId());
}

bool WevoteWorker::isReadyToRun() const {
    return input->hasMessage();
}

bool WevoteWorker::dataFinished() const {
    return input->isEnded();
}

WevoteTaskSettings WevoteWorker::getSettings(U2OpStatus &os) {
    WevoteTaskSettings settings;

    settings.penalty = getValue<int>(WevoteWorkerFactory::PENALTY_ATTR_ID);
    settings.numberOfAgreedTools = getValue<int>(WevoteWorkerFactory::NUMBER_OF_AGREED_TOOLS_ATTR_ID);
    settings.scoreThreshold = getValue<int>(WevoteWorkerFactory::SCORE_THRESHOLD_ATTR_ID);
    settings.numberOfThreads = getValue<int>(WevoteWorkerFactory::NUMBER_OF_THREADS_ATTR_ID);

    const Message message = getMessageAndSetupScriptValues(input);
    settings.inputFileUrl = message.getData().toMap()[BaseSlots::URL_SLOT().getId()].toString();
    CHECK_EXT(!settings.inputFileUrl.isEmpty(), os.setError(tr("Empty input file URL in the message")), settings);

    settings.workingDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    settings.workingDir = GUrlUtils::createDirectory(settings.workingDir + WEVOTE_DIR , "_", os);

    settings.outputFileUrl = getValue<QString>(WevoteWorkerFactory::OUTPUT_FILE_ATTR_ID);
    if (settings.outputFileUrl.isEmpty()) {
        const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
        settings.outputFileUrl = settings.workingDir + "/" + QFileInfo(metadata.getFileUrl()).completeBaseName() + WevoteTask::SUFFIX;
    }

    return settings;
}

}   // namespace LocalWorkflow
}   // namespace U2
