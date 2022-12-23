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
#include "primer3_core/oligotm.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString Primer3TempCalcSettings::KEY_DNA_CONC = "dna_conc";
const QString Primer3TempCalcSettings::KEY_SALT_CONC = "salt_conc";
const QString Primer3TempCalcSettings::KEY_DIVALENT_CONC = "divalent_conc";
const QString Primer3TempCalcSettings::KEY_DNTP_CONC = "dntp_conc";
const QString Primer3TempCalcSettings::KEY_DMSO_CONC = "dmso_conc";
const QString Primer3TempCalcSettings::KEY_DMSO_FACT = "dmso_fact";
const QString Primer3TempCalcSettings::KEY_FPRMAMIDE_CONC = "fprmamide_conc";
const QString Primer3TempCalcSettings::KEY_MAX_LEN = "max_len";
const QString Primer3TempCalcSettings::KEY_TM_METHOD = "tm_method";
const QString Primer3TempCalcSettings::KEY_SALT_CORRECTION = "salt_correction";

QMap<QString, QVariant> Primer3TempCalcSettings::toVariantMap() const {
    auto result = TempCalcSettings::toVariantMap();
    result.insert(KEY_DNA_CONC, QVariant(dnaConc));
    result.insert(KEY_SALT_CONC, QVariant(saltConc));
    result.insert(KEY_DIVALENT_CONC, QVariant(divalentConc));
    result.insert(KEY_DNTP_CONC, QVariant(dntpConc));
    result.insert(KEY_DMSO_CONC, QVariant(dmsoConc));
    result.insert(KEY_DMSO_FACT, QVariant(dmsoFact));
    result.insert(KEY_FPRMAMIDE_CONC, QVariant(formamideConc));
    result.insert(KEY_MAX_LEN, QVariant(nnMaxLen));
    result.insert(KEY_TM_METHOD, QVariant(static_cast<int>(tmMethod)));
    result.insert(KEY_SALT_CORRECTION, QVariant(static_cast<int>(saltCorrections)));

    return result;
}

void Primer3TempCalcSettings::fromVariantMap(const QMap<QString, QVariant>& mapSettings) {
    TempCalcSettings::fromVariantMap(mapSettings);
    dnaConc = mapSettings.value(KEY_DNA_CONC).toDouble();
    saltConc = mapSettings.value(KEY_SALT_CONC).toDouble();
    divalentConc = mapSettings.value(KEY_DIVALENT_CONC).toDouble();
    dntpConc = mapSettings.value(KEY_DNTP_CONC).toDouble();
    dmsoConc = mapSettings.value(KEY_DMSO_CONC).toDouble();
    dmsoFact = mapSettings.value(KEY_DMSO_FACT).toDouble();
    formamideConc = mapSettings.value(KEY_FPRMAMIDE_CONC).toDouble();
    nnMaxLen = mapSettings.value(KEY_MAX_LEN).toInt();
    tmMethod = static_cast<TmMethodType>(mapSettings.value(KEY_TM_METHOD).toInt());
    saltCorrections = static_cast<SaltCorrectionType>(mapSettings.value(KEY_SALT_CORRECTION).toInt());
}

Primer3TempCalc::Primer3TempCalc(Primer3TempCalcSettings* settings)
    : BaseTempCalc(settings) {}

double Primer3TempCalc::getMeltingTemperature(const QByteArray& sequence) {
    auto p3Settings = static_cast<Primer3TempCalcSettings*>(settings);
    SAFE_POINT(p3Settings != nullptr, L10N::nullPointerError("Primer3TempCalcSettings"), INVALID_TM);    
    
    auto result = seqtm(sequence,
                        p3Settings->dnaConc,
                        p3Settings->saltConc,
                        p3Settings->divalentConc,
                        p3Settings->dntpConc,
                        p3Settings->dmsoConc,
                        p3Settings->dmsoFact,
                        p3Settings->formamideConc,
                        p3Settings->nnMaxLen,
                        static_cast<tm_method_type>(p3Settings->tmMethod),
                        static_cast<salt_correction_type>(p3Settings->saltCorrections),
                        INVALID_TM).Tm;
    if (result == OLIGOTM_ERROR) {
        result = INVALID_TM;
    }

    return result;
}

}
