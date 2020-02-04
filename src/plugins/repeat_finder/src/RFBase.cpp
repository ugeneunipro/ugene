/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include "RFBase.h"
#include "RFConstants.h"
#include "RFSArray.h"
#include "RFSArrayWK.h"
#include "RFDiagonal.h"

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

//factory method
RFAlgorithmBase* RFAlgorithmBase::createTask(RFResultsListener* l, const char *seqX, int sizeX,
                                              const char *seqY, int sizeY,
                                              const DNAAlphabet *al, int w, int mismatches,
                                              RFAlgorithm alg, int nThreads)
{
    assert(l!=NULL);
    assert(mismatches < w);
    algoLog.trace(QString("Repeat finder: sizex=%1, sizey=%2, alphabet=%3, w=%4, mismatches=%5, threads=%6")
        .arg(sizeX).arg(sizeY).arg(al->getType()).arg(w).arg(mismatches).arg(nThreads));

    RFAlgorithmBase* res = NULL;
    if (alg == RFAlgorithm_Auto) {
        //alg = RFAlgorithm_Diagonal; //the slowest but tested better
        alg = RFAlgorithm_Suffix;
    }
    if (mismatches == 0) {
        if (alg == RFAlgorithm_Diagonal) {
            res = new RFDiagonalAlgorithmWK(l, seqX, sizeX, seqY, sizeY, al->getType(), w, w);
        } else {
            res = new RFSArrayWAlgorithm(l, seqX, sizeX, seqY, sizeY, al, w);
        }
    } else {
        int matches = w - mismatches;
        bool suffix = false;
        if (alg!=RFAlgorithm_Diagonal) {
            int q = w / (mismatches +1);
            if (q >= 4 || (q == 3 && (al->getType() ==DNAAlphabet_AMINO || al->getType() ==DNAAlphabet_RAW))) {
                suffix = true;
            }
        }
        algoLog.trace(QString("using %1 algorithm").arg(suffix ? "suffix" : "diagonal"));
        if (suffix) {
            res = new RFSArrayWKAlgorithm(l, seqX, sizeX, seqY, sizeY, al->getType(), w, matches);
        } else {
            res = new RFDiagonalAlgorithmWK(l, seqX, sizeX, seqY, sizeY, al->getType(), w, matches);
        }
    }
    res->setMaxParallelSubtasks(nThreads);
    return res;
}

//////////////////////////////////////////////////////////////////////////
// Task

char RFAlgorithmBase::getUnknownChar(const DNAAlphabetType &type) {
    return type == DNAAlphabet_AMINO ? 'X' : type==DNAAlphabet_NUCL ? 'N' : '\0';
}

RFAlgorithmBase::RFAlgorithmBase(RFResultsListener* l, const char* seqx, int sizex, const char* seqy, int sizey,
                                 DNAAlphabetType seqType, int w, int k, TaskFlags flags)
: Task(tr("Find Repeats"), flags),
seqX(seqx),  seqY(seqy), SIZE_X(sizex), SIZE_Y(sizey),
SEQ_TYPE(seqType), WINDOW_SIZE(w), K(k), C(w-k),
resultsListener(l), reportReflected(true)
{
    reflective = seqX == seqY && SIZE_X==SIZE_Y;
    unknownChar = getUnknownChar(seqType);
}

void RFAlgorithmBase::setRFResultsListener(RFResultsListener* newListener) {

    resultsListener = newListener;
}

// adds single result to global results
void RFAlgorithmBase::addToResults(const RFResult& r){
#ifdef _DEBUG
    checkResult(r);
#endif
    CHECK_EXT(NULL != resultsListener, cancel(), );
    resultsListener->onResult(r);
    if (reflective && reportReflected) {
        assert(r.x!=r.y);
        CHECK_EXT(NULL != resultsListener, cancel(), );
        resultsListener->onResult(RFResult(r.y, r.x, r.l, r.c));
    }
}

