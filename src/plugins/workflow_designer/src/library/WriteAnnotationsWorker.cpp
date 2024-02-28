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

#include "WriteAnnotationsWorker.h"

#include <QScopedPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ImportObjectToDatabaseTask.h>
#include <U2Core/MultiTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/ExportAnnotations2CSVTask.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "BaseDocWriter.h"
#include "util/WriteAnnotationsValidator.h"

namespace U2 {
namespace LocalWorkflow {

const QString WriteAnnotationsWorkerFactory::ACTOR_ID("write-annotations");

static const QString WRITE_ANNOTATIONS_IN_TYPE_ID("write-annotations-in-type");
static const QString CSV_FORMAT_ID("csv");
static const QString CSV_FORMAT_NAME("CSV");
static const QString ANN_TABLE_NAME_4_LOCAL_ST("annotations-name");
static const QString ANN_TABLE_NAME_4_SHARED_ST("ann-obj-name");
static const QString ANNOTATIONS_NAME_DEF_VAL("Unknown features");
static const QString SEPARATOR("separator");
static const QString SEPARATOR_DEFAULT_VALUE(",");
static const QString WRITE_NAMES("write_names");
static const QString MERGE_TABLES_LOCAL("merge");
static const QString MERGE_TABLES_SHARED("merge_in_shared_db");

/*******************************
 * WriteAnnotationsWorker
 *******************************/
WriteAnnotationsWorker::WriteAnnotationsWorker(Actor* p)
    : BaseWorker(p) {
}

WriteAnnotationsWorker::~WriteAnnotationsWorker() {
    qDeleteAll(createdAnnotationObjects);
}

void WriteAnnotationsWorker::init() {
    annotationsPort = ports.value(BasePorts::IN_ANNOTATIONS_PORT_ID());
}

static QString getExtension(const QString& formatId) {
    CHECK(formatId != CSV_FORMAT_ID, "csv");
    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
    CHECK(format != nullptr, "");
    QStringList exts = format->getSupportedDocumentFileExtensions();
    CHECK(!exts.isEmpty(), "");
    return exts[0];
}

Task* WriteAnnotationsWorker::takeParameters(QString& formatId, SaveDocFlags& fl, QString& resultPath, U2DbiRef& dstDbiRef, WriteAnnotationsWorker::DataStorage& storage) {
    const QString storageStr = getValue<QString>(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId());
    if (storageStr == BaseAttributes::LOCAL_FS_DATA_STORAGE()) {
        storage = LocalFs;
        formatId = getValue<QString>(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId());
        DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
        fl = SaveDocFlags(getValue<uint>(BaseAttributes::FILE_MODE_ATTRIBUTE().getId()));
        resultPath = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
        if (formatId != CSV_FORMAT_ID && format == nullptr) {
            return new FailTask(tr("Unrecognized formatId: '%1'").arg(formatId));
        }
    } else if (storageStr == BaseAttributes::SHARED_DB_DATA_STORAGE()) {
        storage = SharedDb;
        dstDbiRef = SharedDbUrlUtils::getDbRefFromEntityUrl(getValue<QString>(BaseAttributes::DATABASE_ATTRIBUTE().getId()));
        CHECK(dstDbiRef.isValid(), new FailTask(tr("Invalid shared DB URL")));
        resultPath = getValue<QString>(BaseAttributes::DB_PATH().getId());
        CHECK(!resultPath.isEmpty(), new FailTask(tr("Invalid path in shared DB")));
    } else {
        return new FailTask(tr("Unrecognized data storage: '%1'").arg(storageStr));
    }
    return nullptr;
}

void WriteAnnotationsWorker::updateResultPath(int metadataId, const QString& formatId, DataStorage storage, QString& resultPath, bool byDataset) {
    CHECK(storage == LocalFs, );
    CHECK(resultPath.isEmpty(), );

    MessageMetadata metadata = context->getMetadataStorage().get(metadataId);
    QString suffix = getValue<QString>(BaseAttributes::URL_SUFFIX().getId());
    QString defaultName = actor->getId() + "_output";
    resultPath = BaseDocWriter::generateUrl(metadata, byDataset, suffix, getExtension(formatId), defaultName);
}

Task* WriteAnnotationsWorker::tick() {
    QString formatId;
    SaveDocFlags fl;
    QString resultPath;
    U2DbiRef dstDbiRef;
    DataStorage storage;

    Task* failTask = takeParameters(formatId, fl, resultPath, dstDbiRef, storage);
    CHECK(failTask == nullptr, failTask);

    bool merge = getValue<bool>(MERGE_TABLES_LOCAL);
    while (annotationsPort->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(annotationsPort);
        if (inputMessage.isEmpty()) {
            continue;
        }
        const QVariantMap qm = inputMessage.getData().toMap();

        if (storage == LocalFs) {
            resultPath = qm.value(BaseSlots::URL_SLOT().getId(), resultPath).value<QString>();
            // We have two cases, when we need to write several annotation objects to the same file:
            // 1) If the path to the output file has been defined as a parameter,
            // 2) If there are several (two or more) annotation objects has come from the same file.
            //
            // And we have a case, when we need to write each annotation object to the separate file (roll file name):
            // 1) When we receive annotations from files, which have the same name, but located in different directories.
            // The flag below handle all these cases.
            bool write2TheSameFile = !resultPath.isEmpty();
            updateResultPath(inputMessage.getMetadataId(), formatId, storage, resultPath, merge);
            CHECK(!resultPath.isEmpty(), new FailTask(tr("Unspecified URL to write")));
            resultPath = context->absolutePath(resultPath);
            // to avoid uniting at the same file in case of similar names
            if (!write2TheSameFile) {
                resultPath = GUrlUtils::rollFileName(resultPath, "_", existedResultFiles);
            }
        }

        fetchIncomingAnnotations(qm, resultPath);
        existedResultFiles << resultPath;
    }

    bool done = annotationsPort->isEnded();
    if (!done) {
        return nullptr;
    }

    setDone();
    if (storage == LocalFs) {
        return getSaveDocTask(formatId, fl);
    } else if (storage == SharedDb) {
        return getSaveObjTask(dstDbiRef);
    } else {
        // this branch must never execute, it was added to avoid a compiler warning
        return new FailTask(tr("Unrecognized data storage"));
    }
}

QString WriteAnnotationsWorker::fetchIncomingSequenceName(const QVariantMap& incomingData) {
    const SharedDbiDataHandler seqId = incomingData.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
    QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));

