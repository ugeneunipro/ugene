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

#include "Primer3TempCalc.h"
#include "core/oligotm.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {



Primer3MeltTempCalc::Primer3MeltTempCalc(Primer3MeltTempCalcSettings* settings) 
    : BaseTempCalc (settings) {}

double Primer3MeltTempCalc::getMeltingTemperature(const QByteArray& sequence) {
    auto p3Settings = static_cast<Primer3MeltTempCalcSettings*>(settings);
    SAFE_POINT(p3Settings != nullptr, L10N::nullPointerError("Primer3MeltTempCalcSettings"), INVALID_TM);

    return seqtm(p3Settings->sequence, 
                 p3Settings->dnaConc,
                 p3Settings->saltConc,
                 p3Settings->divalentConc,
                 p3Settings->dnaConc,
                 p3Settings->dmsoConc,
                 p3Settings->dmsoFact,
                 p3Settings->formamideConc,
                 p3Settings->nnMaxLen,
                 static_cast<tm_method_type>(p3Settings->tmMethod),
                 static_cast<salt_correction_type>(p3Settings->saltCorrections),
                 INVALID_TM).Tm;
}

}
