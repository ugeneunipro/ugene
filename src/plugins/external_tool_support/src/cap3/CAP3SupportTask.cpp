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

#include "CAP3SupportTask.h"
#include "CAP3Support.h"

#include <U2Core/AppContext.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/CopyDataTask.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DNAQualityIOUtils.h>

#include <U2Gui/OpenViewTask.h>

namespace U2 {


//////////////////////////////////////////////////////////////////////////
////CAP3SupportTask

CAP3SupportTask::CAP3SupportTask(const CAP3SupportTaskSettings& _settings) :
    ExternalToolSupportTask("CAP3SupportTask", TaskFlags_NR_FOSE_COSC),
    prepareDataForCAP3Task(NULL),
    cap3Task(NULL),
    copyResultTask(NULL),
    settings(_settings)
{
    GCOUNTER( cvar, tvar, "CAP3SupportTask" );
    setMaxParallelSubtasks(1);
}


void CAP3SupportTask::prepare(){
    tmpDirUrl = ExternalToolSupportUtils::createTmpDir(CAP3Support::CAP3_TMP_DIR, stateInfo);
    CHECK_OP(stateInfo, );

    prepareDataForCAP3Task = new PrepareInputForCAP3Task(settings.inputFiles, tmpDirUrl);
    addSubTask(prepareDataForCAP3Task);
}


#define CAP3_EXT ".cap.ace"

QList<Task*> CAP3SupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    propagateSubtaskError();

    if(hasError() || isCanceled()) {
        return res;
    }

    if (subTask == prepareDataForCAP3Task) {
        assert(!prepareDataForCAP3Task->getPreparedPath().isEmpty());
        GUrl inputUrl = prepareDataForCAP3Task->getPreparedPath();
        tmpOutputUrl = inputUrl.getURLString()+ CAP3_EXT;

        QStringList arguments = settings.getArgumentsList();
        arguments.prepend( inputUrl.getURLString() );
        cap3Task = new ExternalToolRunTask(CAP3Support::ET_CAP3_ID, arguments, new CAP3LogParser());
        setListenerForTask(cap3Task);
        cap3Task->setSubtaskProgressWeight(95);
        res.append(cap3Task);
    }
    else if (subTask == cap3Task) {
        if (!QFile::exists(tmpOutputUrl)) {
            if(AppContext::getExternalToolRegistry()->getById(CAP3Support::ET_CAP3_ID)->isValid()){
                stateInfo.setError(tr("Output file not found"));
            }else{
                stateInfo.setError(tr("Output file not found. May be %1 tool path '%2' not valid?")
                                   .arg(AppContext::getExternalToolRegistry()->getById(CAP3Support::ET_CAP3_ID)->getName())
                                   .arg(AppContext::getExternalToolRegistry()->getById(CAP3Support::ET_CAP3_ID)->getPath()));
            }
            return res;
        }

        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        copyResultTask = new CopyDataTask(iof, tmpOutputUrl, iof, settings.outputFilePath);
        res.append(copyResultTask);

    }
    else if (subTask == copyResultTask) {
        if( !QFile::exists(settings.outputFilePath)) {
            stateInfo.setError(tr("Output file not found: copy from tmp dir failed."));
            return res;
        }
        outputFile = settings.outputFilePath;
    }
    return res;
}

QString CAP3SupportTask::getOutputFile() const {
    return outputFile;
}

Task::ReportResult CAP3SupportTask::report() {
    U2OpStatus2Log os;
    ExternalToolSupportUtils::removeTmpDir(tmpDirUrl,os);
    return ReportResult_Finished;
}

//////////////////////////////////////////
////RunCap3AndOpenResultTask
RunCap3AndOpenResultTask::RunCap3AndOpenResultTask(const CAP3SupportTaskSettings &settings) :
    Task(tr("CAP3 run and open result task"), TaskFlags_NR_FOSE_COSC),
    cap3Task(new CAP3SupportTask(settings)),
    openView(settings.openView)
{
    GCOUNTER( cvar, tvar, "RunCap3AndOpenResultTask" );
    cap3Task->setSubtaskProgressWeight(95);
}

void RunCap3AndOpenResultTask::prepare() {
    SAFE_POINT_EXT(cap3Task, setError(tr("Invalid CAP3 task")), );
    addSubTask(cap3Task);
}

QList<Task*> RunCap3AndOpenResultTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> subTasks;

    if (subTask->isCanceled() || subTask->hasError()) {
        return subTasks;
    }


    if (subTask == cap3Task) {
        GUrl url(cap3Task->getOutputFile());

        ProjectLoader* loader = AppContext::getProjectLoader();
        SAFE_POINT_EXT(loader, setError(tr("Project loader is NULL")), subTasks);
        QVariantMap hints;
        hints[ProjectLoaderHint_LoadWithoutView] = !openView;
        Task *loadTask = loader->openWithProjectTask(url, hints);
        if (NULL != loadTask) {
            subTasks << loadTask;
        }
    }

    return subTasks;
}

