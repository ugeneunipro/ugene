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

#ifndef _U2_RF_BASE_ALG_H_
#define _U2_RF_BASE_ALG_H_

#include <U2Core/DNAAlphabet.h>
#include <U2Core/Task.h>
#include <U2Algorithm/RepeatFinderSettings.h>

#include <QVector>

namespace U2 {

#define PCHAR_MATCHES(x, y) (*(x) == *(y) && *(x) != unknownChar)
#define CHAR_MATCHES(x, y)  ((x) == (y) && (x) != unknownChar)


class Tandem{
public:
    Tandem(quint32 _offset, quint32 _repeatLen, quint32 _size):
        offset(_offset),repeatLen(_repeatLen),size(_size),rightSide(_offset+_size-_repeatLen)
    {
    }
    Tandem(const Tandem& t):
        offset(t.offset),repeatLen(t.repeatLen),size(t.size),rightSide(t.rightSide)
    {
    }
    Tandem& operator =(const Tandem& t){
        offset=t.offset;
        repeatLen=t.repeatLen;
        size=t.size;
        rightSide=t.rightSide;
        return *this;
    }
    bool operator < (const Tandem& t) const;
    bool extend (const Tandem& t);

    // offset of tandem from beginning of the sequence
    quint64 offset;
    // repeat len
    int repeatLen;
    //size of tandem in nucleotides
    qint64 size;
    quint64 rightSide;
};

class RFAlgorithmBase : public Task {
    Q_OBJECT
public:
    RFAlgorithmBase(RFResultsListener* l,
                    const char* seqX, int sizeX,
                    const char* seqY, int sizeY, DNAAlphabetType seqType,
                    int w, int k, TaskFlags flags = TaskFlags_NR_FOSCOE);

    void setReportReflected(bool v) {reportReflected = v;}

    void setRFResultsListener(RFResultsListener*);

    void prepare();

    static RFAlgorithmBase* createTask(RFResultsListener* l,
                                        const char *seqx, int sizeX,
                                        const char *seqY, int sizeY,
                                        const DNAAlphabet *al, int w,
                                        int mismatches = 0, RFAlgorithm alg = RFAlgorithm_Auto,
                                        int nThreads = MAX_PARALLEL_SUBTASKS_AUTO);
    static char getUnknownChar(const DNAAlphabetType &type);

protected:
    // adds single result to global results
    void addToResults(const RFResult& r);

    // adds multiple results to global results
    void addToResults(const QVector<RFResult>& newResults);

    //always return true. bool -> to use in assertions
    bool checkResults(const QVector<RFResult>& v);
    bool checkResult(const RFResult& v);

    const char*             seqX;
    const char*             seqY;
    const int               SIZE_X;
    const int               SIZE_Y;
    const DNAAlphabetType   SEQ_TYPE;
    const int               WINDOW_SIZE;
    const int               K;
    const int               C;

    bool                    reflective;
    char                    unknownChar;

    RFResultsListener*      resultsListener;
    bool                    reportReflected;
};

} //namespace

#endif
