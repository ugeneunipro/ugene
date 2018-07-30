/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include "MSAConsensusUtils.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2OpStatusUtils.h>

#include <QVector>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Factory

MSAConsensusAlgorithmFactory::MSAConsensusAlgorithmFactory(const QString& algoId, ConsensusAlgorithmFlags _flags, QObject* p)
: QObject(p), algorithmId(algoId), flags(_flags)
{
}

ConsensusAlgorithmFlags MSAConsensusAlgorithmFactory::getAphabetFlags(const DNAAlphabet* al) {
    if (al->getType() == DNAAlphabet_AMINO) {
        return ConsensusAlgorithmFlag_Amino;
    } else if (al->getType() == DNAAlphabet_NUCL) {
        return ConsensusAlgorithmFlag_Nucleic;
    }
    assert(al->getType() ==  DNAAlphabet_RAW);
    return ConsensusAlgorithmFlag_Raw;
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

char MSAConsensusAlgorithm::INVALID_CONS_CHAR = '\0';
MSAConsensusAlgorithm::MSAConsensusAlgorithm(MSAConsensusAlgorithmFactory* _factory, bool ignoreTrailingLeadingGaps, QObject* p)
    : QObject(p), factory(_factory),
      threshold(0),
      ignoreTrailingAndLeadingGaps(ignoreTrailingLeadingGaps) {

}

MSAConsensusAlgorithm::MSAConsensusAlgorithm(const MSAConsensusAlgorithm &algorithm)
    : QObject(algorithm.parent()), factory(algorithm.factory),
    threshold(algorithm.threshold),
    ignoreTrailingAndLeadingGaps(algorithm.ignoreTrailingAndLeadingGaps) {

}

char MSAConsensusAlgorithm::getConsensusCharAndScore(const MultipleAlignment& ma, int column, int& score,
                                                     QVector<int> seqIdx) const {
    char consensusChar = getConsensusChar(ma, column, seqIdx);

    //now compute score using most freq character
    int nonGaps = 0;
    QVector<int> freqsByChar(256);
    uchar topChar = MSAConsensusUtils::getColumnFreqs(ma, column, freqsByChar, nonGaps, seqIdx);
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

bool MSAConsensusAlgorithm::filterIdx(QVector<int> &seqIdx, const MultipleAlignment& ma, const int pos) const {
    CHECK(ignoreTrailingAndLeadingGaps, true);

    QVector<int> tmp;
    int nSeq = seqIdx.isEmpty() ? ma->getNumRows() : seqIdx.size();
    for (int seq = 0; seq < nSeq; seq++) {
        int rowNum = seqIdx.isEmpty() ? seq : seqIdx[ seq ];
        const MultipleAlignmentRow& row = ma->getRow(rowNum);
        if (row->isTrailingOrLeadingGap(pos)) {
            continue;
        }
        tmp << rowNum;
    }
    if (tmp.size() != nSeq) {
        seqIdx = tmp;
    }
    return !tmp.isEmpty();
}

} //namespace
