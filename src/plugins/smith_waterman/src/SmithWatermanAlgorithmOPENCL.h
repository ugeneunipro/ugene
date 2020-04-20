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

#ifdef SW2_BUILD_WITH_OPENCL

#ifndef _SMITHWATERMANALGORITHM_OPENCL_H
#define _SMITHWATERMANALGORITHM_OPENCL_H

#include <U2Core/AppContext.h>
#include <U2Algorithm/OpenCLGpuRegistry.h>
#include <U2Algorithm/OpenCLHelper.h>

#include "SmithWatermanAlgorithm.h"

namespace U2 {

class SmithWatermanAlgorithmOPENCL : public SmithWatermanAlgorithm {
public:
    typedef int ScoreType;

    SmithWatermanAlgorithmOPENCL();
    ~SmithWatermanAlgorithmOPENCL();

    virtual void launch(const SMatrix& m, const QByteArray & _patternSeq, const QByteArray & _searchSeq, int _gapOpen,
        int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView resultView);
    static quint64 estimateNeededGpuMemory(const SMatrix& sm, const QByteArray & _patternSeq, const QByteArray & _searchSeq);
    static quint64 estimateNeededRamAmount(const SMatrix& sm, const QByteArray & _patternSeq, const QByteArray & _searchSeq,
                                            const SmithWatermanSettings::SWResultView resultView);

private:

    int calcOverlap(int queryLength);
    static int calcPartsNumber(int seqLibLength, int overlapLength);
    static int calcPartSeqSize(int seqLibLength, int overlapLength, int partsNumber);
    static int calcSizeRow(int partsNumber, int partSeqSize);

    static int MAX_BLOCKS_NUMBER;
    static const int MAX_SHARED_VECTOR_LENGTH;

    cl_event clEvent;
    cl_kernel clKernel;
    cl_program clProgram;
    cl_command_queue clCommandQueue;
    cl_context clContext;

    cl_mem queryProfBuf;
    cl_mem seqLibProfBuf;
    cl_mem hDataMaxBuf;
    cl_mem hDataUpBufTmp;
    cl_mem hDataRecBufTmp;
    cl_mem fDataUpBuf;
    cl_mem directionsUpBufTmp;
    cl_mem directionsRecBufTmp;
    cl_mem directionsMaxBuf;

    cl_mem directionsMatrix;
    cl_mem backtraceBegins;
};

} // namespace

#endif
#endif //SW2_BUILD_WITH_OPENCL
