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

#ifndef _U2_MSA_CONSENSUS_UTILS_H_
#define _U2_MSA_CONSENSUS_UTILS_H_

#include <QByteArray>

#include <U2Core/MultipleAlignment.h>
#include <U2Core/U2Region.h>

namespace U2 {

class MSAConsensusAlgorithm;


class U2ALGORITHM_EXPORT MSAConsensusUtils : public QObject {
    Q_OBJECT
public:

    static void updateConsensus(const MultipleAlignment& msa, QByteArray& cons, MSAConsensusAlgorithm* algo);

    static void updateConsensus(const MultipleAlignment& msa, const U2Region& region, QByteArray& cons, MSAConsensusAlgorithm* algo);

    static void updateConsensus(const MultipleAlignment& msa, const QVector<U2Region>& region, QByteArray& cons, MSAConsensusAlgorithm* algo);

    static QString getConsensusPercentTip(const MultipleAlignment& msa, int pos, int minReportPercent, int maxReportChars);

    // mask4 must contain 4 descending percent values (>= used), example: 100, 75, 50, 33
    // countGaps tells if to include gaps into percent computation
    // result: up to 4 most used chars (from 'A' to 'Z') are packed into return value
    // WARN: the lowest val in mask must be >= 25
    // WARN: works only for alphabet chars (A - Z)
    static quint32 packConsensusCharsToInt(const MultipleAlignment& msa, int pos, const int* mask4, bool gapsAffectPercents);

    // up to 4 chars can be unpacked to charVal values, their percent ranges are saved to mark3Pos
    static void unpackConsensusCharsFromInt(quint32 val, char* charVal, int* maskPos);

    // computes frequency by char. Returns the most frequent char that is not gap.
    // nonGapChars = sum of all non-gap characters
    // TODO: use var-length array instead of the vector!
    static uchar getColumnFreqs(const MultipleAlignment& ma, int pos, QVector<int>& freqsByChar,
                                int &nonGapChars, const QVector<int> &seqIdx = QVector<int>());

};

}//namespace

#endif
