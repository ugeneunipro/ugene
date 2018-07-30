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

#include <U2Core/TaskSignalMapper.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowMonitor.h>

#include "KrakenBuildTask.h"
#include "KrakenBuildWorker.h"
#include "KrakenBuildWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

KrakenBuildWorker::KrakenBuildWorker(Actor *actor)
    : BaseWorker(actor, false),
      output(NULL)
{

}

void KrakenBuildWorker::init() {
    output = ports.value(KrakenBuildWorkerFactory::OUTPUT_PORT_ID);
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(KrakenBuildWorkerFactory::OUTPUT_PORT_ID), );
}

Task *KrakenBuildWorker::tick() {
    const KrakenBuildTaskSettings settings = getSettings();
    KrakenBuildTask *task = new KrakenBuildTask(settings);
    task->addListeners(createLogListeners(getListenersCount(settings)));
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
    return task;
}

void KrakenBuildWorker::cleanup() {

}

void KrakenBuildWorker::sl_taskFinished(Task *task) {
    KrakenBuildTask *krakenTask = qobject_cast<KrakenBuildTask *>(task);
    if (!krakenTask->isFinished() || krakenTask->hasError() || krakenTask->isCanceled()) {
        return;
    }

    const QString databaseUrl = krakenTask->getResultDatabaseUrl();

    MessageMetadata metadata;
    if (KrakenBuildTaskSettings::SHRINK == krakenTask->getSettings().mode) {
        metadata = MessageMetadata(krakenTask->getSettings().inputDatabaseUrl, "Dataset 1");
    } else {
        metadata = MessageMetadata("Dataset 1");
    }
    context->getMetadataStorage().put(metadata);

    QVariantMap data;
    data[BaseSlots::URL_SLOT().getId()] = databaseUrl;
    output->put(Message(output->getBusType(), data, metadata.getId()));

    context->getMonitor()->addOutputFile(databaseUrl, getActor()->getId(), true);

    setDone();
}

KrakenBuildTaskSettings KrakenBuildWorker::getSettings() {
    KrakenBuildTaskSettings settings;
    settings.mode = getValue<QString>(KrakenBuildWorkerFactory::MODE_ATTR_ID);
    settings.newDatabaseUrl = getValue<QString>(KrakenBuildWorkerFactory::NEW_DATABASE_NAME_ATTR_ID);
    settings.kMerLength = getValue<int>(KrakenBuildWorkerFactory::K_MER_LENGTH_ATTR_ID);
    settings.minimizerLength = getValue<int>(KrakenBuildWorkerFactory::MINIMIZER_LENGTH_ATTR_ID);
    settings.workOnDisk = getValue<bool>(KrakenBuildWorkerFactory::WORK_ON_DISK_ATTR_ID);
    settings.threadsNumber = getValue<int>(KrakenBuildWorkerFactory::THREADS_NUMBER_ATTR_ID);

    if (settings.mode == KrakenBuildTaskSettings::BUILD) {
        const QList<Dataset> datasets = getValue<QList<Dataset> >(KrakenBuildWorkerFactory::GENOMIC_LIBRARY_ATTR_ID);
        if (!datasets.isEmpty()) {
            foreach (URLContainer *urlContainer, datasets.first().getUrls()) {
                FilesIterator *iterator = urlContainer->getFileUrls();
                while (iterator->hasNext()) {
                    settings.additionalGenomesUrls << iterator->getNextFile();
                }
            }
        }

        settings.maximumDatabaseSize = getValue<int>(KrakenBuildWorkerFactory::MAXIMUM_DATABASE_SIZE_ATTR_ID);
        settings.clean = getValue<bool>(KrakenBuildWorkerFactory::CLEAN_ATTR_ID);
        settings.jellyfishHashSize = getValue<int>(KrakenBuildWorkerFactory::JELLYFISH_HASH_SIZE_ATTR_ID);
    } else {
        settings.numberOfKmers = getValue<int>(KrakenBuildWorkerFactory::NUMBER_OF_K_MERS_ATTR_ID);
        settings.inputDatabaseUrl = getValue<QString>(KrakenBuildWorkerFactory::INPUT_DATABASE_NAME_ATTR_ID);
        settings.shrinkBlockOffset = getValue<int>(KrakenBuildWorkerFactory::SHRINK_BLOCK_OFFSET_ATTR_ID);
    }

    return settings;
}

int KrakenBuildWorker::getListenersCount(const KrakenBuildTaskSettings &settings) const {
    if (settings.mode == KrakenBuildTaskSettings::BUILD) {
        const int addToLibraryCountersCount = settings.additionalGenomesUrls.size();
        const int buildDatabaseCountersCount = 1;
        const int cleanDatabaseCountersCount = settings.clean ? 1 : 0;
        return addToLibraryCountersCount + buildDatabaseCountersCount + cleanDatabaseCountersCount;
    } else {
        const int shrinkDatabaseCounterCount = 1;
        return shrinkDatabaseCounterCount;
    }
}

}   // namespace LocalWorkflow
}   // namespace U2
