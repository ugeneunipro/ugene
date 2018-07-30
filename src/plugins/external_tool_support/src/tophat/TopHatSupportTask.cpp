/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QCoreApplication>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/BedFormat.h>

#include <U2Lang/DbiDataStorage.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowTasksRegistry.h>

#include "ExternalToolSupportL10N.h"
#include "TopHatSupportTask.h"
#include "python/PythonSupport.h"

namespace U2 {

const QString TopHatSupportTask::outSubDirBaseName("tophat_out");

TopHatSupportTask::TopHatSupportTask(const TopHatSettings& _settings)
    : ExternalToolSupportTask(tr("Running TopHat task"), TaskFlags_NR_FOSE_COSC),
      settings(_settings),
      tmpDoc(NULL),
      tmpDocPaired(NULL),
      topHatExtToolTask(NULL),
      tmpDocSaved(false),
      tmpDocPairedSaved(false),
      bowtieIndexTask(NULL)
{
    GCOUNTER(cvar, tvar, "NGS:TopHatTask");
}


TopHatSupportTask::~TopHatSupportTask()
{
    delete tmpDoc;
    delete tmpDocPaired;
}

QString TopHatSupportTask::setupTmpDir() {
    // Add a new subdirectory for temporary files
    QString tmpDirName = "TopHat_" + QString::number(this->getTaskId()) + "_" +
        QDate::currentDate().toString("dd.MM.yyyy") + "_" +
        QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
        QString::number(QCoreApplication::applicationPid()) + "/";

    QString topHatTmpDirName =
        AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(TOPHAT_TMP_DIR);

    // Create the tmp dir
    QDir tmpDir(topHatTmpDirName + "/" + tmpDirName);

    if (tmpDir.exists()) {
        foreach (const QString& file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }

        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
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

ExternalToolSupportTask *TopHatSupportTask::createIndexTask() {
    if (settings.referenceInputType == TopHatSettings::SEQUENCE) {
        QFileInfo referenceGenome(settings.referenceGenome);
        QFileInfo outDir(settings.outDir);

        QDir indexDir(outDir.absolutePath() + "/");
        if (settings.useBowtie1) {
            indexDir = outDir.absolutePath() + "/bowtie1_index/";
        } else {
            indexDir = outDir.absolutePath() + "/bowtie2_index/";
        }
        if (!indexDir.exists()) {
            if (!indexDir.mkpath(indexDir.absolutePath())) {
                stateInfo.setError(tr("Can't create directory for index files "));
                bowtieIndexTask = NULL;
                return NULL;
            }
        }
        settings.buildIndexPathAndBasename = indexDir.absolutePath() + "/" + referenceGenome.baseName();
        if (settings.useBowtie1) {
            bowtieIndexTask = new BowtieBuildIndexTask(referenceGenome.absoluteFilePath(),
                                                       settings.buildIndexPathAndBasename,
                                                       false);
        } else {
            bowtieIndexTask = new Bowtie2BuildIndexTask(referenceGenome.absoluteFilePath(),
                                                         settings.buildIndexPathAndBasename);
        }
        settings.bowtieIndexPathAndBasename = settings.buildIndexPathAndBasename;

        return bowtieIndexTask;
    }
    return NULL;
}

void TopHatSupportTask::prepare() {
    settings.outDir = GUrlUtils::createDirectory(
        settings.outDir + "/" + outSubDirBaseName,
        "_", stateInfo);
    CHECK_OP(stateInfo, );

    workingDirectory = setupTmpDir();
    CHECK_OP(stateInfo, );

    ExternalToolSupportTask *indexTask = createIndexTask();
    CHECK_OP(stateInfo, );
    if (indexTask != NULL) {
        addSubTask(indexTask);
        return;
    }

    if (settings.data.fromFiles) {
        topHatExtToolTask = runTophat();
        addSubTask(topHatExtToolTask);
        return;
    }

    /************************************************************************/
    /* Create save tasks */
    /************************************************************************/
    QString url = workingDirectory + "/tmp_1.fq";
    settings.data.urls << url;
    saveTmpDocTask = createSaveTask(url, tmpDoc, settings.data.seqIds);
    addSubTask(saveTmpDocTask);

    if (settings.data.paired) {
        QString pairedUrl = workingDirectory + "/tmp_2.fq";
        settings.data.pairedUrls << pairedUrl;
        savePairedTmpDocTask = createSaveTask(pairedUrl, tmpDocPaired, settings.data.pairedSeqIds);
        addSubTask(savePairedTmpDocTask);
    }
}

SaveDocumentTask * TopHatSupportTask::createSaveTask(const QString &url, QPointer<Document> &doc, const QList<Workflow::SharedDbiDataHandler> &seqs) {
    DocumentFormat* docFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTQ);
    doc = docFormat->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(url), stateInfo);
    CHECK_OP(stateInfo, NULL);
    doc->setDocumentOwnsDbiResources(false);

    // Add all sequence objects to the document
    foreach (Workflow::SharedDbiDataHandler seqId, seqs) {
        U2SequenceObject* seqObj(Workflow::StorageUtils::getSequenceObject(settings.storage(), seqId));

        if (NULL == seqObj) {
            stateInfo.setError(tr("An unexpected error has occurred during preparing the TopHat task!"));
            taskLog.trace(tr("Preparing TopHatSupportTask internal error: unable to get a sequence object!"));
            return NULL;
        }

        doc->addObject(seqObj);
    }

    return new SaveDocumentTask(doc, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE), url);
}

