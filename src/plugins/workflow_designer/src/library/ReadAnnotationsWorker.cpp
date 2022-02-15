/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
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
#include <U2Lang/NoFailTaskWrapper.h>
#include <U2Lang/WorkflowEnv.h>

#include "DocActors.h"

namespace U2 {
namespace LocalWorkflow {

const QString ReadAnnotationsWorkerFactory::ACTOR_ID("read-annotations");
namespace {
const QString MODE_ATTR("mode");
const QString ANN_TABLE_NAME_ATTR("ann-table-name");
const QString ANN_TABLE_DEFAULT_NAME("Unknown features");

// Merges tables into one with the name "mergedTableName", saved in the db. Annotation groups with the same paths are
// merged into one group. Arguments must be non-null.
static SharedDbiDataHandler mergeAnnotationTables(DbiDataStorage* storage,
                                                  const QList<AnnotationTableObject*>& tables,
                                                  const QString& mergedTableName) {
    U2OpStatusImpl os;
    U2DbiRef dbiRef = storage->getDbiRef();
    DbiOperationsBlock operationBlock(dbiRef, os);
    SAFE_POINT_OP(os, {})
    QScopedPointer<AnnotationTableObject> mergedAnnotationTable(new AnnotationTableObject(mergedTableName, dbiRef));
    AnnotationGroup* mergedRootGroup = mergedAnnotationTable->getRootGroup();

    for (AnnotationTableObject* table : qAsConst(tables)) {
        AnnotationGroup* rootGroup = table->getRootGroup();
        SAFE_POINT(rootGroup != nullptr, QString("Root '%1' is null").arg(table->getGObjectName()), {})

        QStringList groupPaths;
        rootGroup->getSubgroupPaths(groupPaths);
        for (const QString& groupPath : qAsConst(groupPaths)) {
            AnnotationGroup* group = rootGroup->getSubgroup(groupPath, false);
            SAFE_POINT(group != nullptr, QString("Group '%1' is null").arg(groupPath), {});

            QList<SharedAnnotationData> groupData;
            for (Annotation* annotation : group->getAnnotations()) {
                groupData += annotation->getData();
            }

            AnnotationGroup* mergedGroup = mergedRootGroup->getSubgroup(groupPath, true);
            SAFE_POINT(mergedGroup != nullptr, QString("MergedGroup '%1' is null").arg(groupPath), {});
            mergedGroup->addAnnotations(groupData);
        }
    }
    return storage->putAnnotationTable(mergedAnnotationTable.get());
}
}  // namespace

/************************************************************************/
/* Worker */
/************************************************************************/
ReadAnnotationsWorker::ReadAnnotationsWorker(Actor* p)
    : GenericDocReader(p), mode(ReadAnnotationsProto::SPLIT) {
}

void ReadAnnotationsWorker::init() {
    GenericDocReader::init();
    mode = ReadAnnotationsProto::Mode(getValue<int>(MODE_ATTR));
    IntegralBus* outBus = dynamic_cast<IntegralBus*>(ch);
    assert(outBus);
    mtype = outBus->getBusType();
}

Task* ReadAnnotationsWorker::tick() {
    // Difference from GenericReadWorker::tick in datasetData.
    files->tryEmitDatasetEnded();

    bool sendMessages = !cache.isEmpty();
    bool hasNext = files->hasNext();
    bool needMerge = !datasetData.isEmpty();  // Can only be true in ReadAnnotationsProto::MERGE_FILES mode.

    while (!cache.isEmpty()) {
        ch->put(cache.takeFirst());
    }

    if (!sendMessages) {
        if (hasNext) {
            QString newUrl = files->getNextFile();
            return GenericDocReader::createReadTask(newUrl, files->getLastDatasetName());
        }
        if (needMerge) {
            DbiDataStorage* storage = context->getDataStorage();
            SAFE_POINT(storage, "Storage is null", nullptr)
            NoFailTaskWrapper* mergeTask = new NoFailTaskWrapper(new MergeAnnotationsTask(
                datasetData, getValue<QString>(ANN_TABLE_NAME_ATTR), storage));
            datasetData.clear();
            connect(mergeTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
            return mergeTask;
        }
        // Else done, see below.
    }
    if (!hasNext && !needMerge) {
        // The cache is empty and the no more URLs and all tables are merged (if ReadAnnotationsProto::MERGE_FILES
        // mode) -> finish the worker.
        setDone();
        ch->setEnded();
    }
    return nullptr;
}

Task* ReadAnnotationsWorker::createReadTask(const QString& url, const QString& datasetName) {
    bool mergeAnnotations = (mode != ReadAnnotationsProto::SPLIT);
    return new ReadAnnotationsTask(url, datasetName, context, mode, mergeAnnotations ? getValue<QString>(ANN_TABLE_NAME_ATTR) : "");
}

QString ReadAnnotationsWorker::addReadDbObjectToData(const QString& objUrl, QVariantMap& data) {
    SharedDbiDataHandler handler = getDbObjectHandlerByUrl(objUrl);
    data[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(handler);
    // return getObjectName(handler, U2Type::AnnotationTable);
    return getObjectName(handler, 10);
}

void ReadAnnotationsWorker::onTaskFinished(Task* task) {
    if (MergeAnnotationsTask* t = qobject_cast<MergeAnnotationsTask*>(task)) {
        sendData(t->takeResults());
        return;
    }
    ReadAnnotationsTask* t = qobject_cast<ReadAnnotationsTask*>(task);
    if (ReadAnnotationsProto::MERGE_FILES == mode) {
        datasetData << t->takeResults();
        return;
    }

    sendData(t->takeResults());
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
/* ReadAnnotationsTask */
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
    SAFE_POINT(nullptr != context, "Invalid workflow context encountered!", );
}

void ReadAnnotationsTask::prepare() {
    int memUseMB = 0;
    QFileInfo file(url);
    memUseMB = file.size() / (1024 * 1024) + 1;
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
    CHECK_EXT(nullptr != format, stateInfo.setError(tr("Unsupported document format: %1").arg(url)), );

    ioLog.info(tr("Reading annotations from %1 [%2]").arg(url).arg(format->getFormatName()));
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue<U2DbiRef>(context->getDataStorage()->getDbiRef());
    QScopedPointer<Document> doc(format->loadDocument(iof, url, hints, stateInfo));
    CHECK_OP(stateInfo, );

    QList<GObject*> annsObjList = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    QList<AnnotationTableObject*> annotationTables;
    for (GObject* go : qAsConst(annsObjList)) {
        auto annsObj = dynamic_cast<AnnotationTableObject*>(go);
        CHECK_EXT(annsObj != nullptr, setError(L10N::nullPointerError("annsObj")), );
        annotationTables += annsObj;
    }

    QVariantMap m;
    m[BaseSlots::URL_SLOT().getId()] = url;
    m[BaseSlots::DATASET_SLOT().getId()] = datasetName;

    auto addTableToResult = [&m, this](const SharedDbiDataHandler& tableId) {
        m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(tableId);
        results.append(m);
    };
    DbiDataStorage* storage = context->getDataStorage();

    // If "SPLIT" -- transfer (write to the result) tables of annotations from files as is.
    // If "MERGE" and there is only one annotation table, transfer the table as is.
    // If "MERGE_FILES", transfer the file tables as is and merge them in MergeAnnotationsTask.
    // Otherwise ("MERGE" with several tables of annotations in one file), merge these tables into one.
    if (mergeAnnotations != ReadAnnotationsProto::MERGE || annsObjList.size() == 1) {
        for (AnnotationTableObject* go : qAsConst(annotationTables)) {
            QScopedPointer<AnnotationTableObject> annotationTableClone(qobject_cast<AnnotationTableObject*>(go->clone(
                storage->getDbiRef(), stateInfo)));
            CHECK_OP(stateInfo, )
            CHECK_EXT(annotationTableClone != nullptr,
                      setError(L10N::nullPointerError(QString("annotationTableClone '%1'")
                                                          .arg(go->getGObjectName()))), );
            SharedDbiDataHandler tableId = storage->putAnnotationTable(annotationTableClone.data());
            CHECK_EXT(tableId, setError(L10N::nullPointerError(QString("tableId '%1'").arg(go->getGObjectName()))), )
            addTableToResult(tableId);
        }
        return;
    }
    SharedDbiDataHandler tableId = mergeAnnotationTables(storage, annotationTables, mergedAnnTableName);
    CHECK_EXT(tableId, setError(L10N::nullPointerError("tableId")), )
    addTableToResult(tableId);
}

QList<QVariantMap> ReadAnnotationsTask::takeResults() {
    QList<QVariantMap> ret = results;
    results.clear();

    return ret;
}

void ReadAnnotationsTask::cleanup() {
    results.clear();
}

/************************************************************************/
/* MergeAnnotationsTask */
/************************************************************************/
MergeAnnotationsTask::MergeAnnotationsTask(const QList<QVariantMap>& datasetData,
                                           const QString& mergedAnnTableName,
                                           DbiDataStorage* storage)
    : Task(tr("Merge annotations from dataset"), TaskFlag_None), datasetData(datasetData),
      mergedAnnTableName(mergedAnnTableName), storage(storage) {
}

void MergeAnnotationsTask::run() {
    // Preparing for the merge: reading AnnotationTableObjects from the db.
    GAutoDeleteList<AnnotationTableObject> anns;
    for (const QVariantMap& m : qAsConst(datasetData)) {
        const QVariant annsVar = m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()];
        anns.qlist += StorageUtils::StorageUtils::getAnnotationTableObjects(storage, annsVar);
    }
    // Merging tables.
    const SharedDbiDataHandler tableId = mergeAnnotationTables(storage, anns.qlist, mergedAnnTableName);
    CHECK_EXT(tableId, setError(L10N::nullPointerError("tableId")), )

    // Put the merged tables into db.
    QVariantMap m;
    m[BaseSlots::ANNOTATION_TABLE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(tableId);
    m[BaseSlots::DATASET_SLOT().getId()] = datasetData.first()[BaseSlots::DATASET_SLOT().getId()];
    results.append(m);

    // Deleting input db tables.
    DbiOperationsBlock opBlock(storage->getDbiRef(), stateInfo);
    CHECK_OP(stateInfo, );

    DbiConnection con(storage->getDbiRef(), stateInfo);
    CHECK_OP(stateInfo, );
    CHECK(con.dbi->getFeatures().contains(U2DbiFeature_RemoveObjects), );

    for (AnnotationTableObject* object : qAsConst(anns.qlist)) {
        con.dbi->getObjectDbi()->removeObject(object->getEntityRef().entityId, true, stateInfo);
        CHECK_OP_BREAK(stateInfo)
    }
}

QList<QVariantMap> MergeAnnotationsTask::takeResults() const {
    return results;
}

}  // namespace LocalWorkflow
}  // namespace U2
