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

#ifndef U2_GENE2PEAK_FORMAT_LOADER_H_
#define U2_GENE2PEAK_FORMAT_LOADER_H_

#include <QStringList>

#include <U2Core/AnnotationData.h>
#include <U2Core/U2Region.h>

namespace U2 {

class IOAdapter;
class U2OpStatus;

class Gene2PeakFormatLoader : public QObject {
    Q_OBJECT
public:
    Gene2PeakFormatLoader(U2OpStatus &os, IOAdapter *ioAdapter);

    QList<SharedAnnotationData> loadAnnotations();

private:
    enum Columns {
        ChromName = 0,
        StartPos = 1,
        EndPos,
        Gene,
        NA,
        Strand,
        PeakName,
        Tss2peakCenter,
        COLUMNS_COUNT
    };

    SharedAnnotationData parseLine(const QStringList &lineValues);
    QString getChromName(const QStringList &lineValues);
    U2Region getRegion(const QStringList &lineValues);
    QString getGene(const QStringList &lineValues);
    QString getNa(const QStringList &lineValues);
    QString getStrand(const QStringList &lineValues);
    QString getPeakName(const QStringList &lineValues);
    QString getTss2pCenter(const QStringList &lineValues);

    U2OpStatus &os;
    IOAdapter *ioAdapter;
    bool skipLine;
    qint64 currentLineNumber;
};

}   // namespace U2

#endif // U2_GENE2PEAK_FORMAT_LOADER_H_
