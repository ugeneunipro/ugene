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

#include <U2Core/U2SafePoints.h>

#include "U2AssemblyReadIterator.h"


namespace U2 {

U2AssemblyReadIterator::U2AssemblyReadIterator(const QByteArray & read_, QList<U2CigarToken> cigar_, int startPos /* = 0*/) :
offsetInRead(0), read(read_), offsetInToken(0), offsetInCigar(0), cigar(cigar_)
{
    for(int i = 0; i < startPos && hasNext();) {
        skip();
        U2CigarToken t = cigar.at(offsetInCigar);
        if(i + t.count <= startPos) { //we are going to skip the current token
            if(isMatch()) {
                offsetInRead += t.count;
            }
            i += t.count;
            offsetInToken += t.count;
            if(!hasNext()) break;
            advanceToNextToken();
        } else {
            //landing in the current token
            offsetInToken = startPos - i;
            if(isMatch()) {
                offsetInRead += offsetInToken;
            }
            break;
        }
    }
}

bool U2AssemblyReadIterator::hasNext() const {
    assert(offsetInCigar <= cigar.size());
    if(offsetInCigar == cigar.size()) {
        return false;
    }
    //check if staying on the last pos of current token
    if(offsetInToken == cigar.at(offsetInCigar).count) {

        //if staying on the last pos of current token of the last token -> hasn't next
        if(offsetInCigar == cigar.size()-1) {
            return false;
        }
        //check if everything after current pos is insertion/padding/hard clip
        int i = offsetInCigar+1;
        for(; i < cigar.size(); ++i) {
            U2CigarOp op = cigar.at(i).op;
            if(U2CigarOp_I != op && U2CigarOp_S != op && U2CigarOp_P != op && U2CigarOp_H != op) {
                break;
            }
        }

        if(i == cigar.size()) {
            return false; //no matches/deletions found
        }
    }
    return true;
}

char U2AssemblyReadIterator::nextLetter() {
    assert(hasNext());
    skip();
    SAFE_POINT(offsetInCigar < cigar.size(), "CIGAR out of range", 0);
    if(offsetInToken != cigar.at(offsetInCigar).count) { //staying in the current token
        offsetInToken++;
    } else { //current token is finished
        advanceToNextToken();
        offsetInToken = 1;
    }
    bool del = isDeletion();
    char c = del ? '-' : read.at(offsetInRead); //TODO: hardcoded '-'
    offsetInRead += !del; //adjust offsetInRead only when going through match token
    return c;
}

void U2AssemblyReadIterator::advanceToNextToken() {
    offsetInToken = 0;
    offsetInCigar++;
    skip();
}

bool U2AssemblyReadIterator::isMatch() const {
    U2CigarOp op = cigar.at(offsetInCigar).op;
    return U2CigarOp_M == op || U2CigarOp_X == op || U2CigarOp_EQ == op;
}

bool U2AssemblyReadIterator::isInsertion() const {
    U2CigarOp op = cigar.at(offsetInCigar).op;
    return U2CigarOp_I == op || U2CigarOp_S == op;
}

bool U2AssemblyReadIterator::isDeletion() const {
    U2CigarOp op = cigar.at(offsetInCigar).op;
    return U2CigarOp_D == op || U2CigarOp_N == op;
}

bool U2AssemblyReadIterator::isPaddingOrHardClip() const {
    U2CigarOp op = cigar.at(offsetInCigar).op;
    return U2CigarOp_P == op || U2CigarOp_H == op;
}

void U2AssemblyReadIterator::skip() {
    while(hasNext() && !isMatch() && !isDeletion()) {
        skipInsertion();
        skipPaddingAndHardClip();
    }
}

//skip tokens and corresponding letters
void U2AssemblyReadIterator::skipInsertion() {
    while(hasNext() && isInsertion()) {
        offsetInRead += cigar.at(offsetInCigar).count;
        offsetInCigar++;
    }
}

//silently skip this tokens.
void U2AssemblyReadIterator::skipPaddingAndHardClip() {
    while(hasNext() && isPaddingOrHardClip()) {
        offsetInCigar++;
    }
}

namespace {
void check(const QByteArray & expectedSeq, U2AssemblyReadIterator & it) {
    int i = 0;
    while(it.hasNext()) {
        assert(i < expectedSeq.size());
        char expected = expectedSeq[i++];
        char actual = it.nextLetter();
        Q_UNUSED(actual);Q_UNUSED(expected);
        assert(expected == actual);
    }
    assert(i == expectedSeq.size());
}

void t01() {
    QByteArray read("SIIISSAIIIAAASSIS");
    QList<U2CigarToken> cigar;
    //1H 1S 2I 1I 2S 1M 3I 1D 1M 2D 100H 200P 2M 2S 1I 1S 5H
    cigar
        << U2CigarToken(U2CigarOp_H, 1)
        << U2CigarToken(U2CigarOp_S, 1)
        << U2CigarToken(U2CigarOp_I, 2)
        << U2CigarToken(U2CigarOp_I, 1)
        << U2CigarToken(U2CigarOp_S, 2)
        << U2CigarToken(U2CigarOp_M, 1)
        << U2CigarToken(U2CigarOp_I, 3)
        << U2CigarToken(U2CigarOp_D, 1)
        << U2CigarToken(U2CigarOp_M, 1)
        << U2CigarToken(U2CigarOp_D, 2)
        << U2CigarToken(U2CigarOp_H, 100)
        << U2CigarToken(U2CigarOp_P, 200)
        << U2CigarToken(U2CigarOp_M, 2)
        << U2CigarToken(U2CigarOp_S, 2)
        << U2CigarToken(U2CigarOp_I, 1)
        << U2CigarToken(U2CigarOp_S, 1)
        << U2CigarToken(U2CigarOp_H, 5);

    QByteArray expectedRead("A-A--AA");
    U2AssemblyReadIterator it(read, cigar);
    check(expectedRead, it);
}

void t02() {
    QByteArray read("SIIIA");
    QList<U2CigarToken> cigar;
    //1S 3I 1M
    cigar
        << U2CigarToken(U2CigarOp_S, 1)
        << U2CigarToken(U2CigarOp_I, 3)
        << U2CigarToken(U2CigarOp_EQ, 1);
    QByteArray expectedRead("A");
    U2AssemblyReadIterator it(read, cigar);
    check(expectedRead, it);
}

void t03() {
    QByteArray read("ACTS");
    QList<U2CigarToken> cigar;
    //2H 1P 1M 1= 1X 1S 5H
    cigar
        << U2CigarToken(U2CigarOp_H, 2)
        << U2CigarToken(U2CigarOp_P, 1)
        << U2CigarToken(U2CigarOp_M, 1)
        << U2CigarToken(U2CigarOp_EQ, 1)
        << U2CigarToken(U2CigarOp_X, 1)
        << U2CigarToken(U2CigarOp_S, 1)
        << U2CigarToken(U2CigarOp_H, 5);

    QByteArray expectedRead("ACT");
    U2AssemblyReadIterator it(read, cigar);
    check(expectedRead, it);
}

} //ns

void shortReadIteratorSmokeTest() {
    t01();
    t02();
    t03();
}

} //ns