    return seqObj.isNull() ? QString() : seqObj->getSequenceName();
}

bool WriteAnnotationsWorker::getMergeAttribute() const {
    const QString storageStr = getValue<QString>(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId());

    bool merge = false;
    if (BaseAttributes::LOCAL_FS_DATA_STORAGE() == storageStr) {
        merge = getValue<bool>(MERGE_TABLES_LOCAL);
    } else if (BaseAttributes::SHARED_DB_DATA_STORAGE() == storageStr) {
        merge = getValue<bool>(MERGE_TABLES_SHARED);
    } else {
        FAIL("Invalid worker data storage attribute", false);
    }
    return merge;
}

QString WriteAnnotationsWorker::getAnnotationTableName() const {
    const QString storageStr = getValue<QString>(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId());

    QString objName;
    if (storageStr == BaseAttributes::LOCAL_FS_DATA_STORAGE()) {
        objName = getValue<QString>(ANN_TABLE_NAME_4_LOCAL_ST);
    } else if (storageStr == BaseAttributes::SHARED_DB_DATA_STORAGE()) {
        objName = getValue<QString>(ANN_TABLE_NAME_4_SHARED_ST);
    } else {
        FAIL("Invalid worker data storage attribute", ANNOTATIONS_NAME_DEF_VAL);
    }

    if (objName.isEmpty()) {
        objName = ANNOTATIONS_NAME_DEF_VAL;
        coreLog.details(tr("Annotations name not specified. Default value used: '%1'").arg(objName));
    }
    return objName;
}

void WriteAnnotationsWorker::fetchIncomingAnnotations(const QVariantMap& incomingData, const QString& resultPath) {
    QVariant annVar = incomingData[BaseSlots::ANNOTATION_TABLE_SLOT().getId()];
    QList<AnnotationTableObject*> annTables = StorageUtils::getAnnotationTableObjects(context->getDataStorage(), annVar);
    annotationsByUrl[resultPath] << annTables;

    QString seqObjName = fetchIncomingSequenceName(incomingData);
    bool isWriteNames = getValue<bool>(WRITE_NAMES);
    if (isWriteNames && !seqObjName.isEmpty()) {
        for (AnnotationTableObject* annTable : qAsConst(annTables)) {
            foreach (Annotation* annotation, annTable->getAnnotations()) {
                U2Qualifier seqNameQual;
                seqNameQual.name = ExportAnnotations2CSVTask::SEQUENCE_NAME;
                seqNameQual.value = seqObjName;
                annotation->addQualifier(seqNameQual);
            }
        }
    }
}

