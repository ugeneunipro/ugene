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

#include <U2Core/DNAChromatogram.h>

#include "DnaChromatogramUtils.h"

namespace U2 {

void DnaChromatogramUtils::append(DNAChromatogram &chromatogram, const DNAChromatogram &appendedChromatogram) {
    chromatogram.traceLength += appendedChromatogram.traceLength;
    chromatogram.seqLength += appendedChromatogram.seqLength;
    chromatogram.baseCalls += appendedChromatogram.baseCalls;
    chromatogram.A += appendedChromatogram.A;
    chromatogram.C += appendedChromatogram.C;
    chromatogram.G += appendedChromatogram.G;
    chromatogram.T += appendedChromatogram.T;
    chromatogram.prob_A += appendedChromatogram.prob_A;
    chromatogram.prob_C += appendedChromatogram.prob_C;
    chromatogram.prob_G += appendedChromatogram.prob_G;
    chromatogram.prob_T += appendedChromatogram.prob_T;
    chromatogram.hasQV &= appendedChromatogram.hasQV;
}

void DnaChromatogramUtils::crop(DNAChromatogram &chromatogram, int startPos, int length) {
    chromatogram.traceLength = qMin(chromatogram.traceLength - startPos, length);
    chromatogram.seqLength = qMin(chromatogram.seqLength - startPos, length);
    chromatogram.baseCalls = chromatogram.baseCalls.mid(startPos, length);
    chromatogram.A = chromatogram.A.mid(startPos, length);
    chromatogram.C = chromatogram.C.mid(startPos, length);
    chromatogram.G = chromatogram.G.mid(startPos, length);
    chromatogram.T = chromatogram.T.mid(startPos, length);
    chromatogram.prob_A = chromatogram.prob_A.mid(startPos, length);
    chromatogram.prob_C = chromatogram.prob_C.mid(startPos, length);
    chromatogram.prob_G = chromatogram.prob_G.mid(startPos, length);
    chromatogram.prob_T = chromatogram.prob_T.mid(startPos, length);
}

}   // namespace U2
