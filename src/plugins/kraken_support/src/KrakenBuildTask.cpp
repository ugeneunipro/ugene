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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/CopyFileTask.h>
#include <U2Core/Counter.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/U2SafePoints.h>

#include "KrakenBuildTask.h"
#include "KrakenSupport.h"
#include "../../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {

const QString KrakenBuildTaskSettings::BUILD = "build";
const QString KrakenBuildTaskSettings::SHRINK = "shrink";

KrakenBuildTaskSettings::KrakenBuildTaskSettings()
    : mode(BUILD),
      numberOfKmers(0),
      kMerLength(31),
      minimizerLength(15),
      maximumDatabaseSize(0),
      shrinkBlockOffset(1),
      clean(true),
      workOnDisk(false),
      jellyfishHashSize(0),
      threadsNumber(1)
{

}

KrakenBuildTask::KrakenBuildTask(const KrakenBuildTaskSettings &settings)
    : ExternalToolSupportTask(tr("%1 Kraken database"), TaskFlags_NR_FOSE_COSC),
      settings(settings)
{
    GCOUNTER(cvar, tvar, "KrakenBuildTask");
    setTaskName(getTaskName().arg(settings.mode == KrakenBuildTaskSettings::BUILD ? tr("Build") : tr("Shrink")));

    checkSettings();
    CHECK_OP(stateInfo, );

    checkTaxonomy();
}

const QString &KrakenBuildTask::getResultDatabaseUrl() const {
    return settings.newDatabaseUrl;
}

const KrakenBuildTaskSettings &KrakenBuildTask::getSettings() const {
    return settings;
}

void KrakenBuildTask::prepare() {
    QList<Task *> newSubTasks;
    if (settings.mode == KrakenBuildTaskSettings::BUILD) {
        int listenerNumber = 0;

        newSubTasks << prepareTaxonomyData();
        CHECK_OP(stateInfo, );

        foreach (const QString &additionalGenome, settings.additionalGenomesUrls) {
            ExternalToolRunTask *addToLibraryTask = new ExternalToolRunTask(KrakenSupport::BUILD_TOOL_ID, getAddToLibraryArguments(additionalGenome), new ExternalToolLogParser());
            setListenerForTask(addToLibraryTask, listenerNumber++);
            newSubTasks << addToLibraryTask;
        }

        ExternalToolRunTask *buildTask = new ExternalToolRunTask(KrakenSupport::BUILD_TOOL_ID, getBuildArguments(), new ExternalToolLogParser());
        setListenerForTask(buildTask, listenerNumber++);
        newSubTasks << buildTask;

        if (settings.clean) {
            ExternalToolRunTask *cleanTask = new ExternalToolRunTask(KrakenSupport::BUILD_TOOL_ID, getCleanArguments(), new ExternalToolLogParser());
            setListenerForTask(cleanTask, listenerNumber++);
            newSubTasks << cleanTask;
        }
    } else if (settings.mode == KrakenBuildTaskSettings::SHRINK) {
        ExternalToolRunTask *shrinkTask = new ExternalToolRunTask(KrakenSupport::BUILD_TOOL_ID, getShrinkArguments(), new ExternalToolLogParser());
        setListenerForTask(shrinkTask);
        newSubTasks << shrinkTask;
    } else {
        FAIL("An unknown mode", );
    }

    SequentialMultiTask *sequentialTask = new SequentialMultiTask(tr("Sequential execution"), newSubTasks, TaskFlags_NR_FOSE_COSC);
    addSubTask(sequentialTask);
}

void KrakenBuildTask::checkSettings() {
    SAFE_POINT_EXT(settings.mode == KrakenBuildTaskSettings::BUILD || settings.mode == KrakenBuildTaskSettings::SHRINK, setError("Unknown mode"), );
    CHECK_EXT(settings.mode != KrakenBuildTaskSettings::SHRINK || !settings.inputDatabaseUrl.isEmpty(), setError(tr("Input database URL is empty")), );
    CHECK_EXT(settings.mode != KrakenBuildTaskSettings::SHRINK || QFileInfo(settings.inputDatabaseUrl).exists(), setError(tr("Input database doesn't exist")), );
    CHECK_EXT(!settings.newDatabaseUrl.isEmpty(), setError(tr("New database URL is empty")), );
    CHECK_EXT(settings.mode != KrakenBuildTaskSettings::BUILD || !settings.additionalGenomesUrls.isEmpty(), setError(tr("Genomes URLs list to build database from is empty")), );
    SAFE_POINT_EXT(settings.mode != KrakenBuildTaskSettings::SHRINK || 0 < settings.numberOfKmers, setError("K-mers count to move to a new database cannot be less than 0"), );
    SAFE_POINT_EXT(3 <= settings.kMerLength && settings.kMerLength <= 31, setError(QString("K-mer length is out of boundaries: %1").arg(settings.kMerLength)), );
    SAFE_POINT_EXT(1 <= settings.minimizerLength && settings.minimizerLength <= 30, setError(QString("Minimizer length is out of boundaries: %1").arg(settings.minimizerLength)), );
    SAFE_POINT_EXT(settings.minimizerLength < settings.kMerLength, setError("Minimizer length is not less than k-mer length"), );
    SAFE_POINT_EXT(0 <= settings.maximumDatabaseSize, setError(QString("Maximum database size cannot be less than 0: %1").arg(settings.maximumDatabaseSize)), );
    SAFE_POINT_EXT(0 <= settings.shrinkBlockOffset, setError(QString("Shrink block offset cannot be less than 0: %1").arg(settings.shrinkBlockOffset)), );
    SAFE_POINT_EXT(0 <= settings.jellyfishHashSize, setError(QString("Jellyfish hash size cannot be less than 0: %1").arg(settings.jellyfishHashSize)), );
    SAFE_POINT_EXT(0 <= settings.threadsNumber, setError(QString("Threads number cannot be less than 0: %1").arg(settings.threadsNumber)), );
}

