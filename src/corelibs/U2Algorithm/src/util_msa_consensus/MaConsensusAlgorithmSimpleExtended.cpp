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

#include "MaConsensusAlgorithmSimpleExtended.h"
#include <cmath>

#include <QMetaEnum>

#include <U2Core/U2SafePoints.h>

#include "BuiltInConsensusAlgorithms.h"

namespace U2 {

MaConsensusAlgorithmSimpleExtended::MaConsensusAlgorithmSimpleExtended(MaConsensusAlgorithmFactorySimpleExtended* factory, bool ignoreTrailingLeadingGaps)
    : MSAConsensusAlgorithm(factory, ignoreTrailingLeadingGaps) {
}

MaConsensusAlgorithmSimpleExtended::Character MaConsensusAlgorithmSimpleExtended::character2Flag(char character) {
    switch (character) {
        case '-':
            return Gap;
        case 'A':
            return A;
        case 'C':
            return C;
        case 'G':
            return G;
        case 'T':
            return T;
        case 'W':
            return W;
        case 'R':
            return R;
        case 'M':
            return M;
        case 'K':
            return K;
        case 'Y':
            return Y;
        case 'S':
            return S;
        case 'B':
            return B;
        case 'V':
            return V;
        case 'H':
            return H;
        case 'D':
            return D;
        case 'N':
            return N;
        default:
            return None;
    }
}

char MaConsensusAlgorithmSimpleExtended::flag2Character(Character flag) {
    switch (flag) {
        case Gap:
            return '-';
        case A:
            return 'A';
        case C:
            return 'C';
        case G:
            return 'G';
        case T:
            return 'T';
        case W:
            return 'W';
        case R:
            return 'R';
        case M:
            return 'M';
        case K:
            return 'K';
        case Y:
            return 'Y';
        case S:
            return 'S';
        case B:
            return 'B';
        case V:
            return 'V';
        case H:
            return 'H';
        case D:
            return 'D';
        case N:
            return 'N';
        default:
            return MaConsensusAlgorithmSimpleExtended::INVALID_CONS_CHAR;
    }
}

char MaConsensusAlgorithmSimpleExtended::flags2Character(Characters flags) {
    const QMetaEnum characterMetaEnum = MaConsensusAlgorithmSimpleExtended::staticMetaObject.enumerator(0);
    for (int i = 0; i < characterMetaEnum.keyCount(); i++) {
        const Characters currentFlags = static_cast<Characters>(characterMetaEnum.value(i));
        if ((flags & currentFlags) == flags) {
            return flag2Character(static_cast<Character>(characterMetaEnum.value(i)));
        }
    }

    return MaConsensusAlgorithmSimpleExtended::INVALID_CONS_CHAR;
}

char MaConsensusAlgorithmSimpleExtended::mergeCharacters(const QVector<char>& characters) {
    Characters mergedFlag = None;
    foreach (const char character, characters) {
        mergedFlag |= character2Flag(character);
    }
    return flags2Character(mergedFlag);
}

static QVector<QVector<char>> getFrequencies(const Msa& ma, int column, QVector<int> seqIdx) {
    QVarLengthArray<int> frequencies(256);
    memset(frequencies.data(), 0, frequencies.size() * sizeof(int));

    const int nSeq = (seqIdx.isEmpty() ? ma->getRowCount() : seqIdx.size());
    for (int seq = 0; seq < nSeq; seq++) {
        const char c = ma->charAt(seqIdx.isEmpty() ? seq : seqIdx[seq], column);
        frequencies[static_cast<int>(c)]++;
    }

    QVector<QVector<char>> sortedFrequencies(seqIdx.isEmpty() ? ma->getRowCount() + 1 : seqIdx.size() + 1);
    for (int c = 'A'; c <= 'Y'; c++) {
        sortedFrequencies[frequencies[c]] << static_cast<char>(c);
    }
    sortedFrequencies[frequencies['-']] << static_cast<char>('-');
    return sortedFrequencies;
}

char MaConsensusAlgorithmSimpleExtended::getConsensusChar(const Msa& ma, int column) const {
    QVector<int> seqIdx = pickRowsToUseInConsensus(ma, column);
    CHECK(!ignoreTrailingAndLeadingGaps || !seqIdx.isEmpty(), INVALID_CONS_CHAR);

    QVector<QVector<char>> frequencies = getFrequencies(ma, column, seqIdx);

    char bestCharacter = INVALID_CONS_CHAR;
    const int thresholdCount = ceil(((frequencies.size() - 1) * getThreshold()) / 100.0);

    for (int frequency = frequencies.size() - 1; frequency > 0; frequency--) {
        CHECK_CONTINUE(!frequencies[frequency].empty());
        if (frequency >= thresholdCount && frequencies[frequency].size() == 1) {
            // A single character that fits the threshold found
            return frequencies[frequency].first();
        }
        if (frequency >= thresholdCount && frequencies[frequency].size() > 1) {
            // Two characters that fit the threshold found
            return mergeCharacters(frequencies[frequency]);
        }
        if (frequencies[frequency].size() > 1 || (frequencies[frequency].size() == 1 && bestCharacter != INVALID_CONS_CHAR)) {
            // There are no characters that fit the threshold, but we can merge a bit less popular characters
            return mergeCharacters(frequencies[frequency] << bestCharacter);
        }
        if (frequencies[frequency].size() == 1) {
            // There are no characters that fit the threshold and we found a single the most popular character.
            // We need more characters to merge them with this one.
            bestCharacter = frequencies[frequency].first();
        }
    }

    return INVALID_CONS_CHAR;
}

U2::MaConsensusAlgorithmSimpleExtended* MaConsensusAlgorithmSimpleExtended::clone() const {
    return new MaConsensusAlgorithmSimpleExtended(*this);
}

MaConsensusAlgorithmFactorySimpleExtended::MaConsensusAlgorithmFactorySimpleExtended()
    : MSAConsensusAlgorithmFactory(BuiltInConsensusAlgorithms::SIMPLE_EXTENDED_ALGO,
                                   ConsensusAlgorithmFlag_Nucleic | ConsensusAlgorithmFlag_SupportThreshold | ConsensusAlgorithmFlag_AvailableForChromatogram) {
    name = tr("Simple extended");
    description = tr("The algorithm selects the best character from the extended DNA alphabet. "
                     "Only bases with frequencies which are greater than a threshold value are taken into account.");
    minThreshold = 50;
    maxThreshold = 100;
    defaultThreshold = 100;
    thresholdSuffix = "%";
    isSequenceLikeResultFlag = true;
}

MSAConsensusAlgorithm* MaConsensusAlgorithmFactorySimpleExtended::createAlgorithm(const Msa& /*ma*/, bool ignoreTrailingLeadingGaps) {
    return new MaConsensusAlgorithmSimpleExtended(this, ignoreTrailingLeadingGaps);
}

}  // namespace U2
