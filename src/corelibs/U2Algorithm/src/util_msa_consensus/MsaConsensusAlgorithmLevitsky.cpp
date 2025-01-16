/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "MsaConsensusAlgorithmLevitsky.h"

#include <U2Core/Msa.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

MsaConsensusAlgorithmFactoryLevitsky::MsaConsensusAlgorithmFactoryLevitsky()
    : MsaConsensusAlgorithmFactory(BuiltInConsensusAlgorithms::LEVITSKY_ALGO,
                                   ConsensusAlgorithmFlag_Nucleic | ConsensusAlgorithmFlag_SupportThreshold) {
    name = tr("Levitsky");
    description = tr("The algorithm proposed by Victor Levitsky to work with DNA alignments.\n"
                     "Collects global alignment frequency for every symbol using extended (15 symbols) DNA alphabet first.\n"
                     "For every column selects the most rare symbol in the whole alignment with percentage in the column "
                     "greater or equals to the threshold value.");
    minThreshold = 50;
    maxThreshold = 100;
    defaultThreshold = 90;
    thresholdSuffix = "%";
    isSequenceLikeResultFlag = true;
}

MsaConsensusAlgorithm* MsaConsensusAlgorithmFactoryLevitsky::createAlgorithm(const Msa& ma, bool ignoreTrailingLeadingGaps) {
    return new MsaConsensusAlgorithmLevitsky(this, ma, ignoreTrailingLeadingGaps);
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

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
static void registerHit(int* data, char c) {
    int idx = uchar(c);
    data[idx]++;
    switch (c) {
        case 'A':
            data['W']++;
            data['R']++;
            data['M']++;
            data['V']++;
            data['H']++;
            data['D']++;
            data['N']++;
            break;
        case 'C':
            data['M']++;
            data['Y']++;
            data['S']++;
            data['B']++;
            data['V']++;
            data['H']++;
            data['N']++;
            break;
        case 'G':
            data['R']++;
            data['K']++;
            data['S']++;
            data['B']++;
            data['V']++;
            data['D']++;
            data['N']++;
            break;
        case 'T':
        case 'U':
            data['W']++;
            data['K']++;
            data['Y']++;
            data['B']++;
            data['H']++;
            data['D']++;
            data['N']++;
            break;
    }
}

MsaConsensusAlgorithmLevitsky::MsaConsensusAlgorithmLevitsky(MsaConsensusAlgorithmFactoryLevitsky* f, const Msa& ma, bool ignoreTrailingLeadingGaps)
    : MsaConsensusAlgorithm(f, ignoreTrailingLeadingGaps), globalFreqs(QVarLengthArray<int>(256)) {
    reinitializeData(ma);
}

static QByteArray BASE_DNA_CHARS("ACGTU");
static QByteArray EXT_DNA_CHARS_ONE_OF_TWO("WRMKYS");
static QByteArray EXT_DNA_CHARS_ONE_OF_THREE("BVHD");
static QByteArray EXT_DNA_CHARS_ONE_OF_FOUR("N");
static QByteArray ALL_EXT_DNA_CHARACTERS = BASE_DNA_CHARS + EXT_DNA_CHARS_ONE_OF_TWO + EXT_DNA_CHARS_ONE_OF_THREE + EXT_DNA_CHARS_ONE_OF_FOUR;

static const int GROUP1_LAST_IDX = BASE_DNA_CHARS.size() - 1;
static const int GROUP2_LAST_IDX = GROUP1_LAST_IDX + EXT_DNA_CHARS_ONE_OF_TWO.size();
static const int GROUP3_LAST_IDX = GROUP2_LAST_IDX + EXT_DNA_CHARS_ONE_OF_THREE.size();
static int getCharacterGroup(int allExtDnaCharactersIndex) {
    return allExtDnaCharactersIndex <= GROUP1_LAST_IDX   ? 0
           : allExtDnaCharactersIndex <= GROUP2_LAST_IDX ? 1
           : allExtDnaCharactersIndex <= GROUP3_LAST_IDX ? 2
                                                         : 3;
}

enum DnaExtMask {
    None = 0,
    A = 1 << 0,
    C = 1 << 1,
    G = 1 << 2,
    T = 1 << 3,
    U = T,
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
    N = A + C + G + T,
};

static QVector<DnaExtMask> createChar2MaskMapping() {
    QVector<DnaExtMask> result(256, DnaExtMask::None);
    result[U2Msa::GAP_CHAR] = DnaExtMask::None;
    result['A'] = DnaExtMask::A;
    result['C'] = DnaExtMask::C;
    result['G'] = DnaExtMask::G;
    result['T'] = DnaExtMask::T;
    result['U'] = DnaExtMask::U;
    result['W'] = DnaExtMask::W;
    result['R'] = DnaExtMask::R;
    result['M'] = DnaExtMask::M;
    result['K'] = DnaExtMask::K;
    result['Y'] = DnaExtMask::Y;
    result['S'] = DnaExtMask::S;
    result['B'] = DnaExtMask::B;
    result['V'] = DnaExtMask::V;
    result['H'] = DnaExtMask::H;
    result['D'] = DnaExtMask::D;
    result['N'] = DnaExtMask::N;
    return result;
}

static const QVector<DnaExtMask> CHAR_2_MASK = createChar2MaskMapping();

static QVector<char> createMask2CharMapping() {
    QVector<char> result(16, U2Msa::GAP_CHAR);
    for (int c = 0; c < CHAR_2_MASK.length(); c++) {
        int mask = CHAR_2_MASK[c];
        SAFE_POINT(mask >= 0 && mask < result.length(), "Invalid mapping in createChar2MaskMapping()", result);
        result[mask] = (char)c;
    }
    return result;
}

static const QVector<char> MASK_2_CHAR = createMask2CharMapping();

static int mergeCharsIntoMask(const char* chars, int length) {
    int mask = 0;
    for (int i = 0; i < length; i++) {
        mask = mask | CHAR_2_MASK[chars[i]];
    }
    return mask;
}

char MsaConsensusAlgorithmLevitsky::getConsensusChar(const Msa& ma, int column) const {
    QVector<int> seqIdx = pickRowsToUseInConsensus(ma, column);
    CHECK(!ignoreTrailingAndLeadingGaps || !seqIdx.isEmpty(), INVALID_CONS_CHAR);

    // Count column-local frequencies.
    QVarLengthArray<int, 91> localFreqs(91);  // Max tracked 'Z' character index is 90.
    int* freqsData = localFreqs.data();
    std::fill(localFreqs.begin(), localFreqs.end(), 0);
    int nSeq = (seqIdx.isEmpty() ? ma->getRowCount() : seqIdx.size());
    for (int seq = 0; seq < nSeq; seq++) {
        char c = ma->charAt(seqIdx.isEmpty() ? seq : seqIdx[seq], column);
        if (c >= 'A' && c <= 'Z') {
            registerHit(freqsData, c);
        }
    }
    // find all symbols with freq > threshold, select one with the lowest global freq
    double selectedGlobalPercentage = 2;  // Some value > 100% (2 == 200%).
    double thresholdScore = getThreshold() / 100.0;
    int selectedGroupIndex = -1;
    QVarLengthArray<char, 16> selectedChars;
    for (int i = 0; i < ALL_EXT_DNA_CHARACTERS.length(); i++) {
        char c = ALL_EXT_DNA_CHARACTERS[i];
        double localPercentage = (double)localFreqs[uchar(c)] / nSeq;
        if (localPercentage < thresholdScore) {
            continue;
        }
        double globalPercentage = (double)globalFreqs[uchar(c)] / (nSeq * ma->getLength());
        if (globalPercentage > selectedGlobalPercentage) {
            continue;
        }
        int groupIndex = getCharacterGroup(i);
        if (globalPercentage < selectedGlobalPercentage) {
            selectedChars.clear();
            selectedChars.append(c);
            selectedGlobalPercentage = globalPercentage;
            selectedGroupIndex = groupIndex;
        } else if (groupIndex == selectedGroupIndex) {  // Skip characters from more ambiguous groups with the same score.
            SAFE_POINT(selectedGroupIndex >= 0, "Invalid selectedGroupIndex in MSAConsensusAlgorithmLevitsky", U2Msa::GAP_CHAR);
            selectedChars.append(c);
        }
    }
    CHECK(!selectedChars.isEmpty(), U2Msa::GAP_CHAR);
    CHECK(selectedChars.size() > 1, selectedChars[0]);
    int mask = mergeCharsIntoMask(selectedChars.constData(), selectedChars.length());
    return MASK_2_CHAR[mask];
}

MsaConsensusAlgorithmLevitsky* MsaConsensusAlgorithmLevitsky::clone() const {
    return new MsaConsensusAlgorithmLevitsky(*this);
}

void MsaConsensusAlgorithmLevitsky::reinitializeData(const Msa& ma) {
    int* freqsData = globalFreqs.data();
    std::fill(globalFreqs.begin(), globalFreqs.end(), 0);
    const auto& maRows = ma->getRows();
    int len = ma->getLength();
    for (const MsaRow& row : qAsConst(maRows)) {
        for (int i = 0; i < len; i++) {
            char c = row->charAt(i);
            registerHit(freqsData, c);
        }
    }
}

}  // namespace U2
