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

#ifndef _U2_PAIRED_FASTQ_COMPARATOR_H_
#define _U2_PAIRED_FASTQ_COMPARATOR_H_

#include <U2Core/global.h>
#include <U2Core/LocalFileAdapter.h>
#include <U2Core/U2OpStatus.h>

#include <U2Formats/BAMUtils.h>

#include <QFile>


namespace U2 {

/**
 * The FastqSequenceInfo class
 */
class FastqSequenceInfo {
    friend class FastqFileIterator;
public:
    FastqSequenceInfo() {}
    FastqSequenceInfo(const DNASequence& seq);

    bool isValid() const;

    QString getSeqName() const;
    const DNASequence& getDNASeq() const { return seq; }

    bool operator == (const FastqSequenceInfo& other) const;
    bool operator !=(const FastqSequenceInfo& other) const;

private:
    DNASequence seq;
};

/**
 * The PairedFastqComparator class
 */
class U2FORMATS_EXPORT PairedFastqComparator : public QObject {
    Q_OBJECT
public:
    PairedFastqComparator(const QString& inputFile_1, const QString& inputFile_2,
                          const QString& outputFile_1, const QString& outputFile_2,
                          U2OpStatus &os);
    void compare(U2OpStatus& os);

    int getPairsCount() const { return pairsCounter; }
    int getUnpairedCount() const { return droppedCounter; }

private:
    void dropUntilItem(U2OpStatus& os, QList<FastqSequenceInfo>& list, const FastqSequenceInfo& untilItem);

    const FastqSequenceInfo tryToFindPair(U2OpStatus& os, QList<FastqSequenceInfo>& initializer, const FastqSequenceInfo& info, QList<FastqSequenceInfo>& searchIn);

    void tryToFindPairInTail(U2OpStatus& os, FASTQIterator& reads,
                              QList<FastqSequenceInfo>& unpaired, bool iteratorContentIsFirst);

    void writePair(U2OpStatus& os, const FastqSequenceInfo& seqInfo_1, const FastqSequenceInfo& seqInfo_2);

private:
    FASTQIterator it_1;
    FASTQIterator it_2;

    QScopedPointer<LocalFileAdapter> out_1;
    QScopedPointer<LocalFileAdapter> out_2;

    int pairsCounter;
    int droppedCounter;
};

} // namespace

#endif // _U2_PAIRED_FASTQ_COMPARATOR_H_

