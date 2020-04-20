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
#include <QFileInfo>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>

#include <U2Formats/CalculateSequencesNumberTask.h>
#include <U2Formats/StreamSequenceReader.h>

#include "Metaphlan2LogParser.h"
#include "Metaphlan2Support.h"
#include "Metaphlan2Task.h"
#include "Metaphlan2WorkerFactory.h"

namespace U2 {

using namespace LocalWorkflow;

Metaphlan2TaskSettings::Metaphlan2TaskSettings() : isPairedEnd(false),
                                                   numberOfThreads(1),
                                                   normalizeByMetagenomeSize(false),
                                                   presenceThreshold(1) {}

Metaphlan2ClassifyTask::Metaphlan2ClassifyTask(const Metaphlan2TaskSettings& _settings) :
                                    ExternalToolSupportTask(tr("Classify reads with Metaphlan2"),
                                    TaskFlags_NR_FOSE_COSC | TaskFlag_MinimizeSubtaskErrorText),
                                    settings(_settings),
                                    classifyTask(nullptr),
                                    calculateSequencesNumberTask(nullptr) {
    GCOUNTER(cvar, tvar, "Metaphlan2ClassifyTask");

    needToCountSequences = settings.analysisType == Metaphlan2WorkerFactory::ANALYSIS_TYPE_MARKER_AB_TABLE_VALUE &&
                           settings.normalizeByMetagenomeSize;
    sequencesNumber = 0;

    SAFE_POINT_EXT(!settings.databaseUrl.isEmpty(), setError(tr("Metaphlan2 database URL is empty.")), );
    SAFE_POINT_EXT(!settings.bowtie2OutputFile.isEmpty(), setError(tr("Bowtie2 output file URL is empty.")), );
    SAFE_POINT_EXT(!settings.outputFile.isEmpty(), setError(tr("Metaphlan2 output file URL is empty.")), );

    SAFE_POINT_EXT(!settings.tmpDir.isEmpty(), setError("Temporary folder URL is empty."), );
    SAFE_POINT_EXT(!settings.readsUrl.isEmpty(), setError(tr("Reads URL is empty.")), );
    SAFE_POINT_EXT(!settings.isPairedEnd ||
                   !settings.readsUrl.isEmpty(),
                   setError(tr("Paired reads URL is empty, but the 'paired reads' option is set.")), );
}

const QString& Metaphlan2ClassifyTask::getBowtie2OutputUrl() const {
    return settings.bowtie2OutputFile;
}

const QString& Metaphlan2ClassifyTask::getOutputUrl() const {
    return settings.outputFile;
}

void Metaphlan2ClassifyTask::prepare() {
    if (needToCountSequences) {
        calculateSequencesNumberTask = new CalculateSequencesNumberTask(settings.readsUrl);
        addSubTask(calculateSequencesNumberTask);
    } else {
        prepareClassifyTask();
        addSubTask(classifyTask);
    }
}

QList<Task*> Metaphlan2ClassifyTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(!hasError() && !isCanceled(), result);
    CHECK(calculateSequencesNumberTask == subTask, result);

    sequencesNumber = calculateSequencesNumberTask->getSequencesNumber();
    prepareClassifyTask();
    result << classifyTask;

    return result;
}

QStringList Metaphlan2ClassifyTask::getArguments() {
    QStringList arguments;
    if (!settings.isPairedEnd) {
        arguments << QDir::toNativeSeparators(settings.readsUrl);
    } else {
        arguments << QString("%1,%2").arg(QDir::toNativeSeparators(settings.readsUrl)).arg(QDir::toNativeSeparators(settings.pairedReadsUrl));
    }

    arguments << "--nproc" << QString::number(settings.numberOfThreads);
    QString analysisType = settings.analysisType;
    arguments << "-t" << analysisType.replace("-", "_");

    if (settings.analysisType == Metaphlan2WorkerFactory::ANALYSIS_TYPE_REL_AB_VALUE ||
        settings.analysisType == Metaphlan2WorkerFactory::ANALYSIS_TYPE_REL_AB_W_READ_STATS_VALUE) {
        arguments << "--tax_lev" << settings.taxLevel;
    } else if (needToCountSequences) {
        arguments << "--nreads" << QString::number(sequencesNumber);
    } else if (settings.analysisType == Metaphlan2WorkerFactory::ANALYSIS_TYPE_MARKER_PRES_TABLE_VALUE) {
        arguments << "--pres_th" << QString::number(settings.presenceThreshold);
    }

    arguments << "--tmp_dir" << QDir::toNativeSeparators(settings.tmpDir);
    DocumentFormatId formatId = detectInputFormats();
    CHECK_OP(stateInfo, QStringList());

    arguments << "--input_type" << (BaseDocumentFormats::FASTA == formatId ? "fasta" : "fastq");
    arguments << "--bowtie2out" << QDir::toNativeSeparators(settings.bowtie2OutputFile);

    QDir databaseDir(QDir::toNativeSeparators(settings.databaseUrl));
    QStringList filters = QStringList() << "*.pkl";
    QStringList pklFiles = databaseDir.entryList(filters);
    CHECK_EXT(!pklFiles.isEmpty(), stateInfo.setError(tr(".pkl file is absent in the database folder.")), QStringList());
    CHECK_EXT(pklFiles.size() == 1, stateInfo.setError(tr("There is 1 .pkl file in the database folder expected.")), QStringList());

    arguments << "--mpa_pkl" << QDir::toNativeSeparators(QString("%1/%2").arg(settings.databaseUrl).arg(pklFiles.first()));
    arguments << "--bowtie2db" << QDir::toNativeSeparators(settings.databaseUrl);
    arguments << "-o" << QDir::toNativeSeparators(settings.outputFile);

    return arguments;
}


void Metaphlan2ClassifyTask::prepareClassifyTask() {
    classifyTask = new ExternalToolRunTask(Metaphlan2Support::TOOL_ID,
                                            getArguments(),
                                            new Metaphlan2LogParser(),
                                            QString(),
                                            QStringList() << settings.bowtie2ExternalToolPath
                                                          << settings.pythonExternalToolPath);
    setListenerForTask(classifyTask);
}

DocumentFormatId Metaphlan2ClassifyTask::detectInputFormats() {
    DocumentFormatId formatId = detectFormat(GUrl(settings.readsUrl));
    if (settings.isPairedEnd) {
        DocumentFormatId pairedFormatId = detectFormat(GUrl(settings.pairedReadsUrl));
        CHECK_EXT(formatId == pairedFormatId,
                       stateInfo.setError(tr("Input files with PE reads have different format.")),
                       DocumentFormatId());
    }

    return formatId;
}

DocumentFormatId Metaphlan2ClassifyTask::detectFormat(const GUrl& url) {
    DocumentFormatId resultFormatId;
    DocumentUtils::Detection detection = DocumentUtils::detectFormat(url, resultFormatId);
    CHECK_EXT(detection == DocumentUtils::FORMAT,
              stateInfo.setError(tr("Input file format couldn't be detected.")),
              DocumentFormatId());

    CHECK_EXT(resultFormatId == BaseDocumentFormats::FASTA || resultFormatId == BaseDocumentFormats::FASTQ,
              stateInfo.setError(tr("Unexpected input file format detected. It should be FASTA or FASTQ.")),
              DocumentFormatId());

    return resultFormatId;
}

} // namespace U2

