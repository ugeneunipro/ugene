/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "MsaConsensusAlgorithmDefault.h"

#include <QVector>

#include <U2Core/Msa.h>
#include <U2Core/Timer.h>

namespace U2 {

MsaConsensusAlgorithmFactoryDefault::MsaConsensusAlgorithmFactoryDefault()
    : MsaConsensusAlgorithmFactory(BuiltInConsensusAlgorithms::DEFAULT_ALGO, ConsensusAlgorithmFlags_NuclAmino | ConsensusAlgorithmFlag_SupportThreshold) {
    name = tr("Default");
    description = tr("Based on JalView algorithm. Returns '+' if there are 2 characters with high frequency. Returns symbol in lower case if the symbol content in a row is lower than the threshold specified.");
    minThreshold = 1;
    maxThreshold = 100;
    defaultThreshold = 100;
    thresholdSuffix = "%";
}

MsaConsensusAlgorithm* MsaConsensusAlgorithmFactoryDefault::createAlgorithm(const Msa&, bool ignoreTrailingLeadingGaps) {
    return new MsaConsensusAlgorithmDefault(this, ignoreTrailingLeadingGaps);
}

U2::MsaConsensusAlgorithmDefault* MsaConsensusAlgorithmDefault::clone() const {
    return new MsaConsensusAlgorithmDefault(*this);
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

char MsaConsensusAlgorithmDefault::getConsensusCharAndScore(const Msa& ma, int pos, int& cnt) const {
    //    GTIMER(timer, tm, "MSAConsensusAlgorithmDefault::getConsensusCharAndScore");
    QVector<int> seqIdx = pickRowsToUseInConsensus(ma, pos);
    CHECK(!ignoreTrailingAndLeadingGaps || !seqIdx.isEmpty(), INVALID_CONS_CHAR);

    QVarLengthArray<QPair<int, char>, 32> frequencies(32);
    std::fill(frequencies.begin(), frequencies.end(), QPair<int, char>(0, '-'));
    char ch;
    int nSeq = seqIdx.isEmpty() ? ma->getRowCount() : seqIdx.size();
    for (int seq = 0; seq < nSeq; seq++) {
        char c = ma->charAt(seqIdx.isEmpty() ? seq : seqIdx[seq], pos);
        if (c >= 'A' && c <= 'Z') {
            int idx = c - 'A';
            frequencies[idx].first++;
            frequencies[idx].second = c;
        }
    }
    std::sort(frequencies.begin(), frequencies.end());
    int p1 = frequencies[frequencies.size() - 1].first;
    int p2 = frequencies[frequencies.size() - 2].first;
    if (p1 == 0 || (p1 == 1 && nSeq > 1)) {
        ch = U2Msa::GAP_CHAR;
        cnt = 0;
    } else {
        char c1 = frequencies[frequencies.size() - 1].second;
        ch = p2 == p1 ? '+' : c1;
        cnt = p1;
    }

    // lowercase alpha chars with < threshold% content
    int currentThreshold = getThreshold();
    int cntToUseLowerCase = int(currentThreshold / 100.0 * nSeq);
    if (cnt < cntToUseLowerCase && (ch >= 'A' && ch <= 'Z')) {
        ch = char(ch + ('a' - 'A'));
    }
    return ch;
}

}  // namespace U2
