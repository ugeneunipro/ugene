/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ReadAnnotationsWorker.h"

#include <QScopedPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/ZlibAdapter.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/GUIUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "DocActors.h"

namespace U2 {
namespace LocalWorkflow {

const QString ReadAnnotationsWorkerFactory::ACTOR_ID("read-annotations");
namespace {
const QString MODE_ATTR("mode");
const QString ANN_TABLE_NAME_ATTR("ann-table-name");
const QString ANN_TABLE_DEFAULT_NAME("Unknown features");

// Adds all annotations from the "from" table to the "to" table. Annotation groups with the same paths are merged into
// one group. Checks the invariants of passed arguments.
static void addTableToTable(AnnotationTableObject* from, const QScopedPointer<AnnotationTableObject>& to) {
    SAFE_POINT(from != nullptr && to != nullptr, "Table is null", )

    AnnotationGroup* fromRootGroup = from->getRootGroup();
    AnnotationGroup* toRootGroup = to->getRootGroup();
    SAFE_POINT(fromRootGroup != nullptr && toRootGroup != nullptr, "Root is null", )

    QStringList groupPaths;
    fromRootGroup->getSubgroupPaths(groupPaths);
    for (const QString& groupPath : qAsConst(groupPaths)) {
        AnnotationGroup* fromGroup = fromRootGroup->getSubgroup(groupPath, false);
        AnnotationGroup* toGroup = toRootGroup->getSubgroup(groupPath, true);
        SAFE_POINT(fromGroup != nullptr && toGroup != nullptr, QString("Group '%1' is null").arg(groupPath), )

        QList<SharedAnnotationData> groupData;
        for (Annotation* annotation : fromGroup->getAnnotations()) {
            groupData += annotation->getData();
        }
        toGroup->addAnnotations(groupData);
    }
}
}  // namespace

/************************************************************************/
/* Worker */
/************************************************************************/
ReadAnnotationsWorker::ReadAnnotationsWorker(Actor* p)
    : GenericDocReader(p) {
}

void ReadAnnotationsWorker::init() {
    GenericDocReader::init();
    mode = ReadAnnotationsProto::Mode(getValue<int>(MODE_ATTR));
    auto outBus = dynamic_cast<IntegralBus*>(ch);
    SAFE_POINT(outBus != nullptr, "IntegralBus is null!", );
    mtype = outBus->getBusType();
}

Task* ReadAnnotationsWorker::createReadTask(const QString& url, const QString& datasetName) {
    bool mergeAnnotations = mode != ReadAnnotationsProto::SPLIT;
    return new ReadAnnotationsTask(url, datasetName, context, mode, mergeAnnotations ? getValue<QString>(ANN_TABLE_NAME_ATTR) : "");
}

QString ReadAnnotationsWorker::addReadDbObjectToData(const QString& objUrl, QVariantMap& data) {
    SharedDbiDataHandler handler = getDbObjectHandlerByUrl(objUrl);
    data[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);
    // Using local var to have memory address for the static constant. TODO: switch build to std++17.
    // See https://en.cppreference.com/w/cpp/language/static
    auto type = U2Type::AnnotationTable;
    return getObjectName(handler, type);
}

void ReadAnnotationsWorker::onTaskFinished(Task* task) {
    auto readAnnotationsTask = qobject_cast<ReadAnnotationsTask*>(task);
    if (mode == ReadAnnotationsProto::MERGE_FILES) {
        datasetData << readAnnotationsTask->takeResults();
        return;
    }

    sendData(readAnnotationsTask->takeResults());
}

void ReadAnnotationsWorker::sl_datasetEnded() {
    CHECK(datasetData.size() > 0, );

    U2OpStatusImpl os;
    DbiOperationsBlock operationBlock(context->getDataStorage()->getDbiRef(), os);
    SAFE_POINT_OP(os, )
    QScopedPointer<AnnotationTableObject> mergedAnnotationTable(new AnnotationTableObject(
        getValue<QString>(ANN_TABLE_NAME_ATTR), context->getDataStorage()->getDbiRef()));

    foreach (const QVariantMap& m, datasetData) {
        QVariant annsVar = m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()];
        QList<AnnotationTableObject*> annotations = StorageUtils::getAnnotationTableObjects(context->getDataStorage(), annsVar);
        for (AnnotationTableObject* t : qAsConst(annotations)) {
            addTableToTable(t, mergedAnnotationTable);
        }
    }

    SharedDbiDataHandler resultTableId = context->getDataStorage()->putAnnotationTable(mergedAnnotationTable.data());

