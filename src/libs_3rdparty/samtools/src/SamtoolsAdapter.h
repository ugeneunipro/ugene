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

#ifndef _SAMTOOLS_ADAPTER_H_
#define _SAMTOOLS_ADAPTER_H_

extern "C" {
#include <bam.h>
}
#include <U2Core/U2Assembly.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2Type.h>
#include <QVector>

namespace U2 {
class U2OpStatus;

class ReadsContainer {
public:
    void append(const bam1_t & item) { vector.append(item); }

    const bam1_t & operator [](int i) const { return vector[i]; }
    int size() const { return vector.size(); }

    void sortByStartPos();

    // Destructor is required to free var-length fields (bam1_t::data)
    ~ReadsContainer();
private:
    QVector<bam1_t> vector;
};

class ReadsContext {
public:
    ReadsContext(const QString &assemblyName,
        const QMap<QString, int> &assemblyNumMap);

    int getReadAssemblyNum() const;
    int getAssemblyNum(const QString &assemblyName) const;

private:
    QString assemblyName;
    QMap<QString, int> assemblyNumMap;
};

class SamtoolsAdapter : public QObject {
    Q_OBJECT
public:
    /**
        Main function for converting reads from dbi to samtools internal format bam1_t.
        Obtains reads from U2DbiIterator, appends converted ones to given ReadsContainer.
        All samtools functions will accept pointer obtained via ReadsContainer::data()
    */
    static void reads2samtools(U2DbiIterator<U2AssemblyRead> *reads, U2OpStatus &os, ReadsContainer &result);

    static void read2samtools(const U2AssemblyRead &read, U2OpStatus &os, bam1_t &result);
    static void read2samtools(const U2AssemblyRead &read, const ReadsContext &ctx, U2OpStatus &os, bam1_t &result);

    /** Helpers for different fields of bam1_t */

    static QByteArray cigar2samtools(QList<U2CigarToken> cigar, U2OpStatus &os);
    static QByteArray sequence2samtools(QByteArray sequence, U2OpStatus &os);
    static QByteArray quality2samtools(const QByteArray &qualString);
    static QByteArray samtools2quality(const QByteArray &qualString);
    static bool hasQuality(const QByteArray &qualString);

    static QByteArray aux2string(const QList<U2AuxData> &aux);
    static QList<U2AuxData> string2aux(const QByteArray &auxString);
    static QList<U2AuxData> samString2aux(const QByteArray &auxString);

};

} // namespace

#endif
