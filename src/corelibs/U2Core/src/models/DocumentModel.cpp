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

#include <QCoreApplication>
#include <QFileInfo>
#include <QScopedPointer>
#include <QThread>

#include <QScriptEngine>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DeleteObjectsTask.h>
#include <U2Core/GHints.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Task.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UnloadedObject.h>

#include "DocumentModel.h"

namespace U2 {

const QString DocumentFormat::DBI_REF_HINT("dbi_alias");
const QString DocumentFormat::DBI_FOLDER_HINT("dbi_folder");
const QString DocumentFormat::DEEP_COPY_OBJECT("deep_copy_object");
const QString DocumentFormat::STRONG_FORMAT_ACCORDANCE("strong_format_accordance");
const QString DocumentMimeData::MIME_TYPE("application/x-ugene-document-mime");

const int DocumentFormat::READ_BUFF_SIZE = 4194304; //4Mb optimal buffer size for reading from network drives

DocumentFormat::DocumentFormat(QObject* p, const DocumentFormatId& _id, DocumentFormatFlags _flags, const QStringList& fileExts) : QObject(p),
id(_id),
formatFlags(_flags),
fileExtensions(fileExts)
{}

Document* DocumentFormat::createNewLoadedDocument(IOAdapterFactory* iof, const GUrl& url,
    U2OpStatus& os, const QVariantMap& hints) {
    U2DbiRef tmpDbiRef = fetchDbiRef(hints, os);
    CHECK_OP(os, NULL);

    Document* doc = new Document(this, iof, url, tmpDbiRef, QList<UnloadedObjectInfo>(),
        hints, QString());
    doc->setLoaded(true);
    doc->setDocumentOwnsDbiResources(true);
    doc->setModificationTrack(!checkFlags(DocumentFormatFlag_DirectWriteOperations));
    return doc;
}

Document* DocumentFormat::createNewUnloadedDocument(IOAdapterFactory* iof, const GUrl& url,
    U2OpStatus& os, const QVariantMap& hints,
    const QList<UnloadedObjectInfo>& info,
    const QString& instanceModLockDesc) {
    Q_UNUSED(os);
    U2DbiRef dbiRef = (hints[DocumentFormat::DBI_REF_HINT]).value<U2DbiRef>();
    Document* doc = new Document(this, iof, url, dbiRef, info, hints, instanceModLockDesc);
    doc->setModificationTrack(!checkFlags(DocumentFormatFlag_DirectWriteOperations));
    return doc;
}

Document* DocumentFormat::loadDocument(IOAdapterFactory* iof, const GUrl& url, const QVariantMap& hints, U2OpStatus& os) {
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        os.setError(L10N::errorOpeningFileRead(url));
        return NULL;
    }

    Document* res = NULL;

    U2DbiRef dbiRef = fetchDbiRef(hints, os);
    CHECK_OP(os, NULL);

    if (dbiRef.isValid()) {
        DbiConnection con(dbiRef, os);
        CHECK_OP(os, NULL);
        Q_UNUSED(con);

        res = loadDocument(io.data(), dbiRef, hints, os);
        CHECK_OP(os, NULL);
    } else {
        res = loadDocument(io.data(), U2DbiRef(), hints, os);
    }
    return res;
}

U2DbiRef DocumentFormat::fetchDbiRef(const QVariantMap &hints, U2OpStatus &os) const {
    if (hints.contains(DBI_REF_HINT)) {
        return hints.value(DBI_REF_HINT).value<U2DbiRef>();
    } else {
        return AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    }
}

DNASequence* DocumentFormat::loadSequence(IOAdapter*, U2OpStatus& os) {
    os.setError("This document format does not support streaming reading mode");
    return NULL;
}

void DocumentFormat::storeDocument(Document*, IOAdapter*, U2OpStatus& os) {
    assert(0);
    os.setError(tr("Writing is not supported for this format (%1). Feel free to send a feature request though.").arg(formatName));
}

