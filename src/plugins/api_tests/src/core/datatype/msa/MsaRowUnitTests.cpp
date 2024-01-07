/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MsaRowUnitTests.h"

#include <U2Core/DNASequence.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

const int MsaRowTestUtils::rowWithGapsLength = 7;
const int MsaRowTestUtils::rowWithGapsInMiddleLength = 8;
const int MsaRowTestUtils::rowWithoutGapsLength = 5;

const QString MsaRowTestUtils::rowWithGapsName = "Row with gaps name";

MsaRow MsaRowTestUtils::initTestRowWithGaps(Msa& almnt) {
    almnt->setName("For row with gaps");
    almnt->addRow(rowWithGapsName, "---AG-T");
    return almnt->getRow(0)->getExplicitCopy();  // "---AG-T"
}

MsaRow MsaRowTestUtils::initTestRowWithGapsInMiddle(Msa& almnt) {
    almnt->setName("For row with gaps in middle");
    almnt->addRow("Test sequence", "GG-T--AT");
    return almnt->getRow(0)->getExplicitCopy();  // "GG-T--AT"
}

MsaRow MsaRowTestUtils::initTestRowWithTrailingGaps(Msa& almnt) {
    almnt->setName("For row with trailing gaps");
    almnt->addRow("Row with trailing gaps", "CA-GT--T--");
    return almnt->getRow(0)->getExplicitCopy();  // "CA-GT--T--"
}

MsaRow MsaRowTestUtils::initTestRowWithoutGaps(Msa& almnt) {
    almnt->setName("For a row without gaps");
    almnt->addRow("Row without gaps", "ACGTA");
    return almnt->getRow(0)->getExplicitCopy();  // "ACGTA"
}

MsaRow MsaRowTestUtils::initEmptyRow(Msa& almnt) {
    almnt->setName("For empty row");
    almnt->addRow("Empty", "");
    return almnt->getRow(0)->getExplicitCopy();  // ""
}

MsaRow MsaRowTestUtils::initTestRowForModification(Msa& almnt) {
    almnt->setName("For row for modifications");
    almnt->addRow("Test sequence", "A---ACG--GTT-A-C---G");
    return almnt->getRow(0)->getExplicitCopy();  // "A---ACG--GTT-A-C---G"
}

QString MsaRowTestUtils::getRowData(const MsaRow& row) {
    U2OpStatusImpl os;
    QString result = row->toByteArray(os, row->getRowLength()).data();
    SAFE_POINT_OP(os, QString());
    return result;
}

