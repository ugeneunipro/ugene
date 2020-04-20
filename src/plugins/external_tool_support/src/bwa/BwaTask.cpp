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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/MergeBamTask.h>

#include "BwaSupport.h"
#include "BwaTask.h"

namespace U2 {

// BwaBuildIndexTask

BwaBuildIndexTask::BwaBuildIndexTask(const QString &referencePath, const QString &indexPath, const DnaAssemblyToRefTaskSettings &settings):
    ExternalToolSupportTask("Build Bwa index", TaskFlags_NR_FOSCOE),
    referencePath(referencePath),
    indexPath(indexPath),
    settings(settings)
{
}

void BwaBuildIndexTask::prepare() {
    QStringList arguments;
    arguments.append("index");
    QString indexAlg = settings.getCustomValue(BwaTask::OPTION_INDEX_ALGORITHM, "autodetect").toString();
    if(indexAlg != "autodetect") {
        arguments.append("-a");
        arguments.append(indexAlg);
    }
    arguments.append("-p");
    arguments.append(indexPath);
    arguments.append(referencePath);
    ExternalToolRunTask *task = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new LogParser());
    setListenerForTask(task);
    addSubTask(task);
}

// BwaBuildIndexTask::LogParser
BwaBuildIndexTask::LogParser::LogParser() {
}

void BwaBuildIndexTask::LogParser::parseOutput(const QString &partOfLog) {
    ExternalToolLogParser::parseOutput(partOfLog);
}

void BwaBuildIndexTask::LogParser::parseErrOutput(const QString &partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
}

// BwaAlignTask

void cleanupTempDir(const QStringList &tempDirFiles) {
    foreach(const QString& url, tempDirFiles) {
        QFile toDelete(url);
        if (toDelete.exists(url)) {
            toDelete.remove();
        }
    }
}

BwaAlignTask::BwaAlignTask(const QString &indexPath, const QList<ShortReadSet>& shortReadSets, const QString &resultPath, const DnaAssemblyToRefTaskSettings &settings)
    : ExternalToolSupportTask("Bwa reads assembly", TaskFlags_NR_FOSCOE),
      samMultiTask(NULL),
      alignMultiTask(NULL),
      mergeTask(NULL),
      indexPath(indexPath),
      readSets(shortReadSets),
      resultPath(resultPath),
      settings(settings)
{

}

QString BwaAlignTask::getSAIPath(const QString& shortReadsUrl) {
    return QFileInfo(resultPath).absoluteDir().absolutePath() + "/" + QFileInfo(shortReadsUrl).fileName() + ".sai";
}


void BwaAlignTask::prepare() {
    if (readSets.size() == 0) {
        setError(tr("Short reads are not provided"));
        return;
    }

    settings.pairedReads = readSets.at(0).type == ShortReadSet::PairedEndReads;

    const ShortReadSet& readSet = settings.shortReadSets.at(0);
    settings.pairedReads = readSet.type == ShortReadSet::PairedEndReads;

    if (settings.pairedReads) {
        foreach(const ShortReadSet& srSet, settings.shortReadSets) {
            if (srSet.order == ShortReadSet::DownstreamMate) {
                downStreamList.append(srSet);
            } else {
                upStreamList.append(srSet);
            }
        }
        if (upStreamList.size() != downStreamList.size()) {
            setError(tr("Please, provide same number of files with downstream and upstream reads."));
        }
    }

    QList<Task*> alignTasks;
    for (int resultPartsCounter = 0; resultPartsCounter < settings.shortReadSets.size(); resultPartsCounter++) {
        QStringList arguments;
        const ShortReadSet& currentReadSet = settings.shortReadSets[resultPartsCounter];

        arguments.append("aln");

        arguments.append("-n");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_N, 0.04).toString());

        arguments.append("-o");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MAX_GAP_OPENS, 1).toString());

        arguments.append("-e");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MAX_GAP_EXTENSIONS, -1).toString());

        arguments.append("-i");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_INDEL_OFFSET, 5).toString());

        arguments.append("-d");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MAX_LONG_DELETION_EXTENSIONS, 10).toString());

        arguments.append("-l");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_SEED_LENGTH, 32).toString());

        arguments.append("-k");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MAX_SEED_DIFFERENCES, 2).toString());

        arguments.append("-m");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MAX_QUEUE_ENTRIES, 2000000).toString());

        arguments.append("-t");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_THREADS, 1).toString());

        arguments.append("-M");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MISMATCH_PENALTY, 3).toString());

        arguments.append("-O");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_GAP_OPEN_PENALTY, 11).toString());

        arguments.append("-E");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_GAP_EXTENSION_PENALTY, 4).toString());

        arguments.append("-R");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_BEST_HITS, 30).toString());

        arguments.append("-q");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_QUALITY_THRESHOLD, 0).toString());

        arguments.append("-B");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_BARCODE_LENGTH, 0).toString());

        if(settings.getCustomValue(BwaTask::OPTION_LONG_SCALED_GAP_PENALTY_FOR_LONG_DELETIONS, false).toBool()) {
            arguments.append("-L");
        }

        if(settings.getCustomValue(BwaTask::OPTION_NON_ITERATIVE_MODE, false).toBool()) {
            arguments.append("-N");
        }

        arguments.append("-f");
        arguments.append( getSAIPath(currentReadSet.url.getURLString()));
        arguments.append(indexPath);
        arguments.append(currentReadSet.url.getURLString());
        ExternalToolRunTask* alignTask = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new LogParser(), NULL);
        setListenerForTask(alignTask);
        alignTasks.append(alignTask);
    }
    alignMultiTask = new MultiTask(tr("Align reads with BWA Multitask"), alignTasks);
    addSubTask(alignMultiTask);
}

