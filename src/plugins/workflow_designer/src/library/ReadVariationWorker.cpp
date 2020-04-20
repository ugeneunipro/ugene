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
#include <U2Core/AppResources.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/VariantTrackObject.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "DocActors.h"

#include "ReadVariationWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ReadVariationWorkerFactory::ACTOR_ID("read-variations");

/************************************************************************/
/* Worker */
/************************************************************************/
ReadVariationWorker::ReadVariationWorker(Actor *p)
: GenericDocReader(p)
,splitMode(ReadVariationProto::NOSPLIT)
{

}

void ReadVariationWorker::init() {
    GenericDocReader::init();
    splitMode = ReadVariationProto::SplitAlleles(getValue<int>(ReadVariationProto::SPLIT_ATTR));
    IntegralBus *outBus = dynamic_cast<IntegralBus*>(ch);
    assert(outBus);
    mtype = outBus->getBusType();
}

Task * ReadVariationWorker::createReadTask(const QString &url, const QString &datasetName) {
    bool splitAlleles = (splitMode == ReadVariationProto::SPLIT);
    return new ReadVariationTask(url, datasetName, context->getDataStorage(), splitAlleles);
}

void ReadVariationWorker::onTaskFinished(Task *task) {
    ReadVariationTask *t = qobject_cast<ReadVariationTask*>(task);
    MessageMetadata metadata(t->getUrl(), t->getDatasetName());
    context->getMetadataStorage().put(metadata);
    foreach(const QVariantMap &m, t->takeResults()) {
        cache.append(Message(mtype, m, metadata.getId()));
    }
}

QString ReadVariationWorker::addReadDbObjectToData(const QString &objUrl, QVariantMap &data) {
    SharedDbiDataHandler handler = getDbObjectHandlerByUrl(objUrl);
    data[BaseSlots::VARIATION_TRACK_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);
    //return getObjectName(handler, U2Type::VariantTrack);
    return getObjectName(handler, 5);
}

/************************************************************************/
/* Task */
/************************************************************************/
ReadVariationTask::ReadVariationTask(const QString &url, const QString &_datasetName, DbiDataStorage *storage, bool _splitAlleles)
: Task(tr("Read variations from %1").arg(url), TaskFlag_None), url(url), datasetName(_datasetName), storage(storage), splitAlleles(_splitAlleles)
{

}

ReadVariationTask::~ReadVariationTask() {
    results.clear();
}

const QString & ReadVariationTask::getUrl() const {
    return url;
}

const QString & ReadVariationTask::getDatasetName() const {
    return datasetName;
}

QList<QVariantMap> ReadVariationTask::takeResults() {
    QList<QVariantMap> ret = results;
    results.clear();
    return ret;
}

void ReadVariationTask::prepare() {
    int memUseMB = 0;
    QFileInfo file(url);
    memUseMB = file.size() / (1024*1024);
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (iof->getAdapterId() == BaseIOAdapters::GZIPPED_LOCAL_FILE || iof->getAdapterId() == BaseIOAdapters::GZIPPED_HTTP_FILE) {
        memUseMB *= 2.5; //Need to calculate compress level
    }
    coreLog.trace(QString("load document:Memory resource %1").arg(memUseMB));

    if (memUseMB > 0) {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB, false));
    }
}

