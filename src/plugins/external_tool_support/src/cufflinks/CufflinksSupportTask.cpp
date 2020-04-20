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
#include <QDir>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/DifferentialFormat.h>
#include <U2Formats/FpkmTrackingFormat.h>
#include <U2Formats/GTFFormat.h>

#include "CufflinksSupport.h"
#include "CufflinksSupportTask.h"
#include "../ExternalToolSupportL10N.h"
#include "tophat/TopHatSettings.h"

namespace U2 {

const QString CufflinksSupportTask::outSubDirBaseName("cufflinks_out");

CufflinksSupportTask::CufflinksSupportTask(const CufflinksSettings& _settings)
    : ExternalToolSupportTask(tr("Running Cufflinks task"), TaskFlags_NR_FOSE_COSC),
      settings(_settings),
      tmpDoc(NULL),
      convertAssToSamTask(NULL),
      cufflinksExtToolTask(NULL),
      loadIsoformAnnotationsTask(NULL)
{
    GCOUNTER(cvar, tvar, "NGS:CufflinksTask");
}

CufflinksSupportTask::~CufflinksSupportTask()
{
    delete tmpDoc;
}

QString CufflinksSupportTask::initTmpDir() {
    // Add a new subdirectory for temporary files
    QString tmpDirName = "Cufflinks_"+QString::number(this->getTaskId())+"_"+
        QDate::currentDate().toString("dd.MM.yyyy")+"_"+
        QTime::currentTime().toString("hh.mm.ss.zzz")+"_"+
        QString::number(QCoreApplication::applicationPid())+"/";

    QString cufflinksTmpDirName =
        AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(CufflinksSupport::CUFFLINKS_TMP_DIR);

    QDir tmpDir(cufflinksTmpDirName + "/" + tmpDirName);

    if (tmpDir.exists()) {
        foreach (const QString& file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }

        if (!tmpDir.rmdir(tmpDir.absolutePath())){
            stateInfo.setError(ExternalToolSupportL10N::errorRemovingTmpSubdir(tmpDir.absolutePath()));
            return "";
        }
    }

    if (!tmpDir.mkpath(tmpDir.absolutePath())) {
        stateInfo.setError(ExternalToolSupportL10N::errorCreatingTmpSubrir(tmpDir.absolutePath()));
        return "";
    }
    return tmpDir.absolutePath();
}

void CufflinksSupportTask::prepare()
{
    settings.outDir = GUrlUtils::createDirectory(
        settings.outDir + "/" + outSubDirBaseName,
        "_", stateInfo);
    CHECK_OP(stateInfo, );

    workingDirectory = initTmpDir();
    CHECK_OP(stateInfo, );

    if (settings.fromFile) {
        cufflinksExtToolTask = runCufflinks();
        addSubTask(cufflinksExtToolTask);
        return;
    }

    settings.url = workingDirectory + "/tmp.sam";
    DocumentFormat* docFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::SAM);
    tmpDoc = docFormat->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(settings.url), stateInfo);
    CHECK_OP(stateInfo, );

    QScopedPointer<AssemblyObject> assObj(
        Workflow::StorageUtils::getAssemblyObject(settings.storage, settings.assemblyId));

    if (assObj.isNull()) {
        stateInfo.setError(tr("Unable to get an assembly object."));
        return;
    }

    convertAssToSamTask = new ConvertAssemblyToSamTask(assObj->getEntityRef(), settings.url);
    addSubTask(convertAssToSamTask);
}