//////////////////////////////////////////
////CAP3LogParser

CAP3LogParser::CAP3LogParser() {
}

int CAP3LogParser::getProgress() {
    return 0;
}


//////////////////////////////////////////
////PrepareInput

PrepareInputForCAP3Task::PrepareInputForCAP3Task( const QStringList& inputFiles, const QString& outputDirPath)
:Task("PrepareInputForCAP3Task", TaskFlags_FOSCOE), inputUrls(inputFiles), outputDir(outputDirPath), onlyCopyFiles(false)
{
}

void PrepareInputForCAP3Task::prepare() {
    if (inputUrls.size() == 1) {
        const QString& inputFileUrl = inputUrls.first();

        QList<FormatDetectionResult> results = DocumentUtils::detectFormat(inputFileUrl);

        if (!results.isEmpty()) {
            DocumentFormat* format = results.first().format;
            if (format->getFormatId() == BaseDocumentFormats::FASTA) {
                onlyCopyFiles = true;
            }
        }
    }

    if (onlyCopyFiles) {
        // Short path: copy single FASTA file along with quality and constraints to target dir
        QString inputFileUrl = inputUrls.first();
        filesToCopy.append(inputFileUrl);
        QString inputFileUrlBase = GUrl(inputFileUrl).baseFileName();
        QString inputFileDir = GUrl(inputFileUrl).dirPath();
        QString qualFileUrl = inputFileDir + "/" + inputFileUrlBase + ".qual";
        if (QFile::exists(qualFileUrl)) {
            filesToCopy.append(qualFileUrl);
        }
        QString constraintsFileUrl = inputFileDir+ "/" + inputFileUrlBase + ".con";
        if (QFile::exists(constraintsFileUrl)) {
            filesToCopy.append(qualFileUrl);
        }
        foreach (const QString& fileName, filesToCopy) {
            IOAdapterFactory* iof =
                AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
            CopyDataTask* copyTask = new CopyDataTask(iof, fileName, iof, outputDir + "/" + GUrl(fileName).fileName());
            addSubTask(copyTask);
        }
        preparedPath = outputDir + "/" + GUrl(inputFileUrl).fileName();
    }
    else {
        // Long path: load each file, save sequences and qualities to output dir
        QList<GUrl> inputGUrls;
        foreach( const QString& url, inputUrls) {
            inputGUrls.append(url);
        }

        if (!seqReader.init(inputGUrls)) {
            setError(seqReader.getErrorMessage());
            return;
        }

        QString outPath = outputDir + "/" + QString("%1_misc").arg(inputGUrls.first().baseFileName());
        qualityFilePath = outPath + ".qual";

        if (!seqWriter.init(outPath + ".fa")) {
            setError(tr("Failed to initialize sequence writer."));
            return;
        }
    }
}

void PrepareInputForCAP3Task::run()
{
    if (hasError() || onlyCopyFiles) {
        return;
    }

    while (seqReader.hasNext()) {
        if (isCanceled()) {
            return;
        }
        DNASequence* seq = seqReader.getNextSequenceObject();
        if (seq == NULL) {
            setError(seqReader.getErrorMessage());
            return;
        }
        // avoid names duplication
        QByteArray seqName = seq->getName().toLatin1();
        seqName.replace(' ','_');
        seq->setName(seqName);
        bool ok = seqWriter.writeNextSequence(*seq);
        if (!ok) {
            setError(tr("Failed to write sequence %1").arg(seq->getName()));
            return;
        }

        if (!seq->quality.isEmpty()) {
            DNAQualityIOUtils::writeDNAQuality(seqName, seq->quality, qualityFilePath, true /*append*/, true /*decode*/, stateInfo );
            if (stateInfo.hasError()) {
                return;
            }
        }
    }
    preparedPath = seqWriter.getOutputPath().getURLString();
    seqWriter.close();
}

QStringList CAP3SupportTaskSettings::getArgumentsList()
{
    QStringList res;

    res += "-a"; res += QString("%1").arg(bandExpansionSize);
    res += "-b"; res += QString("%1").arg(baseQualityDiffCutoff);
    res += "-c"; res += QString("%1").arg(baseQualityClipCutoff);
    res += "-d"; res += QString("%1").arg(maxQScoreSum);
    res += "-f"; res += QString("%1").arg(maxGapLength);
    res += "-g"; res += QString("%1").arg(gapPenaltyFactor);
    res += "-m"; res += QString("%1").arg(matchScoreFactor);
    res += "-n"; res += QString("%1").arg(mismatchScoreFactor);
    res += "-o"; res += QString("%1").arg(overlapLengthCutoff);
    res += "-p"; res += QString("%1").arg(overlapPercentIdentityCutoff);
    res += "-r"; res += QString("%1").arg((int)reverseReads);
    res += "-s"; res += QString("%1").arg(overlapSimilarityScoreCutoff);
    res += "-t"; res += QString("%1").arg(maxNumberOfWordMatches);
    res += "-y"; res += QString("%1").arg(clippingRange);

    return res;
}

}//namespace