    QVariantMap m;
    m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(resultTableId);
    m[BaseSlots::DATASET_SLOT().getId()] = datasetData.first()[BaseSlots::DATASET_SLOT().getId()];

    sendData(QList<QVariantMap>() << m);
    datasetData.clear();
}

void ReadAnnotationsWorker::sendData(const QList<QVariantMap>& data) {
    foreach (const QVariantMap& m, data) {
        QString url = m[BaseSlots::URL_SLOT().getId()].toString();
        QString datasetName = m[BaseSlots::DATASET_SLOT().getId()].toString();
        MessageMetadata metadata(url, datasetName);
        context->getMetadataStorage().put(metadata);
        cache.append(Message(mtype, m, metadata.getId()));
    }
}

/************************************************************************/
/* Factory */
/************************************************************************/
ReadAnnotationsProto::ReadAnnotationsProto()
    : GenericReadDocProto(ReadAnnotationsWorkerFactory::ACTOR_ID) {
    setCompatibleDbObjectTypes(QSet<GObjectType>() << GObjectTypes::ANNOTATION_TABLE);

    setDisplayName(ReadAnnotationsWorker::tr("Read Annotations"));
    setDocumentation(ReadAnnotationsWorker::tr("Input one or several files with annotations: a file may also contain a sequence (e.g. GenBank format)"
                                               " or contain annotations only (e.g. GTF format). The element outputs message(s) with the annotations data."));
    {  // ports description
        QMap<Descriptor, DataTypePtr> outTypeMap;
        outTypeMap[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        outTypeMap[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        outTypeMap[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_ANNOTATIONS_PORT_ID(), outTypeMap));

        Descriptor outDesc(BasePorts::OUT_ANNOTATIONS_PORT_ID(),
                           ReadAnnotationsWorker::tr("Annotations"),
                           ReadAnnotationsWorker::tr("Annotations."));

        ports << new PortDescriptor(outDesc, outTypeSet, false, true);
    }

    Descriptor md(MODE_ATTR, ReadAnnotationsWorker::tr("Mode"), ReadAnnotationsWorker::tr("<ul>"
                                                                                          "<li><i>\"Separate\"</i> mode keeps the tables as they are;</li>"
                                                                                          "<li><i>\"Merge from file\"</i> unites annotation tables from one file into one annotations table;</li>"
                                                                                          "<li><i>\"Merge from dataset\"</i> unites all annotation tables from all files from dataset;</li>"
                                                                                          "</ul>"));
    attrs << new Attribute(md, BaseTypes::NUM_TYPE(), true, SPLIT);

    Descriptor annTableNameDesc(ANN_TABLE_NAME_ATTR, ReadAnnotationsWorker::tr("Annotation table name"), ReadAnnotationsWorker::tr("The name for the result annotation table that contains merged annotation data from file or dataset."));
    Attribute* objNameAttr = new Attribute(annTableNameDesc, BaseTypes::STRING_TYPE(), false, ANN_TABLE_DEFAULT_NAME);
    objNameAttr->addRelation(new VisibilityRelation(MODE_ATTR, QVariantList() << MERGE << MERGE_FILES));

    attrs << objNameAttr;

    {
        QVariantMap modeMap;
        QString splitStr = ReadAnnotationsWorker::tr("Separate annotation tables");
        QString mergeStr = ReadAnnotationsWorker::tr("Merge annotation tables from file");
        QString mergeFilesStr = ReadAnnotationsWorker::tr("Merge all annotation tables from dataset");
        modeMap[splitStr] = SPLIT;
        modeMap[mergeStr] = MERGE;
        modeMap[mergeFilesStr] = MERGE_FILES;
        getEditor()->addDelegate(new ComboBoxDelegate(modeMap), MODE_ATTR);
    }

    setPrompter(new ReadDocPrompter(ReadAnnotationsWorker::tr("Reads annotations from <u>%1</u>.")));
    if (AppContext::isGUIMode()) {
        setIcon(QIcon(":/U2Designer/images/blue_circle.png"));
    }
}

void ReadAnnotationsWorkerFactory::init() {
    ActorPrototype* proto = new ReadAnnotationsProto();
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASRC(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new ReadAnnotationsWorkerFactory());
}

Worker* ReadAnnotationsWorkerFactory::createWorker(Actor* a) {
    return new ReadAnnotationsWorker(a);
}

/************************************************************************/
/* Task */
/************************************************************************/
ReadAnnotationsTask::ReadAnnotationsTask(const QString& url,
                                         const QString& datasetName,
                                         WorkflowContext* context,
                                         const ReadAnnotationsProto::Mode& mergeAnnotations,
                                         const QString& mergedAnnTableName)
    : Task(tr("Read annotations from %1").arg(url), TaskFlag_None),
      url(url),
      datasetName(datasetName),
      mergeAnnotations(mergeAnnotations),
      mergedAnnTableName(mergedAnnTableName),
      context(context) {
    SAFE_POINT(context != nullptr, "Invalid workflow context encountered!", );
}

void ReadAnnotationsTask::prepare() {
    QFileInfo file(url);
    int memUseMB = file.size() / (1024 * 1024) + 1;
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (BaseIOAdapters::GZIPPED_LOCAL_FILE == iof->getAdapterId()) {
        memUseMB = ZlibAdapter::getUncompressedFileSizeInBytes(url) / (1024 * 1024) + 1;
    } else if (BaseIOAdapters::GZIPPED_HTTP_FILE == iof->getAdapterId()) {
        memUseMB *= 2.5;  // Need to calculate compress level
    }
    coreLog.trace(QString("Load annotations: Memory resource %1").arg(memUseMB));

    if (memUseMB > 0) {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB, false));
    }
}