void WriteAnnotationsWorker::mergeAnnTablesIfNecessary(QList<AnnotationTableObject*>& annTables) const {
    CHECK(getMergeAttribute() == true, );

    auto mergedTable = new AnnotationTableObject(getAnnotationTableName(), context->getDataStorage()->getDbiRef());
    for (AnnotationTableObject* annTable : qAsConst(annTables)) {
        QList<SharedAnnotationData> anns;
        foreach (Annotation* annotation, annTable->getAnnotations()) {
            anns.append(annotation->getData());
        }
        mergedTable->addAnnotations(anns);
    }

    qDeleteAll(annTables);
    annTables.clear();
    annTables << mergedTable;
}

Task* WriteAnnotationsWorker::createWriteMultitask(const QList<Task*>& taskList) const {
    if (taskList.isEmpty()) {
        monitor()->addError(tr("Nothing to write"), getActorId(), WorkflowNotification::U2_WARNING);
        return nullptr;
    } else if (taskList.size() == 1) {
        return taskList.first();
    }
    return new MultiTask(QObject::tr("Save annotations"), taskList);
}

Task* WriteAnnotationsWorker::getSaveObjTask(const U2DbiRef& dstDbiRef) const {
    QList<Task*> taskList;
    foreach (const QString& path, annotationsByUrl.keys()) {
        QList<AnnotationTableObject*> annTables = annotationsByUrl.value(path);
        mergeAnnTablesIfNecessary(annTables);
        for (AnnotationTableObject* annTable : qAsConst(annTables)) {
            taskList << new ImportObjectToDatabaseTask(annTable, dstDbiRef, path);
        }
    }
    return createWriteMultitask(taskList);
}

static QString rollName(const QString& name, const QSet<QString>& usedNames) {
    QString result = name;
    int counter = 1;
    while (usedNames.contains(result)) {
        result = name + QString(" %1").arg(counter);
        counter++;
    }
    return result;
}

static void updateAnnotationsName(AnnotationTableObject* object, QSet<QString>& usedNames) {
    QString newName = rollName(object->getGObjectName(), usedNames);
    usedNames << newName;
    if (object->getGObjectName() != newName) {
        object->setGObjectName(newName);
    }
}

Task* WriteAnnotationsWorker::getSaveDocTask(const QString& formatId, SaveDocFlags& fl) {
    SAFE_POINT(!formatId.isEmpty(), "Invalid format ID", nullptr);

    QList<Task*> taskList;
    QSet<QString> excludeFileNames = DocumentUtils::getNewDocFileNameExcludesHint();
    foreach (const QString& filepath, annotationsByUrl.keys()) {
        QList<AnnotationTableObject*> annTables = annotationsByUrl.value(filepath);
        mergeAnnTablesIfNecessary(annTables);

        Task* task = nullptr;
        if (formatId == CSV_FORMAT_ID) {
            createdAnnotationObjects << annTables;  // will delete in destructor
            TaskStateInfo ti;
            if (fl.testFlag(SaveDoc_Roll) && !GUrlUtils::renameFileWithNameRoll(filepath, ti, excludeFileNames, &coreLog)) {
                return new FailTask(ti.getError());
            }

            QList<Annotation*> annotations;
            for (AnnotationTableObject* annTable : qAsConst(annTables)) {
                annotations << annTable->getAnnotations();
            }

            task = new ExportAnnotations2CSVTask(annotations, QByteArray(), QString(), nullptr, false, getValue<bool>(WRITE_NAMES), filepath, fl.testFlag(SaveDoc_Append), getValue<QString>(SEPARATOR));
        } else {
            fl |= SaveDoc_DestroyAfter;
            IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(filepath));
            DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
            U2OpStatusImpl os;
            QVariantMap hints;
            U2DbiRef dbiRef = context->getDataStorage()->getDbiRef();
            hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(dbiRef);
            hints[DocumentRemovalMode_Synchronous] = QString();
            Document* doc = df->createNewLoadedDocument(iof, filepath, os, hints);
            CHECK_OP(os, new FailTask(os.getError()));
            doc->setDocumentOwnsDbiResources(false);

            QSet<QString> usedNames;
            for (AnnotationTableObject* annTable : qAsConst(annTables)) {
                updateAnnotationsName(annTable, usedNames);
                annTable->setModified(false);
                doc->addObject(annTable);  // savedoc task will delete doc -> doc will delete att
            }
            task = new SaveDocumentTask(doc, fl, excludeFileNames);
        }
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_saveDocTaskFinished()));
        task->setProperty("output_url", filepath);
        taskList << task;
    }
    return createWriteMultitask(taskList);
}

