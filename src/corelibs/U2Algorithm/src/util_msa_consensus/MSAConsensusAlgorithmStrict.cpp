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

#include "MSAConsensusAlgorithmStrict.h"

#include <U2Core/MultipleSequenceAlignment.h>
#include "MSAConsensusUtils.h"

#include <QVector>

namespace U2 {


MSAConsensusAlgorithmFactoryStrict::MSAConsensusAlgorithmFactoryStrict(QObject* p)
: MSAConsensusAlgorithmFactory(BuiltInConsensusAlgorithms::STRICT_ALGO, ConsensusAlgorithmFlags_AllAlphabets
                               | ConsensusAlgorithmFlag_SupportThreshold
                               | ConsensusAlgorithmFlag_AvailableForChromatogram, p)
{
}


QString MSAConsensusAlgorithmFactoryStrict::getDescription() const  {
    return tr("The algorithm returns gap character ('-') if symbol frequency in a column is lower than threshold specified.");
}

QString MSAConsensusAlgorithmFactoryStrict::getName() const  {
    return tr("Strict");
}

MSAConsensusAlgorithm* MSAConsensusAlgorithmFactoryStrict::createAlgorithm(const MultipleAlignment&, bool ignoreTrailingLeadingGaps, QObject* p) {
    return new MSAConsensusAlgorithmStrict(this, ignoreTrailingLeadingGaps, p);
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

char MSAConsensusAlgorithmStrict::getConsensusChar(const MultipleAlignment& ma, int column, QVector<int> seqIdx) const {
    CHECK(filterIdx(seqIdx, ma, column), INVALID_CONS_CHAR);

    QVector<int> freqsByChar(256, 0);
    int nonGaps = 0;
    uchar topChar = MSAConsensusUtils::getColumnFreqs(ma, column, freqsByChar, nonGaps, seqIdx);

    //use gap is top char frequency is lower than threshold
    int nSeq =( seqIdx.isEmpty() ? ma->getNumRows() : seqIdx.size());
    int currentThreshold = getThreshold();
    int cntToUseGap = int(currentThreshold / 100.0 * nSeq);
    int topFreq = freqsByChar[topChar];
    char res = topFreq < cntToUseGap ? U2Msa::GAP_CHAR : (char)topChar;
    return res;
}

MSAConsensusAlgorithmStrict* MSAConsensusAlgorithmStrict::clone() const {
    return new MSAConsensusAlgorithmStrict(*this);
}

} //namespace
