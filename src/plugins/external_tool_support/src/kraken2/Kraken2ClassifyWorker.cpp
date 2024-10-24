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

#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowMonitor.h>

#include "NgsReadsClassificationUtils.h"
#include "Kraken2ClassifyTask.h"
#include "Kraken2ClassifyWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

const QString Kraken2ClassifyWorker::KRAKEN_DIR = "Kraken2";

Kraken2ClassifyWorker::Kraken2ClassifyWorker(Actor *actor)
    : BaseWorker(actor, false),
      input(nullptr),
      //      pairedInput(NULL),
      output(nullptr),
      pairedReadsInput(false) {
}

void Kraken2ClassifyWorker::init() {
    input = ports.value(Kraken2ClassifyWorkerFactory::INPUT_PORT_ID);
    output = ports.value(Kraken2ClassifyWorkerFactory::OUTPUT_PORT_ID);

    SAFE_POINT(nullptr != input, QString("Port with id '%1' is NULL").arg(Kraken2ClassifyWorkerFactory::INPUT_PORT_ID), );
    SAFE_POINT(nullptr != output, QString("Port with id '%1' is NULL").arg(Kraken2ClassifyWorkerFactory::OUTPUT_PORT_ID), );

    pairedReadsInput = (getValue<QString>(Kraken2ClassifyWorkerFactory::INPUT_DATA_ATTR_ID) == Kraken2ClassifyTaskSettings::PAIRED_END);

    // FIXME: the second port is not taken into account
    output->addComplement(input);
    input->addComplement(output);
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
        output->setEnded();
    }

    return nullptr;
}

void Kraken2ClassifyWorker::cleanup() {
}

void Kraken2ClassifyWorker::sl_taskFinished(Task *task) {
    Kraken2ClassifyTask *krakenTask = qobject_cast<Kraken2ClassifyTask *>(task);
    if (!krakenTask->isFinished() || krakenTask->hasError() || krakenTask->isCanceled()) {
        return;
    }

    const QString rawClassificationUrl = krakenTask->getClassificationUrl();
    QVariantMap data;
    const TaxonomyClassificationResult &classificationResult = krakenTask->getParsedReport();
    data[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT_ID] = QVariant::fromValue<U2::LocalWorkflow::TaxonomyClassificationResult>(classificationResult);
    output->put(Message(output->getBusType(), data));
    context->getMonitor()->addOutputFile(rawClassificationUrl, getActor()->getId());

    int classifiedCount = NgsReadsClassificationUtils::countClassified(classificationResult);
    context->getMonitor()->addInfo(tr("There were %1 input reads, %2 reads were classified.").arg(QString::number(classificationResult.size())).arg(QString::number(classifiedCount)), getActor()->getId(), WorkflowNotification::U2_INFO);
}

bool Kraken2ClassifyWorker::isReadyToRun() const {
    return input->hasMessage();
}

bool Kraken2ClassifyWorker::dataFinished() const {
    return input->isEnded();
}

Kraken2ClassifyTaskSettings Kraken2ClassifyWorker::getSettings(U2OpStatus &os) {
    Kraken2ClassifyTaskSettings settings;
    settings.databaseUrl = getValue<QString>(Kraken2ClassifyWorkerFactory::DATABASE_ATTR_ID);
    settings.quickOperation = getValue<bool>(Kraken2ClassifyWorkerFactory::QUICK_OPERATION_ATTR_ID);
    settings.numberOfThreads = getValue<int>(Kraken2ClassifyWorkerFactory::THREADS_NUMBER_ATTR_ID);

    const Message message = getMessageAndSetupScriptValues(input);
    settings.readsUrl = message.getData().toMap()[Kraken2ClassifyWorkerFactory::INPUT_SLOT].toString();

    if (pairedReadsInput) {
        settings.pairedReads = true;
        settings.pairedReadsUrl = message.getData().toMap()[Kraken2ClassifyWorkerFactory::PAIRED_INPUT_SLOT].toString();
    }

    QString tmpDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    tmpDir = GUrlUtils::createDirectory(tmpDir + KRAKEN_DIR, "_", os);

    settings.classificationUrl = getValue<QString>(Kraken2ClassifyWorkerFactory::OUTPUT_URL_ATTR_ID);
    if (settings.classificationUrl.isEmpty()) {
        const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
        QString fileUrl = metadata.getFileUrl();
        settings.classificationUrl = tmpDir + "/" + (fileUrl.isEmpty() ? QString("Kraken_%1.txt").arg(NgsReadsClassificationUtils::CLASSIFICATION_SUFFIX) : NgsReadsClassificationUtils::getBaseFileNameWithSuffixes(metadata.getFileUrl(), QStringList() << "Kraken" << NgsReadsClassificationUtils::CLASSIFICATION_SUFFIX, "txt", pairedReadsInput));
    }
    settings.classificationUrl = GUrlUtils::rollFileName(settings.classificationUrl, "_");

    return settings;
}

}  // namespace LocalWorkflow
}  // namespace U2
