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

#include "MsaDbiUnitTests.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/SQLiteDbi.h>


namespace U2 {

TestDbiProvider MsaTestData::dbiProvider = TestDbiProvider();
const QString& MsaTestData::MSA_DB_URL("msa-dbi.ugenedb");
U2MsaDbi* MsaTestData::msaDbi = NULL;
U2SequenceDbi* MsaTestData::sequenceDbi = NULL;


void MsaTestData::init() {
    SAFE_POINT(NULL == msaDbi, "msaDbi has been already initialized!", );
    SAFE_POINT(NULL == sequenceDbi, "sequenceDbi has been already initialized!", );

    bool ok = dbiProvider.init(MSA_DB_URL, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize in MsaTestData::init()!",);

    U2Dbi* dbi = dbiProvider.getDbi();
    msaDbi = dbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "Failed to get msaDbi!",);

    sequenceDbi = dbi->getSequenceDbi();
    SAFE_POINT(NULL != sequenceDbi, "Failed to get sequenceDbi!",);
}

void MsaTestData::shutdown() {
    if (NULL != msaDbi) {
        SAFE_POINT(NULL != sequenceDbi, "sequenceDbi must also be not NULL on this step!", );

        U2OpStatusImpl os;
        dbiProvider.close();
        msaDbi = NULL;
        sequenceDbi = NULL;
        SAFE_POINT_OP(os, );
    }
}

U2MsaDbi* MsaTestData::getMsaDbi() {
    if (NULL == msaDbi) {
        init();
    }
    return msaDbi;
}

U2SequenceDbi* MsaTestData::getSequenceDbi() {
    if (NULL == sequenceDbi) {
        init();
    }
    return sequenceDbi;
}

IMPLEMENT_TEST(MsaDbiUnitTests, createMsaObject) {
    U2MsaDbi* msaDbi = MsaTestData::getMsaDbi();

    U2AlphabetId testAlphabet = BaseDNAAlphabetIds::AMINO_DEFAULT();

    U2OpStatusImpl os;
    U2DataId msaId = msaDbi->createMsaObject("", "Test name", testAlphabet, os);
    CHECK_NO_ERROR(os);

    const U2Msa& actual = msaDbi->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(testAlphabet.id, actual.alphabet.id, "alphabet");
    CHECK_EQUAL(0, actual.length, "length");
    CHECK_EQUAL(msaId, actual.id, "id");

    qint64 actualNumOfRows = msaDbi->getNumOfRows(msaId, os);
    CHECK_EQUAL(0, actualNumOfRows, "number of rows");
}

IMPLEMENT_TEST(MsaDbiUnitTests, addRows) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaTestData::getMsaDbi();

    // Create an alignment
    U2AlphabetId alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2DataId msaId = msaDbi->createMsaObject("", "Test name", alphabet, os);
    CHECK_NO_ERROR(os);

    // Create sequences
    U2SequenceDbi* sequenceDbi = MsaTestData::getSequenceDbi();
    U2Sequence seq1;
    U2Sequence seq2;
    sequenceDbi->createSequenceObject(seq1, "", os);
    CHECK_NO_ERROR(os);
    sequenceDbi->createSequenceObject(seq2, "", os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow row1;
    row1.sequenceId = seq1.id;
    row1.gstart = 0;
    row1.gend = 5;

    U2MsaGap row1gap1(0, 2);
    U2MsaGap row1gap2(3, 1);
    QList<U2MsaGap> row1gaps;
    row1gaps << row1gap1 << row1gap2;

    row1.gaps = row1gaps;

    U2MsaRow row2;
    row2.sequenceId = seq2.id;
    row2.gstart = 2;
    row2.gend = 4;

    QList<U2MsaRow> rows;
    rows << row1 << row2;

    msaDbi->addRows(msaId, rows, os);
    CHECK_NO_ERROR(os);

    // Get the number of rows
    qint64 actualNumOfRows = msaDbi->getNumOfRows(msaId, os);
    CHECK_EQUAL(2, actualNumOfRows, "number of rows");

    // Get the rows
    QList<U2MsaRow> actualRows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, actualRows.count(), "number of rows");

