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

#include <U2Core/FormatSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObject.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/GHints.h>
#include <U2Core/AppResources.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ZlibAdapter.h>

#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2DbiUtils.h>

#include <QApplication>
#include <QFileInfo>

#include "LoadDocumentTask.h"

#define GObjectHint_NamesList  "gobject-hint-names-list"

namespace U2 {

/* TRANSLATOR U2::LoadUnloadedDocumentTask */

//////////////////////////////////////////////////////////////////////////
// LoadUnloadedDocumentTask

//TODO: support subtask sharing!
//TODO: avoid multiple load tasks when opening view for unloaded doc!

LoadUnloadedDocumentTask::LoadUnloadedDocumentTask(Document* d, const LoadDocumentTaskConfig& _config)
    : DocumentProviderTask("", TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText | TaskFlag_CollectChildrenWarnings),
    loadTask(NULL),
    unloadedDoc(d),
    config(_config) {
    assert(config.checkObjRef.objType != GObjectTypes::UNLOADED);
    assert(unloadedDoc != NULL);

    setVerboseLogMode(true);
    setTaskName(tr("Load '%1'").arg(d->getName()));
    setUseDescriptionFromSubtask(true);
    docOwner = false;
    resultDocument = d;
}

void LoadUnloadedDocumentTask::prepare() {
    if (unloadedDoc == NULL) {
        stateInfo.setError(tr("Document not found"));
        return;
    }
    DocumentFormatId format = unloadedDoc->getDocumentFormatId();
    QString formatName = AppContext::getDocumentFormatRegistry()->getFormatById(format)->getFormatName();
    IOAdapterFactory* iof = unloadedDoc->getIOAdapterFactory();
    const GUrl& url = unloadedDoc->getURL();
    coreLog.details(tr("Starting load document from %1, document format %2").arg(url.getURLString()).arg(formatName));
    QVariantMap hints = unloadedDoc->getGHintsMap();
    QStringList namesList;
    foreach(GObject* obj, unloadedDoc->getObjects()) {
        namesList << obj->getGObjectName();
    }
    hints[GObjectHint_NamesList] = namesList;
    loadTask = new LoadDocumentTask(format, url, iof, hints, config);
    addSubTask(loadTask);

    resName = getResourceName(unloadedDoc);
    AppContext::getResourceTracker()->registerResourceUser(resName, this);
}

void LoadUnloadedDocumentTask::clearResourceUse() {
    if (!resName.isEmpty()) {
        AppContext::getResourceTracker()->unregisterResourceUser(resName, this);
        resName.clear();
    }
}

Task::ReportResult LoadUnloadedDocumentTask::report() {
    Task::ReportResult res = ReportResult_Finished;
    Project* p = AppContext::getProject();

    if (unloadedDoc == NULL) {
        stateInfo.setError(tr("Document was removed"));
    } else {
        propagateSubtaskError();
    }

    if (hasError()) {
        if (!resName.isEmpty()) {
            clearResourceUse();
            resName.clear();
        }
    } else if (isCanceled() || (loadTask != NULL && loadTask->isCanceled())) {
        //do nothing
    } else if (unloadedDoc->isLoaded()) {
        //do nothing
    } else if (p && p->isStateLocked()) {
        res = ReportResult_CallMeAgain; //wait until project is unlocked
    } else {
        assert(unloadedDoc->isStateLocked()); // all unloaded docs are always state locked

        //todo: move to utility method?
        const QList<StateLock*>& locks = unloadedDoc->getStateLocks();
        bool readyToLoad = true;
        foreach(StateLock* lock, locks) {
            if (lock != unloadedDoc->getDocumentModLock(DocumentModLock_IO)
                && lock != unloadedDoc->getDocumentModLock(DocumentModLock_USER)
                && lock != unloadedDoc->getDocumentModLock(DocumentModLock_FORMAT_AS_CLASS)
                && lock != unloadedDoc->getDocumentModLock(DocumentModLock_FORMAT_AS_INSTANCE)
                && lock != unloadedDoc->getDocumentModLock(DocumentModLock_UNLOADED_STATE)) {
                readyToLoad = false;
            }
        }
        if (!readyToLoad) {
            stateInfo.setError(tr("Document is locked")); //todo: wait instead?
        } else {
            Document* sourceDoc = loadTask->getDocument();
            unloadedDoc->loadFrom(sourceDoc); // get all data from source doc;
            assert(!unloadedDoc->isTreeItemModified());
            assert(unloadedDoc->isLoaded());
            if (sourceDoc->getGHintsMap().value(DocumentReadingMode_LoadAsModified, false).toBool()) {
                unloadedDoc->setModified(true);
            }
        }
    }
    if (res == ReportResult_Finished) {
        clearResourceUse();
    }
    return res;
}


QString LoadUnloadedDocumentTask::getResourceName(Document* d) {
    return QString(LoadUnloadedDocumentTask::tr("Project Document:") + ":" + d->getURLString());
}

LoadUnloadedDocumentTask* LoadUnloadedDocumentTask::findActiveLoadingTask(Document* d) {
    QString res = getResourceName(d);
    QList<Task*> tasks = AppContext::getResourceTracker()->getResourceUsers(res);
    foreach(Task* t, tasks) {
        LoadUnloadedDocumentTask* lut = qobject_cast<LoadUnloadedDocumentTask*>(t);
        if (lut != NULL) {
            return lut;
        }
    }
    return NULL;
}

bool LoadUnloadedDocumentTask::addLoadingSubtask(Task* t, const LoadDocumentTaskConfig& config) {
    GObject* o = GObjectUtils::selectObjectByReference(config.checkObjRef, UOF_LoadedAndUnloaded);
    if (o == NULL) {
        t->setError(tr("Annotation object not found"));
        return false;
    }
    if (o->isUnloaded()) {
        t->addSubTask(new LoadUnloadedDocumentTask(o->getDocument(), config));
        return true;
    }
    return false;
}

Document* LoadUnloadedDocumentTask::getDocument(bool) {
    if (unloadedDoc.isNull()) {
        return NULL;
    }
    return DocumentProviderTask::getDocument();
}

//////////////////////////////////////////////////////////////////////////
// Load Document


LoadDocumentTask::LoadDocumentTask(DocumentFormatId f, const GUrl& u,
    IOAdapterFactory* i, const QVariantMap& map, const LoadDocumentTaskConfig& _config)
    : DocumentProviderTask("", TaskFlag_None),
    format(NULL),
    url(u),
    iof(i),
    hints(map),
    config(_config) {
    setTaskName(tr("Read document: '%1'").arg(u.fileName()));
    documentDescription = u.getURLString();
    format = AppContext::getDocumentFormatRegistry()->getFormatById(f);
    init();
}

LoadDocumentTask::LoadDocumentTask(DocumentFormat* f, const GUrl& u,
    IOAdapterFactory* i, const QVariantMap& map, const LoadDocumentTaskConfig& _config)
    : DocumentProviderTask("", TaskFlag_None),
    format(NULL),
    url(u),
    iof(i),
    hints(map),
    config(_config) {
    setTaskName(tr("Read document: '%1'").arg(u.fileName()));
    documentDescription = u.getURLString();
    format = f;
    init();
}

static bool isLoadFromMultipleFiles(QVariantMap& hints) {
    if (hints.value(ProjectLoaderHint_MultipleFilesMode_Flag, false).toBool() == true) { // if that document was/is collected from different files
        if (!QFile::exists(hints[ProjectLoaderHint_MultipleFilesMode_Flag].toString())) {// if not exist - load as collected
            return true;
        }
        hints.remove(ProjectLoaderHint_MultipleFilesMode_Flag); // if exist - remove hints indicated that document is collected . Now document is genbank or clustalw
        hints[DocumentReadingMode_SequenceMergeGapSize] = -1;
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = false;
    }

    return false;
}

void LoadDocumentTask::init() {
    tpm = Progress_Manual;
    CHECK_EXT(format != NULL, setError(tr("Document format is NULL!")), );
    CHECK_EXT(iof != NULL, setError(tr("IO adapter factory is NULL!")), );
    documentDescription = url.getURLString();
    if (format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE)) {
        CaseAnnotationsMode mode = AppContext::getAppSettings()->getFormatAppsSettings()->getCaseAnnotationsMode();
        hints[GObjectHint_CaseAnns] = (int)mode;
    }
}

