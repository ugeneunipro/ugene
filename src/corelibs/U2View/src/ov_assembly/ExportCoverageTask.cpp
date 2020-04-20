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

#include <U2Core/Counter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2CoreAttributes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ExportCoverageTask.h"

namespace U2 {

const QString ExportCoverageSettings::HISTOGRAM = QObject::tr("Histogram");
const QString ExportCoverageSettings::PER_BASE = QObject::tr("Per base");
const QString ExportCoverageSettings::BEDGRAPH = QObject::tr("Bedgraph");
const QString ExportCoverageSettings::HISTOGRAM_EXTENSION = "histogram";
const QString ExportCoverageSettings::PER_BASE_EXTENSION = "txt";
const QString ExportCoverageSettings::BEDGRAPH_EXTENSION = "bedgraph";
const QString ExportCoverageSettings::COMPRESSED_EXTENSION = ".gz";

const QByteArray ExportCoverageTask::SEPARATOR = "\t";
const QList<char> ExportCoverageTask::EXTENDED_CHARACTERS = QList<char>() << 'W' << 'R' << 'M' << 'K' << 'Y' << 'S' << 'B' << 'V' << 'H' << 'D';

void GetAssemblyVisibleNameTask::run(){
    DbiConnection con(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    U2AssemblyDbi *assemblyDbi = con.dbi->getAssemblyDbi();
    SAFE_POINT_EXT(NULL != assemblyDbi, setError(tr("Assembly DBI is NULL")), );

    const U2Assembly assembly = assemblyDbi->getAssemblyObject(assemblyId, stateInfo);
    CHECK_OP(stateInfo, );
    assemblyName = assembly.visualName;
}

ExportCoverageTask::ExportCoverageTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId, const ExportCoverageSettings &settings, TaskFlags flags) :
    Task(tr("Export assembly coverage per base"), flags),
    dbiRef(dbiRef),
    assemblyId(assemblyId),
    settings(settings),
    getAssemblyNameTask(NULL),
    calculateTask(NULL),
    alreadyProcessed(0)
{
    SAFE_POINT_EXT(dbiRef.isValid(), setError(tr("Invalid database reference")), );
    SAFE_POINT_EXT(!assemblyId.isEmpty(), setError(tr("Invalid assembly ID")), );
    SAFE_POINT_EXT(!settings.url.isEmpty(), setError(tr("Invalid destination url")), );

    alphabetChars << 'A' << 'C' << 'G' << 'T';
}

void ExportCoverageTask::prepare() {
    getAssemblyNameTask = new GetAssemblyVisibleNameTask(dbiRef, assemblyId);
    addSubTask(getAssemblyNameTask);

    QDir().mkpath(QFileInfo(settings.url).absoluteDir().absolutePath());
    if (settings.compress) {
        IOAdapterFactory *ioAdapterFactory = IOAdapterUtils::get(BaseIOAdapters::GZIPPED_LOCAL_FILE);
        SAFE_POINT_EXT(NULL != ioAdapterFactory, setError(tr("Can't write the compressed file: IOAdapterFactory is NULL")), );
        ioAdapter.reset(ioAdapterFactory->createIOAdapter());
        bool isSuccess = ioAdapter->open(settings.url, IOAdapterMode_Write);
        CHECK_EXT(isSuccess, setError(L10N::errorOpeningFileWrite(settings.url)), );
    } else {
        IOAdapterFactory *ioAdapterFactory = IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE);
        SAFE_POINT_EXT(NULL != ioAdapterFactory, setError(tr("Can't write the file: IOAdapterFactory is NULL")), );
        ioAdapter.reset(ioAdapterFactory->createIOAdapter());
        bool isSuccess = ioAdapter->open(settings.url, IOAdapterMode_Write);
        CHECK_EXT(isSuccess, setError(L10N::errorOpeningFileWrite(settings.url)), );
    }
    CHECK_OP(stateInfo, );

    calculateTask = new CalculateCoveragePerBaseTask(dbiRef, assemblyId);
    connect(calculateTask, SIGNAL(si_regionIsProcessed(qint64)), SLOT(sl_regionIsProcessed(qint64)), Qt::DirectConnection);
    addSubTask(calculateTask);
}

QList<Task *> ExportCoverageTask::onSubTaskFinished(Task *subTask){
    if(subTask == getAssemblyNameTask){
        assemblyName = getAssemblyNameTask->getAssemblyVisibleName();
    }
    return QList<Task *>();
}
Task::ReportResult ExportCoverageTask::report() {
    if (NULL != calculateTask) {
        SAFE_POINT_EXT(!calculateTask->areThereUnprocessedResults(), setError(tr("Not all regions were processed")), ReportResult_Finished);
    }
    return ReportResult_Finished;
}

