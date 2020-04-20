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

#ifndef _U2_RF_SARRAYWK_ALG_H_
#define _U2_RF_SARRAYWK_ALG_H_

#include <U2Algorithm/SArrayIndex.h>
#include <U2Algorithm/SArrayBasedFindTask.h>
#include "RFBase.h"

#include <QVector>

namespace U2 {

class SArrayIndex;
class RFSArrayWKSubtask;
class RFRollingArray;

class RFSArrayWKAlgorithm : public RFAlgorithmBase  {
    Q_OBJECT
    friend class RFSArrayWKSubtask;
public:
    RFSArrayWKAlgorithm(RFResultsListener* rl, const char* seqX, int sizeX, const char* seqY, int sizeY,
                        DNAAlphabetType seqType, int w, int k);

    ~RFSArrayWKAlgorithm() {cleanup();}

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    void cleanup();

private:
    void addResult(int a, int s, int l, int c);
    void calculate(RFSArrayWKSubtask* t);

public:
    QVector<int>    diagOffsets; //holds start position for the last checked window
    int             ARRAY_SIZE;
    int             SEARCH_SIZE;

    const char      *arraySeq;
    const char      *searchSeq;
    bool            arrayIsX;

    quint32         q;

    CreateSArrayIndexTask*  indexTask;
    int                     nThreads;

};

class RFSArrayWKSubtask : public Task, public SArrayIndex::SAISearchContext {
    Q_OBJECT
    friend class RFSArrayWKAlgorithm;
public:
    RFSArrayWKSubtask(RFSArrayWKAlgorithm* owner, int _tid);
    virtual ~RFSArrayWKSubtask(){}
    void run();

    RFSArrayWKAlgorithm*    owner;

    const int   tid;
};

} //namespace

#endif
