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

#include "MaConsensusAlgorithmSimpleExtended.h"
#include "MSAConsensusAlgorithmLevitsky.h"

#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

MSAConsensusAlgorithmFactoryLevitsky::MSAConsensusAlgorithmFactoryLevitsky(QObject* p)
    : MSAConsensusAlgorithmFactory(BuiltInConsensusAlgorithms::LEVITSKY_ALGO,
                                   ConsensusAlgorithmFlag_Nucleic | ConsensusAlgorithmFlag_SupportThreshold,
                                   p) {
}

QString MSAConsensusAlgorithmFactoryLevitsky::getDescription() const {
    return tr("The algorithm proposed by Victor Levitsky to work with DNA alignments.\n"
              "Collects global alignment frequency for every symbol using extended (15 symbols) DNA alphabet first.\n"
              "For every column selects the most rare symbol in the whole alignment with percentage in the column "
              "greater or equals to the threshold value.");
}

QString MSAConsensusAlgorithmFactoryLevitsky::getName() const {
    return tr("Levitsky");
}

MSAConsensusAlgorithm* MSAConsensusAlgorithmFactoryLevitsky::createAlgorithm(const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps, QObject* p) {
    return new MSAConsensusAlgorithmLevitsky(this, ma, ignoreTrailingLeadingGaps, p);
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

static const QVector<uchar> SINGLE_NUCL = {'A', 'C', 'G', 'T', 'U'};
static const int SINGLE_NUCL_SIZE = SINGLE_NUCL.size();
static const QVector<uchar> DOUBLE_NUCL = {'W', 'R', 'M', 'K', 'Y', 'S'};
static const int SINGLE_AND_DOUBLE_NUCL_SIZE = SINGLE_NUCL_SIZE + DOUBLE_NUCL.size();
static const QVector<uchar> TRIPLE_NUCL = {'B', 'V', 'H', 'D'};
static const int SINGLE_AND_DOUBLE_AND_TRIPLE_NUCL_SIZE = SINGLE_AND_DOUBLE_NUCL_SIZE + TRIPLE_NUCL.size();
static const QVector<uchar> getAllPossibleNucl() {
    QVector<uchar> allPossibleNucl;
    allPossibleNucl.append(SINGLE_NUCL);
    allPossibleNucl.append(DOUBLE_NUCL);
    allPossibleNucl.append(TRIPLE_NUCL);
    allPossibleNucl.append('N');

    return allPossibleNucl;
};
static const QVector<uchar> ALL_POSSIBLE_NUCL = getAllPossibleNucl();

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

MSAConsensusAlgorithmLevitsky::MSAConsensusAlgorithmLevitsky(MSAConsensusAlgorithmFactoryLevitsky* f, const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps, QObject* p)
    : MSAConsensusAlgorithm(f, ignoreTrailingLeadingGaps, p)
    , globalFreqs(QVarLengthArray<int>(256)) {
    int *freqsData = globalFreqs.data();
    std::fill(globalFreqs.begin(), globalFreqs.end(), 0);
    int len = ma->getLength();
    foreach (const MultipleAlignmentRow& row, ma->getRows()) {
        for (int i = 0; i < len; i++) {
            char c = row->charAt(i);
            registerHit(freqsData, c);
        }
    }
}

char MSAConsensusAlgorithmLevitsky::getConsensusChar(const MultipleAlignment& ma, int column, QVector<int> seqIdx) const {
    CHECK(filterIdx(seqIdx, ma, column), INVALID_CONS_CHAR);

    // count local freqs first
    QVarLengthArray<int> localFreqs(256);
    int* freqsData = localFreqs.data();
    std::fill(localFreqs.begin(), localFreqs.end(), 0);
    int nSeq = (seqIdx.isEmpty() ? ma->getRowCount() : seqIdx.size());
    for (int seq = 0; seq < nSeq; seq++) {
        char c = ma->charAt(seqIdx.isEmpty() ? seq : seqIdx[seq], column);
        registerHit(freqsData, c);
    }
    // find all symbols with freq > threshold, select one with the lowest global freq
    QVector<char> selectedChars;
    double selectedGlobalPercentage = 1;
    int maLength = ma->getLength();
    double thresholdScore = getThreshold() / 100.0;
    for (int i = 0; i < ALL_POSSIBLE_NUCL.size(); i++) {
        uchar ch = ALL_POSSIBLE_NUCL[i];
        if (i == SINGLE_NUCL_SIZE
         || i == SINGLE_AND_DOUBLE_NUCL_SIZE
         || i == SINGLE_AND_DOUBLE_AND_TRIPLE_NUCL_SIZE) {
            CHECK_BREAK(selectedChars.isEmpty());
        }

        double localPercentage = (double)freqsData[ch] / nSeq;
        CHECK_CONTINUE(localPercentage >= thresholdScore);

        double globalFreqsCh = (double)globalFreqs[ch];
        double globalPercentage = globalFreqsCh / (nSeq * maLength);
        CHECK_CONTINUE(globalPercentage <= selectedGlobalPercentage);

        if (globalPercentage < selectedGlobalPercentage) {
            selectedChars.clear();
            selectedGlobalPercentage = globalPercentage;
        }

        selectedChars << ch;
    }
    CHECK(!selectedChars.isEmpty(), U2Msa::GAP_CHAR);
    CHECK(selectedChars.size() != 1, selectedChars.first());

    return MaConsensusAlgorithmSimpleExtended::mergeCharacters(selectedChars);
}

MSAConsensusAlgorithmLevitsky* MSAConsensusAlgorithmLevitsky::clone() const {
    return new MSAConsensusAlgorithmLevitsky(*this);
}

}  // namespace U2