const QString &ExportCoverageTask::getUrl() const {
    return settings.url;
}

void ExportCoverageTask::sl_regionIsProcessed(qint64 startPos) {
    if (alreadyProcessed == startPos) {
        QVector<CoveragePerBaseInfo> *regionCoverage = calculateTask->takeResult(startPos);
        if (startPos == 0) {
            identifyAlphabet(regionCoverage);
            writeHeader();
        }
        processRegion(regionCoverage);
        delete regionCoverage;
        CHECK_OP(stateInfo, );

        if (calculateTask->isResultReady(alreadyProcessed)) {
            sl_regionIsProcessed(alreadyProcessed);
        }
    }
}
void ExportCoverageTask::identifyAlphabet(QVector<CoveragePerBaseInfo>* regionCoverage) {
    CHECK(alphabetChars.size() == 4, );
    foreach(const CoveragePerBaseInfo &info, *regionCoverage) {
        QList<char> chars = info.basesCount.keys();
        foreach(char curChar, chars) {
            if (EXTENDED_CHARACTERS.contains(curChar)) {
                alphabetChars.append(EXTENDED_CHARACTERS);
                return;
            }
        }
    }
}

void ExportCoverageTask::writeHeader() {
}

void ExportCoverageTask::write(const QByteArray &dataToWrite) {
    qint64 bytesWritten = ioAdapter->writeBlock(dataToWrite);
    CHECK_EXT(bytesWritten == dataToWrite.length(), setError(L10N::errorWritingFile(ioAdapter->getURL())), );
}

ExportCoverageHistogramTask::ExportCoverageHistogramTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId, const ExportCoverageSettings &settings) :
    ExportCoverageTask(dbiRef, assemblyId, settings, TaskFlags_FOSE_COSC)
{
    GCOUNTER(c, t, "ExportCoverageHistogramTask");
}

void ExportCoverageHistogramTask::run() {
    DbiConnection con(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    U2AttributeDbi *attributeDbi = con.dbi->getAttributeDbi();
    SAFE_POINT_EXT(NULL != attributeDbi, setError(tr("Attribute DBI is NULL")), );

    const U2IntegerAttribute lengthAttribute = U2AttributeUtils::findIntegerAttribute(attributeDbi, assemblyId, U2BaseAttributeName::reference_length, stateInfo);
    CHECK_OP(stateInfo, );
    CHECK_EXT(lengthAttribute.hasValidId(), setError(tr("Can't get the assembly length: attribute is missing")), );

    const qint64 assemblyLength = lengthAttribute.value;
    SAFE_POINT_EXT(0 < assemblyLength, setError(tr("Assembly has zero length")), );

    for (int coverage = settings.threshold; coverage < histogramData.size(); coverage++) {
        if (0 != histogramData.value(coverage, 0)) {
            write(toByteArray(coverage, assemblyLength));
            CHECK_OP(stateInfo, );
        }
    }
}

void ExportCoverageHistogramTask::processRegion(const QVector<CoveragePerBaseInfo> *regionCoverage) {
    foreach (const CoveragePerBaseInfo &info, *regionCoverage) {
        histogramData[info.coverage] = histogramData.value(info.coverage, 0) + 1;
        alreadyProcessed++;
    }
}

QByteArray ExportCoverageHistogramTask::toByteArray(int coverage, qint64 assemblyLength) const {
    return assemblyName.toLocal8Bit() +
            SEPARATOR + QByteArray::number(coverage) +
            SEPARATOR + QByteArray::number(histogramData.value(coverage, 0)) +
            SEPARATOR + QByteArray::number(assemblyLength) +
            SEPARATOR + QByteArray::number((double)histogramData.value(coverage, 0) / assemblyLength) + "\n";
}

ExportCoveragePerBaseTask::ExportCoveragePerBaseTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId, const ExportCoverageSettings &settings) :
    ExportCoverageTask(dbiRef, assemblyId, settings)
{
    GCOUNTER(c, t, "ExportCoveragePerBaseTask");
}


void ExportCoveragePerBaseTask::processRegion(const QVector<CoveragePerBaseInfo> *regionCoverage) {
    writeResult(regionCoverage);
}

