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

#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowMonitor.h>

#include "KrakenClassifyTask.h"
#include "KrakenClassifyWorker.h"
#include "KrakenClassifyWorkerFactory.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationUtils.h"

namespace U2 {
namespace LocalWorkflow {

const QString KrakenClassifyWorker::KRAKEN_DIR = "Kraken";

KrakenClassifyWorker::KrakenClassifyWorker(Actor *actor)
    : BaseWorker(actor, false),
      input(NULL),
//      pairedInput(NULL),
      output(NULL),
      pairedReadsInput(false)
{

}

void KrakenClassifyWorker::init() {
    input = ports.value(KrakenClassifyWorkerFactory::INPUT_PORT_ID);
    output = ports.value(KrakenClassifyWorkerFactory::OUTPUT_PORT_ID);

    SAFE_POINT(NULL != input, QString("Port with id '%1' is NULL").arg(KrakenClassifyWorkerFactory::INPUT_PORT_ID), );
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(KrakenClassifyWorkerFactory::OUTPUT_PORT_ID), );

    pairedReadsInput = (getValue<QString>(KrakenClassifyWorkerFactory::INPUT_DATA_ATTR_ID) == KrakenClassifyTaskSettings::PAIRED_END);

    // FIXME: the second port is not taken into account
    output->addComplement(input);
    input->addComplement(output);
}

Task *KrakenClassifyWorker::tick() {
    if (isReadyToRun()) {
        U2OpStatus2Log os;
        KrakenClassifyTaskSettings settings = getSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        KrakenClassifyTask *task = new KrakenClassifyTask(settings);
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

void KrakenClassifyWorker::cleanup() {

}

void KrakenClassifyWorker::sl_taskFinished(Task *task) {
    KrakenClassifyTask *krakenTask = qobject_cast<KrakenClassifyTask *>(task);
    if (!krakenTask->isFinished() || krakenTask->hasError() || krakenTask->isCanceled()) {
        return;
    }

    const QString rawClassificationUrl = krakenTask->getClassificationUrl();

    QVariantMap data;
    const TaxonomyClassificationResult &classificationResult = krakenTask->getParsedReport();
    data[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT_ID] = QVariant::fromValue<U2::LocalWorkflow::TaxonomyClassificationResult>(classificationResult);
    output->put(Message(output->getBusType(), data));
    context->getMonitor()->addOutputFile(rawClassificationUrl, getActor()->getId());

    LocalWorkflow::TaxonomyClassificationResult::const_iterator it;
    int classifiedCount = NgsReadsClassificationUtils::countClassified(classificationResult);
    context->getMonitor()->addInfo(tr("There were %1 input reads, %2 reads were classified.").arg(QString::number(classificationResult.size())).arg(QString::number(classifiedCount))
        , getActor()->getId(), WorkflowNotification::U2_INFO);
}

bool KrakenClassifyWorker::isReadyToRun() const {
    return input->hasMessage();
}

bool KrakenClassifyWorker::dataFinished() const {
    return input->isEnded();
}

KrakenClassifyTaskSettings KrakenClassifyWorker::getSettings(U2OpStatus &os) {
    KrakenClassifyTaskSettings settings;
    settings.databaseUrl = getValue<QString>(KrakenClassifyWorkerFactory::DATABASE_ATTR_ID);
    settings.quickOperation = getValue<bool>(KrakenClassifyWorkerFactory::QUICK_OPERATION_ATTR_ID);
    settings.minNumberOfHits = getValue<int>(KrakenClassifyWorkerFactory::MIN_HITS_NUMBER_ATTR_ID);
    settings.numberOfThreads = getValue<int>(KrakenClassifyWorkerFactory::THREADS_NUMBER_ATTR_ID);
    settings.preloadDatabase = getValue<bool>(KrakenClassifyWorkerFactory::PRELOAD_DATABASE_ATTR_ID);

    const Message message = getMessageAndSetupScriptValues(input);
    settings.readsUrl = message.getData().toMap()[KrakenClassifyWorkerFactory::INPUT_SLOT].toString();

    if (pairedReadsInput) {
        settings.pairedReads = true;
        settings.pairedReadsUrl = message.getData().toMap()[KrakenClassifyWorkerFactory::PAIRED_INPUT_SLOT].toString();
    }

    QString tmpDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    tmpDir = GUrlUtils::createDirectory(tmpDir + KRAKEN_DIR , "_", os);

    settings.classificationUrl = getValue<QString>(KrakenClassifyWorkerFactory::OUTPUT_URL_ATTR_ID);
    if (settings.classificationUrl.isEmpty()) {
        const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
        QString fileUrl = metadata.getFileUrl();
        settings.classificationUrl = tmpDir + "/" + (fileUrl.isEmpty() ? QString("Kraken_%1.txt").arg(NgsReadsClassificationUtils::CLASSIFICATION_SUFFIX)
        : NgsReadsClassificationUtils::getBaseFileNameWithSuffixes(metadata.getFileUrl(), QStringList() << "Kraken" << NgsReadsClassificationUtils::CLASSIFICATION_SUFFIX,
                                                                    "txt", pairedReadsInput));
    }
    settings.classificationUrl = GUrlUtils::rollFileName(settings.classificationUrl, "_");

    return settings;
}

}   // namespace LocalWorkflow
}   // namespace U2
