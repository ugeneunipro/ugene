/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MsaConsensusAlgorithmUnitTests.h"

#include <U2Algorithm/BuiltInAssemblyConsensusAlgorithms.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusAlgorithmLevitsky.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatusUtils.h>

#include <QSharedPointer>

namespace U2 {

const QString MsaConsensusAlgorithmUnitTestsUtils::alignmentName = "Test alignment name";

char MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase(const QStringList& alignmentRows, int threshold, int columnNumber) {
    auto bestAlphabet = U2AlphabetUtils::findBestAlphabet(alignmentRows.join("").toLocal8Bit());
    MultipleSequenceAlignment alignment(alignmentName, bestAlphabet);

    for (int i = 0; i < alignmentRows.size(); i++) {
        const auto& row = alignmentRows[i];
        alignment->addRow(QString::number(i), row.toLocal8Bit());
    }

    auto factory = AppContext::getMSAConsensusAlgorithmRegistry()->getAlgorithmFactory(BuiltInAssemblyConsensusAlgorithms::LEVITSKY_ALGO);
    auto algorithm = QSharedPointer<MSAConsensusAlgorithm>(factory->createAlgorithm(alignment));
    algorithm->setThreshold(threshold);

    return algorithm->getConsensusChar(alignment, columnNumber);
}

/*
Code table from Victor Levitsky:
A       A       1
T       T       1
G       G       1
C       C       1
W       A,T     2       WEAK
R       A,G     2       PURINE
M       A,C     2       AMINO- (+ charge)
K       T,G     2       KETO- (- charge)
Y       T,C     2       PYRIMIDINE
S       G,C     2       STRONG
B       T,G,C   3       not A (B is near with A in Latin alphabet)
V       A,G,C   3       not T (---||---)
H       A,T,C   3       not G (---||---)
D       A,T,G   3       not C (---||---)
N       A,T,G,C 4       Any

*/

IMPLEMENT_TEST(MsaConsensusAlgorithmUnitTests, levitskyCheckColumnBase) {
    //1. One base mix
    // Single base in the column - this base in consensus
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A"}, 50, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T"}, 50, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U"}, 50, 0)), "consensus base");

