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

#include "MsaConsensusAlgorithmStrict.h"

#include <QVector>

#include <U2Core/Msa.h>
#include <U2Core/U2SafePoints.h>

#include "MsaConsensusUtils.h"

namespace U2 {

MsaConsensusAlgorithmFactoryStrict::MsaConsensusAlgorithmFactoryStrict()
    : MsaConsensusAlgorithmFactory(BuiltInConsensusAlgorithms::STRICT_ALGO, ConsensusAlgorithmFlags_AllAlphabets | ConsensusAlgorithmFlag_SupportThreshold | ConsensusAlgorithmFlag_AvailableForChromatogram) {
    name = tr("Strict");
    description = tr("The algorithm returns gap character ('-') if symbol frequency in a column is lower than threshold specified.");
    minThreshold = 1;
    maxThreshold = 100;
    defaultThreshold = 100;
    thresholdSuffix = "%";
    isSequenceLikeResultFlag = true;
}

MsaConsensusAlgorithm* MsaConsensusAlgorithmFactoryStrict::createAlgorithm(const Msa&, bool ignoreTrailingLeadingGaps) {
    return new MsaConsensusAlgorithmStrict(this, ignoreTrailingLeadingGaps);
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

char MsaConsensusAlgorithmStrict::getConsensusChar(const Msa& ma, int column) const {
    QVector<int> seqIdx = pickRowsToUseInConsensus(ma, column);
    CHECK(!ignoreTrailingAndLeadingGaps || !seqIdx.isEmpty(), INVALID_CONS_CHAR);

    QVector<int> freqsByChar(256, 0);
    int nonGaps = 0;
    uchar topChar = MsaConsensusUtils::getColumnFreqs(ma, column, freqsByChar, nonGaps, seqIdx);

    // use gap is top char frequency is lower than threshold
    int nSeq = (seqIdx.isEmpty() ? ma->getRowCount() : seqIdx.size());
    int currentThreshold = getThreshold();
    double cntToUseGap = currentThreshold / 100.0 * nSeq;
    double topFreq = freqsByChar[topChar];
    char res = topFreq < cntToUseGap ? U2Msa::GAP_CHAR : (char)topChar;
    return res;
}

MsaConsensusAlgorithmStrict* MsaConsensusAlgorithmStrict::clone() const {
    return new MsaConsensusAlgorithmStrict(*this);
}

}  // namespace U2
