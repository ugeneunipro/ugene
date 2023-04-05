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
#pragma once

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVariant>

#include <U2Core/global.h>

namespace U2 {

/** Melting Temperature calculator: Algorithm + Settings. */
class U2ALGORITHM_EXPORT TmCalculator {
public:
    TmCalculator(const QVariantMap& settings);
    virtual ~TmCalculator() = default;

    /**
     * Calculate melting temperature
     * @sequence sequence, which melting temperature should be calculated
     * @return the calculated temperature value
     */
    virtual double getMeltingTemperature(const QByteArray& sequence) = 0;
    /**
     * Calculate annealing temperature of the product
     * Use formula from "Rychlik W, Spencer WJ, Rhoads RE (1990)
     * Optimization of the annealing temperature for DNA amplification in vitro.
     * Nucleic Acids Res 18(21):6409–6412."
     * @product the whole product (has forward and reverse primers on 5' ends of direct and reverse-complementary stand)
     * @forwardPrimer forward primer, located on 3' end of the direct product stand
     * @reversePrimer reverse primer, located on 3' end of the reverse-complementary product stand
     * @return the annealing temperature value
     */
    double getAnnealingTemperature(const QByteArray& product, const QByteArray& forwardPrimer, const QByteArray& reversePrimer);
    const QVariantMap& getSettings() const;

    static constexpr double INVALID_TM = -999999.0;
    /**
     * The algorithm ID.
     * Also used in GUI (e.g. in combo boxes where all algorithms are presented - you see algorithm's ID)
     */
    static const QString KEY_ID;

protected:
    QVariantMap settings;

private:
    static bool isNucleotideSequence(const QByteArray& sequence);
    /**
     * Calculate melting temperature of @initialPrimer (if nucleotide)
     * or @alternativePrimer (if @initialPrimer has symbols from the DNA extended alphabet)
     * @initialPrimer calculate melting temperature of this primer (if nucleotide)
     * @alternativePrimer calculate melting temperature of this primer (if @initialPrimer is not nucleotide)
     * @return the calculated temperature value
     */
    double getMeltingTemperature(const QByteArray& initialPrimer, const QByteArray& alternativePrimer);
};

}  // namespace U2
