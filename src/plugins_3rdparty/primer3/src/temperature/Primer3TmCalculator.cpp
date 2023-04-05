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

#include "Primer3TmCalculator.h"

#include <U2Core/U2SafePoints.h>

#include <U2View/DNAStatisticsTask.h>

#include "primer3_core/oligotm.h"

namespace U2 {

const QString Primer3TmCalculator::KEY_DNA_CONC = "dna_conc";
const QString Primer3TmCalculator::KEY_SALT_CONC = "salt_conc";
const QString Primer3TmCalculator::KEY_DIVALENT_CONC = "divalent_conc";
const QString Primer3TmCalculator::KEY_DNTP_CONC = "dntp_conc";
const QString Primer3TmCalculator::KEY_DMSO_CONC = "dmso_conc";
const QString Primer3TmCalculator::KEY_DMSO_FACT = "dmso_fact";
const QString Primer3TmCalculator::KEY_FORMAMIDE_CONC = "formamide_conc";
const QString Primer3TmCalculator::KEY_MAX_LEN = "max_len";
const QString Primer3TmCalculator::KEY_TM_METHOD = "tm_method";
const QString Primer3TmCalculator::KEY_SALT_CORRECTION = "salt_correction";

Primer3TmCalculator::Primer3TmCalculator(const QVariantMap& settings)
    : TmCalculator(settings) {
}

double Primer3TmCalculator::getMeltingTemperature(const QByteArray& sequence) {
    if (sequence.length() < DNAStatisticsTask::TM_MIN_LENGTH_LIMIT || sequence.length() > DNAStatisticsTask::TM_MAX_LENGTH_LIMIT) {
        return INVALID_TM;
    }
    auto result = seqtm(sequence,
                        settings.value(KEY_DNA_CONC, DNA_CONC_DEFAULT).toDouble(),
                        settings.value(KEY_SALT_CONC, SALT_CONC_DEFAULT).toDouble(),
                        settings.value(KEY_DIVALENT_CONC, DIVALENT_CONC_DEFAULT).toDouble(),
                        settings.value(KEY_DNTP_CONC, DNTP_CONC_DEFAULT).toDouble(),
                        settings.value(KEY_DMSO_CONC, DMSO_CONC_DEFAULT).toDouble(),
                        settings.value(KEY_DMSO_FACT, DMSO_FACT_DEFAULT).toDouble(),
                        settings.value(KEY_FORMAMIDE_CONC, FORMAMIDE_CONC_DEFAULT).toDouble(),
                        settings.value(KEY_MAX_LEN, NN_MAX_LEN_DEFAULT).toInt(),
                        static_cast<tm_method_type>(settings.value(KEY_TM_METHOD, TM_METHOD_DEFAULT).toInt()),
                        static_cast<salt_correction_type>(settings.value(KEY_SALT_CORRECTION, SALT_CORRECTIONS_DEFAULT).toInt()),
                        INVALID_TM)
                      .Tm;
    if (result == OLIGOTM_ERROR) {
        result = INVALID_TM;
    }

    return result;
}

}  // namespace U2
