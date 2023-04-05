/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "RoughTmCalculator.h"

namespace U2 {

RoughTmCalculator::RoughTmCalculator(const QVariantMap& settings)
    : TmCalculator(settings) {
}

double RoughTmCalculator::getMeltingTemperature(const QByteArray& sequence) {
    CHECK(!sequence.isEmpty(), INVALID_TM)

    int nA = 0;
    int nC = 0;
    int nG = 0;
    int nT = 0;
    for (const auto& base : qAsConst(sequence)) {
        switch (base) {
            case 'A':
                nA++;
                break;
            case 'C':
                nC++;
                break;
            case 'G':
                nG++;
                break;
            case 'T':
                nT++;
                break;
            case 'N':
                break;
            default:
                return INVALID_TM;
        }
    }

    double result;
    // for the formulas details see here: https://openwetware.org/wiki/Primer_Tm_estimation_methods
    if (sequence.length() < 14) {
        result = (nA + nT) * 2 + (nG + nC) * 4;
    } else {
        result = 64.9 + 41 * (nG + nC - 16.4) / double(nA + nT + nG + nC);
    }

    return result;
}

}  // namespace U2