void WriteAnnotationsWorker::cleanup() {
}

void WriteAnnotationsWorker::sl_saveDocTaskFinished() {
    auto task = dynamic_cast<Task*>(sender());
    CHECK(task != nullptr, );
    CHECK(task->isFinished(), );
    CHECK(!task->isCanceled() && !task->hasError(), );

    QString filePath = task->property("output_url").toString();
    CHECK(!filePath.isEmpty(), );
    monitor()->addOutputFile(filePath, getActorId());
}

/*******************************
 * WriteAnnotationsWorkerFactory
 *******************************/
void WriteAnnotationsWorkerFactory::init() {
    // ports description
    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_LIST_TYPE();
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        DataTypePtr inSet(new MapDataType(WRITE_ANNOTATIONS_IN_TYPE_ID, inM));
        Descriptor inPortDesc(BasePorts::IN_ANNOTATIONS_PORT_ID(), WriteAnnotationsWorker::tr("Input annotations"), WriteAnnotationsWorker::tr("Input annotations which will be written to output file"));
        portDescs << new PortDescriptor(inPortDesc, inSet, true);
    }
    DocumentFormatConstraints constr;
    constr.supportedObjectTypes.insert(GObjectTypes::ANNOTATION_TABLE);
    constr.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    constr.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    constr.formatsToExclude.insert(BaseDocumentFormats::VECTOR_NTI_SEQUENCE);
    QMap<DocumentFormatId, QString> supportedFormats;
    foreach (const DocumentFormatId& id, AppContext::getDocumentFormatRegistry()->selectFormats(constr)) {
        supportedFormats.insert(id, AppContext::getDocumentFormatRegistry()->getFormatById(id)->getFormatName());
    }
    supportedFormats.insert(CSV_FORMAT_ID, CSV_FORMAT_NAME);
    DocumentFormatId format = supportedFormats.contains(BaseDocumentFormats::PLAIN_GENBANK) ? BaseDocumentFormats::PLAIN_GENBANK : supportedFormats.first();
    // attributes description
    QList<Attribute*> attrs;
    {
        attrs << new Attribute(BaseAttributes::DATA_STORAGE_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, BaseAttributes::LOCAL_FS_DATA_STORAGE());
        auto dbAttr = new Attribute(BaseAttributes::DATABASE_ATTRIBUTE(), BaseTypes::STRING_TYPE(), true);
        dbAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::SHARED_DB_DATA_STORAGE()));
        attrs << dbAttr;
        auto dbPathAttr = new Attribute(BaseAttributes::DB_PATH(), BaseTypes::STRING_TYPE(), true, U2ObjectDbi::ROOT_FOLDER);
        dbPathAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::SHARED_DB_DATA_STORAGE()));
        attrs << dbPathAttr;

        auto docFormatAttr = new Attribute(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, format);
        docFormatAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        attrs << docFormatAttr;
        auto urlAttr = new Attribute(BaseAttributes::URL_OUT_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false);
        urlAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        attrs << urlAttr;
        auto suffixAttr = new Attribute(BaseAttributes::URL_SUFFIX(), BaseTypes::STRING_TYPE(), false);
        suffixAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        attrs << suffixAttr;
        auto fileModeAttr = new Attribute(BaseAttributes::FILE_MODE_ATTRIBUTE(), BaseTypes::NUM_TYPE(), false, SaveDoc_Roll);
        fileModeAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        attrs << fileModeAttr;

        // Merge for Local storage
        Descriptor mergeDesc(MERGE_TABLES_LOCAL, WriteAnnotationsWorker::tr("Merge annotation tables"), WriteAnnotationsWorker::tr("If <i>true</i> all annotation tables from dataset will be merged into one. "
                                                                                                                                   "The value of <i>Annotation table name</i> parameter will be used as the name of result annotation table."));
        auto mergeAttr = new Attribute(mergeDesc, BaseTypes::BOOL_TYPE(), false, false);
        mergeAttr->addRelation(new VisibilityRelation(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(),
                                                      QVariantList()
                                                          << CSV_FORMAT_ID
                                                          << BaseDocumentFormats::PLAIN_EMBL
                                                          << BaseDocumentFormats::PLAIN_GENBANK
                                                          << BaseDocumentFormats::GFF
                                                          << BaseDocumentFormats::PLAIN_SWISS_PROT));
        attrs << mergeAttr;

        Descriptor annotationsNameDesc(ANN_TABLE_NAME_4_LOCAL_ST, WriteAnnotationsWorker::tr("Annotation table name"), WriteAnnotationsWorker::tr("The name for the result annotation table that contains merged annotation data from file or dataset."));
        auto nameAttr = new Attribute(annotationsNameDesc, BaseTypes::STRING_TYPE(), false, ANNOTATIONS_NAME_DEF_VAL);
        nameAttr->addRelation(new VisibilityRelation(MERGE_TABLES_LOCAL, true));
        nameAttr->addRelation(new VisibilityRelation(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(),
                                                     QVariantList()
                                                         << CSV_FORMAT_ID
                                                         << BaseDocumentFormats::PLAIN_EMBL
                                                         << BaseDocumentFormats::PLAIN_GENBANK
                                                         << BaseDocumentFormats::GFF
                                                         << BaseDocumentFormats::PLAIN_SWISS_PROT));
        attrs << nameAttr;

        // Merge for shared DB
        Descriptor merge2Desc(MERGE_TABLES_SHARED, WriteAnnotationsWorker::tr("Merge annotation tables"), WriteAnnotationsWorker::tr("If <i>true</i> all annotation tables from dataset will be merged into one annotation object. "
                                                                                                                                     "The value of <i>Annotation object name</i> parameter will be used as the name of result annotation object."));
        auto merge2Attr = new Attribute(merge2Desc, BaseTypes::BOOL_TYPE(), false, false);
        merge2Attr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::SHARED_DB_DATA_STORAGE()));
        attrs << merge2Attr;

        Descriptor annObjNameDesc(ANN_TABLE_NAME_4_SHARED_ST, WriteAnnotationsWorker::tr("Annotation object name"), WriteAnnotationsWorker::tr("Name of the saved annotation object."));
        auto objNameAttr = new Attribute(annObjNameDesc, BaseTypes::STRING_TYPE(), false, ANNOTATIONS_NAME_DEF_VAL);
        objNameAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::SHARED_DB_DATA_STORAGE()));
        objNameAttr->addRelation(new VisibilityRelation(MERGE_TABLES_SHARED, true));
        attrs << objNameAttr;

        // Attributes for CSV format START
        Descriptor separatorDesc(SEPARATOR, WriteAnnotationsWorker::tr("CSV separator"), WriteAnnotationsWorker::tr("String which separates values in CSV files."));
        auto csvSeparatorAttr = new Attribute(separatorDesc, BaseTypes::STRING_TYPE(), false, SEPARATOR_DEFAULT_VALUE);
        csvSeparatorAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        csvSeparatorAttr->addRelation(new VisibilityRelation(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), CSV_FORMAT_ID));
        attrs << csvSeparatorAttr;

        Descriptor writeNamesDesc(WRITE_NAMES, WriteAnnotationsWorker::tr("Write sequence names"), WriteAnnotationsWorker::tr("Add names of sequences into CSV file."));
        auto seqNamesAttr = new Attribute(writeNamesDesc, BaseTypes::BOOL_TYPE(), false, false);
        seqNamesAttr->addRelation(new VisibilityRelation(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId(), BaseAttributes::LOCAL_FS_DATA_STORAGE()));
        seqNamesAttr->addRelation(new VisibilityRelation(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), CSV_FORMAT_ID));
        attrs << seqNamesAttr;
        // Attributes for CSV format END

        docFormatAttr->addRelation(new FileExtensionRelation(urlAttr->getId()));
    }

    Descriptor protoDesc(WriteAnnotationsWorkerFactory::ACTOR_ID,
                         WriteAnnotationsWorker::tr("Write Annotations"),
                         WriteAnnotationsWorker::tr("The element gets message(s) with annotations data and saves the data"
                                                    " to the specified file(s) in one of the appropriate formats"
                                                    " (GenBank, GTF, etc.)."));
    ActorPrototype* proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);

    // proto delegates
    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m;
        foreach (const QString& key, supportedFormats.keys()) {
            m[supportedFormats.value(key)] = key;
        }
        auto formatDelegate = new ComboBoxDelegate(m);
        formatDelegate->setItemTextFormatter(QSharedPointer<StringFormatter>(new DocumentNameByIdFormatter()));
        formatDelegate->setSortFlag(true);
        delegates[BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId()] = formatDelegate;
        delegates[BaseAttributes::URL_OUT_ATTRIBUTE().getId()] =
            new URLDelegate(FileFilters::createFileFilterByDocumentFormatId(format), "", false, false, true, nullptr, format);
        delegates[BaseAttributes::FILE_MODE_ATTRIBUTE().getId()] = new FileModeDelegate(attrs.size() > 2);
        delegates[BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId()] = new ComboBoxDelegate(BaseAttributes::DATA_STORAGE_ATTRIBUTE_VALUES_MAP());
        delegates[MERGE_TABLES_LOCAL] = new ComboBoxWithBoolsDelegate;
        delegates[MERGE_TABLES_SHARED] = new ComboBoxWithBoolsDelegate;
    }
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new WriteAnnotationsPrompter());
    proto->setPortValidator(BasePorts::IN_ANNOTATIONS_PORT_ID(),
                            new ScreenedSlotValidator(QStringList() << BaseSlots::URL_SLOT().getId() << BaseSlots::DNA_SEQUENCE_SLOT().getId()));
    proto->setValidator(new WriteAnnotationsValidator());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATASINK(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new WriteAnnotationsWorkerFactory());
}