LoadDocumentTask *LoadDocumentTask::getDefaultLoadDocTask(const GUrl &url, const QVariantMap &hints) {
    U2OpStatusImpl os;
    return getDefaultLoadDocTask(os, url, hints);
}

LoadDocumentTask *LoadDocumentTask::getDefaultLoadDocTask(U2OpStatus &os, const GUrl &url, const QVariantMap &hints) {
    CHECK_EXT(!url.isEmpty(), os.setError(tr("The fileURL  to load is empty")), NULL);

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    CHECK_EXT(NULL != iof, os.setError(tr("Cannot get an IO file adapter factory for the file URL: %1").arg(url.getURLString())), NULL);

    QList<FormatDetectionResult> dfs = DocumentUtils::detectFormat(url);
    CHECK_EXT(!dfs.isEmpty(), os.setError(tr("Cannot detect the file format: %1").arg(url.getURLString())), NULL);

    DocumentFormat *df = dfs.first().format;
    SAFE_POINT_EXT(NULL != df, os.setError(tr("Document format is NULL (format ID: '%1', file URL: '%2')").arg(df->getFormatId()).arg(url.getURLString())), NULL);
    return new LoadDocumentTask(df->getFormatId(), url, iof, hints);
}

DocumentProviderTask * LoadDocumentTask::getCommonLoadDocTask(const GUrl & url) {
    if (url.isEmpty()) {
        return NULL;
    }

    IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (iof == NULL) {
        return NULL;
    }

    FormatDetectionConfig conf;
    conf.useImporters = true;
    QList<FormatDetectionResult> dfs = DocumentUtils::detectFormat(url, conf);
    if (dfs.isEmpty()) {
        return NULL;
    }

    DocumentFormat * df = dfs.first().format;
    DocumentImporter * di = dfs.first().importer;
    DocumentProviderTask* task = NULL;

    if (df) {
        task = new LoadDocumentTask(df->getFormatId(), url, iof);
    } else if (di) {
        task = di->createImportTask(dfs.first(), true, QVariantMap());
    }

    return task;
}

