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

#include "MsaUnitTests.h"
#include "MsaRowUnitTests.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatusUtils.h>

#include <QSet>


namespace U2 {

const int MsaTestUtils::rowsNum = 2;
const int MsaTestUtils::firstRowLength = 7;
const int MsaTestUtils::secondRowLength = 9;

const QString MsaTestUtils::alignmentName = "Test alignment name";


MultipleSequenceAlignment MsaTestUtils::initTestAlignment() {
    DNAAlphabetRegistry* alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    const DNAAlphabet* alphabet = alphabetRegistry->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());

    QByteArray firstSequence("---AG-T");
    QByteArray secondSequence("AG-CT-TAA");

    MultipleSequenceAlignment almnt(alignmentName, alphabet);

    almnt->addRow("First row", firstSequence);
    almnt->addRow("Second row", secondSequence);

    return almnt;
}

QString MsaTestUtils::getRowData(const MultipleSequenceAlignment& almnt, int rowNum) {
    if (rowNum < 0 || rowNum > almnt->getNumRows()) {
        return "";
    }

    MultipleSequenceAlignmentRow row = almnt->getMsaRow(rowNum);

    return MsaRowTestUtils::getRowData(row);
}

bool MsaTestUtils::testAlignmentNotChanged(const MultipleSequenceAlignment& almnt) {
    if (9 != almnt->getLength()) {
        return false;
    }

    if ("---AG-T--" != MsaTestUtils::getRowData(almnt, 0)) {
        return false;
    }

    if ("AG-CT-TAA" != MsaTestUtils::getRowData(almnt, 1)) {
        return false;
    }

    return true;
}


/** Tests clear */
IMPLEMENT_TEST(MsaUnitTests, clear_notEmpty) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->clear();
    QList<MultipleSequenceAlignmentRow> rows = almnt->getMsaRows();
    CHECK_EQUAL(0, rows.count(), "number of rows");
}

/** Tests name */
IMPLEMENT_TEST(MsaUnitTests, name_ctor) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    CHECK_EQUAL(MsaTestUtils::alignmentName, almnt->getName(), "alignment name");
}

IMPLEMENT_TEST(MsaUnitTests, name_setName) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    QString newName = "Another name";
    almnt->setName(newName);
    CHECK_EQUAL(newName, almnt->getName(), "alignment name");
}

/** Tests alphabet */
IMPLEMENT_TEST(MsaUnitTests, alphabet_ctor) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    if (NULL == almnt->getAlphabet()) {
        SetError("NULL alphabet");
    }
    CHECK_EQUAL(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), almnt->getAlphabet()->getId(), "alphabet ID");
}

IMPLEMENT_TEST(MsaUnitTests, alphabet_setAlphabet) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();

    DNAAlphabetRegistry* alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    const DNAAlphabet* newAlphabet = alphabetRegistry->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    almnt->setAlphabet(newAlphabet);

    if (NULL == almnt->getAlphabet() || NULL == newAlphabet) {
        SetError("NULL alphabet");
    }
    CHECK_EQUAL(newAlphabet->getId(), almnt->getAlphabet()->getId(), "new alignment ID");
}

/** Tests info */
IMPLEMENT_TEST(MsaUnitTests, info_setGet) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    QVariantMap info;
    QString infoElementName = "Test element name";
    QString infoElementValue = "Test element value";
    info.insert(infoElementName, infoElementValue);
    almnt->setInfo(info);
    QVariantMap actualInfo = almnt->getInfo();
    QString actualValue = actualInfo.value(infoElementName).value<QString>();
    CHECK_EQUAL(infoElementValue, actualValue, "info element value");
}