void ExportCoveragePerBaseTask::writeHeader() {
    QByteArray comments = "#name" + SEPARATOR + "position";

    if (settings.exportCoverage) {
        comments += SEPARATOR + "coverage";
    }

    if (settings.exportBasesCount) {
        foreach(char curChar, alphabetChars) {
            comments += SEPARATOR + curChar;
        }
    }

    write(comments + "\n");
}

QByteArray ExportCoveragePerBaseTask::toByteArray(const CoveragePerBaseInfo &info, int pos) const {
    QByteArray result = assemblyName.toLocal8Bit() + SEPARATOR + QByteArray::number(pos);

    if (settings.exportCoverage) {
        result += SEPARATOR + QByteArray::number(info.coverage);
    }

    if (settings.exportBasesCount) {
        foreach(char curChar, alphabetChars) {
            result += SEPARATOR + QByteArray::number(info.basesCount.value(curChar, 0));
        }
    }

    return result + "\n";
}

void ExportCoveragePerBaseTask::writeResult(const QVector<CoveragePerBaseInfo> *data) {
    CHECK(NULL != data, );

    foreach (const CoveragePerBaseInfo &info, *data) {
        alreadyProcessed++;

        const bool coverageSatisfy = settings.exportCoverage && (settings.threshold <= info.coverage);
        int baseCountsScore = 0;
        foreach(char curChar, alphabetChars) {
            baseCountsScore += info.basesCount.value(curChar, 0);
        }
        const bool basesCountSatisfy = settings.exportBasesCount && (settings.threshold <= baseCountsScore);
        if (!coverageSatisfy && !basesCountSatisfy) {
            continue;
        }

        write(toByteArray(info, alreadyProcessed));
        CHECK_OP(stateInfo, );
    }
}

ExportCoverageBedgraphTask::ExportCoverageBedgraphTask(const U2DbiRef &dbiRef, const U2DataId &assemblyId, const ExportCoverageSettings &settings) :
    ExportCoverageTask(dbiRef, assemblyId, settings),
    currentCoverage(U2Region(), -1)
{
    GCOUNTER(c, t, "ExportCoverageBedgraphTask");
}

QList<Task *> ExportCoverageBedgraphTask::onSubTaskFinished(Task *subTask) {
    CHECK_OP(stateInfo, QList<Task *>());
    if(subTask == getAssemblyNameTask){
        assemblyName = getAssemblyNameTask->getAssemblyVisibleName();
    }else{
        writeRegion();
    }
    return QList<Task *>();
}

void ExportCoverageBedgraphTask::processRegion(const QVector<CoveragePerBaseInfo> *regionCoverage) {
    foreach (const CoveragePerBaseInfo &info, *regionCoverage) {
        if (currentCoverage.second == info.coverage) {
            currentCoverage.first.length++;
        } else {
            writeRegion();
            CHECK_OP(stateInfo, );
            currentCoverage.first = U2Region(alreadyProcessed, 1);
            currentCoverage.second = info.coverage;
        }
        alreadyProcessed++;
    }
}

void ExportCoverageBedgraphTask::writeHeader() {
    QByteArray comments = "#name" +
            SEPARATOR + "start" +
            SEPARATOR + "end" +
            SEPARATOR + "coverage";
    write(comments + "\n");
}

QByteArray ExportCoverageBedgraphTask::toByteArray() const {
    QByteArray result = assemblyName.toLocal8Bit() +
            SEPARATOR + QByteArray::number(currentCoverage.first.startPos) +
            SEPARATOR + QByteArray::number(currentCoverage.first.endPos()) +
            SEPARATOR + QByteArray::number(currentCoverage.second);
    return result + "\n";
}

void ExportCoverageBedgraphTask::writeRegion() {
    CHECK(!currentCoverage.first.isEmpty(), );

    if (settings.threshold > currentCoverage.second) {
        return;
    }

    write(toByteArray());
}

QString ExportCoverageSettings::getFormat(ExportCoverageSettings::Format format) {
    switch (format) {
    case Histogram:
        return HISTOGRAM;
    case PerBase:
        return PER_BASE;
    case Bedgraph:
        return BEDGRAPH;
    default:
        return "";
    }
}

QString ExportCoverageSettings::getFormatExtension(ExportCoverageSettings::Format format) {
    switch (format) {
    case Histogram:
        return HISTOGRAM_EXTENSION;
    case PerBase:
        return PER_BASE_EXTENSION;
    case Bedgraph:
        return BEDGRAPH_EXTENSION;
    default:
        return "";
    }
}

}   // namespace U2