static bool isLoadToMem(const DocumentFormatId& id) {
    // files that use dbi not loaded to memory
    if (id == BaseDocumentFormats::FASTA || id == BaseDocumentFormats::PLAIN_GENBANK ||
        id == BaseDocumentFormats::RAW_DNA_SEQUENCE || id == BaseDocumentFormats::FASTQ
        || id == BaseDocumentFormats::GFF || id == BaseDocumentFormats::PDW) {
        return false;
    }
    return true;
}

void LoadDocumentTask::prepare() {
    if (hasError() || isCanceled()) {
        return;
    }

    int memUseMB = calculateMemory();
    if (memUseMB > 0) {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB, false));
    }
}

static QList<Document*> loadMulti(const QVariantMap& fs, U2OpStatus& os) {
    QList<Document*> docs;

    os.setProgress(0);
    int curentDocIdx = 0;

    QStringList urls = fs[ProjectLoaderHint_MultipleFilesMode_URLsDocumentConsistOf].toStringList();

    foreach(const QString& url, urls) {
        FormatDetectionConfig conf;
        conf.useImporters = true;
        conf.bestMatchesOnly = false;
        GUrl gurl(url);
        QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(gurl, conf);
        CHECK_OPERATION(!formats.isEmpty(), continue);

        int len = 100 / urls.size();
        U2OpStatusChildImpl localOs(&os, U2OpStatusMapping(curentDocIdx * len,
            (curentDocIdx == urls.size() - 1) ? (100 - curentDocIdx * len) : len));

        QVariantMap fsLocal;
        fsLocal.unite(fs);
        fsLocal.remove(DocumentReadingMode_SequenceMergeGapSize);
        SAFE_POINT_EXT(AppContext::getDocumentFormatRegistry() != NULL, os.setError("DocumentFormatRegistry is NULL"), docs);
        DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(formats[0].format->getFormatId());
        SAFE_POINT_EXT(AppContext::getIOAdapterRegistry() != NULL, os.setError("IOAdapterRegistry is NULL"), docs);
        IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(gurl));
        SAFE_POINT_EXT(factory != NULL, os.setError("IOAdapterFactory is NULL"), docs);
        Document* doc = df->loadDocument(factory, gurl, fsLocal, localOs);
        CHECK_OP(os, docs);
        SAFE_POINT_EXT(doc != NULL, os.setError("Document is NULL"), docs);
        docs << df->loadDocument(factory, gurl, fsLocal, localOs);

        CHECK_OP(os, docs);
        curentDocIdx++;
    }
    return docs;
}

