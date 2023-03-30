/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
#include <QVector>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

using MononucleotidesExtinctionCoefficientsMap = QVector<int>;
using DinucleotidesExtinctionCoefficientsMap = QVector<QVector<int>>;

class DNAAlphabet;
class TmCalculator;
class U2SequenceDbi;

struct U2VIEW_EXPORT DNAStatistics {
    DNAStatistics();

    qint64 length;
    double gcContent;
    double meltingTemp;

    double ssMolecularWeight;  // both for nucleotide and amino-acid sequences
    qint64 ssExtinctionCoefficient;
    double ssOd260AmountOfSubstance;  // in nanomoles
    double ssOd260Mass;  // in micrograms

    double dsMolecularWeight;  // only for nucleotide sequences
    qint64 dsExtinctionCoefficient;
    double dsOd260AmountOfSubstance;  // in nanomoles
    double dsOd260Mass;  // in micrograms

    double isoelectricPoint;  // only for amino-acid sequences

    void clear();
};

class U2VIEW_EXPORT DNAStatisticsTask : public BackgroundTask<DNAStatistics> {
    Q_OBJECT
public:
    DNAStatisticsTask(const DNAAlphabet* alphabet, const U2EntityRef& seqRef, const QVector<U2Region>& regions, const QSharedPointer<TmCalculator>& temperatureCalculator);

    /** Minimum sequence length to estimate melting temperature. */
    static constexpr int TM_MIN_LENGTH_LIMIT = 8;

    /** Maximum sequence length to estimate melting temperature. */
    static constexpr int TM_MAX_LENGTH_LIMIT = 10 * 1000;

private:
    void run() override;

    const DNAAlphabet* alphabet;
    U2EntityRef seqRef;
    QVector<U2Region> regions;
    QSharedPointer<TmCalculator> temperatureCalculator;

    QVector<qint64> charactersCount;
    QVector<qint64> rcCharactersCount;
    QVector<QVector<qint64>> dinucleotidesCount;
    QVector<QVector<qint64>> rcDinucleotidesCount;

    static const QVector<double> DNA_MOLECULAR_WEIGHT_MAP;  // DNA nucleotides molecular weights
    static const QVector<double> RNA_MOLECULAR_WEIGHT_MAP;  // RNA nucleotides molecular weights

    static const MononucleotidesExtinctionCoefficientsMap DNA_MONONUCLEOTIDES_EXTINCTION_COEFFICIENTS;
    static const DinucleotidesExtinctionCoefficientsMap DNA_DINUCLEOTIDES_EXTINCTION_COEFFICIENTS;
    static const MononucleotidesExtinctionCoefficientsMap RNA_MONONUCLEOTIDES_EXTINCTION_COEFFICIENTS;
    static const DinucleotidesExtinctionCoefficientsMap RNA_DINUCLEOTIDES_EXTINCTION_COEFFICIENTS;

    static const QVector<double> PROTEIN_MOLECULAR_WEIGHT_MAP;  // protein molecular weight
    static const QVector<double> pKaMap;  // pKa values
    static const QVector<int> PROTEIN_CHARGES_MAP;  // protein charges
    static const QVector<double> GC_RATIO_MAP;  // how much contribution the character makes to the GC content

    void computeStats();
    double calcPi(U2SequenceDbi* sequenceDbi);
    double calcChargeState(const QVector<qint64>& countMap, double pH);
};

}  // namespace U2
