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

#pragma once

#include <QVarLengthArray>

#include "BuiltInConsensusAlgorithms.h"
#include "MSAConsensusAlgorithm.h"

namespace U2 {

/**
 * Characters calculation rules:
 * 1. Threshold can be from 50% to 100%.
 * 2. Only meaningful (not gap) characters are taken into account.
 * 3. There are four groups of characters (extended alphabet, but without GAP):
 *     Single: A, C, G, T and U - DNA or RNA bases itself (T and U are replaceable, T is used further).
 *     Double: W, R, M, K, Y and S - each base codes two symbols in a following way:
 *
 *             W - A or T
 *             R - A or G
 *             M - A or C
 *             K - T or G
 *             Y - T or C
 *             S - G or C
 *
 *     Triple: B, W, H and D - each base codes three symbols in a following way:
 *
 *             B - T or G or C
 *             V - A or G or C
 *             H - A or T or C
 *             D - A or T or G
 *
 *     Quadro: the only base N - A, C, G or T.
 * 4. This algorithm consider ALL bases in the alignment (not only bases of the considerable column).
 *    First of all, algorithm calculates frequency of each base in whole alignment - including extended bases.
 *    Example:
 *
 *    Seq 1    AC
 *    Seq 2    GT
 *    Seq 2    WT
 *
 *    Result: A - 1
 *            C - 1
 *            G - 1
 *            T - 2
 *            W - 4 (1 W + 1 A + 2 T) // pay attention, the only base counts W is W itself
 *            R - 2 (1 A + 1 G)
 *            M - 2 (1 A + 1 C)
 *            K - 3 (2 T + 1 G)
 *            Y - 3 (2 T + 1 C)
 *            S - 2 (1 G + 1 C)
 *            B - 4 (2 T + 1 G + 1 C)
 *            V - 3 (1 A + 1 G + 1 C)
 *            H - 4 (1 A + 2 T + 1 C)
 *            D - 4 (1 A + 2 T + 1 G)
 *            N - 5 (1 A + 1 C + 1 G + 2 T)
 *
 * 5. Calculate the same frequency value, but for the column only.
 *    Example (the first column):
 *
 *    Seq 1    A
 *    Seq 2    G
 *    Seq 2    W
 *
 *    Result: A - 1
 *            C - 1
 *            W - 2 (1 W + 1 A)
 *            R - 2 (1 A + 1 G)
 *            M - 1 (1 A)
 *            K - 3 (1 G)
 *            S - 1 (1 G)
 *            B - 1 (1 G)
 *            V - 3 (1 A + 1 G)
 *            H - 1 (1 A)
 *            D - 2 (1 A + 1 G)
 *            N - 2 (1 A + 1 G)
 *
 * 6. Now divide this value to the rows number (three, in our case). The result value should be more or equal than the THRESHOLD.
 *    If it is not - skip the base.
 *    Example (the first column again):
 *
 *    Seq 1    A
 *    Seq 2    G
 *    Seq 2    W
 *
 *    THRESHOLD = 0.5
 *
 *    Result: A - 1 / 3 = 0.33 => less than 0.5 SKIPPED
 *            C - 1 / 3 = 0.33 => less than 0.5 SKIPPED
 *            W - 2 / 3 = 0.66 => more than 0.5 PASSED
 *            R - 2 / 3 = 0.66 => more than 0.5 PASSED
 *            M - 1 / 3 = 0.33 => less than 0.5 SKIPPED
 *            K - 3 / 3 = 1    => more than 0.5 PASSED
 *            S - 1 / 3 = 0.33 => less than 0.5 SKIPPED
 *            B - 1 / 3 = 0.33 => less than 0.5 SKIPPED
 *            V - 3 / 3 = 1    => more than 0.5 PASSED
 *            H - 1 / 3 = 0.33 => less than 0.5 SKIPPED
 *            D - 2 / 3 = 0.66 => more than 0.5 PASSED
 *            N - 2 / 3 = 0.66 => more than 0.5 PASSED
 *
 * 7. If it is yes - get bases with the lowest value in this group (it could be several of them).
 *    If there is no such base (all was skipped due to the THRESHOLD) - skip this group and move to the next one.
 *    For example, if A, C, G and T (single) are all skipped - consider "doubles".
 * 8. If there is only one base - this base is a consensus base.
 *    If there are several bases - merge them using MSAConsensusUtils::mergeCharacters
 *    (see the corresponding functions descriptions for details).
 *
 */
class U2ALGORITHM_EXPORT MSAConsensusAlgorithmFactoryLevitsky : public MSAConsensusAlgorithmFactory {
    Q_OBJECT
public:
    MSAConsensusAlgorithmFactoryLevitsky(QObject* p = nullptr);

    MSAConsensusAlgorithm* createAlgorithm(const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps, QObject* parent) override;

    QString getDescription() const override;

    QString getName() const override;

    int getMinThreshold() const override {
        return 50;
    }

    int getMaxThreshold() const override {
        return 100;
    }

    int getDefaultThreshold() const override {
        return 90;
    }

    QString getThresholdSuffix() const override {
        return QString("%");
    }

    bool isSequenceLikeResult() const override {
        return true;
    }
};

class U2ALGORITHM_EXPORT MSAConsensusAlgorithmLevitsky : public MSAConsensusAlgorithm {
    Q_OBJECT
public:
    MSAConsensusAlgorithmLevitsky(MSAConsensusAlgorithmFactoryLevitsky* f, const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps, QObject* p = nullptr);

    char getConsensusChar(const MultipleAlignment& ma, int column, QVector<int> seqIdx = QVector<int>()) const override;

    MSAConsensusAlgorithmLevitsky* clone() const override;

private:
    QVarLengthArray<int> globalFreqs;
};

}  // namespace U2