void ReadAnnotationsTask::run() {
    QFileInfo fi(url);
    CHECK_EXT(fi.exists(), stateInfo.setError(tr("File '%1' does not exist").arg(url)), );

    DocumentFormat* format = nullptr;
    QList<DocumentFormat*> fs = DocumentUtils::toFormats(DocumentUtils::detectFormat(url));
    foreach (DocumentFormat* f, fs) {
        if (f->getSupportedObjectTypes().contains(GObjectTypes::ANNOTATION_TABLE)) {
            format = f;
            break;
        }
    }
    CHECK_EXT(format != nullptr, stateInfo.setError(tr("Unsupported document format: %1").arg(url)), );

    ioLog.info(tr("Reading annotations from %1 [%2]").arg(url).arg(format->getFormatName()));
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue<U2DbiRef>(context->getDataStorage()->getDbiRef());
    QScopedPointer<Document> doc(format->loadDocument(iof, url, hints, stateInfo));
    CHECK_OP(stateInfo, );

    QList<GObject*> annsObjList = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);

    QVariantMap m;
    m[BaseSlots::URL_SLOT().getId()] = url;
    m[BaseSlots::DATASET_SLOT().getId()] = datasetName;

    bool isMerge = mergeAnnotations == ReadAnnotationsProto::MERGE && annsObjList.size() > 1;

    // Only needed if merge mode.
    QScopedPointer<AnnotationTableObject> mergedAnnotationTable;
    if (isMerge) {
        QString objectName = mergedAnnTableName.isEmpty() ? ANN_TABLE_DEFAULT_NAME : mergedAnnTableName;
        mergedAnnotationTable.reset(new AnnotationTableObject(objectName, context->getDataStorage()->getDbiRef()));
    }

    for (GObject* go : qAsConst(annsObjList)) {
        auto annsObj = dynamic_cast<AnnotationTableObject*>(go);
        CHECK_EXT(annsObj != nullptr, stateInfo.setError("NULL annotations object"), );

        // If "SPLIT" -- transfer (write to the result) tables of annotations from files as is.
        // If "MERGE" and there is only one annotation table, transfer the table as is.
        // If "MERGE_FILES", transfer the file tables as is and merge them in ReadAnnotationsWorker::sl_datasetEnded.
        // Otherwise ("MERGE" with several tables of annotations in one file), merge these tables into one.
        if (!isMerge) {
            SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(annsObj);
            m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(tableId);
            results.append(m);

            SAFE_POINT(doc->removeObject(go, DocumentObjectRemovalMode_Detach),
                       QString("Cannot remove object '%1' from document '%2'").arg(go->getGObjectName(), doc->getName()), )
        } else {
            U2OpStatusImpl os;
            DbiOperationsBlock operationBlock(context->getDataStorage()->getDbiRef(), os);
            SAFE_POINT_OP(os, )
            addTableToTable(annsObj, mergedAnnotationTable);
        }
    }

    if (isMerge) {
        SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(mergedAnnotationTable.get());
        m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(tableId);
        results.append(m);
    }
}

QList<QVariantMap> ReadAnnotationsTask::takeResults() {
    QList<QVariantMap> ret = results;
    results.clear();
    return ret;
}

void ReadAnnotationsTask::cleanup() {
    results.clear();
}

}  // namespace LocalWorkflow
}  // namespace U2