void addOptionIfBoolParamIsSet(QStringList& arguments, bool optionIsSet, const QString& optionName) {
    if (optionIsSet) {
        arguments << optionName;
    }
}

void addOptionIfStringParamIsSet(QStringList& arguments, const QString& strValue, const QString& optionName) {
    if (!strValue.isEmpty()) {
        arguments << optionName << strValue;
    }
}

ExternalToolRunTask * TopHatSupportTask::runTophat() {
    // Init the arguments list
    QStringList arguments;

    arguments << "-p" << QString::number(TopHatSettings::getThreadsCount());
    arguments << "--output-dir" << settings.outDir;
    arguments << "--mate-inner-dist" << QString::number(settings.mateInnerDistance);
    arguments << "--mate-std-dev" << QString::number(settings.mateStandardDeviation);
    arguments << "--library-type" << settings.libraryType.getLibraryTypeAsStr();

    addOptionIfBoolParamIsSet(arguments, settings.noNovelJunctions, "--no-novel-juncs");

    addOptionIfStringParamIsSet(arguments, settings.rawJunctions, "--raw-juncs");
    addOptionIfStringParamIsSet(arguments, settings.knownTranscript, "-G");

    arguments << "--max-multihits" << QString::number(settings.maxMultihits);
    arguments << "--segment-length" << QString::number(settings.segmentLength);

    addOptionIfBoolParamIsSet(arguments, settings.fusionSearch, "--fusion-search");
    addOptionIfBoolParamIsSet(arguments, settings.transcriptomeOnly, "--transcriptome-only");

    arguments << "--transcriptome-max-hits" << QString::number(settings.transcriptomeMaxHits);

    addOptionIfBoolParamIsSet(arguments, settings.prefilterMultihits, "--prefilter-multihits");

    arguments << "--min-anchor-length" << QString::number(settings.minAnchorLength);
    arguments << "--splice-mismatches" << QString::number(settings.spliceMismatches);
    arguments << "--read-mismatches" << QString::number(settings.readMismatches);
    arguments << "--segment-mismatches" << QString::number(settings.segmentMismatches);

    addOptionIfBoolParamIsSet(arguments, settings.solexa13quals, "--solexa1.3-quals");

    if (settings.bowtieMode == nMode) {
        arguments << "--bowtie-n";
    }

    addOptionIfBoolParamIsSet(arguments, settings.useBowtie1, "--bowtie1");

    // Index base and reads
    arguments << settings.bowtieIndexPathAndBasename;
    arguments << settings.data.urls.join(",");
    if (settings.data.paired) {
        SAFE_POINT(settings.data.urls.size() == settings.data.pairedUrls.size(), "Not equal files count", NULL);
        arguments << settings.data.pairedUrls.join(",");
    }

    // Add Bowtie, samtools an python to the PATH environment variable
    QStringList additionalPaths;
    ExternalTool *pythonTool = AppContext::getExternalToolRegistry()->getByName(ET_PYTHON);
    if (NULL != pythonTool) {
        additionalPaths << QFileInfo(pythonTool->getPath()).dir().absolutePath();
    }

    additionalPaths << QFileInfo(settings.bowtiePath).dir().absolutePath();
    additionalPaths << QFileInfo(settings.samtoolsPath).dir().absolutePath();

    ExternalToolRunTask* runTask = new ExternalToolRunTask(ET_TOPHAT,
        arguments,
        new ExternalToolLogParser(),
        workingDirectory,
        additionalPaths);
    setListenerForTask(runTask);
    return runTask;
}