ExternalToolRunTask * CufflinksSupportTask::runCufflinks() {
    // Init the arguments list
    QStringList arguments;

    arguments << "--no-update-check";

    arguments << "-p" << QString::number(TopHatSettings::getThreadsCount());
    arguments << "--output-dir" << settings.outDir;

    if (!settings.referenceAnnotation.isEmpty()) {
        arguments << "-G" << settings.referenceAnnotation;
    }

    if (!settings.rabtAnnotation.isEmpty()) {
        arguments << "-g" << settings.rabtAnnotation;
    }

    arguments << "--library-type" << settings.libraryType.getLibraryTypeAsStr();

    if (!settings.maskFile.isEmpty()) {
        arguments << "-M" << settings.maskFile;
    }

    if (true == settings.multiReadCorrect) {
        arguments << "--multi-read-correct";
    }

    arguments << "--min-isoform-fraction" << QString::number(settings.minIsoformFraction);

    if (!settings.fragBiasCorrect.isEmpty()) {
        arguments << "--frag-bias-correct" << settings.fragBiasCorrect;
    }

    arguments << "--pre-mrna-fraction" << QString::number(settings.preMrnaFraction);

    arguments << settings.url;

    // Create the Cufflinks task
    ExternalToolRunTask* runTask = new ExternalToolRunTask(CufflinksSupport::ET_CUFFLINKS_ID,
        arguments,
        new ExternalToolLogParser(),
        workingDirectory);
    setListenerForTask(runTask);
    return runTask;
}


QList<Task*> CufflinksSupportTask::onSubTaskFinished(Task* subTask)
{
    QList<Task*> result;

    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return result;
    }

    if (hasError() || isCanceled()) {
        return result;
    }

    if (subTask == convertAssToSamTask) {
        cufflinksExtToolTask = runCufflinks();
        result.append(cufflinksExtToolTask);
    }

    else if (subTask == cufflinksExtToolTask) {
        ExternalToolSupportUtils::appendExistingFile(settings.outDir + "/transcripts.gtf", outputFiles);
        ExternalToolSupportUtils::appendExistingFile(settings.outDir + "/isoforms.fpkm_tracking", outputFiles);
        ExternalToolSupportUtils::appendExistingFile(settings.outDir + "/genes.fpkm_tracking", outputFiles);
        initLoadIsoformAnnotationsTask("transcripts.gtf", CufflinksOutputGtf);
        CHECK(NULL != loadIsoformAnnotationsTask, result);
        result << loadIsoformAnnotationsTask;
    }

    else if (subTask == loadIsoformAnnotationsTask) {
        QScopedPointer<Document> doc(loadIsoformAnnotationsTask->takeDocument());
        SAFE_POINT_EXT(NULL != doc, setError(L10N::nullPointerError("document with annotations")), result);
        doc->setDocumentOwnsDbiResources(false);
        foreach (GObject *object, doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE)) {
            doc->removeObject(object, DocumentObjectRemovalMode_Release);
            isoformLevelAnnotationTables << qobject_cast<AnnotationTableObject *>(object);
        }
    }

    return result;
}

DocumentFormatId CufflinksSupportTask::getFormatId(CufflinksOutputFormat format) {
    switch (format) {
    case CufflinksOutputFpkm:
        return BaseDocumentFormats::FPKM_TRACKING_FORMAT;
    case CufflinksOutputGtf:
        return BaseDocumentFormats::GTF;
    default:
        FAIL("Internal error: unexpected format of the Cufflinks output!", "");
    }
}

void CufflinksSupportTask::initLoadIsoformAnnotationsTask(const QString &fileName, CufflinksOutputFormat format) {
    const QString filePath = settings.outDir + "/" + fileName;

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(NULL != iof, setError(tr("An internal error occurred during getting annotations from a %1 output file!").arg(CufflinksSupport::ET_CUFFLINKS)), );

    QVariantMap hints;
    hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue(settings.storage->getDbiRef());

    loadIsoformAnnotationsTask = new LoadDocumentTask(getFormatId(format), filePath, iof, hints);
}

Task::ReportResult CufflinksSupportTask::report() {
    if (settings.url.isEmpty()) {
        return ReportResult_Finished;
    }

    GUrlUtils::removeDir(workingDirectory, stateInfo);
    return ReportResult_Finished;
}

QList<AnnotationTableObject *> CufflinksSupportTask::getIsoformAnnotationTables() const {
    return isoformLevelAnnotationTables;
}

const QStringList &CufflinksSupportTask::getOutputFiles() const {
    return outputFiles;
}

} // namespace U2
