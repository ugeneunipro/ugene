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

#include <QDir>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Type.h>

#include <U2Lang/DbiDataStorage.h>

#include "CufflinksSupport.h"
#include "CufflinksSupportTask.h"
#include "CuffmergeSupportTask.h"
#include "tophat/TopHatSettings.h"

namespace U2 {

const QString CuffmergeSupportTask::outSubDirBaseName("cuffmerge_out");

CuffmergeSupportTask::CuffmergeSupportTask(const CuffmergeSettings &_settings) :
    ExternalToolSupportTask(tr("Running Cuffmerge task"), TaskFlags_FOSE_COSC), settings(_settings),
    fileNum(0),
    mergeTask(NULL),
    loadResultTask(NULL)
{
    SAFE_POINT_EXT(NULL != settings.storage, setError(tr("Workflow data storage is NULL")), );
    SAFE_POINT_EXT(!settings.annotationTables.isEmpty(), setError(tr("There are no annotations to process")), );
}

CuffmergeSupportTask::~CuffmergeSupportTask() {
    qDeleteAll(docs);
    qDeleteAll(result);
}

void CuffmergeSupportTask::prepare() {
    setupWorkingDirPath();

    settings.outDir = GUrlUtils::createDirectory(
                settings.outDir + "/" + outSubDirBaseName,
                "_", stateInfo);
    CHECK_OP(stateInfo, );

    foreach (const Workflow::SharedDbiDataHandler &annTableHandler, settings.annotationTables) {
        Task *task = createWriteTask(annTableHandler, getAnnsFilePath());
        CHECK_OP(stateInfo, );
        addSubTask(task);
    }
}

QList<Task*> CuffmergeSupportTask::onSubTaskFinished(Task *subTask) {
    if (writeTasks.contains(subTask)) {
        writeTasks.removeOne(subTask);
    }

    QList<Task*> newSubTasks;
    if (writeTasks.isEmpty() && NULL == mergeTask) {
        newSubTasks << createCuffmergeTask();
    }

    else if (subTask == mergeTask) {
        loadResultTask = createLoadResultDocumentTask("merged.gtf");
        CHECK_OP(stateInfo, newSubTasks);
        newSubTasks << loadResultTask;
    }

    else if (subTask == loadResultTask) {
        QScopedPointer<Document> doc(loadResultTask->takeDocument());
        SAFE_POINT_EXT(NULL != doc, setError(L10N::nullPointerError("document with annotations")), newSubTasks);
        doc->setDocumentOwnsDbiResources(false);
        foreach (GObject *object, doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE)) {
            doc->removeObject(object, DocumentObjectRemovalMode_Release);
            result << qobject_cast<AnnotationTableObject *>(object);
        }
    }

    return newSubTasks;
}

void CuffmergeSupportTask::run() {
    ExternalToolSupportUtils::appendExistingFile(settings.outDir + "/merged.gtf", outputFiles);
}

void CuffmergeSupportTask::setupWorkingDirPath() {
    if (0 == QString::compare(settings.workingDir, "default", Qt::CaseInsensitive)) {
        workingDir = ExternalToolSupportUtils::createTmpDir(CufflinksSupport::CUFFMERGE_TMP_DIR, stateInfo);
    } else {
        workingDir = ExternalToolSupportUtils::createTmpDir(settings.workingDir, CufflinksSupport::CUFFMERGE_TMP_DIR, stateInfo);
    }
}

QString CuffmergeSupportTask::getAnnsFilePath() {
    QString filePath = workingDir + QString("/tmp_%1.gtf").arg(fileNum);
    fileNum++;
    return filePath;
}

Task *CuffmergeSupportTask::createWriteTask(const Workflow::SharedDbiDataHandler &annTableHandler, const QString &filePath) {
    Document *doc = prepareDocument(annTableHandler, filePath);
    CHECK_OP(stateInfo, NULL);

    docs << doc;
    SaveDocumentTask *task = new SaveDocumentTask(doc, doc->getIOAdapterFactory(), filePath);
    writeTasks << task;

    return task;
}

