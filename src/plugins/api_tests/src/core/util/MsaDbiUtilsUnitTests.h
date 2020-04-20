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


#ifndef _U2_MSA_DBI_UTILS_UNIT_TESTS_H_
#define _U2_MSA_DBI_UTILS_UNIT_TESTS_H_

#include <U2Core/MultipleSequenceAlignment.h>

#include <unittest.h>

#include "core/dbi/DbiTest.h"


namespace U2 {

class U2MsaDbi;
class U2SequenceDbi;

class Utils {
public:
    static void addRow(U2Dbi *dbi, const U2DataId &msaId,
        const QByteArray &name, const QByteArray &seq, const QList<U2MsaGap> &gaps,
        U2OpStatus &os);
};

class MsaDbiUtilsTestUtils {
public:
    static void init();
    static void shutdown();

    static U2MsaDbi* getMsaDbi();
    static U2SequenceDbi* getSequenceDbi();

    static U2EntityRef initTestAlignment(const qint64 rowConut);
    static U2EntityRef initTestAlignment(const QStringList& rowsData);
    static U2EntityRef initTestAlignment(QList<U2MsaRow>& rows);
    static QStringList getRowNames(U2EntityRef msaRef);

    static U2EntityRef removeRegionTestAlignment(U2OpStatus &os);

public:
    static const QString alignmentName;

private:
    static TestDbiProvider dbiProvider;
    static const QString& MSA_DB_URL;
    static U2MsaDbi* msaDbi;
    static U2SequenceDbi* sequenceDbi;

private:
    static U2MsaRow addRow(const QByteArray &name, const QByteArray &seq, const QList<U2MsaGap> &gaps, U2OpStatus &os);
};


/**
  * Changeing order of rows:
  * moveRows_oneLineInMiddle: shifts one row from the middle of alignment in one position up and down.
  * moveRows_oneLineInMiddleToTheTop: shifts one row from the middle of alignment (with number i)
  *     in i, i-1, i+1 positions up.
  * moveRows_oneLineInMiddleToTheBottom: shifts one row from the middle of alignment (with number i)
  *     in i, i-1, i+1 positions down.
  * moveRows_oneBlockInMiddle: shifts the block of some rows from the middle of alignement in one
  *     position up and down.
  * moveRows_oneBlockInMiddleToTheOutside: tries to shift the block of some rows from the middle of alignement
  *     beyond the borders of the alignment.
  * moveRows_twoBlocksInMiddleWithoutGluing: shifts two blocks from the middle of alignment without
  *     reaching the alignment boundary by any block.
  * moveRows_twoBlocksInMiddleWithGluing: shifts two blocks from the middle of alignment with
  *     reaching the alignment boundary by one block, another block adjoins the first block.
  * moveRows_twoBlocksOnTopWithoutGluing: shifts two blocks (one from the top, one from the middle of alignment)
  *     without reaching the alignment boundary or the top block by the middle block.
  * moveRows_twoBlocksOnTopWithGluing: shifts two blocks (one from the top, one from the middle of alignment)
  *     with reaching the alignment boundary by the middle block, blocks adjoin each other.
  * moveRows_twoBlocksOnBottomWithoutGluing: shifts two blocks (one from the bottom, one from the middle of alignment)
  *     without reaching the alignment boundary or the  bottom block by the middle block.
  * moveRows_twoBlocksOnBottomWithGluing: shifts two blocks (one from the bottom, one from the middle of alignment)
  *     with reaching the alignment boundary by the middle block, blocks adjoin each other.
  * moveRows_threeBlocksWithoutGluing: shifts three blocks (one from the top, one from the middle, one from
  *     the bottom of alignment) without adjoining the alignment boundary or other blocks by the middle block.
  * moveRows_threeBlocksWithOnceGluing: shifts three blocks (one from the top, one from the middle, one from
  *     the bottom of alignment) with adjoining one of the end block (top or botom) by the middle block.
  * moveRows_threeBlocksWithTwiceGluing: shifts three blocks (one from the top, one from the middle, one from
  *     the bottom of alignment) with adjoining all blocks into one big block.
  * moveRows_UnorderedList: unordered list of rows is given to the moveRows function.
  * moveRows_InvalidRowList: invalid row id is given to the moveRows function.
  */
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddle);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddleToTheTop);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddleToTheBottom);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_oneBlockInMiddle);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_oneBlockInMiddleToTheOutside);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksInMiddleWithoutGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksInMiddleWithGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnTopWithoutGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnTopWithGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnBottomWithoutGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnBottomWithGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithoutGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithOnceGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithTwiceGluing);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_UnorderedList);
DECLARE_TEST(MsaDbiUtilsUnitTests, moveRows_InvalidRowList);