QList<Task *> BwaAlignTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    QFileInfo resultPathFileInfo(resultPath);
    if (alignMultiTask == subTask) {
        QList<Task*> samTasks;
        QList<ShortReadSet> &containerToIterate = settings.pairedReads ? downStreamList : settings.shortReadSets;
        for (int resultPartsCounter = 0; resultPartsCounter < containerToIterate.size(); resultPartsCounter++) {
            QStringList arguments;

            arguments.append(settings.pairedReads ? "sampe" : "samse");

            arguments.append("-f");

            if (containerToIterate.size() == 1) {
                arguments.append(resultPath);
            } else {
                QString pathToSort = settings.tmpDirPath + "/" + resultPathFileInfo.baseName() + QString::number(resultPartsCounter);
                urlsToMerge.append(pathToSort);
                arguments.append(pathToSort);
            }
            arguments.append(indexPath);
            if (settings.pairedReads) {
                const ShortReadSet &upSet = upStreamList[resultPartsCounter];
                const ShortReadSet &downSet = downStreamList[resultPartsCounter];
                arguments.append(getSAIPath(upSet.url.getURLString()));
                arguments.append(getSAIPath(downSet.url.getURLString()));
                arguments.append(upSet.url.getURLString());
                arguments.append(downSet.url.getURLString());
            } else {
                const ShortReadSet &currentReadsSet = containerToIterate[resultPartsCounter];
                arguments.append(getSAIPath(currentReadsSet.url.getURLString()));
                arguments.append(currentReadsSet.url.getURLString());
            }
            ExternalToolRunTask *task = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new LogParser(), NULL);
            setListenerForTask(task);
            samTasks.append(task);
        }
        samMultiTask = new MultiTask(tr("Saming reads with BWA Multitask"), samTasks);
        result.append(samMultiTask);
    }
    if (subTask == samMultiTask) {
        if (settings.shortReadSets.size() == 1 || (settings.shortReadSets.size() == 2 && settings.pairedReads)) {
            return result;
        }
        //converting SAM -> BAM
        QStringList bamUrlstoMerge;
        int i = 0;
        foreach(const QString &url, urlsToMerge) {
            QFileInfo urlToConvertFileInfo(url);
            QString convertedBamUrl = settings.tmpDirPath + "/" + resultPathFileInfo.baseName() + "_" + QString::number(i) + ".bam";
            BAMUtils::ConvertOption options(true);
            BAMUtils::convertToSamOrBam(url, convertedBamUrl, options, stateInfo);
            bamUrlstoMerge.append(convertedBamUrl);
            if (stateInfo.isCoR()) {
                cleanupTempDir(urlsToMerge);
                return result;
            }
            i++;
        }
        mergeTask = new MergeBamTask(bamUrlstoMerge, resultPathFileInfo.dir().canonicalPath(), resultPathFileInfo.baseName() + ".bam", true);
        result.append(mergeTask);
    }
    if (subTask == mergeTask) {
        //converting BAM -> SAM
        QString bamResultPath = resultPathFileInfo.dir().canonicalPath() + "/" + resultPathFileInfo.baseName() + ".bam";
        BAMUtils::ConvertOption options(false);
        BAMUtils::convertToSamOrBam(resultPath, bamResultPath, options, stateInfo);
        cleanupTempDir(urlsToMerge);
    }

    return result;
}