void loadHintsNewDocument(bool saveDoc, IOAdapterFactory* iof, Document* doc, U2OpStatus& os) {
    if (saveDoc) {
        QScopedPointer<IOAdapter> io(iof->createIOAdapter());
        QString url = doc->getURLString();
        if (!io->open(url, IOAdapterMode_Write)) {
            os.setError(L10N::errorOpeningFileWrite(url));
        } else {
            //TODO remove after genbank can storing without getWholeSequence
            try {
                doc->getDocumentFormat()->storeDocument(doc, io.data(), os);
            } catch (const std::bad_alloc &) {
                os.setError(QString("Not enough memory to storing %1 file").arg(doc->getURLString()));
            }
        }
    }
}

static Document* loadFromMultipleFiles(IOAdapterFactory* iof, QVariantMap& fs, U2OpStatus& os) {
    QList<Document*> docs = loadMulti(fs, os);
    if (os.isCoR()) {
        foreach(Document* doc, docs) {
            delete doc;
        }
        return NULL;
    }

    Document* doc = NULL;
    QString newStringUrl = fs[ProjectLoaderHint_MultipleFilesMode_URLDocument].toString();
    GUrl newUrl(newStringUrl, GUrl_File);
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(fs[ProjectLoaderHint_MultipleFilesMode_RealDocumentFormat].toString());
    QList<GObject*> newObjects;

    U2DbiRef ref;
    if (fs.value(DocumentReadingMode_SequenceMergeGapSize, -1) != -1) {
        ref = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
        QList<GObject*> sequences = U1SequenceUtils::mergeSequences(docs, ref, newStringUrl, fs, os);
        CHECK_OP(os, NULL);
        newObjects << sequences;
    } else if (fs.value(DocumentReadingMode_SequenceAsAlignmentHint).toBool()) {
        MultipleSequenceAlignmentObject* msaObject = MSAUtils::seqDocs2msaObj(docs, fs, os);
        CHECK_OP(os, NULL);
        SAFE_POINT_EXT(NULL != msaObject, os.setError("The alignment object is NULL!"), NULL);
        newObjects << msaObject;
        ref = U2DbiRef();
    } else {
        os.setError("Multiple files reading mode: unsupported flags");
    }
    CHECK_OP(os, NULL);

    doc = new Document(df, iof, newUrl, ref, newObjects, fs);

    bool saveDoc = fs.value(ProjectLoaderHint_MultipleFilesMode_SaveDocumentFlag, false).toBool();
    loadHintsNewDocument(saveDoc, iof, doc, os);
    if (!saveDoc) {
        fs.insert(ProjectLoaderHint_DontCheckForExistence, true);
    }

    return doc;
}

void LoadDocumentTask::run() {
    CHECK_OP(stateInfo, );
    if (config.createDoc && iof->isResourceAvailable(url) == TriState_No) {
        CHECK_EXT(iof->isIOModeSupported(IOAdapterMode_Write), setError(tr("Document not found %1").arg(url.getURLString())), );
        resultDocument = format->createNewLoadedDocument(iof, url, stateInfo, hints);
        return;
    }

    QStringList renameList = hints.value(GObjectHint_NamesList).toStringList();
    // removing this value from hints -> name list changes are not tracked in runtime
    // and used for LoadUnloadedDocument & LoadDocument privately
    hints.remove(GObjectHint_NamesList);

    try {
        if (isLoadFromMultipleFiles(hints)) {
            resultDocument = loadFromMultipleFiles(iof, hints, stateInfo);
        } else {
            resultDocument = format->loadDocument(iof, url, hints, stateInfo);
        }
    } catch (std::bad_alloc) {
        resultDocument = NULL;
        setError(tr("Not enough memory to load document %1").arg(url.getURLString()));
    }

    if (resultDocument != NULL) {
        if (!renameList.isEmpty()) {
            renameObjects(resultDocument, renameList);
        }
        Document* convertedDoc = DocumentUtils::createCopyRestructuredWithHints(resultDocument, stateInfo, true);
        if (convertedDoc != NULL) {
            delete resultDocument;
            resultDocument = convertedDoc;
        }
        if (hints.contains(DocumentReadingMode_MaxObjectsInDoc)) {
            int maxObjects = hints.value(DocumentReadingMode_MaxObjectsInDoc).toInt();
            int docObjects = resultDocument->getObjects().size();
            if (docObjects > maxObjects) {
                setError(tr("Maximum number of objects per document limit reached for %1. Try different options for opening the document!").arg(resultDocument->getURLString()));
                delete resultDocument;
                resultDocument = NULL;
            }
        }
    }
    if (config.checkObjRef.isValid() && !hasError()) {
        processObjRef();
    }
    if ((NULL != resultDocument) && hints.value(ProjectLoaderHint_DontCheckForExistence, false).toBool()) {
        resultDocument->getGHints()->set(ProjectLoaderHint_DontCheckForExistence, true);
    }
    assert(stateInfo.isCoR() || resultDocument != NULL);
    assert(resultDocument == NULL || resultDocument->isLoaded());
}