    const U2MsaRow& actualRow1 = actualRows[0];
    CHECK_EQUAL(rows.at(0).rowId, actualRow1.rowId, "first row id");
    CHECK_EQUAL(seq1.id, actualRow1.sequenceId, "first row sequence id");
    CHECK_EQUAL(0, actualRow1.gstart, "first row global start");
    CHECK_EQUAL(5, actualRow1.gend, "first row global end");
    CHECK_EQUAL(2, actualRow1.gaps.count(), "first row gaps count");
    U2MsaGap actualRow1Gap1 = actualRow1.gaps[0];
    CHECK_EQUAL(0, actualRow1Gap1.offset, "first row gap1 offset");
    CHECK_EQUAL(2, actualRow1Gap1.gap, "first row gap1 length");
    U2MsaGap actualRow1Gap2 = actualRow1.gaps[1];
    CHECK_EQUAL(3, actualRow1Gap2.offset, "first row gap2 offset");
    CHECK_EQUAL(1, actualRow1Gap2.gap, "first row gap2 length");

    const U2MsaRow& actualRow2 = actualRows[1];
    CHECK_EQUAL(rows.at(1).rowId, actualRow2.rowId, "second row id");
    CHECK_EQUAL(seq2.id, actualRow2.sequenceId, "second row sequence id");
    CHECK_EQUAL(2, actualRow2.gstart, "second row global start");
    CHECK_EQUAL(4, actualRow2.gend, "second row global end");
    CHECK_EQUAL(0, actualRow2.gaps.count(), "second row gaps");
}

IMPLEMENT_TEST(MsaDbiUnitTests, removeRows) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = MsaTestData::getMsaDbi();

    // Create an alignment
    U2DataId msaId = msaDbi->createMsaObject("", "Test name", BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);

    // Create sequences
    U2SequenceDbi* sequenceDbi = MsaTestData::getSequenceDbi();
    U2Sequence seq1;
    U2Sequence seq2;
    U2Sequence seq3;
    sequenceDbi->createSequenceObject(seq1, "", os);
    CHECK_NO_ERROR(os);
    sequenceDbi->createSequenceObject(seq2, "", os);
    CHECK_NO_ERROR(os);
    sequenceDbi->createSequenceObject(seq3, "", os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow row1;
    row1.rowId = 0;
    row1.sequenceId = seq1.id;
    row1.gstart = 0;
    row1.gend = 5;

    U2MsaGap row1gap1(0, 2);
    U2MsaGap row1gap2(3, 1);
    QList<U2MsaGap> row1gaps;
    row1gaps << row1gap1 << row1gap2;

    row1.gaps = row1gaps;

    U2MsaRow row2;
    row2.sequenceId = seq2.id;
    row2.gstart = 2;
    row2.gend = 4;

    U2MsaGap row2gap(1, 2);
    QList<U2MsaGap> row2gaps;
    row2gaps << row2gap;

    row2.gaps = row2gaps;

    U2MsaRow row3;
    row3.sequenceId = seq3.id;
    row3.gstart = 0;
    row3.gend = 10;


    QList<U2MsaRow> rows;
    rows << row1 << row2 << row3;

    msaDbi->addRows(msaId, rows, os);
    CHECK_NO_ERROR(os);

    // Remove the rows
    QList<qint64> rowsToRemove;
    rowsToRemove << rows.at(0).rowId << rows.at(2).rowId;

    msaDbi->removeRows(msaId, rowsToRemove, os);
    CHECK_NO_ERROR(os);

    // Get the number of rows
    qint64 actualNumOfRows = msaDbi->getNumOfRows(msaId, os);
    CHECK_EQUAL(1, actualNumOfRows, "number of rows");

    // Get the rows
    QList<U2MsaRow> actualRows = msaDbi->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, actualRows.count(), "number of rows");

    const U2MsaRow& actualRow = actualRows[0];
    CHECK_EQUAL(rows.at(1).rowId, actualRow.rowId, "row id");
    CHECK_EQUAL(seq2.id, actualRow.sequenceId, "row sequence id");
    CHECK_EQUAL(2, actualRow.gstart, "row global start");
    CHECK_EQUAL(4, actualRow.gend, "row global end");
    CHECK_EQUAL(1, actualRow.gaps.count(), "row gaps");
    U2MsaGap actualRowGap = actualRow.gaps[0];
    CHECK_EQUAL(1, actualRowGap.offset, "row gap offset");
    CHECK_EQUAL(2, actualRowGap.gap, "row gap length");
}

} // namespace
