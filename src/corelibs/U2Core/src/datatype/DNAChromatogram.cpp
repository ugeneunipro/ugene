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

#include <U2Core/U2SafePoints.h>

#include "DNAChromatogram.h"

namespace U2 {

const ushort DNAChromatogram::INVALID_VALUE = 0;
const char DNAChromatogram::DEFAULT_PROBABILITY = 0; // SANGER_TODO: 100?

DNAChromatogram::DNAChromatogram()
    : name("chromatogram"),
      traceLength(0),
      seqLength(0),
      hasQV(false)
{

}

ushort DNAChromatogram::getValue(Trace trace, qint64 position) const {
    SAFE_POINT(0 <= position && position <= traceLength, "The position is out of trace boundaries", 0);
    switch (trace) {
    case Trace_A:
        SAFE_POINT(0 <= position && position <= A.length(), "The position is out of trace A boundaries", 0);
        return A[position];
    case Trace_C:
        SAFE_POINT(0 <= position && position <= C.length(), "The position is out of trace C boundaries", 0);
        return C[position];
    case Trace_G:
        SAFE_POINT(0 <= position && position <= G.length(), "The position is out of trace G boundaries", 0);
        return G[position];
    case Trace_T:
        SAFE_POINT(0 <= position && position <= T.length(), "The position is out of trace T boundaries", 0);
        return T[position];
    default:
        FAIL("An unknown trace", 0);
    }
}

bool DNAChromatogram::operator ==(const DNAChromatogram &otherChromatogram) const {
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

}   // namespace U2