// BwaAlignTask::LogParser
BwaAlignTask::LogParser::LogParser() {
}

void BwaAlignTask::LogParser::parseOutput(const QString &partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
}

void BwaAlignTask::LogParser::parseErrOutput(const QString &partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
    QStringList log = lastPartOfLog;
    QStringList::iterator i = log.begin();
    for (; i!=log.end(); i++) {
        if(i->contains("This application has requested the Runtime to terminate")) {
            QStringList errors;
            for (int strings = 0; strings < 2; i++, strings++) {
                SAFE_POINT(i != log.end(), tr("Log is incomplete"), );
                errors << *i;
            }
            SAFE_POINT(i == log.end(), tr("Log is incorrect"), );
            setLastError(errors.join(" "));
        } else if (i->contains("Abort!")) {
            setLastError(*i);
        } else if (i->contains("[E::")) {
            setLastError(*i);
        }
    }
}


// BwaMemAlignTask

BwaMemAlignTask::BwaMemAlignTask(const QString &indexPath, const DnaAssemblyToRefTaskSettings &settings)
    : ExternalToolSupportTask("BWA MEM reads assembly", TaskFlags_NR_FOSCOE),
      alignMultiTask(NULL),
      mergeTask(NULL),
      indexPath(indexPath),
      resultPath(settings.resultFileName.getURLString()),
      settings(settings)
{

}

void BwaMemAlignTask::prepare() {
    if (settings.shortReadSets.size() == 0) {
        setError(tr("Short reads are not provided"));
        return;
    }

    QList<ShortReadSet> downStreamList;
    QList<ShortReadSet> upStreamList;

    const ShortReadSet& readSet = settings.shortReadSets.at(0);
    settings.pairedReads = readSet.type == ShortReadSet::PairedEndReads;

    if (settings.pairedReads) {
        foreach(const ShortReadSet& srSet, settings.shortReadSets) {
            if (srSet.order == ShortReadSet::DownstreamMate) {
                downStreamList.append(srSet);
            } else {
                upStreamList.append(srSet);
            }
        }
        if (upStreamList.size() != downStreamList.size()) {
            setError(tr("Please, provide same number of files with downstream and upstream reads."));
        }
    }

    QFileInfo resultFileInfo(settings.resultFileName.getURLString());
    QList<Task*> alignTasks;
    for (int resultPartsCounter = 0, pairedReadsCounter = 0; resultPartsCounter < settings.shortReadSets.size(); resultPartsCounter++) {
        QStringList arguments;
        const ShortReadSet &currentReadSet = settings.shortReadSets[resultPartsCounter];
        arguments.append("mem");

        arguments.append("-t");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_THREADS, 1).toString());

        arguments.append("-k");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MIN_SEED, 19).toString());

        arguments.append("-w");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_BAND_WIDTH, 100).toString());

        arguments.append("-d");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_DROPOFF, 100).toString());

        arguments.append("-r");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_INTERNAL_SEED_LOOKUP, float(1.5)).toString());

        arguments.append("-c");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_SKIP_SEED_THRESHOLD, 10000).toString());

        if (settings.getCustomValue(BwaTask::OPTION_SKIP_MATE_RESCUES, false).toBool()) {
            arguments.append("-S");
        }

        if (settings.getCustomValue(BwaTask::OPTION_SKIP_PAIRING, false).toBool()) {
            arguments.append("-P");
        }

        arguments.append("-A");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MATCH_SCORE, 1).toString());

        arguments.append("-B");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_MISMATCH_PENALTY, 4).toString());

        arguments.append("-O");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_GAP_OPEN_PENALTY, 6).toString());

        arguments.append("-E");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_GAP_EXTENSION_PENALTY, 1).toString());

        arguments.append("-L");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_CLIPPING_PENALTY, 5).toString());

        arguments.append("-U");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_UNPAIRED_PENALTY, 17).toString());

        arguments.append("-T");
        arguments.append(settings.getCustomValue(BwaTask::OPTION_SCORE_THRESHOLD, 30).toString());

        arguments.append(indexPath);

        if (settings.pairedReads) {
            if (resultPartsCounter % 2 != 0) {
                continue;
            }
            arguments.append(upStreamList[pairedReadsCounter].url.getURLString());
            arguments.append(downStreamList[pairedReadsCounter].url.getURLString());
            ExternalToolRunTask* alignTask = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new BwaAlignTask::LogParser(), NULL);
            if (upStreamList.size() == 1) {
                alignTask->setStandartOutputFile(settings.resultFileName.getURLString());
            } else {
                QString resultFilePathWithpartNumber = settings.tmpDirPath + "/" + resultFileInfo.baseName() + "_" +
                    QString::number(pairedReadsCounter++) + "." + resultFileInfo.completeSuffix();
                alignTask->setStandartOutputFile(resultFilePathWithpartNumber);
            }
            setListenerForTask(alignTask);
            alignTasks.append(alignTask);
        } else if (settings.shortReadSets.size() > 1) {
            arguments.append(currentReadSet.url.getURLString());
            ExternalToolRunTask* alignTask = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new BwaAlignTask::LogParser(), NULL);
            QString resultFilePathWithpartNumber = settings.tmpDirPath + "/" + resultFileInfo.baseName() + "_" +
                QString::number(resultPartsCounter) + "." + resultFileInfo.completeSuffix();
            alignTask->setStandartOutputFile(resultFilePathWithpartNumber);
            setListenerForTask(alignTask);
            alignTasks.append(alignTask);
        } else {
            arguments.append(currentReadSet.url.getURLString());
            ExternalToolRunTask* alignTask = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new BwaAlignTask::LogParser(), NULL);
            alignTask->setStandartOutputFile(settings.resultFileName.getURLString());
            setListenerForTask(alignTask);
            alignTasks.append(alignTask);
        }
    }
    alignMultiTask = new MultiTask(tr("Align reads with BWA-MEM Multitask"), alignTasks);
    addSubTask(alignMultiTask);
}