void DocumentFormat::storeDocument(Document* doc, U2OpStatus& os, IOAdapterFactory* iof, const GUrl& newDocURL) {
    SAFE_POINT_EXT(formatFlags.testFlag(DocumentFormatFlag_SupportWriting),
        os.setError(tr("Writing is not supported for this format (%1). Feel free to send a feature request though.").arg(formatName)), );

    assert(doc->getDocumentModLock(DocumentModLock_FORMAT_AS_INSTANCE) == NULL);
    if (iof == NULL) {
        iof = doc->getIOAdapterFactory();
    }

    //prepare URL
    GUrl url = newDocURL.isEmpty() ? doc->getURL() : newDocURL;
    if (url.isLocalFile()) {
        QString error;
        QString res = GUrlUtils::prepareFileLocation(url.getURLString(), os);
        CHECK_OP(os, );
        Q_UNUSED(res);
        assert(res == url.getURLString()); //ensure that GUrls are always canonical
    }

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        os.setError(L10N::errorOpeningFileWrite(url));
        return;
    }

    storeDocument(doc, io.data(), os);
}

bool DocumentFormat::checkConstraints(const DocumentFormatConstraints& c) const {
    assert(!supportedObjectTypes.isEmpty()); //extra check for DF state validation

    if (!checkFlags(c.flagsToSupport)) {
        return false; //requested to support writing or streaming but doesn't
    }

    if ((int(c.flagsToExclude) & int(formatFlags)) != 0) {
        return false; // filtered by exclude flags
    }

    if (c.formatsToExclude.contains(id)) {
        return false; // format is explicetely excluded
    }

    if (c.checkRawData && checkRawData(c.rawData).score < c.minDataCheckResult) {
        return false; //raw data is not matched
    }

    bool areTypesSatisfied = !c.allowPartialTypeMapping;
    foreach(const GObjectType &objType, c.supportedObjectTypes) {
        if (c.allowPartialTypeMapping && supportedObjectTypes.contains(objType)) { // at least one type is supported
            areTypesSatisfied = true;
            break;
        } else if (!c.allowPartialTypeMapping && !supportedObjectTypes.contains(objType)) { // the object type is not in the supported list
            areTypesSatisfied = false;
            break;
        }
    }

    return areTypesSatisfied;
}

void DocumentFormat::storeEntry(IOAdapter *, const QMap< GObjectType, QList<GObject*> > &, U2OpStatus &os) {
    os.setError("This document format does not support streaming mode");
}

QString DocumentFormat::getRadioButtonText() const {
    return QString();
}