Worker* WriteAnnotationsWorkerFactory::createWorker(Actor* a) {
    return new WriteAnnotationsWorker(a);
}

/***************************
 * WriteAnnotationsPrompter
 ***************************/
QString WriteAnnotationsPrompter::composeRichDoc() {
    const QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    auto input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_ANNOTATIONS_PORT_ID()));
    QString annName = getProducers(BasePorts::IN_ANNOTATIONS_PORT_ID(), BaseSlots::ANNOTATION_TABLE_SLOT().getId());
    annName = annName.isEmpty() ? unsetStr : annName;

    Attribute* dataStorageAttr = target->getParameter(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId());
    SAFE_POINT(dataStorageAttr != nullptr, "Invalid attribute", QString());
    const QVariant dataStorage = dataStorageAttr->getAttributePureValue();

    QString url;
    QString dbName;
    const bool storeToFs = dataStorage == BaseAttributes::LOCAL_FS_DATA_STORAGE();
    if (storeToFs) {
        static const QString generatedStr = "<font color='blue'>" + tr("default file") + "</font>";
        url = getScreenedURL(input, BaseAttributes::URL_OUT_ATTRIBUTE().getId(), BaseSlots::URL_SLOT().getId(), generatedStr);
        url = getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url);
    } else if (dataStorage == BaseAttributes::SHARED_DB_DATA_STORAGE()) {
        Attribute* dbPathAttr = target->getParameter(BaseAttributes::DB_PATH().getId());
        SAFE_POINT(dbPathAttr != nullptr, "Invalid attribute", QString());
        url = dbPathAttr->getAttributePureValue().toString();
        url = getHyperlink(BaseAttributes::DB_PATH().getId(), url);

        Attribute* dbAttr = target->getParameter(BaseAttributes::DATABASE_ATTRIBUTE().getId());
        SAFE_POINT(dbAttr != nullptr, "Invalid attribute", QString());
        const QString dbUrl = dbAttr->getAttributePureValue().toString();
        dbName = SharedDbUrlUtils::getDbShortNameFromEntityUrl(dbUrl);
        dbName = dbName.isEmpty() ? unsetStr : getHyperlink(BaseAttributes::DATABASE_ATTRIBUTE().getId(), dbName);
    } else {
        FAIL("Unexpected attribute value", QString());
    }

    const QString formatId = getParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId()).value<QString>();
    QString formatName = formatId == CSV_FORMAT_ID ? CSV_FORMAT_NAME : AppContext::getDocumentFormatRegistry()->getFormatById(formatId)->getFormatName();

    return tr("Save all annotations from <u>%1</u> to %2")
               .arg(annName)
               .arg(getHyperlink(BaseAttributes::URL_OUT_ATTRIBUTE().getId(), url)) +
           (storeToFs ? tr(" in %1 format.").arg(getHyperlink(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), formatName)) : tr(" in the ") + QString("<u>%1</u>").arg(dbName) + tr(" database."));
}

}  // namespace LocalWorkflow
}  // namespace U2