/** Tests length */
IMPLEMENT_TEST(MsaUnitTests, length_isEmptyFalse) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    CHECK_FALSE(almnt->isEmpty(), "Method isEmpty() returned 'true' unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, length_isEmptyTrue) {
    MultipleSequenceAlignment almnt;
    CHECK_TRUE(almnt->isEmpty(), "Method isEmpty() returned 'false' unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, length_get) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    int expectedLength = 9; // the length of the longest row
    CHECK_EQUAL(expectedLength, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, length_getForEmpty) {
    MultipleSequenceAlignment almnt;
    CHECK_EQUAL(0, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, length_setLessLength) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    int newLength = 5;
    almnt->setLength(newLength);
    CHECK_EQUAL(newLength, almnt->getLength(), "alignment length");

    CHECK_EQUAL("---AG", MsaTestUtils::getRowData(almnt, 0), "first row data");
    CHECK_EQUAL("AG-CT", MsaTestUtils::getRowData(almnt, 1), "second row data");
}

/** Tests numOfRows */
IMPLEMENT_TEST(MsaUnitTests, numOfRows_notEmpty) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    CHECK_EQUAL(MsaTestUtils::rowsNum, almnt->getNumRows(), "number of rows");
}

IMPLEMENT_TEST(MsaUnitTests, numOfRows_empty) {
    MultipleSequenceAlignment almnt;
    CHECK_EQUAL(0, almnt->getNumRows(), "number of rows");
}

/** Tests trim */
IMPLEMENT_TEST(MsaUnitTests, trim_biggerLength) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    int newLength = 100;
    almnt->setLength(newLength);
    bool result = almnt->trim();
    CHECK_TRUE(result, "Method trim() returned 'false' unexpectedly");
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
    CHECK_EQUAL("AG-CT-TAA", MsaTestUtils::getRowData(almnt, 1), "row data");
}

IMPLEMENT_TEST(MsaUnitTests, trim_leadingGapColumns) {
    QByteArray firstSequence("---AG-T");
    QByteArray secondSequence("--AG-CT-TA");

    MultipleSequenceAlignment almnt("Alignment with leading gap columns", NULL);
    almnt->addRow("First row", firstSequence);
    almnt->addRow("Second row", secondSequence);

    bool result = almnt->trim();

    CHECK_TRUE(result, "Method trim() returned 'false' unexpectedly");
    CHECK_EQUAL(8, almnt->getLength(), "alignment length");
    CHECK_EQUAL("-AG-T---", MsaTestUtils::getRowData(almnt, 0), "first row data");
    CHECK_EQUAL("AG-CT-TA", MsaTestUtils::getRowData(almnt, 1), "second row data");
}

IMPLEMENT_TEST(MsaUnitTests, trim_nothingToTrim) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    bool result = almnt->trim();
    CHECK_FALSE(result, "Method trim() returned 'true' unexpectedly");
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
    CHECK_EQUAL("AG-CT-TAA", MsaTestUtils::getRowData(almnt, 1), "row data");
}

IMPLEMENT_TEST(MsaUnitTests, trim_rowWithoutGaps) {
    QByteArray seq("ACGTAGTCGATC");

    MultipleSequenceAlignment almnt("Alignment", NULL);
    almnt->addRow("Row without gaps", seq);

    bool result = almnt->trim();

    CHECK_FALSE(result, "Method trim() returned 'true' unexpectedly");
    CHECK_EQUAL(12, almnt->getLength(), "alignment length");
    CHECK_EQUAL("ACGTAGTCGATC", MsaTestUtils::getRowData(almnt, 0), "row data");
}

