/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or * modify it under the terms of the GNU General Public License
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

#include "BowtieTask.h"

#include <QFileInfo>

#include <U2Core/AppResources.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>

#include <U2Formats/BgzipTask.h>

#include "BowtieSupport.h"

namespace U2 {

// BowtieBuildTask

BowtieBuildTask::BowtieBuildTask(const QString& referencePath, const QString& indexPath)
    : ExternalToolSupportTask("Bowtie build", TaskFlags_NR_FOSCOE),
      referencePath(referencePath),
      indexPath(indexPath) {
}

void BowtieBuildTask::prepare() {
    {
        QFileInfo file(referencePath);
        if (!file.exists()) {
            stateInfo.setError(tr("Reference file \"%1\" does not exist").arg(referencePath));
            return;
        }
        qint64 memUseMB = file.size() * 3 / 1024 / 1024 + 100;
        coreLog.trace(QString("bowtie-build:Memory resource %1").arg(memUseMB));
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB));
    }

    QStringList arguments;
    arguments.append(referencePath);
    arguments.append(indexPath);

    auto task = new ExternalToolRunTask(BowtieSupport::ET_BOWTIE_BUILD_ID, arguments, new LogParser());
    setListenerForTask(task);
    addSubTask(task);
}

// BowtieBuildIndexTask::LogParser
BowtieBuildTask::LogParser::LogParser()
    : stage(PREPARE),
      substage(UNKNOWN),
      bucketSortIteration(0),
      blockIndex(0),
      blockCount(0),
      substageProgress(0),
      progress(0) {
}

void BowtieBuildTask::LogParser::parseOutput(const QString& partOfLog) {
    ExternalToolLogParser::parseOutput(partOfLog);
    foreach (const QString& buf, lastPartOfLog) {
        QRegExp blockRegExp("Getting block (\\d+) of (\\d+)");
        QRegExp percentRegexp("(\\d+)%");
        if (buf.contains("Binary sorting into buckets")) {
            if (substage != BUCKET_SORT) {
                bucketSortIteration = 0;
                substage = BUCKET_SORT;
                if (stage == PREPARE) {
                    stage = FORWARD_INDEX;
                } else if (stage == FORWARD_INDEX) {
                    stage = MIRROR_INDEX;
                }
            } else {
                bucketSortIteration++;
            }
            substageProgress = bucketSortIteration * 30 / 5;
        } else if (buf.contains(blockRegExp)) {
            substage = GET_BLOCKS;
            blockIndex = blockRegExp.cap(1).toInt() - 1;
            blockCount = blockRegExp.cap(2).toInt();
            substageProgress = 30 + blockIndex * 70 / blockCount;
        } else if (buf.contains(percentRegexp)) {
            int percent = percentRegexp.cap(1).toInt();
            if (substage == BUCKET_SORT) {
                substageProgress = (bucketSortIteration * 30 + percent * 30 / 100) / 5;
            } else if (substage == GET_BLOCKS) {
                substageProgress = 30 + (blockIndex * 70 + percent * 70 / 100) / blockCount;
            }
        }
        if (stage == FORWARD_INDEX) {
            progress = substageProgress / 2;
        } else if (stage == MIRROR_INDEX) {
            progress = 50 + substageProgress / 2;
        } else {
            progress = 0;
        }
    }
}

void BowtieBuildTask::LogParser::parseErrOutput(const QString& partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
}

int BowtieBuildTask::LogParser::getProgress() {
    return progress;
}

// BowtieAlignTask

BowtieAlignTask::BowtieAlignTask(const DnaAssemblyToRefTaskSettings& settings)
    : ExternalToolSupportTask("Bowtie align", TaskFlags_NR_FOSCOE),
      logParser(nullptr),
      settings(settings) {
}

bool BowtieAlignTask::hasResult() const {
    CHECK(logParser != nullptr, false);
    return logParser->hasResult();
}