// adds single result to global results
void RFAlgorithmBase::addToResults(const QVector<RFResult>& results) {
#ifdef _DEBUG
    checkResults(results);
#endif
    CHECK_EXT(NULL != resultsListener, cancel(), );
    resultsListener->onResults(results);
    if (reflective && reportReflected) {
        QVector<RFResult> complResults;
        try {
            complResults.reserve(results.size());
        } catch(...) {
            setError("Not enough memory");
            return;
        }
        foreach(const RFResult& r, results) {
            if (r.x == r.y) {
                assert(r.l == qMin(SIZE_X, SIZE_Y));
                continue;
            }
            complResults.append(RFResult(r.y, r.x, r.l, r.c));
        }
        CHECK_EXT(NULL != resultsListener, cancel(), );
        resultsListener->onResults(complResults);
    }
}

void RFAlgorithmBase::prepare() {
    if (WINDOW_SIZE > qMin(SIZE_X, SIZE_Y)) {
        stateInfo.setError(tr("Repeat length is too large: %1, sequence size: %2").arg(WINDOW_SIZE).arg(qMin(SIZE_X, SIZE_Y)));
        return;
    }
    if (reflective && reportReflected) {
        assert(SIZE_X == SIZE_Y);
        CHECK_EXT(NULL != resultsListener, cancel(), );
        resultsListener->onResult(RFResult(0, 0, SIZE_X));
    }
}

bool RFAlgorithmBase::checkResults(const QVector<RFResult>& v) {
    //debug mode self-check routine
    foreach(const RFResult& r, v) {
        checkResult(r);
    }
    return true;
}

bool RFAlgorithmBase::checkResult(const RFResult& r) {
    assert(r.x >= 0 && r.y >=0 && r.x + r.l <= SIZE_X  && r.y + r.l <= SIZE_Y);

    //check that there is mismatch before and after the result
    if (r.x > 0 && r.y > 0) {
        char cx = seqX[r.x - 1]; Q_UNUSED(cx);
        char cy = seqY[r.y - 1]; Q_UNUSED(cy);
        assert(!CHAR_MATCHES(cx, cy));
    }
    if (r.x + r.l < int(SIZE_X) && r.y + r.l < int(SIZE_Y)) {
        char cx = seqX[r.x + r.l]; Q_UNUSED(cx);
        char cy = seqY[r.y + r.l]; Q_UNUSED(cy);
        assert(!CHAR_MATCHES(cx, cy));
    }

    //check that result starts and ends with match if len > W
    if (r.l > int(WINDOW_SIZE)) {
        char cx = seqX[r.x];
        char cy = seqY[r.y];
        assert(CHAR_MATCHES(cx, cy));Q_UNUSED(cx);Q_UNUSED(cy);

        cx = seqX[r.x + r.l - 1];
        cy = seqY[r.y + r.l - 1];
        assert(CHAR_MATCHES(cx, cy));
    }

    //check that for every window W inside of the result the match rate is valid
    int c = 0;
    int allMatches = 0;
    for (int i=0; i < r.l; i++) {
        char cx = seqX[r.x + i];
        char cy = seqY[r.y + i];
        c+=CHAR_MATCHES(cx, cy) ? 0 : 1;
        allMatches+=CHAR_MATCHES(cx, cy) ? 1 : 0;

        if (i >= int(WINDOW_SIZE)) {
            char cxp = seqX[r.x + i - WINDOW_SIZE];
            char cyp = seqY[r.y + i - WINDOW_SIZE];
            c-=CHAR_MATCHES(cxp, cyp) ? 0 : 1;
        }
        assert(c <= C);
    }
    assert(allMatches == r.c);
    return true;
}

bool Tandem::extend (const Tandem& t){
    qint64 newEnd = qMax(offset+size, t.offset+t.size);
    offset = qMin(offset, t.offset);
    qint64 oldSize = size;
    size = newEnd - offset;
    return size > oldSize;
}

bool Tandem::operator < (const Tandem& t) const{
    return repeatLen<t.repeatLen || (repeatLen==t.repeatLen && rightSide<t.offset);
}

} //namespace