IMPLEMENT_TEST(MsaUnitTests, trim_empty) {
    MultipleSequenceAlignment almnt;
    bool result = almnt->trim();
    CHECK_FALSE(result, "Method trim() returned 'true' unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, trim_trailingGapInOne) {
    QByteArray firstSequence("ACGT");
    QByteArray secondSequence("CAC-");

    MultipleSequenceAlignment almnt("Alignment", NULL);
    almnt->addRow("First row", firstSequence);
    almnt->addRow("Second row", secondSequence);

    bool result = almnt->trim();

    CHECK_FALSE(result, "Method trim() returned 'true' unexpectedly");
    CHECK_EQUAL(4, almnt->getLength(), "alignment length");
    CHECK_EQUAL("ACGT", MsaTestUtils::getRowData(almnt, 0), "first row data");
    CHECK_EQUAL("CAC-", MsaTestUtils::getRowData(almnt, 1), "second row data");
    CHECK_EQUAL(0, almnt->getMsaRow(1)->getGapModel().size(), "number of gaps in the second row");
}

/** Tests simplify */
IMPLEMENT_TEST(MsaUnitTests, simplify_withGaps) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    bool result = almnt->simplify();
    CHECK_TRUE(result, "Method simplify() returned 'false' unexpectedly");
    CHECK_EQUAL(7, almnt->getLength(), "alignment length");
    CHECK_EQUAL("AGT----", MsaTestUtils::getRowData(almnt, 0), "first row data");
    CHECK_EQUAL("AGCTTAA", MsaTestUtils::getRowData(almnt, 1), "second row data");
}

IMPLEMENT_TEST(MsaUnitTests, simplify_withoutGaps) {
    QByteArray seq("ACGTAGTCGATC");

    MultipleSequenceAlignment almnt("Alignment", NULL);
    almnt->addRow("Row without gaps", seq);

    bool result = almnt->simplify();

    CHECK_FALSE(result, "Method simplify() returned 'true' unexpectedly");
    CHECK_EQUAL(12, almnt->getLength(), "alignment length");
    CHECK_EQUAL("ACGTAGTCGATC", MsaTestUtils::getRowData(almnt, 0), "row data");
}

IMPLEMENT_TEST(MsaUnitTests, simplify_empty) {
    MultipleSequenceAlignment almnt;
    bool result = almnt->simplify();
    CHECK_FALSE(result, "Method simplify() returned 'true' unexpectedly");
}

/** Tests sortRows */
IMPLEMENT_TEST(MsaUnitTests, sortRows_byNameAsc) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("CCCCCC");
    QByteArray thirdSequence("TTTTTT");

    MultipleSequenceAlignment almnt("Alignment name", NULL);
    almnt->addRow("NameBA", firstSequence);
    almnt->addRow("NameAB", secondSequence);
    almnt->addRow("NameAA", thirdSequence);

    almnt->sortRowsByName();
    QStringList rowNames = almnt->getRowNames();
    CHECK_EQUAL(3, rowNames.count(), "number of rows");
    CHECK_EQUAL("NameAA", rowNames[0], "order");
    CHECK_EQUAL("NameAB", rowNames[1], "order");
    CHECK_EQUAL("NameBA", rowNames[2], "order");
    CHECK_EQUAL("TTTTTT", MsaTestUtils::getRowData(almnt, 0), "first row sequence");
}

IMPLEMENT_TEST(MsaUnitTests, sortRows_byNameDesc) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("CCCCCC");
    QByteArray thirdSequence("TTTTTT");

    MultipleSequenceAlignment almnt("Alignment name", NULL);
    almnt->addRow("NameAA", firstSequence);
    almnt->addRow("NameBA", secondSequence);
    almnt->addRow("NameAB", thirdSequence);

    almnt->sortRowsByName(MultipleAlignment::Descending);
    QStringList rowNames = almnt->getRowNames();
    CHECK_EQUAL(3, rowNames.count(), "number of rows");
    CHECK_EQUAL("NameBA", rowNames[0], "order");
    CHECK_EQUAL("NameAB", rowNames[1], "order");
    CHECK_EQUAL("NameAA", rowNames[2], "order");
    CHECK_EQUAL("CCCCCC", MsaTestUtils::getRowData(almnt, 0), "first row sequence");
}

