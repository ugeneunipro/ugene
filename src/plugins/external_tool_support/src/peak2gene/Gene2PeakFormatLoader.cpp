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

#include <U2Core/IOAdapter.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>

#include <U2Formats/TabulatedFormatReader.h>

#include "Gene2PeakFormatLoader.h"

namespace U2 {

Gene2PeakFormatLoader::Gene2PeakFormatLoader(U2OpStatus &os, IOAdapter *ioAdapter) :
    os(os),
    ioAdapter(ioAdapter),
    skipLine(false),
    currentLineNumber(0)
{
    CHECK_EXT(NULL != ioAdapter, os.setError(L10N::nullPointerError("IO adapter")), );
    CHECK_EXT(ioAdapter->isOpen(), os.setError(L10N::errorReadingFile(ioAdapter->getURL())), );
}

QList<SharedAnnotationData> Gene2PeakFormatLoader::loadAnnotations() {
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

SharedAnnotationData Gene2PeakFormatLoader::parseLine(const QStringList &lineValues) {
    SharedAnnotationData data(new AnnotationData);
    CHECK_EXT(lineValues.size() == COLUMNS_COUNT, skipLine = true; os.addWarning(QString("Incorrect columns count at line %1: expect %2, got %3")
                                                                                 .arg(currentLineNumber).arg(COLUMNS_COUNT).arg(lineValues.size())), data);

    data->qualifiers << U2Qualifier("chrom", getChromName(lineValues));
    CHECK(!skipLine, data);

    data->location->regions << getRegion(lineValues);
    CHECK(!skipLine, data);

    data->name = getGene(lineValues);
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("NA", getNa(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("Strand", getStrand(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("Peak_name", getPeakName(lineValues));
    CHECK(!skipLine, data);

    data->qualifiers << U2Qualifier("TSS2pCenter", getTss2pCenter(lineValues));
    CHECK(!skipLine, data);

    return data;
}

QString Gene2PeakFormatLoader::getChromName(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[ChromName].isEmpty(), skipLine = true; os.addWarning(QString("Chrom name is empty at line %1")
                                                                               .arg(currentLineNumber)), "");
    return lineValues[ChromName];
}

U2Region Gene2PeakFormatLoader::getRegion(const QStringList &lineValues) {
    bool ok = false;
    const qint64 startPos = lineValues[StartPos].toLongLong(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect start position at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[StartPos])), U2Region());

    const qint64 endPos = lineValues[EndPos].toLongLong(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect end position at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[EndPos])), U2Region());

    return U2Region(startPos, endPos - startPos);
}

QString Gene2PeakFormatLoader::getGene(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[Gene].isEmpty(), skipLine = true; os.addWarning(QString("Genes list is empty at line %1")
                                                                           .arg(currentLineNumber)), "");
    return lineValues[Gene];
}

QString Gene2PeakFormatLoader::getNa(const QStringList &lineValues) {
    bool ok = false;
    lineValues[NA].toInt(&ok);
    CHECK_EXT(ok, skipLine = true; os.addWarning(tr("Incorrect NA value at line %1: '%2'")
                                                 .arg(currentLineNumber).arg(lineValues[NA])), "");
    return lineValues[NA];
}

QString Gene2PeakFormatLoader::getStrand(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[Strand].isEmpty(), skipLine = true; os.addWarning(QString("Strand is empty at line %1")
                                                                            .arg(currentLineNumber)), "");
    return lineValues[Strand];
}

QString Gene2PeakFormatLoader::getPeakName(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[PeakName].isEmpty(), skipLine = true; os.addWarning(QString("Peak name is empty at line %1")
                                                                              .arg(currentLineNumber)), "");
    return lineValues[PeakName];
}

QString Gene2PeakFormatLoader::getTss2pCenter(const QStringList &lineValues) {
    CHECK_EXT(!lineValues[Tss2peakCenter].isEmpty(), skipLine = true; os.addWarning(QString("Strand is empty at line %1")
                                                                                    .arg(currentLineNumber)), "");
    return lineValues[Tss2peakCenter];
}

}   // namespace U2
