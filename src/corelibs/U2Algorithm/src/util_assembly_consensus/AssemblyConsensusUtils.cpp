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

#include "AssemblyConsensusUtils.h"
#include "AssemblyConsensusAlgorithm.h"

namespace U2 {

namespace {
    inline char index2char(int index) {
        return "ACGT"[index];
    }

    inline int char2index(char c) {
        switch(toupper(c)) {
        case 'A' : return 0;
        case 'C' : return 1;
        case 'G' : return 2;
        case 'T' : return 3;
        default  : return -1;
        }
    }
}

U2AssemblyBasesFrequenciesInfo::U2AssemblyBasesFrequenciesInfo()
{
    memset(baseFrequencies, 0, sizeof(baseFrequencies));
}

void U2AssemblyBasesFrequenciesInfo::addToCharFrequency(char c) {
    int index = char2index(c);
    if(index >= 0) {
        ++baseFrequencies[index];
    }
}

qint64 U2AssemblyBasesFrequenciesInfo::getCharFrequency(char c) {
    int index = char2index(c);
    return index >= 0 ? baseFrequencies[index] : 0;
}

char U2AssemblyBasesFrequenciesInfo::getMostFrequentLetter() {
    int mostFrequentIndex = 0;
    for(int i = 1; i < LETTERS_COUNT; ++i) {
        if(baseFrequencies[i] > baseFrequencies[mostFrequentIndex]) {
            mostFrequentIndex = i;
        }
    }
    return baseFrequencies[mostFrequentIndex] > 0 ? index2char(mostFrequentIndex) : AssemblyConsensusAlgorithm::EMPTY_CHAR;
}

QByteArray AssemblyBasesFrequenciesStat::getConsensusFragment() {
    int size = frequencyInfos.size();
    QByteArray res(size, AssemblyConsensusAlgorithm::EMPTY_CHAR);
    for (int i = 0; i < size; ++i) {
        res[i] = frequencyInfos[i].getMostFrequentLetter();
    }
    return res;
}

} //namespace