IMPLEMENT_TEST(MsaUnitTests, sortRows_twoSimilar) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("CCCCCC");
    QByteArray thirdSequence("TTTTTT");
    QByteArray forthSequence("AAAAAA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    QVector<U2Region> unitedRegions;
    almnt->sortRowsBySimilarity(unitedRegions);
    QStringList rowNames = almnt->getRowNames();
    CHECK_EQUAL(4, rowNames.count(), "number of rows");
    CHECK_EQUAL("First", rowNames[0], "order");
    CHECK_EQUAL("Forth", rowNames[1], "order");
    CHECK_EQUAL("Second", rowNames[2], "order");
    CHECK_EQUAL("Third", rowNames[3], "order");

    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("CCCCCC", MsaTestUtils::getRowData(almnt, 2), "row3");
    CHECK_EQUAL("TTTTTT", MsaTestUtils::getRowData(almnt, 3), "row4");
    CHECK_EQUAL(1, unitedRegions.count(), "number of similar regions");
    U2Region reg = unitedRegions[0];
    CHECK_EQUAL(0, reg.startPos, "similar rows region start position");
    CHECK_EQUAL(2, reg.length, "similar rows region length");
}

IMPLEMENT_TEST(MsaUnitTests, sortRows_threeSimilar) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("AAAAAA");
    QByteArray thirdSequence("TTTTTT");
    QByteArray forthSequence("AAAAAA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    QVector<U2Region> unitedRegions;
    almnt->sortRowsBySimilarity(unitedRegions);
    QStringList rowNames = almnt->getRowNames();
    CHECK_EQUAL(4, rowNames.count(), "number of rows");
    CHECK_EQUAL("First", rowNames[0], "order");
    CHECK_EQUAL("Second", rowNames[1], "order");
    CHECK_EQUAL("Forth", rowNames[2], "order");
    CHECK_EQUAL("Third", rowNames[3], "order");

    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 2), "row3");
    CHECK_EQUAL("TTTTTT", MsaTestUtils::getRowData(almnt, 3), "row4");

    CHECK_EQUAL(1, unitedRegions.count(), "number of similar regions");
    U2Region reg = unitedRegions[0];
    CHECK_EQUAL(0, reg.startPos, "similar rows region start position");
    CHECK_EQUAL(3, reg.length, "similar rows region length");
}

IMPLEMENT_TEST(MsaUnitTests, sortRows_similarTwoRegions) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("CCCCCC");
    QByteArray thirdSequence("AAAAAA");
    QByteArray forthSequence("CCCCCC");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    QVector<U2Region> unitedRegions;
    almnt->sortRowsBySimilarity(unitedRegions);
    QStringList rowNames = almnt->getRowNames();
    CHECK_EQUAL(4, rowNames.count(), "number of rows");
    CHECK_EQUAL("First", rowNames[0], "order");
    CHECK_EQUAL("Third", rowNames[1], "order");
    CHECK_EQUAL("Second", rowNames[2], "order");
    CHECK_EQUAL("Forth", rowNames[3], "order");

    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("CCCCCC", MsaTestUtils::getRowData(almnt, 2), "row3");
    CHECK_EQUAL("CCCCCC", MsaTestUtils::getRowData(almnt, 3), "row4");

    CHECK_EQUAL(2, unitedRegions.count(), "number of similar regions");
    U2Region reg = unitedRegions[0];
    CHECK_EQUAL(0, reg.startPos, "similar rows first region start position");
    CHECK_EQUAL(2, reg.length, "similar rows first region length");
    reg = unitedRegions[1];
    CHECK_EQUAL(2, reg.startPos, "similar rows second region start position");
    CHECK_EQUAL(2, reg.length, "similar rows second region length");
}

/** Tests getRows */
IMPLEMENT_TEST(MsaUnitTests, getRows_oneRow) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    MultipleSequenceAlignmentRow row = almnt->getMsaRow(0);
    CHECK_EQUAL("---AG-T--", MsaRowTestUtils::getRowData(row), "first row");
}

