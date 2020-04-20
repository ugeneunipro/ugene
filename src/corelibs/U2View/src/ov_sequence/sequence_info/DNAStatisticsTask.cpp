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

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2DbiUtils.h>

#include <math.h>

#include "DNAStatisticsTask.h"

namespace U2 {

#define MAP_SIZE 256

DNAStatistics::DNAStatistics() {
    clear();
}

void DNAStatistics::clear() {
    length = 0;
    gcContent = 0;
    meltingTemp = 0;

    ssMolecularWeight = 0;
    ssExtinctionCoefficient = 0;
    ssOd260AmountOfSubstance = 0;
    ssOd260Mass = 0;

    dsMolecularWeight = 0;
    dsExtinctionCoefficient = 0;
    dsOd260AmountOfSubstance = 0;
    dsOd260Mass = 0;

    isoelectricPoint = 0;
}

static QVector<double> createDnaMolecularWeightMap() {
    QVector<double> res(MAP_SIZE, 0);

    res['A'] = 251.24;
    res['C'] = 227.22;
    res['G'] = 267.24;
    res['T'] = 242.23;

    // Characters from the extended alphabet are calculated as avarage value
    res['W'] = (res['A'] + res['T']) / 2;
    res['S'] = (res['C'] + res['G']) / 2;
    res['M'] = (res['A'] + res['C']) / 2;
    res['K'] = (res['G'] + res['T']) / 2;
    res['R'] = (res['A'] + res['G']) / 2;
    res['Y'] = (res['C'] + res['T']) / 2;

    res['B'] = (res['C'] + res['G'] + res['T']) / 3;
    res['D'] = (res['A'] + res['G'] + res['T']) / 3;
    res['H'] = (res['A'] + res['C'] + res['T']) / 3;
    res['V'] = (res['A'] + res['C'] + res['G']) / 3;

    res['N'] = (res['A'] + res['C'] + res['G'] + res['T']) / 4;

    return res;
}

static QVector<double> createRnaMolecularWeightMap() {
    QVector<double> res(MAP_SIZE, 0);

    res['A'] = 267.24;
    res['C'] = 243.22;
    res['G'] = 283.24;
    res['U'] = 244.20;

    // Characters from the extended alphabet are calculated as avarage value
    res['W'] = (res['A'] + res['U']) / 2;
    res['S'] = (res['C'] + res['G']) / 2;
    res['M'] = (res['A'] + res['C']) / 2;
    res['K'] = (res['G'] + res['U']) / 2;
    res['R'] = (res['A'] + res['G']) / 2;
    res['Y'] = (res['C'] + res['U']) / 2;

    res['B'] = (res['C'] + res['G'] + res['U']) / 3;
    res['D'] = (res['A'] + res['G'] + res['U']) / 3;
    res['H'] = (res['A'] + res['C'] + res['U']) / 3;
    res['V'] = (res['A'] + res['C'] + res['G']) / 3;

    res['N'] = (res['A'] + res['C'] + res['G'] + res['U']) / 4;

    return res;
}

static QMap<char, QByteArray> createDnaAlphabetResolutionMap() {
    QMap<char, QByteArray> res;
    res['A'] = "A";
    res['C'] = "C";
    res['G'] = "G";
    res['T'] = "T";
    res['W'] = "AT";
    res['S'] = "CG";
    res['M'] = "AC";
    res['K'] = "GT";
    res['R'] = "AG";
    res['Y'] = "CT";
    res['B'] = "CGT";
    res['D'] = "AGT";
    res['H'] = "ACT";
    res['V'] = "ACG";
    res['N'] = "ACGT";
    return res;
}

static QMap<char, QByteArray> createRnaAlphabetResolutionMap() {
    QMap<char, QByteArray> res;
    res['A'] = "A";
    res['C'] = "C";
    res['G'] = "G";
    res['U'] = "U";
    res['W'] = "AU";
    res['S'] = "CG";
    res['M'] = "AC";
    res['K'] = "GU";
    res['R'] = "AG";
    res['Y'] = "CU";
    res['B'] = "CGU";
    res['D'] = "AGU";
    res['H'] = "ACU";
    res['V'] = "ACG";
    res['N'] = "ACGU";
    return res;
}

static void fillMapWithAvarageValues(QVector<QVector<int>> &map, const QMap<char, QByteArray> &alphabetResolutionMap) {
    foreach (const char i, alphabetResolutionMap.keys()) {
        foreach (const char j, alphabetResolutionMap.keys()) {
            if (0 == map[i][j]) {
                // Unambiguous nucleotide pairs are already registered
                // If at least one nucleotide in pair is ambiguous, then the pair value should be an avarage value of all possible variants.
                int value = 0;
                for (int k = 0; k < alphabetResolutionMap[i].length(); ++k) {
                    for (int m = 0; m < alphabetResolutionMap[j].length(); ++m) {
                        char char1 = alphabetResolutionMap[i][k];
                        char char2 = alphabetResolutionMap[j][m];
                        value += map[char1][char2];
                    }
                }
                const int count = alphabetResolutionMap[i].length() * alphabetResolutionMap[j].length();
                value /= count;
                map[i][j] = value;
            }
        }
    }
}

static MononucleotidesExtinctionCoefficientsMap createDnaMononucleotidesExtinctionCoefficients() {
    MononucleotidesExtinctionCoefficientsMap res(MAP_SIZE, 0);
    res['A'] = 15400;
    res['C'] = 7400;
    res['G'] = 11500;
    res['T'] = 8700;
    return res;
}

static DinucleotidesExtinctionCoefficientsMap createDnaDinucleotidesExtinctionCoefficients() {
    DinucleotidesExtinctionCoefficientsMap res(MAP_SIZE, QVector<int>(MAP_SIZE, 0));

    res['A']['A'] = 27400;
    res['A']['C'] = 21200;
    res['A']['G'] = 25000;
    res['A']['T'] = 22800;

    res['C']['A'] = 21200;
    res['C']['C'] = 14600;
    res['C']['G'] = 18000;
    res['C']['T'] = 15200;

    res['G']['A'] = 25200;
    res['G']['C'] = 17600;
    res['G']['G'] = 21600;
    res['G']['T'] = 20000;

    res['T']['A'] = 23400;
    res['T']['C'] = 16200;
    res['T']['G'] = 19000;
    res['T']['T'] = 16800;

    fillMapWithAvarageValues(res, createDnaAlphabetResolutionMap());

    return res;
}

static MononucleotidesExtinctionCoefficientsMap createRnaMononucleotidesExtinctionCoefficients() {
    MononucleotidesExtinctionCoefficientsMap res(MAP_SIZE, 0);
    res['A'] = 15400;
    res['C'] = 7200;
    res['G'] = 11500;
    res['U'] = 9900;
    return res;
}

static DinucleotidesExtinctionCoefficientsMap createRnaDinucleotidesExtinctionCoefficients() {
    DinucleotidesExtinctionCoefficientsMap res(MAP_SIZE, QVector<int>(MAP_SIZE, 0));

    res['A']['A'] = 27400;
    res['A']['C'] = 21000;
    res['A']['G'] = 25000;
    res['A']['U'] = 24000;

    res['C']['A'] = 21000;
    res['C']['C'] = 14200;
    res['C']['G'] = 17800;
    res['C']['U'] = 16200;

    res['G']['A'] = 25200;
    res['G']['C'] = 17400;
    res['G']['G'] = 21600;
    res['G']['U'] = 21200;

    res['U']['A'] = 24600;
    res['U']['C'] = 17200;
    res['U']['G'] = 20000;
    res['U']['U'] = 19600;

    fillMapWithAvarageValues(res, createRnaAlphabetResolutionMap());

    return res;
}

static QVector<double> createProteinMWMap(){
    QVector<double> mwMap(MAP_SIZE, 0);
    mwMap['A'] = 89.09; // ALA
    mwMap['R'] = 174.20; // ARG
    mwMap['N'] = 132.12; // ASN
    mwMap['D'] = 133.10; // ASP
    mwMap['B'] = 132.61; // ASX
    mwMap['C'] = 121.15; // CYS
    mwMap['Q'] = 146.15; // GLN
    mwMap['E'] = 147.13; // GLU
    mwMap['Z'] = 146.64; // GLX
    mwMap['G'] = 75.07; // GLY
    mwMap['H'] = 155.16; // HIS
    mwMap['I'] = 131.17; // ILE
    mwMap['L'] = 131.17; // LEU
    mwMap['K'] = 146.19; // LYS
    mwMap['M'] = 149.21; // MET
    mwMap['F'] = 165.19; // PHE
    mwMap['P'] = 115.13; //PRO
    mwMap['S'] = 105.09; // SER
    mwMap['T'] = 119.12; // THR
    mwMap['W'] = 204.23; // TRP
    mwMap['Y'] = 181.19; // TYR
    mwMap['V'] = 117.15; // VAL
    return mwMap;
}

static QVector<double> createPKAMap() {
    QVector<double> res(MAP_SIZE, 0);
    res['D'] = 4.0;
    res['C'] = 8.5;
    res['E'] = 4.4;
    res['Y'] = 10.0;
    res['c'] = 3.1; // CTERM
    res['R'] = 12.0;
    res['H'] = 6.5;
    res['K'] = 10.4;
    res['n'] = 8.0; // NTERM
    return res;
}

static QVector<int> createChargeMap() {
    QVector<int> res(MAP_SIZE, 0);
    res['D'] = -1;
    res['C'] = -1;
    res['E'] = -1;
    res['Y'] = -1;
    res['c'] = -1; // CTERM
    res['R'] = 1;
    res['H'] = 1;
    res['K'] = 1;
    res['n'] = 1; // NTERM
    return res;
}

static QVector<double> createGcRatioMap() {
    QVector<double> res(MAP_SIZE, 0);
    res['B'] = 2.0 / 3;
    res['C'] = 1;
    res['D'] = 1.0 / 3;
    res['G'] = 1;
    res['H'] = 1.0 / 3;
    res['K'] = 0.5;
    res['M'] = 0.5;
    res['N'] = 0.5;
    res['R'] = 0.5;
    res['S'] = 1;
    res['V'] = 2.0 / 3;
    res['X'] = 0.5;
    res['Y'] = 0.5;
    return res;
}

const QVector<double> DNAStatisticsTask::DNA_MOLECULAR_WEIGHT_MAP = createDnaMolecularWeightMap();
const QVector<double> DNAStatisticsTask::RNA_MOLECULAR_WEIGHT_MAP = createRnaMolecularWeightMap();

const QVector<int> DNAStatisticsTask::DNA_MONONUCLEOTIDES_EXTINCTION_COEFFICIENTS = createDnaMononucleotidesExtinctionCoefficients();
const QVector<QVector<int>> DNAStatisticsTask::DNA_DINUCLEOTIDES_EXTINCTION_COEFFICIENTS = createDnaDinucleotidesExtinctionCoefficients();
const QVector<int> DNAStatisticsTask::RNA_MONONUCLEOTIDES_EXTINCTION_COEFFICIENTS = createRnaMononucleotidesExtinctionCoefficients();
const QVector<QVector<int>> DNAStatisticsTask::RNA_DINUCLEOTIDES_EXTINCTION_COEFFICIENTS = createRnaDinucleotidesExtinctionCoefficients();

const QVector<double> DNAStatisticsTask::PROTEIN_MOLECULAR_WEIGHT_MAP = createProteinMWMap();
const QVector<double> DNAStatisticsTask::pKaMap = createPKAMap();
const QVector<int> DNAStatisticsTask::PROTEIN_CHARGES_MAP = createChargeMap();
const QVector<double> DNAStatisticsTask::GC_RATIO_MAP = createGcRatioMap();

DNAStatisticsTask::DNAStatisticsTask(const DNAAlphabet* alphabet,
                                     const U2EntityRef seqRef,
                                     const QVector<U2Region>& regions)
    : BackgroundTask< DNAStatistics > (tr("Calculate sequence statistics"), TaskFlag_None),
      alphabet(alphabet),
      seqRef(seqRef),
      regions(regions),
      charactersCount(MAP_SIZE, 0),
      rcCharactersCount(MAP_SIZE, 0),
      dinucleotidesCount(MAP_SIZE, QVector<qint64>(MAP_SIZE, 0)),
      rcDinucleotidesCount(MAP_SIZE, QVector<qint64>(MAP_SIZE, 0))
{
    SAFE_POINT_EXT(alphabet != NULL, setError(tr("Alphabet is NULL")), );
}

void DNAStatisticsTask::run() {
    computeStats();
}

void DNAStatisticsTask::computeStats() {
    result.clear();

    U2OpStatus2Log os;
    DbiConnection dbiConnection(seqRef.dbiRef, os);
    CHECK_OP(os, );

    U2SequenceDbi* sequenceDbi = dbiConnection.dbi->getSequenceDbi();
    CHECK(sequenceDbi != NULL, );
    SAFE_POINT_EXT(alphabet != NULL, setError(tr("Alphabet is NULL")), );
    qint64 totalLength = U2Region::sumLength(regions);
    qint64 processedLength = 0;

    result.length = totalLength;
    if (alphabet->isRaw()) {
        // Other stats can't be computed for raw alphabet
        return;
    }

    foreach (const U2Region& region, regions) {
        QList<U2Region> blocks = U2Region::split(region, 1024 * 1024);
        foreach(const U2Region& block, blocks) {
            if (isCanceled() || hasError()) {
                break;
            }
            const QByteArray seqBlock = sequenceDbi->getSequenceData(seqRef.entityId, block, os);
            CHECK_OP(os,);
            const char *sequenceData = seqBlock.constData();

            int previousChar = U2Msa::GAP_CHAR;
            for (int i = 0, n = seqBlock.size(); i < n; i++) {
                const int character = static_cast<int>(sequenceData[i]);
                charactersCount[character]++;

                if (alphabet->isNucleic()) {
                    if (previousChar != U2Msa::GAP_CHAR && character != U2Msa::GAP_CHAR) {
                        dinucleotidesCount[previousChar][character]++;
                    }
                }
                if (U2Msa::GAP_CHAR != character) {
                    previousChar = character;
                }
            }

            if (alphabet->isNucleic()) {
                const QByteArray rcSeqBlock = DNASequenceUtils::reverseComplement(seqBlock, alphabet);
                const char *rcSequenceData = rcSeqBlock.constData();

                int previousRcChar = U2Msa::GAP_CHAR;
                for (int i = 0, n = rcSeqBlock.size(); i < n; i++) {
                    const int rcCharacter = static_cast<int>(rcSequenceData[i]);
                    rcCharactersCount[rcCharacter]++;
                    if (previousRcChar != U2Msa::GAP_CHAR && rcCharacter != U2Msa::GAP_CHAR) {
                        rcDinucleotidesCount[rcCharacter][previousRcChar]++;      // dinucleotides on the complement strand are calculated in 5'->3' direction
                    }
                    if (U2Msa::GAP_CHAR != rcCharacter) {
                        previousRcChar = rcCharacter;
                    }
                }
            }

            processedLength += block.length;
            stateInfo.setProgress(static_cast<int>(processedLength * 100 / totalLength));
            CHECK_OP(stateInfo,);
        }
    }

    const qint64 ungappedLength = totalLength - charactersCount.value(U2Msa::GAP_CHAR, 0);

    if (alphabet->isNucleic()) {
        //  gcContent = ((nG + nC + nS + 0.5*nM + 0.5*nK + 0.5*nR + 0.5*nY + (2/3)*nB + (1/3)*nD + (1/3)*nH + (2/3)*nV + 0.5*nN) / n ) * 100%
        for (int i = 0, n = charactersCount.size(); i < n; ++i) {
            result.gcContent += charactersCount[i] * GC_RATIO_MAP[i];
        }
        result.gcContent = 100.0 * result.gcContent / ungappedLength;

        // Calculating molecular weight
        // Source: http://www.basic.northwestern.edu/biotools/oligocalc.html
        const QVector<double> *molecularWeightMap = nullptr;
        if (alphabet->isRNA()) {
            molecularWeightMap = &RNA_MOLECULAR_WEIGHT_MAP;
        } else if (alphabet->isDNA()) {
            molecularWeightMap = &DNA_MOLECULAR_WEIGHT_MAP;
        }
        SAFE_POINT_EXT(nullptr != molecularWeightMap, os.setError("An unknown alphabet"), );

        for (int i = 0, n = charactersCount.size(); i < n; ++i) {
            result.ssMolecularWeight += charactersCount[i] * molecularWeightMap->at(i);
            result.dsMolecularWeight += charactersCount[i] * molecularWeightMap->at(i) + rcCharactersCount[i] * molecularWeightMap->at(i);
        }

        static const double PHOSPHATE_WEIGHT = 61.97;
        result.ssMolecularWeight += (ungappedLength - 1) * PHOSPHATE_WEIGHT;
        result.dsMolecularWeight += (ungappedLength - 1) * PHOSPHATE_WEIGHT * 2;

        // Calculating extinction coefficient
        // Source: http://www.owczarzy.net/extinctionDNA.htm
        const MononucleotidesExtinctionCoefficientsMap *mononucleotideExtinctionCoefficientsMap = nullptr;
        const DinucleotidesExtinctionCoefficientsMap *dinucleotideExtinctionCoefficientsMap = nullptr;
        if (alphabet->isRNA()) {
            mononucleotideExtinctionCoefficientsMap = &RNA_MONONUCLEOTIDES_EXTINCTION_COEFFICIENTS;
            dinucleotideExtinctionCoefficientsMap = &RNA_DINUCLEOTIDES_EXTINCTION_COEFFICIENTS;
        } else if (alphabet->isDNA()) {
            mononucleotideExtinctionCoefficientsMap = &DNA_MONONUCLEOTIDES_EXTINCTION_COEFFICIENTS;
            dinucleotideExtinctionCoefficientsMap = &DNA_DINUCLEOTIDES_EXTINCTION_COEFFICIENTS;
        }
        SAFE_POINT_EXT(nullptr != mononucleotideExtinctionCoefficientsMap, os.setError("An unknown alphabet"), );
        SAFE_POINT_EXT(nullptr != dinucleotideExtinctionCoefficientsMap, os.setError("An unknown alphabet"), );

        for (int i = 0, n = dinucleotidesCount.size(); i < n; ++i) {
            for (int j = 0, m = dinucleotidesCount[i].size(); j < m; ++j) {
                result.ssExtinctionCoefficient += dinucleotidesCount[i][j] * dinucleotideExtinctionCoefficientsMap->at(i).at(j);
                result.dsExtinctionCoefficient += dinucleotidesCount[i][j] * dinucleotideExtinctionCoefficientsMap->at(i).at(j) +
                                                  rcDinucleotidesCount[i][j] * dinucleotideExtinctionCoefficientsMap->at(i).at(j);
            }
        }

        for (int i = 0; i < charactersCount.count(); ++i) {
            result.ssExtinctionCoefficient -= charactersCount[i] * mononucleotideExtinctionCoefficientsMap->at(i);
            result.dsExtinctionCoefficient -= charactersCount[i] * mononucleotideExtinctionCoefficientsMap->at(i) +
                                              rcCharactersCount[i] * mononucleotideExtinctionCoefficientsMap->at(i);
        }

        // h = 0.287 * SEQ_AT-content + 0.059 * SEQ_GC-content
        const double hypochromicity = 0.287 * (1 - result.gcContent / 100) + 0.059 * (result.gcContent / 100);

        result.dsExtinctionCoefficient *= (1 - hypochromicity);

        // Calculating melting temperature
        const qint64 nA = charactersCount['A'];
        const qint64 nC = charactersCount['C'];
        const qint64 nG = charactersCount['G'];
        const qint64 nT = charactersCount['T'];
        if (totalLength < 15) {
            result.meltingTemp = (nA + nT) * 2 + (nG + nC) * 4;
        } else if (nA + nT + nG + nC != 0) {
            result.meltingTemp = 64.9 + 41 * (nG + nC - 16.4) / static_cast<double>(nA + nT + nG + nC);
        }

        // Calculating nmole/OD260
        if (result.ssExtinctionCoefficient != 0) {
            result.ssOd260AmountOfSubstance = 1000000.0 / result.ssExtinctionCoefficient;
        }

        if (result.dsExtinctionCoefficient != 0) {
            result.dsOd260AmountOfSubstance = 1000000.0 / result.dsExtinctionCoefficient;
        }

        // Calculating μg/OD260
        result.ssOd260Mass = result.ssOd260AmountOfSubstance * result.ssMolecularWeight * 0.001;
        result.dsOd260Mass = result.dsOd260AmountOfSubstance * result.dsMolecularWeight * 0.001;
    } else if (alphabet->isAmino()) {
        // Calculating molecular weight
        for (int i = 0, n = charactersCount.size(); i < n; ++i) {
            result.ssMolecularWeight += charactersCount[i] * PROTEIN_MOLECULAR_WEIGHT_MAP.value(i);
        }
        static const double MWH2O = 18.0;
        result.ssMolecularWeight -= (totalLength - 1) * MWH2O;

        // Calculating isoelectric point
        result.isoelectricPoint = calcPi(sequenceDbi);
    }
}

double DNAStatisticsTask::calcPi(U2SequenceDbi* sequenceDbi) {
    U2OpStatus2Log os;
    QVector<qint64> countMap(256, 0);
    foreach (const U2Region& region, regions) {
        QList<U2Region> blocks = U2Region::split(region, 1024 * 1024);
        foreach(const U2Region& block, blocks) {
            if (isCanceled() || hasError()) {
                break;
            }
            QByteArray seqBlock = sequenceDbi->getSequenceData(seqRef.entityId, block, os);
            CHECK_OP(os, 0);
            const char* sequenceData = seqBlock.constData();
            for (int i = 0, n = seqBlock.size(); i < n; i++) {
                char c = sequenceData[i];
                if (pKaMap[c] != 0) {
                    countMap[c]++;
                }
            }
            CHECK_OP(stateInfo, 0);
        }
    }

    countMap['c'] = 1;
    countMap['n'] = 1;

    static const double CUTOFF = 0.001;
    static const double INITIAL_CUTOFF = 2.0;

    double step = INITIAL_CUTOFF;
    double pH = 0;
    while (step > CUTOFF) {
        if (calcChargeState(countMap, pH) > 0) {
            pH += step;
        } else {
            step *= 0.5;
            pH -= step;
        }
    }
    return pH;
}

double DNAStatisticsTask::calcChargeState(const QVector<qint64>& countMap, double pH ) {
    double chargeState = 0.;
    for (int i = 0; i < countMap.length(); i++) {
        if (isCanceled() || hasError()) {
            break;
        }
        double pKa = pKaMap[i];
        double charge = PROTEIN_CHARGES_MAP[i];
        chargeState += countMap[i] * charge / (1 + pow(10.0, charge * (pH - pKa)));
    }
    return chargeState;
}


} // namespace
