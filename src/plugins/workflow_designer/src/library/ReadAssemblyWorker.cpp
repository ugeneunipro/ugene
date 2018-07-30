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

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/FileStorageUtils.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Formats/BAMUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/ReadDocumentTaskFactory.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowTasksRegistry.h>

#include "DocActors.h"

#include "ReadAssemblyWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ReadAssemblyWorkerFactory::ACTOR_ID("read-assembly");

/************************************************************************/
/* Worker */
/************************************************************************/
ReadAssemblyWorker::ReadAssemblyWorker(Actor *p)
: GenericDocReader(p)
{

}

void ReadAssemblyWorker::init() {
    GenericDocReader::init();
    IntegralBus *outBus = dynamic_cast<IntegralBus*>(ch);
    assert(outBus);
    mtype = outBus->getBusType();
}

Task * ReadAssemblyWorker::createReadTask(const QString &url, const QString &datasetName) {
    WorkflowTasksRegistry *registry = WorkflowEnv::getWorkflowTasksRegistry();
    SAFE_POINT(NULL != registry, "NULL WorkflowTasksRegistry", NULL);
    ReadDocumentTaskFactory *factory = registry->getReadDocumentTaskFactory(ReadFactories::READ_ASSEMBLY);
    SAFE_POINT(NULL != factory, QString("NULL WorkflowTasksRegistry: %1").arg(ReadFactories::READ_ASSEMBLY), NULL);

    QVariantMap hints;
    hints[BaseSlots::DATASET_SLOT().getId()] = datasetName;
    return factory->createTask(url, hints, context);
}

void ReadAssemblyWorker::onTaskFinished(Task *task) {
    ReadDocumentTask *t = qobject_cast<ReadDocumentTask*>(task);
    QList<SharedDbiDataHandler> result = t->takeResult();
    QString url = t->getUrl();
    MessageMetadata metadata(t->getUrl(), t->getDatasetName());
    context->getMetadataStorage().put(metadata);
    foreach(const SharedDbiDataHandler &handler, result) {
        QVariantMap m;
        m[BaseSlots::URL_SLOT().getId()] = url;
        m[BaseSlots::DATASET_SLOT().getId()] = t->getDatasetName();
        m[BaseSlots::ASSEMBLY_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);

        cache.append(Message(mtype, m, metadata.getId()));
    }
    foreach (const QString &url, t->getProducedFiles()) {
        context->getMonitor()->addOutputFile(url, getActor()->getId());
    }
}

QString ReadAssemblyWorker::addReadDbObjectToData(const QString &objUrl, QVariantMap &data) {
    SharedDbiDataHandler handler = getDbObjectHandlerByUrl(objUrl);
    data[BaseSlots::ASSEMBLY_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);
    //return getObjectName(handler, U2Type::Assembly);
    return getObjectName(handler, 4);
}

/************************************************************************/
/* Factory */
/************************************************************************/
ReadAssemblyProto::ReadAssemblyProto()
: GenericReadDocProto(ReadAssemblyWorkerFactory::ACTOR_ID)
{
    setCompatibleDbObjectTypes(QSet<GObjectType>() << GObjectTypes::ASSEMBLY);

    setDisplayName(ReadAssemblyWorker::tr("Read NGS Reads Assembly"));
    setDocumentation(ReadAssemblyWorker::tr("Input one or several files with assembled NGS reads in SAM, BAM, or UGENEDB format."
                                            " The element outputs message(s) with the assembled reads data."
                                            "<br/><br/>Note that some tools require URL(s) of the files as input, not the assembled reads data."));

    { // ports description
        QMap<Descriptor, DataTypePtr> outTypeMap;
        outTypeMap[BaseSlots::ASSEMBLY_SLOT()] = BaseTypes::ASSEMBLY_TYPE();
        outTypeMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        outTypeMap[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_ASSEMBLY_PORT_ID(), outTypeMap));

        Descriptor outDesc(BasePorts::OUT_ASSEMBLY_PORT_ID(),
            ReadAssemblyWorker::tr("Assembly"),
            ReadAssemblyWorker::tr("Assembly"));

        ports << new PortDescriptor(outDesc, outTypeSet, false, true);
    }

    setPrompter(new ReadDocPrompter(ReadAssemblyWorker::tr("Reads assembly from <u>%1</u>.")));
    if (AppContext::isGUIMode()) {
        setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
    }
}

void ReadAssemblyWorkerFactory::init() {
    ActorPrototype *proto = new ReadAssemblyProto();
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASRC(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new ReadAssemblyWorkerFactory());
}

Worker *ReadAssemblyWorkerFactory::createWorker(Actor *a) {
    return new ReadAssemblyWorker(a);
}

} // LocalWorkflow
} // U2
