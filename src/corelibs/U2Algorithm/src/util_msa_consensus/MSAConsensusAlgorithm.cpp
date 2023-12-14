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

#include "MSAConsensusAlgorithm.h"

#include <QVector>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MSAConsensusUtils.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Factory

MSAConsensusAlgorithmFactory::MSAConsensusAlgorithmFactory(const QString& algoId, ConsensusAlgorithmFlags _flags, QObject* p)
    : QObject(p), algorithmId(algoId), flags(_flags) {
}

ConsensusAlgorithmFlags MSAConsensusAlgorithmFactory::getAlphabetFlags(const DNAAlphabet* al) {
    if (al->getType() == DNAAlphabet_AMINO) {
        return ConsensusAlgorithmFlag_Amino;
    } else if (al->getType() == DNAAlphabet_NUCL) {
        return ConsensusAlgorithmFlag_Nucleic;
    }
    assert(al->getType() == DNAAlphabet_RAW);
    return ConsensusAlgorithmFlag_Raw;
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

const char MSAConsensusAlgorithm::INVALID_CONS_CHAR = '\0';

MSAConsensusAlgorithm::MSAConsensusAlgorithm(MSAConsensusAlgorithmFactory* _factory, bool ignoreTrailingLeadingGaps)
    : factory(_factory),
      threshold(0),
      ignoreTrailingAndLeadingGaps(ignoreTrailingLeadingGaps) {
}

MSAConsensusAlgorithm::MSAConsensusAlgorithm(const MSAConsensusAlgorithm& algorithm)
    : QObject(algorithm.parent()), factory(algorithm.factory),
      threshold(algorithm.threshold),
      ignoreTrailingAndLeadingGaps(algorithm.ignoreTrailingAndLeadingGaps) {
}

char MSAConsensusAlgorithm::getConsensusCharAndScore(const MultipleAlignment& ma, int column, int& score) const {
    char consensusChar = getConsensusChar(ma, column);

    // now compute score using most freq character
    int nonGaps = 0;
    QVector<int> freqsByChar(256);
    uchar topChar = MSAConsensusUtils::getColumnFreqs(ma, column, freqsByChar, nonGaps);
    score = freqsByChar[topChar];

    return consensusChar;
}

void MSAConsensusAlgorithm::setThreshold(int val) {
    int newThreshold = qBound(getMinThreshold(), val, getMaxThreshold());
    if (newThreshold == threshold) {
        return;
    }
    threshold = newThreshold;
    emit si_thresholdChanged(newThreshold);
}

QVector<int> MSAConsensusAlgorithm::pickRowsWithCharInCoreArea(const MultipleAlignment& ma, int pos) {
    QVector<int> seqIdx;
    int nSeq = ma->getRowCount();
    for (int rowIndex = 0; rowIndex < nSeq; rowIndex++) {
        const MultipleAlignmentRow& row = ma->getRow(rowIndex);
        if (!row->isTrailingOrLeadingGap(pos)) {
            seqIdx << rowIndex;
        }
    }
    return seqIdx;
}

QVector<int> MSAConsensusAlgorithm::pickRowsToUseInConsensus(const MultipleAlignment& ma, int pos) const {
    CHECK(ignoreTrailingAndLeadingGaps, emptyRowIdxStub);
    return pickRowsWithCharInCoreArea(ma, pos);
}

}  // namespace U2
