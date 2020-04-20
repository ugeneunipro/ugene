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

#ifndef _SMITHWATERMANALGORITHM_CUDA_H
#define _SMITHWATERMANALGORITHM_CUDA_H

#include "SmithWatermanAlgorithm.h"

namespace U2 {

class SmithWatermanAlgorithmCUDA : public SmithWatermanAlgorithm {
public:
    typedef int ScoreType;

    virtual void launch(const SMatrix& m, const QByteArray & _patternSeq, const QByteArray & _searchSeq, int _gapOpen,
        int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView resultView);
    static quint64 estimateNeededGpuMemory( const SMatrix& sm, const QByteArray & _patternSeq, const QByteArray & _searchSeq, const SmithWatermanSettings::SWResultView resultView);
    static quint64 estimateNeededRamAmount(const SMatrix& sm, const QByteArray & _patternSeq, const QByteArray & _searchSeq, const SmithWatermanSettings::SWResultView resultView);
private:

};


} // namespace

#endif
#endif //SW2_BUILD_WITH_CUDA
