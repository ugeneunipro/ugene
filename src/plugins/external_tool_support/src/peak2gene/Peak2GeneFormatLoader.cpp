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

#include <U2Core/IOAdapter.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>

#include <U2Formats/TabulatedFormatReader.h>

#include "Peak2GeneFormatLoader.h"

namespace U2 {

Peak2GeneFormatLoader::Peak2GeneFormatLoader(U2OpStatus &os, IOAdapter *ioAdapter) :
    os(os),
    ioAdapter(ioAdapter),
    skipLine(false),
    currentLineNumber(0)
{
    CHECK_EXT(NULL != ioAdapter, os.setError(L10N::nullPointerError("IO adapter")), );
    CHECK_EXT(ioAdapter->isOpen(), os.setError(L10N::errorReadingFile(ioAdapter->getURL())), );
}

QList<SharedAnnotationData> Peak2GeneFormatLoader::loadAnnotations() {
    QList<SharedAnnotationData> annotations;

    TabulatedFormatReader reader(os, ioAdapter);
    CHECK_OP(os, annotations);

    while (reader.hasNextLine()) {
        skipLine = false;
        currentLineNumber = reader.getCurrentLineNumber();
        SharedAnnotationData annotation = parseLine(reader.getNextLine());
        if (!skipLine) {
            annotations << annotation;
        }
    }

    return annotations;
}

SharedAnnotationData Peak2GeneFormatLoader::parseLine(const QStringList &lineValues) {
    SharedAnnotationData data(new AnnotationData);
    CHECK_EXT(lineValues.size() == COLUMNS_COUNT, skipLine = true; os.addWarning(QString("Incorrect columns count at line %1: expect %2, got %3")
                                                                                 .arg(currentLineNumber).arg(COLUMNS_COUNT).arg(lineValues.size())), data);

    data->qualifiers << U2Qualifier("chrom", getChromName(lineValues));
    CHECK(!skipLine, data);

    data->location->regions << getRegion(lineValues);
    CHECK(!skipLine, data);

    data->name = getPeakName(lineValues);
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("score", getPeakScore(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("NA", getNa(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("Genes", getGenes(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("Strand", getStrand(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("TSS2pCenter", getTss2pCenter(lineValues));
    CHECK(!skipLine, data);

    return data;
}

QString Peak2GeneFormatLoader::getChromName(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[ChromName].isEmpty(), skipLine = true; os.addWarning(QString("Chrom name is empty at line %1")
                                                                               .arg(currentLineNumber)), "");
    return lineValues[ChromName];
}

U2Region Peak2GeneFormatLoader::getRegion(const QStringList &lineValues) {
    bool ok = false;
    const qint64 startPos = lineValues[StartPos].toLongLong(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect start position at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[StartPos])), U2Region());

    const qint64 endPos = lineValues[EndPos].toLongLong(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect end position at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[EndPos])), U2Region());

    return U2Region(startPos, endPos - startPos);
}

QString Peak2GeneFormatLoader::getPeakName(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[PeakName].isEmpty(), skipLine = true; os.addWarning(QString("Peak name is empty at line %1")
                                                                              .arg(currentLineNumber)), "");
    return lineValues[PeakName];
}

QString Peak2GeneFormatLoader::getPeakScore(const QStringList &lineValues) {
    bool ok = false;
    lineValues[PeakScore].toDouble(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect peak score at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[PeakScore])), "");
    return lineValues[PeakScore];
}

QString Peak2GeneFormatLoader::getNa(const QStringList &lineValues) {
    bool ok = false;
    lineValues[NA].toInt(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect NA value at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[NA])), "");
    return lineValues[NA];
}

QString Peak2GeneFormatLoader::getGenes(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[Genes].isEmpty(), skipLine = true; os.addWarning(QString("Genes list is empty at line %1")
                                                                           .arg(currentLineNumber)), "");
    return lineValues[Genes];
}

QString Peak2GeneFormatLoader::getStrand(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[Strand].isEmpty(), skipLine = true; os.addWarning(QString("Strand is empty at line %1")
                                                                            .arg(currentLineNumber)), "");
    return lineValues[Strand];
}

QString Peak2GeneFormatLoader::getTss2pCenter(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[Tss2peakCenter].isEmpty(), skipLine = true; os.addWarning(QString("Strand is empty at line %1")
                                                                                    .arg(currentLineNumber)), "");
    return lineValues[Tss2peakCenter];
}

}   // namespace U2
