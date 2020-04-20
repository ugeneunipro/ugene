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

#include "ColumnCharsCounter.h"
#include "MsaColorSchemePercentageIdententityColored.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

/******************************/
/*Nucleotide*/
/******************************/

Nucleotide::Nucleotide(const char c) : character(c),
                                       frequency(1) {}

bool Nucleotide::operator<(const Nucleotide& other) const {
    SAFE_POINT(MsaColorSchemePercentageIdententityColored::NUCLEOTIDE_LIST.contains(this->character)
            && MsaColorSchemePercentageIdententityColored::NUCLEOTIDE_LIST.contains(other.character), "Unexpected nucleotide", false);

    bool result = false;
    if (this->frequency > other.frequency) {
        result = true;
    } else if (this->frequency == other.frequency) {
        result = MsaColorSchemePercentageIdententityColored::NUCLEOTIDE_LIST.indexOf(this->character) < MsaColorSchemePercentageIdententityColored::NUCLEOTIDE_LIST.indexOf(other.character);
    } else {
        result = false;
    }

    return result;
}

bool Nucleotide::operator==(const Nucleotide& other) const {
    return this->character == other.character && this->frequency == other.frequency;
}

/******************************/
/*ColumnCharsCounter*/
/******************************/

ColumnCharsCounter::ColumnCharsCounter() : gapsNumber(0), nonAlphabetCharsNumber(0) {}

void ColumnCharsCounter::addNucleotide(const char nucleotide) {
    if (isNucleotideAlreadyInList(nucleotide)) {
        increaseNucleotideCounter(nucleotide);
    } else {
        Nucleotide n(nucleotide);
        nucleotideList.append(n);
    }
}

void ColumnCharsCounter::addGap() {
    gapsNumber++;
}

void ColumnCharsCounter::addNonAlphabetCharacter() {
    nonAlphabetCharsNumber++;
}

QList<Nucleotide> ColumnCharsCounter::getNucleotideList() const {
    return nucleotideList;
}

//const int ColumnCharsCounter::getIndexOfNucleotideWithCharacter(const char c) const {
//    int result = -1;
//    foreach(const Nucleotide& nucl, nucleotideList) {
//        CHECK_CONTINUE(nucl.character == c);
//
//        result = nucleotideList.indexOf(nucl);
//        break;
//    }
//
//    return result;
//}

bool ColumnCharsCounter::hasGaps() const {
    return gapsNumber != 0;
}

bool ColumnCharsCounter::hasNonAlphabetCharsNumber() const {
    return nonAlphabetCharsNumber != 0;
}

bool ColumnCharsCounter::hasPercentageMoreThen(const double& threshold) const {
    return getTopCharacterPercentage() >= threshold;
}

void ColumnCharsCounter::sortNucleotideList() {
    std::sort(nucleotideList.begin(), nucleotideList.end());
}

//bool ColumnCharsCounter::hasEqualPercentage() const {
//    CHECK(!nucleotideList.isEmpty(), false);
//
//    bool result = true;
//    double firstPercentage = nucleotideList.first().percentage;
//    foreach(const Nucleotide& n, nucleotideList) {
//        CHECK_CONTINUE(firstPercentage != n.percentage);
//
//        result = false;
//        break;
//    }
//
//    return result;
//}

bool ColumnCharsCounter::isNucleotideAlreadyInList(const char character) const {
    bool result = false;
    foreach(const Nucleotide& n, nucleotideList) {
        CHECK_CONTINUE(n.character == character);

        result = true;
        break;
    }

    return result;
}

void ColumnCharsCounter::increaseNucleotideCounter(const char character) {
    for (auto& n : nucleotideList) {
        CHECK_CONTINUE(n.character == character);

        n.frequency++;
        break;
    }
}

double ColumnCharsCounter::getTopCharacterPercentage() const {
    int charsNumber = gapsNumber + nonAlphabetCharsNumber;
    foreach(const Nucleotide & nucl, nucleotideList) {
        charsNumber += nucl.frequency;
    }
    SAFE_POINT(!nucleotideList.isEmpty(), "Nucleotide List is unexpected empty", 0.0);

    const Nucleotide n = nucleotideList.first();
    double result = (double(n.frequency) / charsNumber) * 100;

    return result;
}

}