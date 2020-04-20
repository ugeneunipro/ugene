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

#include "DNAAlphabet.h"

namespace U2 {

const QString BaseDNAAlphabetIds::RAW() {
    return "RAW_ALPHABET";
}

const QString BaseDNAAlphabetIds::NUCL_DNA_DEFAULT() {
    return "NUCL_DNA_DEFAULT_ALPHABET";
}

const QString BaseDNAAlphabetIds::NUCL_RNA_DEFAULT() {
    return "NUCL_RNA_DEFAULT_ALPHABET";
}

const QString BaseDNAAlphabetIds::NUCL_DNA_EXTENDED() {
    return "NUCL_DNA_EXTENDED_ALPHABET";
}

const QString BaseDNAAlphabetIds::NUCL_RNA_EXTENDED (){
    return "NUCL_RNA_EXTENDED_ALPHABET";
}

const QString BaseDNAAlphabetIds::AMINO_DEFAULT() {
    return "AMINO_DEFAULT_ALPHABET";
}

const QString BaseDNAAlphabetIds::AMINO_EXTENDED() {
    return "AMINO_EXTENDED_ALPHABET";
}

DNAAlphabet::DNAAlphabet(const QString& _id, const QString& _name, DNAAlphabetType _t, const QBitArray& _map,
                         Qt::CaseSensitivity cm, char _defSym)
: id(_id), name(_name), type(_t), map(_map), caseMode(cm), defSym(_defSym)
{
    assert(map[defSym] == true);
    numChars = getAlphabetChars().count();
}


QByteArray DNAAlphabet::getAlphabetChars(bool forceBothCases) const {
    QByteArray res;
    bool bothCases = forceBothCases || isCaseSensitive();
    for (int i = 0; i < 256; i++) {
        if (map[i]) {
            bool skip = (!bothCases) && i >= 'a' && i <= 'z';
            if (!skip)  {
                res.append((char)i);
            }
        }
    }
    assert(!res.isEmpty());
    return res;
}


bool DNAAlphabet::containsAll(const char* str, int len) const {
    for (int i=0; i < len; i++) {
        char c = str[i];
        if (!contains(c)) {
            return false;
        }
    }
    return true;
}

} //namespace