Task::ReportResult LoadDocumentTask::report() {
    if (stateInfo.hasError() || isCanceled()) {
        return ReportResult_Finished;
    }
    resultDocument->setLastUpdateTime();
    return ReportResult_Finished;
}

void LoadDocumentTask::processObjRef() {
    assert(config.checkObjRef.isValid());
    assert(resultDocument != NULL);

    if (GObjectUtils::selectObjectByReference(config.checkObjRef, resultDocument->getObjects(), UOF_LoadedOnly) == NULL) {
        if (config.objFactory == NULL) {
            stateInfo.setError(tr("Object not found: %1").arg(config.checkObjRef.objName));
        } else {
            assert(!resultDocument->isStateLocked());
            Document::Constraints c;
            c.objectTypeToAdd.append(config.checkObjRef.objType);
            bool ok = resultDocument->checkConstraints(c);
            if (!ok) {
                stateInfo.setError(tr("Can't add object. Document format constraints check failed: %1").arg(resultDocument->getName()));
            } else {
                GObject* obj = config.objFactory->create(config.checkObjRef);
                assert(obj != NULL);
                resultDocument->addObject(obj);
            }
        }
    }
}

int LoadDocumentTask::calculateMemory() const {
    int memUseMB = 0;

    if (!format->getFlags().testFlag(DocumentFormatFlag_NoFullMemoryLoad) && isLoadToMem(format->getFormatId())) { // document is fully loaded to memory
        QFileInfo file(url.getURLString());
        memUseMB = file.size() / (1000 * 1000);

        double DEFAULT_COMPRESS_RATIO = 2.5;
        if (iof->getAdapterId() == BaseIOAdapters::GZIPPED_LOCAL_FILE) {
            qint64 fileSizeInBytes = ZlibAdapter::getUncompressedFileSizeInBytes(url);
            if (fileSizeInBytes < 0) {
                memUseMB *= DEFAULT_COMPRESS_RATIO; //Need to calculate compress level
            } else {
                memUseMB = fileSizeInBytes / (1000 * 1000);
            }
        } else if (iof->getAdapterId() == BaseIOAdapters::GZIPPED_HTTP_FILE) {
            memUseMB *= DEFAULT_COMPRESS_RATIO; //Need to calculate compress level
        }
        coreLog.trace(QString("load document:Memory resource %1").arg(memUseMB));
    }

    return memUseMB;
}

void LoadDocumentTask::renameObjects(Document* doc, const QStringList& names) {
    if (doc->getObjects().size() != names.size()) {
        coreLog.trace(QString("Objects renaming failed! Objects in doc: %1, names: %2").arg(doc->getObjects().size()).arg(names.size()));
        return;
    }

    //drop names first
    QSet<QString> usedNames;
    QSet<GObject*> notRenamedObjects;
    foreach(GObject* obj, doc->getObjects()) {
        notRenamedObjects.insert(obj);
        usedNames.insert(obj->getGObjectName());
    }
    const QList<GObject*>& objects = doc->getObjects();
    int nObjects = objects.size();
    int maxIters = nObjects;
    int currentIter = 0; //to avoid endless loop in case of duplicate names
    while (!notRenamedObjects.isEmpty() && currentIter < maxIters) {
        for (int i = 0; i < nObjects; i++) {
            GObject* obj = objects[i];
            if (!notRenamedObjects.contains(obj)) {
                continue;
            }
            QString newName = names[i];
            if (usedNames.contains(newName)) {
                continue;
            }
            QString oldName = obj->getGObjectName();
            obj->setGObjectName(newName);
            usedNames.remove(oldName);
            usedNames.insert(newName);
            notRenamedObjects.remove(obj);
        }
        currentIter++;
    }
}

QString LoadDocumentTask::getURLString() const {
    return url.getURLString();
}

GObject * LDTObjectFactory::create(const GObjectReference &ref) {
    // TODO: handle other core types
    SAFE_POINT(ref.objType == GObjectTypes::ANNOTATION_TABLE, "Invalid object type!", NULL);
    U2OpStatusImpl os;
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    SAFE_POINT_OP(os, NULL);
    return new AnnotationTableObject(ref.objName, dbiRef);
}

}//namespace
