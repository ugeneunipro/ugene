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

#pragma once

#include <emmintrin.h>

#include "SmithWatermanAlgorithm.h"

namespace U2 {

class SmithWatermanAlgorithmSSE2 : public SmithWatermanAlgorithm {
public:
    void launch(const SMatrix& substitutionMatrix, const QByteArray& _patternSeq, const QByteArray& _searchSeq, int _gapOpen, int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView resultView) override;

    static quint64 estimateNeededRamAmount(const QByteArray& _patternSeq,
                                           const QByteArray& _searchSeq,
                                           qint32 gapOpen,
                                           qint32 gapExtension,
                                           quint32 minScore,
                                           quint32 maxScore,
                                           SmithWatermanSettings::SWResultView resultView);

private:
    static const int nElementsInVec = 8;
    void calculateMatrixForMultipleAlignmentResultWithShort();
    void calculateMatrixForAnnotationsResultWithShort();
    void calculateMatrixForMultipleAlignmentResultWithInt();
    void calculateMatrixForAnnotationsResultWithInt();
    int calculateMatrixSSE2(unsigned queryLength, unsigned char* dbSeq, unsigned dbLength, unsigned short gapOpenOrig, unsigned short gapExtend);
};

}  // namespace U2