bool DocumentFormat::isObjectOpSupported(const Document* d, DocObjectOp op, GObjectType t) const {
    assert(d->getDocumentFormat() == this);

    if (!supportedObjectTypes.contains(t)) {
        return false;
    }

    if (!checkFlags(DocumentFormatFlag_SupportWriting)) {
        return false;
    }

    if (op == DocObjectOp_Add) {
        int nObjects = d->getObjects().size();
        if (nObjects != 0 && checkFlags(DocumentFormatFlag_SingleObjectFormat)) {
            return false;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
///Document
const QString Document::UNLOAD_LOCK_NAME = "unload_document_lock";

Document::Document(DocumentFormat* _df, IOAdapterFactory* _io, const GUrl& _url,
    const U2DbiRef& _dbiRef,
    const QList<UnloadedObjectInfo>& unloadedObjects, const QVariantMap& hints,
    const QString& instanceModLockDesc)
    : StateLockableTreeItem(), df(_df), io(_io), url(_url), dbiRef(_dbiRef) {
    documentOwnsDbiResources = false;

    ctxState = new GHintsDefaultImpl(hints);
    name = url.fileName();

    qFill(modLocks, modLocks + DocumentModLock_NUM_LOCKS, (StateLock*)NULL);

    loadStateChangeMode = true;
    addUnloadedObjects(unloadedObjects);
    loadStateChangeMode = false;

    initModLocks(instanceModLockDesc, false);
    checkUnloadedState();
    //    assert(!isModified());
}

Document::Document(DocumentFormat* _df, IOAdapterFactory* _io, const GUrl& _url,
    const U2DbiRef& _dbiRef,
    const QList<GObject*>& _objects, const QVariantMap& hints,
    const QString& instanceModLockDesc)
    : StateLockableTreeItem(), df(_df), io(_io), url(_url), dbiRef(_dbiRef) {
    documentOwnsDbiResources = true;

    ctxState = new GHintsDefaultImpl(hints);
    name = url.fileName();

    loadStateChangeMode = true;
    qFill(modLocks, modLocks + DocumentModLock_NUM_LOCKS, (StateLock*)NULL);
    foreach(GObject* o, _objects) {
        _addObject(o);
    }
    loadStateChangeMode = false;

    initModLocks(instanceModLockDesc, true);

    checkLoadedState();
    assert(!isModified());
}

Document *Document::getSimpleCopy(DocumentFormat *df, IOAdapterFactory *io, const GUrl &url) const {
    Document *result = new Document(df, io, url, this->dbiRef, QList<GObject*>(), this->getGHintsMap());
    result->objects = this->objects;
    result->documentOwnsDbiResources = false;

    return result;
}

Document::~Document() {
    for (int i = 0;i < DocumentModLock_NUM_LOCKS; i++) {
        StateLock* sl = modLocks[i];
        if (sl != NULL) {
            unlockState(sl);
            delete sl;
        }
    }

    if (isDocumentOwnsDbiResources() && dbiRef.isValid()) {
        removeObjectsDataFromDbi(objects);
    }

    delete ctxState;
}

GObject * Document::getObjectById(const U2DataId &id) const {
    return id2Object.value(id, NULL);
}

void Document::setObjectsInUse(const QSet<U2DataId> &objs) {
    objectsInUse = objs;
}

void Document::addObject(GObject* obj) {
    SAFE_POINT(obj != NULL, "Object is NULL", );
    SAFE_POINT(obj->getDocument() == NULL, "Object already belongs to some document", );
    SAFE_POINT(df->isObjectOpSupported(this, DocumentFormat::DocObjectOp_Add, obj->getGObjectType()), "Document format doesn't support new objects adding", );
    SAFE_POINT(isLoaded(), "The destination document is not loaded", );
    SAFE_POINT(obj->getGObjectType() != GObjectTypes::UNLOADED, "Object is not loaded", );

    _addObject(obj);
}

void Document::_addObjectToHierarchy(GObject* obj) {
    SAFE_POINT(obj != NULL, "Object is NULL", );
    obj->setParentStateLockItem(this);
    obj->setGHints(new ModTrackHints(this, obj->getGHintsMap(), true));
    obj->setModified(false);
    objects.append(obj);
    id2Object.insert(obj->getEntityRef().entityId, obj);
}

void Document::_addObject(GObject* obj) {
    SAFE_POINT(obj != NULL, "Object is NULL", );
    _addObjectToHierarchy(obj);
    assert(objects.size() == getChildItems().size());
    emit si_objectAdded(obj);
}

bool Document::removeObject(GObject* obj, DocumentObjectRemovalMode removalMode) {
    SAFE_POINT(df->isObjectOpSupported(this, DocumentFormat::DocObjectOp_Remove, obj->getGObjectType()), "Unsupported format operation", false);

    switch (removalMode) {
    case DocumentObjectRemovalMode_Deallocate:
        return _removeObject(obj, true);
    case DocumentObjectRemovalMode_OnlyNotify:
        emit si_objectRemoved(obj);
        break;
    case DocumentObjectRemovalMode_Release:
        return _removeObject(obj, false);
    }

    return true;
}

bool Document::_removeObject(GObject* obj, bool deleteObjects) {
    SAFE_POINT(obj->getParentStateLockItem() == this, "Invalid parent document!", false);

    if (obj->entityRef.isValid() && objectsInUse.contains(obj->getEntityRef().entityId)) {
        return false;
    }

    obj->setModified(false);

    obj->setParentStateLockItem(NULL);
    objects.removeOne(obj);
    id2Object.remove(obj->getEntityRef().entityId);
    obj->setGHints(new GHintsDefaultImpl(obj->getGHintsMap()));

    SAFE_POINT(objects.size() == getChildItems().size(), "Invalid child object count!", false);

    emit si_objectRemoved(obj);

    if (deleteObjects) {
        removeObjectsDataFromDbi(QList<GObject *>() << obj);
        delete obj;
    }
    return true;
}

void Document::makeClean() {
    if (!isTreeItemModified()) {
        return;
    }
    setModified(false);
    foreach(GObject* obj, objects) {
        obj->setModified(false);
    }
}

void Document::setModificationTrack(bool track) {
    if (df != NULL && df->checkFlags(DocumentFormatFlag_DirectWriteOperations)) {
        StateLockableTreeItem::setModificationTrack(false);
    } else {
        StateLockableTreeItem::setModificationTrack(track);
    }
}

GObject * Document::findGObjectByNameInDb(const QString &name) const {
    U2OpStatusImpl os;
    DbiConnection con(dbiRef, os);
    SAFE_POINT_OP(os, NULL);

    U2ObjectDbi *oDbi = con.dbi->getObjectDbi();
    SAFE_POINT(NULL != oDbi, "Invalid database connection", NULL);

    QScopedPointer<U2DbiIterator<U2DataId> > iter(oDbi->getObjectsByVisualName(name, U2Type::Unknown, os));
    SAFE_POINT_OP(os, NULL);

    while (iter->hasNext()) {
        const U2DataId objId = iter->next();
        GObject *obj = getObjectById(objId);
        if (NULL != obj) {
            return obj;
        }
    }
    return NULL;
}

GObject * Document::findGObjectByNameInMem(const QString &name) const {
    foreach(GObject *obj, objects) {
        if (obj->getGObjectName() == name) {
            return obj;
        }
    }
    return NULL;
}

GObject * Document::findGObjectByName(const QString &name) const {
    return isLoaded() ? findGObjectByNameInDb(name) : findGObjectByNameInMem(name);
}

QList<GObject*> Document::findGObjectByType(GObjectType t, UnloadedObjectFilter f) const {
    return GObjectUtils::select(objects, t, f);
}


void Document::checkUnloadedState() const {
#ifdef _DEBUG
    assert(!isLoaded());
    bool hasNoLoadedObjects = findGObjectByType(GObjectTypes::UNLOADED, UOF_LoadedAndUnloaded).count() == objects.count();
    assert(hasNoLoadedObjects);
    if (!df->checkFlags(DocumentFormatFlag_AllowDuplicateNames)) {
        checkUniqueObjectNames();
    }
#endif
}

void Document::checkUniqueObjectNames() const {
#ifdef _DEBUG
    QVariantMap hints = getGHintsMap();
    bool dontCheckUniqueNames = hints.value(DocumentReadingMode_DontMakeUniqueNames, false).toBool();
    if (dontCheckUniqueNames) {
        return;
    }
    QSet<QString> names;
    foreach(GObject* o, objects) {
        const QString& name = o->getGObjectName();
        assert(!names.contains(name));
        names.insert(name);
    }
#endif
}
void Document::checkLoadedState() const {
#ifdef _DEBUG
    assert(isLoaded());
    bool hasNoUnloadedObjects = findGObjectByType(GObjectTypes::UNLOADED, UOF_LoadedAndUnloaded).isEmpty();
    assert(hasNoUnloadedObjects);
    if (!df->checkFlags(DocumentFormatFlag_AllowDuplicateNames)) {
        checkUniqueObjectNames();
    }
#endif
}

class DocumentChildEventsHelper {
public:
    DocumentChildEventsHelper(Document *doc)
        : doc(doc) {
        if (NULL != doc) {
            doc->d_ptr->receiveChildEvents = false;
        }
    }

    ~DocumentChildEventsHelper() {
        if (NULL != doc) {
            doc->d_ptr->receiveChildEvents = true;
        }
    }

private:
    Document *doc;
};

void Document::loadFrom(Document* sourceDoc) {
    SAFE_POINT(!isLoaded(), QString("Document is already loaded: ").arg(getURLString()), )

        DocumentChildEventsHelper eventsHelper(this);
    Q_UNUSED(eventsHelper);

    sourceDoc->checkLoadedState();
    checkUnloadedState();

    loadStateChangeMode = true;

    QMap<QString, UnloadedObjectInfo> unloadedInfo;

    foreach(GObject* obj, objects) { //remove all unloaded objects but save hints
        unloadedInfo.insert(obj->getGObjectName(), UnloadedObjectInfo(obj));
        _removeObject(obj, documentOwnsDbiResources);
    }

    ctxState->setAll(sourceDoc->getGHints()->getMap());

    lastUpdateTime = sourceDoc->getLastUpdateTime();

    //copy instance mod-locks if any
    StateLock* mLock = modLocks[DocumentModLock_FORMAT_AS_INSTANCE];
    StateLock* dLock = sourceDoc->modLocks[DocumentModLock_FORMAT_AS_INSTANCE];
    if (mLock != NULL) {
        if (dLock == NULL) {
            unlockState(mLock);
            delete mLock;
            modLocks[DocumentModLock_FORMAT_AS_INSTANCE] = NULL;
        } else {
            mLock->setUserDesc(dLock->getUserDesc());
        }
    } else if (dLock != NULL) {
        modLocks[DocumentModLock_FORMAT_AS_INSTANCE] = new StateLock(dLock->getUserDesc());
        lockState(modLocks[DocumentModLock_FORMAT_AS_INSTANCE]);
    }

    dbiRef = sourceDoc->dbiRef;
    documentOwnsDbiResources = sourceDoc->isDocumentOwnsDbiResources();
    trackModifications = sourceDoc->isModificationTracked();
    sourceDoc->dbiRef = U2DbiRef();
    sourceDoc->setDocumentOwnsDbiResources(false);

    QList<GObject*> sourceObjects = sourceDoc->getObjects();
    sourceDoc->unload(false);
    foreach(GObject* obj, sourceObjects) {
        //TODO: add constrains to ObjectRelations!!
        UnloadedObjectInfo info = unloadedInfo.value(obj->getGObjectName());
        if (info.type == obj->getGObjectType()) {
            QVariantMap mergedHints = obj->getGHintsMap();
            foreach(const QString& k, info.hints.keys()) {
                if (!mergedHints.contains(k)) {
                    mergedHints.insert(k, info.hints.value(k));
                } else if (k == GObjectHint_RelatedObjects) {
                    mergedHints[k] = info.hints[k];
                }
            }
            obj->getGHints()->setMap(mergedHints);
        }
        _addObject(obj);
    }
    setLoaded(true);

    //TODO: rebind local objects relations if url!=d.url

    loadStateChangeMode = false;

    checkLoadedState();
}

void Document::setLoaded(bool v) {
    if (v == isLoaded()) {
        return;
    }
    StateLock* l = modLocks[DocumentModLock_UNLOADED_STATE];
    if (v) {
        unlockState(l);
        modLocks[DocumentModLock_UNLOADED_STATE] = NULL;
        delete l;
        checkLoadedState();
    } else {
        assert(l == NULL);
        l = new StateLock(tr("Document is not loaded"));
        modLocks[DocumentModLock_UNLOADED_STATE] = l;
        lockState(l);
        checkUnloadedState();
    }
    emit si_loadedStateChanged();
}

void Document::initModLocks(const QString& instanceModLockDesc, bool loaded) {
    setLoaded(loaded);

    // must be locked for modifications if io-adapter does not support writes
    if (!io->isIOModeSupported(IOAdapterMode_Write)) {
        modLocks[DocumentModLock_IO] = new StateLock(tr("IO adapter does not support write operation"));
        lockState(modLocks[DocumentModLock_IO]);
    }

    // must be locked for modifications if not document format does not support writes
    if (!df->checkFlags(DocumentFormatFlag_SupportWriting)) {
        modLocks[DocumentModLock_FORMAT_AS_CLASS] = new StateLock(tr("No write support for document format"));
        lockState(modLocks[DocumentModLock_FORMAT_AS_CLASS]);
    }

    if (!instanceModLockDesc.isEmpty()) {
        modLocks[DocumentModLock_FORMAT_AS_INSTANCE] = new StateLock(instanceModLockDesc);
        lockState(modLocks[DocumentModLock_FORMAT_AS_INSTANCE]);
    }
}

void Document::setName(const QString& newName) {
    if (name == newName) {
        return;
    }
    name = newName;
    emit si_nameChanged();
}

void Document::setURL(const GUrl& newUrl) {
    assert(!isLoaded() || !isStateLocked());
    if (url == newUrl) {
        return;
    }
    url = newUrl;
    emit si_urlChanged();
}

bool Document::checkConstraints(const Document::Constraints& c) const {
    if (c.stateLocked != TriState_Unknown) {
        if (c.stateLocked == TriState_No && isStateLocked()) {
            return false;
        }
        if (c.stateLocked == TriState_Yes && !isStateLocked()) {
            return false;
        }
    }

    if (!c.formats.isEmpty()) {
        bool found = false;
        foreach(DocumentFormatId f, c.formats) {
            if (df->getFormatId() == f) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    foreach(DocumentModLock l, c.notAllowedStateLocks) {
        if (modLocks[l] != NULL) {
            return false;
        }
    }

    if (!c.objectTypeToAdd.isNull() && !df->isObjectOpSupported(this, DocumentFormat::DocObjectOp_Add, c.objectTypeToAdd)) {
        return false;
    }


    return true;
}


void Document::setUserModLock(bool v) {
    if (hasUserModLock() == v) {
        return;
    }
    if (v) {
        StateLock* sl = new StateLock(tr("Locked by user"));
        modLocks[DocumentModLock_USER] = sl;
        lockState(sl);
    } else {
        StateLock* sl = modLocks[DocumentModLock_USER];
        modLocks[DocumentModLock_USER] = NULL;
        unlockState(sl);
        delete sl;
    }

    //hack: readonly settings are stored in project, so if document is in project -> mark project as modified
    if (getParentStateLockItem() != NULL) {
        getParentStateLockItem()->setModified(true);
    }
}

bool Document::unload(bool deleteObjects) {
    assert(isLoaded());
    DocumentChildEventsHelper eventsHelper(this);
    Q_UNUSED(eventsHelper);

    QList<StateLock*> locks = findLocks(StateLockableTreeFlags_ItemAndChildren, StateLockFlag_LiveLock);
    bool liveLocked = (locks.size() > 1);
    if (locks.size() == 1 && !liveLocked) {
        SAFE_POINT(locks.first() != NULL, tr("Lock is NULL"), false);
        liveLocked &= (locks.first()->getUserDesc() == UNLOAD_LOCK_NAME);
    }
    if (liveLocked) {
        assert(0);
        return false;
    }

    loadStateChangeMode = true;

    QList<UnloadedObjectInfo> unloadedInfo;
    QList<GObject *> tmpObjects;
    foreach(GObject* obj, objects) {
        unloadedInfo.append(UnloadedObjectInfo(obj));
        // exclude objects from the document
        tmpObjects.append(obj);
        _removeObject(obj, false);
    }
    addUnloadedObjects(unloadedInfo);

    // deallocate objects
    if (deleteObjects) {
        if (isDocumentOwnsDbiResources()) {
            removeObjectsDataFromDbi(tmpObjects);
        }
        qDeleteAll(tmpObjects);
    }

    StateLock* fl = modLocks[DocumentModLock_FORMAT_AS_INSTANCE];
    if (fl != NULL) {
        unlockState(fl);
        modLocks[DocumentModLock_FORMAT_AS_INSTANCE] = NULL;
    }

    dbiRef = U2DbiRef();
    documentOwnsDbiResources = false;

    setLoaded(false);

    loadStateChangeMode = false;

    return true;
}

const U2DbiRef& Document::getDbiRef() const {
    return dbiRef;
}

bool Document::isDatabaseConnection() const {
    return BaseDocumentFormats::DATABASE_CONNECTION == df->getFormatId();
}

void Document::setModified(bool modified, const QString& modType) {
    CHECK(!df->checkFlags(DocumentFormatFlag_DirectWriteOperations), );
    if (loadStateChangeMode && modified && modType == StateLockModType_AddChild) { //ignore modification events during loading/unloading
        return;
    }
    StateLockableTreeItem::setModified(modified, modType);
}

bool Document::isModificationAllowed(const QString& modType) {
    bool ok = loadStateChangeMode && modType == StateLockModType_AddChild;
    ok = ok || StateLockableTreeItem::isModificationAllowed(modType);
    return ok;
}


void Document::setGHints(GHints* newHints) {
    assert(newHints != NULL);
    //gobjects in document keep states in parent document map -> preserve gobject hints
    if (newHints == ctxState) {
        return;
    }
    QList<QVariantMap> objectHints;
    for (int i = 0; i < objects.size(); i++) {
        GObject* obj = objects[i];
        objectHints.append(obj->getGHintsMap());
    }

    delete ctxState;
    ctxState = newHints;

    for (int i = 0;i < objects.size(); i++) {
        const QVariantMap& hints = objectHints[i];
        GObject* obj = objects[i];
        obj->getGHints()->setMap(hints);
    }
}

void Document::addUnloadedObjects(const QList<UnloadedObjectInfo>& info) {
    foreach(const UnloadedObjectInfo& oi, info) {
        UnloadedObject* obj = new UnloadedObject(oi);
        obj->moveToThread(thread());
        _addObjectToHierarchy(obj);
        assert(obj->getDocument() == this);
        emit si_objectAdded(obj);
    }
}

QVariantMap Document::getGHintsMap() const {
    return ctxState->getMap();
}

void Document::setupToEngine(QScriptEngine *engine) {
    qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);
}

QScriptValue Document::toScriptValue(QScriptEngine *engine, Document* const &in) {
    return engine->newQObject(in);
}

void Document::fromScriptValue(const QScriptValue &object, Document* &out) {
    out = qobject_cast<Document*>(object.toQObject());
}

void Document::removeObjectsDataFromDbi(QList<GObject *> objects) {
    const bool removeAsynchronously = AppContext::isGUIMode() && QCoreApplication::instance()->thread() == QThread::currentThread()
        && !ctxState->getMap().contains(DocumentRemovalMode_Synchronous);
    if (removeAsynchronously) {
        // Do not remove objects in the main thread to prevent GUI hanging
        DeleteObjectsTask *deleteTask = new DeleteObjectsTask(objects);
        AppContext::getTaskScheduler()->registerTopLevelTask(deleteTask);
    } else {
        U2OpStatus2Log os;
        DbiOperationsBlock opBlock(dbiRef, os);
        CHECK_OP(os, );
        Q_UNUSED(opBlock);

        DbiConnection con(dbiRef, os);
        CHECK_OP(os, );
        CHECK(con.dbi->getFeatures().contains(U2DbiFeature_RemoveObjects), );

        foreach(GObject* object, objects) {
            U2OpStatus2Log osLog;
            SAFE_POINT(object != NULL, "NULL object was provided", );
            con.dbi->getObjectDbi()->removeObject(object->getEntityRef().entityId, true, osLog);
        }
    }
}

void Document::setLastUpdateTime() {
    QFileInfo fi(getURLString());
    if (fi.exists()) {
        lastUpdateTime = fi.lastModified();
    }
}

void Document::propagateModLocks(Document* doc)  const {
    for (int i = 0; i < DocumentModLock_NUM_LOCKS; i++) {
        StateLock* lock = modLocks[i];
        if (lock != NULL && doc->modLocks[i] != NULL) {
            StateLock* newLock = new StateLock(lock->getUserDesc(), lock->getFlags());
            doc->modLocks[i] = newLock;
            doc->lockState(newLock);
        }
    }
}

void Document::setIOAdapterFactory(IOAdapterFactory* iof) {
    io = iof;
}

DocumentMimeData::DocumentMimeData(Document* obj)
    : objPtr(obj) {
    setUrls(QList<QUrl>() << QUrl(GUrlUtils::gUrl2qUrl(obj->getURL())));
}

}//namespace