void ReadVariationTask::run() {
    QFileInfo fi(url);
    if(!fi.exists()){
        stateInfo.setError(tr("File '%1' not exists").arg(url));
        return;
    }
    QList<DocumentFormat*> fs = DocumentUtils::toFormats(DocumentUtils::detectFormat(url));
    DocumentFormat *format = NULL;

    foreach (DocumentFormat *f, fs) {
        const QSet<GObjectType> &types = f->getSupportedObjectTypes();
        if (types.contains(GObjectTypes::VARIANT_TRACK)) {
            format = f;
            break;
        }
    }

    if (format == NULL) {
        stateInfo.setError(tr("Unsupported document format: %1").arg(url));
        return;
    }
    ioLog.info(tr("Reading variations from %1 [%2]").arg(url).arg(format->getFormatName()));
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    QVariantMap hints;
    if (splitAlleles){
        hints[DocumentReadingMode_SplitVariationAlleles] = true;
    }
    hints.insert(DocumentFormat::DBI_REF_HINT, qVariantFromValue(storage->getDbiRef()));
    QScopedPointer<Document> doc(format->loadDocument(iof, url, hints, stateInfo));
    CHECK_OP(stateInfo, );
    doc->setDocumentOwnsDbiResources(false);

    foreach(GObject* go, doc->findGObjectByType(GObjectTypes::VARIANT_TRACK)) {
        VariantTrackObject *trackObj = dynamic_cast<VariantTrackObject*>(go);
        CHECK_EXT(NULL != trackObj, taskLog.error(tr("Incorrect track object in %1").arg(url)), )

        QVariantMap m;
        m[BaseSlots::URL_SLOT().getId()] = url;
        m[BaseSlots::DATASET_SLOT().getId()] = datasetName;
        SharedDbiDataHandler handler = storage->getDataHandler(trackObj->getEntityRef());
        m[BaseSlots::VARIATION_TRACK_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);
        results.append(m);
    }
}

/************************************************************************/
/* Factory */
/************************************************************************/
const QString ReadVariationProto::SPLIT_ATTR("split-mode");
ReadVariationProto::ReadVariationProto()
: GenericReadDocProto(ReadVariationWorkerFactory::ACTOR_ID)
{
    setCompatibleDbObjectTypes(QSet<GObjectType>() << GObjectTypes::VARIANT_TRACK);

    setDisplayName(ReadVariationWorker::tr("Read Variants"));
    setDocumentation(ReadVariationWorker::tr("Input one or several files with variants in one of the formats, supported by UGENE (e.g. VCF)."
                                             " The element outputs message(s) with the variants data."));
    {
        QMap<Descriptor, DataTypePtr> outTypeMap;
        outTypeMap[BaseSlots::VARIATION_TRACK_SLOT()] = BaseTypes::VARIATION_TRACK_TYPE();
        outTypeMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        outTypeMap[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_VARIATION_TRACK_PORT_ID(), outTypeMap));

        Descriptor outDesc(BasePorts::OUT_VARIATION_TRACK_PORT_ID(),
            ReadVariationWorker::tr("Variation track"),
            ReadVariationWorker::tr("Variation track"));

        ports << new PortDescriptor(outDesc, outTypeSet, false, true);
    }

    Descriptor md(ReadVariationProto::SPLIT_ATTR, ReadVariationWorker::tr("Split Alleles"),
        ReadVariationWorker::tr("If the file contains variations with multiple alleles (chr1 100 C G,A), <i>No split</i> mode sends them \"as is\" to the output, "
        "while <i>Split</i> splits them into two variations (chr1 100 C G and chr1 100 C A)."));

    attrs << new Attribute(md, BaseTypes::NUM_TYPE(), true, NOSPLIT);

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap modeMap;
        QString splitStr = ReadVariationWorker::tr("Split");
        QString nosplitmergeStr = ReadVariationWorker::tr("No split");
        modeMap[splitStr] = SPLIT;
        modeMap[nosplitmergeStr] = NOSPLIT;
        getEditor()->addDelegate(new ComboBoxDelegate(modeMap), SPLIT_ATTR);
    }

    setPrompter(new ReadDocPrompter(ReadVariationWorker::tr("Reads variations from <u>%1</u>.")));
    if (AppContext::isGUIMode()) {
        setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
    }
}

void ReadVariationWorkerFactory::init() {
    ActorPrototype *proto = new ReadVariationProto();
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASRC(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new ReadVariationWorkerFactory());
}

Worker *ReadVariationWorkerFactory::createWorker(Actor *a) {
    return new ReadVariationWorker(a);
}

} // LocalWorkflow
} // U2
