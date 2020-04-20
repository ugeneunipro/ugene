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

#ifndef PAIRALIGNMENTSTRINGS_H
#define PAIRALIGNMENTSTRINGS_H

#include <U2Core/U2Region.h>

namespace U2 {

class PairAlignSequences {
public:
    PairAlignSequences();
    void setValues(int _score, const U2Region & _refSubseqInterval, const U2Region & _ptrnSubseqInterval, QByteArray _pairAlignment);

    U2Region refSubseqInterval;
    U2Region ptrnSubseqInterval;
    int score;
    QByteArray pairAlignment;

    bool isAminoTranslated;
    bool isDNAComplemented;

    static const char UP = 'u';
    static const char LEFT = 'l';
    static const char DIAG = 'd';
};

}//namespace

#endif
