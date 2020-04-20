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

#ifndef _U2_RF_DIAGONAL_ALG_H_
#define _U2_RF_DIAGONAL_ALG_H_

#include "RFBase.h"

#include <QMutex>

namespace U2 {

class RFDiagonalWKSubtask;

class RFDiagonalAlgorithmWK : public RFAlgorithmBase {
    Q_OBJECT
    friend class RFDiagonalWKSubtask;
public:
    RFDiagonalAlgorithmWK(RFResultsListener* rl, const char* seqX, int sizeX, const char* seqY, int sizeY,
                            DNAAlphabetType seqType,  int w, int k);

    void prepare();

protected:
    int                 START_DIAG;
    int                 END_DIAG;
};

class RFDiagonalWKSubtask : public Task {
    Q_OBJECT
    friend class RFDiagonalAlgorithmWK;
public:
    RFDiagonalWKSubtask(RFDiagonalAlgorithmWK* owner, int threadNum, int totalThreads);

    void run();

private:
    void processDiagonal(int x, int y);
    int getDiagLen(int d) const;
    int processMatch(const char* x, const char* y, const char* xEnd, const char* yEnd, int c);

    RFDiagonalAlgorithmWK*  owner;

    int                     threadNum;
    int                     nThreads;
    const char*             dataX;
    const char*             dataY;

    QVector<RFResult>       diagResults;

    quint64                 areaS;
    quint64                 currentS;
    quint64                 nextPercentS;
};

} //namespace


#endif
