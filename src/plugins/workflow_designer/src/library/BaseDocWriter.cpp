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

#include "CoreLib.h"

#include <U2Gui/DialogUtils.h>

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowUtils.h>

#include <U2Core/AppContext.h>
#include <U2Core/DeleteObjectsTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/GHints.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ImportObjectToDatabaseTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BaseDocWriter.h"

namespace U2 {
namespace LocalWorkflow {

/**********************************
* BaseDocWriter
**********************************/
BaseDocWriter::BaseDocWriter(Actor* a, const DocumentFormatId& fid)
    : BaseWorker(a), format(NULL), dataStorage(LocalFs), ch(NULL), append(true), fileMode(SaveDoc_Roll), objectsReceived(false)
{
    format = AppContext::getDocumentFormatRegistry()->getFormatById(fid);
}

BaseDocWriter::BaseDocWriter(Actor *a)
    : BaseWorker(a), format(NULL), dataStorage(LocalFs), ch(NULL), append(true), fileMode(SaveDoc_Roll), objectsReceived(false)
{

}

void BaseDocWriter::cleanup() {
    foreach (IOAdapter *io, adapters.values()) {
        if (io->isOpen()) {
            io->close();
        }
    }
}

void BaseDocWriter::init() {
    SAFE_POINT(ports.size() == 1, "Unexpected port count", );
    ch = ports.values().first();
}

#define GZIP_SUFFIX ".gz"

void BaseDocWriter::takeParameters(U2OpStatus &os) {
    Attribute *dataStorageAttr = actor->getParameter(BaseAttributes::DATA_STORAGE_ATTRIBUTE().getId());

    const QString storage = (NULL == dataStorageAttr) ? BaseAttributes::LOCAL_FS_DATA_STORAGE()
                                                      : dataStorageAttr->getAttributeValue<QString>(context);
    if (BaseAttributes::LOCAL_FS_DATA_STORAGE() == storage) {
        dataStorage = LocalFs;

        Attribute *formatAttr = actor->getParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId());
        if (NULL != formatAttr) { // user sets format
            QString formatId = formatAttr->getAttributeValue<QString>(context);
            format = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
        }
        if (NULL == format) {
            os.setError(tr("Document format not set"));
            return;
        }

        fileMode = getValue<uint>(BaseAttributes::FILE_MODE_ATTRIBUTE().getId());
        Attribute *a = actor->getParameter(BaseAttributes::ACCUMULATE_OBJS_ATTRIBUTE().getId());
        if(NULL != a) {
            append = a->getAttributeValue<bool>(context);
        } else {
            append = true;
        }
    } else if (BaseAttributes::SHARED_DB_DATA_STORAGE() == storage) {
        dataStorage = SharedDb;

        const QString fullDbUrl = getValue<QString>(BaseAttributes::DATABASE_ATTRIBUTE().getId());
        dstDbiRef = SharedDbUrlUtils::getDbRefFromEntityUrl(fullDbUrl);
        CHECK_EXT(dstDbiRef.isValid(), os.setError(tr("Invalid database reference")), );

        dstPathInDb = getValue<QString>(BaseAttributes::DB_PATH().getId());
        CHECK_EXT(!dstPathInDb.isEmpty(), os.setError(tr("Empty destination path supplied")), );
    } else {
        os.setError(tr("Unexpected data storage attribute value"));
    }
}

namespace {
    QString toFileName(const QString &base, const QString &suffix, const QString &ext) {
        QString result = base + suffix;
        if (!ext.isEmpty()) {
            result += "." + ext;
        }
        return result;
    }
}

QString BaseDocWriter::getDefaultFileName() const {
    return actor->getId() + "_output";
}

bool BaseDocWriter::ifGroupByDatasets() const {
    Attribute *a = actor->getParameter(BaseAttributes::ACCUMULATE_OBJS_ATTRIBUTE().getId());
    if(NULL == a) {
        return false;
    }
    return a->getAttributeValue<bool>(context);
}

QString BaseDocWriter::getSuffix() const {
    Attribute *a = actor->getParameter(BaseAttributes::URL_SUFFIX().getId());
    if(NULL == a) {
        return "";
    }
    return a->getAttributeValue<QString>(context);
}

QString BaseDocWriter::getExtension() const {
    CHECK(NULL != format, "");
    QStringList exts = format->getSupportedDocumentFileExtensions();
    CHECK(!exts.isEmpty(), "");
    return exts.first();
}

QString BaseDocWriter::getBaseName(const MessageMetadata &metadata, bool groupByDatasets, const QString &defaultName) {
    if (groupByDatasets) {
        if (metadata.getDatasetName().isEmpty()) {
            return defaultName;
        }
        return metadata.getDatasetName();
    } else if (!metadata.getFileUrl().isEmpty()) {
        QFileInfo info(metadata.getFileUrl());
        return info.baseName();
    } else if (!metadata.getDatabaseId().isEmpty()) {
        return metadata.getDatabaseId();
    }
    return defaultName;
}

QString BaseDocWriter::generateUrl(int metadataId) const {
    MessageMetadata metadata = context->getMetadataStorage().get(metadataId);
    return generateUrl(metadata, ifGroupByDatasets(), getSuffix(), getExtension(), getDefaultFileName());
}

QString BaseDocWriter::generateUrl(const MessageMetadata &metadata, bool groupByDatasets, const QString &suffix, const QString &ext, const QString &defaultName) {
    QString baseName = getBaseName(metadata, groupByDatasets, defaultName);
    return toFileName(baseName, suffix, ext);
}

QStringList BaseDocWriter::takeUrlList(const QVariantMap &data, int metadataId, U2OpStatus &os) {
    QString url = getValue<QString>(BaseAttributes::URL_OUT_ATTRIBUTE().getId());
    if (url.isEmpty()) {
        url = data.value(BaseSlots::URL_SLOT().getId()).toString();
    }
    if (url.isEmpty()) {
        url = generateUrl(metadataId);
    }
    if (url.isEmpty()) {
        QString err = tr("Unspecified URL to write %1").arg(format->getFormatName());
        os.setError(err);
        return QStringList();
    }

    QStringList result;
    result << context->absolutePath(url);
    return result;
}

bool BaseDocWriter::isSupportedSeveralMessages() const {
    // if the format can contain only one object then adapters must be created for each message
    if (format->checkFlags(DocumentFormatFlag_SingleObjectFormat)) {
        return false;
    }
    if (format->checkFlags(DocumentFormatFlag_OnlyOneObject)) {
        return false;
    }
    return true;
}

bool BaseDocWriter::ifCreateAdapter(const QString &url) const {
    if (!isSupportedSeveralMessages()) {
        return true;
    }

    // if not accumulate object in one file
    if (!append) {
        return true;
    }

    return (!adapters.contains(url));
}

void BaseDocWriter::openAdapter(IOAdapter *io, const QString &aUrl, const SaveDocFlags &flags, U2OpStatus &os) {
    { // prepare dir
        QFileInfo info(aUrl);
        if (!info.dir().exists()) {
            bool created = info.dir().mkpath(info.dir().absolutePath());
            if (!created) {
                os.setError(tr("Can not create folder: %1").arg(info.dir().absolutePath()));
            }
        }
    }

    // generate a target URL from the source URL
    QString url = aUrl;
    int suffix = 0;
    do {
        if ((0 == suffix) && counters.contains(aUrl)) {
            suffix = counters[aUrl];
        }
        if (suffix > 0) {
            url = GUrlUtils::insertSuffix(aUrl, "_" + QString::number(suffix));
        }
        suffix++;
    } while (monitor()->containsOutputFile(url));

    if (flags.testFlag(SaveDoc_Roll)) {
        TaskStateInfo ti;
        if (!GUrlUtils::renameFileWithNameRoll(url, ti, usedUrls)) {
            os.setError(ti.getError());
            return;
        }
    }
    IOAdapterMode mode = flags.testFlag(SaveDoc_Append) ? IOAdapterMode_Append : IOAdapterMode_Write;
    bool opened = io->open(url, mode);
    if (!opened) {
        os.setError(tr("Can not open a file for writing: %1").arg(url));
    }

    counters[aUrl] = suffix;
}

IOAdapter * BaseDocWriter::getAdapter(const QString &url, U2OpStatus &os) {
    if (!ifCreateAdapter(url)) {
        return adapters[url];
    }

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    openAdapter(io.data(), url, SaveDocFlags(fileMode), os);
    CHECK_OP(os, NULL);

    QString resultUrl = io->getURL().getURLString();
    if (!adapters.contains(url)) {
        adapters[url] = io.data();
    }
    if (!adapters.contains(resultUrl)) {
        adapters[resultUrl] = io.data();
    }
    usedUrls << resultUrl;
    monitor()->addOutputFile(resultUrl, getActorId());

    return io.take();
}

Document * BaseDocWriter::getDocument(IOAdapter *io, U2OpStatus &os) {
    if (docs.contains(io)) {
        return docs[io];
    }

    QVariantMap hints;
    U2DbiRef dbiRef = context->getDataStorage()->getDbiRef();
    hints.insert(DocumentFormat::DBI_REF_HINT, qVariantFromValue(dbiRef));
    Document *doc = format->createNewLoadedDocument(io->getFactory(), io->getURL(), os, hints);
    CHECK_OP(os, NULL);

    doc->setDocumentOwnsDbiResources(false);
    docs[io] = doc;
    return doc;
}

bool BaseDocWriter::isStreamingSupport() const {
    return format->isStreamingSupport();
}

void BaseDocWriter::storeData(const QStringList &urls, const QVariantMap &data, U2OpStatus &os) {
    foreach (const QString &anUrl, urls) {
        IOAdapter *io = getAdapter(anUrl, os);
        CHECK_OP(os, );
        if (isStreamingSupport()) {
            // TODO: make it in separate thread!
            storeEntry(io, data, ch->takenMessages());
        } else {
            Document *doc = getDocument(io, os);
            CHECK_OP(os, );
            data2doc(doc, data);
        }
    }
}

#define CHECK_OS(os) \
    if (os.hasError()) { \
    reportError(os.getError()); \
    continue; \
    }

Task * BaseDocWriter::tick() {
    U2OpStatusImpl os;
    while(ch->hasMessage()) {
        const Message inputMessage = getMessageAndSetupScriptValues(ch);
        takeParameters(os);
        CHECK_OS(os);

        const QVariantMap data = inputMessage.getData().toMap();
        if (!hasDataToWrite(data)) {
            reportError(tr("No data to write"));
            continue;
        }

        if (LocalFs == dataStorage) {
            const QStringList urls = takeUrlList(data, inputMessage.getMetadataId(), os);
            CHECK_OS(os);
            storeData(urls, data, os);
            CHECK_OS(os);

            if (!append) {
                break;
            }
        } else if (SharedDb == dataStorage) {
            Task *result = createWriteToSharedDbTask(data);
            if (NULL == result) {
                continue;
            } else {
                return result;
            }
        } else {
            reportError(tr("Unexpected data storage attribute value"));
        }
    }

    bool done = ch->isEnded() && !ch->hasMessage();
    if (append && !done) {
        return NULL;
    }
    if (done) {
        setDone();
    }
    if (SharedDb == dataStorage && !objectsReceived) {
        reportNoDataReceivedWarning();
    }
    return LocalFs == dataStorage ? processDocs() : NULL;
}

void BaseDocWriter::reportNoDataReceivedWarning() {
    monitor()->addError(tr("Nothing to write"), getActorId(), WorkflowNotification::U2_WARNING);
}

QSet<GObject *> BaseDocWriter::getObjectsToWrite(const QVariantMap &data) const {
    QSet<GObject *> result = getObjectsToWrite(data);
    result.remove(NULL); // eliminate invalid objects
    return result;
}

Task * BaseDocWriter::createWriteToSharedDbTask(const QVariantMap &data) {
    QList<Task *> tasks;
    foreach (GObject *obj, BaseDocWriter::getObjectsToWrite(data)) {
        if (NULL == obj) {
            reportError(tr("Unable to fetch data from a message"));
            continue;
        }
        Task *importTask = new ImportObjectToDatabaseTask(obj, dstDbiRef, dstPathInDb);
        connect(new TaskSignalMapper(importTask), SIGNAL(si_taskFinished(Task *)), SLOT(sl_objectImported(Task *)));
        tasks.append(importTask);
    }
    if (tasks.isEmpty()) {
        return NULL;
    } else {
        objectsReceived = true;
    }
    Task *resultTask = tasks.size() == 1 ? tasks.first() : new MultiTask(tr("Save objects to a shared database"), tasks);
    return resultTask;
}

void BaseDocWriter::sl_objectImported(Task *importTask) {
    ImportObjectToDatabaseTask *realTask = qobject_cast<ImportObjectToDatabaseTask *>(importTask);
    SAFE_POINT(NULL != realTask, "Invalid task detected", );
    delete realTask->getSourceObject();
}

Task* BaseDocWriter::processDocs()
{
    if(adapters.isEmpty()) {
        reportNoDataReceivedWarning();
    }
    if (docs.isEmpty()) {
        return NULL;
    }
    QList<Task*> tlist;
    foreach (IOAdapter *io, docs.keys()) {
        Document *doc = docs[io];
        ioLog.details(tr("Writing to %1 [%2]").arg(io->getURL().getURLString()).arg(format->getFormatName()));
        io->close();
        GHints *hints = doc->getGHints();
        hints->set(DocumentRemovalMode_Synchronous, QString());
        tlist << getWriteDocTask(doc, getDocFlags());
    }
    docs.clear();

    return tlist.size() == 1 ? tlist.first() : new MultiTask(tr("Save documents"), tlist);
}

Task * BaseDocWriter::getWriteDocTask(Document *doc, const SaveDocFlags &flags) {
    return new SaveDocumentTask(doc, flags, DocumentUtils::getNewDocFileNameExcludesHint());
}

SaveDocFlags BaseDocWriter::getDocFlags() const {
    SaveDocFlags flags(fileMode);
    flags |= SaveDoc_DestroyAfter;
    if (flags.testFlag(SaveDoc_Roll)) {
        flags ^= SaveDoc_Roll;
    }
    return flags;
}

QString BaseDocWriter::getUniqueObjectName(const Document *doc, const QString &name) {
    QString result = name;
    int num = 0;
    bool found = false;
    while (NULL != doc->findGObjectByName(result)) {
        found = true;
        num++;
        result = name + QString("_%1").arg(num);
    }

    return found ? result : name;
}

} // Workflow namespace
} // U2 namespace
