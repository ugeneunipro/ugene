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

#include <U2Core/U2SafePoints.h>

#include "Chromatogram.h"

namespace U2 {

Chromatogram::Chromatogram()
    : QSharedDataPointer<ChromatogramData>(new ChromatogramData()) {
}

Chromatogram::Chromatogram(ChromatogramData* data)
    : QSharedDataPointer<ChromatogramData>(data) {
    SAFE_POINT_NN(data, );
}

const QVarLengthArray<char, 4> ChromatogramData::BASE_BY_TRACE = {'A', 'C', 'G', 'T'};

ushort ChromatogramData::getValue(const Trace& trace, qint64 position) const {
    SAFE_POINT(0 <= position && position <= traceLength, "The position is out of trace boundaries", 0);
    switch (trace) {
        case Trace::Trace_A:
            SAFE_POINT(position >= 0 && position <= A.length(), "The position is out of trace A boundaries", 0);
            return A[(int)position];
        case Trace::Trace_C:
            SAFE_POINT(position >= 0 && position <= C.length(), "The position is out of trace C boundaries", 0);
            return C[(int)position];
        case Trace::Trace_G:
            SAFE_POINT(position >= 0 && position <= G.length(), "The position is out of trace G boundaries", 0);
            return G[(int)position];
        case Trace::Trace_T:
            SAFE_POINT(position >= 0 && position <= T.length(), "The position is out of trace T boundaries", 0);
            return T[(int)position];
        default:
            FAIL("An unknown trace", 0);
    }
}

bool ChromatogramData::operator==(const ChromatogramData& otherChromatogram) const {
    return traceLength == otherChromatogram.traceLength &&
           seqLength == otherChromatogram.seqLength &&
           baseCalls == otherChromatogram.baseCalls &&
           A == otherChromatogram.A &&
           C == otherChromatogram.C &&
           G == otherChromatogram.G &&
           T == otherChromatogram.T &&
           prob_A == otherChromatogram.prob_A &&
           prob_C == otherChromatogram.prob_C &&
           prob_G == otherChromatogram.prob_G &&
           prob_T == otherChromatogram.prob_T &&
           hasQV == otherChromatogram.hasQV;
}

bool ChromatogramData::isEmpty() const {
    return traceLength == 0;
}
}  // namespace U2