void BowtieAlignTask::prepare() {
    {
        QString indexSuffixes[] = {".1.ebwt", ".2.ebwt", ".3.ebwt", ".4.ebwt", ".rev.1.ebwt", ".rev.2.ebwt"};

        if (settings.indexFileName.isEmpty()) {
            if (settings.prebuiltIndex) {
                QString indexName = QFileInfo(settings.refSeqUrl.getURLString()).fileName();
                for (int i = 0; i < 6; ++i) {
                    indexName.remove(indexSuffixes[i]);
                }
                settings.indexFileName = settings.refSeqUrl.dirPath() + "/" + indexName;
            } else {
                settings.indexFileName = settings.resultFileName.dirPath() + "/" + settings.resultFileName.baseFileName();
            }
        }

        for (int i = 0; i < 6; i++) {
            QFileInfo file(settings.indexFileName + indexSuffixes[i]);
            if (!file.exists()) {
                stateInfo.setError(tr("Reference index file \"%1\" does not exist").arg(settings.indexFileName + indexSuffixes[i]));
                return;
            }
        }

        static const int SHORT_READ_AVG_LENGTH = 1000;
        QFileInfo file(settings.indexFileName + indexSuffixes[0]);
        qint64 memUseMB = (file.size() * 4 + SHORT_READ_AVG_LENGTH * 10) / 1024 / 1024 + 100;
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB, false));
    }

    QStringList arguments;
    arguments.append(QString("-n"));
    arguments.append(settings.getCustomValue(BowtieTask::OPTION_N_MISMATCHES, 2).toString());
    {
        int vMismatches = settings.getCustomValue(BowtieTask::OPTION_V_MISMATCHES, -1).toInt();
        if (vMismatches != -1) {
            arguments.append(QString("-v"));
            arguments.append(QString::number(vMismatches));
        }
    }
    arguments.append(QString("--maqerr"));
    arguments.append(settings.getCustomValue(BowtieTask::OPTION_MAQERR, 70).toString());
    arguments.append(QString("--seedlen"));
    arguments.append(settings.getCustomValue(BowtieTask::OPTION_SEED_LEN, 28).toString());
    if (settings.getCustomValue(BowtieTask::OPTION_NOMAQROUND, false).toBool()) {
        arguments.append("--nomaqround");
    }
    if (settings.getCustomValue(BowtieTask::OPTION_NOFW, false).toBool()) {
        arguments.append("--nofw");
    }
    if (settings.getCustomValue(BowtieTask::OPTION_NORC, false).toBool()) {
        arguments.append("--norc");
    }
    {
        int maxBts = settings.getCustomValue(BowtieTask::OPTION_MAXBTS, -1).toInt();
        if (-1 != maxBts) {
            arguments.append(QString("--maxbts"));
            arguments.append(QString::number(maxBts));
        }
    }
    if (settings.getCustomValue(BowtieTask::OPTION_TRYHARD, false).toBool()) {
        arguments.append("--tryhard");
    }
    arguments.append(QString("--chunkmbs"));
    arguments.append(settings.getCustomValue(BowtieTask::OPTION_CHUNKMBS, 64).toString());
    {
        int seed = settings.getCustomValue(BowtieTask::OPTION_SEED, -1).toInt();
        if (-1 != seed) {
            arguments.append(QString("--seed"));
            arguments.append(QString::number(seed));
        }
    }
    if (settings.getCustomValue(BowtieTask::OPTION_BEST, false).toBool()) {
        arguments.append("--best");
    }
    if (settings.getCustomValue(BowtieTask::OPTION_ALL, false).toBool()) {
        arguments.append("--all");
    }
    {
        int threads = settings.getCustomValue(BowtieTask::OPTION_THREADS, 1).toInt();
        arguments.append(QString("--threads"));
        arguments.append(QString::number(threads));
    }

    // We assume all datasets have the same format
    if (!settings.shortReadSets.isEmpty()) {
        QList<GUrl> shortReadUrls = settings.getShortReadUrls();
        QList<FormatDetectionResult> detectionResults = DocumentUtils::detectFormat(shortReadUrls.first());
        if (!detectionResults.isEmpty()) {
            if (detectionResults.first().format->getFormatId() == BaseDocumentFormats::FASTA) {
                arguments.append("-f");
            } else if (detectionResults.first().format->getFormatId() == BaseDocumentFormats::RAW_DNA_SEQUENCE) {
                arguments.append("-r");
            } else if (detectionResults.first().format->getFormatId() == BaseDocumentFormats::FASTQ) {
                arguments.append("-q");
            } else {
                setError(tr("Unknown short reads format %1").arg(detectionResults.first().format->getFormatId()));
            }
        }
    } else {
        setError("Short read list is empty!");
        return;
    }
    arguments.append("-S");
    arguments.append(settings.indexFileName);
    {
        // we assume that all datasets have same library type
        ShortReadSet::LibraryType libType = settings.shortReadSets.at(0).type;
        int setCount = settings.shortReadSets.size();

        if (libType == ShortReadSet::SingleEndReads) {
            QStringList readUrlsArgument;
            for (int index = 0; index < setCount; index++) {
                readUrlsArgument.append(settings.shortReadSets[index].url.getURLString());
            }
            arguments.append(readUrlsArgument.join(","));
        } else {
            QStringList upstreamReads, downstreamReads;

            for (int i = 0; i < setCount; ++i) {
                const ShortReadSet& set = settings.shortReadSets.at(i);
                if (set.order == ShortReadSet::UpstreamMate) {
                    upstreamReads.append(set.url.getURLString());
                } else {
                    downstreamReads.append(set.url.getURLString());
                }
            }

            if (upstreamReads.count() != downstreamReads.count()) {
                setError("Unequal number of upstream and downstream reads!");
                return;
            }

            arguments.append("-1");
            arguments.append(upstreamReads.join(","));
            arguments.append("-2");
            arguments.append(downstreamReads.join(","));
        }
    }
    arguments.append(settings.resultFileName.getURLString());
    logParser = new LogParser();
    auto task = new ExternalToolRunTask(BowtieSupport::ET_BOWTIE_ID, arguments, logParser, nullptr);
    setListenerForTask(task);
    addSubTask(task);
}