    // 2. Two bases mix
    // 2.1 The case, when consensus character is defined only by bases of the column
    //
    // A is not taken into account, because its share is 33%, which is less than 50%
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "C", "C"}, 50, 0)), "consensus base");
    // A and C are not taken into account, because their share is 33% and 66%, which is less than 70%.
    // The symbol of the extended alphabet containing both of them is taken - this is M.
    CHECK_EQUAL("M", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "C", "C"}, 70, 0)), "consensus base");
    //The same principle for all other tests - check the table above to find out the consensus base for each second test.
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "G", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("R", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "G", "G"}, 70, 0)), "consensus base");
    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "T", "T"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "T", "T"}, 70, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "U", "U"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "U", "U"}, 70, 0)), "consensus base");

    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "A", "A"}, 50, 0)), "consensus base");
    CHECK_EQUAL("M", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "A", "A"}, 70, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "G", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("S", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "G", "G"}, 70, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "U", "U"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "U", "U"}, 70, 0)), "consensus base");

    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "A", "A"}, 50, 0)), "consensus base");
    CHECK_EQUAL("R", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "A", "A"}, 70, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "C", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("S", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "C", "C"}, 70, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "U", "U"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "U", "U"}, 70, 0)), "consensus base");

    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "A", "A"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "A", "A"}, 70, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "C", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "C", "C"}, 70, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "G", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "G", "G"}, 70, 0)), "consensus base");

    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "A", "A"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "A", "A"}, 70, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "C", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "C", "C"}, 70, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "G", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "G", "G"}, 70, 0)), "consensus base");

    // 2.2 The case, when bases from the whole alignment have influence to the consensus base of a certain column
    //
    // A and C in the first column, but A is used, because it has 25% in whole alignment (less than 75% from C)
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "CC"}, 50, 0)), "consensus base");
    // A and C both skipped, because they have 50% in a column (less thatn 51 required).
    // Y is coorect, because it has 100% in the column (50% from A and 50% from C)
    CHECK_EQUAL("M", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "CC"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AG", "GG"}, 50, 0)), "consensus base");
    CHECK_EQUAL("R", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AG", "GG"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AT", "TT"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AT", "TT"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AU", "UU"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AU", "UU"}, 51, 0)), "consensus base");

    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CA", "AA"}, 50, 0)), "consensus base");
    CHECK_EQUAL("M", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CA", "AA"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CG", "GG"}, 50, 0)), "consensus base");
    CHECK_EQUAL("S", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CG", "GG"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CT", "TT"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CT", "TT"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CU", "UU"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"CU", "UU"}, 51, 0)), "consensus base");

    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GA", "AA"}, 50, 0)), "consensus base");
    CHECK_EQUAL("R", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GA", "AA"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GC", "CC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("S", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GC", "CC"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GT", "TT"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GT", "TT"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GU", "UU"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"GU", "UU"}, 51, 0)), "consensus base");

    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"TA", "AA"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"TA", "AA"}, 51, 0)), "consensus base");
    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"TC", "CC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"TC", "CC"}, 51, 0)), "consensus base");
    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"TG", "GG"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"TG", "GG"}, 51, 0)), "consensus base");

    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"UA", "AA"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"UA", "AA"}, 51, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"UC", "CC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("Y", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"UC", "CC"}, 51, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"UG", "GG"}, 50, 0)), "consensus base");
    CHECK_EQUAL("K", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"UG", "GG"}, 51, 0)), "consensus base");

    // 3. Three bases mix
    // 3.1 The case, when consensus character is defined only by bases of the column
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "C", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("V", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "C", "G"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "C", "T"}, 50, 0)), "consensus base");
    CHECK_EQUAL("H", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "C", "T"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "T", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("D", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "T", "G"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "C", "U"}, 50, 0)), "consensus base");
    CHECK_EQUAL("H", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "C", "U"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "U", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("D", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "A", "U", "G"}, 51, 0)), "consensus base");

    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "A", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("V", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "A", "G"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "A", "T"}, 50, 0)), "consensus base");
    CHECK_EQUAL("H", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "A", "T"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "T", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("B", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "T", "G"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "A", "U"}, 50, 0)), "consensus base");
    CHECK_EQUAL("H", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "A", "U"}, 51, 0)), "consensus base");
    CHECK_EQUAL("C", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "U", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("B", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"C", "C", "U", "G"}, 51, 0)), "consensus base");

    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "A", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("V", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "A", "C"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "A", "T"}, 50, 0)), "consensus base");
    CHECK_EQUAL("D", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "A", "T"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "T", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("B", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "T", "C"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "A", "U"}, 50, 0)), "consensus base");
    CHECK_EQUAL("D", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "A", "U"}, 51, 0)), "consensus base");
    CHECK_EQUAL("G", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "U", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("B", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"G", "G", "U", "C"}, 51, 0)), "consensus base");

    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T", "A", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("H", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T", "A", "C"}, 51, 0)), "consensus base");
    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T", "A", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("D", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T", "A", "G"}, 51, 0)), "consensus base");
    CHECK_EQUAL("T", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T", "C", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("B", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"T", "T", "C", "G"}, 51, 0)), "consensus base");

    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U", "A", "C"}, 50, 0)), "consensus base");
    CHECK_EQUAL("H", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U", "A", "C"}, 51, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U", "A", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("D", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U", "A", "G"}, 51, 0)), "consensus base");
    CHECK_EQUAL("U", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U", "C", "G"}, 50, 0)), "consensus base");
    CHECK_EQUAL("B", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"U", "U", "C", "G"}, 51, 0)), "consensus base");

    // 3.2 The case, when bases from the whole alignment have influence to the consensus base of a certain column
    // We have three bases in column, but one of them is to often appeared is second column and not included into mix
    // !! It is becoming impossible to test all possible options, so there is just several of them !!
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AA", "AA", "CA", "GA"}, 50, 0)), "consensus base");
    CHECK_EQUAL("V", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AA", "AA", "CA", "GA"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "AC", "CC", "GC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("R", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "AC", "CC", "GC"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "AC", "CC", "TC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "AC", "CC", "TC"}, 51, 0)), "consensus base");
    CHECK_EQUAL("A", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "AC", "CC", "UC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AC", "AC", "CC", "UC"}, 51, 0)), "consensus base");

    // 4. Four bases mix
    CHECK_EQUAL("N", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "C", "G", "T"}, 50, 0)), "consensus base");
    CHECK_EQUAL("N", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "C", "G", "T"}, 75, 0)), "consensus base");
    CHECK_EQUAL("N", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "C", "G", "T"}, 100, 0)), "consensus base")
    CHECK_EQUAL("N", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"A", "C", "G", "T"}, 100, 0)), "consensus base")
    CHECK_EQUAL("W", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AG", "CG", "GC", "TC"}, 50, 0)), "consensus base");
    CHECK_EQUAL("N", QString(MsaConsensusAlgorithmUnitTestsUtils::getLevitskyConsensusBase({"AG", "CG", "GC", "TC"}, 70, 0)), "consensus base");
}

}