QList<Task *> BwaMemAlignTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    QFileInfo resultFileInfo(settings.resultFileName.getURLString());
    if (alignMultiTask == subTask) {
        if (settings.shortReadSets.size() == 1 || (settings.shortReadSets.size() == 2 && settings.pairedReads)) {
            return result;
        }
        //converting SAM -> BAM
        int partsCounter = settings.pairedReads ? settings.shortReadSets.size() / 2 : settings.shortReadSets.size();
        for (int i = 0; i < partsCounter; i++) {
            QString resultFilePathWithpartNumber = settings.tmpDirPath + "/" + resultFileInfo.baseName() + "_" +
                QString::number(i) + "." + resultFileInfo.completeSuffix();
            QString bamFilePath = settings.tmpDirPath + "/" + resultFileInfo.baseName() + "_" + QString::number(i) + ".bam";
            BAMUtils::ConvertOption options(true);
            BAMUtils::convertToSamOrBam(resultFilePathWithpartNumber, bamFilePath, options, stateInfo);
            bamUrlstoMerge.append(bamFilePath);
            if (stateInfo.isCoR()) {
                cleanupTempDir(bamUrlstoMerge);
                return result;
            }
        }
        mergeTask = new MergeBamTask(bamUrlstoMerge, settings.tmpDirPath, resultFileInfo.baseName() + ".bam", true);
        result.append(mergeTask);
    }
    if (mergeTask == subTask) {
        //converting BAM -> SAM
        if (settings.cleanTmpDir) {
            cleanupTempDir(bamUrlstoMerge);
        }
        QString bamResultPath = settings.tmpDirPath + "/" + resultFileInfo.baseName() + ".bam";
        BAMUtils::ConvertOption options(false);
        BAMUtils::convertToSamOrBam(resultPath, bamResultPath, options, stateInfo);
    }
    return result;
}

// BwaSwAlignTask

BwaSwAlignTask::BwaSwAlignTask(const QString &indexPath, const DnaAssemblyToRefTaskSettings &settings):
    ExternalToolSupportTask("BWA SW reads assembly", TaskFlags_NR_FOSCOE),
    indexPath(indexPath),
    settings(settings)
{
}