// BowtieAssembleTask::LogParser
BowtieAlignTask::LogParser::LogParser()
    : hasResults(false) {
}

void BowtieAlignTask::LogParser::parseOutput(const QString& partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
}

void BowtieAlignTask::LogParser::parseErrOutput(const QString& partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
    QRegExp blockRegExp("# reads with at least one alignment: (\\d+) \\(\\d+\\.\\d+%\\)");
    QRegExp blockRegExpOld("# reads with at least one reported alignment: (\\d+) \\(\\d+\\.\\d+%\\)");
    for (const QString& buf : qAsConst(lastPartOfLog)) {
        if (buf.contains(blockRegExp)) {
            if (blockRegExp.cap(1).toInt() > 0) {
                hasResults = true;
                break;
            }
        } else if (buf.contains(blockRegExpOld)) {
            if (blockRegExpOld.cap(1).toInt() > 0) {
                hasResults = true;
                break;
            }
        }
    }

    for (const QString& buf : qAsConst(lastPartOfLog)) {
        if (buf.contains("Out of memory")) {
            setLastError(tr("There is not enough memory on the computer!"));
            break;
        }
    }
}

bool BowtieAlignTask::LogParser::hasResult() const {
    return hasResults;
}

// BowtieTask

const QString BowtieTask::taskName = "Bowtie";

const QString BowtieTask::OPTION_N_MISMATCHES = "n-mismatches";
const QString BowtieTask::OPTION_V_MISMATCHES = "v-mismatches";
const QString BowtieTask::OPTION_MAQERR = "maqerr";
const QString BowtieTask::OPTION_SEED_LEN = "seedLen";
const QString BowtieTask::OPTION_NOFW = "nofw";
const QString BowtieTask::OPTION_NORC = "norc";
const QString BowtieTask::OPTION_MAXBTS = "maxbts";
const QString BowtieTask::OPTION_TRYHARD = "tryhard";
const QString BowtieTask::OPTION_CHUNKMBS = "chunkmbs";
const QString BowtieTask::OPTION_NOMAQROUND = "nomaqround";
const QString BowtieTask::OPTION_SEED = "seed";
const QString BowtieTask::OPTION_BEST = "best";
const QString BowtieTask::OPTION_ALL = "all";
const QString BowtieTask::OPTION_THREADS = "threads";

