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

#ifndef _U2_COLUMN_CHARS_COUNTER_H_
#define _U2_COLUMN_CHARS_COUNTER_H_

#include <QList>

namespace U2 {

struct Nucleotide {
    Nucleotide(const char c);
    bool operator<(const Nucleotide& other) const;
    bool operator==(const Nucleotide& other) const;

    char character;
    int frequency;
};

class ColumnCharsCounter {
public:
    ColumnCharsCounter();

    void addNucleotide(const char nucleotide);
    void addGap();
    void addNonAlphabetCharacter();
    QList<Nucleotide> getNucleotideList() const;
    //const int getIndexOfNucleotideWithCharacter(const char c) const;
    bool hasGaps() const;
    bool hasNonAlphabetCharsNumber() const;
    bool hasPercentageMoreThen(const double& threshold) const;
    void sortNucleotideList();
    //bool hasEqualPercentage() const;

private:
    bool isNucleotideAlreadyInList(const char character) const;
    void increaseNucleotideCounter(const char character);
    double getTopCharacterPercentage() const;

    QList<Nucleotide> nucleotideList;
    int gapsNumber;
    int nonAlphabetCharsNumber;
};


}

#endif // _U2_COLUMN_CHARS_COUNTER_H_