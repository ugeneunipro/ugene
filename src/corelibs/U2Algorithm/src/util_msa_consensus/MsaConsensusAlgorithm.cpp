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

#include "MsaConsensusAlgorithm.h"

#include <QVector>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MsaConsensusUtils.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Factory

MsaConsensusAlgorithmFactory::MsaConsensusAlgorithmFactory(const QString& algoId, ConsensusAlgorithmFlags _flags)
    : algorithmId(algoId), flags(_flags) {
}

ConsensusAlgorithmFlags MsaConsensusAlgorithmFactory::getAlphabetFlags(const DNAAlphabet* al) {
    if (al->getType() == DNAAlphabet_AMINO) {
        return ConsensusAlgorithmFlag_Amino;
    } else if (al->getType() == DNAAlphabet_NUCL) {
        return ConsensusAlgorithmFlag_Nucleic;
    }
    assert(al->getType() == DNAAlphabet_RAW);
    return ConsensusAlgorithmFlag_Raw;
}

QString MsaConsensusAlgorithmFactory::getId() const {
    return algorithmId;
}

ConsensusAlgorithmFlags MsaConsensusAlgorithmFactory::getFlags() const {
    return flags;
}

QString MsaConsensusAlgorithmFactory::getDescription() const {
    return description;
}

QString MsaConsensusAlgorithmFactory::getName() const {
    return name;
}

bool MsaConsensusAlgorithmFactory::supportsThreshold() const {
    return flags.testFlag(ConsensusAlgorithmFlag_SupportThreshold);
}

int MsaConsensusAlgorithmFactory::getMinThreshold() const {
    return minThreshold;
}

int MsaConsensusAlgorithmFactory::getMaxThreshold() const {
    return maxThreshold;
};

int MsaConsensusAlgorithmFactory::getDefaultThreshold() const {
    return defaultThreshold;
};

QString MsaConsensusAlgorithmFactory::getThresholdSuffix() const {
    return thresholdSuffix;
}

bool MsaConsensusAlgorithmFactory::isSequenceLikeResult() const {
    return isSequenceLikeResultFlag;
}
//////////////////////////////////////////////////////////////////////////
// Algorithm

const char MsaConsensusAlgorithm::INVALID_CONS_CHAR = '\0';

MsaConsensusAlgorithm::MsaConsensusAlgorithm(MsaConsensusAlgorithmFactory* _factory, bool ignoreTrailingLeadingGaps)
    : factory(_factory),
      threshold(0),
      ignoreTrailingAndLeadingGaps(ignoreTrailingLeadingGaps) {
}

MsaConsensusAlgorithm::MsaConsensusAlgorithm(const MsaConsensusAlgorithm& algorithm)
    : QObject(algorithm.parent()), factory(algorithm.factory),
      threshold(algorithm.threshold),
      ignoreTrailingAndLeadingGaps(algorithm.ignoreTrailingAndLeadingGaps) {
}

char MsaConsensusAlgorithm::getConsensusCharAndScore(const Msa& ma, int column, int& score) const {
    char consensusChar = getConsensusChar(ma, column);

    // now compute score using most freq character
    int nonGaps = 0;
    QVector<int> freqsByChar(256);
    uchar topChar = MsaConsensusUtils::getColumnFreqs(ma, column, freqsByChar, nonGaps);
    score = freqsByChar[topChar];

    return consensusChar;
}

void MsaConsensusAlgorithm::setThreshold(int val) {
    int newThreshold = qBound(getMinThreshold(), val, getMaxThreshold());
    if (newThreshold == threshold) {
        return;
    }
    threshold = newThreshold;
    emit si_thresholdChanged(newThreshold);
}

QVector<int> MsaConsensusAlgorithm::pickRowsWithCharInCoreArea(const Msa& ma, int pos) {
    QVector<int> seqIdx;
    int nSeq = ma->getRowCount();
    for (int rowIndex = 0; rowIndex < nSeq; rowIndex++) {
        const MsaRow& row = ma->getRow(rowIndex);
        if (!row->isTrailingOrLeadingGap(pos)) {
            seqIdx << rowIndex;
        }
    }
    return seqIdx;
}

QVector<int> MsaConsensusAlgorithm::pickRowsToUseInConsensus(const Msa& ma, int pos) const {
    CHECK(ignoreTrailingAndLeadingGaps, emptyRowIdxStub);
    return pickRowsWithCharInCoreArea(ma, pos);
}

QString MsaConsensusAlgorithm::getDescription() const {
    return factory->getDescription();
}

QString MsaConsensusAlgorithm::getName() const {
    return factory->getName();
}

int MsaConsensusAlgorithm::getThreshold() const {
    return threshold;
}

bool MsaConsensusAlgorithm::supportsThreshold() const {
    return factory->supportsThreshold();
}

int MsaConsensusAlgorithm::getMinThreshold() const {
    return factory->getMinThreshold();
}

int MsaConsensusAlgorithm::getMaxThreshold() const {
    return factory->getMaxThreshold();
}

int MsaConsensusAlgorithm::getDefaultThreshold() const {
    return factory->getDefaultThreshold();
}

QString MsaConsensusAlgorithm::getThresholdSuffix() const {
    return factory->getThresholdSuffix();
}

QString MsaConsensusAlgorithm::getId() const {
    return factory->getId();
}

MsaConsensusAlgorithmFactory* MsaConsensusAlgorithm::getFactory() const {
    return factory;
}

}  // namespace U2