/** Tests createRow */
IMPLEMENT_TEST(MsaRowUnitTests, createRow_fromBytes) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "--GG-A---T");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GG-A---T", QString(row->getCore()), "core data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(2, row->getCoreStart(), "core start");
    CHECK_EQUAL(10, row->getCoreEnd(), "core end");
    CHECK_EQUAL(8, row->getCoreLength(), "core length");
    CHECK_EQUAL(10, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_fromBytesTrailing) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "--GG-A---T--");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GG-A---T", QString(row->getCore()), "core data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(2, row->getCoreStart(), "core start");
    CHECK_EQUAL(10, row->getCoreEnd(), "core end");
    CHECK_EQUAL(8, row->getCoreLength(), "core length");
    CHECK_EQUAL(12, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_fromBytesGaps) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "----");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("----", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(0, row->getCoreEnd(), "core end");
    CHECK_EQUAL(0, row->getCoreLength(), "core length");
    CHECK_EQUAL(4, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_oneTrailing) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "A-");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("A-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("A", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(1, row->getCoreEnd(), "core end");
    CHECK_EQUAL(1, row->getCoreLength(), "core length");
    CHECK_EQUAL(2, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_twoTrailing) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "A--");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("A--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("A", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(1, row->getCoreEnd(), "core end");
    CHECK_EQUAL(1, row->getCoreLength(), "core length");
    CHECK_EQUAL(3, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_oneMiddleGap) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "AC-GT");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("AC-GT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("AC-GT", QString(row->getCore()), "core data");
    CHECK_EQUAL(1, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(5, row->getCoreEnd(), "core end");
    CHECK_EQUAL(5, row->getCoreLength(), "core length");
    CHECK_EQUAL(5, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_noGaps) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "ACGT");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("ACGT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("ACGT", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(4, row->getCoreEnd(), "core end");
    CHECK_EQUAL(4, row->getCoreLength(), "core length");
    CHECK_EQUAL(4, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_fromSeq) {
    Msa almnt("Test alignment");
    DNASequence sequence("Test sequence", "GGAT");
    QVector<U2MsaGap> gaps;
    U2MsaGap gapBeginning(0, 2);
    U2MsaGap gapMiddle1(4, 1);
    U2MsaGap gapMiddle2(6, 3);
    gaps << gapBeginning << gapMiddle1 << gapMiddle2;
    U2OpStatusImpl os;
    almnt->addRow("Row", sequence, gaps, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GG-A---T", QString(row->getCore()), "core data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(2, row->getCoreStart(), "core start");
    CHECK_EQUAL(10, row->getCoreEnd(), "core end");
    CHECK_EQUAL(8, row->getCoreLength(), "core length");
    CHECK_EQUAL(10, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_fromSeqTrailing) {
    Msa almnt("Test alignment");
    DNASequence sequence("Test sequence", "GGAT");
    QVector<U2MsaGap> gaps;
    U2MsaGap gapBeginning(0, 2);
    U2MsaGap gapMiddle1(4, 1);
    U2MsaGap gapMiddle2(6, 3);
    U2MsaGap gapTrailing(10, 2);
    gaps << gapBeginning << gapMiddle1 << gapMiddle2 << gapTrailing;
    U2OpStatusImpl os;
    almnt->addRow("Row", sequence, gaps, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GG-A---T", QString(row->getCore()), "core data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(2, row->getCoreStart(), "core start");
    CHECK_EQUAL(10, row->getCoreEnd(), "core end");
    CHECK_EQUAL(8, row->getCoreLength(), "core length");
    CHECK_EQUAL(12, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_fromSeqWithGaps) {
    Msa almnt("Test alignment");
    DNASequence sequence("Test sequence", "GG-AT");
    QVector<U2MsaGap> gaps;
    U2OpStatusImpl os;
    almnt->addRow("Row", sequence, gaps, os);
    CHECK_EQUAL("Failed to create a multiple alignment row", os.getError(), "opStatus");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_gapPositionTooBig) {
    Msa almnt("Test alignment");
    DNASequence sequence("Test sequence", "GGAT");
    QVector<U2MsaGap> gaps;
    U2MsaGap gapBeginning(0, 2);
    U2MsaGap gapMiddle1(4, 1);
    U2MsaGap gapMiddle2(8, 3);
    gaps << gapBeginning << gapMiddle1 << gapMiddle2;
    U2OpStatusImpl os;
    almnt->addRow("Row", sequence, gaps, os);
    CHECK_EQUAL("Failed to create a multiple alignment row", os.getError(), "opStatus");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_negativeGapPos) {
    Msa almnt("Test alignment");
    DNASequence sequence("Test sequence", "ACGT");
    QVector<U2MsaGap> gaps;
    U2MsaGap invalidGap(-1, 2);
    gaps << invalidGap;
    U2OpStatusImpl os;
    almnt->addRow("Row", sequence, gaps, os);
    CHECK_EQUAL("Failed to create a multiple alignment row", os.getError(), "opStatus");
}

IMPLEMENT_TEST(MsaRowUnitTests, createRow_negativeGapOffset) {
    Msa almnt("Test alignment");
    DNASequence sequence("Test sequence", "ACGT");
    QVector<U2MsaGap> gaps;
    U2MsaGap invalidGap(0, -1);
    gaps << invalidGap;
    U2OpStatusImpl os;
    almnt->addRow("Row", sequence, gaps, os);
    CHECK_EQUAL("Failed to create a multiple alignment row", os.getError(), "opStatus");
}

/** Tests rowName */
IMPLEMENT_TEST(MsaRowUnitTests, rowName_rowFromBytes) {
    QString rowName = "Test sequence";
    Msa almnt("Test alignment");
    almnt->addRow(rowName, "AG-T");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL(rowName, row->getName(), "name of the row");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowName_rowFromSeq) {
    QString rowName = "Test sequence";
    Msa almnt("Test alignment");
    almnt->addRow(rowName, "AGT");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL(rowName, row->getName(), "name of the row");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowName_setName) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    QString rowName = "New row name";
    row->setName(rowName);
    CHECK_EQUAL(rowName, row->getName(), "name of the row");
}

/** Tests toByteArray */
IMPLEMENT_TEST(MsaRowUnitTests, toByteArray_noGaps) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithoutGaps(almnt);
    U2OpStatusImpl os;
    QByteArray bytes = row->toByteArray(os, MsaRowTestUtils::rowWithoutGapsLength);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("ACGTA", QString(bytes), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, toByteArray_gapsInBeginningAndMiddle) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    QByteArray bytes = row->toByteArray(os, MsaRowTestUtils::rowWithGapsLength);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---AG-T", QString(bytes), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, toByteArray_incorrectLength) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    QByteArray bytes = row->toByteArray(os, MsaRowTestUtils::rowWithGapsLength - 1);
    CHECK_EQUAL("Failed to get row data", os.getError(), "opStatus");
    CHECK_EQUAL("", QString(bytes), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, toByteArray_greaterLength) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    QByteArray bytes = row->toByteArray(os, MsaRowTestUtils::rowWithGapsLength + 1);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---AG-T-", QString(bytes), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, toByteArray_trailing) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "--GG-A---T--");
    MsaRow row = almnt->getRow(0);
    QByteArray bytes = row->toByteArray(os, 12);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("--GG-A---T--", QString(bytes), "row data");
}

/** Tests simplify */
IMPLEMENT_TEST(MsaRowUnitTests, simplify_gaps) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "--GG-A---T--");
    bool result = almnt->simplify();
    MsaRow row = almnt->getRow(0);
    CHECK_TRUE(result, "simplify() must have returned 'true'!");
    CHECK_EQUAL("GGAT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GGAT", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(4, row->getCoreEnd(), "core end");
    CHECK_EQUAL(4, row->getCoreLength(), "core length");
    CHECK_EQUAL(4, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, simplify_nothingToRemove) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithoutGaps(almnt);
    bool result = almnt->simplify();
    CHECK_FALSE(result, "simplify() must have returned 'false'!");
    CHECK_EQUAL("ACGTA", MsaRowTestUtils::getRowData(row), "row data");
}

/** Tests append */
IMPLEMENT_TEST(MsaRowUnitTests, append_noGapBetweenRows) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    Msa almnt2;
    MsaRow anotherRow = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt2);
    U2OpStatusImpl os;
    almnt->appendRow(0, row->getRowLength(), anotherRow, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---AG-TGG-T--AT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "gaps number");
    CHECK_EQUAL("GG-T--AT", MsaRowTestUtils::getRowData(anotherRow), "another row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, append_gapBetweenRows) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    Msa almnt2;
    MsaRow anotherRow = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt2);
    U2OpStatusImpl os;
    almnt->appendRow(0, row->getRowLength() + 1, anotherRow, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---AG-T-GG-T--AT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(5, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, append_offsetInAnotherRow) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    Msa almnt2;
    MsaRow anotherRow = MsaRowTestUtils::initTestRowWithGaps(almnt2);
    U2OpStatusImpl os;
    almnt->appendRow(0, row->getRowLength() + 2, anotherRow, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG-T--AT-----AG-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, append_trailingInFirst) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithTrailingGaps(almnt);
    Msa almnt2;
    MsaRow anotherRow = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt2);
    U2OpStatusImpl os;
    almnt->appendRow(0, row->getRowLength() + 1, anotherRow, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("CA-GT--T---GG-T--AT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(5, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, append_trailingAndOffset) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithTrailingGaps(almnt);
    Msa almnt2;
    MsaRow anotherRow = MsaRowTestUtils::initTestRowWithGaps(almnt2);
    U2OpStatusImpl os;
    almnt->appendRow(0, row->getRowLength(), anotherRow, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("CA-GT--T-----AG-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, append_invalidLength) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    Msa almnt2;
    MsaRow anotherRow = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt2);
    U2OpStatusImpl os;
    almnt->appendRow(0, row->getRowLength() - 1, anotherRow, os);
    row = almnt->getRow(0);
    CHECK_EQUAL("Failed to append one row to another", os.getError(), "opStatus");
    CHECK_EQUAL("---AG-T", MsaRowTestUtils::getRowData(row), "row data");
}

/** Tests setRowContent */
IMPLEMENT_TEST(MsaRowUnitTests, setRowContent_empty) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->setRowContent(0, "");
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(MsaRowTestUtils::rowWithGapsName, row->getName(), "row name");
    CHECK_EQUAL("-------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(0, row->getCoreEnd(), "core end");
    CHECK_EQUAL(0, row->getCoreLength(), "core length");
    CHECK_EQUAL(7, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, setRowContent_trailingGaps) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->setRowContent(0, "--GG-A---T--");
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(MsaRowTestUtils::rowWithGapsName, row->getName(), "row name");
    CHECK_EQUAL("--GG-A---T--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GG-A---T", QString(row->getCore()), "core data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(2, row->getCoreStart(), "core start");
    CHECK_EQUAL(10, row->getCoreEnd(), "core end");
    CHECK_EQUAL(8, row->getCoreLength(), "core length");
    CHECK_EQUAL(12, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, setRowContent_offsetNoGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->setRowContent(0, "AC-GT", 1);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(MsaRowTestUtils::rowWithGapsName, row->getName(), "row name");
    CHECK_EQUAL("-AC-GT-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("AC-GT", QString(row->getCore()), "core data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(1, row->getCoreStart(), "core start");
    CHECK_EQUAL(6, row->getCoreEnd(), "core end");
    CHECK_EQUAL(5, row->getCoreLength(), "core length");
    CHECK_EQUAL(7, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, setRowContent_offsetGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->setRowContent(0, "--GG", 1);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(MsaRowTestUtils::rowWithGapsName, row->getName(), "row name");
    CHECK_EQUAL("---GG--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("GG", QString(row->getCore()), "core data");
    CHECK_EQUAL(1, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(3, row->getCoreStart(), "core start");
    CHECK_EQUAL(5, row->getCoreEnd(), "core end");
    CHECK_EQUAL(2, row->getCoreLength(), "core length");
    CHECK_EQUAL(7, row->getRowLength(), "row length");
}

IMPLEMENT_TEST(MsaRowUnitTests, setRowContent_emptyAndOffset) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->setRowContent(0, "", 1);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(MsaRowTestUtils::rowWithGapsName, row->getName(), "row name");
    CHECK_EQUAL("-------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL("", QString(row->getCore()), "core data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
    CHECK_EQUAL(0, row->getCoreStart(), "core start");
    CHECK_EQUAL(0, row->getCoreEnd(), "core end");
    CHECK_EQUAL(0, row->getCoreLength(), "core length");
    CHECK_EQUAL(7, row->getRowLength(), "row length");
}

/** Tests insertGaps */
IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_empty) {
    Msa almnt;
    MsaRowTestUtils::initEmptyRow(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 0, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("--", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toGapPosLeft) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 3, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG--T--AT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");

    almnt->insertGaps(0, 7, 2, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG--T----AT", MsaRowTestUtils::getRowData(row), "row data (second insertion)");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toGapPosRight) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 2, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG--T--AT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");

    almnt->insertGaps(0, 5, 2, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG--T----AT", MsaRowTestUtils::getRowData(row), "row data (second insertion)");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toGapPosInside) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 1, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("-----AG-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_insideChars) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 4, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---A--G-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toZeroPosNoGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 0, 3, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---GG-T--AT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toZeroPosGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 0, 3, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("------AG-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toLastPosNoGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 7, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG-T--A--T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(3, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toLastPosGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithTrailingGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 9, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("CA-GT--T---", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_toLastPosOneGap) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "A-");
    almnt->insertGaps(0, 1, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_noGapsYet) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithoutGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 4, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("ACGT-A", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_onlyGaps) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "--");
    almnt->insertGaps(0, 1, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("----", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_oneChar) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("One-char sequence", "A");
    almnt->insertGaps(0, 0, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("--A", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_tooBigPosition) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 10, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_EQUAL("---AG-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "gaps number");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_negativePosition) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    row->insertGaps(-1, 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---AG-T", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, insertGaps_negativeNumOfChars) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->insertGaps(0, 1, -1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_EQUAL("---AG-T", MsaRowTestUtils::getRowData(row), "row data");
}

/** Tests removeRowData */
IMPLEMENT_TEST(MsaRowUnitTests, remove_empty) {
    Msa almnt;
    MsaRowTestUtils::initEmptyRow(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 0, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideGap1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 2, 15, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---G---------------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideGap2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 3, 15, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---G---------------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_leftGapSide) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 7, 9, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---ACG---G---------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_rightGapSide) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 4, 11, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---C---G-----------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideSeq1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 5, 6, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---AT-A-C---G------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideSeq2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 6, 4, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---ACTT-A-C---G----", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_fromZeroPosGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 0, 4, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("G-T----", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_fromZeroPosChar) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 0, 17, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("--G-----------------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_lastPosExactly) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 7, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG-T--A-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_fromLastPos) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 7, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG-T--A-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideOneGap1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 2, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A-ACG--GTT-A-C---G--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(5, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideOneGap2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 2, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A--ACG--GTT-A-C---G-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(5, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideOneGapLong) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Test sequence", "A------GT--C-T");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("A------GT--C-T", MsaRowTestUtils::getRowData(row), "row data");
    almnt->removeChars(0, 2, 3, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---GT--C-T---", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(3, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideTrailingGap) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "AC-GT----");
    almnt->removeChars(0, 5, 2, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("AC-GT----", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_insideCharsOne) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 5, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---AG--GTT-A-C---G-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(5, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_negativePosition) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, -1, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_EQUAL("A---ACG--GTT-A-C---G", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_negativeNumOfChars) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 1, -1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_EQUAL("A---ACG--GTT-A-C---G", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_gapsAtRowEnd1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 9, 12, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---ACG-------------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_gapsAtRowEnd2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 3, 21, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A-------------------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_onlyGapsAfterRemove) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 2, 9, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("-------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_emptyAfterRemove) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 0, 21, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("--------------------", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, remove_oneCharInGaps) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->removeChars(0, 13, 1, os);
    MsaRow row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A---ACG--GTT--C---G-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "number of gaps");
}

/** Tests charAt */
IMPLEMENT_TEST(MsaRowUnitTests, charAt_allCharsNoOffset) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    char ch = row->charAt(-1);
    CHECK_EQUAL('-', ch, "char -1");

    ch = row->charAt(0);
    CHECK_EQUAL('G', ch, "char 0");

    ch = row->charAt(1);
    CHECK_EQUAL('G', ch, "char 1");

    ch = row->charAt(2);
    CHECK_EQUAL('-', ch, "char 2");

    ch = row->charAt(3);
    CHECK_EQUAL('T', ch, "char 3");

    ch = row->charAt(4);
    CHECK_EQUAL('-', ch, "char 4");

    ch = row->charAt(5);
    CHECK_EQUAL('-', ch, "char 5");

    ch = row->charAt(6);
    CHECK_EQUAL('A', ch, "char 6");

    ch = row->charAt(7);
    CHECK_EQUAL('T', ch, "char 7");

    ch = row->charAt(8);
    CHECK_EQUAL('-', ch, "char 8");

    ch = row->charAt(9);
    CHECK_EQUAL('-', ch, "char 9");
}

IMPLEMENT_TEST(MsaRowUnitTests, charAt_offsetAndTrailing) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "-AC-");
    MsaRow row = almnt->getRow(0);

    char ch = row->charAt(-1);
    CHECK_EQUAL('-', ch, "char -1");

    ch = row->charAt(0);
    CHECK_EQUAL('-', ch, "char 0");

    ch = row->charAt(1);
    CHECK_EQUAL('A', ch, "char 1");

    ch = row->charAt(2);
    CHECK_EQUAL('C', ch, "char 2");

    ch = row->charAt(3);
    CHECK_EQUAL('-', ch, "char 3");

    ch = row->charAt(4);
    CHECK_EQUAL('-', ch, "char 4");
}

IMPLEMENT_TEST(MsaRowUnitTests, charAt_onlyCharsInRow) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "ACG");
    MsaRow row = almnt->getRow(0);

    char ch = row->charAt(-1);
    CHECK_EQUAL('-', ch, "char -1");

    ch = row->charAt(0);
    CHECK_EQUAL('A', ch, "char 0");

    ch = row->charAt(1);
    CHECK_EQUAL('C', ch, "char 1");

    ch = row->charAt(2);
    CHECK_EQUAL('G', ch, "char 2");

    ch = row->charAt(3);
    CHECK_EQUAL('-', ch, "char 3");
}

/** Tests rowEqual */
IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_sameContent) {
    Msa almnt;
    MsaRow firstRow = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    MsaRow secondRow = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_TRUE(result, "The first and the second rows are NOT equal unexpectedly!");

    CHECK_TRUE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_FALSE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_noGaps) {
    Msa almnt("Test alignment");
    almnt->addRow("First", "ACT");
    MsaRow firstRow = almnt->getRow(0);
    almnt->addRow("Second", "ACT");
    MsaRow secondRow = almnt->getRow(1);

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_TRUE(result, "The first and the second rows are NOT equal unexpectedly!");

    CHECK_TRUE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_FALSE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_trailingInFirst) {
    Msa almnt("Test alignment");
    almnt->addRow("First", "AC-GT-");
    MsaRow firstRow = almnt->getRow(0);
    Msa almnt2("Test alignment");
    almnt2->addRow("Second", "AC-GT");
    MsaRow secondRow = almnt2->getRow(0);

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_TRUE(result, "The first and the second rows are NOT equal unexpectedly!");

    CHECK_TRUE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_FALSE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_trailingInSecond) {
    Msa almnt("Test alignment");
    almnt->addRow("First", "AC-GT");
    MsaRow firstRow = almnt->getRow(0);
    Msa almnt2("Test alignment");
    almnt2->addRow("Second", "AC-GT--");
    MsaRow secondRow = almnt2->getRow(0);

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_TRUE(result, "The first and the second rows are NOT equal unexpectedly!");

    CHECK_TRUE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_FALSE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_trailingInBoth) {
    Msa almnt("Test alignment");
    almnt->addRow("First", "AC-GT---");
    MsaRow firstRow = almnt->getRow(0);
    Msa almnt2("Test alignment");
    almnt2->addRow("Second", "AC-GT--");
    MsaRow secondRow = almnt2->getRow(0);

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_TRUE(result, "The first and the second rows are NOT equal unexpectedly!");

    CHECK_TRUE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_FALSE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_diffGapModelsGap) {
    Msa almnt("Test alignment");
    almnt->addRow("Test sequence", "--GG-A---T");
    MsaRow firstRow = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T", MsaRowTestUtils::getRowData(firstRow), "first row data");
    Msa almnt2("Test alignment");
    almnt2->addRow("Test sequence", "--GG--A---T");
    MsaRow secondRow = almnt2->getRow(0);
    CHECK_EQUAL("--GG--A---T", MsaRowTestUtils::getRowData(secondRow), "second row data");

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_FALSE(result, "The first and the second rows are EQUAL unexpectedly!");

    CHECK_FALSE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_TRUE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_diffGapModelsOffset) {
    Msa almnt("Test alignment");
    almnt->addRow("Test sequence", "--GG-A---T");
    MsaRow firstRow = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T", MsaRowTestUtils::getRowData(firstRow), "first row data");
    Msa almnt2("Test alignment");
    almnt2->addRow("Test sequence", "--G-GA---T");
    MsaRow secondRow = almnt2->getRow(0);
    CHECK_EQUAL("--G-GA---T", MsaRowTestUtils::getRowData(secondRow), "second row data");

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_FALSE(result, "The first and the second rows are EQUAL unexpectedly!");

    CHECK_FALSE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_TRUE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_diffNumOfGaps) {
    Msa almnt("Test alignment");
    almnt->addRow("Test sequence", "--GG-A---T");
    MsaRow firstRow = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T", MsaRowTestUtils::getRowData(firstRow), "first row data");
    Msa almnt2("Test alignment");
    almnt2->addRow("Test sequence", "--GG-AT");
    MsaRow secondRow = almnt2->getRow(0);
    CHECK_EQUAL("--GG-AT", MsaRowTestUtils::getRowData(secondRow), "second row data");

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_FALSE(result, "The first and the second rows are EQUAL unexpectedly!");

    CHECK_FALSE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_TRUE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

IMPLEMENT_TEST(MsaRowUnitTests, rowsEqual_diffSequences) {
    Msa almnt("Test alignment");
    almnt->addRow("Test sequence", "--GG-A---T");
    MsaRow firstRow = almnt->getRow(0);
    CHECK_EQUAL("--GG-A---T", MsaRowTestUtils::getRowData(firstRow), "first row data");
    almnt->addRow("Test sequence", "--GG-C---T");
    MsaRow secondRow = almnt->getRow(1);
    CHECK_EQUAL("--GG-C---T", MsaRowTestUtils::getRowData(secondRow), "second row data");

    bool result = firstRow->isEqualCore(*secondRow);
    CHECK_FALSE(result, "The first and the second rows are EQUAL unexpectedly!");

    CHECK_FALSE(*firstRow == *secondRow, "Incorrect 'operator=='!");
    CHECK_TRUE(*firstRow != *secondRow, "Incorrect 'operator!='!");
}

/** Tests ungapped */
IMPLEMENT_TEST(MsaRowUnitTests, ungapped_rowWithoutOffset) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    CHECK_EQUAL(5, row->getUngappedLength(), "ungapped length");
    CHECK_EQUAL(-1, row->getUngappedPosition(-1), "pos -1");
    CHECK_EQUAL(0, row->getUngappedPosition(0), "pos 0");
    CHECK_EQUAL(1, row->getUngappedPosition(1), "pos 1");
    CHECK_EQUAL(-1, row->getUngappedPosition(2), "pos 2");
    CHECK_EQUAL(2, row->getUngappedPosition(3), "pos 3");
    CHECK_EQUAL(-1, row->getUngappedPosition(4), "pos 4");
    CHECK_EQUAL(-1, row->getUngappedPosition(5), "pos 5");
    CHECK_EQUAL(3, row->getUngappedPosition(6), "pos 6");
    CHECK_EQUAL(4, row->getUngappedPosition(7), "pos 7");
    CHECK_EQUAL(-1, row->getUngappedPosition(8), "pos 8");
}

IMPLEMENT_TEST(MsaRowUnitTests, ungapped_offsetTrailing) {
    Msa almnt("Test alignment");
    almnt->addRow("Test row", "---AG-T-");
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL(3, row->getUngappedLength(), "ungapped length");
    CHECK_EQUAL(-1, row->getUngappedPosition(-1), "pos -1");
    CHECK_EQUAL(-1, row->getUngappedPosition(0), "pos 0");
    CHECK_EQUAL(-1, row->getUngappedPosition(1), "pos 1");
    CHECK_EQUAL(-1, row->getUngappedPosition(2), "pos 2");
    CHECK_EQUAL(0, row->getUngappedPosition(3), "pos 3");
    CHECK_EQUAL(1, row->getUngappedPosition(4), "pos 4");
    CHECK_EQUAL(-1, row->getUngappedPosition(5), "pos 5");
    CHECK_EQUAL(2, row->getUngappedPosition(6), "pos 6");
    CHECK_EQUAL(-1, row->getUngappedPosition(7), "pos 7");
    CHECK_EQUAL(-1, row->getUngappedPosition(8), "pos 8");
}

/** Tests crop */
IMPLEMENT_TEST(MsaRowUnitTests, crop_empty) {
    Msa almnt;
    MsaRowTestUtils::initEmptyRow(almnt);
    U2OpStatusImpl os;
    almnt->crop({0, 1}, os);
    CHECK_TRUE(os.getError().contains("Incorrect region was passed to MultipleSequenceAlignmentData::crop"),
               QString("opStatus is %1").arg(os.getError()));
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideGap1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({2, 15}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--ACG--GTT-A-C-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideGap2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({3, 15}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("-ACG--GTT-A-C--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_leftGapSide) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({7, 9}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--GTT-A-C", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(3, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_rightGapSide) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({4, 11}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("ACG--GTT-A-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideSeq1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({5, 6}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("CG--GT", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideSeq2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({6, 4}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("G--G", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_fromZeroPosGap) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->crop({0, 4}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("---A", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(1, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_fromZeroPosChar) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({0, 17}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("A---ACG--GTT-A-C-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(4, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_lastPosExactly) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->crop({7, 1}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_fromLastPos) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    almnt->crop({7, 2}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideOneGap1) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({2, 2}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideOneGap2) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({2, 1}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideOneGapLong) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Test sequence", "A------GT--C-T");
    MsaRow row = almnt->getRow(0);

    CHECK_EQUAL("A------GT--C-T", MsaRowTestUtils::getRowData(row), "row data");
    almnt->crop({2, 3}, os);
    row = almnt->getRow(0);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("---", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_insideCharsOne) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({5, 1}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("C", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_negativePosition) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({-1, 1}, os);
    MsaRow row = almnt->getRow(0);
    CHECK_TRUE(os.getError().contains("Incorrect region was passed to MultipleSequenceAlignmentData::crop"),
               QString("opStatus is %1").arg(os.getError()));
    CHECK_EQUAL("A---ACG--GTT-A-C---G", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_negativeNumOfChars) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({1, -1}, os);
    MsaRow row = almnt->getRow(0);
    CHECK_TRUE(os.getError().contains("Incorrect region was passed to MultipleSequenceAlignmentData::crop"),
               QString("opStatus is %1").arg(os.getError()));
    CHECK_EQUAL("A---ACG--GTT-A-C---G", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_trailing) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithTrailingGaps(almnt);
    U2OpStatusImpl os;
    almnt->crop({2, 8}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("-GT--T--", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_trailingToGaps) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithTrailingGaps(almnt);
    U2OpStatusImpl os;
    almnt->crop({0, 9}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("CA-GT--T-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_cropTrailing) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithTrailingGaps(almnt);
    U2OpStatusImpl os;
    almnt->crop({9, 1}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("-", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_oneCharInGaps) {
    Msa almnt;
    MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    almnt->crop({13, 1}, os);
    CHECK_NO_ERROR(os);
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL("A", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(0, row->getGaps().count(), "number of gaps");
}

IMPLEMENT_TEST(MsaRowUnitTests, crop_posMoreThanLength) {
    Msa almnt;
    MsaRowTestUtils::initTestRowWithGaps(almnt);
    U2OpStatusImpl os;
    almnt->crop({13, 1}, os);
    CHECK_TRUE(os.getError().contains("Incorrect region was passed to MultipleSequenceAlignmentData::crop"),
               QString("opStatus is %1").arg(os.getError()));
    MsaRow row = almnt->getRow(0);
    CHECK_EQUAL(7, row->getRowLength(), "row length");
    CHECK_EQUAL("---AG-T", MsaRowTestUtils::getRowData(row), "row data");
    CHECK_EQUAL(2, row->getGaps().count(), "number of gaps");
}

/** Tests mid */
IMPLEMENT_TEST(MsaRowUnitTests, mid_general) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowForModification(almnt);
    U2OpStatusImpl os;
    MsaRow result = row->mid(4, 8, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("ACG--GTT------------", MsaRowTestUtils::getRowData(result), "row data");
    CHECK_EQUAL(1, result->getGaps().count(), "number of gaps");
}

/** Tests upperCase */
IMPLEMENT_TEST(MsaRowUnitTests, upperCase_general) {
    Msa almnt("Test alignment");
    almnt->addRow("Row name", "avn-*y-s");
    MsaRow row = almnt->getRow(0);
    row->toUpperCase();
    CHECK_EQUAL("AVN-*Y-S", MsaRowTestUtils::getRowData(row), "row data");

    QString actualRowName = row->getName();
    CHECK_EQUAL("Row name", actualRowName, "row name");
}

/** Tests replaceChars */
IMPLEMENT_TEST(MsaRowUnitTests, replaceChars_charToChar) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    row->replaceChars('T', 'C', os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG-C--AC", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, replaceChars_nothingToReplace) {
    Msa almnt;
    MsaRow row = MsaRowTestUtils::initTestRowWithGapsInMiddle(almnt);
    U2OpStatusImpl os;
    row->replaceChars('~', '-', os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("GG-T--AT", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, replaceChars_tildasToGapsNoGaps) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Row name", "A~~CC~~~AG~AC~TG");
    MsaRow row = almnt->getRow(0);
    row->replaceChars('~', '-', os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A--CC---AG-AC-TG", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, replaceChars_tildasToGapsWithGaps) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Row name", "A~-CC~-~AG~AC-TG");
    MsaRow row = almnt->getRow(0);
    row->replaceChars('~', '-', os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A--CC---AG-AC-TG", MsaRowTestUtils::getRowData(row), "row data");
}

IMPLEMENT_TEST(MsaRowUnitTests, replaceChars_trailingGaps) {
    U2OpStatusImpl os;
    Msa almnt("Test alignment");
    almnt->addRow("Row name", "A~~CC~~~AG~AC~TG~");
    MsaRow row = almnt->getRow(0);
    row->replaceChars('~', '-', os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("A--CC---AG-AC-TG-", MsaRowTestUtils::getRowData(row), "row data");
}

}  // namespace U2
