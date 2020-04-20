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

#include "PairedFastqComparator.h"

#include <U2Core/U2SafePoints.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/IOAdapter.h>

#include <U2Formats/FastqFormat.h>


namespace U2 {

const int UNPAIRED_LIMIT = 100000;

FastqSequenceInfo::FastqSequenceInfo(const DNASequence& seq)
    : seq(seq) {

}

bool FastqSequenceInfo::isValid() const {
    return !seq.isNull();
}

QString FastqSequenceInfo::getSeqName() const {
    return seq.getName();
}

bool FastqSequenceInfo::operator ==(const FastqSequenceInfo& other) const {
    return seq.getName() == other.getSeqName();
}

bool FastqSequenceInfo::operator !=(const FastqSequenceInfo& other) const {
    return !(*this == other);
}

PairedFastqComparator::PairedFastqComparator(const QString &inputFile_1, const QString &inputFile_2,
                                             const QString &outputFile_1, const QString &outputFile_2,
                                             U2OpStatus &os)
    : it_1(inputFile_1, os),
      it_2(inputFile_2, os),
      out_1(qobject_cast<LocalFileAdapter*>(IOAdapterUtils::open(GUrl(outputFile_1), os, IOAdapterMode_Write))),
      out_2(qobject_cast<LocalFileAdapter*>(IOAdapterUtils::open(GUrl(outputFile_2), os, IOAdapterMode_Write))),
      pairsCounter(0),
      droppedCounter(0)
{
    SAFE_POINT_OP(os, );
}

void PairedFastqComparator::compare(U2OpStatus &os) {

    QList<FastqSequenceInfo> unpaired_1;
    QList<FastqSequenceInfo> unpaired_2;

    FastqSequenceInfo tmp;
    while (it_1.hasNext() && it_2.hasNext() && !os.isCoR()) {
        CHECK_EXT(unpaired_1.size() + unpaired_2.size() < UNPAIRED_LIMIT,
                  os.setError(tr("Too much reads without a pair (>%1). Check the input data are set correctly.").arg(UNPAIRED_LIMIT)), );

        FastqSequenceInfo seqInfo_1(it_1.next());
        FastqSequenceInfo seqInfo_2(it_2.next());

        if (seqInfo_1 == seqInfo_2) {
            writePair(os, seqInfo_1, seqInfo_2);
            CHECK_OP(os, );

            droppedCounter += unpaired_1.size();
            droppedCounter += unpaired_2.size();

            unpaired_1.clear();
            unpaired_2.clear();
            continue;
        }

        if ((tmp = tryToFindPair(os, unpaired_1, seqInfo_1, unpaired_2)).isValid() && !os.isCoR()) {
            writePair(os, seqInfo_1, tmp);
            unpaired_2 << seqInfo_2;
            continue;
        }
        CHECK_OP(os, );

        if ((tmp = tryToFindPair(os, unpaired_2, seqInfo_2, unpaired_1)).isValid() && !os.isCoR()) {
            writePair(os, tmp, seqInfo_2);
            unpaired_1 << seqInfo_1;
            continue;
        }
        CHECK_OP(os, );

        unpaired_1 << seqInfo_1;
        unpaired_2 << seqInfo_2;
    }
    CHECK_OP(os, );

    // for correct counters info
    tryToFindPairInTail(os, it_1, unpaired_2, true);
    CHECK_OP(os, );
    tryToFindPairInTail(os, it_2, unpaired_1, false);
    CHECK_OP(os, );

    out_1->close();
    out_2->close();
}

void PairedFastqComparator::dropUntilItem(U2OpStatus& /*os*/, QList<FastqSequenceInfo>& list, const FastqSequenceInfo& untilItem) {
    CHECK(!list.isEmpty(), );

    FastqSequenceInfo item;
    do {
        item = list.takeFirst();
        droppedCounter++;
    } while (item != untilItem && !list.isEmpty());
    droppedCounter--; // the sequence that is in the pair was count
}

const FastqSequenceInfo PairedFastqComparator::tryToFindPair(U2OpStatus& os, QList<FastqSequenceInfo>& initializer, const FastqSequenceInfo& info, QList<FastqSequenceInfo>& searchIn) {
    int index = searchIn.indexOf(info);
    if (index != -1) {
        FastqSequenceInfo result = searchIn.at(index);
        droppedCounter += initializer.size();
        initializer.clear();

        dropUntilItem(os, searchIn, info);
        return result;
    }
    return FastqSequenceInfo();
}

void PairedFastqComparator::tryToFindPairInTail(U2OpStatus& os, FASTQIterator& reads,
                                                 QList<FastqSequenceInfo>& unpaired, bool iteratorContentIsFirst) {
    QList<FastqSequenceInfo> emptyList;
    while (reads.hasNext() && !os.isCoR()) {
        const FastqSequenceInfo seqInfo_1(reads.next());
        const FastqSequenceInfo seqInfo_2 = tryToFindPair(os, emptyList, seqInfo_1, unpaired);
        if (!seqInfo_2.isValid()) {
            droppedCounter++;
        } else {
            if (iteratorContentIsFirst) {
                writePair(os, seqInfo_1, seqInfo_2);
                CHECK_OP(os, );
            } else {
                writePair(os, seqInfo_2, seqInfo_1);
                CHECK_OP(os, );
            }
        }
    }
}

void writeSequence(U2OpStatus& os, const DNASequence& seq, IOAdapter* ioAdapter) {
    FastqFormat::writeEntry(seq.getName(), seq, ioAdapter, "Writing error", os);
}

void PairedFastqComparator::writePair(U2OpStatus &os, const FastqSequenceInfo &seqInfo_1, const FastqSequenceInfo &seqInfo_2) {
    SAFE_POINT_EXT(seqInfo_1.isValid() && seqInfo_2.isValid(), os.setError(tr("Invalid sequence info")), );

    writeSequence(os, seqInfo_1.getDNASeq(), out_1.data());
    CHECK_OP(os, );

    writeSequence(os, seqInfo_2.getDNASeq(), out_2.data());
    CHECK_OP(os, );

    pairsCounter++;
}

} // namespace
