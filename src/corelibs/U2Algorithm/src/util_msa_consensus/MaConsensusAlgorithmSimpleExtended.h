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

#ifndef _U2_MA_CONSENSUS_ALGORITHM_SIMPLE_EXTENDED_H_
#define _U2_MA_CONSENSUS_ALGORITHM_SIMPLE_EXTENDED_H_

#include "MSAConsensusAlgorithm.h"

namespace U2 {

class MaConsensusAlgorithmFactorySimpleExtended;

/**
 * Characters calculation rules:
 * 1. Threshold can be from 50% to 100%.
 * 2. Meaningful (not gap) characters merge with each other by the rules of the extended DNA alphabet: A + C = M, C + S = S, G + Y = B.
 * 3. Gap merges with any meaningful character to symbol 'N'.
 * 4. If there is the only one character that fits the threshold, the it is the result.
 * 5. If there are two characters that fit the threshold, the the merged character from these two characters is the result.
 *    It can be in case of threshold is equal 50%, and both characters are spotted in 50% of rows.
 * 6. If there are no characters that fit the threshold, then most popular symbols are involved to the calculations:
 *    - The most popular characters are taken (with equal popularity).
 *    - If there are several characters, then they are merged and the merged character is the result.
 *    - If there is only one character, then a bit less popular characters are involved to the calculations.
 *    - A set of "a bit less popular" characters and the most popular character are merged into a single character. This merged character is the result.
 *    - Other not-so-popular-characters are ignored.
 *
 * Examples:
 *
 * threshold = 100%, Column = AAAAAAAAAA
 *      The result character: A - it is spotted in 100% of rows, it fits the threshold
 *
 * threshold = 60%, Column = ------AAAA
 *      The result character: '-' - it is spotted in 60% of rows, it fits the threshold
 *
 * threshold = 80%, Column = AAAAAAAAAC
 *      The result character: A - it is spotted in 90% of rows, it fits the threshold
 *
 * threshold = 80%, Column = AAAAAAACCC
 *      The result character: M - both A and C are spotted too seldom, the most popular character A is taken, it merges with a bit less popular C
 *
 * threshold = 80%, Column = AAAAGGGCCC
 *      The result character: V - all A, C and G are spotted too seldom, the most popular character A is taken, it merges with a bit less popular C and G. Both C and G are taken, because they have the same popularity
 *
 * threshold = 80%, Column = AAAAAGGGGG
 *      The result character: R - both A and G are spotted too seldom, the most popular characters A and G are taken, they merges together to a result. Both A and G are taken, because they have the same popularity
 *
 * threshold = 50%, Column = AAAAAGGGGG
 *      The result character: R - both A and G fit the threshold, the result is the merged character
 *
 * threshold = 50%, Column = AAAAA-----
 *      The result character: N - both A and '-' fit the threshold, the result is the merged character. Gap with a meaningful character A merges to N
 *
 * threshold = 50%, Column = AAAMMMMCCC
 *      The result character: M - M is the most popular character, it is merged with A and C to M.
 *
 * threshold = 50%, Column = AAAMMMMTTT
 *      The result character: V - M is the most popular character, it is merged with A and T to V.
 */

class MaConsensusAlgorithmSimpleExtended : public MSAConsensusAlgorithm {
    Q_OBJECT
public:
    MaConsensusAlgorithmSimpleExtended(MaConsensusAlgorithmFactorySimpleExtended *factory, bool ignoreTrailingLeadingGaps, QObject *parent);

    char getConsensusChar(const MultipleAlignment &ma, int column, QVector<int> seqIdx = QVector<int>()) const;

    virtual MaConsensusAlgorithmSimpleExtended* clone() const;

    enum Character {
        None = 0,
        Gap = 1 << 0,
        A = 1 << 1,
        C = 1 << 2,
        G = 1 << 3,
        T = 1 << 4,
        W = A + T,
        R = A + G,
        M = A + C,
        K = T + G,
        Y = T + C,
        S = G + C,
        B = C + G + T,
        V = A + C + G,
        H = A + C + T,
        D = A + G + T,
        N = A + C + G + T + Gap
    };
    Q_DECLARE_FLAGS(Characters, Character)

private:
    Q_ENUMS(Character)

    static Character character2Flag(char character);
    static char flag2Character(Character flag);
    static char flags2Character(Characters flags);
    static char mergeCharacters(const QVector<char> &characters);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MaConsensusAlgorithmSimpleExtended::Characters)

class MaConsensusAlgorithmFactorySimpleExtended : public MSAConsensusAlgorithmFactory {
    Q_OBJECT
public:
    MaConsensusAlgorithmFactorySimpleExtended(QObject *parent = NULL);

    MSAConsensusAlgorithm *createAlgorithm(const MultipleAlignment &ma, bool ignoreTrailingLeadingGaps, QObject *parent);

    QString getDescription() const;
    QString getName() const;

    int getMinThreshold() const;
    int getMaxThreshold() const;
    int getDefaultThreshold() const;

    QString getThresholdSuffix() const;

    bool isSequenceLikeResult() const;
};

}   // namespace U2

#endif // _U2_MA_CONSENSUS_ALGORITHM_SIMPLE_EXTENDED_H_