void CuffmergeSupportTask::writeFileList() {
    listFilePath = workingDir + "/gtf_list.txt";
    QFile file(listFilePath);
    bool res = file.open(QIODevice::WriteOnly);
    if (!res) {
        stateInfo.setError(tr("Can not create a file: %1").arg(listFilePath));
        return;
    }

    QString data;
    foreach (Document *doc, docs) {
        data += doc->getURLString() + "\n";
    }
    file.write(data.toLatin1());
    file.close();
}

Task * CuffmergeSupportTask::createCuffmergeTask() {
    writeFileList();
    CHECK_OP(stateInfo, NULL);

    QStringList args;
    {
        args << "-p" << QString::number(TopHatSettings::getThreadsCount());
        if (!settings.refAnnsUrl.isEmpty()) {
            args << "--ref-gtf" << settings.refAnnsUrl;
        }
        if (!settings.refSeqUrl.isEmpty()) {
            args << "--ref-sequence" << settings.refSeqUrl;
        }
        args << "-o" << settings.outDir;
        args << "--min-isoform-fraction" << QString::number(settings.minIsoformFraction);
        args << listFilePath;
    }

    QStringList addPaths;
    {
        ExternalToolRegistry *registry = AppContext::getExternalToolRegistry();

        ExternalTool *cm =registry->getById(CufflinksSupport::ET_CUFFMERGE_ID);
        ExternalTool *cc =registry->getById(CufflinksSupport::ET_CUFFCOMPARE_ID);
        QFileInfo cmInfo(cm->getPath());
        QFileInfo ccInfo(cc->getPath());

        addPaths << cmInfo.dir().absolutePath();
        addPaths << ccInfo.dir().absolutePath();
    }

    mergeTask = new ExternalToolRunTask(CufflinksSupport::ET_CUFFMERGE_ID, args, new ExternalToolLogParser(), workingDir, addPaths);
    setListenerForTask(mergeTask);
    return mergeTask;
}

LoadDocumentTask *CuffmergeSupportTask::createLoadResultDocumentTask(const QString &fileName) {
    const QString filePath = settings.outDir + "/" + fileName;

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(NULL != iof, setError(tr("An internal error occurred during getting annotations from a %1 output file!").arg(CufflinksSupport::ET_CUFFMERGE)), NULL);

    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue(settings.storage->getDbiRef());

    return new LoadDocumentTask(BaseDocumentFormats::GTF, filePath, iof, hints);
}

QList<AnnotationTableObject *> CuffmergeSupportTask::takeResult() {
    QList<AnnotationTableObject *> ret = result;
    result.clear();
    return ret;
}

Document *CuffmergeSupportTask::prepareDocument(const Workflow::SharedDbiDataHandler &annTableHandler, const QString &filePath) {
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::GTF);
    SAFE_POINT_EXT(NULL != format, setError(L10N::nullPointerError("GTF format")), NULL);

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(NULL != iof, setError(L10N::nullPointerError("I/O adapter factory")), NULL);

    AnnotationTableObject *annTable = Workflow::StorageUtils::getAnnotationTableObject(settings.storage, annTableHandler);
    SAFE_POINT_EXT(NULL != annTable, setError(L10N::nullPointerError("source annotation data")), NULL);

    Document *doc = format->createNewLoadedDocument(iof, filePath, stateInfo);
    CHECK_OP(stateInfo, NULL);
    doc->setDocumentOwnsDbiResources(false);
    doc->addObject(annTable);

    return doc;
}

QStringList CuffmergeSupportTask::getOutputFiles( ) const {
    return outputFiles;
}

/************************************************************************/
/* CuffmergeSettings */
/************************************************************************/
CuffmergeSettings::CuffmergeSettings() :
    minIsoformFraction(0.05),
    storage(NULL)
{
}

} // U2