IMPLEMENT_TEST(MsaUnitTests, getRows_severalRows) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    QList<MultipleSequenceAlignmentRow> rows = almnt->getMsaRows();
    CHECK_EQUAL(2, rows.count(), "number of rows");
    CHECK_EQUAL("---AG-T--", MsaRowTestUtils::getRowData(rows[0]), "first row");
    CHECK_EQUAL("AG-CT-TAA", MsaRowTestUtils::getRowData(rows[1]), "second row");
}

IMPLEMENT_TEST(MsaUnitTests, getRows_rowNames) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    QStringList rowNames = almnt->getRowNames();
    CHECK_EQUAL(2, rowNames.count(), "number of rows");
    CHECK_EQUAL("First row", rowNames[0], "first row name");
    CHECK_EQUAL("Second row", rowNames[1], "second row name");
}

/** Tests charAt */
IMPLEMENT_TEST(MsaUnitTests, charAt_nonGapChar) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    char result = almnt->charAt(0, 3);
    CHECK_EQUAL('A', result, "char inside first row");
}

IMPLEMENT_TEST(MsaUnitTests, charAt_gap) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    char result = almnt->charAt(1, 2);
    CHECK_EQUAL('-', result, "gap inside second row");
}

/** Tests insertGaps */
IMPLEMENT_TEST(MsaUnitTests, insertGaps_validParams) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->insertGaps(0, 4, 3, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(10, almnt->getLength(), "alignment length");
    CHECK_EQUAL("---A---G-T", MsaTestUtils::getRowData(almnt, 0), "first row");
}

IMPLEMENT_TEST(MsaUnitTests, insertGaps_toBeginningLength) {
    QByteArray firstSequence("ACGT");
    QByteArray secondSequence("ACC");

    U2OpStatusImpl os;
    MultipleSequenceAlignment almnt("Alignment");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);

    almnt->insertGaps(1, 0, 2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(5, almnt->getLength(), "alignment length");
    CHECK_EQUAL("--ACC", MsaTestUtils::getRowData(almnt, 1), "second row");
}

IMPLEMENT_TEST(MsaUnitTests, insertGaps_negativeRowIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->insertGaps(-1, 4, 3, os);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, insertGaps_tooBigRowIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->insertGaps(2, 4, 3, os);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, insertGaps_negativePos) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->insertGaps(0, -1, 3, os);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, insertGaps_tooBigPos) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->insertGaps(0, 10, 3, os);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, insertGaps_negativeCount) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->insertGaps(0, 4, -1, os);
    CHECK_EQUAL("Failed to insert gaps into an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

/** Tests removeData */
IMPLEMENT_TEST(MsaUnitTests, removeChars_validParams) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeChars(1, 0, 2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
    CHECK_EQUAL("---AG-T--", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("-CT-TAA--", MsaTestUtils::getRowData(almnt, 1), "second row");
}

IMPLEMENT_TEST(MsaUnitTests, removeChars_negativeRowIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeChars(-1, 0, 2, os);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, removeChars_tooBigRowIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeChars(2, 0, 2, os);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, removeChars_negativePos) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeChars(1, -1, 2, os);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, removeChars_tooBigPos) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeChars(1, 10, 2, os);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, removeChars_negativeCount) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeChars(1, 0, -1, os);
    CHECK_EQUAL("Failed to remove chars from an alignment", os.getError(), "opStatus");
    CHECK_TRUE(MsaTestUtils::testAlignmentNotChanged(almnt), "Alignment changed unexpectedly");
}

