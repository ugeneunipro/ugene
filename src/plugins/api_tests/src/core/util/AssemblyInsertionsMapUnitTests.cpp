/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "AssemblyInsertionsMapUnitTests.h"

#include <U2Core/U2AssemblyInsertionsMap.h>
#include <U2Core/U2AssemblyReadIterator.h>

namespace U2 {

namespace {

/** Builds a read with an explicit CIGAR: 'sequence' must already match what the CIGAR consumes. */
U2AssemblyRead makeRead(qint64 leftmostPos, const QByteArray& sequence, const QList<U2CigarToken>& cigar) {
    U2AssemblyRead read(new U2AssemblyReadData());
    read->leftmostPos = leftmostPos;
    read->readSequence = sequence;
    read->cigar = cigar;
    return read;
}

QList<U2CigarToken> cigarOf(U2CigarOp op1, int count1, U2CigarOp op2 = U2CigarOp_Invalid, int count2 = 0, U2CigarOp op3 = U2CigarOp_Invalid, int count3 = 0) {
    QList<U2CigarToken> cigar;
    cigar << U2CigarToken(op1, count1);
    if (op2 != U2CigarOp_Invalid) {
        cigar << U2CigarToken(op2, count2);
    }
    if (op3 != U2CigarOp_Invalid) {
        cigar << U2CigarToken(op3, count3);
    }
    return cigar;
}

}  // namespace

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, noReadsGiveNoColumns) {
    U2AssemblyInsertionsMap map;
    map.build(QList<U2AssemblyRead>(), U2Region(100, 10));

    CHECK_TRUE(map.isEmpty(), "map is not empty");
    CHECK_EQUAL(10, map.getColumnCount(), "column count");
    CHECK_EQUAL(0, map.getColumnOfRefPos(100), "column of the first position");
    CHECK_EQUAL(7, map.getColumnOfRefPos(107), "column of a position");
    CHECK_EQUAL(107, map.getRefPosOfColumn(7), "position of a column");
    CHECK_EQUAL(0, map.getInsertionWidthBefore(107), "insertion width");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionInTheMiddleOfTheRead) {
    // 5M3I5M: the 3 inserted letters sit between the reference positions 104 and 105.
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_FALSE(map.isEmpty(), "map is empty");
    CHECK_EQUAL(1, map.getSites().size(), "site count");
    CHECK_EQUAL(105, map.getSites().first().refPos, "site position");
    CHECK_EQUAL(3, map.getSites().first().width, "site width");

    CHECK_EQUAL(3, map.getInsertionWidthBefore(105), "insertion width before 105");
    CHECK_EQUAL(0, map.getInsertionWidthBefore(104), "insertion width before 104");

    // Positions before the insertion keep their columns, the ones after are shifted by 3.
    CHECK_EQUAL(4, map.getColumnOfRefPos(104), "column of 104");
    CHECK_EQUAL(8, map.getColumnOfRefPos(105), "column of 105");
    CHECK_EQUAL(13, map.getColumnCount(), "column count");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, widestInsertionDefinesTheWidth) {
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    reads << makeRead(100, "AAAAAGGGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 4, U2CigarOp_M, 5));
    reads << makeRead(100, "AAAAAGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 2, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(1, map.getSites().size(), "site count");
    CHECK_EQUAL(4, map.getInsertionWidthBefore(105), "insertion width");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionAtTheReadStart) {
    // 3I5M: nothing of the read is aligned before the insertion, it still belongs to position 100.
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "GGGAAAAA", cigarOf(U2CigarOp_I, 3, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(1, map.getSites().size(), "site count");
    CHECK_EQUAL(100, map.getSites().first().refPos, "site position");
    CHECK_EQUAL(3, map.getSites().first().width, "site width");
    CHECK_EQUAL(3, map.getColumnOfRefPos(100), "column of 100");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, trailingInsertionIsIgnored) {
    // 5M3I: there is no reference position after the insertion, so there is no column to put it in.
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGGG", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_TRUE(map.isEmpty(), "map is not empty");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, softClipIsNotAnInsertion) {
    // A soft clip is not aligned to the reference at all, it must not reserve any column.
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "GGGAAAAACC", cigarOf(U2CigarOp_S, 3, U2CigarOp_M, 5, U2CigarOp_S, 2));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_TRUE(map.isEmpty(), "map is not empty");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, hugeInsertionIsTruncated) {
    int hugeLength = U2AssemblyInsertionsMap::MAX_SITE_WIDTH + 100;
    QByteArray sequence = QByteArray("AAAAA") + QByteArray(hugeLength, 'G') + QByteArray("CCCCC");
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, sequence, cigarOf(U2CigarOp_M, 5, U2CigarOp_I, hugeLength, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(U2AssemblyInsertionsMap::MAX_SITE_WIDTH, map.getInsertionWidthBefore(105), "insertion width");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionOutsideTheRegionIsIgnored) {
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5));

    // The insertion belongs to position 105, a region that ends earlier must not see it.
    U2AssemblyInsertionsMap mapBefore;
    mapBefore.build(reads, U2Region(100, 5));
    CHECK_TRUE(mapBefore.isEmpty(), "region before the insertion is not empty");

    // The region starts right at the insertion, which is cut off by the region border.
    U2AssemblyInsertionsMap mapAfter;
    mapAfter.build(reads, U2Region(105, 5));
    CHECK_TRUE(mapAfter.isEmpty(), "region after the insertion is not empty");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, columnsAndPositionsMatchEachOther) {
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    for (qint64 refPos = 100; refPos < 110; refPos++) {
        qint64 column = map.getColumnOfRefPos(refPos);
        CHECK_EQUAL(refPos, map.getRefPosOfColumn(column), "position of its own column");
    }

    // Every column of the insertion belongs to the position the insertion is attached to.
    CHECK_EQUAL(104, map.getRefPosOfColumn(4), "position of the last plain column");
    CHECK_EQUAL(105, map.getRefPosOfColumn(5), "position of the first inserted column");
    CHECK_EQUAL(105, map.getRefPosOfColumn(6), "position of an inserted column");
    CHECK_EQUAL(105, map.getRefPosOfColumn(7), "position of the last inserted column");
    CHECK_EQUAL(105, map.getRefPosOfColumn(8), "position of the reference column");
    CHECK_EQUAL(106, map.getRefPosOfColumn(9), "position of the next column");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, readIteratorReportsInsertedLetters) {
    QByteArray sequence = "AAAAAGGGCCCCC";
    U2AssemblyReadIterator it(sequence, cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5));

    QByteArray letters;
    QByteArray insertions;
    while (it.hasNext()) {
        letters += it.nextLetter();
        insertions += it.getInsertionBeforeLastLetter();
    }
    CHECK_EQUAL(QString("AAAAACCCCC"), QString(letters), "letters");
    CHECK_EQUAL(QString("GGG"), QString(insertions), "inserted letters");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, readIteratorSkipsInsertionsBeforeStartPos) {
    QByteArray sequence = "AAAAAGGGCCCCC";
    QList<U2CigarToken> cigar = cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5);

    // Starting at the position the insertion is attached to: it is on the left of the first
    // returned letter, so it has no column to be drawn in and must not be reported.
    U2AssemblyReadIterator it(sequence, cigar, 5);
    CHECK_TRUE(it.hasNext(), "iterator is exhausted");
    CHECK_EQUAL('C', it.nextLetter(), "first letter");
    CHECK_TRUE(it.getInsertionBeforeLastLetter().isEmpty(), "insertion is reported before the start position");

    // Starting inside the first match: the insertion is still ahead and must be reported.
    U2AssemblyReadIterator it2(sequence, cigar, 4);
    CHECK_EQUAL('A', it2.nextLetter(), "first letter");
    CHECK_TRUE(it2.getInsertionBeforeLastLetter().isEmpty(), "unexpected insertion");
    CHECK_EQUAL('C', it2.nextLetter(), "second letter");
    CHECK_EQUAL(QString("GGG"), QString(it2.getInsertionBeforeLastLetter()), "inserted letters");
}

}  // namespace U2