void BwaSwAlignTask::prepare() {
    if (settings.shortReadSets.size() == 0) {
        setError(tr("Short reads are not provided"));
        return;
    }

    const ShortReadSet& readSet = settings.shortReadSets.at(0);


    settings.pairedReads = readSet.type == ShortReadSet::PairedEndReads;

    if (settings.pairedReads ) {
        setError(tr("BWA SW can not align paired reads"));
        return;
    }

    QStringList arguments;

    arguments.append("bwasw");

    arguments.append("-f");
    arguments.append( settings.resultFileName.getURLString() );

    arguments.append("-a");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_MATCH_SCORE, 1).toString());

    arguments.append("-b");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_MISMATCH_PENALTY, 3).toString());

    arguments.append("-q");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_GAP_OPEN_PENALTY, 5).toString());

    arguments.append("-r");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_GAP_EXTENSION_PENALTY, 2).toString());

    arguments.append("-t");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_THREADS, 1).toString());

    arguments.append("-s");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_CHUNK_SIZE, 10000000).toString());

    arguments.append("-w");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_BAND_WIDTH, 50).toString());

    arguments.append("-m");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_MASK_LEVEL, 0.5).toString());

    arguments.append("-T");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_SCORE_THRESHOLD, 30).toString());

    arguments.append("-z");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_Z_BEST, 1).toString());

    arguments.append("-N");
    arguments.append(settings.getCustomValue(BwaTask::OPTION_REV_ALGN_THRESHOLD, 5).toString());

    if (settings.getCustomValue(BwaTask::OPTION_PREFER_HARD_CLIPPING, false).toBool()) {
        arguments.append("-H");
    }

    arguments.append( indexPath );
    arguments.append( readSet.url.getURLString() );


    Task* alignTask = new ExternalToolRunTask(BwaSupport::ET_BWA_ID, arguments, new BwaAlignTask::LogParser(), NULL);
    addSubTask(alignTask);

}


// BwaTask

const QString BwaTask::taskName = "BWA";

const QString BwaTask::OPTION_INDEX_ALGORITHM = "index-algorithm";
const QString BwaTask::OPTION_N = "n";
const QString BwaTask::OPTION_MAX_GAP_OPENS = "max-gap-opens";
const QString BwaTask::OPTION_MAX_GAP_EXTENSIONS = "max-gap-extensions";
const QString BwaTask::OPTION_INDEL_OFFSET = "indel-offset";
const QString BwaTask::OPTION_MAX_LONG_DELETION_EXTENSIONS = "max-long-deletion-extensions";
const QString BwaTask::OPTION_SEED_LENGTH = "seed-length";
const QString BwaTask::OPTION_MAX_SEED_DIFFERENCES = "max-seed-differences";
const QString BwaTask::OPTION_MAX_QUEUE_ENTRIES = "max-queue-entries";
const QString BwaTask::OPTION_BEST_HITS = "best-hits";
const QString BwaTask::OPTION_QUALITY_THRESHOLD = "quality-threshold";
const QString BwaTask::OPTION_BARCODE_LENGTH = "barcode-length";
const QString BwaTask::OPTION_LONG_SCALED_GAP_PENALTY_FOR_LONG_DELETIONS = "long-scaled-gap-penalty-for-long-deletions";
const QString BwaTask::OPTION_NON_ITERATIVE_MODE = "non-iterative-mode";
const QString BwaTask::OPTION_SW_ALIGNMENT = "bwa-sw-alignment";
const QString BwaTask::OPTION_MEM_ALIGNMENT = "bwa-mem-alignment";
const QString BwaTask::OPTION_PREFER_HARD_CLIPPING = "prefer-hard-clipping";
const QString BwaTask::OPTION_REV_ALGN_THRESHOLD = "rev-algn";
const QString BwaTask::OPTION_Z_BEST = "z-best";
const QString BwaTask::OPTION_CHUNK_SIZE = "chunk-size";
const QString BwaTask::OPTION_MASK_LEVEL = "mask-level";

