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

#ifndef _U2_ASSEMBLY_CONSENSUS_UTILS_H_
#define _U2_ASSEMBLY_CONSENSUS_UTILS_H_

#include <QVector>

#include <U2Core/U2Assembly.h>
#include <U2Core/U2Type.h>

namespace U2 {

/** Raw bases frequency info for consensus */
class U2AssemblyBasesFrequenciesInfo {
public:
    U2AssemblyBasesFrequenciesInfo();

    static const int LETTERS_COUNT = 4;
    qint64 baseFrequencies[LETTERS_COUNT];

    void addToCharFrequency(char c);
    qint64 getCharFrequency(char c);
    char getMostFrequentLetter();
};

class AssemblyBasesFrequenciesStat {
public:
    QVector<U2AssemblyBasesFrequenciesInfo> frequencyInfos;
    QByteArray getConsensusFragment();
};

} // namespace U2

#endif
