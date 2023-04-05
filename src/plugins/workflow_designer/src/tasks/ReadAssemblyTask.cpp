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

#include "ReadAssemblyTask.h"

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileStorageUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/BAMUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowContext.h>

namespace U2 {
namespace Workflow {

/************************************************************************/
/* Factory */
/************************************************************************/
ReadAssemblyTaskFactory::ReadAssemblyTaskFactory()
    : ReadDocumentTaskFactory(ReadFactories::READ_ASSEMBLY) {
}

ReadDocumentTask* ReadAssemblyTaskFactory::createTask(const QString& url, const QVariantMap& hints, WorkflowContext* ctx) {
    QString datasetName = hints.value(BaseSlots::DATASET_SLOT().getId(), "").toString();
    return new ReadAssemblyTask(url, datasetName, ctx);
}

/************************************************************************/
/* Task */
/************************************************************************/
ConvertToIndexedBamTask::ConvertToIndexedBamTask(const DocumentFormatId& _formatId, const GUrl& _url, WorkflowContext* _ctx)
    : Task("Convert assembly file to sorted BAM", TaskFlag_None), formatId(_formatId), url(_url), ctx(_ctx) {
}

void ConvertToIndexedBamTask::run() {
    AppFileStorage* fileStorage = AppContext::getAppFileStorage();
    CHECK_EXT(nullptr != fileStorage, stateInfo.setError("NULL file storage"), );

    QString cashedSortedBam = FileStorageUtils::getSortedBamUrl(url.getURLString(), ctx->getWorkflowProcess());
    if (!cashedSortedBam.isEmpty()) {
        result = cashedSortedBam;
        return;
    }

    GUrl bamUrl = url;
    if (formatId == BaseDocumentFormats::SAM) {
        QString bam = FileStorageUtils::getSamToBamConvertInfo(url.getURLString(), ctx->getWorkflowProcess());
        if (bam.isEmpty()) {
            QString dir = fileStorage->createDirectory();
            bamUrl = dir + "/" + url.fileName() + ".bam";
            BAMUtils::convertSamToBam(stateInfo, url.getURLString(), bamUrl.getURLString());
            CHECK_OP(stateInfo, );

            addConvertedFile(bamUrl);
            FileStorageUtils::addSamToBamConvertInfo(url.getURLString(), bamUrl.getURLString(), ctx->getWorkflowProcess());
        } else {
            bamUrl = bam;
        }
    } else {
        CHECK_EXT(BaseDocumentFormats::BAM == formatId, setError("Only BAM/SAM files could be converted"), );
    }

    bool sorted = BAMUtils::isSortedBam(bamUrl.getURLString(), stateInfo);
    CHECK_OP(stateInfo, );

    GUrl sortedBamUrl = bamUrl;
    if (!sorted) {
        QString dir = fileStorage->createDirectory();
        QString baseName;
        if (dir.isEmpty()) {
            baseName = bamUrl.getURLString();
        } else {
            baseName = dir + "/" + bamUrl.fileName();
        }
        baseName += ".sorted";
        sortedBamUrl = BAMUtils::sortBam(bamUrl.getURLString(), baseName, stateInfo);
        CHECK_OP(stateInfo, );
        addConvertedFile(sortedBamUrl);
    }

    bool indexed = sorted && BAMUtils::hasValidBamIndex(sortedBamUrl.getURLString());
    if (!indexed) {
        BAMUtils::createBamIndex(sortedBamUrl.getURLString(), stateInfo);
        CHECK_OP(stateInfo, );
    }

    // if the file was sorted then it is needed to be saved in the file storage
    if (!sorted) {
        FileStorageUtils::addSortedBamUrl(bamUrl.getURLString(), sortedBamUrl.getURLString(), ctx->getWorkflowProcess());
        if (bamUrl != url) {  // add "SAM file - SORTED_BAM - sorted BAM file"
            FileStorageUtils::addSortedBamUrl(url.getURLString(), sortedBamUrl.getURLString(), ctx->getWorkflowProcess());
        }
    }
    result = sortedBamUrl;
}

GUrl ConvertToIndexedBamTask::getResultUrl() const {
    return result;
}

const QStringList& ConvertToIndexedBamTask::getConvertedFiles() const {
    return convertedFiles;
}

void ConvertToIndexedBamTask::addConvertedFile(const GUrl& url) {
    convertedFiles << url.getURLString();
}

/************************************************************************/
/* ReadAssemblyTask */
/************************************************************************/
ReadAssemblyTask::ReadAssemblyTask(const QString& url, const QString& datasetName, WorkflowContext* _ctx)
    : ReadDocumentTask(url, tr("Read assembly from %1").arg(url), datasetName, TaskFlags_FOSE_COSC | TaskFlag_CollectChildrenWarnings),
      ctx(_ctx), format(nullptr), doc(nullptr), convertTask(nullptr), importTask(nullptr) {
}

static bool isConvertingFormat(const DocumentFormatId& formatId) {
    return (BaseDocumentFormats::SAM == formatId || BaseDocumentFormats::BAM == formatId);
}

void ReadAssemblyTask::prepare() {
    QFileInfo fi(url);
    if (!fi.exists()) {
        stateInfo.setError(tr("File '%1' does not exist").arg(url));
        return;
    }

    FormatDetectionConfig conf;
    conf.useImporters = true;
    conf.excludeHiddenFormats = false;
    QList<FormatDetectionResult> fs = DocumentUtils::detectFormat(url, conf);
    
    foreach (const FormatDetectionResult& f, fs) {
        if (nullptr != f.format) {
            if (isConvertingFormat(f.format->getFormatId())) {
                convertTask = new ConvertToIndexedBamTask(f.format->getFormatId(), url, ctx);
                addSubTask(convertTask);
                return;
            }

            const QSet<GObjectType>& types = f.format->getSupportedObjectTypes();
            if (types.contains(GObjectTypes::ASSEMBLY)) {
                format = f.format;
                break;
            }
        } else if (nullptr != f.importer) {
            if(f.importer->getSupportedObjectTypes().contains(GObjectTypes::ASSEMBLY)) {
                U2OpStatusImpl os;
                U2DbiRef dbiRef = ctx->getDataStorage()->createTmpDbi(os);
                SAFE_POINT_OP(os, );

                QVariantMap hints;
                hints.insert(DocumentFormat::DBI_REF_HINT, qVariantFromValue(dbiRef));
                QString destination = GUrlUtils::rollFileName(AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath() + "/" + fi.baseName(), "_");
                hints.insert(ImportHint_DestinationUrl, destination);
                importTask = f.importer->createImportTask(f, false, hints);
                addSubTask(importTask);
                return;
            }
        }
    }

    if (format == nullptr) {
        stateInfo.setError(tr("Unsupported document format: %1").arg(getUrl()));
        return;
    }
}

QList<Task*> ReadAssemblyTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(nullptr != subTask, result);
    if (subTask->hasError()) {
        if (convertTask == subTask) {
            taskLog.error(subTask->getError());
            setError(tr("Can not read assemblies from the file: %1").arg(getUrl()));
        }
        return result;
    }