/** Tests removeRegion */
IMPLEMENT_TEST(MsaUnitTests, removeRegion_validParams) {
    QByteArray firstSequence("---ACT");
    QByteArray secondSequence("A-CG-A");
    QByteArray thirdSequence("---CGA");
    QByteArray forthSequence("AAAAAA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    almnt->removeRegion(1, 1, 3, 2, false);

    CHECK_EQUAL(4, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---ACT", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("A-A---", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("-GA---", MsaTestUtils::getRowData(almnt, 2), "row3");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 3), "row4");
}

IMPLEMENT_TEST(MsaUnitTests, removeRegion_removeEmpty) {
    QByteArray firstSequence("---ACT");
    QByteArray secondSequence("A-CG-A");
    QByteArray thirdSequence("--AC");
    QByteArray forthSequence("AAAAAA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    almnt->removeRegion(1, 1, 3, 2, true);

    CHECK_EQUAL(3, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---ACT", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("A-A---", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 2), "row3");
}

IMPLEMENT_TEST(MsaUnitTests, removeRegion_trimmed) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->removeRegion(0, 1, 2, 1, false);
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
    CHECK_EQUAL("---AG-T--", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("-CT-TAA--", MsaTestUtils::getRowData(almnt, 1), "second row");
}

/** Tests renameRow */
IMPLEMENT_TEST(MsaUnitTests, renameRow_validParams) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    QString newRowName = "New row name";
    almnt->renameRow(0, newRowName);
    MultipleSequenceAlignmentRow actualRow = almnt->getMsaRow(0);
    CHECK_EQUAL(newRowName, actualRow->getName(), "renamed row name");
}

/** Tests setRowContent */
IMPLEMENT_TEST(MsaUnitTests, setRowContent_validParamsAndNotTrimmed) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->setRowContent(1, "---AC-");
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
    CHECK_EQUAL("---AG-T--", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("---AC----", MsaTestUtils::getRowData(almnt, 1), "second row");
}

IMPLEMENT_TEST(MsaUnitTests, setRowContent_lengthIsIncreased) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->setRowContent(0, "ACGT-ACA-ACA");
    CHECK_EQUAL(12, almnt->getLength(), "alignment length");
    CHECK_EQUAL("ACGT-ACA-ACA", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("AG-CT-TAA---", MsaTestUtils::getRowData(almnt, 1), "second row");
}

/** Tests upperCase */
IMPLEMENT_TEST(MsaUnitTests, upperCase_charsAndGaps) {
    QByteArray sequence1("mMva-ke");
    QByteArray sequence2("avn-*y-s");

    MultipleSequenceAlignment almnt("Alignment with chars in lower-case");
    almnt->addRow("First row", sequence1);
    almnt->addRow("Second row", sequence2);

    almnt->toUpperCase();

    CHECK_EQUAL("MMVA-KE-", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("AVN-*Y-S", MsaTestUtils::getRowData(almnt, 1), "row2");
}

/** Tests crop */
IMPLEMENT_TEST(MsaUnitTests, crop_validParams) {
    QByteArray firstSequence("---ACT");
    QByteArray secondSequence("A-CG-A");
    QByteArray thirdSequence("---CGA");

    U2OpStatusImpl os;
    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);

    U2Region region(1, 4);
    QSet<QString> rowNames;
    rowNames << "First" << "Second";

    almnt->crop(region, rowNames, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL(4, almnt->getLength(), "alignment length");
    CHECK_EQUAL("--AC", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("-CG-", MsaTestUtils::getRowData(almnt, 1), "second row");
}

/** Tests mid */
IMPLEMENT_TEST(MsaUnitTests, mid_validParams) {
    QByteArray firstSequence("---ACT");
    QByteArray secondSequence("A");
    QByteArray thirdSequence("---CGA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);

    MultipleSequenceAlignment almntResult = almnt->mid(2, 3);
    CHECK_EQUAL(3, almntResult->getNumRows(), "number of rows");
    CHECK_EQUAL("-AC", MsaTestUtils::getRowData(almntResult, 0), "first row");
    CHECK_EQUAL("---", MsaTestUtils::getRowData(almntResult, 1), "second row");
    CHECK_EQUAL("-CG", MsaTestUtils::getRowData(almntResult, 2), "third row");

    CHECK_EQUAL("---ACT", MsaTestUtils::getRowData(almnt, 0), "first row of the original");
}

/** Tests addRow */
IMPLEMENT_TEST(MsaUnitTests, addRow_appendRowFromBytes) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->addRow("Added row", "--AACT-GAG");

    CHECK_EQUAL(3, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---AG-T---", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("AG-CT-TAA-", MsaTestUtils::getRowData(almnt, 1), "second row");
    CHECK_EQUAL("--AACT-GAG", MsaTestUtils::getRowData(almnt, 2), "third row");
    CHECK_EQUAL("Added row", almnt->getRowNames().at(2), "added row name");
    CHECK_EQUAL(10, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, addRow_rowFromBytesToIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->addRow("Added row", "--AACT-GAG", 1);

    CHECK_EQUAL(3, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---AG-T---", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("--AACT-GAG", MsaTestUtils::getRowData(almnt, 1), "second row");
    CHECK_EQUAL("AG-CT-TAA-", MsaTestUtils::getRowData(almnt, 2), "third row");
    CHECK_EQUAL("Added row", almnt->getRowNames().at(1), "added row name");
    CHECK_EQUAL(10, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, addRow_zeroBound) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->addRow("Added row", "--AACT-GAG", -2);

    CHECK_EQUAL(3, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("--AACT-GAG", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("---AG-T---", MsaTestUtils::getRowData(almnt, 1), "second row");
    CHECK_EQUAL("AG-CT-TAA-", MsaTestUtils::getRowData(almnt, 2), "third row");
    CHECK_EQUAL("Added row", almnt->getRowNames().at(0), "added row name");
    CHECK_EQUAL(10, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, addRow_rowsNumBound) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    almnt->addRow("Added row", "--AACT-GAG", 3);

    CHECK_EQUAL(3, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---AG-T---", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("AG-CT-TAA-", MsaTestUtils::getRowData(almnt, 1), "second row");
    CHECK_EQUAL("--AACT-GAG", MsaTestUtils::getRowData(almnt, 2), "third row");
    CHECK_EQUAL("Added row", almnt->getRowNames().at(2), "added row name");
    CHECK_EQUAL(10, almnt->getLength(), "alignment length");
}

/** Tests removeRow */
IMPLEMENT_TEST(MsaUnitTests, removeRow_validIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeRow(1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---AG-T--", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, removeRow_negativeIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeRow(-1, os);
    CHECK_EQUAL("Failed to remove a row", os.getError(), "opStatus");
    CHECK_EQUAL(2, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---AG-T--", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("AG-CT-TAA", MsaTestUtils::getRowData(almnt, 1), "second row");
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, removeRow_tooBigIndex) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeRow(2, os);
    CHECK_EQUAL("Failed to remove a row", os.getError(), "opStatus");
    CHECK_EQUAL(2, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL("---AG-T--", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL("AG-CT-TAA", MsaTestUtils::getRowData(almnt, 1), "second row");
    CHECK_EQUAL(9, almnt->getLength(), "alignment length");
}

IMPLEMENT_TEST(MsaUnitTests, removeRow_emptyAlignment) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    U2OpStatusImpl os;
    almnt->removeRow(0, os);
    CHECK_NO_ERROR(os);
    almnt->removeRow(0, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, almnt->getNumRows(), "number of rows");
    CHECK_EQUAL(0, almnt->getLength(), "alignment length");
}

/** Tests moveRowsBlock */
IMPLEMENT_TEST(MsaUnitTests, moveRowsBlock_positiveDelta) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("CCCCCC");
    QByteArray thirdSequence("GGGGGG");
    QByteArray forthSequence("TTTTTT");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    almnt->moveRowsBlock(0, 2, 1);

    CHECK_EQUAL("GGGGGG", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("CCCCCC", MsaTestUtils::getRowData(almnt, 2), "row3");
    CHECK_EQUAL("TTTTTT", MsaTestUtils::getRowData(almnt, 3), "row4");
}

IMPLEMENT_TEST(MsaUnitTests, moveRowsBlock_negativeDelta) {
    QByteArray firstSequence("AAAAAA");
    QByteArray secondSequence("CCCCCC");
    QByteArray thirdSequence("GGGGGG");
    QByteArray forthSequence("TTTTTT");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);
    almnt->addRow("Second", secondSequence);
    almnt->addRow("Third", thirdSequence);
    almnt->addRow("Forth", forthSequence);

    almnt->moveRowsBlock(3, 1, -1);

    CHECK_EQUAL("AAAAAA", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("CCCCCC", MsaTestUtils::getRowData(almnt, 1), "row2");
    CHECK_EQUAL("TTTTTT", MsaTestUtils::getRowData(almnt, 2), "row3");
    CHECK_EQUAL("GGGGGG", MsaTestUtils::getRowData(almnt, 3), "row4");
}

/** Tests replaceChars */
IMPLEMENT_TEST(MsaUnitTests, replaceChars_validParams) {
    QByteArray firstSequence("AGT.C.T");
    QByteArray secondSequence("A.CT.-AA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First row", firstSequence);
    almnt->addRow("Second row", secondSequence);

    almnt->replaceChars(1, '.', '-');
    CHECK_EQUAL("AGT.C.T-", MsaTestUtils::getRowData(almnt, 0), "first sequence");
    CHECK_EQUAL("A-CT--AA", MsaTestUtils::getRowData(almnt, 1), "second sequence");
}

/** Tests appendChars */
IMPLEMENT_TEST(MsaUnitTests, appendChars_validParams) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    const char* str = "-AC-GT-";
    int length = 7;
    almnt->appendChars(0, str, length);
    CHECK_EQUAL("---AG-T---AC-GT-", MsaTestUtils::getRowData(almnt, 0), "first row");
    CHECK_EQUAL(4, almnt->getMsaRow(0)->getGapModel().size(), "number of gaps");
    CHECK_EQUAL(16, almnt->getLength(), "alignment length");
}

/** Tests operPlusEqual */
IMPLEMENT_TEST(MsaUnitTests, operPlusEqual_validParams) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    MultipleSequenceAlignment almnt2 = MsaTestUtils::initTestAlignment();

    *almnt += *almnt2;

    CHECK_EQUAL("---AG-T-----AG-T--", MsaTestUtils::getRowData(almnt, 0), "row1");
    CHECK_EQUAL("AG-CT-TAAAG-CT-TAA", MsaTestUtils::getRowData(almnt, 1), "row2");

    CHECK_EQUAL(4, almnt->getMsaRow(0)->getGapModel().size(), "number of gaps");
    CHECK_EQUAL(18, almnt->getLength(), "alignment length");
}

/** Tests operNotEqual */
IMPLEMENT_TEST(MsaUnitTests, operNotEqual_equal) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    MultipleSequenceAlignment almnt2 = MsaTestUtils::initTestAlignment();

    bool res = (*almnt != *almnt2);
    CHECK_FALSE(res, "Operator!= returned 'True' unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, operNotEqual_notEqual) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    MultipleSequenceAlignment almnt2;

    bool res = (*almnt != *almnt2);
    CHECK_TRUE(res, "Operator!= returned 'False' unexpectedly");
}

/** Tests hasEmptyGapModel */
IMPLEMENT_TEST(MsaUnitTests, hasEmptyGapModel_gaps) {
    MultipleSequenceAlignment almnt = MsaTestUtils::initTestAlignment();
    bool res = almnt->hasEmptyGapModel();

    CHECK_FALSE(res, "Method hasEmptyGapModel() returned 'True' unexpectedly");
}

IMPLEMENT_TEST(MsaUnitTests, hasEmptyGapModel_noGaps) {
    QByteArray firstSequence("AAAAAA");

    MultipleSequenceAlignment almnt("Alignment name");
    almnt->addRow("First", firstSequence);

    bool res = almnt->hasEmptyGapModel();

    CHECK_TRUE(res, "Method hasEmptyGapModel() returned 'False' unexpectedly");
}


} // namespace