void KrakenBuildTask::checkTaxonomy() {
    U2DataPath *taxonomyDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(), setError(tr("Taxonomy data are not set")), );
}

QList<Task *> KrakenBuildTask::prepareTaxonomyData() {
    // These two files are mandarory even for the cleaned database
    static const QStringList TAXONOMY_FILES_TO_COPY = QStringList() << NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID
                                                                    << NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID;

    QList<Task *> copyTasks;

    const QString databaseTaxonomyDirUrl = settings.newDatabaseUrl + "/taxonomy";
    GUrlUtils::prepareDirLocation(databaseTaxonomyDirUrl, stateInfo);
    CHECK_OP(stateInfo, copyTasks);

    U2DataPath *taxonomyDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(), setError(tr("Cannot find taxonomy data")), copyTasks);
    const QString sourceDirUrl = taxonomyDataPath->getPath();

    foreach (const QString &fileName, QDir(sourceDirUrl).entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        if (!TAXONOMY_FILES_TO_COPY.contains(fileName)) {
            const bool succeed = QFile::link(sourceDirUrl + "/" + fileName, databaseTaxonomyDirUrl + "/" + fileName);
            CHECK_EXT(succeed, setError(tr("Can't create a symbolic link to the taxonomy file: %1").arg(sourceDirUrl + "/" + fileName)), copyTasks);
        } else {
            copyTasks << new CopyFileTask(sourceDirUrl + "/" + fileName, databaseTaxonomyDirUrl + "/" + fileName);
        }
    }

    return copyTasks;
}

QStringList KrakenBuildTask::getAddToLibraryArguments(const QString &additionalGenomeUrl) const {
    QStringList arguments;
    arguments << "--add-to-library" << additionalGenomeUrl;
    arguments << "--db" << settings.newDatabaseUrl;
    return arguments;
}

QStringList KrakenBuildTask::getBuildArguments() const {
    QStringList arguments;
    arguments << "--build";
    arguments << "--db" << settings.newDatabaseUrl;
    arguments << "--threads" << QString::number(settings.threadsNumber);
    arguments << "--kmer-len" << QString::number(settings.kMerLength);
    arguments << "--minimizer-len" << QString::number(settings.minimizerLength);

    if (0 < settings.jellyfishHashSize) {
        arguments << "--jellyfish-hash-size" << QString::number(settings.jellyfishHashSize);
    }

    if (0 < settings.maximumDatabaseSize) {
        arguments << "--max-db-size" << QString::number(static_cast<double>(settings.maximumDatabaseSize) / 1024);
    }

    if (settings.workOnDisk) {
        arguments << "--work-on-disk";
    }

    return arguments;
}

QStringList KrakenBuildTask::getCleanArguments() const {
    QStringList arguments;
    arguments << "--clean";
    arguments << "--db" << settings.newDatabaseUrl;
    return arguments;
}

QStringList KrakenBuildTask::getShrinkArguments() const {
    QStringList arguments;
    arguments << "--shrink" << QString::number(settings.numberOfKmers);
    arguments << "--db" << settings.inputDatabaseUrl;
    arguments << "--new-db" << settings.newDatabaseUrl;
    arguments << "--threads" << QString::number(settings.threadsNumber);
    arguments << "--kmer-len" << QString::number(settings.kMerLength);
    arguments << "--minimizer-len" << QString::number(settings.minimizerLength);
    arguments << "--shrink-block-offset" << QString::number(settings.shrinkBlockOffset);
    if (settings.workOnDisk) {
        arguments << "--work-on-disk";
    }
    return arguments;
}

}   // namespace U2
