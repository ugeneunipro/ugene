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

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2AssemblyInsertionsMap.h>
#include <U2Core/U2AssemblyReadIterator.h>

#include <U2View/AssemblyBrowserSettings.h>

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

/** Every read of the filter tests covers [0, 20), its insertion (if any) precedes 'basesBefore'. */
U2AssemblyRead makeInsertionRead(const QByteArray& insertion, qint64 leftmostPos = 0, int basesBefore = 10) {
    const int basesAfter = 20 - basesBefore;
    QList<U2CigarToken> cigar;
    if (basesBefore > 0) {
        cigar << U2CigarToken(U2CigarOp_M, basesBefore);
    }
    if (!insertion.isEmpty()) {
        cigar << U2CigarToken(U2CigarOp_I, insertion.size());
    }
    cigar << U2CigarToken(U2CigarOp_M, basesAfter);
    return makeRead(leftmostPos, QByteArray(basesBefore, 'A') + insertion + QByteArray(basesAfter, 'A'), cigar);
}

QList<U2AssemblyRead> makeInsertionReads(int count, const QByteArray& insertion) {
    QList<U2AssemblyRead> reads;
    for (int i = 0; i < count; i++) {
        reads << makeInsertionRead(insertion);
    }
    return reads;
}

const U2Region FILTER_TEST_REGION(0, 20);

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

    // The region starts right after the position the insertion is attached to.
    U2AssemblyInsertionsMap mapAfter;
    mapAfter.build(reads, U2Region(106, 5));
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

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionOfTheFirstPositionIsKept) {
    // The insertion belongs to the position 105 and a read that starts earlier only reports it
    // after the position before it has been walked through. The map walks that position itself,
    // so a region that begins right at 105 still has the insertion: the columns of a position must
    // not depend on where the view is scrolled to or where the consensus export splits the assembly.
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(105, 5));
    CHECK_EQUAL(3, map.getInsertionWidthBefore(105), "insertion width");
    CHECK_EQUAL(QString("GGG"), QString(map.getInsertionConsensus(105)), "insertion consensus");
    CHECK_EQUAL(3, map.getColumnOfRefPos(105), "column of the first position");
    CHECK_EQUAL(1, map.getSites().first().coverage, "coverage of the site");

    // The base walked through before the region is not a part of it: nothing in front of it counts.
    U2AssemblyInsertionsMap mapOfTheNextPosition;
    mapOfTheNextPosition.build(reads, U2Region(106, 4));
    CHECK_TRUE(mapOfTheNextPosition.isEmpty(), "the insertion of the previous position is reported");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionColumnsAreToldFromTheReferenceOnes) {
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 3, U2CigarOp_M, 5));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    // The columns 5, 6 and 7 are the extra ones, everything else is a real reference position.
    CHECK_EQUAL(-1, map.getInsertionOffsetOfColumn(4), "offset of the last plain column");
    CHECK_EQUAL(0, map.getInsertionOffsetOfColumn(5), "offset of the first inserted column");
    CHECK_EQUAL(1, map.getInsertionOffsetOfColumn(6), "offset of an inserted column");
    CHECK_EQUAL(2, map.getInsertionOffsetOfColumn(7), "offset of the last inserted column");
    CHECK_EQUAL(-1, map.getInsertionOffsetOfColumn(8), "offset of the reference column of 105");
    CHECK_EQUAL(-1, map.getInsertionOffsetOfColumn(9), "offset of the next column");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertedLettersOutvoteTheGaps) {
    // Three reads insert a 'G' before the position 105, one covers the position without inserting.
    QList<U2AssemblyRead> reads;
    for (int i = 0; i < 3; i++) {
        reads << makeRead(100, "AAAAAGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    }
    reads << makeRead(100, "AAAAACCCCC", cigarOf(U2CigarOp_M, 10));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(QString("G"), QString(map.getInsertionConsensus(105)), "consensus of the inserted column");
    CHECK_EQUAL(3, map.getInsertionCoverage(105, 0), "coverage of the inserted column");
    CHECK_EQUAL(4, map.getSites().first().coverage, "coverage of the site position");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, gapsOutvoteTheInsertedLetters) {
    // The column is only there because a single read has a letter in it: everything else is a gap.
    QList<U2AssemblyRead> reads;
    reads << makeRead(100, "AAAAAGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    for (int i = 0; i < 3; i++) {
        reads << makeRead(100, "AAAAACCCCC", cigarOf(U2CigarOp_M, 10));
    }

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(1, map.getInsertionWidthBefore(105), "insertion width");
    CHECK_EQUAL(QString("-"), QString(map.getInsertionConsensus(105)), "consensus of the inserted column");
    CHECK_EQUAL(1, map.getInsertionCoverage(105, 0), "coverage of the inserted column");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, everyInsertionColumnIsCountedOnItsOwn) {
    // Three reads insert "GG", two more insert a single "G", one read has no insertion at all.
    QList<U2AssemblyRead> reads;
    for (int i = 0; i < 3; i++) {
        reads << makeRead(100, "AAAAAGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 2, U2CigarOp_M, 5));
    }
    for (int i = 0; i < 2; i++) {
        reads << makeRead(100, "AAAAAGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    }
    reads << makeRead(100, "AAAAACCCCC", cigarOf(U2CigarOp_M, 10));

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(2, map.getInsertionWidthBefore(105), "insertion width");
    CHECK_EQUAL(5, map.getInsertionCoverage(105, 0), "coverage of the first inserted column");
    CHECK_EQUAL(3, map.getInsertionCoverage(105, 1), "coverage of the second inserted column");
    // 5 letters against 1 gap in the first column, 3 letters against 3 gaps in the second one:
    // a tie is not a majority, the column stays a gap.
    CHECK_EQUAL(QString("G-"), QString(map.getInsertionConsensus(105)), "consensus of the inserted columns");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, theMostFrequentInsertedLetterWins) {
    // Every read covering the position has an insertion, so the gaps have no vote at all here.
    QList<U2AssemblyRead> reads;
    for (int i = 0; i < 3; i++) {
        reads << makeRead(100, "AAAAAACCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    }
    for (int i = 0; i < 2; i++) {
        reads << makeRead(100, "AAAAACCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    }

    U2AssemblyInsertionsMap map;
    map.build(reads, U2Region(100, 10));

    CHECK_EQUAL(QString("A"), QString(map.getInsertionConsensus(105)), "consensus of the inserted column");
    CHECK_EQUAL(5, map.getInsertionCoverage(105, 0), "coverage of the inserted column");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, incrementalBuildMatchesTheBatchOne) {
    QList<U2AssemblyRead> reads;
    for (int i = 0; i < 3; i++) {
        reads << makeRead(100, "AAAAAGGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 2, U2CigarOp_M, 5));
    }
    reads << makeRead(100, "AAAAAGCCCCC", cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 1, U2CigarOp_M, 5));
    reads << makeRead(102, "AAATTTCCC", cigarOf(U2CigarOp_M, 3, U2CigarOp_I, 3, U2CigarOp_M, 3));

    U2AssemblyInsertionsMap batchMap;
    batchMap.build(reads, U2Region(100, 10));

    // The export walks the reads of a region one by one, it cannot hold them all in memory.
    U2AssemblyInsertionsMap incrementalMap;
    incrementalMap.startBuild(U2Region(100, 10));
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        incrementalMap.addSitesOfRead(read);
    }
    incrementalMap.finishSites();
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        incrementalMap.addStatisticsOfRead(read);
    }
    incrementalMap.finishStatistics();

    CHECK_EQUAL(batchMap.getSites().size(), incrementalMap.getSites().size(), "site count");
    CHECK_EQUAL(batchMap.getColumnCount(), incrementalMap.getColumnCount(), "column count");
    for (const U2AssemblyInsertionSite& site : qAsConst(batchMap.getSites())) {
        CHECK_EQUAL(site.width, incrementalMap.getInsertionWidthBefore(site.refPos), "insertion width");
        CHECK_EQUAL(QString(site.consensus), QString(incrementalMap.getInsertionConsensus(site.refPos)), "insertion consensus");
        CHECK_EQUAL(site.coverage, incrementalMap.getSiteBefore(site.refPos)->coverage, "site coverage");
    }
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

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, filterThresholds) {
    U2AssemblyInsertionsFilter none;
    CHECK_TRUE(none.isValid(), "the default filter is not valid");
    CHECK_FALSE(none.isEnabled(), "the default filter is enabled");
    CHECK_TRUE(none.passes(1, 1000000), "the default filter drops an insertion");

    // A single read is no requirement: every insertion is found in at least one.
    CHECK_FALSE(U2AssemblyInsertionsFilter(0, 1).isEnabled(), "1 read is a requirement");
    CHECK_TRUE(U2AssemblyInsertionsFilter(0, 2).isEnabled(), "2 reads are no requirement");
    CHECK_TRUE(U2AssemblyInsertionsFilter(0.1, 0).isEnabled(), "0.1% is no requirement");

    U2AssemblyInsertionsFilter defaults = U2AssemblyInsertionsFilter::getDefault();
    CHECK_EQUAL(2.0, defaults.minPercent, "default percent");
    CHECK_EQUAL(10, defaults.minReads, "default reads");

    // Both thresholds have to be met, each of them inclusively.
    CHECK_TRUE(defaults.passes(10, 500), "exactly 2% and exactly 10 reads do not pass");
    CHECK_FALSE(defaults.passes(9, 100), "9 reads pass");
    CHECK_FALSE(defaults.passes(10, 501), "less than 2% passes");
    CHECK_TRUE(defaults.passes(11, 501), "11 reads of 501 do not pass");

    // A rounded percent still lets the exact share pass.
    CHECK_TRUE(U2AssemblyInsertionsFilter(100.0 / 3, 0).passes(1, 3), "1 read of 3 does not pass 33.(3)%");
    CHECK_FALSE(U2AssemblyInsertionsFilter(33.4, 0).passes(1, 3), "1 read of 3 passes 33.4%");

    CHECK_FALSE(U2AssemblyInsertionsFilter(-1, 0).isValid(), "a negative percent is valid");
    CHECK_FALSE(U2AssemblyInsertionsFilter(100.5, 0).isValid(), "a percent above 100 is valid");
    CHECK_FALSE(U2AssemblyInsertionsFilter(0, -1).isValid(), "a negative number of reads is valid");
    CHECK_FALSE(U2AssemblyInsertionsFilter(qQNaN(), 0).isValid(), "NaN percent is valid");
    CHECK_TRUE(U2AssemblyInsertionsFilter(100, 0).isValid(), "100% is not valid");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, filterNeedsEnoughReads) {
    QList<U2AssemblyRead> reads = makeInsertionReads(9, "C") + makeInsertionReads(11, "");
    U2AssemblyInsertionsMap map;
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_TRUE(map.isEmpty(), "an insertion of 9 reads is shown");
    CHECK_EQUAL(20, map.getColumnCount(), "column count");

    reads << makeInsertionRead("C");
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_EQUAL(1, map.getInsertionWidthBefore(10), "insertion width");
    CHECK_EQUAL(QString("-"), QString(map.getInsertionConsensus(10)), "consensus of 10 letters against 11 gaps");

    // With no filter a single read is enough, as before the filter was there.
    reads = makeInsertionReads(99, "") << makeInsertionRead("GT");
    map.build(reads, FILTER_TEST_REGION);
    CHECK_EQUAL(2, map.getInsertionWidthBefore(10), "insertion width with no filter");
    CHECK_EQUAL(100, map.getSites().first().coverage, "coverage");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, filterNeedsEnoughShareOfTheCoverage) {
    // The share is taken of the reads covering the position, the ones with no insertion included.
    QList<U2AssemblyRead> reads = makeInsertionReads(98, "") + makeInsertionReads(2, "C");
    U2AssemblyInsertionsMap map;
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(2, 0));
    CHECK_EQUAL(1, map.getInsertionWidthBefore(10), "insertion of 2% is not shown");

    // A read that does not reach the position is not a vote for a gap.
    reads << makeInsertionRead("", 11);
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(2, 0));
    CHECK_EQUAL(1, map.getInsertionWidthBefore(10), "a read not covering the position is counted");

    reads << makeInsertionRead("");
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(2, 0));
    CHECK_EQUAL(0, map.getInsertionWidthBefore(10), "insertion of less than 2% is shown");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, distinctInsertionsAreCountedSeparately) {
    // 'C' and 'CC' are different insertions: neither of them supports the other.
    QList<U2AssemblyRead> reads = makeInsertionReads(6, "C") + makeInsertionReads(6, "CC");
    U2AssemblyInsertionsMap map;
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_TRUE(map.isEmpty(), "'C' and 'CC' support each other");

    // The same letters of another case are the same insertion and the same consensus vote.
    reads = makeInsertionReads(5, "ct") + makeInsertionReads(5, "CT") + makeInsertionReads(6, "");
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_EQUAL(2, map.getInsertionWidthBefore(10), "insertion width");
    CHECK_EQUAL(QString("CT"), QString(map.getInsertionConsensus(10)), "consensus of 'ct' and 'CT'");

    // Insertions only sharing the beginning are different, even if both are cut to the same width.
    QByteArray common(U2AssemblyInsertionsMap::MAX_SITE_WIDTH, 'G');
    reads = makeInsertionReads(6, common + "A") + makeInsertionReads(6, common + "T");
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_TRUE(map.isEmpty(), "insertions with a common beginning support each other");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, columnsOfPassedInsertionsHoldEveryRead) {
    // The columns are as wide as the longest insertion that passes. Inside them every read shows
    // its letters and votes with them, whether its own insertion passed or not.
    QList<U2AssemblyRead> reads = makeInsertionReads(10, "CC") + makeInsertionReads(3, "C") + makeInsertionReads(2, "CCCCC") + makeInsertionReads(5, "");
    U2AssemblyInsertionsMap map;
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_EQUAL(2, map.getInsertionWidthBefore(10), "insertion width");
    CHECK_EQUAL(15, map.getInsertionCoverage(10, 0), "letters in the first column");
    CHECK_EQUAL(12, map.getInsertionCoverage(10, 1), "letters in the second column");
    CHECK_EQUAL(QString("CC"), QString(map.getInsertionConsensus(10)), "insertion consensus");
    CHECK_EQUAL(2, map.getSites().first().letterCounts.size(), "size of the letter counts");
    CHECK_EQUAL(22, map.getColumnCount(), "column count");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, coordinatesAfterFiltering) {
    // The columns of the sites left are where they would be with no filtered site at all.
    QList<U2AssemblyRead> reads;
    for (int i = 0; i < 10; i++) {
        reads << makeInsertionRead("TT", 0, 5) << makeInsertionRead("G", 0, 15);
    }
    reads << makeInsertionRead("A", 0, 10);

    U2AssemblyInsertionsMap map;
    map.build(reads, FILTER_TEST_REGION, U2AssemblyInsertionsFilter(0, 10));
    CHECK_EQUAL(2, map.getSites().size(), "site count");
    CHECK_TRUE(map.getSiteBefore(10) == nullptr, "the filtered site is there");
    CHECK_EQUAL(23, map.getColumnCount(), "column count");

    CHECK_EQUAL(4, map.getColumnOfRefPos(4), "column of 4");
    CHECK_EQUAL(7, map.getColumnOfRefPos(5), "column of 5");
    CHECK_EQUAL(12, map.getColumnOfRefPos(10), "column of 10");
    CHECK_EQUAL(18, map.getColumnOfRefPos(15), "column of 15");
    CHECK_EQUAL(22, map.getColumnOfRefPos(19), "column of 19");

    CHECK_EQUAL(5, map.getRefPosOfColumn(5), "position of the column 5");
    CHECK_EQUAL(1, map.getInsertionOffsetOfColumn(6), "offset of the column 6");
    CHECK_EQUAL(10, map.getRefPosOfColumn(12), "position of the column 12");
    CHECK_EQUAL(-1, map.getInsertionOffsetOfColumn(12), "offset of the column 12");
    CHECK_EQUAL(15, map.getRefPosOfColumn(17), "position of the column 17");
    CHECK_EQUAL(0, map.getInsertionOffsetOfColumn(17), "offset of the column 17");
    CHECK_EQUAL(19, map.getRefPosOfColumn(22), "position of the column 22");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, filteredIncrementalBuildMatchesTheBatchOne) {
    QList<U2AssemblyRead> reads = makeInsertionReads(10, "CC") + makeInsertionReads(3, "A") + makeInsertionReads(5, "");
    U2AssemblyInsertionsFilter filter(10, 10);

    U2AssemblyInsertionsMap listed;
    listed.build(reads, FILTER_TEST_REGION, filter);

    U2AssemblyInsertionsMap streamed;
    streamed.startBuild(FILTER_TEST_REGION, filter);
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        streamed.addSitesOfRead(read);
    }
    streamed.finishSites();
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        streamed.addStatisticsOfRead(read);
    }
    streamed.finishStatistics();

    CHECK_EQUAL(listed.getSites().size(), streamed.getSites().size(), "site count");
    CHECK_EQUAL(2, streamed.getInsertionWidthBefore(10), "insertion width");
    CHECK_EQUAL(QString(listed.getInsertionConsensus(10)), QString(streamed.getInsertionConsensus(10)), "insertion consensus");
    CHECK_EQUAL(13, streamed.getInsertionCoverage(10, 0), "letters in the first column");
    CHECK_TRUE(streamed.getFilter() == filter, "the filter of the map");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionOfTheFirstPositionNextToOtherTokens) {
    // The base walked through before the region can be anything that takes a reference position:
    // a match, a deletion, a skipped region. Clipped letters are never a part of the insertion.
    struct Case {
        QString name;
        qint64 regionStart;
        QList<U2CigarToken> cigar;
        QByteArray sequence;
        qint64 sitePos;
    };
    const QList<Case> cases = {
        {"5M2I3D5M", 5, cigarOf(U2CigarOp_M, 5, U2CigarOp_I, 2, U2CigarOp_D, 3) << U2CigarToken(U2CigarOp_M, 5), "AAAAAGTCCCCC", 5},
        {"5M3D2I5M", 8, cigarOf(U2CigarOp_M, 5, U2CigarOp_D, 3, U2CigarOp_I, 2) << U2CigarToken(U2CigarOp_M, 5), "AAAAAGTCCCCC", 8},
        {"5M3N2I5M", 8, cigarOf(U2CigarOp_M, 5, U2CigarOp_N, 3, U2CigarOp_I, 2) << U2CigarToken(U2CigarOp_M, 5), "AAAAAGTCCCCC", 8},
        {"2S2I5M", 0, cigarOf(U2CigarOp_S, 2, U2CigarOp_I, 2, U2CigarOp_M, 5), "TTGTCCCCC", 0},
    };
    for (const Case& c : qAsConst(cases)) {
        QList<U2AssemblyRead> reads;
        for (int i = 0; i < 10; i++) {
            reads << makeRead(0, c.sequence, c.cigar);
        }
        U2AssemblyInsertionsMap map;
        map.build(reads, U2Region(c.regionStart, 3), U2AssemblyInsertionsFilter(2, 10));
        CHECK_EQUAL(1, map.getSites().size(), c.name + ": site count");
        CHECK_EQUAL(c.sitePos, map.getSites().first().refPos, c.name + ": site position");
        CHECK_EQUAL(10, map.getSites().first().coverage, c.name + ": coverage");
        CHECK_EQUAL(QString("GT"), QString(map.getInsertionConsensus(c.sitePos)), c.name + ": insertion consensus");
        CHECK_EQUAL(10, map.getInsertionCoverage(c.sitePos, 1), c.name + ": letters in the second column");
    }
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, regionsToAddOfTheMovedMap) {
    U2AssemblyInsertionsMap map;
    map.build(QList<U2AssemblyRead>(), U2Region(100, 50));
    CHECK_TRUE(map.getRegionsToAdd(U2Region(110, 50)) == QVector<U2Region>({U2Region(150, 10)}), "scrolled right");
    CHECK_TRUE(map.getRegionsToAdd(U2Region(90, 50)) == QVector<U2Region>({U2Region(90, 10)}), "scrolled left");
    CHECK_TRUE(map.getRegionsToAdd(U2Region(90, 70)) == QVector<U2Region>({U2Region(90, 10), U2Region(150, 10)}), "zoomed out");
    CHECK_TRUE(map.getRegionsToAdd(U2Region(110, 10)).isEmpty(), "zoomed in");
    CHECK_TRUE(map.getRegionsToAdd(U2Region(150, 10)) == QVector<U2Region>({U2Region(150, 10)}), "jumped away");
    CHECK_TRUE(map.getRegionsToAdd(U2Region()).isEmpty(), "empty region");
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, movedMapMatchesTheBuiltOne) {
    // Reads of random starts with random insertions, a few positions with frequent ones.
    QList<U2AssemblyRead> reads;
    quint32 seed = 229;
    auto next = [&seed](int bound) {
        seed = seed * 1103515245 + 12345;
        return int((seed >> 16) % bound);
    };
    for (int i = 0; i < 2000; i++) {
        const qint64 start = next(300);
        QList<U2CigarToken> cigar;
        QByteArray sequence;
        int matches = 0;
        for (qint64 pos = start; pos < start + 40; pos++) {
            const bool frequent = pos % 37 == 0 && next(3) == 0;
            if (matches > 0 && (frequent || next(50) == 0)) {
                const int size = frequent ? 2 : 1 + next(3);
                cigar << U2CigarToken(U2CigarOp_M, matches) << U2CigarToken(U2CigarOp_I, size);
                for (int j = 0; j < size; j++) {
                    sequence += frequent ? 'G' : "ACGT"[next(4)];
                }
                matches = 0;
            }
            sequence += 'A';
            matches++;
        }
        cigar << U2CigarToken(U2CigarOp_M, matches);
        reads << makeRead(start, sequence, cigar);
    }

    // Scrolled both ways, zoomed in and out, jumped away: always the same as built from scratch.
    const QList<U2AssemblyInsertionsFilter> filters = {U2AssemblyInsertionsFilter(), U2AssemblyInsertionsFilter::getDefault(), U2AssemblyInsertionsFilter(20, 5)};
    const QList<U2Region> path = {U2Region(101, 80), U2Region(120, 80), U2Region(90, 80), U2Region(110, 30), U2Region(60, 150), U2Region(250, 60), U2Region(0, 340)};
    for (const U2AssemblyInsertionsFilter& filter : qAsConst(filters)) {
        U2AssemblyInsertionsMap moved;
        moved.build(reads, U2Region(100, 80), filter);
        for (const U2Region& region : qAsConst(path)) {
            QList<U2AssemblyInsertionsMap> parts;
            for (const U2Region& regionToAdd : moved.getRegionsToAdd(region)) {
                parts << U2AssemblyInsertionsMap();
                parts.last().build(reads, regionToAdd, filter);
            }
            moved.moveTo(region, parts);

            U2AssemblyInsertionsMap built;
            built.build(reads, region, filter);
            const QString where = QString("min %1%, %2 reads, region %3").arg(filter.minPercent).arg(filter.minReads).arg(region.toString());
            CHECK_FALSE(built.isEmpty(), where + ": no insertions to compare");
            CHECK_TRUE(moved.getRegion() == region, where + ": region");
            CHECK_EQUAL(built.getColumnCount(), moved.getColumnCount(), where + ": column count");
            CHECK_EQUAL(built.getSites().size(), moved.getSites().size(), where + ": site count");
            for (int i = 0; i < built.getSites().size(); i++) {
                const U2AssemblyInsertionSite& expected = built.getSites().at(i);
                const U2AssemblyInsertionSite& actual = moved.getSites().at(i);
                CHECK_EQUAL(expected.refPos, actual.refPos, where + ": site position");
                CHECK_EQUAL(expected.width, actual.width, where + ": site width");
                CHECK_EQUAL(expected.coverage, actual.coverage, where + ": site coverage");
                CHECK_TRUE(expected.letterCounts == actual.letterCounts, where + ": letter counts");
                CHECK_EQUAL(QString(expected.consensus), QString(actual.consensus), where + ": consensus");
            }
            for (qint64 pos = region.startPos; pos < region.endPos(); pos++) {
                CHECK_EQUAL(built.getColumnOfRefPos(pos), moved.getColumnOfRefPos(pos), where + ": column of a position");
            }
        }
    }
}

IMPLEMENT_TEST(AssemblyInsertionsMapUnitTests, insertionsFilterSettings) {
    static const QString PERCENT_KEY = "assembly_browser/insertions_min_percent";
    static const QString READS_KEY = "assembly_browser/insertions_min_reads";
    Settings* settings = AppContext::getSettings();

    // The test changes the settings of the running UGENE: put them back whatever the result is.
    struct SettingsRestorer {
        SettingsRestorer(Settings* settings)
            : settings(settings) {
            for (const QString& key : {PERCENT_KEY, READS_KEY}) {
                if (settings->contains(key)) {
                    values[key] = settings->getValue(key);
                }
            }
        }
        ~SettingsRestorer() {
            for (const QString& key : {PERCENT_KEY, READS_KEY}) {
                if (values.contains(key)) {
                    settings->setValue(key, values[key]);
                } else {
                    settings->remove(key);
                }
            }
        }
        Settings* settings;
        QMap<QString, QVariant> values;
    } restorer(settings);

    settings->remove(PERCENT_KEY);
    settings->remove(READS_KEY);
    CHECK_TRUE(AssemblyBrowserSettings::getInsertionsFilter() == U2AssemblyInsertionsFilter::getDefault(), "nothing saved: not the default filter");

    AssemblyBrowserSettings::setInsertionsFilter(U2AssemblyInsertionsFilter(7.5, 3));
    U2AssemblyInsertionsFilter saved = AssemblyBrowserSettings::getInsertionsFilter();
    CHECK_EQUAL(7.5, saved.minPercent, "saved percent");
    CHECK_EQUAL(3, saved.minReads, "saved reads");

    // A broken or hand-edited value must not break the view: the defaults are used instead.
    settings->setValue(PERCENT_KEY, "not a number");
    CHECK_TRUE(AssemblyBrowserSettings::getInsertionsFilter() == U2AssemblyInsertionsFilter::getDefault(), "a broken percent is used");
    settings->setValue(PERCENT_KEY, 150);
    CHECK_TRUE(AssemblyBrowserSettings::getInsertionsFilter() == U2AssemblyInsertionsFilter::getDefault(), "a percent above 100 is used");
    settings->setValue(PERCENT_KEY, 5);
    settings->setValue(READS_KEY, -1);
    CHECK_TRUE(AssemblyBrowserSettings::getInsertionsFilter() == U2AssemblyInsertionsFilter::getDefault(), "a negative number of reads is used");
}

}  // namespace U2