/**
  * Trim gaps:
  * trim_noGaps: trim the alignment without trimable gaps.
  * trim_leadingGaps: trim some leading gaps.
  * trim_trailingGaps: trim some trailing gaps.
  * trim_leadingGapsCutOff: trim the leading gaps, some gaps are not deleted, but cut off.
  * trim_trailingGaps: trim the trailing gaps, some gaps are not deleted, but cut off.
  * trim_leadingAndTrailingGaps: trim the leading and trailing gaps with cut off.
  * trim_gapsOnly: trim the alignment consists from gaps only.
  */
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_noGaps);
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_leadingGaps);
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_trailingGaps);
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_leadingGapsCutOff);
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_trailingGapsCutOff);
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_leadingAndTrailingGaps);
DECLARE_TEST(MsaDbiUtilsUnitTests, trim_gapsOnly);

DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_oneRow);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_threeRows);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_lengthChange);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_allRows);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_all);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_negativePos);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_wrongId);
DECLARE_TEST(MsaDbiUtilsUnitTests, removeRegion_wrongCount);

/**
  * updateMsa - updates a Msa in a database with data from a MultipleSequenceAlignment.
  * The msa stored in the database is not empty by default.
  *     ^   empty               - there is the empty msa stored in the database, the MultipleSequenceAlignment contains new rows.
  *     ^   nothingNew          - the MultipleSequenceAlignment contains the same information.
  *     ^   newOrder            - the MultipleSequenceAlignment contains the same rows in other order.
  *     ^   newName             - the MultipleSequenceAlignment contains the same rows, the msa name changes.
  *     ^   newAlphabet         - the MultipleSequenceAlignment contains the same rows, the msa alphabet changes.
  *     ^   newContent          - the MultipleSequenceAlignment doesn't contain new rows, some rows data changes.
  *     ^   newSequence         - the MultipleSequenceAlignment doesn't contain new rows, some rows have new sequences.
  *     ^   additionalRows      - the MultipleSequenceAlignment contains new rows, no rows are deleted.
  *     ^   removeRows          - the MultipleSequenceAlignment doesn't contain some rows.
  *     ^   clear               - the MultipleSequenceAlignment is empty.
  */
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_empty);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_nothingNew);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_newOrder);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_newName);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_newAlphabet);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_newContent);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_newSequence);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_additionalRows);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_removeRows);
DECLARE_TEST(MsaDbiUtilsUnitTests, updateMsa_clear);

}   // namespace

DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddle);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddleToTheTop);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_oneLineInMiddleToTheBottom);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_oneBlockInMiddle);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_oneBlockInMiddleToTheOutside);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_twoBlocksInMiddleWithoutGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_twoBlocksInMiddleWithGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnTopWithoutGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnTopWithGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnBottomWithoutGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_twoBlocksOnBottomWithGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithoutGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithOnceGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_threeBlocksWithTwiceGluing);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_UnorderedList);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, moveRows_InvalidRowList);

DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_noGaps);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_leadingGaps);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_trailingGaps);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_leadingGapsCutOff);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_trailingGapsCutOff);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_leadingAndTrailingGaps);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, trim_gapsOnly);

DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_oneRow);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_threeRows);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_lengthChange);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_allRows);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_all);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_negativePos);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_wrongId);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, removeRegion_wrongCount);

DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_empty);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_nothingNew);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_newOrder);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_newName);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_newAlphabet);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_newContent);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_newSequence);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_additionalRows);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_removeRows);
DECLARE_METATYPE(MsaDbiUtilsUnitTests, updateMsa_clear);

#endif // _U2_MSA_DBI_UTILS_UNIT_TESTS_H_
