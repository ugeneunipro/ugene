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

#ifdef SW2_BUILD_WITH_CUDA

#ifndef _SW_CUDA_CPP_H
#define _SW_CUDA_CPP_H

#include <U2Core/U2Region.h>
#include <U2Algorithm/SmithWatermanSettings.h>

struct resType
{
    U2::U2Region refSubseq;
    U2::U2Region patternSubseq;
    QByteArray pairAlign;
    int score;
};

class sw_cuda_cpp {
public:
    typedef int ScoreType;

    QList<resType> launch(const char * seqLib, int seqLibLength, ScoreType* queryProfile, ScoreType qProfLen,
                            int queryLength, ScoreType gapOpen, ScoreType gapExtension, ScoreType maxScore,
                            U2::SmithWatermanSettings::SWResultView resultView);
    static quint64 estimateNeededGpuMemory( int seqLibLength, ScoreType qProfLen, int queryLength, const U2::SmithWatermanSettings::SWResultView resultView);
    static quint64 estimateNeededRamAmount(int seqLibLength, ScoreType qProfLen, int queryLength, const U2::SmithWatermanSettings::SWResultView resultView);
    static const int MAX_BLOCKS_NUMBER;
    static const int MAX_SHARED_VECTOR_LENGTH;
};

#endif

#endif //SW2_BUILD_WITH_CUDA