const QStringList BowtieTask::indexSuffixes = QStringList() << ".1.ebwt"
                                                            << ".2.ebwt"
                                                            << ".3.ebwt"
                                                            << ".4.ebwt"
                                                            << ".rev.1.ebwt"
                                                            << ".rev.2.ebwt";
const QStringList BowtieTask::largeIndexSuffixes = QStringList() << ".1.ebwtl"
                                                                 << ".2.ebwtl"
                                                                 << ".3.ebwtl"
                                                                 << ".4.ebwtl"
                                                                 << ".rev.1.ebwtl"
                                                                 << ".rev.2.ebwtl";

BowtieTask::BowtieTask(const DnaAssemblyToRefTaskSettings& settings, bool justBuildIndex)
    : DnaAssemblyToReferenceTask(settings, TaskFlags_NR_FOSCOE, justBuildIndex),
      buildIndexTask(nullptr),
      alignTask(nullptr),
      unzipTask(nullptr) {
}

void BowtieTask::prepare() {
    if (GzipDecompressTask::checkZipped(settings.refSeqUrl)) {
        temp.open();  // opening creates new temporary file
        temp.close();
        unzipTask = new GzipDecompressTask(settings.refSeqUrl, GUrl(QFileInfo(temp).absoluteFilePath()));
        settings.refSeqUrl = GUrl(QFileInfo(temp).absoluteFilePath());
    }

    if (!isBuildOnlyTask) {
        setUpIndexBuilding(indexSuffixes);
        if (!settings.prebuiltIndex) {
            setUpIndexBuilding(largeIndexSuffixes);
        }
    }

    if (!settings.prebuiltIndex) {
        QString indexFileName = settings.indexFileName;
        if (indexFileName.isEmpty()) {
            if (isBuildOnlyTask) {
                indexFileName = settings.refSeqUrl.dirPath() + "/" + settings.refSeqUrl.baseFileName();
            } else {
                indexFileName = settings.resultFileName.dirPath() + "/" + settings.resultFileName.baseFileName();
            }
        }
        buildIndexTask = new BowtieBuildTask(settings.refSeqUrl.getURLString(), indexFileName);
        buildIndexTask->addListeners(QList<ExternalToolListener*>() << getListener(0));
    }
    if (!isBuildOnlyTask) {
        alignTask = new BowtieAlignTask(settings);
        alignTask->addListeners(QList<ExternalToolListener*>() << getListener(1));
    }

    if (unzipTask != nullptr) {
        addSubTask(unzipTask);
    } else if (!settings.prebuiltIndex) {
        addSubTask(buildIndexTask);
    } else if (!isBuildOnlyTask) {
        addSubTask(alignTask);
    } else {
        Q_ASSERT(false);
    }
}

Task::ReportResult BowtieTask::report() {
    if (!isBuildOnlyTask) {
        hasResults = alignTask->hasResult();
    }
    return ReportResult_Finished;
}

QList<Task*> BowtieTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;

    if (subTask == unzipTask) {
        if (!settings.prebuiltIndex) {
            result.append(buildIndexTask);
        } else if (!isBuildOnlyTask) {
            result.append(alignTask);
        }
    }

    if ((subTask == buildIndexTask) && !isBuildOnlyTask) {
        result.append(alignTask);
    }
    return result;
}

// BowtieTaskFactory

DnaAssemblyToReferenceTask* BowtieTaskFactory::createTaskInstance(const DnaAssemblyToRefTaskSettings& settings, bool justBuildIndex) {
    return new BowtieTask(settings, justBuildIndex);
}

}  // namespace U2