QList<Task*> TopHatSupportTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;

    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return result;
    }

    if (hasError() || isCanceled()) {
        return result;
    }

    if (subTask == saveTmpDocTask || subTask == savePairedTmpDocTask) {
        if (subTask == saveTmpDocTask) {
            tmpDocSaved = true;
        }

        if (subTask == savePairedTmpDocTask) {
            tmpDocPairedSaved = true;
        }

        if (tmpDocSaved && (tmpDocPairedSaved || settings.data.pairedSeqIds.isEmpty())) {
            if (settings.referenceInputType == TopHatSettings::SEQUENCE) {
                ExternalToolSupportTask *indexTask = createIndexTask();
                CHECK_OP(stateInfo, result);
                if (indexTask != NULL) {
                    result.append(indexTask);
                }
            } else {
                topHatExtToolTask = runTophat();
                result.append(topHatExtToolTask);
            }
        }
    } else if (subTask == bowtieIndexTask) {
        settings.bowtieIndexPathAndBasename = settings.buildIndexPathAndBasename;
        topHatExtToolTask = runTophat();
        result.append(topHatExtToolTask);
    } else if (subTask == topHatExtToolTask) {
        registerOutputFiles();
        renameOutputFiles();
        if (!QFile::exists(outputFiles.value(ACCEPTED_HITS))) {
            setError(tr("TopHat was not able to map reads to the reference."));
            return result;
        }

        // Get assembly output
        Workflow::WorkflowTasksRegistry* registry = Workflow::WorkflowEnv::getWorkflowTasksRegistry();
        SAFE_POINT(NULL != registry, "Internal error during parsing TopHat output: NULL WorkflowTasksRegistry", result);
        Workflow::ReadDocumentTaskFactory* factory = registry->getReadDocumentTaskFactory(Workflow::ReadFactories::READ_ASSEMBLY);
        SAFE_POINT(NULL != factory, QString("Internal error during parsing TopHat output:"
                                            " NULL WorkflowTasksRegistry: %1").arg(Workflow::ReadFactories::READ_ASSEMBLY), result);
        SAFE_POINT(NULL != settings.workflowContext(), "Internal error during parsing TopHat output: NULL workflow context!", result);

        readAssemblyOutputTask = factory->createTask(outputFiles.value(ACCEPTED_HITS), QVariantMap(), settings.workflowContext());
        result.append(readAssemblyOutputTask);
    } else if (subTask == readAssemblyOutputTask) {
        Workflow::ReadDocumentTask* readDocTask = qobject_cast<Workflow::ReadDocumentTask*>(subTask);
        SAFE_POINT(NULL != readDocTask, "Internal error during parsing TopHat output: NULL read document task!", result);

        QList<Workflow::SharedDbiDataHandler> acceptedHitsResults;
        acceptedHitsResults = readDocTask->takeResult();

        if (acceptedHitsResults.isEmpty()) {
            setError(tr("There are no accepted hits in the result"));
        } else {
            // FIXME: Currently only the first assembly object is taken into account
            acceptedHits = acceptedHitsResults[0];
        }
    }

    return result;
}

bool removeTmpDir(QString dirName)
{
    bool result = true;
    QDir tmpDir(dirName);
    if (tmpDir.exists()) {
        foreach (QFileInfo info, tmpDir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir()) {
                result = removeTmpDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }

        result = tmpDir.rmdir(dirName);
    }

    return result;
}


Task::ReportResult TopHatSupportTask::report()
{
    if (workingDirectory.isEmpty()) {
        return ReportResult_Finished;
    }

    removeTmpDir(workingDirectory);
    return ReportResult_Finished;
}

QStringList TopHatSupportTask::getOutputFiles() const {
    return outputFiles.values();
}

QString TopHatSupportTask::getOutBamUrl() const {
    return outputFiles.value(ACCEPTED_HITS, "");
}

QString TopHatSupportTask::getSampleName() const {
    return settings.sample;
}

void TopHatSupportTask::registerOutputFile(FileRole role, const QString &url) {
    outputFiles.insert(role, QFile::exists(url) ? url : "");
}

void TopHatSupportTask::registerOutputFiles() {
    registerOutputFile(ACCEPTED_HITS, settings.outDir + "/accepted_hits.bam");
    registerOutputFile(JUNCTIONS, settings.outDir + "/junctions.bed");
    registerOutputFile(INSERTIONS, settings.outDir + "/insertions.bed");
    registerOutputFile(DELETIONS, settings.outDir + "/deletions.bed");
}

void TopHatSupportTask::renameOutputFile(TopHatSupportTask::FileRole role, const QString &newUrl) {
    const QString oldUrl = outputFiles.value(role, "");
    CHECK(!oldUrl.isEmpty(), );
    const bool copied = QFile::copy(oldUrl, newUrl);
    CHECK(copied, );
    outputFiles[role] = newUrl;
}

void TopHatSupportTask::renameOutputFiles() {
    CHECK(!settings.sample.isEmpty(), );
    renameOutputFile(ACCEPTED_HITS, settings.outDir + "/" + GUrlUtils::rollFileName(GUrlUtils::fixFileName(settings.sample + ".bam"), "_"));
    renameOutputFile(JUNCTIONS, settings.outDir + "/" + GUrlUtils::rollFileName(GUrlUtils::fixFileName(settings.sample + "_junctions.bed"), "_"));
    renameOutputFile(INSERTIONS, settings.outDir + "/" + GUrlUtils::rollFileName(GUrlUtils::fixFileName(settings.sample + "_insertions.bed"), "_"));
    renameOutputFile(DELETIONS, settings.outDir + "/" + GUrlUtils::rollFileName(GUrlUtils::fixFileName(settings.sample + "_deletions.bed"), "_"));
}

}
