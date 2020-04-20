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
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowMonitor.h>

#include "DiamondBuildWorker.h"
#include "DiamondBuildWorkerFactory.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationUtils.h"

namespace U2 {
namespace LocalWorkflow {

const QString DiamondBuildWorker::DIAMOND_BUILD_DIR = "diamond_build";

DiamondBuildWorker::DiamondBuildWorker(Actor *actor)
    : BaseWorker(actor, false),
      output(NULL)
{

}

void DiamondBuildWorker::init() {
    output = ports.value(DiamondBuildWorkerFactory::OUTPUT_PORT_ID);
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(DiamondBuildWorkerFactory::OUTPUT_PORT_ID), );
}

Task *DiamondBuildWorker::tick() {
    U2OpStatus2Log os;
    const DiamondBuildTaskSettings settings = getSettings(os);
    CHECK_OP(os, new FailTask(os.getError()));

    DiamondBuildTask *task = new DiamondBuildTask(settings);
    task->addListeners(createLogListeners());
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
    return task;
}

void DiamondBuildWorker::cleanup() {

}

void DiamondBuildWorker::sl_taskFinished(Task *task) {
    DiamondBuildTask *diamondTask = qobject_cast<DiamondBuildTask *>(task);
    if (!diamondTask->isFinished() || diamondTask->hasError() || diamondTask->isCanceled()) {
        return;
    }

    const QString databaseUrl = diamondTask->getDatabaseUrl();

    MessageMetadata metadata("Dataset 1");
    context->getMetadataStorage().put(metadata);

    QVariantMap data;
    data[BaseSlots::URL_SLOT().getId()] = databaseUrl;
    output->put(Message(output->getBusType(), data, metadata.getId()));

    context->getMonitor()->addOutputFile(databaseUrl, getActor()->getId(), true);

    setDone();
}

DiamondBuildTaskSettings DiamondBuildWorker::getSettings(U2OpStatus &os) {
    DiamondBuildTaskSettings settings;
    settings.databaseUrl = getValue<QString>(DiamondBuildWorkerFactory::DATABASE_ATTR_ID);
    settings.databaseUrl = GUrlUtils::ensureFileExt(settings.databaseUrl, QStringList("dmnd")).getURLString();
    settings.databaseUrl = GUrlUtils::rollFileName(settings.databaseUrl, "_");

    const QList<Dataset> datasets = getValue<QList<Dataset> >(DiamondBuildWorkerFactory::GENOMIC_LIBRARY_ATTR_ID);
    if (!datasets.isEmpty()) {
        foreach (URLContainer *urlContainer, datasets.first().getUrls()) {
            FilesIterator *iterator = urlContainer->getFileUrls();
            while (iterator->hasNext()) {
                settings.genomesUrls << iterator->getNextFile();
            }
        }
    }

    settings.workingDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    settings.workingDir = GUrlUtils::createDirectory(settings.workingDir + DIAMOND_BUILD_DIR , "_", os);
    CHECK_OP(os, settings);

    U2DataPathRegistry *dataPathRegistry = AppContext::getDataPathRegistry();
    SAFE_POINT_EXT(NULL != dataPathRegistry, os.setError("U2DataPathRegistry is NULL"), settings);

    U2DataPath *taxonomyDataPath = dataPathRegistry->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(), os.setError(tr("Taxonomy classification data from NCBI are not available.")), settings);
    settings.taxonMapUrl = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_PROT_ACCESSION_2_TAXID_ITEM_ID);
    settings.taxonNodesUrl = taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID);

    return settings;
}

}   // namespace LocalWorkflow
}   // namespace U2