    if (importTask == subTask) {
        doc = importTask->takeDocument(false);
    } else if (convertTask == subTask) {
        url = convertTask->getResultUrl().getURLString();
        format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::BAM);
        producedFiles << convertTask->getConvertedFiles();
    }
    return result;
}

void ReadAssemblyTask::run() {
    if (format == nullptr && nullptr == doc) {
        return;
    }

    QScopedPointer<Document> docPtr(nullptr);
    bool useGC = true;
    if (doc == nullptr) {
        useGC = false;
        ioLog.info(tr("Reading assembly from %1 [%2]").arg(url).arg(format->getFormatName()));
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
        QVariantMap hints;
        {
            // TODO: fix this hardcoded DBI id recognition
            U2DbiFactoryId fId;
            if (BaseDocumentFormats::UGENEDB == format->getFormatId()) {
                fId = DEFAULT_DBI_ID;
            } else if (BaseDocumentFormats::BAM == format->getFormatId()) {
                SAFE_POINT(nullptr != convertTask, "Internal error! Converting stage is missed", );
                fId = BAM_DBI_ID;
            }
            U2DbiFactory* dbiFactory = AppContext::getDbiRegistry()->getDbiFactoryById(fId);
            SAFE_POINT(nullptr != dbiFactory, QString("Unknown dbi factory id: %").arg(fId), );

            U2OpStatusImpl os;
            U2DbiRef dbiRef(dbiFactory->getId());
            dbiRef.dbiId = QString::fromLocal8Bit(url.toLocal8Bit().constData());
            ctx->getDataStorage()->openDbi(dbiRef, os);
            CHECK_OP(os, );

            hints.insert(DocumentFormat::DBI_REF_HINT, qVariantFromValue(dbiRef));
        }
        docPtr.reset(format->loadDocument(iof, url, hints, stateInfo));
        CHECK_OP(stateInfo, );
    } else {
        useGC = true;
        docPtr.reset(doc);
        doc = nullptr;
    }
    CHECK(!docPtr.isNull(), );
    docPtr->setDocumentOwnsDbiResources(false);

    QList<GObject*> assemblies = docPtr->findGObjectByType(GObjectTypes::ASSEMBLY);
    if (assemblies.isEmpty()) {
        setError(tr("No assemblies in the file: %1").arg(getUrl()));
        return;
    }
    foreach (GObject* go, assemblies) {
        auto assemblyObj = dynamic_cast<AssemblyObject*>(go);
        CHECK_EXT(nullptr != assemblyObj, taskLog.error(tr("Incorrect assembly object in %1").arg(url)), );

        SharedDbiDataHandler handler = ctx->getDataStorage()->getDataHandler(assemblyObj->getEntityRef(), useGC);
        result << handler;
    }
}

}  // namespace Workflow
}  // namespace U2