const QString BwaTask::OPTION_THREADS = "threads";
const QString BwaTask::OPTION_MIN_SEED = "min-seed";
const QString BwaTask::OPTION_BAND_WIDTH = "band-width";
const QString BwaTask::OPTION_DROPOFF = "dropoff";
const QString BwaTask::OPTION_INTERNAL_SEED_LOOKUP = "seed-lookup";
const QString BwaTask::OPTION_SKIP_SEED_THRESHOLD = "seed-threshold";
const QString BwaTask::OPTION_DROP_CHAINS_THRESHOLD = "drop-chains";
const QString BwaTask::OPTION_MAX_MATE_RESCUES = "mate-rescue";
const QString BwaTask::OPTION_SKIP_MATE_RESCUES = "skip-mate-rescues";
const QString BwaTask::OPTION_SKIP_PAIRING = "skip-pairing";
const QString BwaTask::OPTION_MATCH_SCORE = "match-score";
const QString BwaTask::OPTION_MISMATCH_PENALTY = "mistmatch-penalty";
const QString BwaTask::OPTION_GAP_OPEN_PENALTY = "gap-open-penalty";
const QString BwaTask::OPTION_GAP_EXTENSION_PENALTY = "gap-ext-penalty";
const QString BwaTask::OPTION_CLIPPING_PENALTY = "clipping-penalty";
const QString BwaTask::OPTION_UNPAIRED_PENALTY = "inpaired-panalty";
const QString BwaTask::OPTION_SCORE_THRESHOLD = "score-threshold";

const QString BwaTask::ALGORITHM_BWA_SW = "BWA-SW";
const QString BwaTask::ALGORITHM_BWA_ALN = "BWA";
const QString BwaTask::ALGORITHM_BWA_MEM = "BWA-MEM";

const QStringList BwaTask::indexSuffixes = QStringList() << ".amb" << ".ann" << ".bwt" << ".pac" << ".sa";

BwaTask::BwaTask(const DnaAssemblyToRefTaskSettings &settings, bool justBuildIndex):
    DnaAssemblyToReferenceTask(settings, TaskFlags_NR_FOSCOE, justBuildIndex),
    buildIndexTask(NULL),
    alignTask(NULL)
{
    GCOUNTER(cvar, tvar, "NGS:BWATask");
}

void BwaTask::prepare() {
    if (!justBuildIndex) {
        setUpIndexBuilding(indexSuffixes);
    }
    QString indexFileName = settings.indexFileName;
    if (indexFileName.isEmpty()) {
        indexFileName = settings.refSeqUrl.getURLString();
    }

    if (!settings.prebuiltIndex) {
        buildIndexTask = new BwaBuildIndexTask(settings.refSeqUrl.getURLString(), indexFileName, settings);
        buildIndexTask->addListeners(QList <ExternalToolListener*>() << getListener(0));
    }

    int upStreamCount = 0;
    int downStreamCount = 0;
    foreach(const ShortReadSet& srSet, settings.shortReadSets) {
        if (srSet.order == ShortReadSet::DownstreamMate) {
            downStreamCount++;
        }
        else {
            upStreamCount++;
        }
    }

    if(!justBuildIndex) {
        if (settings.getCustomValue(OPTION_SW_ALIGNMENT, false) == true) {
            if(settings.shortReadSets.size() > 1) {
                setError(tr("Multiple read files are not supported by bwa-sw. Please combine your reads into single FASTA file."));
                return;
            }
            alignTask = new BwaSwAlignTask(indexFileName, settings);
            alignTask->addListeners(QList <ExternalToolListener*>() << getListener(1));
        }
        else  if (settings.getCustomValue(OPTION_MEM_ALIGNMENT, false) == true) {
            if (downStreamCount != upStreamCount && settings.pairedReads) {
                setError(tr("Please, provide same number of files with downstream and upstream reads."));
                return;
            }

            alignTask = new BwaMemAlignTask(indexFileName, settings);
            alignTask->addListeners(QList <ExternalToolListener*>() << getListener(1));
        }
        else{
            alignTask = new BwaAlignTask(indexFileName, settings.shortReadSets, settings.resultFileName.getURLString(), settings);
            alignTask->addListeners(QList <ExternalToolListener*>() << getListener(1));
        }
    }

    if (!settings.prebuiltIndex) {
        addSubTask(buildIndexTask);
    }
    else if(!justBuildIndex) {
        addSubTask(alignTask);
    }
    else {
        assert(false);
    }
}

Task::ReportResult BwaTask::report() {
    if(!justBuildIndex) {
        hasResults = true;
    }
    return ReportResult_Finished;
}

QList<Task *> BwaTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    if((subTask == buildIndexTask) && !justBuildIndex) {
        result.append(alignTask);
    }
    return result;
}

// BwaTaskFactory

DnaAssemblyToReferenceTask *BwaTaskFactory::createTaskInstance(const DnaAssemblyToRefTaskSettings &settings, bool justBuildIndex) {
    return new BwaTask(settings, justBuildIndex);
}
} // namespace U2
