#include "TmCalculator.h"
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

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/U2AlphabetUtils.h>

#include "TmCalculator.h"

namespace U2 {

const QString TmCalculator::KEY_ID = "id";

TmCalculator::TmCalculator(const QVariantMap& _settings)
    : settings(_settings) {
}

double TmCalculator::getAnnealingTemperature(const QByteArray& product, const QByteArray& forwardPrimer, const QByteArray& reversePrimer) {
    CHECK(isNucleotideSequence(product), INVALID_TM);

    double forwardTm = getMeltingTemperature(forwardPrimer, product.left(forwardPrimer.length()));
    CHECK(forwardTm != INVALID_TM, INVALID_TM);

    double reverseTm = getMeltingTemperature(reversePrimer, DNASequenceUtils::reverseComplement(product.right(reversePrimer.length())));
    CHECK(reverseTm != INVALID_TM, INVALID_TM);

    double primersTm = (forwardTm + reverseTm) / 2;
    double productTm = getMeltingTemperature(product);
    /* Rychlik W, Spencer WJ, Rhoads RE (1990)
       Optimization of the annealing temperature for DNA amplification in vitro.
       Nucleic Acids Res 18(21):6409–6412. */
    return 0.3 * primersTm + 0.7 * productTm - 14.9;
}

bool TmCalculator::isNucleotideSequence(const QByteArray& sequence) {
    auto alphabet = U2AlphabetUtils::findBestAlphabet(sequence.data(), sequence.length());
    SAFE_POINT(alphabet != nullptr, L10N::nullPointerError("DNAAlphabet"), false);

    return alphabet->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
}

double TmCalculator::getMeltingTemperature(const QByteArray& initialPrimer, const QByteArray& alternativePrimer) {
    if (isNucleotideSequence(initialPrimer)) {
        return getMeltingTemperature(initialPrimer);
    }
    if (isNucleotideSequence(alternativePrimer)) {
        return getMeltingTemperature(alternativePrimer);
    }
    FAIL("At least one primer should have nucleotide alphabet", INVALID_TM);
}

const QVariantMap& TmCalculator::getSettings() const {
    return settings;
}

}  // namespace U2
